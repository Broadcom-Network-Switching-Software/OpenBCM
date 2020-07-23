/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 * 
 * 
 * The below CINT demonstrates MPLS L3VPN which show BCM886XX being PE-based L3VPN.    
 * Available for 88660_A0 and above.                                                                             
 * MPLS L3VPN model consists of three kind of devices:                                                                                                               
 * 1. Customer Edge device (CE): Resides on customer network and connected with one or more interfaces to provide edge device.                                       
 * The CE is not familiar with the existance of VPN and not with the MPLS core application.                                                                          
 * 2. Provide Edge device (PE): Resides on service provider network and connects to one or more CEs.                                                                 
 * PE is responsible on receiving and sending traffic from / to the MPLS network between the CE and P.                                                               
 * PE is in charge of all the VPN processing.                                                                                                                        
 * 3. Provider device (P): Backbone router on a service provider network. Only in charge of the MPLS network in the backbone and not familir with the VPN processing.
 * It may be that PE device will fucntional as P for other VPN processing of not directly connected CE to MPLS backbone.                                             
 *                                                                                                                                                                   
 * In the CINT below we show BCM886XX as a PE device.                                                                                                                
 * Terminology compared between BCM SDK (and Dune Arch) to L3VPN network:                                                                                            
 * 1. Site (IP address or group of IPs) in PE device usually being termed as IP route address or IP host address (in case of exact match)                            
 * 2. VPN instance - different routes may belong to VPN instances. Each VPN instance usually maintain its own routing table.                                         
 *  VPN instance is usually implemented by VRF object. VRF is a set by the RIF interface (L3 interface).                                                             
 * 3. VC label, VLAN - The identification of VPN instance is usually being handled by VLAN from the CE side or by VC label from the MPLS core side.                  
 *  VC label is usually implemented by a regular LSP label in BCM SDK. bcm_mpls_tunnel_terminator_create in charge of the termination part                           
 * and bcm_mpls_tunnel_initaitor_create in charge of the encapsulation part.                                                                                         
 * We note that there is no need to use MPLS_PORT for this functionality as we don't need to learn nor to maintain any L2VPN properties.                             
 *  VLAN usually may provide from CE side the VRF by:                                                                                                                
 *  (a) A premiliary lookup of Port x VLAN which creates AC-LIF                                                                                                      
 * (b) AC-LIF is only used for RIF mapped (and so VRF mapped).                                                                                                       
 *  In case RIF is equal to VLAN user may drop AC-LIF creation and relay on Port default AC-LIF.                                                                     
 * Port default AC-LIF is set on init and provide the capability of mapping RIF directly form VLAN.                                                                  
 *  For more information on VLAN - RIF model mappings please see cint_ip_route_explicit_rif.c                                                                        
 * 
 * The device can function also as IP-GRE core application instead of MPLS but in cint_l3vpn we focused on MPLS core
 * 
 * 
 * The below BCM SDK settings try to provide L3VPN example topology and traffic examples show UC, MC packet scenarios in this network topology:
 * BCM> cint ../sand/utility/cint_sand_utils_mpls.c
 * BCM> cint cint_qos.c 
 * BCM> cint utility/cint_utils_l3.c
 * BCM> cint cint_mpls_lsr.c 
 * BCM> cint cint_ip_route.c 
 * BCM> cint cint_l3vpn.c
 * BCM> cint
 * cint> int inP = 201;
 * cint> int outP = 202;
 * cint> l3vpn_run_defaults_with_ports(unit, inP, outP);
 * 
 *
 *
 *
 * 
 * Traffic:
 *   - UC packets:
 *     - Packet Incoming from access side to mpls network: CE1->P1
 *     - Packet Incoming from MPLS network to access side.
 *     - Packet is IP routed in access side. 
 *     - Packet is routed in MPLS network
 * 
 * 
 *   - MC packets:
 *     - Packet Incoming from MPLS network, to P2, CE1. (Bud node)
 *     - Packet Incoming from MPLS network, to PE2 (Continue)
 *     - Packet Incoming from MPLS network, to CE1, CE2 (Leaf node)
 *     - Packet Incoming from access side, to CE2, P1
 * 
 * 
 *  Traffic from RouterCE1 to RouterP
 * 
 *  Routing to overlay: host10 to host90
 *  Purpose: - check mpls & vc encapsulations, check new ethernet header
 * 
 *    Send:                                           
 *             ----------------------------------------
 *        eth: |    DA       |     SA          | VLAN |
 *             ----------------------------------------
 *             | routerPE1_mac | routerCE1_mac |  v1  |  
 *             ----------------------------------------
 *                 -------------------------- 
 *             ip: |   SIP     | DIP        | 
 *                 --------------------------
 *                 | host10_ip |  host90_ip | 
 *                 -------------------------- 
 *    Receive:
 *             -------------------------------------
 *        eth: |    DA       |     SA      | VLAN  |
 *             -------------------------------------
 *             | routerP_mac | routerPE1_mac |  v2 |  
 *             -------------------------------------  
 *        mpls:   --------------------------   
 *                |   LABEL   | LABEL      |   
 *                --------------------------             
 *                | MPLS LABEL|  VC_LABEL  |          
 *                --------------------------             
 *                          -------------------------- 
 *                      ip: |   SIP     | DIP        | 
 *                          -------------------------- 
 *                          | host10_ip |  host90_ip | 
 *                          --------------------------  
 *  
 * 
 * 
 * 
 *   Traffic from RouterP2 to RouterCE1
 *   
 *   Tunnel termination, ip routing: host91 to host 11
 *   Purpose: - check mpls and vc label termination, check new ethernet header
 * 
 *    Send:  
 *             -------------------------------------
 *        eth: |    DA         |   SA      | VLAN  |
 *             -------------------------------------
 *             | routerPE1_mac | routerP_mac |  v2 |  
 *             -------------------------------------  
 *        mpls:   --------------------------   
 *                |   LABEL   | LABEL      |   
 *                --------------------------             
 *                | MPLS LABEL|  VC_LABEL  |          
 *                --------------------------             
 *                          -------------------------- 
 *                      ip: |   SIP     | DIP        | 
 *                          -------------------------- 
 *                          | host81_ip |  host11_ip | 
 *                          --------------------------      
 *   
 *    Receive:  
 *             ----------------------------------------
 *        eth: |    DA         |   SA          | VLAN |
 *             ----------------------------------------
 *             | routerCE1_mac | routerPE1_mac |  v1  |  
 *             ----------------------------------------
 *                 -------------------------- 
 *             ip: |   SIP     | DIP        | 
 *                 --------------------------
 *                 | host81_ip |  host11_ip | 
 *                 --------------------------    
 *   
 *   
 *   
 *   Traffic from RouterCE2 to RouterCE1
 * 
 *   ip routing: host21 to host 11
 * 
 *    Send:  
 *             ----------------------------------------
 *        eth: |    DA         |   SA          | VLAN |
 *             ----------------------------------------
 *             | routerPE1_mac | routerCE2_mac |  v1  |  
 *             ----------------------------------------
 *                 -------------------------- 
 *             ip: |   SIP     | DIP        | 
 *                 --------------------------
 *                 | host21_ip |  host11_ip | 
 *                 --------------------------
 * 
 *    Receive:  
 *             ----------------------------------------
 *        eth: |    DA         |   SA          | VLAN |
 *             ----------------------------------------
 *             | routerCE1_mac | routerPE1_mac |  v1  |  
 *             ----------------------------------------
 *                 -------------------------- 
 *             ip: |   SIP     | DIP        | 
 *                 --------------------------
 *                 | host21_ip |  host11_ip | 
 *                 --------------------------
 * 
 * 
 * 
 *   Traffic from RouterP2 to RouterP3
 *   
 *   mpls Tunnel swap
 *   Purpose: - check swap mpls, check new ethernet header
 * 
 *    Send:  
 *             -------------------------------------
 *        eth: |    DA         |   SA      | VLAN  |
 *             -------------------------------------
 *             | routerPE1_mac | routerPE2_mac |  v2 |  
 *             -------------------------------------  
 *        mpls:   --------------------------   
 *                |   LABEL   | LABEL      |   
 *                --------------------------             
 *                | MPLS LABEL|  VC_LABEL  |          
 *                --------------------------             
 *                          -------------------------- 
 *                      ip: |   SIP     | DIP        | 
 *                          -------------------------- 
 *                          | host81_ip |  host11_ip | 
 *                          --------------------------      
   */


struct cint_l3vpn_info_s {
    int ce_port; /* access port */
    int p_port; /* provider port */
    uint8 my_mac[6]; /* my mac of the router */
    uint8 next_hop_to_P1_mac[6]; /* next hop to P1 for mpls tunnel */
    uint8 next_hop_to_P2_mac[6]; /* next hop to P2 for mpls tunnel */
    uint8 ce1_mac[6]; /* next hop for routing to customer edge */
    int vlan_access_CE1; /* vlan in access network. For CE1. */
    int vlan_access_CE2; /* vlan in access network. For CE2 */
    int vlan_to_vpn_P1; /*   vlan in core network. For P1 */
    int vlan_to_vpn_P2; /* vlan in core network. For P2  */
    int vlan_to_vpn_P1_hi_fec; /* vlan in core network. For P1 for hi-fec implementation */

    uint8 enable_hierarchical_fec; /* add VC label and mpls tunnels using hierarchical fec. 
                                      If disabled, only create mpls tunnels using 1 fec */

    bcm_mpls_label_t mpls_label_to_access;  /* mpls label in access direction. Label to terminate */
    bcm_mpls_label_t mpls_in_label_swap;    /* mpls label in network provider. Label to swap with mpls_eg_label_swap */
    bcm_mpls_label_t mpls_eg_label_swap;    /* mpls label in network provider. swap label mpls_in_label_swap */

};

int verbose1 = 1;
cint_l3vpn_info_s l3vpn_info;

void 
l3vpn_info_init(int unit, 
                        int ce_port,
                        int p_port,
                        uint8* my_mac,
                        uint8* next_hop_to_P1_mac,
                        uint8* next_hop_to_P2_mac,
                        uint8* ce1_mac,
                        int vlan_access_CE1,
                        int vlan_access_CE2,
                        int vlan_to_vpn_P1,
                        int vlan_to_vpn_P2, 
                        int vlan_to_vpn_P1_hi_fec,
                        uint8 enable_hierarchical_fec, 
                        bcm_mpls_label_t mpls_label_to_access, 
                        bcm_mpls_label_t mpls_in_label_swap, 
                        bcm_mpls_label_t mpls_eg_label_swap) {

    int rv; 

    l3vpn_info.ce_port = ce_port; 
    l3vpn_info.p_port = p_port; 
    sal_memcpy(l3vpn_info.my_mac, my_mac, 6);
    sal_memcpy(l3vpn_info.next_hop_to_P1_mac, next_hop_to_P1_mac, 6);
    sal_memcpy(l3vpn_info.next_hop_to_P2_mac, next_hop_to_P2_mac, 6);
    sal_memcpy(l3vpn_info.ce1_mac, ce1_mac, 6);
    l3vpn_info.vlan_access_CE1       = vlan_access_CE1; 
    l3vpn_info.vlan_access_CE2      = vlan_access_CE2; 
    l3vpn_info.vlan_to_vpn_P1          = vlan_to_vpn_P1; 
    l3vpn_info.vlan_to_vpn_P2         = vlan_to_vpn_P2; 
    l3vpn_info.vlan_to_vpn_P1_hi_fec  = vlan_to_vpn_P1_hi_fec; 
    l3vpn_info.enable_hierarchical_fec = enable_hierarchical_fec; 
    l3vpn_info.mpls_label_to_access    = mpls_label_to_access; 
    l3vpn_info.mpls_in_label_swap      = mpls_in_label_swap; 
    l3vpn_info.mpls_eg_label_swap      = mpls_eg_label_swap; 
}

int 
l3vpn_run(int unit) {
    int rv; 
    int vrf = 0;

    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    if (verbose1) {
        printf("l3vpn_run create vswitch\n");
    }

    /** Routing Interfaces **/ 
    int ing_intf;  /* interface_id for ingress router interface */
    int egr_intf;  /* interface_id for egress router interface */


    /* create ingress router instance for access packets:  
     * - create a bridge instance with vsi = vlan
     * - create flooding MC group for bridge instance
     * - add port to vlan
     * - create router instance
     * - create incoming router interface and outgoing interface */
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P1, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P1, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P1, l3vpn_info.ce_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.ce_port, l3vpn_info.vlan_to_vpn_P1);
      return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = l3vpn_info.vlan_to_vpn_P1;
    intf.my_global_mac = l3vpn_info.my_mac;
    intf.my_lsb_mac = l3vpn_info.my_mac;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /* create egress router instance for access packets */ 
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P2, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P2, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P2, l3vpn_info.p_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.p_port, l3vpn_info.vlan_to_vpn_P2);
      return rv;
    }
    
    intf.vsi = l3vpn_info.vlan_to_vpn_P2;
    
    rv = l3__intf_rif__create(unit, &intf);
    egr_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /** tunnels **/

    /* create tunnels: to build 2 mpls headers (mpls label and VC label) */
    int tunnel_id = 0;

    mpls_tunnel_properties.label_in = 200;
	mpls_tunnel_properties.label_out = 400;
	mpls_tunnel_properties.next_pointer_intf = egr_intf;
	
	printf("Trying to create tunnel initiator\n");
	rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
	if (rv != BCM_E_NONE) {
       printf("Error, in mpls__create_tunnel_initiator__set\n");
       return rv;
    }

    tunnel_id = mpls_tunnel_properties.tunnel_id;

    if (verbose1) {
        printf("Configured mpls tunnels\n");
    }

    /* create LL for tunnel. Link the tunnel with the LL,
       return fec pointer to VC label tunnel */
    int fec; 
    bcm_if_t encap_id; 
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, l3vpn_info.next_hop_to_P1_mac , 6);
    l3eg.out_tunnel_or_rif = tunnel_id;
    l3eg.out_gport = l3vpn_info.p_port;
    l3eg.vlan = l3vpn_info.vlan_to_vpn_P2;
    l3eg.fec_id = fec;
    l3eg.arp_encap_id = encap_id;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;
    if (verbose1) {
        printf("Configured LL: %d\n", encap_id);
    }

    /* tunnel termination: terminate 2 mpls headers (mpls label and VC label) */
    if(soc_property_get(unit ,"mpls_termination_label_index_enable",0)) {
        int lbl_val = l3vpn_info.mpls_label_to_access;
        BCM_MPLS_INDEXED_LABEL_SET(l3vpn_info.mpls_label_to_access, lbl_val, 1);  
       
    }
    rv = mpls_add_term_entry(unit, l3vpn_info.mpls_label_to_access, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_term_entry for label %x \n", l3vpn_info.mpls_label_to_access);
        return rv;
    }

    /* tunnel termination: terminate 2 mpls headers (mpls label and VC label) */
    /* vc label is 200 (see create_tunnel_initiator function, where the vc label is encapsulated) */
    rv = mpls_add_term_entry(unit, 0xc8, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_term_entry for label %x \n", l3vpn_info.mpls_label_to_access);
        return rv;
    }

    if (verbose1) {
        printf("Configured tunnel termination for labels %x %x \n", l3vpn_info.mpls_label_to_access, 0xc8);
    }

    /* create tunnels using hierarchical fec:
           build 2  mpls headers (mpls label and VC label) */
    int hi_fec = 0; 
    if (l3vpn_info.enable_hierarchical_fec) {
        int cascaded_fec = 0; 

        if (verbose1) {
            printf("create heriarchical fec\n");
        }

        tunnel_id = 0;

        mpls_tunnel_properties.tunnel_id = 0;
        mpls_tunnel_properties.label_in = 200;
		mpls_tunnel_properties.label_out = 0;
		mpls_tunnel_properties.next_pointer_intf = egr_intf;
	
        /* create 1 mpls tunnel: mpls label */	
		printf("Trying to create tunnel initiator\n");
		rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
		if (rv != BCM_E_NONE) {
           printf("Error, in mpls__create_tunnel_initiator__set\n");
           return rv;
        }

        tunnel_id = mpls_tunnel_properties.tunnel_id;

        if (verbose1) {
            printf("hierarchical fec: create mpls tunnel: mpls label \n");
        }

        /* create cascaded fec for mpls */
        encap_id = 0;
        create_l3_egress_s l3eg1;
        l3eg1.l3_flags = BCM_L3_CASCADED;
        l3eg1.allocation_flags = BCM_L3_CASCADED;
        sal_memcpy(l3eg1.next_hop_mac_addr, l3vpn_info.next_hop_to_P1_mac , 6);
        l3eg1.out_tunnel_or_rif = tunnel_id;
        l3eg1.out_gport = l3vpn_info.p_port;
        l3eg1.vlan = l3vpn_info.vlan_to_vpn_P1_hi_fec;
        l3eg1.fec_id = cascaded_fec;
        l3eg1.arp_encap_id = encap_id;

        rv = l3__egress__create(unit,&l3eg1);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        cascaded_fec = l3eg1.fec_id;
        encap_id = l3eg1.arp_encap_id;
        if (verbose1) {
            printf("hierarchical fec: Configured LL: %d\n", encap_id);
        }

        /* create 1 mpls tunnel: VC label */
        tunnel_id = 0;

        mpls_tunnel_properties.tunnel_id = 0;
        mpls_tunnel_properties.label_in = 200;
        mpls_tunnel_properties.label_out = 0;
        BCM_L3_ITF_SET(mpls_tunnel_properties.next_pointer_intf, BCM_L3_ITF_TYPE_LIF, 0);

        printf("Trying to create tunnel initiator\n");
        rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
        if (rv != BCM_E_NONE) {
           printf("Error, in mpls__create_tunnel_initiator__set\n");
           return rv;
        }

		tunnel_id = mpls_tunnel_properties.tunnel_id;

        if (verbose1) {
            printf("hierarchical fec: create mpls tunnel: vc label \n");
        }


        /* 
         * create additional fec for vc label:
         * which point to cascaded fec:
         * - FEC contains another fec: to build the mpls tunnel
         *                outlif: build the vc tunnel
         * Note: No need to build LL encapsulation here, since LL is already built at cascaded fec. 
         */

        /* get fwd group from cascaded fec */
        int fwd_group_cascaded_fec; 
        BCM_GPORT_FORWARD_PORT_SET(fwd_group_cascaded_fec, cascaded_fec); 
        if (verbose1) {
            printf("hierarchical fec: BCM_GPORT_FORWARD_PORT_SET: %x\n", fwd_group_cascaded_fec);
        }

        bcm_failover_t failover_id_fec = 0;
        if (!is_device_or_above(unit,JERICHO_B0)) {
            rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_failover_create\n");
            return rv;
            }
        }

        encap_id = 0; 
        create_l3_egress_s l3eg2;
        l3eg2.allocation_flags = BCM_L3_INGRESS_ONLY;
        l3eg2.l3_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
        sal_memcpy(l3eg2.next_hop_mac_addr, l3vpn_info.next_hop_to_P1_mac , 6);
        l3eg2.out_tunnel_or_rif = tunnel_id;
        l3eg2.out_gport = fwd_group_cascaded_fec;
        l3eg2.vlan = l3vpn_info.vlan_to_vpn_P1_hi_fec;
        l3eg2.fec_id = hi_fec;
        l3eg2.arp_encap_id = encap_id;
        l3eg2.failover_id = failover_id_fec;
        l3eg2.failover_if_id = 0;

        if (!is_device_or_above(unit,JERICHO_B0)) {
            rv = l3__egress__create(unit,&l3eg2);
            if (rv != BCM_E_NONE) {
                printf("Error, l3__egress__create\n");
                return rv;
            }
            l3eg2.failover_if_id = l3eg2.fec_id;
            l3eg2.arp_encap_id = 0;
        }

        rv = l3__egress__create(unit,&l3eg2);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        hi_fec = l3eg2.fec_id;
        encap_id = l3eg2.arp_encap_id;
    }

    /** ip routing at access network **/

    /* create LL for access routing. after tunnel termination (for P to CE)
                                    or for simple routing from CE1 to CE2*/

    int fec_access_routing; 
    encap_id = 0; 
    l3eg.allocation_flags = 0;
    sal_memcpy(l3eg.next_hop_mac_addr, l3vpn_info.ce1_mac , 6);
    l3eg.out_tunnel_or_rif = 0;
    l3eg.out_gport = l3vpn_info.ce_port;
    l3eg.vlan = l3vpn_info.vlan_to_vpn_P1;
    l3eg.fec_id = fec_access_routing;
    l3eg.arp_encap_id = encap_id;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec_access_routing = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;

    /** mpls routing at provider network **/

    /* create LL for P2 */
    int fec_to_p2; 
    encap_id = 0; 
    sal_memcpy(l3eg.next_hop_mac_addr, l3vpn_info.next_hop_to_P2_mac , 6);
    l3eg.out_tunnel_or_rif = 0;
    l3eg.out_gport = l3vpn_info.p_port;
    l3eg.vlan = l3vpn_info.vlan_to_vpn_P2;
    l3eg.fec_id = fec_to_p2;
    l3eg.arp_encap_id = encap_id;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec_to_p2 = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;

    /* perform mpls swap */
    rv = mpls_add_switch_entry(unit, 
                          l3vpn_info.mpls_in_label_swap , /* incoming label to swap */
                          l3vpn_info.mpls_eg_label_swap,  /* new label */
                          fec_to_p2); /* new LL */
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_switch_entry \n");
        return rv;
    }


    /** add routing entry to provider network and to customer network **/

    int route = 0x5A5A5A5A; /* 90.90.90.90 */
    int mask  = 0xfffffff0; 

    /* to provider network. */

    /* Add route to host90 */
    rv = add_route(unit, route, mask , vrf, fec); 

    /* for hierarchical fec. Add route to host99  */
    if (l3vpn_info.enable_hierarchical_fec) {
        route = 0x63636363; 
        rv = add_route(unit, route, mask , vrf, hi_fec); 
    }

    /* to customer network */

    /* add route to host11 */
    route = 0x0B0B0B0B ; /* 11.11.11.11 */ 
    mask  = 0xfffffff0; 
    rv = add_route(unit, route, mask, vrf, fec_access_routing); 


    return rv; 
}





int 
l3vpn_run_defaults_with_ports(int unit, int accessPort, int networkPort) {

    uint8 my_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P2_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
    uint8 ce1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    my_mac[5] = 0x55; 
    next_hop_to_P1_mac[5] = 0x12; 
    next_hop_to_P2_mac[5] = 0x34; 
    ce1_mac[4] = 0xce; 
    ce1_mac[5] = 0x1; 

    l3vpn_info_init(unit,
                            accessPort, /* access port  */
                            networkPort, /* provider port */
                            my_mac, /* my mac of the router */
                            next_hop_to_P1_mac, /* next hop to P1 for mpls tunnel */
                            next_hop_to_P2_mac, /* next hop to P2 for mpls tunnel */
                            ce1_mac, /* next hop for routing to customer edge CE1 */
                            10, /* vlan in access network. For CE1. */
                            20, /* vlan in access network. For CE2 */
                            30, /* vlan in core network. For P1 */
                            40, /* vlan in core network. For P2 */
                            50, /* vlan in core network. For P1 for hi-fec */   
                            0,  /* also create tunnels using hi-fec */
                            1000, /* mpls label to terminate */
                            100, /* mpls label in network provider. Label to swap with mpls_eg_label_swap */
                            101  /* mpls label in network provider. swap label mpls_in_label_swap */        
                            ); 

    return l3vpn_run(unit);
}




int 
l3vpn_run_with_defaults(int unit) {

    return l3vpn_run_defaults_with_ports(unit, 
                                                 201, /* access port  */
                                                 202 /* provider port */); 
}

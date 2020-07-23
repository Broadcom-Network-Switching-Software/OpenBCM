/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *  
 * Cint desmonstare mirroring according to <port,vlan>, 
 *      - mirror_with_span_example: for SPAN example simple mirror without any editing on packet.
 *      - mirror_with_rspan_example: for RSPAN example "tunnel" into Vlan-tag
 *      - mirror_with_erspan_example: for ERSPAN example tunnel into SPANoGREoIPoE.
 *  
 *  G_mirror_type: is a global paramet, that determine if to do inbound or out-bount mirroring.
 *  
 * packets to send:
 *   - send from/to in/out-port with vlan 2 --> ERSPAN tunnel
 *   - send from/to in/out-port with vlan 3 --> RSPAN tunnel
 *   - send from/to in/out-port with vlan 4 --> SPAN tunnel
 *  
 * Note: 
 *   ARAD (A,B) support UC ERSPAN mirroring.
 *   To support multicast ERSPAN outbound mirroring a new soc property introduce "custom_feature_erspan_mc_support=1"
 *   In case it is set , first 16 entries in ISID-table are used for ERSPAN feature.
 *   User can allocate for MIM, VXLAN, L2GRE only VPNs that pass the constraint (vsi & 0xFFF) > 16.
 *   Also, VPN must be allocated for those application WITH-ID only.
 */




/* 
   new_mode:
   use Tunnel API to build the ERSPAN tunnel
    and mirror API to set mirror destination + [encap-id]. 
    legacy-mode: mirror API set both
*/


/* how to run:

cint;
cint_reset();
exit; 
cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c 
cint ../../../../src/examples/dpp/cint_l2_mact.c 
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_ip_tunnel.c 
cint ../../../../src/examples/dpp/cint_mirror_erspan.c
cint
int rv; 
 
int new_mode; 
new_mode = 1;
 
 
int in_vlan=2, in_port=13,out_port=14; 
int mirror_port = 13; 
int span_mirror_port = 13;
int verbose = 2; 
verbose = 2;
G_mirror_type = BCM_MIRROR_PORT_INGRESS; 
rv = mirror_with_erspan_example(unit,in_port,in_vlan++,out_port,mirror_port,0x123,0xab, 0x8100, 7); 
rv = mirror_with_rspan_example (unit,in_port,in_vlan++,out_port,mirror_port,0xabc, 0x8100, 7);
rv = mirror_with_span_example (unit,in_port,in_vlan++,out_port,span_mirror_port); 
 

exit;
vlan add 2 PBM=0x3e000
vlan add 3 PBM=0x3e000
vlan add 4 PBM=0x3e000
m IPT_FORCE_LOCAL_OR_FABRIC FORCE_LOCAL=1 

*/ 


/* 
   remove port 13 from membership of 3. 
dune "ppd_api trap_mgmt trap_to_eg_action_map_set trap_type_bitmap_ndx vsi_membership eg_action_profile 1"
dune "ppd_api eg_filter port_info_set out_port_ndx 13 enable_type vsi_membership"
dune "ppd_api eg_filter vsi_port_membership_set vsid_ndx 3 out_port_ndx 13 is_member 0"
dune "ppd_api eg_filter vsi_port_membership_set vsid_ndx 0xabc out_port_ndx 13 is_member 1"
>>expected packet pass
     
dune "ppd_api eg_filter vsi_port_membership_set vsid_ndx 0xabc out_port_ndx 13 is_member 0"
>>expected packet is filtered
*/



/* can be BCM_MIRROR_PORT_INGRESS or BCM_MIRROR_PORT_EGRESS*/
int G_mirror_type;
G_mirror_type = BCM_MIRROR_PORT_INGRESS;

int new_mode = 1;
int set_outbound_mode(int outbound){
    if(outbound) {
        G_mirror_type = BCM_MIRROR_PORT_EGRESS;
    } else {
        G_mirror_type = BCM_MIRROR_PORT_INGRESS;
    }
    return 0;
}


/*
 * buid two IP tunnels with following information:
 * Tunnnel 1: 
 *   -  sip   = 160.0.0.17
 *   -  dip   =  161.0.0.17
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeGre4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 00:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + No Fec defined over this tunnel
 *  
 * Tunnnel 2: 
 *   -  sip   = 170.0.0.17
 *   -  dip   =  171.0.0.17
 *   -  dscp  = 11;
 *   -  ttl   = 50;
 *   -  type  = bcmTunnelTypeGre4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 20:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + Define FEC point to this tunnel
 *  
 *   returned value:
 *      intf_id[] : includes the interface-id to forward to the built tunnels.
 *         is IP-tunnel Id.        
 *      tunnel_gport[] includes the gport-id pointing to the IP tunnel
 */
int ipv4_tunnel_build_tunnels_erspan(int unit, int out_port, bcm_if_t* intf_id, bcm_gport_t *tunnel_gport, bcm_mac_t next_hop_mac){
    int rv;
    int ing_intf_in;
    int ing_intf_out; 
    int fec[2] = {0x0,0x0};
    int flags;
    int out_vlan = 100;
    int encap_id[2]={0x0,0x0};
    /* my-MAC {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  */
    bcm_mac_t mac_address  = ip_tunnel_my_mac_get();
    bcm_tunnel_initiator_t tunnel_1;

    /* tunnel 1 info*/
    int sip1 = 0xA0000011; /* 160.0.0.17*/
    int dip1 = 0xA1000011; /* 161.0.0.17*/
    int dscp1 = 10; 
    int ttl1 = 60; 
    int type1 = ip_tunnel_glbl_info.tunnel_1_type;    
    int tunnel_itf1=0;
    
    /*** create egress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
      return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    
    /*** create IP tunnel 1 ***/
    bcm_tunnel_initiator_t_init(&tunnel_1);
    tunnel_1.dip = dip1;
    tunnel_1.sip = sip1;
    tunnel_1.dscp = dscp1;
    tunnel_1.flags = 0;
    tunnel_1.ttl = ttl1;
    tunnel_1.type = type1;
    tunnel_1.vlan = out_vlan;    
    tunnel_1.dscp_sel = bcmTunnelDscpAssign;
    tunnel_itf1 = *tunnel_gport;
    rv = add_ip_tunnel(unit,&tunnel_itf1,&tunnel_1);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 1\n");
    }
    if (!is_device_or_above(unit,JERICHO)) {
      tunnel_1.vlan = tunnel_itf1 % 16; /* tunnel_id % 16. Used in case custom_feature_erspan_mc_support is set */      
      rv = remove_ip_tunnel(unit,tunnel_itf1);
      if (rv != BCM_E_NONE) {
          printf("Error, remove_ip_tunnel 1\n");
      }
      rv = add_ip_tunnel(unit,&tunnel_itf1,&tunnel_1);
      if (rv != BCM_E_NONE) {
          printf("Error, add_ip_tunnel second-try 1\n");
      }
    }


    if(verbose >= 1) {
        printf("created IP tunnel_1 on intf:0x%08x \n",tunnel_itf1);
    }

    /*** using egress object API set MAC address for tunnel 1 interface, without allocating FEC enty ***/
    create_l3_egress_s l3eg2;
    sal_memcpy(l3eg2.next_hop_mac_addr, next_hop_mac , 6);
    l3eg2.out_tunnel_or_rif = tunnel_itf1;
    l3eg2.vlan = 0;
    l3eg2.arp_encap_id = encap_id[0];

    rv = l3__egress_only_encap__create(unit,&l3eg2);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_encap__create\n");
        return rv;
    }

    encap_id[0] = l3eg2.arp_encap_id;
    if(verbose >= 1) {
        printf("no FEC is allocated FEC-id =0x%08x, \n", fec[0]);
        printf("next hop mac at encap-id 0x%08x, \n", encap_id[0]);
    }

   /* interface for tunnel_1 is IP-tunnel ID */
    *intf_id = tunnel_itf1;

    /* refernces to created tunnels as gport */
    *tunnel_gport = tunnel_1.tunnel_id;
   
    return rv;
}

/*
 * set inbound mirror according to <in_vlan, in_port> to VLAN-tag (tunnel) 
 * over  <dest_port> 
 *  
 * in_vlan: vlan identifed on packet (or port default). 
 * in_port: incomming local port. 
 *  ** mirror info ** 
 * dest_port: destination system port for mirroring. 
 * vlan-tag: vlan-id, tpid, pcp
 */
int mirror_with_rspan_example(int unit, int in_port, int in_vlan, int dest_port, int mirror_port, int new_vlan_id, int new_tpid, int new_pcp, int new_cfi) {
    int rv;
    bcm_mirror_destination_t dest = {0}, dest_get = {0};
    bcm_mirror_port_info_t port_info = {0};
    int out_count;
    int intf;
    int is_qax;
    char cmd[300] = {0};
    bcm_mac_t dummy_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  


    /* open in_vlan and attach in_port to it */
     rv = l2_open_vlan_with_port(unit, in_vlan, in_port);
     if(rv != BCM_E_NONE){
             printf("failed to add inbound in_port+in_vlan to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    rv = l2_open_vlan_with_port(unit, in_vlan, dest_port);
     if(rv != BCM_E_NONE){
             printf("failed to add inbound in_port+in_vlan to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    /* build interface with new-vlan (RSPAN-vlan) */
    rv = vlan__open_vlan_per_mc(unit, new_vlan_id, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", new_vlan_id, unit);
    }
    rv = bcm_vlan_gport_add(unit, new_vlan_id, mirror_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", mirror_port, new_vlan_id);
      return rv;
    }
    
    create_l3_intf_s create_intf;
    create_intf.vsi = new_vlan_id;
    create_intf.my_global_mac = dummy_mac_address;
    create_intf.my_lsb_mac = dummy_mac_address;
    
    rv = l3__intf_rif__create(unit, &create_intf);
    intf = create_intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

     if(verbose >= 2 ) {
         printf("opened RIF-id:0x%08x \n", intf);
     }

     /* Create the tunnels seperately from the mirror.*/
     bcm_tunnel_initiator_t rspan_tunnel;
     bcm_l3_intf_t l3_intf;
     bcm_tunnel_initiator_t_init(& rspan_tunnel);

     rspan_tunnel.type       = bcmTunnelTypeRspan;
     /* vlan-id */
     rspan_tunnel.vlan = new_vlan_id;
     rspan_tunnel.tpid = new_tpid;
     rspan_tunnel.pkt_pri = new_pcp;
     rspan_tunnel.pkt_cfi = new_cfi;

     /* set tunnel_id to control which outlif to set the span tunnel
        BCM_GPORT_TUNNEL_ID_SET(erspan_tunnel.tunnel_id,0x5000);
     */
     rv = bcm_tunnel_initiator_create(unit, &l3_intf, &rspan_tunnel);
     if (rv != BCM_E_NONE) {
         printf("Error, create tunnel initiator \n");
         return rv;
     }


     /*
      * if mirror destination is Unicast, set encap-id 
      */
     dest.flags = BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
     dest.encap_id = BCM_GPORT_TUNNEL_ID_GET(rspan_tunnel.tunnel_id);

     /* 2. set Mirror destination */
     dest.gport = mirror_port; /* dest port*/

    
    /* create the destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination 1, return value %d\n", rv);
        return rv;
    }
    if(verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    if (rv = bcm_mirror_destination_get(unit,dest.mirror_dest_id,&dest_get)) {
        printf("failed to get mirror destination 1, return value %d\n", rv);
        return rv;
    }
    if (dest_get.encap_id != dest.encap_id) {
        printf("get to get mirror destination encap id error\n");
        return -1;
    }

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */    
    if (rv = bcm_mirror_port_vlan_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port, in_vlan, G_mirror_type, dest.mirror_dest_id)) {
        printf("failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    /* set System mirror ID in case of Egress mirroring */
    if (G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(port_info.mirror_system_id, 40); /* Set Outbound System mirror ID to be 40. That will be the value of the Source System port */
        rv = bcm_mirror_port_info_set(unit, dest_port, BCM_MIRROR_PORT_EGRESS, &port_info);
        if (rv != 0) {
            printf("failed bcm_mirror_port_info_set, return value %d\n", rv);
            return rv;
        }
    }

    if(verbose >= 2 ) {
        printf("created RSPAN mirror, encap-id: 0x%08x  dest:0x%08x \n", dest.encap_id,dest.mirror_dest_id);
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }
        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }
        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port);
    }

    rv = is_qumran_ax(unit, &is_qax);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_qumran_ax\n", rv);
        return rv;
    }

    if (is_qax) {
        bshell(unit, "m ITE_ITPP_GENERAL_CFG ENABLE_MRR_SNP_NEW_HEADERS=0 ENABLE_OUTRIF_STAMPING=0");
    }

    return rv;
}	


/*
 * mirror_with_erspan_example  
 * - build IP tunnels. 
 * - create mirror destination with erspan, 
 * - set inbound mirror send to create tunnel
 */
/* for B1 and below (when ll_tpid is 0 no tag is created) */
int mirror_with_erspan_example(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int span_id, 
                               int ll_vlan_id, int ll_tpid, int ll_pcp){
    int intf_id;
    bcm_gport_t tunnel_gport;
    int rv, is_qax;
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    /* set tunnel type to include GRE*/
    ip_tunnel_glbl_init(unit,0); 
    /* build IP tunnels, and get back interfaces */
    rv = ipv4_tunnel_build_tunnels_erspan(unit, mirror_port, &intf_id, &tunnel_gport,next_hop_mac);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    /* add inbound mirror to send out of the create ip tunnel with  span encapsulation */
    rv = mirror_into_tunnel(unit, in_vlan, in_port, out_port, mirror_port, intf_id, span_id, ll_vlan_id, ll_tpid, ll_pcp, next_hop_mac);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    rv = is_qumran_ax(unit, &is_qax);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_qumran_ax\n", rv);
        return rv;
    }

    if (is_qax) {
        bshell(unit,"m ITE_STAMPING_FTMH_OUTLIF_ENABLE STAMP_FTMH_OUTLIF_EN=0xaa");
    } else {
        bshell(unit,"m IPT_STAMPING_FTMH_OUTLIF_ENABLE STAMP_FTMH_OUTLIF_TYPE_EN=0x7775");
    }
    
    return rv;
}

int mirror_with_erspan_example_iproute(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int span_id, 
                               int ll_vlan_id, int ll_tpid, int ll_pcp){
    int intf_id;
    bcm_gport_t tunnel_gport;
    int rv;
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    /* set tunnel type to include GRE*/
    ip_tunnel_glbl_init(unit,0); 
    /* build IP tunnels, and get back interfaces */
    rv = ipv4_tunnel_build_tunnels_erspan(unit, mirror_port, &intf_id, &tunnel_gport,next_hop_mac);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    /* add inbound mirror to send out of the create ip tunnel with  span encapsulation */
    rv = mirror_into_tunnel_iproute(unit, in_vlan, in_port, out_port, mirror_port, intf_id, span_id, ll_vlan_id, ll_tpid, ll_pcp, next_hop_mac, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

	bshell(unit,"m IPT_STAMPING_FTMH_OUTLIF_ENABLE STAMP_FTMH_OUTLIF_TYPE_EN=0x7775");
	
    return rv;
}


/* 
 * This function is used for testing mirror-with-span on a trunk (link aggregation groups [LAG]) for load balancing (LB)
 * It receives a trunk id and and initilizes the mirror_with_span_example with that lag as the mirror_port 
 * The test should show that the load is balanced evenly between the local ports in the trunk specified 
 */
int mirror_with_span_lag_lb_example(int unit, int in_port, int in_vlan, int dest_port, int trunk_id) {
    int rv;
    bcm_gport_t mirror_port;
    BCM_GPORT_TRUNK_SET(mirror_port,trunk_id);
    return mirror_with_span_example(unit,in_port,in_vlan,dest_port,mirror_port);
}

/*
 * set inbound mirror according to <in_vlan, in_port> to <dest_port> 
 *  
 * in_vlan: vlan identifed on packet (or port default). 
 * in_port: incomming local port. 
 *  ** mirror info ** 
 * dest_port: destination system port for mirroring. 
 */
int mirror_with_span_example(int unit, int in_port, int in_vlan, int dest_port, int mirror_port) {
    int rv;
    bcm_mirror_destination_t dest = {0};
    bcm_mirror_port_info_t port_info = {0};
    bcm_gport_t port1, out_gport;
    int out_count;

     rv = l2_open_vlan_with_port(unit, in_vlan, in_port);
     if(rv != BCM_E_NONE){
             printf("failed to add inbound port1+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    rv = l2_open_vlan_with_port(unit, in_vlan, dest_port);
    if(rv != BCM_E_NONE){
             printf("failed to add inbound port1+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

     /* enabling RSPAN on port (in case port is currently configured with ERSPAN)- */
     /* port can do ERSPAN or RSPAN/SPAN but not both */
     bcm_port_control_set(unit, mirror_port, bcmPortControlErspanEnable, 0);
     if ((rv != BCM_E_NONE)) {
         printf("Error, bcm_port_control_set failed: %d\n", rv);
         return rv;
     }

    /* set mirror destinations parameters, as his is simple mirroring,
     * only physical port is in use other parameters should be cleared (0)
     */
    dest.gport = mirror_port;
    /* no tunnels */
    dest.flags = 0;
    dest.tunnel_id = 0; 
    
    /* create mirror destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination 1, return value %d\n", rv);
        return rv;
    }

    if(verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }
    
    /* mirror <in_port, in_vlan> to created mirror destination
     * BCM_MIRROR_PORT_INGRESS: used to indicate inbound mirroring
     */
    if (rv = bcm_mirror_port_vlan_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port, in_vlan, G_mirror_type, dest.mirror_dest_id)) {
        printf("failed to add inbound port1+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    /* set System mirror ID in case of Egress mirroring */
    if (G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(port_info.mirror_system_id, 40); /* Set Outbound System mirror ID to be 40. That will be the value of the Source System port */
        rv = bcm_mirror_port_info_set(unit, dest_port, BCM_MIRROR_PORT_EGRESS, &port_info);
        if (rv != 0) {
            printf("failed bcm_mirror_port_info_set, return value %d\n", rv);
            return rv;
        }
    }

    if(verbose >= 2 ) {
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }
        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }

        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port);
    }


  return rv;
}	

void erspan_set_eei_invalid(int unit, bcm_gport_t mirror_id)
{
    int offset, is_qax, rv, i;
    uint32 user_header_size = 0, val[2] = {0};
    char cmd[300] = {0};

    printf("Mirror id is: %#x\n", mirror_id);

    rv = is_qumran_ax(unit, &is_qax);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_qumran_ax\n", rv);
        return rv;
    }

    if (BCM_GPORT_IS_MIRROR_MIRROR(mirror_id)) {
        offset = BCM_GPORT_MIRROR_GET(mirror_id);
        printf("Erspan set eei invalid: mirror id is %d\n", offset);

        if (is_qax) {
            offset += 16;
            bshell(unit, "m ITE_ITPP_GENERAL_CFG ENABLE_MRR_SNP_NEW_HEADERS=1");
            sprintf(cmd, "mod ITE_SNP_MIRR_SYS_HEADER_INFO %d 1 HEADER_SIZE=0x10 NEW_SYSTEM_HEADER=0x14c0064653202004001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", offset);
            bshell(unit, cmd);
        }
    } else {
        printf("Mirror id is illegal!\n");
    }
}


/*
 * set inbound mirror according to <in_vlan, in_port> to IP tunnel <ip_tunnel> with <erspan_value> encapsulation 
 * over  <dest_port> 
 *  
 * in_vlan: vlan identifed on packet (or port default). 
 * in_port: incomming local port. 
 *  ** mirror info ** 
 * dest_port:    destination system port for mirroring. 
 * ip_tunnel:    IP-tunnel ID. crearing using <ipv4_tunnel_build_tunnels_erspan> 
 * erspan_value: erspan value to push in packet. 
 * vlan-tag:     vlan-tag to push in packet ll_vlan_id, ll_tpid, int ll_pcp (when tpid is 0 no tag is created)
 */
int mirror_into_tunnel(int unit, int in_vlan, int in_port, int dest_port, int mirror_port, bcm_if_t ip_tunnel, int erspan_value, 
                       int ll_vlan_id, int ll_tpid, int ll_pcp, bcm_mac_t next_hop) {
    int rv;
    bcm_mirror_destination_t dest = {0}, dest_get = {0};
    int out_count, is_arad_only;
    bcm_vlan_t vlan1 = 100;
    bcm_mirror_port_info_t port_info = {0};
    bcm_mac_t ll_src_mac  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  
    bcm_mac_t ll_dst_mac  = next_hop;  

    /* open in_vlan and attach in_port to it */
    rv = l2_open_vlan_with_port(unit, in_vlan, in_port);
    if(rv != BCM_E_NONE){
            printf("failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
       return rv;
   }
  
    rv = l2_open_vlan_with_port(unit, in_vlan, dest_port);
    if(rv != BCM_E_NONE){
            printf("failed to add inbound out_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
       return rv;
   }

     /* create ERSPAN tunnel */

     /* Arad+: use Tunnel API to build the ERSPAN tunnel
      * and mirror API to set mirror destination + [encap-id]. 
      *  
      * legacy-mode: mirror API set both 
      */
     
     if (!is_device_or_above(unit,JERICHO)) {

         /* enabling ERSPAN on port - */
         /* port can do ERSPAN or RSPAN/SPAN but not both */
         rv = bcm_port_control_set(unit, mirror_port, bcmPortControlErspanEnable, 1);
         if ((rv != BCM_E_NONE)) {
             printf("Error, bcm_port_control_set failed: %d\n", rv);
             return rv;
         }

         dest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
         dest.tunnel_id = ip_tunnel;

         /* SPAN Header */
         dest.flags |= BCM_MIRROR_DEST_TUNNEL_IP_GRE | BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID;
         dest.span_id  = erspan_value;

         /* L2 */
         dest.flags |= BCM_MIRROR_DEST_TUNNEL_L2;
         /* vlan TAG */
         dest.tpid     = ll_tpid; /* tpid=0 means no tagging */
         dest.vlan_id  = ll_vlan_id;
         dest.pkt_prio = ll_pcp;
         /* MACs */
         dest.dst_mac  = ll_dst_mac;
         dest.src_mac  = ll_src_mac;

         /* the API forces encap_ip=tunnel_id[3:0] */
         dest.encap_id  = dest.tunnel_id & 0xf;

         /* set Mirror destination */
         dest.gport = mirror_port; /* dest port*/
     } else {
         /* 1. build ERSPAN tunnel */
         bcm_tunnel_initiator_t erspan_tunnel;
         bcm_l3_intf_t intf;


         bcm_tunnel_initiator_t_init(&erspan_tunnel);
         bcm_l3_intf_t_init(&intf);
         /* span info */
         erspan_tunnel.type       = bcmTunnelTypeErspan;
         erspan_tunnel.span_id    = erspan_value;
         erspan_tunnel.l3_intf_id = ip_tunnel; /* points to ip tunnel */
         /* set tunnel_idto control which outlif to set the span tunnel
            BCM_GPORT_TUNNEL_ID_SET(erspan_tunnel.tunnel_id,0x5000);
         */
         rv = bcm_tunnel_initiator_create(unit, &intf, &erspan_tunnel);
         if (rv != BCM_E_NONE) {
             printf("Error, create tunnel initiator \n");
             return rv;
         }


             /* if mirror desination is Multicast:
              * then set dest.gport using BCM_GPORT_MCAST_SET 
              * and that's all. 
              *  
              * if mirror destination is Unicast, set encap-id 
              */
             /*  */
             dest.flags = BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID | BCM_MIRROR_DEST_EEI_INVALID;
             dest.encap_id = BCM_GPORT_TUNNEL_ID_GET(erspan_tunnel.tunnel_id); 

             /* 2. set Mirror destination */
             dest.gport = mirror_port; /* dest port*/
         }

    
     /* create the destination */
     if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination 1, return value %d\n", rv);
        return rv;
    }

    if(verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
        printf("created ERSPAN mirror, encap-id: 0x%08x  ip-tunnel:0x%08x  dest:0x%08x \n", dest.encap_id,ip_tunnel,  dest.mirror_dest_id);
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }
        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }
        
        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port);
    }

    if (rv = bcm_mirror_destination_get(unit,dest.mirror_dest_id,&dest_get)) {
        printf("failed to get mirror destination information, return value %d\n", rv);
        return rv;
    }

    rv = is_arad_only(unit, &is_arad_only);
    if (rv != 0)  {
        printf("Get arad failed: %d\n", rv);
        return rv;
    }

    if (is_arad_only) {
        if (dest_get.encap_id != (dest.tunnel_id & 0xf)) {
            printf("get to get mirror destination encap id error\n");
            return -1;
        }
    } else {
        if (dest_get.encap_id != dest.encap_id) {
            printf("get mirror destination encap id error\n");
            return -1;
        }
    }

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */    
    if (rv = bcm_mirror_port_vlan_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port, in_vlan, G_mirror_type, dest.mirror_dest_id)) {
        printf("failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    /* set System mirror ID in case of Egress mirroring */
    if (G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(port_info.mirror_system_id, 40); /* Set Outbound System mirror ID to be 40. That will be the value of the Source System port */
        rv = bcm_mirror_port_info_set(unit, dest_port, BCM_MIRROR_PORT_EGRESS, &port_info);
        if (rv != 0) {
            printf("failed bcm_mirror_port_info_set, return value %d\n", rv);
            return rv;
        }
    }

  return rv;
}


int mirror_into_tunnel_iproute(int unit, int in_vlan, int in_port, int dest_port, int mirror_port, bcm_if_t ip_tunnel, int erspan_value, 
                       int ll_vlan_id, int ll_tpid, int ll_pcp, bcm_mac_t next_hop, int is_mc, int mc_id) {
    int rv;
    bcm_mirror_destination_t dest = {0};
    int out_count;
    bcm_vlan_t vlan1 = 100;
    bcm_mirror_port_info_t port_info = {0};
    bcm_mac_t ll_src_mac  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  
    bcm_mac_t ll_dst_mac  = next_hop;  
    int gports[1], cuds[1];
    int my_mod = 0, remote_mod = 0;

    /* setup ip route */
    printf("------------------setup ip route begin----------------------\n");
    rv = basic_example(unit,1,in_port,dest_port);
    if (rv != BCM_E_NONE) {
        printf("Error, create ip route failed!\n");
        return rv;
    }
    printf("------------------setup ip route end----------------------\n");
     /* create ERSPAN tunnel */

     /* Arad+: use Tunnel API to build the ERSPAN tunnel
      * and mirror API to set mirror destination + [encap-id]. 
      *  
      * legacy-mode: mirror API set both 
      */
     
     if (!is_device_or_above(unit,JERICHO)) {
         /* enabling ERSPAN on port - */
         /* port can do ERSPAN or RSPAN/SPAN but not both */
         rv = bcm_port_control_set(unit, mirror_port, bcmPortControlErspanEnable, 1);
         if ((rv != BCM_E_NONE)) {
             printf("Error, bcm_port_control_set failed: %d\n", rv);
             return rv;
         }

         dest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
         dest.tunnel_id = ip_tunnel;

         /* SPAN Header */
         dest.flags |= BCM_MIRROR_DEST_TUNNEL_IP_GRE | BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID;
         dest.span_id  = erspan_value;

         /* L2 */
         dest.flags |= BCM_MIRROR_DEST_TUNNEL_L2;
         /* vlan TAG */
         dest.tpid     = ll_tpid; /* tpid=0 means no tagging */
         dest.vlan_id  = ll_vlan_id;
         dest.pkt_prio = ll_pcp;
         /* MACs */
         dest.dst_mac  = ll_dst_mac;
         dest.src_mac  = ll_src_mac;

         /* the API forces encap_ip=tunnel_id[3:0] */
         dest.encap_id  = dest.tunnel_id & 0xf;

         /* set Mirror destination */
         dest.gport = mirror_port; /* dest port*/
     } else {
         /* 1. build ERSPAN tunnel */
         bcm_tunnel_initiator_t erspan_tunnel;
         bcm_l3_intf_t intf;


         bcm_tunnel_initiator_t_init(&erspan_tunnel);
         bcm_l3_intf_t_init(&intf);
         /* span info */
         erspan_tunnel.type       = bcmTunnelTypeErspan;
         erspan_tunnel.span_id    = erspan_value;
         erspan_tunnel.l3_intf_id = ip_tunnel; /* points to ip tunnel */
         /* set tunnel_idto control which outlif to set the span tunnel
            BCM_GPORT_TUNNEL_ID_SET(erspan_tunnel.tunnel_id,0x5000);
         */
         rv = bcm_tunnel_initiator_create(unit, &intf, &erspan_tunnel);
         if (rv != BCM_E_NONE) {
             printf("Error, create tunnel initiator \n");
             return rv;
         }


             /* if mirror desination is Multicast:
              * then set dest.gport using BCM_GPORT_MCAST_SET 
              * and that's all. 
              *  
              * if mirror destination is Unicast, set encap-id 
              */
             /*  */
             if (is_mc) {
                 print bcm_stk_my_modid_get(0,&my_mod);
                 if (my_mod != 0) {
                     remote_mod=2;
                 }
                 BCM_GPORT_MODPORT_SET(gports[0],remote_mod,in_port);
                 cuds[0]=BCM_GPORT_TUNNEL_ID_GET(erspan_tunnel.tunnel_id);

                 rv = multicast__open_egress_mc_group_with_local_ports(unit,mc_id,gports,cuds,1,0);
                 if (rv != BCM_E_NONE) {
                     printf("Error, create ergress multicast\n");
                     return rv;
                 }
 
                 BCM_GPORT_MCAST_SET(dest.gport,mc_id);
             } else {
                 dest.flags = BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID | BCM_MIRROR_DEST_EEI_INVALID;
                 dest.encap_id = BCM_GPORT_TUNNEL_ID_GET(erspan_tunnel.tunnel_id); 

                 /* 2. set Mirror destination */
                 dest.gport = mirror_port; /* dest port*/
             }
         }

     /* create the destination */
     if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination 1, return value %d\n", rv);
        return rv;
    }

    if(verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
        printf("created ERSPAN mirror, encap-id: 0x%08x  ip-tunnel:0x%08x  dest:0x%08x \n", dest.encap_id,ip_tunnel,  dest.mirror_dest_id);
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }
        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }
        
        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port);
    }
    
    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */    
    if (rv = bcm_mirror_port_vlan_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port, in_vlan, G_mirror_type, dest.mirror_dest_id)) {
        printf("failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    /* set System mirror ID in case of Egress mirroring */
    if (G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(port_info.mirror_system_id, 40); /* Set Outbound System mirror ID to be 40. That will be the value of the Source System port */
        rv = bcm_mirror_port_info_set(unit, dest_port, BCM_MIRROR_PORT_EGRESS, &port_info);
        if (rv != 0) {
            printf("failed bcm_mirror_port_info_set, return value %d\n", rv);
            return rv;
        }
    }

  return rv;
}



/* print erspan_tunnel_get(0,0x3000);*/
int erspan_tunnel_get(int unit, int outlif){
    int rv;
    bcm_tunnel_initiator_t erspan_tunnel;
    bcm_l3_intf_t intf;
    bcm_tunnel_initiator_t_init(&erspan_tunnel);
    bcm_l3_intf_t_init(&intf);
    /* span info */
    intf.l3a_tunnel_idx = outlif;
    rv = bcm_tunnel_initiator_get(unit,&intf,&erspan_tunnel);
    if (rv != BCM_E_NONE) {
        printf("failed bcm_tunnel_initiator_get %d\n", rv);
        return rv;
    }
    print erspan_tunnel;
    return rv;
}

/* print erspan_tunnel_destroy(0,0x3000); */
int erspan_tunnel_destroy(int unit, int outlif){
    int rv;
    bcm_tunnel_initiator_t erspan_tunnel;
    bcm_l3_intf_t intf;
    bcm_tunnel_initiator_t_init(&erspan_tunnel);
    bcm_l3_intf_t_init(&intf);
    /* span info */
    intf.l3a_tunnel_idx = outlif;
    rv = bcm_tunnel_initiator_clear(unit,&intf);
    if (rv != BCM_E_NONE) {
        printf("failed bcm_tunnel_initiator_clear %d\n", rv);
        return rv;
    }
    return rv;
}


int destroy_all_mirrors(int unit, int in_port, int dest_port, int in_vlan_0, int in_vlan_1, int in_vlan_2){
    int rv = 0;
    bcm_mirror_port_vlan_dest_delete_all(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port, in_vlan_0, G_mirror_type);
    if (rv != BCM_E_NONE) {
        printf("failed bcm_mirror_port_vlan_dest_delete_all (in_vlan_0)%d\n", rv);
        return rv;
    }
    bcm_mirror_port_vlan_dest_delete_all(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port, in_vlan_1, G_mirror_type);
    if (rv != BCM_E_NONE) {
        printf("failed bcm_mirror_port_vlan_dest_delete_all (in_vlan_1)%d\n", rv);
        return rv;
    }
    bcm_mirror_port_vlan_dest_delete_all(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS)?in_port:dest_port, in_vlan_2, G_mirror_type);
    if (rv != BCM_E_NONE) {
        printf("failed bcm_mirror_port_vlan_dest_delete_all (in_vlan_2)%d\n", rv);
        return rv;
    }
    rv = bcm_mirror_destination_traverse(unit, destroy_mirror_destination_cb, NULL);
    if (rv != BCM_E_NONE) {
        printf("failed bcm_mirror_destination_traverse %d\n", rv);
        return rv;
    }
    return rv;
}

int destroy_mirror_destination_cb(int unit, bcm_mirror_destination_t *mirror_dest, void *user_data) {
    int rv = 0;
    printf("\nDestroying mirror_des_id : %d\n", mirror_dest->mirror_dest_id);
    rv = bcm_mirror_destination_destroy(unit,mirror_dest->mirror_dest_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_mirror_destination_destroy %d\n", rv);
    }
    return rv;
}

/*
 * run_with_dvapi_ipv4_tunnel_id_get  
 * - build IP tunnels. 
 * - get ip tunnel ID, 
 *
 * For ARAD B1 and below (when ll_tpid is 0 no tag is created)
 * Must use egress MC as destination to mirror routed packets in QAX, refer to example in mirror_with_erspan_multicast_example
 */
/*  */
int run_with_dvapi_ipv4_tunnel_id_get(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int span_id, 
                               int ll_vlan_id, int ll_tpid, int ll_pcp){
    int intf_id;
    bcm_gport_t tunnel_gport;
    int rv;
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    /* set tunnel type to include GRE*/
    ip_tunnel_glbl_init(unit,0); 
    /* build IP tunnels, and get back interfaces */
    rv = ipv4_tunnel_build_tunnels_erspan(unit, mirror_port, &intf_id, &tunnel_gport, next_hop_mac);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    /* add inbound mirror to send out of the create ip tunnel with  span encapsulation */
    return intf_id;
}

/*
 * QAX must use egress MC as destination to mirror routed packets
 */
int mirror_with_erspan_multicast_example(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int span_id, 
                               int ll_vlan_id, int ll_tpid, int ll_pcp, int mc_id){
    int intf_id;
    bcm_gport_t tunnel_gport;
    int rv;
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    /* set tunnel type to include GRE*/
    ip_tunnel_glbl_init(unit,0);
    /* build IP tunnels, and get back interfaces */
    rv = ipv4_tunnel_build_tunnels_erspan(unit, mirror_port, &intf_id, &tunnel_gport,next_hop_mac);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    /* add inbound mirror to send out of the create ip tunnel with  span encapsulation */
    rv = mirror_into_tunnel_iproute(unit, in_vlan, in_port, out_port, mirror_port, intf_id, span_id, ll_vlan_id, ll_tpid, ll_pcp, next_hop_mac, 1, mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    bshell(unit,"m IPT_STAMPING_FTMH_OUTLIF_ENABLE STAMP_FTMH_OUTLIF_TYPE_EN=0x7775");

    return rv;
}

/*
* SPAN mirroring with timestamp example.
*
* bcm_tunnel_initiator_t is extended when used to create the RSPAN tunnel:
*     aux_data = 0, typical RPAN processing, Encap = {DA, SA, TPID, VLAN}
*     aux_data = 1, SPAN with NTP timestamp tag, Encap = {DA, SA, TPID, NTP_TIMESTAMP(48b)}
*     aux_data = 2, SPAN with 1588 timestamp tag, Encap = {DA, SA, TPID, 1588_TIMESTAMP(48b)}
*
* SOC properties:
*     bcm886xx_rspan_tunnel_enable=1
*     phy_1588_dpll_frequency_lock=1
*     phy_1588_dpll_phase_initial_lo=0x40000000
*     phy_1588_dpll_phase_initial_hi=0x10000000
*
*     For 1588 timestamp:
*         oam_enable=1
*         num_oamp_ports.BCM88650=1
*
* Run:
*     mirror_with_timestamp_tag_example(0, 1, 13, 10, 14, 200, 0x88B5);
*
* Send:
*     debug bcm rx
*     tx 1 psrc=13 data=0000000000020000000000018100000a0899
*
* Expect:
*     000000000002000000000001 <88B5 XXXXXXXXXXXX> 8100000a0899 (where XXXXXXXXXXXX is the timestamp).
*/
int mirror_with_timestamp_tag_example(int unit, int type, int in_port, int in_vlan, int dest_port, int mirror_port, int tpid) {
    bcm_mirror_destination_t dest;
    bcm_tunnel_initiator_t rspan_tunnel;
    bcm_l3_intf_t l3_intf;
    bcm_pbmp_t p, u;
    int rv;

    bcm_vlan_create(unit, in_vlan);

    BCM_PBMP_PORT_SET(p, in_port);
    BCM_PBMP_PORT_ADD(p, dest_port);
    BCM_PBMP_CLEAR(u);
    rv = bcm_vlan_port_add(unit, in_vlan, p, u);


    bcm_tunnel_initiator_t_init(&rspan_tunnel);
    rspan_tunnel.type = bcmTunnelTypeRspan;
    rspan_tunnel.tpid = tpid;
    rspan_tunnel.aux_data = type;

    rv = bcm_tunnel_initiator_create(unit, &l3_intf, &rspan_tunnel);
    if (rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    bcm_mirror_destination_t_init(&dest);
    dest.flags = BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
    dest.encap_id = BCM_GPORT_TUNNEL_ID_GET(rspan_tunnel.tunnel_id);
    dest.gport = mirror_port;

    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination 1, return value %d\n", rv);
        return rv;
    }

    if (rv = bcm_mirror_port_vlan_dest_add(unit, in_port, in_vlan, BCM_MIRROR_PORT_INGRESS, dest.mirror_dest_id)) {
        printf("failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    return rv;
}

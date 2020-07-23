/*~~~~~~~~~~~~~~~~~~~~~~~Mulitpoint VPLS Service~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_vswitch_vpls_ecmp.c
 * Purpose: Example of Open Multi-Point VPLS service with MPLS-ECMP configuration. 
 *          The attached ports can be attachment circuits or PWEs.
 *
 * Attachment circuit (AC): Ethernet port attached to the service based on port-vlan-vlan, 
 *                          connect from access side. 
 * PWE: Virtual circuit attached to the service based on VC-label. Connect to the MPLS 
 *      network side.
 * 
 * For this service, multiple logical ports can be attached where each logical port can be:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
 * |                                             . . .       . . .                                       |
 * |                                           .       .   .       .                                     |
 * |                                       . .    +---------+     .   . .                                |
 * |                                     .        |         |  /\   .     .                              |
 * |                                   .          |   PE3   |  \\          .                             |
 * |                                   .       /\ | Switch  |/\ \\         .                             |
 * |                                    .      / +---------+\\  \\       .                              |
 * |                                   .      /              \\  +------+ .                             |
 * |                                  . +------+               \\ |TUNNEL| .                             |
 * |                                 .  |TUNNEL|                \\+------+.                              |
 * |                                 .  +------+                 \\  \\    .          . . .              |
 * |                  . .            .    /                       \\  \\    .      . .      .     .  .   |
 * |             .  .     . .       .    /\/    +------------+    \/\ \/\   .     . +----+    . .      . | 
 * |    . .   .   +----+     .  +---------+ <---| - - - - - -|---- +---------+   .  |UNIc|             . |
 * |  .     .     |UNIa|--------|         | ----| - - - - - -|---> |         |----- +----+  +----+   .   |           
 * |  .  +----+   +----+     .  |   PE1   |     |    MPLS    |     |   PE2   |--------------|UNId|     . |
 * |   . |UNIb|-----------------| Switch  | <---| - TUNNEL- -|---- | Switch  |--- +----+    +----+     . |
 * | .   +----+              .  +---------+ ----| - - - - - -|---> +---------+  . |UNIe|             .   |
 * |.            ETHERNET     .      .     . .  +------------+              .  .  +----+   ETHERNET .    |
 * | .     .       .     . . .         . .     .       .       .    .      .    .      .       .     .   |
 * |   . .   . . .   . .                         . . .   . . .   . .  . . .      . . .   . . .   . .     |
 * |                                                                                                     |
 * |           +-------------+      +---------------------------------+      +-------------------------+ |
 * |           | Ethr | Data |      | Ethr | MPLS | PWE | Ethr | Data |      | Ethr |S-TAG|C-TAG| Data | |
 * |           +-------------+      +---------------------------------+      +-------------------------+ | 
 * |                                                                                                     |
 * |                      +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                              |
 * |                      | Figure 13: Multipoing VPLS Service Attachment |                              |
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                                                                                                           
 *  This CINT configures a provider edge as shown in Figure 14.                                              
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  |  Access-P1: Access port, defined on port1 with outer-VID10 and inner-VID20 |
 *  |                                                                            |
 *  |                                                                            |
 *  |                                                                            |
 *  |                                                                            |
 *  |                                              .  .  . .  . .  .   .         |
 *  |       . .   . . .                           .                     .        |
 *  |   . .     .       .                        . TUNNELS:(1000,1500)   .       |
 *  |  . +--+             .                      . (ECMP)  (1001,1501)   .       |
 *  | .  |  |----------------------- +---------+    .      (1002,1502)   .       |
 *  | .  +--+  Access-P1<1,10,20>    |         |    .           ...      .       |
 *  |  .                   .         |   PE1   @     .     (1010,1510)  .        |
 *  | .                     .        | Switch  |\     .            .  .          |
 *  | .                    .         +---------+ \  .                .           |             
 *  |  .                 .                .       \ .                 .          |
 *  |    .               .                  .     +---------------+   .          |      
 *  |     .   ETHERNET .                     .  <-|---PWE(2010,in_port)---|-   . |
 *  |       . .  . . .                     .     -|---PWE(1982,out_port)--|->  . |
 *  |                                      .      +------------------------+ .   |
 *  |                                      .            \            .           |
 *  |                                        .           \PWE-1  . .             |
 *  |                                          .       .  @    .                 |
 *  |                                            . . .   . . .                   |      
 *  |            +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *  |            | Figure 14: CINT Provider Edge Configuration |                 |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Explanation:
 *  -   Access-P1: Access port, defined on port 1 with outer-VID 10 and inner-VID 20.
 *  -   PWE-1: network port with incoming VC = 2010, egress-VC = 1982 defined over ecmp tunnels 1000-1010 and 1500-1510.
 *  -   For access ports outer-Tag TPID is 0x8100 and inner Tag TPID is 0x9100.
 *  -   Access-P1 refer to the logical interface (attachment circuit).
 *  -   PWE1 refers to MPLS network ports.
 *  -   P1 refers to the physical ports PWE1 are attached to.
 *
 * Headers:
 *  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *   |  | DA |SA||TPID1 |Prio|VID||  MPLS         |  MPLS         ||   PWE    ||Eth||Data | |
 *   |  |0:11|  ||0x8100|    |100||Label:1000-1010|Label:1500-1510||Label:2010||   ||     | | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                             |
 *   |                | Figure 15: Packets Received from PWE1 |                             |  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *   |  | DA |SA||TPID1 |Prio|VID||  MPLS         |  MPLS         ||   PWE    ||Eth||Data | |
 *   |  |0:11|  ||0x8100|    |100||Label:1000-1010|Label:1500-1510||Label:1982||   ||     | | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                             |
 *   |                | Figure 15: Packets Received from PWE1 |                             |  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * Access side packets:
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |                       tag1                 tag2                           |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   | DA | SA || TPID1 | Prio | VID || TPID2 | Prio | VID ||    Data    |   |
 *   |   |    |    || 0x8100|      | 10  || 0x9100|      | 20  ||            |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |        +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+       |
 *   |        | Figure 18: Packets Received/Transmitted on Access Port 1 |       |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 *
 * Calling sequence:
 *  -   Port TPIDs setting:
 *      -   For P1 set outer TPID to 0x8100 and no inner TPID (using cint_port_tpid.c).
 *  -   Set up MPLS tunnels. Refer to mpls_tunnels_config().
 *      -   Set MPLS L2 termination (on ingress).
 *          -    MPLS packets arriving with DA 00:00:00:00:00:11 and VID 100 causes L2 termination.  
 *          -    Calls bcm_l2_tunnel_add().
 *      -   Add pop entries to MPLS switch.
 *          -    MPLS packet arriving from the MPLS network side with labels 1000-1010 or 1500-1510 the label/s 
 *               are popped (refer to mpls_add_pop_entry).
 *          -    Calls bcm_mpls_tunnel_switch_create().
 *          -    Create MPLS L3 interface (on egress).
 *          -    Packet routed to this L3 interface is set with this MAC. 
 *          -    Calls bcm_l3_intf_create().
 *      -   Create MPLS tunnels over the created l3 interface.
 *          -    Packet routed to above interface is tunneled into MPLS ECMP tunnel, where labels set according to 
 *               load balancing to the values 1000-1010 and 1500-1510.
 *          -    First, tunnel inteface is created.
 *          -    Calls bcm_mpls_tunnel_initiator_create().
 *      -   Create egress object points to the above l3-interface.
 *          -    Packet routed to this egress-object is forwarded to P1 tunneled with above MPLS tunnels 
 *               and with the L3-interface VID and MAC.
 *          -    Calls bcm_l3_egress_create().
 *      -   After creating several interfaces, connect them into one ecmp object.
 *          -    Calls bcm_l3_egress_ecmp_create().
 *  -   Create multipoint VSI #6202 (refer to mpls_port__vswitch_vpls_vpn_create__set).
 *      -   calling bcm_mpls_vpn_id_create().
 *          -    You has to supply VPN ID and multicast groups. For this purpose, the following flags have 
 *               to be present BCM_MPLS_VPN_VPLS|BCM_MPLS_VPN_WITH_ID.
 *          Note that uc/mc/bc group have the same value as VSI. Another option is to set uc_group = VSI, 
 *          and mc = uc + 4k, and bc = mc + 4k.
 *      -   Use open_ingress_mc_group() to open multicast group for flooding.
 *          -    Multicast group ID is equal to VPN ID.
 *  -   Add MPLS-ports to the VSI.
 *      -   vswitch_vpls_add_access_port_1 creates attachment circuit and add them to the Vswitch and 
 *          update the Multicast group.
 *      -   vswitch_vpls_add_network_port_1 creates PWE and add them to the VSI and update the 
 *          multicast group.
 *          - PWE is added in the calls: INGRESS_ONLY configures the ingress object (temination+learning)
 *                                       EGRESS_ONLY configures the egress onject (encapsulation)
 *                                       mpls_port_id must be the same for ingress and egress so that split horizon filter will work.
 * 
 * 
 * Traffic:
 *  Flooding packets incoming from MPLS network:
 *  Send the following packet, where the DA of the inner Ethernet is unknown on VSI.
 *
 *  Flooding Packets Incoming from MPLS Network same as Packets Received from PWE1, see Figure 15
 *
 *  -   Packet is flooded (as DA is not known):
 *      -   Packet is received. 
 *          -    In access P1: physical port P1 with VLAN tag1: VID =10, VLAN tag2: VLAN ID = 20.  
 *          Note that the packet is not received on PWE1 due to split-horizon filtering. 
 *          A packet received from HUB is not flooded back to HUB.
 *  -   The SA of the packet is learned 6202.
 *      -   Call l2_print_mact(unit) to print MAC table content.
 *
 *  Flooding packets incoming access side.
 *  -   Send Ethernet packet from access-P1:
 *      -   Any SA
 *      -   Unknown DA
 *      -   VLAN tag1: VLAN tag type 0x8100, VID =10
 *      -   VLAN tag2: VLAN tag type 0x9100, VID =20 
 *
 *  Flooding Packets from Incoming Access Side same as Packets Received/Transmitted 
 *  on Access Port 1, see Figure 18.
 * 
 *  -   Packet is flooded (as DA is not known).
 *      -   Packet is received: 
 *          -   On physical port P1:
 *              -  PWE1: With VC label. MPLS tunnels in multicast come from only one ECMP entry, in this case the first {1000,1500},
 *              as configured by the multicast table configuration.
 *          Note that the packet is not received on access P1, as packet was injected 
 *          from this logical port, that is, hair-pin filtering.
 *  -   The SA of the packet is learned on VSI 6202.
 *      -   Call l2_print_mact(unit) to print MAC table content. 
 *
 * Sending to known DA
 *  -   Send Ethernet packet from any logical interface PWE or Access port with known DA on VSI 6202 
 *      the packet will be forwarded to the specific logical port with required editing.
 *
 * Remarks:
 *  -   For this application at least two ports are needed, one for access side and one for MPLS network side
 *  -   User can statically add entry to the MACT pointing to a gport using 
 *      vswitch_add_l2_addr_to_gport(unit, gport_id)
 *  -   Set verbose = 1/0 to show/hide informative prints
 *  -   Calling vswitch_vpls_run() as is more than one time would fail because this call will try open same 
 *      VPN twice,  to avoid this user should set vswitch_vpls_info_1.vpn_id different value in order to 
 *      open new service.
 *
 * To Activate Above Settings Run:
 *  	BCM> cint examples/sand/utility/cint_sand_utils_global.c
 *  	BCM> cint examples/dpp/utility/cint_utils_vlan.c
 *  	BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
 * 		BCM> cint examples/dpp/utility/cint_utils_mpls_port.c
 *  	BCM> cint examples/dpp/utility/cint_utils_l3.c
 *  	BCM> cint examples/dpp/utility/cint_utils_multicast.c
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_advanced_vlan_translation_mode.c
 *      BCM> cint examples/dpp/cint_qos.c
 *      BCM> cint examples/dpp/cint_mpls_lsr.c
 *      BCM> cint examples/dpp/cint_vswitch_metro_mp.c
 *      BCM> cint examples/dpp/cint_vswitch_vpls.c
 *      BCM> cint examples/dpp/cint_vswitch_vpls_ecmp.c
 *      BCM> cint
 *      cint> int rv;
 *      cint> rv = vswitch_vpls_run_with_defaults(unit, second_unit); 
 * 
 * Script adjustment:
 *  User can adjust the following attribute of the application
 *  -   vpn_id:             VPN id to create 
 *  -   ac_in_port:         accesss port: physical port connect to customer 
 *  -   pwe_in_port:        network port: physical port connect to MPLs network (for incoming packets)
 *  -   pwe_out_port:       network port: physical port connect to MPLs network (for outgoing packets) 
 *  -   in_vc_label:        incomming VC label 
 *  -   eg_vc_label:        egress VC label 
 *  -   in_tunnel_label:    inner tunnel label 
 *  -   out_tunnel_label:   outer tunnel label 
 */

struct cint_vswitch_vpls_ecmp_info_s {
    int ecmp_size;
    /* tunnels info */
    int in_tunnel_label[10];
    int out_tunnel_label[10];
	
  /* After ecmp_hashing_main is run successfully, this will contain the ECMP object that was created. */
  /* Only works with the bcm_l3_egress_ecmp_* interfaces.*/
  bcm_l3_egress_ecmp_t ecmp;
  /* After ecmp_hashing_main is run successfully, this will contain the FEC objects that were created. */
  bcm_if_t egress_intfs[10]; /* FECs */	
};
cint_vswitch_vpls_ecmp_info_s vswitch_vpls_ecmp_info;

void
vswitch_vpls_ecmp_info_init(int ac_port, int pwe_in_port, int pwe_out_port, int ac_port1_outer_vlan, int ac_port1_inner_vlan,
                       int ac_port2_outer_vlan, int ac_port2_inner_vlan, int vpn_id) {
    int i;

    vswitch_vpls_ecmp_info.ecmp_size = 10;

    /* VPN id to create */
    if (vpn_id < 0) {
        vswitch_vpls_info_1.vpn_id = default_vpn_id;
    } else {
        vswitch_vpls_info_1.vpn_id = vpn_id; 
    }

    /* incomming packet attributes */
    vswitch_vpls_info_1.ac_in_port = ac_port;
    vswitch_vpls_info_1.pwe_in_port = pwe_in_port;
    vswitch_vpls_info_1.pwe_out_port = pwe_out_port;
    vswitch_vpls_info_1.in_vc_label = 2010;
    vswitch_vpls_info_1.eg_vc_label = 1982;

    /* tunnel info */
    for (i=0; i<vswitch_vpls_ecmp_info.ecmp_size; i++) {
        vswitch_vpls_ecmp_info.in_tunnel_label[i] = 1000+i;
        vswitch_vpls_ecmp_info.out_tunnel_label[i] = 1500+i;
    }
    
    vswitch_vpls_info_1.ac_port1_outer_vlan = ac_port1_outer_vlan;
    vswitch_vpls_info_1.ac_port1_inner_vlan = ac_port1_inner_vlan;
    vswitch_vpls_info_1.ac_port2_outer_vlan = ac_port2_outer_vlan;
    vswitch_vpls_info_1.ac_port2_inner_vlan = ac_port2_inner_vlan;

    vswitch_vpls_info_1.access_index_1 = 2;
    vswitch_vpls_info_1.access_index_2 = 3;
    vswitch_vpls_info_1.access_index_3 = 3;

    printf("vswitch_vpls_ecmp_info_init %d\n", vswitch_vpls_info_1.vpn_id);
}

/* initialize the tunnels for mpls routing
 * set termination, for MPLS label 1000,1002
 */
int
mpls_tunnels_config(int unit, int second_unit){

    int CINT_NO_FLAGS = 0;
    int ingress_intf;
    int tunnel_ids[10];
    int egress_intf[10];
    int encap_id[10];
    bcm_mpls_egress_label_t label_array[2];
    bcm_pbmp_t pbmp;
    int rv;
    bcm_mpls_label_t label_for_pop_entry_in, label_for_pop_entry_out;
    int i;
    create_l3_intf_s intf;

    /* set mpls tunneling information with default values */
    mpls_lsr_init(vswitch_vpls_info_1.pwe_in_port, vswitch_vpls_info_1.pwe_out_port, 0x11, 0x22, 0/*in_label*/, 0/*out_label*/, 100, 200, 0);
    mpls_lsr_info_1.eg_port = vswitch_vpls_info_1.pwe_out_port;

    /* open vlan */
    printf("open vlan %d\n", mpls_lsr_info_1.in_vid);
    rv = bcm_vlan_create(unit, mpls_lsr_info_1.in_vid);
    print rv;

    /* add vlan to pwe_port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.eg_port);

    rv = bcm_vlan_port_add(unit, mpls_lsr_info_1.in_vid, pbmp, pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
        return rv;
    }
    
    if (second_unit >= 0)
    {
        printf("open vlan %d\n", mpls_lsr_info_1.in_vid);
        rv = bcm_vlan_create(second_unit, mpls_lsr_info_1.in_vid);
        print rv;

        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.eg_port);

        rv = bcm_vlan_port_add(unit, mpls_lsr_info_1.in_vid, pbmp, pbmp);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
            return rv;
        }
    }
    
    /* l2 termination for mpls routing: packet received on those VID MAC will be L2 terminated  */
    intf.vsi = mpls_lsr_info_1.in_vid;
    intf.my_global_mac = mpls_lsr_info_1.my_mac;
    intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }
    
    if (second_unit >= 0)
    {
        rv = l3__intf_rif__create(second_unit, &intf);
    	ingress_intf = intf.rif;
    	if (rv != BCM_E_NONE) {
    		printf("Error, in l3__intf_rif__create\n");
    		return rv;
    	}
    }
    
    /*  create ingress object, packet will be routed to */
    intf.vsi = mpls_lsr_info_1.eg_vid;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }
    
    if (second_unit >= 0)
    {
        rv = l3__intf_rif__create(second_unit, &intf);
    	ingress_intf = intf.rif;
    	if (rv != BCM_E_NONE) {
    		printf("Error, in l3__intf_rif__create\n");
    		return rv;
    	}
    }

    for (i = 0; i < vswitch_vpls_ecmp_info.ecmp_size; i++) {
        /* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled */
        bcm_mpls_egress_label_t_init(&label_array[0]);
        bcm_mpls_egress_label_t_init(&label_array[1]);
        label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
        if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
           label_array[0].exp = 2;
           if (is_device_or_above(unit, JERICHO2)) {
              label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
           } else {
              label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
           }
        } else {
           if (is_device_or_above(unit, JERICHO2)) {
              label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
           } else {
              label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
           }
        }
        label_array[0].label = vswitch_vpls_ecmp_info.in_tunnel_label[i];
        label_array[0].ttl = 20;
        
        label_array[1].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
        if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
           label_array[1].exp = 4;
           if (is_device_or_above(unit, JERICHO2)) {
              label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
           } else {
              label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
           }
        } else {
           if (is_device_or_above(unit, JERICHO2)) {
              label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
           } else {
              label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
           }
        }
        label_array[1].label = vswitch_vpls_ecmp_info.out_tunnel_label[i];
        label_array[1].ttl = 40;

        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            label_array[1].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }

        if (!is_device_or_above(unit, JERICHO2)) {
            label_array[1].l3_intf_id = ingress_intf;
            rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
        } else {
            /** In JR2, l3_intf_id is used for next-outlif. It must be LIF type.*/
            label_array[1].l3_intf_id = 0;

            /** To avoid failing leagcy tests, we call sand API in JR2 only.*/
            rv = sand_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
        }        
        
        tunnel_ids[i] = label_array[0].tunnel_id;
        printf("Labels: %d, %d, Tunnel id: %d\n", label_array[0].label, label_array[1].label, tunnel_ids[i]);

        if (second_unit >= 0)
        {
            rv = bcm_mpls_tunnel_initiator_create(second_unit,0,2,label_array);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_mpls_tunnel_initiator_create\n");
                return rv;
            }
        }
    }
    vswitch_vpls_info_1.tunnel_id = tunnel_ids[0]; /* interface for multicast PWE */

    /* create ECMP egress object points to this tunnel/interface */

    /* create 10 FEC entries (all with the same out-RIF)
           each FEC will point to a different out-port.
           also set vlan-port membership. each out-port will have a different vlan */
    create_l3_egress_s l3eg;
    for (i = 0; i < vswitch_vpls_ecmp_info.ecmp_size; i++) {
        /* create FEC */       
        sal_memcpy(l3eg.next_hop_mac_addr, mpls_lsr_info_1.next_hop_mac , 6);
        l3eg.out_tunnel_or_rif = tunnel_ids[i];
        l3eg.out_gport = mpls_lsr_info_1.eg_port;
        l3eg.vlan = mpls_lsr_info_1.eg_vid;
        l3eg.fec_id =vswitch_vpls_ecmp_info.egress_intfs[i];
        l3eg.arp_encap_id = encap_id[i];
        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        vswitch_vpls_ecmp_info.egress_intfs[i] = l3eg.fec_id;
        encap_id[i] = l3eg.arp_encap_id;
        printf("Configured tunnel id: %d\n", tunnel_ids[i]);
    }

    bcm_l3_egress_ecmp_t_init(&vswitch_vpls_ecmp_info.ecmp);
    vswitch_vpls_ecmp_info.ecmp.max_paths = vswitch_vpls_ecmp_info.ecmp_size;

    /* create an ECMP, containing the FEC entries */
    rv = bcm_l3_egress_ecmp_create(unit, &vswitch_vpls_ecmp_info.ecmp, vswitch_vpls_ecmp_info.ecmp_size, vswitch_vpls_ecmp_info.egress_intfs);
    if (rv != BCM_E_NONE) {
        printf("bcm_l3_egress_ecmp_create failed: %d \n", rv);
        return rv;
    }

    vswitch_vpls_shared_info_1.egress_intf = vswitch_vpls_ecmp_info.ecmp.ecmp_intf;
    if (second_unit >= 0) {
        create_l3_egress_s l3eg;
        for (i = 0; i < vswitch_vpls_ecmp_info.ecmp_size; i++) {
            /* create FEC */
            sal_memcpy(l3eg.next_hop_mac_addr, mpls_lsr_info_1.next_hop_mac , 6);
            l3eg.out_tunnel_or_rif = tunnel_ids[i];
            l3eg.out_gport = mpls_lsr_info_1.eg_port;
            l3eg.vlan = mpls_lsr_info_1.eg_vid;
            l3eg.fec_id =vswitch_vpls_ecmp_info.egress_intfs[i];
            l3eg.arp_encap_id = encap_id[i];

            rv = l3__egress__create(second_unit,&l3eg);
            if (rv != BCM_E_NONE) {
                printf("Error, l3__egress__create\n");
                return rv;
            }

            vswitch_vpls_ecmp_info.egress_intfs[i] = l3eg.fec_id;
            encap_id[i] = l3eg.arp_encap_id;
            printf("Configured tunnel id: %d\n", tunnel_ids[i]);
        }

        bcm_l3_egress_ecmp_t_init(&vswitch_vpls_ecmp_info.ecmp);
        vswitch_vpls_ecmp_info.ecmp.max_paths = vswitch_vpls_ecmp_info.ecmp_size;

        /* create an ECMP, containing the FEC entries */
        rv = bcm_l3_egress_ecmp_create(second_unit, &vswitch_vpls_ecmp_info.ecmp, vswitch_vpls_ecmp_info.ecmp_size, vswitch_vpls_ecmp_info.egress_intfs);
        if (rv != BCM_E_NONE) {
            printf("bcm_l3_egress_ecmp_create failed: %d \n", rv);
            return rv;
        }

        vswitch_vpls_shared_info_1.egress_intf = vswitch_vpls_ecmp_info.ecmp.ecmp_intf;
    }

    
    for (i = 0; i < vswitch_vpls_ecmp_info.ecmp_size; i++) {
        /* add switch entries to pop MPLS labels  */
        if (mpls_termination_label_index_enable) {
            BCM_MPLS_INDEXED_LABEL_SET(&label_for_pop_entry_out, vswitch_vpls_ecmp_info.out_tunnel_label[i], vswitch_vpls_info_1.access_index_1);
            BCM_MPLS_INDEXED_LABEL_SET(&label_for_pop_entry_in, vswitch_vpls_ecmp_info.in_tunnel_label[i], 1);
        }
        else {
            label_for_pop_entry_out = vswitch_vpls_ecmp_info.out_tunnel_label[i];
            label_for_pop_entry_in = vswitch_vpls_ecmp_info.in_tunnel_label[i];
        }

        rv = mpls_add_pop_entry(unit, label_for_pop_entry_out);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_add_pop_entry\n");
            return rv;
        }
        if (second_unit >= 0)
        {
            rv = mpls_add_pop_entry(second_unit,label_for_pop_entry_out);
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_add_pop_entry on second unit\n");
                return rv;
            }
        }
        
        rv = mpls_add_pop_entry(unit,label_for_pop_entry_in);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_add_pop_entry\n");
            return rv;
        }
        if (second_unit >= 0)
        {
            rv = mpls_add_pop_entry(second_unit,label_for_pop_entry_in);
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_add_pop_entry\n");
                return rv;
            }
        }
    }
   
    return rv;
}


/*
 * create mpls-port
 *   vlan-port: is Logical interface identified by (port-vlan-vlan). 
 */
int
vswitch_vpls_add_network_port_1(int unit, int second_unit,  bcm_gport_t *port_id){
    int rv;
    bcm_mpls_port_t mpls_port_1;
    int unit_or_second = unit;
    int is_two_dev = 0;
    int modid;
    int modport;
    bcm_gport_t egress_mpls_port_id;
    bcm_if_t pwe_encap_id;
    int tm_port;
    mpls_port__ingress_only_info_s ingress_port;
    mpls_port__egress_only_info_s egress_port;
    
    if (second_unit >= 0)
    {
        unit_or_second = second_unit;
        is_two_dev = 1;    
    }   
    
    
    /* Set parameters for both ingress and egress */

    /* set port attribures, key <BC>*/
    printf("mpls_termination_label_index_enable %d",mpls_termination_label_index_enable);
    if (mpls_termination_label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(ingress_port.ingress_pwe_label, vswitch_vpls_info_1.in_vc_label, vswitch_vpls_info_1.access_index_2);
    }
    else {
        printf("MATCH LABEL %x",vswitch_vpls_info_1.in_vc_label);
        ingress_port.ingress_pwe_label = vswitch_vpls_info_1.in_vc_label;
    }

    ingress_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    ingress_port.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;
    if (pwe_cw) {
        ingress_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }

    ingress_port.port = vswitch_vpls_info_1.pwe_in_port;
    ingress_port.learn_egress_label.label = vswitch_vpls_info_1.eg_vc_label;
    ingress_port.learn_egress_label.ttl = 10;
    ingress_port.learn_egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    
    /* Ingress_only */
    ingress_port.ingress_matching_criteria = BCM_MPLS_PORT_MATCH_LABEL;
    ingress_port.learn_egress_if = vswitch_vpls_shared_info_1.egress_intf; /*Learning - MPLS ECMP */
    if (is_device_or_above(unit,JERICHO)) { /* adjust gport id to size of jericho lif table (bigger two times than arad) */
        ingress_port.mpls_port_id = 0x1880A000;
        ingress_port.encap_id = 0xA000;
    } else {
        ingress_port.mpls_port_id = 0x18805000;
        ingress_port.encap_id = 0x5000;
    }
    ingress_port.vpn = vswitch_vpls_shared_info_1.vpn;

    rv = mpls_port__ingress_only_create(unit, &ingress_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }
    printf("INGRESS: add port 0x%08x to vpn \n\r", ingress_port.mpls_port_id);
    printf("ENCAP ID: a%d \n\r", ingress_port.encap_id);
    if (verbose1) {
        printf("add port 0x%08x to vpn \n\r", ingress_port.mpls_port_id);
    }
    mpls_lsr_info_1.mpls_port_id = ingress_port.mpls_port_id;

    /* handle of the created in gport */
    *port_id = ingress_port.mpls_port_id;


    /* Egress_only */
    egress_port.mpls_port_id = ingress_port.mpls_port_id;
    egress_port.encap_id = ingress_port.encap_id;
    egress_port.egress_label = ingress_port.learn_egress_label;
    egress_port.flags = ingress_port.flags;
    egress_port.egress_tunnel_if = vswitch_vpls_info_1.tunnel_id; /* This is used for multicast only */

    if (is_device_or_above(unit, JERICHO2)) {
        egress_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        egress_port.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    }

    rv = mpls_port__egress_only_create(unit_or_second, &egress_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }
    printf("EGRESS: add port 0x%08x to vpn \n\r", egress_port.mpls_port_id);
    egress_mpls_port_id = egress_port.mpls_port_id;

    BCM_L3_ITF_SET(pwe_encap_id, BCM_L3_ITF_TYPE_LIF, egress_port.encap_id);

    if (is_two_dev)
    {
        egress_port.flags |= BCM_MPLS_PORT_WITH_ID;
        egress_port.vpn = vswitch_vpls_shared_info_1.vpn;
        rv = mpls_port__egress_only_create(unit, &egress_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add two devices\n");
            return rv;
        }
    }
    
    /* update Multicast to have the added port  */
    if (is_device_or_above(unit_or_second, JERICHO2)) {
        modport = vswitch_vpls_info_1.pwe_in_port;
    }
    else if (is_device_or_above(unit_or_second,JERICHO)) {
        rv = get_core_and_tm_port_from_port(unit_or_second, vswitch_vpls_info_1.pwe_in_port, &modid, &tm_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
        }
        BCM_GPORT_MODPORT_SET(modport, modid, tm_port);
    }
    else {
        rv = bcm_stk_modid_get(unit_or_second, &modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_get\n");
            return rv;
        }
        BCM_GPORT_MODPORT_SET(modport, modid, vswitch_vpls_info_1.pwe_in_port);
    }

    /* Adding egress mpls port to multicast because when egress and ingress are configured separately the multicast should point to the outlif */
    rv = multicast__mpls_port_add(unit_or_second, vswitch_vpls_shared_info_1.vpn, modport, egress_mpls_port_id, 0/*is_egress*/);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__mpls_port_add\n");
        return rv;
    }
    if (is_two_dev)
    {
        rv = multicast__mpls_port_add(unit, vswitch_vpls_shared_info_1.vpn, modport, egress_mpls_port_id, 0/*is_egress*/);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__mpls_port_add\n");
            return rv;
        }
    }
    if(verbose1){
        printf("add port   0x%08x to multicast \n\r",egress_mpls_port_id);
    }
    
    return rv;
}

int
vswitch_vpls_run(int unit, int second_unit){

    int nof_outer_tpids;
    int nof_inner_tpids;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_port_t mpls_port_1;
    bcm_vlan_t  vpn;
    int rv;
    int unit_or_second = second_unit;
    int trap_code;
    bcm_rx_trap_config_t trap_config;

    /* init values */
  
    /* set ports to identify double tags packets */

    if (second_unit < 0)
    {
        unit_or_second = unit;
    }

    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    if (pwe_cw) {
        int control_word = is_device_or_above(unit, JERICHO2) ? 0 : pwe_cw;
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPWControlWord, control_word);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set\n");
            return rv;
        }
    }
  
    printf("vswitch_vpls_run %d\n", vswitch_vpls_info_1.vpn_id);

    /* JR2 does not support per port TPIDs, we use the default TPID configurations. */
    if (!is_device_or_above(unit, JERICHO2)) {
        port_tpid_init(vswitch_vpls_info_1.pwe_in_port,1,0);
        rv = port_tpid_set(unit_or_second);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set\n");
            return rv;
        }
    
        port_tpid_init(vswitch_vpls_info_1.ac_in_port,1,1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set\n");
            return rv;
        }
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
        are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode || is_device_or_above(unit, JERICHO2)) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }
   
    /* Init vlan */  
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port1_outer_vlan);
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port1_inner_vlan);
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port2_outer_vlan);
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port2_inner_vlan);

    if (second_unit >= 0)
    {
        vlan__init_vlan(second_unit,vswitch_vpls_info_1.ac_port1_outer_vlan);
        vlan__init_vlan(second_unit,vswitch_vpls_info_1.ac_port1_inner_vlan);
        vlan__init_vlan(second_unit,vswitch_vpls_info_1.ac_port2_outer_vlan);
        vlan__init_vlan(second_unit,vswitch_vpls_info_1.ac_port2_inner_vlan);
    } 
    printf("vswitch_vpls_run configure mpls tunnels\n");
    /* configure MPLS tunnels */
    rv  = mpls_tunnels_config(unit, second_unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_tunnels_config\n");
        return rv;
    }

    printf("vswitch_vpls_run create vswitch\n");
    /* create vswitch */
    rv = mpls_port__vswitch_vpls_vpn_create__set(unit, vswitch_vpls_info_1.vpn_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_port__vswitch_vpls_vpn_create__set\n");
        return rv;
    }
    if (second_unit >= 0)
    {
        rv = mpls_port__vswitch_vpls_vpn_create__set(second_unit, vswitch_vpls_info_1.vpn_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_port__vswitch_vpls_vpn_create__set\n");
            return rv;
        }
    }

    printf("vswitch_vpls_run add vlan access port\n");
    /* add mpls access port */
    rv = vswitch_vpls_add_access_port_1(unit, second_unit, &vswitch_vpls_info_1.access_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_add_access_port_1\n");
        return rv;
    }
    printf("Added access port\n");

    printf("vswitch_vpls_run add mpls network port\n");
    /* add mpls network port */
    rv = vswitch_vpls_add_network_port_1(unit, second_unit, &network_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_add_network_port_1\n");
        return rv;
    }
    printf("Added network port\n");

    return rv;
}



int
vswitch_vpls_run_with_defaults(int unit, int second_unit){

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    vswitch_vpls_ecmp_info_init(1,13,13,10,20,15,30,default_vpn_id);
    return vswitch_vpls_run(unit, second_unit);
}

int
vswitch_vpls_run_with_defaults_dvapi(int unit, int acP, int pweP){
    int second_unit = -1;

    int rv = BCM_E_NONE;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }
    vswitch_vpls_ecmp_info_init(acP,pweP,pweP,10,20,15,30,default_vpn_id);
    return vswitch_vpls_run(unit, second_unit);
}


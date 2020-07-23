/*~~~~~~~~~~~~~~~~~~~~~~~Mulitpoint VPLS Service~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_vswitch_vpls_roo.c
 * Purpose: Example of Open Multi-Point VPLS service with PWE-ECMP configuration,
 *          Routing over PWE and Bridging over PWE (using the same HW objects)
 *          The attached ports can be attachment circuits or PWEs.
 * BCM88660 supports only Bridging over PWE
 * BCM88675 supports both Bridging over PWE and Routing over PWE
 *
 *
 * Attachment circuit (AC): Ethernet port attached to the service based on port-vlan-vlan, 
 *                          connect from access side. 
 * 
 * For this service, multiple logical ports can be attached where each logical port can be:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
 * |                                             . . .       . . .                                       |
 * |                                           .       .   .       .                                     |
 * |                                       . .    +---------+     .   . .                                |
 * |                                     .        |         |  /\   .     .                              |
 * |                                   .          |   PE3   |  \\          .                             |
 * |                                   .       /\ |   OEBR  |/\ \\         .                             |
 * |                                    .      // +---------+\\  \\       .                              |
 * |                                   .      //              \\  +------+ .                             |
 * |                                  . +------+               \\ |TUNNEL| .                             |
 * |                                 .  |TUNNEL|                \\+------+.                              |
 * |                                 .  +------+                 \\  \\    .          . . .              |
 * |                  . .            .    //                      \\  \\    .      . .      .     .  .   |
 * |             .  .     . .       .    /\/    +------------+    \/\ \/\   .     . +-------+ . .   .    | 
 * |    . .   .   +----+     .  +---------+ <---| - - - - - -|---- +---------+   .  |Routec |       .    |
 * |  .     .     |UNIa|--------|         | ----| - - - - - -|---> |         |----- +-------+ +----+ .   |           
 * |  .  +------+ +----+     .  |   PE1   |     |    MPLS    |     |   PE2   |----------------|UNId|   . |
 * |   . |Routeb| --------------|  OEBR   | <---| - TUNNEL- -|---- |   OEBR  |--- +----+      +----+   . |
 * | .   +------+            .  +---------+ ----| - - - - - -|---> +---------+  . |UNIe|             .   |
 * |.            Native       .      .     . .  +------------+              .  .  +----+   Native   .    |
 * | .     .       .     . . .         . .     .       .       .    .      .    .      .       .     .   |
 * |   . .   . . .   . .                         . . .   . . .   . .  . . .      . . .   . . .   . .     |
 * |                                                                                                     |
 * |      +---------------------+    +--------------------------------------+  +-----------------+       |
 * |      |  Ethr   | IP | Data |    | Ethr | MPLS | PWE | Ethr | IP | Data |  | Ethr |IP | Data |       |
 * |      +---------------------+    +--------------------------------------+  +-----------------+       | 
 * |                                                                                                     |
 * |                      +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+      |
 * |                      | Figure : Multipoing VPLS Service Attachment in Routing Overlay Netowrk|      |
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
 *  |   . .     .       .                        . TUNNELS:(1982,1000)   .       |
 *  |  . +--+             .                      . (ECMP)  (1982,1001)   .       |
 *  | .  |  |----------------------- +---------+    .      (1982,1002)   .       |
 *  | .  +--+  Access-P1<1,10,20>    |         |    .          ...       .       |
 *  |  .                   .         |   PE1   @     .     (1982,1010)  .        |
 *  | .                     .        | OEBR    |\     .               .          |
 *  | .                    .         +---------+ \  .                   .        |             
 *  |  .                 .                .       \ .                    .       |
 *  |    .               .                  .     +-----------------------+  .   |      
 *  |     .   NATIVE   .                     .  <-|---PWE(2010,in_port)---|-   . |
 *  |       . .  . . .                     .     -|---PWE(1982,out_port)--|->  . |
 *  |                                      .      +-----------------------+ .    |
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
 *  -   PWE-1: network port with incoming VC = 2010, egress-VC set on ECMP with PWE lablel 1982 and tunnels 1000-1010 and 1500-1510.
 *  -   For access ports outer-Tag TPID is 0x8100 and inner Tag TPID is 0x9100.
 *  -   Access-P1 refer to the logical interface (attachment circuit).
 *  -   PWE1 refers to MPLS network ports.
 *  -   P1 refers to the physical ports PWE1 are attached to.
 *
 *	 Following figure shows HW model for Routing (and bridging) over PWE:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *|                                           Ingress-Egress                           |
 *|                                                  |                                 |
 *|                                  LB                                                |
 *|                                                  |                  EEDB           |
 *| +-------+ ECMP-    +------+PWE-MPLS-FEC+------+     +----+     +----------+        |
 *| |       |  PWE-MPLS|      |----------->|      |  |  |    |---->|PWE, MPLS |- |     |                              
 *| | MACT  | -------->| ECMP |----------->| FEC  |---->|GLEM|     |----------|  |     | 
 *| |       |          |      |----------->|      |  |  |    |  ---|- - - - - - -      | 
 *| +-------+          +------+            +------+     +----+  |  |          |        |
 *|                                                             |  |          |        | 
 *|                                                             |->|----------|        |
 *|                                                                |Link-Layer|        |  
 *|                                                                +----------+        |
 *|                                                                                    |        
 *-------------------------------------------------------------------------------------|
 *
 * Bridge Headers:
 *  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS         ||   PWE    ||Eth||Data | |
 *   |  |0:11|  ||0x8100|    |100||Label:1500-1510||Lable:2010||   ||     | | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+             |
 *   |                | Figure 15: Packets Received from PWE1 |             |  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS         ||   PWE    ||Eth||Data   | |
 *   |  |0:11|  ||0x8100|    |100||Label:1500-1510||Lable:1982||   ||       | | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+               |
 *   |                | Figure 15: Packets Transmitted to PWE1|               |  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * Access side packets:
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |                       tag1                 tag2                           |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   | DA | SA || TIPD1 | Prio | VID || TIPD2 | Prio | VID ||    Data    |   |
 *   |   |    |    || 0x8100|      | 10  || 0x9100|      | 20  ||            |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |        +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+       |
 *   |        | Figure 18: Packets Received/Transmitted on Access Port 1 |       |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * Routing out of overlay:
 * 
 * Send through port 203 IPv4oETHoMPLSoMPLSoETH out of core. Route into access with IPv4oETH on port 202.
 * 
 *   ----------->
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++-+-+-+-+-+-+-+-+-+-+-+-+-++-+--++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+
 *   |    |      DA      | SA           ||TPID1 |VID||   MPLS   ||MPLS (PWE)||      DA      | SA           ||  SIP       ||  DIP          ||  TTL ||
 *   |    |00:00:00:00:11|00:00:00:00:02||0x8100|100||Label:1002||Label:2010||00:00:00:00:11|00:00:03:03:03||171.17.17.17||160.161.161.161|| 128  ||
 *   |    |              |              ||      |   ||Exp:0     ||Exp:0     ||              |              ||            ||               ||      ||
 *   |    |              |              ||      |   ||TTL:64    ||TTL:64    ||              |              ||            ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++-+-+-+-+-+-+-+-+-+-+-+-+-+--+--++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *   <------------
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-  
 *   |    |      DA      | SA  ||TPID1 |VID||  SIP       ||  DIP          ||  TTL ||
 *   |    |00:00:00:00:88|00:44||0x8100|400||171.17.17.17||160.161.161.161|| 127  ||
 *   |    |              |     ||      |   ||            ||               ||      ||
 *   |    |              |     ||      |   ||            ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * Routing into overlay:
 * 
 * Send through port 202 IPv4oETH into core. Route into core with IPv4oETHoMPLSoMPLSoETH on port 203.
 * 
 *   ----------->
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-  
 *   |    |      DA      | SA  ||TPID1 |VID||  SIP       ||  DIP          ||  TTL ||
 *   |    |00:00:00:00:11|00:03||0x8100|100||171.17.17.18||160.161.161.162|| 128  ||
 *   |    |              |     ||      |   ||            ||               ||      ||
 *   |    |              |     ||      |   ||            ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *   <------------
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+--++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+
 *   |    |      DA      | SA           ||TPID1 |VID||   MPLS   ||MPLS (PWE)||TPID1 |VID|      DA      | SA           ||  SIP       ||  DIP          ||  TTL ||
 *   |    |00:00:00:00:55|00:00:00:00:11||0x8100|200||Label:1001||Label:1982||0x8100|700|00:00:00:00:99|00:00:00:00:66||171.17.17.18||160.161.161.162|| 127 ||
 *   |    |              |              ||      |   ||Exp:0     ||Exp:0     ||      |   |              |              ||            ||               ||      ||
 *   |    |              |              ||      |   ||TTL:10    ||TTL:10    ||      |   |              |              ||            ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+++-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+--++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Calling sequence:
 *  -   Port TPIDs setting:
 *      -   For P1 set outer TPID to 0x8100 and no inner TPID (using cint_port_tpid.c).
 *  -    Create LL egress object interface (on egress). 
 *      -    Packet routed to this egress-object is forwarded to P1 tunneled with the L3-interface VID and MAC.
 *      -    Calls bcm_l2_egress_create() (for routing over PWE)
 *           Call bcm_l3_egress_create() (for bridging over PWE)
 *  -   Add AC and MPLS-ports to the VSI.
 *      -   vswitch_vpls_add_access_port_1 creates attachment circuit and add them to the Vswitch and 
 *          update the Multicast group.
 *      -   vswitch_vpls_add_network_port_1 creates PWE and add them to the VSI and update the 
 *          multicast group.
 *          - PWE is added in the calls: INGRESS_ONLY configures the ingress object (temination+learning)
 *                                       EGRESS_ONLY configures the egress onject (encapsulation)
 *                                       FORWARDING_GROUP configured the FEC
 *                                       mpls_port_id must be the same for ingress and egress so that split horizon filter will work.
 *          - PWE egress interface is egress object created above.
 *          - After creating several forwarding groups, connect them into one ecmp object.
 *          -    Packet routed to above interface is tunneled into PWE ECMP, where tunnels are set according to 
 *               load balancing to the values 1000-1010 and 1500-1510. (these will be created later)
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
 *      -   Create MPLS tunnels pointed by the MPLS-ports.
 *          -    Tunnel inteface of the MPLS label is created on the same entry as the mpls_port.
 *          -    Calls bcm_mpls_tunnel_initiator_get() and then bcm_mpls_tunnel_initiator_create().  
 *  -   Create multipoint VSI #6202 (refer to mpls_port__vswitch_vpls_vpn_create__set).
 *      -   calling bcm_mpls_vpn_id_create().
 *          -    You has to supply VPN ID and multicast groups. For this purpose, the following flags have 
 *               to be present BCM_MPLS_VPN_VPLS|BCM_MPLS_VPN_WITH_ID.
 *          Note that uc/mc/bc group have the same value as VSI. Another option is to set uc_group = VSI, 
 *          and mc = uc + 4k, and bc = mc + 4k.
 *      -   Use open_ingress_mc_group() to open multicast group for flooding.
 *          -    Multicast group ID is equal to VPN ID.
 *  -   Create Native Routing Interface (for routing over PWE only)
 *      -   create_l3_intf create native routing interface. 
 *      -   create_l3_egress create fec for native routing. (hierarchical fec: point to MPLS tunnel FEC): 
 *          Build native ethernet link layer with vlan 700
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
 *  Sending IPv4oETHoMPLSoMPLSoETH, route out of overlay with IPv4oETH:
 *  Send packet through port 203. PWE (2010) + MPLS (1002) labels will be processed, packet will route with
 *  SIP == 171.17.17.17, DIP == 160.161.161.161.
 * 
 *  Sending IPv4oETH, route into overlay with IPv4oETHoMPLSoMPLSoETH:
 *  Send packet through port 202 with SIP == 171.17.17.18, DIP == 160.161.161.162. DIP will be processed, the result of which
 *  is an encapsulation of PWE (1982) + MPLS (1001).
 * 
 * Remarks:
 *  -   For this application at least two ports are needed, one for access side and one for MPLS network side
 *  -   User can statically add entry to the MACT pointing to a gport using 
 *      vswitch_add_l2_addr_to_gport(unit, gport_id)
 *  -   Set verbose = 1/0 to show/hide informative prints
 *  -   Calling vswitch_vpls_roo_run() as is more than one time would fail because this call will try open same 
 *      VPN twice,  to avoid this user should set vswitch_vpls_info_1.vpn_id different value in order to 
 *      open new service.
 *
 * To Activate Above Settings Run:
 *  	BCM> cint examples/sand/utility/cint_sand_utils_global.c
 *  	BCM> cint examples/dpp/utility/cint_utils_vlan.c
 *  	BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
 * 		BCM> cint examples/dpp/utility/cint_utils_mpls_port.c
 *  	BCM> cint examples/dpp/utility/cint_utils_l3.c
 *      BCM> cint examples/dpp/utility/cint_utils_multicast.c
 *      BCM> cint examples/dpp/utility/cint_utils_roo.c
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_advanced_vlan_translation_mode.c
 *      BCM> cint examples/dpp/cint_qos.c
 *      BCM> cint examples/dpp/cint_mpls_lsr.c
 *      BCM> cint examples/dpp/cint_vswitch_metro_mp.c
 *      BCM> cint examples/dpp/cint_vswitch_vpls.c
 *      BCM> cint examples/dpp/cint_vswitch_vpls_roo.c
 *      BCM> cint examples/dpp/cint_system_vswitch_encoding.c
 *      BCM> cint
 *      cint> int rv;
 *      cint> rv = vswitch_vpls_roo_run_with_defaults(unit); 
 * 
 * Script adjustment:
 *  User can adjust the following attribute of the application
 *  -   vpn_id:             VPN id to create 
 *  -   ac_in_port:         accesss port: physical port connect to customer 
 *  -   pwe_in_port:        network port: physical port connect to MPLs network (for incoming packets)
 *  -   pwe_out_port:       network port: physical port connect to MPLs network (for outgoing packets) 
 *  -   in_vc_label:        incomming VC label 
 *  -   eg_vc_label:        egress VC label 
 *  -   in_tunnel_label:       inner tunnel label 
 *  -   ecmp_size:          number of members in ECMP
 *  -   out_to_core_vlan:   vlan on the PWE outgoing packets
 *  -   enable_routing_over_pwe: enable routing over pwe. If disabled, bridging over pwe. To enable from Jericho devices. 
 */



struct cint_vswitch_vpls_roo_info_s {
    int ecmp_size;
    /* tunnels info */
    int in_tunnel_label[10];
    bcm_if_t pwe_encap_id[10];

    int core_to_access_dip; /* dip of core to access routing out of overlay (over vpls) traffic */
    int access_to_core_dip; /* dip of access to core routing into overlay (over vpls) traffic */
	
    /* After ecmp_hashing_main is run successfully, this will contain the ECMP object that was created. */
    /* Only works with the bcm_l3_egress_ecmp_* interfaces.*/
    bcm_l3_egress_ecmp_t ecmp;
    /* After ecmp_hashing_main is run successfully, this will contain the FEC objects that were created. */
    bcm_if_t fec_intfs[10]; /* FECs */	
    bcm_if_t ll_intfs[10]; /* LLs, can be different per PWE outlif. In this example using only one */	
    int ll_intf_id; /* LL interface id */

    int out_to_core_vlan; /* vlan in Core direction.  for vpls LL */
    int first_pwe_outlif; /* outlifs are configured with id in this mode. */

    uint8 enable_routing_over_pwe; /* enable routing over pwe. If disabled, bridging over pwe */

    int native_out_to_core_vlan; /* vlan in Core direction. For native LL */
    int native_core_to_access_vlan; /* vlan in access direction. For native LL */

    uint8 mac[6]; /* next hop for vlpls LL */
    uint8 native_mac[6]; /* next hop for native LL (routing into overlay traffic)*/
    uint8 access_dst_mac[6]; /* next hop for native LL (routing out of overlay traffic) */
    uint8 access_src_mac[6]; /* sa for native LL (routing out of overlay traffic) */
    uint8 core_src_mac[6];   /* sa for native LL (routing into overlay traffic) */

    bcm_mpls_label_t out_tunnel_label_1; /* To be used for mpls stack in qax and above */
    bcm_mpls_label_t out_tunnel_label_2; /* To be used for mpls stack in qax and above */
};

cint_vswitch_vpls_roo_info_s vswitch_vpls_roo_info;

/* In QAX, build an extended PWEoMPLSoMPLSoMPLS entry for vpls roo */
int is_vswitch_vpls_roo_validate_extended_stack = 0;

static int eg_qos_id;

static int roo_dscp_to_exp_map_enable=0;
static int roo_qos_map_id;

static int extend_label=0;

void
vswitch_vpls_roo_info_init(int unit, int ac_port, int pwe_in_port, int pwe_out_port, int ac_port1_outer_vlan, int ac_port1_inner_vlan,
                       int ac_port2_outer_vlan, int ac_port2_inner_vlan, int vpn_id, uint8 enable_routing_over_pwe) {
    int rv; 
    int i;

    vswitch_vpls_roo_info.enable_routing_over_pwe = enable_routing_over_pwe;  

    if (!is_device_or_above(unit,JERICHO) && vswitch_vpls_roo_info.enable_routing_over_pwe) {
        printf("Error, routing over pwe is supported only from Jericho \n");
        return rv;
    }

    vswitch_vpls_roo_info.ecmp_size = 10;
    vswitch_vpls_roo_info.out_to_core_vlan = 200;

    if (is_device_or_above(unit,JERICHO)) {
        vswitch_vpls_roo_info.first_pwe_outlif = 0x8000;  
    } else {
        vswitch_vpls_roo_info.first_pwe_outlif = 0x5000;    
    }

    vswitch_vpls_roo_info.native_out_to_core_vlan = 700; 
    vswitch_vpls_roo_info.native_core_to_access_vlan = 400;

    vswitch_vpls_roo_info.core_to_access_dip = 0xA0A1A1A1;
    vswitch_vpls_roo_info.access_to_core_dip = 0xA0A1A1A2;

    for (i=0; i<6; i++) {
        vswitch_vpls_roo_info.mac[i] = 0;
        vswitch_vpls_roo_info.native_mac[i] = 0;
        vswitch_vpls_roo_info.access_dst_mac[i] = 0;
        vswitch_vpls_roo_info.access_src_mac[i] = 0;
        vswitch_vpls_roo_info.core_src_mac[i] =0;
    }
    vswitch_vpls_roo_info.mac[5] = 0x55;
    vswitch_vpls_roo_info.native_mac[5] = 0x99;
    vswitch_vpls_roo_info.access_dst_mac[5] = 0x88;
    vswitch_vpls_roo_info.access_src_mac[5] = 0x44;
    vswitch_vpls_roo_info.core_src_mac[5] = 0x66;

    /* VPN id to create */
    if (vpn_id < 0) {
        vswitch_vpls_info_1.vpn_id = default_vpn_id;
    } else {
        vswitch_vpls_info_1.vpn_id = vpn_id; 
        g_mpls_port_utils.vpn = vpn_id; 
    }

    /* incomming packet attributes */
    vswitch_vpls_info_1.ac_in_port = ac_port;
    vswitch_vpls_info_1.pwe_in_port = pwe_in_port;
    vswitch_vpls_info_1.pwe_out_port = pwe_out_port;
    vswitch_vpls_info_1.in_vc_label = 2010;
    vswitch_vpls_info_1.eg_vc_label = 1982;

    /* tunnel info */
    for (i=0; i<vswitch_vpls_roo_info.ecmp_size; i++) {
        vswitch_vpls_roo_info.in_tunnel_label[i] = 1000+i;
    }
    
    vswitch_vpls_info_1.ac_port1_outer_vlan = ac_port1_outer_vlan;
    vswitch_vpls_info_1.ac_port1_inner_vlan = ac_port1_inner_vlan;
    vswitch_vpls_info_1.ac_port2_outer_vlan = ac_port2_outer_vlan;
    vswitch_vpls_info_1.ac_port2_inner_vlan = ac_port2_inner_vlan;
    if (verbose1) {
        printf("vswitch_vpls_roo_info_init %d\n", vswitch_vpls_info_1.vpn_id);
    }

    vswitch_vpls_info_1.access_index_1 = 2;
    vswitch_vpls_info_1.access_index_2 = 3;
    vswitch_vpls_info_1.access_index_3 = 3;

    vswitch_vpls_roo_info.out_tunnel_label_1 = 6000;
    vswitch_vpls_roo_info.out_tunnel_label_2 = 7000;

    /* set mpls tunneling information with default values */
    mpls_lsr_init(vswitch_vpls_info_1.pwe_in_port, vswitch_vpls_info_1.pwe_out_port, 0x11, 0x22, 0/*in_label*/, 0/*out_label*/, 100, 200, 0);
    mpls_lsr_info_1.eg_port = vswitch_vpls_info_1.pwe_out_port;

}

/* This function receives PWE pointer and MPLS tunnel strucutre and add them to the same EEDB entry
   In QAX, the PWE binded with MPLS entry will point to another MPLS binded with MPLS entry */
int
vswitch_vpls_roo_fill_egress_encap_pwe_entry_with_additional_mpls_tunnel(int unit, int pwe_encap_id, bcm_mpls_egress_label_t * tunnel_label) 
{

    int rv;
    bcm_mpls_egress_label_t label_array[2];
    int label_count;
    bcm_if_t next_pointer = 0;
    int tunnel_id;

    /* For each PWE EEDB entry, add MPLS label in the same entry */
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    rv = bcm_mpls_tunnel_initiator_get(unit, pwe_encap_id, 1, &label_array, &label_count);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_get\n");
        return rv;
    }

    /* In QAX and above we have the ability to add an additional mpls entry on the stack
       as opposed to lesser devices in which we could place only one entry (PWE binded with MPLS)
       In QAX, this stack will be comprosed of PWE binded with MPLS + MPLS binded with MPLS */
    if (is_device_or_above(unit, JERICHO_PLUS) && is_vswitch_vpls_roo_validate_extended_stack) {
        /* Build an EEDB entry with two tunnels (labels equal to 0)*/
        mpls__egress_tunnel_utils_s mpls_tunnel_properties;
        int tunnel_index = 0;

        mpls_tunnel_properties.label_in = vswitch_vpls_roo_info.out_tunnel_label_1;
        mpls_tunnel_properties.label_out = vswitch_vpls_roo_info.out_tunnel_label_2;
        mpls_tunnel_properties.egress_action = BCM_MPLS_EGRESS_ACTION_PUSH;
        /* This entry will be pointed by the PWE binded with MPLS entry, so it needs to point
           to the LL which was formerly pointed by the PWE entry that was fetched above */
        mpls_tunnel_properties.next_pointer_intf = label_array[0].l3_intf_id;

        /* Create egress tunnel */
        rv  = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties); /* 1 */
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in mpls__create_tunnel_initiator__set\n");
            return rv;
        }

        /* Connect the PWE binded with MPLS entry to this entry */
        next_pointer = mpls_tunnel_properties.tunnel_id;
    } else { /* JERICHO */
        /* Connect the PWE binded with MPLS entry to the LL which was formerly pointed by the PWE entry that was fetched above*/
        next_pointer = label_array[0].l3_intf_id;
    }


    label_count = 2;
    /* PWE label - leaving as is */
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
    /* Add MPLS label in the second place in the same EEDB entry */
    label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
    label_array[1].flags |= tunnel_label->flags;
    label_array[1].tunnel_id = tunnel_label->tunnel_id;
    /* In QAX, point to additional MPLS binded with MPLS entry
       In Jericho, point to the LL which was formerly pointed by the PWE entry that was fetched above
    */
    label_array[1].l3_intf_id = next_pointer;
    label_array[1].label = tunnel_label->label;
    label_array[1].ttl = tunnel_label->ttl;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, label_count, label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }



    if (verbose1) {
        printf("Labels: %d, %d, Tunnel id: %d\n", label_array[0].label, label_array[1].label, label_array[0].tunnel_id);
    }

    /* update entry with the LL pointer */
    /* Routing Over PWE */
    if (vswitch_vpls_roo_info.enable_routing_over_pwe) {
        /* In ROO, the overlay LL encapsulation is built with a different API call 
           (bcm_l2_egress_create instead of bcm_l3_egress create) */
        bcm_l2_egress_t l2_egress_overlay; 
        bcm_l2_egress_t_init(&l2_egress_overlay);
        l2_egress_overlay.flags = BCM_L2_EGRESS_REPLACE | BCM_L2_EGRESS_WITH_ID; 

        l2_egress_overlay.dest_mac   = vswitch_vpls_roo_info.mac;  /* next hop. default: {0x00, 0x00, 0x00, 0x00, 0x00, 0x55} */
        l2_egress_overlay.src_mac    = mpls_lsr_info_1.my_mac;  /* my-mac */
        l2_egress_overlay.outer_vlan = vswitch_vpls_roo_info.out_to_core_vlan; /* PCP DEI (0) + outer_vlan (200=0xC8)  */
        l2_egress_overlay.ethertype  = 0x8847;       /* ethertype for MPLS */
        l2_egress_overlay.outer_tpid = 0x8100;      /* outer tpid */
        l2_egress_overlay.l3_intf  = label_array[0].tunnel_id; /* tunnel interface */
        l2_egress_overlay.encap_id =  vswitch_vpls_roo_info.ll_intfs[0];
        if (extend_label) {
            l2_egress_overlay.mpls_extended_label_value = 1234;
            l2_egress_overlay.outer_vlan = 0;
            l2_egress_overlay.flags = BCM_L2_EGRESS_REPLACE | BCM_L2_EGRESS_ETHERTYPE_REPLACE |BCM_L2_EGRESS_WITH_ID; 
        }

        bcm_if_t encap_id; 
        rv = roo__overlay_link_layer_create(unit, &l2_egress_overlay,
                                            (BCM_L3_KEEP_DSTMAC|BCM_L3_KEEP_VLAN), 
                                            &encap_id,
                                            0);
        if (rv != BCM_E_NONE) {
            printf("Error, roo__overlay_link_layer_create \n");
            return rv; 
        }

        if (extend_label) {
            printf("Linking PWE(%X) to LL-Extended Encap(%X)\n", label_array[0].tunnel_id, vswitch_vpls_roo_info.ll_intfs[0]);
                    
            BCM_GPORT_TUNNEL_ID_SET(tunnel_id, (tunnel_label->tunnel_id & 0xFFFFF));
            rv = bcm_port_control_set(unit, tunnel_id, bcmPortControlMplsEncapsulateExtendedLabel,1);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_control_set (%x)\n", tunnel_id);
                return rv;
            }
            printf("\n set the pwe wide entry properties (%x)", tunnel_id); 

        }
        
        if (roo_dscp_to_exp_map_enable) {
             bcm_gport_t gport;
             BCM_GPORT_TUNNEL_ID_SET(gport,BCM_L3_ITF_VAL_GET(vswitch_vpls_roo_info.ll_intfs[0]));
             rv = bcm_qos_port_map_set(unit,gport,-1,roo_qos_map_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in roo__overlay_link_layer qos map failed %d\n",rv);
                return rv;
             }
	    if (verbose1) {
	        printf("roo dscp to exp mapping completed,roo_qos_map_id =  0x%x\n", roo_qos_map_id);
            }
        }


    } 
    /* bridging Over PWE */
    else {
        create_l3_egress_s l3eg;
        l3eg.allocation_flags = BCM_L3_WITH_ID|BCM_L3_KEEP_DSTMAC|BCM_L3_KEEP_VLAN|BCM_L3_REPLACE;
        sal_memcpy(l3eg.next_hop_mac_addr, vswitch_vpls_roo_info.mac , 6);
        l3eg.out_tunnel_or_rif = tunnel_label->tunnel_id;
        l3eg.out_gport = vswitch_vpls_info_1.pwe_out_port;
        l3eg.vlan = vswitch_vpls_roo_info.out_to_core_vlan;
        l3eg.arp_encap_id = vswitch_vpls_roo_info.ll_intfs[0];

        rv = l3__egress_only_encap__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_encap__create\n");
            return rv;
        }

        vswitch_vpls_roo_info.ll_intfs[0] = l3eg.arp_encap_id;                                                                                                                                                                                              
    }

    return rv;
}

/* 
 * This function configures MPLS tunnels on already existing PWE encapsulation DB entries.
 */
int
mpls_tunnels_config(int unit){

    int ingress_intf;
    bcm_mpls_egress_label_t mpls_tunnel_label;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    int rv;
    bcm_mpls_label_t label_for_pop_entry_in;
    int i;
    
    /* open vlan */
    if (verbose1) {
        printf("open vlan %d\n", mpls_lsr_info_1.in_vid);
    }
    rv = bcm_vlan_create(unit, mpls_lsr_info_1.in_vid);
    if (rv < 0) {
        printf("Error in bcm_vlan_create.\n");
        print rv;
        return rv;
    }

    /* add vlan to pwe_port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.eg_port);
    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_port_add(unit, mpls_lsr_info_1.in_vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
        return rv;
    }
    
    /* l2 termination for mpls routing: packet received on those VID MAC will be L2 terminated  */
    create_l3_intf_s intf;
    intf.vsi = mpls_lsr_info_1.in_vid;
    intf.my_global_mac = mpls_lsr_info_1.my_mac;
    intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }
    
    /*  create ingress object, packet will be routed to */
    intf.vsi = mpls_lsr_info_1.eg_vid;
    
    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }

    for (i = 0; i < vswitch_vpls_roo_info.ecmp_size; i++) {

        bcm_mpls_egress_label_t_init(&mpls_tunnel_label);

        if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
            mpls_tunnel_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
            mpls_tunnel_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        } else {
            mpls_tunnel_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            mpls_tunnel_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_COPY;
        }
        mpls_tunnel_label.tunnel_id = vswitch_vpls_roo_info.pwe_encap_id[i];
        mpls_tunnel_label.l3_intf_id = ingress_intf; /* here should be the overlay */
        mpls_tunnel_label.label = vswitch_vpls_roo_info.in_tunnel_label[i];
        mpls_tunnel_label.ttl = 40;

        rv = vswitch_vpls_roo_fill_egress_encap_pwe_entry_with_additional_mpls_tunnel(unit, vswitch_vpls_roo_info.pwe_encap_id[i], &mpls_tunnel_label);
        if (rv != BCM_E_NONE) {
        	printf("Error, in vswitch_vpls_roo_fill_egress_encap_pwe_entry_with_additional_mpls_tunnel\n");
        	return rv;
        }
    }
    
    for (i = 0; i < vswitch_vpls_roo_info.ecmp_size; i++) {
        /* add switch entries to pop MPLS labels  */
        if (mpls_termination_label_index_enable) {
            BCM_MPLS_INDEXED_LABEL_SET(&label_for_pop_entry_in, vswitch_vpls_roo_info.in_tunnel_label[i], 1);
        }
        else {
            label_for_pop_entry_in = vswitch_vpls_roo_info.in_tunnel_label[i];
        }
        
        rv = mpls_add_pop_entry(unit,label_for_pop_entry_in);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_add_pop_entry\n");
            return rv;
        }
    }
   
    return rv;
}

/* 
 * Configure mpls port INGRESS attributes: InLif and learning information
 */

int
vswitch_vpls_ingress_mpls_port_create(int unit){

    int rv;
    mpls_port__ingress_only_info_s ingress_port;

    /* 
     *  Ingress_only
     */
    ingress_port.flags = BCM_MPLS_PORT_NETWORK;
    

    /* Set parameters for ingress mpls port */
    ingress_port.ingress_matching_criteria = BCM_MPLS_PORT_MATCH_LABEL;
    if (mpls_termination_label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(ingress_port.ingress_pwe_label, vswitch_vpls_info_1.in_vc_label, vswitch_vpls_info_1.access_index_2);
    }
    else {
        ingress_port.ingress_pwe_label = vswitch_vpls_info_1.in_vc_label;
    }

    ingress_port.port = BCM_GPORT_INVALID; /* Port is not required for PWE-InLIF */
    ingress_port.learn_egress_if = 0; /* PWE In-LIF for the case of FORWARD_GROUP do not care about MPLS egress_if. It is not part of learning information. */
    /* Converting PWE-MPLS-ECMP-IF to forwarding group mpls-port gport encoding */
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(ingress_port.failover_port_id, vswitch_vpls_roo_info.ecmp.ecmp_intf); /* Learning - PWE ECMP (forwarding froup) */


    /* Only one inlif is configured. It should be equal to one of the Outlifs to get same-interface filtering (Multicast group member) */
    int mpls_port_id_temp;
    BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_id_temp, 0,  vswitch_vpls_roo_info.first_pwe_outlif) ;
    BCM_GPORT_MPLS_PORT_ID_SET(ingress_port.mpls_port_id, mpls_port_id_temp); 
    ingress_port.encap_id = vswitch_vpls_roo_info.first_pwe_outlif;
    ingress_port.vpn = vswitch_vpls_shared_info_1.vpn;
    rv = mpls_port__ingress_only_create(unit, &ingress_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }

    if (verbose1) {
        printf("INGRESS: add port 0x%08x to vpn \n\r", ingress_port.mpls_port_id);
        printf("learning info is : 0x%08x, \n", ingress_port.failover_port_id);
    }

    /* handle of the created gport */
    network_port_id = ingress_port.mpls_port_id;

        
    return rv;   
}

/* 
 * Configure mpls port EGRESS attributes: OutLif
 */
int
vswitch_vpls_egress_mpls_port_create(int unit){
    int rv;
    int modid;
    int modport;
    bcm_gport_t egress_mpls_port_id;
    int i;
    int tm_port;
    mpls_port__egress_only_info_s egress_port;
    
    /* 
     *  Egress only
     */
    egress_port.flags = BCM_MPLS_PORT_NETWORK;

    /* Set parameters for egress mpls port */
    if (pwe_cw) {
        egress_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    /* egress label parameters for pwe encapsulation configuration */
    egress_port.egress_label.label = vswitch_vpls_info_1.eg_vc_label;
    egress_port.egress_label.ttl = 10;
    egress_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;

    for (i = 0; i < vswitch_vpls_roo_info.ecmp_size; i++) {
        /* encap id is the outlif */
        egress_port.encap_id = vswitch_vpls_roo_info.first_pwe_outlif + 2*i;

        /* mpls_port_id is the outlif id encapsulated as MPLS_PORT */
        int mpls_port_id_temp;
        BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_id_temp, 0,  egress_port.encap_id) ;
        BCM_GPORT_MPLS_PORT_ID_SET(egress_port.mpls_port_id, mpls_port_id_temp);
      
        egress_port.egress_tunnel_if = vswitch_vpls_roo_info.ll_intfs[0]/*For ROO this should be the LL*/;

        if (verbose1) {
            printf("ADDING MPLS PORT EGRESS: mpls_port_id: %d, encap_id: %d\n", egress_port.mpls_port_id, egress_port.encap_id);
        }

        rv = mpls_port__egress_only_create(unit, &egress_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add\n");
            return rv;
        }
        if (verbose1) {
            printf("EGRESS: add port 0x%08x to vpn \n\r", egress_port.mpls_port_id);
        }

        if (i==0) {
            egress_mpls_port_id = egress_port.mpls_port_id; /* save this port for multicast */
        }

        BCM_L3_ITF_SET(vswitch_vpls_roo_info.pwe_encap_id[i], BCM_L3_ITF_TYPE_LIF, egress_port.encap_id);
    }
    

    /* update Multicast table */
    if (is_device_or_above(unit,JERICHO)) {
        rv = get_core_and_tm_port_from_port(unit, vswitch_vpls_info_1.pwe_in_port, &modid, &tm_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
        }
        BCM_GPORT_MODPORT_SET(modport, modid, tm_port);
    }
    else {
        rv = bcm_stk_modid_get(unit, &modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_get\n");
            return rv;
        }
        BCM_GPORT_MODPORT_SET(modport, modid, vswitch_vpls_info_1.pwe_in_port);
    }
    /* Adding egress mpls port to multicast because when egress and ingress are configured separately the multicast should point to the outlif */

    rv = multicast__mpls_port_add(unit, vswitch_vpls_shared_info_1.vpn, modport, egress_mpls_port_id, 0/*is_egress*/);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__mpls_port_add\n");
        return rv;
    }
    if(verbose1){
        printf("add port   0x%08x to multicast \n\r",egress_mpls_port_id);
    }     
    
    return rv;
}

/* 
 * Configure mpls port FORWARD GROUP attributes: FEC
 */
int
vswitch_vpls_forward_group_mpls_port_create(int unit){
    int rv;
    mpls_port__forward_group__info_s forward_group_info;
    int i;
    
     /* 
     *  Forwarding groups (FEC) 
     */
    
    /* Routing Over PWE */
    if (vswitch_vpls_roo_info.enable_routing_over_pwe) {
        forward_group_info.flags2 = BCM_MPLS_PORT2_CASCADED;
    }

    forward_group_info.port = vswitch_vpls_info_1.pwe_in_port;

    for (i = 0; i < vswitch_vpls_roo_info.ecmp_size; i++) {
        forward_group_info.encap_id = vswitch_vpls_roo_info.first_pwe_outlif + 2*i; /* this is the outlif, configured by mpls_port with EGRESS_ONLY flag set */

        rv = mpls_port__forward_group__create(unit, &forward_group_info);
        if (rv != BCM_E_NONE) {
            printf("mpls_port__forward_group__create failed: %d \n", rv);
            return rv;
        }

        BCM_L3_ITF_SET(vswitch_vpls_roo_info.fec_intfs[i], BCM_L3_ITF_TYPE_FEC, BCM_GPORT_SUB_TYPE_FORWARD_GROUP_GET(forward_group_info.mpls_port_forward_group_id)); 
        if (verbose1) {
            printf("vswitch_vpls_roo_info.fec_intfs[%d] 0x%08x \n\r", i, vswitch_vpls_roo_info.fec_intfs[i]);
        }
    }
        
    return rv;    
}



/*
 * Create mpls-port: created using separate ingress, egress and forwarding group calls 
 * Also creates ECMP group. 
 */
int
vswitch_vpls_add_network_port_1(int unit){
    int rv;

    rv = vswitch_vpls_egress_mpls_port_create(unit);
    if (rv != BCM_E_NONE) {
        printf("vswitch_vpls_egress_mpls_port_create failed: %d \n", rv);
        return rv;
    }

    rv = vswitch_vpls_forward_group_mpls_port_create(unit);
    if (rv != BCM_E_NONE) {
        printf("vswitch_vpls_forward_group_mpls_port_create failed: %d \n", rv);
        return rv;
    }

    /* Create ECMP. The members are mpls port forwarding groups */
    bcm_l3_egress_ecmp_t_init(&vswitch_vpls_roo_info.ecmp);
    vswitch_vpls_roo_info.ecmp.max_paths = vswitch_vpls_roo_info.ecmp_size;

    /* create an ECMP, containing the FEC entries */
    rv = bcm_l3_egress_ecmp_create(unit, &vswitch_vpls_roo_info.ecmp, vswitch_vpls_roo_info.ecmp_size, vswitch_vpls_roo_info.fec_intfs);
    if (rv != BCM_E_NONE) {
        printf("bcm_l3_egress_ecmp_create failed: %d \n", rv);
        return rv;
    }

    rv = vswitch_vpls_ingress_mpls_port_create(unit);
    if (rv != BCM_E_NONE) {
        printf("vswitch_vpls_ingress_mpls_port_create failed: %d \n", rv);
        return rv;
    }
        
    return rv;
}

int
qos_map_l3_egress_dscp_profile(int unit)
{
    bcm_qos_map_t l3_eg_map;
    int dscp, dp;
    int rv;

    /* Create QOS (Remark) profile ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &eg_qos_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_EGRESS\n");
        return rv;
    }
    printf("created Remark-profile-id =0x%08x, \n", eg_qos_id);

    /* set QoS mapping for L3 in egress:
       map Out-DSCP-EXP (TOS) = In-DSCP-EXP + DP */
    for (dscp = 0; dscp < 64; dscp++) {
        for (dp = 0; dp < 2; dp++) { 
            bcm_qos_map_t_init(&l3_eg_map);
            l3_eg_map.color = dp; /* Set internal color (DP) */ 
            l3_eg_map.int_pri = dscp; /* in-DSCP-EXP */
            l3_eg_map.remark_int_pri = (dscp + dp) & 0xff; /* TOS */

            rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3, l3_eg_map, eg_qos_id);
            if (rv != BCM_E_NONE) {
                printf("Error, set egress QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
                return rv;
            }       
        }       
    }

    printf("Set egress QoS mapping for L3\n"); 
    return rv;
}

/* 
 * This function ctrates VPLS ROO configuration. 
 * In contrary to the known VPLS sequence, here we first create LL, then mpls_port and only afterwards the MPLS tunnels.
 */
int
vswitch_vpls_roo_run(int unit){

    int rv;
    int native_ingress_intf; /* l3_intf for native routing. */
    bcm_failover_t failover_id_fec;
    create_l3_egress_s l3_egress_into_overlay, l3_egress_out_of_overlay;
    l3_ipv4_route_entry_utils_s route_entry;
    bcm_l3_host_t l3_host_simple_routing; /* LEM entry for simple out of overlay routing */
    create_l3_intf_s access_intf, core_intf;

    /* init values */
  
    /* set ports to identify double tags packets */

    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    if (pwe_cw) {
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPWControlWord, pwe_cw);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set\n");
            return rv;
        }
    }
  
    if (verbose1) {
        printf("vswitch_vpls_roo_run %d\n", vswitch_vpls_info_1.vpn_id);
    }

    port_tpid_init(vswitch_vpls_info_1.pwe_in_port,1,0);
    rv = port_tpid_set(unit);
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

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
        are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
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

    if (verbose1) {
        printf("vswitch_vpls_roo_run create vswitch\n");
    }
    /* create vswitch */
    rv = mpls_port__vswitch_vpls_vpn_create__set(unit, vswitch_vpls_info_1.vpn_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_port__vswitch_vpls_vpn_create__set\n");
        return rv;
    }

    /* create LL for VPLS tunnel */
    /* Routing Over PWE */
    
    if (vswitch_vpls_roo_info.enable_routing_over_pwe) {
        /* In ROO, the overlay LL encapsulation is built with a different API call 
           (bcm_l2_egress_create instead of bcm_l3_egress create) */
        bcm_l2_egress_t l2_egress_overlay; 
        bcm_l2_egress_t_init(&l2_egress_overlay);

        l2_egress_overlay.dest_mac   = vswitch_vpls_roo_info.mac;  /* next hop. default: {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d} */
        l2_egress_overlay.src_mac    = mpls_lsr_info_1.my_mac;  /* my-mac */
        l2_egress_overlay.outer_vlan = vswitch_vpls_roo_info.out_to_core_vlan; /* PCP DEI (0) + outer_vlan (200=0xC8)  */
        l2_egress_overlay.ethertype  = 0x8847;       /* ethertype for MPLS */
        l2_egress_overlay.outer_tpid = 0x8100;      /* outer tpid */
        if (extend_label) {
            l2_egress_overlay.mpls_extended_label_value = 1234;
            l2_egress_overlay.outer_vlan = 0;
        }

        rv = roo__overlay_link_layer_create(unit, 
                                            &l2_egress_overlay, 
                                            0, /* l3 flags */ 
                                            &(vswitch_vpls_roo_info.ll_intfs[0]),
                                            0); 
        if (rv != BCM_E_NONE) {
            printf("Error, in roo__overlay_link_layer_create\n");
            return rv;
        }

        if (roo_dscp_to_exp_map_enable) {
             bcm_gport_t gport;
             BCM_GPORT_TUNNEL_ID_SET(gport,BCM_L3_ITF_VAL_GET(vswitch_vpls_roo_info.ll_intfs[0]));
             rv = bcm_qos_port_map_set(unit,gport,-1,roo_qos_map_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in roo__overlay_link_layer qos map failed %d\n",rv);
                return rv;
             }
	    if (verbose1) {
	        printf("roo dscp to exp mapping completed,roo_qos_map_id =  0x%x\n", roo_qos_map_id);
            }
        }
    } 
    /* switching over PWE */
    else {
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, vswitch_vpls_roo_info.mac , 6);
        l3eg.out_tunnel_or_rif = 0;
        l3eg.out_gport = vswitch_vpls_info_1.pwe_out_port;
        l3eg.vlan = vswitch_vpls_roo_info.out_to_core_vlan;
        l3eg.arp_encap_id = vswitch_vpls_roo_info.ll_intfs[0];

        rv = l3__egress_only_encap__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_encap__create\n");
            return rv;
        }

        vswitch_vpls_roo_info.ll_intfs[0] = l3eg.arp_encap_id;
    }

    if (verbose1) {
        printf("Configured LL: %d\n", vswitch_vpls_roo_info.ll_intfs[0]);
    }

    /* Routing Over PWE */
    if (vswitch_vpls_roo_info.enable_routing_over_pwe) {
        /* Create native router interface */
	    create_l3_intf_s intf;
	    intf.vsi = vswitch_vpls_info_1.vpn_id;
	    intf.my_global_mac = mpls_lsr_info_1.my_mac;
	    intf.my_lsb_mac = mpls_lsr_info_1.my_mac;

            if (eg_qos_id != 0) {
                intf.qos_map_id = eg_qos_id;
                intf.qos_map_valid = 1;
            }
 
	    rv = l3__intf_rif__create(unit, &intf);
	    native_ingress_intf = intf.rif;
	    if (rv != BCM_E_NONE) {
	    	printf("Error, in l3__intf_rif__create\n");
	    	return rv;
	    }
    }

    bcm_if_t native_intf; 
    int native_ll_intf_id;

    /* For ROO first the MPLS PORT should be configured and only then the MPLS TUNNEL */
    if (verbose1) {
        printf("vswitch_vpls_roo_run add vlan access port\n");
    }
    /* add mpls access port */
    rv = vswitch_vpls_add_access_port_1(unit, -1/*second_unit*/, &vswitch_vpls_info_1.access_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_add_access_port_1\n");
        return rv;
    }

    if (verbose1) {
        printf("vswitch_vpls_roo_run add mpls network port\n");
    }
    /* add mpls network port */
    rv = vswitch_vpls_add_network_port_1(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_add_network_port_1\n");
        return rv;
    }

    if (verbose1) {
        printf("vswitch_vpls_roo_run add mpls tunnel\n");
    }

    /* configure MPLS tunnels */
    rv  = mpls_tunnels_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_tunnels_config\n");
        return rv;
    }

    /*
       1) Routing into overlay
       2) Routing out of overlay
    */
    if (vswitch_vpls_roo_info.enable_routing_over_pwe) {

        /* 1: Routing into overlay */
        /* From Jericho, support hierachical fec:
         * create additional fec for native routing. 
         * create fec which point to cascaded fec:
         * - create outLif using next hop 
         * - FEC contains another FEC: to build overlay  
         * -              outLif and outRif: to build native LL.
         */
        /* Hierarchical FEC does not work when the 1st FEC hierarchy is unprotected.
        * In case we are using hierarchical FEC, the first FEC must be protected.
        * This was fixed in Jericho B0.
        */

        if (!is_device_or_above(unit,JERICHO_B0)) {
            /* create failover id*/
            rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_failover_create 1\n");
                return rv;
            }
            if (verbose1) {
                printf("Failover id: 0x%x\n", failover_id_fec);
            }
        }

        l3_egress_into_overlay.l3_flags = 0;  /* flags  */
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(l3_egress_into_overlay.out_gport, vswitch_vpls_roo_info.ecmp.ecmp_intf); /* dest: fec-id instead of port */
        l3_egress_into_overlay.vlan = vswitch_vpls_roo_info.native_out_to_core_vlan; 
        l3_egress_into_overlay.out_tunnel_or_rif = native_ingress_intf; /* l3_intf  */
        l3_egress_into_overlay.next_hop_mac_addr = vswitch_vpls_roo_info.native_mac; /* next-hop */
        l3_egress_into_overlay.failover_id = failover_id_fec;
        l3_egress_into_overlay.failover_if_id = 0;
        /* BCM_L3_NATIVE_ENCAP is for QAX and above devices:
           indicate we build a native LL. (set eedb entry in a native LL eedb phase)  */
        if (is_device_or_above(unit,JERICHO_PLUS)) {
            l3_egress_into_overlay.l3_flags = BCM_L3_NATIVE_ENCAP;
        }

        if (!is_device_or_above(unit,JERICHO_B0)) {
            /* Create protected FEC */
            rv = l3__egress__create(unit, &l3_egress_into_overlay);
            if (rv != BCM_E_NONE) {
                printf("Error, l3__egress__create\n");
                return rv;
            }
            l3_egress_into_overlay.failover_if_id = l3_egress_into_overlay.fec_id;
            l3_egress_into_overlay.arp_encap_id = 0;
        }
        /* primary FEC */
        rv = l3__egress__create(unit, &l3_egress_into_overlay);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        /* Routing interface, facing core side*/
        core_intf.my_global_mac = mpls_lsr_info_1.my_mac;
        core_intf.my_lsb_mac = vswitch_vpls_roo_info.core_src_mac;
        core_intf.vsi = vswitch_vpls_roo_info.native_out_to_core_vlan;
        rv = l3__intf_rif__create(unit, &core_intf);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__intf_rif__create\n");
            return rv;
        }

        /* Add LPM entry for access to core traffic*/
        sal_memset(&route_entry, 0, sizeof(route_entry));
        route_entry.address =  vswitch_vpls_roo_info.access_to_core_dip;
        route_entry.mask = 0xffffffff;
        rv = l3__ipv4_route__add(unit, 
                                 route_entry,
                                 0,     /* router instance */
                                 l3_egress_into_overlay.fec_id /* data of LPM entry: cascaded fec */        
                                 ); 
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__ipv4_route__add\n");
            return rv;
        }

        /* 2: Routing out of overlay */

        /* Routing interface, facing access side*/
        access_intf.my_global_mac = mpls_lsr_info_1.my_mac;
        access_intf.my_lsb_mac = vswitch_vpls_roo_info.access_src_mac;
        access_intf.vsi = vswitch_vpls_roo_info.native_core_to_access_vlan;
        rv = l3__intf_rif__create(unit, &access_intf);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__intf_rif__create\n");
            return rv;
        }

        /* Create simple fec for routing out of overlay */
        l3_egress_out_of_overlay.l3_flags = 0;  /* flags  */
        l3_egress_out_of_overlay.out_gport = vswitch_vpls_info_1.ac_in_port; /* dest: fec-id instead of port */
        l3_egress_out_of_overlay.vlan = vswitch_vpls_roo_info.native_core_to_access_vlan;  /* Connecting to the above created out rif*/
        l3_egress_out_of_overlay.next_hop_mac_addr = vswitch_vpls_roo_info.access_dst_mac; /* next-hop */ 
        rv = l3__egress__create(unit, &l3_egress_out_of_overlay);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

       /* Add LEM entry, for core to access traffic*/
        bcm_l3_host_t_init(l3_host_simple_routing);  
        /* key of host entry */ 
        l3_host_simple_routing.l3a_ip_addr = vswitch_vpls_roo_info.core_to_access_dip; /* ip host entry */
        /* data of host entry */
        l3_host_simple_routing.l3a_intf  = l3_egress_out_of_overlay.fec_id;
        rv = bcm_l3_host_add(unit, &l3_host_simple_routing);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_host_add\n");
            return rv;
        }

    }


    return rv;
}



int
vswitch_vpls_roo_run_with_defaults(int unit){

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    vswitch_vpls_roo_info_init(unit, 1,13,13,10,20,15,30,620, 0);
    return vswitch_vpls_roo_run(unit);
}

int
vswitch_vpls_roo_run_with_defaults_dvapi(int unit, int acP, int pweP,int enable_roo_routing){
    int rv = BCM_E_NONE;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }
    vswitch_vpls_roo_info_init(unit, acP,pweP,pweP,10,20,15,30,620,enable_roo_routing);
    return vswitch_vpls_roo_run(unit);
}

int vswitch_vpls_roo_qos_map_init(int unit,int enable)
{
    int rc;
    bcm_qos_map_t roo_qos_map;
    int i = 0;

    roo_dscp_to_exp_map_enable = enable;

    rc = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &roo_qos_map_id);

   printf("roo_qos_map_id = %d\n",roo_qos_map_id);
   for (i = 0; i < 256; i++)
    {
	 bcm_qos_map_t_init(&roo_qos_map);
	 roo_qos_map.remark_int_pri = i; 
	 roo_qos_map.exp = 3;

        rc = bcm_qos_map_add(unit, BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3, &roo_qos_map, roo_qos_map_id);
	if (rc != BCM_E_NONE) {
	    printf("Error, roo qos map failed rc=%d,  unit %d \n", rc, unit);
	 }
    }

   return 0;
}



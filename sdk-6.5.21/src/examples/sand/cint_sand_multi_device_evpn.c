/* $Id: cint_sand_multi_device_evpn.c,
 * v 1.15 2013/07/25 12:39:34
 * v 1.16 2018/06/25 12:39:34
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_multi_device_evpn.c
 * Purpose: Example of a EVPN application. 
 */

/* 
 *  EVPN core diagram:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                            .  .  . . .      . . .   . . .                                            |
 *  |                                          .            .  .       .       .                                           |
 *  |   Access                               .                                  .                                          |                       
 *  |                                      .                MPLS Core            . .                                       |
 *  |                                    .   /+-----+  -   -   -   -   -   -+-----+  .                                     |             
 *  |   +--------+                     .      | P1  | \                 /   | P3  | \                                      | 
 *  |  /        /|                    .   /   |     |                       |     | \  .                                   | 
 *  | +--------+ |                   .        +-----+   \             /     +-----+   \  .                                 |
 *  | |  CE1   | |\                 .  /       / |                             |      \   .                                |
 *  | |        |/  \    +--------+ .                      \         /                   \   .+--------+\                   |
 *  | +--------+\ esi  /        /| /        /    |                             |       \    /        /|                    |
 *  |             200\+--------+ |                          \     /                       \+--------+ | \                  |
 *  |             \   |  PE1   | |      /        |                             |        \  |   PE3  | |                    |
 *  |                 |        |/ \                          \ /                           |        |/   \                 |
 *  |               \ +--------+     /           |                             |         \/+--------+                      |
 *  |                 /             \                        / \                                          \                |
 *  |                             /              |                             |        / \           esi    +--------+    |
 *  |               /  \+--------+    \                    /     \                         +--------+ 100   /        /|    |
 *  |                  /        /|\              |                             |      /   /        /|    / +--------+ |    |
 *  |             /   +--------+ |      \                /         \                     +--------+ |      |  CE2   | |    |
 *  |   +--------+ esi|  PE2   | |. \            |                             |    /    |   PE4  | |  /   |        |/     |
 *  |  /        /| 300|        |/  .      \            /             \                   |        |/       +--------+      |
 *  | +--------+ |  / +--------+    . \          |                             |  /     /+--------+  /                     |
 *  | |  CE3   | | /                 .      \+-----+  /                 \   +-----+       .                                |
 *  | |        |/                      .\    | P2  |                        | P4  |   / .                                  |
 *  | +--------+ /                      . \  |     |/                     \ |     |    .                                   |
 *  |                                   .   \+-----+  -   -   -   -   -  -  +-----+ / .                                    |
 *  |                                     .                                   . .  .                                       |
 *  |                                      .               . .       .      .                                              |
 *  |                                        .  .   .  .  .   .  .  . . . .                                                |
 *  |                                                                                                                      |
 *  |                                                                                                                      |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Configuration:
 *
 *  Soc properties:
 *
 *  bcm886xx_roo_enable=1
 *  evpn_enable=1
 *  roo_extension_label_encapsulation=1
 *  bcm886xx_pph_learn_extension_disable=0
 *
 *  BCM shell + cint calls (start from BCM shell):
 *  Pass add_frr_label=1 for adding a frr label to mpls stack, otherwise pass add_frr_label=0.
 *  field_processor_evpn_example() sets pmf configurations.
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_port.c
 * cint ../../../../src/examples/sand/utils/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/sand/utils/cint_sand_utils_vlan.c
 * cint ../../../../src/examples/sand/utils/cint_sand_utils_l3.c
 * cint ../../../../src/examples/sand/cint_sand_multi_device_ipv4_uc.c
 * cint ../../../../src/examples/sand/cint_field_evpn.c
 * cint ../../../../src/examples/sand/cint_sand_multi_device_evpn.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int ce1_port = 200;
 * int ce3_port = 201;
 * int p1_port = 202;
 * int p2_port = 203;
 * int no_lsp = 0;
 * evpn_add_frr_label = 1;
 * evpn_nof_linker_layer_tags=1;
 * rv = evpn_pe_config(unit,ce1_port,ce3_port,p1_port,p2_port);
 * print rv;
 * rv = field_processor_evpn_example(unit);
 * print rv;
 *
 *
 *  EVPN scenarios configured in this cint:
 *
 *  1)  Global configurations for pe:
 *      1.1) Configure vsi for this application. will be used for the flooding domain of the application.
 *      1.2) Open a muilticast group that will serve as the flooding domain of the application. Receives
 *           as a parameter the above configured vsi.
 *      1.3) Configure port properties for this application.
 *
 *  2)  Create ingress pe known unicast (locally learned destination).
 *      Packet arrives from ce1, destined for ce3. The mac address
 *      for ce3 was previously learned at pe1
 *      2.1) Create in ac for ce1.
 *      2.2) Create out ac for ce3.
 *
 *  3)  Create ingress pe known unicast traffic with remote learning.
 *      Packet arrives from ce1, destined for ce2. The mac address
 *      for ce2 was previously learned at pe3 and distributed to pe1 via BGP.
 *      Packet exits pe1 with evpn over lsp over (optional) frr label.
 *      3.1) Create in ac for ce1.
 *      3.2) Create a (outgoing) rif with pe1's my_mac facing p1.
 *      3.3) Create egress tunnel: evpn over lsp. Set the tunnel to point to the above created rif.
 *      3.4) Create a fec pointing to this tunnel.
 *      3.5) Create a MACT entry with ce2's mac, the egress tunnel's gport as destination.
 *      3.6) Set LL information (next hop for the above created tunnel).
 *      3.7) Optional: Add FRR label to be placed on top of the mpls stack.
 *           Note: in Jericho Plus and above devices, the sequence is different compared to jericho.
 *           See below the new configuration with this numbering and the is jericho plus or above device condition.
 *
 *  4)  Create ingress pe bum traffic: receive a packet from ce1
 *      with an unknown DA. Replicate the packet with a MC group (flooding domain) comprising of:
 *      1. A copy to ce3, which belongs to esi 300, of which pe1 is the DF.
 *      2. Two copies with IML over lsp over (optional) frr label, for remote pes not attached to ce1's esi.
 *      3. One copy with ESI over IML over lsp over (optional) frr label for pe2, which is attached to ce1's esi.   
 *      4.1) Create in ac for ce1.
 *      4.2) Set additional data for ce1's ac for pmf processing.
 *      4.3) Set PMF inlif profile for ce1's ac.
 *      4.4) Create ac for ce3 (add it to flooding domain).
 *      4.5) Create a (outgoing) rif with pe's my_mac facing p1.
 *      4.6) Create egress tunnels: two with IML over lsp, one with esi over iml over lsp. Set them
 *          to point to the link layer entry.
 *      4.7) Set LL information (next hop for the above created tunnels).
 *      4.8) Set additional data for the tunnel of pe1->pe2, pe1->pe3, pe1->pe4.
 *      4.9) Optional: Add FRR label to be placed on top of the mpls stack (all three copies; FRR labels vary according to the copy).
 *           Note: in Jericho Plus and above devices, the sequence is different compared to jericho.
 *           See below the new configuration with this numbering and the is jericho plus or above device condition.
 *  5)  Create egress pe known unicast. Pe1 receives a packet destined for ce3, SA==ce2_mac.
 *      ce3's mac was learned by pe3 from pe1 via BGP. Packet is EVPN over LSP.
 *      Terminate the tunnels and forward to ce3 (pe1 is ce3' DF).
 *      5.1) Create out ac for ce3.
 *      5.2) Create in ac for p1.
 *      5.3) Create ingress tunnel (for termination) with EVPN ovel LSP.
 *
 *  6)  Create egress pe bum traffic. Handle two cases:
 *      1. Send a packet with IML over LSP from pe3, with unknown destination. Packet should
 *      be flooded to ce3 (pe1 is it's DF) and to remote pes (will be pruned due to orientation). In fact,
 *      only the packet to ce3 will exit the device.
 *      2. Send a packet with ESI over IML over LSP from pe2, with unknown destination.
 *      Packet should be flooded to ce1 (will be filtered due to same esi value as in ESI label) and to
 *      remote PEs (will be pruned due to orientation).
 *      6.1) Create out ac for ce3.
 *      6.2) Create in ac for p1 and an in ac for p2.
 *      6.3) Create a (outgoing) rif with pe's my_mac facing p1.
 *      6.4) Create egress tunnels: two with IML over lsp, one with esi over iml over lsp. Set them
 *           to point to the link layer entry (eventually they will be pruned due to orientation).
 *      6.5) Set LL information (next hop for the above created tunnels).
 *      6.6) Set additional data for the tunnel of pe1->pe2, pe1->pe3, pe1->pe4.
 *      6.7) Set IML label range.
 *      6.8) Create 4 ingress tunnels (representing 2 label stacks) :
 *           1. for pe2->pe1 create one tunnel with lsp,
 *             the other with IML (and an indication for ESI).
 *           2. for pe3->pe1 create one tunnel with lsp, the other with IML (without
 *              an indication for ESI.
 *
 *  7)  Create unicast routing into evpn core: Packet arrives from ce1 as unicast ipv4 over ethernet, destined for ce2.
 *      Packet exits pe1 with ipv4 over evpn over lsp over (optional) frr label.
 *      7.1) Create in ac for ce1.
 *      7.2) Create a rif (incoming + outgoing linker layer).
 *      7.3) Create a rif (outgoing native).
 *      7.4) Create LL information (linker layer), to pointed by the egress tunnel.
 *      7.5) Create egress tunnel: evpn over lsp. Set the tunnel to point to the above created linker layer.
 *      7.6) Create an overlay fec (cascaded) pointing to this tunnel.
 *      7.7) Create a native fec pointing to the overlay fec.
 *      7.8) Create routing entry pointing to native fec.
 *      7.9) Optional: Add FRR label to be placed on top of the mpls stack.
 *           Note: in Jericho Plus and above devices, the sequence is different compared to jericho.
 *           See below the new configuration with this numbering and the is jericho plus or above device condition.   
 *  8)  Create multicast routing into evpn core: Packet arrives from ce1 as multicast ipv4 over ethernet.
 *      Packet exits pe1 with 4 copies:
 *      1. A copy to ce3, which belongs to esi 300, of which pe1 is the DF (falls back to bridge).
 *      2. Two copies with IML over lsp over (optional) frr label, for remote pes not attached to ce1's esi.
 *      3. One copy with ESI over IML over lsp over (optional) frr label for pe2, which is attached to ce1's esi (falls back to bridge).
 *      8.1)  Create in ac for ce1.
 *      8.2)  Create out ac for ce3.
 *      8.3)  Set PMF inlif profile for ce1's ac.
 *      8.4)  Create a rif (incoming + outgoing linker layer).
 *      8.5)  Create a rif (outgoing native).
 *      8.6)  Create egress tunnels: two with IML over lsp, one with esi over iml over lsp. Set them
 *            to point to the link layer entry.
 *      8.7)  Open multicast group and add members to the group (each copy contains outrif and outlif. Packets
 *            that fall back to bridge entail in rif == out rif.
 *      8.8)  Create a routing entry with the multicast group as destination.
 *      8.9) Set additional data for the tunnel of pe1->pe2, pe1->pe3, pe1->pe4.
 *      8.10) Optional: Add FRR label to be placed on top of the mpls stack.
 *            Note: in Jericho Plus and above devices, the sequence is different compared to jericho.
 *            See below the new configuration with this numbering and the is jericho plus or above device condition.
 *
 *  Traffic:
 *
 *  Send traffic according to the above scenarios. We assume evpn_add_frr_label=1 , evpn_nof_linker_layer_tags=1, no_lsp=0.
 *
 *  ##############################################
 *  Scenario 1: Global configurations. No traffic.
 *  ##############################################
 *
 *
 *  ##################################################################
 *  Scenario 2: Ingress pe known unicast (locally learned destination)
 *  ##################################################################
 *
 *  Sending packet from port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:14|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:14|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 *  #################################################################
 *  Scenario 3: Ingress pe known unicast traffic with remote learning
 *  #################################################################
 *
 *  Sending packet from port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:13|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5200||Label:8000||01:13|01:12|0x8100|20 ||    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 *  ##################################
 *  Scenario 4: Ingress pe bum traffic
 *  ##################################
 * 
 *  Sending packet from port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:17|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet (First copy) on port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |01:16|11:00:00:01:11||0x8100|20 ||Label:9100||Label:5100||Label:6100||Label:200 ||01:17|01:12|0x8100|20 ||    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||TTL:1     ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Second copy) on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5300||Label:6300||01:17|01:12|0x8100|20 ||    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Third copy) on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5200||Label:6200||01:17|01:12|0x8100|20 ||    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Fourth copy) on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:17|01:12||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 *  ###################################
 *  Scenario 5: Egress pe known unicast
 *  ###################################
 * 
 *  Sending packet from port == 202:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:5200||Label:8000||01:14|01:13|0x8100|20 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:60    ||TTL:60    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:14|01:13||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 *  #################################
 *  Scenario 6: Egress pe bum traffic
 *  #################################
 * 
 *  Sending packet from port == 202:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:15||0x8100|20 ||Label:5200||Label:6200||01:17|01:13|0x8100|20 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:32    ||TTL:32    ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-
 *   |    | DA  | SA  ||TPID1 |VID||Data|
 *   |    |01:17|01:13||0x8100|20 ||    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *   Sending packet from port == 202:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    |      DA      | SA  ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||Data|
 *   |    |11:00:00:01:11|01:16||0x8100|20 ||Label:5100||Label:6100||Label:300 ||01:17|01:14|0x8100|20 ||    |
 *   |    |              |     ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||    | 
 *   |    |              |     ||      |   ||TTL:60    ||TTL:20    ||TTL:1     ||     |     |      |   ||    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *   Receiving: None (all exiting packets are filtered).
 * 
 * 
 *  #####################################
 *  Scenario 7: Unicast routing into evpn
 *  #####################################
 * 
 *  Sending packet from port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    | DA  | SA  ||TPID1 |VID||      SIP      |      DIP      |TTL|Data|
 *   |    |01:13|01:12||0x8100|20 ||160.161.161.163|160.161.161.162|64 |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA  | SA  |TPID1 |VID||      SIP      |      DIP      |TTL|Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5200||Label:6200||01:13|01:18|0x8100|30 ||160.161.161.163|160.161.161.162|63 |    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||     |     |      |   ||               |               |   |    |
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||     |     |      |   ||               |               |   |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 *  #######################################
 *  Scenario 8: Multicast routing into evpn
 *  #######################################
 * 
 *  Sending packet from port == 200:   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    | DA              | SA  ||TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:00:5E:00:01:02|01:12||0x8100|20 ||224.0.1.1|224.0.1.2|64 |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (First copy) on port == 203:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   ||   MPLS   || DA              | SA  |TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:16|11:00:00:01:11||0x8100|20 ||Label:9100||Label:5100||Label:6100||Label:200 ||01:00:5E:00:01:02|01:12|0x8100|20 ||224.0.1.1|224.0.1.2|64 |    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||Exp:0     ||                 |     |      |   ||         |         |   |    |
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||TTL:1     ||                 |     |      |   ||         |         |   |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Second copy) on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA              | SA              |TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5300||Label:6300||01:00:5E:00:01:02|00:11:00:00:01:18|0x8100|30 ||224.0.1.1|224.0.1.2|63 |    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||                 |                 |      |   ||         |         |   |    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||                 |                 |      |   ||         |         |   |    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Third copy) on port == 202:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   
 *   |    | DA  |      SA      ||TPID1 |VID||   MPLS   ||   MPLS   ||   MPLS   || DA              | SA              |TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:15|11:00:00:01:11||0x8100|20 ||Label:9000||Label:5200||Label:6200||01:00:5E:00:01:02|00:11:00:00:01:18|0x8100|30 ||224.0.1.1|224.0.1.2|63 |    |
 *   |    |     |              ||      |   ||Exp:0     ||Exp:0     ||Exp:0     ||                 |                 |      |   ||         |         |   |    | 
 *   |    |     |              ||      |   ||TTL:255   ||TTL:60    ||TTL:20    ||                 |                 |      |   ||         |         |   |    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet (Fourth copy) on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |    | DA              | SA  ||TPID1 |VID||   SIP   |   DIP   |TTL|Data|
 *   |    |01:00:5E:00:01:02|01:12||0x8100|20 ||224.0.1.1|224.0.1.2|64 |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
   
/* Global variable that holds the bit for iml_bos_indication */   
int evpn_iml_bos_indication = 1<<25;
/* Global variable that determines whether to add a frr label on top of the encapsulation stack */ 
int evpn_add_frr_label = 0;
/* Global variable that determines whether to perform ive modification on the inner ethernet */ 
int evpn_no_inner_ive = 1;
/* Will hold all ports of the application */
int evpn_ingress_port_array[2];
int evpn_egress_port_array[2];

/* Hold the name space of per PE.*/
int dual_homed_esi_peer_profile = 5;


/* Number of vlan tags in linker layer entries of this application */
int evpn_nof_linker_layer_tags = 0;

/* Indicates whether labels are terminated in indexed mode */
/* evpn_is_indexed_termination == 1 implies that iml and evpn labels will be placed in index 2,
   lsp labels will be placed in index 1 */
int evpn_is_indexed_termination = 0;

/* Create configuration without LSP tunnels between the PEs */
int no_lsp = 0;

/* Global variable that holds tive setttings for tagged packet */   
vlan_edit_utils_s g_ive_edit_utils = {5,    /* edit profile */
                                      2,    /* tag format - default for single tag*/
                                      20};   /* action ID */

struct cint_evpn_info_s {
    int ce1_port; /* ce1 port (access)*/
    int ce3_port; /* ce3 port (access) */
    int p1_port;  /* p1 port (provider) */
    int p2_port;  /* p2 port (provider) */
    int ce1_sys_port; /* ce1 port (access)*/
    int ce3_sys_port; /* ce3 port (access) */
    int p1_sys_port;  /* p1 port (provider) */
    int p2_sys_port;  /* p2 port (provider) */
    int ce1_vlan; /* ce1's vlan */
    int ce2_vlan; /* ce2's vlan */
    int ce3_vlan; /* ce3's vlan */
    int p1_vlan;  /* p1's vlan */
    int p2_vlan;  /* p2's vlan */
    int native_vlan;  /* native vlan */
    int p1_inner_vlan; /* p1's inner vlan*/
    int p2_inner_vlan; /* p2's inner vlan*/
    int tpid;     /* tpid value for all vlan tags in the application*/
    int vsi; /* vsi representing the service in this application */
    int pe2_id; /* pe2's id */
    int pe3_id; /* pe3's id */
    int pe4_id; /* pe4's id */
    int pe1_pe2_ce3_esi_label; /* The esi of pe1, pe2, ce3 */
    int pe1_pe2_ce1_esi_label; /* The esi of pe1, pe2, ce1 */
    bcm_mac_t global_mac; /* global mac of PE1 */
    bcm_mac_t my_mac_lsb; /* PE1's my mac lsb facing p1 */
    bcm_mac_t my_mac; /* PE1's my mac facing p1 */
    bcm_mac_t my_mac2; /* PE1's my mac facing p1 */
    bcm_mac_t ce1_mac;  /* ce1's mac address*/
    bcm_mac_t ce2_mac; /* ce2's mac address*/
    bcm_mac_t ce3_mac ; /* ce3's mac address*/
    bcm_mac_t p1_mac; /* p1's mac address*/
    bcm_mac_t p2_mac; /* p2's mac address*/
    bcm_mac_t pe3_mac; /* pe3's mac address*/
    bcm_if_t pe1_rif; /* rif of flooding domain */
    bcm_if_t pe1_rif2; /* rif of flooding domain */
    bcm_gport_t p1_ing_vport_id; /* ingress vlan port id of p1's AC */
    bcm_gport_t p2_ing_vport_id; /* ingress vlan port id of p2's AC */
    bcm_gport_t ce1_ing_vport_id; /* ingress vlan port id of ce1's AC */
    bcm_gport_t ce3_ing_vport_id; /* ingress vlan port id of ce3's AC */
    bcm_gport_t p1_egr_vport_id; /* egress vlan port id of p1's AC */
    bcm_gport_t p2_egr_vport_id; /* egress vlan port id of p2's AC */
    bcm_gport_t ce1_egr_vport_id; /* egress vlan port id of ce1's AC */
    bcm_gport_t ce3_egr_vport_id; /* egress vlan port id of ce3's AC */
    bcm_mpls_label_t evpn_label; /* EVPN label for ingress and egress pe */
    bcm_mpls_label_t pe1_pe2_lsp_label; /* lsp label in pe1<-->pe2 */
    bcm_mpls_label_t pe1_pe3_lsp_label; /* lsp label in pe1<-->pe3 */
    bcm_mpls_label_t pe1_pe4_lsp_label; /* lsp label in pe1<-->pe4 */
    bcm_mpls_label_t pe1_pe2_iml_label; /* iml label in pe1<-->pe2 */
    bcm_mpls_label_t pe1_pe3_iml_label; /* lsp label in pe1<-->pe3 */
    bcm_mpls_label_t pe1_pe4_iml_label; /* lsp label in pe1<-->pe4 */
    bcm_mpls_label_t iml_range_low_label; /* low bound of iml range in egress pe */
    bcm_mpls_label_t iml_range_high_label; /* high bound of iml range in egress pe */
    bcm_mpls_label_t pe1_frr_label; /* frr (fast re-route) label for tunnel starting at pe1  */
    bcm_mpls_label_t pe2_frr_label; /* frr (fast re-route) label for tunnel starting at pe2  */
    uint32 evpn_profile; /* user defined evpn profile : 0-7 */
    bcm_ip_t ce2_ip; /* ce2's ip */ 
    bcm_ip_t mc_ip;  /* multicast ip for multicast routing into evpn */
    int pe1_linker_layer_encap_id; /* Linker layer encap id for pe1 */
    int pe2_linker_layer_encap_id; /* Linker layer encap id for pe2 */
    int pe1_frr_tunnel_id; /* FRR label tunnel id for pe1 (JR2 and above only) */
    int pe2_frr_tunnel_id; /* FRR label tunnel id for pe1 (JR2 and above only) */
    int pe1_pe3_iml_no_esi_tunnel_id; /* Egress Tunnel id of pe1<-->pe3 iml over lsp (no esi) */
    int pe1_pe4_iml_no_esi_tunnel_id; /* Egress Tunnel id of pe1<-->pe4 iml over lsp (no esi) */
    int pe1_pe2_iml_esi_tunnel_id;    /* Egress Tunnel id of pe1<-->pe2 iml over lsp (with esi) */
    int pe1_pe3_evpn_lsp_tunnel_id;   /* Egress Tunnel id of pe1<-->pe3 evpn over lsp */
    bcm_gport_t pe1_pe2_iml_label_term_tunnel_id; /* Tunnel id of pe1_pe2_iml terminated label */
    bcm_gport_t pe1_pe2_iml_label_term_tunnel_id_2; /* Tunnel id of pe1_pe2_iml terminated label */
    int pe1_pe3_evpn_lsp_fec;         /* Overlay FEC pointing to evpn over lsp tunnel */ 
    int pe1_ce2_native_fec;           /* Native FEC pointing to Overlay FEC pointing to evpn over lsp tunnel*/
    bcm_multicast_t mc_id;            /* multicast id for routing into evpn bum traffic copies*/    


};


cint_evpn_info_s cint_evpn_info = 
/* ports :
   ce1_port | ce3_port | p1_port | p2_port  */
    {200,      201,        202 ,     203,
/* ce1_sys_port | ce3_sys_port | p1_sys_port | p2_sys_port  */
    0,             0,            0 ,           0, 
/* vlans:
   ce1_vlan | ce2_vlan | ce3_vlan | p1_vlan | p2_vlan | native_vlan | p1_inner_vlan | p2_inner_vlan */
    20,          20,        20,        20,      20,          30,           40,              50,
/* tpid */
   0x8100,
/* vsi */
    20, 
/* pe2_id             | pe3_id             |  pe4_id*/
    /*100*/0x22222,    /*125*/0x33333,     /*150*/0x44444, 
/* pe1_pe2_ce3_esi_label    |  pe1_pe2_ce1_esi_label */ 
    /*300*/0x55555,             /*200*/0x11111,
/*               global_mac              |            my_mac_lsb             |               my_mac                | */              
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x11}, {0x00, 0x11, 0x00, 0x00, 0x01, 0x11},
/*                 my_mac2                             ce1_mac                               ce2_mac                 */
    {0x00, 0x11, 0x00, 0x00, 0x01, 0x18}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x12}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x13},
/*                 ce3_mac                              p1_mac                                p2_mac                 */     
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x14},{0x00, 0x00, 0x00, 0x00, 0x01, 0x15}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x16}, 
/*                 pe3_mac              */
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x17},
/*  pe1_rif | pe1_rif2 */ 
       0,        0,
/* p1_ing_vport_id | p2_ing_vport_id | ce1_ing_vport_id | ce3_ing_vport_id */
        0,            0 ,           0,              0,
/* p1_egr_vport_id | p2_egr_vport_id | ce1_egr_vport_id | ce3_egr_vport_id */
        0,            0 ,           0,              0, 
/* evpn_label | pe1_pe2_lsp_label | pe1_pe3_lsp_label | pe1_pe4_lsp_label */                                                                                                                 
       8000,          5100,               5200,               5300,
/* pe1_pe2_iml_label | pe1_pe3_iml_label | pe1_pe4_iml_label | iml_range_low_label */ 
         6100,               6200,               6300,                6000,
/* iml_range_high_label | pe1_frr_label | pe2_frr_label */                                                                         
       7000,                  828300,            828600, 
/* evpn_profile */
        1,
/*   ce2_ip  */
   0xA0A1A1A2 /* 160.161.161.162 */,
/*   mc_ip   */ 
   0xE0000103 /* 224.0.1.3 */,
/* pe1_linker_layer_encap_id | pe2_linker_layer_encap_id | pe1_frr_tunnel_id | pe2_frr_tunnel_id */
              0,                          0,                     0,                  0,
/* pe1_pe3_iml_no_esi_tunnel_id | pe1_pe4_iml_no_esi_tunnel_id | pe1_pe2_iml_esi_tunnel_id | pe1_pe3_evpn_lsp_tunnel_id | pe1_pe2_iml_label_term_tunnel_id | pe1_pe2_iml_label_term_tunnel_id_2 */
              0,                              0,                            0,                            0,                              0,                                0,
/* pe1_pe3_evpn_lsp_fec */
             0,
/* pe1_ce2_native_fec */
             0,
/* mc_id */
   10960,
};



void multi_dev_evpn_info_init(
    int ingress_unit,
    int egress_unit,
    int ce1_sys_port,
    int ce3_sys_port, 
    int p1_sys_port,
    int p2_sys_port)
{
    int ce1_port, ce3_port, p1_port, p2_port;

    cint_evpn_info.ce1_sys_port = ce1_sys_port;
    cint_evpn_info.ce3_sys_port = ce3_sys_port;
    cint_evpn_info.p1_sys_port = p1_sys_port;
    cint_evpn_info.p2_sys_port = p2_sys_port;

    /** Local ports were set in tests*/ 
    evpn_ingress_port_array[0] = cint_evpn_info.ce1_port;
    evpn_ingress_port_array[1] = cint_evpn_info.ce3_port; 
    evpn_egress_port_array[0] = cint_evpn_info.p1_port;
    evpn_egress_port_array[1] = cint_evpn_info.p2_port;

    if (no_lsp) {
        cint_evpn_info.pe1_pe2_lsp_label = 0;
        cint_evpn_info.pe1_pe3_lsp_label = 0;
        cint_evpn_info.pe1_pe4_lsp_label = 0;
    }

    if (is_device_or_above(ingress_unit, JERICHO2)) {
        /** In JR2, single tag format ID should be 4-7.*/
        g_ive_edit_utils.tag_format = 4;
    }
}

/* Main function */
int multi_dev_evpn_pe_config(
    int ingress_unit,
    int egress_unit,
    int ce1_sys_port,
    int ce3_sys_port, 
    int p1_sys_port,
    int p2_sys_port)
{

    int rv = BCM_E_NONE;

    multi_dev_evpn_info_init(ingress_unit, egress_unit, ce1_sys_port, ce3_sys_port, p1_sys_port, p2_sys_port); 

    /* Configure vsi for this application */
    rv = bcm_vswitch_create_with_id(ingress_unit, cint_evpn_info.vsi); /* 1.1 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error(%d), in bcm_vswitch_create_with_id with vid = %d\n", cint_evpn_info.vsi);
        return rv;
    }

    if (egress_unit != ingress_unit) {
        /** Create the vsi in egress device in multi-dev case.*/
        rv = bcm_vswitch_create_with_id(egress_unit, cint_evpn_info.vsi); /* 1.1 */
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error(%d), in bcm_vswitch_create_with_id with vid = %d\n", cint_evpn_info.vsi);
            return rv;
        }
    }

    /* 
     * Create multicast group (flooding for this service).
     * multicast id has to be the same one as the vswitch.
     * EVPN use ingress MC.
     */
    egress_mc = 0;
    rv = multicast__open_mc_group(ingress_unit, &cint_evpn_info.vsi, 0); /* 1.2 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in multicast__open_mc_group\n");
        return rv;
    }
    printf("\ncreate mc group (0x%08X) successfully!\n", cint_evpn_info.vsi);

    /* Configure port properties for this application */
    rv = multi_dev_evpn_configure_port_properties(ingress_unit, egress_unit,
                                                  evpn_ingress_port_array, evpn_egress_port_array, 2, 2); /* 1.3 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in multi_dev_evpn_configure_port_properties\n");
        return rv;
    }
    printf("\nconfigure port properties successfully!\n");

    /* Configure L2 information */
    rv = multi_dev_evpn_create_l2_interfaces(ingress_unit, egress_unit); /* 2.1, 2.2, 3.1, 4.1, 4.2, 4.3, 4.4, 5.1, 5.2 ,6.1, 6.2, 7.1, 8.1, 8.2, 8.3 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in multi_dev_evpn_create_l2_interfaces\n");
        return rv;
    }
    printf("\ncreate l2 interface successfully!\n");

    /* Configure L3 information (router interfaces) */
    rv = multi_dev_evpn_create_l3_interfaces(ingress_unit, egress_unit); /* 3.2, 4.5, 7.2, 7.3, 8.4, 8.5 */ 
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in multi_dev_evpn_create_l3_interfaces\n");
        return rv;
    }
    printf("\ncreate l3 interface successfully!\n");

    /* Create egress tunnels for forwarding */
    rv = multi_dev_evpn_create_egress_tunnels(ingress_unit, egress_unit, 1); /* 3.3, 3.4, 3.5, 4.6, 4.8, 6.4, 6.5, 6.6, 7.5, 8.6, 8.9 */
    if (rv != BCM_E_NONE) {
         print rv;
         printf("Error, in multi_dev_evpn_create_egress_tunnels\n");
         return rv;
    }
    printf("\ncreate egress tunnel successfully!\n");

    /* Configure EVE for EVPN in QAX and above, In Jericho, EVE is performed via bcm_l2_egress_create*/
    rv = evpn_create_eve(egress_unit);
    if (rv != BCM_E_NONE) {
        printf("unit:%d, Error: %d, in evpn_create_ac with evpn_create_eve\n", egress_unit, rv);
        return rv;
    }
    printf("\nConfigure EVE for EVPN in QAX and above successfully!\n");

    /* Create ingress tunnels for termination*/
    rv = evpn_create_ingress_tunnels(ingress_unit); /* 5.3, 6.7, 6.8 */
    if (rv != BCM_E_NONE) {
         print rv;
         printf("Error, in evpn_create_ingress_tunnels\n");
         return rv;
     }
    printf("\ncreate ingress tunnel successfully!\n");

    rv = multi_dev_evpn_routing_into_evpn(ingress_unit, egress_unit); /* 7.7, 7.8, 8.7, 8.8 */
    if (rv != BCM_E_NONE) {
         print rv;
         printf("Error, in multi_dev_evpn_routing_into_evpn\n");
         return rv;
     }
    printf("\nconfigure routing into evpn successfully!\n");

    print cint_evpn_info;
    return rv;
}


/* Create AC, add ESEM entry for EVE, add it to flooding domain */
int evpn_create_ac(int unit, int flags,  int vlan,
                   int vsi, int port, bcm_gport_t *ing_vport_id, bcm_gport_t *egr_vport_id,
                   bcm_mac_t mac_address, int add_to_flooding_domain)
{

    int rv = BCM_E_NONE;
    l2__mact_properties_s mact_properties;
    bcm_port_match_info_t port_match_info;

    /* Create a vlan port */
    rv = l2__port_vlan__create(unit, flags,  port, vlan, 0, ing_vport_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__port_vlan__create\n");
        return rv;
    }
    rv = bcm_vswitch_port_add(unit, vsi, *ing_vport_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vswitch_port_add\n");
        return rv;
    }

    /* Add an ESEM entry that points to the out AC for EVE */
    if (is_device_or_above(unit, JERICHO2)){
        uint32 flags_i;
        flags_i |= flags;
        flags_i = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
        flags_i |= BCM_VLAN_PORT_VLAN_TRANSLATION;
        rv = l2__port_vlan__create(unit, flags_i, port, vlan, vsi, egr_vport_id);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in l2__port_vlan__create\n");
            return rv;
        }
    } else {
        *egr_vport_id = *ing_vport_id;

        bcm_port_match_info_t_init(&port_match_info);
        port_match_info.match = BCM_PORT_MATCH_PORT_VLAN;
        port_match_info.port = port;
        port_match_info.match_vlan = vsi;
        port_match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
        rv = bcm_port_match_add(unit, *egr_vport_id ,&port_match_info); /* associating the port to lif */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_match_add\n");
            print rv;
            return rv;
        }
    }

    /* add the vlan port to the flooding domain (cud)*/
    if (add_to_flooding_domain) {
        rv = multicast__gport_encap_add(unit, cint_evpn_info.vsi, port, *ing_vport_id, 0);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in multicast__vlan_encap_add\n");
            return rv;
        }
    }
    return rv;

}


/* Create routing interface*/
int multi_dev_evpn_create_l3_interfaces(int ingress_unit, int egress_unit)
{
    int rv = BCM_E_NONE;
    create_l3_intf_s ll_intf, native_intf;

    ll_intf.my_lsb_mac = cint_evpn_info.my_mac_lsb;
    ll_intf.my_global_mac = cint_evpn_info.global_mac;
    ll_intf.rif = cint_evpn_info.pe1_rif;
    ll_intf.vsi = cint_evpn_info.vsi;
    ll_intf.rpf_valid = 0;
    /* create rif */
    rv = l3__intf_rif__create(ingress_unit, &ll_intf); /* 3.2, 4.5, 6.3, 7.2, 8.4 */
    if (rv != BCM_E_NONE) {
        printf("unit: %d, Error: %d, in l3__intf_rif__create\n", ingress_unit, rv);
        return rv;
    }

    cint_evpn_info.pe1_rif = ll_intf.rif;

    native_intf.my_lsb_mac = cint_evpn_info.my_mac2;
    native_intf.my_global_mac = cint_evpn_info.global_mac;
    native_intf.rif = cint_evpn_info.pe1_rif2;
    native_intf.vsi = cint_evpn_info.native_vlan;
    native_intf.rpf_valid = 0;
    /* create rif */
    rv = l3__intf_rif__create(ingress_unit, &native_intf); /* 7.3 , 8.5*/
    if (rv != BCM_E_NONE) {
        printf("unit: %d, Error: %d, in l3__intf_rif__create\n", ingress_unit, rv);
        return rv;
    }

    cint_evpn_info.pe1_rif2 = native_intf.rif;

    /** Create the same L3 interface in egress device in case of multi-dev*/
    if (ingress_unit != egress_unit) {
        ll_intf.my_lsb_mac = cint_evpn_info.my_mac_lsb;
        ll_intf.my_global_mac = cint_evpn_info.global_mac;
        ll_intf.rif = cint_evpn_info.pe1_rif;
        ll_intf.vsi = cint_evpn_info.vsi;
        ll_intf.rpf_valid = 0;
        /* create rif */
        rv = l3__intf_rif__create(egress_unit, &ll_intf); /* 3.2, 4.5, 6.3, 7.2, 8.4 */
        if (rv != BCM_E_NONE) {
            printf("unit: %d, Error: %d, in l3__intf_rif__create\n", egress_unit, rv);
            return rv;
        }

        native_intf.my_lsb_mac = cint_evpn_info.my_mac2;
        native_intf.my_global_mac = cint_evpn_info.global_mac;
        native_intf.rif = cint_evpn_info.pe1_rif2;
        native_intf.vsi = cint_evpn_info.native_vlan;
        native_intf.rpf_valid = 0;
        /* create rif */
        rv = l3__intf_rif__create(egress_unit, &native_intf); /* 7.3 , 8.5*/
        if (rv != BCM_E_NONE) {
            printf("unit: %d, Error: %d, in l3__intf_rif__create\n", egress_unit, rv);
            return rv;
        }
    }

    return rv;
}

/* Set egress tunnel */
int multi_dev_evpn_set_egress_tunnel(int ingress_unit, int egress_unit, int add_to_flooding_domain, int set_split_horizon,
                                     int flags, bcm_mpls_label_t label_in, bcm_mpls_label_t label_out,
                                     bcm_mpls_egress_action_t egress_action, int port, int *tunnel_id, int out_rif){
    
    int rv = BCM_E_NONE;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    bcm_gport_t tunnel_gport;

    if (is_device_or_above(egress_unit, JERICHO2)) {
        /** In JR2, LSP and IML/EVPN label can't be created in the same entry.*/
        int next_encap_id = out_rif;
        bcm_mpls_egress_label_t label_array;

        if (label_out != mpls_default_egress_label_value) {
            bcm_mpls_egress_label_t_init(&label_array);
            label_array.label = label_out;
            label_array.l3_intf_id = next_encap_id;
            label_array.ttl = ext_mpls_ttl;
            label_array.flags = flags | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            /** It should be a LSP, unset the _EVPN/_IML flag.*/
            label_array.flags &= (~BCM_MPLS_EGRESS_LABEL_EVPN);
            label_array.flags &= (~BCM_MPLS_EGRESS_LABEL_IML);

            label_array.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            if (mpls_pipe_mode_exp_set) {
                label_array.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
                label_array.exp = ext_mpls_exp;
            } else {
                label_array.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            }
            label_array.encap_access = bcmEncapAccessTunnel2;
            rv = bcm_mpls_tunnel_initiator_create(egress_unit, 0, 1, &label_array);
            if (rv != BCM_E_NONE) {
                print rv;
                printf("Error, in bcm_mpls_tunnel_initiator_create\n");
                return rv;
            }
            next_encap_id = label_array.tunnel_id;
        }

        bcm_mpls_egress_label_t_init(&label_array);
        label_array.label = label_in;
        label_array.l3_intf_id = next_encap_id;
        label_array.ttl = mpls_ttl;
        label_array.flags = flags | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        if (mpls_pipe_mode_exp_set) {
            label_array.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
            label_array.exp = mpls_exp;
        } else {
            label_array.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        }
        label_array.encap_access = bcmEncapAccessTunnel1;
        rv = bcm_mpls_tunnel_initiator_create(egress_unit, 0, 1, &label_array);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_mpls_tunnel_initiator_create\n");
            return rv;
        }
        *tunnel_id =  label_array.tunnel_id;
    } else {
        sal_memset(&mpls_tunnel_properties, 0, sizeof(mpls_tunnel_properties));
        mpls_tunnel_properties.flags = flags;
        mpls_tunnel_properties.flags_out = flags;
        mpls_tunnel_properties.label_in = label_in;
        mpls_tunnel_properties.label_out = label_out;
        mpls_tunnel_properties.egress_action = egress_action;
        mpls_tunnel_properties.next_pointer_intf = out_rif;

        /* Create egress tunnel */
        rv  = mpls__create_tunnel_initiator__set(egress_unit, &mpls_tunnel_properties);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in mpls__create_tunnel_initiator__set\n");
            return rv;
        }

        *tunnel_id =  mpls_tunnel_properties.tunnel_id;
    }

    /** Encode the tunnel ID as gport.*/
    BCM_GPORT_TUNNEL_ID_SET(tunnel_gport,  *tunnel_id);

    /* Adding tunnel_id to flooding domain (cud)*/
    if (add_to_flooding_domain) {
        rv = multicast__gport_encap_add(ingress_unit, cint_evpn_info.vsi, port, tunnel_gport, 0);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in multicast__mpls_encap_add\n");
            return rv;
        }
    }

    /* configure split horizon for this tunnel */
    if (set_split_horizon) {
        rv = bcm_port_class_set(egress_unit, tunnel_gport, bcmPortClassForwardEgress, 1);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_port_class_set, evpn_set_egress_tunnel\n");
            return rv;
        }
    }

    return rv;
}


/* Set ingress tunnel (for termination)*/
int evpn_set_ingress_tunnel(int unit, int set_split_horizon, \
                            int flags, bcm_mpls_label_t label, bcm_mpls_label_t second_label, \
                            bcm_mpls_switch_action_t action, bcm_if_t tunnel_if, bcm_gport_t *tunnel_id, bcm_gport_t *tunnel_id_2){

    int rv = BCM_E_NONE;
    mpls__ingress_tunnel_utils_s mpls_tunnel_properties;
    int tmp_tunnel_id = 0;
    int lbl, second_lbl;
    int mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);


    mpls_tunnel_properties.flags = flags;
    mpls_tunnel_properties.tunnel_if = tunnel_if;
    mpls_tunnel_properties.vpn = cint_evpn_info.vsi;
    mpls_tunnel_properties.action = action;
    mpls_tunnel_properties.label = label;
    if (mpls_termination_label_index_enable) {
        lbl = label;
        BCM_MPLS_INDEXED_LABEL_SET(mpls_tunnel_properties.label, lbl, 1);
    }
    mpls_tunnel_properties.second_label = second_label;
     if (mpls_termination_label_index_enable) {
         second_lbl = second_label;
        BCM_MPLS_INDEXED_LABEL_SET(mpls_tunnel_properties.second_label, second_lbl, 2);
    }
    mpls_tunnel_properties.tunnel_id = 0;

    /* Create ingress tunnel */
    rv  = mpls__add_switch_entry(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    }

    *tunnel_id = mpls_tunnel_properties.tunnel_id;

    if (set_split_horizon) {

        /* Tunnel encoding need for bcm_port_class_set*/
        tmp_tunnel_id = mpls_tunnel_properties.tunnel_id;
        /* Unset iml_bos_indication (bit 25), if it is set */
        evpn_unset_iml_bos_indication(tmp_tunnel_id);

        /* Set split horizon: orientation==1*/
        rv = bcm_port_class_set(unit,tmp_tunnel_id, bcmPortClassForwardIngress, 1);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_port_class_set\n");
            return rv;
        }
    }

    /* Insert another copy (in case the tunnel is in a different index in the stack */
    if (mpls_termination_label_index_enable) {
        lbl = label;
        BCM_MPLS_INDEXED_LABEL_SET(mpls_tunnel_properties.label, lbl, 2);
        rv  = mpls__add_switch_entry(unit, &mpls_tunnel_properties);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in mpls__add_switch_entry\n");
            return rv;
        }

        *tunnel_id_2 = mpls_tunnel_properties.tunnel_id;
        if (set_split_horizon) {

            /* Tunnel encoding need for bcm_port_class_set*/
            tmp_tunnel_id = mpls_tunnel_properties.tunnel_id;
            /* Unset iml_bos_indication (bit 25), if it is set */
            evpn_unset_iml_bos_indication(tmp_tunnel_id);

            /* Set split horizon: orientation==1*/
            rv = bcm_port_class_set(unit,tmp_tunnel_id, bcmPortClassForwardIngress, 1);
            if (rv != BCM_E_NONE) {
                print rv;
                printf("Error, in bcm_port_class_set\n");
                return rv;
            }
        }
    }

    return rv;
}

/* Create egress tunnels with LL information */
int multi_dev_evpn_create_egress_tunnels(int ingress_unit, int egress_unit, int config_wide_data)
{
    int rv = BCM_E_NONE;
    uint64 data;
    int encap_id = 0;
    l2__mact_properties_s mact_properties;
    create_l3_egress_s l3_egress;
    int gport_forward_fec = 0;
    int add_to_flooding_domain = 0;
    int flags = 0, jr1_special_flags = 0;
    int pe1_next_encap_of_lsp, pe2_next_encap_of_lsp;
    bcm_mpls_egress_action_t egress_action = BCM_MPLS_EGRESS_ACTION_SWAP;

    COMPILER_64_ZERO(data);

    if (!is_device_or_above(egress_unit, JERICHO2)) {
        jr1_special_flags = BCM_MPLS_EGRESS_LABEL_WIDE;
        egress_action = BCM_MPLS_EGRESS_ACTION_PUSH;
    }

    /* Create linker layer entries for tunnels */
    rv = evpn_create_linker_layer_entries(egress_unit); /* 3.6, 3.7, 4.7, 6.5, 7.4*/
    if (rv != BCM_E_NONE) {
        printf("unit: %d, Error: %d, in evpn_create_linker_layer_entries\n", egress_unit, rv);
        return rv;
    }

    pe1_next_encap_of_lsp = cint_evpn_info.pe1_linker_layer_encap_id;
    pe2_next_encap_of_lsp = cint_evpn_info.pe2_linker_layer_encap_id;
    if (evpn_add_frr_label && is_device_or_above(egress_unit,JERICHO2))
    {
        pe1_next_encap_of_lsp = cint_evpn_info.pe1_frr_tunnel_id;
        pe2_next_encap_of_lsp = cint_evpn_info.pe2_frr_tunnel_id;
    }

    /* Create tunnel to pe3, which is not attached to ce1's esi*/
    add_to_flooding_domain = 1;
    flags = jr1_special_flags | BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_EVPN;
    rv = multi_dev_evpn_set_egress_tunnel(ingress_unit, egress_unit, add_to_flooding_domain, 1,
                                flags, cint_evpn_info.pe1_pe3_iml_label, cint_evpn_info.pe1_pe3_lsp_label, \ /* 4.6, 6.4, 8.6 */
                                egress_action, cint_evpn_info.p1_sys_port, &cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id, pe1_next_encap_of_lsp);

    if (rv != BCM_E_NONE) {
        printf("Error: %d, in evpn_set_egress_tunnel\n", rv);
        return rv;
    }

    /* Create tunnel to pe4, which is not attached to ce1's esi*/
    add_to_flooding_domain = 1;
    flags = jr1_special_flags | BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_EVPN;
    rv = multi_dev_evpn_set_egress_tunnel(ingress_unit, egress_unit, add_to_flooding_domain, 1, \
                            flags, cint_evpn_info.pe1_pe4_iml_label, cint_evpn_info.pe1_pe4_lsp_label, \ /* 4.6, 6.4, 8.6 */
                            egress_action, cint_evpn_info.p1_sys_port, &cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id, pe1_next_encap_of_lsp);

    if (rv != BCM_E_NONE) {
        printf("Error: %d, in evpn_set_egress_tunnel\n", rv);
        return rv;
    }


    /* Create tunnel to pe2, which is attached to ce1's esi*/
    add_to_flooding_domain = 1;
    flags = jr1_special_flags | BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_EVPN;
    rv = multi_dev_evpn_set_egress_tunnel(ingress_unit, egress_unit, add_to_flooding_domain, 1,  \
                            flags, cint_evpn_info.pe1_pe2_iml_label, cint_evpn_info.pe1_pe2_lsp_label, \ /* 4.6, 6.4, 8.6 */
                            egress_action, cint_evpn_info.p2_sys_port, &cint_evpn_info.pe1_pe2_iml_esi_tunnel_id, pe2_next_encap_of_lsp);

    if (rv != BCM_E_NONE) {
        printf("Error: %d, in evpn_set_egress_tunnel\n", rv);
        return rv;
    }


    /* Create egress tunnel with evpn and lsp labels */
    add_to_flooding_domain = 0;
    flags = BCM_MPLS_EGRESS_LABEL_EVPN;
    rv  = multi_dev_evpn_set_egress_tunnel(ingress_unit, egress_unit, add_to_flooding_domain, 0,  \ /* 3.3, 7.5 */
                            flags,cint_evpn_info.evpn_label , cint_evpn_info.pe1_pe3_lsp_label, \
                            egress_action, 0, &cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id, pe1_next_encap_of_lsp);
    if (rv != BCM_E_NONE) {
        printf("Error: %d, in evpn_set_egress_tunnel\n", rv);
        return rv;
    }

    printf("cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id: %d\n",cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id);
    rv = evpn_create_overlay_fec_entry(ingress_unit); /* 3.4, 7.6 */
    if (rv != BCM_E_NONE) {
        printf("unit: %d, Error: %d, in evpn_create_overlay_fec_entry\n", ingress_unit, rv);
        return rv;
    }

    /* Create forwarding entry for DA==ce2_mac. Result is LSP*/
    BCM_GPORT_FORWARD_PORT_SET(gport_forward_fec, cint_evpn_info.pe1_pe3_evpn_lsp_fec);
    mact_properties.gport_id = gport_forward_fec;
    mact_properties.mac_address = cint_evpn_info.ce2_mac;
    mact_properties.vlan = cint_evpn_info.ce2_vlan;
    rv = l2__mact_entry_create(ingress_unit, &mact_properties); /* 3.5 */
    if (rv != BCM_E_NONE) {
        printf("unit: %d, Error: %d, in l2__mact_entry_create\n", ingress_unit, rv);
        return rv;
    }

    if (!is_device_or_above(egress_unit, JERICHO2)) {
        /* data == PE id*/
        COMPILER_64_SET(data, 0, cint_evpn_info.pe2_id);
        /* Tunnel encoding is needed for bcm_port_wide_data_set */
        BCM_GPORT_TUNNEL_ID_SET(cint_evpn_info.pe1_pe2_iml_esi_tunnel_id,cint_evpn_info.pe1_pe2_iml_esi_tunnel_id);
        rv = bcm_port_wide_data_set(egress_unit, cint_evpn_info.pe1_pe2_iml_esi_tunnel_id,BCM_PORT_WIDE_DATA_EGRESS, data); /* 4.8, 6.6, 8.9 */ 
        if (rv != BCM_E_NONE) {
            printf("unit: %d, Error: %d, in bcm_port_wide_data_set with \n", egress_unit, rv);
            return rv;
        }

        /* data == PE id*/
        COMPILER_64_SET(data, 0, cint_evpn_info.pe3_id);
        /* Tunnel encoding is needed for bcm_port_wide_data_set */
        BCM_GPORT_TUNNEL_ID_SET(cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id,cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id);
        rv = bcm_port_wide_data_set(egress_unit, cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id,BCM_PORT_WIDE_DATA_EGRESS, data); /* 4.8, 6.6, 8.9 */ 
        if (rv != BCM_E_NONE) {
            printf("unit: %d, Error: %d, in bcm_port_wide_data_set with \n", egress_unit, rv);
            return rv;
        }

        /* data == PE id*/
        COMPILER_64_SET(data, 0, cint_evpn_info.pe4_id);
        /* Tunnel encoding is needed for bcm_port_wide_data_set */
        BCM_GPORT_TUNNEL_ID_SET(cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id,cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id);
        rv = bcm_port_wide_data_set(egress_unit, cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id,BCM_PORT_WIDE_DATA_EGRESS, data); /* 4.8, 6.6, 8.9 */ 
        if (rv != BCM_E_NONE) {
            printf("unit: %d, Error: %d, in bcm_port_wide_data_set with \n", egress_unit, rv);
            return rv;
        }
    } else {
        bcm_mpls_esi_info_t esi_info;

        bcm_mpls_esi_info_t_init(&esi_info);
        esi_info.esi_label = cint_evpn_info.pe1_pe2_ce1_esi_label;
        esi_info.out_class_id = dual_homed_esi_peer_profile;
        esi_info.src_port = cint_evpn_info.ce1_sys_port;
        rv = bcm_mpls_esi_encap_add(egress_unit, &esi_info);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_mpls_esi_encap_add esi-300\n");
            return rv;
        }

        esi_info.esi_label = cint_evpn_info.pe1_pe2_ce3_esi_label;
        esi_info.out_class_id = dual_homed_esi_peer_profile - 1;
        esi_info.src_port = cint_evpn_info.ce3_sys_port;
        rv = bcm_mpls_esi_encap_add(egress_unit, &esi_info);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_mpls_esi_encap_add esi-300\n");
            return rv;
        }

        printf("- Set dual homed ESI peer profile on IML label lif\n");
        bcm_gport_t lif_gport;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(lif_gport, cint_evpn_info.pe1_pe2_iml_esi_tunnel_id);
        rv = bcm_port_class_set(egress_unit, lif_gport, bcmPortClassEgress, dual_homed_esi_peer_profile);
        if (rv != BCM_E_NONE)
        {
            printf("unit: %d, ERROR: %d, in bcm_port_class_set for dual homed peer id (%d) of lif (0x%08x)\n",
                   egress_unit, rv, dual_homed_esi_peer_profile, lif_gport);
            return rv;
        }
    }

    /*In Jericho2, FRR label should be encapsulated by general encap stages.*/
    if (evpn_add_frr_label && !is_device_or_above(egress_unit, JERICHO2)) {
        rv = evpn_enable_frr_labels_addition(egress_unit); /* 3.7, 4.9, 7.9, 8.10 */
        if (rv != BCM_E_NONE) {
            printf("unit: %d, Error: %d, in evpn_enable_frr_labels_addition with \n", egress_unit, rv);
            return rv;
        }
    }


    return rv;
}

/* Create ingress tunnels (for termination) for egress pe bum traffic */
int evpn_create_ingress_tunnels(int unit){

    int rv = BCM_E_NONE;
    int flags = 0 ;
    bcm_mpls_range_action_t action;
    bcm_gport_t dummy_tunnel_id;
    int ingress_label = 0;
    bcm_mpls_label_t indexed_ingress_label = 0;
    int evpn_mpls_index_label = 0;
    int mpls_term_1_or_2_labels = !is_device_or_above(unit, JERICHO2) ? 0 : *dnxc_data_get(unit, "mpls", "general", "mpls_term_1_or_2_labels", NULL);

    evpn_is_indexed_termination = is_device_or_above(unit, JERICHO2) ? 0 : soc_property_get(unit , "mpls_termination_label_index_enable",0);

    /* Setting IML range*/
    action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    rv = bcm_mpls_range_action_add(unit,cint_evpn_info.iml_range_low_label, cint_evpn_info.iml_range_high_label,action); /* 6.7 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_mpls_range_action_add\n");
        return rv;
    }

    if (!no_lsp) {
        /* Set LSP ltermination label of pe3-->pe1 */
        /* If this is indexed termination mode, indicate the index for the label */
        if (evpn_is_indexed_termination) {
            evpn_mpls_index_label = 1;
        }
        ingress_label = cint_evpn_info.pe1_pe3_lsp_label;
        BCM_MPLS_INDEXED_LABEL_SET(indexed_ingress_label, ingress_label, evpn_mpls_index_label);

        rv = evpn_set_ingress_tunnel(unit, 1, flags, indexed_ingress_label, 0, \ /* 5.3, 6.8 */
                                     BCM_MPLS_SWITCH_ACTION_POP, 0, &dummy_tunnel_id, &cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id_2);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in evpn_set_ingress_tunnel\n");
            return rv;
        }
    }

    /* If this is indexed termination mode, indicate the index for the label */
    if (evpn_is_indexed_termination) {
        if (no_lsp) {
            evpn_mpls_index_label = 1;
        } else {
            evpn_mpls_index_label = 2;
        }
    }
    ingress_label = cint_evpn_info.pe1_pe3_iml_label;
    BCM_MPLS_INDEXED_LABEL_SET(indexed_ingress_label, ingress_label, evpn_mpls_index_label);
    /* Set IML termination label of pe3-->pe1 (no esi expected, indicated by BCM_MPLS_SWITCH_EXPECT_BOS) */
    flags = BCM_MPLS_SWITCH_EVPN_IML|BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    flags |= mpls_term_1_or_2_labels ? 0 : BCM_MPLS_SWITCH_EXPECT_BOS;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, indexed_ingress_label, 0, \ /* 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0, &dummy_tunnel_id, &cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id_2);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    if (!no_lsp) {
        /* If this is indexed termination mode, indicate the index for the label */
        if (evpn_is_indexed_termination) {
            evpn_mpls_index_label = 1;
        }
        ingress_label = cint_evpn_info.pe1_pe2_lsp_label;
        BCM_MPLS_INDEXED_LABEL_SET(indexed_ingress_label, ingress_label, evpn_mpls_index_label);
        /* Set LSP termination label of pe2-->pe1 */
        flags = 0;
        rv = evpn_set_ingress_tunnel(unit, 1, flags, indexed_ingress_label, 0, \ /* 6.8 */
                                     BCM_MPLS_SWITCH_ACTION_POP, 0, &dummy_tunnel_id, &cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id_2);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in evpn_set_ingress_tunnel\n");
            return rv;
        }
    }

    /* If this is indexed termination mode, indicate the index for the label */
    if (evpn_is_indexed_termination) {
        evpn_mpls_index_label = 2;
    }
    ingress_label = cint_evpn_info.pe1_pe2_iml_label;
    BCM_MPLS_INDEXED_LABEL_SET(indexed_ingress_label, ingress_label, evpn_mpls_index_label);
    /* Set IML termination label of pe2-->pe1 (esi is expected, indicated by the absence of BCM_MPLS_SWITCH_EXPECT_BOS) */
    flags = BCM_MPLS_SWITCH_EVPN_IML|BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, indexed_ingress_label, 0, \ /* 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0, &cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id, &cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id_2);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    /* Set PMF inLif profile*/
    bcm_port_class_t pclass = is_device_or_above(unit, JERICHO2) ? bcmPortClassFieldIngressVport : bcmPortClassFieldIngress;
    if (evpn_mpls_index_label == 2 && no_lsp == 0) {
        rv = bcm_port_class_set(unit,cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id_2, pclass, cint_evpn_info.evpn_profile);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_port_class_set \n");
            return rv;
        }      
    } else {
        rv = bcm_port_class_set(unit,cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id, pclass, cint_evpn_info.evpn_profile);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_port_class_set \n");
            return rv;
        }
    }

    /* If this is indexed termination mode, indicate the index for the label */
    if (evpn_is_indexed_termination) {
        if (no_lsp) {
            evpn_mpls_index_label = 1;
        } else {
            evpn_mpls_index_label = 2;
        }
    }
    ingress_label = cint_evpn_info.evpn_label;
    BCM_MPLS_INDEXED_LABEL_SET(indexed_ingress_label, ingress_label, evpn_mpls_index_label);
    /* Create ingress tunnel : EVPN over LSP (to be terminated) */
    flags = BCM_MPLS_SWITCH_NEXT_HEADER_L2; /* Set for this inlif ethernet as next protocol */
    rv = evpn_set_ingress_tunnel(unit, 1, flags, indexed_ingress_label, \ /* 5.3 */
                           0,BCM_MPLS_SWITCH_ACTION_POP, BCM_IF_INVALID, &dummy_tunnel_id, &cint_evpn_info.pe1_pe2_iml_label_term_tunnel_id_2);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    }

    return rv;
}

/* Create attachment circuits for this application*/
int multi_dev_evpn_create_l2_interfaces(int ingress_unit, int egress_unit)
{
    int rv = BCM_E_NONE;
    uint64 data;
    int add_to_flooding_domain = 0;
    uint32 flags;

    COMPILER_64_ZERO(data);

    /*
     * Ingress Device configurations
     */
    if (is_device_or_above(ingress_unit, JERICHO2)) {
        /** JR2 use port.pmf_data carry ES+PE_ID rather than LIF.wide_data*/
        flags = 0;
    } else {
        flags = BCM_VLAN_PORT_INGRESS_WIDE;
    }
    rv = evpn_create_ac(ingress_unit, flags, \ /* 2.1, 3.1, 4.1, 7.1, 8.1 */
                        cint_evpn_info.ce1_vlan, cint_evpn_info.vsi,cint_evpn_info.ce1_sys_port, \
                        &(cint_evpn_info.ce1_ing_vport_id), &(cint_evpn_info.ce1_egr_vport_id),
                        cint_evpn_info.ce1_mac, 0);
    if (rv != BCM_E_NONE) {
        printf("Error: %d, in evpn_create_ac with ce1\n", rv);
        return rv;
    }

    /* data == esi label + PE id and shift by an mpls label size */
    uint32 pe_id_esi_hi = (cint_evpn_info.pe2_id >> 12);
    uint32 pe_id_esi_lo = ((cint_evpn_info.pe2_id & 0xFFF) << 20) | cint_evpn_info.pe1_pe2_ce1_esi_label;
    COMPILER_64_SET(data, pe_id_esi_hi, pe_id_esi_lo);
    if (!is_device_or_above(ingress_unit, JERICHO2)) {
        /* Set additional info: PE id + esi*/
        rv = bcm_port_wide_data_set(ingress_unit, cint_evpn_info.ce1_ing_vport_id, BCM_PORT_WIDE_DATA_INGRESS, data); /* 4.2 */
        if (rv != BCM_E_NONE) {
            printf("unit: %d, Error: %d, in bcm_port_wide_data_set\n", ingress_unit, rv);
            return rv;
        }
    } else {
        /** In JR2, ESI label + PE-ID is configured in PORT.KEY_GEN_VAR(40/64)*/
        /* Set the general data.*/
        bcm_gport_t ce1_gport;
        BCM_GPORT_LOCAL_SET(ce1_gport, cint_evpn_info.ce1_port);
        rv = bcm_port_class_set(ingress_unit, ce1_gport, bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData, COMPILER_64_LO(data));
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_port_class_set bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData\n", rv);
            return rv;
        }
        rv = bcm_port_class_set(ingress_unit, ce1_gport, bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh, COMPILER_64_HI(data));
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_port_class_set bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh\n", rv);
            return rv;
        }
    }

    /* Set PMF inLif profile*/
    bcm_port_class_t pclass = is_device_or_above(ingress_unit, JERICHO2) ? bcmPortClassFieldIngressVport : bcmPortClassFieldIngress;
    rv = bcm_port_class_set(ingress_unit, cint_evpn_info.ce1_ing_vport_id, pclass, cint_evpn_info.evpn_profile); /* 4.3, 8.3 */
    if (rv != BCM_E_NONE) {
        printf("unit: %d, Error: %d, in bcm_port_class_set\n", ingress_unit, rv);
        return rv;
    }

    add_to_flooding_domain = 1;
    rv = evpn_create_ac(ingress_unit, 0, cint_evpn_info.ce3_vlan, \ /* 2.2, 4.4, 5.1, 6.1, 8.2 */
                        cint_evpn_info.vsi,cint_evpn_info.ce3_sys_port, &(cint_evpn_info.ce3_ing_vport_id),
                        &(cint_evpn_info.ce3_egr_vport_id), cint_evpn_info.ce3_mac, add_to_flooding_domain);
    if (rv != BCM_E_NONE) {
        printf("unit: %d, Error: %d, in evpn_create_and_statically_learn_ac with ce1\n", ingress_unit, rv);
        return rv;
    }

    /*
     * Egress Device configurations
     */
    rv = evpn_create_ac(egress_unit, 0, cint_evpn_info.p1_vlan,  \/* 5.2, 6.2 */
                        cint_evpn_info.vsi ,cint_evpn_info.p1_sys_port, &(cint_evpn_info.p1_ing_vport_id),
                        &(cint_evpn_info.p1_egr_vport_id),cint_evpn_info.p1_mac, 0);
    if (rv != BCM_E_NONE) {
        printf("unit: %d, Error: %d, in evpn_create_ac with ce1\n", egress_unit, rv);
        return rv;
    }

    rv = evpn_create_ac(egress_unit, 0, cint_evpn_info.p2_vlan, \/*  6.2 */
                        cint_evpn_info.vsi ,cint_evpn_info.p2_sys_port, &(cint_evpn_info.p2_ing_vport_id),
                        &(cint_evpn_info.p2_egr_vport_id),cint_evpn_info.p2_mac, 0);
    if (rv != BCM_E_NONE) {
        printf("unit: %d, Error: %d, in evpn_create_ac with ce1\n", egress_unit, rv);
        return rv;
    }

    if (!evpn_no_inner_ive) {
        rv = multi_dev_evpn_create_ive(ingress_unit, egress_unit);
        if (rv != BCM_E_NONE) {
            printf("Error: %d, in evpn_create_ac with evpn_create_ive\n", rv);
            return rv;
        }

        if (is_device_or_above(egress_unit, JERICHO_PLUS)) {
            rv = vlan__native_default_out_ac_allocate_and_set_dflt_action(egress_unit, 100, 0, -1);
            if (rv != BCM_E_NONE) {
                printf("unit:%d, Error: %d, in evpn_create_ac with vlan__native_default_out_ac_allocate_and_set_dflt_action\n", ingress_unit, rv);
                return rv;
            }
        } else {
            rv = vlan__default_ve_profile_set(egress_unit, 1);
            if (rv != BCM_E_NONE) {
                printf("unit:%d, Error: %d, in evpn_create_ac with vlan__default_ve_profile_set\n", ingress_unit, rv);
                return rv;
            }
        }
    }

    return rv;
}

/* Configure routing into evpn for unicast and multicast traffic */
int multi_dev_evpn_routing_into_evpn(int ingress_unit, int egress_unit)
{
    int rv = BCM_E_NONE;
    l3_ipv4_route_entry_utils_s route_entry;
    int cuds[4];
    int ports[4];
    int nof_ports = 0;
    int is_egress = 0;
    multicast_forwarding_entry_dip_sip_s forwarding_entry;
    bcm_l3_intf_t intf;

    /* Configure unicast routing into EVPN :
       1. Create Native FEC, set it to point to overlay FEC pointing to EVPN over lsp.
       2. Create Routing entry pointing to the native FEC.
    */

    /* 1) Create Native FEC */
    rv = multi_dev_evpn_create_native_arp_fec_entry(ingress_unit, egress_unit); /* 7.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, in multi_dev_evpn_create_native_arp_fec_entry\n");
        return rv;
    }

    /* 2) Create Routing entry: Add LPM entry for access to EVPN core traffic */
    sal_memset(&route_entry, 0, sizeof(route_entry));
    route_entry.address =  cint_evpn_info.ce2_ip;
    route_entry.mask = 0xffffffff;
    rv = l3__ipv4_route__add(ingress_unit, 
                             route_entry,
                             0,     /* router instance */
                             cint_evpn_info.pe1_ce2_native_fec/* data of LPM entry:  fec */        
                             );  /* 7.8 */
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__ipv4_route__add\n");
        return rv;
    }

    /* Configure multicast routing into EVPN :
       1) Open multicast group and add members to the group (Same as flooding members in ingress bum traffic).
       2) Add forwarding entry for the multicast destination ip. Entry wil point to the created multicast group.
    */

    bcm_l3_intf_t_init(&intf);
    intf.l3a_vid = cint_evpn_info.vsi;
    intf.l3a_mac_addr = cint_evpn_info.my_mac;
    
    /* 1) Open multicast group and add members to the group (each copy contains outrif and outlif) */
    rv = multi_dev_evpn_add_routing_into_evpn_multicast_entry(ingress_unit, egress_unit); /* 8.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, in multi_dev_evpn_add_routing_into_evpn_multicast_entry\n");
        print cint_evpn_info;
        return rv;
    }

    /** Open the same multicast group in ingress device in multi-dev case.*/
    if (ingress_unit != egress_unit) {
        int mc_id = cint_evpn_info.mc_id & 0xffffff;
        rv = bcm_multicast_create(ingress_unit, BCM_MULTICAST_WITH_ID|BCM_MULTICAST_EGRESS_GROUP|
                                 (is_device_or_above(ingress_unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3), &mc_id); /* 8.7 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create\n");
            return rv;
        }
    }

    /* 2) Add forwarding entry for the multicast destination ip. */
    forwarding_entry.src_ip = -1;
    forwarding_entry.mc_ip = cint_evpn_info.mc_ip;
    rv = multicast__create_forwarding_entry_dip_sip(ingress_unit, &forwarding_entry, cint_evpn_info.mc_id, /* inRif*/0,0); /* 8.8 */
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__create_forwarding_entry_dip_sip\n");
        print cint_evpn_info;
        return rv;
    }

    return rv;
}

/* Unset iml_bos_indication (bit 25), if it is set */
void evpn_unset_iml_bos_indication(int *tunnel_id){

    /* unset bit 25 */
    int iml_bos_indication_unset_mask = 0xfdffffff;
    if (*tunnel_id & evpn_iml_bos_indication) {
        *tunnel_id &= 0xfdffffff;
    }

    return;
}

/* Configure properties for ports of the application */
int evpn_configure_port_properties(int unit, int *port_array, int nof_ports){

    int rv = BCM_E_NONE;
    int i;
    bcm_pbmp_t pbmp;

    for (i=0; i< nof_ports; i++) {
        /* set VLAN domain to each port */
        rv = port__vlan_domain__set(unit, port_array[i], port_array[i]); /* 1.3 */
        if (rv != BCM_E_NONE) {
            printf("uint: %d, Error: %d, in port__vlan_domain__set, port=%d, \n", unit, rv, port_array[i]);
            return rv;
        }

        /* Set outer and inner tpid */
        if (!is_device_or_above(unit, JERICHO2))
        {
            /** For JR2, we use the default TPID configurations*/
            rv = port__tpid__set(unit, port_array[i], cint_evpn_info.tpid, cint_evpn_info.tpid); /* 1.3 */
            if (rv != BCM_E_NONE) {
                printf("uint: %d, Error: %d, in port__tpid__set, port=%d, \n", unit, rv, port_array[i]);
                return rv;
            }
        } else {
            bcm_port_tpid_class_t tpid_class;
            bcm_port_tpid_class_t_init(&tpid_class);
            tpid_class.tpid1 = cint_evpn_info.tpid;
            tpid_class.tpid2 = cint_evpn_info.tpid;
            tpid_class.port = port_array[i];
            tpid_class.tag_format_class_id = 16;
            rv = bcm_port_tpid_class_set(unit, &tpid_class);
            if (rv != BCM_E_NONE) {
                printf("uint: %d, Error: %d, in bcm_port_tpid_class_set, port=%d, \n", unit, rv, port_array[i]);
                return rv;
            }

            tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
            tpid_class.port = port_array[i];
            tpid_class.tag_format_class_id = 4;
            rv = bcm_port_tpid_class_set(unit, &tpid_class);
            if (rv != BCM_E_NONE) {
                printf("uint: %d, Error: %d, in bcm_port_tpid_class_set, port=%d, \n", unit, rv, port_array[i]);
                return rv;
            }
        }

        rv = bcm_vlan_gport_add(unit, cint_evpn_info.vsi, port_array[i], BCM_VLAN_GPORT_ADD_INGRESS_ONLY); /* 1.3 */
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", port_array[i], cint_evpn_info.vsi);
            return rv;
        }
    }

    return rv;
}

/* Configure properties for ports of the application */
int multi_dev_evpn_configure_port_properties(
    int ingress_unit,
    int egress_unit,
    int *ingress_port_array,
    int *egress_port_array,
    int nof_ingress_ports,
    int nof_egress_ports)
{

    int rv = BCM_E_NONE;
    int i;
    bcm_pbmp_t pbmp;

    rv = evpn_configure_port_properties(ingress_unit, ingress_port_array, nof_ingress_ports); /* 1.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, in evpn_configure_port_properties for ingress port\n");
        return rv;
    }

    rv = evpn_configure_port_properties(egress_unit, egress_port_array, nof_egress_ports); /* 1.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, in evpn_configure_port_properties for egress port\n");
        return rv;
    }

    if (ingress_unit != egress_unit) {
        rv = evpn_configure_port_properties(ingress_unit, egress_port_array, nof_egress_ports); /* 1.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, in evpn_configure_port_properties for egress port\n");
            return rv;
        }
    }

    return rv;
}

/* Configure linker layer entries for application: these entries are used
   for both bridging and routing into evpn */
int evpn_create_linker_layer_entries(int unit){

    int rv = BCM_E_NONE;
    int encap_id;
    bcm_l2_egress_t arp_entry;
    uint64 wide_data;
    int gport = 0;
    int flags2 = 0;
    bcm_mpls_egress_label_t label_array;


    bcm_l2_egress_t_init(&arp_entry);

    /** JR2 use the ARP format that support VE, since no esem access can be used for outer AC.*/
    flags2 = is_device_or_above(unit, JERICHO2) ? BCM_L3_FLAGS2_VLAN_TRANSLATION : 0;

    /* Create linker layer for tunnels starting at pe1 */
    arp_entry.dest_mac   = cint_evpn_info.p1_mac;  /* next hop */
    arp_entry.src_mac    = cint_evpn_info.my_mac;  /* my-mac */
    arp_entry.outer_vlan = cint_evpn_info.p1_vlan; /* For qax and above, this will signify vsi in bcm_l3_egress_create */
    arp_entry.ethertype  = 0x8847;       /* ethertype for IP */
    if (evpn_nof_linker_layer_tags > 0) {
        arp_entry.outer_tpid = cint_evpn_info.tpid; /* Signifies at least one vlan tag. No tpid means untagged packet */
        if (evpn_nof_linker_layer_tags == 2) {
            arp_entry.inner_vlan = cint_evpn_info.p1_inner_vlan; /* egress inner vlan  */
            arp_entry.inner_tpid = cint_evpn_info.tpid;

            /** JR2 use ARP_AC format to support 2 tags.*/
            flags2 |= is_device_or_above(unit, JERICHO2) ? BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS : 0;
        }
    }

    if (evpn_add_frr_label && !is_device_or_above(unit,JERICHO2)) {
        /** JR2 use the general encap stages for FRR label encapsulation.*/
        arp_entry.mpls_extended_label_value = cint_evpn_info.pe1_frr_label; /* 3.7, 4.9, 7.9, 8.10 */
        if (is_device_or_above(unit,JERICHO_PLUS)) {
            flags2 |= BCM_L3_FLAGS2_EGRESS_WIDE;
        }
    }

    rv = roo__overlay_link_layer_create(unit, &arp_entry, 0, &encap_id, flags2);
    if (rv != BCM_E_NONE) {
        printf("Error, in l2__egress_create\n");
        return rv;
    }
    cint_evpn_info.pe1_linker_layer_encap_id = encap_id;

    if (is_device_or_above(unit,JERICHO_PLUS) && evpn_add_frr_label) { /* 3.7, 4.9, 7.9, 8.10 */
        if (!is_device_or_above(unit,JERICHO2)) {
            BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, cint_evpn_info.pe1_linker_layer_encap_id);
            /* Fill wide entry and configure outlif as having additional label */
            COMPILER_64_SET(wide_data, 0, cint_evpn_info.pe1_frr_label);
            rv = bcm_port_wide_data_set(unit, gport, BCM_PORT_WIDE_DATA_EGRESS/*flags*/, wide_data);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_wide_data_set\n");
                return rv;
            }
        } else {
            /* JR2, create FRR label encapsolation with tunnel_initiate.*/
            bcm_mpls_egress_label_t_init(&label_array);

            label_array.label = cint_evpn_info.pe1_frr_label;
            label_array.l3_intf_id = cint_evpn_info.pe1_linker_layer_encap_id;
            label_array.ttl = 255;
            label_array.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            label_array.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            if (mpls_pipe_mode_exp_set) {
                label_array.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
                label_array.exp = ext_mpls_exp;
            } else {
                label_array.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            }
            label_array.encap_access = bcmEncapAccessTunnel3;
            rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label_array);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), in bcm_mpls_tunnel_initiator_create\n", rv);
                return rv;
            }

            cint_evpn_info.pe1_frr_tunnel_id = label_array.tunnel_id;
        }
    }

    encap_id = 0;

    /* Create linker layer for the tunnel starting at pe2 */

    bcm_l2_egress_t_init(&arp_entry);

    /** JR2 use the ARP format support VE, since no esem access can be used for outer AC.*/
    flags2 = is_device_or_above(unit, JERICHO2) ? BCM_L3_FLAGS2_VLAN_TRANSLATION : 0;

    /* Create linker layer for tunnels starting at pe2 */
    arp_entry.dest_mac   = cint_evpn_info.p2_mac;  /* next hop */
    arp_entry.src_mac    = cint_evpn_info.my_mac;  /* my-mac */
    arp_entry.outer_vlan = cint_evpn_info.p2_vlan;
    arp_entry.ethertype  = 0x8847;       /* ethertype for IP */
    if (evpn_nof_linker_layer_tags > 0) {
        arp_entry.outer_tpid = cint_evpn_info.tpid;
        if (evpn_nof_linker_layer_tags == 2) {
            arp_entry.inner_vlan = cint_evpn_info.p2_inner_vlan; /* egress inner vlan  */
            arp_entry.inner_tpid = cint_evpn_info.tpid;

            /** JR2 use ARP_AC format to support 2 tags.*/
            flags2 |= is_device_or_above(unit, JERICHO2) ? BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS : 0;
        }
    }

    if (evpn_add_frr_label && !is_device_or_above(unit,JERICHO2)) {
        arp_entry.mpls_extended_label_value = cint_evpn_info.pe2_frr_label; /* 3.7, 4.9, 7.9, 8.10 */
        if (is_device_or_above(unit,JERICHO_PLUS) && !is_device_or_above(unit,JERICHO2)) {
            flags2 |= BCM_L3_FLAGS2_EGRESS_WIDE;
        }
    }

    rv = roo__overlay_link_layer_create(unit, &arp_entry, 0, &encap_id, flags2);
    if (rv != BCM_E_NONE) {
        printf("Error, in l2__egress_create\n");
        return rv;
    }

    cint_evpn_info.pe2_linker_layer_encap_id = encap_id;

    if (is_device_or_above(unit,JERICHO_PLUS) && evpn_add_frr_label) { /* 3.7, 4.9, 7.9, 8.10 */
        if (!is_device_or_above(unit,JERICHO2)) {
            BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, cint_evpn_info.pe2_linker_layer_encap_id);
            /* Fill wide entry and configure outlif as having additional label */
            COMPILER_64_SET(wide_data, 0, cint_evpn_info.pe2_frr_label);
            rv = bcm_port_wide_data_set(unit, gport, BCM_PORT_WIDE_DATA_EGRESS/*flags*/, wide_data);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_wide_data_set\n");
                return rv;
            }
        } else {
            /* JR2, create FRR label encapsolation with tunnel_initiate.*/
            bcm_mpls_egress_label_t_init(&label_array);

            label_array.label = cint_evpn_info.pe2_frr_label;
            label_array.l3_intf_id = cint_evpn_info.pe2_linker_layer_encap_id;
            label_array.ttl = 255;
            label_array.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            label_array.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            if (mpls_pipe_mode_exp_set) {
                label_array.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
                label_array.exp = ext_mpls_exp;
            } else {
                label_array.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            }
            label_array.encap_access = bcmEncapAccessTunnel3;
            rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label_array);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), in bcm_mpls_tunnel_initiator_create\n", rv);
                return rv;
            }

            cint_evpn_info.pe2_frr_tunnel_id = label_array.tunnel_id;
        }
    }

    return rv;
}


/* Create a FEC entry pointing to  an egress evpn entry. */
int evpn_create_overlay_fec_entry(int unit){

    int rv = BCM_E_NONE;

    create_l3_egress_s l3_egress;

    l3_egress.allocation_flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_L3_CASCADED;
    l3_egress.fec_id = 0;
    l3_egress.l3_flags = is_device_or_above(unit, JERICHO2) ? BCM_L3_2ND_HIERARCHY : 0;
    l3_egress.arp_encap_id = 0;
    l3_egress.out_gport = cint_evpn_info.p1_sys_port;
    l3_egress.vlan = cint_evpn_info.p1_vlan;
    l3_egress.out_tunnel_or_rif = cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id;
    rv = l3__egress_only_fec__create(unit, &l3_egress);/* 3.4, 7.6 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__egress_only_fec__create\n");
        return rv;
    }

    cint_evpn_info.pe1_pe3_evpn_lsp_fec = l3_egress.fec_id;;

    return rv;

}

/* Create a native fec entry with native ARP for unicast routing into evpn */
int multi_dev_evpn_create_native_arp_fec_entry(int ingress_unit, int egress_unit)
{

    int rv = BCM_E_NONE;
    bcm_failover_t failover_id_fec;
    create_l3_egress_s l3_egress_into_overlay;
    int gport_forward_fec = 0, arp_encap_id;
    l3_egress_s l3_egress;

    /*
     * Create native ARP, which will be carried by native FEC.
     */
    sand_utils_l3_egress_s_init(egress_unit, 0, &l3_egress);
    if (is_device_or_above(egress_unit, JERICHO_PLUS)) {
        l3_egress.l3_flags = BCM_L3_NATIVE_ENCAP;
    } else {
        l3_egress.l3_flags = 0;  /* flags  */
    }
    l3_egress.vlan = cint_evpn_info.native_vlan; /* out-vlan */
    l3_egress.next_hop_mac_addr = cint_evpn_info.ce2_mac; /* next-hop */
    l3_egress.failover_if_id = 0;
    l3_egress.allocation_flags = BCM_L3_EGRESS_ONLY;
    rv = sand_utils_l3_egress_create(egress_unit, &l3_egress); /* 7.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, sand_utils_l3_egress_create\n");
        return rv;
    }

    arp_encap_id = l3_egress.arp_encap_id;

    /* Hierarchical FEC does not work when the 1st FEC hierarchy is unprotected.
     * In case we are using hierarchical FEC, the first FEC must be protected.
     * This was fixed in Jericho B0.
     */
    if (!is_device_or_above(ingress_unit,JERICHO_B0)) {
        /* create failover id*/
        rv = bcm_failover_create(ingress_unit, BCM_FAILOVER_FEC, &failover_id_fec);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_failover_create 1\n");
            return rv;
        }
    }

    BCM_GPORT_FORWARD_PORT_SET(gport_forward_fec, cint_evpn_info.pe1_pe3_evpn_lsp_fec);

    /* primary FEC */
    sand_utils_l3_egress_s_init(ingress_unit, 0, &l3_egress);
    l3_egress.out_gport = gport_forward_fec;     /* dest: fec-id instead of port */
    l3_egress.vlan = cint_evpn_info.native_vlan; /* out-vlan */
    l3_egress.out_tunnel_or_rif = cint_evpn_info.pe1_rif2; /* l3_intf  */
    l3_egress.next_hop_mac_addr = cint_evpn_info.ce2_mac; /* next-hop */
    l3_egress.failover_id = failover_id_fec;
    l3_egress.failover_if_id = 0;
    l3_egress.arp_encap_id = arp_encap_id;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;

    if (!is_device_or_above(ingress_unit, JERICHO_B0)) {
        /* Create protected FEC */
        rv = sand_utils_l3_egress_create(ingress_unit, &l3_egress); /* 7.7 */
        if (rv != BCM_E_NONE) {
            printf("Error, sand_utils_l3_egress_create\n");
            return rv;
        }

        l3_egress.failover_if_id = l3_egress.fec_id;
    }

    /* primary FEC */
    rv = sand_utils_l3_egress_create(ingress_unit, &l3_egress); /* 7.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, sand_utils_l3_egress_create\n");
        return rv;
    }

    cint_evpn_info.pe1_ce2_native_fec = l3_egress.fec_id;

    return rv;
}


/* Enable addition of frr labels on top of the egress tunnels configured in this application */
int evpn_enable_frr_labels_addition(int unit){

    int rv = BCM_E_NONE;
    int tunnel_id;
    int arp_id;

    /* 
     * In Jericho, the port control set is done for the MPLS tunnel id
     * In Jerihco Plus, the port control set is done for the LL layer that is attached to the MPLS
     */
    if (is_device_or_above(unit, JERICHO_PLUS)) {
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(arp_id, cint_evpn_info.pe1_linker_layer_encap_id);
        /* Enabling addition of frr label via pe1's link layer*/
        rv = bcm_port_control_set(unit, arp_id, bcmPortControlMPLSEncapsulateAdditionalLabel,1); /* 4.9, 8.10 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set\n");
            return rv;
        }

        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(arp_id, cint_evpn_info.pe2_linker_layer_encap_id);
        /* Enabling addition of frr label via pe2's link layer*/
        rv = bcm_port_control_set(unit, arp_id, bcmPortControlMPLSEncapsulateAdditionalLabel,1); /* 4.9, 8.10 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set\n");
            return rv;
        }

    } else {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_id,cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id);

        /* Enabling addition of frr label on top of pe1_pe3_iml_no_esi tunnel*/
        rv = bcm_port_control_set(unit, tunnel_id, bcmPortControlMplsEncapsulateExtendedLabel,1); /* 4.9, 8.10 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set\n");
            return rv;
        }

        BCM_GPORT_TUNNEL_ID_SET(tunnel_id,cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id);
        /* Enabling addition of frr label on top of pe1_pe4_iml_no_esi tunnel*/
        rv = bcm_port_control_set(unit, tunnel_id, bcmPortControlMplsEncapsulateExtendedLabel,1); /* 4.9, 8.10 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set\n");
            return rv;
        }

        BCM_GPORT_TUNNEL_ID_SET(tunnel_id,cint_evpn_info.pe1_pe2_iml_esi_tunnel_id);
        /* Enabling addition of frr label on top of pe1_pe2_iml_esi tunnel*/
        rv = bcm_port_control_set(unit, tunnel_id, bcmPortControlMplsEncapsulateExtendedLabel,1); /* 4.9, 8.10 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set\n");
            return rv;
        }

        BCM_GPORT_TUNNEL_ID_SET(tunnel_id,cint_evpn_info.pe1_pe3_evpn_lsp_tunnel_id);
        /* Enabling addition of frr label on top of pe1_pe3_evpn_lsp tunnel*/
        rv = bcm_port_control_set(unit, tunnel_id, bcmPortControlMplsEncapsulateExtendedLabel,1); /* 3.7, 7.9 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set\n");
            return rv;
        }
    }

    return rv;
}

/*
 * Get egress replication ports per the egress mc member ports.
 * ( Only when there are egress mc members in its corresponding core, will the ERP port be returned.
 *   This is on model consideration to avoid two ERP copies being sent to the same core.)
 */
int sand_erp_port_get(int unit, int nof_erp_ports, int nof_member_ports, int *member_port, int *erp_sysport, int *erp_count)
{
    int rv = BCM_E_NONE, count_erp = 0, port_id;
    bcm_gport_t erp_local_port_gport[2], erp_sysport_gport;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint8 core_0_valid = 0, core_1_valid = 0;

    /** Check if the members exist in different cores.*/
    for (port_id = 0; port_id < nof_member_ports; port_id++) {
        bcm_port_mapping_info_t_init(mapping_info);
        rv = bcm_port_get(unit, member_port[port_id], &flags, &interface_info,&mapping_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_get(), rv=%d, erp_count=%d, erp_local_port=0x%x.\n", rv, count_erp, erp_local_port_gport[port_id]);
            return rv;
        }

        core_0_valid = (mapping_info.core == 0) ? TRUE : core_0_valid;
        core_1_valid = (mapping_info.core == 1) ? TRUE : core_1_valid;
    }

    /** Get ERP ports per request*/
    rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, nof_erp_ports, &erp_local_port_gport, &count_erp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_internal_get(), rv=%d.\n", rv);
        return rv;
    }

    /** Return the ERP port only if there are members on its corresponding core.*/
    *erp_count = 0;
    for (port_id = 0; port_id < count_erp; port_id++) {
        bcm_port_mapping_info_t_init(mapping_info);
        rv = bcm_port_get(unit, erp_local_port_gport[port_id], &flags, &interface_info,&mapping_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_get(), rv=%d, erp_count=%d, erp_local_port=0x%x.\n", rv, count_erp, erp_local_port_gport[port_id]);
            return rv;
        }

        if (((mapping_info.core == 0) && (core_0_valid)) ||
            ((mapping_info.core == 1) && (core_1_valid))) {
            rv = bcm_stk_gport_sysport_get(unit, erp_local_port_gport[port_id], &erp_sysport_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_stk_gport_sysport_get(), rv=%d, count_erp=%d, erp_local_port_gport=0x%x.\n", rv, count_erp, erp_local_port_gport[port_id]);
                return rv;
            }
            erp_sysport[(*erp_count)++] = erp_sysport_gport;

            printf("count_erp=%d, erp_local_port_gport=0x%x, erp_sysport[%d]=0x%x, core = %d.\n",
                   count_erp, erp_local_port_gport[port_id], port_id, erp_sysport[port_id], mapping_info.core);
        }
    }

    return rv;
}

/*
 * Open multicast group and add members to the group (each copy contains outrif and outlif)
 * Ingress + Egress MC is used.
 */
int multi_dev_evpn_add_routing_into_evpn_multicast_entry(int ingress_unit, int egress_unit)
{
    int rv = BCM_E_NONE;
    bcm_multicast_replication_t reps[3];
    int global_lif_mask = 0x3ffff;
    uint32 flags = 0;

    int egr_mc_id;

    /* manually change the mc entries. 1st Mc entry with OUTRIF and link it to the 2nd mc entry with outlif */
    bcm_multicast_replication_t_init(reps);

    /* Open the egress multicast group */
    flags = BCM_MULTICAST_WITH_ID|BCM_MULTICAST_EGRESS_GROUP|(is_device_or_above(egress_unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3);
    egr_mc_id = cint_evpn_info.mc_id;
    rv = bcm_multicast_create(egress_unit, flags, &egr_mc_id); /* 8.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create egress mc id 0x%08X\n", egr_mc_id);
        return rv;
    }

    /** Add remote copies to egress MC*/
    if (!is_device_or_above(egress_unit, JERICHO2)) {
        BCM_GPORT_LOCAL_SET(reps[0].port, cint_evpn_info.p1_port);
        reps[0].encap1 = cint_evpn_info.pe1_rif2; /* OutRif */
        reps[0].encap2 = cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id & global_lif_mask; /* OutLif */
        reps[0].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;    

        BCM_GPORT_LOCAL_SET(reps[1].port, cint_evpn_info.p1_port);
        reps[1].encap1 = cint_evpn_info.pe1_rif2; /* OutRif */
        reps[1].encap2 = cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id & global_lif_mask; /* OutLif */
        reps[1].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF; 

        /* Create one tunnel copy with it's tunnel id (outlif) and outrif (inRif == outRif for falling back to bridge) */    
        BCM_GPORT_LOCAL_SET(reps[2].port, cint_evpn_info.p2_port);
        reps[2].encap1 = cint_evpn_info.pe1_rif; /* OutRif */
        reps[2].encap2 = cint_evpn_info.pe1_pe2_iml_esi_tunnel_id & global_lif_mask; /* OutLif */
        reps[2].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF; 

        flags = 0;
    } else {
        bcm_if_t cuds[2];
        bcm_if_t rep_index;

        /** 1st copy */
        cuds[0] = cint_evpn_info.pe1_rif2; /* OutRif */
        cuds[1] = cint_evpn_info.pe1_pe3_iml_no_esi_tunnel_id; /* OutLif */
        rv = bcm_multicast_encap_extension_create(egress_unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 1st copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[0].port = cint_evpn_info.p1_port;
        reps[0].encap1 = rep_index;

        /** 2nd copy */
        cuds[0] = cint_evpn_info.pe1_rif2; /* OutRif */
        cuds[1] = cint_evpn_info.pe1_pe4_iml_no_esi_tunnel_id; /* OutLif */
        rv = bcm_multicast_encap_extension_create(egress_unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 2nd copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[1].port = cint_evpn_info.p1_port;
        reps[1].encap1 = rep_index;

        /** 3rd copy */
        cuds[0] = cint_evpn_info.pe1_rif; /* OutRif */
        cuds[1] = cint_evpn_info.pe1_pe2_iml_esi_tunnel_id; /* OutLif */
        rv = bcm_multicast_encap_extension_create(egress_unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
             printf("Error in 3rd copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[2].port = cint_evpn_info.p2_port;
        reps[2].encap1 = rep_index;

        flags = BCM_MULTICAST_EGRESS_GROUP;
    }

    /* Add the copies to the multicast group */
    rv = bcm_multicast_set(egress_unit, egr_mc_id, flags, 3, reps); /* 8.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, unit %d, bcm_multicast_set egress mc group 0x08X\n", egress_unit, egr_mc_id);
        return rv;
    }

    /* Open the ingress multicast group */
    flags = BCM_MULTICAST_WITH_ID|BCM_MULTICAST_INGRESS_GROUP|(is_device_or_above(ingress_unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L3);
    rv = bcm_multicast_create(ingress_unit, flags, &cint_evpn_info.mc_id); /* 8.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create ingress mc id 0x%08X\n", cint_evpn_info.mc_id);
        return rv;
    }

    /** Add local copies to ingress MC*/
    bcm_multicast_replication_t_init(reps);
    if (!is_device_or_above(ingress_unit, JERICHO2)) {
        /* Create AC copy with it's vlan port id (outlif) and outrif (pe1_rif)
           This copy will fack back to bridge, due to inRif == OutRif*/
        BCM_GPORT_LOCAL_SET(reps[2].port, cint_evpn_info.ce3_port);
        reps[2].encap1 = cint_evpn_info.pe1_rif; /* OutRif */
        reps[2].encap2 = cint_evpn_info.ce3_ing_vport_id & global_lif_mask; /* OutLif */  
        reps[2].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF; 

        flags = BCM_MULTICAST_INGRESS;
    } else {
        /** 4th copy */
        reps[2].port =  cint_evpn_info.ce3_port;
        reps[2].encap1 = cint_evpn_info.pe1_rif;

        flags = BCM_MULTICAST_INGRESS_GROUP;
    }

    /** Get egress MC port.*/
    int nof_cores, erp_sys_port[2] = { 0 }, erp_count;
    int egr_mc_member[2] = {cint_evpn_info.p1_port, cint_evpn_info.p2_port};
    int reps_offset, is_qax_qux = 0, is_qmx = 0;

    if (is_device_or_above(egress_unit, JERICHO2)) {
        nof_cores = *dnxc_data_get(egress_unit, "device", "general", "nof_cores", NULL);
    } else {
        print is_qumran_ax(egress_unit, &is_qax_qux);
        print is_qumran_mx_only(egress_unit, &is_qmx);
        nof_cores = (is_qax_qux || is_qmx) ? 1 : 2;
    }

    /*In case egress unit is single core, only one erp port is needed. reps[1] ~ reps[2]*/
    reps_offset = 1;

    rv = sand_erp_port_get(egress_unit, nof_cores, 2, egr_mc_member, erp_sys_port, &erp_count);
    if (rv != BCM_E_NONE) {
        printf("Error, unit %d, sand_erp_port_get nof_core = %d\n", ingress_unit, nof_cores);
        return rv;
    }
    reps[1].port = erp_sys_port[0];
    reps[1].encap1 = is_device_or_above(egress_unit, JERICHO2) ? egr_mc_id : BCM_MULTICAST_L3_GET(egr_mc_id);

    if (erp_count > 1) {
        reps[0].port = erp_sys_port[1];
        reps[0].encap1 = is_device_or_above(egress_unit, JERICHO2) ? egr_mc_id : BCM_MULTICAST_L3_GET(egr_mc_id);

        reps_offset = 0; 
    }

    /* Add the copies to the multicast table */
    rv = bcm_multicast_set(ingress_unit, cint_evpn_info.mc_id, flags, 3-reps_offset, reps+reps_offset); /* 8.7 */
    if (rv != BCM_E_NONE) {
        printf("Error, unit %d, bcm_multicast_set ingress mc group 0x%08X\n", ingress_unit, cint_evpn_info.mc_id);
        return rv;
    }

    return rv;
}


/* Create eve configurations for EVPN:
   1) For untagged (overlay) link layer, action is none, as incoming link layer is terminated, so no need to do anything.
   2) For one-tagged (overlay) link layer, action for outer tag is add, as incoming link layer is terminated, so add one tag
   3) For double-tagged (overlay) link layer, action for outer and inner tags is add, as incoming link layer is terminated, so add two tags */
int evpn_create_eve(int unit){

    int rv = BCM_E_NONE;
    bcm_vlan_t p1_outer_vlan = 0, p1_inner_vlan = 0;
    bcm_vlan_t p2_outer_vlan = 0, p2_inner_vlan = 0;
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    int egr_outer_ac_lif;

    if (is_device_or_above(unit,JERICHO_PLUS)) {
        if (evpn_nof_linker_layer_tags == 0) {
            outer_action = bcmVlanActionNone;
            inner_action = bcmVlanActionNone;
            p1_outer_vlan = 0;
            p1_inner_vlan = 0;
            p2_outer_vlan = 0;
            p2_inner_vlan = 0;
        /* set editing actions*/
        } else if (evpn_nof_linker_layer_tags == 1) {
            outer_action = is_device_or_above(unit, JERICHO2) ? bcmVlanActionArpVlanTranslateAdd: bcmVlanActionAdd;
            inner_action = bcmVlanActionNone;
            p1_outer_vlan = cint_evpn_info.p1_vlan;
            p1_inner_vlan = 0;
            p2_outer_vlan = cint_evpn_info.p2_vlan;
            p2_inner_vlan = 0;
        } else if (evpn_nof_linker_layer_tags == 2) {
            outer_action = is_device_or_above(unit, JERICHO2) ? bcmVlanActionArpVlanTranslateAdd: bcmVlanActionAdd;
            inner_action = is_device_or_above(unit, JERICHO2) ? bcmVlanActionArpVlanTranslateAdd: bcmVlanActionAdd;
            p1_outer_vlan = cint_evpn_info.p1_vlan;
            p1_inner_vlan = cint_evpn_info.p1_inner_vlan;
            p2_outer_vlan = cint_evpn_info.p2_vlan;
            p2_inner_vlan = cint_evpn_info.p2_inner_vlan;
        }

        if (is_device_or_above(unit, JERICHO2)) {
            /* JR2 uses the ARP_AC format for outer link layer and AC*/
            BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(egr_outer_ac_lif, BCM_L3_ITF_VAL_GET(cint_evpn_info.pe1_linker_layer_encap_id));
            BCM_GPORT_VLAN_PORT_ID_SET(egr_outer_ac_lif, egr_outer_ac_lif);
        } else {
            egr_outer_ac_lif = cint_evpn_info.p1_egr_vport_id;
        }

        /* Per AC LIf, determine the edit profile given inner and outer vlans */
        rv = vlan_port_translation_set(unit, p1_outer_vlan, p1_inner_vlan, egr_outer_ac_lif, g_eve_edit_utils.edit_profile, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_translation_set\n");
            return rv;
        }

        if (is_device_or_above(unit, JERICHO2)) {
            /* JR2 uses the ARP_AC format for outer link layer and AC*/
            BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(egr_outer_ac_lif, BCM_L3_ITF_VAL_GET(cint_evpn_info.pe2_linker_layer_encap_id));
            BCM_GPORT_VLAN_PORT_ID_SET(egr_outer_ac_lif, egr_outer_ac_lif);
        } else {
            egr_outer_ac_lif = cint_evpn_info.p2_egr_vport_id;
        }
        rv = vlan_port_translation_set(unit, p2_outer_vlan, p2_inner_vlan, egr_outer_ac_lif, g_eve_edit_utils.edit_profile, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_translation_set\n");
            return rv;
        }

        /* Set an EVE action given both actions*/
        rv = vlan_translate_action_set(unit, g_eve_edit_utils.action_id, 0, 0, outer_action, inner_action);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translate_action_set\n");
            return rv;
        }

        /* Install the action */
        rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_default_translate_action_class_set\n");
            return rv;
        }
    }

    if (rv != BCM_E_NONE) {
        printf("Error, evpn_create_eve\n");
    }

    return rv;
}

int multi_dev_evpn_create_ive(int ingress_unit, int egress_unit)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_action_set_t action;
    int flags;
    bcm_vlan_port_translation_t port_trans;

    flags = BCM_VLAN_ACTION_SET_INGRESS;
 
    /* Set port translation */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = 0;
    port_trans.new_inner_vlan = 0;
    port_trans.gport = cint_evpn_info.ce1_ing_vport_id;
    port_trans.vlan_edit_class_id = g_ive_edit_utils.edit_profile;
    port_trans.flags = flags;
    rv = bcm_vlan_port_translation_set(ingress_unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /* Create action IDs*/
    rv = bcm_vlan_translate_action_id_create(ingress_unit, flags | BCM_VLAN_ACTION_SET_WITH_ID, &(g_ive_edit_utils.action_id));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    if (ingress_unit != egress_unit) {
        /* Create the same action IDs in multi-dev case.*/
        rv = bcm_vlan_translate_action_id_create(egress_unit, flags | BCM_VLAN_ACTION_SET_WITH_ID, &(g_ive_edit_utils.action_id));
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }
    }

  
    /* Set translation action 1. outer action, set TPID 0x8100. */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionDelete;
    action.dt_inner = bcmVlanActionNone;
    action.outer_tpid = cint_evpn_info.tpid;
    rv = bcm_vlan_translate_action_id_set(egress_unit, flags, g_ive_edit_utils.action_id, &action);
     if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /* Set translation action class for single tagged packets with TPID 0x8100 */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = g_ive_edit_utils.edit_profile;
    action_class.tag_format_class_id = g_ive_edit_utils.tag_format;
    action_class.vlan_translation_action_id = g_ive_edit_utils.action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set(ingress_unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    return rv;
}


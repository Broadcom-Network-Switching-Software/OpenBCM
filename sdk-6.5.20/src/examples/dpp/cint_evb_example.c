/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_evb_example.c
 * Purpose: Edge Virtual Bridge (defined in 802.1Qbg) describes device and protocols
 * connecting between End Stations (VMs) and the Data centers network.
 * In this example we focus on VM switching.
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |        --------------------------------------------------------------------------------       |
 *  |        | EVB station                                                                  |       |
 *  |        |  -------  -------  ------- -------- -------- ------- -------                 |       |
 *  |        |  |VM0  |  |VM1  |  |VM2  | | VM3  | | VM4  | |VM5  | |VM6  |                 |       |
 *  |        |  ||  | |  ||  | |  ||  | | | |  | | | |  | | ||  | | ||  | |                 |       |
 *  |        |  |V1 V2|  |V2 V3|  |V2 V3| | V2 V3| | V2 V3| |V2 V3| |V2 V3|                 |       |
 *  |        |  -------  -------  ------- -------- -------- ------- -------                 |       |
 *  |        |     | |       \        |       \       |       |       /                     |       |
 *  |        |     | -----|   -----   -----    --------       --------                      |       |
 *  |        |     |      |       |        |        |         |                             |       |
 *  |        |    ------ ------ ------  --------  ------  --------                          |       |
 *  |        |    |ER-A| |ER-B| |ER-C|  \ ER-D /  |ER-E|  \ ER-F /                          |       |
 *  |        |    ------ ------ ------   ------   ------   ------                           |       |
 *  |        |       |      |       |     |         |        |                              |       |
 *  |        |       ----------------------------------------------                         |       |
 *  |        |                                |                                             |       |
 *  |        |                                |                                             |       |
 *  |        |                         ---------------------                                |       |
 *  |        |                         | A  B  C  D  E  F  |                                |       |
 *  |        |                         | S-VLAN componenet |                                |       |
 *  |        |                         ---------------------                                |       |
 *  |        ------------------------------------|||-----------------------------------------       |
 *  |                                             |                                                 |
 *  |                                             |(UAP)                                            |
 *  |        ------------------------------------|||-----------------------------------------       |
 *  |        | EVB Bridge               ---------------------      \ - V3 (C,D,E,F)         |       |
 *  |        |                          | A  B  C  D  E  F  |      / - V1 (A)               |       |
 *  |        |                          | S-VLAN componenet |      | - V2 (B,C,D,E,F)       |       |
 *  |        |                          ---------------------                               |       |
 *  |        |                            /  |  |\ |\ |\ |\                                 |       |
 *  |        |                           /   |  |\ |\ |\ |\                                 |       |
 *  |        |                          /    |  |\ |\ |\ |\                                 |       |
 *  |        |                         /     ||||\|\|\|\|\|\|\\\\\\\\\\\                    |       |
 *  |        |                        /                     |           \                   |       |
 *  |        |                       +--------+             |            \                  |       |
 *  |        |                      /        /|         +--------+      +--------+          |       |
 *  |        |                     +--------+ |        /        /|     /        /|          |       |
 *  |        |                     |  V1    | |       +--------+ |    +--------+ |          |       |
 *  |        |                     |C-VLAN  |/        |  V2    | |    |  V3    | |          |       |
 *  |        |                     +--------+         |C-VLAN  |/     |C-VLAN  |/           |       |
 *  |        |                          |             +--------+      +--------+            |       |
 *  |        |                           \              /                   /               |       |
 *  |        |                            ----------------------------------                |       |
 *  |        |                                      |                                       |       |
 *  |        |                                      |                                       |       |
 *  |        |                                      |                                       |       |
 *  |        |-------------------------------------|||--------------------------------------|       |
 *  |                                                (Bridge)                                       | 
 *  |                                                                                               |
 *  |                                                                                               |
 *  |                               +~~~~~~~~~~~~~~~~~~~~+                                          |
 *  |                               | Figure : EVB Model |                                          |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                                                                                                  
 * Explanation:                                                                             
 *  *   BCM886xx is used as EVB Bridge.Connects the VMs to the core
 *  *   The EVB station comprised of Edge Relays (ERs) that are specialized bridges within the station used for
 *      bridging support between multiple VM and an external network (in this example bridge). ER can be of type
 *      VEB or VEPA.
 *  *   Referring to figure above:
 *      * Example set 7 servers VMs (VM0-6) and 3 C-VLANs (1-3).
 *      * Trapezoid ER is a VEPA ER (ER-D,F)
 *      * Rectangle ER is a VEB ER (ER-A,B,C,E)
 *      * ER-G (not showed in figure above) is the default-S-channel i.e. Once packet comes with no
 *      S-channel EVB bridge identify it as ER-G.
 *      * ER-G, ER-A are connected to Initial-C-VLAN (3 in our example).  
 *   
 *  Notes:
 *  1. In this cint example we assume VSI = C-VLAN component for simplicity. User can set different mappings.  
 *  2. Current version use bcmPortClassFieldIngressPacketProcessing in order to set VEB / VEPA per Logical-Interface.
 *  On next version a new sequence will be introduced to do that.
 * 
 *  Main settings:
 *  1. Configure the device as an EVB bridge (soc properties) evb_enable=1.
 *  2. Configure port to be EVB Uplink access port (set_uplink_port example function below).
 *  3. Create ERs (virtual ports vlan_port_create example function below).
 *  4. Set ER Reflective Relay VEB/VEPA. (reflective_relay_set example function below).
 *  5. Add ERs to VSI, Configure multicast group including ERs (vswitch_add_port example function).
 *  6. FP: enable same-interface filter (learn_data_rebuild_example example function).
 * 
 * 
 *  How-to-run:
 *  Define the following SOC properties:
 *   - evb_enable=1
 *   - logical_port_drop=1   (logical port drop LIF-ID 1 )
 *
   cint src/examples/sand/utility/cint_sand_utils_global.c
   cint src/examples/dpp/cint_port_tpid.c
   cint src/examples/dpp/cint_vswitch_metro_mp.c
   cint src/examples/dpp/cint_field_learn_data_rebuild.c
   cint src/examples/dpp/cint_evb_example.c
   cint
   int unit = 0;
   int uplink_port = 13;
   int bridge_port = 14;
   main_evb_example(unit, uplink_port, bridge_port);
 * 
 *  Traffic examples:
 *  1.  VEB Unknown Unicast / Unknown Multicast / Broadcast traffic example :
 *      - The EVB station receive packet from VM3 with C-VLAN 4. Packet is unknown.
 *      - ER-E floods one member to VM4 (locally connected) since VM4 is a member of C-VLAN 4.
 *      - ER-E forwards one copy of the packet to other members by sending the packet to the EVB bridge with S-channel 6 (ER-E S-channel)
 *      - Packet enters BCM886xx (The EVB Bridge) with S-channel 6 and C-VLAN 4.
 *      - The EVB bridge floods the packet to all C-VLAN 4 ERs members which are : ER-B,ER-C,ER-D,ER-F and to the network. Packet is learnt.     
 *      - ER-A has no C-VLAN 4 bridge members and ER-E is the source ER, hence both ERs do not receive a copy
 *      - The EVB Station replicate copies for example: ER-F replicates a copy per VM5 and VM6.
 *  Traffic example:
 *      - From UAP port:
 *              -   ethernet header with any DA , SA 00:00:00:00:00:05
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 6 (S-channel). tag type 0x9100, VID 4 (C-VLAN)
 *      - Expect on UAP port:
 *              -   4 copies same DA and SA as original packet. All packets receive also C-VLAN (as inner vlan) tag type 0x9100, VID 4 (C-VLAN)
 *                  - ER-B reprenset by VLAN-tag type 0x8100, VID = 3     (outer vlan)
 *                  - ER-C reprenset by VLAN-tag type 0x8100, VID = 4     (outer vlan)
 *                  - ER-D reprenset by VLAN-tag type 0x8100, VID = 5     (outer vlan)
 *                  - ER-F reprenset by VLAN-tag type 0x8100, VID = 4095  (outer vlan)
 *      - Expect on Bridge port:
 *              -   1 copy same DA and SA as original packet. VLAN tags: tag type 0x9100, VID 4 (C-VLAN)
 * 
 *  2.  VEB Unicast traffic to VEB example ER-B => ER-E:
 *      - Assuming stage 1 is done packet is learnt. packet SA 5 VLAN-Port of ER-E, C-VLAN4.
 *      - Send packet from VM-0 C-VLAN 4 to VM-3 C-VLAN 4. Sending actually from ER-B to ER-E.
 *      - ER-B send packet received from VM0 to the EVB bridge
 *      - The EVB Bridge sends the packet to ER-E.
 *      - ER-E directs the packet to the final destination - VM3.
 *  Traffic example:
 *      - From UAP port:
 *              -   ethernet header with DA 00:00:00:00:00:05, any SA
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 3 (S-channel ER-B). tag type 0x9100, VID 4 (C-VLAN)
 *      - Expect on UAP port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 6 (S-channel ER-E). tag type 0x9100, VID 4 (C-VLAN)                  
 *      Note: Sending packet from VEB ER-E to ER-E will be dropped (Send packet with S-channel 6)
 * 
 *  3.  Network Unicast traffic to VEB example :
 *      - Assuming stage 1 is done, packet is learnt. packet SA 5 VLAN-Port of ER-E, C-VLAN4.
 *      - Send packet from network C-VLAN 4 to VM-3 C-VLAN 4. Sending actually from Network to ER-E.
 *      - The EVB Bridge sends the packet to ER-E.
 *      - ER-E directs the packet to the final destination - VM3.
 *  Traffic example:
 *      - From Network port:
 *              -   ethernet header with DA 00:00:00:00:00:05, any SA
 *              -   VLAN tags: VLAN tag type 0x9100, VID 4 (C-VLAN)
 *      - Expect on UAP port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 6 (S-channel ER-E). tag type 0x9100, VID 4 (C-VLAN)           
 * 
 *  4.  VEPA unknown Unicast / Multicast / Broadcast traffic example:
 *      - The EVB station receive packet from VM5 with C-VLAN 5. Packet is unknown.
 *      - ER-F forwards one copy of the packet to other members by sending the packet to the EVB bridge with S-channel 4095 (ER-F S-channel)
 *      - Packet enters BCM886xx (The EVB Bridge) with S-channel 4095 and C-VLAN 5.
 *      - The EVB bridge floods the packet to all C-VLAN 5 ERs members which are : ER-C,ER-D,ER-E,ER-F, and to the network. Packet is learnt.     
 *      - ER-A , ER-B have no C-VLAN 4 bridge members. Note ER-F receieve packet since ER type is VEPA.
 *      - The EVB Station replicate copies for example: ER-C replicates a copy to VM1.
 *  Traffic example:
 *      - From UAP port:
 *              -   ethernet header with any DA , SA 00:00:00:00:00:07
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 4095 (S-channel). tag type 0x9100, VID 5 (C-VLAN)
 *      - Expect on UAP port:
 *              -   4 copies same DA and SA as original packet. All packets receive also C-VLAN (as inner vlan) tag type 0x9100, VID 5 (C-VLAN)
 *                  - ER-C reprenset by VLAN-tag type 0x8100, VID = 4     (outer vlan)
 *                  - ER-D reprenset by VLAN-tag type 0x8100, VID = 5     (outer vlan)
 *                  - ER-E reprenset by VLAN-tag type 0x8100, VID = 6     (outer vlan)
 *                  - ER-F reprenset by VLAN-tag type 0x8100, VID = 4095  (outer vlan)
 *      - Expect on Bridge port:
 *              -   1 copy same DA and SA as original packet. VLAN tags: tag type 0x9100, VID 5 (C-VLAN)
 * 
 *  5. VEPA to VEPA Unicast traffic example ER-F => ER-F:
 *      - Assuming stage 4 is done packet is learnt. packet SA 7 VLAN-Port of ER-F, C-VLAN5.
 *      - Send packet from VM-5 C-VLAN 5 to VM-6 C-VLAN 5. Sending actually from ER-F to ER-F.
 *      - ER-F send packet received from VM5 to the EVB bridge
 *      - The EVB Bridge sends the packet back to ER-F.
 *      - ER-F directs the packet to the final destination - VM3.
 *  Traffic example:
 *      - From UAP port:
 *              -   ethernet header with DA 00:00:00:00:00:07, any SA
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 4095 (S-channel ER-F). tag type 0x9100, VID 5 (C-VLAN)
 *      - Expect on UAP port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 4095 (S-channel ER-F). tag type 0x9100, VID 5 (C-VLAN)      
 * 
 * 
 *  6. VEPA/VEB special cases example (in the special cases we used VEB ER):
 *  Special case 1:
 *  S-channel exist but no C-VLAN. Send from ER-A without C-VLAN.
 *      - VEB ER-A forwards the packet to the EVB bridge with no C-tag.
 *      - The EVB bridge runs initial C-VLAN procedure.
 *      - The EVB bridge sends the packet to ER-G untagged and Bridge port.
 *      - Packet is learnt.
 *      - Bridge port forwards the packet to the EVB bridge.
 *      - The EVB bridge sends the packet to ER-A with S-channel but untagged.  
 *  Traffic example (1):      
 *      - From UAP port:
 *              -   ethernet header with SA 00:00:00:00:01:00, any DA
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 2 (S-channel ER-A). 
 *      - Expect on UAP port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: untaggged (ER-G)
 *      - Expect on Bridge port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: VLAN tag type 0x9100, VID = 3 (Initial-C-VLAN)
 *  Traffic example (2):      
 *      - From Bridge port:
 *              -   ethernet header with any SA, DA 00:00:00:00:01:00
 *              -   VLAN tags: VLAN tag type 0x9100, VID = 3 (Initial-C-VLAN). 
 *      - Expect on UAP port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 2 (S-channel ER-A).
 *
 *  Special case 2:
 *  No S-channel. ER-G send without S-chanel but with C-tag.
 *      - VEB ER-G forwards the packet to the EVB bridge with only C-VLAN.
 *      - The EVB bridge includes it in the member of default S-channel for port.
 *      - The EVB bridge sends the packet to ER-A with S-channel and no C-VLAN, and to Bridge port with C-VLAN.
 *      - Packet is learnt.
 *      - VEB ER-A forwards the packet to the EVB bridge.
 *      - The EVB bridge sends the packet to ER-G without S-channel and no C-tag (since it is initial-C-VLAN).
 *  Traffic example (1):      
 *      - From UAP port:
 *              -   ethernet header with SA 00:00:00:00:02:00, any DA
 *              -   VLAN tags: VLAN tag type 0x9100, VID = 3 (Initial-C-VLAN). 
 *      - Expect on UAP port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 2 (S-channel ER-A) with no C-tag. 
 *      - Expect on Bridge port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: VLAN tag type 0x9100, VID = 3 (Initial-C-VLAN)
 *  Traffic example (2):      
 *      - From UAP port:
 *              -   ethernet header with any SA, DA 00:00:00:00:02:00
 *              -   VLAN tags: VLAN tag type 0x8100 VID = 2 (S-channel ER-A) , VLAN tag type 0x9100, VID = 3 (Initial-C-VLAN). 
 *      - Expect on UAP port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: untagged (ER-G Initial-C-VID 3). 
 * 
 *  Special case 3:
 *  No S-channel, No C-VLAN. Send from ER-G untagged.
 *      - VEB ER-G forwards the packet to the EVB bridge without S-channel and no C-tag.
 *      - The EVB bridge includes it in the member of default S-channel for port.
 *      - The EVB bridge runs initial C-VLAN procedure.
 *      - Packet is learnt.
 *      - Bridge port forwards the packet to the EVB bridge.
 *      - The EVB bridge sends the packet to ER-G without S-channel and no C-tag (untagged packet).
 *  Traffic example (1):      
 *      - From UAP port:
 *              -   ethernet header with AA 00:00:00:00:03:00, any DA
 *              -   VLAN tags: untagged
 *      - Expect on UAP port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: VLAN tag type 0x8100, VID = 2 (S-channel ER-A) with no C-tag. 
 *      - Expect on Bridge port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: VLAN tag type 0x9100, VID = 3 (Initial-C-VLAN)
 *  Traffic example (2):      
 *      - From Bridge port:
 *              -   ethernet header with any SA, DA 00:00:00:00:03:00
 *              -   VLAN tags: VLAN tag type 0x9100, VID = 3 (Initial-C-VLAN). 
 *      - Expect on UAP port:
 *              -   1 copy same DA and SA as original packet.
 *              -   VLAN tags: untagged
 */

struct evb_bridge_example_info_s {    
    int initial_c_vlan;   
    /* Uplink port */
    int uplink_access_port;
    /* C-VLAN bridge port */
    int bridge_port;
    /* Number of C-VLAN components */
    int nof_c_vlans;
    /* C-VLAN componenets */
    int c_vlans[3];
    /* Number of ERs */
    int nof_ers;  
    /* Number of Virutal_ports in bridge side */
    int nof_bridge_virtual_ports;
    /* If non-zero then basic mode is assumed (bcm886xx_logical_interface_bridge_filter_enable == 1). */
    /* Only for Arad+ */
    int same_interface_filter_basic_mode;
};


struct evb_er_info_s {
    int er_type; /*ER types 0-VEB, 1-VEPA */
    int c_vlans[3]; /* up to 3 C-VLANs each ER will support in our example. In case of -1 means no c_vlan */
    int virtual_gport[3]; /* Virutal-Ports represent each ER on C-VLANs. Saved for later use. */
    int s_channel; /* ER S-Channel (VLAN tag ID). -1 means no S-channel (use default s channel) */
    int uap; /* Uplink access port */
};

struct evb_bridge_info_s {
    int c_vlan; /* C-VLAN */
    bcm_gport_t bridge_virtual_gport; /* save Virtual-Port information */
    int bridge_port; /* Bridge port */
};

evb_er_info_s evb_er_info[7];
evb_bridge_info_s evb_bridge_info[3];
evb_bridge_example_info_s evb_bridge_example_info;
int verbose = 1;

/* Initalize parameters for application */
void evb_init(int unit, int uplink_port, int bridge_port)
{    
    int index = 0;
    int index_c_vlan = 0;

    evb_bridge_example_info.nof_ers = 7;
    evb_bridge_example_info.initial_c_vlan = 3;
    /*  Default S-channel is always 1 according to Qbg */
    evb_bridge_example_info.nof_c_vlans = 3; /* C-VLANs 3,4,5 */
    evb_bridge_example_info.bridge_port = bridge_port;
    evb_bridge_example_info.uplink_access_port = uplink_port;

    /* init */
    for (index = 0; index < evb_bridge_example_info.nof_ers; index++) {
        for (index_c_vlan = 0; index_c_vlan < 3; index_c_vlan++) {
            evb_er_info[index].c_vlans[index_c_vlan] = -1;
        }
        evb_er_info[index].s_channel = -1;        
    }

    index = 0;
    
    /* ER-A: VEB, C-VLAN 3, S-Channel 2, Connected to VM0 */
    evb_er_info[index].er_type = 0; /* VEB */
    evb_er_info[index].s_channel = 2;
    evb_er_info[index].c_vlans[0] = 3;
    evb_er_info[index].uap = evb_bridge_example_info.uplink_access_port;
    index++;

    /* ER-B: VEB, C-VLAN 4, S-Channel 3, Connected to VM0 */
    evb_er_info[index].er_type = 0; /* VEB */
    evb_er_info[index].s_channel = 3;
    evb_er_info[index].c_vlans[0] = 4;
    evb_er_info[index].uap = evb_bridge_example_info.uplink_access_port;
    index++;

    /* ER-C: VEB, C-VLANS 4,5 , S-Channel 4, Connected to VM1 */
    evb_er_info[index].er_type = 0; /* VEB */
    evb_er_info[index].s_channel = 4;
    evb_er_info[index].c_vlans[0] = 4;
    evb_er_info[index].c_vlans[1] = 5;
    evb_er_info[index].uap = evb_bridge_example_info.uplink_access_port;
    index++;

    /* ER-D: VEPA, C-VLANS 4,5 , S-Channel 5, Connected to VM2 */
    evb_er_info[index].er_type = 1; /* VEPA */
    evb_er_info[index].s_channel = 5;
    evb_er_info[index].c_vlans[0] = 4;
    evb_er_info[index].c_vlans[1] = 5;
    evb_er_info[index].uap = evb_bridge_example_info.uplink_access_port;
    index++;

    /* ER-E: VEB, C-VLANS 4,5 , S-Channel 6, Connected to VM3, VM4 */
    evb_er_info[index].er_type = 0; /* VEB */
    evb_er_info[index].s_channel = 6;
    evb_er_info[index].c_vlans[0] = 4;
    evb_er_info[index].c_vlans[1] = 5;
    evb_er_info[index].uap = evb_bridge_example_info.uplink_access_port;
    index++;

    /* ER-F: VEPA, C-VLANS 4,5 , S-Channel 4095, Connected to VM5, VM6 */
    evb_er_info[index].er_type = 1; /* VEPA */
    evb_er_info[index].s_channel = 4095;
    evb_er_info[index].c_vlans[0] = 4;
    evb_er_info[index].c_vlans[1] = 5;
    evb_er_info[index].uap = evb_bridge_example_info.uplink_access_port;
    index++;

    /* ER-G: VEB, C-VLANS 3 , S-Channel 1, Connected to VM7 (not showed in figure) */
    evb_er_info[index].er_type = 0; /* VEB */
    evb_er_info[index].s_channel = 1;
    evb_er_info[index].c_vlans[0] = evb_bridge_example_info.initial_c_vlan;    
    evb_er_info[index].uap = evb_bridge_example_info.uplink_access_port;

    /* C-VLAN componenets */
    evb_bridge_example_info.c_vlans[0] = 3;
    evb_bridge_example_info.c_vlans[1] = 4;
    evb_bridge_example_info.c_vlans[2] = 5;

    /* Bridge information */
    index = 0;
    /* C-VLAN 3 */
    evb_bridge_info[index].c_vlan = 3;
    evb_bridge_info[index].bridge_port = evb_bridge_example_info.bridge_port;
    index++;

    /* C-VLAN 4 */
    evb_bridge_info[index].c_vlan = 4;
    evb_bridge_info[index].bridge_port = evb_bridge_example_info.bridge_port;
    index++;

    /* C-VLAN 5 */
    evb_bridge_info[index].c_vlan = 5;
    evb_bridge_info[index].bridge_port = evb_bridge_example_info.bridge_port;
    
    evb_bridge_example_info.nof_bridge_virtual_ports = 3;
    evb_bridge_example_info.same_interface_filter_basic_mode = 0;

    if (verbose) {
        printf("done  evb_bridge_init\n");
    }
}

/* 
 * Configure Physical Port (EVB Uplink Access Port)
 */
int set_uplink_port(int unit, int uplink_access_port, int initial_c_vlan)
{
    int rv = 0;
    bcm_port_evb_type_t evb_type;
    int enable;
    int flags;

    /* Must be called before TPID settings. Set port to be EVB. */
    evb_type = bcmPortEvbTypeUplinkAccess;

    rv = bcm_port_control_set(unit, uplink_access_port, bcmPortControlEvbType, evb_type);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_control_set type uplink FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Set TPIDs: two outers signifying S-tag and C-tag. and inner tag singnify C-tag in case of double-tagged. */
    /* In this example: TPID 0x8100 - S-tag , 0x9100 - C-tag */
    port_tpid_init(uplink_access_port,2,1);
    port_tpid_info1.outer_tpids[1] = 0x9100;
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set, in unit %d\n", unit);
        print rv;
        return rv;
    }

    /* Set Vlan Domain to be 1:1 mapping */
    rv = bcm_port_class_set(unit, uplink_access_port, bcmPortClassId, uplink_access_port);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_class_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Uplink access ports: Disable Port VLAN membership, Tag/UnTag and set Port-Default to drop */
    flags = 0;        
    rv = bcm_port_vlan_member_set(unit, uplink_access_port, flags);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_vlan_member_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Set port-Default to drop */
    enable = 1;
    
    rv = bcm_vlan_control_port_set(unit, uplink_access_port, bcmVlanTranslateIngressMissDrop, enable);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_vlan_control_port_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Enable same interface (pruning check) */
    enable = 1;
    rv = bcm_port_control_set(unit, uplink_access_port, bcmPortControlBridge, enable);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_control_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }    

    /* Initial C-VLAN */
    rv = bcm_port_untagged_vlan_set(unit, uplink_access_port,  initial_c_vlan);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_untagged_vlan_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }     

    if (verbose) {
        printf("set uplink-port:0x0%8x  \n", uplink_access_port);
    }
    return rv;
}

/* 
 * Configure Physical Port (Bridge port - port facing overlay, network)
 */
int set_bridge_port(int unit, int bridge_port)
{
    int rv = 0;
    bcm_port_evb_type_t evb_type;
    int enable;
    int flags;

    evb_type = bcmPortEvbTypeNone;

    rv = bcm_port_control_set(unit, bridge_port, bcmPortControlEvbType, evb_type);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_control_set type uplink FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    port_tpid_init(bridge_port,1,0);
    /* In bridge port outer-TPID is 0x9100 */
    port_tpid_info1.outer_tpids[0] = 0x9100;

    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set, in unit %d\n", unit);
        print rv;
        return rv;
    }

    /* Set Bridge to be customer port */
    rv = bcm_port_dtag_mode_set(unit, bridge_port, BCM_PORT_DTAG_MODE_EXTERNAL);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_dtag_mode_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Set Vlan Domain to be 1:1 mapping */
    rv = bcm_port_class_set(unit, bridge_port, bcmPortClassId, bridge_port);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_class_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }
        
    /* bridge port : filters Ingress, Egress */
    flags = BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS;
    rv = bcm_port_vlan_member_set(unit, bridge_port, flags);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_vlan_member_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Drop disable */
    enable = 0;
    rv = bcm_vlan_control_port_set(unit, bridge_port, bcmVlanTranslateIngressMissDrop, enable);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_vlan_control_port_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Enable same interface (pruning check) */
    enable = 1;
    rv = bcm_port_control_set(unit, bridge_port, bcmPortControlBridge, enable);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_control_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }    

    if (verbose) {
        printf("set bridge-port:0x0%8x  \n", bridge_port);
    }

    return rv;
}

/* create vlan_port (Logical interface identified by port-vlan) */
int
vlan_port_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_port_match_t match,
    bcm_gport_t *gport,
    bcm_vlan_t outer_vlan, /* incoming outer vlan and also outgoing vlan */
    bcm_vlan_t inner_vlan,
    int flags
    ) {

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);
  
    vp.criteria = match;
    vp.port = port_id;
    vp.match_vlan = outer_vlan; 
    vp.match_inner_vlan = inner_vlan;
    vp.egress_vlan = outer_vlan; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.egress_inner_vlan = inner_vlan;
    vp.vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
    vp.flags = flags; 
   
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    if (verbose) {
        printf("done  created vlan-port:0x0%8x  \n", vp.vlan_port_id);
    }
    *gport = vp.vlan_port_id;  
    return BCM_E_NONE;
}


/*
 * Set Reflective Relay. 
 * According to ER type set VIrtual-Port to Enable or disable Reflective Relay. 
 * In case ER type is VEB then Disable Reflective Relay (i.e. same interface filter is enabled) 
 * In case ER type is VEPA then Enable Reflective Relay (i.e. same interface filter is disabled)
 */
int reflective_relay_set(int unit, bcm_gport_t virtual_port, int er_type)
{
    int value;
    int rv;

    /* 
     * For Arad+ advanced mode:
     * It is important to set the inlif profile before disabling the
     * same interface filter since the same interface filter is
     * disabled for the inlif profile of the passed LIF.
     */
    if (!evb_bridge_example_info.same_interface_filter_basic_mode) {

      /* temp settings (will be removed) */
      if (er_type == 0 /* VEB */) {
          value = 0; /* Set LIF profile LSB to be 0*/
      } else { /* VEPA */
          value = 1; /* Set LIF profile LSB to be 1*/
      }

      rv = bcm_port_class_set(unit, virtual_port, bcmPortClassFieldIngressPacketProcessing, value);
      if (BCM_FAILURE(rv)) {
          printf("BCM bcm_port_control_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
          return rv;
      }
    }

    if (er_type == 0 /* VEB */) {
        value = 1; /* VEB 1 means Disable Reflective Relay */
    } else { /* VEPA */
        value = 0; /* VEPA 0 means Enable Reflective Relay */
    }
   
    rv = bcm_port_control_set(unit, virtual_port, bcmPortControlBridge, value);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_control_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    return rv;
}

int vlan_port_translate_action_set(int unit, int virtual_gport, int initial_c_vlan)
{
    int rv;
    bcm_vlan_action_set_t action;

    rv = bcm_vlan_translate_action_get(unit, virtual_gport, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_vlan_translate_action_get FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Double tag - remove outer (S-channel) and inner  */
    action.dt_outer = bcmVlanActionDelete;
    action.dt_inner = bcmVlanActionDelete;

    /* Single tag ot - remove outer (S-channel) */
    action.ot_outer = bcmVlanActionDelete;
    action.ot_inner = bcmVlanActionNone;
    
    /* Single tag it - remove inner (C-VLAN) */
    action.it_outer = bcmVlanActionNone;
    action.it_inner = bcmVlanActionDelete;

    /* Untagged - Dont touch */
    action.ut_inner = bcmVlanActionNone;
    action.ut_outer = bcmVlanActionNone;

    rv = bcm_vlan_translate_action_create(unit, virtual_gport, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_vlan_translate_action_create FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    return rv;    
}

int vlan_port_translate_egress_action_set(int unit, int virtual_gport, int s_channel, int c_vlan, int initial_c_vlan)
{
    int rv;
    bcm_vlan_action_set_t action;

    rv = bcm_vlan_translate_egress_action_get(unit, virtual_gport, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_vlan_translate_action_get FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }    

    /* Always untagged */    
    if (s_channel != 1) {        
        /* Add S-channel, in case it isnt default-S-channel */
        action.ut_outer = bcmVlanActionAdd;     
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.outer_tpid = 0x8100;
        action.new_outer_vlan = s_channel;    
    } else {
        action.ut_outer = bcmVlanActionNone;        
    }

    if (c_vlan != initial_c_vlan) {
        /* Add C-vlan in case it isnt the initial C-vlan */
        action.ut_inner = bcmVlanActionAdd;
        action.inner_tpid_action = bcmVlanTpidActionModify;
        action.inner_tpid = 0x9100;
        action.new_inner_vlan = c_vlan;
    } else {
        action.ut_inner = bcmVlanActionNone;
    } 

    rv = bcm_vlan_translate_egress_action_add(unit, virtual_gport, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_vlan_translate_action_create FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    return rv;    
}


/*
 * Main Applicaiton to run
 */ 
int evb_application_example(int unit) {

    int rv;
    int er_type;
    uint32 flags;
    int enable;
    bcm_vlan_port_match_t match;
    int s_channel;
    bcm_vlan_t c_vlan;
    int index_c_vlan;
    int index_bridge;
    int index_er;
    int uplink_port;
    bcm_vlan_action_set_t action;
    
    /* Set ports configuration */
    rv = set_uplink_port(unit, evb_bridge_example_info.uplink_access_port, evb_bridge_example_info.initial_c_vlan);
    if (BCM_FAILURE(rv)) {
        printf("BCM set_uplink_port type uplink FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    rv = set_bridge_port(unit, evb_bridge_example_info.bridge_port);
    if (BCM_FAILURE(rv)) {
        printf("BCM set_bridge_port type none FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Create VSIs (C-VLAN componenets) */
    for (index_c_vlan = 0; index_c_vlan < evb_bridge_example_info.nof_c_vlans; index_c_vlan++) {
        c_vlan = evb_bridge_example_info.c_vlans[index_c_vlan];
        rv = vswitch_create(unit, &c_vlan, 1 /* with ID assume VSI = C-VLAN, see notes above. */);
        if (BCM_FAILURE(rv)) {
            printf("BCM vswitch_create %d  FAIL %d: %s\n", c_vlan, rv, bcm_errmsg(rv));
            return rv;
        }
    }

    /* 
     * UAP (uplink access port) side:                                                                                                        .
     * Create ER Virtual-ports (ER on C-VLANs). 
     * Match criteria: port x S-channel (ER) x C-vlan.
     */
    match = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;    
    flags = 0;

    for (index_er = 0; index_er < evb_bridge_example_info.nof_ers; index_er++) {
        /* Check each ER which CVLAN members it has */
        s_channel = evb_er_info[index_er].s_channel;
        er_type = evb_er_info[index_er].er_type;
        uplink_port = evb_er_info[index_er].uap;
        
        for (index_c_vlan = 0; index_c_vlan < evb_bridge_example_info.nof_c_vlans; index_c_vlan++) {
            c_vlan = evb_er_info[index_er].c_vlans[index_c_vlan];            
            if (c_vlan != 0xffff) {
                /* In case ER is in C-VLAN call virtual port creation */ 
                         
                /* 
                 * Virtual Port sequence for UAP side 
                 */ 

                /* 1. Create Virtual-Port */
                rv = vlan_port_create(unit, uplink_port, match, &(evb_er_info[index_er].virtual_gport[index_c_vlan]), s_channel, c_vlan, flags);
                if (BCM_FAILURE(rv)) {
                    printf("BCM vlan_port_create FAIL %d: %s\n", rv, bcm_errmsg(rv));
                    return rv;
                }                

                /* 2. Set VLAN editing - Ingress remove S-channel to be single-tag (C-tag) */
                rv = vlan_port_translate_action_set(unit, evb_er_info[index_er].virtual_gport[index_c_vlan], evb_bridge_example_info.initial_c_vlan);
                if (BCM_FAILURE(rv)) {
                    printf("BCM vlan_port_translate_action_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
                    return rv;
                }

                /* 3. Set VLAN editing - Egress Add C-tag */
                rv = vlan_port_translate_egress_action_set(unit, evb_er_info[index_er].virtual_gport[index_c_vlan], s_channel, c_vlan, evb_bridge_example_info.initial_c_vlan);
                if (BCM_FAILURE(rv)) {
                    printf("BCM vlan_port_translate_egress_action_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
                    return rv;
                }

                /* 
                 *  4. Set ER TYPE 
                 *  Each ER in c vlan indicate if its VEB or VEPA
                 *  Note: User must set all Virtual-Ports that go to the same ER with the same type (VEB/VEPA).
                 *  For example ER-A has 2 C-VLAN members. Both Virtual-Ports must go to the same source.
                 */
                rv = reflective_relay_set(unit, evb_er_info[index_er].virtual_gport[index_c_vlan],er_type);
                if (rv != BCM_E_NONE) {
                    printf("Error, reflective_relay_set\n");
                    return rv;
                }

                /* 5. Add ports to VSWITCH (C-VLAN) */
                rv = vswitch_add_port(unit, c_vlan, uplink_port, evb_er_info[index_er].virtual_gport[index_c_vlan]);
                if (rv != BCM_E_NONE) {
                    printf("Error, vswitch_add_port\n");
                    return rv;
                }     
            }                                 
        }        
    }

    /* 
     * Bridge port side: 
     * Create Virtual-Port for Bridge port 
     * Each Virtual-Port is actually port x C-VLAN match 
     */
    match = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    flags = 0;    

    for (index_bridge = 0; index_bridge < evb_bridge_example_info.nof_bridge_virtual_ports; index_bridge++) {
        c_vlan = evb_bridge_info[index_bridge].c_vlan;

        /* 1. Add VLAN to membership. It is used since C-VLAN is lower than 4K. in case Service is higher than 4K disable the filter of membership */
        flags = 0; 
        rv = bcm_vlan_gport_add(unit, c_vlan, evb_bridge_info[index_bridge].bridge_port, flags);
        if (BCM_FAILURE(rv)) {
            printf("BCM bcm_vlan_gport_add FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return rv;
        }

        /* 2. Create Virtual-Port */
        flags = 0;
        rv = vlan_port_create(unit, evb_bridge_info[index_bridge].bridge_port, match, &(evb_bridge_info[index_bridge].bridge_virtual_gport), c_vlan, 0, flags);
        if (BCM_FAILURE(rv)) {
            printf("BCM vlan_port_create FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return rv;
        }

        /* 3. Change EVE so TPID attach will be 0x9100 and not as given from vlan_port_create */
        
        /* Set egress vlan editor of NNI LIF to do nothing, when creating NNi LIF */
        rv = bcm_vlan_translate_egress_action_get(unit, evb_bridge_info[index_bridge].bridge_virtual_gport, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
          printf("Error, in bcm_vlan_translate_egress_action_get %s\n", bcm_errmsg(rv));
          return rv;
        }
        
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.outer_tpid = 0x9100;
        rv = bcm_vlan_translate_egress_action_add(unit, evb_bridge_info[index_bridge].bridge_virtual_gport, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
          printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
          return rv;
        }

        /* 4. Add Virutal-Port to VSI */
        rv = vswitch_add_port(unit, c_vlan, evb_bridge_info[index_bridge].bridge_port, evb_bridge_info[index_bridge].bridge_virtual_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, vswitch_add_port\n");
            return rv;
        }        
    }
    
    return 0;
}

int main_evb_example(int unit, int uplink_port, int bridge_port) {
    int rv = BCM_E_NONE;
    bcm_pbmp_t uap_pbmp;

    BCM_PBMP_CLEAR(uap_pbmp);

    /* initial parameters */
    evb_init(unit, uplink_port, bridge_port);

    rv = evb_application_example(unit);    
    if (rv != BCM_E_NONE) {
        printf("Error, evb_application_example\n");
        return rv;
    }

    if (!is_device_or_above(unit,ARAD_PLUS)) {
      /* 
       * enable flexible same interface filter per Incoming Logical Interface
       */
      BCM_PBMP_PORT_ADD(uap_pbmp, uplink_port);
      rv = learn_data_rebuild_example(unit,uap_pbmp, evb_bridge_example_info.same_interface_filter_basic_mode);
      if (rv != BCM_E_NONE) {
          printf("Error, learn_data_rebuild_example\n");
          return rv;
      }
    }

    return rv;
}

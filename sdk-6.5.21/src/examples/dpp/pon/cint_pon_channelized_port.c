/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_pon_channelized_port.c
* Purpose: An example of the PON channlized port application and how to set different properties (VTT lookup mode and VLAN range) per LLID profile based on channelized port. 
*          The following CINT provides a calling sequence example to bring up two channelized ports per PON port and 
*          set the different tunnel_id to be mapped to different LLID profile and bring up one main PON services N:1 Model.
*          In this system, ARAD is used in a chip set to provide PON OLT functionality. 
*          ARAD ports are either network facing NNI ports (port numbers >= 128) or facing PON ports (port numbers 0-7).
*          Packets on the NNI ports are Ethernet packets, tagged with 0,1 or 2 VLAN tags identify the user and service (ONU), and class of service. 
*          Packets on the PON ports are Ethernet packets with an outermost VLAN-Tag that encodes the Logical Link Identification (LLID), i.e., service or ONU#. 
*          We refer to that tag as the Tunnel-ID (BCM APIs term) or PON-Channel ID (Arch term). The Tunnel-ID is prepended to the VLAN-Tag stack (as the outermost tag). 
*          Tunnel-ID it always present on packets on PON ports, which may have C-Tag and S-Tag as well. 
*          When packet is forwarded downstream from NNI-Port to PON-Ports, the incoming VLAN-header from the NNI port may be retained, stripped, or modified, and a Tunnel-ID is inserted.
*          When packet are forwarded upstream from PON-Port to a NNI-Ports, the Incoming VLAN-header from the PON port may be retained, stripped, or modified, and the Tunnel-ID is stripped.
*
* Calling sequence:
*
* Initialization:
*  1. Add the following port configureations to config-sand.bcm to enable two channlized ports on PON port 5.
*        ucode_port_129.BCM88650=10GBase-R15
*        ucode_port_5.BCM88650=10GBase-R14.0
*        ucode_port_13.BCM88650=10GBase-R14.1
*  2. Add the following PON application enabling configureations to config-sand.bcm
*        pon_application_support_enabled_5.BCM88650=TRUE
*        vlan_match_criteria_mode=PON_PCP_ETHERTYPE
*  3. PON regular initial routine 
*        - call pon_app_init()
*
*
*  4. Create PON PP port with different PON tunnel profile id 0.
*        - call bcm_port_pon_tunnel_add()
*
*  5. Map tunnel id 1000 to PON PP port 5 (indirectly mapped to PON tunnel profile id 0).
*        - call bcm_port_pon_tunnel_map_set()
*  6. Set TPIDs of PON pp port 5.
*        - call bcm_port_tpid_delete_all()
*        - call bcm_port_tpid_add()
*        - call bcm_port_inner_tpid_set()
*  7. Set VLAN domain of PON pp port 5 to 5.
*        - call bcm_port_class_set()
*  8. Enable additional port tunnel lookup in PON PP port 5.
*        - call bcm_vlan_control_port_set() with bcmVlanPortLookupTunnelEnable
*  9. Add VLAN range info to PON PP port if necessary:
*        - Call bcm_vlan_translate_action_range_add() with action bcmVlanActionCompressed.
*
*  10. Map tunnel id 1001 to channelized PON PP port 13 (indirectly mapped to PON tunnel profile id 0).
*        - call bcm_port_pon_tunnel_map_set()
*  11. Set TPIDs of PON pp port 5.
*        - call bcm_port_tpid_delete_all()
*        - call bcm_port_tpid_add()
*        - call bcm_port_inner_tpid_set()
*  12. Set VLAN domain of PON pp port 13 to 13.
*        - call bcm_port_class_set()
*  13. Enable additional port tunnel lookup in PON PP port 13.
*        - call bcm_vlan_control_port_set() with bcmVlanPortLookupTunnelEnable
*  14. Add VLAN range info to PON PP port if necessary:
*        - Call bcm_vlan_translate_action_range_add() with action bcmVlanActionCompressed.
*
*
*  15. Create PON PP port with different PON tunnel profile id 1.
*        - call bcm_port_pon_tunnel_add()
*
*  16. Map tunnel id 1002 to PON PP port 21 (indirectly mapped to PON tunnel profile id 1).
*        - call bcm_port_pon_tunnel_map_set()
*  17. Set TPIDs of PON pp port 21.
*        - call bcm_port_tpid_delete_all()
*        - call bcm_port_tpid_add()
*        - call bcm_port_inner_tpid_set()
*  18. Set VLAN domain of PON pp port 21to 21.
*        - call bcm_port_class_set()
*  19. Enable ingore inner VLAN tag lookup in PON PP port 21.
*        - call bcm_vlan_control_port_set() with bcmVlanPortIgnoreInnerPktTag
*  20. Add VLAN range info to PON PP port if necessary:
*        - Call bcm_vlan_translate_action_range_add() with action bcmVlanActionCompressed.
*
*  21. Map tunnel id 1003 to channelized PON PP port 29 (indirectly mapped to PON tunnel profile id 1).
*        - call bcm_port_pon_tunnel_map_set()
*  22. Set TPIDs of PON pp port 13.
*        - call bcm_port_tpid_delete_all()
*        - call bcm_port_tpid_add()
*        - call bcm_port_inner_tpid_set()
*  23. Set VLAN domain of PON pp port 29 to 29.
*        - call bcm_port_class_set()
*  24. Enable ingore inner VLAN tag lookup in PON PP port 29.
*        - call bcm_vlan_control_port_set() with bcmVlanPortIgnoreInnerPktTag
*  25. Add VLAN range info to PON PP port if necessary:
*        - Call bcm_vlan_translate_action_range_add() with action bcmVlanActionCompressed.
*
*
*  26. Create PON PP port with different PON tunnel profile id 2.
*        - call bcm_port_pon_tunnel_add()
*
*  27. Map tunnel id 1004 to PON PP port 37 (indirectly mapped to PON tunnel profile id 2).
*        - call bcm_port_pon_tunnel_map_set()
*  28. Set TPIDs of PON pp port 37.
*        - call bcm_port_tpid_delete_all()
*        - call bcm_port_tpid_add()
*        - call bcm_port_inner_tpid_set()
*  29. Set VLAN domain of PON pp port 37 to 37.
*        - call bcm_port_class_set()
*  30. Set PON pp port 21 to discard all packets.
*        - call bcm_port_discard_set() with BCM_PORT_DISCARD_ALL
*
*  31. Map tunnel id 1005 to PON PP port 45 (indirectly mapped to PON tunnel profile id 2).
*        - call bcm_port_pon_tunnel_map_set()
*  32. Set TPIDs of PON pp port 45.
*        - call bcm_port_tpid_delete_all()
*        - call bcm_port_tpid_add()
*        - call bcm_port_inner_tpid_set()
*  33. Set VLAN domain of PON pp port 45 to 45.
*        - call bcm_port_class_set()
*  34. Set PON pp port 21 to discard all packets.
*        - call bcm_port_discard_set() with BCM_PORT_DISCARD_ALL
*
* 1:1 Service:
* Set up sequence:
*  1. Create PON LIF
*        - Call bcm_vlan_port_create() with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN
*  2. Set PON LIF ingress VLAN editor.
*        - Call bcm_vlan_translate_action_create()
*  3. Set PON LIF egress VLAN editor.
*        - Call bcm_vlan_translate_egress_action_add()
*  4. Create NNI LIF
*        - Call bcm_vlan_port_create()with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
*  5. Cross connect the 2 LIFs
*        - Call bcm_vswitch_cross_connect_add()
* Clean up sequence:
*  1. Delete the cross connected LIFs.
*        - Call bcm_vswitch_cross_connect_delete()
*  2. Delete PON LIFs.
*        - Call bcm_vlan_port_destroy()
*  3. Delete NNI LIFs.
*        - Call bcm_vlan_port_destroy()
*
* Service Model:
* 1:1 Service:
*     N(1) PON Port 5  <-----------------------------  CrossConnect  ------------> 1 NNI Port 129
*     Tunnel-ID 1000 SVLAN 20 PON_PP_PORT 5 <------------------------------------> SVLAN 200
*     Tunnel-ID 1000 SVLAN 20 CVLAN 21 PON_PP_PORT 5 <---------------------------> SVLAN 201 CVLAN 21
*     Tunnel-ID 1000 SVLAN range 50~60 PON_PP_PORT 5 <---------------------------> SVLAN 70
*
*     Tunnel-ID 1001 SVLAN 20 PON_PP_PORT 13 <------------------------------------> SVLAN 210
*     Tunnel-ID 1001 SVLAN 20 CVLAN 21 PON_PP_PORT 13 <---------------------------> SVLAN 202 CVLAN 21
*     Tunnel-ID 1001 SVLAN range 50~60 PON_PP_PORT 13 <---------------------------> SVLAN 500
*
*     Tunnel-ID 1002 SVLAN 30 PON_PP_PORT 21 <-----------------------------------> SVLAN 300
*     Tunnel-ID 1002 SVLAN 30 CVLAN 31 PON_PP_PORT 21 <--------------------------> SVLAN 303 CVLAN 31
*     Tunnel-ID 1002 SVLAN range 80~90 PON_PP_PORT 21 <--------------------------> SVLAN 800
*
*     Tunnel-ID 1003 SVLAN 30 PON_PP_PORT 29 <-----------------------------------> SVLAN 310
*     Tunnel-ID 1003 SVLAN 30 CVLAN 31 PON_PP_PORT 29 <--------------------------> SVLAN 304 CVLAN 31
*     Tunnel-ID 1003 SVLAN range 80~90 PON_PP_PORT 29 <--------------------------> SVLAN 1000
*
*     Tunnel-ID 1004 SVLAN 40 PON_PP_PORT 37 <-----------------------------------> SVLAN 400
*     Tunnel-ID 1005 SVLAN 40 PON_PP_PORT 45 <-----------------------------------> SVLAN 410
*
* Traffic:
*  1. PON Port 5 PON PP Port 5 Tunnel-ID 1000 SVLAN 20 <-CrossConnect-> NNI Port 129 SVLAN 200
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 20
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 200
*
*  2. PON Port 5 PON PP Port 5 Tunnel-ID 1000 SVLAN 20 CVLAN 21 <-CrossConnect-> NNI Port 129 SVLAN 201 CVLAN 21
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 20, VLAN tag type 0x9100, VID = 21
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 201, VLAN tag type 0x9100, VID = 21
*
*  3. PON Port 5 PON PP Port 5 Tunnel-ID 1000 SVLAN range 50~60 <-CrossConnect-> NNI Port 129 SVLAN 70
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 55
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 70
*
*  4. PON Port 5 PON PP Port 13 Tunnel-ID 1001 SVLAN 20 <-CrossConnect-> NNI Port 129 SVLAN 210
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1001
*              -   VLAN tag: VLAN tag type 0x8100, VID = 20
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 210
*
*  5. PON Port 5 PON PP Port 13 Tunnel-ID 1001 SVLAN 20 CVLAN 21 <-CrossConnect-> NNI Port 129 SVLAN 202 CVLAN 21
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1001
*              -   VLAN tag: VLAN tag type 0x8100, VID = 20, VLAN tag type 0x9100, VID = 21
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 202, VLAN tag type 0x9100, VID = 21
*
*  6. PON Port 5 PON PP Port 13 Tunnel-ID 1001 SVLAN range 50~60 <-CrossConnect-> NNI Port 129 SVLAN 500
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1001
*              -   VLAN tag: VLAN tag type 0x8100, VID = 55
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 500
*
*  7. PON Port 5 PON PP Port 21 Tunnel-ID 1002 SVLAN 30 <-CrossConnect-> NNI Port 129 SVLAN 300
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1002
*              -   VLAN tag: VLAN tag type 0x8100, VID = 30
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 300
*
*  8. PON Port 5 PON PP Port 21 Tunnel-ID 1002 SVLAN 30 CVLAN 31 <-CrossConnect-> NNI Port 129 SVLAN 303 CVLAN 31
*    - Upstream is matched by Port 5 Tunnel-ID 1002 SVLAN 30 <--CrossConnect--> NNI Port 129 SVLAN 300, becasue LLID profile ingore 2nd VLAN tag.
*    - Packets received in NNI port have outer VLAN 30, inner VLAN 31, because matched service doesn't have double tag modifications.
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1001
*              -   VLAN tag: VLAN tag type 0x8100, VID = 30, VLAN tag type 0x9100, VID = 31
*    - Downstrean is macthed by NNI Port 129 SVLAN 303 CVLAN 31 <--CrossConnect--> Port 5 Tunnel-ID 1002 SVLAN 30 CVLAN 31.
*    - Packets received in PON port have outer VLAN 30, inner VLAN 31.
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 30, VLAN tag type 0x9100, VID = 31
*  9. PON Port 5 PON PP Port 21 Tunnel-ID 1002 SVLAN range 80~90 <-CrossConnect-> NNI Port 129 SVLAN 800
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1002
*              -   VLAN tag: VLAN tag type 0x8100, VID = 85
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 800
*
*  10. PON Port 5 PON PP Port 29 Tunnel-ID 1003 SVLAN 30 <-CrossConnect-> NNI Port 129 SVLAN 310
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1003
*              -   VLAN tag: VLAN tag type 0x8100, VID = 30
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 310
*
*  11. PON Port 5 PON PP Port 29 Tunnel-ID 1003 SVLAN 30 CVLAN 31 <-CrossConnect-> NNI Port 129 SVLAN 304 CVLAN 31
*    - Upstream is matched by Port 5 Tunnel-ID 1003 SVLAN 30 <--CrossConnect--> NNI Port 129 SVLAN 310, becasue LLID profile ingore 2nd VLAN tag.
*    - Packets received in NNI port have outer VLAN 30, inner VLAN 31, because matched service doesn't have double tag modifications.
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1003
*              -   VLAN tag: VLAN tag type 0x8100, VID = 30, VLAN tag type 0x9100, VID = 31
*    - Downstrean is macthed by NNI Port 129 SVLAN 304 CVLAN 31 <--CrossConnect--> Port 5 Tunnel-ID 1003 SVLAN 30 CVLAN 31.
*    - Packets received in PON port have outer VLAN 30, inner VLAN 31.
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 30, VLAN tag type 0x9100, VID = 31
*  12. PON Port 5 PON PP Port 29Tunnel-ID 1003 SVLAN range 800~900 <-CrossConnect-> NNI Port 129 SVLAN 1000
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1003
*              -   VLAN tag: VLAN tag type 0x8100, VID = 855
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 1000
*
*  13. PON Port 5 PON PP Port 27 Tunnel-ID 1004 SVLAN 40 <-CrossConnect-> NNI Port 129 SVLAN 400
*    - Upstream is dropped, becasue PON pp port 21 drops all packets.
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1004
*              -   VLAN tag: VLAN tag type 0x8100, VID = 40
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 400
*
*  14. PON Port 5 PON PP Port 45 Tunnel-ID 1005 SVLAN 40 <-CrossConnect-> NNI Port 129 SVLAN 410
*    - Upstream is dropped, becasue PON pp port 21 drops all packets.
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1005
*              -   VLAN tag: VLAN tag type 0x8100, VID = 40
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 410
*
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_tpid.c
*      BCM> cint examples/dpp/cint_pon_utils.c
*      BCM> cint examples/dpp/cint_pon_channelized_port.c
*      BCM> cint
*      cint> pon_tunnel_channelized_service_init(unit, pon_port, nni_port);
*      cint> pon_tunnel_channelized_mapping_1_1_service(unit);
*      cint> pon_tunnel_channelized_mapping_1_1_service_cleanup(unit);
*/

/* The channelized port mapping  for PON profile 0 and 1 
 * ucode port/local port is equal to system port.
 * ucode port/local port of PON channelized port 0 is equal to PON physical port.
 * ucode port/local port of PON channelized port 1 is euqal to PON physical port + 8.
 * PON pp port = PON_profile << 4 | local_port.
 */
/*
 * -----------------------------------------------------------------------------------------------------------
 * | PON profile | PON physical port |                PON channelized port 0               |               PON channelized port 1                |
 * |---------- |--------------- |------------------------------------------------------------------------------
 * |                 |                          | ucode port/local port | pp port| system port | ucode port/local port | pp port| system port |
 * |---------------------------------------------------------------------------------------------------------|
 * |      0          |             0           |              0               |     0     |        0        |             8                 |     8    |        8        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      0          |             1           |              1               |     1     |        1        |             9                 |     9    |        9        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      0          |             2           |              2               |     2     |        2        |             10               |    10    |      10        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      0          |             3           |              3               |     3     |        3        |             11               |    11    |      11        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      0          |             4           |              4               |     4     |        4        |             12               |    12    |      12        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      0          |             5           |              5               |     5     |        5        |             13               |    13    |      13        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      0          |             6           |              6               |     6     |        6        |             14               |    14    |      14        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      0          |             7           |              7               |     7     |        7        |             15               |    15    |      15        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |---------------------------------------------------------------------------------------------------------| 
 * |      1          |             0           |              0               |    16    |        0        |             8                 |    24    |        8        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      1          |             1           |              1               |    17    |        1        |             9                 |    25    |        9        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      1          |             2           |              2               |    18    |        2        |             10               |    26    |      10        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      1          |             3           |              3               |    19    |        3        |             11               |    27    |      11        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      1          |             4           |              4               |    20    |        4        |             12               |    28    |      12        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      1          |             5           |              5               |    21    |        5        |             13               |    29    |      13        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      1          |             6           |              6               |    22    |        6        |             14               |    30    |      14        |
 * |---------------------------------------------------------------------------------------------------------| 
 * |      1          |             7           |              7               |    23    |        7        |             15               |    31    |      15        |
 * |---------------------------------------------------------------------------------------------------------| 
*/

struct pon_tunnel_service_info_s{
    int service_mode;
    int up_lif_type;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    int down_lif_type;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
};

struct pon_vlan_range_info_s {
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t inner_vlan_high;
    bcm_vlan_t inner_vlan_low;
};

struct pon_tunnel_profile_info_s{
    bcm_tunnel_id_t tunnel_id[2]; /* tunnel_ids mapped to tunnel_profile_id, could be more */
    uint16 tunnel_profile_id;
    int tunnel_lookup_mode;
    int is_drop_all_packets;
    pon_vlan_range_info_s vlan_range[2]; /* vlan ranges set to pon_pp_port, could be more, but less than 32 */
    bcm_gport_t pon_pp_port[2];
};

bcm_port_t pon_port = 5;
bcm_port_t nni_port = 129;
int num_of_tunnel_profile = 0;
int num_of_tunnel_mapping = 0;

pon_tunnel_profile_info_s pon_tunnel_profile[3];
pon_tunnel_service_info_s pon_tunnel_mapping[18];

/* Init PON tunnel profile info */
int pon_tunnel_profile_init(int unit)
{
    int rv = 0;
    int index;
    
    /* Tunnel Profile 0
     * PON port double lookup
     * VLAN Range: outer VLAN[50, 60]
     */
    index = 0;
    pon_tunnel_profile[index].tunnel_id[0] = 1000;
    pon_tunnel_profile[index].tunnel_id[1] = 1001;
    pon_tunnel_profile[index].tunnel_profile_id = 0;
    pon_tunnel_profile[index].tunnel_lookup_mode = bcmVlanPortLookupTunnelEnable;
    pon_tunnel_profile[index].is_drop_all_packets = 0;
    pon_tunnel_profile[index].vlan_range[0].outer_vlan_high = 60;
    pon_tunnel_profile[index].vlan_range[0].outer_vlan_low = 50;
    pon_tunnel_profile[index].vlan_range[0].inner_vlan_high = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[0].inner_vlan_low = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[1].outer_vlan_high = 600;
    pon_tunnel_profile[index].vlan_range[1].outer_vlan_low = 500;
    pon_tunnel_profile[index].vlan_range[1].inner_vlan_high = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[1].inner_vlan_low = BCM_VLAN_INVALID;

    /* Tunnel Profile 1
     * PON port ignore inner packet tag
     * VLAN Range: outer VLAN[80, 90]
     */
    index++;
    pon_tunnel_profile[index].tunnel_id[0] = 1002;
    pon_tunnel_profile[index].tunnel_id[1] = 1003;
    pon_tunnel_profile[index].tunnel_profile_id = 1;
    pon_tunnel_profile[index].tunnel_lookup_mode = bcmVlanPortIgnoreInnerPktTag;
    pon_tunnel_profile[index].is_drop_all_packets = 0;
    pon_tunnel_profile[index].vlan_range[0].outer_vlan_high = 90;
    pon_tunnel_profile[index].vlan_range[0].outer_vlan_low = 80;
    pon_tunnel_profile[index].vlan_range[0].inner_vlan_high = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[0].inner_vlan_low = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[1].outer_vlan_high = 900;
    pon_tunnel_profile[index].vlan_range[1].outer_vlan_low = 800;
    pon_tunnel_profile[index].vlan_range[1].inner_vlan_high = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[1].inner_vlan_low = BCM_VLAN_INVALID;


    /* Tunnel Profile 2
     * PON port double lookup
     * VLAN Range: none
     * Drop all packets
     */
    index++;
    pon_tunnel_profile[index].tunnel_id[0] = 1004;
    pon_tunnel_profile[index].tunnel_id[1] = 1005;
    pon_tunnel_profile[index].tunnel_profile_id = 2;
    pon_tunnel_profile[index].tunnel_lookup_mode = bcmVlanPortLookupTunnelEnable;
    pon_tunnel_profile[index].is_drop_all_packets = 1;
    pon_tunnel_profile[index].vlan_range[0].outer_vlan_high = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[0].outer_vlan_low = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[0].inner_vlan_high = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[0].inner_vlan_low = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[1].outer_vlan_high = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[1].outer_vlan_low = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[1].inner_vlan_high = BCM_VLAN_INVALID;
    pon_tunnel_profile[index].vlan_range[1].inner_vlan_low = BCM_VLAN_INVALID;

    num_of_tunnel_profile = index + 1;

    return rv;
}

/* Set PON tunnel profile to ARAD */
int pon_tunnel_profile_set(int unit, bcm_port_t port_pon, int tunnel_index)
{
    int rv = 0;
    int index;
    int j;
    uint32 flags;
    bcm_gport_t tunnel_port_id;
    bcm_vlan_action_set_t action;

    for (index = 0; index < num_of_tunnel_profile; index++) {

        flags = BCM_PORT_PON_TUNNEL_WITH_ID;
        tunnel_port_id = pon_tunnel_profile[index].tunnel_profile_id;

        rv = bcm_port_pon_tunnel_add(unit, port_pon, flags, &tunnel_port_id);
        if (rv != BCM_E_NONE)
        {    
            printf("Error, bcm_port_pon_tunnel_add!\n");
            print rv;
            return rv;
        }
        
        /* Map tunnel_id to PON in pp Port */
        pon_tunnel_profile[index].pon_pp_port[tunnel_index] = tunnel_port_id;
        rv = bcm_port_pon_tunnel_map_set(unit, port_pon, pon_tunnel_profile[index].tunnel_id[tunnel_index], tunnel_port_id);
        if (rv != BCM_E_NONE)
        {    
            printf("Error, bcm_petra_port_pon_tunnel_map_set!\n");
            print rv;
            return rv;
        }

        if (is_advanced_vlan_translation_mode) {
            rv = pon_tpid_profile_init(unit, tunnel_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in pon_tpid_profile_init %s\n", bcm_errmsg(rv));
                print rv;
                return rv;
            }
        } else {
            /* Set TPIDs of PON in PP port. outer 0x8100, inner 0x9100 */
            port_tpid_init(tunnel_port_id, 1, 1);
            rv = port_tpid_set(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, port_tpid_set\n");
                print rv;
                return rv;
            }
            
            /* set inner tpids PON port to recognize single ctag frames */
            for(j = 0; j < port_tpid_info1.nof_inners; ++j){
                rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[j], 0);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[j]);
                    return rv;
                }
            }
        }

        /* Set PON PP PORT VLAN Domain */
        bcm_port_class_set(unit, tunnel_port_id, bcmPortClassId, tunnel_port_id);
        
        /* Enable additional port tunnel lookup or ignore 2nd VLAN tag in PON ports */
        rv = bcm_vlan_control_port_set(unit, tunnel_port_id, pon_tunnel_profile[index].tunnel_lookup_mode, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_port_set %s\n", bcm_errmsg(rv));
            print rv;
            return rv;
        }

        if (pon_tunnel_profile[index].is_drop_all_packets) {
            if (is_advanced_vlan_translation_mode) {
                rv = pon_port_discard_set(unit, tunnel_port_id, BCM_PORT_DISCARD_ALL);
                if (rv != BCM_E_NONE) {
                    printf("Error, in pon_port_discard_set %s\n", bcm_errmsg(rv));
                    print rv;
                    return rv;
                }
            } else {
                rv = bcm_port_discard_set(unit, tunnel_port_id, BCM_PORT_DISCARD_ALL);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_port_discard_set %s\n", bcm_errmsg(rv));
                    print rv;
                    return rv;
                }
            }
        }

        /* Set VLAN range to PON_in_pp Port */
        bcm_vlan_action_set_t_init(&action);
        if ((pon_tunnel_profile[index].vlan_range[tunnel_index].outer_vlan_low != BCM_VLAN_INVALID) &&
            (pon_tunnel_profile[index].vlan_range[tunnel_index].inner_vlan_low != BCM_VLAN_INVALID)) {
            action.dt_outer = bcmVlanActionCompressed;
            action.new_outer_vlan = pon_tunnel_profile[index].vlan_range[tunnel_index].outer_vlan_low;
            action.dt_inner = bcmVlanActionCompressed;
            action.new_inner_vlan = pon_tunnel_profile[index].vlan_range[tunnel_index].inner_vlan_low;                
        } else if (pon_tunnel_profile[index].vlan_range[tunnel_index].outer_vlan_low != BCM_VLAN_INVALID) {
            action.ot_outer = bcmVlanActionCompressed;
            action.new_outer_vlan = pon_tunnel_profile[index].vlan_range[tunnel_index].outer_vlan_low;
        } else if (pon_tunnel_profile[index].vlan_range[tunnel_index].inner_vlan_low != BCM_VLAN_INVALID) {
            action.it_inner = bcmVlanActionCompressed;
            action.new_outer_vlan = pon_tunnel_profile[index].vlan_range[tunnel_index].inner_vlan_low;
        } else {
            continue;
        }

        rv = bcm_vlan_translate_action_range_add(
                     unit, 
                     tunnel_port_id, 
                     pon_tunnel_profile[index].vlan_range[tunnel_index].outer_vlan_low, 
                     pon_tunnel_profile[index].vlan_range[tunnel_index].outer_vlan_high, 
                     pon_tunnel_profile[index].vlan_range[tunnel_index].inner_vlan_low, 
                     pon_tunnel_profile[index].vlan_range[tunnel_index].inner_vlan_high, 
                     &action);
        if (rv != BCM_E_NONE)
        {
            printf("error, bcm_vlan_translate_action_range_add!\n");
            print rv;
            return rv;
        }
    }

    return rv;
}

int pon_pp_port_get(bcm_tunnel_id_t tunnel_id, bcm_gport_t *pon_pp_port)
{
    int i,j;

    for (i = 0; i < num_of_tunnel_profile; i++) {
        for (j = 0; j < 2; j++) {
            if (pon_tunnel_profile[i].tunnel_id[j] == tunnel_id) {
                *pon_pp_port = pon_tunnel_profile[i].pon_pp_port[j];
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_NOT_FOUND;
}

/* Init pon tunnel service info */
int pon_tunnel_channelized_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index;
    uint32 flags;

    pon_port = port_pon;
    nni_port = port_nni;

    /* PON Port 5 Tunnel-ID 1000 SVLAN 20 <-CrossConnect-> NNI Port 129 SVLAN 200*/
    index = 0;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1000;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan   = 20;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 200;

    /* PON Port 5 Tunnel-ID 1000 SVLAN 20 CVLAN 21 <-CrossConnect-> NNI Port 129 SVLAN 201 CVLAN 21*/
    index++;
    pon_tunnel_mapping[index].service_mode = o_i_tag_to_o2_i_tag;
    pon_tunnel_mapping[index].tunnel_id  = 1000;
    pon_tunnel_mapping[index].up_lif_type = match_o_i_tag;
    pon_tunnel_mapping[index].up_ovlan   = 20;
    pon_tunnel_mapping[index].up_ivlan   = 21;
    pon_tunnel_mapping[index].down_lif_type = match_o_i_tag;
    pon_tunnel_mapping[index].down_ovlan = 201;
    pon_tunnel_mapping[index].down_ivlan = 21;

    /* PON Port 5 Tunnel-ID 1000 SVLAN range 50~60 <-CrossConnect-> NNI Port 129 SVLAN 70 */
    index++;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1000;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan   = 50;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 70;

    /* PON Port 5 Tunnel-ID 1001 SVLAN 20 <-CrossConnect-> NNI Port 129 SVLAN 210*/
    index++;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1001;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan   = 20;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 210;

    /* PON Port 5 Tunnel-ID 1001 SVLAN 20 CVLAN 21 <-CrossConnect-> NNI Port 129 SVLAN 202 CVLAN 21*/
    index++;
    pon_tunnel_mapping[index].service_mode = o_i_tag_to_o2_i_tag;
    pon_tunnel_mapping[index].tunnel_id  = 1001;
    pon_tunnel_mapping[index].up_lif_type = match_o_i_tag;
    pon_tunnel_mapping[index].up_ovlan   = 20;
    pon_tunnel_mapping[index].up_ivlan   = 21;
    pon_tunnel_mapping[index].down_lif_type = match_o_i_tag;
    pon_tunnel_mapping[index].down_ovlan = 202;
    pon_tunnel_mapping[index].down_ivlan = 21;

    /* PON Port 5 Tunnel-ID 1001 SVLAN range 50~60 <-CrossConnect-> NNI Port 129 SVLAN 500 */
    index++;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1001;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan   = 50;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 500;

    /* PON Port 5 Tunnel-ID 1002 SVLAN 30 <-CrossConnect-> NNI Port 129 SVLAN 300*/
    index++;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1002;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan   = 30;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 300;

    /* PON Port 5 Tunnel-ID 1002 SVLAN 30 CVLAN 31 <-CrossConnect-> NNI Port 129 SVLAN 303 CVLAN 31*/
    index++;
    pon_tunnel_mapping[index].service_mode = o_i_tag_to_o2_i_tag;
    pon_tunnel_mapping[index].tunnel_id  = 1002;
    pon_tunnel_mapping[index].up_lif_type = match_o_i_tag;
    pon_tunnel_mapping[index].up_ovlan   = 30;
    pon_tunnel_mapping[index].up_ivlan   = 31;
    pon_tunnel_mapping[index].down_lif_type = match_o_i_tag;
    pon_tunnel_mapping[index].down_ovlan = 303;
    pon_tunnel_mapping[index].down_ivlan = 31;

    /* PON Port 5 Tunnel-ID 1002 SVLAN range 80~90 <-CrossConnect-> NNI Port 129 SVLAN 800 */
    index++;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1002;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan = 80;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 800;

    /* PON Port 5 Tunnel-ID 1003 SVLAN 30 <-CrossConnect-> NNI Port 129 SVLAN 310*/
    index++;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1003;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan   = 30;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 310;

    /* PON Port 5 Tunnel-ID 1003 SVLAN 30 CVLAN 31 <-CrossConnect-> NNI Port 129 SVLAN 304 CVLAN 31*/
    index++;
    pon_tunnel_mapping[index].service_mode = o_i_tag_to_o2_i_tag;
    pon_tunnel_mapping[index].tunnel_id  = 1003;
    pon_tunnel_mapping[index].up_lif_type = match_o_i_tag;
    pon_tunnel_mapping[index].up_ovlan   = 30;
    pon_tunnel_mapping[index].up_ivlan   = 31;
    pon_tunnel_mapping[index].down_lif_type = match_o_i_tag;
    pon_tunnel_mapping[index].down_ovlan = 304;
    pon_tunnel_mapping[index].down_ivlan = 31;

    /* PON Port 5 Tunnel-ID 1003 SVLAN range 80~90 <-CrossConnect-> NNI Port 129 SVLAN 1000 */
    index++;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1003;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan = 80;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 1000;


    /* PON Port 5 Tunnel-ID 1004 SVLAN 40 <-CrossConnect-> NNI Port 129 SVLAN 400*/
    index++;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1004;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan   = 40;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 400;

    /* PON Port 5 Tunnel-ID 1005 SVLAN 40 <-CrossConnect-> NNI Port 129 SVLAN 410*/
    index++;
    pon_tunnel_mapping[index].service_mode = otag_to_otag2;
    pon_tunnel_mapping[index].tunnel_id  = 1005;
    pon_tunnel_mapping[index].up_lif_type = match_otag;
    pon_tunnel_mapping[index].up_ovlan   = 40;
    pon_tunnel_mapping[index].down_lif_type = match_otag;
    pon_tunnel_mapping[index].down_ovlan = 410;

    num_of_tunnel_mapping = index+1;

    pon_app_init(unit, port_pon, port_nni, 0, 0);

    pon_tunnel_profile_init(unit);

    pon_tunnel_profile_set(unit, port_pon, 0);
    pon_tunnel_profile_set(unit, port_pon+8, 1);


    return 0;
}

/*
 * Set up 1:1 sercies, using port cross connect.
 */
int pon_tunnel_channelized_mapping_1_1_service(int unit)
{
    int rv;
    int index, is_with_id = 0;
    bcm_vswitch_cross_connect_t gports;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport, pon_pp_port;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;

    for (index = 0; index < num_of_tunnel_mapping; index++)
    {
        service_mode  = pon_tunnel_mapping[index].service_mode;
        tunnel_id     = pon_tunnel_mapping[index].tunnel_id;
        pon_lif_type  = pon_tunnel_mapping[index].up_lif_type;
        up_ovlan      = pon_tunnel_mapping[index].up_ovlan;
        up_ivlan      = pon_tunnel_mapping[index].up_ivlan;
        nni_lif_type  = pon_tunnel_mapping[index].down_lif_type;
        down_ovlan    = pon_tunnel_mapping[index].down_ovlan;
        down_ivlan    = pon_tunnel_mapping[index].down_ivlan;

        pon_gport = 0;
        nni_gport = 0;

        /* Get pon_pp_port by tunnel_id */
        pon_pp_port_get(tunnel_id, &pon_pp_port);

        /* Create PON LIF */        
        pon_lif_create(unit, pon_pp_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, 0, 0, &pon_gport);

        /* Set PON LIF ingress VLAN editor */
        pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL);

        /* Set PON LIF egress VLAN editor */
        pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);

        /* Create NNI LIF */
        nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);

        pon_tunnel_mapping[index].pon_gport = pon_gport;
        pon_tunnel_mapping[index].nni_gport = nni_gport;

        /* Cross connect the 2 LIFs */
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.port1 = pon_gport;
        gports.port2 = nni_gport;
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            print rv;
            return rv;
        }
    }

    return rv;
}

/*
 * clean up the configurations of 1:1 sercies.
 */
int pon_tunnel_channelized_mapping_1_1_service_cleanup(int unit)
{
    int rv;
    int index;    
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_svlan_high;
    bcm_vswitch_cross_connect_t gports;

    for (index = 0; index < num_of_tunnel_mapping; index++)
    {   
        /* Delete the cross connected LIFs */
        gports.port1 = pon_tunnel_mapping[index].pon_gport;
        gports.port2 = pon_tunnel_mapping[index].nni_gport;
        rv = bcm_vswitch_cross_connect_delete(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_delete\n");
            print rv;
            return rv;
        }
        
        /* Delete PON LIF */
        rv = bcm_vlan_port_destroy(unit, pon_tunnel_mapping[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }

        /* Delete NNI LIF */
        rv = bcm_vlan_port_destroy(unit, pon_tunnel_mapping[index].nni_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }
    }

    return rv;        
}


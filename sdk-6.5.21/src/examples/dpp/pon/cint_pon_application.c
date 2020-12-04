/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_pon_application.c
* Purpose: An example of the PON application. 
*          The following CINT provides a calling sequence example to bring up three main PON services categories: 1:1 Model , N:1 Model and TLS. 
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
*  1. Add the following port configureations to config-sand.bcm
*        ucode_port_129.BCM88650=10GBase-R15
*        ucode_port_5.BCM88650=10GBase-R14
*  2. Add the following PON application enabling configureations to config-sand.bcm
*        pon_application_support_enabled_5.BCM88650=TRUE
*        vlan_match_criteria_mode=PON_PCP_ETHERTYPE
*  3. Set PON port and NNI port to recognize single stag, single ctag and s_c_tag frames.
*        - call port_tpid_init()
*        - call bcm_port_tpid_add()
*  4. Remove Ports from VLAN 1.
*        - call bcm_vlan_gport_delete_all()
*  5. Disable membership in PON ports (SDK initialization already disabled membership in NNI ports).
*        - call bcm_port_vlan_member_set()
*  6. Enable additional port tunnel lookup in PON ports.
*        - call bcm_vlan_control_port_set()
*
* 1:1 Service:
* Set up sequence:
*  1. Create PON LIF
*        - Call bcm_vlan_port_create() with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED, 
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE,
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE
*  2. Set PON LIF ingress VLAN editor.
*        - Call bcm_vlan_translate_action_create()
*  3. Set PON LIF egress VLAN editor.
*        - Call bcm_vlan_translate_egress_action_add()
*  4. Create NNI LIF
*        - Call bcm_vlan_port_create()with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
*  4. Cross connect the 2 LIFs
*        - Call bcm_vswitch_cross_connect_add()
* Clean up sequence:
*  1. Delete the cross connected LIFs.
*        - Call bcm_vswitch_cross_connect_delete()
*  2. Delete PON LIFs.
*        - Call bcm_vlan_port_destroy()
*  3. Delete NNI LIFs.
*        - Call bcm_vlan_port_destroy()
*
* N:1 Service:
* Set up sequence:
*  1. Create vswitch for basic bridging
*        - Call bcm_vswitch_create()
*        - Call bcm_multicast_create() 
*  2. Create PON LIF
*        - Call bcm_vlan_port_create() with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED, 
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE,
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE
*  3. Add PON LIF to vswitch
*        - Call bcm_vswitch_port_add()
*  4. Set multicast settings,Add PON-LIFs to MC-ID = VSI + 16K
*        - Call bcm_multicast_vlan_encap_get()
*        - Call bcm_multicast_ingress_add()
*  5. Set PON LIF ingress VLAN editor.
*        - Call bcm_vlan_translate_action_create()
*  6. Set PON LIF egress VLAN editor.
*        - Call bcm_vlan_translate_egress_action_add()
*  7. Create NNI LIF only once
*        - Call bcm_vlan_port_create()with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED           
*  8. Add NNI LIF to vswitch
*        - Call bcm_vswitch_port_add()
*  9. Set multicast settings, Add NNI-LIF to MC-ID = VSI
*        - Call bcm_multicast_vlan_encap_get()
*        - Call bcm_multicast_ingress_add()
*  10.Set the multicast group offset to flood downstream packets in multicast group
*        - Call bcm_port_control_set (NNI-LIF, offset= X constant)
* Clean up sequence:
*  1. Remove PON and NNI LIFs from VSI.
*        - Call bcm_vswitch_port_delete()
*  2. Delete PON and NNI LIFs.
*        - Call bcm_vlan_port_destroy()
*
* TLS Service:
* Set up sequence:
*  1. Create PON LIF
*        - Call bcm_vlan_port_create() with criteria BCM_VLAN_PORT_MATCH_PORT_TUNNEL and flag BCM_VLAN_PORT_TRANSPARENT.
*  2. Add PON LIF to vswitch
*        - Call bcm_vswitch_port_add()
*  3. Set multicast settings,Add PON-LIFs to MC-ID = VSI + 16K
*        - Call bcm_multicast_vlan_encap_get()
*        - Call bcm_multicast_ingress_add()
*  4. Set PON LIF ingress VLAN editor.
*        - Call bcm_vlan_translate_action_create()
*  5. Set PON LIF egress VLAN editor.
*        - Call bcm_vlan_translate_egress_action_add()
*  6. Create NNI LIF
*        - Call bcm_vlan_port_create()with criterias BCM_VLAN_PORT_MATCH_PORT.
*  7. Add NNI LIF to vswitch
*        - Call bcm_vswitch_port_add()
*  8. Set multicast settings, Add NNI-LIF to MC-ID = VSI
*        - Call bcm_multicast_vlan_encap_get()
*        - Call bcm_multicast_ingress_add()
*  9. Set the multicast group offset to flood downstream packets in multicast group
*        - Call bcm_port_control_set (NNI-LIF, offset= X constant)
* Clean up sequence:
*  1. Remove PON and NNI LIFs from VSI.
*        - Call bcm_vswitch_port_delete()
*  2. Delete PON and NNI LIFs.
*        - Call bcm_vlan_port_destroy()
*  3. Destroy the multicast groups.
*        - Call bcm_multicast_destroy()
*  4. Destroy the VSI.
*        - Call bcm_vswitch_destroy()
*
* Service Model:
* 1:1 Service:
*     N(1) PON Port 5  <-----------------------------  CrossConnect  ------------> 1 NNI Port 129
*     Tunnel-ID 1000 CVLAN 20 <--------------------------------------------------> CVLAN 200
*     Tunnel-ID 1001 SVLAN 30 CVLAN 31 <-----------------------------------------> SVLAN 300 CVLAN 31
*     Tunnel-ID 1002 PCP 2 CVLAN 40 <--------------------------------------------> CVLAN 400
*     Tunnel-ID 1003 PCP 3 SVLAN 50 CVLAN 51 <-----------------------------------> SVLAN 500 CVLAN 51
*     Tunnel-ID 1004 PCP 4 CVLAN 60 ETHERTYPE 0x0899 <---------------------------> CVLAN 600
*     Tunnel-ID 1005 PCP 5 SVLAN 70 CVLAN 71 ETHERTYPE 0x0899 <------------------> SVLAN 700 CVLAN 71
*     Tunnel-ID 1006 ETHERTYPE 0x0899 <------------------------------------------> CVLAN 900
*     Tunnel-ID 1007 CVLAN 1100 ETHERTYPE 0x0899 <-------------------------------> CVLAN 1200
*     Tunnel-ID 1008 SVLAN 1300 CVLAN 1400 ETHERTYPE 0x0899 <--------------------> CVLAN 1500
*     Tunnel-ID 1009 CVLAN 34 CVLAN 35 <----------------------------------------> SVLAN 304 CVLAN 305
* Traffic:
*  1. PON Port 5 Tunnel-ID 1000 CVLAN 20 <-CrossConnect-> NNI Port 129 CVLAN 200
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x9100, VID = 20
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x9100, VID = 200
*
*  2. PON Port 5 Tunnel-ID 1001 SVLAN 30 CVLAN 31 <-CrossConnect-> NNI Port 129 SVLAN 300 CVLAN 31
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1001
*              -   VLAN tag: VLAN tag type 0x8100, VID = 30, VLAN tag type 0x9100, VID = 31
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 300,VLAN tag type 0x9100, VID = 31
*
*  3. PON Port 5 Tunnel-ID 1002 PCP 2 CVLAN 40 <-CrossConnect-> NNI Port 129 CVLAN 400
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1002
*              -   VLAN tag: VLAN tag type 0x9100, VID = 40
*              -   PCP:2
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x9100, VID = 400
*
*  4. PON Port 5 Tunnel-ID 1003 PCP 3 SVLAN 50 CVLAN 51 <-CrossConnect-> NNI Port 129 SVLAN 500 CVLAN 51
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1003
*              -   VLAN tag: VLAN tag type 0x8100, VID = 50, VLAN tag type 0x9100, VID = 51
*              -   PCP:3
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 500,VLAN tag type 0x9100, VID = 51
*
*  5. PON Port 5 Tunnel-ID 1004 PCP 4 CVLAN 60 ETHERTYPE 0x0899 <-CrossConnect-> NNI Port 129 CVLAN 600
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1005
*              -   VLAN tag: VLAN tag type 0x9100, VID = 60
*              -   PCP:3
*              -   ETHERTYPE:0x0899
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x9100, VID = 600
*
*  6. PON Port 5 Tunnel-ID 1005 PCP 5 SVLAN 70 CVLAN 71 ETHERTYPE 0x0899 <-CrossConnect-> NNI Port 129 SVLAN 700 CVLAN 71
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1005
*              -   VLAN tag: VLAN tag type 0x8100, VID = 70, VLAN tag type 0x9100, VID = 71
*              -   PCP:5
*              -   ETHERTYPE:0x0899
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 700,VLAN tag type 0x9100, VID = 71
*
*  7. PON Port 5 Tunnel-ID 1006 ETHERTYPE 0x0899 <-CrossConnect-> NNI Port 129 CVLAN 900
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1006
*              -   ETHERTYPE:0x0899
*        - From NNI port:
*              -   ethernet header with any DA, SA
*              -   VLAN tag: VLAN tag type 0x9100, VID = 900
*
*  8. PON Port 5 Tunnel-ID 1007 CVLAN 1100 ETHERTYPE 0x0899 <-CrossConnect-> NNI Port 129 CVLAN 1200
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1007
*              -   VLAN tag: VLAN tag type 0x9100, VID = 1100
*              -   ETHERTYPE:0x0899
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x9100, VID = 1200
*
*  9. PON Port 5 Tunnel-ID 1008 SVLAN 1300 CVLAN 1400 ETHERTYPE 0x0899 <-CrossConnect-> NNI Port 129 CVLAN 1500
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1005
*              -   VLAN tag: VLAN tag type 0x8100, VID = 1300, VLAN tag type 0x9100, VID = 1400
*              -   ETHERTYPE:0x0899
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x9100, VID = 1500
*  a. PON Port 5 Tunnel-ID 1009 CVLAN 34 CVLAN 35 <-CrossConnect-> NNI Port 304 CVLAN 305
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1005
*              -   VLAN tag: VLAN tag type 0x9100, VID = 34, VLAN tag type 0x9100, VID = 35
*              -   ETHERTYPE:0x0899
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 304, VLAN tag type 0x9100, VID = 305
*
* N:1 Service:
*     N(6) PON Port 5  <--------------------------->  VSI  <---> 1 NNI Port 129
*     Tunnel-ID 1000 ----------------------------|
*     Tunnel-ID 1000 CVLAN 80   -----------------|
*     Tunnel-ID 1000 CVLAN 81   -----------------|--- 4096  ---  SVLAN 800 CVLAN 801
*     Tunnel-ID 1000 CVLAN 82   -----------------|
*     Tunnel-ID 1000 SVLAN 80 CVLAN 801   -------|
* Traffic:
*  1. N(6-1) PON Port 5 Tunnel-ID 1000 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801
*        - From PON port:
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:02
*              -   Tunnel ID:1000
*              -   VLAN tag: no VLAN tag
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 800, VLAN tag type 0x9100, VID = 801
*
*  2. N(6-2) PON Port 5 Tunnel-ID 1000 CVLAN 80 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801
*        - From PON port:
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:03
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x9100, VID = 80
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:03, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 800, VLAN tag type 0x9100, VID = 801
*
*  3. N(6-3) PON Port 5 Tunnel-ID 1000 CVLAN 81 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801
*        - From PON port:
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:04
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x9100, VID = 81
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:04, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 800, VLAN tag type 0x9100, VID = 801
*
*  4. N(6-4) PON Port 5 Tunnel-ID 1000 CVLAN 82 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801
*        - From PON port:
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:05
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x9100, VID = 82
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:05, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 800, VLAN tag type 0x9100, VID = 801
*
*  5. N(6-5) PON Port 5 Tunnel-ID 1000 SVLAN 80 CVLAN 801 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801
*        - From PON port:
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:06
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 80, VLAN tag type 0x9100, VID = 801
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:06, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 800, VLAN tag type 0x9100, VID = 801
*
* TLS Service:
*     PON Port 5  <--------------------------->  VSI  <---> NNI Port 129
*     Tunnel-ID 1000 CVLAN 45   --------------- 4097 -----  CVLAN 45
*     Tunnel-ID 1000 SVLAN 55 CVLAN 65   ------ 4097 -----  SVLAN 55 CVLAN 65
* Traffic:
*  1. PON Port 5 Tunnel-ID 1000 CVLAN 45 <-> VSI 4097 <-> NNI Port 129 CVLAN 45
*        - From PON port:
*              -   ethernet header with DA 00:00:00:00:00:11, SA 00:00:00:00:00:22
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x9100, VID = 45
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:22, SA 00:00:00:00:00:11
*              -   VLAN tag: VLAN tag type 0x9100, VID = 45
*
*  2. PON Port 5 Tunnel-ID 1000 SVLAN 55 CVLAN 65  <-> VSI 4097 <-> NNI Port 129 SVLAN 55 CVLAN 65 
*        - From PON port:
*              -   ethernet header with DA 00:00:00:00:00:33, SA 00:00:00:00:00:44
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 55, VLAN tag type 0x9100, VID = 65
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:44, SA 00:00:00:00:00:33
*              -   VLAN tag: VLAN tag type 0x8100, VID = 55, VLAN tag type 0x9100, VID = 65
*
* 3 tag  manipulation Service:
*     PON Port 5  <--------------------------->  VSI  <---> NNI Port 129
*     Tunnel-ID 1000 SVLAN 100 CVLAN 10 ------- 4097 -----  SVLAN 200 CVLAN 20
*
* Traffic:
*  1. PON Port 5 Tunnel-ID 1000 SVLAN 100 CVLAN 10 <-> VSI 4097 <-> NNI Port 129 SVLAN 200 CVLAN 20
*        - From PON port:
*              -   ethernet header with DA 00:00:00:00:00:11, SA 00:00:00:00:00:22
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100, VLAN tag type 0x9100, VID = 10
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:22, SA 00:00:00:00:00:11
*              -   VLAN tag: VLAN tag type 0x8100, VID = 200, VLAN tag type 0x9100, VID = 20

*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_tpid.c
*      BCM> cint examples/dpp/cint_pon_utils.c
*      BCM> cint examples/dpp/cint_pon_application.c
*      BCM> cint
*      cint> pon_service_init(unit, pon_port, nni_port);
*      cint> pon_1_1_service(unit);
*      cint> pon_n_1_service(unit);
*      cint> pon_tls_service(unit);
*      cint> pon_3tag_manipulation_service(unit);
*      cint> nni_double_lookup_service(unit);
*      cint> pon_1_1_service_cleanup(unit);
*      cint> pon_n_1_service_cleanup(unit);
*      cint> pon_tls_service_cleanup(unit);
*      cint> nni_double_lookup_service_cleanup(unit);
*      cint> pon_3tag_manipulation_service_cleanup(unit);
*/

struct pon_service_info_s{
    int service_mode;
    int up_lif_type;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    int down_lif_type;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_vlan_t down_pcp;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
};

pon_service_info_s pon_1_1_service_info[11];
pon_service_info_s pon_n_1_service_info[7];
pon_service_info_s pon_tls_service_info;
pon_service_info_s pon_lif_lkup_service_info[3];
pon_service_info_s pon_3tag_manipulation_service_info;
bcm_vswitch_cross_connect_t nni_double_lkup_gports;

bcm_port_t pon_port = 5;
bcm_port_t nni_port = 129;
bcm_vlan_t n_1_service_vsi;   /*VSI for N:1 services*/
bcm_vlan_t tls_service_vsi;   /*VSI for TLS services*/
bcm_vlan_t manipulation_service_vsi;   /*VSI for 3 tag manipulation services*/
int nof_1_1_services = 0;
int nof_n_1_services = 0;
int nof_lif_lkup_services = 0;
int verbose = 0;

/*
 *Initialize the service models and set up PON application configurations.
 */
int pon_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index;
    int tunnel_base = 0;

    pon_port = port_pon;
    nni_port = port_nni;

    if(soc_property_get(unit, spn_PON_PP_PORT_MAPPING_BYPASS, 0)){
        tunnel_base = 0x8000;
    }

   /* PON lif lkup services*/
    if(soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"pon_lif_lkup",0)){
        /* PON Port 5 Tunnel-ID 1000 SVLAN 30 CVLAN 31<-CrossConnect-> NNI Port 129 SVLAN 500 CVLAN 31*/
        index = 0;
        pon_lif_lkup_service_info[index].service_mode = o_i_tag_to_o2_i_tag;
        pon_lif_lkup_service_info[index].tunnel_id  = tunnel_base+1000;
        pon_lif_lkup_service_info[index].up_lif_type = match_otag;
        pon_lif_lkup_service_info[index].up_ovlan   = 30;
        pon_lif_lkup_service_info[index].down_lif_type = match_otag;
        pon_lif_lkup_service_info[index].down_ovlan = 500;
        pon_lif_lkup_service_info[index].down_ivlan = 31;
        
        /* PON Port 5 Tunnel-ID 1000 SVLAN 30 CVLAN 31 <-CrossConnect-> NNI Port 129 SVLAN 600 CVLAN 31*/
        index++;
        pon_lif_lkup_service_info[index].service_mode = o_i_tag_to_o2_i_tag;
        pon_lif_lkup_service_info[index].tunnel_id  = tunnel_base+1000;
        pon_lif_lkup_service_info[index].up_lif_type = match_o_i_tag;
        pon_lif_lkup_service_info[index].up_ovlan   = 30;
        pon_lif_lkup_service_info[index].up_ivlan   = 31;
        pon_lif_lkup_service_info[index].down_lif_type = match_o_i_tag;
        pon_lif_lkup_service_info[index].down_ovlan = 600;
        pon_lif_lkup_service_info[index].down_ivlan = 31;
    }

    /*count number of lif_lkup services*/
    nof_lif_lkup_services = index + 1;

    /*1:1 services*/
    /* PON Port 5 Tunnel-ID 1000 CVLAN 20 <-CrossConnect-> NNI Port 129 CVLAN 200*/
    index = 0;
    pon_1_1_service_info[index].service_mode = otag_to_otag2;
    pon_1_1_service_info[index].tunnel_id  = tunnel_base+1000;
    pon_1_1_service_info[index].up_lif_type = match_otag;
    pon_1_1_service_info[index].up_ovlan   = 20;
    pon_1_1_service_info[index].down_lif_type = match_otag;
    pon_1_1_service_info[index].down_ovlan = 200;

    /* PON Port 5 Tunnel-ID 1001 SVLAN 30 CVLAN 31 <-CrossConnect-> NNI Port 129 SVLAN 300 CVLAN 31*/
    index++;
    pon_1_1_service_info[index].service_mode = o_i_tag_to_o2_i_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_base+1001;
    pon_1_1_service_info[index].up_lif_type = match_o_i_tag;
    pon_1_1_service_info[index].up_ovlan   = 30;
    pon_1_1_service_info[index].up_ivlan   = 31;
    pon_1_1_service_info[index].down_lif_type = match_o_i_tag;
    pon_1_1_service_info[index].down_ovlan = 300;
    pon_1_1_service_info[index].down_ivlan = 31;

    /* PON Port 5 Tunnel-ID 1002 PCP 2 CVLAN 40 <-CrossConnect-> NNI Port 129 PCP 2 CVLAN 400*/
    index++;
    pon_1_1_service_info[index].service_mode = otag_to_otag2;
    pon_1_1_service_info[index].tunnel_id  = tunnel_base+1002;
    pon_1_1_service_info[index].up_lif_type = match_otag_cos;
    pon_1_1_service_info[index].up_pcp     = 2;
    pon_1_1_service_info[index].up_ovlan   = 40;
    pon_1_1_service_info[index].down_lif_type = match_otag;
    pon_1_1_service_info[index].down_ovlan = 400;

    /* PON Port 5 Tunnel-ID 1003 PCP 3 SVLAN 50 CVLAN 51 <-CrossConnect-> NNI Port 129 SVLAN 500 CVLAN 51*/
    index++;
    pon_1_1_service_info[index].service_mode = o_i_tag_to_o2_i_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_base+1003;
    pon_1_1_service_info[index].up_lif_type = match_o_i_tag_cos;
    pon_1_1_service_info[index].up_pcp     = 3;
    pon_1_1_service_info[index].up_ovlan   = 50;
    pon_1_1_service_info[index].up_ivlan   = 51;
    pon_1_1_service_info[index].down_lif_type = match_o_i_tag;
    pon_1_1_service_info[index].down_ovlan = 500;
    pon_1_1_service_info[index].down_ivlan = 51;

    /* PON Port 5 Tunnel-ID 1004 PCP 4 CVLAN 60 ETHERTYPE 0x0899 <-CrossConnect-> NNI Port 129 CVLAN 600*/
    index++;
    pon_1_1_service_info[index].service_mode = otag_to_otag2;
    pon_1_1_service_info[index].tunnel_id    = tunnel_base+1004;
    pon_1_1_service_info[index].up_lif_type = match_otag_cos_ethertype;
    pon_1_1_service_info[index].up_pcp       = 4;
    pon_1_1_service_info[index].up_ovlan     = 60;
    pon_1_1_service_info[index].up_ethertype = 0x0899;
    pon_1_1_service_info[index].down_lif_type = match_otag;
    pon_1_1_service_info[index].down_ovlan   = 600;

    /* PON Port 5 Tunnel-ID 1005 PCP 5 SVLAN 70 CVLAN 71 ETHERTYPE 0x0899 <-CrossConnect-> NNI Port 129 SVLAN 700 CVLAN 71*/
    index++;
    pon_1_1_service_info[index].service_mode = o_i_tag_to_o2_i_tag;
    pon_1_1_service_info[index].tunnel_id    = tunnel_base+1005;
    pon_1_1_service_info[index].up_lif_type = match_o_i_tag_cos_ethertype;
    pon_1_1_service_info[index].up_pcp       = 5;
    pon_1_1_service_info[index].up_ovlan     = 70;
    pon_1_1_service_info[index].up_ivlan     = 71;
    pon_1_1_service_info[index].up_ethertype = 0x0899;
    pon_1_1_service_info[index].down_lif_type = match_o_i_tag;
    pon_1_1_service_info[index].down_ovlan   = 700;
    pon_1_1_service_info[index].down_ivlan   = 71;

    /* PON Port 5 Tunnel-ID 1006 ETHERTYPE 0x0899 <-CrossConnect-> NNI Port 129 CVLAN 900*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_otag;
    pon_1_1_service_info[index].tunnel_id    = tunnel_base+1006;
    pon_1_1_service_info[index].up_lif_type = match_ethertype;
    pon_1_1_service_info[index].up_ethertype = 0x0899;
    pon_1_1_service_info[index].down_lif_type = match_otag;
    pon_1_1_service_info[index].down_ovlan   = 900;

    /* PON Port 5 Tunnel-ID 1007 ETHERTYPE 0x0899 CVLAN 1100<-CrossConnect-> NNI Port 129 CVLAN 1200*/
    index++;
    pon_1_1_service_info[index].service_mode = otag_to_otag2;
    pon_1_1_service_info[index].tunnel_id    = tunnel_base+1007;
    pon_1_1_service_info[index].up_lif_type = match_otag_ethertype;
    pon_1_1_service_info[index].up_ovlan     = 1100;
    pon_1_1_service_info[index].up_ethertype = 0x0899;
    pon_1_1_service_info[index].down_lif_type = match_otag;
    pon_1_1_service_info[index].down_ovlan   = 1200;

    /* PON Port 5 Tunnel-ID 1008 SVLAN 1300 CVLAN 1400 ETHERTYPE 0x0899 <-CrossConnect-> NNI Port 129 SVLAN 1500 CVLAN 1600*/
    index++;
    pon_1_1_service_info[index].service_mode = o_i_tag_to_o2_i_tag;
    pon_1_1_service_info[index].tunnel_id    = tunnel_base+1008;
    pon_1_1_service_info[index].up_lif_type = match_o_i_tag_ethertype;
    pon_1_1_service_info[index].up_ovlan     = 1300;
    pon_1_1_service_info[index].up_ivlan     = 1400;
    pon_1_1_service_info[index].up_ethertype = 0x0899;
    pon_1_1_service_info[index].down_lif_type = match_o_i_tag;
    pon_1_1_service_info[index].down_ovlan   = 1500;
    pon_1_1_service_info[index].down_ivlan   = 1400;

    /* PON Port 5 Tunnel-ID 1009 CVLAN 34 CVLAN 35 <-CrossConnect-> NNI Port 129 SVLAN 304 CVLAN 305*/
    if (is_advanced_vlan_translation_mode) {
        index++;
        pon_1_1_service_info[index].service_mode = o_i_tag_to_o2_i2_tag_2;
        pon_1_1_service_info[index].tunnel_id  = tunnel_base+1009;
        pon_1_1_service_info[index].up_lif_type = match_t_o_i_tag;
        pon_1_1_service_info[index].up_ovlan   = 34;
        pon_1_1_service_info[index].up_ivlan   = 35;
        pon_1_1_service_info[index].down_lif_type = match_o_i_tag;
        pon_1_1_service_info[index].down_ovlan = 304;
        pon_1_1_service_info[index].down_ivlan = 305;       
    }

    /*count number of 1_1 services*/
    nof_1_1_services = index + 1;

    /*N:1 services*/
    /* N(6-1) PON Port 5 Tunnel-ID 1000 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801 */
    index = 0;
    pon_n_1_service_info[index].service_mode = untag_to_o_i_tag;
    pon_n_1_service_info[index].tunnel_id = tunnel_base+1000;
    pon_n_1_service_info[index].up_lif_type = match_untag;
    pon_n_1_service_info[index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 801;

    /* N(6-2) PON Port 5 Tunnel-ID 1000 CVLAN 80 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801 */
    index++;
    pon_n_1_service_info[index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[index].tunnel_id = tunnel_base+1000;
    pon_n_1_service_info[index].up_lif_type = match_otag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 801;

    /* N(6-3) PON Port 5 Tunnel-ID 1000 CVLAN 81 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801 */
    index++;
    pon_n_1_service_info[index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[index].tunnel_id = tunnel_base+1000;
    pon_n_1_service_info[index].up_lif_type = match_otag;
    pon_n_1_service_info[index].up_ovlan = 81;
    pon_n_1_service_info[index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 801;

    /* N(6-4) PON Port 5 Tunnel-ID 1000 CVLAN 82 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801 */
    index++;
    pon_n_1_service_info[index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[index].tunnel_id = tunnel_base+1000;
    pon_n_1_service_info[index].up_lif_type = match_otag;
    pon_n_1_service_info[index].up_ovlan = 82;
    pon_n_1_service_info[index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 801;

    /* N(6-5) PON Port 5 Tunnel-ID 1000 SVLAN 80 CVLAN 801 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801 */
    index++;
    pon_n_1_service_info[index].service_mode = o_i_tag_to_o2_i_tag;
    pon_n_1_service_info[index].tunnel_id = tunnel_base+1000;
    pon_n_1_service_info[index].up_lif_type = match_o_i_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].up_ivlan = 801;
    pon_n_1_service_info[index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 801;

    /*count number of n_1 services*/
    nof_n_1_services = index + 1;

    /* TLS: Frames with Tunnel-ID 1000 and unconfigured tags are transparent to NNI
     * PON Port 5 Tunnel-ID 1000 any tag <-> VSI 4096 <-> 1 NNI Port 129 any tag.
     */
    pon_tls_service_info.service_mode = transparent_all_tags;
    pon_tls_service_info.tunnel_id    = tunnel_base+1000;
    pon_tls_service_info.up_lif_type = match_all_tags;
    pon_tls_service_info.down_lif_type = match_all_tags;

    /* triple_tag_manipulation: s+c tag in downstream frame will change to s'+c' tag, and auto add a tunnel ID
     * PON Port 5 Tunnel-ID 1000 s_tag 100 c_tag 10  <-> VSI 4096 <-> 1 NNI Port 129 s_tag 200 c_tag 20.
     */
    pon_3tag_manipulation_service_info.service_mode = triple_tag_manipulation;
    pon_3tag_manipulation_service_info.up_lif_type = match_t_o_i_tag;
    pon_3tag_manipulation_service_info.tunnel_id = tunnel_base+1000;
    pon_3tag_manipulation_service_info.up_ovlan = 100;
    pon_3tag_manipulation_service_info.up_ivlan = 10;
    pon_3tag_manipulation_service_info.down_lif_type = match_o_i_tag;
    pon_3tag_manipulation_service_info.down_ovlan = 200;
    pon_3tag_manipulation_service_info.down_ivlan = 20;

    pon_app_init(unit, pon_port, nni_port, 0, 0);

    return 0;
}

/*
 * Set up 1:1 sercies, using port cross connect.
 */
int pon_1_1_service(int unit)
{
    int rv;
    int index, is_with_id = 0;
    bcm_vswitch_cross_connect_t gports;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    uint8 down_pcp;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;

    for (index = 0; index < nof_1_1_services; index++)
    {
        pon_gport = 0;
        nni_gport = 0;
        service_mode = pon_1_1_service_info[index].service_mode;
        tunnel_id    = pon_1_1_service_info[index].tunnel_id;
        pon_lif_type = pon_1_1_service_info[index].up_lif_type;
        up_ovlan     = pon_1_1_service_info[index].up_ovlan;
        up_ivlan     = pon_1_1_service_info[index].up_ivlan;
        up_pcp       = pon_1_1_service_info[index].up_pcp;
        up_ethertype = pon_1_1_service_info[index].up_ethertype;
        nni_lif_type = pon_1_1_service_info[index].down_lif_type;
        down_ovlan   = pon_1_1_service_info[index].down_ovlan;
        down_ivlan   = pon_1_1_service_info[index].down_ivlan;
        down_pcp     = pon_1_1_service_info[index].down_pcp;

        /* Create PON LIF */
        rv = pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_lif_create index:%d\n", index);
            print rv;
            return rv;
        }

        /* Set PON LIF ingress VLAN editor */
        rv = pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_port_ingress_vt_set index:%d\n", index);
            print rv;
            return rv;
        }

        /* Set PON LIF egress VLAN editor */
        rv = pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_port_egress_vt_set index:%d\n", index);
            print rv;
            return rv;
        }

        /* Create NNI LIF */
        rv = nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_ovlan, down_ivlan, down_pcp, &nni_gport, &encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, nni_lif_create index:%d\n", index);
            print rv;
            return rv;
        }

        pon_1_1_service_info[index].pon_gport = pon_gport;
        pon_1_1_service_info[index].nni_gport = nni_gport;

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
 * Set up N:1 sercies, create VSI and add PON and NNI LIFs to it.
 */
int pon_n_1_service(int unit)
{
    int rv;
    int index, is_with_id = 0;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;

    vswitch_create(unit, &n_1_service_vsi);

    for (index = 0; index < nof_n_1_services; index++)
    {
        pon_gport = 0;
        nni_gport = 0;
        service_mode = pon_n_1_service_info[index].service_mode;
        tunnel_id    = pon_n_1_service_info[index].tunnel_id;
        pon_lif_type = pon_n_1_service_info[index].up_lif_type;
        up_ovlan     = pon_n_1_service_info[index].up_ovlan;
        up_ivlan     = pon_n_1_service_info[index].up_ivlan;
        up_pcp       = pon_n_1_service_info[index].up_pcp;
        up_ethertype = pon_n_1_service_info[index].up_ethertype;
        nni_lif_type = pon_n_1_service_info[index].down_lif_type;
        down_ovlan   = pon_n_1_service_info[index].down_ovlan;
        down_ivlan   = pon_n_1_service_info[index].down_ivlan;

        /* Create PON LIF */
        rv = pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_lif_create index:%d\n", index);
            print rv;
            return rv;
        }

        /* Add PON LIF to vswitch */
        rv = bcm_vswitch_port_add(unit, n_1_service_vsi, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }

        /*For downstream*/
        rv = multicast_vlan_port_add(unit, n_1_service_vsi+lif_offset, pon_port, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast_vlan_port_add index:%d\n", index);
            print rv;
            return rv;
        }

        /* Set PON LIF ingress VLAN editor */
        rv = pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_port_ingress_vt_set index:%d\n", index);
            print rv;
            return rv;
        }

        /* Set PON LIF egress VLAN editor */
        rv = pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_port_egress_vt_set index:%d\n", index);
            print rv;
            return rv;
        }

        /*Only create one NNI LIF*/
        if (index == 0)
        {            
            /* Create NNI LIF */
            rv = nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);
            if (rv != BCM_E_NONE) {
                printf("Error, nni_lif_create index:%d\n", index);
                print rv;
                return rv;
            }

            /*For upstream*/
            rv = multicast_vlan_port_add(unit, n_1_service_vsi, nni_port, nni_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, multicast_vlan_port_add index:%d\n", index);
                print rv;
                return rv;
            }

            /* add to vswitch */
            rv = bcm_vswitch_port_add(unit, n_1_service_vsi, nni_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vswitch_port_add\n");
                return rv;
            }
            
            /* Set the multicast group offset to flood downstream packets in multicast group(n_1_service_vsi+lif_offset) */
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownUcastGroup, lif_offset);
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownMcastGroup, lif_offset);
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodBroadcastGroup, lif_offset);
        }
        else
        {
            nni_gport = pon_n_1_service_info[0].nni_gport;
        }

        pon_n_1_service_info[index].pon_gport = pon_gport;
        pon_n_1_service_info[index].nni_gport = nni_gport;
    }

    return rv;
}

/*
 * Set up tls sercies, add PON and NNI LIFs to VSI which is same as VSI of N:1 sercies.
 */
int pon_tls_service(int unit)
{
    int rv;
    int index, is_with_id = 0;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;

    vswitch_create(unit, &tls_service_vsi);

    service_mode = pon_tls_service_info.service_mode;
    tunnel_id    = pon_tls_service_info.tunnel_id;
    pon_lif_type = pon_tls_service_info.up_lif_type;
    up_ovlan     = pon_tls_service_info.up_ovlan;
    up_ivlan     = pon_tls_service_info.up_ivlan;
    up_pcp       = pon_tls_service_info.up_pcp;
    up_ethertype = pon_tls_service_info.up_ethertype;
    nni_lif_type = pon_tls_service_info.down_lif_type;
    down_ovlan   = pon_tls_service_info.down_ovlan;
    down_ivlan   = pon_tls_service_info.down_ivlan;

    /* Create PON LIF */
    pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);

    /* Add to vswitch */
    rv = bcm_vswitch_port_add(unit, tls_service_vsi, pon_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /*Add pon lif to downstream multicast group*/
    multicast_vlan_port_add(unit, tls_service_vsi+lif_offset, pon_port, pon_gport);

    /* Set PON LIF ingress VLAN editor */
    pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL);

    /* Set PON LIF egress VLAN editor */
    pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);

    /* Create NNI LIF */
    nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);

    /* Add to vswitch */
    rv = bcm_vswitch_port_add(unit, tls_service_vsi, nni_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    
    /* Set the multicast group offset to flood downstream packets in multicast group(tls_service_vsi+lif_offset) */
    bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownUcastGroup, lif_offset);
    bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownMcastGroup, lif_offset);
    bcm_port_control_set(unit, nni_gport, bcmPortControlFloodBroadcastGroup, lif_offset);

    /*Add nni lif to upstream multicast group*/
    multicast_vlan_port_add(unit, tls_service_vsi, nni_port, nni_gport);

    pon_tls_service_info.pon_gport = pon_gport;
    pon_tls_service_info.nni_gport = nni_gport;

    return rv;
}

/*
 * Set up 3 tag manipulation service.
 * In this service, we can change tunnel ID, SVLAN and CVLAN in same time
 */
int pon_3tag_manipulation_service(int unit)
{
    int rv;
    int index, is_with_id = 0;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;

    vswitch_create(unit, &manipulation_service_vsi);

    service_mode = pon_3tag_manipulation_service_info.service_mode;
    tunnel_id    = pon_3tag_manipulation_service_info.tunnel_id;
    pon_lif_type = pon_3tag_manipulation_service_info.up_lif_type;
    up_ovlan     = pon_3tag_manipulation_service_info.up_ovlan;
    up_ivlan     = pon_3tag_manipulation_service_info.up_ivlan;
    up_pcp       = pon_3tag_manipulation_service_info.up_pcp;
    up_ethertype = pon_3tag_manipulation_service_info.up_ethertype;
    nni_lif_type = pon_3tag_manipulation_service_info.down_lif_type;
    down_ovlan   = pon_3tag_manipulation_service_info.down_ovlan;
    down_ivlan   = pon_3tag_manipulation_service_info.down_ivlan;

    /* Create PON LIF */
    pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);

    /* Add to vswitch */
    rv = bcm_vswitch_port_add(unit, manipulation_service_vsi, pon_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /*Add pon lif to downstream multicast group*/
    multicast_vlan_port_add(unit, manipulation_service_vsi+lif_offset, pon_port, pon_gport);

    /* Set PON LIF ingress VLAN editor */
    pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL);

    /* Set PON LIF egress VLAN editor */
    pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);

    /* Create NNI LIF */
    nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);

    /* Add to vswitch */
    rv = bcm_vswitch_port_add(unit, manipulation_service_vsi, nni_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /*Add nni lif to upstream multicast group*/
    multicast_vlan_port_add(unit, manipulation_service_vsi, nni_port, nni_gport);
    
    /* Set the multicast group offset to flood downstream packets in multicast group(manipulation_service_vsi+lif_offset) */
    bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownUcastGroup, lif_offset);
    bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownMcastGroup, lif_offset);
    bcm_port_control_set(unit, nni_gport, bcmPortControlFloodBroadcastGroup, lif_offset);

    pon_3tag_manipulation_service_info.pon_gport = pon_gport;
    pon_3tag_manipulation_service_info.nni_gport = nni_gport;

    return rv;
}

/*
 * Set up NNI double lookup sercies, using port cross connect.
 */
int nni_double_lookup_service(int unit)
{
    int rv;
    int index, is_with_id=0;
    bcm_tunnel_id_t tunnel_id=1000;
    bcm_vlan_t up_ovlan=2000;
    bcm_vlan_t up_ivlan=0;
    uint8 up_pcp=0;
    bcm_ethertype_t up_ethertype=0;
    bcm_vlan_t down_ovlan=3000;
    bcm_vlan_t down_ivlan=0;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type=match_otag;
    int nni_lif_type=match_otag;
    bcm_vlan_action_set_t action;

    if(soc_property_get(unit, spn_PON_PP_PORT_MAPPING_BYPASS, 0)){
        tunnel_id = (0x8000 + 1000);
    }

    /* Enable double lookup in NNI port */
    rv = bcm_vlan_control_port_set(unit, nni_port, bcmVlanPortDoubleLookupEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_control_port_set bcmVlanPortDoubleLookupEnable %s\n", nni_port, bcm_errmsg(rv));
        return rv;
    }

    /* Create PON LIF */
    pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);

    /* Set PON LIF ingress VLAN editor */
    pon_port_ingress_vt_set(unit, otag_to_otag2, down_ovlan, down_ivlan, pon_gport, 0, NULL);

    /* Set PON LIF egress VLAN editor */
    pon_port_egress_vt_set(unit, otag_to_o2_i2_tag, tunnel_id, up_ovlan, up_ivlan, pon_gport);

    /* Create NNI LIF */
    nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);

    /* Cross connect the 2 LIFs */
    bcm_vswitch_cross_connect_t_init(&nni_double_lkup_gports);

    nni_double_lkup_gports.port1 = pon_gport;
    nni_double_lkup_gports.port2 = nni_gport;
    rv = bcm_vswitch_cross_connect_add(unit, &nni_double_lkup_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        print rv;
        return rv;
    }
    
    return 0;
}

/*
 * Set up lif lookup sercies, using port cross connect.
 */
int pon_lif_lkup_service(int unit,int flag)
{
    int rv;
    int index=0, is_with_id = 0;
    bcm_vswitch_cross_connect_t gports;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;

    /* Only add tls entry in SEMB */
    if(flag == 1){
        pon_tls_service(unit);
        return BCM_E_NONE;
    }
    
    /*Add single tag entry to SEMA && tls entry*/
    if(flag == 2){
       index = 0; 
    }
    /*Add double tag  entry to SEMA && tls entry && single tag entry*/
    else if(flag == 3){
        index = 1; 
    }

    pon_gport = 0;
    nni_gport = 0;
    service_mode = pon_lif_lkup_service_info[index].service_mode;
    tunnel_id    = pon_lif_lkup_service_info[index].tunnel_id;
    pon_lif_type = pon_lif_lkup_service_info[index].up_lif_type;
    up_ovlan     = pon_lif_lkup_service_info[index].up_ovlan;
    up_ivlan     = pon_lif_lkup_service_info[index].up_ivlan;
    up_pcp       = pon_lif_lkup_service_info[index].up_pcp;
    up_ethertype = pon_lif_lkup_service_info[index].up_ethertype;
    nni_lif_type = pon_lif_lkup_service_info[index].down_lif_type;
    down_ovlan   = pon_lif_lkup_service_info[index].down_ovlan;
    down_ivlan   = pon_lif_lkup_service_info[index].down_ivlan;
    
    /* Create PON LIF */
    rv = pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_lif_create index:%d\n", index);
        print rv;
        return rv;
    }
    
    /* Set PON LIF ingress VLAN editor */
    rv = pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_port_ingress_vt_set index:%d\n", index);
        print rv;
        return rv;
    }
    
    /* Set PON LIF egress VLAN editor */
    rv = pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_port_egress_vt_set index:%d\n", index);
        print rv;
        return rv;
    }
    
    /* Create NNI LIF */
    rv = nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, nni_lif_create index:%d\n", index);
        print rv;
        return rv;
    }
    
    pon_lif_lkup_service_info[index].pon_gport = pon_gport;
    pon_lif_lkup_service_info[index].nni_gport = nni_gport;
    
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
    
    return rv;
}
    

/*
 * clean up the configurations of 1:1 sercies.
 */
int pon_1_1_service_cleanup(int unit)
{
    int rv;
    int index;
    bcm_vswitch_cross_connect_t gports;

    for (index = 0; index < nof_1_1_services; index++)
    {
        /* Delete the cross connected LIFs */
        gports.port1 = pon_1_1_service_info[index].pon_gport;
        gports.port2 = pon_1_1_service_info[index].nni_gport;
        rv = bcm_vswitch_cross_connect_delete(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_delete\n");
            print rv;
            return rv;
        }

        /* Delete PON LIF */
        rv = bcm_vlan_port_destroy(unit, pon_1_1_service_info[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }

        /* Delete NNI LIF */
        rv = bcm_vlan_port_destroy(unit, pon_1_1_service_info[index].nni_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }
    }

    return rv;        
}

/*
 * clean up the configurations of N:1 sercies.
 */
int pon_n_1_service_cleanup(int unit)
{
    int rv;
    int index;
    bcm_gport_t pon_gport, nni_gport;

    for (index = 0; index < nof_n_1_services; index++)
    {
        pon_gport = pon_n_1_service_info[index].pon_gport;
        nni_gport = pon_n_1_service_info[index].nni_gport;
        rv = vswitch_delete_port(unit, n_1_service_vsi, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, vswitch_metro_delete_port\n");
            return rv;
        }

        /*Only one NNI LIF*/
        if (index == 0)
        {
            rv = vswitch_delete_port(unit, n_1_service_vsi, nni_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, vswitch_metro_delete_port\n");
                return rv;
            }
        }
    }

    rv = vswitch_delete(unit, n_1_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_delete \n");
        return rv;
    }
    
    return rv;        
}

/*
 * clean up the configurations of TLS sercies.
 */
int pon_tls_service_cleanup(int unit)
{
    int rv;

    rv = vswitch_delete_port(unit, tls_service_vsi, pon_tls_service_info.pon_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_delete_port\n");
        return rv;
    }

    rv = vswitch_delete_port(unit, tls_service_vsi, pon_tls_service_info.nni_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_delete_port\n");
        return rv;
    }

    rv = vswitch_delete(unit, tls_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_delete \n");
        return rv;
    }

    return rv;        
}

/*
 * clean up the configurations of 3 tag manipulation service.
 */
int pon_3tag_manipulation_service_cleanup(int unit)
{
    int rv;

    rv = vswitch_delete_port(unit, manipulation_service_vsi, pon_3tag_manipulation_service_info.pon_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_delete_port\n");
        return rv;
    }

    rv = vswitch_delete_port(unit, manipulation_service_vsi, pon_3tag_manipulation_service_info.nni_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_delete_port\n");
        return rv;
    }

    rv = vswitch_delete(unit, manipulation_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_delete \n");
        return rv;
    }

    return rv;        
}

/*
 * clean up the configurations of NNI double lookup sercies.
 */
int nni_double_lookup_service_cleanup(int unit)
{
    int rv;

    /* disable double lookup in NNI port */
    rv = bcm_vlan_control_port_set(unit, nni_port, bcmVlanPortDoubleLookupEnable, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_control_port_set bcmVlanPortDoubleLookupEnable %s\n", nni_port, bcm_errmsg(rv));
        return rv;
    }

    /* Delete the cross connected LIFs */
    rv = bcm_vswitch_cross_connect_delete(unit, &nni_double_lkup_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_delete\n");
        print rv;
        return rv;
    }

    /* Delete PON LIF */
    rv = bcm_vlan_port_destroy(unit, nni_double_lkup_gports.port1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy \n");
        return rv;
    }

    /* Delete NNI LIF */
    rv = bcm_vlan_port_destroy(unit, nni_double_lkup_gports.port2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy \n");
        return rv;
    }

    return rv;        
}

/*
 * clean up the configurations of lif_lkup sercies.
 */
int pon_lif_lkup_service_cleanup(int unit)
{
    int rv;
    int index;
    bcm_vswitch_cross_connect_t gports;
    
    pon_tls_service_cleanup(unit);

    for (index = 0; index < nof_lif_lkup_services; index++)
    {
        /* Delete the cross connected LIFs */
        gports.port1 = pon_lif_lkup_service_info[index].pon_gport;
        gports.port2 = pon_lif_lkup_service_info[index].nni_gport;
        rv = bcm_vswitch_cross_connect_delete(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_delete\n");
            print rv;
            return rv;
        }

        /* Delete PON LIF */
        rv = bcm_vlan_port_destroy(unit, pon_lif_lkup_service_info[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }

        /* Delete NNI LIF */
        rv = bcm_vlan_port_destroy(unit, pon_lif_lkup_service_info[index].nni_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }
    }

    return rv;        
}

/*
 * Print all service info.
 * type = 0, Print 1:1 service info
 * type = 1, Print N:1 service info
 * type = 2, Print TLS service info
 */
void pon_service_info_dump(int type)
{
    int index;
    int num;
    pon_service_info_s *pon_service_info;

    if (type == 0)
    {
        pon_service_info = pon_1_1_service_info;
        num = nof_1_1_services;
        printf("pon_1_1_service_info:\n");
    }
    else if (type == 1)
    {
        pon_service_info = pon_n_1_service_info;
        num = nof_n_1_services;
        printf("pon_n_1_service_info:\n");
    }
    else if (type == 2)
    {
        pon_service_info = &pon_tls_service_info;
        printf("pon_tls_service_info:\n");
    }
    else
    {
        printf("Error, wrong type!\n");
        return;
    }

    if ((type == 0) || (type == 1))
    {
        for (index = 0; index < num; index++)
        {
            printf("service_index:%d\tservice_mode:%d\n", index, pon_service_info[index].service_mode);
            printf("up_lif_type:%d\ttunnel_id:%d\tup_ovlan:%d\tup_ivlan:%d\tup_pcp:%d\tup_ethertype:0x%x\tpon_gport:0x%x\n", 
                pon_service_info[index].up_lif_type, pon_service_info[index].tunnel_id, 
                pon_service_info[index].up_ovlan, pon_service_info[index].up_ivlan, 
                pon_service_info[index].up_pcp, pon_service_info[index].up_ethertype, 
                pon_service_info[index].pon_gport);
            printf("down_lif_type:%d\tdown_ovlan:%d\tdown_ivlan:%d\tnni_gport:0x%x\n\n", 
                pon_service_info[index].down_lif_type, pon_service_info[index].down_ovlan, 
                pon_service_info[index].down_ivlan, pon_service_info[index].nni_gport);
        }
    }
    else
    {
        printf("service_mode:%d\n", pon_service_info->service_mode);
        printf("up_lif_type:%d\ttunnel_id:%d\tup_ovlan:%d\tup_ivlan:%d\tup_pcp:%d\tup_ethertype:0x%x\tpon_gport:0x%x\n", 
            pon_service_info->up_lif_type, pon_service_info->tunnel_id, 
            pon_service_info->up_ovlan, pon_service_info->up_ivlan, 
            pon_service_info->up_pcp, pon_service_info->up_ethertype, 
            pon_service_info->pon_gport);
        printf("down_lif_type:%d\tdown_ovlan:%d\tdown_ivlan:%d\tnni_gport:0x%x\n\n", 
            pon_service_info->down_lif_type, pon_service_info->down_ovlan, 
            pon_service_info->down_ivlan, pon_service_info->nni_gport);
    }
}

int pon_n_1_service_verify_l2_entry_exist_and_match(int unit, uint8 mac_lsb, bcm_vlan_t vid, int n_1_service_index) {
    int rv = BCM_E_NONE;
    bcm_mac_t mac;
    bcm_l2_addr_t l2_addr;

    mac[5] = mac_lsb;
    rv = bcm_l2_addr_get(unit, mac, vid, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("The specified MAC address is not learned.\n");
        return rv;
    }

    if (l2_addr.port != pon_n_1_service_info[n_1_service_index].pon_gport) {
        printf("Packet learned on 0x%08x, expected 0x%08x\n", l2_addr.port, pon_n_1_service_info[n_1_service_index].pon_gport);
        return BCM_E_PARAM;
    }

    return rv;
}

int pon_n_1_service_learn_raw_outlif_dvapi(int unit,  bcm_port_t port_pon, bcm_port_t port_nni) {
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    int index = 0;
    int encap_id = 0x1234;
    int rv;

    fwd_group=1;
    pon_service_init(unit, port_pon, port_nni);
    pon_n_1_service(unit);

    bcm_l2_addr_t_init(&l2addr, mac, n_1_service_vsi);
    l2addr.port = pon_n_1_service_info[index].pon_gport;
    l2addr.encap_id = encap_id;
    l2addr.flags = BCM_L2_SET_ENCAP_INVALID | BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_addr_add failed.\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2addr, mac, n_1_service_vsi);
    rv = bcm_l2_addr_get(unit, mac, n_1_service_vsi, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_addr_get failed.\n");
        return rv;
    }

    if ((l2addr.encap_id != encap_id) || ((l2addr.flags & BCM_L2_SET_ENCAP_INVALID) == 0)) {
        printf("Received encap_id 0x%x, received flag 0x%08x.\n", l2addr.encap_id, l2addr.flags);
        return BCM_E_FAIL;
    }

    /* For Global mode, l2_entry.gport is from SW DB and it should be equel to gport of bcm_vlan_port_create().
     * And the gport will look like as 0x444xxxxx. Otherwise, gport is set by BCM_GPORT_FORWARD_PORT_SET, and the value will be 0x98xxxxxx
     */
    if (0 == soc_property_get(unit , "bcm88xxx_system_resource_management",0)) {
        if (l2addr.port != pon_n_1_service_info[index].pon_gport) {
            printf("Received port 0x%x, expected port 0x%x.\n", l2addr.port, pon_n_1_service_info[index].pon_gport);
            return BCM_E_FAIL;
        }
    }

    rv = bcm_l2_addr_delete(unit, mac, n_1_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_addr_delete failed.\n");
        return rv;
    }
    
    rv = bcm_l2_addr_get(unit, mac, n_1_service_vsi, &l2addr);
    if (rv != BCM_E_NOT_FOUND) {
        printf("unexpected return code %d.\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * IPMC example at PON application.
 * In this case L3 MC group has 2 encaps, one is ARP and another is pon_out_ac
 *
 * Steps:
 * cd ../../../../src
 * cint examples/dpp/cint_port_tpid.c
 * cint examples/sand/utility/cint_sand_utils_global.c
 * cint examples/dpp/pon/cint_pon_utils.c
 * cint examples/dpp/pon/cint_pon_application.c
 * cint examples/dpp/utility/cint_utils_l3.c
 * cint examples/sand/cint_ip_route_basic.c
 * cint examples/sand/cint_ipmc_route_basic.c
 * cint
 * pon_service_init(0, pon_port, nni_port);
 * pon_n_1_service(0);
 *
 *
 * traffic behavior 
 * Bridge Service:
 *     PON Port 5  <--------------------------->      VSI  <---> 1 NNI Port 129
 *     Tunnel-ID 1000 CVLAN 80   -----------------|--- 4096  ---  SVLAN 800 CVLAN 801
 *
 * IPMC Service:
 *     PON Port 5  <--------------------------->      VSI  <---> 1 NNI Port 129
 *     Tunnel-ID 1000 CVLAN 80   -----------------|--- 400  ---  SVLAN 100
 */
int pon_n_1_service_ipmc_example(int unit,  bcm_port_t pon_port, bcm_port_t nni_port) {
    /* NNI l3 intf */
    bcm_mac_t nni_intf_in_mac_address = { 0x00, 0x00, 0x00, 0x00, 0xab, 0x1d };
    int nni_intf = 100;
    create_l3_intf_s nni_in_intf;
    int vrf = 10;
    /* PON l3 intf */
    bcm_mac_t pon_intf_in_mac_address = { 0x00, 0x00, 0x00, 0x00, 0xab, 0x1e };
    create_l3_intf_s pon_in_intf;
    int pon_vrf = 11;
    int pon_rif = 400;
    /* ARP configuration */
    bcm_l3_egress_t l3eg;
    bcm_if_t intf = 0;
    bcm_mac_t out_mac         = { 0x00, 0x45, 0x45, 0x45, 0x45, 0x01 };
    /* MC */
    bcm_multicast_replication_t reps[1];
    int mc_id = 6000;
    bcm_ipmc_addr_t data;
    bcm_ip_t mc_ip_addr = 0xE0E0E001;
    bcm_ip_t mc_ip_mask = 0xffffffff;
    bcm_ip_t s_ip_addr = 0xC0800101;
    bcm_ip_t s_ip_mask = 0xffffffff;
    /* PON gport (Tunnel-ID 1000 CVLAN 80) */
    int index = 1;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_translate_action_class_t action_class;
    int egr_vlan_action_id = 0;
    int mc_flags = 0;
    bcm_vlan_action_set_t action;
    int is_qax_or_qux = 0;
    int rv;

    /* create l3 intf at NNI side */ 
    nni_in_intf.vsi = nni_intf;
    nni_in_intf.my_global_mac = nni_intf_in_mac_address;
    nni_in_intf.my_lsb_mac = nni_intf_in_mac_address;
    nni_in_intf.urpf_mode = bcmL3IngressUrpfDisable;
    nni_in_intf.vrf = vrf;
    nni_in_intf.vrf_valid = 1;
    rv = l3__intf_rif__create(unit, &nni_in_intf);
    if (rv != BCM_E_NONE) {
        printf("l3__intf_rif__create return code %d.\n", rv);
        return rv;
    }
    /* create l3 intf at PON side */
    pon_in_intf.vsi = pon_rif;
    pon_in_intf.my_global_mac = pon_intf_in_mac_address;
    pon_in_intf.my_lsb_mac = pon_intf_in_mac_address;
    pon_in_intf.urpf_mode = bcmL3IngressUrpfDisable;
    pon_in_intf.vrf = pon_vrf;
    pon_in_intf.vrf_valid = 1;
    rv = l3__intf_rif__create(unit, &pon_in_intf);
    if (rv != BCM_E_NONE) {
        printf("l3__intf_rif__create return code %d.\n", rv);
        return rv;
    }

    /* ARP configuration with vsi */
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, out_mac, 6);
    l3eg.vlan   = pon_rif;
    l3eg.port   = pon_port;
    l3eg.flags  = BCM_L3_EGRESS_ONLY;
    rv = bcm_l3_egress_create(unit, 0, &l3eg, &intf);
    if (rv != BCM_E_NONE) {
        printf("bcm_l3_egress_create return code %d.\n", rv);
        return rv;
    }

    rv = is_qumran_ax(unit, &is_qax_or_qux);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_qumran_ax\n", rv);
        return rv;
    }

    if (is_qax_or_qux) {
        mc_flags = BCM_MULTICAST_INGRESS_GROUP|BCM_MULTICAST_TYPE_L3|BCM_MULTICAST_WITH_ID;
    } else {
        mc_flags = BCM_MULTICAST_EGRESS_GROUP|BCM_MULTICAST_TYPE_L3|BCM_MULTICAST_WITH_ID;
    }

    /* Create L3 MC group 
     */
    rv = bcm_multicast_create(unit, mc_flags, &mc_id); 
    if (rv != BCM_E_NONE) {
        printf("bcm_multicast_create return code %d.\n", rv);
        return rv;
    }
    /* Add ARP pointer and pon_out_ac to L3 MC group */
    BCM_GPORT_LOCAL_SET(reps[0].port, pon_port);
    /* Reuse pon_out_ac(Tunnel-ID 1000 CVLAN 80) */
    reps[0].encap1 = l3eg.encap_id & 0x3ffff; /* ARP */
    reps[0].encap2 =  pon_n_1_service_info[index].pon_gport & 0x3ffff; /* out_ac */
    reps[0].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;
    if (is_qax_or_qux) {
        mc_flags = BCM_MULTICAST_INGRESS;
    } else {
        /* egress */
        mc_flags = 0;
    }

    rv = bcm_multicast_set(unit, mc_id, mc_flags, 1, reps);
    if (rv != BCM_E_NONE) {
        printf("bcm_multicast_set return code %d.\n", rv);
        return rv;
    }

    /* Add route */
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip_addr;
    data.mc_ip_mask = mc_ip_mask;
    data.s_ip_addr = s_ip_addr;
    data.s_ip_mask = s_ip_mask;
    data.vid = nni_intf;
    data.vrf = vrf;
    data.group = mc_id;
    rv = bcm_ipmc_add(unit, &data);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_add return code %d.\n", rv);
        return rv;
    }

    /* To get same tunnel ID and vid(Tunnel-ID 1000 CVLAN 80)such as bridge service,
     * Add EVE for untag & pon_out_ac's edit profile
     */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.gport = pon_n_1_service_info[1].pon_gport;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_get(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_get %d\n", rv);
        return rv;
    }

    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &egr_vlan_action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create %d\n", rv);
        return rv;
    }

    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x8100;
    action.dt_inner = bcmVlanActionAdd;
    action.dt_outer = bcmVlanActionAdd;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, egr_vlan_action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set %d\n", rv);
        return rv;
    }

    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = port_trans.vlan_edit_class_id;
    action_class.tag_format_class_id = no_tag;
    action_class.vlan_translation_action_id = egr_vlan_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
        return rv;
    }

    return rv;
}


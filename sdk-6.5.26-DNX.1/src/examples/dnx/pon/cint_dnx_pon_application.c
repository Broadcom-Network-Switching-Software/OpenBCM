/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* File: cint_dnx_pon_application.c
* Purpose: An example of the PON application. 
*          The following CINT provides a calling sequence example to bring up three main PON services categories: 1:1 Model , N:1 Model and TLS. 
*          Packets on the NNI ports are Ethernet packets, tagged with 0,1 or 2 VLAN tags identify the user and service (ONU), and class of service. 
*          Packets on the PON ports are Ethernet packets with an outermost VLAN-Tag that encodes the PON_SubPort_ID and PON_Channel_ID, i.e., service or ONU#. 
*          We refer to that tag as the PON_Tag (PON_SubPort_ID: PON_Tag[15:12],  PON_Channel_ID: PON_Tag[11:0]).. 
*          PON_Tag it always present on packets on PON ports, which may have C-Tag and S-Tag as well. 
*          When packet is forwarded downstream from NNI-Port to PON-Ports, the incoming VLAN-header from the NNI port may be retained, stripped, or modified, and a Tunnel-ID is inserted.
*          When packet are forwarded upstream from PON-Port to a NNI-Ports, the Incoming VLAN-header from the PON port may be retained, stripped, or modified, and the Tunnel-ID is stripped.
*
* Calling sequence:
*
* 1:1 Service:
* Set up sequence:
*  1. Enable the PON service on port
*       - Call bcm_port_control_set with bcmPortControlPONEnable
*  2. Mapping PTC+PON_SubPort_ID to PP_PORT
*      - Call bcm_port_extender_mapping_info_set with bcmPortExtenderMappingTypePonTunnel
*  3. Set port as STC/DTC port
*      - Call bcm_vlan_control_port_set with bcmVlanPortDoubleLookupEnable
*  4. Create PON LIF
*        - Call bcm_vlan_port_create() with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED, 
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE,
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE
*  5. Set PON LIF ingress VLAN editor.
*        - Call pon_port_ingress_vt_set()
*  6. Set PON LIF egress VLAN editor.
*        - Call pon_port_egress_vt_set()
*  5. Create NNI LIF
*        - Call bcm_vlan_port_create()with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
*  7. Cross connect the 2 LIFs
*        - Call bcm_vswitch_cross_connect_add()
*
* To Activate Above Settings Run:
*      BCM> cint examples/sand/utility/cint_sand_utils_global.c
*      BCM> cint examples/sand/utility/cint_sand_utils_vlan.c
*      BCM> cint examples/dnx/pon/cint_dnx_pon_utils.c
*      BCM> cint examples/dnx/pon/cint_dnx_pon_application.c
*      BCM> cint
*      cint> pon_1_1_service_init(unit, pon_port, nni_port);
*      cint> pon_1_1_service(unit);
*
*  Service Model:
* 1:1 Service:
*     N(1) PON Port 200  <------------------------  CrossConnect  -------------> 1 NNI Port 201
*                                              UNTAG(5)
*     Tunnel-ID 1000 <---------------------------------------------------------> SVLAN 10
*     Tunnel-ID 1001 <---------------------------------------------------------> CVLAN 20
*     Tunnel-ID 1002 <---------------------------------------------------------> SVLAN 30 CVLAN 31
*     Tunnel-ID 1003 <---------------------------------------------------------> CVLAN 40 CVLAN 41
*     Tunnel-ID 1004 <---------------------------------------------------------> SVLAN 50 SVLAN 51
*                                              SVLAN(6)
*     Tunnel-ID 1000 SVLAN 100<------------------------------------------------> SVLAN 100
*     Tunnel-ID 1001 SVLAN 10<-------------------------------------------------> SVLAN 110
*     Tunnel-ID 1002 SVLAN 20<-------------------------------------------------> CVLAN 120
*     Tunnel-ID 1003 SVLAN 30<-------------------------------------------------> SVLAN 130 CVLAN 131
*     Tunnel-ID 1004 SVLAN 40<-------------------------------------------------> CVLAN 140 CVLAN 141
*     Tunnel-ID 1004 SVLAN 50<-------------------------------------------------> SVLAN 150 SVLAN 151
*                                              CVLAN(6)
*     Tunnel-ID 1000 CVLAN 200<------------------------------------------------> CVLAN 200
*     Tunnel-ID 1001 CVLAN 10<-------------------------------------------------> CVLAN 210
*     Tunnel-ID 1002 CVLAN 20<-------------------------------------------------> SVLAN 120
*     Tunnel-ID 1003 CVLAN 30<-------------------------------------------------> SVLAN 230 CVLAN 231
*     Tunnel-ID 1004 CVLAN 40<-------------------------------------------------> CVLAN 240 CVLAN 241
*     Tunnel-ID 1004 CVLAN 50<-------------------------------------------------> SVLAN 250 SVLAN 251
*                                              SVLAN + CVLAN(7)
*     Tunnel-ID 1000 SVLAN 1000 CVLAN 20<--------------------------------------> SVLAN 1000
*     Tunnel-ID 1001 SVLAN 10 CVLAN 10<----------------------------------------> SVLAN 1010
*     Tunnel-ID 1002 SVLAN 20 CVLAN 1020<--------------------------------------> CVLAN 1020
*     Tunnel-ID 1003 SVLAN 30 CVLAN 40<----------------------------------------> CVLAN 1030
*     Tunnel-ID 1004 SVLAN 40 CVLAN 50<----------------------------------------> SVLAN 1040 CVLAN 1050
*     Tunnel-ID 1005 SVLAN 50 CVLAN 60<----------------------------------------> CVLAN 1050 CVLAN 1060
*     Tunnel-ID 1006 SVLAN 60 CVLAN 70<----------------------------------------> SVLAN 1060 SVLAN 1070
*                                              CVLAN + CVLAN(6)
*     Tunnel-ID 1000 CVLAN 2000 CVLAN 20<--------------------------------------> CVLAN 2000
*     Tunnel-ID 1001 CVLAN 10 CVLAN 10<----------------------------------------> CVLAN 2010
*     Tunnel-ID 1002 CVLAN 20 CVLAN 30<----------------------------------------> SVLAN 2020
*     Tunnel-ID 1003 CVLAN 30 CVLAN 40<----------------------------------------> SVLAN 2030 CVLAN 2040
*     Tunnel-ID 1004 CVLAN 40 CVLAN 50<----------------------------------------> CVLAN 2040 CVLAN 2050
*     Tunnel-ID 1004 CVLAN 50 CVLAN 60<----------------------------------------> SVLAN 2050 SVLAN 2060
*                                              SVLAN + SVLAN(6)
*     Tunnel-ID 1000 SVLAN 3000 SVLAN 20<--------------------------------------> SVLAN 3000
*     Tunnel-ID 1001 SVLAN 10 SVLAN 10<----------------------------------------> SVLAN 3010
*     Tunnel-ID 1002 SVLAN 20 SVLAN 30<----------------------------------------> CVLAN 3020
*     Tunnel-ID 1003 SVLAN 30 SVLAN 40<----------------------------------------> SVLAN 3030 CVLAN 3040
*     Tunnel-ID 1004 SVLAN 40 SVLAN 50<----------------------------------------> CVLAN 3040 CVLAN 3050
*     Tunnel-ID 1004 SVLAN 50 SVLAN 60<----------------------------------------> SVLAN 3050 SVLAN 3060
*
* N:1 Service:
* Set up sequence:
*  1. Enable the PON service on port
*       - Call bcm_port_control_set with bcmPortControlPONEnable
*  2. Mapping PTC+PON_SubPort_ID to PP_PORT
*      - Call bcm_port_extender_mapping_info_set with bcmPortExtenderMappingTypePonTunnel
*  3. Set port as STC/DTC port
*      - Call bcm_vlan_control_port_set with bcmVlanPortDoubleLookupEnable
*  4. Create PON LIF
*        - Call bcm_vlan_port_create() with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED, 
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE,
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE
*  5. Add PON LIF to vswitch
*        - Call bcm_vswitch_port_add()
*  6. Set multicast settings,Add PON-LIFs to MC-ID = VSI + 16K
*        - Call bcm_multicast_vlan_encap_get()
*        - Call bcm_multicast_ingress_add()
*  7. Set PON LIF ingress VLAN editor.
*        - Call pon_port_ingress_vt_set()
*  8. Set PON LIF egress VLAN editor.
*        - Call pon_port_egress_vt_set()
*  9. Create NNI LIF
*        - Call bcm_vlan_port_create()with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
*  10. Add NNI LIF to vswitch
*        - Call bcm_vswitch_port_add()
*  11. Set multicast settings, Add NNI-LIF to MC-ID = VSI
*        - Call bcm_multicast_vlan_encap_get()
*        - Call bcm_multicast_ingress_add()
*  12.Set the multicast group offset to flood downstream packets in multicast group
*        - Call bcm_port_control_set (NNI-LIF, offset= X constant)
*
* To Activate Above Settings Run:
*      BCM> cint examples/sand/utility/cint_sand_utils_global.c
*      BCM> cint examples/sand/utility/cint_sand_utils_vlan.c
*      BCM> cint examples/dnx/pon/cint_dnx_pon_utils.c
*      BCM> cint examples/dnx/pon/cint_dnx_pon_application.c
*      BCM> cint
*      cint> pon_n_1_service_init(unit, pon_port, nni_port);
*      cint> pon_n_1_service(unit);
*
* N:1 Service:
*     N(6) PON Port 200  <--------------------------->  VSI  <-----------------> 1 NNI Port 202
*     Tunnel-ID 1000 <-------------------------------|
*     Tunnel-ID 1000 SVLAN 80<-----------------------|
*     Tunnel-ID 1000 CVLAN 80< ----------------------|--- 4096  ---------------> SVLAN 800 CVLAN 810
*     Tunnel-ID 1000 SVLAN 80 CVLAN 81< -------------|
*     Tunnel-ID 1000 CVLAN 80 CVLAN 81<--------------|
*     Tunnel-ID 1000 SVLAN 80 SVLAN 81< -------------|
*
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
    bcm_gport_t pon_pp_port;
    uint32 ip_anti_spoofing_enable;
    uint32 mac_sav_enable;
};

struct pon_service_multi_subport_info_s{
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
    bcm_gport_t pon_pp_port;
    bcm_port_t pon_subport;
    uint32 ip_anti_spoofing_enable;
};

bcm_port_t pon_port = 200;
bcm_port_t pon_port_mode2 = 202;
bcm_port_t nni_port = 201;
uint32 pon_subport_id = 0;
uint32 pon_subport_id_mode_2 = 0;
int tunnel_base = 1000;
int channel_mode = 1;

int nof_1_1_services;
pon_service_info_s pon_1_1_service_info[36];

bcm_vlan_t n_1_service_vsi;
int nof_n_1_services;

pon_service_info_s pon_n_1_service_info[6];

int nof_pon_m_subport_mode_1;
int nof_pon_m_subport_mode_2;
int nof_1_1_services_multi_subport;

int pon_mode_1 = 1;
int pon_mode_2 = 2;
pon_service_multi_subport_info_s pon_m_subport_info_mode_1[4];
pon_service_multi_subport_info_s pon_m_subport_info_mode_2[4];
pon_service_multi_subport_info_s pon_1_1_service_multi_subport_info[4];
/*
 *Initialize the service models and set up PON application configurations.
 */
int pon_1_1_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index = 0;
    uint32 flags;
    int tunnel_idx;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int bit_shift = 0;

    bit_shift = channel_mode == 1 ? 12:13;
    pon_port = port_pon;
    nni_port = port_nni;

    /*1. PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index = 0;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = untag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 10;

    /*2. PON Port 200 Tunnel-ID 1001 <-CrossConnect-> NNI Port 201 CVLAN 20*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 20;

    /*3. PON Port 200 Tunnel-ID 1002 <-CrossConnect-> NNI Port 201 SVLAN 30 CVLAN 31*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_s_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 30;
    pon_1_1_service_info[index].down_ivlan = 31;

    /*4. PON Port 200 Tunnel-ID 1003 <-CrossConnect-> NNI Port 201 CVLAN 40 CVLAN 41*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_c_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 40;
    pon_1_1_service_info[index].down_ivlan = 41;

    /*5. PON Port 200 Tunnel-ID 1004 <-CrossConnect-> NNI Port 201 SVLAN 50 SVLAN 51*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_s_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 50;
    pon_1_1_service_info[index].down_ivlan = 51;

    /*6. PON Port 200 Tunnel-ID 1000 SVLAN 10 <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index++;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = s_tag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 100;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 100;

    /*7. PON Port 200 Tunnel-ID 1001 SVLAN 10 <-CrossConnect-> NNI Port 201 SVLAN 110*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 110;

    /*8. PON Port 200 Tunnel-ID 1002(DTC)/1001(STC) SVLAN 20 <-CrossConnect-> NNI Port 201 CVLAN 120*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 120;


    /*9. PON Port 200 Tunnel-ID 1003(DTC)/1002(STC) SVLAN 30 <-CrossConnect-> NNI Port 201 SVLAN 130 CVLAN 131*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 130;
    pon_1_1_service_info[index].down_ivlan = 131;

    /*10. PON Port 200 Tunnel-ID 1004(DTC)/1003(STC) SVLAN 40 <-CrossConnect-> NNI Port 201 CVLAN 140 CVLAN 141*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 140;
    pon_1_1_service_info[index].down_ivlan = 141;

    /*11. PON Port 200 Tunnel-ID 1005(DTC)/1004(STC) SVLAN 50 <-CrossConnect-> NNI Port 201 SVLAN 150 SVLAN 151*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 150;
    pon_1_1_service_info[index].down_ivlan = 151;

    /*12. PON Port 200 Tunnel-ID 1000 CVLAN 10 <-CrossConnect-> NNI Port 201 CVLAN 10*/
    index++;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = c_tag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 200;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 200;

    /*13. PON Port 200 Tunnel-ID 1001 CVLAN 10 <-CrossConnect-> NNI Port 201 CVLAN 210*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 210;

    /*14. PON Port 200 Tunnel-ID 1002 CVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 220*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 220;

    /*15. PON Port 200 Tunnel-ID 1003(DTC)/1001(STC) CVLAN 30 <-CrossConnect-> NNI Port 201 SVLAN 130 CVLAN 131*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 230;
    pon_1_1_service_info[index].down_ivlan = 231;

    /*16. PON Port 200 Tunnel-ID 1004(DTC)/1002(STC) CVLAN 40 <-CrossConnect-> NNI Port 201 CVLAN 240 CVLAN 241*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 240;
    pon_1_1_service_info[index].down_ivlan = 241;


    /*17. PON Port 200 Tunnel-ID 1005(DTC)/1003(STC) CVLAN 50 <-CrossConnect-> NNI Port 201 SVLAN 150 SVLAN 151*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 250;
    pon_1_1_service_info[index].down_ivlan = 251;    

    /*18. PON Port 200 Tunnel-ID 1000 SVLAN 1000 CVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 1000*/
    index++;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 1000;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 1000;

    /*19. PON Port 200 Tunnel-ID 1001 SVLAN 10 CVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 1010*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 1010;

    /*20. PON Port 200 Tunnel-ID 1002 SVLAN 1000 CVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 1010*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].up_ivlan = 1020;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 1020;

    /*21. PON Port 200 Tunnel-ID 1003 SVLAN 30 CVLAN 40 <-CrossConnect-> NNI Port 201 CVLAN 1030*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].up_ivlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 1030;

    /*22. PON Port 200 Tunnel-ID 1004 SVLAN 40 CVLAN 50 <-CrossConnect-> NNI Port 201 SVLAN 1040 CVLAN 1050*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].up_ivlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 1040;
    pon_1_1_service_info[index].down_ivlan = 1050;

    /*23. PON Port 200 Tunnel-ID 1005 SVLAN 50 CVLAN 60 <-CrossConnect-> NNI Port 201 CVLAN 1050 CVLAN 1060*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].up_ivlan = 60;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 1050;
    pon_1_1_service_info[index].down_ivlan = 1060;

    /*24 PON Port 200 Tunnel-ID 1006 SVLAN 60 CVLAN 70 <-CrossConnect-> NNI Port 201 SVLAN 1060 SVLAN 1070*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 60;
    pon_1_1_service_info[index].up_ivlan = 70;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 1060;
    pon_1_1_service_info[index].down_ivlan = 1070;

    /*25. PON Port 200 Tunnel-ID 1000 CVLAN 2000 CVLAN 20 <-CrossConnect-> NNI Port 201 CVLAN 2000*/
    index++;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 2000;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 2000;

    /*26. PON Port 200 Tunnel-ID 1001 CVLAN 10 CVLAN 20 <-CrossConnect-> NNI Port 201 CVLAN 2010*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 2010;

    /*27. PON Port 200 Tunnel-ID 1002 CVLAN 20 CVLAN 30 <-CrossConnect-> NNI Port 201 SVLAN 2020*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].up_ivlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 2020;

    /*28. PON Port 200 Tunnel-ID 1003 CVLAN 30 CVLAN 40 <-CrossConnect-> NNI Port 201 SVLAN 2030 CVLAN 2040*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].up_ivlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 2030;
    pon_1_1_service_info[index].down_ivlan = 2040;

    /*29. PON Port 200 Tunnel-ID 1004 CVLAN 40 CVLAN 50 <-CrossConnect-> NNI Port 201 CVLAN 2040 CVLAN 2050*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].up_ivlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 2040;
    pon_1_1_service_info[index].down_ivlan = 2050;

    /*30. PON Port 200 Tunnel-ID 1005 CVLAN 50 CVLAN 60 <-CrossConnect-> NNI Port 201 SVLAN 1050 SVLAN 1060*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].up_ivlan = 60;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 2050;
    pon_1_1_service_info[index].down_ivlan = 2060;

    /*31. PON Port 200 Tunnel-ID 1000 SVLAN 3000 SVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 3000*/
    index++;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 3000;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 3000;

    /*32. PON Port 200 Tunnel-ID 1001 SVLAN 10 SVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 3010*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 3010;

    /*33. PON Port 200 Tunnel-ID 1002 SVLAN 20 SVLAN 30 <-CrossConnect-> NNI Port 201 CVLAN 3020*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].up_ivlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 3020;

    /*34. PON Port 200 Tunnel-ID 1003 SVLAN 30 SVLAN 40 <-CrossConnect-> NNI Port 201 SVLAN 3030 CVLAN 3040*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].up_ivlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 3030;
    pon_1_1_service_info[index].down_ivlan = 3040;

    /*35. PON Port 200 Tunnel-ID 1004 SVLAN 40 SVLAN 50 <-CrossConnect-> NNI Port 201 CVLAN 3040 CVLAN 3050*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].up_ivlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 3040;
    pon_1_1_service_info[index].down_ivlan = 3050;

    /*36. PON Port 200 Tunnel-ID 1005 SVLAN 50 SVLAN 60 <-CrossConnect-> NNI Port 201 SVLAN 3050 SVLAN 3060*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].up_ivlan = 60;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 3050;
    pon_1_1_service_info[index].down_ivlan = 3060;


    /*count number of 1_1 services*/
    nof_1_1_services = index + 1;

    rv = bcm_port_get(unit,port_pon,&flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlPONEnable $rv\n");
        return rv;
    }

    for (index = 0; index < nof_1_1_services; index++) {
        pon_1_1_service_info[index].pon_pp_port = mapping_info.pp_port;
    }

    return 0;
}
/*
 *Initialize the service models and set up PON application configurations.
 */
int pon_1_1_service_stc_mode_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index = 0;
    uint32 flags;
    int tunnel_idx;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int bit_shift = 0;

    bit_shift = channel_mode == 1 ? 12:13;
    pon_port = port_pon;
    nni_port = port_nni;

    /*1. PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index = 0;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = untag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 10;

    /*2. PON Port 200 Tunnel-ID 1001 <-CrossConnect-> NNI Port 201 CVLAN 20*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 20;

    /*3. PON Port 200 Tunnel-ID 1002 <-CrossConnect-> NNI Port 201 SVLAN 30 CVLAN 31*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_s_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 30;
    pon_1_1_service_info[index].down_ivlan = 31;

    /*4. PON Port 200 Tunnel-ID 1003 <-CrossConnect-> NNI Port 201 CVLAN 40 CVLAN 41*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_c_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 40;
    pon_1_1_service_info[index].down_ivlan = 41;

    /*5. PON Port 200 Tunnel-ID 1004 <-CrossConnect-> NNI Port 201 SVLAN 50 SVLAN 51*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_s_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 50;
    pon_1_1_service_info[index].down_ivlan = 51;

    /*6. PON Port 200 Tunnel-ID 1000 SVLAN 10 <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index++;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = s_tag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 100;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 100;

    /*7. PON Port 200 Tunnel-ID 1001 SVLAN 10 <-CrossConnect-> NNI Port 201 SVLAN 110*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 110;

    /*8. PON Port 200 Tunnel-ID 1002(DTC)/1001(STC) SVLAN 20 <-CrossConnect-> NNI Port 201 CVLAN 120*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 120;


    /*9. PON Port 200 Tunnel-ID 1003(DTC)/1002(STC) SVLAN 30 <-CrossConnect-> NNI Port 201 SVLAN 130 CVLAN 131*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 130;
    pon_1_1_service_info[index].down_ivlan = 131;

    /*10. PON Port 200 Tunnel-ID 1004(DTC)/1003(STC) SVLAN 40 <-CrossConnect-> NNI Port 201 CVLAN 140 CVLAN 141*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 140;
    pon_1_1_service_info[index].down_ivlan = 141;

    /*11. PON Port 200 Tunnel-ID 1005(DTC)/1004(STC) SVLAN 50 <-CrossConnect-> NNI Port 201 SVLAN 150 SVLAN 151*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 150;
    pon_1_1_service_info[index].down_ivlan = 151;

    /*12. PON Port 200 Tunnel-ID 1000 CVLAN 10 <-CrossConnect-> NNI Port 201 CVLAN 10*/
    index++;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = c_tag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 200;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 200;

    /*13. PON Port 200 Tunnel-ID 1001 CVLAN 10 <-CrossConnect-> NNI Port 201 CVLAN 210*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 210;

    /*14. PON Port 200 Tunnel-ID 1002 CVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 220*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 220;

    /*15. PON Port 200 Tunnel-ID 1003(DTC)/1001(STC) CVLAN 30 <-CrossConnect-> NNI Port 201 SVLAN 130 CVLAN 131*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 230;
    pon_1_1_service_info[index].down_ivlan = 231;

    /*16. PON Port 200 Tunnel-ID 1004(DTC)/1002(STC) CVLAN 40 <-CrossConnect-> NNI Port 201 CVLAN 240 CVLAN 241*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 240;
    pon_1_1_service_info[index].down_ivlan = 241;


    /*17. PON Port 200 Tunnel-ID 1005(DTC)/1003(STC) CVLAN 50 <-CrossConnect-> NNI Port 201 SVLAN 150 SVLAN 151*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 250;
    pon_1_1_service_info[index].down_ivlan = 251;    

    /*count number of 1_1 services*/
    nof_1_1_services = index + 1;

    rv = bcm_port_get(unit,port_pon,&flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlPONEnable $rv\n");
        return rv;
    }

    for (index = 0; index < nof_1_1_services; index++) {
        pon_1_1_service_info[index].pon_pp_port = mapping_info.pp_port;
    }

    return 0;
}

/*
 *Initialize the service models and set up PON application configurations.
 */
int pon_n_1_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index = 0;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int match_mode = 0;
    int tunnel_id = tunnel_base;
    int bit_shift = 0;

    bit_shift = channel_mode == 1 ? 12:13;
    pon_port = port_pon;
    nni_port = port_nni;
    tunnel_id = pon_subport_id << bit_shift | tunnel_base;

    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index = 0;
    pon_n_1_service_info[index].service_mode = untag_to_s_c_tag;
    pon_n_1_service_info[index].tunnel_id  = tunnel_id;
    pon_n_1_service_info[index].up_lif_type = match_untag;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 CVLAN 20*/
    index++;
    pon_n_1_service_info[index].service_mode = s_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = tunnel_id;    
    pon_n_1_service_info[index].up_lif_type = match_s_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 30 CVLAN 31*/
    index++;
    pon_n_1_service_info[index].service_mode = c_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = tunnel_id;
    pon_n_1_service_info[index].up_lif_type = match_c_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 CVLAN 40 CVLAN 41*/
    index++;
    pon_n_1_service_info[index].service_mode = s_c_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = tunnel_id;
    pon_n_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].up_ivlan = 81;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 50 SVLAN 51*/
    index++;
    pon_n_1_service_info[index].service_mode = c_c_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = tunnel_id;
    pon_n_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].up_ivlan = 81;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1000 SVLAN 10 <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index++;
    pon_n_1_service_info[index].service_mode = s_s_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = tunnel_id;
    pon_n_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].up_ivlan = 81;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /*count number of 1_1 services*/
    nof_n_1_services = index + 1;

    rv = bcm_port_get(unit,port_pon,&flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlPONEnable $rv\n");
        return rv;
    }

    for (index = 0; index < nof_n_1_services; index++) {
        pon_n_1_service_info[index].pon_pp_port = mapping_info.pp_port;
    }

    return 0;
}

/*
 *Initialize the service models and set up PON application configurations.
 */
int pon_n_1_service_stc_mode_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index = 0;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int tunnel_id = tunnel_base;
    int match_mode = 0;
    pon_port = port_pon;
    nni_port = port_nni;
    int bit_shift = 0;

    bit_shift = channel_mode == 1 ? 12:13;
    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index = 0;
    tunnel_id = pon_subport_id << bit_shift | tunnel_base;
    pon_n_1_service_info[index].service_mode = untag_to_s_c_tag;
    pon_n_1_service_info[index].tunnel_id  = tunnel_id;
    pon_n_1_service_info[index].up_lif_type = match_untag;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 CVLAN 20*/
    index++;
    pon_n_1_service_info[index].service_mode = s_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = tunnel_id;    
    pon_n_1_service_info[index].up_lif_type = match_s_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 30 CVLAN 31*/
    index++;

    pon_n_1_service_info[index].service_mode = c_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = tunnel_id;
    pon_n_1_service_info[index].up_lif_type = match_c_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /*count number of 1_1 services*/
    nof_n_1_services = index + 1;

    rv = bcm_port_get(unit,port_pon,&flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlPONEnable $rv\n");
        return rv;
    }

    for (index = 0; index < nof_n_1_services; index++) {
        pon_n_1_service_info[index].pon_pp_port = mapping_info.pp_port;
    }

    return 0;
}

/*
 * Set up 1:1 sercies, using port cross connect.
 * mode: 1: DTC mode; 0: STC mode
 */
int pon_1_1_service(int unit,int dtc_mode)
{
    int rv;
    int index, is_with_id = 0;
    uint32 flags;
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
    bcm_port_extender_mapping_info_t mapping_info;
    bcm_vlan_port_t vlan_port_pon;
    bcm_vlan_port_t vlan_port_nni;
    bcm_vswitch_cross_connect_t cross_connect_gports;

    /* Enable the PON */
    rv = bcm_port_control_set(unit, pon_port, bcmPortControlPONEnable, channel_mode);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlPONEnable $rv\n");
        return rv;
    }

    /* Mapping PTC + PON_SubPort_ID to PON_PP_PORT */
    for (index = 0; index < nof_1_1_services; index++) {
        flags = BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS;
        mapping_info.phy_port = pon_port;
        mapping_info.tunnel_id = pon_1_1_service_info[index].tunnel_id;
        mapping_info.pp_port = pon_1_1_service_info[index].pon_pp_port;
        rv = bcm_port_extender_mapping_info_set(unit, flags, bcmPortExtenderMappingTypePonTunnel,&mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_extender_mapping_info_set failed $rv\n");
            return rv;
        }
    }

    /* Configure the port as STC/DTC */
    rv = bcm_vlan_control_port_set(unit, pon_port, bcmVlanPortDoubleLookupEnable, dtc_mode);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_control_port_set failed bcmVlanPortDoubleLookupEnable $rv\n");
        return rv;
    }

    for (index = 0; index < nof_1_1_services; index++) {
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
        rv = pon_lif_create(unit, pon_port, pon_lif_type, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, 0, &pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_lif_create index:%d\n", index);
            print rv;
            return rv;
        }
        pon_1_1_service_info[index].pon_gport = pon_gport;

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
        pon_1_1_service_info[index].nni_gport = nni_gport;

        rv = bcm_vlan_gport_add(unit, pon_1_1_service_info[index].down_ovlan, nni_port, 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_gport_add NNI failed $rv\n");
            return rv;
        }

        /* Cross connect the 2 LIFs */
        cross_connect_gports.port1= pon_1_1_service_info[index].pon_gport;
        cross_connect_gports.port2= pon_1_1_service_info[index].nni_gport;
        rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_gports);
    }

    return rv;
}
int pon_1_1_service_cleanup(int unit)
{
    bcm_port_extender_mapping_info_t mapping_info;
    int rv, index = 0;
    bcm_vswitch_cross_connect_t cross_connect_gports;

    for (index = 0; index < nof_1_1_services; index++) {
        cross_connect_gports.port1= pon_1_1_service_info[index].pon_gport;
        cross_connect_gports.port2= pon_1_1_service_info[index].nni_gport;
        rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect_gports);
    }
    /* clean PON LIF ingress VLAN editor */
    rv = pon_port_egress_vt_destroy(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_port_egress_vt_destroy\n");
        print rv;
        return rv;
    }
    for (index = 0; index < nof_1_1_services; index++) {
        rv = pon_port_ingress_vt_destroy(unit,pon_1_1_service_info[index].service_mode, pon_1_1_service_info[index].down_ovlan,
                               pon_1_1_service_info[index].down_ivlan, pon_1_1_service_info[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_port_ingress_vt_destroy index:%d\n", index);
            print rv;
            return rv;
        }
    }

    return rv;
}

/*
 * Set up n:1 sercies.
 */
int pon_n_1_service(int unit, int dtc_mode)
{
    int rv;
    int index, is_with_id = 0;
    uint32 flags;
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
    bcm_port_extender_mapping_info_t mapping_info;
    bcm_vlan_port_t vlan_port_pon;
    bcm_vlan_port_t vlan_port_nni;
    bcm_vswitch_cross_connect_t cross_connect_gports;


    /* Enable the PON */
    rv = bcm_port_control_set(unit, pon_port, bcmPortControlPONEnable, channel_mode);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlPONEnable $rv\n");
        return rv;
    }

    /* Mapping PTC + PON_SubPort_ID to PON_PP_PORT */
    for (index = 0; index < nof_n_1_services; index++) {
        flags = BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS;
        mapping_info.phy_port = pon_port;
        mapping_info.tunnel_id = pon_n_1_service_info[index].tunnel_id;
        mapping_info.pp_port = pon_n_1_service_info[index].pon_pp_port;
        rv = bcm_port_extender_mapping_info_set(unit, flags, bcmPortExtenderMappingTypePonTunnel,&mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_extender_mapping_info_set failed $rv\n");
            return rv;
        }
    }

    /* Configure the port as STC/DTC */
    rv = bcm_vlan_control_port_set(unit, pon_port, bcmVlanPortDoubleLookupEnable, dtc_mode);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_control_port_set failed bcmVlanPortDoubleLookupEnable $rv\n");
        return rv;
    }

    vswitch_create(unit, &n_1_service_vsi);

    for (index = 0; index < nof_n_1_services; index++) {
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
        down_pcp     = pon_n_1_service_info[index].down_pcp;

        /* Create PON LIF */
        rv = pon_lif_create(unit, pon_port, pon_lif_type, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, 0, &pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_lif_create index:%d\n", index);
            print rv;
            return rv;
        }
        pon_n_1_service_info[index].pon_gport = pon_gport;

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

        if (index == 0)
        {
            /* Create NNI LIF */
            rv = nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_ovlan, down_ivlan, down_pcp, &nni_gport, &encap_id);
            if (rv != BCM_E_NONE) {
                printf("Error, nni_lif_create index:%d\n", index);
                print rv;
                return rv;
            }
            pon_n_1_service_info[index].nni_gport = nni_gport;
            
            rv = bcm_vlan_gport_add(unit, pon_n_1_service_info[index].down_ovlan, nni_port, 0);
            if (rv != BCM_E_NONE) {
                printf("bcm_vlan_gport_add NNI failed $rv\n");
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
    }

    return rv;
}
/*
 * clean up the configurations of N:1 sercies.
 */
int pon_n_1_service_cleanup(int unit)
{
    bcm_gport_t pon_gport, nni_gport;
    int rv, index = 0;

    /* clean the configure of the port as STC/DTC */
    for (index = 0; index < nof_n_1_services; index++)
    {
        pon_gport = pon_n_1_service_info[index].pon_gport;
        nni_gport = pon_n_1_service_info[index].nni_gport;
        rv = bcm_vswitch_port_delete(unit, n_1_service_vsi, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_delete\n");
            return rv;
        }

        /*Only one NNI LIF*/
        if (index == 0)
        {
            rv = bcm_vswitch_port_delete(unit, n_1_service_vsi, nni_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vswitch_port_delete\n");
                return rv;
            }
        }
    }

    /*1. delete multicast group*/
    rv = bcm_multicast_destroy(unit, n_1_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_destroy \n");
         return rv;
    }

    if (lif_offset != 0) {
        rv = bcm_multicast_destroy(unit, n_1_service_vsi+lif_offset);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_destroy \n");
            return rv;
        }
    }

    /*2. destroy vswitch */
    rv = bcm_vswitch_destroy(unit,n_1_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_destroy \n");
        return rv;
    }
    /* clean PON LIF ingress VLAN editor */
    rv = pon_port_egress_vt_destroy(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_port_egress_vt_destroy \n");
        print rv;
        return rv;
    }
    for (index = 0; index < nof_n_1_services; index++) {
        rv = pon_port_ingress_vt_destroy(unit,pon_n_1_service_info[index].service_mode, pon_n_1_service_info[index].down_ovlan,
                               pon_n_1_service_info[index].down_ivlan, pon_n_1_service_info[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_port_ingress_vt_destroy index:%d\n", index);
            print rv;
            return rv;
        }
    }

    return rv;
}

/*
 *Initialize the multi-subport configuration for 2 pon modes.
 */
int pon_1_1_service_m_subport_m_mode_set(int unit, uint32 subport_0_mode_1,uint32 subport_0_mode_2, bcm_port_t port_nni)
{
    int rv =0;
    /*set mode 1 configurations*/
    rv = pon_1_1_service_m_subport_one_mode_set(unit, pon_mode_1, subport_0_mode_1, 0, port_nni);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_1_1_service_m_subport_one_mode_set mode 1\n");
        print rv;
        return rv;
    }
    printf("pon_1_1_service_m_subport_one_mode_set mode 1 Succeed!\n");

    /*set mode 2 configurations*/
    rv = pon_1_1_service_m_subport_one_mode_set(unit, pon_mode_2, 0, subport_0_mode_2, port_nni);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_1_1_service_m_subport_one_mode_set mode 2\n");
        print rv;
        return rv;
    }
    printf("pon_1_1_service_m_subport_one_mode_set mode 2 Succeed!\n");

    return 0;
}

/*
 *Initialize the multi-subport configuration for 2 pon modes.
 */
int pon_1_1_service_m_subport_one_mode_set(int unit, int pon_mode,uint32 subport_0_mode_1,uint32 subport_0_mode_2, bcm_port_t port_nni)
{
    int rv =0;
    uint32 subport_0 = 0; 
    if (pon_mode == pon_mode_1)
    {
        /*set mode 1 configurations*/
        subport_0 = subport_0_mode_1;
    }
    else if (pon_mode == pon_mode_2)
    {
        /*set mode 2 configurations*/
        subport_0 = subport_0_mode_2;
    }
    
    rv = pon_1_1_service_m_subport_one_mode_init(unit, pon_mode, subport_0, subport_0+1, subport_0+2, subport_0+3, port_nni);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_1_1_service_m_subport_one_mode_init mode %d\n",pon_mode);
        print rv;
        return rv;
    }
    printf("pon_1_1_service_m_subport_one_mode_init mode %d Succeed!\n",pon_mode);
    rv = pon_1_1_service_m_subport_one_mode_config(unit, pon_mode, port_nni);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_1_1_service_m_subport_one_mode_config mode %d\n",pon_mode);
        print rv;
        return rv;
    }
    printf("pon_1_1_service_m_subport_one_mode_config mode %d Succeed!\n",pon_mode);

    return 0;
}


int pon_1_1_service_m_subport_one_mode_init(int unit, int pon_mode, uint32 subport_1,uint32 subport_2,uint32 subport_3,uint32 subport_4, bcm_port_t port_nni)
{
    int rv = 0;
    int index = 0,is_with_id = 0;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_tunnel_id_t tunnel_id = 0;
    int bit_shift = 12;
    int service_mode;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    uint8 down_pcp;
    int pon_lif_type, nni_lif_type,encap_id;
    bcm_gport_t pon_gport, nni_gport;
    bcm_port_extender_mapping_info_t extender_mapping_info;
    bcm_vswitch_cross_connect_t cross_connect_gports;    
    bcm_gport_t sys_gport;
    char cmd[300] = {0};
    int tunnel_id_base_mode = 0;

    nni_port = port_nni;
    
    if (pon_mode == pon_mode_1)
    {
        /* Mode 1: DNX_PON_PORT_MODE_1*/
        bit_shift = 12;
        tunnel_id_base_mode = tunnel_base;

        /*subport 0*/
        index = 0;
        tunnel_id = index << bit_shift | tunnel_id_base_mode + index;
        pon_m_subport_info_mode_1[index].pon_subport = subport_1;
        pon_m_subport_info_mode_1[index].service_mode = untag_to_s_tag;
        pon_m_subport_info_mode_1[index].tunnel_id  = tunnel_id;
        pon_m_subport_info_mode_1[index].up_lif_type = match_untag;
        pon_m_subport_info_mode_1[index].down_lif_type = match_s_tag;
        pon_m_subport_info_mode_1[index].down_ovlan = 10;
        
        /*subport 1*/
        /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 30 CVLAN 31*/
        index++;    
        tunnel_id = index << bit_shift | tunnel_id_base_mode + index;
        pon_m_subport_info_mode_1[index].pon_subport = subport_2;
        pon_m_subport_info_mode_1[index].service_mode = s_tag_to_s2_tag;
        pon_m_subport_info_mode_1[index].tunnel_id  = tunnel_id;    
        pon_m_subport_info_mode_1[index].up_lif_type = match_s_tag;
        pon_m_subport_info_mode_1[index].up_ovlan = 10;
        pon_m_subport_info_mode_1[index].down_lif_type = match_s_tag;
        pon_m_subport_info_mode_1[index].down_ovlan = 110;
        
        /*subport 2*/
        index++;
        tunnel_id = index << bit_shift | tunnel_id_base_mode + index;
        pon_m_subport_info_mode_1[index].pon_subport = subport_3;
        pon_m_subport_info_mode_1[index].service_mode = untag_to_c_tag;
        pon_m_subport_info_mode_1[index].tunnel_id  = tunnel_id;    
        pon_m_subport_info_mode_1[index].up_lif_type = match_untag;
        pon_m_subport_info_mode_1[index].down_lif_type = match_c_tag;
        pon_m_subport_info_mode_1[index].down_ovlan = 20;
        
        /*subport 3*/
        index++;
        tunnel_id = index << bit_shift | tunnel_id_base_mode + index;    
        pon_m_subport_info_mode_1[index].pon_subport = subport_4;
        pon_m_subport_info_mode_1[index].service_mode = c_tag_to_s2_c2_tag;
        pon_m_subport_info_mode_1[index].tunnel_id  = tunnel_id;    
        pon_m_subport_info_mode_1[index].up_lif_type = match_c_tag;
        pon_m_subport_info_mode_1[index].up_ovlan = 30;
        pon_m_subport_info_mode_1[index].down_lif_type = match_s_c_tag;
        pon_m_subport_info_mode_1[index].down_ovlan = 130;
        pon_m_subport_info_mode_1[index].down_ivlan = 131;
        
        /*count number of 1_1 services*/
        nof_pon_m_subport_mode_1 = index + 1;
    }    
    else if (pon_mode == pon_mode_2)
    {
        /* Mode 2: DNX_PON_PORT_MODE_2*/
        bit_shift = 13;
        tunnel_id_base_mode = tunnel_base + 0x1000;
        /*subport 0*/
        index = 0;
        tunnel_id = index << bit_shift | tunnel_id_base_mode + index;
        pon_m_subport_info_mode_2[index].pon_subport = subport_1;
        pon_m_subport_info_mode_2[index].service_mode = untag_to_s_tag;
        pon_m_subport_info_mode_2[index].tunnel_id  = tunnel_id;
        pon_m_subport_info_mode_2[index].up_lif_type = match_untag;
        pon_m_subport_info_mode_2[index].down_lif_type = match_s_tag;
        pon_m_subport_info_mode_2[index].down_ovlan = 300;
        
        /*subport 1*/
        index++;    
        tunnel_id = index << bit_shift | tunnel_id_base_mode + index;
        pon_m_subport_info_mode_2[index].pon_subport = subport_2;
        pon_m_subport_info_mode_2[index].service_mode = s_tag_to_s2_tag;
        pon_m_subport_info_mode_2[index].tunnel_id  = tunnel_id;    
        pon_m_subport_info_mode_2[index].up_lif_type = match_s_tag;
        pon_m_subport_info_mode_2[index].up_ovlan = 400;
        pon_m_subport_info_mode_2[index].down_lif_type = match_s_tag;
        pon_m_subport_info_mode_2[index].down_ovlan = 1400;
        
        /*subport 2*/
        index++;
        tunnel_id = index << bit_shift | tunnel_id_base_mode + index;
        pon_m_subport_info_mode_2[index].pon_subport = subport_3;
        pon_m_subport_info_mode_2[index].service_mode = untag_to_c_tag;
        pon_m_subport_info_mode_2[index].tunnel_id  = tunnel_id;    
        pon_m_subport_info_mode_2[index].up_lif_type = match_untag;
        pon_m_subport_info_mode_2[index].down_lif_type = match_c_tag;
        pon_m_subport_info_mode_2[index].down_ovlan = 300;
        
        /*subport 3*/
        index++;
        tunnel_id = index << bit_shift | tunnel_id_base_mode + index;    
        pon_m_subport_info_mode_2[index].pon_subport = subport_4;
        pon_m_subport_info_mode_2[index].service_mode = c_tag_to_s2_c2_tag;
        pon_m_subport_info_mode_2[index].tunnel_id  = tunnel_id;    
        pon_m_subport_info_mode_2[index].up_lif_type = match_c_tag;
        pon_m_subport_info_mode_2[index].up_ovlan = 500;
        pon_m_subport_info_mode_2[index].down_lif_type = match_s_c_tag;
        pon_m_subport_info_mode_2[index].down_ovlan = 1500;
        pon_m_subport_info_mode_2[index].down_ivlan = 1510;
        
        /*count number of 1_1 services*/
        nof_pon_m_subport_mode_2 = index + 1;
    }
    else
    {
        rv = BCM_E_PARAM;
        printf("Enable PON port with invalid PON mode %d!\n",pon_mode);
        return rv;
    }
    return 0;
}
 
/*
*set the n:1 service model and set up PON application configurations for 4 subports.
*/
int pon_1_1_service_m_subport_one_mode_config(int unit, int pon_mode, bcm_port_t port_nni)
{
    int rv = 0;
    int index = 0,is_with_id = 0;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_tunnel_id_t tunnel_id = 0;
    int bit_shift = 12;
    int service_mode;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    uint8 down_pcp;
    int pon_lif_type, nni_lif_type,encap_id;
    bcm_gport_t pon_gport, nni_gport;
    bcm_port_extender_mapping_info_t extender_mapping_info;
    bcm_vswitch_cross_connect_t cross_connect_gports;    
    bcm_gport_t sys_gport;
    char cmd[300] = {0};
    int tunnel_id_base_mode = 0;    
    pon_service_multi_subport_info_s pon_m_subport_info[4];
    int nof_pon_m_subport = 0;

    sal_memset(pon_m_subport_info,0,sizeof(pon_m_subport_info));

    nni_port = port_nni;
    
    if (pon_mode == pon_mode_1)
    {        
        tunnel_id_base_mode = tunnel_base;
        nof_pon_m_subport = nof_pon_m_subport_mode_1;        
        sal_memcpy(pon_m_subport_info,pon_m_subport_info_mode_1,sizeof(pon_m_subport_info));
    }    
    else if (pon_mode == pon_mode_2)
    {
        tunnel_id_base_mode = tunnel_base + 0x1000;
        nof_pon_m_subport = nof_pon_m_subport_mode_2;        
        sal_memcpy(pon_m_subport_info,pon_m_subport_info_mode_2,sizeof(pon_m_subport_info));
    }    
    else
    {
        rv = BCM_E_PARAM;
        printf("Enable PON port with invalid PON mode %d!\n",pon_mode);
        return rv;
    }
    
    /* Enable the PON */
    for (index = 0; index < nof_pon_m_subport; index++) 
    {
        rv = bcm_port_control_set(unit, pon_m_subport_info[index].pon_subport, bcmPortControlPONEnable, pon_mode);
        if (rv != BCM_E_NONE) {
            printf("Enable COE port[%d] failed!\n", pon_m_subport_info[index].pon_subport);
            return rv;
        }
        
        /* Mapping PTC + PON_SubPort_ID to PON_PP_PORT */
        flags = BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS;
        extender_mapping_info.phy_port = pon_m_subport_info[index].pon_subport;
        extender_mapping_info.tunnel_id = pon_m_subport_info[index].tunnel_id;
        rv = bcm_port_extender_mapping_info_set(unit, flags, bcmPortExtenderMappingTypePonTunnel,&extender_mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_extender_mapping_info_set failed %d\n", rv);
            return rv;
        }
        /* Configure the port as STC/DTC */
        rv = bcm_vlan_control_port_set(unit, pon_m_subport_info[index].pon_subport, bcmVlanPortDoubleLookupEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_control_port_set failed bcmVlanPortDoubleLookupEnable %d\n",rv);
            return rv;
        }
    }
    for (index = 0; index < nof_pon_m_subport; index++)
    {
        pon_gport = 0;
        nni_gport = 0;
        service_mode = pon_m_subport_info[index].service_mode;
        pon_lif_type = pon_m_subport_info[index].up_lif_type;
        up_ovlan     = pon_m_subport_info[index].up_ovlan;
        up_ivlan     = pon_m_subport_info[index].up_ivlan;
        up_pcp       = pon_m_subport_info[index].up_pcp;
        up_ethertype = pon_m_subport_info[index].up_ethertype;
        nni_lif_type = pon_m_subport_info[index].down_lif_type;
        down_ovlan   = pon_m_subport_info[index].down_ovlan;
        down_ivlan   = pon_m_subport_info[index].down_ivlan;
        down_pcp     = pon_m_subport_info[index].down_pcp;
    
        /* Create PON LIF */
        rv = pon_lif_create(unit, pon_m_subport_info[index].pon_subport, pon_lif_type, pon_m_subport_info[index].tunnel_id, 
                          pon_m_subport_info[index].tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, 0, &pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_lif_create index:%d\n", index);
            print rv;
            return rv;
        }
        pon_m_subport_info[index].pon_gport = pon_gport;
    
        /* Set PON LIF ingress VLAN editor */
        rv = pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_port_ingress_vt_set index:%d\n", index);
            print rv;
            return rv;
        }
    
        /* Set PON LIF egress VLAN editor */
        rv = pon_port_egress_vt_set(unit, service_mode, pon_m_subport_info[index].tunnel_id, up_ovlan, up_ivlan, pon_gport);
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
        pon_m_subport_info[index].nni_gport = nni_gport;
    
        rv = bcm_vlan_gport_add(unit, pon_m_subport_info[index].down_ovlan, nni_port, 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_gport_add NNI failed %d\n", rv);
            return rv;
        }
        /* Cross connect the 2 LIFs */
        cross_connect_gports.port1= pon_m_subport_info[index].pon_gport;
        cross_connect_gports.port2= pon_m_subport_info[index].nni_gport;
        rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_gports);
    
    }
    if (pon_mode == pon_mode_1)
    {
        sal_memcpy(pon_m_subport_info_mode_1, pon_m_subport_info,sizeof(pon_m_subport_info));
    }
    else if (pon_mode == pon_mode_2)
    {
        sal_memcpy(pon_m_subport_info_mode_2, pon_m_subport_info,sizeof(pon_m_subport_info));
    }
    return 0;

}

/*
 *cleanup the PON application configurations for multi-subports with 2 modes.
 */
int pon_1_1_service_m_subport_m_mode_cleanup(int unit)
{
    int rv = 0;
    int i = 0;

    for (i = pon_mode_1; i <= pon_mode_2; i++)
    {
        rv = pon_1_1_service_m_subport_one_mode_cleanup(unit, i);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_1_1_service_m_subport_one_mode_cleanup mode %d!\n",i);
            print rv;
            return rv;
        }
    }
    return rv;
}

/*
 *cleanup the PON application configurations for given mode.
 */
int pon_1_1_service_m_subport_one_mode_cleanup(int unit, int pon_mode)
{
    bcm_port_extender_mapping_info_t mapping_info;
    int rv, index = 0;
    bcm_vswitch_cross_connect_t cross_connect_gports;

    if (pon_mode == pon_mode_1)
    {
        /** clean PON mode 1 configuration*/
        for (index = 0; index < nof_pon_m_subport_mode_1; index++) {
            cross_connect_gports.port1= pon_m_subport_info_mode_1[index].pon_gport;
            cross_connect_gports.port2= pon_m_subport_info_mode_1[index].nni_gport;
            rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect_gports);
        }
        /** clean PON LIF ingress VLAN editor */
        rv = pon_port_egress_vt_destroy(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_port_egress_vt_destroy\n");
            print rv;
            return rv;
        }
        for (index = 0; index < nof_pon_m_subport_mode_1; index++) {
            rv = pon_port_ingress_vt_destroy(unit,pon_m_subport_info_mode_1[index].service_mode, pon_m_subport_info_mode_1[index].down_ovlan,
                                   pon_m_subport_info_mode_1[index].down_ivlan, pon_m_subport_info_mode_1[index].pon_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, pon_port_ingress_vt_destroy index:%d\n", index);
                print rv;
                return rv;
            }
        }     
    }
    else if (pon_mode == pon_mode_2)
    {
        /** clean PON mode 2 configuration*/
        for (index = 0; index < nof_pon_m_subport_mode_2; index++) {
            cross_connect_gports.port1= pon_m_subport_info_mode_2[index].pon_gport;
            cross_connect_gports.port2= pon_m_subport_info_mode_2[index].nni_gport;
            rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect_gports);
        }
        /** clean PON LIF ingress VLAN editor */
        rv = pon_port_egress_vt_destroy(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_port_egress_vt_destroy\n");
            print rv;
            return rv;
        }
        for (index = 0; index < nof_pon_m_subport_mode_2; index++) {
            rv = pon_port_ingress_vt_destroy(unit,pon_m_subport_info_mode_2[index].service_mode, pon_m_subport_info_mode_2[index].down_ovlan,
                                   pon_m_subport_info_mode_2[index].down_ivlan, pon_m_subport_info_mode_2[index].pon_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, pon_port_ingress_vt_destroy index:%d\n", index);
                print rv;
                return rv;
            }
        }
    }

    return rv;
}


/**disable all the PON subports for given pon modes*/
int pon_1_1_service_m_subport_m_mode_disable(int unit)
{
    int rv = 0;
    int i = 0;
    for (i = pon_mode_1; i <= pon_mode_2; i++)
    {
        rv = pon_1_1_service_m_subport_one_mode_disable(unit,i);
        if (rv != BCM_E_NONE) {
            printf("Error, pon_1_1_service_m_subport_one_mode_disable mode %d!\n",i);
            print rv;
            return rv;
        }
        
    }
    return rv;
}

/**disable all the PON subports for given pon modes*/
int pon_1_1_service_m_subport_one_mode_disable(int unit,int pon_mode)
{
    int rv, index = 0;
    /**disable for mode 1*/
    if (pon_mode == pon_mode_1)
    {
        for (index = 0; index < nof_pon_m_subport_mode_1; index++) {
            rv = bcm_port_control_set(unit, pon_m_subport_info_mode_1[index].pon_subport, bcmPortControlPONEnable, 0);
            if (rv != BCM_E_NONE) {
                printf("Disable PON port[%d] failed!\n", pon_m_subport_info_mode_1[index].pon_subport);
                return rv;
            }
        }
    }
    else if (pon_mode == pon_mode_2)
    {        
        /**disable for mode 2*/
        for (index = 0; index < nof_pon_m_subport_mode_2; index++) {
            rv = bcm_port_control_set(unit, pon_m_subport_info_mode_2[index].pon_subport, bcmPortControlPONEnable, 0);
            if (rv != BCM_E_NONE) {
                printf("Disable PON port[%d] failed!\n", pon_m_subport_info_mode_2[index].pon_subport);
                return rv;
            }
        }
    }
    else
    {
        rv = BCM_E_PARAM;
        printf("Enable PON port with invalid PON mode %d!\n",pon_mode);
        return rv;
    }
    return rv;
}

bcm_gport_t mirror_dest_id;

/*
 * packets egress from channelized nif ports need mirroring to pon_port(cpu port)
 */
int pon_1_1_service_m_subport_mirror_create(int unit, bcm_port_t mir_src, bcm_port_t mir_dest)
{
    int rv;
    int flags;
    bcm_mirror_destination_t dest;

    bcm_mirror_destination_t_init(&dest);

    flags = BCM_MIRROR_PORT_EGRESS;

    if (BCM_GPORT_IS_SET(mir_dest))
    {
        dest.gport = mir_dest;
    }
    else
    {
        BCM_GPORT_LOCAL_SET(dest.gport, mir_dest);
    }

    rv = bcm_mirror_destination_create(unit, &dest);
    if(rv != BCM_E_NONE)
    {
        printf("failed to create mirror destination , return value %d\n", rv);
        return rv;
    }
    
    mirror_dest_id = dest.mirror_dest_id;
    
    rv = bcm_mirror_port_dest_add(unit, mir_src, flags, mirror_dest_id);
    if(rv != BCM_E_NONE)
    {
        printf("failed to add mir_src in bcm_mirror_port_dest_add\n");
        return rv;
    }

    return rv;
}

int pon_1_1_service_m_subport_mirror_destroy(int unit, bcm_port_t mir_src)
{
    int rv;
    int flags;

    flags = BCM_MIRROR_PORT_EGRESS;
    rv = bcm_mirror_port_dest_delete(unit, mir_src, flags, mirror_dest_id);
    if(rv != BCM_E_NONE)
    {
        printf("failed to add mir_src in bcm_mirror_port_dest_add\n");
        return rv;
    }

    rv = bcm_mirror_destination_destroy(unit, mirror_dest_id);
    if(rv != BCM_E_NONE)
    {
        printf("failed to create mirror destination , return value %d\n", rv);
        return rv;
    }

    return rv;
}

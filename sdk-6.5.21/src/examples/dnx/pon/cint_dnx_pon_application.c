/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
};

bcm_port_t pon_port = 200;
bcm_port_t nni_port = 201;

int nof_1_1_services;
pon_service_info_s pon_1_1_service_info[36];

bcm_vlan_t n_1_service_vsi; 
int nof_n_1_services;
pon_service_info_s pon_n_1_service_info[6];

/*
 *Initialize the service models and set up PON application configurations.
 */
int pon_1_1_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index = 0;
    uint32 flags;
    int tunnel_base = 1000;
    int tunnel_idx;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    pon_port = port_pon;
    nni_port = port_nni;
    
    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 10*/   
    index = 0; 
    tunnel_idx = tunnel_base;
    pon_1_1_service_info[index].service_mode = untag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 10;

    /* PON Port 200 Tunnel-ID 1001 <-CrossConnect-> NNI Port 201 CVLAN 20*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 20;

    /* PON Port 200 Tunnel-ID 1002 <-CrossConnect-> NNI Port 201 SVLAN 30 CVLAN 31*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_s_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 30;
    pon_1_1_service_info[index].down_ivlan = 31;

    /* PON Port 200 Tunnel-ID 1003 <-CrossConnect-> NNI Port 201 CVLAN 40 CVLAN 41*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_c_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 40;
    pon_1_1_service_info[index].down_ivlan = 41;

    /* PON Port 200 Tunnel-ID 1004 <-CrossConnect-> NNI Port 201 SVLAN 50 SVLAN 51*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_s_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 50;
    pon_1_1_service_info[index].down_ivlan = 51;

    /* PON Port 200 Tunnel-ID 1000 SVLAN 10 <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index++;
    tunnel_idx = tunnel_base;
    pon_1_1_service_info[index].service_mode = s_tag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 100;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 100;

    /* PON Port 200 Tunnel-ID 1001 SVLAN 10 <-CrossConnect-> NNI Port 201 SVLAN 110*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 110;

    /* PON Port 200 Tunnel-ID 1002 SVLAN 20 <-CrossConnect-> NNI Port 201 CVLAN 120*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 120;

    /* PON Port 200 Tunnel-ID 1003 SVLAN 30 <-CrossConnect-> NNI Port 201 SVLAN 130 CVLAN 131*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 130;
    pon_1_1_service_info[index].down_ivlan = 131;
    
    /* PON Port 200 Tunnel-ID 1004 SVLAN 40 <-CrossConnect-> NNI Port 201 CVLAN 140 CVLAN 141*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 140;
    pon_1_1_service_info[index].down_ivlan = 141;
    
    /* PON Port 200 Tunnel-ID 1005 SVLAN 50 <-CrossConnect-> NNI Port 201 SVLAN 150 SVLAN 151*/
    index++;
    pon_1_1_service_info[index].service_mode = s_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 150;
    pon_1_1_service_info[index].down_ivlan = 151;
    
    /* PON Port 200 Tunnel-ID 1000 CVLAN 10 <-CrossConnect-> NNI Port 201 CVLAN 10*/
    index++;
    tunnel_idx = tunnel_base;
    pon_1_1_service_info[index].service_mode = c_tag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 200;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 200;
    
    /* PON Port 200 Tunnel-ID 1001 CVLAN 10 <-CrossConnect-> NNI Port 201 CVLAN 210*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 210;
    
    /* PON Port 200 Tunnel-ID 1002 CVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 220*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 220;
    
    /* PON Port 200 Tunnel-ID 1003 CVLAN 30 <-CrossConnect-> NNI Port 201 SVLAN 130 CVLAN 131*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 230;
    pon_1_1_service_info[index].down_ivlan = 231;
    
    /* PON Port 200 Tunnel-ID 1004 CVLAN 40 <-CrossConnect-> NNI Port 201 CVLAN 240 CVLAN 241*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 240;
    pon_1_1_service_info[index].down_ivlan = 241;
    
    /* PON Port 200 Tunnel-ID 1005 CVLAN 50 <-CrossConnect-> NNI Port 201 SVLAN 150 SVLAN 151*/
    index++;
    pon_1_1_service_info[index].service_mode = c_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 250;
    pon_1_1_service_info[index].down_ivlan = 251;    
    
    /* PON Port 200 Tunnel-ID 1000 SVLAN 1000 CVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 1000*/
    index++;
    tunnel_idx = tunnel_base;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 1000;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 1000;
    
    /* PON Port 200 Tunnel-ID 1001 SVLAN 10 CVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 1010*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 1010;
    
    /* PON Port 200 Tunnel-ID 1002 SVLAN 1000 CVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 1010*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].up_ivlan = 1020;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 1020;
    
    /* PON Port 200 Tunnel-ID 1003 SVLAN 30 CVLAN 40 <-CrossConnect-> NNI Port 201 CVLAN 1030*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].up_ivlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 1030;
    
    /* PON Port 200 Tunnel-ID 1004 SVLAN 40 CVLAN 50 <-CrossConnect-> NNI Port 201 SVLAN 1040 CVLAN 1050*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].up_ivlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 1040;
    pon_1_1_service_info[index].down_ivlan = 1050;
    
    /* PON Port 200 Tunnel-ID 1005 SVLAN 50 CVLAN 60 <-CrossConnect-> NNI Port 201 CVLAN 1050 CVLAN 1060*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].up_ivlan = 60;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 1050;
    pon_1_1_service_info[index].down_ivlan = 1060;
    
    /* PON Port 200 Tunnel-ID 1006 SVLAN 60 CVLAN 70 <-CrossConnect-> NNI Port 201 SVLAN 1060 SVLAN 1070*/
    index++;
    pon_1_1_service_info[index].service_mode = s_c_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].up_ovlan = 60;
    pon_1_1_service_info[index].up_ivlan = 70;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 1060;
    pon_1_1_service_info[index].down_ivlan = 1070;
    
    /* PON Port 200 Tunnel-ID 1000 CVLAN 2000 CVLAN 20 <-CrossConnect-> NNI Port 201 CVLAN 2000*/
    index++;
    tunnel_idx = tunnel_base;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 2000;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 2000;
    
    /* PON Port 200 Tunnel-ID 1001 CVLAN 10 CVLAN 20 <-CrossConnect-> NNI Port 201 CVLAN 2010*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 2010;
    
    /* PON Port 200 Tunnel-ID 1002 CVLAN 20 CVLAN 30 <-CrossConnect-> NNI Port 201 SVLAN 2020*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].up_ivlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 2020;
    
    /* PON Port 200 Tunnel-ID 1003 CVLAN 30 CVLAN 40 <-CrossConnect-> NNI Port 201 SVLAN 2030 CVLAN 2040*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].up_ivlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 2030;
    pon_1_1_service_info[index].down_ivlan = 2040;
    
    /* PON Port 200 Tunnel-ID 1004 CVLAN 40 CVLAN 50 <-CrossConnect-> NNI Port 201 CVLAN 2040 CVLAN 2050*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].up_ivlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 2040;
    pon_1_1_service_info[index].down_ivlan = 2050;
    
    /* PON Port 200 Tunnel-ID 1005 CVLAN 50 CVLAN 60 <-CrossConnect-> NNI Port 201 SVLAN 1050 SVLAN 1060*/
    index++;
    pon_1_1_service_info[index].service_mode = c_c_tag_to_s2_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].up_ovlan = 50;
    pon_1_1_service_info[index].up_ivlan = 60;
    pon_1_1_service_info[index].down_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].down_ovlan = 2050;
    pon_1_1_service_info[index].down_ivlan = 2060;
    
    /* PON Port 200 Tunnel-ID 1000 SVLAN 3000 SVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 3000*/
    index++;
    tunnel_idx = tunnel_base;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 3000;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 3000;
    
    /* PON Port 200 Tunnel-ID 1001 SVLAN 10 SVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 3010*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 3010;
    
    /* PON Port 200 Tunnel-ID 1002 SVLAN 20 SVLAN 30 <-CrossConnect-> NNI Port 201 CVLAN 3020*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].up_ivlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 3020;
    
    /* PON Port 200 Tunnel-ID 1003 SVLAN 30 SVLAN 40 <-CrossConnect-> NNI Port 201 SVLAN 3030 CVLAN 3040*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_s2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 30;
    pon_1_1_service_info[index].up_ivlan = 40;
    pon_1_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_1_1_service_info[index].down_ovlan = 3030;
    pon_1_1_service_info[index].down_ivlan = 3040;
    
    /* PON Port 200 Tunnel-ID 1004 SVLAN 40 SVLAN 50 <-CrossConnect-> NNI Port 201 CVLAN 3040 CVLAN 3050*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_c2_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 40;
    pon_1_1_service_info[index].up_ivlan = 50;
    pon_1_1_service_info[index].down_lif_type = match_c_c_tag;
    pon_1_1_service_info[index].down_ovlan = 3040;
    pon_1_1_service_info[index].down_ivlan = 3050;
    
    /* PON Port 200 Tunnel-ID 1005 SVLAN 50 SVLAN 60 <-CrossConnect-> NNI Port 201 SVLAN 3050 SVLAN 3060*/
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
int pon_n_1_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index = 0;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    pon_port = port_pon;
    nni_port = port_nni;
    
    /* PON Port 200 Tunnel-ID 1000 <-CrossConnect-> NNI Port 201 SVLAN 10*/   
    index = 0;
    pon_n_1_service_info[index].service_mode = untag_to_s_c_tag;
    pon_n_1_service_info[index].tunnel_id  = 1000;
    pon_n_1_service_info[index].up_lif_type = match_untag;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1001 <-CrossConnect-> NNI Port 201 CVLAN 20*/
    index++;
    pon_n_1_service_info[index].service_mode = s_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = 1000;    
    pon_n_1_service_info[index].up_lif_type = match_s_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1002 <-CrossConnect-> NNI Port 201 SVLAN 30 CVLAN 31*/
    index++;
    pon_n_1_service_info[index].service_mode = c_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = 1000;
    pon_n_1_service_info[index].up_lif_type = match_c_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1003 <-CrossConnect-> NNI Port 201 CVLAN 40 CVLAN 41*/
    index++;
    pon_n_1_service_info[index].service_mode = s_c_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = 1000;
    pon_n_1_service_info[index].up_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].up_ivlan = 81;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1004 <-CrossConnect-> NNI Port 201 SVLAN 50 SVLAN 51*/
    index++;
    pon_n_1_service_info[index].service_mode = c_c_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = 1000;
    pon_n_1_service_info[index].up_lif_type = match_c_c_tag;
    pon_n_1_service_info[index].up_ovlan = 80;
    pon_n_1_service_info[index].up_ivlan = 81;
    pon_n_1_service_info[index].down_lif_type = match_s_c_tag;
    pon_n_1_service_info[index].down_ovlan = 800;
    pon_n_1_service_info[index].down_ivlan = 810;

    /* PON Port 200 Tunnel-ID 1000 SVLAN 10 <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index++;
    pon_n_1_service_info[index].service_mode = s_s_tag_to_s2_c2_tag;
    pon_n_1_service_info[index].tunnel_id  = 1000;
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
 * Set up 1:1 sercies, using port cross connect.
 */
int pon_1_1_service(int unit)
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
    rv = bcm_port_control_set(unit, pon_port, bcmPortControlPONEnable, 1);
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
    rv = bcm_vlan_control_port_set(unit, pon_port, bcmVlanPortDoubleLookupEnable, 1);
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
        rv = pon_lif_create(unit, pon_port, pon_lif_type, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);
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

/*
 * Set up n:1 sercies.
 */
int pon_n_1_service(int unit)
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
    rv = bcm_port_control_set(unit, pon_port, bcmPortControlPONEnable, 1);
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
    rv = bcm_vlan_control_port_set(unit, pon_port, bcmVlanPortDoubleLookupEnable, 1);
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
        rv = pon_lif_create(unit, pon_port, pon_lif_type, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);
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



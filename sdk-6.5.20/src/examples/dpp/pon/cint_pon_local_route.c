/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_pon_local_route.c
* Purpose: An example of the PON application for local route feature. 
*
* Calling sequence:
*
* Initialization:
*  1. Add the following port configureations to config-sand.bcm
*        ucode_port_129.BCM88650=10GBase-R15
*        ucode_port_5.BCM88650=10GBase-R14
*        ucode_port_4.BCM88650=10GBase-R13
*  2. Add the following soc properties to config-sand.bcm
*        local_switching_enable=1
*        bcm886xx_auxiliary_table_mode=1 (for arad/+ devices only)
*        split_horizon_forwarding_groups_mode=1 (for jericho and jericho above device)
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
* N:1 Service:
*            PON Port 4,5  <--------------------->  VSI  <---> 1 NNI Port 129
*     PON 4 Tunnel-ID 1000 CVLAN 60--------------|
*     PON 4 Tunnel-ID 1000 CVLAN 61--------------|
*     PON 5 Tunnel-ID 1001 CVLAN 62--------------|--- 4096  ---  SVLAN 600 CVLAN 601
*     PON 5 Tunnel-ID 1001 CVLAN 63--------------|
*           PON Port 4,5  <--------------------->  VSI  <---> 1 NNI Port 129
*     PON 4 Tunnel-ID 1000 CVLAN 80--------------|
*     PON 4 Tunnel-ID 1000 CVLAN 81--------------|
*     PON 5 Tunnel-ID 1001 CVLAN 82--------------|--- 4097  ---  SVLAN 800 CVLAN 801
*     PON 5 Tunnel-ID 1001 CVLAN 83--------------|
*
* Traffic:
*
*                           NNI 129 SVLAN 800 CVLAN 801
*                           Send: 5.1
*                           Receive: 1.1,1.5,2.1,3.1,4.1,4.5
*                                        |
*                                        | InG=0
*                                        | OutG=0
*                                        |
*              _ _ _ _ _ _ _ _ _ _ _ _ _ | _ _ _ _ _ _ _ _ _ _ _ _
*             |                                                   |
*             |                    VSI = 4097                     | 
*             |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _| 
*               |               |               |               |
*               |               |               |               |
*               | InG=1         | InG=1         | InG=1         | InG=1
*               | OutG=1        | OutG=0        | OutG=1        | OutG=0
*               |               |               |               |
*               |               |               |               |
*   PON 4 Tunnel 1000 CVLAN 80  | PON 5 Tunnel 1001 CVLAN 80    |
*   Send: 1.1,1.2,1.3,1.4,1.5   | Send: 3.1                     |
*   Receive:                    | Receive:                      |
*                               |                               |
*                  PON 4 Tunnel 1000 CVLAN 80      PON 5 Tunnel 1001 CVLAN 80
*                  Send: 2.1                       Send: 4.1,4.2,4.3,4.4,4.5                     
*                  Receive: 1.2,1.5,3.1,4.3,4.5    Receive: 1.4,1.5,2.1,3.1                                       
*
*
*
*  - From PON port 4 Tunnel-ID 1000 CVLAN 80:
*      -   ethernet header with DA 00:00:00:00:81:01, SA 00:00:00:00:04:01 --(1.1) 
*      -   ethernet header with DA 00:00:00:00:04:02, SA 00:00:00:00:04:01 --(1.2)
*      -   ethernet header with DA 00:00:00:00:05:01, SA 00:00:00:00:04:01 --(1.3)
*      -   ethernet header with DA 00:00:00:00:05:02, SA 00:00:00:00:04:01 --(1.4)
*      -   ethernet header with DA 00:00:00:00:ff:ff, SA 00:00:00:00:04:01 --(1.5)
*      -   Tunnel ID:1000
*      -   VLAN tag: VLAN tag type 0x9100, VID = 80
*
*  - From PON port 4 Tunnel-ID 1000 CVLAN 81:
*      -   ethernet header with DA 00:00:00:00:ff:ff, SA 00:00:00:00:04:02 --(2.1)
*      -   Tunnel ID:1000
*      -   VLAN tag: VLAN tag type 0x9100, VID = 81
*
*  - From PON port 5 Tunnel-ID 1001 CVLAN 82:
*      -   ethernet header with DA 00:00:00:00:ff:ff, SA 00:00:00:00:05:01 --(3.1)
*      -   Tunnel ID:1000
*      -   VLAN tag: VLAN tag type 0x9100, VID = 82
*
*  - From PON port 5 Tunnel-ID 1001 CVLAN 83:
*      -   ethernet header with DA 00:00:00:00:81:01, SA 00:00:00:00:05:02 --(4.1)
*      -   ethernet header with DA 00:00:00:00:04:01, SA 00:00:00:00:05:02 --(4.2)
*      -   ethernet header with DA 00:00:00:00:04:02, SA 00:00:00:00:05:02 --(4.3)
*      -   ethernet header with DA 00:00:00:00:05:01, SA 00:00:00:00:05:02 --(4.4)
*      -   ethernet header with DA 00:00:00:00:ff:ff, SA 00:00:00:00:05:02 --(4.5)
*      -   Tunnel ID:1000
*      -   VLAN tag: VLAN tag type 0x9100, VID = 83
*
*  - From NNI port 129 SVLAN 800 CVLAN 801:
*      -   ethernet header with DA 00:00:00:00:ff:ff, SA 00:00:00:00:81:01 --(5.1)
*      -   VLAN tag: VLAN tag type 0x8100, VID = 800, VLAN tag type 0x9100, VID = 801
*
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_tpid.c
*      BCM> cint examples/dpp/pon/cint_pon_utils.c
*      BCM> cint examples/dpp/pon/cint_pon_local_route.c
*      BCM> cint
*      cint> pon_app_init(unit, pon_port_1, pon_port_2, nni_port);
*      cint> pon_n_1_service(unit);
*      cint> pon_n_1_service_verfiy(unit);
*      cint> pon_n_1_service_cleanup(unit);
*
* Note: This example is available for normal vlan mode. 
*       For advance vlan mode, please refer to cint_pon_application.c, and user can add local route function with cint_pon_application.c
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
    bcm_port_t pon_port;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
    int pon_local_route;
    int nni_local_route;
    int pon_egress_forward_group;    
};

bcm_port_t pon_port_1 = 4;
bcm_port_t pon_port_2 = 5;
bcm_port_t nni_port = 129;
pon_service_info_s pon_n_1_service_info[2][7];

bcm_vlan_t n_1_service_vsi[2];   /*VSI for N:1 services*/
int num_of_n_1_services[2];


/*
 *Initialize the service models and set up PON application configurations.
 */
int pon_app_init(int unit, bcm_port_t port_pon_1, bcm_port_t port_pon_2, bcm_port_t port_nni)
{
    int rv = 0; 
    int service;
    int index;
    
    local_route_flag = 1;
    /* Disable to use different MC for update and downstream */
    lif_offset = 0;
    
    pon_port_1 = port_pon_1;
    pon_port_2 = port_pon_2;
    nni_port = port_nni;
    
    /* N:1 Service without the local route */
    service = 0;
    /* PON Port 4 Tunnel-ID 1000 CVLAN 60 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 600 CVLAN 601 */
    index = 0;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1000;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 60;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 600;
    pon_n_1_service_info[service][index].down_ivlan = 601;
    pon_n_1_service_info[service][index].pon_port = pon_port_1;
    pon_n_1_service_info[service][index].pon_local_route = 0;
    pon_n_1_service_info[service][index].nni_local_route = 0;
    pon_n_1_service_info[service][index].pon_egress_forward_group = 1;
    
    /* PON Port 4 Tunnel-ID 1000 CVLAN 61 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 600 CVLAN 601 */
    index++;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1000;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 61;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 600;
    pon_n_1_service_info[service][index].down_ivlan = 601;
    pon_n_1_service_info[service][index].pon_port = pon_port_1;
    pon_n_1_service_info[service][index].pon_local_route = 0;
    pon_n_1_service_info[service][index].nni_local_route = 0;
    pon_n_1_service_info[service][index].pon_egress_forward_group = 1;

    /* PON Port 5 Tunnel-ID 1001 CVLAN 62 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 600 CVLAN 601 */
    index++;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1001;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 62;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 600;
    pon_n_1_service_info[service][index].down_ivlan = 601;
    pon_n_1_service_info[service][index].pon_port = pon_port_2;
    pon_n_1_service_info[service][index].pon_local_route = 0;
    pon_n_1_service_info[service][index].nni_local_route = 0;
    pon_n_1_service_info[service][index].pon_egress_forward_group = 1;

    /* PON Port 5 Tunnel-ID 1001 CVLAN 63 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 600 CVLAN 601 */
    index++;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1001;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 63;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 600;
    pon_n_1_service_info[service][index].down_ivlan = 601;
    pon_n_1_service_info[service][index].pon_port = pon_port_2;
    pon_n_1_service_info[service][index].pon_local_route = 0;
    pon_n_1_service_info[service][index].nni_local_route = 0;
    pon_n_1_service_info[service][index].pon_egress_forward_group = 1;

    /*count number of n_1 services*/
    num_of_n_1_services[service] = index + 1;
    
    /* N:1 Service with the local route */
    service = 1;
    /* PON Port 4 Tunnel-ID 1000 CVLAN 80 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801 */
    index = 0;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1000;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 80;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 800;
    pon_n_1_service_info[service][index].down_ivlan = 801;
    pon_n_1_service_info[service][index].pon_port = pon_port_1;
    pon_n_1_service_info[service][index].pon_local_route = 1;
    pon_n_1_service_info[service][index].nni_local_route = 1;
    pon_n_1_service_info[service][index].pon_egress_forward_group = 1;
    
    /* PON Port 4 Tunnel-ID 1000 CVLAN 81 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801 */
    index++;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1000;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 81;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 800;
    pon_n_1_service_info[service][index].down_ivlan = 801;
    pon_n_1_service_info[service][index].pon_port = pon_port_1;
    pon_n_1_service_info[service][index].pon_local_route = 1;
    pon_n_1_service_info[service][index].nni_local_route = 1;
    pon_n_1_service_info[service][index].pon_egress_forward_group = 0;

    /* PON Port 5 Tunnel-ID 1001 CVLAN 82 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801 */
    index++;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1001;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 82;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 800;
    pon_n_1_service_info[service][index].down_ivlan = 801;
    pon_n_1_service_info[service][index].pon_port = pon_port_2;
    pon_n_1_service_info[service][index].pon_local_route = 1;
    pon_n_1_service_info[service][index].nni_local_route = 1;
    pon_n_1_service_info[service][index].pon_egress_forward_group = 1;

    /* PON Port 5 Tunnel-ID 1001 CVLAN 83 <-> VSI 4096 <-> 1 NNI Port 129 SVLAN 800 CVLAN 801 */
    index++;
    pon_n_1_service_info[service][index].service_mode = otag_to_o_i2_tag;
    pon_n_1_service_info[service][index].tunnel_id = 1001;
    pon_n_1_service_info[service][index].up_lif_type = match_otag;
    pon_n_1_service_info[service][index].up_ovlan = 83;
    pon_n_1_service_info[service][index].down_lif_type = match_o_i_tag;
    pon_n_1_service_info[service][index].down_ovlan = 800;
    pon_n_1_service_info[service][index].down_ivlan = 801;
    pon_n_1_service_info[service][index].pon_port = pon_port_2;
    pon_n_1_service_info[service][index].pon_local_route = 1;
    pon_n_1_service_info[service][index].nni_local_route = 1;
    pon_n_1_service_info[service][index].pon_egress_forward_group = 0;

    /*count number of n_1 services*/
    num_of_n_1_services[service] = index + 1;

    bcm_port_class_set(unit, pon_port_1, bcmPortClassId, pon_port_1);
    bcm_port_class_set(unit, pon_port_2, bcmPortClassId, pon_port_2);
    bcm_port_class_set(unit, nni_port, bcmPortClassId, nni_port);

    /* set ports to identify double tags packets and treat packets with ctags to stag in Arad */
    port_tpid_init(pon_port_1, 1, 1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        print rv;
        return rv;
    }

    /* set inner tpids of PON port to recognize single ctag frames */
    for(index = 0; index < port_tpid_info1.nof_inners; ++index){
        rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[index], 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[index]);
            return rv;
        }
    }
    
    /* set ports to identify double tags packets and treat packets with ctags to stag in Arad */
    port_tpid_init(pon_port_2, 1, 1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        print rv;
        return rv;
    }

    /* set inner tpids of PON port to recognize single ctag frames */
    for(index = 0; index < port_tpid_info1.nof_inners; ++index){
        rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[index], 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[index]);
            return rv;
        }
    }

    port_tpid_init(nni_port,1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        print rv;
        return rv;
    }

    /* set inner tpids of NNI port to recognize single ctag frames */
    for(index = 0; index < port_tpid_info1.nof_inners; ++index){
        rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[index], 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[index]);
            return rv;
        }
    }

    /* Remove Ports from VLAN 1 (Done by init application) */
    bcm_vlan_gport_delete_all(unit, 1);

    /* Disable membership in PON ports */
    rv = bcm_port_vlan_member_set(unit, pon_port_1, 0x0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_member_set\n");
        print rv;
        return rv;
    }

    /* Enable additional port tunnel lookup in PON ports */
    rv = bcm_vlan_control_port_set(unit, pon_port_1, bcmVlanPortLookupTunnelEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_control_port_set %s\n", pon_port, bcm_errmsg(rv));
		print rv;
        return rv;
    }
    
    /* Disable membership in PON ports */
    rv = bcm_port_vlan_member_set(unit, pon_port_2, 0x0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_member_set\n");
        print rv;
        return rv;
    }

    /* Enable additional port tunnel lookup in PON ports */
    rv = bcm_vlan_control_port_set(unit, pon_port_2, bcmVlanPortLookupTunnelEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_control_port_set %s\n", pon_port, bcm_errmsg(rv));
		print rv;
        return rv;
    }

    return 0;
}

/*
 * Set up N:1 sercies, create VSI and add PON and NNI LIFs to it.
 */
int pon_n_1_service(int unit)
{
    int rv;
    int service;
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
    bcm_port_t port_pon;    
    int pon_local_route;
    int nni_local_route;
    int pon_egress_forward_group;
    
    for (service = 0; service < 2; service++) 
    {
        vswitch_create(unit, &n_1_service_vsi[service]);
		    for (index = 0; index < num_of_n_1_services[service]; index++)
		    {
                pon_gport = 0;
                nni_gport = 0;
		        service_mode           = pon_n_1_service_info[service][index].service_mode;
		        tunnel_id              = pon_n_1_service_info[service][index].tunnel_id;
		        pon_lif_type           = pon_n_1_service_info[service][index].up_lif_type;
		        up_ovlan               = pon_n_1_service_info[service][index].up_ovlan;
		        up_ivlan               = pon_n_1_service_info[service][index].up_ivlan;
		        up_pcp                 = pon_n_1_service_info[service][index].up_pcp;
		        up_ethertype           = pon_n_1_service_info[service][index].up_ethertype;
		        nni_lif_type           = pon_n_1_service_info[service][index].down_lif_type;
		        down_ovlan             = pon_n_1_service_info[service][index].down_ovlan;
		        down_ivlan             = pon_n_1_service_info[service][index].down_ivlan;
		        port_pon               = pon_n_1_service_info[service][index].pon_port;
		        pon_local_route        = pon_n_1_service_info[service][index].pon_local_route;
		        nni_local_route        = pon_n_1_service_info[service][index].nni_local_route;
		        pon_egress_forward_group = pon_n_1_service_info[service][index].pon_egress_forward_group;
		
		        /* Create PON LIF */
		        pon_lif_create(unit, port_pon, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport);
		
		        /* Add PON LIF to vswitch */
		        rv = bcm_vswitch_port_add(unit, n_1_service_vsi[service], pon_gport);
		        if (rv != BCM_E_NONE) {
		            printf("Error, bcm_vswitch_port_add\n");
		            return rv;
		        }
		
		        /*For downstream*/
		        multicast_vlan_port_add(unit, n_1_service_vsi[service], port_pon, pon_gport);
		
		        /* Set PON LIF ingress VLAN editor */
		        pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL);
		
		        /* Set PON LIF egress VLAN editor */
		        pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);
		        
		        /* Set the PON local route switching */
		        bcm_port_control_set(unit, pon_gport, bcmPortControlLocalSwitching, pon_local_route);
		        
		        /* Set the PON egress as forward group 0 */
		        if (pon_egress_forward_group == 0) {
                    if (is_device_or_above(unit, JERICHO)) {
                        set_port_as_hub(unit,1,pon_gport,0);
                    }
                    else{
                         bcm_port_class_set(unit, pon_gport, bcmPortClassForwardEgress, 0);
                    }
		        }
		
		        /*Only create one NNI LIF*/
		        if (index == 0)
		        {            
		            /* Create NNI LIF */
		            nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);
		
		            /*For upstream*/
		            multicast_vlan_port_add(unit, n_1_service_vsi[service], nni_port, nni_gport);		 
		            
		            /* Add NNI LIF to vswitch */
		            rv = bcm_vswitch_port_add(unit, n_1_service_vsi[service], nni_gport);
		            if (rv != BCM_E_NONE) {
		                printf("Error, bcm_vswitch_port_add\n");
		                return rv;
		            }  		            
		            
		            /* Set the NNI local route switching */
		            bcm_port_control_set(unit, nni_gport, bcmPortControlLocalSwitching, nni_local_route);
		        }
		        else
		        {
		            nni_gport = pon_n_1_service_info[service][0].nni_gport;
		        }
		
		        pon_n_1_service_info[service][index].pon_gport = pon_gport;
		        pon_n_1_service_info[service][index].nni_gport = nni_gport;
		    }
		}

    return rv;
}


/*
 * verify the configurations of N:1 sercies.
 */
int pon_n_1_service_verify(int unit)
{
    int rv;
    int service;
    int index;
    bcm_gport_t pon_gport, nni_gport;
    int pon_local_route;
    int nni_local_route;
    int pon_ingress_forward_group;
    int pon_egress_forward_group;
    int nni_ingress_forward_group;
    int nni_egress_forward_group;
    
    for (service = 0; service < 2; service++) 
    {
		    for (index = 0; index < num_of_n_1_services[service]; index++)
		    {
		        pon_gport = pon_n_1_service_info[service][index].pon_gport;
		        nni_gport = pon_n_1_service_info[service][index].nni_gport;		        
            
            /* Get the PON local route switching */
		        rv = bcm_port_control_get(unit, pon_gport, bcmPortControlLocalSwitching, &pon_local_route);
		        if (rv != BCM_E_NONE) {
		            printf("Error, bcm_port_control_get in PON\n");
		            return rv;
		        }		        
		        if (pon_local_route != pon_n_1_service_info[service][index].pon_local_route) {
		        	  printf("Error, pon_local_route get failure[%d][%d]\n", service, index);
		        	  return BCM_E_FAIL;		            
		        }
		        
		        /* Get the PON ingress forward group */
		        rv = bcm_port_class_get(unit, pon_gport, bcmPortClassForwardIngress, &pon_ingress_forward_group);
		        if (rv != BCM_E_NONE) {
		            printf("Error, bcm_port_class_get in PON ingress\n");
		            return rv;
		        }		        
		        /* The PON ingress forward group is always set as 1 */
		        if (pon_ingress_forward_group != 1) {
		        	  printf("Error, pon_ingress_forward_group get failure[%d][%d]\n", service, index);
		        	  return BCM_E_FAIL;		            
		        }
		        
		        /* Get the PON egress forward group */
		        rv = bcm_port_class_get(unit, pon_gport, bcmPortClassForwardEgress, &pon_egress_forward_group);
		        if (rv != BCM_E_NONE) {
		            printf("Error, bcm_port_class_get in PON egress\n");
		            return rv;
		        }		        
		        if (pon_egress_forward_group != pon_n_1_service_info[service][index].pon_egress_forward_group) {
		        	  printf("Error, pon_egress_forward_group get failure[%d][%d]\n", service, index);
		        	  return BCM_E_FAIL;		            
		        }
		
		        /*Only one NNI LIF*/
		        if (index == 0)
		        {
		            /* Get the NNI local route switching */
		            rv = bcm_port_control_get(unit, nni_gport, bcmPortControlLocalSwitching, &nni_local_route);
		            if (rv != BCM_E_NONE) {
		                printf("Error, bcm_port_control_get in NNI\n");
		                return rv;
		            }		        
		            if (nni_local_route != pon_n_1_service_info[service][index].nni_local_route) {
		        	      printf("Error, nni_local_route get failure[%d][%d]\n", service, index);
		        	      return BCM_E_FAIL;		            
		            }
		        
		            /* Get the NNI ingress forward group */
		            rv = bcm_port_class_get(unit, nni_gport, bcmPortClassForwardIngress, &nni_ingress_forward_group);
		            if (rv != BCM_E_NONE) {
		                printf("Error, bcm_port_class_get in NNI ingress\n");
		                return rv;
		            }		        
		            /* The NNI ingress forward group is always set as 0 */
		            if (nni_ingress_forward_group != 0) {
		        	      printf("Error, nni_ingress_forward_group get failure[%d][%d]\n", service, index);
		        	      return BCM_E_FAIL;		            
		            }
		        
		            /* Get the NNI egress forward group */
		            rv = bcm_port_class_get(unit, nni_gport, bcmPortClassForwardEgress, &nni_egress_forward_group);
		            if (rv != BCM_E_NONE) {
		                printf("Error, bcm_port_class_get in nni egress\n");
		                return rv;
		            }
		            /* The NNI egress forward group is always set as 0 */
		            if (nni_egress_forward_group != 0) {
		        	      printf("Error, nni_egress_forward group get failure[%d][%d]\n", service, index);
		        	      return BCM_E_FAIL;		            
		            }
		        }
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
    int service;
    int index;
    bcm_gport_t pon_gport, nni_gport;
    int pon_egress_forward_group;
    
    local_route_flag = 0;
    for (service = 0; service < 2; service++) 
    {
		    for (index = 0; index < num_of_n_1_services[service]; index++)
		    {
		        pon_gport = pon_n_1_service_info[service][index].pon_gport;
		        nni_gport = pon_n_1_service_info[service][index].nni_gport;
		        pon_egress_forward_group = pon_n_1_service_info[service][index].pon_egress_forward_group;
            
            /* Reset the PON local route switching */
		        bcm_port_control_set(unit, pon_gport, bcmPortControlLocalSwitching, 0);
		        
		        /* Reset the PON egress as forward group 1 */
		        if (pon_egress_forward_group == 0) {
                   if (is_device_or_above(unit, JERICHO)) {
                        set_port_as_hub(unit,1,pon_gport,1);
                   }
                   else{
                        bcm_port_class_set(unit, pon_gport, bcmPortClassForwardEgress, 1);
                   }
		        }
		            
		        rv = vswitch_delete_port(unit, n_1_service_vsi[service], pon_gport);
		        if (rv != BCM_E_NONE) {
		            printf("Error, vswitch_metro_delete_port\n");
		            return rv;
		        }
		
		        /*Only one NNI LIF*/
		        if (index == 0)
		        {
		            /* Reset the NNI local route switching */
		            bcm_port_control_set(unit, nni_gport, bcmPortControlLocalSwitching, 0);
		        
		            rv = vswitch_delete_port(unit, n_1_service_vsi[service], nni_gport);
		            if (rv != BCM_E_NONE) {
		                printf("Error, vswitch_metro_delete_port\n");
		                return rv;
		            }
		        }
		    }
		
		    rv = vswitch_delete(unit, n_1_service_vsi[service]);
		    if (rv != BCM_E_NONE) {
		        printf("Error, vswitch_delete \n");
		        return rv;
		    }
	  }
    
    return rv;        
}


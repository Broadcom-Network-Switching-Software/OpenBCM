/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_pon_vlan_range.c
* Purpose: An example of how to match incoming VLAN using VLAN range of PON application.
*          The following CINT provides a calling sequence example to set VLAN range of port to match incoming VLAN and bring up one main PON services 1:1 Model.
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
*  1. Add VLAN range info to PON port
*        - Call bcm_vlan_translate_action_range_add() with action bcmVlanActionCompressed.        
*  2. Create PON LIF
*        - Call bcm_vlan_port_create() with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN
*  3. Set PON LIF ingress VLAN editor.
*        - Call bcm_vlan_translate_action_create()
*  4. Set PON LIF egress VLAN editor.
*        - Call bcm_vlan_translate_egress_action_add()
*  5. Add VLAN range info to NNI port
*        - Call bcm_vlan_translate_action_range_add() with action bcmVlanActionCompressed    
*  6. Create NNI LIF
*        - Call bcm_vlan_port_create()with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
*  7. Cross connect the 2 LIFs
*        - Call bcm_vswitch_cross_connect_add()
* Clean up sequence:
*  1. Delete the cross connected LIFs.
*        - Call bcm_vswitch_cross_connect_delete()
*  2. Delete VLAN range info from PON port
*        - Call bcm_vlan_translate_action_range_delete()       
*  3. Delete PON LIFs.
*        - Call bcm_vlan_port_destroy()
*  3. Delete VLAN range info from NNI port
*        - Call bcm_vlan_translate_action_range_delete()       
*  4. Delete NNI LIFs.
*        - Call bcm_vlan_port_destroy()
*
* Service Model:
* 1:1 Service:
*     N(1) PON Port 5  <-----------------------------  CrossConnect  ------------> 1 NNI Port 129
*     Tunnel-ID 1000 SVLAN range 10~20  <----------------------------------------> SVLAN range 30~40
*     Tunnel-ID 1000 SVLAN range 50~60 CVLAN range 70~80 <-----------------------> SVLAN range 90~100 CVLAN range 110~120
*
* Traffic within VLAN range:
*  1. PON Port 5 Tunnel-ID 1000 SVLAN range 10~20 <-CrossConnect-> NNI Port 129 SVLAN range 30~40
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 10
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 30
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 15
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 35
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 20
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 40
*
*  2. PON Port 5 Tunnel-ID 1000 SVLAN range 50~60 CVLAN range 70~80 <-CrossConnect-> NNI Port 129 SVLAN range 90~100 CVLAN range 110~120
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 50, VLAN tag type 0x9100, VID = 70
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 90, VLAN tag type 0x9100, VID = 110
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 55, VLAN tag type 0x9100, VID = 75
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 95, VLAN tag type 0x9100, VID = 115
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 60, VLAN tag type 0x9100, VID = 80
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100,VLAN tag type 0x9100, VID = 120
*
* Traffic out of VLAN range, can't be matched. All are dropped:
*  1. PON Port 5 Tunnel-ID 1000 SVLAN range 10~20 <-CrossConnect-> NNI Port 129 SVLAN range 30~40
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 5
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 25
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 25
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 45
*
*  2. PON Port 5 Tunnel-ID 1000 SVLAN range 50~60 CVLAN range 70~80 <-CrossConnect-> NNI Port 129 SVLAN range 90~100 CVLAN range 110~120
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 45, VLAN tag type 0x9100, VID = 70
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 85, VLAN tag type 0x9100, VID = 110
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 50, VLAN tag type 0x9100, VID = 65
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 90, VLAN tag type 0x9100, VID = 105
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 45, VLAN tag type 0x9100, VID = 65
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 85, VLAN tag type 0x9100, VID = 105
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_tpid.c
*      BCM> cint examples/dpp/cint_pon_utils.c
*      BCM> cint examples/dpp/cint_pon_vlan_range.c
*      BCM> cint
*      cint> pon_vlan_range_init(unit, pon_port, nni_port);
*      cint> pon_vlan_range_1_1_service(unit);
*      cint> pon_vlan_range_1_1_service_cleanup(unit);
*
* To Test VLAN Range Settings Only Run:
*      cint> pon_vlan_range_action_add(unit, is_pon, index);
*      cint> pon_vlan_range_action_get(unit, is_pon, index);
*      cint> pon_vlan_range_action_delete(unit, is_pon, index);
*      cint> pon_vlan_range_action_delete_all(unit);
*/

struct vlan_range_service_info_s{
    int service_mode;
    int up_lif_type;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_svlan_low;
    bcm_vlan_t up_svlan_high;
    bcm_vlan_t up_cvlan_low;
    bcm_vlan_t up_cvlan_high;
    int down_lif_type;
    bcm_vlan_t down_svlan_low;
    bcm_vlan_t down_svlan_high;
    bcm_vlan_t down_cvlan_low;
    bcm_vlan_t down_cvlan_high;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
};

vlan_range_service_info_s vlan_range_service_info[2];

uint32 num_of_vlan_range_service;

bcm_port_t pon_port = 5;
bcm_port_t nni_port = 129;

/*
 *Initialize the service models and set up PON application configurations.
 */
int pon_vlan_range_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index;

    pon_port = port_pon;
    nni_port = port_nni;

    /* PON Port 5 Tunnel-ID 1000 SVLAN range 10~20 <-CrossConnect-> NNI Port 129 SVLAN range 30~40 */
    index = 0;
    vlan_range_service_info[index].service_mode = otag_to_otag2;
    vlan_range_service_info[index].tunnel_id  = 1000;

    vlan_range_service_info[index].up_lif_type = match_otag;
    
    vlan_range_service_info[index].up_svlan_low = 10;
    vlan_range_service_info[index].up_svlan_high = 20;
    vlan_range_service_info[index].up_cvlan_low = BCM_VLAN_INVALID;
    vlan_range_service_info[index].up_cvlan_high = BCM_VLAN_INVALID;

    vlan_range_service_info[index].down_lif_type = match_otag;
    vlan_range_service_info[index].down_svlan_low = 30;
    vlan_range_service_info[index].down_svlan_high = 40;
    vlan_range_service_info[index].down_cvlan_low = BCM_VLAN_INVALID;
    vlan_range_service_info[index].down_cvlan_high = BCM_VLAN_INVALID;

    /* PON Port 5 Tunnel-ID 1000 SVLAN range 50~60 CVLAN range 70~80 <-CrossConnect-> NNI Port 129 SVLAN range 90~100 CVLAN range 110~120 */
    index++;
    vlan_range_service_info[index].service_mode = o_i_tag_to_o2_i_tag;
    vlan_range_service_info[index].tunnel_id  = 1000;

    vlan_range_service_info[index].up_lif_type = match_o_i_tag;
    vlan_range_service_info[index].up_svlan_low = 50;
    vlan_range_service_info[index].up_svlan_high = 60;
    vlan_range_service_info[index].up_cvlan_low = 70;
    vlan_range_service_info[index].up_cvlan_high = 80;

    vlan_range_service_info[index].down_lif_type = match_o_i_tag;
    vlan_range_service_info[index].down_svlan_low = 90;
    vlan_range_service_info[index].down_svlan_high = 100;
    vlan_range_service_info[index].down_cvlan_low = 110;
    vlan_range_service_info[index].down_cvlan_high = 120;

    num_of_vlan_range_service = index+1;

    pon_app_init(unit, pon_port, nni_port, 0, 0);

    return 0;
}

/*
 * Set up 1:1 sercies, using port cross connect. Match VLAN using VLAN range.
 */
int pon_vlan_range_1_1_service(int unit)
{
    int rv;
    int index, is_with_id = 0;
    bcm_vswitch_cross_connect_t gports;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_svlan_low;
    bcm_vlan_t up_svlan_high;
    bcm_vlan_t up_cvlan_low;
    bcm_vlan_t up_cvlan_high;
    bcm_vlan_t down_svlan_low;
    bcm_vlan_t down_svlan_high;
    bcm_vlan_t down_cvlan_low;
    bcm_vlan_t down_cvlan_high;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;
    uint8 up_pcp=0;
    bcm_ethertype_t up_ethertype=0;

    for (index = 0; index < num_of_vlan_range_service; index++)
    {
        pon_gport = 0;
        nni_gport = 0;
        service_mode = vlan_range_service_info[index].service_mode;
        tunnel_id    = vlan_range_service_info[index].tunnel_id;
        
        pon_lif_type = vlan_range_service_info[index].up_lif_type;
        up_svlan_low = vlan_range_service_info[index].up_svlan_low;
        up_svlan_high= vlan_range_service_info[index].up_svlan_high;
        up_cvlan_low = vlan_range_service_info[index].up_cvlan_low;
        up_cvlan_high= vlan_range_service_info[index].up_cvlan_high;

        if (up_cvlan_low == BCM_VLAN_INVALID)
        {
            up_cvlan_low = 0;
        }

        if (up_cvlan_high == BCM_VLAN_INVALID)
        {
            up_cvlan_high = 0;
        }

        nni_lif_type   = vlan_range_service_info[index].down_lif_type;
        down_svlan_low = vlan_range_service_info[index].down_svlan_low;
        down_svlan_high= vlan_range_service_info[index].down_svlan_high;
        down_cvlan_low = vlan_range_service_info[index].down_cvlan_low;
        down_cvlan_high= vlan_range_service_info[index].down_cvlan_high;

        if (down_cvlan_low == BCM_VLAN_INVALID)
        {
            down_cvlan_low = 0;
        }

        if (down_cvlan_low == BCM_VLAN_INVALID)
        {
            down_cvlan_low = 0;
        }

        /* Add PON LIF VLAN range action */
        pon_vlan_range_action_add(unit, 1, index);

        /* Create PON LIF */
        pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_svlan_low, up_cvlan_low, up_pcp, up_ethertype, &pon_gport);

        /* Set PON LIF ingress VLAN editor */
        pon_port_ingress_vt_set(unit, service_mode, down_svlan_low, down_cvlan_low, pon_gport, 0, NULL);
        
        /* Set PON LIF egress VLAN editor */
        pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_svlan_low, up_cvlan_low, pon_gport);

        /* Add NNI LIF VLAN range action */
        pon_vlan_range_action_add(unit, 0, index);

        /* Create NNI LIF */
        nni_lif_create(unit, nni_port, nni_lif_type, is_with_id, down_svlan_low, down_cvlan_low, 0, &nni_gport, &encap_id);

        vlan_range_service_info[index].pon_gport = pon_gport;
        vlan_range_service_info[index].nni_gport = nni_gport;

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
 * clean up the configurations of 1:1 sercies. Clear VLAN range info.
 */
int pon_vlan_range_1_1_service_cleanup(int unit)
{
    int rv;
    int index;
    bcm_vswitch_cross_connect_t gports;

    for (index = 0; index < num_of_vlan_range_service; index++)
    {
        /* Delete the cross connected LIFs */
        gports.port1 = vlan_range_service_info[index].pon_gport;
        gports.port2 = vlan_range_service_info[index].nni_gport;
        rv = bcm_vswitch_cross_connect_delete(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_delete\n");
            print rv;
            return rv;
        }

        /* Delete PON LIF VLAN range action */
        pon_vlan_range_action_delete(unit, 1, index);

        /* Delete PON LIF */
        rv = bcm_vlan_port_destroy(unit, vlan_range_service_info[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }

        /* Delete NNI LIF VLAN range action */
        pon_vlan_range_action_delete(unit, 0, index);

        /* Delete NNI LIF */
        rv = bcm_vlan_port_destroy(unit, vlan_range_service_info[index].nni_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }
    }
      
    return rv;        
}

/*
 * Add VLAN range info to PON port or NNI port.
 */
int pon_vlan_range_action_add(int unit, int is_pon, int index)
{
    int rv;
    bcm_port_t port;
    int service_mode;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;
    bcm_vlan_action_set_t action;

    service_mode = vlan_range_service_info[index].service_mode;

    if (is_pon)
    {
        port = pon_port;
        outer_vlan_low = vlan_range_service_info[index].up_svlan_low;
        outer_vlan_high= vlan_range_service_info[index].up_svlan_high;
        inner_vlan_low = vlan_range_service_info[index].up_cvlan_low;
        inner_vlan_high= vlan_range_service_info[index].up_cvlan_high;
    }
    else
    {    
        port = nni_port;
        outer_vlan_low = vlan_range_service_info[index].down_svlan_low;
        outer_vlan_high= vlan_range_service_info[index].down_svlan_high;
        inner_vlan_low = vlan_range_service_info[index].down_cvlan_low;
        inner_vlan_high= vlan_range_service_info[index].down_cvlan_high;
    }

    bcm_vlan_action_set_t_init(&action);
    if (service_mode == otag_to_otag2)
    {
        action.ot_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
    }
    else if (service_mode == o_i_tag_to_o2_i_tag)
    {
        action.dt_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
        action.dt_inner = bcmVlanActionCompressed;
        action.new_inner_vlan = inner_vlan_low;
    
    }
    rv = bcm_vlan_translate_action_range_add(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, &action);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_add!\n");
        print rv;
        return rv;
    }


    return rv;
}

/*
 * Get VLAN range info from PON port or NNI port.
 */
int pon_vlan_range_action_get(int unit, int is_pon, int index)
{
    int rv;
    bcm_port_t port;
    int service_mode;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;
    bcm_vlan_action_set_t action;
    char lif_desc[10];

    service_mode = vlan_range_service_info[index].service_mode;
    
    if (is_pon)
    {
        port = pon_port;
        outer_vlan_low = vlan_range_service_info[index].up_svlan_low;
        outer_vlan_high= vlan_range_service_info[index].up_svlan_high;
        inner_vlan_low = vlan_range_service_info[index].up_cvlan_low;
        inner_vlan_high= vlan_range_service_info[index].up_cvlan_high;
        sprintf(lif_desc, "PON LIF %d", index);
    }
    else
    {    
        port = nni_port;
        outer_vlan_low = vlan_range_service_info[index].down_svlan_low;
        outer_vlan_high= vlan_range_service_info[index].down_svlan_high;
        inner_vlan_low = vlan_range_service_info[index].down_cvlan_low;
        inner_vlan_high= vlan_range_service_info[index].down_cvlan_high;
        sprintf(lif_desc, "NNI LIF %d", index);
    }

    bcm_vlan_action_set_t_init(&action);
    rv = bcm_vlan_translate_action_range_get(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, &action);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_get!\n");
        print rv;
        return rv;
    }

    if ((service_mode == otag_to_otag2) && 
        ((action.ot_outer != bcmVlanActionCompressed) || 
        (action.new_outer_vlan != outer_vlan_low)))
    {
        printf("Err, %s VLAN range action not correct!\n", lif_desc);        
        print action;
        return BCM_E_FAIL;
    }
    else if ((service_mode == o_i_tag_to_o2_i_tag) && 
             ((action.dt_outer != bcmVlanActionCompressed) || 
             (action.new_outer_vlan != outer_vlan_low) || 
             (action.dt_inner != bcmVlanActionCompressed) || 
             (action.new_inner_vlan != inner_vlan_low)))
    {        
        printf("Err, %s VLAN range action not correct!\n", lif_desc);        
        print action;
        return BCM_E_FAIL;
    }
    printf("%s: \n", lif_desc);
    print action;

    return;
}

/*
 * Delete VLAN range info from PON port or NNI port.
 */
int pon_vlan_range_action_delete(int unit, int is_pon, int index)
{
    int rv;
    bcm_port_t port;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;

    if (is_pon)
    {
        port = pon_port;
        outer_vlan_low = vlan_range_service_info[index].up_svlan_low;
        outer_vlan_high= vlan_range_service_info[index].up_svlan_high;
        inner_vlan_low = vlan_range_service_info[index].up_cvlan_low;
        inner_vlan_high= vlan_range_service_info[index].up_cvlan_high;
    }
    else
    {    
        port = nni_port;
        outer_vlan_low = vlan_range_service_info[index].down_svlan_low;
        outer_vlan_high= vlan_range_service_info[index].down_svlan_high;
        inner_vlan_low = vlan_range_service_info[index].down_cvlan_low;
        inner_vlan_high= vlan_range_service_info[index].down_cvlan_high;
    }

    rv = bcm_vlan_translate_action_range_delete(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_delete!\n");
        print rv;
        return rv;
    }

    return rv;
}

/*
 * Delete all VLAN range info from PON port or NNI port.
 */
int pon_vlan_range_action_delete_all(int unit)
{
    int rv;

    rv = bcm_vlan_translate_action_range_delete_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_add!\n");
        print rv;
        return rv;
    }

    return rv;
}


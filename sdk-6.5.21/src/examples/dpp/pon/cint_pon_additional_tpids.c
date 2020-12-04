/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_pon_additional_tpid.c
* Purpose: An example of how to use additional TPID for NNI port of PON application.
*          The following CINT provides a calling sequence example to set the different tunnel_id to be mapped to different LLID profile and bring up one main PON services 1:1 Model.
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
*  1. PON regular initial routine 
*        - call pon_app_init()
*
*  2. Create PON PP port with different PON tunnel profile id 0.
*        - call bcm_port_pon_tunnel_add()
*  3. Map tunnel id 1000/1001 to PON PP port 5 (indirectly mapped to PON tunnel profile id 0).
*        - call bcm_port_pon_tunnel_map_set()
*  4. Set TPIDs (0x8100, 0x9100) to PON pp port 5.
*        - call bcm_port_tpid_delete_all()
*        - call bcm_port_tpid_add()
*        - call bcm_port_inner_tpid_set()
*  5. Set VLAN domain of PON pp port 5 to 5.
*        - call bcm_port_class_set()
*  6. Enable additional port tunnel lookup in PON PP port 5.
*        - call bcm_vlan_control_port_set() with bcmVlanPortLookupTunnelEnable
*
*  7. Create PON PP port with different PON tunnel profile id 1.
*        - call bcm_port_pon_tunnel_add()
*  8. Map tunnel id 1002/1003 to PON PP port 13 (indirectly mapped to PON tunnel profile id 1).
*        - call bcm_port_pon_tunnel_map_set()
*  9. Set TPIDs (0x8100, 0x88a8) to PON pp port 13.
*        - call bcm_port_tpid_delete_all()
*        - call bcm_port_tpid_add()
*        - call bcm_port_inner_tpid_set()
*  10. Set VLAN domain of PON pp port 13 to 13.
*        - call bcm_port_class_set()
*  11. Enable additional port tunnel lookup in PON PP port 13.
*        - call bcm_vlan_control_port_set() with bcmVlanPortLookupTunnelEnable
*
*  12. Add additional TPID to global table.
*        - call bcm_switch_tpid_add()
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
*     N(1) PON Port 5  <--------------------------------------------  CrossConnect  ----------------> 1 NNI Port 129
*     Tunnel-ID 1000 Outer TPID 0x9100 CVLAN 20 PON_PP_PORT 5 <------------------------------------> Outer TPID 0x88a8 CVLAN 200
*     Tunnel-ID 1001 Outer TPID 0x9100 CVLAN 30 PON_PP_PORT 5 <------------------------------------> Outer TPID 0x9100 CVLAN 300
*     Tunnel-ID 1001 Outer TPID 0x8100 SVLAN 40 Inner TPID 0x9100 CVLAN 41 PON_PP_PORT 5 <------------> Outer TPID 0x8100 SVLAN 400 Inner TPID 0x9100 CVLAN 401
*
*     Tunnel-ID 1002 Outer TPID 0x88a8 CVLAN 50 PON_PP_PORT 13 <------------------------------------> Outer TPID 0x88a8 CVLAN 500
*     Tunnel-ID 1002 Outer TPID 0x88a8 CVLAN 60 PON_PP_PORT 13 <------------------------------------> Outer TPID 0x9100 CVLAN 600
*     Tunnel-ID 1003 Outer TPID 0x8100 CVLAN 70 Inner TPID 0x88a8 CVLAN 71 PON_PP_PORT 13 <-----------> Outer TPID 0x8100 SVLAN 700 Inner TPID 0x9100 CVLAN 701
*     Tunnel-ID 1003 Outer TPID 0x8100 CVLAN 80 Inner TPID 0x88a8 CVLAN 81 PON_PP_PORT 13 <-----------> Outer TPID 0x8100 SVLAN 800 Inner TPID 0x88a8 CVLAN 801
*
* Traffic:
*  1. PON Port 5 PON PP Port 5 Tunnel-ID 1000 Outer TPID 0x9100 CVLAN 20 <-CrossConnect-> NNI Port 129 Outer TPID 0x88a8 CVLAN 200
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x9100, VID = 20
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x88a8, VID = 200
*
*  2. PON Port 5 PON PP Port 5 Tunnel-ID 1000 Outer TPID 0x9100 CVLAN 30 <-CrossConnect-> NNI Port 129 Outer TPID 0x9100 CVLAN 300
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x9100, VID = 30
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x9100, VID = 300
*
*  3. PON Port 5 PON PP Port 5 Tunnel-ID 1000 Outer TPID 0x8100 SVLAN 40 Inner TPID 0x9100 CVLAN 41 <-CrossConnect-> NNI Port 129 Outer TPID 0x8100 SVLAN 400 Inner TPID 0x9100 CVLAN 401
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 40, VLAN tag type 0x9100, VID = 41
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 401, VLAN tag type 0x9100, VID = 401
*
*  4. PON Port 5 PON PP Port 13 Tunnel-ID 1000 Outer TPID 0x88a8 CVLAN 50 <-CrossConnect-> NNI Port 129 Outer TPID 0x9100 CVLAN 500
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x88a8, VID = 50
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x88a8, VID = 500
*
*  5. PON Port 5 PON PP Port 13 Tunnel-ID 1000 Outer TPID 0x88a8 CVLAN 60 <-CrossConnect-> NNI Port 129 Outer TPID 0x88a8 CVLAN 600
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x88a8, VID = 60
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x88a8, VID = 600
*
*  6. PON Port 5 PON PP Port 13 Tunnel-ID 1000 Outer TPID 0x8100 SVLAN 70 Inner TPID 0x88a8 CVLAN 71 <-CrossConnect-> NNI Port 129 Outer TPID 0x8100 SVLAN 700 Inner TPID 0x9100 CVLAN 701
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 70, VLAN tag type 0x88a8, VID = 71
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 701, VLAN tag type 0x9100, VID = 701
*
*  7. PON Port 5 PON PP Port 13 Tunnel-ID 1000 Outer TPID 0x8100 SVLAN 80 Inner TPID 0x88a8 CVLAN 81 <-CrossConnect-> NNI Port 129 Outer TPID 0x8100 SVLAN 800 Inner TPID 0x88a8 CVLAN 801
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   Tunnel ID:1000
*              -   VLAN tag: VLAN tag type 0x8100, VID = 80, VLAN tag type 0x88a8, VID = 81
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 801, VLAN tag type 0x88a8, VID = 801
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_tpid.c
*      BCM> cint examples/dpp/pon/cint_pon_utils.c
*      BCM> cint examples/dpp/pon/cint_pon_additional_tpid.c
*      BCM> cint
*      cint> pon_additional_tpid_service_init(unit, pon_port, nni_port);
*      cint> pon_additional_tpid_1_1_service(unit);
*      cint> pon_additional_tpid_1_1_service_cleanup(unit);
*/

outer_tpid = 0x8100;
inner_tpid = 0x9100;
uint16 outer_tpid_u16 = 0x8100;
uint16 inner_tpid_u16 = 0x9100;

uint32 additional_tpid_u32 = 0x88a8;
uint16 additional_tpid = 0x88a8;

struct pon_tunnel_profile_info_s{
    bcm_tunnel_id_t tunnel_id[2]; /* tunnel_ids mapped to tunnel_profile_id, could be more */
    uint16 tunnel_profile_id;
    uint32 outer_tpid;
    uint32 inner_tpid;
    bcm_gport_t pon_pp_port;
};

pon_tunnel_profile_info_s pon_tunnel_profile[3];
int num_of_tunnel_profile;

enum additional_tpid_ing_vlan_action_id_e {
    ing_no_op_0=0,
    ing_no_op_1,
    ing_no_op_2,
    ing_no_op_3,
    ing_mod_tpid_0, /* C <> C */
    ing_mod_tpid_1,
    ing_rep_c_tag_0, /* C <> C' */
    ing_rep_c_tag_1,
    ing_rep_c_tag_2,
    ing_rep_s_c_tag_0,
    ing_rep_s_c_tag_1
};

enum additional_tpid_egr_vlan_action_id_e {
    egr_add_vsi = 0,
    egr_no_op_tag = 1,
    egr_no_op_0, /* C <> C*/
    egr_no_op_1, /* S+C <> S+C*/
    egr_rep_tu_add_c_tag_0,
    egr_rep_tu_add_c_tag_1,
    egr_rep_tu_add_c_tag_2,
    egr_add_tu_rep_s_c_tag_0,   /*S+C<>S'+C'*/
    egr_add_tu_rep_s_c_tag_1
};

struct pon_service_info_s{
    int service_mode;
    int up_lif_type;
    bcm_tunnel_id_t tunnel_id;
    uint16 up_outer_tpid;
    uint16 up_inner_tpid;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    int down_lif_type;
    uint16 down_outer_tpid;
    uint16 down_inner_tpid;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
};

bcm_port_t pon_port = 5;
bcm_port_t nni_port = 129;

pon_service_info_s pon_additional_tpid_services[10];
int nof_1_1_services = 0;

int pon_additional_tpid_profile_init(int unit, bcm_port_t port, uint32 tpid_outer, uint32 tpid_inner)
{
    int rv = 0;
    int index;
    bcm_port_tpid_class_t port_tpid_class;

     /* remove old tpids on port */
    rv = bcm_port_tpid_delete_all(unit,port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_delete_all $rv\n");
        return rv;
    }

    /* identify TPID on port */
    rv = bcm_port_tpid_add(unit, port, tpid_outer,0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_add $rv\n");
        return rv;
    }
    
    /* identify TPID on port */
    rv = bcm_port_inner_tpid_set(unit, port, tpid_inner);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_inner_tpid_set $rv\n");
        return rv;
    }
            
    /* outer: TPID1, inner: ANY ==> S_tag */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = tpid_outer;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = s_tag;
    port_tpid_class.flags = 0;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-tag $rv\n");
        return rv;
    }

    /* outer: TPID2, inner: ANY ==> c_tag */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = tpid_inner;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = c_tag;
    port_tpid_class.flags = 0;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set c-tag $rv\n");
        return rv;
    }

    /* outer: TPID1, inner: TPID2 ==> S_c_tag */
    /* when TPID found as outer, then consider as tag and accepted format */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = tpid_outer;
    port_tpid_class.tpid2 = tpid_inner;
    port_tpid_class.tag_format_class_id = s_c_tag;
    port_tpid_class.flags = 0;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }

    /* outer: TPID2, inner: TPID2 ==> C_c_tag */
    /* when TPID found as outer, then consider as tag and accepted format */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = tpid_inner;
    port_tpid_class.tpid2 = tpid_inner;
    port_tpid_class.tag_format_class_id = c_c_tag;
    port_tpid_class.flags = 0;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set c-c-tag $rv\n");
        return rv;
    }

    return rv;
}

int nni_ing_vlan_action_modify_tpid_init(int unit)
{
    int rv = BCM_E_NONE;
    int ing_vlan_action_id = ing_mod_tpid_0;
    int ing_vlan_edit_class_id = 1;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t_init(&action);

    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &ing_vlan_action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
        return rv;
    }

    action.outer_tpid = additional_tpid;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set ing $rv\n");
        return rv;
    }

    /* Set translation action class for c_tag packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ing_vlan_edit_class_id;
    action_class.tag_format_class_id = c_tag;
    action_class.vlan_translation_action_id = ing_vlan_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
        return rv;
    }
    
    return rv;
}


/* Init PON tunnel profile info */
int pon_additional_tpid_tunnel_profile_init(int unit)
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
    pon_tunnel_profile[index].outer_tpid = outer_tpid;
    pon_tunnel_profile[index].inner_tpid = inner_tpid;


    /* Tunnel Profile 1
     * PON port ignore inner packet tag
     * VLAN Range: outer VLAN[80, 90]
     */
    index++;
    pon_tunnel_profile[index].tunnel_id[0] = 1002;
    pon_tunnel_profile[index].tunnel_id[1] = 1003;
    pon_tunnel_profile[index].tunnel_profile_id = 1;
    pon_tunnel_profile[index].outer_tpid = outer_tpid;
    pon_tunnel_profile[index].inner_tpid = additional_tpid_u32;

    num_of_tunnel_profile = index + 1;

    return rv;
}

/* Set PON tunnel profile to ARAD */
int pon_additional_tpid_tunnel_profile_set(int unit, bcm_port_t port_pon)
{
    int rv = 0;
    int index;
    int j;
    uint32 flags;
    bcm_gport_t tunnel_port_id;
    bcm_vlan_action_set_t action;

    for (index = 0; index < num_of_tunnel_profile; index++) {
        flags = BCM_PORT_PON_TUNNEL_WITH_ID;
        tunnel_port_id= pon_tunnel_profile[index].tunnel_profile_id;
        rv = bcm_port_pon_tunnel_add(unit, pon_port, flags, &tunnel_port_id);
        if (rv != BCM_E_NONE)
        {    
            printf("Error, bcm_port_pon_tunnel_add!\n");
            print rv;
            return rv;
        }

        pon_tunnel_profile[index].pon_pp_port = tunnel_port_id;

        /* Map tunnel_id to PON in pp Port */
        for (j = 0; j < 2; j++) {
            rv = bcm_port_pon_tunnel_map_set(unit, pon_port, pon_tunnel_profile[index].tunnel_id[j], tunnel_port_id);
            if (rv != BCM_E_NONE)
            {    
                printf("Error, bcm_petra_port_pon_tunnel_map_set!\n");
                print rv;
                return rv;
            }
        }

        rv = pon_additional_tpid_profile_init(unit, tunnel_port_id, pon_tunnel_profile[index].outer_tpid, pon_tunnel_profile[index].inner_tpid);
        if (rv != BCM_E_NONE) {
            printf("Error, in pon_tpid_profile_init %s\n", bcm_errmsg(rv));
            print rv;
            return rv;
        }

        /* Set PON PP PORT VLAN Domain */
        bcm_port_class_set(unit, tunnel_port_id, bcmPortClassId, tunnel_port_id);
        
        /* Enable additional port tunnel lookup in PON ports */
        rv = bcm_vlan_control_port_set(unit, tunnel_port_id, bcmVlanPortLookupTunnelEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_port_set %s\n", bcm_errmsg(rv));
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
                *pon_pp_port = pon_tunnel_profile[i].pon_pp_port;
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_NOT_FOUND;
}

int pon_additional_tpid_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    int rv = 0;
    int index;
    bcm_switch_tpid_info_t tpid_info;

    pon_port = port_pon;
    nni_port = port_nni;

    /*1:1 services*/
    index = 0;
    pon_additional_tpid_services[index].service_mode = otag_to_otag2;
    pon_additional_tpid_services[index].tunnel_id  = 1000;
    pon_additional_tpid_services[index].up_lif_type = match_otag;
    pon_additional_tpid_services[index].up_outer_tpid = inner_tpid;
    pon_additional_tpid_services[index].up_ovlan      = 20;
    pon_additional_tpid_services[index].down_lif_type = match_otag;
    pon_additional_tpid_services[index].down_outer_tpid = additional_tpid;
    pon_additional_tpid_services[index].down_ovlan = 200;

    index++;
    pon_additional_tpid_services[index].service_mode = otag_to_otag2;
    pon_additional_tpid_services[index].tunnel_id  = 1001;
    pon_additional_tpid_services[index].up_lif_type = match_otag;
    pon_additional_tpid_services[index].up_outer_tpid = inner_tpid;
    pon_additional_tpid_services[index].up_ovlan      = 30;
    pon_additional_tpid_services[index].down_lif_type = match_otag;
    pon_additional_tpid_services[index].down_outer_tpid = inner_tpid;
    pon_additional_tpid_services[index].down_ovlan = 300;

    index++;
    pon_additional_tpid_services[index].service_mode = o_i_tag_to_o2_i2_tag;
    pon_additional_tpid_services[index].tunnel_id  = 1001;
    pon_additional_tpid_services[index].up_lif_type = match_o_i_tag;
    pon_additional_tpid_services[index].up_outer_tpid = outer_tpid;
    pon_additional_tpid_services[index].up_inner_tpid = inner_tpid;
    pon_additional_tpid_services[index].up_ovlan      = 40;
    pon_additional_tpid_services[index].up_ivlan      = 41;
    pon_additional_tpid_services[index].down_lif_type = match_o_i_tag;
    pon_additional_tpid_services[index].down_outer_tpid = outer_tpid;
    pon_additional_tpid_services[index].down_inner_tpid = inner_tpid;
    pon_additional_tpid_services[index].down_ovlan = 400;
    pon_additional_tpid_services[index].down_ivlan = 401;

    index++;
    pon_additional_tpid_services[index].service_mode = otag_to_otag2;
    pon_additional_tpid_services[index].tunnel_id  = 1002;
    pon_additional_tpid_services[index].up_lif_type = match_otag;
    pon_additional_tpid_services[index].up_outer_tpid = additional_tpid;
    pon_additional_tpid_services[index].up_ovlan      = 50;
    pon_additional_tpid_services[index].down_lif_type = match_otag;
    pon_additional_tpid_services[index].down_outer_tpid = additional_tpid;
    pon_additional_tpid_services[index].down_ovlan = 500;

    index++;
    pon_additional_tpid_services[index].service_mode = otag_to_otag2;
    pon_additional_tpid_services[index].tunnel_id  = 1002;
    pon_additional_tpid_services[index].up_lif_type = match_otag;
    pon_additional_tpid_services[index].up_outer_tpid = additional_tpid;
    pon_additional_tpid_services[index].up_ovlan      = 60;
    pon_additional_tpid_services[index].down_lif_type = match_otag;
    pon_additional_tpid_services[index].down_outer_tpid = inner_tpid;
    pon_additional_tpid_services[index].down_ovlan = 600;

    /* PON Port 5 Tunnel-ID 1001 SVLAN 30 CVLAN 31 <-CrossConnect-> NNI Port 129 SVLAN 300 CVLAN 31*/
    index++;
    pon_additional_tpid_services[index].service_mode = o_i_tag_to_o2_i2_tag;
    pon_additional_tpid_services[index].tunnel_id  = 1003;
    pon_additional_tpid_services[index].up_lif_type = match_o_i_tag;
    pon_additional_tpid_services[index].up_outer_tpid = outer_tpid;
    pon_additional_tpid_services[index].up_inner_tpid = additional_tpid;
    pon_additional_tpid_services[index].up_ovlan      = 70;
    pon_additional_tpid_services[index].up_ivlan      = 71;
    pon_additional_tpid_services[index].down_lif_type = match_o_i_tag;
    pon_additional_tpid_services[index].down_outer_tpid = outer_tpid;
    pon_additional_tpid_services[index].down_inner_tpid = inner_tpid;
    pon_additional_tpid_services[index].down_ovlan = 700;
    pon_additional_tpid_services[index].down_ivlan = 701;

    /* Not supported yet */
    /*index++;
    pon_additional_tpid_services[index].service_mode = o_i_tag_to_o2_i2_tag;
    pon_additional_tpid_services[index].tunnel_id  = 1003;
    pon_additional_tpid_services[index].up_lif_type = match_o_i_tag;
    pon_additional_tpid_services[index].up_outer_tpid = outer_tpid;
    pon_additional_tpid_services[index].up_inner_tpid = additional_tpid;
    pon_additional_tpid_services[index].up_ovlan      = 80;
    pon_additional_tpid_services[index].up_ivlan      = 81;
    pon_additional_tpid_services[index].down_lif_type = match_o_i_tag;
    pon_additional_tpid_services[index].down_outer_tpid = outer_tpid;
    pon_additional_tpid_services[index].down_inner_tpid = additional_tpid;
    pon_additional_tpid_services[index].down_ovlan = 800;
    pon_additional_tpid_services[index].down_ivlan = 801;*/

    /*count number of 1_1 services*/
    nof_1_1_services = index + 1;

    pon_app_init(unit, pon_port, nni_port, 0, 0);

    pon_additional_tpid_tunnel_profile_init(unit);

    pon_additional_tpid_tunnel_profile_set(unit, pon_port);

    nni_ing_vlan_action_modify_tpid_init(unit);

    sal_memset(&tpid_info, 0, sizeof(tpid_info));

    tpid_info.tpid_type = bcmTpidTypeInner;
    tpid_info.tpid_value = additional_tpid;
    rv = bcm_switch_tpid_add(unit, 0, &tpid_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_tpid_add failed! $rv\n");
        return rv;
    }

    return rv;
}

/*
 * Create PON LIF according to the LIF type.
 * PON LIF can be created based on Port or Flow.
 */
int pon_additional_tpid_lif_create(int unit, bcm_port_t port_pon, int is_flow, int queue,
    pon_service_info_s *service_info, bcm_gport_t *gport)
{
    int rv;
    int index;
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);

    if (is_flow) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(vlan_port.port, queue);
    } else {
        vlan_port.port = port_pon;
    }        
    
    /* Set ingress vlan editor of PON LIF to do nothing, when creating PON LIF */
    if (soc_property_get(unit , "custom_feature_l3_source_bind_arp_relay",0)) {
        vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE | BCM_VLAN_PORT_NETWORK | BCM_VLAN_PORT_FORWARD_GROUP);
    } else {
        vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE | BCM_VLAN_PORT_NETWORK);
    }
    vlan_port.match_tunnel_value  = service_info->tunnel_id&0x7FF;
    vlan_port.egress_tunnel_value = service_info->tunnel_id;

    switch(service_info->up_lif_type) {
        case match_otag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN;
            vlan_port.match_vlan = service_info->up_ovlan;
            break;
        case match_o_i_tag:
        case match_t_o_i_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED;
            vlan_port.match_vlan        = service_info->up_ovlan;
            vlan_port.match_inner_vlan  = service_info->up_ivlan;
            /* Set egress_vlan and egress_inner_vlan with non-zero to create 3 tags manipulation vlan port */
            vlan_port.egress_vlan       = service_info->up_ovlan;
            vlan_port.egress_inner_vlan = service_info->up_ivlan;
            break;

        default:
            printf("ERR: pon_lif_create INVALID PARAMETER lif_type %d\n", service_info->up_lif_type);
            return BCM_E_PARAM;
    }

    if(util_verbose) {
        printf("pon_lif created:\n");
        print vlan_port;
    }
    
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("pon_lif_create index failed! %s\n", bcm_errmsg(rv));
    }

    *gport = vlan_port.vlan_port_id;

    if(util_verbose) {
        print vlan_port.vlan_port_id;
    }
  
    return rv;
}


/*
 * Create NNI LIF according to the LIF type.
 * nni LIF can be created on a trunk group by port_nni = trunk_gport
 */
int nni_additional_tpid_lif_create(int unit, bcm_port_t port_nni, int is_with_id, 
    pon_service_info_s *service_info, bcm_gport_t *gport, bcm_if_t *encap_id)
{
    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_action_set_t action;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_t vid;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = port_nni;
    if (is_with_id) /* Create NNI LIF with same LIF ID */
    {
        vlan_port.flags = BCM_VLAN_PORT_ENCAP_WITH_ID | BCM_VLAN_PORT_WITH_ID | 0x800 /* BCM_VLAN_PORT_MATCH_ONLY */;
        vlan_port.encap_id = *encap_id;
        vlan_port.vlan_port_id = *gport;
    } else {
        /* Set ingress vlan editor of NNI LIF to do nothing, when creating NNi LIF */
        vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE);
    }    

    switch(service_info->down_lif_type) {
        case match_all_tags:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
            break;
        case match_otag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.match_vlan  = service_info->down_ovlan;
            break;
        case match_o_i_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port.match_vlan        = service_info->down_ovlan;
            vlan_port.match_inner_vlan  = service_info->down_ivlan;
            break;
        default:
            printf("ERR: nni_lif_create INVALID PARAMETER lif_type %d\n", service_info->down_lif_type);
            return BCM_E_PARAM;
    }

    if(util_verbose) {
        printf("nni_lif created:\n");
        print vlan_port;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("nni_lif_create index failed! %s\n", bcm_errmsg(rv));
    }

    *gport = vlan_port.vlan_port_id;

    if(util_verbose) {
        print vlan_port.vlan_port_id;
    }

        /* In case of port tpid_profile != 0, need to init a VLAN action 
         * to modify the tpid_profile of IVE to the correct vlaue 
         */
    if (service_info->down_outer_tpid == additional_tpid) {
        bcm_vlan_port_translation_t_init(&port_trans);
        port_trans.gport = vlan_port.vlan_port_id;
        port_trans.vlan_edit_class_id = 1;
        port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set egr $rv\n");
            return rv;
        }
    }

    /* Set egress VLAN action of NNI LIF to do nothing */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.gport = vlan_port.vlan_port_id;
    port_trans.vlan_edit_class_id = 1;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set egr $rv\n");
        return rv;
    }

    return rv;
}

int pon_additional_tpid_create_addition_tpid_profile(int unit, uint16 tpid_outer, uint16 tpid_inner)
{
    int rv;
    uint32 flags = 0;
    int ing_vlan_action_id;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t action_2;
    bcm_vlan_action_set_t_init(&action);

    flags = BCM_VLAN_ACTION_SET_TPID_ALLOW_CREATE;

    action.outer_tpid = tpid_outer;
    action.inner_tpid = tpid_inner;

    ing_vlan_action_id = ing_mod_tpid_1;

    rv = bcm_vlan_translate_action_id_get(unit, BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action_2);
    if (rv == BCM_E_NOT_FOUND) {
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &ing_vlan_action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
            return rv;
        }
    }

    rv = bcm_vlan_translate_action_id_set(unit, flags|BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set ing $rv\n");
        return rv;
    }
    
    return rv;
}

int pon_additional_tpid_destroy_addition_tpid_profile(int unit, uint16 tpid_outer, uint16 tpid_inner)
{
    int rv;
    uint32 flags = 0;
    int ing_vlan_action_id;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t action_2;
    bcm_vlan_action_set_t_init(&action);

    flags = BCM_VLAN_ACTION_SET_TPID_ALLOW_DESTROY;

    action.outer_tpid = tpid_outer;
    action.inner_tpid = tpid_inner;

    ing_vlan_action_id = ing_mod_tpid_1;

    rv = bcm_vlan_translate_action_id_set(unit, flags|BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set ing $rv\n");
        return rv;
    }

    return rv;
}


/*
 * Set up the ingress VLAN editor of PON LIF according to service type.
 * Translate svlan or cvlan.
 */
int pon_additional_tpid_port_ingress_vt_set(int unit, pon_service_info_s *service_info, 
        bcm_gport_t pon_gport, int qos_map_id, int* action_id)
{
    int rv=0;
    uint32 flags = 0;
    int ing_tag_format;
    int ing_vlan_action_id;
    int ing_vlan_edit_class_id;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t action_2;
    bcm_vlan_action_set_t_init(&action);

    action.outer_tpid = service_info->down_outer_tpid;
    action.inner_tpid = service_info->down_inner_tpid;

    switch (service_info->service_mode)
    {
        case otag_to_otag2:
            /* PON LIF ingress action: replace inner tag. */        
            action.dt_outer = bcmVlanActionReplace;
            if (qos_map_id > 0) {
                action.dt_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
    
            ing_tag_format = c_tag;
            
            if (service_info->down_outer_tpid == outer_tpid_u16) {
                ing_vlan_action_id = ing_rep_c_tag_0;
                ing_vlan_edit_class_id = 2;
            }
            else if (service_info->down_outer_tpid == inner_tpid_u16) {
                ing_vlan_action_id = ing_rep_c_tag_1;
                ing_vlan_edit_class_id = 3;
            }
            else { /* additional_tpid */
                ing_vlan_action_id = ing_rep_c_tag_2;
                ing_vlan_edit_class_id = 4;
            }
            break;

        case o_i_tag_to_o2_i2_tag:
            /* PON LIF ingress action: replace outer tag and replace inner tag. */
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionReplace;
            if (qos_map_id > 0) {
                action.dt_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
            
            ing_tag_format = s_c_tag;

            if (service_info->down_inner_tpid == inner_tpid_u16) {
                ing_vlan_action_id = ing_rep_s_c_tag_0;
                ing_vlan_edit_class_id = 5;
            }
            else { /* additional_tpid */
                ing_vlan_action_id = ing_rep_s_c_tag_1;
                ing_vlan_edit_class_id = 6;
            }
            break;
            
        default:
            return BCM_E_PARAM;
    }

    rv = bcm_vlan_translate_action_id_get(unit, BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action_2);
    if (rv == BCM_E_NOT_FOUND) {
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &ing_vlan_action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
            return rv;
        }

        rv = bcm_vlan_translate_action_id_set(unit, flags|BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set ing $rv\n");
            return rv;
        }
    }

    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = service_info->down_ovlan;
    port_trans.new_inner_vlan = service_info->down_ivlan;
    port_trans.gport = pon_gport;
    port_trans.vlan_edit_class_id = ing_vlan_edit_class_id;
    port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set ing $rv\n");
        return rv;
    }

    if (NULL != action_id) {
        *action_id = ing_vlan_action_id;
    }

    /* Set translation action class for single tagged packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ing_vlan_edit_class_id;
    action_class.tag_format_class_id = ing_tag_format;
    action_class.vlan_translation_action_id = ing_vlan_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
        return rv;
    }
        
    if (qos_map_id > 0) { 
        rv = bcm_qos_port_map_set(unit, pon_gport, qos_map_id, -1);  
        if (rv != BCM_E_NONE) { 
            printf("Error, bcm_qos_port_map_set ing $rv\n");  
            return rv;
        }
    }

    return rv;
}

int pon_additional_tpid_port_egress_vt_set(int unit, pon_service_info_s *service_info, bcm_gport_t pon_gport)
{
    int rv;
    int triple_tags_service=0;
    int egr_tag_format;
    int egr_vlan_action_id;
    int egr_vlan_edit_class_id;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t action_2;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_action_set_t_init(&action);
    
    switch (service_info->service_mode)
    {
        case otag_to_otag2:
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionAdd;
            action.outer_tpid = 0x8100; /* tunnel id */
            action.inner_tpid = service_info->up_outer_tpid; /* outer VLAN */

            egr_tag_format = c_tag;
            if (service_info->up_outer_tpid == outer_tpid_u16) {
                egr_vlan_action_id = egr_rep_tu_add_c_tag_0;
                egr_vlan_edit_class_id = 2;
            }
            else if (service_info->up_outer_tpid == inner_tpid_u16) {
                egr_vlan_action_id = egr_rep_tu_add_c_tag_1;
                egr_vlan_edit_class_id = 3;
            }
            else { /* additional_tpid */
                egr_vlan_action_id = egr_rep_tu_add_c_tag_2;
                egr_vlan_edit_class_id = 4;
            }
            break;

        case o_i_tag_to_o2_i2_tag:
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionReplace;
            action.outer_tpid = service_info->up_outer_tpid; /* outer VLAN */
            action.inner_tpid = service_info->up_inner_tpid; /* inner VLAN */

            egr_tag_format = s_c_tag;
            if (service_info->up_inner_tpid == inner_tpid_u16) {
                egr_vlan_action_id = egr_add_tu_rep_s_c_tag_0;
                egr_vlan_edit_class_id = 5;
            }
            else { /* additional_tpid */
                egr_vlan_action_id = egr_add_tu_rep_s_c_tag_1;
                egr_vlan_edit_class_id = 6;
            }
            triple_tags_service = 1;
            break;
            
        default:
            return BCM_E_PARAM;
    }

    rv = bcm_vlan_translate_action_id_get(unit, BCM_VLAN_ACTION_SET_EGRESS, egr_vlan_action_id, &action_2);
    if (rv == BCM_E_NOT_FOUND) {
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &egr_vlan_action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create egr $rv\n");
            return rv;
        }

        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, egr_vlan_action_id, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set egr $rv\n");
            return rv;
        }
    }

    bcm_vlan_port_translation_t_init(&port_trans);
    if (triple_tags_service) {
        port_trans.new_outer_vlan = service_info->up_ovlan;
        port_trans.new_inner_vlan = service_info->up_ivlan;
    } else {
        port_trans.new_outer_vlan = service_info->tunnel_id;
        port_trans.new_inner_vlan = service_info->up_ovlan;
    }
    port_trans.gport = pon_gport;
    port_trans.vlan_edit_class_id = egr_vlan_edit_class_id;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set egr $rv\n");
        return rv;
    }

    /* Set translation action class for single tagged packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = egr_vlan_edit_class_id;
    action_class.tag_format_class_id = egr_tag_format;
    action_class.vlan_translation_action_id = egr_vlan_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
        return rv;
    }
    
    return rv;
}

/*
 * Set up 1:1 sercies, using port cross connect.
 */
int pon_additional_tpid_1_1_service(int unit)
{
    int rv;
    int index, is_with_id = 0;
    bcm_vswitch_cross_connect_t gports;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport, pon_pp_port;
    int pon_lif_type, nni_lif_type;

    for (index = 0; index < nof_1_1_services; index++)
    {
        tunnel_id    = pon_additional_tpid_services[index].tunnel_id;
        pon_lif_type = pon_additional_tpid_services[index].up_lif_type;
        up_ovlan     = pon_additional_tpid_services[index].up_ovlan;
        up_ivlan     = pon_additional_tpid_services[index].up_ivlan;
        up_pcp       = pon_additional_tpid_services[index].up_pcp;
        up_ethertype = pon_additional_tpid_services[index].up_ethertype;
        nni_lif_type = pon_additional_tpid_services[index].down_lif_type;
        down_ovlan   = pon_additional_tpid_services[index].down_ovlan;
        down_ivlan   = pon_additional_tpid_services[index].down_ivlan;

        /* Get pon_pp_port by tunnel_id */
        pon_pp_port_get(tunnel_id, &pon_pp_port);

        /* Create PON LIF */
        pon_additional_tpid_lif_create(unit, pon_pp_port, 0, 0, &pon_additional_tpid_services[index], &pon_gport);

        /* Set PON LIF ingress VLAN editor */
        pon_additional_tpid_port_ingress_vt_set(unit, &pon_additional_tpid_services[index], pon_gport, 0, NULL);
        
        /* Set PON LIF egress VLAN editor */
        pon_additional_tpid_port_egress_vt_set(unit, &pon_additional_tpid_services[index], pon_gport);

        /* Create NNI LIF */
        nni_additional_tpid_lif_create(unit, nni_port, is_with_id, &pon_additional_tpid_services[index], &nni_gport, &encap_id);

        pon_additional_tpid_services[index].pon_gport = pon_gport;
        pon_additional_tpid_services[index].nni_gport = nni_gport;

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
int pon_additional_tpid_1_1_service_cleanup(int unit)
{
    int rv;
    int index;
    bcm_vswitch_cross_connect_t gports;
    uint32 flags = 0;
    int ing_vlan_action_id;
    bcm_vlan_action_set_t action;
    bcm_switch_tpid_info_t tpid_info;

    bcm_vlan_action_set_t_init(&action);

    for (index = 0; index < nof_1_1_services; index++)
    {
        /* Delete the cross connected LIFs */
        gports.port1 = pon_additional_tpid_services[index].pon_gport;
        gports.port2 = pon_additional_tpid_services[index].nni_gport;
        rv = bcm_vswitch_cross_connect_delete(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_delete\n");
            print rv;
            return rv;
        }

        /* Delete PON LIF */
        rv = bcm_vlan_port_destroy(unit, pon_additional_tpid_services[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }

        /* Delete NNI LIF */
        rv = bcm_vlan_port_destroy(unit, pon_additional_tpid_services[index].nni_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy \n");
            return rv;
        }
    }

    sal_memset(&tpid_info, 0, sizeof(tpid_info));

    tpid_info.tpid_type = bcmTpidTypeInner;
    tpid_info.tpid_value = additional_tpid;
    rv = bcm_switch_tpid_delete(unit, &tpid_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_tpid_add failed! $rv\n");
        return rv;
    }

    return rv;
}


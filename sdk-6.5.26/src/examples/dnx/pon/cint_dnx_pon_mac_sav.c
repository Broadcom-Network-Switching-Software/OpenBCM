/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* File: cint_dnx_pon_mac_sav.c
* Purpose: An example of the PON MAC SAV application on 1:1 Model. 
*          The following CINT provides a calling sequence example to bring up three main PON services categories: 1:1 Model. 
*          N:1 Model can support the MAC SAV feature without any code modification.
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
*  7. Create NNI LIF
*        - Call bcm_vlan_port_create()with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
*  8. Bing PON LIF with LIF_Profile as MAC_SAV
*        - Call bcm_port_class_set(unit, pon_gport, bcmPortClassFieldIngressVport, in_lif_profile_mac_sav);
*  9. Add Static MAC entries associating {MAC, learn_data}
*        - Call bcm_l2_addr_add() with following criterias:
*          BCM_L2_STATIC and BCM_L2_MOVE_PORT
*  10. Cross connect the 2 LIFs
*        - Call bcm_vswitch_flexible_connect_add()
*           Connect PON LIF(EXTENDED_P2P_W_SRC_LKP) to NNI LIF
*        - Call bcm_vswitch_cross_connect_add()
*           Connect NNI LIF to PON LIF(EXTENDED_P2P_W_SRC_LKP)
*        - Call bcm_vswitch_cross_connect_add()
*           Connect PON LIF <--> NNI LIF
*  11. Configure MAC_SAV trap in the PMF
*        - ((LIF_Profile == `MAC_SAV) & Found == 0x1 & Match == 0x0)
*           Drop the packet and Do not learn
*        - ((LIF_Profile == `MAC_SAV) & Found == 0x0))
*           Drop the packet and Do not learn
*
*/

int in_lif_profile_mac_sav = 2;
bcm_field_group_t cint_attach_fg_id = 0;
bcm_field_entry_t cint_attach_entry_id[2] = {0, 0};
bcm_field_context_t cint_attach_context_id_eth;
bcm_field_presel_t cint_attach_presel_eth = 5;
bcm_field_action_t action_drop = 0;


/*
 * Add the MAC entries associating {MAC, learn_data} 
 */
int pon_mac_sav_add(int unit, int index)
{
    int rv;
    bcm_mac_t mac_address[4]  = 
        {
            { 0x00, 0x00, 0x00, 0x00, 0xBB, 0xBB },
            { 0x00, 0x00, 0x00, 0x00, 0xCC, 0xCC },
            { 0x00, 0x00, 0x00, 0x00, 0xDD, 0xDD },
            { 0x00, 0x00, 0x00, 0x00, 0xEE, 0xEE }
        };
    bcm_vlan_t vsi = 0;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac_address[index], vsi);

    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MOVE_PORT;
    l2_addr.port = pon_1_1_service_info[index].pon_gport;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add index:%d\n", index);
        print rv;
        return rv;
    }

    return rv;
}

/*
 * Delete the MAC entries associating {MAC, learn_data} 
 */
int pon_mac_sav_del(int unit, int index)
{
    int rv;
    bcm_mac_t mac_address[4]  = 
        {
            { 0x00, 0x00, 0x00, 0x00, 0xBB, 0xBB },
            { 0x00, 0x00, 0x00, 0x00, 0xCC, 0xCC },
            { 0x00, 0x00, 0x00, 0x00, 0xDD, 0xDD },
            { 0x00, 0x00, 0x00, 0x00, 0xEE, 0xEE }
        };
    bcm_vlan_t vsi = 0;

    rv = bcm_l2_addr_delete(unit, mac_address[index], vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_delete index:%d\n", index);
        print rv;
        return rv;
    }

    return rv;
}

int pon_mac_sav_1_1_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
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
	
    /*1. PON Port 200 Tunnel-ID 1000(MAC_SAV) <-CrossConnect-> NNI Port 201 SVLAN 10*/
    index = 0;
    tunnel_idx = pon_subport_id << bit_shift | tunnel_base;
    pon_1_1_service_info[index].service_mode = untag_to_s_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 10;
    pon_1_1_service_info[index].mac_sav_enable = 1;

    /*2. PON Port 200 Tunnel-ID 1001 <-CrossConnect-> NNI Port 201 CVLAN 20*/
    index++;
    pon_1_1_service_info[index].service_mode = untag_to_c_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_untag;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 20;
    pon_1_1_service_info[index].mac_sav_enable = 0;

    /*3. PON Port 200 Tunnel-ID 1002 SVLAN 10 SVLAN 20 <-CrossConnect-> NNI Port 201 SVLAN 3010*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_s2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 10;
    pon_1_1_service_info[index].up_ivlan = 20;
    pon_1_1_service_info[index].down_lif_type = match_s_tag;
    pon_1_1_service_info[index].down_ovlan = 3010;
    pon_1_1_service_info[index].mac_sav_enable = 1;

    /*4. PON Port 200 Tunnel-ID 1003 SVLAN 20 SVLAN 30 <-CrossConnect-> NNI Port 201 CVLAN 3020*/
    index++;
    pon_1_1_service_info[index].service_mode = s_s_tag_to_c2_tag;
    pon_1_1_service_info[index].tunnel_id  = tunnel_idx++;
    pon_1_1_service_info[index].up_lif_type = match_s_s_tag;
    pon_1_1_service_info[index].up_ovlan = 20;
    pon_1_1_service_info[index].up_ivlan = 30;
    pon_1_1_service_info[index].down_lif_type = match_c_tag;
    pon_1_1_service_info[index].down_ovlan = 3020;
    pon_1_1_service_info[index].mac_sav_enable = 0;	

    nof_1_1_services = index + 1;

    rv = bcm_port_get(unit,port_pon,&flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlPONEnable $rv\n");
        return rv;
    }

    for (index = 0; index < nof_1_1_services; index++) {
        pon_1_1_service_info[index].pon_pp_port = mapping_info.pp_port;
    }

    return rv;
}

/*
 * Set up 1:1 sercies, using port cross connect.
 * mode: 1: DTC mode; 0: STC mode
 */
int pon_mac_sav_1_1_service(int unit,int dtc_mode)
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
    int pon_lif_type, nni_lif_type, mac_sav;
    bcm_vlan_action_set_t action;
    bcm_port_extender_mapping_info_t mapping_info;
    bcm_vlan_port_t vlan_port_pon;
    bcm_vlan_port_t vlan_port_nni;
    bcm_vswitch_flexible_connect_match_t port_match; 
    bcm_vswitch_flexible_connect_fwd_t port_fwd;
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
        mac_sav      = pon_1_1_service_info[index].mac_sav_enable;

        /* Create PON LIF */
        rv = pon_lif_create(unit, pon_port, pon_lif_type, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, mac_sav, &pon_gport);
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

        if (pon_1_1_service_info[index].mac_sav_enable) {
            /* Configure the PON MAC SAV LIF with special in_lif_profile */
            rv = bcm_port_class_set(unit, pon_1_1_service_info[index].pon_gport, bcmPortClassFieldIngressVport, in_lif_profile_mac_sav);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_port_class_set\n", rv);
                return rv;
            }

            rv = pon_mac_sav_add(unit, index);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), pon_mac_sav_add\n", rv);
                return rv;
            }
        }

        /* Cross connect the 2 LIFs */
        if (pon_1_1_service_info[index].mac_sav_enable) {			
            bcm_vswitch_flexible_connect_match_t_init(&port_match);
            bcm_vswitch_flexible_connect_fwd_t_init(&port_fwd);

            port_match.match = BCM_PORT_MATCH_PORT;
            port_match.match_port = pon_1_1_service_info[index].pon_gport;
            port_fwd.forward_port = nni_port;
            port_fwd.forward_encap = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(pon_1_1_service_info[index].nni_gport);
            rv = bcm_vswitch_flexible_connect_add(unit, 0, &port_match, &port_fwd);
            if (rv != BCM_E_NONE)
            {
                printf("Error(%d), bcm_vswitch_flexible_connect_add\n", rv);
                return rv;
            }

            cross_connect_gports.port1= pon_1_1_service_info[index].nni_gport;
            cross_connect_gports.port2= pon_1_1_service_info[index].pon_gport;
            cross_connect_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
            rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_gports);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), bcm_vswitch_cross_connect_add\n", rv);
                return rv;
            }
        } else {
            cross_connect_gports.port1= pon_1_1_service_info[index].pon_gport;
            cross_connect_gports.port2= pon_1_1_service_info[index].nni_gport;
            cross_connect_gports.flags = 0;
            rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_gports);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), bcm_vswitch_cross_connect_add\n", rv);
                return rv;
            }
        }
    }
	
    rv = cint_field_pon_mac_sav_main(unit);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), cint_field_pon_mac_sav_main\n", rv);
        return rv;
    }
	
    return rv;
}

int pon_mac_sav_1_1_service_cleanup(int unit)
{
    bcm_port_extender_mapping_info_t mapping_info;
    int rv, index = 0;
    bcm_vswitch_flexible_connect_match_t port_match; 
    bcm_vswitch_cross_connect_t cross_connect_gports;

    rv = cint_field_pon_mac_sav_destroy(unit);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), cint_field_pon_mac_sav_destroy\n", rv);
        return rv;
    }
	
    for (index = 0; index < nof_1_1_services; index++) {
        if (pon_1_1_service_info[index].mac_sav_enable) {
            port_match.match = BCM_PORT_MATCH_PORT;
            port_match.match_port = pon_1_1_service_info[index].pon_gport;
            rv = bcm_vswitch_flexible_connect_delete(unit, &port_match);
            if (rv != BCM_E_NONE)
            {
                printf("Error(%d), bcm_vswitch_flexible_connect_delete\n", rv);
                return rv;
            }

            cross_connect_gports.port1= pon_1_1_service_info[index].nni_gport;
            cross_connect_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
            rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect_gports);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), bcm_vswitch_cross_connect_delete\n", rv);
                return rv;
            }
        } else {
            cross_connect_gports.port1= pon_1_1_service_info[index].pon_gport;
            cross_connect_gports.port2= pon_1_1_service_info[index].nni_gport;
            cross_connect_gports.flags = 0;
            rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect_gports);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), bcm_vswitch_cross_connect_delete\n", rv);
                return rv;
            }
        }
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

int cint_field_pon_mac_sav_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_gport_t gport;
    int i_act;
    void *dest_char;
    int rv = BCM_E_NONE;
  
    /* Create drop action */
    rv = cint_field_action_drop_main(unit,bcmFieldStageIngressPMF1, &action_drop);
    if(rv != BCM_E_NONE)
    {
        printf("Error in field_action_drop_main\n");
        return rv;
    }    
	
    /* Init FG */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    
    /* Set quals */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.qual_types[1] = bcmFieldQualifySaLookupAcceptedStrength;	
    fg_info.qual_types[2] = bcmFieldQualifyLearnMatch;	
    fg_info.qual_types[3] = bcmFieldQualifyLearnFound;
 
    /* Set actions */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = action_drop;
    fg_info.action_types[1] = bcmFieldActionIngressDoNotLearn;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "MAC_SAV_Trap_0", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_attach_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_attach_fg_id);
   

    /**Attach Ethernet context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Ethernet", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_attach_context_id_eth);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, cint_attach_fg_id, cint_attach_context_id_eth, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",cint_attach_fg_id, cint_attach_context_id_eth);
    
    
    /* Add entries after attach */
    /* Entry MAC_SAV trap for ((LIF_Profile == MAC_SAV) & Found == 0x1 & Match == 0x0) */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = pon_port;
    ent_info.entry_qual[0].mask[0] = 0x1FF;
    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = 0;
    ent_info.entry_qual[1].mask[0] = 0x1;
    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = 0;
    ent_info.entry_qual[2].mask[0] = 0x1;
    ent_info.entry_qual[3].type = fg_info.qual_types[3];
    ent_info.entry_qual[3].value[0] = 1;
    ent_info.entry_qual[3].mask[0] = 0x1;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0; 
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = 0; 

    rv = bcm_field_entry_add(unit, 0, cint_attach_fg_id, &ent_info, &cint_attach_entry_id[0]);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)\n",cint_attach_entry_id[0]);

    /* Entry MAC_SAV trap for ((LIF_Profile == MAC_SAV) & Found == 0x0) */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 2;

    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = pon_port;
    ent_info.entry_qual[0].mask[0] = 0x1FF;
    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = 0;
    ent_info.entry_qual[1].mask[0] = 0x0;
    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = 0;
    ent_info.entry_qual[2].mask[0] = 0x0;
    ent_info.entry_qual[3].type = fg_info.qual_types[3];
    ent_info.entry_qual[3].value[0] = 0;
    ent_info.entry_qual[3].mask[0] = 0x1;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0; 
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = 0; 

    rv = bcm_field_entry_add(unit, 0, cint_attach_fg_id, &ent_info, &cint_attach_entry_id[1]);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)\n",cint_attach_entry_id[1]);

    /* Add Presel */		
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_attach_presel_eth;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_attach_context_id_eth;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    p_data.qual_data[0].qual_mask = 0x1F;
    p_data.qual_data[1].qual_type = bcmFieldQualifyInVportClass;
    p_data.qual_data[1].qual_value = in_lif_profile_mac_sav;
    p_data.qual_data[1].qual_mask = 0xf;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ethernet) \n",
            cint_attach_presel_eth,cint_attach_context_id_eth);

    return 0;
}

int cint_field_pon_mac_sav_destroy(int unit)
{
    int i;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_attach_presel_eth;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }
	
    rv = bcm_field_group_context_detach(unit, cint_attach_fg_id, cint_attach_context_id_eth);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach eth\n", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit,bcmFieldStageIngressPMF1,cint_attach_context_id_eth);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy eth\n", rv);
        return rv;
    }

    for (i = 0; i < 2; i++) {
        rv = bcm_field_entry_delete(unit,cint_attach_fg_id,entry_qual_info,cint_attach_entry_id[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_delete \n", rv);
            return rv;
        }
    }

    rv = bcm_field_group_delete(unit,cint_attach_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete ipv4\n", rv);
        return rv;
    }

    rv = bcm_field_action_destroy(unit, action_drop);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_destroy action_prefix_stat_action\n", rv);
        return rv;
    }

    return 0;
}



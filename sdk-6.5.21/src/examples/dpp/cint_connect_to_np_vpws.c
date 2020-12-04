/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 * Cint file to configure ingress chip to support connecting with NP
 */

/*
 * 1. SOC config:
 *        custom_feature_conn_to_np_enable=1
 *        custom_feature_conn_to_np_debug=0
 *        bcm886xx_vlan_translate_mode=1
 *        ftmh_dsp_extension_add.BCM88675=1
 *        system_ftmh_load_balancing_ext_mode.BCM88675=ENABLED
 *        field_class_id_size_0.BCM88675=8
 *        field_class_id_size_2.BCM88675=24
 *
 * 2. Calling sequence:
 *        2.1 Create LIF
 *
 *        2.2 Add Data-format entry for ILKN port
 *
 *        2.3 PMF configuration to extract LIF-extension to user header
 *
 *
 *
 *
 *
 *
 * To Activate Above Settings Run:
 *      cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c 
 *      cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
 *      cint ../../../../src/examples/dpp/utility/cint_utils_mpls_port.c 
 *      cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c  
 *      cint ../../../../src/examples/dpp/cint_port_tpid.c 
 *      cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c 
 *      cint ../../../../src/examples/dpp/cint_vswitch_cross_connect_p2p.c  
 *      cint ../../../../src/examples/dpp/cint_connect_to_np_vpws.c
 *
 *      service_port_init(13,13,14);
 *      print config_port_ingress(0, ac_port);
 *
 *      print config_ivec_value_mapping(0,0);
 *      print config_ivec_value_mapping(0,1);
 *      print config_ivec_value_mapping(0,2);
 *      print config_ivec_value_mapping(0,3);
 *
 *      print config_ingress_vpws(0,0);
 *      print config_vlan_status_on_lif(0,vlan_port.vlan_port_id,3);
 *      print config_ingress_vpws(0,1);
 *      print config_extract_lif_auxiliary_data(0);
 */

int ac_port=13;
int pw_port=13;
int ilk_port=14;



/*
 * Setting for IVEC to carry vlan tag number
 */
int outer_tpid = 0x8100;
int inner_tpid = 0x9100;
uint16 no_tag = 0;
uint16 c_tag = 1;
uint16 s_tag = 2;
uint16 prio_tag = 3;
uint16 i_tag = 4;
uint16 c_c_tag = 5;
uint16 s_c_tag = 6;
uint16 prio_c_tag = 7;
uint16 c_s_tag = 9;
uint16 s_s_tag = 10;
uint16 prio_s_tag = 11;

int tpid_profile_tag_format_init(int unit, bcm_port_t port)
{
    int rv = 0;
    int index;
    bcm_port_tpid_class_t port_tpid_class;
        
    rv = bcm_port_tpid_delete_all(unit,port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_delete_all $rv\n");
        return rv;
    }

    /* identify TPID on port */
    rv = bcm_port_tpid_add(unit, port, outer_tpid,0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_add $rv\n");
        return rv;
    }
    
    /* identify TPID on port */
    rv = bcm_port_inner_tpid_set(unit, port, inner_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_inner_tpid_set $rv\n");
        return rv;
    }
    
    /* no tag */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = no_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    /*s-tag*/
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = outer_tpid;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = s_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    
    /*c-tag*/
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = inner_tpid;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = c_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    
    /*s_c-tag*/
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = outer_tpid;
    port_tpid_class.tpid2 = inner_tpid;
    port_tpid_class.tag_format_class_id = s_c_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    
    /*c_c-tag*/
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = inner_tpid;
    port_tpid_class.tpid2 = inner_tpid;
    port_tpid_class.tag_format_class_id = c_c_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    return rv;
}


int config_ivec_value_mapping(int unit, uint32 vlan_edit_profile)
{
    int i;
    bcm_vlan_translate_action_class_t action_class;
    uint8 ivec_value;
    int rv = 0;

    bcm_vlan_translate_action_class_t_init(&action_class);
    
    for (i = 0; i <= 10; i++) {
        action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
        action_class.vlan_edit_class_id = vlan_edit_profile;
        action_class.tag_format_class_id = i;        /* tag_format */
        if ((i==2) || (i==1)) {
            ivec_value=vlan_edit_profile & 0x3 | 1<<2;
        } else if (i==0) {
            ivec_value=vlan_edit_profile & 0x3;
        } else {
            ivec_value=vlan_edit_profile & 0x3 | 3<<2;
        }
        ivec_value = ivec_value | 0x30;
        printf("IVEC value is %#x\n", ivec_value);
        action_class.vlan_translation_action_id = ivec_value;

        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set: $rv\n");
            return rv;
        }

    }

    return rv;
}

/*
 * config VLAN_STATUS on a LIF
 */
int config_vlan_status_on_lif(int unit, bcm_gport_t g_port, uint32 vlan_status)
{
    int rv = 0;
    /* set vlan-edit-profile for created lif */
    bcm_vlan_port_translation_t translation;
    
    translation.gport = g_port;
    translation.flags=BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_port_translation_get(unit, &translation);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_get: $rv\n");
        return rv;
    }

    translation.vlan_edit_class_id=vlan_status;
    rv = bcm_vlan_port_translation_set(unit, &translation);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set: $rv\n");
        return rv;
    }
    printf("Config vlan_status on LIF[%#x] successfully\n", g_port);
    return rv;
}

int config_port_ingress(int unit, bcm_port_t port)
{
    int rv = 0;

    rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set: $rv\n");
        return rv;
    }

    rv = tpid_profile_tag_format_init(unit, port);
    if (rv != BCM_E_NONE) {
        printf("Error, tpid_profile_tag_format_init: $rv\n");
        return rv;
    }
    return rv;
}


/*
 * 
 * 1. config vlan status on one lif which create on this port
 *      g_port: lif gport
 */
int config_ivec_ingress(int unit, bcm_gport_t g_port, uint32 vlan_status)
{
    int rv = 0;
    
    rv = config_vlan_status_on_lif(unit, g_port, vlan_status);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif: $rv\n");
        return rv;
    }


    /* if vlan_status vlaues are same on different lifs, configure once with config_ivec_value_mapping() */
    rv = config_ivec_value_mapping(unit, vlan_status);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ivec_value_mapping: $rv\n");
        return rv;
    }

    printf("config_ivec_ingress sucessfully!\n");
    return rv;
}

/*
 *  LIF-Extension 16 LSB:
 *  0====================================================>15
 *  |----------------------------------------------------
 *  |   2   | 1 | 1 | 1 | 1 | 1 |
 *  |----------------------------------------------------
 *      |     |   |   |   |   |
 *      |     |   |   |   |   |
 *  DS_MODE   |   |   |   |   |  
 *          R/M  IGMP |   |   |
 *                    |   |   |
 *                  H/S  R/L  B_F
 */


int ac_create(int unit, bcm_vlan_port_t *vlan_port, int port, uint16 vlan)
{
    int rv = 0;
    uint64 data_udh;

    bcm_vlan_port_t_init(vlan_port);
    vlan_port->flags |= BCM_VLAN_PORT_INGRESS_WIDE | BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
    vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port->port = port;
    vlan_port->match_vlan = vlan;
    vlan_port->egress_vlan = 3;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }

    /* write auxiliary data to lif extension
     * DS_MODE: 1
     * R/L:     1
     * H/S:     1
     */
    COMPILER_64_SET(data_udh,0x12345678, 0x9abcdef1);
    rv = bcm_port_wide_data_set(unit, vlan_port->vlan_port_id, BCM_PORT_WIDE_DATA_INGRESS, data_udh);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    return rv;
}

int pw_create(int unit, bcm_mpls_port_t *mpls_port, int port, int vsi)
{
    int rv = 0;
    uint64 data_udh;
    
    bcm_mpls_port_t_init(mpls_port);
    cross_connect_info_init(ac_port, 0, port, 0);

    /*mpls_port->flags2 |= BCM_MPLS_PORT2_INGRESS_WIDE;
        
    rv = mpls_port_create(unit,pw_port,mpls_port,0,0,0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port_create: $rv\n");
        return rv;
    }
    */

    int egress_intf;
    int in_vc_label = cross_connect_info.in_vc_label_base++;
    int eg_vc_label = cross_connect_info.eg_vc_label_base++;
    
    rv = mpls_tunnels_config(unit, port, &egress_intf, in_vc_label, eg_vc_label);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_tunnels_config\n");
        return rv;
    }
    
    /* add port, according to VC label */
    bcm_mpls_port_t_init(mpls_port);
  
    /* set port attribures */
    mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port->match_label = in_vc_label;
    mpls_port->flags2= BCM_MPLS_PORT2_INGRESS_WIDE;
    mpls_port->egress_tunnel_if = egress_intf;    
    mpls_port->flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port->port = port;
    mpls_port->egress_label.label = eg_vc_label;    
    mpls_port->failover_id = 0;
    mpls_port->failover_port_id = 0;
    mpls_port->failover_mc_group = 0;
    
    rv = bcm_mpls_port_add(unit, vsi, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }
        
    printf("pw created...\n");

    /* write auxiliary data to lif extension
     * DS_MODE: 1
     * R/L:     1
     * H/S:     1
     */
    COMPILER_64_SET(data_udh,0x12345678, 0x9abcdef1);
    rv = bcm_port_wide_data_set(unit, mpls_port->mpls_port_id, BCM_PORT_WIDE_DATA_INGRESS, data_udh);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    printf("pw write auxiliary data...\n");
    return rv;
}

uint32 g_port_out=13, g_flow_id=0x123, g_slot_out=0, g_out_fp=0x321, g_ps=0, g_main_type=1, g_sub_type=0;

void data_entry_init(uint32 port_out, uint32 flow_id, uint32 slot_out, uint32 out_fp, uint32 ps, uint32 main_type, uint32 sub_type)
{
    g_port_out=port_out;
    g_flow_id=flow_id;
    g_slot_out=slot_out;
    g_out_fp=out_fp;
    g_ps=ps;
    g_main_type=main_type;
    g_sub_type=sub_type;
}


void fill_data_format_entry(uint32 *data, uint32 port_out, uint32 flow_id, uint32 slot_out, uint32 out_fp, uint32 ps, uint32 main_type, uint32 sub_type)
{
    data[0] = (sub_type&0x3) | ((main_type&0x7) << 2) | ((ps&0x7) << 5) | ((out_fp&0x1ffff) << 8) | ((slot_out&0x7f) << 25);
    data[1] = (flow_id&0xffff) | ((port_out&0xff) << 16);
}

int data_format_entry_create(int unit, int *global_lif_id)
{
    uint32 data[2];
    int rv = 0;
    
    bcm_tunnel_initiator_t virtual_tunnel;
    bcm_tunnel_initiator_t_init(&virtual_tunnel);

    virtual_tunnel.type = bcmTunnelTypeCount;

    /* fill information into data-entry */
    fill_data_format_entry(data, g_port_out, g_flow_id, g_slot_out, g_out_fp, g_ps, g_main_type, g_sub_type);

    virtual_tunnel.aux_data = data[0];
    virtual_tunnel.flow_label = data[1];
    virtual_tunnel.ttl = 0;

    rv = bcm_tunnel_initiator_create(unit, NULL, &virtual_tunnel);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create: $rv\n");
        return rv;
    }

    *global_lif_id = BCM_GPORT_TUNNEL_ID_GET(virtual_tunnel.tunnel_id);
    
    return rv;        
}


int data_format_entry_clear(int unit, int global_lif_id)
{
    int rv = 0;
    bcm_l3_intf_t intf;

    intf.l3a_intf_flags = BCM_L3_INGRESS_INTERFACE_INVALID;
    intf.l3a_source_vp = global_lif_id;

    rv = bcm_tunnel_initiator_clear(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_clear: $rv\n");
        return rv;
    }
    printf("data_format_entry_clear[%#x] successfully!\n", global_lif_id);

    return rv;
}


bcm_field_presel_set_t nph_ing_psset;
int nif_ports[] = {13};


int nph_ingress_presel_create(int unit)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int presel_id;
    int index;
    int rv;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }
    
    /* Define the set of nif ports */
    BCM_PBMP_CLEAR(pbm);

    /* Add nif port here, don't include ILK ports */
    for (index=0; index<sizeof(nif_ports)/sizeof(nif_ports[0]); index++) {
        BCM_PBMP_PORT_ADD(pbm, nif_ports[index]);
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, presel_id | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }
    
    BCM_FIELD_PRESEL_INIT(nph_ing_psset);
    BCM_FIELD_PRESEL_ADD(nph_ing_psset, presel_id);      

    return rv;
}


int config_extract_lif_auxiliary_data_with_ports(int unit)
{
    int rv = 0;
    
    /* config for user header */
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext;
    bcm_field_extraction_action_t extact;

    rv = nph_ingress_presel_create(unit);
    if (BCM_E_NONE != rv) {
        printf("Error in nph_ingress_presel_create\n");
        return rv;
    }

    bcm_field_data_qualifier_t lif_extension;
    bcm_field_data_qualifier_t_init(&lif_extension);
    lif_extension.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    lif_extension.offset = 0;
    lif_extension.length = 16;
    lif_extension.qualifier = bcmFieldQualifyInVPortWide;
    rv = bcm_field_data_qualifier_create(unit, &lif_extension);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create: $rv\n");
        return rv;
    }


    bcm_field_group_config_t_init(&grp);
    bcm_field_extraction_action_t_init(&extact);
    bcm_field_extraction_field_t_init(&ext);

    grp.group = -1;

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(unit, grp.qset, lif_extension.qual_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add: $rv\n");
        return rv;
    }


    grp.priority = 15;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_ing_psset;

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_config_create: $rv\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_action_set: $rv\n");
        return rv;
    }


    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_create: $rv\n");
        return rv;
    }

    extact.action = bcmFieldActionClassDestSet;
    ext.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext.bits = 16;
    ext.lsb = 0;
    ext.qualifier = lif_extension.qual_id;
    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 1, &ext);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_direct_extraction_action_add: $rv\n");
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_install: $rv\n");
        return rv;
    }

    return rv;
}



int config_extract_lif_auxiliary_data(int unit)
{
    int rv = 0;
    
    /* config for user header */
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext;
    bcm_field_extraction_action_t extact;

    bcm_field_data_qualifier_t lif_extension;
    bcm_field_data_qualifier_t_init(&lif_extension);
    lif_extension.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    lif_extension.offset = 0;
    lif_extension.length = 16;
    lif_extension.qualifier = bcmFieldQualifyInVPortWide;
    rv = bcm_field_data_qualifier_create(unit, &lif_extension);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create: $rv\n");
        return rv;
    }


    bcm_field_group_config_t_init(&grp);
    bcm_field_extraction_action_t_init(&extact);
    bcm_field_extraction_field_t_init(&ext);

    grp.group = -1;

    BCM_FIELD_QSET_INIT(grp.qset);
    rv = bcm_field_qset_data_qualifier_add(unit, grp.qset, lif_extension.qual_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add: $rv\n");
        return rv;
    }


    grp.priority = 15;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeDirectExtraction;

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_config_create: $rv\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_action_set: $rv\n");
        return rv;
    }


    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_create: $rv\n");
        return rv;
    }

    extact.action = bcmFieldActionClassDestSet;
    ext.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext.bits = 16;
    ext.lsb = 0;
    ext.qualifier = lif_extension.qual_id;
    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 1, &ext);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_direct_extraction_action_add: $rv\n");
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_install: $rv\n");
        return rv;
    }

    return rv;
}


int cross_connect_lif_to_ilk_direction(int unit, bcm_gport_t lif_gport, int ilkn_port, bcm_gport_t virtual_tunnel)
{
    int rv;
    bcm_vswitch_cross_connect_t gports;
    bcm_gport_t gport_ilk;
    
    BCM_GPORT_LOCAL_SET(gport_ilk, ilkn_port);
    gports.port1 = lif_gport;
    gports.port2 = gport_ilk;
    gports.encap1= BCM_FORWARD_ENCAP_ID_INVALID;
    gports.encap2= virtual_tunnel;
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add: $rv\n");
        return rv;
    }
    return rv;
}

void service_port_init(int t_ac_port, int t_pw_port, int t_ilk_port)
{
    ac_port = t_ac_port;
    pw_port = t_pw_port;
    ilk_port = t_ilk_port;
}

void config_p2p_service_port_and_lag(int port, int tgid)
{ 
    int rv;
    bcm_gport_t gport;    

    bcm_vlan_port_t vlan_port_1;
    bcm_vlan_port_t vlan_port_2;
    bcm_vlan_port_t_init(&vlan_port_1);
    bcm_vlan_port_t_init(&vlan_port_2);
    vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT;
    BCM_GPORT_LOCAL_SET(gport, port);
    vlan_port_1.port = gport;
    BCM_GPORT_TRUNK_SET(gport, tgid);
    vlan_port_2.port = gport;
    
    print bcm_vlan_port_create(0, &vlan_port_1);
    print bcm_vlan_port_create(0, &vlan_port_2);
    
    bcm_vswitch_cross_connect_t gports;
    gports.port1= vlan_port_1.vlan_port_id;
    gports.port2= vlan_port_2.vlan_port_id;
    gports.encap1=BCM_FORWARD_ENCAP_ID_INVALID;
    gports.encap2=BCM_FORWARD_ENCAP_ID_INVALID;
    print bcm_vswitch_cross_connect_add(0, &gports);
}


bcm_vlan_port_t vlan_port;
bcm_mpls_port_t mpls_port;

/* create lif cross-connect to ILKN port */
int config_ingress_vpws(int unit, int lif_type)
{
    int rv = 0;

    bcm_gport_t g_virtual_tunnel_lif_id;

    if (lif_type == 0) {        /* lif is AC */
        rv = ac_create(unit, &vlan_port, ac_port, 20);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_create: $rv\n");
            return rv;
        }
        printf("AC create sucessfully!\n");
        rv = data_format_entry_create(unit, &g_virtual_tunnel_lif_id);
        if (rv != BCM_E_NONE) {
            printf("Error, data_format_entry_create: $rv\n");
            return rv;
        }
        printf("data-entry create successfully\n");
        rv = cross_connect_lif_to_ilk_direction(unit, vlan_port.vlan_port_id, ilk_port, g_virtual_tunnel_lif_id);
        if (rv != BCM_E_NONE) {
            printf("Error, cross_connect_lif_to_ilk_bidirection: $rv\n");
            return rv;
        }
        printf("AC configure sucessfully!\n");
    } else if (lif_type == 1) {        /* lif is PW */
        rv = pw_create(unit, &mpls_port, pw_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_create: $rv\n");
            return rv;
        }

        rv = data_format_entry_create(unit, &g_virtual_tunnel_lif_id);
        if (rv != BCM_E_NONE) {
            printf("Error, data_format_entry_create: $rv\n");
            return rv;
        }

        rv = cross_connect_lif_to_ilk_direction(unit, mpls_port.mpls_port_id, ilk_port, g_virtual_tunnel_lif_id);
        if (rv != BCM_E_NONE) {
            printf("Error, cross_connect_lif_to_ilk_bidirection: $rv\n");
            return rv;
        }
        printf("pw create sucessfully!\n");

                
    } else {        /* wrong lif type */
        printf("Wrong input of lif type[%d]\n", lif_type);
    }
    return rv;
}


int config_ingress_vpws_test(int unit)
{
    int pwe_ps=7;
    int pwe_output_fp=0x1000;
    int ac_ps=4;
    int ac_output_fp=0x2000;

    int rv;
    int i;
    
    printf("ac_port[%d], pw_port[%d], ilk_port[%d]\n", ac_port, pw_port, ilk_port);

    rv = config_port_ingress(unit, ac_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_port_ingress: $rv\n");
        return rv;
    }

    for (i = 0; i < 4; i++) {
        rv = config_ivec_value_mapping(unit, i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ivec_value_mapping[%d]\n",i);
            return rv;
        }
    }

    
    data_entry_init(13, 0x1001, 0, pwe_output_fp, pwe_ps, 1, 0);
    printf("Create AC for VPWS\n");
    rv = config_ingress_vpws(0,0);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ingress_vpws-ac: $rv\n");
        return rv;
    }
    printf("Config Vlan_status[2] on AC\n");
    rv = config_vlan_status_on_lif(unit,vlan_port.vlan_port_id,2);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif: $rv\n");
        return rv;
    }

    
    data_entry_init(13, 0x1001, 0, ac_output_fp, ac_ps, 0, 0);
    printf("Create AC for VPWS\n");
    rv = config_ingress_vpws(unit,1);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ingress_vpws-pw: $rv\n");
        return rv;
    }

    rv = config_extract_lif_auxiliary_data(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, config_extract_lif_auxiliary_data: $rv\n");
        return rv;
    }

    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port: $rv\n");
        return rv;
    }

    return rv;
}


/*
 * cint ../../../../src/examples/dpp/cint_trunk.c
 */
int ilk_ports_for_lag[] = {13, 14, 15};
int ac_port_lag = 200;
int pw_port_lag = 200;

int init_ilk_port_for_lag(int p0, int p1, int p2)
{
    ilk_ports_for_lag[0] = p0;
    ilk_ports_for_lag[1] = p1;
    ilk_ports_for_lag[2] = p2;

    return 0;
}


int config_ingres_lag_member(int unit, int member_port, uint16 flow_id, uint16 slot_out, uint16 port_out)
{
    int rv;
    uint32 lag_member_info;
    
    lag_member_info = (flow_id&0xffff) | (port_out&0xff) << 16 | ((slot_out)&0xff) << 24;
    printf("Lag member info: %#x\n", member_port);

    rv = bcm_port_control_set(unit, member_port, bcmPortControlReserved280, lag_member_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set: $rv\n");
        return rv;
    }

    return rv;
}


int clear_ingres_lag(int unit)
{

}


int cross_connect_lif_to_ilk_direction_lag(int unit, bcm_gport_t lif_gport, int tgid, bcm_gport_t virtual_tunnel)
{
    int rv;
    bcm_vswitch_cross_connect_t gports;
    bcm_gport_t gport_ilk;
    
    BCM_GPORT_TRUNK_SET(gport_ilk, tgid);
    gports.port1 = lif_gport;
    gports.port2 = gport_ilk;
    gports.encap1= BCM_FORWARD_ENCAP_ID_INVALID;
    gports.encap2= virtual_tunnel;
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add: $rv\n");
        return rv;
    }
    return rv;
}



/* -------------------------------------------------
 *   (port 200)
 *  injected pkt --------|      |----->LAG-130.0(port13)
 *                       |------|
 *                              |----->LAG-130.1(port14)
 *                              |
 *                              |----->LAG-130.2(port15)
 * -------------------------------------------------
 *  SOC config:
 custom_feature_conn_to_np_enable=1
 custom_feature_conn_to_np_debug=0
 bcm886xx_vlan_translate_mode=1
 ftmh_dsp_extension_add.BCM88675=1
 system_ftmh_load_balancing_ext_mode.BCM88675=ENABLED
 field_class_id_size_0.BCM88675=8
 field_class_id_size_2.BCM88675=24
 custom_feature_vendor_customer65=1
 custom_feature_disable_xtmh=0
 custom_feature_vendor_custom_pp_port_13=1
 custom_feature_vendor_custom_pp_port_14=1
 custom_feature_vendor_custom_pp_port_15=1
 */

int config_ingress_vpws_lag_test(int unit)
{
    int rv,i;
    bcm_gport_t global_lif_id_ac, global_lif_id_pw;
    bcm_trunk_t tid = 130;

    rv = config_port_ingress(unit, ac_port_lag);
    if (rv != BCM_E_NONE) {
        printf("Error, config_port_ingress: $rv\n");
        return rv;
    }

    for (i = 0; i < 4; i++) {
        rv = config_ivec_value_mapping(unit, i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ivec_value_mapping[%d]\n",i);
            return rv;
        }
    }


    /* 1. create LAG group */
    rv = trunk_create(unit, tid, 200, 3, &ilk_ports_for_lag, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, trunk_create: $rv\n");
        return rv;
    }

    /* 2. Config info for every LAG member */
    for (i = 0; i < 3; i++) {
        rv = config_ingres_lag_member(unit, ilk_ports_for_lag[i], 0x3000+i, i, 10+i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ingres_lag_member[%d]: $rv\n", i);
            return rv;
        }
    }

    /* 3. Create service from AC to Lif on LAG */
    /* 3.1 create AC/PW */
    rv = ac_create(unit, vlan_port, ac_port_lag, 20);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_create: $rv\n");
        return rv;
    }

    /* 3.2 create data-entry */
    g_sub_type = 2; /* this is special for VPWS if dest is on lag */
    rv = data_format_entry_create(unit, &global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create: $rv\n");
        return rv;
    }
    rv = pw_create(unit, &mpls_port, pw_port_lag, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_create: $rv\n");
        return rv;
    }
    g_sub_type = 2; /* this is special for VPWS if dest is on lag */
    rv = data_format_entry_create(unit, &global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create: $rv\n");
        return rv;
    }
    
    /* 3.3 cross connect AC/PW -> LAG(with data-entry) */
    rv = cross_connect_lif_to_ilk_direction_lag(unit, vlan_port.vlan_port_id, tid, global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, cross_connect_lif_to_ilk_direction_lag: $rv\n");
        return rv;
    }
    rv = cross_connect_lif_to_ilk_direction_lag(unit, mpls_port.mpls_port_id, tid, global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, cross_connect_lif_to_ilk_bidirection: $rv\n");
        return rv;
    }   


    /* 4. Config other */
    rv = config_vlan_status_on_lif(unit,vlan_port.vlan_port_id,2);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif: $rv\n");
        return rv;
    }

    rv = config_extract_lif_auxiliary_data(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, config_extract_lif_auxiliary_data: $rv\n");
        return rv;
    }

    /* 5. Config LAG ilk */
    printf("Config ingress of ilk_port...\n");
    for (i = 0; i < 3; i++) {
        rv = config_ilk_port(unit, ilk_ports_for_lag[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ilk_port\n");
            return rv;
        }
    }
    
    return rv;
}



/*
 *====================================================================================================
 *------------------------------------VPLS cint-------------------------------------------------------
 *
 *
 *
 *====================================================================================================
 */

bcm_mac_t dest_mac = {0x00,0x00,0x00,0x00,0x00,0x12};
bcm_mac_t dest_mac1 = {0x04,0x05,0x06,0x07,0x08,0x09};

int l2_addr_add_point_to_data_entry(int unit, bcm_mac_t mac, uint16 vid, bcm_gport_t port, int encap_id) {
    int rv;
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.port = port;
    l2addr.vid = vid;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.encap_id = encap_id;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }
    return BCM_E_NONE;
}


int l2_addr_add_point_to_data_entry_lag(int unit, bcm_mac_t mac, uint16 vid, bcm_trunk_t tgid, int encap_id) {
    int rv;
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vid);
    bcm_gport_t     gport;

    BCM_GPORT_TRUNK_SET(gport, tgid);
    l2addr.port = gport;
    l2addr.tgid = tgid;
    l2addr.vid = vid;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.encap_id = encap_id;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }
    return BCM_E_NONE;
}



int config_ilk_port(int unit, bcm_gport_t ilk_port)
{
    int rv;
    bcm_port_extender_mapping_info_t mapping_info;
    
    mapping_info.user_define_value=0;
    mapping_info.pp_port=ilk_port;
    mapping_info.phy_port=ilk_port;

    rv = bcm_port_extender_mapping_info_set(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_extender_mapping_info_set\n");
        return rv;
    }

    return rv;
}


/* create AC and PW and added to VSI */
int config_ingress_vpls_uc(int unit)
{
    bcm_gport_t global_lif_id_ac, global_lif_id_pw;
    int pwe_ps=7;
    int pwe_output_fp=0x1000;
    int ac_ps=3;
    int ac_output_fp=0x2000;
    int vsi=30;
    int rv;
    int i;

    printf("ac_port[%d], pw_port[%d], ilk_port[%d]\n", ac_port, pw_port, ilk_port);
    rv = config_port_ingress(unit, ac_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_port_ingress\n");
        return rv;
    }

    for (i = 0; i < 4; i++) {
        rv = config_ivec_value_mapping(unit,i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ivec_value_mapping[%d]\n",i);
            return rv;
        }
    }

    printf("Create VPN...\n");
    rv = bcm_vswitch_create_with_id(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create_with_id\n");
        return rv;
    }

    printf("Create LIF...\n");
    rv = ac_create(unit, &vlan_port, ac_port, 20);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_create\n");
        return rv;
    }

    rv = pw_create(unit, &mpls_port, pw_port, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, pw_create\n");
        return rv;
    }

    /* for pw */
    data_entry_init(13, 0x1001, 0, ac_output_fp, ac_ps, 1, 1);
    rv = data_format_entry_create(unit, &global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for pw failed\n");
        return rv;
    }
    rv = l2_addr_add_point_to_data_entry(unit,dest_mac1,vsi,ilk_port,global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry for pw failed\n");
        return rv;
    }
    /* for ac */
    data_entry_init(13, 0x1000, 0, pwe_output_fp, pwe_ps, 1, 1);
    rv = data_format_entry_create(unit, &global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for ac failed\n");
        return rv;
    }
    rv = l2_addr_add_point_to_data_entry(unit,dest_mac,vsi,ilk_port,global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry for ac failed\n");
        return rv;
    }
    
    printf("Add AC LIF to VSI...\n");
    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    
    printf("Config VlanStatus on AC...\n");
    rv = config_vlan_status_on_lif(unit,vlan_port.vlan_port_id,1);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif\n");
        return rv;
    }

    rv = config_extract_lif_auxiliary_data(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, config_extract_lif_auxiliary_data\n");
        return rv;
    }
    
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    printf("Config ingress of VPLS UC successfully\n");
    return rv;

}

int config_ingress_vpls_uc_lag(int unit)
{
    int rv,i;
    bcm_gport_t global_lif_id_ac, global_lif_id_pw;
    bcm_trunk_t tid = 130;
    int vsi=30;

    rv = config_port_ingress(unit, ac_port_lag);
    if (rv != BCM_E_NONE) {
        printf("Error, config_port_ingress\n");
        return rv;
    }

    for (i = 0; i < 4; i++) {
        rv = config_ivec_value_mapping(unit,i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ivec_value_mapping[%d]\n",i);
            return rv;
        }
    }

    rv = trunk_create(unit, tid, 200, 3, &ilk_ports_for_lag, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, trunk_create: $rv\n");
        return rv;
    }

    for (i = 0; i < 3; i++) {
        rv = config_ingres_lag_member(unit, ilk_ports_for_lag[i], 0x3000+i, i, 10+i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ingres_lag_member[%d]: $rv\n", i);
            return rv;
        }
    }

    printf("Create VPN...\n");
    rv = bcm_vswitch_create_with_id(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create_with_id\n");
        return rv;
    }

    printf("Create LIF...\n");
    rv = ac_create(unit, &vlan_port, ac_port_lag, 20);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_create\n");
        return rv;
    }
    printf("Add AC LIF to VSI...\n");
    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    
    printf("Create PW in the VSI...\n");
    rv = pw_create(unit, &mpls_port, pw_port_lag, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, pw_create\n");
        return rv;
    }

    g_sub_type = 3; /* this is special for VPLS if dest is on lag */
    rv = data_format_entry_create(unit, &global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for ac failed\n");
        return rv;
    }
    rv = data_format_entry_create(unit, &global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for pw failed\n");
        return rv;
    }

    rv = l2_addr_add_point_to_data_entry_lag(unit,dest_mac1,vsi,tid,global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry_lag for pw failed\n");
        return rv;
    }

    rv = l2_addr_add_point_to_data_entry_lag(unit,dest_mac,vsi,tid,global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry_lag for ac failed\n");
        return rv;
    }

    printf("Config VlanStatus on AC...\n");
    rv = config_vlan_status_on_lif(unit,vlan_port.vlan_port_id,1);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif\n");
        return rv;
    }

    rv = config_extract_lif_auxiliary_data(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, config_extract_lif_auxiliary_data\n");
        return rv;
    }

    printf("Config ingress of ilk_port...\n");
    for (i = 0; i < 3; i++) {
        rv = config_ilk_port(unit, ilk_ports_for_lag[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ilk_port\n");
            return rv;
        }
    }    
    return rv;
}




bcm_mac_t mc_mac = {0x01,0x00,0x5e,0x00,0x00,0x01};

int l2_addr_add_multicast(int unit, bcm_mac_t mac, uint16 vid, bcm_multicast_t mc_id, int encap_id) {
    int rv;
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vid);
    
    l2addr.vid = vid;
    l2addr.encap_id = encap_id;
    l2addr.l2mc_group = mc_id;
    l2addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;


    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }
    return BCM_E_NONE;
}


/**
 *  config VPLS MC with two cases:
 *      1. flood in the VSI
 *      2. Create Multicast group in the VSI
 */
int config_ingress_vpls_mc(int unit)
{
    bcm_gport_t global_lif_id_ac, global_lif_id_pw;
    int pwe_ps=7;
    int pwe_output_fp=0x1000;
    int ac_ps=3;
    int ac_output_fp=0x2000;
    int vsi=30;
    int rv;
    int i;

    int flag;
    int cud;
    bcm_gport_t dest_gport; 
    bcm_gport_t global_lif_id_mc;
    bcm_vlan_control_vlan_t vsi_control;

    bcm_multicast_t mc_id_reserve=6001;
    bcm_mac_t mc_mac = {0x01,0x00,0x5e,0x00,0x00,0x01};
    
    /* mc_id should < 4096 for flood application */
    bcm_multicast_t mc_id=vsi;

    printf("ac_port[%d], pw_port[%d], ilk_port[%d]\n", ac_port, pw_port, ilk_port);
    rv = config_port_ingress(unit, ac_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_port_ingress\n");
        return rv;
    }

    for (i = 0; i < 4; i++) {
        rv = config_ivec_value_mapping(unit,i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ivec_value_mapping[%d]\n",i);
            return rv;
        }
    }

    printf("Create VPN...\n");
    rv = bcm_vswitch_create_with_id(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create_with_id\n");
        return rv;
    }

    printf("Create LIF...\n");
    rv = ac_create(unit, &vlan_port, ac_port, 20);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_create\n");
        return rv;
    }

    rv = pw_create(unit, &mpls_port, pw_port, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, pw_create\n");
        return rv;
    }

    printf("Add AC LIF to VSI...\n");
    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /* for pw */
    data_entry_init(13, 0x1001, 0, ac_output_fp, ac_ps, 1, 1);
    rv = data_format_entry_create(unit, &global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for pw failed\n");
        return rv;
    }
    rv = l2_addr_add_point_to_data_entry(unit,dest_mac1,vsi,ilk_port,global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry for ac failed\n");
        return rv;
    }
    /* for ac */
    data_entry_init(13, 0x1000, 0, pwe_output_fp, pwe_ps, 1, 1);
    rv = data_format_entry_create(unit, &global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for ac failed\n");
        return rv;
    }
    rv = l2_addr_add_point_to_data_entry(unit,dest_mac,vsi,ilk_port,global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry for ac failed\n");
        return rv;
    }

    printf("Config VlanStatus on AC...\n");
    rv = config_vlan_status_on_lif(unit,vlan_port.vlan_port_id,1);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif\n");
        return rv;
    }
    
    rv = config_extract_lif_auxiliary_data(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, config_extract_lif_auxiliary_data\n");
        return rv;
    }
    
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }


    /*
     * config for VPN flood
     */
    printf("Create flood group...\n");
    flag=BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP;
    rv = bcm_multicast_create(unit,flag,&mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create\n");
        return rv;
    }

    bcm_vlan_control_vlan_t_init(&vsi_control);
    vsi_control.forwarding_vlan = vsi;
    vsi_control.unknown_unicast_group   = mc_id;
    vsi_control.unknown_multicast_group = mc_id;
    vsi_control.broadcast_group         = mc_id;
    rv = bcm_vlan_control_vlan_set(unit,vsi,vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }

    data_entry_init(13, 0x1000, 0, mc_id, 7, 2, 2);
    rv = data_format_entry_create(unit, &global_lif_id_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for flood\n");
        return rv;
    }

    print global_lif_id_mc;

    cud=global_lif_id_mc;
    BCM_GPORT_LOCAL_SET(dest_gport,ilk_port);
    rv = bcm_multicast_ingress_add(unit,mc_id,dest_gport,cud);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    /*
     * config for VPLS multicast group in VPN
     */

    /* 1. create one mc group to add ilk port as destination, containing only one copy */
    mc_id=6000;    
    flag=BCM_MULTICAST_TYPE_L2|BCM_MULTICAST_WITH_ID|BCM_MULTICAST_INGRESS_GROUP;
    print bcm_multicast_create(unit,flag,&mc_id_reserve);

    data_entry_init(13, 0x1000, 0, mc_id, 7, 2, 2);
    print data_format_entry_create(unit, &global_lif_id_mc);
    print global_lif_id_mc;

    print l2_addr_add_multicast(unit,mc_mac,vsi,mc_id_reserve,global_lif_id_mc);

    cud=global_lif_id_mc;
    BCM_GPORT_LOCAL_SET(dest_gport,ilk_port);
    print bcm_multicast_ingress_add(unit,mc_id_reserve,dest_gport,cud);

    /* 2. create real mc group */

    printf("Config ingress of VPLS MC successfully\n");
    return rv;


}


/*
 *====================================================================================================
 *------------------------------------MPLS cint-------------------------------------------------------
 * SOC config on ingress side
 *
 * custom_feature_conn_to_np_enable=1
 * custom_feature_conn_to_np_debug=0
 * bcm886xx_vlan_translate_mode=1
 * ftmh_dsp_extension_add.BCM88675=1
 * system_ftmh_load_balancing_ext_mode.BCM88675=ENABLED
 * field_class_id_size_0.BCM88675=8
 * field_class_id_size_2.BCM88675=24
 * custom_feature_injection_with_user_header_enable=1
 * custom_feature_vendor_custom_pp_port_14=1
 * custom_feature_vendor_customer65=1
 * custom_feature_disable_xtmh=0
 *------------------------------------------------------------------------
 * Add soc config for CPU inject packet: custom_feature_injection_with_user_header_enable=1
 *
 *
 * MPLS SWAP
 * Files should cint:
 *  cd ../../../../src/examples/dpp
 *  cint ../sand/utility/cint_sand_utils_global.c
 *  cint ../sand/utility/cint_sand_utils_mpls.c
 *  cint cint_qos.c
 *  cint cint_multi_device_utils.c
 *  cint utility/cint_utils_l2.c
 *  cint utility/cint_utils_l3.c
 *  cint cint_mpls_lsr.c
 *  
 *  cint cint_connect_to_np_vpws.c
 *  cint cint_nph_egress.c
 *  cint
 *
 *  print config_ingress_mpls_push(0);
 *   
 *====================================================================================================
 */

int out_port_mpls = 13;
int in_port_mpls = 13;

unsigned char cpu_port_to_sent_oam=203;

/* ptch + ITMH.base + PPH.base + user header */
/*
 * cpu_pkt[9]=0xA5: indicate this MPLS packet is injected from CPU port.
 */
unsigned char cpu_pkt[] = 
   {0x70, 0xc8, \   /*ptch*/
    0x62, 0x00, 0x00, 0x02, \ /*ITMH.base*/
    0x05, 0x00, 0x00, 0xA5, 0x00, 0x00, 0x00, \ /*PPH.base*/
    0x12, 0x34, 0x56, 0x78, \   /* user header */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x0A, 0x00, 0x02, 0x00, 0x81, 0x00, 0x00, 0x1E, \
    0x88, 0x47, 0x00, 0x19, 0x02, 0x0A, 0x00, 0x3E, 0x85, 0x0B, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, \
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, \
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, \
    0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, \
    0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45};

/* ptch + user header */
unsigned char cpu_pkt_1[] = 
   {0x80, 0xc8, \
    0x12, 0x34, 0x56, 0x78, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x0A, 0x00, 0x02, 0x00, 0x81, 0x00, 0x00, 0x1E, \
    0x88, 0x47, 0x00, 0x19, 0x02, 0x0A, 0x00, 0x3E, 0x85, 0x0B, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, \
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, \
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, \
    0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, \
    0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45};


int hw_cpu_tx_packet(int unit, int pkt_len)
{
    int rc;
    int pkt_flags;

    bcm_pkt_t* tx_pkt;
    pkt_flags = BCM_TX_CRC_ALLOC;
    rc = bcm_pkt_alloc(unit, pkt_len, pkt_flags, &tx_pkt);
    if (rc != BCM_E_NONE) {
        printf("error, bcm_pkt_alloc\n");
        return rc;
    }
    sal_memset(tx_pkt->_pkt_data.data, 0, pkt_len);
    tx_pkt->pkt_data = &tx_pkt->_pkt_data;

    sal_memcpy(tx_pkt->_pkt_data.data, cpu_pkt, pkt_len);

    rc = bcm_tx(unit, tx_pkt, NULL);
    if (rc != BCM_E_NONE) { 
        printf("error, bcm_tx\n");
        return rc;
    }

    rc = bcm_pkt_free(0, tx_pkt);
    if (rc != BCM_E_NONE) {
        printf("error, bcm_pkt_free\n");
        return rc;
    } 
    return rc;
}


void mpls_service_port_init(int t_in_port, int t_out_port, int t_ilk_port)
{
    in_port_mpls = t_in_port;
    out_port_mpls = t_out_port;
    ilk_port = t_ilk_port;
}


int config_ingress_mpls_swap(int unit)
{
    int rv;
    int out_sysport, in_sysport;

    int my_mac_lsb = 0x11;  /* set MAC to 00:00:00:00:00:11 */
    int next_hop_lsb = 0x22; /* set MAC to 00:00:00:00:00:22 */
    int in_label = 1000;
    int out_label  = 2000;
    int in_vid = 10;
    int eg_vid = 10;
    int out_to_tunnel = 1;
    int term_label = 400;
    uint32 next_header_flags = 0; /* indicate next protocol is MPLS */
    int units_ids[1];    
    
    bcm_gport_t global_lif_id;
    create_l3_egress_s l3_egress_fec;

    units_ids[0] = unit;

    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    rv = port_to_system_port(unit, in_port_mpls, &in_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for in_port\n");
        return rv;
    }

    /* init */
    mpls_lsr_init(in_sysport, out_sysport, my_mac_lsb, next_hop_lsb, in_label, out_label, in_vid, eg_vid, out_to_tunnel);

    /* 1. mpls lsr config */
    rv =  mpls_lsr_config_init(units_ids, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_lsr_config_init\n");
        return rv;
    }

    /* 2.  add mpls switch entry */
    /* 2.1 create date entry */
    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,3);
    rv = data_format_entry_create(unit, &global_lif_id);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry: %#x\n",global_lif_id);
    
    /* 2.2 create FEC */
    l3_egress_fec.out_gport = mpls_lsr_info_1.eg_port;
    l3_egress_fec.vlan = mpls_lsr_info_1.eg_vid;
    l3_egress_fec.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;
    l3_egress_fec.arp_encap_id = global_lif_id;
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    printf("FEC[%#x] to resolve ilk_port[%d]-outlif[%#x]\n",l3_egress_fec.fec_id, ilk_port, global_lif_id);
    
    /* 2.3 add switch entry */
    rv = mpls_add_switch_entry(unit,mpls_lsr_info_1.in_label, mpls_lsr_info_1.eg_label, l3_egress_fec.fec_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_switch_entry\n");
        return rv;
    }

    /* 3. add mpls termination entry */
    rv = mpls_add_term_entry_multi_device(units_ids,1,term_label,next_header_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_multi_device\n");
        return rv;
    }


    /* 4. optional, config when running test on Ingress chip.*/
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    return 0;
}


int config_ingress_mpls_pop(int unit)
{
    int rv;
    int out_sysport, in_sysport;

    int my_mac_lsb = 0x11;  /* set MAC to 00:00:00:00:00:11 */
    int next_hop_lsb = 0x22; /* set MAC to 00:00:00:00:00:22 */
    int in_label = 1000;
    int out_label  = 2000;
    int in_vid = 20;
    int eg_vid = 20;
    int out_to_tunnel = 1;
    int term_label = 400;
    uint32 next_header_flags = 0; /* indicate next protocol is MPLS */
    int units_ids[1];
    int tunnel_id;
    
    bcm_gport_t global_lif_id;
    create_l3_egress_s l3_egress_fec;

    units_ids[0] = unit;

    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    rv = port_to_system_port(unit, in_port_mpls, &in_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for in_port\n");
        return rv;
    }

    /* init */
    mpls_lsr_init(in_sysport, out_sysport, my_mac_lsb, next_hop_lsb, in_label, out_label, in_vid, eg_vid, out_to_tunnel);

    /* 1. mpls lsr config */
    rv =  mpls_lsr_config_init(units_ids, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_lsr_config_init\n");
        return rv;
    }

    /* 2.  add mpls pop entry */
    /* 2.1 create date entry */
    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,3);
    rv = data_format_entry_create(unit, &global_lif_id);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry: %#x\n",global_lif_id);
    
    /* 2.2 create FEC */
    l3_egress_fec.out_gport = mpls_lsr_info_1.eg_port;
    l3_egress_fec.vlan = mpls_lsr_info_1.eg_vid;
    l3_egress_fec.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;
    l3_egress_fec.arp_encap_id = global_lif_id;
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    printf("FEC[%#x] to resolve ilk_port[%d]-outlif[%#x]\n",l3_egress_fec.fec_id, ilk_port, global_lif_id);
    
    /* 2.3 add pop entry */
    rv = mpls_add_php_entry(unit, mpls_lsr_info_1.in_label, 0, 0, l3_egress_fec.fec_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_info\n");
        return rv;
    }

    /* 3. add mpls termination entry */
    rv = mpls_add_term_entry_multi_device(units_ids,1,term_label,next_header_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_multi_device\n");
        return rv;
    }


    /* 4. optional, config when running test on Ingress chip.*/
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    return 0;
}


int config_ingress_mpls_push(int unit)
{
    int rv;
    int out_sysport, in_sysport;

    int my_mac_lsb = 0x11;  /* set MAC to 00:00:00:00:00:11 */
    int next_hop_lsb = 0x22; /* set MAC to 00:00:00:00:00:22 */
    int in_label = 1000;
    int out_label  = 2000;
    int in_vid = 30;
    int eg_vid = 30;
    int out_to_tunnel = 1;
    int term_label = 400;
    uint32 next_header_flags = 0; /* indicate next protocol is MPLS */
    int units_ids[1];    
    
    bcm_gport_t global_lif_id;
    create_l3_egress_s l3_egress_fec;

    units_ids[0] = unit;

    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    rv = port_to_system_port(unit, in_port_mpls, &in_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for in_port\n");
        return rv;
    }

    /* init */
    mpls_lsr_init(in_sysport, out_sysport, my_mac_lsb, next_hop_lsb, in_label, out_label, in_vid, eg_vid, out_to_tunnel);

    /* 1. mpls lsr config */
    rv =  mpls_lsr_config_init(units_ids, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_lsr_config_init\n");
        return rv;
    }


    /* 2.  add mpls push entry */
    /* 2.1 create date entry */
    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,3);
    rv = data_format_entry_create(unit, &global_lif_id);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry: %#x\n",global_lif_id);

    /* 2.2 create FEC */
    l3_egress_fec.out_gport = mpls_lsr_info_1.eg_port;
    l3_egress_fec.vlan = mpls_lsr_info_1.eg_vid;
    l3_egress_fec.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;
    l3_egress_fec.arp_encap_id = global_lif_id;
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    printf("FEC[%#x] to resolve ilk_port[%d]-outlif[%#x]\n",l3_egress_fec.fec_id, ilk_port, global_lif_id);

    /* 2.3 add push entry */
    rv = mpls_add_switch_push_entry(unit, mpls_lsr_info_1.in_label, mpls_lsr_info_1.eg_label, l3_egress_fec.fec_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_info\n");
        return rv;
    }

    /* 3. add mpls termination entry */
    rv = mpls_add_term_entry_multi_device(units_ids,1,term_label,next_header_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_multi_device\n");
        return rv;
    }

    /* 4. optional, config when running test on Ingress chip.*/
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    return 0;

}


/*
 *====================================================================================================
 * Add soc config for CPU inject packet: custom_feature_injection_with_user_header_enable=1
 *
 * Files should cint:
 *  cd ../../../../src/examples/dpp
 *  cint ../sand/utility/cint_sand_utils_global.c
 *  cint ../sand/utility/cint_sand_utils_mpls.c
 *  cint cint_qos.c
 *  cint cint_multi_device_utils.c
 *  cint utility/cint_utils_l2.c
 *  cint utility/cint_utils_l3.c
 *  cint cint_mpls_lsr.c
 *  
 *  cint cint_connect_to_np_vpws.c
 *  cint cint_nph_egress.c
 *  cint
 *  print config_ingress_mpls_cpu(0, cpu_port_to_sent_oam);
 *  
 *====================================================================================================
 */

int config_ingress_mpls_cpu(int unit, int cpu_port)
{
    int rv;
    bcm_gport_t global_lif_id;
    create_l3_egress_s l3_egress_fec;
    int out_sysport;

    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,3);
    rv = data_format_entry_create(unit, &global_lif_id);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry: %#x\n",global_lif_id);
    /* create FEC */
    l3_egress_fec.out_gport = out_sysport;
    l3_egress_fec.arp_encap_id = global_lif_id;
    
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    printf("FEC[%#x] to resolve ilk_port[%d]-outlif[%#x]\n",l3_egress_fec.fec_id, ilk_port, global_lif_id); 


    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }


    /* construct cpu packet: OAM over MPLS
     * Carried with PTCH_2 and user define header
 */
    cpu_pkt[1] = cpu_port;

    cpu_pkt[4] = l3_egress_fec.fec_id & 0xff;
    cpu_pkt[3] = (l3_egress_fec.fec_id & 0xff00)>>8;
    cpu_pkt[2] = (cpu_pkt[2]&0xfe) | ((l3_egress_fec.fec_id & 0x10000)>>16);
        

    rv = hw_cpu_tx_packet(unit, 100);
    if (rv != BCM_E_NONE) {
        printf("Error, hw_cpu_tx_packet\n");
        return rv;
    }



    return BCM_E_NONE;
}



/*
 *====================================================================================================
 *------------------------------------L3 VPN cint-------------------------------------------------------
 * SOC config on ingress side
 *
 * custom_feature_conn_to_np_enable=1
 * custom_feature_conn_to_np_debug=0
 * bcm886xx_vlan_translate_mode=1
 * ftmh_dsp_extension_add.BCM88675=1
 * system_ftmh_load_balancing_ext_mode.BCM88675=ENABLED
 * field_class_id_size_0.BCM88675=8
 * field_class_id_size_2.BCM88675=24
 * custom_feature_vendor_custom_pp_port_14=1
 * custom_feature_vendor_customer65=1
 * custom_feature_disable_xtmh=0
 *------------------------------------------------------------------------
 * Files should cint:
 *
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint cint_qos.c 
 * cint utility/cint_utils_l3.c
 * cint utility/cint_utils_l2.c
 * cint utility/cint_utils_vlan.c
 * cint cint_mpls_lsr.c 
 * cint cint_ip_route.c 
 * cint cint_l3vpn.c
 *  
 *  cint cint_connect_to_np_vpws.c
 *  cint cint_nph_egress.c
 *  cint
 *
 *  print config_ingress_l3vpn(0);
 *   
 *====================================================================================================
 */

int ce_port_l3vpn = 13;
int p_port_l3vpn = 13;
uint32 g_nhi_ecmp_fp = 0x11122; /*value for NextHopIndex or LspEmcpFP, if ecmp_en==0, use 16LSB as NHI*/
uint16 g_ulei_port=0x3344;


void fill_data_format_entry1(uint32 *data, uint32 nhi, uint8 ecmp_en, uint16 lsb)
{
    if (ecmp_en == 0) {
        data[0] = ((nhi&0xffff)<<4) | (lsb&0x03) | (((g_ulei_port<<2)&0xff) << 24);
        data[1] = ((g_ulei_port<<2)&0x7f00)>>8;
    } else {
        data[0] = ((nhi&0x1ffff)<<4) | (lsb&0x03) | (((g_ulei_port<<2)&0xff) << 24);
        data[1] = (((g_ulei_port<<2)&0x7f00)>>8) | (1<<15);        
    }

}

int data_format_entry1_create(int unit, int *global_lif_id, uint32 next_eep, uint8 ecmp_en, uint16 lsb)
{
    uint32 data[2];
    int rv = 0;
    
    bcm_tunnel_initiator_t virtual_tunnel;
    bcm_tunnel_initiator_t_init(&virtual_tunnel);

    virtual_tunnel.type = bcmTunnelTypeCount;

    /* fill information into data-entry */
    fill_data_format_entry1(data, g_nhi_ecmp_fp, ecmp_en, lsb);

    virtual_tunnel.aux_data = data[0];
    virtual_tunnel.flow_label = data[1];
    virtual_tunnel.dip = next_eep;
    virtual_tunnel.ttl = 1; /*indicate this is data format-entry 1, default data-format entry 0*/

    rv = bcm_tunnel_initiator_create(unit, NULL, &virtual_tunnel);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create: $rv\n");
        return rv;
    }

    *global_lif_id = BCM_GPORT_TUNNEL_ID_GET(virtual_tunnel.tunnel_id);
    
    return rv;        
}

/*
 * global_lif_id ------->   |--------------|  g_lif_id  |--------------|
 *                          | data-entry 1 |----------->| data-entry 0 |
 *                          |--------------|            |--------------|
 */

int two_data_format_entry_create(int unit, int *global_lif_id, uint8 ecmp_en, uint16 lsb)
{
    int rv;
    uint32 next_eep;
    int g_lif_id;

    /* 1. create data entry 0 */
    rv = data_format_entry_create(unit, &g_lif_id);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Create data-entry 0 successfully, global_lif[%#x]\n", g_lif_id);

    next_eep = g_lif_id;

    /* 2. create data entry 1 and next eep point to data entry 0 */
    rv = data_format_entry1_create(unit, global_lif_id, next_eep, ecmp_en, lsb);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry1_create\n");
        return rv;
    }
    
    return 0;
}


int config_ingress_l3vpn(int unit)
{
    uint8 my_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P2_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
    uint8 ce1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    my_mac[5] = 0x55; 
    next_hop_to_P1_mac[5] = 0x12; 
    next_hop_to_P2_mac[5] = 0x34; 
    ce1_mac[4] = 0xce; 
    ce1_mac[5] = 0x1;

    int rv;
    int vrf = 10;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    int ing_intf;
    int egr_intf;
    int out_sysport;

    int global_lif_id;
    bcm_gport_t gport_tmp;

    /* 0. init for L3 vpn config */
    l3vpn_info_init(unit,
        ce_port_l3vpn,
        p_port_l3vpn,
        my_mac,
        next_hop_to_P1_mac,
        next_hop_to_P2_mac,
        ce1_mac,
        10,20,30,40,50,
        0,
        1000,100,101
        );

    /* 1.1 create VSI for ce side and add ce port to VSI, then create l3 intf for it*/
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P1, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P1, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P1, l3vpn_info.ce_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.ce_port, l3vpn_info.vlan_to_vpn_P1);
      return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = l3vpn_info.vlan_to_vpn_P1;   /*vid=30*/
    intf.my_global_mac = l3vpn_info.my_mac;
    intf.my_lsb_mac = l3vpn_info.my_mac;
    intf.vrf_valid = 1;
    intf.vrf = vrf;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*--------------------------------------------------------------------------------
     * config for direction: CE->PE->P
     *--------------------------------------------------------------------------------
     */
    /* 2.1. create VSI for core side and add p port to VSI, then create l3 intf on this VSI*/
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P2, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P2, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P2, l3vpn_info.p_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.p_port, l3vpn_info.vlan_to_vpn_P2);
      return rv;
    }

    intf.vsi = l3vpn_info.vlan_to_vpn_P2;   /*vid=40*/

    rv = l3__intf_rif__create(unit, &intf);
    egr_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /* 2.2. create virtual tunnel */
    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,2);

    /*
     * if ecmp is enabled, set as:
     *      "two_data_format_entry_create(unit, &global_lif_id, 1, 0);"
     */
    rv = two_data_format_entry_create(unit, &global_lif_id, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry 1: %#x\n",global_lif_id);


    /* 2.3. create FEC to resolve to data-entry encap */
    int fec; 
    bcm_if_t encap_id; 
    create_l3_egress_s l3eg, l3eg1;

    l3eg.out_gport = out_sysport;
    l3eg.arp_encap_id = global_lif_id;

    printf("Create FEC to point to ILK...\n");
    rv = l3__egress_only_fec__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;
    printf("FEC:   %#X\n", l3eg.fec_id);

    /* 2.4. Add IP route entry belonged to this FEC */
    int route = 0x5A5A5A5A; /* 90.90.90.90 */
    int mask  = 0xfffffff0; 
    /* Add route to host90 */
    rv = add_route(unit, route, mask , vrf, fec);

    /*--------------------------------------------------------------------------------
     * config for direction: P->PE->CE
     *--------------------------------------------------------------------------------
     */
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P2, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P2, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P2, l3vpn_info.p_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.p_port, l3vpn_info.vlan_to_vpn_P2);
      return rv;
    }
    
    intf.vsi = l3vpn_info.vlan_to_vpn_P2;
    
    rv = l3__intf_rif__create(unit, &intf);
    egr_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /* tunnel termination: terminate 2 mpls headers (mpls label and VC label) */
    rv = mpls_add_term_entry(unit, l3vpn_info.mpls_label_to_access, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_term_entry for label %x \n", l3vpn_info.mpls_label_to_access);
        return rv;
    }

    /* tunnel termination: terminate 2 mpls headers (mpls label and VC label) */
    /* vc label is 200 (see create_tunnel_initiator function, where the vc label is encapsulated) */
    rv = mpls_add_term_entry(unit, 0xc8, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_term_entry for label %x \n", l3vpn_info.mpls_label_to_access);
        return rv;
    }

    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,0,2);
    rv = two_data_format_entry_create(unit, &global_lif_id, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry 1: %#x\n",global_lif_id);


    int fec_access_routing; 
    encap_id = 0; 

    l3eg.out_gport = out_sysport;
    l3eg.arp_encap_id = global_lif_id;

    rv = l3__egress_only_fec__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec_access_routing = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;


    /* add route to CE port */
    route = 0x0B0B0B0B ; /* 11.11.11.11 */ 
    mask  = 0xfffffff0; 
    rv = add_route(unit, route, mask, vrf, fec_access_routing);
    
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    return rv;
}


/*
 *====================================================================================================
 *------------------------------------L2 to L3 VPN cint-------------------------------------------------------
 * SOC config on ingress side
 *
 * custom_feature_conn_to_np_enable=1
 * custom_feature_conn_to_np_debug=0
 * bcm886xx_vlan_translate_mode=1
 * ftmh_dsp_extension_add.BCM88675=1
 * system_ftmh_load_balancing_ext_mode.BCM88675=ENABLED
 * field_class_id_size_0.BCM88675=8
 * field_class_id_size_2.BCM88675=24
 * custom_feature_vendor_custom_pp_port_14=1
 * custom_feature_vendor_customer65=1
 * custom_feature_disable_xtmh=0
 * bcm886xx_ipv6_tunnel_enable=0
 * bcm886xx_roo_enable=1
 * 
 *------------------------------------------------------------------------

 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint utility/cint_utils_vlan.c
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_mpls_port.c
 * cint utility/cint_utils_l2.c
 * cint utility/cint_utils_l3.c
 * cint utility/cint_utils_multicast.c
 * cint utility/cint_utils_roo.c
 * cint cint_port_tpid.c
 * cint cint_advanced_vlan_translation_mode.c
 * cint cint_qos.c
 * cint cint_mpls_lsr.c
 * cint cint_vswitch_metro_mp.c
 * cint cint_vswitch_vpls.c
 * cint cint_vswitch_vpls_roo.c
 * cint cint_system_vswitch_encoding.c
 * cint cint_connect_to_np_vpws.c
 * cint cint_nph_egress.c
 * cint
 *
 * config_ingress_l2vpn_to_l3vpn(0);
 */

int config_ingress_vpn_bridge(int unit)
{

    int rv;
    int native_ingress_intf; /* l3_intf for native routing. */
    bcm_failover_t failover_id_fec;
    create_l3_egress_s l3_egress_into_overlay, l3_egress_out_of_overlay;
    l3_ipv4_route_entry_utils_s route_entry;
    bcm_l3_host_t l3_host_simple_routing; /* LEM entry for simple out of overlay routing */
    create_l3_intf_s access_intf, core_intf;

    int out_sysport;
    int vrf = 10;


    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }
  
    port_tpid_init(vswitch_vpls_info_1.pwe_in_port,1,0);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }
    
    port_tpid_init(vswitch_vpls_info_1.ac_in_port,1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
        are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }
   
    /* Init vlan */  
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port1_outer_vlan);
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port1_inner_vlan);
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port2_outer_vlan);
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port2_inner_vlan);

    if (verbose1) {
        printf("vswitch_vpls_roo_run create vswitch\n");
    }
    /* create vswitch */
    rv = mpls_port__vswitch_vpls_vpn_create__set(unit, vswitch_vpls_info_1.vpn_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_port__vswitch_vpls_vpn_create__set\n");
        return rv;
    }

    /* create LL for VPLS tunnel */
    /* Routing Over PWE */
    
        /* In ROO, the overlay LL encapsulation is built with a different API call 
           (bcm_l2_egress_create instead of bcm_l3_egress create) */
    bcm_l2_egress_t l2_egress_overlay; 
    bcm_l2_egress_t_init(&l2_egress_overlay);

    l2_egress_overlay.dest_mac   = vswitch_vpls_roo_info.mac;  /* next hop. default: {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d} */
    l2_egress_overlay.src_mac    = mpls_lsr_info_1.my_mac;  /* my-mac */
    l2_egress_overlay.outer_vlan = vswitch_vpls_roo_info.out_to_core_vlan; /* PCP DEI (0) + outer_vlan (200=0xC8)  */
    l2_egress_overlay.ethertype  = 0x8847;       /* ethertype for MPLS */
    l2_egress_overlay.outer_tpid = 0x8100;      /* outer tpid */

    rv = roo__overlay_link_layer_create(unit, 
                                        &l2_egress_overlay, 
                                        0, /* l3 flags */ 
                                        &(vswitch_vpls_roo_info.ll_intfs[0]),
                                        0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in roo__overlay_link_layer_create\n");
        return rv;
    }

    if (roo_dscp_to_exp_map_enable) {
         bcm_gport_t gport;
         BCM_GPORT_TUNNEL_ID_SET(gport,BCM_L3_ITF_VAL_GET(vswitch_vpls_roo_info.ll_intfs[0]));
         rv = bcm_qos_port_map_set(unit,gport,-1,roo_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in roo__overlay_link_layer qos map failed %d\n",rv);
            return rv;
         }
    if (verbose1) {
        printf("roo dscp to exp mapping completed,roo_qos_map_id =  0x%x\n", roo_qos_map_id);
        }
    }
    

    if (verbose1) {
        printf("Configured LL: %d\n", vswitch_vpls_roo_info.ll_intfs[0]);
    }

    /* Routing Over PWE */
    /* Create native router interface */
    create_l3_intf_s intf;
    intf.vsi = vswitch_vpls_info_1.vpn_id;
    intf.my_global_mac = mpls_lsr_info_1.my_mac;
    intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
    intf.vrf_valid = 1;
    intf.vrf = vrf;

    rv = l3__intf_rif__create(unit, &intf);
    native_ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }

    bcm_if_t native_intf; 
    int native_ll_intf_id;

    /* For ROO first the MPLS PORT should be configured and only then the MPLS TUNNEL */
    if (verbose1) {
        printf("vswitch_vpls_roo_run add vlan access port\n");
    }
    /* add mpls access port */
    rv = vswitch_vpls_add_access_port_1(unit, -1/*second_unit*/, &vswitch_vpls_info_1.access_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_add_access_port_1\n");
        return rv;
    }

    if (verbose1) {
        printf("vswitch_vpls_roo_run add mpls network port\n");
    }
    /* add mpls network port */
    rv = vswitch_vpls_add_network_port_1(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_add_network_port_1\n");
        return rv;
    }

    if (verbose1) {
        printf("vswitch_vpls_roo_run add mpls tunnel\n");
    }

    /* configure MPLS tunnels */
    rv  = mpls_tunnels_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_tunnels_config\n");
        return rv;
    }

    int fec; 
    bcm_if_t encap_id; 
    create_l3_egress_s l3eg, l3eg1;
    int global_lif_id=0x3211;

    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,2);
    rv = two_data_format_entry_create(unit, &global_lif_id, 0, 2);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry 1: %#x\n",global_lif_id);

    
    l3eg.out_gport = out_sysport;
    l3eg.arp_encap_id = global_lif_id;
    
    printf("Create FEC to point to ILK...\n");
    rv = l3__egress_only_fec__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }
    
    fec = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;
    printf("FEC:   %#X\n", l3eg.fec_id);
    
    int route = 0xA0A1A1A1; /* 160.161.161.161 */
    int mask  = 0xfffffff0; 
    /* Add route to host90 */
    rv = add_route(unit, route, mask, vrf, fec);

    return rv;


}

int config_ingress_l2vpn_to_l3vpn(int unit)
{
    int rv;
    int vpn_id=620;

    /* init
     * Take vswitch_vpls_info_1 to store global info 
     */
    vswitch_vpls_roo_info_init(unit, ilk_port, pw_port, pw_port, 10, 20, 15, 30, vpn_id, 1);

    /*
     *  mutation of "vswitch_vpls_roo_run()"
     */
    rv = config_ingress_vpn_bridge(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ingress_vpn_bridge\n");
        return rv;
    }

    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    return rv;

}


/*
 *====================================================================================================
 *------------------------------------L3VPN over lag--------------------------------------------------
 */
int config_ingress_l3vpn_lag(int unit)
{
    bcm_trunk_t tid = 130;

    uint8 my_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P2_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
    uint8 ce1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    my_mac[5] = 0x55; 
    next_hop_to_P1_mac[5] = 0x12; 
    next_hop_to_P2_mac[5] = 0x34; 
    ce1_mac[4] = 0xce; 
    ce1_mac[5] = 0x1;

    int rv;
    int i;
    int vrf = 10;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    int ing_intf;
    int egr_intf;
    bcm_gport_t gport;

    int global_lif_id;
    bcm_gport_t gport_tmp;

    /* 0. init for L3 vpn config */
    l3vpn_info_init(unit,
        ce_port_l3vpn,
        p_port_l3vpn,
        my_mac,
        next_hop_to_P1_mac,
        next_hop_to_P2_mac,
        ce1_mac,
        10,20,30,40,50,
        0,
        1000,100,101
        );

    /* 0.1 create VSI for ce side and add ce port to VSI, then create l3 intf for it*/
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P1, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P1, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P1, l3vpn_info.ce_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.ce_port, l3vpn_info.vlan_to_vpn_P1);
      return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = l3vpn_info.vlan_to_vpn_P1;   /*vid=30*/
    intf.my_global_mac = l3vpn_info.my_mac;
    intf.my_lsb_mac = l3vpn_info.my_mac;
    intf.vrf_valid = 1;
    intf.vrf = vrf;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*--------------------------------------------------------------------------------
     * Create LAG Group
     *--------------------------------------------------------------------------------
     */
    /* 1.1 create LAG group */
    rv = trunk_create(unit, tid, 200, 3, &ilk_ports_for_lag, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, trunk_create: $rv\n");
        return rv;
    }

    /* 1.2 Config info for every LAG member */
    for (i = 0; i < 3; i++) {
        rv = config_ingres_lag_member(unit, ilk_ports_for_lag[i], 0x3000+i, i, 10+i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ingres_lag_member[%d]: $rv\n", i);
            return rv;
        }
    }

    /*--------------------------------------------------------------------------------
     * config for direction: CE->PE->P
     *--------------------------------------------------------------------------------
     */
    /* 2.1. create VSI for core side and add p port to VSI, then create l3 intf on this VSI*/
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P2, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P2, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P2, l3vpn_info.p_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.p_port, l3vpn_info.vlan_to_vpn_P2);
      return rv;
    }

    intf.vsi = l3vpn_info.vlan_to_vpn_P2;   /*vid=40*/
    
    rv = l3__intf_rif__create(unit, &intf);
    egr_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /* 2.2. create virtual tunnel */
    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,2);
    rv = two_data_format_entry_create(unit, &global_lif_id, 0, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry 1: %#x\n",global_lif_id);

    
    /* 2.3. create FEC to resolve to data-entry encap */

    int fec; 
    bcm_if_t encap_id; 
    create_l3_egress_s l3eg, l3eg1;

    BCM_GPORT_TRUNK_SET(gport, tid);

    l3eg.out_gport = gport;
    l3eg.arp_encap_id = global_lif_id;

    printf("Create FEC to point to ILK...\n");
    rv = l3__egress_only_fec__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;
    printf("FEC:   %#X\n", l3eg.fec_id);

    /* 2.4. Add IP route entry belonged to this FEC */
    int route = 0x5A5A5A5A; /* 90.90.90.90 */
    int mask  = 0xfffffff0; 
    /* Add route to host90 */
    rv = add_route(unit, route, mask , vrf, fec);

    printf("Config ingress of ilk_port...\n");
    for (i = 0; i < 3; i++) {
        rv = config_ilk_port(unit, ilk_ports_for_lag[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ilk_port\n");
            return rv;
        }
    }

    return rv;
}



/*
 *====================================================================================================
 *------------------------------------L2 to L3 VPN cint-----------------------------------------------
 * SOC config on ingress side
 *
 * custom_feature_conn_to_np_enable=1
 * custom_feature_conn_to_np_debug=0
 * bcm886xx_vlan_translate_mode=1
 * ftmh_dsp_extension_add.BCM88675=1
 * system_ftmh_load_balancing_ext_mode.BCM88675=ENABLED
 * field_class_id_size_0.BCM88675=8
 * field_class_id_size_2.BCM88675=24
 * custom_feature_vendor_custom_pp_port_14=1
 * custom_feature_vendor_customer65=1
 * custom_feature_disable_xtmh=0
 * bcm886xx_ipv6_tunnel_enable=0
 * 
 *------------------------------------------------------------------------
 cd ../../../../src/examples/dpp
 cint ../sand/utility/cint_sand_utils_global.c
 cint ../sand/utility/cint_sand_utils_mpls.c
 cint cint_qos.c 
 cint utility/cint_utils_l3.c
 cint utility/cint_utils_l2.c
 cint utility/cint_utils_vlan.c
 cint cint_mpls_lsr.c 
 cint cint_ipmc.c
 
 cint cint_l3vpn.c
 
 cint cint_connect_to_np_vpws.c
 cint cint_nph_egress.c
 cint
 
 print config_ingress_ipmc(0);

 */

int config_ingress_ipmc(int unit)
{
    int ud;
    int cud;

    uint8 my_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P2_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
    uint8 ce1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    my_mac[5] = 0x55; 
    next_hop_to_P1_mac[5] = 0x12; 
    next_hop_to_P2_mac[5] = 0x34; 
    ce1_mac[4] = 0xce; 
    ce1_mac[5] = 0x1;

    int rv;
    int vrf = 10;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    int ing_intf;
    int egr_intf;
    int out_sysport;
    int pw_outflow=0x12345;

    int global_lif_id;
    bcm_gport_t gport_tmp;
    bcm_gport_t dest_gport;
    bcm_multicast_t mc_id;
    bcm_gport_t global_lif_id_mc;

    /* 0. init for L3 vpn config */
    l3vpn_info_init(unit,
        ce_port_l3vpn,
        p_port_l3vpn,
        my_mac,
        next_hop_to_P1_mac,
        next_hop_to_P2_mac,
        ce1_mac,
        10,20,30,40,50,
        0,
        1000,100,101
        );

    /* 1.1 create VSI for ce side and add ce port to VSI, then create l3 intf for it*/
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P1, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P1, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P1, l3vpn_info.ce_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.ce_port, l3vpn_info.vlan_to_vpn_P1);
      return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = l3vpn_info.vlan_to_vpn_P1;   /*vid=30*/
    intf.my_global_mac = l3vpn_info.my_mac;
    intf.my_lsb_mac = l3vpn_info.my_mac;
    intf.vrf_valid = 1;
    intf.vrf = vrf;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }


    MulticastNum = 1;
    /* 2. Create Multicast Group for each entry */
    mcL3Create(unit);
    /* 3. Clear IPMC table */
    rv = bcm_ipmc_remove_all(unit);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_delete_all returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add IPv4 MC entries */
    addMulticast(unit, 30);
    /* Traverse IPv4 IPMC table */
    rv = bcm_ipmc_traverse(unit, 0, ipmcTraversePrintCb, &ud);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_traverse\n");
        return rv;
    }

    mc_id=mc.ipmc_index;    
    /* MainType=1 and subType=1 to indicate the service */
    data_entry_init(13, 0x1000, 0, mc_id, 7, 1, 0);
    rv = data_format_entry_create(unit, &global_lif_id_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for flood\n");
        return rv;
    }

    print global_lif_id_mc;

    cud=global_lif_id_mc;    
    BCM_GPORT_LOCAL_SET(dest_gport,ilk_port);
    rv = bcm_multicast_ingress_add(unit,mc_id,dest_gport,cud);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }


    return rv;
}

/*
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../../../src/examples/dpp/cint_mpls_mc.c
 * cint ../../../../src/examples/dpp/cint_connect_to_np_vpws.c
 * cint ../../../../src/examples/dpp/cint_nph_egress.c
 * cint
 *
 * print config_ingress_p2mp(0);
 */


int config_ingress_p2mp(int unit)
{
    int rv;
    bcm_gport_t dest_gport;
    bcm_multicast_t mc_id;
    bcm_gport_t global_lif_id_mc;
    int cud;


    rv = mpls_mc_config(unit, in_port_mpls, ilk_port, 0, 0, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_mc_config\n");
        return rv;
    }

    mc_id = mpls_mc_info.mc_group;

    data_entry_init(13, 0x1000, 0, mc_id, 7, 0, 2);
    rv = data_format_entry_create(unit, &global_lif_id_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for flood\n");
        return rv;
    }

    cud=global_lif_id_mc;
    BCM_GPORT_LOCAL_SET(dest_gport,ilk_port);
    rv = bcm_multicast_ingress_add(unit, mc_id, dest_gport, cud);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_multicast_ingress_add\n");
      return rv;
    }
    
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }
    return rv;
}




/*
 * This cint example will only include local flow in P2MP, non-local flow please above.
 */

int recycle_port=170;

int config_ingress_p2mp_local_ipmc(int unit)
{
    int ud;
    int cud;

    uint8 my_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P2_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
    uint8 ce1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    my_mac[5] = 0x11; 
    next_hop_to_P1_mac[5] = 0x12; 
    next_hop_to_P2_mac[5] = 0x34; 
    ce1_mac[4] = 0xce; 
    ce1_mac[5] = 0x1;

    int rv;
    int vrf = 10;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    int ing_intf;
    int egr_intf;
    int out_sysport;
    int pw_outflow=0x12345;

    int global_lif_id;
    bcm_gport_t gport_tmp;
    bcm_gport_t dest_gport;
    bcm_multicast_t mc_id;
    bcm_gport_t global_lif_id_mc;

    /* 0. init for L3 vpn config */
    l3vpn_info_init(unit,
        ce_port_l3vpn,
        p_port_l3vpn,
        my_mac,
        next_hop_to_P1_mac,
        next_hop_to_P2_mac,
        ce1_mac,
        10,20,100,40,50,
        0,
        1000,100,101
        );

    /* 1.1 create VSI for ce side and add ce port to VSI, then create l3 intf for it*/
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P1, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P1, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P1, l3vpn_info.ce_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.ce_port, l3vpn_info.vlan_to_vpn_P1);
      return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = l3vpn_info.vlan_to_vpn_P1;   /*vid=100*/
    intf.my_global_mac = l3vpn_info.my_mac;
    intf.my_lsb_mac = l3vpn_info.my_mac;
    intf.vrf_valid = 1;
    intf.vrf = vrf;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }


    MulticastNum = 1;
    /* 2. Create Multicast Group for each entry */
    mcL3Create(unit);
    /* 3. Clear IPMC table */
    rv = bcm_ipmc_remove_all(unit);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_delete_all returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add IPv4 MC entries */
    addMulticast(unit, 100);
    /* Traverse IPv4 IPMC table */
    rv = bcm_ipmc_traverse(unit, 0, ipmcTraversePrintCb, &ud);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_traverse\n");
        return rv;
    }

    mc_id=mc.ipmc_index;    
    /* MainType=1 and subType=1 to indicate the service */
    data_entry_init(13, 0x1000, 0, mc_id, 7, 1, 0);
    rv = data_format_entry_create(unit, &global_lif_id_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for flood\n");
        return rv;
    }

    print global_lif_id_mc;

    cud=global_lif_id_mc;    
    BCM_GPORT_LOCAL_SET(dest_gport,ilk_port);
    rv = bcm_multicast_ingress_add(unit,mc_id,dest_gport,cud);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    return rv;
}


/*

SOC config:
    custom_feature_conn_to_np_enable=1
    custom_feature_conn_to_np_debug=0
    bcm886xx_vlan_translate_mode=1
    ftmh_dsp_extension_add=1
    system_ftmh_load_balancing_ext_mode=ENABLED
    field_class_id_size_0=8
    field_class_id_size_2=24
    custom_feature_vendor_custom_pp_port_14=1
    custom_feature_vendor_customer65=1
    custom_feature_disable_xtmh=0 
    ucode_port_170=RCY.0:core_0.170
    tm_port_header_type_out_170=ETH
    tm_port_header_type_in_170=INJECTED_2_PP

cint config:
    cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
    cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
    cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
    cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c
    cint ../../../../src/examples/dpp/cint_mpls_mc.c
    cint ../../../../src/examples/dpp/cint_l3vpn.c
    cint ../../../../src/examples/dpp/cint_ipmc.c
    cint ../../../../src/examples/dpp/cint_connect_to_np_vpws.c
    cint ../../../../src/examples/dpp/cint_nph_egress.c
    cint
    print config_ingress_p2mp_local_flow(0);
    exit;
 
 */


/*
 * Packet for each round:
 * 
 * 1st round
 *    |--------------------|------------|------------|-------------------------|-------------------------
 *    |    DMAC + SMAC     |  Vlan-Tag  | MPLS shim  |       IP Header         |    payload
 *    |--------------------|------------|------------|-------------------------|-------------------------
 *
 * 2nd round(recycled packet)
 *
 *    |--------------------|------------|---------|-------------------------|-------------------------
 *    |    DMAC + SMAC     |  Vlan-Tag  | 0x0800  |       IP Header         |    payload
 *    |--------------------|------------|---------|-------------------------|-------------------------
 *
 * Link layer header and IP header kept as original.
 *
 * Cint configuration should:
 *      1. config for MPLS MC in the first round
 *      2. config for IP MC in the second round.
 */

int config_ingress_p2mp_local_flow(int unit)
{
    int rv;
    bcm_gport_t dest_gport;
    bcm_multicast_t mc_id;
    bcm_gport_t global_lif_id_mc;
    int cud;

    /*1. config for first loop*/
    rv = mpls_mc_config(unit, in_port_mpls, recycle_port, 0, 0, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_mc_config\n");
        return rv;
    }

    mc_id = mpls_mc_info.mc_group;

    /* 
     * data_entry.2lsb = 1: P2MP local service should recycle.
     */   

    data_entry_init(13, 0x1000, 0, mc_id, 7, 0, 1);
    rv = data_format_entry_create(unit, &global_lif_id_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for flood\n");
        return rv;
    }

    cud=global_lif_id_mc;
    BCM_GPORT_LOCAL_SET(dest_gport,recycle_port);
    rv = bcm_multicast_ingress_add(unit, mc_id, dest_gport, cud);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_add\n");
        return rv;
    }
    
    /*2. config for second loop*/
    rv = config_ingress_p2mp_local_ipmc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in config_ingress_p2mp_local_ipmc\n");
        return rv;
    }

    /*config for rcy port, configured by user*/
    rv = bcm_port_control_set(unit, recycle_port, bcmPortControlReserved281, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in config_ingress_p2mp_local_ipmc\n");
        return rv;
    }
    return rv;
}

bcm_gport_t src_sys_port_for_ptp = 1024; /* always = 1024 */

/*
 * ptp_tx_port: ptp tx port on CPU
 */
int config_ingress_ptp_field(int unit, bcm_gport_t ptp_tx_port)
{
    int rv;

    rv = bcm_stk_sysport_gport_set(unit, src_sys_port_for_ptp, ptp_tx_port);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_stk_sysport_gport_set\n");
      return rv;
    }

    return rv;
}


/*
 *====================================================================================================
 *------------------------------------Parser header-------------------------------------------------------
 *
 *
 *
 *====================================================================================================
 */

void parse_xtmh_header(uint8 *xtmh)
{
    uint8 tmp;
    uint16 tmp16;
    printf("=========================XTMH=======================\n");
    printf("xtmh.type(2):       %#x\n", (xtmh[0]&0xc0)>>6);
    printf("xtmh.P_T(2):        %#x\n", (xtmh[0]&0x30)>>4);
    tmp = (xtmh[0]&0xf)<<2 | (xtmh[1]&0xc0)>>6; 
    printf("xtmh.PktHeadLen(6): %#x\n", tmp);
    printf("xtmh.DP(3):         %#x\n", (xtmh[1]&0x38)>>3);
    printf("xtmh.RSV(3):        %#x\n", (xtmh[1]&0x07));
    tmp16 = xtmh[2];
    tmp16 = tmp16<<8 | xtmh[3];
    printf("xtmh.Flow_ID(16):   %#x\n", tmp16);
    printf("====================================================\n");
}

void parse_nph_header(uint8 *nph, int type)
{
    uint16 tmp16;
    uint32 tmp32;
    
    printf("==========================NPH=======================\n");
    printf("NPH.M/U(1):         %#x\n", (nph[0]&0x80)>>7);
    printf("NPH.Main_type(3):   %#x\n", (nph[0]&0x70)>>4);
    printf("NPH.Sub_type(4):    %#x\n", (nph[0]&0x0f));

    printf("NPH.MulPriority(2): %#x\n", (nph[1]&0xc0)>>6);
    printf("NPH.PTP(1):         %#x\n", (nph[1]&0x20)>>5);
    printf("NPH.CoS(3):         %#x\n", (nph[1]&0x1c)>>2);
    printf("NPH.Color(2):       %#x\n", (nph[1]&0x03));

    tmp16 = nph[2];
    tmp16 = tmp16<<8 | nph[3];
    printf("NPH.Slot_in(16):    %#x\n", tmp16);

    printf("NPH.VlanStatus(2):  %#x\n", (nph[4]&0xc0)>>6);
    printf("NPH.Del_Len(6):     %#x\n", (nph[4]&0x3f));

    printf("NPH.DFT_PRI(4):     %#x\n", (nph[5]&0xf0)>>4);
    printf("NPH.PS(3):          %#x\n", (nph[5]&0x0e)>>1);

    if (type == 3) {
        tmp32 = (nph[5]&0x01)<<16 | (nph[6]<<8) | nph[7];
        printf("NPH.MC_FP(17):      %#x\n", tmp32);
    } else {
        tmp32 = (nph[5]&0x01)<<16 | (nph[6]<<8) | nph[7];
        printf("NPH.Output_FP(17):  %#x\n", tmp32);
    }
    
    printf("NPH.R/L(1):         %#x\n", (nph[8]&0x80)>>7);
    printf("NPH.H/S(1):         %#x\n", (nph[8]&0x40)>>6);
    printf("NPH.Tag_num(2):     %#x\n", (nph[8]&0x30)>>4);

    if (type == 3) {
        tmp16 = (nph[8]&0x0f)<<4 | (nph[9]&0xf0)>>4;
        printf("NPH.RSV(8):         %#x\n", tmp16);        
        printf("NPH.Hash_value(4):  %#x\n", (nph[9]&0x0f));
    } else {
        printf("NPH.B_F(1):         %#x\n", (nph[8]&0x08)>>3);
        tmp16 = (nph[8]&0x07)<<8 | nph[9];
        printf("NPH.Port_Out(11):   %#x\n", tmp16);

    }
    printf("NPH.Learn_on(1):    %#x\n", (nph[10]&0x80)>>7);
    printf("NPH.Learn_stat(1):  %#x\n", (nph[10]&0x40)>>6);
    
    if (type == 3) {
        printf("RSV(1):     %#x\n", (nph[10]&0x20)>>5);
    } else {
        printf("NPH.MSP_OPO(1):     %#x\n", (nph[10]&0x20)>>5);
    }
    /*vpws*/
    if (type == 0) {        
        tmp16 = (nph[10]&0x1f)<<8 | nph[11];
        printf("NPH.Ulei_Port(13):  %#x\n", tmp16);
        
        printf("NPH.RSV(1):         %#x\n", (nph[12]&0x80)>>7);
        printf("NPH.Slot_Out(7):    %#x\n", (nph[12]&0x7f));
        printf("NPH.RSV(1):         %#x\n", (nph[13]&0x80)>>7);
        printf("NPH.DS_Mode(2):     %#x\n", (nph[13]&0x60)>>5);
        printf("NPH.RSV(5):         %#x\n", (nph[13]&0x1f));
        tmp16 = nph[14]<<8 | nph[15];
        printf("NPH.NH_inde(16):    %#x\n", tmp16);        
        
    } else if (type == 1) {/*vpls*/
        tmp16 = (nph[10]&0x1f)<<8 | nph[11];
        printf("NPH.VPN_ID(13):     %#x\n", tmp16);

        printf("NPH.RSV(1):         %#x\n", (nph[12]&0x80)>>7);
        printf("NPH.Slot_Out(7):    %#x\n", (nph[12]&0x7f));
        printf("NPH.RSV(1):         %#x\n", (nph[13]&0x80)>>7);
        printf("NPH.DS_Mode(2):     %#x\n", (nph[13]&0x60)>>5);
        printf("NPH.RSV(4):         %#x\n", (nph[13]&0x1e)>>1);

        tmp32 = (nph[13]&0x01)<<16 | nph[14]<<8 | nph[15];
        printf("NPH.Src_FP(17):     %#x\n", tmp32);
    } else if (type == 3) {/*vpls MC*/
        tmp16 = (nph[10]&0x1f)<<8 | nph[11];
        printf("NPH.VPN_ID(13):     %#x\n", tmp16);
        
        tmp16 = nph[12]<<1 | (nph[13]&0x80)>>7;
        printf("NPH.RSV(9):         %#x\n", tmp16);
        printf("NPH.DS(2):          %#x\n", (nph[13]&0x60)>>5);
        printf("NPH.MAC_pri(2):     %#x\n", (nph[13]&0x18)>>3);
        printf("NPH.IGMP(1):        %#x\n", (nph[13]&0x04)>>2);
        printf("NPH.R/M(1):         %#x\n", (nph[13]&0x02)>>1);
        tmp32 = (nph[13]&0x01)<<16 | nph[14]<<8 | nph[15];
        printf("NPH.Src_FP(17):     %#x\n", tmp32);
    }
}


/*
 * qualifier for pfq3 is needed
 */
struct tag_format_map_t{
    int tag_format;
    int tag_num;
};

tag_format_map_t tag_format_to_tag_num_mapping_table[] = {

    {0x0,                                                               0   },
    {0x10, /*one tpid0*/                                      1 },
    {0x8,   /*one tpid1*/                                      1    },
    {0x11, /*outer tpid1,inner tpid0*/                 2    },
    {0x9 , /*outer tpid0,inner tpid0*/                  2   },
    {0xa, /*outer tpid0,inner tpid1*/                   2   },
    {0x12,  /*outer tpid1,inner tpid1*/                2    },
};

uint32 pwe_type_identify_bit=0x00000400;
uint32 pwe_vlan_status_mask=0x00000300;
uint32 pwe_vlan_status_lsb= 8;
uint16 tag_format_mask = 0x1b;/*0b11011*/

int pwe_vlan_status_transfer_to_egress_init(int unit)
{
     bcm_field_qset_t  qset;
     bcm_field_aset_t  aset;
     bcm_field_group_t group;
    int pri=10;
    uint64 vsdata,vsmask;
    int rv;
    int tag_format,vlan_status;

    /*BIT 8-9 of extension data of pwe lif is vlan_status
      * Bit 10 identify the PWE lif  
      */
    COMPILER_64_SET(vsmask,0x00000000, pwe_type_identify_bit|pwe_vlan_status_mask);

 
 
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyLearnVlan);  
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInVPortWide);  
   
    rv = bcm_field_group_create(0, qset, pri, &group);
    if (rv != BCM_E_NONE){
        printf("bcm_field_group_create failed %d\n",rv);
    }    

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionVlanActionSetNew);

    rv = bcm_field_group_action_set(0, group, aset);
    if (rv != BCM_E_NONE){
        printf("bcm_field_group_action_set failed %d\n",rv);
    }    

    rv = bcm_field_group_install(0, group);
    if (rv != BCM_E_NONE){
        printf("bcm_field_group_install failed %d\n",rv);
    }    


    /*install the entry*/
    for(vlan_status = 0; vlan_status < 4;vlan_status++){
        for(tag_format= 0; tag_format < sizeof(tag_format_to_tag_num_mapping_table)/sizeof(tag_format_to_tag_num_mapping_table[0]);tag_format++){

            bcm_field_entry_t ent;

            bcm_field_entry_create(0, group, &ent);         
            if (rv != BCM_E_NONE){
                printf("bcm_field_entry_create failed %d\n",rv);
            }    
                    
            bcm_field_qualify_LearnVlan(0,ent,tag_format_to_tag_num_mapping_table[tag_format].tag_format,tag_format_mask);
            COMPILER_64_SET(vsdata,0x00000000, ((vlan_status << pwe_vlan_status_lsb)&pwe_vlan_status_mask)|pwe_type_identify_bit);

            bcm_field_qualify_InVPortWide(0,ent,vsdata,vsmask);
            bcm_field_action_add(0, ent, bcmFieldActionVlanActionSetNew, (0x3 <<4 ) |(tag_format_to_tag_num_mapping_table[tag_format].tag_num<< 2) |vlan_status , 0);        
            rv = bcm_field_entry_install(0, ent);
            if (rv != BCM_E_NONE){
                printf("bcm_field_entry_install failed %d\n",rv);
            }    

        }
    }
}

int pwe_vlan_status_config(int unit, bcm_gport_t gport, int vlan_status)
{
    uint64 data_w;
    uint32 lo32_data,hi32_data;
    int rv;

    rv = bcm_port_wide_data_get(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, &data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_get failed: $rv\n", rv);
        return rv;
    }
    hi32_data = COMPILER_64_HI(data_w);
    lo32_data = COMPILER_64_LO(data_w);
    lo32_data &= ~(pwe_vlan_status_mask);
    lo32_data |= ((vlan_status<<pwe_vlan_status_lsb) & pwe_vlan_status_mask) |pwe_type_identify_bit ;

    COMPILER_64_SET(data_w, hi32_data, lo32_data);
    rv = bcm_port_wide_data_set(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_set failed: $rv\n", rv);
        return rv;
    }

    return rv;

}



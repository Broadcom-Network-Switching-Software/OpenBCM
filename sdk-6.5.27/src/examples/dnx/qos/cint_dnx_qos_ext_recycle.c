/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
* 
* File: cint_dnx_qos_recycle.c
* Purpose: An example of the ext recycle qos model. 
*          Tecycle eedb entry can work in INITIAL model and UNIFORM model. 
*          If qos model is UNIFORM, then the inheritanced nwk_qos from previous stage will copy to rch header.
*          If qos model is INITIAL, then the PPH nwk_qos will copy to rch header.
*          The default qos model in recycle entry is INITIAL. 
*          This feauture is supported only for extension encap service. 
*
* Calling sequence:
*
* Set up sequence:
*  1. IP forward and egress mpls encap label 2000,then extention recycle
*  2. 2nd pass egress mpls encap label 1000 (swap).
*
* Traffic:
*  1.  IPoETH: 
          TPID 0x8100, VID 10, IPv4.TOS 3
*  2.  output packet :
*           initial model :   TPID 0x8100, VID 100, MPLS.LABEL=1000, MPLS.EXP=3
*           uniform model :   TPID 0x8100, VID 100, MPLS.LABEL=1000, MPLS.EXP=6
*
* To Activate Above Settings Run:
 * Test Scenario - start
      cint ../../../../src/examples/dnx/cint_dnx_qos_ext_recycle.c
      cint
      dnx_qos_ext_recycle_service(unit,in_Port,out_Port,is_pipe,mode);
 * Test Scenario - end
*/

int fec_id1;
int fec_id2;
int arp_id2;
int tunnel_id1;
int tunnel_id2;
int recycle_id;
int ac_gport;
uint32 rcy_field_context;
int rcy_presel_id;
int rcy_field_group_id;
bcm_field_entry_t entry_handle;

int dnx_qos_ext_recycle_main(int unit, int in_port, int out_port, int rcy_port, int qos_model)
{
    char error_msg[200]={0,};
    int in_vid = 10;
    bcm_mac_t my_mac={0};
    bcm_mac_t next_hop_mac={0};
    bcm_vlan_port_t vlan_port;
    bcm_l3_intf_t l3_intf;
    bcm_l3_ingress_t l3_ing_intf;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    bcm_l2_egress_t recycle_entry;
    bcm_gport_t fec_gport;
    bcm_gport_t tunnel_gport;
    int route = 0x7fffff03;
    int mask = 0xfffffff0;
    int udp_port =2000;
    uint32 rcy_port_profile = 3;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    /*create arp*/
    bcm_l3_egress_t_init(&l3eg);
    next_hop_mac[5]=0xaa;
    l3eg.mac_addr = next_hop_mac;
    l3eg.vlan = 100;
    l3eg.flags = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid), "in ARP");
    arp_id2 = l3eg.encap_id;

    /*create second MPLS*/
    bcm_mpls_egress_label_t_init(&label_array[0]);
    label_array[0].label = 1000;
    label_array[0].l3_intf_id = arp_id2;
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array), "in second pass tunnel");
    tunnel_id2 = label_array[0].tunnel_id;

    /*Create second pass FEC*/
    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = tunnel_id2;
    l3eg.encap_id = 0;
    l3eg.port = out_port;
    l3eg.flags = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &l3egid), "in fec");
    fec_id2 = l3egid;

    /*Craete RCH*/
    BCM_GPORT_FORWARD_PORT_SET(fec_gport, BCM_L3_ITF_VAL_GET(fec_id2));
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER |BCM_L2_EGRESS_DEST_PORT|BCM_L2_EGRESS_EXTENDED_COPY_DEST_ENCAP;
    recycle_entry.dest_port = fec_gport;
    recycle_entry.encap_id=0;
    recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    recycle_entry.egress_qos_model.egress_qos = qos_model;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_create(unit, &recycle_entry), "in recycle");
    recycle_id = recycle_entry.encap_id;

    /*first pass MPLS*/
    bcm_mpls_egress_label_t_init(&label_array[0]);
    label_array[0].label = 2000;
    label_array[0].l3_intf_id = recycle_id;
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
    label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    label_array[0].exp = 6;
    label_array[0].ttl = 32;
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array), "in first pass tunnel");
    tunnel_id1 = label_array[0].tunnel_id;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.match_vlan = in_vid;
    vlan_port.port = in_port;
    vlan_port.vsi = in_vid;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "in mpls php");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit,in_vid,in_port,0), "");
    ac_gport = vlan_port.vlan_port_id;

   /*In RIF*/
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = in_vid;
    my_mac[5] = 0x11;
    sal_memcpy(&l3_intf.l3a_mac_addr, my_mac,6);
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, l3_intf), "in qos ext recycle");
    bcm_l3_ingress_t_init(&l3_ing_intf);
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_get(unit, l3_intf.l3a_intf_id, &l3_ing_intf), "in qos ext recycle");
    l3_ing_intf.flags |= BCM_L3_INGRESS_WITH_ID;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, l3_ing_intf, &l3_intf.l3a_intf_id), "in qos ext recycle");

    /*first pass FEC*/
    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = tunnel_id1;
    l3eg.port = rcy_port;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &l3egid), "");
    fec_id1 = l3egid;
    /*
     * 7. Add Route entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, route, mask, 0, fec_id1), "");

    if (!ibch1_supported)
    {
        /*set recycle port property*/
        bcm_port_control_set(unit, rcy_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedEncap);
    }

    /*redirect packet to 2nd pass tunnel*/
    /**set recycle port profile*/
    bcm_port_class_set(unit, rcy_port, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, rcy_port_profile);
    /** set recycle traffic profile, to remove RCH*/
    bcm_port_class_set(unit, rcy_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4);
    
    bcm_field_context_t context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    void *dest_char;

    /********* Create PRESELECTOR *************/
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "qos recycle", sizeof(context_info.name));
    snprintf(error_msg, sizeof(error_msg), "context %d", context_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id), error_msg);
    rcy_field_context = context_id;

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = 60;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    presel_entry_data.qual_data[0].qual_arg = 0;  
    presel_entry_data.qual_data[0].qual_value = rcy_port_profile;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    snprintf(error_msg, sizeof(error_msg), "context %d", context_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data), error_msg);
    rcy_presel_id = presel_entry_id.presel_id;

    /** Init the fg_info structure. */
    bcm_field_group_t fg_id;
    bcm_field_entry_t entry_handle;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    
    dest_char = &(fg_info.name[0]);
    sal_strncpy(dest_char, "ENTRY_HIT", sizeof(fg_info.name));
    fg_info.name[sizeof(fg_info.name) - 1] = 0;
    
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 3;
    fg_info.qual_types[0] = bcmFieldQualifyMplsLabel;

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    fg_info.action_types[0] = bcmFieldActionOutVport0;
    fg_info.action_with_valid_bit[0] = TRUE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    fg_info.action_types[1] = bcmFieldActionRedirect;
    fg_info.action_with_valid_bit[1] = TRUE;
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    fg_info.action_types[2] = bcmFieldActionIngressDoNotLearn;
    fg_info.action_with_valid_bit[2] = TRUE;
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];

    /** Create the field group. */
    snprintf(error_msg, sizeof(error_msg), "on field_group_id = %d", fg_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &fg_id), error_msg);
    rcy_field_group_id = fg_id;

    /** Attach the created field group to the context. */
    snprintf(error_msg, sizeof(error_msg), "on field_group_id = %d, contex_id = %d", fg_id, context_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info), error_msg);

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.priority = 1;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    /*lable=2000, exp=6,ttl=32*/
    entry_info.entry_qual[0].value[0] = 0x7d0d20;
    entry_info.entry_qual[0].mask[0] = 0xFFFF;

    entry_info.entry_action[0].type = fg_info.action_types[0];
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(tunnel_gport,tunnel_id2);
    entry_info.entry_action[0].value[0] = tunnel_gport;

    entry_info.entry_action[1].type = fg_info.action_types[1];
    entry_info.entry_action[1].value[0] = out_port;

    entry_info.entry_action[2].type = fg_info.action_types[2];
    entry_info.entry_action[2].value[0] = 0;

    entry_info.core = BCM_CORE_ALL;

    /** Add an entry to the created field group. */
    snprintf(error_msg, sizeof(error_msg), "on field_group_id = %d", fg_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle), error_msg);
    return BCM_E_NONE;
}


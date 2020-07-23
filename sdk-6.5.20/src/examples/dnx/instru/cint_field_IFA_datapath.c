/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 /*
 * Configuration example start:
 *
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
 * cint ../../../src/examples/dnx/field/cint_field_IFA_datapath.c
 * cint;
 * int unit = 0;
 * cint_field_IFA_datapath_main(unit);
 *
 * Configuration example end
 *
 *  IFA 1.0 using datapath only
 */


bcm_field_group_t cint_field_IFA_datapath_ingress_fg_id=-1;
bcm_field_group_t cint_field_IFA_datapath_ingress_fg_id_2p_tsh=-1;
bcm_field_group_t cint_field_IFA_ipmf2_fg_id=-1;
bcm_field_group_t cint_field_IFA_datapath_ingress_fg_id_ipmf3=-1;
bcm_field_context_t cint_field_IFA_datapath_ingress_context_id_ipv4;
bcm_field_context_t cint_field_IFA_datapath_ingress_context_id_ipmf3;
bcm_field_presel_t cint_field_IFA_datapath_transit_presel_ipv4 = 6;
bcm_field_presel_t cint_field_IFA_datapath_ingress_presel_ipv4 = 7;
bcm_field_qualify_t cint_field_IFA_datapath_ingress_probe_qual;
bcm_field_qualify_t cint_field_IFA_datapath_ingress_2ndpass=0;
bcm_field_qualify_t cint_field_IFA_datapath_ingress_ptch1P=0;
bcm_field_qualify_t cint_field_IFA_datapath_ingress_ptch1PBase=0;
bcm_field_action_t cint_field_IFA_datapath_ingress_act_container_1b;
bcm_field_qualify_t cint_field_IFA_datapath_ingress_qual_container_1b;
bcm_field_action_t cint_field_IFA_datapath_ingress_act_IPTCommand_1b;
bcm_field_entry_t cint_field_ip_snoop_ent_id;
int priority_initiator = 2;
int priority_transit = 1;

uint32 cint_field_IFA_datapath_ingress_ace_entry_handle;
bcm_field_group_t cint_field_IFA_datapath_ingress_node_egress_fg_id;
bcm_field_context_t cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4;
bcm_field_presel_t cint_field_IFA_datapath_ingress_node_egress_presel_ipv4 = 4;
bcm_field_entry_t cint_field_IFA_datapath_ingress_node_egress_entry_handle;


/**
* \brief
*
* Egress parser always assume that FLOW extension present
* Hence need to set the valid bit for flow ext to TRUE
* Since PMF is the one to generate the Flow id we set the action to be flow_id=0 flow_profile=0 and flow_valid=1
* \param [in] unit        - Device id
* \param [in] context_id  - PMF context_id to add flow_id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
appl_dnx_field_flow_id_init(
    int unit,
    bcm_field_context_t context_id)
{
    int rv=0;
    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t flow_action = 0;
    bcm_field_action_info_t action_info_predefined;
    void *dest_char;

    rv = bcm_field_action_info_get
                    (unit, bcmFieldActionLatencyFlowId, bcmFieldStageIngressPMF1, &action_info_predefined);

    /** Create destination action to ignore destination qualifier which is 0*/
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionLatencyFlowId;
    action_info.prefix_size = action_info_predefined.size;
    /**Valid bit is the lsb*/
    action_info.prefix_value = 1;
    action_info.size = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "Flow_for_ifa_valid_bit", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &flow_action);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_actions = 1;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[0] = flow_action;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Flow_FG", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    /**Make the action to be lowest priority*/
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 0);

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);

    return rv;
}


/* Example of PMF Ingress Entry creation for IFA 1.0, to be used for for Initiator and transit nodes.
 * IFA 1.0 packets need to be trapped for transit node proccess.
 * Qualifier is based on probe_header_1 and probe_header_2 match.
 * The Actions:
 *  1. Set IPT profile as 3
 *  2. Set container to be used in PMF3
 *  3. Set Outlif for accessing metadata counter
 */
int cint_field_IFA_datapath_intermediate_ingress_entry_add(int unit, uint32 probe_1, uint32 probe_2, bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    int rv=0;

    /**Its better to add entries after attach, for batter bank allocation*/
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority_transit;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = cint_field_IFA_datapath_ingress_probe_qual;
    ent_info.entry_qual[0].value[0] = probe_2;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;
    ent_info.entry_qual[0].value[1] = probe_1;
    ent_info.entry_qual[0].mask[1] = 0xffffffff;

    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = bcmFieldActionIPTProfile;
    ent_info.entry_action[0].value[0] = 3;
    ent_info.entry_action[1].type = cint_field_IFA_datapath_ingress_act_container_1b;
    ent_info.entry_action[1].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_ingress_fg_id, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Probe_1:(0x%x) Probe_2:(0x%x) Tail_Profile (3) INT (1)\n",
            *entry_handle,
            ent_info.entry_qual[0].value[0],
            ent_info.entry_qual[0].value[1]);
    return 0;
}

/* Example of field configuration for IFA 1.0, to be used for for transit node.
 * iPMF1 traps packet by probe_headers, sets IPT profile, sets outlif for metada counter,
 * and uses container to flag iPMF3 to set
 * IPTCommand */
int cint_field_IFA_datapath_intermediate_ingress_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_get_t qual_info_get_offset;

    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=64;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_probe_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_probe_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 31;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    action_info.action_type = bcmFieldActionContainer;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_a_cont_1b", sizeof(action_info.name));
    rv = bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_IFA_datapath_ingress_probe_qual;
    /* Set actions */
    fg_info.nof_actions = 3;
    /* Tail Edit profile */
    fg_info.action_types[0] = bcmFieldActionIPTProfile;
    /* INT command*/
    fg_info.action_types[1] = cint_field_IFA_datapath_ingress_act_container_1b;
    /* Set Oam Lif for Counting */
    fg_info.action_types[2] = bcmFieldActionOutVport1Raw;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_datapath", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_IFA_datapath_ingress_fg_id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPv4", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_IFA_datapath_ingress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /* Add flow_id in field */
    rv = appl_dnx_field_flow_id_init(unit,cint_field_IFA_datapath_ingress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in appl_dnx_field_flow_id_init\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    /* Look for the probe header at Fwd+2 layer with no offset */
    attach_info.key_info.qual_info[0].input_arg = 2;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id, cint_field_IFA_datapath_ingress_context_id_ipv4, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n",cint_field_IFA_datapath_ingress_fg_id,cint_field_IFA_datapath_ingress_context_id_ipv4);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_ingress_context_id_ipv4;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_transit_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    /*iPMF3 Configuration*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_q_cont_1b", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_qual_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 9;
    action_info.stage = bcmFieldStageIngressPMF3;
    action_info.action_type = bcmFieldActionIPTCommand;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPT_c_1b", sizeof(action_info.name));
    bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_IPTCommand_1b);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_IFA_datapath_ingress_qual_container_1b;
    /* Set actions */
    fg_info.nof_actions = 1;
    /* INT command*/
    fg_info.action_types[0] = cint_field_IFA_datapath_ingress_act_IPTCommand_1b;
    fg_info.action_with_valid_bit[0]=FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_dp_IPT_c", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_IFA_datapath_ingress_fg_id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_iPMF3", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_field_IFA_datapath_ingress_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
    printf("Error (%d), in bcm_field_context_create\n", rv);
    return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    bcm_field_qualifier_info_get_t_init(& qual_info_get_offset);
    bcm_field_qualifier_info_get(unit,bcmFieldQualifyContainer,bcmFieldStageIngressPMF3,&qual_info_get_offset);
    attach_info.key_info.qual_info[0].offset = qual_info_get_offset.offset;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id_ipmf3, cint_field_IFA_datapath_ingress_context_id_ipmf3, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_ingress_context_id_ipmf3;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF3) context(%d) fwd_layer(Ipv4) \n",
    cint_field_IFA_datapath_transit_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    return 0;
}

/* Example of deleting the field configuration for IFA 1.0 transit node.*/
int field_attach_destroy(int unit)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_group_context_detach(unit, cint_field_IFA_datapath_ingress_fg_id, cint_field_IFA_datapath_ingress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach ipv4\n", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit,bcmFieldStageIngressPMF1,cint_field_IFA_datapath_ingress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy ipv4\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit,cint_field_IFA_datapath_ingress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete ipv4\n", rv);
        return rv;
    }

    return 0;
}


/**
*  Example to configure IFA Initiator node.
*    - This example configures IFA initiator node by IFA flow generation for IPv4 packets.
*
*  Operation principle:
*      The traffic is filtered (ETH packets with dest_mac=00:0c:00:02:00:00 in this example) for IFA, and requested packets are snooped.
*      While the original packet is forwarded in regular path, the snooped packet is encapsulated by IFA
*      header and IFA metadata (of the first node) in 2 cycles:
*
*
*  Configuration:
*    - Snooping configuration including setting the outlif for snooped copy.
*      This outlif is used to increment counter and stamp it on the IFA header.
*    - iPMF1 configuration
*        a. Set field group with 2 qualifiers and 4 actions
*           Qualifiers:
*             - ETH Destination address
*             - IFA Probe Header
*            Actions:
*             0- Set Container for iPMF3
*             1- Snoop Packet
*             2- Set Tail Edit Profile = 3
*             3- Set Outlif for Metadata counter access
*         b. Set 2 TCAM Entries:
*             - on destination mac hit use actions 1 and attach to FirstIFA contexts in egress
*             - on IFA Probe Header use actions 0,2,3 and attach to IFA contexts in egress
*    - iPMF3 configuration
*         a. Qualifier Container is set.
*            Actions: Set INT = 1
*    - ePMF configuration
*         a. Qualifier: Destination Port is Recycle
*            Action: Set Tail Edit Profile = 3
*
* Parameters:
*  unit        -  Device id
*  Outlif      -  Assigned Outlif for counting
*  recycle_port-  Port to be used for recyling
*/
int cint_field_group_const_example_IFA_gen(int unit,int Outlif,int recycle_port)
{
    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_context_t context_id = cint_field_IFA_datapath_ingress_context_id_ipv4;
    bcm_field_context_t ipmf1_context_id=0;
    bcm_field_action_t action_type = bcmFieldActionSnoop;
    char * name = "IFA_const_initiator";

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    bcm_field_entry_info_t ent_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_get_t qual_info_get_offset;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    bcm_field_context_param_info_t param_info;
    int sys_recycle_port;
    int outlif_encap_id;
    void *dest_char;
    int id;

    int rv = BCM_E_NONE;

    rv = bcm_field_action_info_get(unit,action_type,stage,&action_info_get_size);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get cint_const_action\n", rv);
        return rv;
    }

    bcm_gport_t snoop_trap_gport_id;
    int mirror_dest_id;
    bcm_port_config_t port_config;
    int system_recycle_port;

    BCM_GPORT_SYSTEM_PORT_ID_SET(system_recycle_port, recycle_port);

    /*
     * Configure snooping
     */

    outlif_encap_id = Outlif & 0x1fffff;

    rv = cint_field_ip_snoop_set(unit, system_recycle_port, 1, 1, outlif_encap_id, &snoop_trap_gport_id, &mirror_dest_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get cint_const_action\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=64;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_probe_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_probe_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Create a user defined action for iPMF3 (Setting INT)
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 31;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    action_info.action_type = bcmFieldActionContainer;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_a_cont_1b", sizeof(action_info.name));
    rv = bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;

    /* Constant FG : number of qualifiers in key must be 0 (empty key) */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = cint_field_IFA_datapath_ingress_probe_qual;
    fg_info.qual_types[1] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[2] = bcmFieldQualifyDstMac;

    /* Set actions */
    fg_info.nof_actions = 4;
    /* INT command*/
    fg_info.action_types[0] = cint_field_IFA_datapath_ingress_act_container_1b;
    fg_info.action_types[1] = bcmFieldActionSnoop;
    /* Tail Edit profile */
    fg_info.action_types[2] = bcmFieldActionIPTProfile;
    /* Outlif for Metadata Counter */
    fg_info.action_types[3] = bcmFieldActionOutVport1Raw;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id);

    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    id = cint_field_IFA_datapath_ingress_fg_id;
    printf("FG (%s) created ID: %d \n", name,id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    ipmf1_context_id = context_id;
    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set\n", rv);
        return rv;
    }

    /* Add flow_id in field */
    rv = appl_dnx_field_flow_id_init(unit,context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in appl_dnx_field_flow_id_init\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    /* Look for the probe header at layer 3 with no offset */
    attach_info.key_info.qual_info[0].input_arg = 3;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;

    /* Set Layer for Qualifier to Layer 0 = ETH */
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];
    attach_info.payload_info.action_types[3] = fg_info.action_types[3];
    attach_info.payload_info.action_types[4] = fg_info.action_types[4];
    attach_info.payload_info.action_types[5] = fg_info.action_types[5];

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("FG (%d) Attached to context: %d \n",id,context_id);

    /*
     * ---------------------------------------------------
     * Use 1st pass Egress timestamp instead of 2nd pass ingress pass.
     * Egress timestamp was passed in PTCH1plus header, and was put on UDH
     * here.
     * If UDH is non-zero, get the 1st pass egress TS from UDH
     * and use it as rx timestamp.
     * Else, use ingress timestamp.
     *
     * In case of second pass, iPMF2 takes the timestamp from
     * PTCH1plus-TS and puts it in UDH
 */
    if (*dnxc_data_get(unit,"instru","ifa","ingress_tod_feature",NULL))
    {

      bcm_field_qualifier_info_create_t_init(&qual_info);
      qual_info.size=1;
      dest_char = &(qual_info.name[0]);
      sal_strncpy_s(dest_char, "IFA_2pass", sizeof(qual_info.name));
      rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_2ndpass);
      if (rv != BCM_E_NONE)
      {
          printf("Error (%d), in bcm_field_qualifier_create\n", rv);
          return rv;
      }

      bcm_field_qualifier_info_create_t_init(&qual_info);
      qual_info.size=32;
      dest_char = &(qual_info.name[0]);
      sal_strncpy_s(dest_char, "IFA_ptch1P", sizeof(qual_info.name));
      rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_ptch1P);
      if (rv != BCM_E_NONE)
      {
          printf("Error (%d), in bcm_field_qualifier_create\n", rv);
          return rv;
      }

      bcm_field_qualifier_info_create_t_init(&qual_info);
      qual_info.size=2;
      dest_char = &(qual_info.name[0]);
      sal_strncpy_s(dest_char, "IFA_ptch1PBase", sizeof(qual_info.name));
      rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_ptch1PBase);
      if (rv != BCM_E_NONE)
      {
          printf("Error (%d), in bcm_field_qualifier_create\n", rv);
          return rv;
      }

      bcm_field_group_info_t_init(&fg_info);

      fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
      fg_info.stage = bcmFieldStageIngressPMF2;

      /* Set quals */
      fg_info.nof_quals = 3;
      fg_info.qual_types[0] = cint_field_IFA_datapath_ingress_2ndpass;
      fg_info.qual_types[1] = cint_field_IFA_datapath_ingress_ptch1P;
      fg_info.qual_types[2] = cint_field_IFA_datapath_ingress_ptch1PBase;
      /* Set actions */
      fg_info.nof_actions = 2;
      fg_info.action_types[0] = bcmFieldActionUDHData0;
      fg_info.action_types[1] = bcmFieldActionUDHBase0;
      fg_info.action_with_valid_bit[0]=TRUE;
      fg_info.action_with_valid_bit[1]=FALSE;

      dest_char = &(fg_info.name[0]);
      sal_strncpy_s(dest_char, "IFA_2P_TSH", sizeof(fg_info.name));
      rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id_2p_tsh);
      if (rv != BCM_E_NONE)
      {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
      }
      printf("Created DE field group (%d) in iPMF2 \n", cint_field_IFA_datapath_ingress_fg_id_2p_tsh);

      bcm_field_group_attach_info_t_init(&attach_info);

      attach_info.key_info.nof_quals = fg_info.nof_quals;
      attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
      attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
      attach_info.key_info.qual_types[2] = fg_info.qual_types[2];

      attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
      attach_info.key_info.qual_info[0].input_arg = cint_field_IFA_datapath_ingress_fg_id;

      rv = bcm_field_group_action_offset_get(unit, 0, cint_field_IFA_datapath_ingress_fg_id,cint_field_IFA_datapath_ingress_act_container_1b, &attach_info.key_info.qual_info[0].offset);

      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in bcm_field_group_action_offset_get\n", rv);
         return rv;
      }

      attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
      attach_info.key_info.qual_info[1].input_arg = 0;
      attach_info.key_info.qual_info[1].offset = -32;

      attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
      attach_info.key_info.qual_info[2].input_arg = 1;
      attach_info.key_info.qual_info[2].offset = 0;

      attach_info.payload_info.nof_actions = fg_info.nof_actions;
      attach_info.payload_info.action_types[0] = fg_info.action_types[0];
      attach_info.payload_info.action_types[1] = fg_info.action_types[1];

      rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id_2p_tsh, context_id, &attach_info);
      if (rv != BCM_E_NONE)
      {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
      }
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_ingress_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    /* For initiator fwd layer is 0 so we need to look at layer+1 to look for ipv4 */

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_ingress_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 1;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_ingress_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);


    /**Its batter to add entries after attach, for batter bank allocation*/
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority_initiator;

    ent_info.nof_entry_quals = 2;
    ent_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
    ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeIp4;
    ent_info.entry_qual[0].mask[0] = 0x1F;
    ent_info.entry_qual[1].type = bcmFieldQualifyDstMac;
    ent_info.entry_qual[1].value[0] = 0x00020000;
    ent_info.entry_qual[1].value[1] = 0x000c;
    ent_info.entry_qual[1].mask[0] = -1;
    ent_info.entry_qual[1].mask[1] = -1;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionSnoop;
    ent_info.entry_action[0].value[0] = snoop_trap_gport_id;

    rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_ingress_fg_id, &ent_info, &cint_field_ip_snoop_ent_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Tail_Profile (3) \n", cint_field_ip_snoop_ent_id);

    id = cint_field_IFA_ipmf2_fg_id;
    printf("FG (%s) created ID: %d \n", name,id);

    /*iPMF3 Configuration*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_q_cont_1b", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_qual_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 9;
    action_info.stage = bcmFieldStageIngressPMF3;
    action_info.action_type = bcmFieldActionIPTCommand;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPT_c_1b", sizeof(action_info.name));
    rv = bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_IPTCommand_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_IFA_datapath_ingress_qual_container_1b;

    /* Set actions */
    fg_info.nof_actions = 1;
    /* INT command*/
    fg_info.action_types[0] = cint_field_IFA_datapath_ingress_act_IPTCommand_1b;
    fg_info.action_with_valid_bit[0]=FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_dp_IPT_c", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_IFA_datapath_ingress_fg_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created DE field group (%d) in iPMF3 \n", cint_field_IFA_datapath_ingress_fg_id_ipmf3);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_iPMF3", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_field_IFA_datapath_ingress_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
    printf("Error (%d), in bcm_field_context_create\n", rv);
    return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    bcm_field_qualifier_info_get_t_init(& qual_info_get_offset);
    bcm_field_qualifier_info_get(unit,bcmFieldQualifyContainer,bcmFieldStageIngressPMF3,&qual_info_get_offset);
    attach_info.key_info.qual_info[0].offset = qual_info_get_offset.offset;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id_ipmf3, cint_field_IFA_datapath_ingress_context_id_ipmf3, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_ingress_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_ingress_context_id_ipmf3;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = ipmf1_context_id;
    p_data.qual_data[0].qual_mask = 0x1f;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF3) context(%d) iPMF1_ctx(%d) \n",
    cint_field_IFA_datapath_ingress_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipmf3,ipmf1_context_id);

    /* *********************** */
    /* ePMF for Initiator Node */
    /* *********************** */

     /*
      * Configure the ACE format.
      */
      bcm_field_ace_format_info_t_init(&ace_format_info);

      ace_format_info.nof_actions = 1;

      ace_format_info.action_types[0] = bcmFieldActionIPTProfile;

      dest_char = &(ace_format_info.name[0]);
      sal_strncpy_s(dest_char, "AM_ACE_INIT", sizeof(ace_format_info.name));
      rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in bcm_field_ace_format_add \n", rv);
         return rv;
      }

      bcm_field_ace_entry_info_t_init(&ace_entry_info);

      ace_entry_info.nof_entry_actions = 1;
      ace_entry_info.entry_action[0].type = bcmFieldActionIPTProfile;
      ace_entry_info.entry_action[0].value[0] = 3;

      rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &cint_field_IFA_datapath_ingress_ace_entry_handle);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in bcm_field_ace_entry_add \n", rv);
         return rv;
      }
      printf("ACE Format (%d) and ACE entry 0x(%x) created \n", ace_format_id,cint_field_IFA_datapath_ingress_ace_entry_handle);

      bcm_field_group_info_t_init(&fg_info);
      fg_info.fg_type = bcmFieldGroupTypeTcam;
      fg_info.stage = bcmFieldStageEgress;

      /* Set quals */
      fg_info.nof_quals = 1;
      fg_info.qual_types[0] = bcmFieldQualifyDstPort;
      /* Set actions */
      fg_info.nof_actions = 1;
      fg_info.action_types[0] = bcmFieldActionAceEntryId;

      dest_char = &(fg_info.name[0]);
      sal_strncpy_s(dest_char, "IFA_datapath_e_init", sizeof(fg_info.name));
      rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_node_egress_fg_id);
      if (rv != BCM_E_NONE)
      {
          printf("Error (%d), in bcm_field_group_add\n", rv);
          return rv;
      }
      printf("Created TCAM field group (%d) in ePMF \n", cint_field_IFA_datapath_ingress_node_egress_fg_id);

      /**Attach  context**/
      bcm_field_group_attach_info_t_init(&attach_info);
      bcm_field_context_info_t_init(&context_info);

      dest_char = &(context_info.name[0]);
      sal_strncpy_s(dest_char, "IFA_eg_init_IPv4", sizeof(context_info.name));
      rv = bcm_field_context_create(unit, 0, fg_info.stage, &context_info, &cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4);
      if (rv != BCM_E_NONE)
      {
          printf("Error (%d), in bcm_field_context_create\n", rv);
          return rv;
      }

      attach_info.key_info.nof_quals = fg_info.nof_quals;
      attach_info.payload_info.nof_actions = fg_info.nof_actions;
      attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
      attach_info.payload_info.action_types[0] = fg_info.action_types[0];

      attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

      rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_node_egress_fg_id, cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4, &attach_info);

      if (rv != BCM_E_NONE)
      {
          printf("Error (%d), in bcm_field_group_context_attach\n", rv);
          return rv;
      }

      printf("Attached  FG (%d) to context (%d)\n",cint_field_IFA_datapath_ingress_node_egress_fg_id,cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4);

      bcm_field_entry_info_t_init(&ent_info);
      ent_info.priority = priority_initiator;

      ent_info.nof_entry_quals = fg_info.nof_quals;
      ent_info.entry_qual[0].type = fg_info.qual_types[0];

      /**The value should be encoded as GPORT_SYTEM_PORT*/
      BCM_GPORT_SYSTEM_PORT_ID_SET(sys_recycle_port,recycle_port);
      ent_info.entry_qual[0].value[0] = sys_recycle_port;
      ent_info.entry_qual[0].mask[0] = -1;

      ent_info.nof_entry_actions = fg_info.nof_actions;
      ent_info.entry_action[0].type = fg_info.action_types[0];
      ent_info.entry_action[0].value[0] = cint_field_IFA_datapath_ingress_ace_entry_handle;

      rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_ingress_node_egress_fg_id, &ent_info, &cint_field_IFA_datapath_ingress_node_egress_entry_handle);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in bcm_field_entry_add\n", rv);
         return rv;
      }
      printf("FG ID: (%d) Entry add: id:(0x%x)  Ace_Entry_id (0x%x) \n",cint_field_IFA_datapath_ingress_node_egress_fg_id,
          cint_field_IFA_datapath_ingress_node_egress_entry_handle,cint_field_IFA_datapath_ingress_ace_entry_handle);


      bcm_field_presel_entry_id_info_init(&p_id);
      bcm_field_presel_entry_data_info_init(&p_data);

      p_id.presel_id = cint_field_IFA_datapath_ingress_node_egress_presel_ipv4;
      p_id.stage = fg_info.stage;
      p_data.entry_valid = TRUE;
      p_data.context_id = cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4;

      p_data.nof_qualifiers = 1;

      p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
      p_data.qual_data[0].qual_arg = 0;
      p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
      p_data.qual_data[0].qual_mask = 0x1F;


      rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in dnx_field_presel_set \n", rv);
         return rv;
      }

      printf("Presel (%d) was configured for stage(ePMF) context(%d) fwd_layer(Ipv4) \n",
              cint_field_IFA_datapath_ingress_node_egress_presel_ipv4,cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4);

    return rv;
}

/* $Id: cint_ipfix_field.c
 * This file includes functions for configuring field for IPFIX.
 * Main functions:
 * cint_ipfix_field_main - Main field configuration. Configures iPMF1 field group to trap packets for IPFIX,
 *                         and iPMF3 to set UDH with source and destination ports.
 * cint_ipfix_field_export_packet - Main function to trap IPFIX export packets coming from the eventor.
 * cint_ipfix_mirror_create - Function to configure mirroring, to create ipfix metadata packet.
 * cint_ipfix_field_egress - Function to create ePMF configuration to enable correct egress context selection for IPFIX
*/


bcm_field_group_t   cint_ipfix_field_fg_id = 0;
int cint_ipfix_field_range_id = 1;
bcm_field_qualify_t cint_ipfix_field_qual_id = 0;

bcm_field_qualify_t cint_ipfix_field_utils_sn_qualifier_id = BCM_FIELD_INVALID;
bcm_field_group_t cint_ipfix_tx_field_fg_id = BCM_FIELD_INVALID;
bcm_field_entry_t cint_ipfix_field_utils_entry_id = BCM_FIELD_INVALID;
bcm_field_qualify_t cint_field_IPFIX_egress_snooped_qual = BCM_FIELD_INVALID;;
bcm_field_group_t cint_field_IPFIX_egress_fg_id = BCM_FIELD_INVALID;
bcm_field_context_t cint_field_IPFIX_egress_context_id_ipv4;
bcm_field_presel_t cint_field_IPFIX_egress_presel_ipv4 = 6;
uint32 cint_field_IPFIX_egress_ace_entry_handle;
bcm_field_action_t cint_field_IPFIX_rx_act_container_1b;
bcm_field_group_t cint_field_IPFIX_fg_id_ipmf3=-1;
bcm_field_context_t cint_field_IPFIX_context_id_ipmf3;
bcm_field_presel_t cint_field_IPFIX_presel = 6;
bcm_field_qualify_t cint_field_Valid_bit_src_port;
bcm_field_qualify_t cint_field_Valid_bit_dest_port;
bcm_field_qualify_t cint_field_Valid_bit_udh_base_type;
bcm_field_qualify_t cint_field_udh_base;
bcm_field_context_t cint_field_IPFIX_rx_iPMF1;

/* 
* FG entry for preventing snooping the IPFIX export packet coming from eventor port
* It's priority must be higher then the snoop priority!
*/
uint32 field_eventor_port_entry_priority = 199; 
uint32 field_snoop_entry_priority = 200;

/*
 *  Encap estimation requirement indication. Should be updated in cint_ipfix_field_egress
 * for devices that require encap estimation.
 */
int estimate_encap_size = 0;

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

/**
* \brief
*  Configure TCAM FG with relevant qualifier/actions for trapping packets for IPFIX flow.
*  iPMF1 traps packet by 5-tupple, sets snooped packet as statistical sampling and sets Container for iPMF3
*  iPMF3 When container is set (from iPMF1), Sets UDH with original source and destination ports
* \param [in] unit        -  Device id
* \param [in] context_id  -  Context to attach the FG to
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_ipfix_field_main(int unit, bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_range_info_t range_info;
    bcm_field_qualifier_info_get_t qual_info_get;
    int layer_record_type_offset;
    void *dest_char;
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_field_action_info_t action_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_get_t qual_info_get_offset;
    bcm_field_qualifier_info_get_t qualifier_info_get;
    bcm_field_action_t cint_src_port_action_id;
    bcm_field_action_t cint_dest_port_action_id;
    int container_offset;
    int device_type_v2;

    proc_name = "cint_ipfix_field_main";

    device_type_v2 = *(dnxc_data_get(unit,  "instru", "ipfix", "ipfix_v2", NULL));

    /**
    * Init the fg_info structure.
    */

    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 31;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    action_info.action_type = bcmFieldActionContainer;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IPFIX_a_cont_1b", sizeof(action_info.name));
    rv = bcm_field_action_create(unit,0,&action_info,&cint_field_IPFIX_rx_act_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypePacketHeaderSize;
    range_info.min_val = 0;
    range_info.max_val = 144;
    /**
    * Set the range.
    */
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, cint_ipfix_field_range_id, &range_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_field_range_set\n", proc_name, rv);
       return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Fwd_plus_1_layer_t", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,qual_info, &cint_ipfix_field_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s, Error (%d), in bcm_field_qualifier_create \n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordType, bcmFieldStageIngressPMF1, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_qualifier_info_get \n", proc_name, rv);
        return rv;
    }
    layer_record_type_offset = qual_info_get.offset;

    /********************
     * Create and attach TCAM group in iPMF1
     * ******************/
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[1] = cint_ipfix_field_qual_id;
    fg_info.qual_types[2] = bcmFieldQualifyPacketLengthRangeCheck;
    fg_info.qual_types[3] = bcmFieldQualifyInPort; /* qualifier for preventing snooping of IPFIX export packet coming from Eventor port !*/

    /* Set actions */
    /* Action 0 - Set Snooped packet as Staistical Sampling 
     * Action 1 - Set container to be trapped by iPMF3, where UDH is set with src and dst ports 
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionStatSampling;
    fg_info.action_types[1] = cint_field_IPFIX_rx_act_container_1b;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ipfix_rx_fg", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_ipfix_field_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    printf("%s: FG Id (%d) was add to iPMF1 Stage\n", proc_name, cint_ipfix_field_fg_id);

    /**Create iPMF1 context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IPFIX_RX_iPMF1", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, fg_info.stage, &context_info, &cint_field_IPFIX_rx_iPMF1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    if(device_type_v2 == 1)
    {
        /* Add flow_id in field */
        rv = appl_dnx_field_flow_id_init(unit,cint_field_IPFIX_rx_iPMF1);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in appl_dnx_field_flow_id_init\n", rv);
            return rv;
        }
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = 15;
    p_id.stage = fg_info.stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IPFIX_rx_iPMF1;

    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1f;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg = -1;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = layer_record_type_offset;

    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;

    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    rv = bcm_field_group_context_attach(unit, 0, cint_ipfix_field_fg_id, cint_field_IPFIX_rx_iPMF1, &attach_info);

    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
    printf("%s: FG Id (%d) in iPMF1 attached to context (%d) \n", proc_name, cint_ipfix_field_fg_id, cint_field_IPFIX_rx_iPMF1);

    if(device_type_v2 == 0)
    {
        /*iPMF3 Configuration*/

        /*
         * Since destination of the snooped packet is eventor, we copy
         * the original source and destination ports to UDH.
         */
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size=1;
        dest_char = &(qual_info.name[0]);
        sal_strncpy_s(dest_char, "ipfix_Valid_bit_src_port", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_Valid_bit_src_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_create\n", rv);
            return rv;
        }

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size=1;
        dest_char = &(qual_info.name[0]);
        sal_strncpy_s(dest_char, "ipfix_Valid_bit_dest_port", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_Valid_bit_dest_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_create\n", rv);
            return rv;
        }

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size=1;
        dest_char = &(qual_info.name[0]);
        sal_strncpy_s(dest_char, "ipfix_Valid_bit_udh_base_type", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_Valid_bit_udh_base_type);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_create\n", rv);
            return rv;
        }

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size=8;
        dest_char = &(qual_info.name[0]);
        sal_strncpy_s(dest_char, "ipfix_udh_base", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_udh_base);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_create\n", rv);
            return rv;
        }

        rv = bcm_field_qualifier_info_get(unit,bcmFieldQualifySrcPort,bcmFieldStageIngressPMF3,qualifier_info_get);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
            return rv;
        }
   
        bcm_field_action_info_t_init(&action_info);
        action_info.action_type = bcmFieldActionUDHData0;
        action_info.stage = bcmFieldStageIngressPMF3;
        action_info.prefix_size = 32 - qualifier_info_get.size;
        action_info.prefix_value = 0;
        action_info.size = qualifier_info_get.size;
        dest_char = &(action_info.name[0]);
        sal_strncpy_s(dest_char, "src_port_action", sizeof(action_info.name));
        rv = bcm_field_action_create(unit, 0, &action_info, &cint_src_port_action_id);
        if (rv != BCM_E_NONE)
        {
           printf("%s: Error (%d), in bcm_field_action_create\n", proc_name, rv);
           return rv;
        }

        rv = bcm_field_qualifier_info_get(unit,bcmFieldQualifyDstPort,bcmFieldStageIngressPMF3,qualifier_info_get);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_add\n", rv);
            return rv;
        }
   
        bcm_field_action_info_t_init(&action_info);
        action_info.action_type = bcmFieldActionUDHData1;
        action_info.stage = bcmFieldStageIngressPMF3;
        action_info.prefix_size = 32 - qualifier_info_get.size;
        action_info.prefix_value = 0;
        action_info.size = qualifier_info_get.size;
        dest_char = &(action_info.name[0]);
        sal_strncpy_s(dest_char, "dest_port_action", sizeof(action_info.name));
        rv = bcm_field_action_create(unit, 0, &action_info, &cint_dest_port_action_id);
        if (rv != BCM_E_NONE)
        {
           printf("%s: Error (%d), in bcm_field_action_create\n", proc_name, rv);
           return rv;
        }

        bcm_field_group_info_t_init(&fg_info);
        fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
        fg_info.stage = bcmFieldStageIngressPMF3;

        /* Set quals */
        fg_info.nof_quals = 6;
        fg_info.qual_types[0] = cint_field_Valid_bit_src_port;
        fg_info.qual_types[1] = bcmFieldQualifySrcPort;
        fg_info.qual_types[2] = cint_field_Valid_bit_dest_port;
        fg_info.qual_types[3] = bcmFieldQualifyDstPort;
        fg_info.qual_types[4] = cint_field_Valid_bit_udh_base_type;
        fg_info.qual_types[5] = cint_field_udh_base;
        fg_info.nof_actions = 3;
        fg_info.action_types[0] = cint_src_port_action_id;
        fg_info.action_types[1] = cint_dest_port_action_id;
        fg_info.action_types[2] = bcmFieldActionUDHBase;
        fg_info.action_with_valid_bit[0]=TRUE;
        fg_info.action_with_valid_bit[1]=TRUE;
        fg_info.action_with_valid_bit[2]=TRUE;

        dest_char = &(fg_info.name[0]);
        sal_strncpy_s(dest_char, "IPFIX_rx_ipmf3_c", sizeof(fg_info.name));
        rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_IPFIX_fg_id_ipmf3);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_add\n", rv);
            return rv;
        }
        printf("Created TCAM field group (%d) in iPMF3 \n", cint_field_IPFIX_fg_id_ipmf3);

        /**Attach  context**/
        bcm_field_group_attach_info_t_init(&attach_info);
        bcm_field_context_info_t_init(&context_info);

        dest_char = &(context_info.name[0]);
        sal_strncpy_s(dest_char, "IFA_iPMF3", sizeof(context_info.name));
        rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_field_IPFIX_context_id_ipmf3);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_context_create\n", rv);
            return rv;
        }

        rv = bcm_field_qualifier_info_get(unit,bcmFieldQualifyContainer,bcmFieldStageIngressPMF3,qualifier_info_get);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
            return rv;
        }
        container_offset = qualifier_info_get.offset;

        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        attach_info.key_info.qual_types[0] = cint_field_Valid_bit_src_port;
        attach_info.key_info.qual_types[1] = bcmFieldQualifySrcPort;
        attach_info.key_info.qual_types[2] = cint_field_Valid_bit_dest_port;
        attach_info.key_info.qual_types[3] = bcmFieldQualifyDstPort;
        attach_info.key_info.qual_types[4] = cint_field_Valid_bit_udh_base_type;
        attach_info.key_info.qual_types[5] = cint_field_udh_base;

        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeConst;

        attach_info.key_info.qual_info[0].offset = container_offset;
        attach_info.key_info.qual_info[2].offset = container_offset;
        attach_info.key_info.qual_info[4].offset = container_offset;

        attach_info.key_info.qual_info[5].input_arg = 5;

        attach_info.payload_info.action_types[0] = fg_info.action_types[0];
        attach_info.payload_info.action_types[1] = fg_info.action_types[1];
        attach_info.payload_info.action_types[2] = fg_info.action_types[2];

        rv = bcm_field_group_context_attach(unit, 0, cint_field_IPFIX_fg_id_ipmf3, cint_field_IPFIX_context_id_ipmf3, &attach_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_context_attach\n", rv);
            return rv;
        }

        bcm_field_presel_entry_id_info_init(&p_id);
        bcm_field_presel_entry_data_info_init(&p_data);

        p_id.presel_id = cint_field_IPFIX_presel;
        p_id.stage = bcmFieldStageIngressPMF3;
        p_data.entry_valid = TRUE;
        p_data.context_id = cint_field_IPFIX_context_id_ipmf3;
        p_data.nof_qualifiers = 1;

        p_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
        p_data.qual_data[0].qual_value = cint_field_IPFIX_rx_iPMF1;
        p_data.qual_data[0].qual_mask = 0x3F;

        rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in dnx_field_presel_set \n", rv);
            return rv;
        }

        printf("Presel (%d) was configured for stage(iPMF3) context(%d) fwd_layer(Ipv4) \n",cint_field_IPFIX_presel,cint_field_IPFIX_context_id_ipmf3);
    } /* iPMF3 */
    return rv;
}

/**
* \brief
*  Adds TCAM entry to FG of iPMF1
* \param [in] unit        -  Device id
* \param [in] fwd_layer        -  Forwarding layer type
* \param [in] fwd_layer_plus_one        -  One layer after forwarding type
* \param [in] is_in_range - if set to true then packet length should be in the set range
* \param [in] gport_stat_samp        -  Chosen stat_sample profile action for the set of qualifiers
* \param [out] entry_handle        -  entry Id (used to manage entry)
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_ipfix_field_entry_add(int unit,bcm_field_layer_type_t fwd_layer,
                                            bcm_field_layer_type_t fwd_layer_plus_one,
                                                int is_in_range,
                                                uint32 gport_stat_samp,
                                                bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    uint32 fwd_l = fwd_layer;
    uint32 fwd_l_plus = fwd_layer_plus_one;
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "cint_ipfix_field_entry_add";

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = field_snoop_entry_priority;

    ent_info.nof_entry_quals = 3;
    ent_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
    ent_info.entry_qual[0].value[0] = fwd_layer;
    ent_info.entry_qual[0].mask[0] = 0x1F;

    ent_info.entry_qual[1].type = cint_ipfix_field_qual_id;
    ent_info.entry_qual[1].value[0] = fwd_layer_plus_one;
    ent_info.entry_qual[1].mask[0] = 0x0;

    ent_info.entry_qual[2].type = bcmFieldQualifyPacketLengthRangeCheck;
    ent_info.entry_qual[2].value[0] = cint_ipfix_field_range_id;
    if(is_in_range == TRUE)
    {
        ent_info.entry_qual[2].mask[0] = 0x3;
    }
    else
    {
        ent_info.entry_qual[2].mask[0] = 0x0;
    }

    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = bcmFieldActionStatSampling;
    ent_info.entry_action[0].value[0] = gport_stat_samp;
    ent_info.entry_action[0].value[1] = 5;
    ent_info.entry_action[1].type = cint_field_IPFIX_rx_act_container_1b;
    ent_info.entry_action[1].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, cint_ipfix_field_fg_id, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_field_entry_add\n", proc_name, rv);
       return rv;
    }
    printf("%s: TCAM Entry Id (0x%x) Fwd_layer (%d) fwd_plus_one (%d) is_in_range (%d) with Stat_Code (0x%x) \n",
                            proc_name, *entry_handle, fwd_l,fwd_l_plus,is_in_range,gport_stat_samp);

    return rv;
}

/**
* \brief
*  Adds TCAM entry to FG of iPMF1
*  Prevents the IPFIX export packet from snooping
* \param [in] unit         -  Device id
* \param [in] eventor_port  -  Eventor Port
* \param [out] entry_handle -  entry Id (used to manage entry)
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_ipfix_field_entry_add_eventor_port(int unit, int eventor_port, int fg_id, bcm_field_entry_t *entry_handle)
{
   bcm_field_entry_info_t ent_info;
   int rv = BCM_E_NONE;
   char *proc_name;

   proc_name = "cint_ipfix_field_entry_add_eventor_port";

   bcm_field_entry_info_t_init(&ent_info);
   ent_info.priority = field_eventor_port_entry_priority;

   ent_info.nof_entry_quals = 1;
   ent_info.entry_qual[0].type = bcmFieldQualifyInPort;
   ent_info.entry_qual[0].value[0] = eventor_port;
   ent_info.entry_qual[0].mask[0] = -1;

   /* action = DO NOTHING - that is skip the snooping since the priority of snopping is lower */
   ent_info.nof_entry_actions = 0;

   rv = bcm_field_entry_add(unit, 0, fg_id, &ent_info, entry_handle);
   if (rv != BCM_E_NONE)
   {
       printf("%s: Error (%d), in bcm_field_entry_add\n", proc_name, rv);
       return rv;
   }

   printf("%s: TCAM Entry Id (0x%x) with NO ACTION is added (for port = %d) per fg_id= %d \n", proc_name, *entry_handle, eventor_port, fg_id);

   return rv;
}

/**
* \brief
*  Adds TCAM entry to FG of iPMF1
*  Adds field group to trap the IPFIX export packet according to in-port
*  and sets counter parameters for sequence number stamping
* \param [in] unit         -  Device id
* \param [in] eventor_port -  Eventor Port (used as in-port of the IPFIX export packet)
* \param [in] context_id -  field context Id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_ipfix_field_export_quals_set(int unit, int eventor_port, int context_id)
{
   int rv = BCM_E_NONE;
   bcm_field_stage_t bcm_stage = bcmFieldStageIngressPMF1;
   bcm_field_group_info_t field_group_info;
   bcm_field_group_attach_info_t attach_info;
   bcm_field_qualifier_info_create_t qual_info;
   bcm_field_qualifier_info_get_t qual_info_get;
   void *dest_char;
   char *proc_name;

   proc_name = "cint_ipfix_field_export_quals_set";

   /* Create user defined qualifier */
   rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordType, bcm_stage, &qual_info_get);
   if (rv != BCM_E_NONE)
   {
     printf("%s: Error (%d), in bcm_dnx_field_qualifier_info_get for bcmFieldQualifyLayerRecordType\n", proc_name, rv);
     return rv;
   }

   bcm_field_qualifier_info_create_t_init(&qual_info);
   qual_info.size = qual_info_get.size;
   dest_char = &(qual_info.name[0]);
   sal_strncpy_s(dest_char, "IPFIX-Export-qual", sizeof(qual_info.name));
   rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_ipfix_field_utils_sn_qualifier_id);
   if (rv != BCM_E_NONE)
   {
      printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
      return rv;
   }
   bcm_field_group_info_t_init(&field_group_info);
   field_group_info.fg_type = bcmFieldGroupTypeTcam;
   field_group_info.stage = bcm_stage;

   /* Set quals:
    * - in-port is eventor port 
    * - forwarding layer is IP 
    * - forwarding layer +1 is UDP 
    * - UDP destination source is 4739 (IPFIX) 
    */
   field_group_info.nof_quals = 4;
   field_group_info.qual_types[0] = bcmFieldQualifyInPort;
   field_group_info.qual_types[1] = bcmFieldQualifyLayerRecordType;
   field_group_info.qual_types[2] = cint_ipfix_field_utils_sn_qualifier_id; /* user defined qualifier*/
   field_group_info.qual_types[3] = bcmFieldQualifyL4DstPort;

   /* Set actions:
    * - update Application Specific - OAM - OAM_Sub_Type 
    * - update counter ID
    *
    * Note:
    * See set_counter_resource
    * For couner_if 0,1,2:
    * Ingress command_id are 7,8,9
    * Egress command_id are 0,1,2
    */
   field_group_info.nof_actions = 4;
   field_group_info.action_types[0] = bcmFieldActionStatId0 + 7;  /* for ingress OAM - use 7*/
   field_group_info.action_types[1] = bcmFieldActionStatProfile0 + 7; /* for ingress OAM - use 7*/
   field_group_info.action_types[2] = bcmFieldActionOamRaw;
   field_group_info.action_types[3] = bcmFieldActionStatOamLM;

   dest_char = &(field_group_info.name[0]);
   sal_strncpy_s(dest_char, "ipfix_tx_fg", sizeof(field_group_info.name));

   rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &field_group_info, &cint_ipfix_tx_field_fg_id);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in bcm_field_group_add\n", proc_name, rv);
      return rv;
   }
   printf("%s: FG Id (%d) was add to iPMF1 Stage\n", proc_name, cint_ipfix_tx_field_fg_id);

   bcm_field_group_attach_info_t_init(&attach_info);
   attach_info.key_info.nof_quals = field_group_info.nof_quals;
   attach_info.payload_info.nof_actions = field_group_info.nof_actions;
   attach_info.payload_info.action_types[0] = field_group_info.action_types[0];
   attach_info.payload_info.action_types[1] = field_group_info.action_types[1];
   attach_info.payload_info.action_types[2] = field_group_info.action_types[2];
   attach_info.payload_info.action_types[3] = field_group_info.action_types[3];
   
   attach_info.key_info.qual_types[0] = field_group_info.qual_types[0];
   attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

   attach_info.key_info.qual_types[1] = field_group_info.qual_types[1];
   attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
   attach_info.key_info.qual_info[1].input_arg = 0;
   attach_info.key_info.qual_info[1].offset = 0;
    
   attach_info.key_info.qual_types[2] = field_group_info.qual_types[2];
   attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerRecordsFwd;
   attach_info.key_info.qual_info[2].input_arg = 1;
   attach_info.key_info.qual_info[2].offset = qual_info_get.offset;

   attach_info.key_info.qual_types[3] = field_group_info.qual_types[3];
   attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerFwd;
   attach_info.key_info.qual_info[3].input_arg = 1;
   attach_info.key_info.qual_info[3].offset = 0;

   rv = bcm_field_group_context_attach(unit, 0, cint_ipfix_tx_field_fg_id, cint_field_IPFIX_rx_iPMF1, &attach_info);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
      return rv;
   }

   printf("%s: FG Id (%d) in iPMF1 attached to context (%d)\n", proc_name, cint_ipfix_tx_field_fg_id, cint_field_IPFIX_rx_iPMF1);

   return rv; 
}

int
cint_ipfix_field_export_entry_add(int unit, int eventor_port)
{
   int rv = BCM_E_NONE;
   char *proc_name;
   int device_type_v2;

   bcm_field_entry_info_t ent_info;
   uint32 bcm_value[BCM_FIELD_QUAL_WIDTH_IN_WORDS];
   uint32 hw_value[BCM_FIELD_QUAL_WIDTH_IN_WORDS];

   proc_name = "cint_ipfix_field_export_entry_add";

   device_type_v2 = *(dnxc_data_get(unit,  "instru", "ipfix", "ipfix_v2", NULL));

   bcm_field_entry_info_t_init(&ent_info);
   ent_info.priority = 0;
    
   /* The qualifiers are:
   * - in-port is eventor port 
   * - forwarding layer is IP 
   * - forwarding layer +1 is UDP (user defined qualifier)
   * - UDP destination source is 4739 (IPFIX) 
   */
   ent_info.nof_entry_quals = 4;

   ent_info.entry_qual[0].type = bcmFieldQualifyInPort;
   ent_info.entry_qual[0].value[0] = eventor_port;
   ent_info.entry_qual[0].mask[0] = -1;
      
   ent_info.entry_qual[1].type = bcmFieldQualifyLayerRecordType;
   ent_info.entry_qual[1].value[0] = bcmFieldLayerTypeIp4;
   ent_info.entry_qual[1].mask[0] = -1;

   bcm_value[0] = bcmFieldLayerTypeUdp;
   rv = bcm_field_qualifier_value_map(unit, bcmFieldStageIngressPMF1, bcmFieldQualifyLayerRecordType, bcm_value, hw_value);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in bcm_field_qualifier_value_map\n", proc_name, rv);
      return rv;
   }

   ent_info.entry_qual[2].type = cint_ipfix_field_utils_sn_qualifier_id;
   ent_info.entry_qual[2].value[0] = hw_value[0];
   ent_info.entry_qual[2].mask[0] = -1;

   ent_info.entry_qual[3].type = bcmFieldQualifyL4DstPort;
   ent_info.entry_qual[3].value[0] = cint_ipfix_basic_info.udp_dst_port;
   ent_info.entry_qual[3].mask[0] = -1;

   /* Set actions:
     * - update Application Specific - OAM - OAM_Sub_Type 
     * - update counter ID
     *
     * Note:
     * See set_counter_resource:
     * For couner_if 0,1,2:
     * Ingress command_id are 7,8,9
     * Egress command_id are 0,1,2
     */
   ent_info.nof_entry_actions = 4;
   ent_info.entry_action[0].type = bcmFieldActionStatId0 + 7;  /* for ingress OAM - use 7*/
   ent_info.entry_action[1].type = bcmFieldActionStatProfile0 + 7; /* for ingress OAM - use 7*/
   ent_info.entry_action[2].type = bcmFieldActionOamRaw;
   ent_info.entry_action[3].type = bcmFieldActionStatOamLM;

   ent_info.entry_action[0].value[0] = cint_ipfix_basic_info.ipfix_sequence_counter_id; /* Counter id for IPFIX sequence number */
   ent_info.entry_action[1].value[0] = 1; /* is_meter - Is meter acts as "counter-increment" indication. DONT CARE */
   ent_info.entry_action[1].value[1] = 1; /* is_lm - AM latches on to LM counting mechanism - is_lm must be set to one*/
   ent_info.entry_action[1].value[2] = 0; /* type_id */
   ent_info.entry_action[1].value[3] = 1; /* valid */   
   /* OAM action is encoded in the following way:
    * LS  Oam-Up-Mep(1), Oam-Sub-Type(4), Oam-Offset(8), Oam-Stamp-Offset(8) MS
    * Use Sub-type DBAL_ENUM_FVAL_OAM_SUB_TYPE_IPFIX_TX_COMMAND (=14).
    * */
   if(device_type_v2 == 1)
   {
       ent_info.entry_action[2].value[0] = (11 << 16) | (0xff);
   }
   else
   {
       ent_info.entry_action[2].value[0] = 14 << 16;
   }
   /* command-id 7 maps to lm_read_index 0, command-id 8 to lm_read_index 1, command-id 9 to lm_read_index 2,  */
   ent_info.entry_action[3].value[0] = bcmFieldStatOamLmIndex0;
     
   rv = bcm_field_entry_add(unit, 0, cint_ipfix_tx_field_fg_id, &ent_info, &cint_ipfix_field_utils_entry_id);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in bcm_field_entry_add\n", proc_name, rv);
      return rv;
   }

   printf("%s: TCAM Entry Id (0x%x) was added (port =%d, counter = (if = %d, base = %d) fg_id = %d. \n",
                        proc_name, cint_ipfix_field_utils_entry_id, eventor_port, 
                        0, cint_ipfix_basic_info.ipfix_sequence_counter_id,
                        cint_ipfix_tx_field_fg_id);

   return rv;
}

/*
* Functions handling the configuration of IPFIX export packet .
*/
int
cint_ipfix_field_export_packet(int unit, int eventor_port, int ipfix_out_port, int context_id)
{
   int rv = BCM_E_NONE;
   char *proc_name;

   proc_name = "cint_ipfix_field_export_packet";

   /** Create IPFIX export packet qualifiers */
   rv = cint_ipfix_field_export_quals_set(unit, eventor_port, context_id);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in cint_ipfix_field_export_quals_set\n", proc_name, rv);
      return rv;
   }
   
   /** Add IPFIX SN entry */
   rv = cint_ipfix_field_export_entry_add(unit, eventor_port);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in cint_ipfix_field_export_entry_add\n", proc_name, rv);
      return rv;
   }
   return rv; 
}

/*
 * This function creates/replace a mirror destination with a sample rate of dividend / divisor, sets it to
 * crop the packet, and sets its destination to the ipfix encap id + out_port.
 * Note: 
 * For create - mirror_dest_id is returned. 
 * For replace - mirror_dest_id must contains the mirror encap id. 
 * The user can disable / enable a created mirror by performing replace and set the sample_rate_dividend to 0 /1.
 */
int
cint_ipfix_mirror_create(int unit,
                    int out_port,
                    bcm_gport_t ipfix_encap_id,
                    int sample_rate_dividend,
                    int sample_rate_divisor,
                    int *mirror_dest_id,
					int is_replace)
{
    int rv;
    bcm_mirror_destination_t mirror_dest;
    char *proc_name;
    bcm_mirror_header_info_t mirror_header_info;
    int mirror_header_flags = 0;
    int device_type_v2;

    proc_name = "cint_ipfix_mirror_create";

    device_type_v2 = *(dnxc_data_get(unit,  "instru", "ipfix", "ipfix_v2", NULL));

    /*
     * Create the mirror destination.
     */
    bcm_mirror_destination_t_init(&mirror_dest);

    if (is_replace == 0) {
       /* It is create: */
       mirror_dest.flags = BCM_MIRROR_DEST_IS_STAT_SAMPLE;
    } else {
       /* It is replace: */
       mirror_dest.flags = BCM_MIRROR_DEST_IS_STAT_SAMPLE | BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_REPLACE;
       mirror_dest.mirror_dest_id = *mirror_dest_id;
    }
	
	mirror_dest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
	mirror_dest.encap_id = ipfix_encap_id;
    mirror_dest.packet_copy_size = 256; /* Fixed value. */
    mirror_dest.sample_rate_dividend = sample_rate_dividend; /* 0 = disable , 1 = enable */
    mirror_dest.sample_rate_divisor = sample_rate_divisor;
    mirror_dest.gport = out_port;
	
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_mirror_destination_create\n", proc_name, rv);
        return rv;
    }

    printf("%s: bcm_mirror_destination_create(flags = 0x%08X, mirror_dest_id = 0x%08X)\n", proc_name, mirror_dest.flags, mirror_dest.mirror_dest_id);

    *mirror_dest_id = mirror_dest.mirror_dest_id;

    if(device_type_v2 == 1)
    {

        bcm_mirror_header_info_t_init(&mirror_header_info);
        mirror_header_info.tm.ase_ext.valid = TRUE;
        mirror_header_info.tm.ase_ext.ase_type = bcmPktDnxAseTypeTrajectoryTrace;
        mirror_header_info.tm.flow_id_ext.valid = TRUE;
        mirror_header_info.tm.flow_id_ext.flow_id = 0;
        mirror_header_info.tm.flow_id_ext.flow_profile = 0;
        mirror_header_flags = BCM_MIRROR_DEST_IS_STAT_SAMPLE | BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER;
        mirror_header_info.tm.out_vport = out_port;

        /* Inform eParser that second FTMH exists */
        mirror_header_info.pp.bytes_to_remove = 38;

        rv = bcm_mirror_header_info_set(unit,mirror_header_flags,*mirror_dest_id,mirror_header_info);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_mirror_destination_create\n", proc_name, rv);
            return rv;
        }

        printf("%s: bcm_mirror_header_info_set(mirror_dest_id = 0x%08X, ipfix_encap_id = 0x%08X, out_vport = 0x%08X)\n", proc_name, mirror_dest.mirror_dest_id, ipfix_encap_id, mirror_header_info.tm.out_vport);
    }

    return rv;
}

int
cint_ipfix_field_egress_group(int unit)
{

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;

    void *dest_char;
    int rv = BCM_E_NONE;

    /*
    * Configure the ACE format.
    */
    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;

    dest_char = &(ace_format_info.name[0]);
    sal_strncpy_s(dest_char, "AM_ACE", sizeof(ace_format_info.name));
    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_format_add \n", rv);
       return rv;
    }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = 1;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextIpfixRx;

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &cint_field_IPFIX_egress_ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_entry_add \n", rv);
       return rv;
    }
    printf("ACE Format (%d) and ACE entry 0x(%x) created \n", ace_format_id,cint_field_IPFIX_egress_ace_entry_handle);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    fg_info.qual_types[0] = bcmFieldQualifyRepCopy;
    /* Set actions */
    fg_info.action_types[0] = bcmFieldActionAceEntryId;
    if(estimate_encap_size==1)
    {
        fg_info.nof_actions = 2;
        fg_info.action_types[1] = bcmFieldActionEstimatedStartPacketStrip;
        fg_info.action_with_valid_bit[1]=TRUE;
    }

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IPFIX_egress", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IPFIX_egress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in ePMF \n", cint_field_IPFIX_egress_fg_id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IPFIX_RX", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, fg_info.stage, &context_info, &cint_field_IPFIX_egress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    if(estimate_encap_size==1)
    {
        attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IPFIX_egress_fg_id, cint_field_IPFIX_egress_context_id_ipv4, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n",cint_field_IPFIX_egress_fg_id,cint_field_IPFIX_egress_context_id_ipv4);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IPFIX_egress_presel_ipv4;
    p_id.stage = fg_info.stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IPFIX_egress_context_id_ipv4;

    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(ePMF) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IPFIX_egress_presel_ipv4,cint_field_IPFIX_egress_context_id_ipv4);

    return rv;
}

/* Example of PMF Egress Entry creation for IPFIX
 * Snooped IPFIX packets need to be set with ACE Context value for PRP context Selection
 * Qualifier is on replica.
 * The Actions set ePMF ACE context bits
 */
int cint_ipfix_field_egress_entry_add(int unit, bcm_field_entry_t *entry_handle)
{

    bcm_field_entry_info_t ent_info;
    int system_out_port;
    int rv=BCM_E_NONE;

    /**Its batter to add entries after attach, for batter bank allocation*/
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = bcmFieldQualifyRepCopy;
    ent_info.entry_qual[0].value[0] = bcmFieldRepCopyTypeStatSample;
    ent_info.entry_qual[0].mask[0] = 3;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionAceEntryId;
    ent_info.entry_action[0].value[0] = cint_field_IPFIX_egress_ace_entry_handle;
    if(estimate_encap_size==1)
    {
        /* If encap estimation is required, add constant value to Extimated btr.
         * Since the packet is trancated when snooped we can use constant value. */
        ent_info.nof_entry_actions = 2;
        ent_info.entry_action[1].type = bcmFieldActionEstimatedStartPacketStrip;
        ent_info.entry_action[1].value[0] = 80;
    }
    rv = bcm_field_entry_add(unit, 0, cint_field_IPFIX_egress_fg_id, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x) FG_ID (%d) \n Probe_1:(0x%x) Probe_2:(0x%x) Ace_Entry(0x%x)\n",
            *entry_handle,
            cint_field_IPFIX_egress_fg_id,
            ent_info.entry_qual[0].value[0],
            ent_info.entry_qual[0].value[1],cint_field_IPFIX_egress_ace_entry_handle);

    return rv;
}

/*
  IPFIX Egress field main function
  Creates field group and adds an entry
*/
int
cint_ipfix_field_egress(int unit, bcm_instru_ipfix_encap_info_t *ipfix_encap_info)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_field_entry_t entry_handle = 0;

    proc_name = "cint_ipfix_field_egress";

    if (*dnxc_data_get (unit,"instru","ipt","estimated_encap_size",NULL))
    {
      /* J2P devices require encapsulation size estimation */
      estimate_encap_size = 1;
    }

    /* Configure field */
    rv = cint_ipfix_field_egress_group(unit);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_ipfix_field_egress_group\n", proc_name, rv);
       return rv;
    }

    /* Set ePMF to trap IPFIX Rx packet */
    rv = cint_ipfix_field_egress_entry_add(unit,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in cint_ipfix_field_egress_entry_add\n", proc_name, rv);
       return rv;
    }

    return rv;
}

 /*
 * Configuration:
 *
 * cint ../../../src/examples/dnx/sflow/cint_sflow_field_advanced.c
 * cint
 * cint_sflow_field_main(0);
 * cint_sflow_field_destroy(0);
 *
 * Set up a TCAM FG that qualify upon (FWD_Layer,FWD_Layer+1,Packet_legnth_range) and action (Stat_Sampling)
 *
 */

bcm_field_group_t cint_sflow_field_fg_id = 0;
bcm_field_group_info_t cint_sflow_field_group_info;
int cint_sflow_field_range_id_0 = 0;    /* header size is 0 to 127*/
int cint_sflow_field_range_id_1 = 1;    /* header size is 128 to 144*/
bcm_field_qualify_t cint_sflow_field_qual_id = 0;

/* Note:
In order to run SFLOW Header-sampling in parallel to SFLOW Extended Gateway - 
presel-ID must be lower than the presel-IDs of Extended-gateway (see cint_sflow_field_extend_gateway.c)
*/
bcm_field_presel_t sflow_header_sampling_base_presel_id = 95;


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
*  
Packet-Length (if <144B)
Layer[curr+1].Type (is IPv4 or IPv6)** 
* 
*TCAM possible result:

Non IP Packet
Non IP Short packet (short packet < 128B)
IP Packet
IP Short packet (short packet < 128B)
For each one of the four possibilities, the PMF action is:

Statistical-Sampling, with Sniff-Profile (different profile for each of the 4 options above) 
* \see
*   * None
*/
int cint_sflow_field_advanced_entry_add(int unit, bcm_field_layer_type_t layer_type,
                                        int check_layer_type, uint32 entry_priority,
                                        int is_ip_short_packet,
                                         uint32 gport_stat_samp,
                                         bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;

    char *proc_name;

    proc_name = "cint_sflow_field_advanced_entry_add";

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = entry_priority;

    ent_info.nof_entry_quals = 2;

    /* Value+Mask on the packet-header size */
    ent_info.entry_qual[0].type = cint_sflow_field_group_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = is_ip_short_packet? cint_sflow_field_range_id_0:cint_sflow_field_range_id_1;
    ent_info.entry_qual[0].mask[0] = 0x3;


    /* Value+Mask on the Layer-type qualifier */
    ent_info.entry_qual[1].type = cint_sflow_field_group_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = layer_type;
    ent_info.entry_qual[1].mask[0] = check_layer_type? 0x1F:0;


    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionStatSampling;
    ent_info.entry_action[0].value[0] = gport_stat_samp;
    ent_info.entry_action[0].value[1] = 5;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_sflow_field_fg_id, &ent_info, entry_handle), "");

    uint32 u32 = layer_type;
    printf("%s: TCAM Entry Id=(0x%x), layer_type=(%d), is_ip_short_packet=(%d) with Stat_Code (0x%x) \n",
                            proc_name, *entry_handle, u32, is_ip_short_packet, gport_stat_samp);

    return BCM_E_NONE;
}


/**
* \brief
*  Configure TCAM FG with relevant qualifier/actions
* \param [in] unit        -  Device id
* \param [in] context_id        -  Context to attach the FG to
* \return
*   int - Error Type
* \remark
*   * Creates the qualifiers
*   For L2 bridging

Packet-Length (if <144B)
Layer[curr+1].Type (is IPv4 or IPv6)
For L3 forwarding

Packet-Length (if <144B)
Layer[curr].Type (is IPv4 or IPv6)
* \see
*   * None
*/
int cint_sflow_field_advanced_group_create(int unit)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_range_info_t range_info;
    void *dest_char;
    char *proc_name;

    proc_name = "cint_sflow_field_advanced_group_create";

    /**
    * Init the fg_info structure.
    */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypePacketHeaderSize;
    /**
    * Set the range.
    */
    range_info.min_val = 0;
    range_info.max_val = 127;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, cint_sflow_field_range_id_0, &range_info),
        "cint_sflow_field_range_id_0");
    range_info.min_val = 128;
    range_info.max_val = 144;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, cint_sflow_field_range_id_1, &range_info),
        "cint_sflow_field_range_id_1");

    /********************
     * Create and attach TCAM group in iPMF1
     * ******************/
    bcm_field_group_info_t_init(&cint_sflow_field_group_info);
    cint_sflow_field_group_info.fg_type = bcmFieldGroupTypeTcam;
    cint_sflow_field_group_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    cint_sflow_field_group_info.nof_quals = 3;
    cint_sflow_field_group_info.qual_types[0] = bcmFieldQualifyPacketLengthRangeCheck;
    cint_sflow_field_group_info.qual_types[1] = bcmFieldQualifyLayerRecordType;
    cint_sflow_field_group_info.qual_types[2] = bcmFieldQualifyInPort; /* qualifier for preventing snooping of SFLOW DATAGRAM coming from Eventor port !*/

    /* Set actions */
    cint_sflow_field_group_info.nof_actions = 1;
    cint_sflow_field_group_info.action_types[0] = bcmFieldActionStatSampling;

    dest_char = &(cint_sflow_field_group_info.name[0]);
    sal_strncpy_s(dest_char, "s_flow_fg", sizeof(cint_sflow_field_group_info.name));

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &cint_sflow_field_group_info, &cint_sflow_field_fg_id), "");
    printf("%s: FG Id (%d) was add to iPMF1 Stage\n",proc_name, cint_sflow_field_fg_id);

    return BCM_E_NONE;
}

/**
* \brief
*  Configure TCAM FG with relevant qualifier/actions
* \param [in] unit        -  Device id
* \param [in] context_id        -  Context to attach the FG to
* \return
*   int - Error Type
* \remark
*   * Creates the qualifiers
*   For L2 bridging

Packet-Length (if <144B)
Layer[curr+1].Type (is IPv4 or IPv6)
For L3 forwarding

Packet-Length (if <144B)
Layer[curr].Type (is IPv4 or IPv6)
* \see
*   * None
*/
int cint_sflow_field_advanced_group_context_create(int unit, bcm_field_context_t *context_id, char *context_name)
{
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t context_param;
    void *dest_char;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, context_name, sizeof(context_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id), "");

    /* Defining System header profiles of the context - so the PTCH layer will be deleted */
    bcm_field_context_param_info_t_init(&context_param);
    context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    context_param.param_val = bcmFieldSystemHeaderProfileFtmhTshPphUdh;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_param_set(unit,0,bcmFieldStageIngressPMF1,*context_id,&context_param),
        "system header info");
    bcm_field_context_param_info_t_init(&context_param);
    context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset1, 0);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_param_set(unit,0,bcmFieldStageIngressPMF2,*context_id,&context_param),
        "system header info");

    return BCM_E_NONE;
}


/**
* \brief
*  Configure TCAM FG with relevant qualifier/actions
* \param [in] unit        -  Device id
* \param [in] context_id        -  Context to attach the FG to
* \return
*   int - Error Type
* \remark
*   * Creates the qualifiers
*   For L2 bridging

Packet-Length (if <144B)
Layer[curr+1].Type (is IPv4 or IPv6)
For L3 forwarding

Packet-Length (if <144B)
Layer[curr].Type (is IPv4 or IPv6)
* \see
*   * None
*/
int cint_sflow_field_advanced_group_context_presel_set(int unit, bcm_field_context_t context_id,  bcm_field_presel_t presel_id, bcm_field_AppType_t app_type)
{
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    uint32 layer_type;
    char *proc_name;

    proc_name = "cint_sflow_field_advanced_group_context_presel_set";

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_data);
    
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    presel_data.entry_valid = TRUE;
    presel_data.context_id = context_id;
    presel_data.nof_qualifiers = 1;
    /*presel_data.nof_qualifiers = 0; */
    /*
    * bcmFieldQualifyAppTypePredefined is working only with KBP
    * presel_data.qual_data[0].qual_type = bcmFieldQualifyAppTypePredefined; 
    * presel_data.qual_data[0].qual_value = app_type;
    * presel_data.qual_data[0].qual_mask = 0x3F;
    */
    printf("%s: 0: context_id= %d, presel_id=%d, app_type=%d \n", proc_name, context_id, presel_id, app_type+0);

    if ( app_type == bcmFieldAppTypeL2 ) {
        layer_type = bcmFieldLayerTypeEth;

        printf("%s: 1: layer_type=%d \n", proc_name, layer_type);
    }
    else {
        layer_type = bcmFieldLayerTypeIp4;

        printf("%s: 2: layer_type=%d \n", proc_name, layer_type);
    }

    presel_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType; 
    presel_data.qual_data[0].qual_value = layer_type;
    presel_data.qual_data[0].qual_mask = 0x1F;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_data), "");

    return BCM_E_NONE;
}


/**
* \brief
*  Configure TCAM FG with relevant qualifier/actions
* \param [in] unit        -  Device id
* \param [in] context_id        -  Context to attach the FG to
* \return
*   int - Error Type
* \remark
*   * Attach the qualifiers to context
* \see
*   * None
*/
int cint_sflow_field_advanced_group_attach(int unit, bcm_field_context_t context_id, int is_bridge)
{
    bcm_field_group_attach_info_t attach_info;
    char *proc_name;

    proc_name = "cint_sflow_field_advanced_group_attach";

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = cint_sflow_field_group_info.nof_quals;

    attach_info.key_info.qual_types[0] = cint_sflow_field_group_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;


    attach_info.key_info.qual_types[1] = cint_sflow_field_group_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    /*
    For L2 bridging
        Layer[curr+1].Type (is IPv4 or IPv6)
    For L3 forwarding 
        Layer[curr].Type (is IPv4 or IPv6) 
    */
    attach_info.key_info.qual_info[1].input_arg = is_bridge? 1:0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.key_info.qual_types[2] = cint_sflow_field_group_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;


    attach_info.payload_info.nof_actions = cint_sflow_field_group_info.nof_actions;
    attach_info.payload_info.action_types[0] = cint_sflow_field_group_info.action_types[0];

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_sflow_field_fg_id, context_id, &attach_info), "");
    printf("%s: FG Id (%d) in iPMF1 attached to context (%d) \n", proc_name, cint_sflow_field_fg_id, context_id);

    return BCM_E_NONE;
}


/**
* \brief
*  Destroy the configuration.
*   Detach context from FG
*   Delete FG
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_sflow_field_advanced_destroy(int unit,bcm_field_context_t context_id)
{
    char error_msg[200]={0,};

    /* Detach the IPMF2 FG from its context */
    snprintf(error_msg, sizeof(error_msg), "fg %d from context %d", cint_sflow_field_fg_id, context_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_sflow_field_fg_id, context_id), error_msg);

    snprintf(error_msg, sizeof(error_msg), "fg %d", cint_sflow_field_fg_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_sflow_field_fg_id), error_msg);

    return BCM_E_NONE;
}

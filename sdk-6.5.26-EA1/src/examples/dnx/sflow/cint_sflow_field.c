 /*
 * Configuration:
 *
 * cint ../../../src/examples/dnx/sflow/cint_sflow_field.c
 * cint
 * cint_sflow_field_main(0);
 * cint_sflow_field_destroy(0);
 *
 * Set up a TCAM FG that qualify upon (FWD_Layer,FWD_Layer+1,Packet_legnth_range) and action (Stat_Sampling)
 *
 */

bcm_field_group_t   cint_sflow_field_fg_id = 0;
int cint_sflow_field_range_id = 1;
bcm_field_qualify_t cint_sflow_field_qual_id = 0;
bcm_field_group_t cint_sflow_egress_field_fg_id = 0;

/* 
* FG entry for preventing snooping the SFLOW DATAGRAM coming from eventor port
* It's priority must be higher then the snoop priority!
*/
uint32 field_eventor_port_entry_priority = 199; 
uint32 field_snoop_entry_priority = 200;


/**
* \brief
*  Adds TCAM entry to FG of iPMF3
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
int cint_sflow_field_entry_add(int unit,bcm_field_layer_type_t fwd_layer,
                                            bcm_field_layer_type_t fwd_layer_plus_one,
                                                int is_in_range,
                                                int is_egress_sflow, 
                                                int in_port,
                                                int mirror_code,
                                                uint32 gport_stat_samp,
                                                bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    uint32 fwd_l = fwd_layer;
    uint32 fwd_l_plus = fwd_layer_plus_one;
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "cint_sflow_field_entry_add";

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = field_snoop_entry_priority;

    ent_info.nof_entry_quals = is_egress_sflow ? 4 : 3;
    ent_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
    ent_info.entry_qual[0].value[0] = fwd_layer;
    ent_info.entry_qual[0].mask[0] = is_egress_sflow ? 0x0: 0x1F;


    ent_info.entry_qual[1].type = cint_sflow_field_qual_id;
    ent_info.entry_qual[1].value[0] = fwd_layer_plus_one;
    ent_info.entry_qual[1].mask[0] = 0x0;


    ent_info.entry_qual[2].type = bcmFieldQualifyPacketLengthRangeCheck;
    ent_info.entry_qual[2].value[0] = cint_sflow_field_range_id;
    if(is_in_range == TRUE)
    {
        ent_info.entry_qual[2].mask[0] = 0x3;
    }
    else
    {
        ent_info.entry_qual[2].mask[0] = 0x0;
    }

    if (is_egress_sflow) 
    {
        ent_info.entry_qual[3].type = bcmFieldQualifyMirrorCode;
        ent_info.entry_qual[3].value[0] = BCM_GPORT_MIRROR_GET(mirror_code);
        ent_info.entry_qual[3].mask[0] = 0x7;
    }

    ent_info.nof_entry_actions = is_egress_sflow ? 2 : 1;
    ent_info.entry_action[0].type = bcmFieldActionStatSampling;
    ent_info.entry_action[0].value[0] = gport_stat_samp;
    ent_info.entry_action[0].value[1] = 5;
    if (is_egress_sflow) {
        ent_info.entry_action[1].type = bcmFieldActionMirrorIngressRaw;
        ent_info.entry_action[1].value[0] = 0;
    }

    rv = bcm_field_entry_add(unit, 0, cint_sflow_field_fg_id, &ent_info, entry_handle);
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
*  Configure TCAM FG with relevant qualifier/actions
* \param [in] unit        -  Device id
* \param [in] context_id        -  Context to attach the FG to
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_sflow_field_main(int unit, int is_egress_sflow, bcm_field_context_t context_id)
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

    proc_name = "cint_sflow_field_main";

    /**
    * Init the fg_info structure.
    */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypePacketHeaderSize;
    range_info.min_val = 0;
    range_info.max_val = 144;
    /**
    * Set the range.
    */
    rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, cint_sflow_field_range_id, &range_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s: Error (%d), in bcm_field_range_set\n", proc_name, rv);
       return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Fwd_plus_1_layer_t", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,qual_info, &cint_sflow_field_qual_id);
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
    fg_info.qual_types[1] = cint_sflow_field_qual_id;
    fg_info.qual_types[2] = bcmFieldQualifyPacketLengthRangeCheck;
    if (!is_egress_sflow) {
        fg_info.qual_types[3] = bcmFieldQualifyInPort; /* qualifier for preventing snooping of SFLOW DATAGRAM coming from Eventor port !*/
    } else {
        fg_info.qual_types[3] = bcmFieldQualifyMirrorCode;
    }

    /* Set actions */
    fg_info.nof_actions = is_egress_sflow ? 2 : 1;
    fg_info.action_types[0] = bcmFieldActionStatSampling;
	if (is_egress_sflow) {
		fg_info.action_types[1] = bcmFieldActionMirrorIngressRaw;
	}

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "s_flow_fg", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_sflow_field_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    printf("%s: FG Id (%d) was add to iPMF1 Stage\n", proc_name, cint_sflow_field_fg_id);

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
    if (is_egress_sflow) {
        attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_sflow_field_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
    printf("%s: FG Id (%d) in iPMF1 attached to context (%d) \n", proc_name, cint_sflow_field_fg_id, context_id);

    return rv;
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
int cint_sflow_field_destroy(int unit,bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "cint_sflow_field_destroy";

    /* Detach the IPMF2 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_sflow_field_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", proc_name, rv, cint_sflow_field_fg_id, context_id);
        return rv;
    }

    rv = bcm_field_group_delete(unit, cint_sflow_field_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_delete  fg %d \n", proc_name, rv, cint_sflow_field_fg_id);
        return rv;
    }

    return rv;
}

/**
* \brief
*  Configure TCAM FG with relevant qualifier/actions
* \param [in] unit        -  Device id
* \param [in] context_id        -  Context to attach the FG to
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_sflow_egress_field_main(int unit, bcm_field_context_t context_id)
{
	int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;    

    bcm_field_group_info_t_init(&fg_info);  
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EGR_MIRROR_FG", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;    
    fg_info.qual_types[0] = bcmFieldQualifyOutPort;
    fg_info.action_types[0] = bcmFieldActionMirrorEgress;
    fg_info.action_with_valid_bit[0] = TRUE;        

    /** Create the field group. */
    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_sflow_egress_field_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, cint_sflow_egress_field_fg_id);
        return rv;
    }

    printf("Field Group ID %d was created. \n", cint_sflow_egress_field_fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);	
    attach_info.key_info.nof_quals = fg_info.nof_quals;    
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;	
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    /** Attach the created field group to the context. */
    rv = bcm_field_group_context_attach(unit, 0, cint_sflow_egress_field_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv, context_id);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", cint_sflow_egress_field_fg_id, context_id);	

    return rv;
}

/**
* \brief
*  Adds TCAM entry to FG of iPMF3
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
int cint_sflow_egress_field_entry_add(
    int unit,
    int route_out_port,
    int mirror_profile)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_id;    

    bcm_field_entry_info_t_init(&entry_info);	
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
	entry_info.entry_qual[0].type = bcmFieldQualifyOutPort;
    entry_info.entry_qual[0].value[0] = route_out_port;
    entry_info.entry_qual[0].mask[0] = 0xFFFF;
    entry_info.entry_action[0].type = bcmFieldActionMirrorEgress;
    entry_info.entry_action[0].value[0] = mirror_profile;

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, cint_sflow_egress_field_fg_id, &entry_info, &entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", entry_id, entry_id, cint_sflow_egress_field_fg_id);

    return rv;
}


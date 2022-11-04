 /*
 * Configuration:
 *
 * cint ../../../src/examples/dnx/sflow/cint_sflow_field_utils.c
 * cint
 *
 */


/* SFLOW SN FG */
bcm_field_group_t cint_sflow_field_utils_sn_field_fg_id = BCM_FIELD_INVALID;
bcm_field_qualify_t cint_sflow_field_utils_sn_qualifier_id = BCM_FIELD_INVALID;
bcm_field_entry_t cint_cflow_field_utils_entry_id = BCM_FIELD_INVALID;

bcm_field_group_t cint_sflow_field_utils_estimate_encap_size_fg_id = BCM_FIELD_INVALID;
bcm_field_context_t cint_sflow_field_utils_estimate_encap_size_context_id = BCM_FIELD_INVALID;
bcm_field_action_t cint_sflow_field_utils_estimate_encap_size_action_id = BCM_FIELD_INVALID;
bcm_field_qualify_t cint_sflow_field_stimate_encap_size_qualifier_id = BCM_FIELD_INVALID;
bcm_field_presel_t cint_sflow_field_stimate_encap_size_presel_id = 60;

int
cint_sflow_field_utils_sn_update_quals_set(int unit, int eventor_port, int context_id)
{
   bcm_field_stage_t bcm_stage = bcmFieldStageIngressPMF1;
   bcm_field_group_info_t field_group_info;
   bcm_field_group_attach_info_t attach_info;
   bcm_field_qualifier_info_create_t qual_info;
   bcm_field_qualifier_info_get_t qual_info_get;
   void *dest_char;
   char *proc_name;

   proc_name = "cint_sflow_field_utils_sn_update_quals_set";

   /* Create user defined qualifier */
   BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordType, bcm_stage, &qual_info_get),
        "for bcmFieldQualifyLayerRecordType");

   bcm_field_qualifier_info_create_t_init(&qual_info);
   qual_info.size = qual_info_get.size;
   dest_char = &(qual_info.name[0]);
   sal_strncpy_s(dest_char, "SFLOW-SN-qual", sizeof(qual_info.name));
   BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_sflow_field_utils_sn_qualifier_id), "");
   bcm_field_group_info_t_init(&field_group_info);
   field_group_info.fg_type = bcmFieldGroupTypeTcam;
   field_group_info.stage = bcm_stage;

   /* Set quals:
    * - in-port is eventor port 
    * - forwarding layer is IP 
    * - forwarding layer +1 is UDP 
    * - UDP destination source is 6343 (SFLOW) 
    */
   field_group_info.nof_quals = 4;
   field_group_info.qual_types[0] = bcmFieldQualifyInPort;
   field_group_info.qual_types[1] = bcmFieldQualifyLayerRecordType;
   field_group_info.qual_types[2] = cint_sflow_field_utils_sn_qualifier_id; /* user defined qualifier*/
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
   sal_strncpy_s(dest_char, "cint_sflow_field_utils_sn_field_fg_id", sizeof(field_group_info.name));

   BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &field_group_info, &cint_sflow_field_utils_sn_field_fg_id), "");
   printf("%s: FG Id (%d) was add to iPMF1 Stage\n", proc_name, cint_sflow_field_utils_sn_field_fg_id);

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

   BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_sflow_field_utils_sn_field_fg_id, context_id, &attach_info), "");

   printf("%s: FG Id (%d) in iPMF1 attached to context (%d)\n", proc_name, cint_sflow_field_utils_sn_field_fg_id, context_id);


   return BCM_E_NONE; 

}

int
cint_sflow_field_utils_sn_update_entry_add(int unit, int eventor_port)
{
   char *proc_name;

   bcm_field_entry_info_t ent_info;
   uint32 bcm_value[BCM_FIELD_QUAL_WIDTH_IN_WORDS];
   uint32 hw_value[BCM_FIELD_QUAL_WIDTH_IN_WORDS];

   proc_name = "cint_sflow_field_utils_sn_update_entry_add";

   bcm_field_entry_info_t_init(&ent_info);
   ent_info.priority = 0;
    
   /* The qualifiers are:
   * - in-port is eventor port 
   * - forwarding layer is IP 
   * - forwarding layer +1 is UDP (user defined qualifier)
   * - UDP destination source is 6343 (SFLOW) 
   */
   ent_info.nof_entry_quals = 4;

   ent_info.entry_qual[0].type = bcmFieldQualifyInPort;
   ent_info.entry_qual[0].value[0] = eventor_port;
   ent_info.entry_qual[0].mask[0] = -1;
      
   ent_info.entry_qual[1].type = bcmFieldQualifyLayerRecordType;
   ent_info.entry_qual[1].value[0] = bcmFieldLayerTypeIp4;
   ent_info.entry_qual[1].mask[0] = -1;

   bcm_value[0] = bcmFieldLayerTypeUdp;
   BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_value_map(unit, bcmFieldStageIngressPMF1, bcmFieldQualifyLayerRecordType, bcm_value, hw_value), "");

   ent_info.entry_qual[2].type = cint_sflow_field_utils_sn_qualifier_id;
   ent_info.entry_qual[2].value[0] = hw_value[0];
   ent_info.entry_qual[2].mask[0] = -1;

   ent_info.entry_qual[3].type = bcmFieldQualifyL4DstPort;
   ent_info.entry_qual[3].value[0] = cint_sflow_basic_info.udp_dst_port;
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

   ent_info.entry_action[0].value[0] = cint_sflow_basic_info.sn_counter_stat_id;
   ent_info.entry_action[1].value[0] = 1; /* is_meter - Is meter acts as "counter-increment" indication. DONT CARE */
   ent_info.entry_action[1].value[1] = 1; /* is_lm - AM latches on to LM counting mechanism - is_lm must be set to one*/
   ent_info.entry_action[1].value[2] = 0; /* type_id */
   ent_info.entry_action[1].value[3] = 1; /* valid */   
   /* OAM action is encoded in the following way:
    * LS  Oam-Up-Mep(1), Oam-Sub-Type(4), Oam-Offset(8), Oam-Stamp-Offset(8) MS
    * Use Sub-type DBAL_ENUM_FVAL_OAM_SUB_TYPE_SFLOW_SEQUENCE_RESET (=13). 
    * Set Oam-Stamp-Offset to 2, as PTCH size, so Oam-Stamp-Offset - Layer_offset[0] is not negative, 
    * else overflow error may be generated which irrelevant for this case
    * */
   ent_info.entry_action[2].value[0] = 13 << 16 | 2;
   /* command-id 7 maps to lm_read_index 0, command-id 8 to lm_read_index 1, command-id 9 to lm_read_index 2,  */
   ent_info.entry_action[3].value[0] = bcmFieldStatOamLmIndex0;
     
   BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_sflow_field_utils_sn_field_fg_id, &ent_info, &cint_cflow_field_utils_entry_id), "");

   printf("%s: TCAM Entry Id (0x%x) was added (port =%d, counter = (if = %d, base = %d) fg_id = %d. \n",
                        proc_name, cint_cflow_field_utils_entry_id, eventor_port, 
                        cint_sflow_basic_info.counter_if, cint_sflow_basic_info.counter_base,
                        cint_sflow_field_utils_sn_field_fg_id);

   return BCM_E_NONE;
}


/**
* \brief
*  Adds TCAM entry to FG of iPMF3
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
int cint_sflow_field_utils_entry_add_eventor_port(int unit, int eventor_port, int fg_id, bcm_field_entry_t *entry_handle, int field_entry_priority)
{
   bcm_field_entry_info_t ent_info;
   char *proc_name;

   proc_name = "cint_sflow_field_utils_entry_add_eventor_port";

   bcm_field_entry_info_t_init(&ent_info);
   ent_info.priority = field_entry_priority;

   ent_info.nof_entry_quals = 1;
   ent_info.entry_qual[0].type = bcmFieldQualifyInPort;
   ent_info.entry_qual[0].value[0] = eventor_port;
   ent_info.entry_qual[0].mask[0] = -1;

   /* action = DO NOTHING - that is skipp the snooping since the priority of snopping is lower */
   ent_info.nof_entry_actions = 0;

   BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, fg_id, &ent_info, entry_handle), "");

   printf("%s: TCAM Entry Id (0x%x) with NO ACTION is added (for port = %d) per fg_id= %d \n", proc_name, *entry_handle, eventor_port, fg_id);

   return BCM_E_NONE;
}

/**
* \brief
*  create epmf rule under new context, which estimate the sflow termination size
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_sflow_field_egress_estimate_encap_size(int unit)
{
    void *dest_char;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_entry_t ent_id;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info, action_info_get;
    bcm_field_qualifier_info_get_t qual_info_get;
    uint32 estimate_encap_size_required =
    		*dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

    if (estimate_encap_size_required &&
        cint_sflow_field_utils_estimate_encap_size_context_id == BCM_FIELD_INVALID)
    {

        /*
         * create a direct Extraction Field Group in ePMF
         */
        bcm_field_context_info_t_init(&context_info);
        dest_char = &(context_info.name[0]);
        sal_strncpy_s(dest_char, "SflowEpmfBtrCtx", sizeof(context_info.name));
        BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create
                (unit, 0, bcmFieldStageEgress, &context_info, &cint_sflow_field_utils_estimate_encap_size_context_id), "");
        BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_info_get
                            (unit, bcmFieldQualifyLayerRecordOffset, bcmFieldStageEgress, &qual_info_get), "");
        /***
         * create the context selection based on UDH base0=0x3 and base1=0x2
         * */
        bcm_field_presel_entry_id_info_init(&p_id);
        bcm_field_presel_entry_data_info_init(&p_data);

        p_id.presel_id = cint_sflow_field_stimate_encap_size_presel_id;
        p_id.stage = bcmFieldStageEgress;
        p_data.entry_valid = TRUE;
        p_data.context_id = cint_sflow_field_utils_estimate_encap_size_context_id;
        p_data.nof_qualifiers = 4;
        p_data.qual_data[0].qual_type = bcmFieldQualifyUDHBase0;
        p_data.qual_data[0].qual_arg = 0;
        p_data.qual_data[0].qual_value = 0x3;
        p_data.qual_data[0].qual_mask = 0x3;
        p_data.qual_data[1].qual_type = bcmFieldQualifyUDHBase1;
        p_data.qual_data[1].qual_arg = 0;
        p_data.qual_data[1].qual_value = 0x2;
        p_data.qual_data[1].qual_mask = 0x3;
        p_data.qual_data[2].qual_type = bcmFieldQualifyUDHBase2;
        p_data.qual_data[2].qual_arg = 0;
        p_data.qual_data[2].qual_value = 0;
        p_data.qual_data[2].qual_mask = 0x3;
        p_data.qual_data[3].qual_type = bcmFieldQualifyUDHBase3;
        p_data.qual_data[3].qual_arg = 0;
        p_data.qual_data[3].qual_value = 0;
        p_data.qual_data[3].qual_mask = 0x3;
        BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &p_data), "");

        bcm_field_group_info_t_init(&fg_info);
        bcm_field_group_attach_info_t_init(&attach_info);

        fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
        fg_info.stage = bcmFieldStageEgress;
        dest_char = &(fg_info.name[0]);
        sal_strncpy_s(dest_char, "Sflow_Epmf_Btr_fg", sizeof(fg_info.name));
        fg_info.nof_quals = 1;
        fg_info.nof_actions = 1;
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 8;
        dest_char = &(qual_info.name[0]);
        sal_strncpy_s(dest_char, "Sflow_Epmf_Btr_qual", sizeof(qual_info.name));
        BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_sflow_field_stimate_encap_size_qualifier_id), "");

        bcm_field_action_info_t_init(&action_info);
        BCM_IF_ERROR_RETURN_MSG(bcm_field_action_info_get
                        (unit, bcmFieldActionEstimatedStartPacketStrip, bcmFieldStageEgress, &action_info_get), "");
        action_info.action_type = bcmFieldActionEstimatedStartPacketStrip;
        action_info.size = qual_info_get.size;
        action_info.prefix_size = action_info_get.size - action_info.size;
        action_info.prefix_value = 0;
        action_info.stage = bcmFieldStageEgress;
        dest_char = &(action_info.name[0]);
        sal_strncpy_s(dest_char, "Sflow_Epmf_Ebtr_action", sizeof(action_info.name));
        BCM_IF_ERROR_RETURN_MSG(bcm_field_action_create(unit, 0, &action_info, &cint_sflow_field_utils_estimate_encap_size_action_id), "");
        fg_info.nof_quals = 1;
        fg_info.qual_types[0] = bcmFieldQualifyLayerRecordOffset;
        fg_info.action_types[0] = cint_sflow_field_utils_estimate_encap_size_action_id;
        fg_info.action_with_valid_bit[0] = 0;
        attach_info.payload_info.action_types[0] = fg_info.action_types[0];
        attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
        attach_info.key_info.qual_info[0].input_arg = 3;

        /** Create the field group. */
        BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &cint_sflow_field_utils_estimate_encap_size_fg_id), "");
        /** Attach the created field group to the context. */
        BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach
                (unit, 0, cint_sflow_field_utils_estimate_encap_size_fg_id,
                 cint_sflow_field_utils_estimate_encap_size_context_id, &attach_info), "");
    }
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
int cint_sflow_field_utils_destroy(int unit, int context_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;
    char error_msg[200]={0,};


    if (cint_sflow_field_utils_sn_field_fg_id != BCM_FIELD_INVALID) {

       snprintf(error_msg, sizeof(error_msg), "fg %d from context %d", cint_sflow_field_utils_sn_field_fg_id, context_id);
       BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_sflow_field_utils_sn_field_fg_id, context_id), error_msg);

       BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_sflow_field_utils_sn_field_fg_id, NULL, cint_cflow_field_utils_entry_id), "");

       snprintf(error_msg, sizeof(error_msg), "fg_id = %d", cint_sflow_field_utils_sn_field_fg_id);
       BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_sflow_field_utils_sn_field_fg_id), error_msg);

       BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_destroy(unit, cint_sflow_field_utils_sn_qualifier_id), "");
    }

    if (cint_sflow_field_utils_estimate_encap_size_fg_id != BCM_FIELD_INVALID)
    {
        snprintf(error_msg, sizeof(error_msg), "fg %d from context %d",
            cint_sflow_field_utils_estimate_encap_size_fg_id, cint_sflow_field_utils_estimate_encap_size_context_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_sflow_field_utils_estimate_encap_size_fg_id,
                                            cint_sflow_field_utils_estimate_encap_size_context_id), error_msg);
        snprintf(error_msg, sizeof(error_msg), "fg_id = %d", cint_sflow_field_utils_estimate_encap_size_fg_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_sflow_field_utils_estimate_encap_size_fg_id), error_msg);
        cint_sflow_field_utils_estimate_encap_size_fg_id = BCM_FIELD_INVALID;
        int action_id = cint_sflow_field_utils_estimate_encap_size_action_id;
        snprintf(error_msg, sizeof(error_msg), "action_id = %d", action_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_field_action_destroy(unit, cint_sflow_field_utils_estimate_encap_size_action_id), error_msg);
        cint_sflow_field_utils_estimate_encap_size_action_id = BCM_FIELD_INVALID;
        BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_destroy(unit, cint_sflow_field_stimate_encap_size_qualifier_id), "");
        cint_sflow_field_stimate_encap_size_qualifier_id = BCM_FIELD_INVALID;
        bcm_field_presel_entry_data_info_init(&presel_entry_data);
        presel_entry_data.entry_valid = 0;
        presel_entry_data.context_id = 0;
        p_id.stage = bcmFieldStageEgress;
        p_id.presel_id = cint_sflow_field_stimate_encap_size_presel_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_field_context_destroy(unit, bcmFieldStageEgress, cint_sflow_field_utils_estimate_encap_size_context_id), "");
        cint_sflow_field_utils_estimate_encap_size_context_id = BCM_FIELD_INVALID;
    }
    return BCM_E_NONE;
}

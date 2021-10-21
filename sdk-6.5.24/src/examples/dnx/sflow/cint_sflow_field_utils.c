 /*
 * Configuration:
 *
 * cint ../../../src/examples/dnx/sflow/cint_sflow_field_utils.c
 * cint
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
   int rv = BCM_E_NONE;
   bcm_field_stage_t bcm_stage = bcmFieldStageIngressPMF1;
   bcm_field_group_info_t field_group_info;
   bcm_field_group_attach_info_t attach_info;
   bcm_field_qualifier_info_create_t qual_info;
   bcm_field_qualifier_info_get_t qual_info_get;
   void *dest_char;
   char *proc_name;

   proc_name = "cint_sflow_field_utils_sn_update_quals_set";

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
   sal_strncpy_s(dest_char, "SFLOW-SN-qual", sizeof(qual_info.name));
   rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_sflow_field_utils_sn_qualifier_id);
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

   rv = bcm_field_group_add(unit, 0, &field_group_info, &cint_sflow_field_utils_sn_field_fg_id);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in bcm_field_group_add\n", proc_name, rv);
      return rv;
   }
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

   rv = bcm_field_group_context_attach(unit, 0, cint_sflow_field_utils_sn_field_fg_id, context_id, &attach_info);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
      return rv;
   }

   printf("%s: FG Id (%d) in iPMF1 attached to context (%d)\n", proc_name, cint_sflow_field_utils_sn_field_fg_id, context_id);


   return rv; 

}

int
cint_sflow_field_utils_sn_update_entry_add(int unit, int eventor_port)
{
   int rv = BCM_E_NONE;
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
   rv = bcm_field_qualifier_value_map(unit, bcmFieldStageIngressPMF1, bcmFieldQualifyLayerRecordType, bcm_value, hw_value);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in bcm_field_qualifier_value_map\n", proc_name, rv);
      return rv;
   }

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
 */
   ent_info.entry_action[2].value[0] = 13 << 16;
   /* command-id 7 maps to lm_read_index 0, command-id 8 to lm_read_index 1, command-id 9 to lm_read_index 2,  */
   ent_info.entry_action[3].value[0] = bcmFieldStatOamLmIndex0;
     
   rv = bcm_field_entry_add(unit, 0, cint_sflow_field_utils_sn_field_fg_id, &ent_info, &cint_cflow_field_utils_entry_id);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), in bcm_field_entry_add\n", proc_name, rv);
      return rv;
   }

   printf("%s: TCAM Entry Id (0x%x) was added (port =%d, counter = (if = %d, base = %d) fg_id = %d. \n",
                        proc_name, cint_cflow_field_utils_entry_id, eventor_port, 
                        cint_sflow_basic_info.counter_if, cint_sflow_basic_info.counter_base,
                        cint_sflow_field_utils_sn_field_fg_id);

   return rv;
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
   int rv = BCM_E_NONE;
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
    int rv = BCM_E_NONE;
    char *proc_name;
    uint32 estimate_encap_size_required =
    		*dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

    if (estimate_encap_size_required &&
        cint_sflow_field_utils_estimate_encap_size_context_id == BCM_FIELD_INVALID)
    {
        proc_name = "cint_sflow_field_egress_estimate_encap_size";

        /*
         * create a direct Extraction Field Group in ePMF
         */
        bcm_field_context_info_t_init(&context_info);
        dest_char = &(context_info.name[0]);
        sal_strncpy_s(dest_char, "SflowEpmfBtrCtx", sizeof(context_info.name));
        rv = bcm_field_context_create
                (unit, 0, bcmFieldStageEgress, &context_info, &cint_sflow_field_utils_estimate_encap_size_context_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_context_create\n", proc_name, rv);
            return rv;
        }
        bcm_field_qualifier_info_get
                            (unit, bcmFieldQualifyLayerRecordOffset, bcmFieldStageEgress, &qual_info_get);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_qualifier_info_get\n", proc_name, rv);
            return rv;
        }
        /***
         * create the context selection based on UDH base0=0x3 and base1=0x2
 */
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
        rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_presel_set\n", proc_name, rv);
            return rv;
        }

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
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_sflow_field_stimate_encap_size_qualifier_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
            return rv;
        }

        bcm_field_action_info_t_init(&action_info);
        rv = bcm_field_action_info_get
                        (unit, bcmFieldActionEstimatedStartPacketStrip, bcmFieldStageEgress, &action_info_get);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_action_info_get\n", proc_name, rv);
            return rv;
        }
        action_info.action_type = bcmFieldActionEstimatedStartPacketStrip;
        action_info.size = qual_info_get.size;
        action_info.prefix_size = action_info_get.size - action_info.size;
        action_info.prefix_value = 0;
        action_info.stage = bcmFieldStageEgress;
        dest_char = &(action_info.name[0]);
        sal_strncpy_s(dest_char, "Sflow_Epmf_Ebtr_action", sizeof(action_info.name));
        rv = bcm_field_action_create(unit, 0, &action_info, &cint_sflow_field_utils_estimate_encap_size_action_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_action_create\n", proc_name, rv);
            return rv;
        }
        fg_info.nof_quals = 1;
        fg_info.qual_types[0] = bcmFieldQualifyLayerRecordOffset;
        fg_info.action_types[0] = cint_sflow_field_utils_estimate_encap_size_action_id;
        fg_info.action_with_valid_bit[0] = 0;
        attach_info.payload_info.action_types[0] = fg_info.action_types[0];
        attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
        attach_info.key_info.qual_info[0].input_arg = 3;

        /** Create the field group. */
        rv = bcm_field_group_add(unit, 0, &fg_info, &cint_sflow_field_utils_estimate_encap_size_fg_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_group_add\n", proc_name, rv);
            return rv;
        }
        /** Attach the created field group to the context. */
        rv = bcm_field_group_context_attach
                (unit, 0, cint_sflow_field_utils_estimate_encap_size_fg_id,
                 cint_sflow_field_utils_estimate_encap_size_context_id, &attach_info);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
            return rv;
        }
    }
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
int cint_sflow_field_utils_destroy(int unit, int context_id)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t p_id;
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "cint_sflow_field_utils_destroy";

    if (cint_sflow_field_utils_sn_field_fg_id != BCM_FIELD_INVALID) {

       rv = bcm_field_group_context_detach(unit, cint_sflow_field_utils_sn_field_fg_id, context_id);
       if (rv != BCM_E_NONE)
       {
           printf("%s: Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", proc_name, rv, cint_sflow_field_utils_sn_field_fg_id, context_id);
           return rv;
       }

       rv = bcm_field_entry_delete(unit, cint_sflow_field_utils_sn_field_fg_id, NULL, cint_cflow_field_utils_entry_id);
       if (rv != BCM_E_NONE)
       {
          printf("%s: Error (%d), in bcm_field_entry_delete \n", proc_name, rv);
          return rv;
       }

       rv = bcm_field_group_delete(unit, cint_sflow_field_utils_sn_field_fg_id);
       if (rv != BCM_E_NONE)
       {
           printf("%s: Error (%d), in bcm_field_group_delete  fg_id = %d \n", proc_name, rv, cint_sflow_field_utils_sn_field_fg_id);
           return rv;
       }

       rv = bcm_field_qualifier_destroy(unit, cint_sflow_field_utils_sn_qualifier_id);
       if (rv != BCM_E_NONE)
       {
           printf("%s: Error (%d), in bcm_field_qualifier_destroy \n", proc_name, rv);
           return rv;
       }
    }

    if (cint_sflow_field_utils_estimate_encap_size_fg_id != BCM_FIELD_INVALID)
    {
        rv = bcm_field_group_context_detach(unit, cint_sflow_field_utils_estimate_encap_size_fg_id,
                                            cint_sflow_field_utils_estimate_encap_size_context_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", proc_name, rv,
            cint_sflow_field_utils_estimate_encap_size_fg_id, cint_sflow_field_utils_estimate_encap_size_context_id);
            return rv;
        }
        rv = bcm_field_group_delete(unit, cint_sflow_field_utils_estimate_encap_size_fg_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_group_delete  fg_id = %d \n", proc_name, rv,
                   cint_sflow_field_utils_estimate_encap_size_fg_id);
            return rv;
        }
        cint_sflow_field_utils_estimate_encap_size_fg_id = BCM_FIELD_INVALID;
        rv = bcm_field_action_destroy(unit, cint_sflow_field_utils_estimate_encap_size_action_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_action_destroy  action_id = %d \n", proc_name, rv,
                   cint_sflow_field_utils_estimate_encap_size_action_id);
            return rv;
        }
        cint_sflow_field_utils_estimate_encap_size_action_id = BCM_FIELD_INVALID;
        rv = bcm_field_qualifier_destroy(unit, cint_sflow_field_stimate_encap_size_qualifier_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s: Error (%d), in bcm_field_qualifier_destroy \n", proc_name, rv);
            return rv;
        }
        cint_sflow_field_stimate_encap_size_qualifier_id = BCM_FIELD_INVALID;
        bcm_field_presel_entry_data_info_init(&presel_entry_data);
        presel_entry_data.entry_valid = 0;
        presel_entry_data.context_id = 0;
        p_id.stage = bcmFieldStageEgress;
        p_id.presel_id = cint_sflow_field_stimate_encap_size_presel_id;
        rv = bcm_field_presel_set(unit, 0, &p_id, &presel_entry_data);
        if(rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_presel_set\n");
            return rv;
        }

        rv = bcm_field_context_destroy(unit, bcmFieldStageEgress, cint_sflow_field_utils_estimate_encap_size_context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_context_destroy \n", rv);
            return rv;
        }
        cint_sflow_field_utils_estimate_encap_size_context_id = BCM_FIELD_INVALID;
    }
    return rv;
}

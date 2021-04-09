/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * Purpose:
 *     FEC ID in learning payload is 15 bits,whereas the FEC ID in JR2 is 18 bits wide.
 *     this will tranlsate the global FEC ID from learning
 *     payload to local FEC ID used for local FEC access.
 *     this is an example.
 */


/**
 * FEC ID in learning payload is 15 bits,whereas the
 * FEC ID in JR2 is 18 bits wide.
 * this feature will tranlsate the global FEC ID from learning
 * payload to local FEC ID used for local FEC access.
 */

 struct fec_id_alignment_field_s {
     bcm_field_presel_t presel_id;
     bcm_field_stage_t stage;
     bcm_field_group_t fg_id;
     bcm_field_context_t context_id;
     bcm_field_entry_t entry_id;
     uint32 pp_port_cs_profile;
 };
fec_id_alignment_field_s fec_id_alignment_field = {
    100,
    bcmFieldStageIngressPMF1,
    0,
    0,
    0,
    0};


 int
fec_id_alignment_field_db_init(
    int unit,
    bcm_field_group_t *fg_id,
    int port)
{
   /*
    * Context "Default_Ctx"
    * Qualifys:
    *   bcmFieldQualifyPortClassPacketProcessing == default(0)
    *   bcmFieldQualifyLayerRecordType == Eth
    *   bcmFieldQualifyDstPort == Dst type is FEC and 6MSB is 0
    * Actions:
    *   fwd_action_dst = local_fec_id
    */
   int rv;
   bcm_field_presel_entry_data_t p_data;
   bcm_field_presel_entry_id_t p_id;
   bcm_field_context_t context_id;
   bcm_field_context_info_t context_info;
   void *dest_char;
   bcm_field_group_t fg_id_i = 0;

   bcm_field_group_info_t fg_info;
   bcm_field_group_attach_info_t attach_info;

   bcm_field_qualifier_info_create_t qual_info;
   bcm_field_qualify_t qual_id;
   bcm_field_qualifier_info_get_t qual_info_get;
   bcm_field_range_info_t range_info;
   char *proc_name;
   uint32 olp_port_class_id = 0;
   bcm_field_context_param_info_t param_info;

   proc_name = "fec_id_alignment_field_db_init";

   bcm_field_group_info_t_init(&fg_info);
   fg_info.fg_type = bcmFieldGroupTypeTcam;
   fg_info.stage = bcmFieldStageIngressPMF1;

   /*
    * Set quals
    */
   fg_info.nof_quals = 3;
   fg_info.qual_types[0] = bcmFieldQualifyPortClassPacketProcessing;
   fg_info.qual_types[1] = bcmFieldQualifyLayerRecordType;
   fg_info.qual_types[2] = bcmFieldQualifyDstPort;

   /*
    * Set actions
    */
   fg_info.nof_actions = 1;
   fg_info.action_types[0] = bcmFieldActionRedirect;

   dest_char = &(fg_info.name[0]);
   sal_strncpy_s(dest_char, "J1_FEC_ID_ALIGN_FG", sizeof(fg_info.name));
   rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id_i);
   if (rv != BCM_E_NONE)
   {
       printf("Error, bcm_field_group_add\n");
       return rv;
   }
   *fg_id = fg_id_i;
   fec_id_alignment_field.fg_id = fg_id_i;

   context_id = *dnxc_data_get(unit, "field", "context", "default_context", NULL);
   fec_id_alignment_field.context_id = context_id;

   bcm_field_group_attach_info_t_init(&attach_info);

   attach_info.key_info.nof_quals = fg_info.nof_quals;
   attach_info.payload_info.nof_actions = fg_info.nof_actions;
   attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
   attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
   attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
   attach_info.payload_info.action_types[0] = fg_info.action_types[0];
   attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
   attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
   attach_info.key_info.qual_info[1].input_arg = 0;
   attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;

   rv = bcm_field_group_context_attach(unit, 0, fg_id_i, context_id, &attach_info);
   if (rv != BCM_E_NONE)
   {
       printf("Error, bcm_field_group_context_attach\n");
       return rv;
   }

   rv = bcm_port_class_get(unit, port,
                   bcmPortClassFieldIngressPMF1PacketProcessingPortCs, &olp_port_class_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error, bcm_port_class_get\n");
       return rv;
   }
   fec_id_alignment_field.pp_port_cs_profile = olp_port_class_id;

   return rv;
}

/**
 * FEC ID alignment entry adding routing
*/
int fec_id_alignment_field_entry_add(int unit,  bcm_field_group_t fg_id, int fec_id)
{
    bcm_field_entry_info_t entry_info;
    bcm_gport_t in_port_gport;
    int rv = BCM_E_NONE;
    uint32 global_fec_id = 0;
    uint32 global_fec_destination = 0, local_fec_destination = 0;
    bcm_field_entry_t entry_id;
    bcm_gport_t local_fec_gport;

    global_fec_id = fec_id & 0x7FFF;
    BCM_GPORT_FORWARD_PORT_SET(local_fec_gport, fec_id);

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = 3;
    entry_info.nof_entry_actions = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyPortClassPacketProcessing;
    entry_info.entry_qual[0].value[0] = fec_id_alignment_field.pp_port_cs_profile;
    entry_info.entry_qual[0].mask[0] = 0x7;
    entry_info.entry_qual[1].type = bcmFieldQualifyLayerRecordType;
    entry_info.entry_qual[1].value[0] = bcmFieldLayerTypeEth;
    entry_info.entry_qual[1].mask[0] = 0x1F;
    entry_info.entry_qual[2].type = bcmFieldQualifyDstPort;
    entry_info.entry_qual[2].value[0] = global_fec_id;
   /*
    * In IOP mode, the learned FEC ID is 16 bits for JR2 and learned FEC ID is 15 bit for J2C and above device.
    * So the 6msb of forward action destination shall be 00000x.
    */
    entry_info.entry_qual[2].mask[0] = 0x1F7FFF;
    entry_info.entry_action[0].type = bcmFieldActionRedirect;
    entry_info.entry_action[0].value[0] = local_fec_gport;

    rv = bcm_field_entry_add(unit, 0, fg_id , &entry_info, &entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    fec_id_alignment_field.entry_id = entry_id;
    printf("entry %d is installed, convert the global FEC 0x%x to local FEC 0x%x",entry_id,global_fec_id,fec_id);

    return rv;

}

int fec_id_alignment_field_add(int unit,  int access_port, int fec_id)
{
    int rv = BCM_E_NONE;

    if (is_device_or_above(unit, JERICHO2) &&
        (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)) {
        bcm_gport_t gport;
        bcm_field_group_t fg_id;

        if (!BCM_GPORT_IS_TRUNK(access_port)) {
            BCM_GPORT_LOCAL_SET(gport,access_port);
        } else {
            gport = access_port;
        }
        rv = fec_id_alignment_field_db_init(unit, &fg_id, gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error, fec_id_alignment_field_db_init\n");
            return rv;
        }

        rv = fec_id_alignment_field_entry_add(unit, fg_id, fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, fec_id_alignment_field_entry_add\n");
            return rv;
        }

    }

    return rv;
}

int fec_id_alignment_field_destroy(int unit)
{
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_t presel_id = fec_id_alignment_field.presel_id;
    bcm_field_stage_t stage = fec_id_alignment_field.stage;
    bcm_field_context_t context_id = fec_id_alignment_field.context_id;
    bcm_field_group_t fg_id = fec_id_alignment_field.fg_id;
    bcm_field_entry_t entry_id = fec_id_alignment_field.entry_id;

    /* Destroy the TCAM FG */
    rv = bcm_field_entry_delete(unit, fg_id, NULL, entry_id);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, fg_id, context_id);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, fg_id);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail in bcm_field_group_delete\n", rv);
        return rv;
    }

    return rv;

}


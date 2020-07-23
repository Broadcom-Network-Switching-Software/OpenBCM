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
 * cint ../../../src/examples/dnx/field/cint_field_action_drop.c
 * cint ../../../src/examples/dnx/field/cint_field_static_sa_move.c
 * int unit = 0;
 * field_static_sa_move_main(unit);
 *
 * Configuration example end
 *
 *
 *   This Cint shows how to handle static SA move events
 *   MACT entry with strength '0' is transplant-able. It also doesn't age out (as dynamic entries do).
 *   Received packet performs a learning lookup and hit entry is an entry with strength '0' which needs to be updated.
 *   We create a rule that captures this case (accepted strength '0', found '1', match '0')
 *   In this example we drop the packet. The user may want to snoop to CPU, set learn_enable to '0' or any other action.
 *
 *   Configuration:
 *    The context selected based on Forwarding layer Ethernet
 *    Create a Field group that Qualifiers: accepted_strength (from general data) and found_match bits (from learn_info)
 *    Add TCAM entry that includes accepted strength '0'(transplant-able entry) and found_match 2 (found and not matched)
 *    The action is drop.
 *    The configured stage will be iPMF1 (can be configured in same way in other stages)
 */

bcm_field_group_t cint_attach_fg_id=0;
bcm_field_entry_t cint_attach_entry_id = 0;
bcm_field_context_t cint_attach_context_id_eth;
bcm_field_presel_t cint_attach_presel_eth = 5;
bcm_field_action_t action_drop = 0;

int field_static_sa_move_main(int unit)
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
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = bcmFieldQualifySaLookupAcceptedStrength;
    fg_info.qual_types[1] = bcmFieldQualifyLearnMatch;
    fg_info.qual_types[2] = bcmFieldQualifyLearnFound;
 
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = action_drop;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "static_transplant", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_attach_fg_id);
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
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, cint_attach_fg_id, cint_attach_context_id_eth, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",cint_attach_fg_id,cint_attach_context_id_eth);
    
    
    /* Add entries after attach */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 3;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 0;
    ent_info.entry_qual[0].mask[0] = 0x1;
    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = 0;
    ent_info.entry_qual[1].mask[0] = 0x1;
    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = 1;
    ent_info.entry_qual[2].mask[0] = 0x1;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0;
 

    rv = bcm_field_entry_add(unit, 0, cint_attach_fg_id, &ent_info, &cint_attach_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)\n",cint_attach_entry_id);
    
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_attach_presel_eth;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_attach_context_id_eth;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    p_data.qual_data[0].qual_mask = 0x1F;


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

int field_static_sa_move_destroy(int unit)
{
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

    rv = bcm_field_entry_delete(unit,cint_attach_fg_id,entry_qual_info,cint_attach_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete \n", rv);
        return rv;
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


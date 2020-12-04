/*
 * $Id: cint_dnx_oam_mpls_transit_signal.c
l.c This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
l.c 
l.c Copyright 2007-2020 Broadcom Inc. All rights reserved.File: cint_oam_mpls_transit_signal.c 
 *
 * Purpose: Examples for Mpls OAM transit signal on P-node. 
 *
 * Usage:
 *
 * cd ../../../../src/examples/
 * cint sand/utility/cint_sand_utils_global.c 
 * cint sand/utility/cint_sand_utils_l3.c 
 * cint dnx/utility/cint_dnx_utility_mpls.c 
 * cint dnx/mpls/cint_mpls_operations_without_eedb.c
 * cint dnx/oam/cint_dnx_oam_mpls_transit_signal.c 
 * cint
 * int unit=0;
 * print cint_oam_mpls_send_signal_in_p_node(unit,200, 201, 2, 0x8902);

 */


/**
 * Creating ePFM to qualify OAM packet CCMoGACHoGALoMPLSxoETH, and set ace_value
 * Qualify: ingress_PP_ports+GAL+GACH.channel type+OAM.CCM
 * Actioin:  set ace_value hit the context TERMINATION__MPLS__TRANSIT_CCM_SD to modify CCM.EI in OAM payload
 * @param unit : unit
 * @param in_port : in port
 * @param nof_labels : how many mpls labels in the packet
 * @param channel_type : the channel type of mpls-tp oam packet.
 * @return error code
 */
int cint_field_user_qual_mpls_tp_oam_main(int unit,int in_port,int nof_labels, int channel_type)
{   
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t user_qual_id, user_qual_id2,user_qual_id3;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_format_t ace_format_id;
    bcm_field_context_t qual_header_context_id;
    bcm_field_group_t qual_header_fg_id=0;
    bcm_field_entry_t qual_header_entry_id[2] = {0};
    bcm_field_presel_t qual_header_presel = 5;
    uint32 ace_entry_handle;
    uint32 qual_print;
    void *dest_char;
    bcm_gport_t gport;
    int rv = BCM_E_NONE;
    
    /** 1.  define qualifier */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 20;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "MPLS-TP GAL ", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit,0,&qual_info,&user_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 16; 
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "MPLS-TP ACH ", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &user_qual_id2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }
    
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8; 
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "CCM Opcode", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &user_qual_id3);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /** 2, define ace action */
    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;
    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }
    
    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextMplsTransitCCMSignal;
    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }
    printf("++++++++ace_entry_handle %d +++++++\n", ace_entry_handle);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;

    /* Set quals */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = bcmFieldQualifySrcPort;
    fg_info.qual_types[1] = user_qual_id;
    fg_info.qual_types[2] = user_qual_id2;
    fg_info.qual_types[3] = user_qual_id3;
    
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionAceEntryId;

    /** 3, create group */
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "MPLS TP CCM set SD", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &qual_header_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", qual_header_fg_id);

    /** 4, Attach Ethernet context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Ethernet", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &qual_header_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; 
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1]; 
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2]; 
    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    
    /** qualify 1 InPort*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = 0;
    
    /** qualify 2 GAL label 13*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = nof_labels *32; /** start offset 0, label is 20bits*/
    
    /** qualify 3 ACH --  Channel Type*/
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = nof_labels *32 + 48; /** 32+16*/
    
    /** qualify 4 CCM opcode */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[3].input_arg = 0;
    attach_info.key_info.qual_info[3].offset = nof_labels *32 +72; /** 64+8*/

    rv = bcm_field_group_context_attach(unit, 0, qual_header_fg_id, qual_header_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",qual_header_fg_id,qual_header_context_id);
    
    /**Its better to add entries after attach, for better bank allocation*/
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    /*4-2*/
    ent_info.nof_entry_quals = 4;
    /**in port 13*/
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, in_port);
    ent_info.entry_qual[0].value[0] = gport;
    ent_info.entry_qual[0].mask[0] = 0xFFFF;
    
    /** GAL label 13 */
    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = 13;
    ent_info.entry_qual[1].mask[0] = 0xfffff; /** 20bits*/
    /**  ACH*/
    ent_info.entry_qual[2].type = fg_info.qual_types[2];
    ent_info.entry_qual[2].value[0] = channel_type;
    ent_info.entry_qual[2].mask[0] = 0xffff; /**  16bits*/
    /** CCM Opcode -1*/
    ent_info.entry_qual[3].type = fg_info.qual_types[3];
    ent_info.entry_qual[3].value[0] = 1;
    ent_info.entry_qual[3].mask[0] = 0xff; /** 8bits*/

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, qual_header_fg_id, &ent_info, &(qual_header_entry_id[0]));
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Qual_val:(0x%x) Action_val:(%d)\n",qual_header_entry_id[0],ent_info.entry_qual[0].value[0],
                                                                            ent_info.entry_action[0].value[0]);
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = qual_header_presel;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = qual_header_context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeMpls;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(Egress) context(%d) fwd_layer(mpls) \n",
            qual_header_presel,qual_header_context_id);

    return rv;
}


/**
 *  This example shows P node how to modify the CCM.EI bit in payload. 
 *  1, Create ePMF to qualify packet (format is CCMoGACHoGALoMPLSxoETH)
 *  2, Create LSR
 *  3, Send packet from in port to out port,
 *  4, To see if CCM.EI bit was modified in payload. 
 * @param unit
 * @param in_port
 * @param out_port
 * @param nof_labels
 * @param channel_type
 * @return error code
 */
int cint_oam_mpls_send_signal_in_p_node(int unit,int in_port, int out_port, int nof_labels, int channel_type)
{
    int rv = BCM_E_NONE;

    /** Create ePMF */
    rv = cint_field_user_qual_mpls_tp_oam_main(unit,in_port,nof_labels,channel_type); 
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    /** Create LSR */
    rv = mpls_operations_without_eedb_main(unit,in_port,out_port,1);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    return rv;
}



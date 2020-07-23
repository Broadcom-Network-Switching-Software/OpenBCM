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
 * cint ../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint ../../../src/examples/dnx/field/cint_field_bfd_echo_j2.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id_bfd_echo = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * int is_ipv6 = 0;
 * bcm_field_presel_t ipmf1_presel_id = 50;
 * bcm_field_stage_t ipmf1_stage = bcmFieldStageIngressPMF1;
 * bcm_field_layer_type_t fwd_layer = bcmFieldLayerTypeEth;
 * field_presel_fwd_layer_main(unit, ipmf1_presel_id, ipmf1_stage, fwd_layer, &context_id_bfd_echo, "ctx_ipmf1");
 * cint_field_bfd_echo_main(unit, context_id_bfd_echo, is_ipv6);
 *
 * Configuration example end
 */


bcm_field_group_t cint_field_echo_bfd_fg_id=0;

bcm_field_qualify_t cint_field_bfd_echo_desc_qual_id;
int cint_field_echo_trap_id;

/* indicate the echo BFD traffic take ptch header or not*/
int traffic_with_ptch;

/*
 * create conditioned Direct extraction Field group
 * the condition is hit when ipmf1 FG had a hit, sets on the valid bit of ipmf1 action
 * The action is to update oamOffset bits to the offset of Layer X
 * All other bits in bcmFieldActionOam are set to 0 (run field action pred bcm stage=ipmf1 to see all fields of bcmFieldActionOam)
*/
int cint_field_echo_bfd_de_for_oam_offset(
  int unit,
    bcm_field_context_t context_id_of_ipmf1_tcam_fg,
    bcm_field_group_t fg_id_of_ipmf1_tcam_fg,
    int offset_of_valid_bit_of_ipmf1_tcam_action)
{
    bcm_field_context_t context_id=0; /**Should be same context as TCAM in ipmf1*/
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t zero_qual_id;
    bcm_field_qualify_t hit_qual_id;
    bcm_field_action_info_t action_info;
    bcm_field_action_t oam_action = 0;
    void *dest_char;
    int rv=0;

    /** Create destination action to ignore destination qualifier which is 0*/
    bcm_field_action_info_t_init(&action_info);
    /* oamOffset is at mid of bcmFieldActionOam hence we set the prefix to be 0 */
    action_info.action_type = bcmFieldActionOam;
    action_info.prefix_size = 5;
    action_info.prefix_value = 0;
    action_info.size = 16;
    action_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "OamOffset_bfd", sizeof(action_info.name));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    /* zero qualifiers is used to set LSBs of the ActionOAM to be 0 */
    qual_info.size = 8;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "zero_qual_bfd", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &zero_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "hit_bit_bfd", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &hit_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    rv = bcm_field_action_create(unit, 0, &action_info, &oam_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 3;
    fg_info.stage = bcmFieldStageIngressPMF2;

    fg_info.qual_types[0] = hit_qual_id;
    fg_info.qual_types[1] = zero_qual_id;
    fg_info.qual_types[2] = bcmFieldQualifyLayerRecordOffset;

    fg_info.nof_actions = 1;
    /**Valid bit is hit in iPMF1 TCAM*/
    fg_info.action_with_valid_bit[0] = TRUE;

    fg_info.action_types[0] = oam_action;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Oam_offset_bfd", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0 , &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[0].input_arg  = fg_id_of_ipmf1_tcam_fg;
    /* set the valid bit / condition bit of the action be the valid bit of ipmf1 FG. i.e.
     * if hit occured in iPMF1 the bit will be set to 1
     */
    attach_info.key_info.qual_info[0].offset     = offset_of_valid_bit_of_ipmf1_tcam_action;

    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    /* set 0 for LSB of actionOam */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg  = 0;
    attach_info.key_info.qual_info[1].offset     = 0;

    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerRecordsFwd;
    /**Take the offset from FWD_layer +2 where BFD offset is placed*/
    attach_info.key_info.qual_info[2].input_arg  = 2;
    attach_info.key_info.qual_info[2].offset     = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id_of_ipmf1_tcam_fg, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    return rv;
}
/*
 * Valid the packet take with PTCH header for echo bfd to caculate the oam offset and reduce 2
 * create conditioned Direct extraction Field group
 * the condition is hit when ipmf1 FG had a hit, sets on the valid bit of ipmf1 action
 * 1) The action is to update oamOffset bits to the offset of Layer X
 * 2) Use FEM to reduce 2 bytes ptch for OAM_OFFSET
 * 3) All other bits in bcmFieldActionOam are set to 0 (run field action pred bcm stage=ipmf1 to see all fields of bcmFieldActionOam)
*/

int cint_field_echo_bfd_de_for_oam_offset_with_ptch(
  int unit,
    bcm_field_context_t context_id_of_ipmf1_tcam_fg,
    bcm_field_group_t fg_id_of_ipmf1_tcam_fg,
    int offset_of_valid_bit_of_ipmf1_tcam_action)
{
    bcm_field_context_t context_id=0; /**Should be same context as TCAM in ipmf1*/
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t zero_qual_id;
    bcm_field_qualify_t hit_qual_id;
    bcm_field_action_info_t action_info;
    bcm_field_action_t oam_action = 0;
    bcm_field_fem_action_info_t  fem_action_info;
    int mapping_bits_index, conditions_index, extractions_index =0;
    bcm_field_action_t fem_increment_void_action_id;
    bcm_field_action_priority_t action_priority;
    void *dest_char;
    int rv=0;

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "fem_increment", sizeof(action_info.name));
    action_info.size = 9; /*1bit hit + 8bits layer_offset*/
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    rv = bcm_field_action_create(unit, 0, &action_info, &fem_increment_void_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "hit_bit_bfd", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &hit_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 2;
    fg_info.stage = bcmFieldStageIngressPMF2;

    fg_info.qual_types[0] = hit_qual_id;
    fg_info.qual_types[1] = bcmFieldQualifyLayerRecordOffset;

    fg_info.nof_actions = 1;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[0] = fem_increment_void_action_id;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Oam_offset_bfd", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN , &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /** FEM actions. */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;
    fem_action_info.condition_msb = 3;

    for (conditions_index = 0; conditions_index < 16; conditions_index++)
    {
        fem_action_info.fem_condition[conditions_index].extraction_id = 0 ;
         fem_action_info.fem_condition[conditions_index].is_action_valid = conditions_index & 1; /*hit valid bit*/
    }

    fem_action_info.fem_extraction[extractions_index].action_type =bcmFieldActionOam;
    fem_action_info.fem_extraction[extractions_index].increment =0xfe00;/*offset -2, ptch-2, LSB don't care*/
    /* Oam-Up-Mep(1), Oam-Sub-Type(4), Oam-Offset(8), Oam-Stamp-Offset(8)*/
    /*LSB[7:0] == 0*/
    for (mapping_bits_index = 0; mapping_bits_index <= 7; mapping_bits_index++)
    {
        fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset = 0;
        fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value = 0;
        fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
    }
    /*Oam-Offset[7:0] extract from key [8:1]*/
    for (mapping_bits_index = 8; mapping_bits_index <= 15; mapping_bits_index++)
    {
        fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset = mapping_bits_index -7 ; /*key 1~8*/
        fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value = 0;
        fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
    }
    /*MSB[20:16] == 0*/
    for (mapping_bits_index = 16; mapping_bits_index <= 20;mapping_bits_index++)
    {
        fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset = 0;
        fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value = 0;
        fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
    }

    /** We choose FEM 10, because this FEM is not used by any applications and it is 24 bits, where increment can be performed. */
    action_priority = BCM_FIELD_ACTION_POSITION(3, 2);
    rv = bcm_field_fem_action_add(unit,0 , fg_id, action_priority, &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_add\n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[0].input_arg  = fg_id_of_ipmf1_tcam_fg;
    /* set the valid bit / condition bit of the action be the valid bit of ipmf1 FG. i.e.
     * if hit occured in iPMF1 the bit will be set to 1
     */
    attach_info.key_info.qual_info[0].offset     = offset_of_valid_bit_of_ipmf1_tcam_action;

    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    /**Take the offset from FWD_layer +2 where BFD offset is placed*/
    attach_info.key_info.qual_info[1].input_arg  = 2;
    attach_info.key_info.qual_info[1].offset     = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id_of_ipmf1_tcam_fg, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    return rv;
}

/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] context_id_bfd_echo  -  Context id to which attach the info to
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_bfd_echo_main(int unit,bcm_field_context_t context_id_bfd_echo, int is_ipv6)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual_id;
    void *dest_char;
    int index;
    int rv = BCM_E_NONE;
    int offset;
    int ii;

    bcm_field_qualifier_info_create_t_init(&qual_info);

    qual_info.size = 16;

    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Discriminator", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_bfd_echo_desc_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = is_ipv6 ? bcmFieldQualifyDstIp6 : bcmFieldQualifyDstIp;
    fg_info.qual_types[1] = bcmFieldQualifyL4DstPort;
    fg_info.qual_types[2] = cint_field_bfd_echo_desc_qual_id;
    fg_info.qual_types[3] = bcmFieldQualifyForwardingLayerIndex;

    /* Set actions */
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.action_types[1] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_types[2] = bcmFieldActionForwardingLayerIndex;

    dest_char = &(fg_info.name[0]);

    if (is_ipv6) {
        sal_strncpy_s(dest_char, "OAM_BFD_ECHO_IPv6", sizeof(fg_info.name));
    } else {
        sal_strncpy_s(dest_char, "OAM_BFD_ECHO", sizeof(fg_info.name));
    }

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_echo_bfd_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_echo_bfd_fg_id);



    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Packet assumed to be Forwarded by IPv4/6 Layer*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    /**Layer above IPv4 is UDP*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;

    /*Discriminator taken from PDU at offset 10*/
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[2].input_arg = 2;
    attach_info.key_info.qual_info[2].offset = 80;

    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    
    rv = bcm_field_group_context_attach(unit, 0, cint_field_echo_bfd_fg_id, context_id_bfd_echo, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",cint_field_echo_bfd_fg_id,context_id_bfd_echo);

    rv = bcm_field_group_action_offset_get(unit,0,cint_field_echo_bfd_fg_id,bcmFieldActionTrap,&offset);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }

    /* calculate oam offset for echo BFD */
    if (traffic_with_ptch) {
        /*traffice take ptch header, then oam_offset should reduce by 2*/
        rv = cint_field_echo_bfd_de_for_oam_offset_with_ptch(unit,context_id_bfd_echo,cint_field_echo_bfd_fg_id,offset-1);
    }
    else{
        /*traffice doesn't take ptch header as it's from NIF*/
        rv = cint_field_echo_bfd_de_for_oam_offset(unit,context_id_bfd_echo,cint_field_echo_bfd_fg_id,offset-1);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_echo_bfd_de_for_oam_offset\n", rv);
        return rv;
    }

    rv = bcm_rx_trap_type_get(unit,0, is_ipv6 ? bcmRxTrapOamBfdIpv6 : bcmRxTrapOamBfdIpv4, &cint_field_echo_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_rx_trap_type_get\n", rv);
        return rv;
    }

    return 0;
}

void cint_field_bfd_echo_reverse_memcpy(void *dst, void* src, int size) {
    int i;
    uint32 *destination = dst;
    uint8 *source = src;
    sal_memset(destination, 0 , size);
    for (i = 0; i < size; ++i)
        destination[size/4-1-(i/4)] |= (source[i] << (8 * (3 - (i & 3))));
}

/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \param [in] dest_ip      -  Dest ip value to set in entry add
* \param [in] local_discr  -  Discriminator value to set in entry
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/

int cint_field_bfd_echo_entry_add(int unit, uint32 dest_ip, bcm_ip6_t *dest_ip6, uint16 local_discr)
{
    bcm_field_entry_info_t ent_info;
    bcm_field_entry_t cint_bfd_echo_entry_id;
    int trap_action_value = 0;
    int max_trap_str = 15;
    bcm_gport_t trap_gport;
    int rv = BCM_E_NONE;
    int is_ipv6 = (dest_ip6 != NULL);
    int sizeof_ipv6 = 16;
    int ii;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = local_discr;
    ent_info.nof_entry_quals = 4;
    ent_info.entry_qual[0].type = is_ipv6 ? bcmFieldQualifyDstIp6 : bcmFieldQualifyDstIp;

    if (is_ipv6) {
        cint_field_bfd_echo_reverse_memcpy(ent_info.entry_qual[0].value, dest_ip6, 16);
        sal_memset(ent_info.entry_qual[0].mask, 0xff, 16);
    } else {
        ent_info.entry_qual[0].value[0] = dest_ip;
        ent_info.entry_qual[0].mask[0] = 0xffffffff;
    }

    ent_info.entry_qual[1].type = bcmFieldQualifyL4DstPort;
    ent_info.entry_qual[1].value[0] = 0x0ec9;
    ent_info.entry_qual[1].mask[0] = 0xffff;

    ent_info.entry_qual[2].type = cint_field_bfd_echo_desc_qual_id;
    ent_info.entry_qual[2].value[0] = local_discr;
    ent_info.entry_qual[2].mask[0] = 0xffff;

    ent_info.entry_qual[3].type = bcmFieldQualifyForwardingLayerIndex;
    ent_info.entry_qual[3].mask[0] = 0x7;

    ent_info.nof_entry_actions = 3;
    ent_info.entry_action[0].type = bcmFieldActionTrap;
    BCM_GPORT_TRAP_SET(trap_gport, cint_field_echo_trap_id, max_trap_str, 0);
    /*trap_action_value = (local_discr << 13) + (max_trap_str << 9) + cint_field_echo_trap_id;*/
    
    ent_info.entry_action[0].value[0] = trap_gport;
    ent_info.entry_action[0].value[1] = local_discr;

    ent_info.entry_action[1].type = bcmFieldActionEgressForwardingIndex;
    ent_info.entry_action[2].type = bcmFieldActionForwardingLayerIndex;


    for (ii = 0; ii < 8; ii++) {
        ent_info.priority = ii;
        ent_info.entry_qual[3].value[0] = ii;
        /* BFD index = IPv6 index + 2 */
        ent_info.entry_action[1].value[0] = (ii + 2) % 8;
        ent_info.entry_action[2].value[0] = (ii + 2) % 8;
        rv = bcm_field_entry_add(unit, 0, cint_field_echo_bfd_fg_id, &ent_info, &(cint_bfd_echo_entry_id));
        if (rv != BCM_E_NONE)
        {
           printf("Error (%d), in bcm_field_entry_add\n", rv);
           return rv;
        }
    }
    printf("Entry add: id:(0x%x) ", cint_bfd_echo_entry_id);

    if (is_ipv6) {
        printf("dest_ip: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ",
                 dest_ip6[0], dest_ip6[1],
                 dest_ip6[2], dest_ip6[3],
                 dest_ip6[4], dest_ip6[5],
                 dest_ip6[6], dest_ip6[7],
                 dest_ip6[8], dest_ip6[9],
                 dest_ip6[10], dest_ip6[11],
                 dest_ip6[12], dest_ip6[13],
                 dest_ip6[14], dest_ip6[15]);
    } else {
        printf("dest_ip:(0x%x) ", ent_info.entry_qual[0].value[0]);
    }

    printf("L4Port:(%d) Desc(%d) Trap_Action (%d) \n",ent_info.entry_qual[1].value[0],
                                                      ent_info.entry_qual[2].value[0],
                                                      ent_info.entry_action[0].value[0]);
    return 0;
}

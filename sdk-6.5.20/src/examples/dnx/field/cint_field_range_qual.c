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
 * cint ../../../src/examples/dnx/field/cint_field_range_qual.c
 * int unit = 0;
 * bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * cint_field_range_qual_l4_ops_main(unit, stage, context_id);
 *
 * Configuration example end
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_range_qual.c
 * exit
 * ************************************************************************
 *
 * This CINT shows how to set a Range in L4 Dest and Src Port/Packet Header Size.
 * and use it as a qualifier.
 *
 * Configuration steps:
 * 1.Set a Range using bcm_field_range_set for needed Range Type(Packet Header Size or L4 SRC/DST Ports).
 * 2.Add a Fg with qualifier bcmFieldQualifyPacketLengthRangeCheck for packet header size and
 * bcmFieldQualifyL4PortRangeCheck for L4 SRC/DST ports.
 * 3.Attach the Fg to a context.
 * 4.Add an entry to the FG.
 *
 * Main functions:
 * cint_field_range_qual_packet_header_size_main(unit,stage,context_id);
 * cint_field_range_qual_l4_ops_main(unit,stage,context_id);
 *
 * Destroy function:
 * cint_field_range_qual_destroy(unit,stage,range_type,context_id);
 */

/**
  * Global variables
  * {
  */
  /**
  * The entry_id. Used for creating and destroying the entry
  */
bcm_field_entry_t CINT_FIELD_RANGE_QUAL_ENTRY_ID;
/**
 * The range_info. Used for creating and destroying a range.
 */
bcm_field_range_info_t range_info;
/**
 * The ID of the FG. Used for creating and for deleting.
 */
bcm_field_group_t CINT_FIELD_RANGE_QUAL_FG_ID;
/**
 * The Range ID of the packet header size. Used for creating and reseting a range.
 */
bcm_field_range_t CINT_FIELD_RANGE_ID_PACKET_HEADER_SIZE = 1;
/**
 * The Range ID of L4 Ports. Used for creating and reseting a range.
 */
bcm_field_range_t CINT_FIELD_RANGE_ID_L4_PORT = 0;
/**
 * Global variables
 * }
 */
/**
 * \brief
 *
 *  This function sets all required HW
 *  configuration for bcm_field_group_add and bcm_field_group_context_attach
 *  and bcm_field_entry_add
 *  to be performed for IPMF1, IPMF2 and EPMF.
 *
 * \param [in] unit - The unit number.
 * \param [in] qual_type - The type of the qualifier used in the
 * bcm_field_group_add, bcm_field_group_context_attach, bcm_field_entry_add
 * \param [in] context_id - The ID of the context
 * \param [in] stage - The PMF stage
 * \param [in] qual_value - The value to qualify upon in the entry
 * \param [in] mask_value - The mask to qualify upon in the entry
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_range_fg_create_context_attach_entry_add(
    int unit,
    bcm_field_qualify_t qual_type,
    bcm_field_context_t context_id,
    bcm_field_stage_t stage,
    uint32 qual_value,
    uint32 mask_value)
{
    char *proc_name;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_range_fg_create_context_attach_entry_add";
    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);
    /**
     * Fill the fg_info structure
     */
    fg_info.stage = stage;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    fg_info.qual_types[0] = qual_type;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "range_qual_FG", sizeof(fg_info.name));
    /**
     * Add the FG.
     */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &CINT_FIELD_RANGE_QUAL_FG_ID);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    /**
     * Init the attach_info.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    /**
     * Fill the attach_info structure.
     */
    attach_info.key_info.nof_quals = 1;
    attach_info.payload_info.nof_actions = 1;
    attach_info.key_info.qual_types[0] = qual_type;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.action_types[0] = bcmFieldActionDropPrecedence;
    /**
     * Attach the FG to the context.
     */
    rv = bcm_field_group_context_attach(unit, 0, CINT_FIELD_RANGE_QUAL_FG_ID, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
   /**
    * Fill the entry_info.
    */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.priority = 1;

    entry_info.nof_entry_quals = 1;
    entry_info.entry_qual[0].type = qual_type;
    entry_info.entry_qual[0].value[0] = qual_value;
    entry_info.entry_qual[0].mask[0] = mask_value;

    entry_info.nof_entry_actions = 1;
    entry_info.entry_action[0].type = bcmFieldActionDropPrecedence;
    entry_info.entry_action[0].value[0] = BCM_FIELD_COLOR_YELLOW;
    /**
     * Add an entry to the FG
     */
    rv = bcm_field_entry_add(unit, 0, CINT_FIELD_RANGE_QUAL_FG_ID, &entry_info, &CINT_FIELD_RANGE_QUAL_ENTRY_ID);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add entry_id(%d)\n", rv, CINT_FIELD_RANGE_QUAL_ENTRY_ID);
        return rv;
    }
    printf("FG %d was created and attached to the context %d\n", CINT_FIELD_RANGE_QUAL_FG_ID, context_id);
            return rv;
    return rv;
}
/**
 *
 * \brief
 *
 *  This function sets the range for bcmFieldRangeTypePacketHeaderSize range type.
 *
 * \param [in] unit - The unit number.
 * \param [in] stage - The Field stage.
 * \param [in] context_id - The Id of the context
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_range_qual_packet_header_size_main(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{
    bcm_field_range_info_t range_info;
    char *proc_name;
    uint32 qual_value;
    uint32 mask_value;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_range_qual_packet_header_size_main";
    /**
     * Init the fg_info structure.
     * Values are in Byte resolution
     */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypePacketHeaderSize;
    range_info.min_val = 75;
    range_info.max_val = 83;
    /**
     * Set the range.
     */
    rv = bcm_field_range_set(unit, 0, stage, CINT_FIELD_RANGE_ID_PACKET_HEADER_SIZE, &range_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_range_set\n", proc_name, rv);
        return rv;
    }
    /**
     * For packet_header_size ranges, qualifier is the value of the first range ID to which the packet_header_size fits.
     */
    qual_value = CINT_FIELD_RANGE_ID_PACKET_HEADER_SIZE;
    mask_value = 0x3;
    /**
     * Create a FG, attach it to a context and add an entry.
     */
    rv = cint_field_range_fg_create_context_attach_entry_add(unit, bcmFieldQualifyPacketLengthRangeCheck, context_id, stage, qual_value, mask_value);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_range_fg_create_context_attach_entry_add\n", proc_name, rv);
        return rv;
    }
    return rv;
}
/**
 *
 * \brief
 *
 *  This function is setting a range for L4 Ops.
 *
 * \param [in] unit - The unit number.
 * \param [in] range_type - The type of the range set in the TCL test.
 * Can be bcmFieldRangeTypeL4SrcPort or bcmFieldRangeTypeL4DstPort
 * \param [in] stage - The field stage
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_range_qual_l4_ops_main(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{
    bcm_field_range_info_t range_info;
    char *proc_name;
    uint32 qual_value;
    uint32 mask_value;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_range_qual_l4_ops_main";
    /**
     * Init the fg_info structure.
     */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeL4SrcPort;
    range_info.min_val = 200;
    range_info.max_val = 2000;
    rv = bcm_field_range_set(unit, 0, stage, CINT_FIELD_RANGE_ID_L4_PORT, &range_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s Error (%d), in bcm_field_range_set\n", proc_name, rv);
       return rv;
    }
    /**
     * Init the fg_info structure.
     */
    bcm_field_range_info_t_init(&range_info);
    range_info.range_type = bcmFieldRangeTypeL4DstPort;
    range_info.min_val = 200;
    range_info.max_val = 2000;
    rv = bcm_field_range_set(unit, 0, stage, CINT_FIELD_RANGE_ID_L4_PORT, &range_info);
    if (rv != BCM_E_NONE)
    {
       printf("%s Error (%d), in bcm_field_range_set\n", proc_name, rv);
       return rv;
    }
    /**
     * For L4 port ranges, the bit within the bitmap corresponding to a range_id is the range_id istelf.
     */
    qual_value = 1 << CINT_FIELD_RANGE_ID_L4_PORT;
    mask_value = 1 << CINT_FIELD_RANGE_ID_L4_PORT;
    /**
     * Create a FG, attach it to a context and add an entry.
     */
    rv = cint_field_range_fg_create_context_attach_entry_add(unit, bcmFieldQualifyL4PortRangeCheck, context_id, stage, qual_value, mask_value);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_range_fg_create_context_attach_entry_add\n", proc_name, rv);
        return rv;
    }
    return rv;
}
/**
 * The destroy function of the test. It is deleting the whole configuration.
 *
 * \param [in] unit - The unit number.
 * \param [in] stage - The Field stage.
 * \param [in] context_id - The ID of the context.
 *
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_range_qual_destroy(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_range_type_t range_type,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    /**
     *  Set the range with default values
     */
    if (range_type == bcmFieldRangeTypePacketHeaderSize)
    {
        bcm_field_range_info_t_init(&range_info);
        range_info.range_type = range_type;
        rv = bcm_field_range_set(unit, 0, stage, CINT_FIELD_RANGE_ID_PACKET_HEADER_SIZE, &range_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_range_set\n",rv);
            return rv;
        }
    }
    if (range_type == bcmFieldRangeTypeL4SrcPort || range_type == bcmFieldRangeTypeL4DstPort)
    {
        bcm_field_range_info_t_init(&range_info);
        range_info.range_type = bcmFieldRangeTypeL4SrcPort;
        rv = bcm_field_range_set(unit, 0, stage, CINT_FIELD_RANGE_ID_L4_PORT, &range_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_range_set\n",rv);
            return rv;
        }
        bcm_field_range_info_t_init(&range_info);
        range_info.range_type = bcmFieldRangeTypeL4DstPort;
        rv = bcm_field_range_set(unit, 0, stage, CINT_FIELD_RANGE_ID_L4_PORT, &range_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_range_set\n",rv);
            return rv;
        }
    }
    /**
     * Delete the created entry.
     */
    rv = bcm_field_entry_delete(unit, CINT_FIELD_RANGE_QUAL_FG_ID, NULL, CINT_FIELD_RANGE_QUAL_ENTRY_ID);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }
    /**
     * Detach the context.
     */
    rv = bcm_field_group_context_detach(unit, CINT_FIELD_RANGE_QUAL_FG_ID, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_dnx_field_group_context_detach\n", rv);
        return rv;
    }
    /**
     * Delete the FG.
     */
    rv = bcm_field_group_delete(unit, CINT_FIELD_RANGE_QUAL_FG_ID);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
    return rv;
}

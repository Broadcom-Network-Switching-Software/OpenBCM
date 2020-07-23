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
 * cint ../../../src/examples/dnx/internal/field/cint_field_vw.c
 * cint;
 * int unit = 0;
 * int ipmf1_ctx = 0;
 * int ipmf3_ctx = 0;
 * field_vw_main(unit,ipmf1_ctx,ipmf2_ctx);
 *
 * An example of using virtual wires in PMF.
 * 1. Collect the destination IP address of the packet in iPMF2 and load it unto a virtual wire.
 * 2. Read the destination IP address in iPMF3 and perform an action based on it.
 */

bcm_field_group_t Cint_field_vw_fg_id_ipmf2 = BCM_FIELD_ID_INVALID;
bcm_field_group_t Cint_field_vw_fg_id_ipmf3 = BCM_FIELD_ID_INVALID;
bcm_field_entry_t Cint_field_vw_entry_id_ipmf3 = BCM_FIELD_ID_INVALID;

int field_vw_ipmf2(
    int unit,
    bcm_field_context_t context_id_ipmf2,
    int ipv4_header_layer,
    int ipv4_header_offset_in_layer)
{
    int rv = BCM_E_NONE;
    char *virtual_wire_name;
    bcm_field_name_to_id_info_t name_to_id_info;
    uint32 ids[BCM_FIELD_NAME_TO_ID_MAX_IDS];
    bcm_field_action_t vw_action_0;
    bcm_field_action_t vw_action_1;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int nof_ids;
    void *dest_char;
    char *proc_name;

    proc_name = "field_vw_ipmf2";
    virtual_wire_name = "vw_PMF_TEST___ACTION";

    /*
     * Get the two actions from the virtual wire.
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    dest_char = &(name_to_id_info.name[0]);
    sal_strncpy_s(dest_char, virtual_wire_name, sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAction;
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, ids);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_name_to_id\n", proc_name, rv);
        return rv;
    }
    if (nof_ids != 2)
    {
        printf("%s bcm_field_name_to_id returned %d IDs, expected 2.\n", proc_name, nof_ids);
        return BCM_E_CONFIG;
    }
    vw_action_0 = ids[0];
    vw_action_1 = ids[1];

    /*
     * Create a direct extraction field group to copy the IP addresses to the virtual wire.
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = vw_action_0;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = vw_action_1;
    fg_info.action_with_valid_bit[1] = FALSE;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_vw_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = ipv4_header_layer;
    attach_info.key_info.qual_info[0].offset = ipv4_header_offset_in_layer;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1]; 
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = ipv4_header_layer;
    attach_info.key_info.qual_info[1].offset = ipv4_header_offset_in_layer;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_vw_fg_id_ipmf2, context_id_ipmf2, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int field_vw_ipmf3(
    int unit,
    bcm_field_context_t context_id_ipmf3,
    int ipv4_header_layer,
    int ipv4_header_offset_in_layer)
{
    int rv = BCM_E_NONE;
    char *virtual_wire_name;
    bcm_field_name_to_id_info_t name_to_id_info;
    uint32 ids[BCM_FIELD_NAME_TO_ID_MAX_IDS];
    bcm_field_qualify_t vw_qual;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    int nof_ids;
    void *dest_char;
    char *proc_name;

    proc_name = "field_vw_ipmf3";
    virtual_wire_name = "vw_PMF_TEST___ACTION";

    /*
     * Get the qualifier from the virtual wire.
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    dest_char = &(name_to_id_info.name[0]);
    sal_strncpy_s(dest_char, "vw_PMF_TEST___ACTION", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, ids);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_name_to_id\n", proc_name, rv);
        return rv;
    }
    vw_qual = ids[0];

    /*
     * Create a TCAM field group to qualify upon the addresses.
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = vw_qual;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionStartPacketStrip;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_vw_fg_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    rv = bcm_field_group_context_attach(unit, 0, Cint_field_vw_fg_id_ipmf3, context_id_ipmf3, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
    
    /*
     * Create an example TCAM entry.
     */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    /** Source IP address*/
    entry_info.entry_qual[0].value[0] = 0x02020202;
    entry_info.entry_qual[0].mask[0] = 0xFFFFFFFF;
    /** Destination IP address*/
    entry_info.entry_qual[0].value[1] = 0x01010101;
    entry_info.entry_qual[0].mask[1] = 0xFFFFFFFF;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 1;
    entry_info.entry_action[0].value[1] = 20;
    rv = bcm_field_entry_add(unit, 0, Cint_field_vw_fg_id_ipmf3, &entry_info, &Cint_field_vw_entry_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 * Configure iPMF2 and iPMF3 to collect the destination IP address and remove bytes from the packet accordingly.
 *
 * \param [in] unit                        - Device id
 * \param [in] context_id_ipmf2            - The iPMF2 context.
 * \param [in] context_id_ipmf3            - The iPMF3 context.
 * \param [in] ipv4_header_layer           - The layer in which the IPv4 header is found.
 * \param [in] ipv4_header_offset_in_layer - The offset within the layer to the beginning of the IPv4 header.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int field_vw_main(
    int unit,
    bcm_field_context_t context_id_ipmf2,
    bcm_field_context_t context_id_ipmf3,
    int ipv4_header_layer,
    int ipv4_header_offset_in_layer)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "field_vw_main";

    rv = field_vw_ipmf2(unit, context_id_ipmf2, ipv4_header_layer, ipv4_header_offset_in_layer);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in field_vw_ipmf2\n", proc_name, rv);
        return rv;
    }

    rv = field_vw_ipmf3(unit, context_id_ipmf3, ipv4_header_layer, ipv4_header_offset_in_layer);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in field_vw_ipmf2\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int field_vw_destroy(
    int unit,
    bcm_field_context_t context_id_ipmf2,
    bcm_field_context_t context_id_ipmf3)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "field_vw_destroy";

    rv = bcm_field_group_context_detach(unit, Cint_field_vw_fg_id_ipmf2, context_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_context_detach.\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_group_delete(unit, Cint_field_vw_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_delete.\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, Cint_field_vw_fg_id_ipmf3, NULL, Cint_field_vw_entry_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_entry_delete.\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, Cint_field_vw_fg_id_ipmf3, context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_context_detach.\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_group_delete(unit, Cint_field_vw_fg_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("%s: Error (%d), in bcm_field_group_delete.\n", proc_name, rv);
        return rv;
    }

    return rv;
}

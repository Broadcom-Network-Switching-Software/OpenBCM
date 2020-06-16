/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_user_qual_ethertype.c
 * Purpose: Shows an example for configuration of user qualifier on ethertype.
 *          Relevant for untagged and tagged ethernet packets.
 *
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dnx/field/cint_field_always_hit_context.c
 * cint ../../../../src/examples/dnx/field/cint_field_user_qual_ethertype.c
 * cint;
 * int unit = 0;
 * cint_field_user_qual_ethertype_main(unit, 0x800);
 *
 * Configuration example end
 */

bcm_field_presel_t cint_field_user_qual_ethertype_presel_id = 3;
bcm_field_context_t cint_field_user_qual_ethertype_context_id = 0;
bcm_field_group_t cint_field_user_qual_ethertype_fg_id = 0;
bcm_field_entry_t cint_field_user_qual_ethertype_ent_id;
bcm_field_qualify_t cint_field_user_qual_ethertype_id;

/**
* \brief
*  Configure IPMF1 user qualifier on ethertype.
* \param [in] unit        -  Device id
* \param [in] ether_type  -  ethernet type
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_user_qual_ethertype_main(int unit, int ether_type)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_create_t etype_qual_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    rv = cint_field_always_hit_context_main(unit, cint_field_user_qual_ethertype_presel_id, bcmFieldStageIngressPMF1, &cint_field_user_qual_ethertype_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_always_hit_context_main\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&etype_qual_info);
    etype_qual_info.size = 16;
    dest_char = &(etype_qual_info.name[0]);
    sal_strncpy_s(dest_char, "Etype_qual", sizeof(etype_qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &etype_qual_info, &cint_field_user_qual_ethertype_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    /*
     * Create and attach TCAM group in IPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_user_qual_ethertype_id;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    printf("Creating IPMF1 FG\n");
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_user_qual_ethertype_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    /* The Ethertype is always in last 16 bits of the Ethernet layer (layer 0) */
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = -16;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];


    printf("Attaching IPMF1 FG\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_field_user_qual_ethertype_fg_id, cint_field_user_qual_ethertype_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf ("TCAM FG_ID %d is attached to Context %d at stage iPMF1\n", cint_field_user_qual_ethertype_fg_id, cint_field_user_qual_ethertype_context_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = ether_type;
    ent_info.entry_qual[0].mask[0] = 0xFFFF;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 3;

    printf("Adding Entry to the IPMF1 FG\n");
    rv = bcm_field_entry_add(unit, 0, cint_field_user_qual_ethertype_fg_id, &ent_info, &cint_field_user_qual_ethertype_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry add: id:(0x%x)  Qual_val:(0x%x) Action_val:(%d)\n", cint_field_user_qual_ethertype_ent_id, ent_info.entry_qual[0].value[0],
                                                                      ent_info.entry_action[0].value[0]);

    return rv;
}

/**
* \brief
*  Destroy all configuration done for IPMF1 user qualifier on ethertype.
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_user_qual_ethertype_destroy(int unit)
{
    int rv = BCM_E_NONE;

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, cint_field_user_qual_ethertype_fg_id, NULL, cint_field_user_qual_ethertype_ent_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, cint_field_user_qual_ethertype_ent_id, cint_field_user_qual_ethertype_fg_id);
       return rv;
    }
    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_field_user_qual_ethertype_fg_id, cint_field_user_qual_ethertype_context_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_field_user_qual_ethertype_fg_id, cint_field_user_qual_ethertype_context_id);
       return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, cint_field_user_qual_ethertype_fg_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_field_user_qual_ethertype_fg_id);
       return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, cint_field_user_qual_ethertype_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy\n", rv);
        return rv;
    }

    rv = cint_field_always_hit_contex_destroy(unit, cint_field_user_qual_ethertype_presel_id, bcmFieldStageIngressPMF1, cint_field_user_qual_ethertype_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_always_hit_context_destroy\n", rv);
        return rv;
    }

    return rv;
}

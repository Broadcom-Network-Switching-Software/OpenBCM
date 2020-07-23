/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * This cint shows how to qualify on IPv6 fragmented non first in IPv6 LR Qualifier.
 *
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_ipv6_fragmented_non_first.c
 * cint;
 * int unit = 0;
 * bcm_field_stage_t field_stage = bcmFieldStageIngressPMF1;
 * int input_arg = 1;
 * uint32 non_first_fragmented = 1;
 * cint_field_ipv6_fragmented_non_first_main(unit, field_stage, input_arg, non_first_fragmented);
 *
 * Configuration example end
 */

int Cint_field_ipv6_fragmented_non_first_tc_action_val = 4;

bcm_field_group_t Cint_field_ipv6_fragmented_non_first_fg_id = 0;
bcm_field_entry_t Cint_field_ipv6_fragmented_non_first_entry_handle;

/**
 * \brief
 *  This function creates a TCAM field group,
 *  attaching it to a context and adding an entry.
 *
 * \param [in] unit   - Device id
 * \param [in] field_stage  - Field stage
 * \param [in] input_arg  - Attach info input argument, indicates
 *  where the IPv6 header resides in the packet.
 * \param [in] non_first_fragmented  - 0 or 1, depends on the test scenario.
 *  Indicates the qualifier value.
 *      - 0 indicates that an IPv6 Layer were the IPv6 Header is EITHER:
 *          - NOT Fragmented (i.e. there is no fragment extension)
 *          - Fragmented and this IS the 1st Fragment
 *      - 1 indicates that an IPv6 Layer were the IPv6 Header is
 *        fragmented and this IS NOT 1st Fragment.
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_ipv6_fragmented_non_first_main(
    int unit,
    bcm_field_stage_t field_stage,
    int input_arg,
    uint32 non_first_fragmented)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = field_stage;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyIPv6FragmentedNonFirst;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_ipv6_fragmented_non_first_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Field Group ID %d was created. \n", Cint_field_ipv6_fragmented_non_first_fg_id);

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
    attach_info.key_info.qual_info[0].input_arg = input_arg;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, Cint_field_ipv6_fragmented_non_first_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_ipv6_fragmented_non_first_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT);

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = non_first_fragmented;
    entry_info.entry_qual[0].mask[0] = 0x1;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = Cint_field_ipv6_fragmented_non_first_tc_action_val;

    rv = bcm_field_entry_add(unit, 0, Cint_field_ipv6_fragmented_non_first_fg_id, &entry_info, &Cint_field_ipv6_fragmented_non_first_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", Cint_field_ipv6_fragmented_non_first_entry_handle,
            Cint_field_ipv6_fragmented_non_first_entry_handle, Cint_field_ipv6_fragmented_non_first_fg_id);

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit         - Device id
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_ipv6_fragmented_non_first_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, Cint_field_ipv6_fragmented_non_first_fg_id, NULL, Cint_field_ipv6_fragmented_non_first_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, Cint_field_ipv6_fragmented_non_first_fg_id);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, Cint_field_ipv6_fragmented_non_first_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach FG %d from default context\n", rv, Cint_field_ipv6_fragmented_non_first_fg_id);
        return rv;
    }

    rv = bcm_field_group_delete(unit, Cint_field_ipv6_fragmented_non_first_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  FG %d \n", rv, Cint_field_ipv6_fragmented_non_first_fg_id);
        return rv;
    }

    return rv;
}

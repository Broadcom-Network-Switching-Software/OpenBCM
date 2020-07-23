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
 * cint ../../../src/examples/dnx/field/cint_field_port_general_data.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * int in_port = 200;
 * uint32 general_data_lsb = 0xff5;
 * uint32 general_data_msb = 0x3d;
 * field_port_general_data_main(unit, context_id, in_port, general_data_lsb, general_data_msb);
 *
 * Configuration example end
 *
 *   This cint shows an example of using per PP port general data.
 */


/** Values saved for delete.*/
bcm_field_group_t Port_general_data_fg_id;
bcm_field_entry_t Port_general_data_entry_handle;
bcm_field_entry_t Port_general_data_entry_handle_default;
uint32 Port_general_data_lsb_old_value;
uint32 Port_general_data_msb_old_value;


/**
* \brief
*  Configures and uses a port general data
* \param [in] unit             - Device ID
* \param [in] context_id       - Context to attach the FG to.
* \param [in] in_port          - The port whose general data we change.
* \param [in] general_data_lsb - The 32 lsb of the general data.
* \param [in] general_data_msb - The msb of the general data.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_port_general_data_main(
    int unit,
    bcm_field_context_t context_id, 
    int in_port,
    uint32 general_data_lsb,
    uint32 general_data_msb)
{
    bcm_gport_t gport_in;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_entry_info_t entry_info;
    void * dest_char;
    int rv = BCM_E_NONE;

    /* Make a Gport out of the in_port.*/
    BCM_GPORT_LOCAL_SET(gport_in, in_port);
    

    /* Get the old Port_general_data value to be resumed after delete.*/
    rv = bcm_port_class_get(unit, gport_in, bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData, &Port_general_data_lsb_old_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_class_get bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData\n", rv);
        return rv;
    }
    rv = bcm_port_class_get(unit, gport_in, bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh, &Port_general_data_msb_old_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_class_get bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh\n", rv);
        return rv;
    }

    /* Set the general data.*/
    rv = bcm_port_class_set(unit, gport_in, bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData, general_data_lsb);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_class_get bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData\n", rv);
        return rv;
    }
    rv = bcm_port_class_set(unit, gport_in, bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh, general_data_msb);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_class_get bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh\n", rv);
        return rv;
    }

    /* Configure the field group.*/
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.qual_types[0] = bcmFieldQualifyPortClassPacketProcessingGeneralData;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Port_general_data", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Port_general_data_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    rv = bcm_field_group_context_attach(unit, 0, Port_general_data_fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = general_data_lsb;
    entry_info.entry_qual[0].value[1] = general_data_msb;
    entry_info.entry_qual[0].mask[0] = 0xffffff;
    entry_info.entry_qual[0].mask[1] = 0xff;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 2;
    rv = bcm_field_entry_add(unit, 0, Port_general_data_fg_id, &entry_info, &Port_general_data_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    /* Create a default entry for the case of an error.*/
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0;
    entry_info.entry_qual[0].value[1] = 0;
    entry_info.entry_qual[0].mask[0] = 0;
    entry_info.entry_qual[0].mask[1] = 0;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 1;
    rv = bcm_field_entry_add(unit, 0, Port_general_data_fg_id, &entry_info, &Port_general_data_entry_handle_default);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    
    
    return rv;
    
}

/**
* \brief
*  Delete the configurations.
* \param [in] unit        - Device ID
* \param [in] context_id  - Context to detach the field group from.
* \param [in] in_port     - The port whose general data we changed.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_port_general_data_destroy(
    int unit,
    bcm_field_context_t context_id,
    int in_port)
{
    bcm_gport_t gport_in;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int rv = BCM_E_NONE;

    /* Make a Gport out of the in_port.*/
    BCM_GPORT_LOCAL_SET(gport_in, in_port);
    
    

    rv = bcm_field_entry_delete(unit, Port_general_data_fg_id, &entry_qual_info, Port_general_data_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, Port_general_data_fg_id, &entry_qual_info, Port_general_data_entry_handle_default);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, Port_general_data_fg_id, context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_group_delete(unit, Port_general_data_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    rv = bcm_port_class_set(unit, gport_in, bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData, Port_general_data_lsb_old_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_class_get bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData\n", rv);
        return rv;
    }
    rv = bcm_port_class_set(unit, gport_in, bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh, Port_general_data_msb_old_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_class_get bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh\n", rv);
        return rv;
    }

    return rv;
}

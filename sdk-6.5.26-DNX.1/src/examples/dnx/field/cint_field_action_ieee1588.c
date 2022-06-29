 /*
 * Example of bcmFieldActionIEEE1588.
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_ieee1588_action.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * int in_port = 200;
 * int packet_is_1588 = 0;
 * int compensate_time_stamp = 0;
 * int command = 0;
 * int encapsulation = 0;
 * int header_offset = 0;
 *
 * cint_field_in_port_qual_action_ieee1588_main(unit, context_id, in_port, packet_is_1588, compensate_time_stamp, command, encapsulation, offset);
 *
 * Configuration example end
 *
 * Injected traffic 1588oE
 * tx 1 psrc=200 data=0x0000000000020000000000038100000188F70002002c000200000000000000000000000000030000000000fffe0000660001fc00000000000000000000fa
 *
 * Please be aware that this case is only for Jericho2 generation.
 */

bcm_field_group_t ieee1588_fg_id = 0;
bcm_field_entry_t ieee1588_ent_id;


/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_in_port_qual_action_ieee1588_destory(int unit, bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, ieee1588_fg_id, NULL, ieee1588_ent_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, ieee1588_fg_id, context_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, ieee1588_fg_id));

    return BCM_E_NONE;
}

/**
* \brief
*  Using bcmFieldActionIEEE1588 to set various parameters for 1588 frames
*  bcmFieldActionIEEE1588
*  Bits(0:7): header_offset. Bit(8): encapsulation. Bits(9:10) command. Bit(11): compensate_time_stamp. Bit(12): packet_is_ieee1588.",
* \param [in] unit                      -  Device id
* \param [in] context_id                -  PMF context Id
* \param [in] in_port                   -  in pp_port id
* \param [in] packet_is_1588            -  indicating whether the packet is 1588.
* \param [in] compensate_time_stamp     -  If IngressP2pDelay will be used.
* \param [in] command                   -  Command used by egress pipeline, indicating if CF needs to be update.
* \param [in] encapsulation             -  0 - UDP, 1- ETH
* \param [in] header_offset             -  The beginning of the 1588 header from the beginning of the packet original header.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_in_port_qual_action_ieee1588_main(
    int unit,
    bcm_field_context_t context_id,
    int in_port,
    int packet_is_1588,
    int compensate_time_stamp,
    int command,
    int encapsulation,
    int header_offset
    )
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t trap_gport;
    void *dest_char;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionIEEE1588;

    printf("Creating first group\n");
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ieee1588_action", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &ieee1588_fg_id));


    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    printf("Attaching field group\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, ieee1588_fg_id, context_id, &attach_info));

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = in_port;
    ent_info.entry_qual[0].mask[0] = 0x1ff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    /*
     * Bits(0:7): header_offset. Bit(8): encapsulation. Bits(9:10) command. Bit(11): compensate_time_stamp. Bit(12): packet_is_ieee1588
     */
    ent_info.entry_action[0].value[0] = ((packet_is_1588 & 0x1) << 12)
                                        || ((compensate_time_stamp & 0x1 ) << 11)
                                        || ((command & 0x3 ) << 9)
                                        || ((encapsulation & 0x1) << 8)
                                        || (header_offset& 0xff);

    printf("Adding Entry\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, ieee1588_fg_id, &ent_info, &ieee1588_ent_id));

    return BCM_E_NONE;
}

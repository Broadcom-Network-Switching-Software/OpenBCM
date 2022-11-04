/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * This cint contains ePMF functions that are needed for OAM primary VLAN.
 * If a packet with primary VLAN is injected into UP MEP OAM-LIF which enabled OAM primary VLAN feature.
 This reconfiguration ensures that the packet will perform an additional packet classification
 on the OAM-LIF and the immediately followed primary VLAN.
 */


/*
 * Configuration example start:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_oam_primary_vlan.c
  cint;
  int unit = 0;
  int primary_vlan_port_id =  ;
  int vlan_port_id = 0x44801001;
  int primary_vlan = 10;
  int double_tags = 0; // FALSE
  cint_field_oam_primary_vlan_main(unit, primary_vlan_port_id, vlan_port_id, primary_vlan, double_tags);
 * Test Scenario - end
  Note:
  "primary_vlan_port_id" is new OAM-LIF created by API bcm_oam_primary_vlan_create()
  "vlan_port_id" is OAM-LIF created by API bcm_vlan_port_create()
  "double_tags" 1: double tags  0: single tag

  Details see oam_primary_vlan_example(unit)  adding acc MEP - upmep for primary VLAN.

 */

int Cint_field_oam_primary_vlan_context_presel_id = 124;


int cint_field_oam_primary_vlan_main(
    int unit,
    int primary_vlan_port_id,
    int vlan_port_id,
    int primary_vlan,
    int double_tags)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_format_t ace_format_id;
    int action_index;
    bcm_field_entry_info_t oam_entry_info;
    bcm_field_group_info_t fg_info;
    bcm_field_context_info_t context_info;
    bcm_field_context_t oam_primary_vlan_context_id;
    uint32 ace_entry_handle;
    void * dest_char;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_group_t fg_id;
    int gloal_out_lif;
    int oam_lif;

    gloal_out_lif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(vlan_port_id));
    oam_lif = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_OAM_PRIMARY_VLAN_GET(BCM_GPORT_VLAN_PORT_ID_GET(primary_vlan_port_id));

    /** Define ace action */
    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 2;
    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;
    ace_format_info.action_types[1] = bcmFieldActionStat0;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id), "");

    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextOamPrimaryVlan;
    ace_entry_info.entry_action[1].type = bcmFieldActionStat0;
    /** The value of the ACE entry is {b'20 ace_stat_meter_obj_id, b'5 ace_stat_meter_obj_cmd} */
    ace_entry_info.entry_action[1].value[0] = 0;
    ace_entry_info.entry_action[1].value[1] = oam_lif;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle), "");

    /** Create a new Context on ePMF */
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "oam_primary_vlan", sizeof(context_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &oam_primary_vlan_context_id), "");

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_id.presel_id = Cint_field_oam_primary_vlan_context_presel_id;
    presel_entry_id.stage = bcmFieldStageEgress;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = oam_primary_vlan_context_id;
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 1;
    presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeY1731;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data), "");

    /** Field Group: to set Context value */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeExactMatch;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyOutInterface;
    fg_info.qual_types[1] = bcmFieldQualifyVlanId;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionAceEntryId;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ACE ctxt value: oam primary vlan", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &fg_id), "oam primary vlan");

    /** attach program to given context */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0]; /* global outlif */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1]; /* inner vlan id */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = double_tags ? 128 : 96; /* primary VLAN */ 

    for (action_index = 0; action_index < fg_info.nof_actions; action_index++)
    {
        attach_info.payload_info.action_types[action_index] = fg_info.action_types[action_index];
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, fg_id, oam_primary_vlan_context_id, &attach_info), "");
    printf("Attached  FG (%d) to context (%d)\n", fg_id, oam_primary_vlan_context_id);

    bcm_field_entry_info_t_init(&oam_entry_info);
    oam_entry_info.nof_entry_quals = fg_info.nof_quals;
    oam_entry_info.entry_qual[0].type = fg_info.qual_types[0];
    oam_entry_info.entry_qual[0].value[0] = gloal_out_lif;
    oam_entry_info.entry_qual[0].mask[0] = 0xFFFFF;
    oam_entry_info.entry_qual[1].type = fg_info.qual_types[1];
    oam_entry_info.entry_qual[1].value[0] = primary_vlan;
    oam_entry_info.entry_qual[1].mask[0] = 0xFFF;
    oam_entry_info.nof_entry_actions = fg_info.nof_actions;
    oam_entry_info.entry_action[0].type = fg_info.action_types[0];
    oam_entry_info.entry_action[0].value[0] = ace_entry_handle;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, fg_id, &oam_entry_info, NULL), "");
    printf("Entry was added to field group %d. \n", fg_id);

    return BCM_E_NONE;
}


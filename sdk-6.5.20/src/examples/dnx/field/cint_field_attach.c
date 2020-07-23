/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 /*
 * Configuration example start:
 *
 * cint;                                                                  
  cint_reset();
  exit;
  cint ../../../src/examples/dnx/field/cint_field_attach.c
  cint ../../../../src/examples/dnx/field/cint_field_attach.c
  cint;
  int unit = 0;
  field_attach_main(unit);
 *
 * Configuration example end
 *
 *   This cint shows the unique ussage of bcm_field_group_context_attach() API
 *   Case: Received Packet with same packet structure ETH_IPv4_TCP, 
 *         We need to create rule to packets that have specific SRC_IPv4 addresses will set a certain pri (TC)
 *         however the forwarding layer in some cases can be ETH and in other IPv4
 *         To avoid TCAM entries duplication we would like that both will Access the same Database and have same set of Rules
 *
 *   Configuration:
 *    1st context selected based on Forwarding layer Ethernet
 *    2nd context selected based on Forwarding layer IP
 *    Create a Field group that Qualifier= SRC_IPv4 and Action=TC (priority) (TCAM database set pairs of SRC_IPv4 and the wanted priority)
 *    1st context will attach SRC_IPv4 to read from layer FWD+1
 *    2nd context will attach SRC_IPv4 to read from layer FWD
 *    Both attached to same Field group and thus access same entries in TCAM
 *    The configured stage will be iPMF1 (can be configured in same way in other stages)
 */


bcm_field_group_t cint_attach_fg_id=0;
bcm_field_entry_t cint_attach_entry_id[2] = {0};
bcm_field_context_t cint_attach_context_id_eth;
bcm_field_context_t cint_attach_context_id_ipv4;
bcm_field_presel_t cint_attach_presel_eth = 5;
bcm_field_presel_t cint_attach_presel_ipv4 = 6;




/**
* \brief
*  Configures the written above
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_attach_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void *dest_char;
    int rv = BCM_E_NONE;


    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SrcIPv4_Lookup", sizeof(fg_info.name));
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
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;

    
    rv = bcm_field_group_context_attach(unit, 0, cint_attach_fg_id, cint_attach_context_id_eth, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Attached  FG (%d) to context (%d)\n",cint_attach_fg_id,cint_attach_context_id_eth);

    /**Attach IPv4 context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IPv4", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_attach_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    
    rv = bcm_field_group_context_attach(unit, 0, cint_attach_fg_id, cint_attach_context_id_ipv4, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n",cint_attach_fg_id,cint_attach_context_id_ipv4);

    /**Its batter to add entries after attach, for batter bank allocation*/
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;


    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    /**IP Address 10.0.0.1*/
    ent_info.entry_qual[0].value[0] = 0x0A000001;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, cint_attach_fg_id, &ent_info, &(cint_attach_entry_id[0]));
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Src_ip:(0x%x) tc_val:(%d)\n",cint_attach_entry_id[0],ent_info.entry_qual[0].value[0],
                                                                            ent_info.entry_action[0].value[0]);
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_attach_presel_eth;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_attach_context_id_eth;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
    p_data.qual_data[0].qual_mask = 0x1F;

    /*Make sure that IPv4 is after Ethernet Forwarding*/
    p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[1].qual_arg = 1;
    p_data.qual_data[1].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[1].qual_mask = 0x1F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ethernet) fwd_layer+1(Ipv4) \n",
            cint_attach_presel_eth,cint_attach_context_id_eth);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_attach_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_attach_context_id_ipv4;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(IPv4) \n",
            cint_attach_presel_ipv4,cint_attach_context_id_ipv4);



    return 0;
}

int field_attach_destroy(int unit)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_attach_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = FALSE;
    p_data.context_id = 0;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    p_id.presel_id = cint_attach_presel_eth;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    rv = bcm_field_group_context_detach(unit, cint_attach_fg_id, cint_attach_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach ipv4\n", rv);
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

    rv = bcm_field_context_destroy(unit,bcmFieldStageIngressPMF1,cint_attach_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy ipv4\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit,cint_attach_fg_id,entry_qual_info,cint_attach_entry_id[0]);
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

    return 0;
}

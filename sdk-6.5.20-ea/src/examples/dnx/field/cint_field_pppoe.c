/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_pppoe.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * int uni_port = 200;
 * cint_field_pppoe_protocol_fg(unit, context_id, uni_port);
 * cint_field_pppoe_protocol_pppoe_anti_spoofing_fg(unit, context_id, uni_port);
 *
 * Configuration example end
 *
 * **************************************************************************************************************************************************
 * This CINT script is an example for followed usage
 *    Use PMF to redirect PPPoE protocol packets to CPU
 *        -- cint_field_pppoe_protocol_fg()
 *    Use PMF to drop packet fail to PPPoE anti-spoofing 
 *        -- cint_field_pppoe_protocol_pppoe_anti_spoofing_fg()
 */

bcm_field_group_t   cint_pppoe_fg_id =0;
bcm_gport_t in_port_gport;
bcm_field_entry_t entry[3];
bcm_field_entry_t eth_ent_id;
bcm_field_action_t action_drop_id= 0;
bcm_field_qualify_t etype_qual_id=0, pppoe_qual_id=0;
 
int cint_field_pppoe_protocol_fg(int unit, bcm_field_context_t context_id, int uni_port)
{
    int i = 0;
    uint32 data[3] = {0x8021,0x8057,0xC021}; /* PPPoE header protocols */
    uint32 mask = 0xffff;
    int sys_gport = 0;
    int dest_port  = 15;
    uint32 ethType_data = 0x8863;
    uint32 ethType_mask = 0xffff;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t etype_qual_info, pppoe_qual_info;
    bcm_field_context_t pppoe_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    /*
     * create 2 user defined qualifiers:
     * 1. L2 Etype Offset
     * 2. PPPoE Protocol
     */
    bcm_field_qualifier_info_create_t_init(&etype_qual_info);
    etype_qual_info.size = 16;
    dest_char = &(etype_qual_info.name[0]);
    sal_strncpy_s(dest_char, "Etype_qual", sizeof(etype_qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &etype_qual_info, &etype_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&pppoe_qual_info);
    pppoe_qual_info.size = 16;
    dest_char = &(pppoe_qual_info.name[0]);
    sal_strncpy_s(dest_char, "PPPoE_qual", sizeof(pppoe_qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &pppoe_qual_info, &pppoe_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }


    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.qual_types[1] = etype_qual_id;
    fg_info.qual_types[2] = pppoe_qual_id;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionRedirect;

    printf("Creating first group\n");
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "PPPoE protocol", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_pppoe_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add for PPPoE protocol ID\n", rv);
        return rv;
    }

    printf("Attaching PPPoe FG to the created context...\r\n");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 128; /* Offset to L2 Ethertype */

    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = 192;  /* Offset to PPPoE protocol */
    
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    rv = bcm_field_group_context_attach(unit, 0, cint_pppoe_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, dest_port);
    
    /* PPPoE control packet: ethernet type = 0x8863 */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    BCM_GPORT_LOCAL_SET(in_port_gport,uni_port);
    ent_info.nof_entry_quals = 2;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = in_port_gport;
    ent_info.entry_qual[0].mask[0] = 0x1ff;
    ent_info.entry_qual[1].type = etype_qual_id;
    ent_info.entry_qual[1].value[0] = ethType_data;
    ent_info.entry_qual[1].mask[0] = ethType_mask;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = sys_gport;

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, cint_pppoe_fg_id, &ent_info, &eth_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }


    ethType_data = 0x8864;

    /* PPPoE session stage protocols - 0x8864 + 0x8021/8057/C021  */
    for (i = 0; i < 3; i++)
    {
        bcm_field_entry_info_t_init(&ent_info);
        ent_info.priority = 1;
        BCM_GPORT_LOCAL_SET(in_port_gport,uni_port);
        ent_info.nof_entry_quals = 3;
        ent_info.entry_qual[0].type = fg_info.qual_types[0];
        ent_info.entry_qual[0].value[0] = in_port_gport;
        ent_info.entry_qual[0].mask[0] = 0x1ff;
        ent_info.entry_qual[1].type = etype_qual_id;
        ent_info.entry_qual[1].value[0] = ethType_data;
        ent_info.entry_qual[1].mask[0] = ethType_mask;
        ent_info.entry_qual[2].type = pppoe_qual_id;
        ent_info.entry_qual[2].value[0] = data[i];
        ent_info.entry_qual[2].mask[0] = mask;

        ent_info.nof_entry_actions = fg_info.nof_actions;
        ent_info.entry_action[0].type = fg_info.action_types[0];
        ent_info.entry_action[0].value[0] = sys_gport;

        printf("Adding Entry\n");
        rv = bcm_field_entry_add(unit, 0, cint_pppoe_fg_id, &ent_info, &entry[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_add\n", rv);
            return rv;
        }

    }

    return rv;
}

int cint_field_pppoe_protocol_fg_destroy(int unit, bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    int i=0;

    /* Delete entry from Field group */
    rv = bcm_field_entry_delete(unit, cint_pppoe_fg_id, NULL, eth_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, eth_ent_id, cint_pppoe_fg_id);
        return rv;
    }
    for (i = 0; i < 3; i++)
	{
    	/* Delete entry from Field group */
		rv = bcm_field_entry_delete(unit, cint_pppoe_fg_id, NULL, entry[i]);
		if (rv != BCM_E_NONE)
		{
			printf("Error (%d), in bcm_field_entry_delete entry %d from fg %d\n", rv, entry[i], cint_pppoe_fg_id);
			return rv;
		}
	}
    /* Detach the IPMF1 FG from its context */
    rv = bcm_field_group_context_detach(unit, cint_pppoe_fg_id, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d\n", rv, cint_pppoe_fg_id, context_id);
        return rv;
    }

    /* Delete  FG */
    rv = bcm_field_group_delete(unit, cint_pppoe_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, cint_pppoe_fg_id);
        return rv;
    }

    /*delete user-defined qualifiers*/
    rv = bcm_field_qualifier_destroy(unit,etype_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy  qualifier %d \n", rv, etype_qual_id);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit,pppoe_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy  qualifier %d \n", rv, pppoe_qual_id);
        return rv;
    }

    return rv;
}


int cint_field_pppoe_protocol_pppoe_anti_spoofing_fg(int unit, bcm_field_context_t context_id, int uni_port)
{
    int i = 0;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry[2];
    bcm_field_data_qualifier_t data_qual;
    uint32 data[2] = {0x0021, 0x0057};  /* allow only IPv4/IPv6 next protocol */
    uint32 mask = 0xffff;
    bcm_field_data_qualifier_t data_qual_ethType;
    uint32 ethType_data = 0x8864;             /* Allow only PPPoE EtherType */
    uint32 ethType_mask = 0xffff;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t etype_qual_info, pppoe_qual_info;
    bcm_field_context_t pppoe_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    /*Create Drop action */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionForward;
    action_info.prefix_size = 31;
    /**11 bit of qualifier are 0 and to drop all dest bit are 1's. With lsb 1 from entry payload we get 0x001FFFFF*/
    action_info.prefix_value = 0x000FFFFF;
    action_info.size = 1;
    action_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "action_drop_1b", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &action_drop_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_create action_prefix_stat_action\n");
        return rv;
    }


    /*
     * create 2 user defined qualifiers:
     * 1. L2 Etype Offset
     * 2. PPPoE Protocol
     */
    bcm_field_qualifier_info_create_t_init(&etype_qual_info);
    etype_qual_info.size = 16;

    rv = bcm_field_qualifier_create(unit, 0, &etype_qual_info, &etype_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&pppoe_qual_info);
    pppoe_qual_info.size = 16;

    rv = bcm_field_qualifier_create(unit, 0, &pppoe_qual_info, &pppoe_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }


    /*
      * Create and attach TCAM group in iPMF1
      */
     bcm_field_group_info_t_init(&fg_info);
     fg_info.fg_type = bcmFieldGroupTypeTcam;
     fg_info.stage = bcmFieldStageIngressPMF1;
     fg_info.nof_quals = 4;
     fg_info.qual_types[0] = bcmFieldQualifyInPort;
     fg_info.qual_types[1] = bcmFieldQualifyL2SrcHit;
     fg_info.qual_types[2] = etype_qual_id;
     fg_info.qual_types[3] = pppoe_qual_id;
     fg_info.nof_actions = 1;
     fg_info.action_types[0] = action_drop_id;

     printf("Creating first group\n");
     dest_char = &(fg_info.name[0]);
     sal_strncpy_s(dest_char, "PPPoE protocol", sizeof(fg_info.name));
     rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_pppoe_fg_id);
     if (rv != BCM_E_NONE)
     {
         printf("Error (%d), in bcm_field_group_add for PPPoE protocol ID\n", rv);
         return rv;
     }

     printf("Attaching PPPoe FG to the created context...\r\n");

     bcm_field_group_attach_info_t_init(&attach_info);

     attach_info.key_info.nof_quals = fg_info.nof_quals;
     attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
     attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
     attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
     attach_info.key_info.qual_types[3] = fg_info.qual_types[2];

     attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
     attach_info.key_info.qual_info[0].input_arg = 0;
     attach_info.key_info.qual_info[0].offset = 0;

     attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
     attach_info.key_info.qual_info[1].input_arg = 0;
     attach_info.key_info.qual_info[1].offset = 0;

     attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
     attach_info.key_info.qual_info[2].input_arg = 0;
     attach_info.key_info.qual_info[2].offset = -80; /* Offset to L2 Ethertype */

     attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerFwd;
     attach_info.key_info.qual_info[3].input_arg = 0;
     attach_info.key_info.qual_info[3].offset = -16;  /* Offset to PPPoE protocol */

     attach_info.payload_info.nof_actions = fg_info.nof_actions;
     attach_info.payload_info.action_types[0] = fg_info.action_types[0];

     rv = bcm_field_group_context_attach(unit, 0, cint_pppoe_fg_id, context_id, &attach_info);
     if (rv != BCM_E_NONE)
     {
         printf("Error (%d), in bcm_field_group_context_attach\n", rv);
         return rv;
     }

    BCM_GPORT_LOCAL_SET(in_port_gport,uni_port);

    /* PPPoE data packet  */
    for (i = 0; i < 2; i++)
    {
        bcm_field_entry_info_t_init(&ent_info);
        ent_info.priority = 1;

        ent_info.nof_entry_quals = 4;
        ent_info.entry_qual[0].type = fg_info.qual_types[0];
        ent_info.entry_qual[0].value[0] = in_port_gport;
        ent_info.entry_qual[0].mask[0] = 0x1ff;
        ent_info.entry_qual[1].type = fg_info.qual_types[1];
        ent_info.entry_qual[1].value[0] = 1;
        ent_info.entry_qual[1].mask[0] = 1;
        ent_info.entry_qual[2].type = etype_qual_id;
        ent_info.entry_qual[2].value[0] = ethType_data;
        ent_info.entry_qual[2].mask[0] = ethType_mask;
        ent_info.entry_qual[3].type = pppoe_qual_id;
        ent_info.entry_qual[3].value[0] = data[i];
        ent_info.entry_qual[3].mask[0] = mask;

        ent_info.nof_entry_actions = fg_info.nof_actions;
        ent_info.entry_action[0].type = action_drop_id;
        ent_info.entry_action[0].value[0] = 1;

        printf("Adding Entry\n");
        rv = bcm_field_entry_add(unit, 0, cint_pppoe_fg_id, &ent_info, &entry[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_add\n", rv);
            return rv;
        }
    }


    return rv;

}

/* Create a pselect for IP anti-spoofing static mode in upstream.
 * params:
 *    unit: device number
 *    in_port:  PON port want to enable IP spoofing static mode.
 */
int cint_field_pppoe_sipv4_anti_spoofing_presel_set(/* in */int unit,
		                                 /* in */bcm_field_presel_t presel_id,
		                                 /* in */bcm_field_context_t context_id,
                                          /* in */int in_port)
{
    void *dest_char;
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_gport_t in_port_gport;

    BCM_GPORT_LOCAL_SET(in_port_gport,in_port);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;
    
    p_data.qual_data[1].qual_type = bcmFieldQualifyInPort;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = in_port_gport;
    p_data.qual_data[1].qual_mask = 0x1FF;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured context(%d)) \n",
        presel_id, context_id);

    return result;
}






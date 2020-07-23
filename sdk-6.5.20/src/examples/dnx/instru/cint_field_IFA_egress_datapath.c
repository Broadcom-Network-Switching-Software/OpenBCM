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
 * cint ../../../src/examples/dnx/field/cint_field_IFA_egress_datapath.c
 * cint;
 * int unit = 0;
 * cint_field_IFA_datapath_egress_main(unit);
 *
 * Configuration example end
 *
 *  IFA 1.0 using datapath only - Egress node
 */


bcm_field_group_t cint_field_IFA_datapath_egress_fg_id=0;
bcm_field_group_t cint_field_IFA_datapath_egress_fg_id_2nd_pass=0;
bcm_field_context_t cint_field_IFA_datapath_egress_context_id_ipv4;
bcm_field_context_t cint_field_IFA_datapath_egress_context_id_2nd_pass;
bcm_field_presel_t cint_field_IFA_datapath_egress_presel_ipv4 = 6;
bcm_field_qualify_t cint_field_IFA_datapath_egress_probe_qual;
uint32 cint_field_IFA_datapath_egress_ace_entry_handle;
int cint_field_IFA_2nd_p_fhei_trap_id;


/* Example of PMF Egress Entry creation for IFA 1.0, to be used for for Egress nodes.
 * IFA 1.0 packets need to be trapped for egress node proccess.
 * Qualifier is based on probe_header_1 and probe_header_2 match.
 * The Actions set ePMF context bits to "IFA-Trap-Context" for trap context*/
int cint_field_IFA_datapath_egress_entry_add(int unit, uint32 probe_1, uint32 probe_2, bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    int rv=BCM_E_NONE;

    /**Its batter to add entries after attach, for batter bank allocation*/
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;


    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = cint_field_IFA_datapath_egress_probe_qual;
    ent_info.entry_qual[0].value[0] = probe_2;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;
    ent_info.entry_qual[0].value[1] = probe_1;
    ent_info.entry_qual[0].mask[1] = 0xffffffff;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionAceEntryId;
    ent_info.entry_action[0].value[0] = cint_field_IFA_datapath_egress_ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_egress_fg_id, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x) FG_ID (%d) \n Probe_1:(0x%x) Probe_2:(0x%x) Ace_Entry(0x%x)\n",
            *entry_handle,
            cint_field_IFA_datapath_egress_fg_id,
            ent_info.entry_qual[0].value[0],
            ent_info.entry_qual[0].value[1],cint_field_IFA_datapath_egress_ace_entry_handle);

    return rv;
}
/** Entry add for 2nd path. Trap is done according recycle port and packet trapped to cpu port .*/
int cint_field_IFA_datapath_ingress_2nd_pass_entry_add(int unit, bcm_field_entry_t *entry_handle)
{
    bcm_field_entry_info_t ent_info;
    bcm_gport_t trap_gport;
    int rv=BCM_E_NONE;

    /**Its batter to add entries after attach, for batter bank allocation*/
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    /*Set the  the recycled  port is that one that was configured
     * as well set the trap to port CPU*/
    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = bcmFieldQualifyInPortRaw;
    ent_info.entry_qual[0].value[0] = 0x10;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionTrap;
    BCM_GPORT_TRAP_SET(trap_gport, cint_field_IFA_2nd_p_fhei_trap_id, 15, 0);
    ent_info.entry_action[0].value[0] = trap_gport;
    ent_info.entry_action[0].value[1] = 0xDEAD;

    rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_egress_fg_id_2nd_pass, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }

    printf("Entry add: id:(0x%x) FG_ID (%d) \n RecyclePort (%d) Trap_Code (0x%x)\n",
            *entry_handle,
            cint_field_IFA_datapath_egress_fg_id_2nd_pass,
            ent_info.entry_qual[0].value[1],cint_field_IFA_2nd_p_fhei_trap_id);


    return rv;
}

/** Create group for 2nd path, according recycle port */
int cint_field_IFA_egress_datapath_2nd_pass_group(int unit)
{

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    bcm_rx_trap_config_t trap_config;
    bcm_port_config_t port_config;
    bcm_gport_t cpu_port;
    void *dest_char;
    int rv = BCM_E_NONE;


    /**Currently use Default context i.e. not hit
     * later the context can be chosen based on in_port profile and set the recycled port profile to same values as for PMF CS (using bcm_port_class_set())*/
    cint_field_IFA_datapath_egress_context_id_2nd_pass = BCM_FIELD_CONTEXT_ID_DEFAULT;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = cint_field_IFA_datapath_egress_probe_qual;
    fg_info.qual_types[1] = bcmFieldQualifyInPortRaw;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionTrap;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_dp_eg_2nd_p", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_egress_fg_id_2nd_pass);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_IFA_datapath_egress_fg_id_2nd_pass);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    /* Look for the probe header at Fwd+2 layer with no offset */
    attach_info.key_info.qual_info[0].input_arg = 2;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;



    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_egress_fg_id_2nd_pass, cint_field_IFA_datapath_egress_context_id_2nd_pass, &attach_info);

    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n",cint_field_IFA_datapath_egress_fg_id_2nd_pass,cint_field_IFA_datapath_egress_context_id_2nd_pass);


    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_egress_presel_ipv4;
    p_id.stage = fg_info.stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_egress_context_id_2nd_pass;

    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    /**Currently the presel selection is commented out since default context is used
     * read the comment about in the begging of the function*/
   /* rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);*/
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

  /*  printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_egress_presel_ipv4,cint_field_IFA_datapath_egress_context_id_2nd_pass);*/


    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_port_config_get failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &cint_field_IFA_2nd_p_fhei_trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_create \n");
        return rv;
    }

    trap_config.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST;
    BCM_PBMP_ITER(port_config.cpu, cpu_port)
    {
        trap_config.dest_port = cpu_port;
        break;
    }

    rv = bcm_rx_trap_set(unit, cint_field_IFA_2nd_p_fhei_trap_id, &trap_config);
    if(rv != BCM_E_NONE)
    {
       printf("Error in bcm_rx_trap_set \n");
       return rv;
    }

    return rv;

}

int cint_field_IFA_egress_datapath_group(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    void *dest_char;
    int rv = BCM_E_NONE;

    /*
    * Configure the ACE format.
    */
    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;

    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;

    dest_char = &(ace_format_info.name[0]);
    sal_strncpy_s(dest_char, "AM_ACE", sizeof(ace_format_info.name));
    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_format_add \n", rv);
       return rv;
    }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = 1;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextIFATrap;

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &cint_field_IFA_datapath_egress_ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_entry_add \n", rv);
       return rv;
    }
    printf("ACE Format (%d) and ACE entry 0x(%x) created \n", ace_format_id,cint_field_IFA_datapath_egress_ace_entry_handle);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=64;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_eg_probe_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_egress_probe_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_IFA_datapath_egress_probe_qual;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionAceEntryId;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_datapath_egress", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_egress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in ePMF \n", cint_field_IFA_datapath_egress_fg_id);


    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPv4", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, fg_info.stage, &context_info, &cint_field_IFA_datapath_egress_context_id_ipv4);
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
    /* Look for the probe header at Fwd+2 layer with no offset */
    attach_info.key_info.qual_info[0].input_arg = 2;
    attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_egress_fg_id, cint_field_IFA_datapath_egress_context_id_ipv4, &attach_info);

    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n",cint_field_IFA_datapath_egress_fg_id,cint_field_IFA_datapath_egress_context_id_ipv4);


    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_egress_presel_ipv4;
    p_id.stage = fg_info.stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_egress_context_id_ipv4;

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

    printf("Presel (%d) was configured for stage(ePMF) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_egress_presel_ipv4,cint_field_IFA_datapath_egress_context_id_ipv4);

    return rv;
}

/* Example of egress field configuration for IFA 1.0, to be used for for egress node:
 *  1. ePMF traps packet by probe_headers and sets ePMF context bits to "IFA-Trap-Context"
 *  2. Configure trap on ingress side for 2nd path
 */
int cint_field_IFA_datapath_egress_main(int unit)
{
    int rv = BCM_E_NONE;

    rv = cint_field_IFA_egress_datapath_group(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_IFA_egress_datapath_group \n", rv);
       return rv;
    }

    rv = cint_field_IFA_egress_datapath_2nd_pass_group(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_field_IFA_egress_datapath_2nd_pass_group \n", rv);
       return rv;
    }

    return rv;
}


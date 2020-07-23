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
 * cint ../../../src/examples/dnx/field/cint_field_external_with_pmf.c
 * cint;
 * int unit = 0;
 * cint_field_external_pmf_main(unit);
 *
 * Configuration example end
 */


/*
 * Global variables.
 * Declared global to make easy traffic and destroy functions.
 */

bcm_field_group_t cint_field_external_pmf_fg_id = BCM_FIELD_INVALID;
uint32 cint_field_external_pmf_payload_id = 2;
bcm_field_AppType_t cint_field_external_pmf_external_app_type = bcmFieldAppTypeIp4UcastRpf;
uint32 cint_field_external_pmf_payload_offset = 120;
uint32 cint_field_external_pmf_payload_value = 0xabc;
uint32 cint_field_external_pmf_payload_value_default = 0xdef;
uint32 cint_field_external_pmf_payload_size = 32;
uint32 cint_field_external_pmf_priority = 10;
bcm_field_action_t cint_field_external_pmf_external_uda = bcmFieldActionCount;
bcm_field_presel_entry_id_t cint_field_external_pmf_presel_entry_id;
bcm_field_context_t cint_field_external_pmf_ipmf1_context = BCM_FIELD_INVALID;
bcm_field_entry_t cint_field_external_pmf_entry_id[5] = {0,0,0,0,0};
bcm_field_entry_t cint_field_external_pmf_entry_id_default = 0;

bcm_field_group_t cint_field_external_pmf_fg_id_ipmf2 = BCM_FIELD_INVALID;
bcm_field_qualify_t cint_field_external_pmf_ipmf2_payload_qual = bcmFieldQualifyCount;
bcm_field_action_t cint_field_external_pmf_ipmf2_action_type = bcmFieldActionForward;
bcm_field_action_t cint_field_external_pmf_ipmf2_uda = bcmFieldActionCount;



int
cint_field_external_pmf_fg_add_flags(
    int unit,
    unsigned int flags,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int qual_idx = 0;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *tmp_ptr;
    proc_name = "cint_field_external_pmf_fg_add";

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    dest_char = &(fg_info_p->name[0]);
    tmp_ptr = dest_char;

    snprintf(tmp_ptr, sizeof(fg_info_p->name), "ext_FG");
    /*
     *  Add qualifiers to the Field Group
     */
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifySrcMac;
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifyDstIp;
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifyIp4Ttl;
    fg_info_p->qual_types[qual_idx++] = bcmFieldQualifyL4SrcPort;
    fg_info_p->nof_quals = qual_idx;
    /*
     * Mark bcmFieldQualifyL4SrcPort as a ranged qualifier.
     */
    fg_info_p->qual_is_ranged[3] = TRUE;
    
    /*
     *  Create a void user defined action
     */
    bcm_field_action_info_t_init(&action_info);

    action_info.stage = bcmFieldStageExternal;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = cint_field_external_pmf_payload_size;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_uda", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_pmf_external_uda);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[0] = cint_field_external_pmf_external_uda;
    fg_info_p->action_with_valid_bit[0] = FALSE;

    fg_info_p->nof_actions = 1;

    rv = bcm_field_group_add(unit, flags, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_pmf_fg_add(
    int unit,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p)
{
    int rv = BCM_E_NONE;

    rv = cint_field_external_pmf_fg_add_flags(unit, 0, fg_info_p, fg_id_p);

    return rv;
}
int
cint_field_external_pmf_fg_attach(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_AppType_t external_app_type /*opcode*/,
    uint32 payload_id,
    uint32 payload_offset)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    uint32 qual_ndx, action_ndx;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_context_t context_id;
    proc_name = "cint_field_external_pmf_fg_attach";

    bcm_field_group_attach_info_t_init(&group_attach_info);

    /*
     * Build the attach information
     */
    group_attach_info.key_info.nof_quals = fg_info_p->nof_quals;
    group_attach_info.payload_info.nof_actions = fg_info_p->nof_actions;
    /* Payload id is the lookup id in the external */
    group_attach_info.payload_info.payload_id = payload_id;
    /*
     * Payload offset is the location of the result inside the external payload buffer.
     * In order to extract the result this value should be used
     * as the qualifier offset in the PMF12.
 */
    group_attach_info.payload_info.payload_offset = payload_offset;

    for (qual_ndx = 0; qual_ndx < fg_info_p->nof_quals; qual_ndx++)
    {
        group_attach_info.key_info.qual_types[qual_ndx] = fg_info_p->qual_types[qual_ndx];
    }

    for (action_ndx = 0; action_ndx < fg_info_p->nof_actions; action_ndx++)
    {
        group_attach_info.payload_info.action_types[action_ndx] = fg_info_p->action_types[action_ndx];
    }
    /*
     * Build the qualifier attach information
     */
    qual_ndx = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_arg = -1;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;

    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;

    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 0;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;

    group_attach_info.key_info.qual_info[qual_ndx].input_arg = 1;
    group_attach_info.key_info.qual_info[qual_ndx].input_type = bcmFieldInputTypeLayerFwd;
    group_attach_info.key_info.qual_info[qual_ndx++].offset = 0;
    
    /* Cast */
    context_id  = (external_app_type & 0x00ff) ;

    /*
     * Call the attach API
     */
    rv = bcm_field_group_context_attach(unit, 0, external_fg_id, context_id, &group_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach fg_id %d external_app_type %d context %d\n", proc_name, rv, external_fg_id,   (external_app_type & 0x00ff), context_id);
        return rv;
    }

    return rv;
}


int
cint_field_external_pmf_entry_add(
    int unit,
    int index,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;

    proc_name = "cint_field_external_pmf_entry_add";

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.entry_qual[0].type = bcmFieldQualifySrcMac;
    entry_info.entry_qual[0].value[0] = 0x00000004;
    entry_info.entry_qual[0].mask[0] = 0xffffffff;
    entry_info.entry_qual[0].value[1] = 0x0002;
    entry_info.entry_qual[0].mask[1] = 0xffff;
    entry_info.entry_qual[1].type = bcmFieldQualifyDstIp;
    entry_info.entry_qual[1].value[0] = 0x7fffff02+index;
    entry_info.entry_qual[1].mask[0] = 0xffffffff;
    entry_info.entry_qual[2].type = bcmFieldQualifyIp4Ttl;
    entry_info.entry_qual[2].value[0] = 0x20;
    entry_info.entry_qual[2].mask[0] = 0xfE;
    /* 
     * For a ranged qualifier, value[0] is the minimum of the range and max_value[0] is the maximum.
     * Mask is not used for a ranged qualifier.
     */
    entry_info.entry_qual[3].type = bcmFieldQualifyL4SrcPort;
    entry_info.entry_qual[3].value[0] = 1000;
    entry_info.entry_qual[3].max_value[0] = 2000;

    entry_info.nof_entry_quals = fg_info_p->nof_quals;
    
    entry_info.entry_action[0].type = fg_info_p->action_types[0];
    entry_info.entry_action[0].value[0] = cint_field_external_pmf_payload_value;

    entry_info.nof_entry_actions = fg_info_p->nof_actions;
    
    entry_info.priority = cint_field_external_pmf_priority;

    rv = bcm_field_entry_add(unit, 0, external_fg_id, &entry_info, &cint_field_external_pmf_entry_id[index]);
    if (rv != BCM_E_NONE)
    {
        void *dest_char;
        char *fg_name;
        dest_char = &(fg_info_p->name[0]);
        fg_name = dest_char;

        printf("%s Error (%d), in bcm_field_entry_add fg_id %d fg_name %s \n", proc_name, rv, external_fg_id, fg_name);
        return rv;
    }    

    return rv;
}

int
cint_field_external_pmf_context_ipmf1_create(
    int unit)
{
    char *proc_name;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t ipmf1_p_data; 
    void *dest_char;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_pmf_context_ipmf1_create";
    
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "ext_PMF_test", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_external_pmf_ipmf1_context);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&cint_field_external_pmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf1_p_data);
    
    cint_field_external_pmf_presel_entry_id.presel_id = 15;
    cint_field_external_pmf_presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf1_p_data.entry_valid = TRUE;
    ipmf1_p_data.context_id = cint_field_external_pmf_ipmf1_context;
    ipmf1_p_data.nof_qualifiers = 1;
    ipmf1_p_data.qual_data[0].qual_type = bcmFieldQualifyAppType;
    ipmf1_p_data.qual_data[0].qual_value = cint_field_external_pmf_external_app_type;
    ipmf1_p_data.qual_data[0].qual_mask = 0x3F;

    rv = bcm_field_presel_set(unit, 0, &cint_field_external_pmf_presel_entry_id, &ipmf1_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }

    printf("Configured context id %d on iPMF1\n", cint_field_external_pmf_ipmf1_context);

    return rv;
}


int
cint_field_external_pmf_fg_ipmf2_add(
    int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    void *dest_char;
    int ii;
    bcm_field_group_info_t fg_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_group_attach_info_t attach_info;
    int action_type_size;
    
    proc_name = "cint_field_external_pmf_fg_ipmf2_add";

    bcm_field_group_info_t_init( &fg_info);
    

    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ext_PMF_FG", sizeof(fg_info.name));

    /*
     *  Create a user defined qualifier
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = cint_field_external_pmf_payload_size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "ext_iPMF2", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_external_pmf_ipmf2_payload_qual);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }

    rv = bcm_field_action_info_get(unit, cint_field_external_pmf_ipmf2_action_type, bcmFieldStageIngressPMF2, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_dnx_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    action_type_size = action_info.size;
    
    /*
     *  Create a void user defined action
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = cint_field_external_pmf_ipmf2_action_type;
    action_info.size = cint_field_external_pmf_payload_size;
    action_info.prefix_size = action_type_size - cint_field_external_pmf_payload_size;
    action_info.prefix_value = 0;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_uda_PMF", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_pmf_ipmf2_uda);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyExternalHit2;
    fg_info.qual_types[1] = cint_field_external_pmf_ipmf2_payload_qual;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_field_external_pmf_ipmf2_uda;
    fg_info.action_with_valid_bit[0] = TRUE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_external_pmf_fg_id_ipmf2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    for(ii=0; ii< fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii<fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeExternal;

    attach_info.key_info.qual_info[1].offset = cint_field_external_pmf_payload_offset;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_external_pmf_fg_id_ipmf2, cint_field_external_pmf_ipmf1_context, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    printf("Config DE FG on iPMF2, fg_id=%d\n", cint_field_external_pmf_fg_id_ipmf2);

    return rv;
}


/**
 * \brief - run external main function
 *
 * \param [in] unit - the unit number in system
 *
 *  Usage: run in cint shell "cint_field_external_pmf_main(0)"
 */
int
cint_field_external_pmf_main(int unit)
{
    bcm_field_group_info_t fg_info;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_pmf_main";

    bcm_field_group_info_t_init( &fg_info);

    rv = cint_field_external_pmf_fg_add(unit, &fg_info, &cint_field_external_pmf_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_add fg_id %d\n", proc_name, rv, cint_field_external_pmf_fg_id);
        return rv;
    }

    rv = cint_field_external_pmf_fg_attach(unit, cint_field_external_pmf_fg_id, &fg_info, cint_field_external_pmf_external_app_type, cint_field_external_pmf_payload_id, cint_field_external_pmf_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_attach fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), bcm_switch_control_set - ExternalTcamSync failed fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));
        return rv;
    }

    rv = cint_field_external_pmf_entry_add(unit, 0, cint_field_external_pmf_fg_id, &fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_entry_add fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));
        return rv;
    }

    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));

    rv = cint_field_external_pmf_context_ipmf1_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_context_ipmf1_create\n", proc_name, rv);
        return rv;
    }
    
    rv = cint_field_external_pmf_fg_ipmf2_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_ipmf2_add\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_pmf_destroy(int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_pmf_destroy";

    /*
     * At this point external field group cannot be deleted, so we only delete the entries.
     */
    rv = bcm_field_entry_delete(unit, cint_field_external_pmf_fg_id, NULL, cint_field_external_pmf_entry_id[0]);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete fg_id %d \n", proc_name, rv, cint_field_external_pmf_fg_id);
        return rv;
    }

    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_pmf_presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_pmf_destroy_all(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int index;

    proc_name = "cint_field_external_pmf_destroy";

    for (index=0; index<5; index++)
    {
        rv = bcm_field_entry_delete(unit, cint_field_external_pmf_fg_id, NULL, cint_field_external_pmf_entry_id[index]);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_entry_delete fg_id %d \n", proc_name, rv, cint_field_external_pmf_fg_id);
            return rv;
        }
    }

    return rv;
}

/***************************************************************************************************************************/
/************************************** ACL Counters test ******************************************************************/
/***************************************************************************************************************************/
bcm_field_group_info_t acl_counters_fg_info;
bcm_field_group_info_t get_acl_counters_fg_info;
/**
 * \brief - run external counters main function
 * cint procedure
 *  - create field group with acl counters enabled
 *  - run sync
 *  - add 3 entries to table.
 *
 * \param [in] unit - the unit number in system
 *
 *  Usage: run in cint shell "cint_field_external_counters_pmf_main(0)"
 */
int
cint_field_external_counters_pmf_main(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int index;

    proc_name = "cint_field_external_counters_pmf_main";

    bcm_field_group_info_t_init( &acl_counters_fg_info);

    rv = cint_field_external_pmf_fg_add_flags(unit, (BCM_FIELD_FLAG_EXTERNAL_COUNTERS),
                                              &acl_counters_fg_info, &cint_field_external_pmf_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_add fg_id %d\n", proc_name, rv, cint_field_external_pmf_fg_id);
        return rv;
    }

    rv = cint_field_external_pmf_fg_attach(unit, cint_field_external_pmf_fg_id, &acl_counters_fg_info, cint_field_external_pmf_external_app_type,
            cint_field_external_pmf_payload_id, cint_field_external_pmf_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_pmf_fg_attach fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_pmf_fg_id,
                (cint_field_external_pmf_external_app_type & 0x00ff));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), bcm_switch_control_set - ExternalTcamSync failed fg_id %d external_app_type %d\n",
                proc_name, rv, cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));
        return rv;
    }

    /** add 5 entries to the first 3 we will send packets, the other 2 are for validation of counter=0 */
    for (index=0; index<5; index++)
    {
        rv = cint_field_external_pmf_entry_add(unit, index, cint_field_external_pmf_fg_id, &acl_counters_fg_info);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in cint_field_external_pmf_entry_add fg_id %d external_app_type %d\n", proc_name, rv,
                    cint_field_external_pmf_fg_id,  (cint_field_external_pmf_external_app_type & 0x00ff));
            return rv;
        }
    }

    return rv;
}

/**
 * \brief - read external counters function
 *
 * - call bcm_field_group_info_get to get the acl_db pointer
 * - use KBP SDK in order to read the external counters
 * call dnx_kbp_db_counter_read_initiate to initiate the counter read process
 * loop on call to dnx_kbp_db_is_counter_read_complete until is_complete is true.
 * extract the specific entry counters.
 * it is expected the countes to have the following result:
 * 1 hit for 1st entry, 2 hits for 2nd entry and 3 hits for 3rd entry.
 *
 * \param [in] unit - the unit number in system
 *
 *  Usage: run in cint shell "cint_field_external_get_main(0)"
 */
int
cint_field_external_counters_get_main(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    int is_complete;
    uint64 value;
    int entry_handle;
    int index;

    COMPILER_64_ZERO(value);

    proc_name = "cint_field_external_counters_get_main";

    rv = bcm_field_group_info_get(unit, cint_field_external_pmf_fg_id, &acl_counters_fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_info_get\n", proc_name, rv);
        return rv;
    }
    printf("before kbp_db_counter_read_initiate\n\n");
    rv = dnx_kbp_db_counter_read_initiate(unit, acl_counters_fg_info.ext_acl_db);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_kbp_db_counter_read_initiate\n", proc_name, rv);
        return rv;
    }
    do {
        rv = dnx_kbp_db_is_counter_read_complete(unit, acl_counters_fg_info.ext_acl_db, &is_complete);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in dnx_kbp_db_is_counter_read_complete\n", proc_name, rv);
            return rv;
        }
    } while (is_complete == 0);
    printf("after kbp_db_is_counter_read_complete\n\n");

    /* read first 3 entries we expect to get 1,2,3 packet read*/
    for (index=0; index<3; index++)
    {
        entry_handle = (cint_field_external_pmf_entry_id[index] & 0x3FFFFFF);
        rv = dnx_kbp_entry_get_counter_value(unit, acl_counters_fg_info.ext_acl_db, &entry_handle, &value);
        if (rv != BCM_E_NONE)
        {
           printf("%s Error (%d), in dnx_kbp_entry_get_counter_value\n", proc_name, rv);
           return rv;
        }

        printf("entry %d value is: %d,%d\n", index, COMPILER_64_HI(value), COMPILER_64_LO(value));
        if (COMPILER_64_LO(value) != (index+1))
        {
            printf("entry %d value is: %d,%d expected: %d\n", index, COMPILER_64_HI(value), COMPILER_64_LO(value), (index+1));
            rv = BCM_E_EXISTS;
            return rv;
        }
    }

    /* read last 2 entries we expect them to have counter = 0*/
    for (index=3; index<5; index++)
    {
        entry_handle = (cint_field_external_pmf_entry_id[index] & 0x3FFFFFF);
        rv = dnx_kbp_entry_get_counter_value(unit, acl_counters_fg_info.ext_acl_db, &entry_handle, &value);
        if (rv != BCM_E_NONE)
        {
           printf("%s Error (%d), in dnx_kbp_entry_get_counter_value\n", proc_name, rv);
           return rv;
        }

        printf("entry %d value is: %d,%d\n", index, COMPILER_64_HI(value), COMPILER_64_LO(value));
        if (COMPILER_64_LO(value) != 0)
        {
            printf("entry %d value is: %d,%d expected: 0\n", COMPILER_64_HI(value), COMPILER_64_LO(value));
            rv = BCM_E_EXISTS;
            return rv;
        }
    }

    return rv;
}

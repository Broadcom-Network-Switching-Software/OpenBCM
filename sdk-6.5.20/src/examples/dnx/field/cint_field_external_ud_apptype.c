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
 * cint ../../src/examples/dnx/field/cint_field_external_ud_apptype.c
 * cint;
 * int unit = 0;
 * cint_field_external_ud_apptype_main(unit);
 *
 * Configuration example end
 *
 *  Configures user defined AppTypes and uses them, including using preselectors.
 */


/*
 * Global variables.
 * Declared global to make easy traffic and destroy functions.
 */

bcm_field_group_t cint_field_external_ud_apptype_fg_id_predef = BCM_FIELD_INVALID;
bcm_field_group_t cint_field_external_ud_apptype_fg_id_user_1 = BCM_FIELD_INVALID;
bcm_field_group_t cint_field_external_ud_apptype_fg_id_user_2 = BCM_FIELD_INVALID;
uint32 cint_field_external_ud_apptype_payload_id = 5;
bcm_field_AppType_t cint_field_external_ud_apptype_external_app_type_predef = bcmFieldAppTypeIp6McastRpf;
bcm_field_AppType_t cint_field_external_ud_apptype_external_app_type_user_1 = bcmFieldAppTypeCount;
bcm_field_AppType_t cint_field_external_ud_apptype_external_app_type_user_2 = bcmFieldAppTypeCount;
uint32 cint_field_external_ud_apptype_payload_offset = 40;
uint32 cint_field_external_ud_apptype_payload_value_predef = 0x10;
uint32 cint_field_external_ud_apptype_payload_value_user_1 = 0x11;
uint32 cint_field_external_ud_apptype_payload_value_user_2 = 0x12;
uint32 cint_field_external_ud_apptype_payload_size_predef = 16;
uint32 cint_field_external_ud_apptype_payload_size_user_1 = 24;
uint32 cint_field_external_ud_apptype_payload_size_user_2 = 32;
bcm_field_action_t cint_field_external_ud_apptype_external_uda_predef = bcmFieldActionCount;
bcm_field_action_t cint_field_external_ud_apptype_external_uda_user_1 = bcmFieldActionCount;
bcm_field_action_t cint_field_external_ud_apptype_external_uda_user_2 = bcmFieldActionCount;
bcm_field_presel_entry_id_t cint_field_external_ud_apptype_presel_entry_id_1;
bcm_field_presel_entry_id_t cint_field_external_ud_apptype_presel_entry_id_2;
bcm_field_entry_t cint_field_external_ud_apptype_entry_id_predef = 0;
bcm_field_entry_t cint_field_external_ud_apptype_entry_id_user_1 = 0;
bcm_field_entry_t cint_field_external_ud_apptype_entry_id_user_2 = 0;
bcm_field_presel_entry_id_t cint_field_external_ud_apptype_presel_entry_id_info_1_predef;
bcm_field_presel_entry_id_t cint_field_external_ud_apptype_presel_entry_id_info_2_user_1;
bcm_field_presel_entry_id_t cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_1;
bcm_field_presel_entry_id_t cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_2;


bcm_field_context_t cint_field_external_ud_apptype_ipmf1_context_1 = BCM_FIELD_INVALID;
bcm_field_context_t cint_field_external_ud_apptype_ipmf1_context_2 = BCM_FIELD_INVALID;
bcm_field_presel_t cint_field_external_ud_apptype_ipmf1_presel_id_1 = 85;
bcm_field_presel_t cint_field_external_ud_apptype_ipmf1_presel_id_2 = 86;
bcm_field_group_t cint_field_external_ud_apptype_fg_id_ipmf2 = BCM_FIELD_INVALID;
bcm_field_qualify_t cint_field_external_ud_apptype_ipmf2_payload_qual = bcmFieldQualifyCount;
bcm_field_action_t cint_field_external_ud_apptype_ipmf2_action = bcmFieldActionTtlSet;

int
cint_field_external_ud_apptype_apptype_create(
    int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_t presel_id_1 = 71;
    bcm_field_presel_t presel_id_2 = 72;
    bcm_field_presel_t presel_id_3_1 = 73;
    bcm_field_presel_t presel_id_3_2 = 74;
    proc_name = "cint_field_external_ud_apptype_apptype_create";
    bcm_field_context_t app_type_user_1_as_context;
    bcm_field_context_t app_type_user_2_as_context;

    /* Crate new AppTypes.*/
    
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "user_apptype_1", sizeof(context_info.name));
    context_info.cascaded_from = cint_field_external_ud_apptype_external_app_type_predef + 0;
    app_type_user_1_as_context = cint_field_external_ud_apptype_external_app_type_user_1 + 0;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageExternal, &context_info, &app_type_user_1_as_context);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    cint_field_external_ud_apptype_external_app_type_user_1 = app_type_user_1_as_context + 0;
    printf("cint_field_external_ud_apptype_external_app_type_user_1: %d\n",cint_field_external_ud_apptype_external_app_type_user_1+0);

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "user_apptype_2", sizeof(context_info.name));
    context_info.cascaded_from = cint_field_external_ud_apptype_external_app_type_predef + 0;
    app_type_user_2_as_context = cint_field_external_ud_apptype_external_app_type_user_2 + 0;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageExternal, &context_info, &app_type_user_2_as_context);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    cint_field_external_ud_apptype_external_app_type_user_2 = app_type_user_2_as_context + 0;
    printf("cint_field_external_ud_apptype_external_app_type_user_2: %d\n",cint_field_external_ud_apptype_external_app_type_user_2+0);

    /* Add preselectors.*/

    bcm_field_presel_entry_id_info_init(&cint_field_external_ud_apptype_presel_entry_id_info_1_predef);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    cint_field_external_ud_apptype_presel_entry_id_info_1_predef.presel_id = presel_id_1;
    cint_field_external_ud_apptype_presel_entry_id_info_1_predef.stage = bcmFieldStageExternal;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = cint_field_external_ud_apptype_external_app_type_predef+0;
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[0].qual_arg = -1;
    presel_entry_data.qual_data[0].qual_value = 0x40;
    presel_entry_data.qual_data[0].qual_mask = 0x40;
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_info_1_predef, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&cint_field_external_ud_apptype_presel_entry_id_info_2_user_1);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    cint_field_external_ud_apptype_presel_entry_id_info_2_user_1.presel_id = presel_id_2;
    cint_field_external_ud_apptype_presel_entry_id_info_2_user_1.stage = bcmFieldStageExternal;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = cint_field_external_ud_apptype_external_app_type_user_1+0;
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 1;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTcp;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_info_2_user_1, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_1);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_1.presel_id = presel_id_3_1;
    cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_1.stage = bcmFieldStageExternal;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = cint_field_external_ud_apptype_external_app_type_user_2+0;
    presel_entry_data.nof_qualifiers = 1;
    /* Qualifying upon on UDP through layer record qualifier for JR2_A0 */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x12;
    presel_entry_data.qual_data[0].qual_mask = 0x7E;
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_1, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_2);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_2.presel_id = presel_id_3_2;
    cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_2.stage = bcmFieldStageExternal;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = cint_field_external_ud_apptype_external_app_type_user_2+0;
    presel_entry_data.nof_qualifiers = 1;
    /* Qualifying upon UDP through layer record type for JR2_B1 */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 1;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeUdp;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_2, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set\n", rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_ud_apptype_fg_add_predef(
    int unit,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *tmp_ptr;
    proc_name = "cint_field_external_ud_apptype_fg_add_predef";

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    dest_char = &(fg_info_p->name[0]);
    tmp_ptr = dest_char;

    snprintf(tmp_ptr, sizeof(fg_info_p->name), "ext_FG_predef");
    /*
     *  Add qualifiers to the Field Group
     */
    fg_info_p->qual_types[0] = bcmFieldQualifySrcMac;
    
    fg_info_p->nof_quals = 1;
    
    /*
     *  Create a void user defined action, if it was not created already.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = bcmFieldStageExternal;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = cint_field_external_ud_apptype_payload_size_predef;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_uda_predef", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_ud_apptype_external_uda_predef);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[0] = cint_field_external_ud_apptype_external_uda_predef;
    fg_info_p->action_with_valid_bit[0] = FALSE;

    fg_info_p->nof_actions = 1;

    rv = bcm_field_group_add(unit, 0, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_ud_apptype_fg_add_user_1(
    int unit,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *tmp_ptr;
    proc_name = "cint_field_external_ud_apptype_fg_add_user_1";

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    dest_char = &(fg_info_p->name[0]);
    tmp_ptr = dest_char;

    snprintf(tmp_ptr, sizeof(fg_info_p->name), "ext_FG_user_1");
    /*
     *  Add qualifiers to the Field Group
     */
    fg_info_p->qual_types[0] = bcmFieldQualifySrcMac;
    
    fg_info_p->nof_quals = 1;
    
    /*
     *  Create a void user defined action, if it was not created already.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = bcmFieldStageExternal;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = cint_field_external_ud_apptype_payload_size_user_1;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_uda_user_1", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_ud_apptype_external_uda_user_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[0] = cint_field_external_ud_apptype_external_uda_user_1;
    fg_info_p->action_with_valid_bit[0] = FALSE;

    fg_info_p->nof_actions = 1;

    rv = bcm_field_group_add(unit, 0, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_ud_apptype_fg_add_user_2(
    int unit,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_group_t * fg_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_action_info_t action_info;
    void *dest_char;
    char *tmp_ptr;
    proc_name = "cint_field_external_ud_apptype_fg_add_user_2";

    fg_info_p->stage = bcmFieldStageExternal;
    fg_info_p->fg_type = bcmFieldGroupTypeExternalTcam;

    dest_char = &(fg_info_p->name[0]);
    tmp_ptr = dest_char;

    snprintf(tmp_ptr, sizeof(fg_info_p->name), "ext_FG_user_2");
    /*
     *  Add qualifiers to the Field Group
     */
    fg_info_p->qual_types[0] = bcmFieldQualifySrcMac;

    fg_info_p->nof_quals = 1;
    
    /*
     *  Create a void user defined action, if it was not created already.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.stage = bcmFieldStageExternal;
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = cint_field_external_ud_apptype_payload_size_user_2;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "ext_uda_user_21", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_external_ud_apptype_external_uda_user_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    fg_info_p->action_types[0] = cint_field_external_ud_apptype_external_uda_user_2;
    fg_info_p->action_with_valid_bit[0] = FALSE;

    fg_info_p->nof_actions = 1;

    rv = bcm_field_group_add(unit, 0, fg_info_p, fg_id_p);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_action_create\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_ud_apptype_fg_attach(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p,
    bcm_field_AppType_t external_app_type,
    uint32 payload_id,
    uint32 payload_offset)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    uint32 qual_ndx, action_ndx;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_context_t context_id;
    proc_name = "cint_field_external_ud_apptype_fg_attach";

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

    group_attach_info.key_info.qual_info[0].input_arg = 0;
    group_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    group_attach_info.key_info.qual_info[0].offset = 0;
    
    /* Cast */
    context_id  = external_app_type + 0 ;

    /*
     * Call the attach API
     */
    rv = bcm_field_group_context_attach(unit, 0, external_fg_id, context_id, &group_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach fg_id %d external_app_type %d context %d\n", proc_name, rv, external_fg_id, (external_app_type + 0), context_id);
        return rv;
    }

    return rv;
}

int
cint_field_external_ud_apptype_entry_add(
    int unit,
    bcm_field_group_t external_fg_id,
    bcm_field_group_info_t *fg_info_p,
    int action_value,
    bcm_field_entry_t * entry_id_p)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t entry_info;

    proc_name = "cint_field_external_ud_apptype_entry_add";

    bcm_field_entry_info_t_init(&entry_info);

    entry_info.entry_qual[0].type = fg_info_p->qual_types[0];
    entry_info.entry_qual[0].value[0] = 0x07000100;
    entry_info.entry_qual[0].value[1] = 0x0000;
    entry_info.entry_qual[0].mask[0] = 0xFFFFFFFF;
    entry_info.entry_qual[0].mask[1] = 0xFFFF;

    entry_info.nof_entry_quals = fg_info_p->nof_quals;
    
    entry_info.entry_action[0].type = fg_info_p->action_types[0];
    entry_info.entry_action[0].value[0] = action_value;

    entry_info.nof_entry_actions = fg_info_p->nof_actions;
    
    entry_info.priority = 10;

    /*
     * Call the attach API
     */
    rv = bcm_field_entry_add(unit, 0, external_fg_id, &entry_info, entry_id_p);
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
cint_field_external_ud_apptype_context_ipmf1_create(
    int unit)
{
    char *proc_name;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t ipmf1_p_data; 
    void *dest_char;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_ud_apptype_context_ipmf1_create";
    
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "ext_PMF_test_1", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_external_ud_apptype_ipmf1_context_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "ext_PMF_test_2", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_external_ud_apptype_ipmf1_context_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&cint_field_external_ud_apptype_presel_entry_id_1);
    bcm_field_presel_entry_data_info_init(&ipmf1_p_data);
    
    cint_field_external_ud_apptype_presel_entry_id_1.presel_id = cint_field_external_ud_apptype_ipmf1_presel_id_1;
    cint_field_external_ud_apptype_presel_entry_id_1.stage = bcmFieldStageIngressPMF1;
    ipmf1_p_data.entry_valid = TRUE;
    ipmf1_p_data.context_id = cint_field_external_ud_apptype_ipmf1_context_1;
    ipmf1_p_data.nof_qualifiers = 1;
    ipmf1_p_data.qual_data[0].qual_type = bcmFieldQualifyAppType;
    ipmf1_p_data.qual_data[0].qual_value = cint_field_external_ud_apptype_external_app_type_user_1;
    ipmf1_p_data.qual_data[0].qual_mask = 0x3F;

    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_1, &ipmf1_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&cint_field_external_ud_apptype_presel_entry_id_2);
    bcm_field_presel_entry_data_info_init(&ipmf1_p_data);
    
    cint_field_external_ud_apptype_presel_entry_id_2.presel_id = cint_field_external_ud_apptype_ipmf1_presel_id_2;
    cint_field_external_ud_apptype_presel_entry_id_2.stage = bcmFieldStageIngressPMF1;
    ipmf1_p_data.entry_valid = TRUE;
    ipmf1_p_data.context_id = cint_field_external_ud_apptype_ipmf1_context_2;
    ipmf1_p_data.nof_qualifiers = 1;
    ipmf1_p_data.qual_data[0].qual_type = bcmFieldQualifyAppTypePredefined;
    ipmf1_p_data.qual_data[0].qual_value = cint_field_external_ud_apptype_external_app_type_predef;
    ipmf1_p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_2, &ipmf1_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in dnx_field_presel_set\n", proc_name, rv);
        return rv;
    }

    printf("Configured context ids %d and %d on iPMF1\n", 
           cint_field_external_ud_apptype_ipmf1_context_1, cint_field_external_ud_apptype_ipmf1_context_2);

    return rv;
}


int
cint_field_external_ud_apptype_fg_ipmf2_add(
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
    
    proc_name = "cint_field_external_ud_apptype_fg_ipmf2_add";

    bcm_field_group_info_t_init( &fg_info);
    

    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ext_PMF_FG", sizeof(fg_info.name));

    /*
     *  Create a user defined qualifier
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    rv = bcm_field_action_info_get(unit, cint_field_external_ud_apptype_ipmf2_action, bcmFieldStageIngressPMF2, &action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_dnx_field_action_info_get\n", proc_name, rv);
        return rv;
    }
    qual_info.size = action_info.size;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "ext_iPMF2", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_external_ud_apptype_ipmf2_payload_qual);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_qualifier_create\n", proc_name, rv);
        return rv;
    }

    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyExternalHit6;
    fg_info.qual_types[1] = cint_field_external_ud_apptype_ipmf2_payload_qual;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = cint_field_external_ud_apptype_ipmf2_action;
    fg_info.action_with_valid_bit[0] = TRUE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_external_ud_apptype_fg_id_ipmf2);
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
    attach_info.key_info.qual_info[1].offset = cint_field_external_ud_apptype_payload_offset;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_external_ud_apptype_fg_id_ipmf2, cint_field_external_ud_apptype_ipmf1_context_1, &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }

    printf("Config DE FG on iPMF2, fg_id=%d\n", cint_field_external_ud_apptype_fg_id_ipmf2);

    return rv;
}


/**
 * \brief - run external main function
 *
 * \param [in] unit - the unit number in system
 *
 *  Usage: run in cint shell "cint_field_external_ud_apptype_main(0)"
 */
int
cint_field_external_ud_apptype_main(int unit)
{
    char *proc_name;
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info_predef;
    bcm_field_group_info_t fg_info_user_1;
    bcm_field_group_info_t fg_info_user_2;

    proc_name = "cint_field_external_ud_apptype_main";

    bcm_field_group_info_t_init(&fg_info_predef);
    bcm_field_group_info_t_init(&fg_info_user_1);
    bcm_field_group_info_t_init(&fg_info_user_2);

    rv = cint_field_external_ud_apptype_apptype_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_apptype_create.\n", proc_name, rv);
        return rv;
    }

    rv = cint_field_external_ud_apptype_fg_add_predef(unit, &fg_info_predef, &cint_field_external_ud_apptype_fg_id_predef);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_fg_add_predef.\n", proc_name, rv);
        return rv;
    }
    rv = cint_field_external_ud_apptype_fg_add_user_1(unit, &fg_info_user_1, &cint_field_external_ud_apptype_fg_id_user_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_fg_add_user_1.\n", proc_name, rv);
        return rv;
    }
    rv = cint_field_external_ud_apptype_fg_add_user_2(unit, &fg_info_user_2, &cint_field_external_ud_apptype_fg_id_user_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_fg_add_user_2.\n", proc_name, rv);
        return rv;
    }

    rv = cint_field_external_ud_apptype_fg_attach(unit, cint_field_external_ud_apptype_fg_id_predef, &fg_info_predef, cint_field_external_ud_apptype_external_app_type_predef, cint_field_external_ud_apptype_payload_id, cint_field_external_ud_apptype_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_fg_attach fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_ud_apptype_fg_id_predef,  (cint_field_external_ud_apptype_external_app_type_predef + 0));
        return rv;
    }
    rv = cint_field_external_ud_apptype_fg_attach(unit, cint_field_external_ud_apptype_fg_id_user_1, &fg_info_user_1, cint_field_external_ud_apptype_external_app_type_user_1, cint_field_external_ud_apptype_payload_id, cint_field_external_ud_apptype_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_fg_attach fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_ud_apptype_fg_id_user_1,  (cint_field_external_ud_apptype_external_app_type_user_1 + 0));
        return rv;
    }
    rv = cint_field_external_ud_apptype_fg_attach(unit, cint_field_external_ud_apptype_fg_id_user_2, &fg_info_user_2, cint_field_external_ud_apptype_external_app_type_user_2, cint_field_external_ud_apptype_payload_id, cint_field_external_ud_apptype_payload_offset);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_fg_attach fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_ud_apptype_fg_id_user_2,  (cint_field_external_ud_apptype_external_app_type_user_2 + 0));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), bcm_switch_control_set - ExternalTcamSync failed fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_ud_apptype_fg_id,  (cint_field_external_ud_apptype_external_app_type_user_1 + 0));
        return rv;
    }

    rv = cint_field_external_ud_apptype_entry_add(unit, cint_field_external_ud_apptype_fg_id_predef, &fg_info_predef, cint_field_external_ud_apptype_payload_value_predef, &cint_field_external_ud_apptype_entry_id_predef);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_entry_add fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_ud_apptype_fg_id_predef,  (cint_field_external_ud_apptype_external_app_type_predef + 0));
        return rv;
    }
    rv = cint_field_external_ud_apptype_entry_add(unit, cint_field_external_ud_apptype_fg_id_user_1, &fg_info_user_1, cint_field_external_ud_apptype_payload_value_user_1, &cint_field_external_ud_apptype_entry_id_user_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_entry_add fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_ud_apptype_fg_id_user_1,  (cint_field_external_ud_apptype_external_app_type_user_1 + 0));
        return rv;
    }
    rv = cint_field_external_ud_apptype_entry_add(unit, cint_field_external_ud_apptype_fg_id_user_2, &fg_info_user_2, cint_field_external_ud_apptype_payload_value_user_2, &cint_field_external_ud_apptype_entry_id_user_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_entry_add fg_id %d external_app_type %d\n", proc_name, rv, cint_field_external_ud_apptype_fg_id_user_2,  (cint_field_external_ud_apptype_external_app_type_user_2 + 0));
        return rv;
    }

    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", cint_field_external_ud_apptype_fg_id_predef,  (cint_field_external_ud_apptype_external_app_type_predef + 0));
    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", cint_field_external_ud_apptype_fg_id_user_1,  (cint_field_external_ud_apptype_external_app_type_user_1 + 0));
    printf("Config external FG on IFWD2, fg_id=%d, external_app_type %d\n", cint_field_external_ud_apptype_fg_id_user_2,  (cint_field_external_ud_apptype_external_app_type_user_2 + 0));

    rv = cint_field_external_ud_apptype_context_ipmf1_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_context_ipmf1_create\n", proc_name, rv);
        return rv;
    }
    
    rv = cint_field_external_ud_apptype_fg_ipmf2_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_external_ud_apptype_fg_ipmf2_add\n", proc_name, rv);
        return rv;
    }

    return rv;
}

int
cint_field_external_ud_apptype_destroy(int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    char *proc_name;
    int rv = BCM_E_NONE;

    proc_name = "cint_field_external_ud_apptype_destroy";

    /*
     * At this point external field group cannot be deleted, so we only delete the entries.
     */
    rv = bcm_field_entry_delete(unit, cint_field_external_ud_apptype_fg_id_predef, NULL, cint_field_external_ud_apptype_entry_id_predef);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete predef \n", proc_name, rv, cint_field_external_ud_apptype_fg_id);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, cint_field_external_ud_apptype_fg_id_user_1, NULL, cint_field_external_ud_apptype_entry_id_user_1);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete user_1 \n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, cint_field_external_ud_apptype_fg_id_user_2, NULL, cint_field_external_ud_apptype_entry_id_user_2);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_entry_delete user_2 \n", proc_name, rv);
        return rv;
    }

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_1, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_2, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_info_1_predef, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_info_2_user_1, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_1, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    rv = bcm_field_presel_set(unit, 0, &cint_field_external_ud_apptype_presel_entry_id_info_3_user_2_2, &presel_entry_data);
    if(rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }

    return rv;
}

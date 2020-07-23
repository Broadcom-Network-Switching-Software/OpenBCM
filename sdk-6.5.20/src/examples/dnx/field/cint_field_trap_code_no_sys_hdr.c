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
 * cint ../../../src/examples/dnx/field/cint_field_trap_code_no_sys_hdr.c
 * cint;
 * int unit = 0;
 * bcm_field_presel_t presel_id = 50;
 * uint32 trap_code = 15;
 * uint32 trap_mask = 15;
 * field_trap_code_no_sys_hdr_main(unit, presel_id, trap_code, trap_mask);
 *
 * Configuration example end
 *
 * This CINT creates a new context and configures it to not use any system header,
 * then it adds a preselector of the given trap_code/trap_mask for the new context.
 * The preselector presel_id is also given.
 */

bcm_field_context_t context_id;
bcm_field_presel_entry_id_t presel_entry_id;

int field_trap_code_no_sys_hdr_main(int unit, bcm_field_presel_t presel_id, uint32 trap_code, uint32 trap_mask)
{
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t context_param;
    int rv = BCM_E_NONE;

    printf("Creating new context...\r\n");
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    printf("Successfully created new context %d\r\n", context_id);

    printf("Changing new context SYS HDR profile to None (No SYS HDRs are added)...\r\n");
    context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    context_param.param_arg = 0;
    context_param.param_val = bcmFieldSystemHeaderProfileNone;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &context_param);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set\n", rv);
        return rv;
    }

    printf("Adding trap_code preselector for the context...\r\n");
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    presel_data.entry_valid = TRUE;
    presel_data.context_id = context_id;
    presel_data.nof_qualifiers = 1;
    presel_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapCode;
    presel_data.qual_data[0].qual_value = trap_code;
    presel_data.qual_data[0].qual_mask = trap_mask;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }
    printf("CINT loaded successfully\r\n");

    return rv;
}

int field_trap_code_no_sys_hdr_destroy(int unit)
{
    bcm_field_presel_entry_data_t presel_data;
    int rv = BCM_E_NONE;

    printf("Disabling preselector...\r\n");
    presel_data.entry_valid = FALSE;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }

    printf("Destroying context...\r\n");

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy context_id %d \n", rv, context_id);
        return rv;
    }

    printf("CINT destroyed successfully\r\n");

    return rv;
}

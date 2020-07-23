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
 * cint ../../../src/examples/dnx/field/cint_field_exem_5fg_same_entries.c
 * cint;
 * int unit = 0;
 * cint_field_exem_5fg_same_entries_main(unit);
 *
 * Configuration example end
 *
 *   This CINT creates 5 EXEM field group with the same 3 entries values in IPMF1 stage.
 *   When configuring several EXEM field group with entries with the same values, collisions may occur.
 *   In order to prevent such a case, we're adding a qualifier key that is used as prefix.
 *   There are 2 ways of adding such prefix: const qualifier per field group and key_gen_var per context.
 *   This CINT shows how to set constant value per context and use it as a qualifier for building an entry key.
 *
 *   CINT flow:
 *    1. Create contexts.
 *    2. Set the contexts param to be bcmFieldContextParamTypeKeyVal.
 *    3. Create 5 field groups.
 *    4. Attach each FG to 2 contexts.
 *    5. Add the same 3 entries to each field group.
 */

 /**
  * Global variables
  */
int cint_field_exem_5fg_same_entries_nof_fgs = 5;
int cint_field_exem_5fg_same_entries_nof_entries_per_fg = 3;
int cint_field_exem_5fg_same_entries_nof_ctxs = cint_field_exem_5fg_same_entries_nof_fgs * 2;
bcm_field_group_t cint_field_exem_5fg_same_entries_fg_id_list[cint_field_exem_5fg_same_entries_nof_fgs];
bcm_field_context_t cint_field_exem_5fg_same_entries_context_id_list[cint_field_exem_5fg_same_entries_nof_ctxs];
bcm_field_stage_t cint_field_exem_5fg_same_entries_stage = bcmFieldStageIngressPMF1;
bcm_field_stage_t cint_field_exem_5fg_same_entries_ctx_stage = bcmFieldStageIngressPMF1;
bcm_field_entry_info_t cint_field_exem_5fg_same_entries_entry_info_list[cint_field_exem_5fg_same_entries_nof_fgs * cint_field_exem_5fg_same_entries_nof_entries_per_fg];
uint32 cint_field_exem_5fg_same_entries_src_ip_value = 0x01010101;
uint32 cint_field_exem_5fg_same_entries_dst_ip_value = 0x10101010;
bcm_field_qualify_t cint_field_exem_5fg_same_entries_key_gen_qual;

/**
 * The values for bcmFieldQualifyKeyGenVar that we add in the
 *  bcm_field_context_param_set and bcm_field_entry_add.
 */
uint32 cint_field_exem_5fg_same_entries_key_gen_values[cint_field_exem_5fg_same_entries_nof_fgs] = {0xABCDEF0, 0xABCDEF1, 0xABCDEF2, 0xABCDEF3, 0xABCDEF4};

/**
 * \brief
 *  This function creates an user defined qualifier
 *
 * \param [in]  unit            - Device id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_exem_5fg_same_entries_ud_qual_create(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;

    /**
     * Create 3 bit user defined qualifier based on bcmFieldQualifyKeyGenVar, to be used as a prefix qualifier
     * The qualifier size is 3 bit to support 5 field groups
     */
    qual_info.size = 3;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "prefix_qual", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_exem_5fg_same_entries_key_gen_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    return rv;
}
/**
 * \brief
 *  This function creates contexts
 *
 * \param [in]  unit            - Device id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_exem_5fg_same_entries_ctx_create(int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t param_info;
    int rv = BCM_E_NONE;
    int ii;

    for (ii = 0; ii < cint_field_exem_5fg_same_entries_nof_ctxs; ii++)
    {
        bcm_field_context_info_t_init(&context_info);
        rv = bcm_field_context_create(unit, 0, cint_field_exem_5fg_same_entries_ctx_stage, &context_info, &cint_field_exem_5fg_same_entries_context_id_list[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_context_create\n", rv);
            return rv;
        }

        printf("Context %d was created. \n", cint_field_exem_5fg_same_entries_context_id_list[ii]);
    }

    return rv;
}

/**
 * \brief
 *  This function creates field groups and attach them to 2 contexts each
 *
 * \param [in]  unit            - Device id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_exem_5fg_same_entries_fg_configure(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info[cint_field_exem_5fg_same_entries_nof_fgs];
    int ii;

    /*
     * Create field groups
     */
    for (ii = 0; ii < cint_field_exem_5fg_same_entries_nof_fgs; ii++)
    {
        rv = cint_field_exem_5fg_same_entries_add_field_group(unit, &fg_info[ii], ii);

        if(rv != BCM_E_NONE)
        {
            printf("Error (%d) in cint_field_exem_5fg_same_entries_add_field_group\n", rv);
            return rv;
        }
    }

    /**
     * Attach field groups to contexts
     */
    for (ii = 0; ii < cint_field_exem_5fg_same_entries_nof_fgs; ii++)
    {
        rv = cint_field_exem_5fg_same_entries_attach_field_group(unit, fg_info[ii], ii);

        if(rv != BCM_E_NONE)
        {
            printf("Error (%d) in cint_field_exem_5fg_same_entries_attach_field_group\n", rv);
            return rv;
        }
    }

    /**
     * Add entries to the field groups
     */
    for (ii = 0; ii < cint_field_exem_5fg_same_entries_nof_fgs; ii++)
    {
        rv = cint_field_exem_5fg_same_entries_add_entries(unit, fg_info[ii], ii);

        if(rv != BCM_E_NONE)
        {
            printf("Error (%d) in cint_field_exem_5fg_same_entries_add_entries\n", rv);
            return rv;
        }
    }

    return rv;
}

/**
* \brief
*  This function creates field group
* \param [in] unit           - Device ID.
* \param [in] fg_info        - field group info.
* \param [in] fg_idx         - field group index.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_exem_5fg_same_entries_add_field_group(
        int unit,
        bcm_field_group_info_t *fg_info,
        int fg_idx)
{
    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeExactMatch;
    fg_info->stage = cint_field_exem_5fg_same_entries_stage;
    fg_info->nof_quals = 3;
    fg_info->qual_types[0] = cint_field_exem_5fg_same_entries_key_gen_qual;
    fg_info->qual_types[1] = bcmFieldQualifySrcIp;
    fg_info->qual_types[2] = bcmFieldQualifyDstIp;
    fg_info->nof_actions = 1;
    fg_info->action_types[0] = bcmFieldActionPrioIntNew;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, fg_info, &cint_field_exem_5fg_same_entries_fg_id_list[fg_idx]);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Field Group %d was created. \n", cint_field_exem_5fg_same_entries_fg_id_list[fg_idx]);

    return rv;
}

/**
* \brief
*  This function attach given field group to 2 contexts
* \param [in] unit           - Device ID.
* \param [in] fg_info        - field group info.
* \param [in] fg_idx         - field group index.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_exem_5fg_same_entries_attach_field_group(
        int unit,
        bcm_field_group_info_t fg_info,
        int fg_idx)
{
    int rv = BCM_E_NONE;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_context_param_info_t param_info;

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];

    /** Get the offset of KEY_GEN_VAR qualifier. */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF1, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    /** key_gen is located in the bits 0:2 from KEY_GEN_VAR. */
    attach_info.key_info.qual_info[0].offset = qual_info_get.offset;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[2].input_arg = 0;
    attach_info.key_info.qual_info[2].offset = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    /**
     * Attach the field group to 2 contexts
     */
    rv = bcm_field_group_context_attach(unit, 0, cint_field_exem_5fg_same_entries_fg_id_list[fg_idx], cint_field_exem_5fg_same_entries_context_id_list[fg_idx * 2], &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Field Group %d was attached to context %d. \n", cint_field_exem_5fg_same_entries_fg_id_list[fg_idx], cint_field_exem_5fg_same_entries_context_id_list[fg_idx * 2]);

    rv = bcm_field_group_context_attach(unit, 0, cint_field_exem_5fg_same_entries_fg_id_list[fg_idx], cint_field_exem_5fg_same_entries_context_id_list[fg_idx * 2 + 1], &attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Field Group %d was attached to context %d. \n", cint_field_exem_5fg_same_entries_fg_id_list[fg_idx], cint_field_exem_5fg_same_entries_context_id_list[fg_idx * 2 + 1]);

    /**
     * Fill the info for context_param_set
     * Define a constant value that can be used as a qualifier for lookups performed by the contexts, by setting the contexts param_type to be bcmFieldContextParamTypeKeyVal
     */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = cint_field_exem_5fg_same_entries_key_gen_values[fg_idx];
    param_info.param_arg = 0;

    rv = bcm_field_context_param_set(unit, 0, cint_field_exem_5fg_same_entries_ctx_stage, cint_field_exem_5fg_same_entries_context_id_list[fg_idx * 2], &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set\n", rv);
        return rv;
    }

    rv = bcm_field_context_param_set(unit, 0, cint_field_exem_5fg_same_entries_ctx_stage, cint_field_exem_5fg_same_entries_context_id_list[fg_idx * 2 + 1], &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set\n", rv);
        return rv;
    }

    return rv;
}

/**
* \brief
*  This function adds entries to given field group
* \param [in] unit           - Device ID.
* \param [in] fg_info        - field group info.
* \param [in] fg_idx         - field group index.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_exem_5fg_same_entries_add_entries(
        int unit,
        bcm_field_group_info_t fg_info,
        int fg_idx)
{
   int rv = BCM_E_NONE;
   int jj;

   /**
    * Add 5 entries to the field group, with the same values, except key_gen_var
    */
   for (jj = 0; jj < cint_field_exem_5fg_same_entries_nof_entries_per_fg ; jj++)
   {
       int entry_index = fg_idx * cint_field_exem_5fg_same_entries_nof_entries_per_fg + jj;
       bcm_field_entry_info_t_init(&cint_field_exem_5fg_same_entries_entry_info_list[entry_index]);

       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].nof_entry_quals = fg_info.nof_quals;
       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].entry_qual[0].type = fg_info.qual_types[0];
       /**
        * Fill key_gen_qual value with 3 lsb bits of the value set in bcm_field_context_param_set
        */
       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].entry_qual[0].value[0] = cint_field_exem_5fg_same_entries_key_gen_values[fg_idx] & 7;
       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].entry_qual[1].type = fg_info.qual_types[1];
       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].entry_qual[1].value[0] = (jj+1) * cint_field_exem_5fg_same_entries_src_ip_value;
       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].entry_qual[2].type = fg_info.qual_types[2];
       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].entry_qual[2].value[0] = (jj+1) * cint_field_exem_5fg_same_entries_dst_ip_value;

       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].nof_entry_actions = fg_info.nof_actions;
       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].entry_action[0].type = fg_info.action_types[0];
       cint_field_exem_5fg_same_entries_entry_info_list[entry_index].entry_action[0].value[0] = 1;

       rv = bcm_field_entry_add(unit, 0, cint_field_exem_5fg_same_entries_fg_id_list[fg_idx], &cint_field_exem_5fg_same_entries_entry_info_list[entry_index], NULL);
       if(rv != BCM_E_NONE)
       {
           printf("Error (%d) in bcm_field_entry_add\n", rv);
           return rv;
       }
   }

   printf("%d Entries were added to field Group %d.\n", jj, cint_field_exem_5fg_same_entries_fg_id_list[fg_idx]);

   return rv;
}

 /**
 * \brief
 *  This function runs the external main function
 * \param [in] unit           - Device ID.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_exem_5fg_same_entries_main(int unit)
{
    int rv = BCM_E_NONE;

    /**
     * Create qualifier
     */
    rv = cint_field_exem_5fg_same_entries_ud_qual_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_exem_5fg_same_entries_ud_qual_create\n", rv);
        return rv;
    }

    /*
     * Create contexts
     */
    rv = cint_field_exem_5fg_same_entries_ctx_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_exem_5fg_same_entries_ctx_create\n", rv);
        return rv;
    }

    /*
     * Create field groups and add entries
     */
    rv = cint_field_exem_5fg_same_entries_fg_configure(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_exem_5fg_same_entries_fg_add\n", rv);
        return rv;
    }
    return rv;
}

/**
 * \brief
 *  This function destroys all allocated data by the configuration
 *
 * \param [in]  unit            - Device id
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_exem_5fg_same_entries_destroy(int unit)
{
    int rv = BCM_E_NONE;
    int ii, jj;

    /**
     * Loop over all the field groups
     */
    for (ii = 0; ii < cint_field_exem_5fg_same_entries_nof_fgs; ii++)
    {
        for (jj = 0; jj < cint_field_exem_5fg_same_entries_nof_entries_per_fg; jj++)
        {
            /*
             * Delete the entries from the field group
             */
            rv = bcm_field_entry_delete(unit, cint_field_exem_5fg_same_entries_fg_id_list[ii], cint_field_exem_5fg_same_entries_entry_info_list[ii * cint_field_exem_5fg_same_entries_nof_entries_per_fg + jj].entry_qual, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_entry_delete\n", rv);
                return rv;
            }
        }

        printf("%d Entries were deleted from field group %d.\n", jj, cint_field_exem_5fg_same_entries_context_id_list[ii * 2]);

        /*
         * Detach the field group from its contexts
         */
        rv = bcm_field_group_context_detach(unit, cint_field_exem_5fg_same_entries_fg_id_list[ii], cint_field_exem_5fg_same_entries_context_id_list[ii * 2]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_group_context_detach\n");
            return rv;
        }

        printf("Field Group %d was detach from context %d. \n", cint_field_exem_5fg_same_entries_fg_id_list[ii], cint_field_exem_5fg_same_entries_context_id_list[ii * 2]);

        rv = bcm_field_group_context_detach(unit, cint_field_exem_5fg_same_entries_fg_id_list[ii], cint_field_exem_5fg_same_entries_context_id_list[ii * 2 + 1]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_group_context_detach\n");
            return rv;
        }

        printf("Field Group %d was detach from context %d.\n", cint_field_exem_5fg_same_entries_fg_id_list[ii], cint_field_exem_5fg_same_entries_context_id_list[ii * 2 + 1]);

        /*
         * Delete the field group
         */
        rv = bcm_field_group_delete(unit, cint_field_exem_5fg_same_entries_fg_id_list[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_group_delete\n");
            return rv;
        }

        printf("Field Group %d was deleted.\n", cint_field_exem_5fg_same_entries_fg_id_list[ii]);
    }

    /*
     * Delete the contexts
     */
    for(ii = 0; ii < cint_field_exem_5fg_same_entries_nof_ctxs; ii++)
    {
        rv = bcm_field_context_destroy(unit, cint_field_exem_5fg_same_entries_ctx_stage, cint_field_exem_5fg_same_entries_context_id_list[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_context_destroy\n");
            return rv;
        }
         printf("Context %d was deleted.\n", cint_field_exem_5fg_same_entries_context_id_list[ii]);
     }

    /*
     * Delete the user defined qualifier
     */
    rv = bcm_field_qualifier_destroy(unit, cint_field_exem_5fg_same_entries_key_gen_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy.\n");
        return rv;
    }
    printf("Qualifier was deleted.\n");

    return rv;
}


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
 * cint ../../../src/examples/dnx/field/cint_field_ace.c
 * cint;
 * int unit = 0;
 * bcm_field_context_t epmf_context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 * int write_value = 1;
 * field_ace_main(unit, epmf_context_id, write_value);
 *
 * Configuration example end
 *
 *   This cint creates an ACE format and uses it for traffic testing.
 */



bcm_field_ace_format_t ace_format_id;
uint32 ace_entry_handle;
bcm_field_group_t epmf_fg_id;
bcm_field_entry_t epmf_entry_handle;
bcm_field_action_t Cint_field_ace_action_id;
/** The number of bits in the smallest action used.*/
int min_size=2;


/**
* \brief
*  Creates an ACE format and ACE entry and an ePMF field group that points to the ACE entry.
* \param [in] unit             - Device ID
* \param [in] epmf_context_id  - Context to attach ePMF FG to
* \param [in] write_value      - The value the ACE entry is supposed to write.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_ace_main(int unit, bcm_field_context_t epmf_context_id, int write_value)
{
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_group_info_t ipmf1_fg_info;
    bcm_field_group_attach_info_t ipmf1_attach_info;
    bcm_field_group_info_t epmf_fg_info;
    bcm_field_group_attach_info_t epmf_attach_info;
    bcm_field_entry_info_t epmf_entry_info;
    bcm_field_action_info_t action_info;
    void * dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;
    unsigned int uwriteval;

    uwriteval=write_value;
    if ((uwriteval>>min_size) != 0)
    {
        printf("write_value (0x%x) cannot use more than %d bits.\n", write_value, min_size);
        return BCM_E_PARAM;
    }

    /**
    * Init the used structures
    */
   bcm_field_action_info_t_init(&action_info);
   /**
    * Fill action info
    */
   action_info.action_type = bcmFieldActionAceContextValue;
   dest_char = &(action_info.name[0]);
   sal_strncpy_s(dest_char, "uda_ace_context", sizeof(action_info.name));
   action_info.stage = bcmFieldStageEgressExtension;
   action_info.size = 0;
   action_info.prefix_size = 16;
   action_info.prefix_value = 0xFFF0;
   rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_ace_action_id);
   if (rv != BCM_E_NONE)
   {
       printf("%s(): Error (%d), in bcm_field_action_create \n", proc_name, rv);
       return rv;
   }

    /* 
     * Configure the ACE format.
     */
    bcm_field_ace_format_info_t_init(&ace_format_info);
 
    ace_format_info.nof_actions = 4;

    ace_format_info.action_types[0] = bcmFieldActionPrioIntNew;
    ace_format_info.action_types[1] = bcmFieldActionTtlSet;
    ace_format_info.action_types[2] = bcmFieldActionDropPrecedence;
    ace_format_info.action_types[3] = Cint_field_ace_action_id;


    /* We do not give the TTL action a valid bit.*/
    ace_format_info.action_with_valid_bit[1]=FALSE;

    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;

    ace_entry_info.entry_action[0].type = bcmFieldActionPrioIntNew;
    ace_entry_info.entry_action[0].value[0] = write_value;
    ace_entry_info.entry_action[1].type = bcmFieldActionTtlSet;
    ace_entry_info.entry_action[1].value[0] = write_value;
    ace_entry_info.entry_action[2].type = bcmFieldActionDropPrecedence;
    ace_entry_info.entry_action[2].value[0] = write_value;
    ace_entry_info.entry_action[3].type = Cint_field_ace_action_id;
    ace_entry_info.entry_action[3].value[0] = bcmFieldAceContextNull;
    

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }
    /*
     * Create a field group in ePMF that uses the ACE entry.
     */
    
    bcm_field_group_info_t_init(&epmf_fg_info);
    epmf_fg_info.fg_type = bcmFieldGroupTypeTcam;
    epmf_fg_info.nof_quals = 1;
    epmf_fg_info.stage = bcmFieldStageEgress;
    epmf_fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    epmf_fg_info.nof_actions = 3;
    /* The first action decides the ACE entry that will happen. */
    epmf_fg_info.action_types[0] = bcmFieldActionAceEntryId;
    /* Two more actions will set values to TC an DP fields during the ePMF stage before the ACE ACR.*/
    epmf_fg_info.action_types[1] = bcmFieldActionPrioIntNew;
    epmf_fg_info.action_types[2] = bcmFieldActionDropPrecedence;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &epmf_fg_info, &epmf_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&epmf_entry_info);
    epmf_entry_info.nof_entry_quals = epmf_fg_info.nof_quals;
    epmf_entry_info.nof_entry_actions = epmf_fg_info.nof_actions;
    for(ii=0; ii< epmf_fg_info.nof_quals; ii++)
    {
        epmf_entry_info.entry_qual[ii].type = epmf_fg_info.qual_types[ii];
    }
    for(ii=0; ii<epmf_fg_info.nof_actions; ii++)
    {
        epmf_entry_info.entry_action[ii].type = epmf_fg_info.action_types[ii];
    }
    epmf_entry_info.entry_qual[0].mask[0] = 0;
    epmf_entry_info.entry_action[0].value[0] = ace_entry_handle;
    epmf_entry_info.entry_action[1].value[0] = 3;
    epmf_entry_info.entry_action[2].value[0] = 3;

    rv = bcm_field_entry_add(unit, 0, epmf_fg_id, &epmf_entry_info, &epmf_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }  
    
    bcm_field_group_attach_info_t_init(&epmf_attach_info);
    epmf_attach_info.key_info.nof_quals = epmf_fg_info.nof_quals;
    epmf_attach_info.payload_info.nof_actions = epmf_fg_info.nof_actions;
    for(ii=0; ii< epmf_fg_info.nof_quals; ii++)
    {
        epmf_attach_info.key_info.qual_types[ii] = epmf_fg_info.qual_types[ii];
    }
    for(ii=0; ii<epmf_fg_info.nof_actions; ii++)
    {
        epmf_attach_info.payload_info.action_types[ii] = epmf_fg_info.action_types[ii];
    }
    epmf_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    epmf_attach_info.key_info.qual_info[0].input_arg = 0;
    epmf_attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, epmf_fg_id, epmf_context_id, &epmf_attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }  

    printf("ACE format (%d), ACE entry (%d) and ePMF FG (%d) created. FG attached to context %d.\n",
           ace_format_id, ace_entry_handle, epmf_fg_id, epmf_context_id);
    return rv;
    
}

/**
* \brief
*  Delete the ACE format and the ePMF field group that uses it.
* \param [in] unit             - Device ID
* \param [in] epmf_context_id  - Context to detach the ePMF field group info from.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_ace_destroy(int unit, bcm_field_context_t epmf_context_id)
{
    int rv = BCM_E_NONE;
    int ii= 0;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];

    rv = bcm_field_group_context_detach(unit, epmf_fg_id, epmf_context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach\n", rv);
        return rv;
    }

    rv = bcm_field_entry_delete(unit, epmf_fg_id, &entry_qual_info, epmf_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }
  
    rv = bcm_field_group_delete(unit, epmf_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    rv = bcm_field_ace_entry_delete(unit, ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_delete\n", rv);
        return rv;
    }   
  
    rv = bcm_field_ace_format_delete(unit, ace_format_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_delete\n", rv);
        return rv;
    }
    
    rv = bcm_field_action_destroy(unit, Cint_field_ace_action_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_destroy\n", rv);
        return rv;
    }

    return rv;
}

/**
* \brief
*  Update the value of the action in the ACE entry.
* \param [in] unit             - Device ID
* \param [in] epmf_context_id  - Context to attach ePMF FG to
* \param [in] write_value      - The value the ACE entry is supposed to write.
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_ace_update(int unit, int write_value)
{
    bcm_field_ace_entry_info_t ace_entry_info;
    int rv = BCM_E_NONE;
    unsigned int uwriteval;
    int flags;

    uwriteval=write_value;
    if ((uwriteval>>min_size) != 0)
    {
        printf("write_value (0x%x) cannot use more than %d bits.\n", write_value, min_size);
        return BCM_E_PARAM;
    }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    ace_entry_info.nof_entry_actions = 3;
    
    ace_entry_info.entry_action[0].type = bcmFieldActionPrioIntNew;
    ace_entry_info.entry_action[0].value[0] = write_value;
    ace_entry_info.entry_action[1].type = bcmFieldActionTtlSet;
    ace_entry_info.entry_action[1].value[0] = write_value;
    ace_entry_info.entry_action[2].type = bcmFieldActionDropPrecedence;
    ace_entry_info.entry_action[2].value[0] = write_value;
    
    flags = BCM_FIELD_FLAG_UPDATE;
    rv = bcm_field_ace_entry_add(unit, flags, ace_format_id, &ace_entry_info, &ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    printf("Updated ACE entry %d with value %d.\n", ace_entry_handle, write_value);
    return rv;
    
}

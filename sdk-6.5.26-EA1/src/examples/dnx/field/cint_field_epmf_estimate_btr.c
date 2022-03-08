/*
 * Configuration example:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../src/examples/dnx/field/cint_field_epmf_estimate_btr.c
 * cint;
 * int unit = 0;
 * cint_field_epmf_estimate_btr_create_main(unit, port_profile, estimate_btr);
 *
 * The cint add to "estimate bytes to remove" a const value from EPMF, based on the outPort and No-PPH indication.
 * Example of possible usage: If the port configured with MACSEC enabled (by API bcm_xflow_macsec_port_control_set)
 * SDK assume SVTAG will be added, and add its size to the estimate bytes to add.
 * But if packet injected with ITMH.Type=No-PPH, SVTAG will not be added (It should injected already with SVTAG).
 * To compensate it, user can add EPMF rule, which add to estimate bytes to remove the SVTAG size (4 bytes).
 */

 /**
  * Global variables
  */
bcm_field_group_t epmf_estimate_btr_fg_id = BCM_FIELD_ID_INVALID;
bcm_field_context_t epmf_estimate_btr_context_id = BCM_FIELD_ID_INVALID;
bcm_field_presel_t  epmf_estimate_btr_presel_id = 50;

/**
* \brief
*  Runs main function for epmf estimate btr. CS qual: out_port_profile and GTMH-NO-PPH
* \param [in] unit       -  Device id
* \param [in] out_port   -  out local port
* \param [in] estimate_btr_value - estimate bytes to remove value
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_epmf_estimate_btr_create_main(
    int unit,
    int out_pp_port_profile,
    int estimate_btr_value)
{
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_t action_id;
    bcm_field_action_info_t action_info, action_info_get;
    void *dest_char;
    if ( (*dnxc_data_get(unit, "dev_init", "general", "egress_estimated_bta_btr_hw_config", NULL)) &&
         (epmf_estimate_btr_context_id == BCM_FIELD_ID_INVALID))
    {
        /*
         * create a direct Extraction Field Group in ePMF
         */
        bcm_field_context_info_t_init(&context_info);
        dest_char = &(context_info.name[0]);
        sal_strncpy_s(dest_char, "SVTAG_NO_PPH_BTR_EPMF_CTX", sizeof(context_info.name));
        rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &epmf_estimate_btr_context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_context_create\n");
            return rv;
        }
        /***
         * create the context selection based on out_port and No-PPH
         * */
        bcm_field_presel_entry_id_info_init(&p_id);
        bcm_field_presel_entry_data_info_init(&p_data);

        p_id.presel_id = epmf_estimate_btr_presel_id;
        p_id.stage = bcmFieldStageEgress;
        p_data.entry_valid = TRUE;
        p_data.context_id = epmf_estimate_btr_context_id;
        p_data.nof_qualifiers = 2;

        p_data.qual_data[0].qual_type = bcmFieldQualifyPphPresent;
        p_data.qual_data[0].qual_arg = 0;
        p_data.qual_data[0].qual_value = 0x0;
        p_data.qual_data[0].qual_mask = 0x1;
        p_data.qual_data[1].qual_type = bcmFieldQualifyPortClassPacketProcessing;
        p_data.qual_data[1].qual_arg = 0;
        p_data.qual_data[1].qual_value = out_pp_port_profile;
        p_data.qual_data[1].qual_mask = 0x7;

        rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_presel_set\n");
            return rv;
        }

        bcm_field_group_info_t_init(&fg_info);

        fg_info.fg_type = bcmFieldGroupTypeConst;
        fg_info.stage = bcmFieldStageEgress;
        dest_char = &(fg_info.name[0]);
        sal_strncpy_s(dest_char, "SVTAG_NO_PPH_BTR_EPMF_FG", sizeof(fg_info.name));
        fg_info.nof_quals = 0;
        fg_info.nof_actions = 1;

        bcm_field_action_info_t_init(&action_info);
        rv = bcm_field_action_info_get
                        (unit, bcmFieldActionEstimatedStartPacketStrip, bcmFieldStageEgress, &action_info_get);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_info_get\n");
            return rv;
        }
        action_info.action_type = bcmFieldActionEstimatedStartPacketStrip;
        action_info.size = 0;
        action_info.prefix_size = action_info_get.size - action_info.size;
        action_info.prefix_value = estimate_btr_value;
        action_info.stage = bcmFieldStageEgress;
        dest_char = &(action_info.name[0]);
        sal_strncpy_s(dest_char, "SVTAG_NO_PPH_BTR_EPMF_ACT", sizeof(action_info.name));
        rv = bcm_field_action_create(unit, 0, &action_info, &action_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_action_create\n");
            return rv;
        }

        bcm_field_group_info_t_init(&fg_info);

        fg_info.fg_type = bcmFieldGroupTypeConst;
        fg_info.stage = bcmFieldStageEgress;
        dest_char = &(fg_info.name[0]);
        sal_strncpy_s(dest_char, "SVTAG_NO_PPH_BTR_EPMF_ACT", sizeof(fg_info.name));
        fg_info.nof_quals = 0;
        fg_info.nof_actions = 1;

        fg_info.action_types[0] = action_id;
        fg_info.action_with_valid_bit[0] = 0;
        /** Create the field group. */
        rv = bcm_field_group_add(unit, 0, &fg_info, &epmf_estimate_btr_fg_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_group_add\n");
            return rv;
        }

        bcm_field_group_attach_info_t_init(&attach_info);
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        attach_info.payload_info.action_types[0] = fg_info.action_types[0];

        /** Attach the created field group to the context. */
        rv = bcm_field_group_context_attach(unit, 0, epmf_estimate_btr_fg_id, epmf_estimate_btr_context_id, &attach_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_group_context_attach\n");
            return rv;
        }

        printf("epmf rule was created: Field Group %d, context %d. \n", epmf_estimate_btr_fg_id, epmf_estimate_btr_context_id);

    }
    return rv;
}

/**
* \brief
*  Destroy all configuration
* \param [in] unit        -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
cint_field_epmf_estimate_btr_destroy(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = epmf_estimate_btr_presel_id;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = FALSE;
    p_data.context_id = epmf_estimate_btr_context_id;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    /*
     * Detach the TCAM FG from its context
     */
    rv = bcm_field_group_context_detach(unit, epmf_estimate_btr_fg_id, epmf_estimate_btr_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    printf("TCAM Field Group %d was detach from context %d. \n", epmf_estimate_btr_fg_id, epmf_estimate_btr_context_id);

    /*
     * Delete FG
     */
    rv = bcm_field_group_delete(unit, epmf_estimate_btr_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    printf("TCAM Field Group %d was deleted. \n", epmf_estimate_btr_fg_id);

    /*
     * Delete the context
     */
    rv = bcm_field_context_destroy(unit, bcmFieldStageEgress, epmf_estimate_btr_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy context_id %d \n", rv, epmf_estimate_btr_context_id);
        return rv;
    }
    return rv;
}

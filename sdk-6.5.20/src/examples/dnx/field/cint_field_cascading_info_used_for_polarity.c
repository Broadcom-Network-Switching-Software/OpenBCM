/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 *  Using cascading info as polarity value.
 *
 *  Configuration example start:
 *
 *  cint;
 *  cint_reset();
 *  exit;
 *  cint ../../src/examples/dnx/field/cint_field_cascading_info_used_for_polarity.c
 *  cint;
 *  int unit = 0;
 *  bcm_field_context_t context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
 *  int dst_mac_lsb = 0x02001003;
 *  bcm_port_t in_port = 200;
 *  cint_field_cascading_info_used_for_polarity_main(unit, in_port, dst_mac_lsb, context_id);
 *
 *  Configuration example end
 *
 *  This cint shows how to use IPMF1 cascading info in IPMF2 as value for polarity bit.
 *  In case of TCAM hit, Trap action occur. In case of TCAM miss, Snoop action occur.
 *
 *  CINT configuration:
 *      - Add a TCAM fg in iPMF1 with action void 1 bit
 *          - Qualifier of the packet are DA + in_port
 *      - Add  2 DE FG in iPMF2  both are set to use_valid_bit=TRUE
 *          - 1st FG has polarity 1 with action Trap and the condition is the cascading bit from iPMF1
 *          - 2nd FG has polarity 0 with action Snoop and the condition is the cascading bit from iPMF1
 */

bcm_gport_t Cint_field_cascading_info_used_for_polarity_de_const_trap_q_val;
bcm_gport_t Cint_field_cascading_info_used_for_polarity_de_const_snp_q_val;
int Cint_field_cascading_info_used_for_polarity_trap_code = 0x4f;
int Cint_field_cascading_info_used_for_polarity_trap_str = 0x7;
int Cint_field_cascading_info_used_for_polarity_trap_qual = 0xf;
int Cint_field_cascading_info_used_for_polarity_snp_code = 0x3f;

/** Global variables to be passed through function. */
bcm_field_group_t Cint_field_cascading_info_used_for_polarity_tcam_fg_id;
bcm_field_entry_t Cint_field_cascading_info_used_for_polarity_tcam_entry_handle;
bcm_field_action_t Cint_field_cascading_info_used_for_polarity_tcam_void_action_id;

bcm_field_qualify_t Cint_field_cascading_info_used_for_polarity_de_val_bit_qual_id;
bcm_field_qualify_t Cint_field_cascading_info_used_for_polarity_de_const_trap_q_id;
bcm_field_qualify_t Cint_field_cascading_info_used_for_polarity_de_const_snoop_q_id;
bcm_field_group_t Cint_field_cascading_info_used_for_polarity_de_trap_fg_id;
bcm_field_group_t Cint_field_cascading_info_used_for_polarity_de_snoop_fg_id;

/**
 * \brief
 *  Creates 1b user qualifier to qualify upon cascading result from IPMF1.
 *
 * \param [in] unit   - Device id
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_cascading_info_used_for_polarity_val_bit_qual_create(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;

    /** Create 1b user qualifier to qualify upon cascading result from IPMF1. */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "Polarity_val_bit", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_cascading_info_used_for_polarity_de_val_bit_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_create\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function creates a TCAM field group,
 *  attaching it to a context and adding an entry.
 *
 * \param [in] unit   - Device id
 * \param [in] in_port  - In_port to qualify upon.
 * \param [in] dst_mac_lsb  - 32 lsb bits of the DST_MAC address to qualify upon.
 * \param [in] context_id  - Context id.
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_cascading_info_used_for_polarity_tcam_fg_create(
    int unit,
    bcm_port_t in_port,
    int dst_mac_lsb,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;
    bcm_field_action_info_t action_info;
    bcm_field_group_t fg_id;
    bcm_gport_t in_gport;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_entry_info_t_init(&entry_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Polarity_TCAM", sizeof(fg_info.name));
    fg_info.nof_quals = 2;
    fg_info.nof_actions = 1;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.priority = 1;

    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.qual_types[1] = bcmFieldQualifyDstMac;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;
    BCM_GPORT_LOCAL_SET(in_gport, in_port);
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = in_gport;
    entry_info.entry_qual[0].mask[0] = 0x1FF;
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[1].value[0] = dst_mac_lsb;
    entry_info.entry_qual[1].mask[0] = 0xFFFFFFFF;

    /** Create an user action which will be used as condition bit in DE FGs in IPMF2 stage. */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "Polarity_TCAM", sizeof(action_info.name));
    action_info.size = 1;
    action_info.stage = bcmFieldStageIngressPMF1;
    action_info.prefix_size = 31;
    action_info.prefix_value = 0;
    rv = bcm_field_action_create(unit, 0, &action_info, &Cint_field_cascading_info_used_for_polarity_tcam_void_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create on action_id = '%s' \n", rv, action_info.name);
        return rv;
    }

    fg_info.action_types[0] = Cint_field_cascading_info_used_for_polarity_tcam_void_action_id;
    fg_info.action_with_valid_bit[0] = FALSE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 1;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &Cint_field_cascading_info_used_for_polarity_tcam_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, Cint_field_cascading_info_used_for_polarity_tcam_fg_id);
        return rv;
    }
    printf("Field Group ID %d was created. \n", Cint_field_cascading_info_used_for_polarity_tcam_fg_id);

    rv = bcm_field_group_context_attach(unit, 0, Cint_field_cascading_info_used_for_polarity_tcam_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_cascading_info_used_for_polarity_tcam_fg_id, context_id);

    rv = bcm_field_entry_add(unit, 0, Cint_field_cascading_info_used_for_polarity_tcam_fg_id, &entry_info, &Cint_field_cascading_info_used_for_polarity_tcam_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", Cint_field_cascading_info_used_for_polarity_tcam_entry_handle,
            Cint_field_cascading_info_used_for_polarity_tcam_entry_handle, Cint_field_cascading_info_used_for_polarity_tcam_fg_id);

    return rv;
}

/**
 * \brief
 *  This function creates a DE field group and attaching it to a context.
 *
 * \param [in] unit   - Device id
 * \param [in] is_trap   - Indicates if the function will be called to create
 *  DE FG with action trap, otherwise it will create DE FG with action snoop.
 * \param [in] context_id  - Context id.
 * \param [out] fg_id  - Field group ID, which was created.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_cascading_info_used_for_polarity_de_fg_create(
    int unit,
    int is_trap,
    bcm_field_context_t context_id,
    bcm_field_group_t * fg_id)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    int offset_of_valid_bit_of_ipmf1_tcam_action;
    bcm_field_qualifier_info_create_t qual_info;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "EFES_polarity", sizeof(fg_info.name));
    fg_info.nof_quals = 2;
    fg_info.nof_actions = 1;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    /** Get the offset of the user created action in IPMF1 TCAM FG, will be used as condition bit in DE FGs. */
    rv = bcm_field_group_action_offset_get(unit, 0, Cint_field_cascading_info_used_for_polarity_tcam_fg_id,
                                           Cint_field_cascading_info_used_for_polarity_tcam_void_action_id, &offset_of_valid_bit_of_ipmf1_tcam_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_action_offset_get\n", rv);
        return rv;
    }

    fg_info.qual_types[0] = Cint_field_cascading_info_used_for_polarity_de_val_bit_qual_id;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[0].input_arg = Cint_field_cascading_info_used_for_polarity_tcam_fg_id;
    attach_info.key_info.qual_info[0].offset = offset_of_valid_bit_of_ipmf1_tcam_action;

    if (is_trap)
    {
        /** Create 32b const user qualifier to update the value of Trap action. */
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 32;
        dest_char = &(qual_info.name[0]);
        sal_strncpy_s(dest_char, "Polarity_trap_cons", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_cascading_info_used_for_polarity_de_const_trap_q_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_create\n", rv);
            return rv;
        }

        /**
         * Encode the Trap Value as follow --->
         * ((trap_code (9b)) | (trap_str (4b) << 9 (trap_code_size)) | (trap_qual << 13 (trap_str_size + trap_code_size)))
         */
        Cint_field_cascading_info_used_for_polarity_de_const_trap_q_val =
            ((Cint_field_cascading_info_used_for_polarity_trap_code) | (Cint_field_cascading_info_used_for_polarity_trap_str << 9) |
                (Cint_field_cascading_info_used_for_polarity_trap_qual << 13));

        fg_info.qual_types[1] = Cint_field_cascading_info_used_for_polarity_de_const_trap_q_id;
        attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
        attach_info.key_info.qual_info[1].input_arg = Cint_field_cascading_info_used_for_polarity_de_const_trap_q_val;
        attach_info.key_info.qual_info[1].offset = 0;

        fg_info.action_types[0] = bcmFieldActionTrap;
        attach_info.payload_info.action_info[0].valid_bit_polarity = 1;
    }
    else
    {
        /** Create 20b const user qualifier to update the value of Snoop action. */
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 20;
        dest_char = &(qual_info.name[0]);
        sal_strncpy_s(dest_char, "Polarity_snp_cons", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_cascading_info_used_for_polarity_de_const_snoop_q_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_qualifier_create\n", rv);
            return rv;
        }

        /**
         * Encode the Snoop Value as follow --->
         * ((snoop_code (9b)) | (snoop_str (4b) << 9 (snoop_code_size)) | (snoop_qual << 12 (snoop_str_size (4b) + snoop_code_size (9b) - sniff_indication_size (1b))))
         */
        Cint_field_cascading_info_used_for_polarity_de_const_snp_q_val =
            ((Cint_field_cascading_info_used_for_polarity_snp_code) | (Cint_field_cascading_info_used_for_polarity_trap_str << 9) |
                (Cint_field_cascading_info_used_for_polarity_trap_qual << 12));

        fg_info.qual_types[1] = Cint_field_cascading_info_used_for_polarity_de_const_snoop_q_id;
        attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
        attach_info.key_info.qual_info[1].input_arg = Cint_field_cascading_info_used_for_polarity_de_const_snp_q_val;
        attach_info.key_info.qual_info[1].offset = 0;

        fg_info.action_types[0] = bcmFieldActionSnoop;
        attach_info.payload_info.action_info[0].valid_bit_polarity = 0;
    }
    fg_info.action_with_valid_bit[0] = TRUE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];


    rv = bcm_field_group_add(unit, 0, &fg_info, fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, *fg_id);
        return rv;
    }
    printf("Field Group ID %d was created. \n", *fg_id);

    rv = bcm_field_group_context_attach(unit, 0, *fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("Field Group ID %d was attached to Context ID %d. \n", *fg_id, context_id);

    return rv;
}

/**
 * \brief
 *  Creates FWD Context in IPMF1 stage. Creates a TCAM FG in IPMF1 with
 *  1b void action to be used as condition bit in IPMF2. Creates 2 DE FGs
 *  in IPMF2 in case cascading result is 1, then Trap action will be performed (DE FG 1).
 *  Otherwise if cascading result is 0, then Snoop action will be performed (DE FG 2).
 *
 * \param [in] unit   - Device id
 * \param [in] in_port  - In_port to qualify upon.
 * \param [in] dst_mac_lsb  - 32 lsb bits of the DST_MAC address to qualify upon.
 * \param [in] context_id  - Context id.
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_cascading_info_used_for_polarity_main(
    int unit,
    bcm_port_t in_port,
    int dst_mac_lsb,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    rv = cint_field_cascading_info_used_for_polarity_tcam_fg_create(unit, in_port, dst_mac_lsb, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_cascading_info_used_for_polarity_tcam_fg_create\n", rv);
        return rv;
    }

    rv = cint_field_cascading_info_used_for_polarity_val_bit_qual_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_cascading_info_used_for_polarity_val_bit_qual_create\n", rv);
        return rv;
    }

    rv = cint_field_cascading_info_used_for_polarity_de_fg_create(unit, TRUE, context_id, &Cint_field_cascading_info_used_for_polarity_de_trap_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_cascading_info_used_for_polarity_de_fg_create\n", rv);
        return rv;
    }

    rv = cint_field_cascading_info_used_for_polarity_de_fg_create(unit, FALSE, context_id,  &Cint_field_cascading_info_used_for_polarity_de_snoop_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_cascading_info_used_for_polarity_de_fg_create\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit         - Device id
 * \param [in] context_id  - Context id.
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_cascading_info_used_for_polarity_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, Cint_field_cascading_info_used_for_polarity_tcam_fg_id, NULL,
                                Cint_field_cascading_info_used_for_polarity_tcam_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_entry_delete\n");
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, Cint_field_cascading_info_used_for_polarity_de_trap_fg_id,
                                        context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, Cint_field_cascading_info_used_for_polarity_de_snoop_fg_id,
                                        context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, Cint_field_cascading_info_used_for_polarity_tcam_fg_id,
                                        context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, Cint_field_cascading_info_used_for_polarity_tcam_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    rv = bcm_field_group_delete(unit, Cint_field_cascading_info_used_for_polarity_de_trap_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    rv = bcm_field_group_delete(unit, Cint_field_cascading_info_used_for_polarity_de_snoop_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, Cint_field_cascading_info_used_for_polarity_de_val_bit_qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy. \n");
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, Cint_field_cascading_info_used_for_polarity_de_const_trap_q_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy. \n");
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, Cint_field_cascading_info_used_for_polarity_de_const_snoop_q_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_destroy. \n");
        return rv;
    }

    rv = bcm_field_action_destroy(unit, Cint_field_cascading_info_used_for_polarity_tcam_void_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_action_destroy. \n");
        return rv;
    }

    return rv;
}

/**
 *  Sets different trap per core relating on polarity value.
 *
 *  Configuration example start:

   cint ../../../src/examples/dnx/field/cint_field_direct_ext_different_trap_per_core.c
   cint ../../../../src/examples/dnx/field/cint_field_direct_ext_different_trap_per_core.c
   cint;
   int unit = 0;
   bcm_field_context_t context_id = 0;
   cint_field_direct_ext_different_trap_per_core_main(unit, context_id);
 *
 *  Configuration example end
 *
 *  This cint shows how upon polarity bit different trap can be hit per core.
 *  In this case polarity bit depends on lsb bit of InPort, which is indicating the core ID,
 *  if core is '0' the polarity will be set to '0' and trap with DROP action will be performed,
 *  otherwise if core is '1' trap with FWD to CPU action will be performed.
 *
 *  CINT configuration:
 *      1. create 2 traps bcmRxTrapUserDefine
 *          - TRAP_ID1 and TRAP_ID2
 *      2. Configure each trap with different destination
 *          - TRAP_ID1 to drop
 *          - TRAP_ID2 to CPU
 *      3. Create 2 trap actions and use prefix to set the value
 *          - action_1  to have TRAP_ID1
 *          - action_2 to have TRAP_ID2
 *      4. Creating 2 DE FGs in IPMF2 stage and attaching them to a the given context
 *          - both FGs will have use_valid_bit=TRUE
 *      5. The condition bit for 2 bits is the lsb of qualifier bcmQualifyInPort (which is the core id)
 *          - 1st FG has polarity 0 with Action_1
 *          - 2nd FG has polarity 1 with action_2
 *
 */

int Cint_field_direct_ext_different_trap_per_core_trap_str = 0xf;
int Cint_field_direct_ext_different_trap_per_core_trap_qual = 0xf;

/** Global variables to be passed through function. */
bcm_field_group_t Cint_field_direct_ext_different_trap_per_core_fg_id_1;
bcm_field_group_t Cint_field_direct_ext_different_trap_per_core_fg_id_2;
bcm_field_action_t Cint_field_direct_ext_different_trap_per_core_trap_drop_act_id;
bcm_field_action_t Cint_field_direct_ext_different_trap_per_core_trap_cpu_act_id;
bcm_field_qualify_t Cint_field_direct_ext_different_trap_per_core_val_bit_qual_id;
bcm_field_qualify_t Cint_field_direct_ext_different_trap_per_core_key_gen_qual_id;

int Cint_field_direct_ext_different_trap_per_core_trap_id_cpu;
int Cint_field_direct_ext_different_trap_per_core_trap_id_drop;

/**
 * \brief
 *  Creates user qualifiers needed for DE FGs.
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
cint_field_direct_ext_different_trap_per_core_qual_create(
    int unit)
{
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;

    /** Create 1b user qualifier to qualify upon lsb bit of bcmQualifyInPort, which indicates core_id. */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "trap_per_core_val_bit", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_direct_ext_different_trap_per_core_val_bit_qual_id));

    /** Create 9b user qualifier to qualify upon 9 bits trap code stored in bcmFieldQualifyKeyGenVar. */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 9;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "trap_per_core_key_gen", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_create(unit, 0, &qual_info, &Cint_field_direct_ext_different_trap_per_core_key_gen_qual_id));

    return BCM_E_NONE;
}

/**
 * \brief
 *  Creates 2 traps one with drop action and one with FWD to CPU.
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
cint_field_direct_ext_different_trap_per_core_trap_config(
    int unit)
{
    bcm_rx_trap_config_t trap_config;

    /** Create trap with destination BLACK_HOLE to drop the packet. */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;
    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &Cint_field_direct_ext_different_trap_per_core_trap_id_drop));
    BCM_IF_ERROR_RETURN(bcm_rx_trap_set(unit, Cint_field_direct_ext_different_trap_per_core_trap_id_drop, &trap_config));

    printf("Trap ID %d was created. \n", Cint_field_direct_ext_different_trap_per_core_trap_id_drop);

    /** Create trap with destination CPU to forward the packet to CPU. */
    trap_config.dest_port = BCM_GPORT_LOCAL_CPU;
    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &Cint_field_direct_ext_different_trap_per_core_trap_id_cpu));
    BCM_IF_ERROR_RETURN(bcm_rx_trap_set(unit, Cint_field_direct_ext_different_trap_per_core_trap_id_cpu, &trap_config));

    printf("Trap ID %d was created. \n", Cint_field_direct_ext_different_trap_per_core_trap_id_cpu);

    return BCM_E_NONE;
}

/**
 * \brief
 *  This function creates a DE field group,
 *  attaching it to a context. The created FG will do action
 *  Trap which will update destination to drop.
 *
 * \param [in] unit   - Device id
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
cint_field_direct_ext_different_trap_per_core_fg_trap_drop_create(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_get_t qual_info_get;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Trap_drop", sizeof(fg_info.name));
    fg_info.nof_quals = 2;
    fg_info.nof_actions = 1;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    fg_info.qual_types[0] = Cint_field_direct_ext_different_trap_per_core_val_bit_qual_id;
    fg_info.qual_types[1] = Cint_field_direct_ext_different_trap_per_core_key_gen_qual_id;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    /** Get the offset of IN_PORT qualifier. */
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_info_get(unit, bcmFieldQualifyInPort, bcmFieldStageIngressPMF2, &qual_info_get));

    attach_info.key_info.qual_info[0].offset = qual_info_get.offset;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    /** Get the offset of KEY_GEN_VAR qualifier. */
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_info_get(unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF2, &qual_info_get));
    /** The first trap id is located in the bits 0:8 from KEY_GEN_VAR. */
    attach_info.key_info.qual_info[1].offset = qual_info_get.offset;

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionTrap;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "Trap_drop", sizeof(action_info.name));
    action_info.size = 9;
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.prefix_size = 23;
    /** Encode the prefix value as follow ---> ((trap_str (4b) | (trap_qual << 4 (trap_str_size))). */
    action_info.prefix_value = ((Cint_field_direct_ext_different_trap_per_core_trap_str) |
                                (Cint_field_direct_ext_different_trap_per_core_trap_qual << 4));
    BCM_IF_ERROR_RETURN(bcm_field_action_create(unit, 0, &action_info, &Cint_field_direct_ext_different_trap_per_core_trap_drop_act_id));

    fg_info.action_types[0] = Cint_field_direct_ext_different_trap_per_core_trap_drop_act_id;
    fg_info.action_with_valid_bit[0] = TRUE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0,9);
    attach_info.payload_info.action_info[0].valid_bit_polarity = 0;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &Cint_field_direct_ext_different_trap_per_core_fg_id_1));
    printf("Field Group ID %d was created. \n", Cint_field_direct_ext_different_trap_per_core_fg_id_1);

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, Cint_field_direct_ext_different_trap_per_core_fg_id_1, context_id, &attach_info));
    printf("Field Group ID %d was attached to Context ID %d. \n",
           Cint_field_direct_ext_different_trap_per_core_fg_id_1, context_id);

    return BCM_E_NONE;
}

/**
 * \brief
 *  This function creates a TCAM field group,
 *  attaching it to a context and adding an entry.
 *
 * \param [in] unit   - Device id
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
cint_field_direct_ext_different_trap_per_core_fg_trap_cpu_create(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_get_t qual_info_get;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Trap_cpu", sizeof(fg_info.name));
    fg_info.nof_quals = 2;
    fg_info.nof_actions = 1;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    fg_info.qual_types[0] = Cint_field_direct_ext_different_trap_per_core_val_bit_qual_id;
    fg_info.qual_types[1] = Cint_field_direct_ext_different_trap_per_core_key_gen_qual_id;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    /** Get the offset of IN_PORT qualifier. */
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_info_get(unit, bcmFieldQualifyInPort, bcmFieldStageIngressPMF1, &qual_info_get));

    attach_info.key_info.qual_info[0].offset = qual_info_get.offset;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    /** Get the offset of KEY_GEN_VAR qualifier. */
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_info_get(unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF1, &qual_info_get));
    /** The second trap id is located in the bits 9:17 from KEY_GEN_VAR. */
    attach_info.key_info.qual_info[1].offset = (qual_info_get.offset + 9);

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionTrap;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "Trap_cpu", sizeof(action_info.name));
    action_info.size = 9;
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.prefix_size = 23;
    /** Encode the prefix value as follow ---> ((trap_str (4b) | (trap_qual << 4 (trap_str_size))). */
    action_info.prefix_value = ((Cint_field_direct_ext_different_trap_per_core_trap_str) |
                                (Cint_field_direct_ext_different_trap_per_core_trap_qual << 4));
    BCM_IF_ERROR_RETURN(bcm_field_action_create(unit, 0, &action_info, &Cint_field_direct_ext_different_trap_per_core_trap_cpu_act_id));

    fg_info.action_types[0] = Cint_field_direct_ext_different_trap_per_core_trap_cpu_act_id;
    fg_info.action_with_valid_bit[0] = TRUE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0,10);
    attach_info.payload_info.action_info[0].valid_bit_polarity = 1;

    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &Cint_field_direct_ext_different_trap_per_core_fg_id_2));
    printf("Field Group ID %d was created. \n", Cint_field_direct_ext_different_trap_per_core_fg_id_2);

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, Cint_field_direct_ext_different_trap_per_core_fg_id_2,context_id, &attach_info));
    printf("Field Group ID %d was attached to Context ID %d. \n",
           Cint_field_direct_ext_different_trap_per_core_fg_id_2, context_id);

    return BCM_E_NONE;
}

/**
 * \brief
 *  Creates FWD Context in IPMF1 stage. Creates a TCAM FG in IPMF1 with
 *  1b void action to be used as condition bit in IPMF2. Creates 2 DE FGs
 *  in IPMF2 in case cascading result is 1, then Trap action will be performed (DE FG 1).
 *  Otherwise if cascading result is 0, then Snoop action will be performed (DE FG 2).
 *
 * \param [in] unit   - Device id
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
cint_field_direct_ext_different_trap_per_core_main(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_context_param_info_t param_info;

    BCM_IF_ERROR_RETURN(cint_field_direct_ext_different_trap_per_core_trap_config(unit));

    /** Fill the info for context_param_set. */
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    /** Encode the KEY_GEN_VAR value as follow ---> ((trap_id_drop (9b) | (trap_id_cpu << 9 (trap_id_size))). */
    param_info.param_val = ((Cint_field_direct_ext_different_trap_per_core_trap_id_drop) |
                            (Cint_field_direct_ext_different_trap_per_core_trap_id_cpu << 9));
    param_info.param_arg = 0;
    /** Set the context param. */
    BCM_IF_ERROR_RETURN(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &param_info));

    BCM_IF_ERROR_RETURN(cint_field_direct_ext_different_trap_per_core_qual_create(unit));

    BCM_IF_ERROR_RETURN(cint_field_direct_ext_different_trap_per_core_fg_trap_drop_create(unit, context_id));

    BCM_IF_ERROR_RETURN(cint_field_direct_ext_different_trap_per_core_fg_trap_cpu_create(unit, context_id));

    return BCM_E_NONE;
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
cint_field_direct_ext_different_trap_per_core_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, Cint_field_direct_ext_different_trap_per_core_fg_id_1,context_id));
    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, Cint_field_direct_ext_different_trap_per_core_fg_id_2,context_id));

    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, Cint_field_direct_ext_different_trap_per_core_fg_id_1));
    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, Cint_field_direct_ext_different_trap_per_core_fg_id_2));

    BCM_IF_ERROR_RETURN(bcm_field_qualifier_destroy(unit, Cint_field_direct_ext_different_trap_per_core_val_bit_qual_id));
    BCM_IF_ERROR_RETURN(bcm_field_qualifier_destroy(unit, Cint_field_direct_ext_different_trap_per_core_key_gen_qual_id));

    BCM_IF_ERROR_RETURN(bcm_field_action_destroy(unit, Cint_field_direct_ext_different_trap_per_core_trap_drop_act_id));

    BCM_IF_ERROR_RETURN(bcm_field_action_destroy(unit, Cint_field_direct_ext_different_trap_per_core_trap_cpu_act_id));

    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_destroy(unit, Cint_field_direct_ext_different_trap_per_core_trap_id_drop));

    BCM_IF_ERROR_RETURN(bcm_rx_trap_type_destroy(unit, Cint_field_direct_ext_different_trap_per_core_trap_id_cpu));

    return BCM_E_NONE;
}

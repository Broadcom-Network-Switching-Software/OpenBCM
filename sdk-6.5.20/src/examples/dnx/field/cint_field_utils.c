/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_utils.c
 *
 *
 * Utility functions about field processor configurations.
 */

/* *INDENT-OFF* */

int FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS      = 3;
/** Maximal qualifiers(or actions) per field group */
int FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG = 16;
/** Maximal number of TCAM banks */
int FIELD_UTIL_MAX_TCAM_BANKS = 16;

/** Valid BCM qualifiers for "field_util_basic_tcam_fg_t_init" */
static bcm_field_qualify_t bcm_qualifiers_for_init[FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] =
    { bcmFieldQualifyColor, bcmFieldQualifyCount, bcmFieldQualifyCount };

/** Valid BCM qualifier values for "field_util_basic_tcam_fg_t_init" */
static uint32 bcm_qualifier_values_for_init[FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] = { 0x1, 0x1, 0x1 };

/** Valid BCM actions for "field_util_basic_tcam_fg_t_init" */
static bcm_field_action_t bcm_actions_for_init[FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] =
    { bcmFieldActionDropPrecedence, bcmFieldActionCount, bcmFieldActionCount };

/** Valid BCM action values for "field_util_basic_tcam_fg_t_init" */
static uint32 bcm_action_values_for_init[FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] = { 0x1, 0x1, 0x1 };

/**
 * Global variables for FG creation and attaching
 * They will be used by function 'field_fg_create_and_attach'
 */
int nof_qual_for_fg_creation; /* Indicate how many qualifiers in qual_list[] are used */
int nof_act_for_fg_creation; /* Indicate how many actions in act_list[] are used*/
bcm_field_qualify_t qual_list[32]; /* Qaulifier list for FG creation */
bcm_field_qualify_attach_info_t attach_info[32]; /* Qualifier info for FG attaching */
bcm_field_action_t act_list[32]; /* Action list for FG creation */
bcm_field_action_attach_info_t act_attach_info[32]; /* Action attach info for FG creation */
int act_valid_bits[32]= {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; /* Action valid bit list  */
int bank_alloc_mode = bcmFieldTcamBankAllocationModeAuto; /* TCAM bank alloc-mode */
int nof_tcam_banks = 0; /* Number of requested banks */
int tcam_bank_ids[16]; /* Array to store requested bank IDs */

/**
 * Global variables for field entry creation
 * They will be used by function 'field_entry_add'
 */
int nof_qual_for_ent_creation; /* Indicate how many qualifiers in ent_qual_list[] are used */
int nof_act_for_ent_creation; /* Indicate how many actions in ent_act_list[] are used*/
bcm_core_t ent_core_id = BCM_CORE_ALL; /* Core to install entry on */
bcm_field_qualify_t ent_qual_list[32]; /* Qaulifier list for field entry creation */
bcm_field_action_t ent_act_list[32]; /* Action list for field entry creation */
uint32 ent_qual_datas[32]; /* Qualifier datas */
uint32 ent_qual_masks[32]; /* Qualifier data masks */
uint32 ent_act_vals[32]; /* Action values */

/* *INDENT-ON* */

/*
 * The values will be filled for qualifiers.
 * Larger than 32b, will be written to the next elements in the array.
 * Single value array is equal to a single qualifier or action value.
 */
struct field_util_qual_values_t
{
    uint32 value[BCM_FIELD_QUAL_WIDTH_IN_WORDS];
};

/*
 * The value will be filled for actions.
 * Larger than 32b, will be written to the next elements in the array.
 * Single value array is equal to a single qualifier or action value.
 */
struct field_util_action_values_t
{
    uint32 value[BCM_FIELD_ACTION_WIDTH_IN_WORDS];
};

struct field_util_basic_tcam_fg_t
{
    /**
     * Field Group ID is input and output parameter.
     * If the field is left empty, it will be initialized to -1 at init and used as output
     * for by 'bcm_field_group_create()'.
     */
    int fg_id;

    /**
     * Context for testing - If the field is not explicitly set
     *  by the user, BCM_FIELD_CONTEXT_ID_DEFAULT will be set on init.
     */
    bcm_field_context_t context;

    /**
     * Returned parameter of bcm_field_entry_add().
     * Used when we want to destroy an entry.
     */
    bcm_field_entry_t entry_handle;

    /**
     * PMF stage identifier
     */
    bcm_field_stage_t stage;

    /**
     * BCM Qualifiers for the Field Group(Key)
     */
    bcm_field_qualify_t bcm_qual[FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

    /**
   * Indicate how to read the relevant qualifier from PBUS
   * The information is aligned to the qualifier types array above
   * Number of elements set by qual_types array
   */
    bcm_field_qualify_attach_info_t qual_info[FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

    /**
     * BCM Qualifier Values, every qual_values member contains an array of 4 Value fields(accommodating large values)
     */
    field_util_qual_values_t qual_values[FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

    /**
     *BCM  Actions for the Field Group .
     */
    bcm_field_action_t bcm_actions[FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

    /**
     * BCM Action Values
     */
    field_util_action_values_t action_values[FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS];

};

struct field_util_fg_t
{
    /**
     * Field Group ID is input and output parameter.
     * If the field is left empty, it will be initialized to -1 at init and used as output
     * for by 'bcm_field_group_create()'.
     */
    bcm_field_group_t fg_id;

    /**
     * PMF stage identifier
     */
    bcm_field_stage_t stage;

    /**
     * Field group type
     */
    bcm_field_group_type_t fg_type;

    /**
     * Context for testing - If the field is not explicitly set
     *  by the user, BCM_FIELD_CONTEXT_ID_DEFAULT will be set on init.
     */
    bcm_field_context_t context;

    /**
     * BCM Qualifiers for the Field Group(Key)
     */
    bcm_field_qualify_t bcm_qual[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /**
     * Indicate how to read the relevant qualifier from PBUS
     * The information is aligned to the qualifier types array above
     * Number of elements set by qual_types array
     */
    bcm_field_qualify_attach_info_t qual_info[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /**
     *BCM Actions for the Field Group
     */
    bcm_field_action_t bcm_actions[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /**
     *Valid bit for each action
     *If set, then additional bit is added in payload to indicate weather this action is valid or not
     */
    int act_valid_bit[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /**
     *The priority for each action
     */
    int act_priorities[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /**
     *Specified TCAM bank IDs for the field group.
     *Only valid for TCAM group
     */
    bcm_field_group_tcam_info_t tcam_info;

};

struct field_util_entry_info_t
{
    /** Field group id, to which entry is added */
    bcm_field_group_t fg_id;

    /**
     * BCM qualifiers for the FP entry
     */
    bcm_field_qualify_t bcm_qual[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /**
     * BCM Qualifier Values, every qual_values member contains an array of 4 Value fields(accommodating large values)
     */
    field_util_qual_values_t qual_values[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /**
     * BCM Qualifier Masks, every qual_masks member contains an array of 4 Value fields(accommodating large values)
     */
    field_util_qual_values_t qual_masks[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /**
     *BCM  Actions for FP entry
     */
    bcm_field_action_t bcm_actions[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /**
     * BCM Action values
     */
    field_util_action_values_t action_values[FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG];

    /** Entry priority */
    uint32 ent_pri;

    /**
     * Returned parameter of bcm_field_entry_add().
     * Used when we want to destroy an entry.
     */
    bcm_field_entry_t entry_handle;
    /* Core to install entry on */
    bcm_core_t core;

};

/** 
 * Structure of field group attributes
 * It can be translated to 'field_util_fg_t' for field group creation and attaching
 * Structure 'field_util_fg_t' can be used to create and attach FG directly. But it must be initialized firstly
 * So 'field_util_fg_t' can't be created and assigned values in one step
 * Now it can be achieved by this structure
 */
struct field_group_and_attach_info_t {
    int nof_quals;
    int nof_actions;
    field_util_fg_t fg_util_info;
};

/** 
 * Structure of field entries attributes
 * It can be translated to 'field_util_entry_info_t' for field entry creation
 * Structure 'field_util_entry_info_t' can be used to create field entry directly. But it must be initialized firstly
 * So 'field_util_entry_info_t' can't be created and assigned values in one step
 * Now it can be achieved by this structure
 */
struct field_entry_info_t {
    int nof_quals;
    int nof_actions;
    field_util_entry_info_t ent_utlil_info;
};

/**
 * \brief
 *  Initializes the ctest_bcm_field_util_basic_tcam_fg_t structure to default valid values.
 *  The structure is initialized and ready for a default valid configuration.
 *  The qualifiers and actions are loaded from arrays at the top of the file.
 *
 * \param [in]  unit                    - Device ID
 * \param [in/out]  tcam_fg_util_p          - Structure holding all relevant info for creating a full configuration,
 *                                        ready for traffic testing of the set qualifiers and actions supplied.
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
void
field_util_basic_tcam_fg_t_init(
    int unit,
    field_util_basic_tcam_fg_t * tcam_fg_util_p)
{
    int ii;

    tcam_fg_util_p->fg_id = -1;
    tcam_fg_util_p->context = BCM_FIELD_CONTEXT_ID_DEFAULT;
    tcam_fg_util_p->stage = bcmFieldStageIngressPMF1;

    for (ii = 0; ii < FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS; ii++)
    {
        /*
         *  We are initializing the array to be filled with DP qualifier and value 0x1
         *  the rest of the qualifiers are bcmFieldQualifyCount
         */
        tcam_fg_util_p->bcm_qual[ii] = bcm_qualifiers_for_init[ii];
        tcam_fg_util_p->qual_values[ii].value[0] = bcm_qualifier_values_for_init[ii];

        /*
         * We are initializing the array to be filled with action DP , value 0x1,
         *  the rest of the actions are bcmFieldActionCount
         */
        tcam_fg_util_p->bcm_actions[ii] = bcm_actions_for_init[ii];
        tcam_fg_util_p->action_values[ii].value[0] = bcm_action_values_for_init[ii];

        /*
         * Initializing the qualifier info to valid values.
         */
        tcam_fg_util_p->qual_info[ii].input_type = BCM_FIELD_INVALID;
        tcam_fg_util_p->qual_info[ii].input_arg = BCM_FIELD_INVALID;
        tcam_fg_util_p->qual_info[ii].offset = BCM_FIELD_INVALID;
    }
}

/**
 * \brief
 *  Initializes the field_util_fg_t structure to default valid values.
 *  The structure is initialized and ready for a default valid configuration.
 *
 * \param [in]  unit                    - Device ID
 * \param [in/out]  fg_util_p           - Structure holding all relevant info for field group creation/attaching
 *
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void
field_util_fg_t_init(
    int unit,
    field_util_fg_t * fg_util_p)
{
    int ii;

    fg_util_p->fg_id = -1;
    fg_util_p->context = BCM_FIELD_CONTEXT_ID_DEFAULT;
    fg_util_p->stage = bcmFieldStageIngressPMF1;
    fg_util_p->fg_type = bcmFieldGroupTypeTcam;

    /** Initializing TCAM info */
    fg_util_p->tcam_info.nof_tcam_banks = 0;
    sal_memset(fg_util_p->tcam_info.tcam_bank_ids, 0, sizeof(fg_util_p->tcam_info.tcam_bank_ids));
    fg_util_p->tcam_info.bank_allocation_mode = 0;

    for (ii = 0; ii < FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG; ii++)
    {
        /** Initializing all qualifiers to bcmFieldQualifyCount */
        fg_util_p->bcm_qual[ii] = bcmFieldQualifyCount;

        /** Initializing the qualifier info to valid values */
        fg_util_p->qual_info[ii].input_type = BCM_FIELD_INVALID;
        fg_util_p->qual_info[ii].input_arg = BCM_FIELD_INVALID;
        fg_util_p->qual_info[ii].offset = BCM_FIELD_INVALID;

        /** Initializing all actions to bcmFieldActionCount */
        fg_util_p->bcm_actions[ii] = bcmFieldActionCount;

        /** Initializing action priority as DON'T CARE */
        fg_util_p->act_priorities[ii] = BCM_FIELD_ACTION_DONT_CARE;
        
        /** Initializing action valid bit setting */
        fg_util_p->act_valid_bit[ii] = 1;
    }
}

/**
 * \brief
 *  Initializes the field_util_entry_info_t structure to default valid values.
 *  The structure is initialized and ready for a default valid configuration.
 *
 * \param [in]  unit                    - Device ID
 * \param [in/out]  ent_info_p          - Structure holding all relevant info for field entry creation
 *
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void
field_util_entry_info_init(
    int unit,
    field_util_entry_info_t * ent_info_p)
{
    int ii;
    field_util_qual_values_t qual_values_masks_init; /* Used to initialize qualfier datas&masks */
    field_util_action_values_t action_values_init; /* Used to initialize action values */

    ent_info_p->fg_id = -1;
    ent_info_p->ent_pri = 0;

    for (ii = 0; ii < FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG; ii++)
    {
        /** Initializing all entry qualifiers to bcmFieldQualifyCount */
        ent_info_p->bcm_qual[ii] = bcmFieldQualifyCount;

        /** Initializing qualifier values and masks */
        ent_info_p->qual_values[ii] = qual_values_masks_init;
        ent_info_p->qual_masks[ii] = qual_values_masks_init;

        /** Initializing all actions to bcmFieldActionCount */
        ent_info_p->bcm_actions[ii] = bcmFieldActionCount;

        /** Initializing action value */
        ent_info_p->action_values[ii] = action_values_init;
    }
    ent_info_p->core = BCM_CORE_ALL;
}

/* Convert structure 'field_group_and_attach_info_t' to structure 'field_util_fg_t' */
void
field_util_fg_t_convert(
    int unit,
    field_group_and_attach_info_t fg_attach_info,
    field_util_fg_t * fg_utlil_p)
{
    int i;

    /* Init structure */
    field_util_fg_t_init(unit, fg_utlil_p);

    /* Translate FG_ID/stage/FG_Type/Context */
    fg_utlil_p->fg_id = fg_attach_info.fg_util_info.fg_id;
    fg_utlil_p->stage = fg_attach_info.fg_util_info.stage;
    fg_utlil_p->fg_type = fg_attach_info.fg_util_info.fg_type;
    fg_utlil_p->context = fg_attach_info.fg_util_info.context;

    /* Translate qualifiers */
    for (i = 0; i < fg_attach_info.nof_quals; i++)
    {
        fg_utlil_p->bcm_qual[i] = fg_attach_info.fg_util_info.bcm_qual[i];
        fg_utlil_p->qual_info[i] = fg_attach_info.fg_util_info.qual_info[i];
    }

    /* Translate actions */
    for (i = 0; i < fg_attach_info.nof_actions; i++)
    {
        fg_utlil_p->bcm_actions[i] = fg_attach_info.fg_util_info.bcm_actions[i];
        if (fg_attach_info.fg_util_info.act_priorities[i] != 0) {
            fg_utlil_p->act_priorities[i] = fg_attach_info.fg_util_info.act_priorities[i];
        }
        fg_utlil_p->act_valid_bit[i] = fg_attach_info.fg_util_info.act_valid_bit[i];
    }

    /* Init TCAM info */
    fg_utlil_p->tcam_info = fg_attach_info.fg_util_info.tcam_info;
}

/* Convert structure 'field_entry_info_t' to structure 'field_util_entry_info_t' */
void
field_util_entry_t_convert(
    int unit,
    field_entry_info_t fg_entry_info,
    field_util_entry_info_t * entry_utlil_p)
{
    int i;

    /* Init structure and set FG ID*/
    field_util_entry_info_init(unit, entry_utlil_p);
    entry_utlil_p->fg_id = fg_entry_info.ent_utlil_info.fg_id;

    /* Translate entry qualifiers */
    for (i = 0; i < fg_entry_info.nof_quals; i++)
    {
        entry_utlil_p->bcm_qual[i] = fg_entry_info.ent_utlil_info.bcm_qual[i];
        entry_utlil_p->qual_values[i] = fg_entry_info.ent_utlil_info.qual_values[i];
        entry_utlil_p->qual_masks[i] = fg_entry_info.ent_utlil_info.qual_masks[i];
    }

    /* Translate entry actions */
    for (i = 0; i < fg_entry_info.nof_actions; i++)
    {
        entry_utlil_p->bcm_actions[i] = fg_entry_info.ent_utlil_info.bcm_actions[i];
        entry_utlil_p->action_values[i] = fg_entry_info.ent_utlil_info.action_values[i];
    }

    /* Translate entry priority and entry ID */
    entry_utlil_p->ent_pri = fg_entry_info.ent_utlil_info.ent_pri;
    entry_utlil_p->entry_handle = fg_entry_info.ent_utlil_info.entry_handle;
}

/**
 * \brief
 *  This function creates a TCAM field group to the relevant stage using the provided
 *  qualifiers and actions in the "field_util_basic_tcam_fg_t" structure.
 *  The provided BCM quals and actions are translated into DNX encoded and then set.
 *  It returns the Field Group ID and FG info.
 *
 * \param [in]  unit                      - Device ID
 * \param [in/out]  tcam_fg_util_p        - Input information from user(stage, bcm_quals, bcm_actions).
 *                                          Output is 'fg_id' if not explicitly set.
 * \param [out] fg_info_p                 - Field group info structure with set parameters
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int
field_util_basic_tcam_fg_add(
    int unit,
    field_util_basic_tcam_fg_t * tcam_fg_util_p,
    bcm_field_group_info_t * fg_info_p)
{
    int qual_action_iter;
    int rv = BCM_E_NONE;
    uint32 flags;

    bcm_field_group_info_t_init(fg_info_p);

    fg_info_p->stage = tcam_fg_util_p->stage;
    fg_info_p->fg_type = bcmFieldGroupTypeTcam;
    /** Loop over the supplied arrays of BCM qualifiers and actions, which we limit to 3 */
    for (qual_action_iter = 0; qual_action_iter < FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS; qual_action_iter++)
    {
        /** We treat bcmFieldQualifyCount as an Invalid value */
        if (tcam_fg_util_p->bcm_qual[qual_action_iter] != bcmFieldQualifyCount)
        {
            fg_info_p->qual_types[qual_action_iter] = tcam_fg_util_p->bcm_qual[qual_action_iter];
            fg_info_p->nof_quals++;
        }

        /** We treat bcmFieldActionCount as an Invalid value */
        if (tcam_fg_util_p->bcm_actions[qual_action_iter] != bcmFieldActionCount)
        {
            fg_info_p->action_types[qual_action_iter] = tcam_fg_util_p->bcm_actions[qual_action_iter];
            fg_info_p->nof_actions++;
        }
    }

    if (tcam_fg_util_p->fg_id == -1)
    {
        /** Initialize the FG ID to 0 to avoid error in the group_add() */
        tcam_fg_util_p->fg_id = 0;
        flags = BCM_FIELD_FLAG_MSB_RESULT_ALIGN;
    }
    else
    {
        flags = BCM_FIELD_FLAG_WITH_ID | BCM_FIELD_FLAG_MSB_RESULT_ALIGN;
    }

    rv = bcm_field_group_add(unit, flags, fg_info_p, &tcam_fg_util_p->fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function initializes the attach information for the context that is provided in
 *  "tcam_fg_util_p". The attach info structure(attach_info_p) is filled with all the relevant
 *  information.
 *  The context provided is created if it is not DNX_FIELD_CONTEXT_ID_DEFAULT(unit), before the field group
 *  passed is attached to it, with the filled-in attach info.
 *
 * \param [in] unit                      - Device ID
 * \param [in] fg_info_p                 - Field group info structure with set parameters.
 * \param [in/out] tcam_fg_util_p            - Input information from user(stage and context)
 * \param [out] attach_info_p            - Attach info to be filled and attached.
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int
field_util_basic_context_attach(
    int unit,
    field_util_basic_tcam_fg_t * tcam_fg_util_p,
    bcm_field_group_info_t * fg_info_p,
    bcm_field_group_attach_info_t * attach_info_p)
{
    int rv = BCM_E_NONE;
    int action_iter, qual_iter;
    bcm_field_qualify_t bcm_qual;
    bcm_field_context_info_t context_info;
    bcm_field_stage_t stage_for_context_create;

    bcm_field_group_attach_info_t_init(attach_info_p);
    /*
     * Setting the attach information of the context.
     */
    sal_memcpy(attach_info_p->key_info.qual_types, fg_info_p->qual_types, sizeof(attach_info_p->key_info.qual_types));
    sal_memcpy(attach_info_p->payload_info.action_types, fg_info_p->action_types,
               sizeof(attach_info_p->payload_info.action_types));
    attach_info_p->payload_info.nof_actions = fg_info_p->nof_actions;
    attach_info_p->key_info.nof_quals = fg_info_p->nof_quals;

    for (qual_iter = 0; qual_iter < attach_info_p->key_info.nof_quals; qual_iter++)
    {
        attach_info_p->key_info.qual_info[qual_iter].input_type = tcam_fg_util_p->qual_info[qual_iter].input_type;
        attach_info_p->key_info.qual_info[qual_iter].input_arg = tcam_fg_util_p->qual_info[qual_iter].input_arg;
        attach_info_p->key_info.qual_info[qual_iter].offset = tcam_fg_util_p->qual_info[qual_iter].offset;
    }
    /** Default context is automatically created on init, for every stage */
    if (tcam_fg_util_p->context != BCM_FIELD_CONTEXT_ID_DEFAULT)
    {
        /** We check if the context_id is already allocated(created), if yes, we don't want to create again */
        rv = bcm_field_context_info_get(unit, fg_info_p->stage, tcam_fg_util_p->context, &context_info);
        if (rv == BCM_E_NOT_FOUND)
        {
            bcm_field_context_info_t_init(&context_info);
            if (tcam_fg_util_p->stage == bcmFieldStageIngressPMF2)
            {
                stage_for_context_create = bcmFieldStageIngressPMF1;
            }
            else
            {
                stage_for_context_create = tcam_fg_util_p->stage;
            }
            rv = bcm_field_context_create
                (unit, BCM_FIELD_FLAG_WITH_ID, stage_for_context_create, &context_info, &tcam_fg_util_p->context);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_context_create\n", rv);
                return rv;
            }
        }

    }

    rv = bcm_field_group_context_attach(unit, 0, tcam_fg_util_p->fg_id, tcam_fg_util_p->context, attach_info_p);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function adds an entry to the TCAM.
 *  Currently it adds all qualifiers and actions set in the attach info and
 *  gets the values from the supplied tcam_fg_util_p structure(filled by the user).
 *  It returns a filled in entry_info_p structure
 *
 *
 * \param [in] unit                      - Device ID
 * \param [in] tcam_fg_util_p            - Input information from user(stage, context and fg_id)
 * \param [in] attach_info_p             - Attach info to be filled and attached.
 * \param [out] entry_info_p             - Entry information to be set in TCAM
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int
field_util_basic_entry_add(
    int unit,
    field_util_basic_tcam_fg_t * tcam_fg_util_p,
    bcm_field_group_attach_info_t * attach_info_p,
    bcm_field_entry_info_t * entry_info_p)
{
    int rv = BCM_E_NONE;
    int action_iter, qual_iter, ii;

    bcm_field_entry_info_t_init(entry_info_p);
   /** Setting mid-range priority */
    entry_info_p->priority = 5;
    entry_info_p->nof_entry_actions = attach_info_p->payload_info.nof_actions;
    entry_info_p->nof_entry_quals = attach_info_p->key_info.nof_quals;

   /** Loop over the supplied arrays of BCM actions, which we limit to 3 */
    for (action_iter = 0; action_iter < attach_info_p->payload_info.nof_actions; action_iter++)
    {
       /** Setting action info and value to the entry */
        entry_info_p->entry_action[action_iter].type = attach_info_p->payload_info.action_types[action_iter];
        for (ii = 0; ii < BCM_FIELD_ACTION_WIDTH_IN_WORDS; ii++)
        {
            entry_info_p->entry_action[action_iter].value[ii] = tcam_fg_util_p->action_values[action_iter].value[ii];
        }
    }

   /** Loop over the supplied array of BCM qualifiers , which we limit to 3 */
    for (qual_iter = 0; qual_iter < attach_info_p->key_info.nof_quals; qual_iter++)
    {
       /** Setting qualifier info and value to the entry */
        entry_info_p->entry_qual[qual_iter].type = attach_info_p->key_info.qual_types[qual_iter];
        for (ii = 0; ii < BCM_FIELD_QUAL_WIDTH_IN_WORDS; ii++)
        {
            entry_info_p->entry_qual[qual_iter].value[ii] = tcam_fg_util_p->qual_values[qual_iter].value[ii];
            entry_info_p->entry_qual[qual_iter].mask[ii] = 0xFFFFFFFF;
        }
    }

    rv = bcm_field_entry_add(unit, 0, tcam_fg_util_p->fg_id, entry_info_p, &(tcam_fg_util_p->entry_handle));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

int
field_util_basic_tcam_fg_full(
    int unit,
    field_util_basic_tcam_fg_t * tcam_fg_util_p)
{
    int rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;

    /*
     * Creating a field group with the info supplied by the user inside tcam_fg_util_p.
     */
    rv = field_util_basic_tcam_fg_add(unit, tcam_fg_util_p, &fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_util_basic_tcam_fg_add\n", rv);
        return rv;
    }

    /*
     * Creating(if context supplied is not DNX_FIELD_CONTEXT_ID_DEFAULT(unit)) context and
     * attaching the supplied FG to it.
     */
    rv = field_util_basic_context_attach(unit, tcam_fg_util_p, &fg_info, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_util_basic_context_attach\n", rv);
        return rv;
    }

    /*
     * Adding single entry containing all qualifiers and actions(for now), to the previously created FG.
     */
    rv = field_util_basic_entry_add(unit, tcam_fg_util_p, &attach_info, &entry_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_util_basic_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function creates a TCAM field group to the relevant stage using the provided
 *  qualifiers and actions in the "field_util_fg_t" structure.
 *  It returns the Field Group ID.
 *
 * \param [in]  unit                      - Device ID
 * \param [in/out]  fg_util_p             - Input field group information from user. Includes FG
 *                                          type, stage, bcm_quals, bcm_actions and bank info, etc.
 *                                          Output is 'fg_id' if not explicitly set.
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int
field_util_fg_add(
    int unit,
    field_util_fg_t * fg_util_p)
{
    int qual_action_iter;
    int rv = BCM_E_NONE;
    uint32 flags;
    bcm_field_group_info_t fg_info_t;

    /** Initialize fg_info_t and start to fill each field with the values in fg_util_p */
    bcm_field_group_info_t_init(&fg_info_t);
    fg_info_t.stage = fg_util_p->stage;
    fg_info_t.fg_type = fg_util_p->fg_type;

    /** Loop over the supplied arrays of BCM qualifiers and actions */
    for (qual_action_iter = 0; qual_action_iter < FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG; qual_action_iter++)
    {
        /** We treat bcmFieldQualifyCount as an Invalid value */
        if (fg_util_p->bcm_qual[qual_action_iter] != bcmFieldQualifyCount)
        {
            fg_info_t.qual_types[qual_action_iter] = fg_util_p->bcm_qual[qual_action_iter];
            fg_info_t.nof_quals++;
        }

        /** We treat bcmFieldActionCount as an Invalid value */
        if (fg_util_p->bcm_actions[qual_action_iter] != bcmFieldActionCount)
        {
            fg_info_t.action_types[qual_action_iter] = fg_util_p->bcm_actions[qual_action_iter];
            fg_info_t.action_with_valid_bit[qual_action_iter] = fg_util_p->act_valid_bit[qual_action_iter];
            fg_info_t.nof_actions++;
        }
    }

    /** Check the number of qualifiers and actions */
    if (fg_info_t.nof_quals == 0 || fg_info_t.nof_actions == 0)
    {
        printf("Error: No qualifier or action provided in fg_util_p\n");
        return -1;
    }

    /** Add TCAM info */
    fg_info_t.tcam_info = fg_util_p->tcam_info;

    /** Create with ID if fg_id specified */
    if (fg_util_p->fg_id == -1)
    {
        flags = BCM_FIELD_FLAG_MSB_RESULT_ALIGN;
    }
    else
    {
        flags = BCM_FIELD_FLAG_WITH_ID | BCM_FIELD_FLAG_MSB_RESULT_ALIGN;
    }

    /** Starting to create the FG */
    rv = bcm_field_group_add(unit, flags, &fg_info_t, &fg_util_p->fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function attaches FG on a context with the information that is provided in
 *  "field_util_fg_t" structure.
 *
 *  The context(except default context) should be created prior to call this function
 *
 * \param [in] unit                      - Device ID
 * \param [in] flags                     - This field is not used currently
 * \param [in/out] fg_util_p             - Input field group information from user. Includes FG ID,
 *                                         context ID, bcm_quals, bcm_actions, key info and action priorities.
 * \param [int] compare_id               - Comparision ID. In not in use should be initialized to BCM_FIELD_INVALID!
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int
field_util_fg_context_attach(
    int unit,
    uint32 flags,
    field_util_fg_t * fg_util_p,
    int compare_id
)
{
    int i, rv = BCM_E_NONE;
    bcm_field_group_info_t fg_info_get;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_t context_id;

    /** Get the fg info */
    rv = bcm_field_group_info_get(unit, fg_util_p->fg_id, &fg_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error: Failed to get group info for FG %d\n", fg_util_p->fg_id);
        return rv;
    }

    /** Init attach_info */
    bcm_field_group_attach_info_t_init(&attach_info);

    /** Add key info */
    attach_info.key_info.nof_quals = fg_info_get.nof_quals;
    for (i = 0; i < fg_info_get.nof_quals; i++)
    {
        if (fg_util_p->bcm_qual[i] == bcmFieldQualifyCount || 
            fg_util_p->qual_info[i].input_type == BCM_FIELD_INVALID || 
            fg_util_p->qual_info[i].input_arg == BCM_FIELD_INVALID || 
            fg_util_p->qual_info[i].offset == BCM_FIELD_INVALID)
        {
            printf("Error: Some key info is not provided in array %d\n", i);
            return -1;
        }

        attach_info.key_info.qual_types[i] = fg_util_p->bcm_qual[i];
        attach_info.key_info.qual_info[i].input_type = fg_util_p->qual_info[i].input_type;
        attach_info.key_info.qual_info[i].input_arg =  fg_util_p->qual_info[i].input_arg;
        attach_info.key_info.qual_info[i].offset = fg_util_p->qual_info[i].offset;
    }

    /** Add payload(action) info */
    attach_info.payload_info.nof_actions = fg_info_get.nof_actions;
    for (i = 0; i < fg_info_get.nof_actions; i++)
    {
        if (fg_util_p->bcm_actions[i] == bcmFieldActionCount)
        {
            printf("Error: Invalid action in bcm_actions[%d]\n", i);
            return -1;
        }

        attach_info.payload_info.action_types[i] = fg_util_p->bcm_actions[i];
        attach_info.payload_info.action_info[i].priority = fg_util_p->act_priorities[i];
    }

    /** Set compare_id */
    attach_info.compare_id = compare_id;

    /** Starting field group attaching */
    flags = 0; /* The flags field is not used currently */
    context_id = fg_util_p->context; /* The context ID must to be created firstly */
    rv = bcm_field_group_context_attach(unit, flags, fg_util_p->fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  This function adds an entry to the TCAM according to the provided entry info in
 *  "field_util_entry_info_t" structure
 *  The number of qualifiers and actions in the entry can be user defined
 *  It returns a entry handle
 *
 * \param [in] unit                      - Device ID
 * \param [uint32] flags                 - Not used currently
 * \param [in/out] ent_info_util_p       - Input field group information from user. Includes FG ID,
 *                                         entry priority, bcm_quals, bcm_actions, qualifier values&masks
 *                                         and actions values. 
 *                                         The returned entry handle will be written back to this structure
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int
field_util_field_entry_add(
    int unit,
    uint32 flags,
    field_util_entry_info_t * ent_info_util_p)
{
    int rv = BCM_E_NONE;
    int i, ii;
    bcm_field_entry_info_t entry_info;

    bcm_field_entry_info_t_init(entry_info);
    entry_info.priority = ent_info_util_p->ent_pri;

    entry_info.nof_entry_actions = 0;
    entry_info.nof_entry_quals = 0;

   /** Loop over the supplied arrays of BCM actions and qualifiers */
    for (i = 0; i < FIELD_UTIL_MAX_QUALS_ACTIONS_PER_FG; i++)
    {
        /** Setting action info and value to the entry */
        if (ent_info_util_p->bcm_actions[i] != bcmFieldActionCount)
        {
            entry_info.entry_action[i].type = ent_info_util_p->bcm_actions[i];
            for (ii = 0; ii < BCM_FIELD_ACTION_WIDTH_IN_WORDS; ii++)
            {
                entry_info.entry_action[i].value[ii] = ent_info_util_p->action_values[i].value[ii];
            }
            entry_info.nof_entry_actions++;
        }

        /** Setting qualifier info and value to the entry */
        if (ent_info_util_p->bcm_qual[i] != bcmFieldQualifyCount)
        {
            entry_info.entry_qual[i].type = ent_info_util_p->bcm_qual[i];
            for (ii = 0; ii < BCM_FIELD_QUAL_WIDTH_IN_WORDS; ii++)
            {
                entry_info.entry_qual[i].value[ii] = ent_info_util_p->qual_values[i].value[ii];
                entry_info.entry_qual[i].mask[ii] = ent_info_util_p->qual_masks[i].value[ii];
            }
            entry_info.nof_entry_quals++;
        }
    }
    entry_info.core = ent_info_util_p->core;

    /* Starting to create the field entry */
    rv = bcm_field_entry_add(unit, flags, ent_info_util_p->fg_id, &entry_info, &(ent_info_util_p->entry_handle));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  Function to create a FG and attach it on related context according to user defined FG attributes
 *  This is a easier way to create and attach a FG
 *  Before calling this function, below 6 global variables need to be configured:
 *  - nof_qual_for_fg_creation (how many qualifiers in qual_list[] are used)
 *  - nof_act_for_fg_creation  (how many actions in act_list[] are used)
 *  - qual_list[]              (Qualifier list for FG creation)
 *  - act_list[]               (Action list for FG creation)
 *  - attach_info[]            (Qualifier attaching info)
 *  - act_attach_info[]        (Action attaching info)
 *
 * \param [in]  unit           - Device ID
 * \param [in]  fg_id          - Field group ID
 * \param [in]  stage          - Field stage
 * \param [in]  fg_type        - Field group type
 * \param [in]  context_id     - The context ID for attaching the FG
 * \param [in]  rv_exp         - The expected error code for the FG creation
 * \param [in]  err_reason     - 1: Expect error for FG creation; 2: Expect error for FG attaching
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   - This function doesn't support creating FG with TCAM bank info
 *   - This function doesn't support attaching FG with action priorities
 *   - User can call 'field_util_fg_add' and 'field_util_fg_context_attach' to achieve above 2 purposes
 * \see
 *   * None
 */
int field_fg_create_and_attach(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_stage_t stage,
    bcm_field_group_type_t fg_type,
    bcm_field_context_t context_id,
    uint32 rv_exp,
    int err_reason)
{
    int i, rv;
    field_util_fg_t fg_utlil;

    /* Init structure */
    field_util_fg_t_init(unit, &fg_utlil);

    /* Define attributes for the field group */
    fg_utlil.fg_id = fg_id;
    fg_utlil.stage = stage;
    fg_utlil.fg_type = fg_type;
    fg_utlil.context = context_id;

    /* Fill qualifier info */
    for (i = 0; i < nof_qual_for_fg_creation; i++)
    {
        fg_utlil.bcm_qual[i] = qual_list[i];
        fg_utlil.qual_info[i] = attach_info[i];
    }

    /* Fill action info */
    for (i = 0; i < nof_act_for_fg_creation; i++)
    {
        fg_utlil.bcm_actions[i] = act_list[i];
        fg_utlil.act_priorities[i] = act_attach_info[i].priority;
        fg_utlil.act_valid_bit[i] = act_valid_bits[i];
    }

    /* Add TCAM bank info */
    if (bank_alloc_mode != bcmFieldTcamBankAllocationModeAuto)
    {
        fg_utlil.tcam_info.bank_allocation_mode = bank_alloc_mode;
        fg_utlil.tcam_info.nof_tcam_banks = nof_tcam_banks;
    
        for (i = 0; i < nof_tcam_banks; i++)
        {
            if (i < FIELD_UTIL_MAX_TCAM_BANKS)
            {
                fg_utlil.tcam_info.tcam_bank_ids[i] = tcam_bank_ids[i];
            }
        }
    }

    /* Create the FG with the structure */
    rv = field_util_fg_add(unit, &fg_utlil);
    if(rv_exp != BCM_E_NONE && err_reason == 1)
    {
        /* Expect error for the FG creation */
        if (rv != rv_exp)
        {
            printf("Unmatched error code in FG creation. Expect %d, but %d\n", rv_exp, rv);
            return BCM_E_FAIL;
        } else {
            printf("Expected error code %d in FG creation\n", rv);
            return BCM_E_NONE;
        }
    }
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in FG creation\n", rv);
        return rv;
    }

    /* Attach the FG it on corresponding context ID with the structure */
    rv = field_util_fg_context_attach(unit, 0, &fg_utlil, BCM_FIELD_INVALID);
    if(rv_exp != BCM_E_NONE && err_reason == 2)
    {
        /* Expect error for FG attaching */
        if (rv != rv_exp)
        {
            printf("Unmatched error code in FG attaching. Expect %d, but %d\n", rv_exp, rv);
            return BCM_E_FAIL;
        } else {
            printf("Expected error code %d in FG attaching\n", rv);
            return BCM_E_NONE;
        }
    }
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in FG attaching\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  Function to create a field entry according to user defined entry attributes
 *  This is a easier field entry creation method than function 'field_util_field_entry_add'
 *  Before calling this function, below 7 global variables should be configured:
 *  - nof_qual_for_ent_creation (how many qualifiers in ent_qual_list[] are used)
 *  - nof_act_for_ent_creation (how many actions in ent_act_list[] are used)
 *  - ent_qual_list[]  (Qualifier list for entry creation)
 *  - ent_act_list[]   (Action list for entry creation)
 *  - ent_qual_datas[] (Qualifier data list)
 *  - ent_qual_masks[] (Qualifier mask list)
 *  - ent_act_vals[]   (Action value list)
 *
 * \param [in]  unit           - Device ID
 * \param [in]  fg_id          - Field group ID
 * \param [in]  entry_pri      - The priority of the field entry
 * \param [in/out] entry_id_p  - The returned entry handle will be written back to this parameter
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   - This function can't be used if the qualifier data(or mask) width is greater than 32bits
 *   - Function 'field_util_field_entry_add' can be used for above case
 * \see
 *   * None
 */
int field_entry_add(
    int unit,
    bcm_field_group_t fg_id,
    uint32 entry_pri,
    bcm_field_entry_t * entry_id_p)
{
    int i, rv;
    field_util_entry_info_t ent_utlil;

    /* Init structure */
    field_util_entry_info_init(unit, &ent_utlil);

    /* Starting to define attributes for the field entry */
    ent_utlil.fg_id = fg_id;
    ent_utlil.ent_pri = entry_pri;

    /* Add entry qualifiers and qualifier datas&masks */
    for (i = 0; i < nof_qual_for_ent_creation; i++)
    {
        ent_utlil.bcm_qual[i] = ent_qual_list[i];
        ent_utlil.qual_values[i].value[0] = ent_qual_datas[i];
        ent_utlil.qual_masks[i].value[0] = ent_qual_masks[i];
    }

    /* Add entry actions and action values */
    for (i = 0; i < nof_act_for_ent_creation; i++)
    {
        ent_utlil.bcm_actions[i] = ent_act_list[i];
        ent_utlil.action_values[i].value[0] = ent_act_vals[i];
    }
    ent_utlil.core = ent_core_id;

    /* Create the field entry */
    rv = field_util_field_entry_add(unit, 0, &ent_utlil);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_util_field_entry_add\n", rv);
        return rv;
    }

    /* Write back the entry handle */
    *entry_id_p = ent_utlil.entry_handle;

    return rv;
}

/**
 * \brief
 *  Function to create a context in specified stage and create a preselector entry
 *  to map relevant traffic to the context
 *
 * \param [in]  unit           - Device ID
 * \param [in]  stage          - Field stage
 * \param [in]  context_id     - Context ID to be created
 * \param [in]  presel_id      - ID of the preselector entry
 * \param [in]  qual_type      - Qualifier type of the preselector entry
 * \param [in]  qual_value     - Value of the qualifier
 * \param [in]  qual_mask      - Mask of the qualifier
 * \param [in]  qual_mask      - Qualifier argument
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int field_context_and_presel_entry_create(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    bcm_field_presel_t presel_id,
    bcm_field_qualify_t qual_type,
    uint32 qual_value,
    uint32 qual_mask,
    int qual_arg)
{
    int rv;
    uint32 flag_with_id = BCM_FIELD_FLAG_WITH_ID;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;

    /* For iPMF1 and iPMF2 we are creating a preselector in iPMF1 stage */
    if (stage == bcmFieldStageIngressPMF2)
    {
        stage = bcmFieldStageIngressPMF1;
    }

    /* Create the context with ID */
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, flag_with_id, stage, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), fail in bcm_field_context_create\n", rv);
        return rv;
    }

    /* Create presel entry to map relevant traffic to the context */
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = stage;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = qual_type;
    presel_entry_data.qual_data[0].qual_value = qual_value;
    presel_entry_data.qual_data[0].qual_mask = qual_mask;
    presel_entry_data.qual_data[0].qual_arg = qual_arg;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), fail in bcm_field_presel_set\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  Function to destroy a presel entry with the given presel ID and stage
 *
 * \param [in]  unit           - Device ID
 * \param [in]  stage          - Field stage. Could be iPMF1/2/3 and ePMF
 * \param [in]  presel_id      - Presel entry ID
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */
int field_presel_entry_destroy(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_presel_t presel_id)
{
    uint32 rv;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;

    /* For iPMF2, translate it to iPMF1 */
    if (stage == bcmFieldStageIngressPMF2)
    {
        stage = bcmFieldStageIngressPMF1;
    }

    /* Set the presel ID and stage */
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = stage;

    /* Invalid the presel entry */
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;

    /* Destroy the presel entry */
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), Failed to destroy presel entry %d\n", rv, presel_id);
        return rv;
    }

    return rv;
}


/*********************************************************************************************
 *                               L2 and L3 utilities   START--->                             *
 */

struct field_utils_l2_l3_config_t
{
    /** In and Out ports. */
    bcm_port_t in_port;
    bcm_port_t out_port;

    bcm_vlan_t vid;

    /** MAC DST and SRC addresses. */
    bcm_mac_t dst_mac;

    /** IPv4 and IPv6 Destination addresses. */
    bcm_ip_t ipv4_dip;
    bcm_ip6_t ipv6_dip;

};

void cint_field_utils_l2_l3_config_t_init(
    field_utils_l2_l3_config_t * l2_config)
{
    if (NULL != l2_config) {
        sal_memset(l2_config, 0, sizeof(l2_config));
    }

    return;
}

int cint_field_utils_l2_basic_bridge_create(
    int unit,
    field_utils_l2_l3_config_t l2_config)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;
    bcm_pbmp_t pbmp, untag_pbmp;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t inlif;

    /** Create VSI */
    rv = bcm_vlan_create(unit, l2_config.vid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create\n");
        return rv;
    }

    if (!BCM_GPORT_IS_TRUNK(l2_config.in_port)) {
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(untag_pbmp);
        BCM_PBMP_PORT_ADD(pbmp, l2_config.in_port);
        BCM_PBMP_PORT_ADD(untag_pbmp, l2_config.in_port);
        rv = bcm_vlan_port_add(unit, l2_config.vid, pbmp, untag_pbmp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_add\n");
            return rv;
        }
    } else {
        rv = bcm_vlan_gport_add (unit, l2_config.vid, l2_config.in_port, BCM_VLAN_PORT_UNTAGGED);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_gport_add \n");
            return rv;
        }
    }

    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = l2_config.vid;
    vlan_port.port =  l2_config.in_port;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in Dummy bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    inlif = vlan_port.vlan_port_id;

    bcm_l2_addr_t_init(&l2_addr, l2_config.dst_mac, l2_config.vid);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = l2_config.out_port;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}

int cint_field_utils_ports_create(
    int unit,
    field_utils_l2_l3_config_t l3_config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;

    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = l3_config.vid;
    vlan_port.port =  l3_config.in_port;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in Dummy bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, l3_config.vid, l3_config.in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    bcm_port_match_info_t_init(&match_info);

    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in Dummy bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = l3_config.out_port;
    rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_match_add\n");
        return rv;
    }

    return rv;
}

int cint_field_utils_l3_intf_create(
    int unit,
    field_utils_l2_l3_config_t l3_config,
    bcm_if_t * intf_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t intf;

    if(!intf_id) {
        printf("NULL pointer received\n");
        return -1;
    }

    *intf_id = 0;

    bcm_l3_intf_t_init(&intf);
    intf.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(intf.l3a_mac_addr, l3_config.dst_mac, 6);
    intf.l3a_intf_id = l3_config.vid;
    intf.l3a_vrf = l3_config.vid;
    intf.l3a_vid = l3_config.vid;
    /* Set qos map id to 0 as default */
    intf.dscp_qos.qos_map_id = 0;

    rv = bcm_l3_intf_create(unit, &intf);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_intf_create error: %d\n", rv);
        return rv;
    }

    *intf_id = intf.l3a_intf_id;

    return rv;
}

int cint_field_utils_l3_ingress_create(
    int unit,
    field_utils_l2_l3_config_t l3_config,
    bcm_if_t * intf_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_ingress_t ingress_intf;

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_intf.vrf = l3_config.vid;
    /* Set qos map id to 0 as default */
    ingress_intf.qos_map_id = 0;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, intf_id);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_ingress_create error: %d\n", rv);
        return rv;
    }

    return rv;
}

int cint_field_utils_l3_egress_create(
    int unit,
    field_utils_l2_l3_config_t l3_config,
    bcm_if_t intf_id,
    bcm_if_t * eg_intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t egress_intf;
    bcm_mac_t next_hop_mac = { 0x22, 0x00, 0x00, 0x00, 0x00, 0x22 };
    bcm_if_t eg_arp_intf;
    bcm_gport_t gport;

    bcm_l3_egress_t_init(&egress_intf);
    egress_intf.flags = 0;
    sal_memcpy(egress_intf.mac_addr, next_hop_mac, 6);
    egress_intf.encap_id = 4100;
    egress_intf.vlan = l3_config.vid;
    rv = bcm_l3_egress_create(unit, BCM_L3_WITH_ID | BCM_L3_EGRESS_ONLY, &egress_intf, &eg_arp_intf);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_egress_create error: %d\n", rv);
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, l3_config.out_port);
    bcm_l3_egress_t_init(&egress_intf);
    egress_intf.flags = 0;
    egress_intf.port = gport;
    egress_intf.intf = intf_id;
    egress_intf.encap_id = 4100;
    rv = bcm_l3_egress_create(unit, BCM_L3_WITH_ID | BCM_L3_INGRESS_ONLY, &egress_intf, eg_intf);
    if(rv != BCM_E_NONE) {
        printf("bcm_l3_egress_create error: %d\n", rv);
        return rv;
    }

    return rv;
}

int cint_field_utils_ipv4_route_config(
    int unit,
    field_utils_l2_l3_config_t l3_config)
{
    int rv = BCM_E_NONE;
    bcm_if_t ingress_intf_id;
    bcm_if_t egress_intf_id = 0xB008;
    bcm_l3_route_t l3route;

    rv = cint_field_utils_ports_create(unit, l3_config);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_ports_create\n");
        return rv;
    }

    rv = cint_field_utils_l3_intf_create(unit, l3_config, &ingress_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_l3_intf_create\n");
        return rv;
    }

    rv = cint_field_utils_l3_ingress_create(unit, l3_config, &ingress_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_l3_ingress_create\n");
        return rv;
    }

    rv = cint_field_utils_l3_egress_create(unit, l3_config, ingress_intf_id, &egress_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_l3_egress_create\n");
        return rv;
    }

    bcm_l3_route_t_init(&l3route);
    l3route.l3a_ip_mask = 0xFFFFFFFF;
    l3route.l3a_intf = egress_intf_id;
    l3route.l3a_vrf = l3_config.vid;
    l3route.l3a_subnet = l3_config.ipv4_dip;
    rv = bcm_l3_route_add(unit, &l3route);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_route_add\n");
        return rv;
    }

    return rv;
}

int cint_field_utils_ipv6_route_config(
    int unit,
    field_utils_l2_l3_config_t l3_config)
{
    int rv = BCM_E_NONE;
    bcm_if_t ingress_intf_id;
    bcm_if_t egress_intf_id = 0xB008;
    bcm_l3_route_t l3route;

    bcm_ip6_t mask_full = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    rv = cint_field_utils_ports_create(unit, l3_config);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_ports_create\n");
        return rv;
    }

    rv = cint_field_utils_l3_intf_create(unit, l3_config, &ingress_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_l3_intf_create\n");
        return rv;
    }

    rv = cint_field_utils_l3_ingress_create(unit, l3_config, &ingress_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_l3_ingress_create\n");
        return rv;
    }

    rv = cint_field_utils_l3_egress_create(unit, l3_config, ingress_intf_id, &egress_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, cint_field_utils_l3_egress_create\n");
        return rv;
    }

    bcm_l3_route_t_init(&l3route);
    sal_memcpy(l3route.l3a_ip6_net, l3_config.ipv6_dip, 16);
    sal_memcpy(l3route.l3a_ip6_mask, mask_full, 16);
    l3route.l3a_intf = egress_intf_id;
    l3route.l3a_vrf = l3_config.vid;
    l3route.l3a_flags = BCM_L3_IP6;
    rv = bcm_l3_route_add(unit, &l3route);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_route_add\n");
        return rv;
    }

    return rv;
}

/*********************************************************************************************
 *                    <--- END   L2 and L3 utilities                                         *
 */
/**
 * \brief
 *  Function is creating a value of all ones depending on size.
 *
 * \param [in]  full_size           - The size.
 * \param [out] value          - The value with all ones.
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */

int cint_field_utils_full_size_value_get(
    int full_size,
    uint32 * value)
{
    int size_iterator;
    int rv = BCM_E_NONE;

    *value = 0;
    for (size_iterator = 0; size_iterator < full_size; size_iterator++)
    {
        *value = *value*2 + 1;
    }

    return rv;
}

/*****************************************************************************
* Function: _string
* Purpose:  Used to initialize strings in the static test configurations.
* Params:
*           str (IN) - String
* Returns:
*           The input string
 */
static uint8 *_string(const char *str) {
    uint8 rstr[BCM_FIELD_MAX_SHORT_NAME_LEN];
    sal_memcpy(rstr, str, sal_strnlen(str,BCM_FIELD_MAX_SHORT_NAME_LEN));
    return rstr;
}

/*********************************************************************************************
 *                             -->  START of ACE utils                                       *
 */
/**
 * \brief
 *  Function is creating an ace_format.
 *
 * \param [in]  unit                - The unit.
 * \param [in]  flags               - The flags (with or without ID).
 * \param [in]  ace_format_info_p   - All needed information for ace_format config
 * \param [in,out]  ace_format_id_p - Pointer to Ace format ID
 *                            as in - in case flag WITH_ID is set,
 *                                    will hold the Ace format ID
 *                           as out - in case flag WITH_ID is not set,
 *                                    will return the created Ace format ID
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */

int cint_field_utils_ace_format_cfg(
    int unit,
    uint32 flags,
    bcm_field_ace_format_info_t * ace_format_info_p,
    bcm_field_ace_format_t * ace_format_id_p)
{
    bcm_field_ace_format_info_t ace_format_info;
    int ace_iter = 0;
    void *dest_char;
    int rv = BCM_E_NONE;

    /**
     * Init ACE format structure.
     */
    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.name = ace_format_info_p->name;
    ace_format_info.nof_actions = ace_format_info_p->nof_actions;
    /**
     * Iterate between all actions and add the specific info for each of them.
     */
    for (ace_iter = 0; ace_iter < ace_format_info.nof_actions; ace_iter++)
    {
        ace_format_info.action_types[ace_iter] = ace_format_info_p->action_types[ace_iter];
        ace_format_info.action_with_valid_bit[ace_iter] = ace_format_info_p->action_with_valid_bit[ace_iter];
    }

    /**
     * Configure the ACE format.
     */
    rv = bcm_field_ace_format_add(unit, flags, &ace_format_info, ace_format_id_p);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }
    printf("The ace_format %d was created\n", *ace_format_id_p);
    return rv;
}

/**
 * \brief
 *  Function is creating an ace entry.
 *
 * \param [in]  unit                - The unit.
 * \param [in]  flags               - The flags (with or without ID).
 * \param [in]  ace_format_id       - The ID of the ace_format.
 * \param [in]  ace_entry_info_p    - All needed information for ace_entry config
 * \param [in,out]  entry_handle    - Pointer to Ace entry ID
 *                            as in - in case flag WITH_ID is set,
 *                                    will hold the Ace entry ID
 *                           as out - in case flag WITH_ID is not set,
 *                                    will return the created Ace entry ID
 *
 * \return
 *   shr_error_e - Type of Error
 * \remark
 *   * None
 * \see
 *   * None
 */

int cint_field_utils_ace_entry_cfg(
    int unit,
    uint32 flags,
    bcm_field_ace_format_t ace_format_id,
    bcm_field_ace_entry_info_t * ace_entry_info_p,
    uint32 * entry_handle_p)
{
    bcm_field_ace_entry_info_t ace_entry_info;
    int ace_iter;
    int rv = BCM_E_NONE;

    /**
     * Init ACE entry structure.
     */
    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = ace_entry_info_p->nof_entry_actions;
    /**
     * Iterate between all actions and add the specific info for each of them.
     */
    for (ace_iter = 0; ace_iter < ace_entry_info.nof_entry_actions; ace_iter++)
    {
        ace_entry_info.entry_action[ace_iter].type = ace_entry_info_p->entry_action[ace_iter].type;
        ace_entry_info.entry_action[ace_iter].value[0] = ace_entry_info_p->entry_action[ace_iter].value[0];
        printf("Action %d is added to the array \n",ace_iter);
    }
    /**
     * Configure the ACE entry.
     */
    rv = bcm_field_ace_entry_add(unit, flags, ace_format_id, &ace_entry_info, entry_handle_p);
    if(rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
       return rv;
    }

    printf("The ace_entry %d was created\n", *entry_handle_p);
    return rv;
}

/*********************************************************************************************
 *                              <--  END of ACE utils                                        *
 */

/**
 * \brief
 *  Function to returns the name of the given BCM field stage.
 *
 * \param [in]  stage                - The unit.
 * \return
 *   char - Stage name
 * \remark
 *   * None
 * \see
 *   * None
 */
char * cint_field_utils_stage_name_get(bcm_field_stage_t stage)
{
    switch (stage)
    {
        case bcmFieldStageEgress:
        {
            return  "Egress";
            break;
        }
        case bcmFieldStageExternal:
        {
            return "External";
            break;
        }
        case bcmFieldStageIngressPMF1:
        {
            return "IngressPMF1";
            break;
        }
        case bcmFieldStageIngressPMF2:
        {
            return "IngressPMF2";
            break;
        }
        case bcmFieldStageIngressPMF3:
        {
            return "IngressPMF3";
            break;
        }
        default:
        {
            return "Invalid";
            break;
        }
    }

    return "Invalid";
}

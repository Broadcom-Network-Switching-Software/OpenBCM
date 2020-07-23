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
 * cint ../../../src/examples/dnx/field/cint_field_fem.c
 * cint;
 * int unit = 0;
 * cint_field_fem_simple_case_DE_main(unit);
 * cint_field_fem_simple_case_TCAM_main(unit);
 *
 * Configuration example end
 *
 * This CINT creates a FEM action and attaches it to DE or TCAM field group.
 *
 *
 * ************************************************************************
 * The main functions:
 * cint_field_fem_simple_case_DE_main(unit);
 * cint_field_fem_simple_case_TCAM_main(unit);
 *
 * The destroy function:
 * cint_field_fem_test_config_destroy(unit,fg_type)
 * fg_type can be TCAM or DE.
 */
 /**
  * Global variables
  */
 /**
  * {
  */
/**
 * The ID of the context
 */
bcm_field_context_t Cint_field_fem_context_id = 9;
/**
 * Invalid FG ID
 */
int Cint_field_fem_invalid_fg_id = -1;
/**
 * The TCAM type of the FG
 */
int FG_type_TCAM = 1;
/**
 * The ID of the preselector
 */
bcm_field_presel_t Cint_field_fem_presel_id = 10;
/**
 * The number of quals for preselection
 */
int Cint_field_fem_presel_nof_quals = 1;
/**
 * The value of the preselector qualifier
 */
uint32 Cint_field_fem_presel_qual_value = 0x1;
/**
 * The mask of the preselector qualifier
 */
uint32 Cint_field_fem_presel_qual_mask = 0x1;
/**
 * The FEM ID of the fem action.
 */
/* int Cint_field_fem_id_first = 10; #### REMOVE */
bcm_field_array_n_position_t  Cint_field_array_n_position_first = {3, 2};
/**
 * The FEM ID of the second action (bcmFieldActionMirrorIngress)
 */
/* int Cint_field_fem_id_second = 11; #### REMOVE */
bcm_field_array_n_position_t  Cint_field_array_n_position_second = {3, 3};
/**
 * The FEM ID of the third action (bcmFieldActionOutVport0)
 */
/* int Cint_field_fem_id_third = 12; #### REMOVE */
bcm_field_array_n_position_t  Cint_field_array_n_position_third = {3, 4};
/**
 * The number of FGs used in the test
 */
int Cint_field_fem_nof_fg = 1;
/**
 * The number of Actions per FG used in the test
 */
int Cint_field_fem_nof_actions = 2;
int Cint_field_fem_nof_de_actions = 1;
/**
 * The number of Quals per FG used in the test
 */
int Cint_field_fem_nof_quals = 1;
/**
 * The number of conditions used for creating a fem action.
 */
int Cint_field_fem_nof_conditions  = 1;
/**
 * The number of Actions per FG used in the test for simple_Case
 */
int Cint_field_fem_nof_fem_actions  = 1;
/**
 * The number of Actions in overriding FG
 */
int Cint_field_fem_nof_actions_for_overriding_fg = 3;
/**
 * The number of FEMs used in the test.
 */
int Cint_field_fem_nof_fems = 1;
/**
 * The number of mapping_bits/output_bit used for creating a fem action,
 */
int Cint_field_fem_nof_mapping_bits_ActionForward = 21;
/**
 * The number of condition IDs.
 */
int Cint_field_fem_nof_condition_ids = 16;
/**
 * The value of the condition MSB.
 */
int Cint_field_fem_condition_msb  = 19;
/**
 * The ID of the condition simple_Case.
 */
int Cint_field_fem_condition_id  = 6;
/**
 * The number of mapping_bits for Action Mirror.
 */
int Cint_field_fem_nof_mapping_bits_ActionMirror = 9;
/**
 * The number of mapping_bits for Action OutVport0.
 */
int Cint_field_fem_nof_mapping_bits_OutVport0 = 22;
/**
 * The presel_entry. Used for creating and destroying the presel
 */
bcm_field_presel_entry_id_t presel_entry_id;
/**
 * The priority of the entry.
 */
uint32 Cint_field_fem_entry_priority = 1;
/**
  * The entry_id of the overriding FG. Used for creating and destroying the entry
  */
bcm_field_entry_t entry_id_overriding;
/**
  * The entry_id for the FEM TCAM FG. Used for creating and destroying the entry
  */
bcm_field_entry_t entry_id;
/**
 * Structure, which contains information,
 * for creating of user quals and predefined one.
 */
struct cint_field_fem_qual_info_t {
    /**
     * Qualifier name
     */
    char * name;
    /**
     * Qualifier size
     */
    uint32 qual_size;
    /**
     * The attach info needed for each qualifier (input_type, input_arg and offset)
     */
    bcm_field_qualify_attach_info_t qual_attach_info;
    /**
     * The ID of the qualifier.
     */
    bcm_field_qualify_t qual_id;
     /**
     * The value of the qualifier.
     * Needed in case the FG is from type TCAM.
     * And we should create an entry.
     */
    uint32 qual_value;
    /**
     * The mask of the qualifier.
     * Needed in case the FG is from type TCAM.
     * And we should create an entry.
     */
    uint32 qual_mask;
};
/**
 * \brief
 *   Structure of descriptors of actions to use for testing new interface
 *   for direct extraction.
 *   Note that we are only handling user defined actions.
 */
struct cint_field_fem_actions_info_t {
    /**
     * Action name
     */
    char * name;
    /**
     * Number of bits to assign to this user defined action
     */
    unsigned int action_size;
    /**
     * Prefix size for this user defined action.
     */
    unsigned int prefix_size;
    /**
     * The prefix value of the action
     */
    int prefix_value;
    /**
     * Action valid bit
     */
    int action_valid_bit;
    /**
     * The type for this action.
     */
    bcm_field_action_t action_type;
    /**
     * Memory space to load identifier of this user defined action (result
     * of bcm_field_action_create()).
     */
    bcm_field_action_t action_id;
    /**
     * The value of the action.
     * Needed in case the FG is from type TCAM.
     * And we should create an entry.
     */
    uint32 action_value;
};
/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
struct cint_field_fem_fg_info_t {
    /**
     * The FG_ID
     */
    bcm_field_group_t fg_id;
    /**
     * The type of the FG
     */
    bcm_field_group_type_t fg_type;
    /**
     * The number of qualifiers for the FG
     */
    int nof_quals;
    /**
     * The number of actions for the FG
     */
    int nof_actions;
    /**
     * The attach info for qualifiers
     */
    cint_field_fem_qual_info_t * qual_info;
    /**
     * The attach info for actions
     */
    cint_field_fem_actions_info_t * action_info;
    /**
     * Entry ID.
     */
    bcm_field_entry_t entry_id;
    /**
     * The entry priority.
     * Needed in case the FG is from type TCAM.
     * And we should create an entry.
     */
    uint32 entry_priority;
};
/**
 * This structure holds the needed information for all conditions.
 */
struct cint_field_fem_conditions_t {
    /**
     * The ID of the condition
     */
    int condition_ids[Cint_field_fem_nof_condition_ids];
    /**
     * Array of structures, each describing
     * the condition information for the FEM:
     * extraction_id - Choose one of 4 action extraction tables that
     *                          will produce an action.
     * is_action_valid - If TRUE, the FEM will produce an action for the
     *                         given 4 bits word.
     */
    bcm_field_fem_condition_t fem_condition[BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM];
};
/**
 * This structure holds the needed information for all extractions.
 */
struct cint_field_fem_extraction_t {
    /**
     * The number of mapping bits per FEM.
     * Used for iterating between all of the mapping bits.
     */
    int nof_mapping_bits;
    /**
     * Add a constant value to the result of the extraction.
     */
    uint32 increment;
    /**
     * Action type to be performed by this extraction.
     */
    bcm_field_action_t action_type;
    /**
     * Array of extraction commands. Each extract 1 bit of the action value.
     *     source_type -  Output source type. For more details look 'bcm_field_fem_extraction_output_source_type_t'
     *                    If we choose the source type to be bcmFieldFemExtractionOutputSourceTypeForce -
     *                    The output bit is equal to the bit set into the 'forced_value' field
     *                    If we choose the source type to be bcmFieldFemExtractionOutputSourceTypeKey -
     *                    The output bit is equal to the bit from the offset that we set into 'offset' field
     * offset - Offset from key select.
     * forced_value - The forced value.
     */
    bcm_field_fem_extraction_output_bit_t output_bit[BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM];
};
/**
 * Structure, which contains need information
 * for configuring of FEM condition actions.
 */
struct cint_field_fem_action_info_t {
   /**
    * Fem Identifier
    */
    /* bcm_field_fem_id_t fem_id; #### REMOVE */
    /*
     * Structure replacing FEM_ID to indicate selected FEM by
     * its priority in the general FES/FEM structure.
     * See DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET
     */
    bcm_field_array_n_position_t field_array_n_position;
    /**
     * Structure holds all FEM input information.
     *   input_offset - Offset inside the FG-ID result buffer. The FEM input is 32 bits long.
     *                  When the FG result buffer is longer than 32 bits, offset is required
     *                  (in resolution of 16 bits).
     *   overriding_fg_id - If valid FG-ID, override the main group result buffer with 16 bits
     *                      result from this specified TCAM-FG result.
     *                      If not, give not existing FG ID.
     */
    bcm_field_fem_input_info_t fem_input;
    /**
     * Select the MSB of a 4 bit chunk that
     * will choose the condition
     * taken from the 32 input bits selected
     * by input offset (range of 3-31).
     * The condition MSB is the same for all 16 conditions!
     */
    uint8 condition_msb;
    /**
     * The number of conditions per FEM.
     * Used for iterating between all of the conditions.
     */
    int nof_conditions;
    /**
     * look at cint_field_fem_conditions_t
     */
    cint_field_fem_conditions_t * fem_conditions;
    /**
     * The number of extractions per FEM.
     * Used for iterating between all of the extractions.
     */
    int nof_fem_actions;
    /**
     * look at cint_field_fem_extraction_t
     */
    cint_field_fem_extraction_t * fem_extractions;
};
/* *INDENT-OFF* */
/**
 * This array will be used for creating User Defined Actions.
 * We are calling it in Cint_field_fem_DE_fg_info_arr.
 */
cint_field_fem_actions_info_t Cint_field_fem_user_actions_arr[Cint_field_fem_nof_de_actions] = {
/** action_name */   /** action_size */ /** prefix_size */  /** prefix_value */  /** valid bit*/ /** action_type */
  {"user_action_1",          32,            0,               0x00000000,            FALSE,       bcmFieldActionVoid, {0}}
};
/**
 * This array will be used for creating User Defined Qualifiers.
 * We are calling it in Cint_field_fem_DE_fg_info_arr.
 */
cint_field_fem_qual_info_t Cint_field_fem_user_quals_arr[Cint_field_fem_nof_quals] = {
  /** qual_name */  /* size */  /* {input type */                /* input arg */    /* offset} */
  {"user_qual_1",      32,    {bcmFieldInputTypeLayerAbsolute,      0,                 0}}
};
/**
 * This array will be used for creating a TCAM FEM FG.
 * We are calling it in Cint_field_fem_overriding_fg_info_arr.
 */
cint_field_fem_actions_info_t Cint_field_fem_overriding_fg_actions_arr[Cint_field_fem_nof_actions_for_overriding_fg] = {
    /** User action values */    /** valid bit */  /** The action type */            /** ID*/                           /** Action value */
     {0,0,0,0,                    TRUE,             bcmFieldActionDropPrecedence, bcmFieldActionDropPrecedence,                  0x2},
     /** User action values */    /** valid bit */  /** The action type */            /** ID*/                           /** Action value */
     {0,0,0,0,                    TRUE,     bcmFieldActionIngressTimeStampInsert, bcmFieldActionIngressTimeStampInsert,          0x11111111},
     /** action_name */   /** action_size */ /** prefix_size */  /** prefix_value */  /** valid bit*/   /** action_type */
     {"user_action_void",          28,              4,                 0x0,                FALSE,       bcmFieldActionVoid, {0}, 0xABCD000},
};
/**
 * This array will be used for creating User Defined Qualifiers.
 * We are calling it in Cint_field_fem_overriding_fg_info_arr.
 */
cint_field_fem_qual_info_t Cint_field_fem_overriding_fg_quals_arr[Cint_field_fem_nof_quals] = {
/** User qual values */  /* {input type */                /* input arg */    /* offset} */ /** The qual */  /** Entry qual value    qual_mask */
  {0,0,                     {bcmFieldInputTypeLayerAbsolute,      2,               0},    bcmFieldQualifyL4SrcPort,         0xc8,           0xFFFF}
};
/**
 * This array will be used for creating a TCAM FEM FG.
 * We are calling it in Cint_field_fem_TCAM_fg_info_arr.
 */
cint_field_fem_qual_info_t Cint_field_fem_tcam_quals_info_arr[Cint_field_fem_nof_quals] = {
/** User qual values */  /* {input type */                /* input arg */    /* offset} */ /** The qual */  /** Entry qual value    qual_mask */
  {0,0,                    {bcmFieldInputTypeLayerAbsolute,      2,               0},    bcmFieldQualifyL4DstPort,         0x12c,           0xFFFF}
};

/**
 * This array will be used for creating a TCAM FEM FG.
 * We are calling it in Cint_field_fem_TCAM_fg_info_arr
 */
cint_field_fem_actions_info_t Cint_field_fem_tcam_actions_info_arr[Cint_field_fem_nof_actions] = {
/** action_name */   /** action_size */ /** prefix_size */  /** prefix_value */  /** valid bit*/ /** action_type */ /** action_type */   /** action_value */
  {"user_action_1",          32,            0,               0x00000000,            FALSE,       bcmFieldActionVoid,  bcmFieldActionVoid,  0x1196c698},
  {"user_action_2",          32,            0,               0x00000000,            FALSE,       bcmFieldActionVoid,  bcmFieldActionVoid,  0x1196c698}
};
/**
 * This array will be used for creating a DE FG.
 */
cint_field_fem_fg_info_t Cint_field_fem_DE_fg_info_arr[Cint_field_fem_nof_fg] = {
   {
        /** FG ID */
        0,
        /** FG_type */
        bcmFieldGroupTypeDirectExtraction,
        /** nof_quals */
        Cint_field_fem_nof_quals,
        /** nof_actions */
        Cint_field_fem_nof_de_actions,
        /** qual_info */
        Cint_field_fem_user_quals_arr,
        /** action_info */
        Cint_field_fem_user_actions_arr
   }
};
/**
 * This array will be used for creating a TCAM FG.
 */
cint_field_fem_fg_info_t Cint_field_fem_overriding_fg_info_arr[Cint_field_fem_nof_fg] = {
   {
        /** FG ID */
        0,
        /** FG_type */
        bcmFieldGroupTypeTcam,
        /** nof_quals */
        Cint_field_fem_nof_quals,
        /** nof_actions */
        Cint_field_fem_nof_actions_for_overriding_fg,
        /** qual_info */
        Cint_field_fem_overriding_fg_quals_arr,
        /** action_info */
        Cint_field_fem_overriding_fg_actions_arr,
        /** Entry ID */
        entry_id_overriding,
        /** The entry priority*/
        Cint_field_fem_entry_priority
   }
};
/**
 * This array will be used for creating a TCAM FG.
 */
cint_field_fem_fg_info_t Cint_field_fem_TCAM_fg_info_arr[Cint_field_fem_nof_fg] = {
   {
        /** FG ID */
        0,
        /** FG_type */
        bcmFieldGroupTypeTcam,
        /** nof_quals */
        Cint_field_fem_nof_quals,
        /** nof_actions */
        Cint_field_fem_nof_actions,
        /** qual_info */
        Cint_field_fem_tcam_quals_info_arr,
        /** action_info */
        Cint_field_fem_tcam_actions_info_arr,
        /** Entry ID */
        entry_id,
        /** The entry priority*/
        Cint_field_fem_entry_priority
   }
};
/**
 *
 * Start of the FEM configuration
 *
 * {
 */
/**
 * The condition information per FEM
 */
cint_field_fem_conditions_t Cint_field_fem_condition_arr [Cint_field_fem_nof_fem_actions] = {
    {
        /** The ID of the condition */
        {Cint_field_fem_condition_id},
        /**
         * Array of structures, each describing
         * the condition information for the FEM:
         * extraction_id - Choose one of 4 action extraction tables that
         *                          will produce an action.
         * is_action_valid - If TRUE, the FEM will produce an action for the
         *                         given 4 bits word.
         */
        {{0,TRUE}}
    }
};
/**
 * The extractions information per FEM
 */
 cint_field_fem_extraction_t Cint_field_fem_extractions_arr[Cint_field_fem_nof_fem_actions] = {
     {
         /** The number of mapping_bits*/
         Cint_field_fem_nof_mapping_bits_ActionForward,
         /** The value in the increment */
         0,
         /** The action type */
         bcmFieldActionForward,
         /**
          * Mapping bits information
          */
         {
             /** src_type                 |       offset | force_value */
             {bcmFieldFemExtractionOutputSourceTypeKey, 0, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 1, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 2, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 3, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 4, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 5, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 6, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 7, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 8, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 9, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 10, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 11, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 12, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 13, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 14, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 15, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 16, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 17, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 18, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 19, 0},
             {bcmFieldFemExtractionOutputSourceTypeKey, 20, 0}
         }
     }
 };
 /**
  * The whole needed information for creating a FEM.
  */
 cint_field_fem_action_info_t Cint_field_fem_action_info_arr[Cint_field_fem_nof_fem_actions] = {
     {
         /** The FEM ID*/
         Cint_field_array_n_position_first,
         {
         /** The input offset*/
             0,
             /** The ID of the FG. In this case illegal ID*/
             0
         },
         /** Condition MSB*/
         Cint_field_fem_condition_msb,
         /** The number of conditions*/
         Cint_field_fem_nof_conditions,
         /** The condition array*/
         Cint_field_fem_condition_arr,
         /** The number of extractions*/
         Cint_field_fem_nof_fem_actions,
         /** The extraction array*/
         Cint_field_fem_extractions_arr
     }
 };
 /**
  *
  * End of the FEM configuration
  *
  * }
  */


/* *INDENT-ON* */

/**
 * }
 */

/**
 * \brief
 *
 *  This function contains the bcm_field_presel_set
 *  application and bcm_field_context_create.
 *  This function sets all required HW
 *  configuration for bcm_field_presel_set and bcm_field_context_create
 *  to be performed for IPMF1.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_fem_context_create_presel_set(
    int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_info_t context_info;
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fem_context_create_presel_set";
   /**
    * Init the context_info
    */
    bcm_field_context_info_t_init(&context_info);
    /**
     * Create the context with ID
     */
    rv = bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &Cint_field_fem_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }
   /**
    * Fill the info in presel_entry_data and bcm_field_presel_entry_id_t structures
    */
    presel_entry_id.presel_id = Cint_field_fem_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;
    presel_entry_data.context_id = Cint_field_fem_context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = Cint_field_fem_presel_nof_quals;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = Cint_field_fem_presel_qual_value;
    presel_entry_data.qual_data[0].qual_mask = Cint_field_fem_presel_qual_mask;
    /**
     * Set the presel
     */
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_presel_set\n", proc_name, rv);
        return rv;
    }
    return rv;
}
/**
 * This function configure the user qualifiers.
 * It is used later in the cint_field_fem_fg_config.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - The PMF stage.
 * \param [in] qual_info_p - Pointer to Structure which contains
 *      all needed information for creating a qualifier.
 */
int
cint_field_fem_user_qual_config(
    int unit,
    bcm_field_stage_t field_stage,
    cint_field_fem_qual_info_t * qual_info_p)
{
    bcm_field_qualifier_info_create_t qual_info;
    char * qual_name;
    void * tmp_p;
    int rv;
    char *proc_name;
    void *dest_char;

    proc_name = "field_fem_user_qual_config";

    tmp_p = qual_info.name;
    qual_name = tmp_p;
    rv = BCM_E_NONE;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    dest_char = &(qual_info.name[0]);
    sal_strncpy(dest_char, qual_info_p->name, sizeof(qual_info.name));
    qual_info.name[sizeof(qual_info.name) - 1] = 0;
    qual_info.size = qual_info_p->qual_size;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &qual_info_p->qual_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_qualifier_create on qual_id = %d (%s) \n", proc_name, rv,
               qual_info_p->qual_id, qual_info_p->name);
        return rv;
    }
    return rv;
}
/**
 * This function configure the user actions.
 * It is used later in the cint_field_fem_fg_config.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - The PMF stage.
 * \param [in] action_info_p - Pointer to Structure which contains
 *      all needed information for creating a action.
 */
int
cint_field_fem_user_action_config(
    int unit,
    bcm_field_stage_t field_stage,
    cint_field_fem_actions_info_t * action_info_p)
{
    bcm_field_action_info_t action_info;
    char * action_name;
    int rv;
    void * tmp_p;
    char *proc_name;
    void *dest_char;

    proc_name = "field_fem_user_action_config";

    rv = BCM_E_NONE;
    tmp_p = action_info.name;
    action_name = tmp_p;

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = action_info_p->action_type;
    dest_char = &(action_info.name[0]);
    sal_strncpy(dest_char, action_info_p->name, sizeof(action_info.name));
    action_info.name[sizeof(action_info.name) - 1] = 0;
    action_info.size = action_info_p->action_size;
    action_info.stage = field_stage;
    action_info.prefix_size = action_info_p->prefix_size;
    action_info.prefix_value = action_info_p->prefix_value;

    rv = bcm_field_action_create(unit, 0, &action_info, &action_info_p->action_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error (%d), in bcm_field_action_create on action_id = '%s' \n", proc_name, rv, action_info_p->name);
        return rv;
    }
    return rv;
}
/**
 * This function is configuring all needed FEM actions.
 * This function is using the API bcm_field_fem_action_add to add new FEM actions.
 * It is filling the structure bcm_field_fem_action_info_t with the data from the internal
 * structure cint_field_fem_action_info_t.
 *
 *
 * \param [in] unit - The unit number.
 * \param [in] fg_id - The Id for the Fg needed to call the bcm_field_fem_action_add API.
 * \param [in] fem_action_info_p - Pointer to all needed information for configuring a FEM.
 * \param [in] fem_iterator - It is iterating between all of the fem_actions that have to be created.
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int
cint_field_fem_action_config(
    int unit,
    bcm_field_group_t fg_id,
    cint_field_fem_action_info_t * fem_action_info_p,
    int fem_iterator)
{
    char *proc_name;
    int mapping_bits_index, conditions_index, extractions_index;
    bcm_field_fem_action_info_t fem_action_info;
    bcm_field_action_priority_t action_priority;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fem_action_config";
    int * condition_id_p = fem_action_info_p->fem_conditions[fem_iterator].condition_ids;

    /**
     * Init the fem_action_info structure.
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    /**
     * Fill the FEM input data. (the input offset and the overriding_fg_id).
     * Note that If we don't have override we just put illegal FG ID!
     */
    fem_action_info.fem_input.input_offset = fem_action_info_p->fem_input.input_offset;
    fem_action_info.fem_input.overriding_fg_id = fem_action_info_p->fem_input.overriding_fg_id;
    /**
     *  Fill the Cndition MSB.
     */
    fem_action_info.condition_msb = fem_action_info_p->condition_msb;
    /**
     * Iterate between all of the conditions and fill the conditions data(extraction_id, is_action_valid).
     * (If the action is valid or not and the extraction FG ID if we have overriding FG ID)
     */
    for (conditions_index = 0; conditions_index < fem_action_info_p->nof_conditions; conditions_index++)
    {
        fem_action_info.fem_condition[condition_id_p[conditions_index]].extraction_id =
            fem_action_info_p->fem_conditions[fem_iterator].fem_condition[conditions_index].extraction_id;
        fem_action_info.fem_condition[condition_id_p[conditions_index]].is_action_valid =
            fem_action_info_p->fem_conditions[fem_iterator].fem_condition[conditions_index].is_action_valid;
    }
    /**
     * Iterate between all of the extractions and set the extraction data.
     * (The action_type, the increment, and the output_bits)
     */
    for (extractions_index = 0; extractions_index < 1; extractions_index++)
    {
        fem_action_info.fem_extraction[extractions_index].action_type =
             fem_action_info_p->fem_extractions[fem_iterator].action_type;
        fem_action_info.fem_extraction[extractions_index].increment =
            fem_action_info_p->fem_extractions[fem_iterator].increment;
        /**
         * Iterate between all of the mapping_bits(output_bits) and fill the mapping_bits(output_bits) related data.
         * (The source type - it shows if we use the key offset or a forced value,
         *  The offset if the source type is equal to bcmFieldFemExtractionOutputSourceTypeKey,
         *  The forced value(0 or 1) if the source type is equal to bcmFieldFemExtractionOutputSourceTypeForce)
         */
        for (mapping_bits_index = 0; mapping_bits_index < fem_action_info_p->fem_extractions[fem_iterator].nof_mapping_bits; mapping_bits_index++)
        {
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type =
                fem_action_info_p->fem_extractions[fem_iterator].output_bit[mapping_bits_index].source_type;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset =
                fem_action_info_p->fem_extractions[fem_iterator].output_bit[mapping_bits_index].offset;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value =
                fem_action_info_p->fem_extractions[fem_iterator].output_bit[mapping_bits_index].forced_value;
        }
    }
    {
        int array_id, fem_position_in_array;

        array_id = fem_action_info_p->field_array_n_position.array_id;
        fem_position_in_array = fem_action_info_p->field_array_n_position.fem_position_in_array;
        action_priority = BCM_FIELD_ACTION_POSITION(array_id,fem_position_in_array);
    }
    rv = bcm_field_fem_action_add(unit, 0, fg_id, action_priority, &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_fem_action_add for fg_id = %d | array_id = %d | fem_position_in_array = %d\n", proc_name, rv,
               fg_id, fem_action_info_p->field_array_n_position.array_id, fem_action_info_p->field_array_n_position.fem_position_in_array);
        return rv;
    }
    return rv;
}
/**
 * \brief
 *  This function sets all required HW
 *  configuration for bcm_field_group_add and bcm_field_group_context_attach
 *  to be performed for IPMF2.
 *  In case the field group is from type TCAM the bcm_field_entry_add too.
 *  If we want to create a FG which is using FEM action we are  calling the function
 *   cint_field_fem_action_config which is creating fem actions.
 *
 * \param [in] unit - The unit number.
 * \param [in] stage - The PMF stage.
 * \param [in] fem_fg_info - Pointer to Structure which contains
 *      all needed information for creating a FG.
 * \param [in] is_fem_fg - Shows if the FG that
 *                   we are creating is using FEM actions or not.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int static
cint_field_fem_fg_config(
    int unit,
    bcm_field_stage_t stage,
    cint_field_fem_fg_info_t * fem_fg_info_p,
    cint_field_fem_action_info_t * fem_action_info_p,
    int is_fem_fg)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    int qual_index, fem_iterator, action_index;
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fem_fg_config";
    /**
     * Init the fg_info structure.
     */
    bcm_field_group_info_t_init(&fg_info);
    /**
     * Init the attach_info.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    /**
     * Fill the fg_info structure
     */
    fg_info.stage = stage;
    fg_info.fg_type = fem_fg_info_p->fg_type;
    fg_info.nof_quals = fem_fg_info_p->nof_quals;
    fg_info.nof_actions = fem_fg_info_p->nof_actions;
    attach_info.key_info.nof_quals = fem_fg_info_p->nof_quals;
    attach_info.payload_info.nof_actions = fem_fg_info_p->nof_actions;
    /**
     * For TCAM
     */
    if (fg_info.fg_type == bcmFieldGroupTypeTcam)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.priority = fem_fg_info_p->entry_priority;
        entry_info.nof_entry_quals = fem_fg_info_p->nof_quals;
        entry_info.nof_entry_actions = fem_fg_info_p->nof_actions;
    }
    /**
     * Iterate between all of the qualifiers and add them in the fg_info structure.
     */
    for (qual_index = 0; qual_index < fem_fg_info_p->nof_quals; qual_index++)
    {
        /**
         * Check if the qualifier is User defined and if it is create the qual using
         * cint_field_fem_user_qual_config function
         */
        if (fem_fg_info_p->qual_info[qual_index].name != NULL)
        {
            rv = cint_field_fem_user_qual_config(unit, fg_info.stage, &(fem_fg_info_p->qual_info[qual_index]));
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in cint_field_fem_user_qual_config. \n", rv);
                return rv;
            }
        }
        fg_info.qual_types[qual_index] = fem_fg_info_p->qual_info[qual_index].qual_id;
        attach_info.key_info.qual_types[qual_index] = fem_fg_info_p->qual_info[qual_index].qual_id;
        attach_info.key_info.qual_info[qual_index].input_type = fem_fg_info_p->qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg = fem_fg_info_p->qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset = fem_fg_info_p->qual_info[qual_index].qual_attach_info.offset;
       /**
        * IF the FG is TCAM we should create an entry,
        * Here we fill all entry qual related info.
        */
       if (fg_info.fg_type == bcmFieldGroupTypeTcam)
       {
           entry_info.entry_qual[qual_index].type = fem_fg_info_p->qual_info[qual_index].qual_id;
           entry_info.entry_qual[qual_index].value[0] = fem_fg_info_p->qual_info[qual_index].qual_value;
           entry_info.entry_qual[qual_index].mask[0] = fem_fg_info_p->qual_info[qual_index].qual_mask;
       }
    }
    /**
     * Iterate between all of the actions and add them in the fg_info structure.
     */
    for (action_index = 0; action_index < fem_fg_info_p->nof_actions; action_index++)
    {
        /**
         * Check if the action is User defined and if it is create the action using
         * cint_field_fem_user_action_config function
         */
        if (fem_fg_info_p->action_info[action_index].name != NULL)
        {
            rv = cint_field_fem_user_action_config(unit, fg_info.stage, &(fem_fg_info_p->action_info[action_index]));
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in cint_field_fem_user_action_config. \n", rv);
                return rv;
            }
        }
        fg_info.action_with_valid_bit[action_index] = fem_fg_info_p->action_info[action_index].action_valid_bit;
        fg_info.action_types[action_index] = fem_fg_info_p->action_info[action_index].action_id;
        attach_info.payload_info.action_types[action_index] =  fem_fg_info_p->action_info[action_index].action_id;
        /**
         * IF the FG is TCAM we should create an entry,
         * Here we fill all entry action related info.
         */
        if (fg_info.fg_type == bcmFieldGroupTypeTcam)
        {
            entry_info.entry_action[action_index].type = fem_fg_info_p->action_info[action_index].action_id;
            entry_info.entry_action[action_index].value[0] = fem_fg_info_p->action_info[action_index].action_value;
        }
    }
    /**
     * Add the FG.
     */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fem_fg_info_p->fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    /**
     * Attach the fem action to the FG.
     */
    if (is_fem_fg == TRUE)
    {
        for (fem_iterator = 0; fem_iterator < fem_action_info_p->nof_fem_actions; fem_iterator++)
        {
            rv = cint_field_fem_action_config(unit, fem_fg_info_p->fg_id, &fem_action_info_p[fem_iterator], fem_iterator);
            if (rv != BCM_E_NONE)
            {
                printf("%s Error (%d), in cint_field_fem_action_config \n", proc_name, rv);
                return rv;
            }
        }
    }
    /**
     * Attach the FG to the context.
     */
    rv = bcm_field_group_context_attach(unit, 0, fem_fg_info_p->fg_id, Cint_field_fem_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
    /**
     * IF the FG is TCAM we should create an entry!
     */
    if (fg_info.fg_type == bcmFieldGroupTypeTcam)
    {
        /**
         * Add an entry to the FG
         */
         rv = bcm_field_entry_add(unit, 0, fem_fg_info_p->fg_id, &entry_info, &(fem_fg_info_p->entry_id));
         if (rv != BCM_E_NONE)
         {
             printf("Error (%d), in bcm_field_entry_add ipmf1 entry_id(%d)\n", rv, entry_id);
             return rv;
         }
    }
    return rv;
}

 /**
 * SIMPLE_CASE DE configuration!
 *   1. We are creating 1 DE FG with 1 action and 1 qualifier and 1 FEM action.
 *   2. We are creating 1 TCAM FG with 1 qual and 1 action
 *        which should override first 16 bits of the result
 *
 * The FEM FG configuration:
 * PACKET HEADER
 * The key is created from the bits of the ETH header,
 * because the input_arg is equal to 0.
 * For example!
 *        _____________________  _______________________
 * field [        ETH.DA       ][          ETH.SA       ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [  11:96:c6:98:00:01  ][    00:00:00:00:00:01  ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~
 *        ______________________________________________________
 * field [               ETH.DA in binary                       ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [ 00010001:10010110:11000110:10011000:00000000:00000001]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We are using DE field group thats why:
 * The position in the key depends on Qualifiers sequence.
 *  * Qualifier 0 is added to LSB.
 *  * Following qualifiers follow in the direction of MSB.
 *      * We have 2 user defined qualifiers which are of
 *        type bcmFieldInputTypeLayerAbsolute(means that we are
 *        taking the key from the header)
 *      * The offset and the input arg are 0 in the two qualifiers
 *        (means that we are taking 32 bits from the beginning of the header)
 *
 * THE KEY should look like this
 *          _________________________
 * field    [       Qualifier 0      ]
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary   [000100011001011010011000]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa     [      0x1196c698        ]
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 *
 *
 * The condition MSB is 19 (means that we are taking the bits (19,18,17,16)
 *  for condition ID from the key) so the value of condition ID is:
 *
 *                      ___________________
 * condition ID  binary [      0110       ]
 *                      ~~~~~~~~~~~~~~~~~~~
 *                      ___________________
 * condition ID  hexa   [      0x6        ]
 *                      ~~~~~~~~~~~~~~~~~~~
 *                       ___________________
 * condition ID decimal [        6        ]
 *                       ~~~~~~~~~~~~~~~~~~~
 *
 *
 * The expected action value(The output bits are 21 bits taken from the key):
 *         _________________________
 * field   [       Output bits      ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary  [000101101100011010011000]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [         0x16c698       ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * The overriding FG configuration:
 *  It is a TCAM Fg with 1 qualifier (bcmFieldQualifySrcPort) with value "0xc8"
 *  The SrcPort is 16 bits. The value depends on the contents of the packet.
 *  and 1 action (bcmFieldActionForward) with value "0x11111"
 *
 * The key is the value of the qualifier:
 *         _________________________
 * field   [       Qualifier 0      ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary  [    0000000011001000    ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [          0x00c8        ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * The result is the value of the result buffer contents:
 * NOTE THAT The actions are aligned from MSB to LSB
 * We have 3 actions:
 *       1. User defined action void (size 28 bits) value 0's
 *       2. bcmFieldActionIngressTimeStampInsert (size 32 bits + 1 valid_bit) and value 0x11111111
 *       3. bcmFieldActionDropPrecedence (size 2bits + 1 valid_bit) and value 0x2
 *         _________________________
 * field   [      Result buffer    ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [   0x00000001111111111d ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * The 16 LSB which are going to be override:
 *
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary  [    0001000100011101    ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [          0x111d         ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * The result after override (the 16 LSB bits must be taken from the TCAM result)
 *
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary [ 000101100001000100010001 ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [        0x16111d         ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * \param [in] unit - The unit number.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_fem_simple_case_DE_main(
    int unit)
{
    char *proc_name;
    bcm_field_stage_t stage;
    cint_field_fem_fg_info_t fg_info;
    cint_field_fem_action_info_t fem_info;
    int fem_iterator;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fem_simple_case_DE_main";
    /**
     * Create a preselector which is going to qualify upon Ethernet header
     */
    printf("Create a preselector which is going to qualify upon Ethernet header \n");
    rv = cint_field_fem_context_create_presel_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_fem_context_create_presel_set \n", proc_name, rv);
        return rv;
    }
    /**
     * Config the overriding FG, including attaching to the context and adding a TCAM entry.
     */
    printf("Config the overriding FG, including attaching to the context and adding an TCAM entry. \n");
    rv = cint_field_fem_fg_config(unit, bcmFieldStageIngressPMF2, Cint_field_fem_overriding_fg_info_arr,
            Cint_field_fem_action_info_arr, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_fem_fg_config \n", proc_name, rv);
        return rv;
    }
    /** Set the value of the created overriding FG ID above in the FEM action array. */
    Cint_field_fem_action_info_arr[0].fem_input.overriding_fg_id = Cint_field_fem_overriding_fg_info_arr[0].fg_id;
    /**
     * Create DE Fg which is using FEM actions and Attach it to the context.
     */
    printf("Create DE Fg which is using FEM actions and Attach it to the context.. \n");
    rv = cint_field_fem_fg_config(unit, bcmFieldStageIngressPMF2, Cint_field_fem_DE_fg_info_arr,
            Cint_field_fem_action_info_arr, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_fem_fg_config \n", proc_name, rv);
        return rv;
    }
    return rv;
}
/**
 * SIMPLE_CASE TCAM configuration!
 *
 * The FEM FG configuration:
 *   1. We are creating 1 TCAM FG with 1 action and 1 qualifier and 1 FEM action.
 *   2. We are creating 1 TCAM FG with 1 qual and 1 action
 *         which should override first 16 bits of the result
 *
 * The value of the Qualifier that we are set in the entry qual_value
 *  is the value of the key.
 *
 * THE KEY should look like this
 *          ___________________________
 * field   [The value of Qual Dst Port]
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary   [      000100101100       ]
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa     [           0x12c         ]
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * The value of the action that we put in the entry is the actual Payload.
 *
 * THE PAYLOAD should look like this:
 *
 *          _________________________
 * field    [The value of void action]
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary   [000100011001011010011000]
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa     [       0x1196c698        ]
 *          ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 *
 *
 * The condition MSB is 19 (means that we are taking the bits (19,18,17,16)
 *  for condition ID from the payload) so the value of condition ID is:
 *
 *                      ___________________
 * condition ID  binary [      0110       ]
 *                      ~~~~~~~~~~~~~~~~~~~
 *                      ___________________
 * condition ID  hexa   [        0x6       ]
 *                      ~~~~~~~~~~~~~~~~~~~
 *                       ___________________
 * condition ID decimal [        6        ]
 *                       ~~~~~~~~~~~~~~~~~~~
 *
 *
 * The expected action value(The output bits are 21 bits taken from the key):
 *         _________________________
 * field   [       Output bits      ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary  [000101101100011010011000]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [        0x16c698         ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * The overriding FG configuration:
 *  It is a TCAM Fg with 1 qualifier (bcmFieldQualifySrcPort) with value "0xc8"
 *
 * The key is the value of the qualifier:
 *         _________________________
 * field   [       Qualifier 0      ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary  [    0000000011001000    ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [         0x00c8          ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * The result is the value of the result buffer contents:
 * NOTE THAT The actions are aligned from MSB to LSB
 * We have 3 actions:
 *       1. User defined action void (size 28 bits) value 0's
 *       2. bcmFieldActionIngressTimeStampInsert (size 32 bits + 1 valid_bit) and value 0x11111111
 *       3. bcmFieldActionDropPrecedence (size 2bits + 1 valid_bit) and value 0x2
 *         _________________________
 * field   [      Result buffer    ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [  0x00000001111111111d  ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * The 16 LSB which are going to be override:
 *
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary  [    0001000100011101    ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [          0x111d         ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * The result after override (the 16 LSB bits must be taken from the TCAM result)
 *
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary [ 000101100001000100010001 ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa    [        0x16111d         ]
 *         ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                          LSB
 *
 * \param [in] unit - The unit number.=
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_fem_simple_case_TCAM_main(
    int unit)
{
    char *proc_name;
    bcm_field_stage_t stage;
    cint_field_fem_fg_info_t fg_info;
    cint_field_fem_action_info_t fem_info;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fem_simple_case_TCAM_main";
    /**
     * Create a preselector which is going to qualify upon Ethernet header
     */
    printf("Create a preselector which is going to qualify upon Ethernet header \n");
    rv = cint_field_fem_context_create_presel_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_fem_context_create_presel_set \n", proc_name, rv);
        return rv;
    }
    /**
    * Config the overriding FG, including attaching to the context and adding a TCAM entry.
    */
    printf("Config the overriding FG, including attaching to the context and adding an TCAM entry. \n");
    rv = cint_field_fem_fg_config(unit, bcmFieldStageIngressPMF1, Cint_field_fem_overriding_fg_info_arr,
               Cint_field_fem_action_info_arr, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_fem_fg_config \n", proc_name, rv);
        return rv;
    }
    /** Set the value of the created overriding FG ID above in the FEM action array. */
    Cint_field_fem_action_info_arr[0].fem_input.overriding_fg_id = Cint_field_fem_overriding_fg_info_arr[0].fg_id;
    /**
     * Create DE Fg which is using FEM actions and Attach it to the context.
     */
    printf("Create DE Fg which is using FEM actions and Attach it to the context.. \n");
    rv = cint_field_fem_fg_config(unit, bcmFieldStageIngressPMF2, Cint_field_fem_TCAM_fg_info_arr,
            Cint_field_fem_action_info_arr, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_fem_fg_config \n", proc_name, rv);
        return rv;
    }
    return rv;
}


/** Destroys all allocated data from the FEM configuration.
 *
 * \param [in] unit - The unit number.
 * \param [in] fg_type - Shows the type of the FG, if it is DE or TCAM.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_fem_test_config_destroy(
    int unit,
    int fg_type)
{
    int qual_index, action_index, fem_index;
    bcm_field_presel_entry_data_t presel_entry_data;
    cint_field_fem_fg_info_t * fg_info_p;
    int rv;
    int fem_was_removed;

    rv = BCM_E_NONE;
    /**
     * If the FG type is DE, set the FG array to be the DE array,
     * else set the FG array to be the TCAM array
     */
    if(fg_type == FG_type_TCAM)
    {
        fg_info_p = Cint_field_fem_TCAM_fg_info_arr;
    }
    else
    {
        fg_info_p = Cint_field_fem_DE_fg_info_arr;
    }
    /**
     * Init and reset the presel
     */
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }
    /**
     * Detach the context
     */
    rv = bcm_field_group_context_detach(unit, fg_info_p->fg_id,
                                        Cint_field_fem_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    /**
     * Iterate across all of the FEMs actions
     */
    for (fem_index = 0; fem_index < Cint_field_fem_action_info_arr[0].nof_fem_actions; fem_index++)
    {
        fem_was_removed = 0;
        /**
         * If we have override, detach the context and delete the FG
         */
        if (Cint_field_fem_action_info_arr[fem_index].fem_input.overriding_fg_id != Cint_field_fem_invalid_fg_id)
        {
           /**
            * Delete the created entry.
            */
            rv = bcm_field_entry_delete(unit, Cint_field_fem_action_info_arr[fem_index].fem_input.overriding_fg_id, NULL,
                                        Cint_field_fem_overriding_fg_info_arr[0].entry_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_entry_delete\n", rv);
                return rv;
            }
            /**
             * Detach the context
             */
            rv = bcm_field_group_context_detach(unit, Cint_field_fem_action_info_arr[fem_index].fem_input.overriding_fg_id,
                                                                                           Cint_field_fem_context_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_field_group_context_detach\n");
                return rv;
            }
            if (!fem_was_removed)
            {
                /**
                 * Delete the FEM action. Should be done before deleting an active 'overriding filed group'
                 */
                bcm_field_action_priority_t action_priority;
                int array_id, fem_position_in_array;

                array_id = Cint_field_fem_action_info_arr[fem_index].field_array_n_position.array_id;
                fem_position_in_array = Cint_field_fem_action_info_arr[fem_index].field_array_n_position.fem_position_in_array;
                action_priority = BCM_FIELD_ACTION_POSITION(array_id,fem_position_in_array);
                rv = bcm_field_fem_action_delete(unit, fg_info_p->fg_id,action_priority);
                if (rv != BCM_E_NONE)
                {
                    printf("Error (%d), in bcm_field_fem_action_delete. \n", rv);
                    return rv;
                }
                fem_was_removed = 1;
            }
            /**
             * Delete the FG
             */
            rv = bcm_field_group_delete(unit, Cint_field_fem_action_info_arr[fem_index].fem_input.overriding_fg_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_field_group_delete\n");
                return rv;
            }
        }
        if (!fem_was_removed)
        {
            /**
             * If it has not already been deleted, delete the FEM action
             */
            bcm_field_action_priority_t action_priority;
            int array_id, fem_position_in_array;

            array_id = Cint_field_fem_action_info_arr[fem_index].field_array_n_position.array_id;
            fem_position_in_array = Cint_field_fem_action_info_arr[fem_index].field_array_n_position.fem_position_in_array;

            action_priority = BCM_FIELD_ACTION_POSITION(array_id,fem_position_in_array);
            rv = bcm_field_fem_action_delete(unit, fg_info_p->fg_id,action_priority);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_fem_action_delete. \n", rv);
                return rv;
            }
        }
    }
    if(fg_type == FG_type_TCAM)
    {
        /**
         * Delete the created entry.
         */
        rv = bcm_field_entry_delete(unit, fg_info_p->fg_id, NULL, fg_info_p->entry_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_delete\n", rv);
            return rv;
        }
    }
    /**
     * Delete the FEM FG
     */
    rv = bcm_field_group_delete(unit, fg_info_p->fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    /**
     * Destroy the context
     */
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_fem_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_destroy\n");
        return rv;
    }
    /**
     * Iterate between all of the qualifiers
     */
    for (qual_index = 0; qual_index < fg_info_p->nof_quals; qual_index++)
    {
        /**
         * If we have user defined qual destroy it.
         */
        if (fg_info_p->qual_info[qual_index].name != NULL)
        {
            /**
             * Destroy the user defined qualifier
             */
            rv = bcm_field_qualifier_destroy(unit, fg_info_p->qual_info[qual_index].qual_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_field_qualifier_destroy \n");
                return rv;
            }
        }
    }
    /**
     * Iterate between all of the actions
     */
    for (action_index = 0; action_index < fg_info_p->nof_actions; action_index++)
    {
        /**
         * If we have user defined actions - destoy them.
         */
        if (fg_info_p->action_info[action_index].name != NULL)
        {
            /**
             * Destroy the actions
             */
            rv = bcm_field_action_destroy(unit, fg_info_p->action_info[action_index].action_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_field_action_destroy dest_action\n");
                return rv;
            }
        }
    }
    return rv;
}

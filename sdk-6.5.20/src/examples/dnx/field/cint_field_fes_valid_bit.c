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
 * cint ../../../src/examples/dnx/field/cint_field_fes_valid_bit.c
 * cint;
 * int unit = 0;
 * cint_field_fes_valid_bit_main(unit);
 *
 * Configuration example end
 *
 * ************************************************************************
 *
 *
 *
 * TCL test for traffic test based on this cint:
 *
 *
 *   cd ../../../regress/bcm;
 *   tcl;
 *   source tests/testDvapiArad.tcl;
 *   Logging::LevelSet 10;
 *   NTestSeedSet 123456789;
 *
 *   For running the test use:
 *   test run ResetOnFail::AT_Dnx_Cint_field_fes_valid_bit
 */

/**
 * THE CINT CONFIGURATION.
 *
 * The key is created from the bits of the ETH header,
 * because the input_arg is equal to 0.
 * We are using DE field group thats why:
 * The position in the key depends on Qualifiers sequence.
 *  * Qualifier 0 is added to LSB.
 *  * Following qualifiers follow in the direction of MSB.
 *      * We have 3 user defined qualifiers which are of
 *        type bcmFieldInputTypeLayerAbsolute(means that we are
 *        taking the key from the header)
 *      * First qualifier is with size 33 bits with input arg 0 and offset 0
 *      * Second qualifier is with size 32 bits with input arg 0 and offset 0
 *      * Third qualifier is with size 3 bits with input arg 0 and offset 20
 *
 * We are adding 3 different actions (2 with active valid_bit and 1 with disabled valid_bit
 *       * First Action is bcmFieldActionUDHData0 with valid bit TRUE
 *       * Second Action is bcmFieldActionIngressTimeStampInsert with valid_bit FALSE
 *       * Third Action is bcmFieldActionDropPrecedence with valid_bit TRUE
 *
 ** In the traffic test we are sending 2 packets:
 *    1. Packet is setting the valid bit to 1.
 *    2. Second packet is setting the valid bit to 0.
 *
 *
 * In the first case we are setting the valid bits to be equal to 1
 * and we are expecting all of the actions to be performed.
 *
 * THE HEADER OF THE FIRST PACKET:
 *      _____________________  _______________________
 * field [        ETH.DA       ][          ETH.SA       ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [  11:96:c6:98:F0:01  ][    00:00:00:00:00:01  ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~
 *        ______________________________________________________
 * field [               ETH.DA in binary                       ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [ 00010001:10010110:11000110:10011000:11110000:00000001]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 * THE KEY should look like this
 *        __________________________________  _________________________________   ________________________
 * field  [       Qualifier_3               ] [       Qualifier_2              ]  [       Qualifier_1     ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary [000100011001011011000110100110001] [00010001100101101100011010011000]  [          011          ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa   [                                      0x1196c69808cb634c3                                      ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *              MSB                                                                                     LSB
 *
 * The value which we are expecting for each action is:
 *       * First action must be with value 01
 *       Because it is taking the value from the LSB of the key.
 *       The first bit is the valid bit and the second 2 bits are the actual value.
 *       * Second action must be with value 1196c698
 *       Because its valid bit is being set to FALSE.
 *       So it will appear no matter what kind of packet we are sending.
 *       * Third action must be with value 1196c698
 *       Because it is taking the value from the LSB of the key.
 *       The first bit is the valid bit and the following 32 bits are the actual value.
 *
 *
 *
 * In the second case we are setting the valid bits to be equal to 0
 * and we are expecting all of the actions to NOT be performed.
 *
 * THE HEADER OF THE SECOND PACKET:
 *        _____________________  _______________________
 * field [        ETH.DA       ][          ETH.SA       ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [  11:96:c4:98:00:01  ][    00:00:00:00:00:01  ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~
 *        ______________________________________________________
 * field [               ETH.DA in binary                       ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [ 00010001:10010110:11000100:10011000:00000000:00000001]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 * THE KEY should look like this
 *        __________________________________  _________________________________   ________________________
 * field  [       Qualifier_3               ] [       Qualifier_2              ]  [       Qualifier_1     ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~
 * binary [000100011001011011000110100110000] [00010001100101101100011010011000]  [          010          ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * hexa   [                                      0x1196c49808cb624c2                                      ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *              MSB                                                                                 LSB
 *
 *
 * The value which we are expecting for each action is:
 *       * First action valid bit is set to TRUE. It takes the valid_bit value from the input.
 *       The valid bit value is the LSB. In this case it is set to 0, so the action mustn't be performed.
 *       The expected value of the action must be 0.
 *       * Second action must be with value 1196c498
 *       Because its valid bit is being set to FALSE.
 *       So it will appear no matter what kind of packet we are sending.
 *       * Third action valid bit is set to TRUE. It takes the valid_bit value from the input.
 *       The valid bit value is the LSB. In this case it is set to 0, so the action mustn't be performed.
 *       The expected value of the action must be 0.
 */

 /**
  * Global variables
  * {
  */
/**
 * The ID of the context
 */
bcm_field_context_t Cint_field_fes_valid_bit_context_id = 9;
/**
 * The number of FGs
 */
int Cint_field_fes_valid_bit_nof_fg = 1;
/**
 * The ID of the FG
 */
int Cint_field_fes_valid_bit_fg_id = 0;
/**
 * The ID of the preselector
 */
bcm_field_presel_t Cint_field_fes_valid_bit_presel_id = 10;
/**
 * The number of quals for preselection
 */
int Cint_field_fes_valid_bit_presel_nof_quals = 1;
/**
 * The value of the preselector qualifier
 */
uint32 Cint_field_fes_valid_bit_presel_qual_value = 0x1;
/**
 * The mask of the preselector qualifier
 */
uint32 Cint_field_fes_valid_bit_presel_qual_mask = 0x1;
/**
 * The number of Actions per FG used in the test
 */
int Cint_field_fes_valid_bit_nof_actions = 3;
/**
 * The number of Quals per FG used in the test
 */
int Cint_field_fes_valid_bit_nof_quals = 3;
/**
 * The presel_entry. Used for creating and destroying the presel
 */
bcm_field_presel_entry_id_t presel_entry_id;
/**
 * Structure, which contains information,
 * for creating of user quals and predefined one.
 */
struct cint_field_fes_valid_bit_qual_info_t {
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
struct cint_field_fes_valid_bit_actions_info_t {
    /**
     * Action valid bit
     */
    int action_valid_bit;
    /**
     * The type for this action.
     */
    bcm_field_action_t action_type;
};
/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
struct cint_field_fes_valid_bit_fg_info_t {
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
    cint_field_fes_valid_bit_qual_info_t * qual_info;
    /**
     * The attach info for actions
     */
    cint_field_fes_valid_bit_actions_info_t * action_info;
};
/* *INDENT-OFF* */
/**
 * This array will be used for creating User Defined Qualifiers.
 * We are calling it in Cint_field_fes_valid_bit_de_fg_info.
 */
cint_field_fes_valid_bit_qual_info_t Cint_field_fes_valid_bit_user_quals_arr[Cint_field_fes_valid_bit_nof_quals] = {
/** qual_name */  /* size */  /* {input type */                /* input arg */    /* offset} */
  {"DA[22:20]",      3,    {bcmFieldInputTypeLayerAbsolute,      0,                 20}},
  {"DA[31:0]",      32,    {bcmFieldInputTypeLayerAbsolute,      0,                 0}},
  {"DA[32:0]",      33,    {bcmFieldInputTypeLayerAbsolute,      0,                 0}}
};
/**
 * This array will be used for creating a TCAM Fes valid bit FG.
 * We are calling it in Cint_field_fes_valid_bit_de_fg_info.
 */
cint_field_fes_valid_bit_actions_info_t Cint_field_fes_valid_bit_action_arr[Cint_field_fes_valid_bit_nof_actions] = {

  /** valid bit */  /** The action type */
  {TRUE,           bcmFieldActionDropPrecedence},
  /** valid bit */  /** The action type */
  {FALSE,          bcmFieldActionIngressTimeStampInsert},
  /** valid bit */  /** The action type */
  {TRUE,           bcmFieldActionUDHData0}

};
/**
 * This array will be used for creating a DE FG.
 */
cint_field_fes_valid_bit_fg_info_t Cint_field_fes_valid_bit_de_fg_info = {
        /** FG ID */
        Cint_field_fes_valid_bit_fg_id,
        /** FG_type */
        bcmFieldGroupTypeDirectExtraction,
        /** nof_quals */
        Cint_field_fes_valid_bit_nof_quals,
        /** nof_actions */
        Cint_field_fes_valid_bit_nof_actions,
        /** qual_info */
        Cint_field_fes_valid_bit_user_quals_arr,
        /** action_info */
        Cint_field_fes_valid_bit_action_arr
};

/**
 * Global variables
 * }
 */
/* *INDENT-ON* */

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
cint_field_fes_valid_bit_context_create_presel_set(
    int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_info_t context_info;
    char *proc_name;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fes_valid_bit_context_create_presel_set";
   /**
    * Init the context_info
    */
    bcm_field_context_info_t_init(&context_info);
    /**
     * Create the context with ID
     */
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Fes_valid_bit_context", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &Cint_field_fes_valid_bit_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_context_create\n", proc_name, rv);
        return rv;
    }
   /**
    * Fill the info in presel_entry_data and bcm_field_presel_entry_id_t structures
    */
    presel_entry_id.presel_id = Cint_field_fes_valid_bit_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;
    presel_entry_data.context_id = Cint_field_fes_valid_bit_context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = Cint_field_fes_valid_bit_presel_nof_quals;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = Cint_field_fes_valid_bit_presel_qual_value;
    presel_entry_data.qual_data[0].qual_mask = Cint_field_fes_valid_bit_presel_qual_mask;
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
 * It is used later in the cint_field_fes_valid_bit_fg_config.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - The PMF stage.
 * \param [in] qual_info_p - Pointer to Structure which contains
 *      all needed information for creating a qualifier.
 */
int
cint_field_fes_valid_bit_user_qual_config(
    int unit,
    bcm_field_stage_t field_stage,
    cint_field_fes_valid_bit_qual_info_t * qual_info_p)
{
    bcm_field_qualifier_info_create_t qual_info;
    char * qual_name;
    void * tmp_p;
    int rv;
    char *proc_name;
    void *dest_char;

    proc_name = "cint_field_fes_valid_bit_user_qual_config";

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
 * \brief
 *  This function sets all required HW
 *  configuration for bcm_field_group_add and bcm_field_group_context_attach
 *  to be performed for IPMF2.
 *
 * \param [in] unit - The unit number.
 * \param [in] stage - The PMF stage.
 * \param [in] fes_fg_info_p - Pointer to Structure which contains
 *      all needed information for creating a FG.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t
 */
int static
cint_field_fes_valid_bit_fg_config(
    int unit,
    bcm_field_stage_t stage,
    cint_field_fes_valid_bit_fg_info_t * fes_fg_info_p)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    int qual_index, action_index;
    char *proc_name;
    void *dest_char;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fes_valid_bit_fg_config";
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
    fg_info.fg_type = fes_fg_info_p->fg_type;
    fg_info.nof_quals = fes_fg_info_p->nof_quals;
    fg_info.nof_actions = fes_fg_info_p->nof_actions;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Fes_valid_bit_FG", sizeof(fg_info.name));
    /**
     * Iterate between all of the qualifiers and add them in the fg_info structure.
     */
    for (qual_index = 0; qual_index < fes_fg_info_p->nof_quals; qual_index++)
    {
        /**
         * Check if the qualifier is User defined and if it is create the qual using
         * cint_field_fes_valid_bit_user_qual_config function
         */
        if (fes_fg_info_p->qual_info[qual_index].name != NULL)
        {
            rv = cint_field_fes_valid_bit_user_qual_config(unit, fg_info.stage, &(fes_fg_info_p->qual_info[qual_index]));
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in cint_field_fes_valid_bit_user_qual_config. \n", rv);
                return rv;
            }
        }
        fg_info.qual_types[qual_index] = fes_fg_info_p->qual_info[qual_index].qual_id;
    }
    /**
     * Iterate between all of the actions and add them in the fg_info structure.
     */
    for (action_index = 0; action_index < fes_fg_info_p->nof_actions; action_index++)
    {
        fg_info.action_types[action_index] = fes_fg_info_p->action_info[action_index].action_type;
        /**
         * Here we take action's valid bit from the input
         */
        fg_info.action_with_valid_bit[action_index] = fes_fg_info_p->action_info[action_index].action_valid_bit;
    }
    /**
     * Add the FG.
     */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fes_fg_info_p->fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }
    /**
     * Fill the attach info structure
     */
    attach_info.key_info.nof_quals = fes_fg_info_p->nof_quals;
    attach_info.payload_info.nof_actions = fes_fg_info_p->nof_actions;
    /**
     * Iterate between all of the qualifiers and add them in the attach_info structure.
     */
    for (qual_index = 0; qual_index < fes_fg_info_p->nof_quals; qual_index++)
    {
        attach_info.key_info.qual_types[qual_index] = fes_fg_info_p->qual_info[qual_index].qual_id;
        attach_info.key_info.qual_info[qual_index].input_type = fes_fg_info_p->qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg = fes_fg_info_p->qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset = fes_fg_info_p->qual_info[qual_index].qual_attach_info.offset;
    }
    /**
     * Iterate between all of the actions and add them in the fg_info structure.
     */
    for (action_index = 0; action_index < fes_fg_info_p->nof_actions; action_index++)
    {
        attach_info.payload_info.action_types[action_index] =  fes_fg_info_p->action_info[action_index].action_type;
    }
    /**
     * Attach the FG to the context.
     */
    rv = bcm_field_group_context_attach(unit, 0, fes_fg_info_p->fg_id, Cint_field_fes_valid_bit_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in bcm_field_group_context_attach\n", proc_name, rv);
        return rv;
    }
return rv;
}
/**
 * This function is doing a configuration for the fes valid bit.
 * (Look into the beginning of the file for more info!)
 * \param [in] unit - The unit number.
 * \param [in] stage_iterator - This iterator is set in the TCL test.
 *                                   It is used for iterating between the
 *                                   two stages (IPMF2 and IPMF3).
 * \param [in] dir_ext_fg_iterator - This iterator is set in the TCL test.
 *                                   It is used for iterating between all of the
 *                                   FG that needs to be created.
 *  \return
 *  int - Error Type, in sense of bcm_error_t.
 */
int
cint_field_fes_valid_bit_main(
    int unit)
{
    char *proc_name;

    int rv = BCM_E_NONE;
    proc_name = "cint_field_fes_valid_bit_main";
    /**
     * Create a preselector which is going to qualify upon Ethernet header
     */
    printf("Create a preselector which is going to qualify upon Ethernet header \n");
    rv = cint_field_fes_valid_bit_context_create_presel_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_fes_valid_bit_context_create_presel_set \n", proc_name, rv);
        return rv;
    }
    /**
     * Create DE Fg and Attach it to the context.
     */
    printf("Create DE Fg and Attach it to the context. \n");
    rv = cint_field_fes_valid_bit_fg_config(unit, bcmFieldStageIngressPMF2, &Cint_field_fes_valid_bit_de_fg_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error (%d), in cint_field_fes_valid_bit_fg_config \n", proc_name, rv);
        return rv;
    }
    return rv;
}

/** Destroys all allocated data from the FES valid bit configuration. */
int
cint_field_fes_valid_bit_destroy(
    int unit)
{
    int qual_index;
    bcm_field_presel_entry_data_t presel_entry_data;

    int rv = BCM_E_NONE;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, Cint_field_fes_valid_bit_de_fg_info.fg_id,
                                        Cint_field_fes_valid_bit_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }
    rv = bcm_field_group_delete(unit, Cint_field_fes_valid_bit_de_fg_info.fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, Cint_field_fes_valid_bit_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_context_destroy\n");
        return rv;
    }

    for (qual_index = 0; qual_index < Cint_field_fes_valid_bit_de_fg_info.nof_quals; qual_index++)
    {
        rv = bcm_field_qualifier_destroy(unit, Cint_field_fes_valid_bit_de_fg_info.qual_info[qual_index].qual_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_field_qualifier_destroy \n");
            return rv;
        }
    }
    return rv;
}

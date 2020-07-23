/** \file itmh_appl_init.h
 * ITMH Programmable mode appl
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
* Include files.
* {
*/
#include <appl/diag/parse.h>
/*
 * }
 */

#ifndef APPL_REF_DNX_ITMH_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_DNX_ITMH_INIT_H_INCLUDED

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Number of Actions (used as array size) */
#define ITMH_NOF_IPMF2_ACTIONS           12
/** Number of Data Qualifiers needed (used as array size) */
#define ITMH_NOF_IPMF2_QUALIFIERS        10
/** Number of FEMs in IPMF2 (used as array size) */
#define ITMH_NOF_IPMF2_FEMS              3
/** Number of Field Groups needed (used as array size) */
#define ITMH_NOF_FG                      1
/** ID of field group, which is going to be created in IPMF2 stage */
#define ITMH_IPMF2_FG_ID                 10


/** Offset of PPH Qualifier for ITMH Header[6:5] (2b) */
#define PPH_TYPE_QUAL_OFFSET          5
/**
 * Offset of DP Qualifier for ITMH Header[9:8] (2b)
 * Offset of SNOOP_DATA Qualifier for ITMH Header[35:31] (5b)
 * Offset of TC Qualifier for ITMH Header[38:36] (3b)
 * Offset of DST Qualifier for ITMH Header[30:10] (21b)
 */
#define  DP_DST_SNOOP_TC_QUAL_OFFSET  8
/** Offset of EXTENSION PRESENT qualifier in ITMH header [39:39] (1b) */
#define  EXT_PRESENT_QUAL_OFFSET  39
/** Offset of OUT_LIF Qualifier for ITMH-Ext Header[23:2] (2b) */
#define  OUT_LIF_QUAL_OFFSET    42
/** offset of ASE_INJ Qualifier for ITMH Header[4:4] (1b) */
#define ASE_INJ_QUAL_OFFSET           4
/** Offset of MIRROR Qualifier for ITMH Header[7:7] (1b) */
#define MIRROR_QUAL_OFFSET            7
/** Offset of TRAP_CODE Qualifier, used to fill the trap code with constant value (9b) */
#define TRAP_CODE_ZEROS_QUAL_OFFSET    0
/**
 *  Offset of FWD_STRENGTH Qualifier (4b).
 */
#define ITMH_FWD_STR_QUAL_OFFSET   0
/** Offset of OAM_VALID_BIT Qualifier for ITMH Header[1:1] (1b) */
#define J2_ITMH_OAM_VALID_BIT_QUAL_OFFSET      1

/** Length of PPH Qualifier for ITMH Header[6:5] (2b) */
#define PPH_TYPE_QUAL_LENGTH          2
/**
 * Length of DP Qualifier for ITMH Header[9:8] (2b)
 * Length of SNOOP_DATA Qualifier for ITMH Header[35:31] (5b)
 * Length of TC Qualifier for ITMH Header[38:36] (3b)
 * Length of DST Qualifier for ITMH Header[30:10] (21b)
 */
#define  DP_DST_SNOOP_TC_QUAL_LENGTH  31
/** Length of EXTENSION PRESENT qualifier in ITMH header [39:39] (1b) */
#define  EXT_PRESENT_QUAL_LENGTH  1
/** Length of OUT_LIF Qualifier for ITMH-Ext Header[23:2] (22b) */
#define  OUT_LIF_QUAL_LENGTH    22
/** Length of ASE_INJ Qualifier for ITMH Header[4:4] (1b) */
#define ASE_INJ_QUAL_LENGTH           1
/** Length of MIRROR Qualifier for ITMH Header[7:7] (1b) */
#define MIRROR_QUAL_LENGTH            1
/** Length of TRAP_CODE Qualifier, used to fill the trap code with constant value (9b) */
#define TRAP_CODE_ZEROS_QUAL_LENGTH    9
/** Length of FWD_STRENGTH Qualifier (4b) */
#define ITMH_FWD_STR_QUAL_LENGTH   4
/** Length of OAM_VALID_BIT Qualifier (1b) */
#define J2_ITMH_OAM_VALID_BIT_QUAL_LENGTH   1
/** Length of FWD_LAYER_INDEX Qualifier Constant value of 0x2 (3b) */
#define J2_ITMH_FWD_LAYER_INDEX_QUAL_LENGTH 3


/** Length of user defined SNOOP_DATA action (5b) */
#define  SNOOP_DATA_ACTION_LENGTH         5
/** Length of user defined DESTINATION action (21b) */
#define  DST_ACTION_LENGTH                21
/** Length of user defined EXT_EXIST action (1b) */
#define  EXT_EXIST_ACTION_LENGTH          1
/** Length of user defined OUT_LIF action (21b) */
#define  OUT_LIF_ACTION_LENGTH            22
/** Length of user defined ASE_INJ action (1b) */
#define  ASE_INJ_ACTION_LENGTH            1
/** Length of user defined MIRROR action (1b) */
#define  MIRROR_ACTION_LENGTH             1
/** Length of user defined FWD_STRENGTH action (4b) */
#define ITMH_FWD_STR_ACTION_LENGTH           4

/**
 * Structure, which contains information,
 * for creating of user actions and predefined one.
 */
typedef struct
{
    char * name;
    bcm_field_qualify_t qual_id;
    uint32 qual_length;
    bcm_field_qualify_attach_info_t qual_attach_info;

} field_itmh_qual_info_t;

/**
 * Structure, which contains need information,
 * for creating of user actions and predefined one.
 */
typedef struct {
    char * name;
    bcm_field_action_t action_id;
    bcm_field_action_info_t action_info;
    uint8 action_with_valid_bit;

} field_itmh_action_info_t;

/**
 * Structure, which contains need information
 * for configuring of FEM condition actions.
 */
typedef struct {
    int nof_conditions;
    int condition_ids[16];
    int nof_extractions;
    int nof_mapping_bits;
/*    bcm_field_fem_id_t fem_id; #### REMOVE */
    /*
     * Structure replacing FEM_ID to indicate selected FEM by
     * its priority in the general FES/FEM structure.
     * See DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET
     */
    bcm_field_array_n_position_t field_array_n_position;
    bcm_field_fem_action_info_t fem_action_info;

} field_itmh_fem_action_info_t;

/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
typedef struct
{
    bcm_field_group_t fg_id;
    bcm_field_stage_t stage;
    bcm_field_group_type_t fg_type;
    int nof_quals;
    int nof_actions;
    field_itmh_qual_info_t * itmh_qual_info;
    field_itmh_action_info_t * itmh_action_info;
    field_itmh_fem_action_info_t * itmh_fem_action_info;

} field_itmh_fg_info_t;


/*
 * }
 */


/**
 * \brief - This function initialize ITMH application:
 *          1. Create User-Defined qualifiers for ITMH and ITMH-extension
 *          2. Create field groups to parse the ITMH header / extension and update signals
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_itmh_init(
    int unit);

/*
 * }
 */
#endif /* ITMH_APPL_INIT_H_INCLUDED */

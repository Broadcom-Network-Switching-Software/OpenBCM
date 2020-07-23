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

#ifndef APPL_REF_FIELD_J1_ITMH_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_FIELD_J1_ITMH_INIT_H_INCLUDED

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Number of Actions (used as array size) */
#define J1_ITMH_NOF_IPMF2_ACTIONS           10
/** Number of Data Qualifiers needed (used as array size) */
#define J1_ITMH_NOF_IPMF2_QUALIFIERS        10
/** Number of FEMs in IPMF2 (used as array size) */
#define J1_ITMH_NOF_IPMF2_FEMS              3
/** Number of Field Groups needed (used as array size) */
#define J1_ITMH_NOF_FG                      1
/** ID of field group, which is going to be created in IPMF2 stage */
#define J1_ITMH_IPMF2_FG_ID                 14

/** Offset of PPH Qualifier for ITMH Header[1:0] (2b) */
#define J1_ITMH_PPH_TYPE_QUAL_OFFSET          0
/**
 * Offset of DP Qualifier for ITMH Header[9:8] (2b)
 * Offset of SNOOP_DATA Qualifier for ITMH Header[35:31] (5b)
 * Offset of TC Qualifier for ITMH Header[38:36] (3b)
 * Offset of DST Qualifier for ITMH Header[30:10] (21b)
 */
#define  J1_ITMH_DP_DST_SNOOP_TC_QUAL_OFFSET  3
/** Offset of EXTENSION PRESENT qualifier in ITMH header [31:31] (1b) */
#define  J1_ITMH_EXT_PRESENT_QUAL_OFFSET  31
/** Offset of OUT_LIF Qualifier for ITMH-Ext Header[4:23] (20b) */
#define  J1_ITMH_OUT_LIF_QUAL_OFFSET    36
/** Offset of MIRROR Qualifier for ITMH Header[2:2] (1b) */
#define J1_ITMH_MIRROR_QUAL_OFFSET            2
/** Offset of FWD_STRENGTH Qualifier (4b). */
#define J1_ITMH_FWD_STR_QUAL_OFFSET   0

/** Length of PPH Qualifier for ITMH Header[6:5] (2b) */
#define J1_ITMH_PPH_TYPE_QUAL_LENGTH          2
/**
 * Length of DP Qualifier for ITMH Header[9:8] (2b)
 * Length of SNOOP_DATA Qualifier for ITMH Header[35:31] (5b)
 * Length of TC Qualifier for ITMH Header[38:36] (3b)
 * Length of DST Qualifier for ITMH Header[30:10] (21b)
 */
#define  J1_ITMH_DP_DST_SNOOP_TC_QUAL_LENGTH 28
/** Length of EXTENSION PRESENT qualifier in ITMH header [39:39] (1b) */
#define  J1_ITMH_EXT_PRESENT_QUAL_LENGTH  1
/** Length of OUT_LIF Qualifier for ITMH-Ext Header[23:2] (22b) */
#define  J1_ITMH_OUT_LIF_QUAL_LENGTH    20
/** Length of MIRROR Qualifier for ITMH Header[7:7] (1b) */
#define J1_ITMH_MIRROR_QUAL_LENGTH            1
/** Length of FWD_LAYER_INDEX Qualifier Constant value of 0x1 (3b) */
#define J1_ITMH_FWD_LAYER_INDEX_QUAL_LENGTH 3

/** Length of user defined SNOOP_DATA action (4b) */
#define  J1_ITMH_SNOOP_DATA_ACTION_LENGTH         4
/** Length of user defined DESTINATION action (19b) */
#define  J1_ITMH_DST_ACTION_LENGTH                19
/** Length of user defined EXT_EXIST action (1b) */
#define  J1_ITMH_EXT_EXIST_ACTION_LENGTH          1
/** Length of user defined OUT_LIF action (21b) */
#define  J1_ITMH_OUT_LIF_ACTION_LENGTH            20
/** Length of user defined ASE_INJ action (1b) */
#define  J1_ITMH_ASE_INJ_ACTION_LENGTH            1
/** Length of user defined MIRROR action (1b) */
#define  J1_ITMH_MIRROR_ACTION_LENGTH             1
/** Length of user defined FWD_STRENGTH action (4b) */
#define J1_ITMH_FWD_STR_ACTION_LENGTH           4

/**
 * Structure, which contains information,
 * for creating of user actions and predefined one.
 */
typedef struct
{
    char *name;
    bcm_field_qualify_t qual_id;
    uint32 qual_length;
    bcm_field_qualify_attach_info_t qual_attach_info;

} field_j1_itmh_qual_info_t;

/**
 * Structure, which contains need information,
 * for creating of user actions and predefined one.
 */
typedef struct
{
    char *name;
    bcm_field_action_t action_id;
    bcm_field_action_info_t action_info;
    uint8 action_with_valid_bit;

} field_j1_itmh_action_info_t;

/**
 * Structure, which contains need information
 * for configuring of FEM condition actions.
 */
typedef struct
{
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

} field_j1_itmh_fem_action_info_t;

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
    field_j1_itmh_qual_info_t *itmh_qual_info;
    field_j1_itmh_action_info_t *itmh_action_info;
    int nof_fems;
    field_j1_itmh_fem_action_info_t *itmh_fem_action_info;
} field_j1_itmh_fg_info_t;

/*
 * }
 */
/**
 * \brief
 *  Callback for J1 ITMH application which determines whether to run the application
 *
 * \param [in] unit - The unit number.
 *        [out] dynamic_flags - application use
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_j1_itmh_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief - This function initialize J1 mode ITMH application:
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
shr_error_e appl_dnx_field_j1_itmh_init(
    int unit);

/*
 * }
 */
#endif /* APPL_REF_FIELD_J1_ITMH_INIT_H_INCLUDED */

/** \file appl_ref_field_ftmh_mc_init.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_FIELD_FTMH_MC_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_FIELD_FTMH_MC_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define PORT_CLASS_ID 0x7

/** Number of Actions (used as array size) */
#define FTMH_MC_NOF_IPMF2_ACTIONS           5
#define FTMH_MC_NOF_IPMF3_ACTIONS           4
/** Number of Data Qualifiers needed (used as array size) */
#define FTMH_MC_NOF_IPMF2_QUALIFIERS        9
#define FTMH_MC_NOF_IPMF3_QUALIFIERS        1
/** Number of Field Groups needed (used as array size) */
#define FTMH_MC_NOF_FG                      2

/** Offset of MC ID DESTINATION in FTMH header [67:49] (19b) */
#define MC_ID_DST_QUAL_OFFSET            49
/** Length of MC ID DESTINATION in FTMH header [67:49] (19b) */
#define MC_ID_DST_QUAL_LENGTH            19
#define MC_ID_DST_CONST_QUAL_LENGTH      2
/** Length of user defined MC ID DESTINATION action (19b) */
#define MC_ID_DST_ACTION_LENGTH          21

/** Offset of LAG key qualifier in FTMH header [103:88] (16b) */
#define LAG_KEY_QUAL_OFFSET      88
/** Length of LAG key qualifier in FTMH header [103:88] (16b) */
#define LAG_KEY_QUAL_LENGTH      16
/** Length of user defined LAG key action (8b) */
#define LAG_KEY_ACTION_LENGTH    16

/** Offset of Packet_size qualifier in FTMH header [6:0] (7b) */
#define PACKET_SIZE_QUAL_OFFSET      0
/** Length of Packet_size qualifier in FTMH header [6:0] (7b) */
#define PACKET_SIZE_QUAL_LENGTH      7
/** Length of user defined TC action (3b) */
#define PACKET_SIZE_ACTION_LENGTH    7

/** Offset of TC qualifier in FTMH header [16:14] (3b) */
#define TC_QUAL_OFFSET      14
/** Length of TC qualifier in FTMH header [16:14] (3b) */
#define TC_QUAL_LENGTH      3
/** Length of user defined TC action (3b) */
#define TC_ACTION_LENGTH    3

/** Offset of DP qualifier in FTMH header [42:41] (2b) */
#define DP_QUAL_OFFSET      41
/** Length of DP qualifier in FTMH header [42:41] (2b) */
#define DP_QUAL_LENGTH      2
/** Length of user defined DP action (2b) */
#define DP_ACTION_LENGTH    2

/** Offset of AQM Profile qualifier in FTMH header [69] (1b) */
#define AQM_PROFILE_QUAL_OFFSET        69
/** Length of AQM Profile qualifier in FTMH header [69] (1b) */
#define AQM_PROFILE_QUAL_LENGTH        1
/** Length of user defined AQM Profile action (2b) */
#define AQM_PROFILE_ACTION_LENGTH      1

/** Offset of Parsing start offset qualifier in PPH header [89:83] (7b) */
#define PPH_PARSING_OFFSET_QUAL_OFFSET      83
/** Length of Parsing start offset qualifier in FTMH header [89:83] (7b) */
#define PPH_PARSING_OFFSET_QUAL_LENGTH        7
#define PPH_PARSING_OFFSET_VALID_QUAL_LENGTH  1
/** Length of user defined Parsing start offset action (8b) */
#define PPH_PARSING_OFFSET_ACTION_LENGTH    8

#define CONTAINER_ACTION_LENGTH    LAG_KEY_ACTION_LENGTH + PPH_PARSING_OFFSET_ACTION_LENGTH + PACKET_SIZE_ACTION_LENGTH

/**
 * Structure, which contains information,
 * for creating of user qualifiers and predefined one.
 */
typedef struct
{
    char *name;
    bcm_field_qualify_t qual_id;
    uint32 qual_length;
    bcm_field_qualify_attach_info_t qual_attach_info;
} field_ftmh_mc_qual_info_t;

/**
 * Structure, which contains need information,
 * for creating of user actions and predefined one.
 */
typedef struct
{
    char *name;
    bcm_field_action_t action_id;
    bcm_field_action_info_t action_info;
} field_ftmh_mc_action_info_t;

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
    field_ftmh_mc_qual_info_t *ftmh_mc_qual_info;
    field_ftmh_mc_action_info_t *ftmh_mc_action_info;
} field_ftmh_mc_fg_info_t;

/*
 * }
 */
/**
 * \brief
 * configure PMF context to parse FTMH packets for MC packets
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_field_ftmh_mc_init(
    int unit);

/**
 * \brief
 *   This function determines if the app should be skipped
 *
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_ftmh_mc_cb(
    int unit,
    int *dynamic_flags);
/*
 * }
 */
#endif /* APPL_REF_FIELD_FTMH_MC_INIT_H_INCLUDED */

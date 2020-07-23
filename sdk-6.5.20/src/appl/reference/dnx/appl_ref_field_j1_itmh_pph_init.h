
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif


#include <appl/diag/parse.h>


#ifndef APPL_REF_FIELD_J1_ITMH_PPH_INIT_H_INCLUDED

#define APPL_REF_FIELD_J1_ITMH_PPH_INIT_H_INCLUDED






#define J1_ITMH_PPH_HEADER_0_SIZE        32

#define J1_ITMH_PPH_HEADER_1_SIZE        24

#define J1_ITMH_PPH_HEADER_2_SIZE        48

#define J1_ITMH_PPH_HEADER_3_SIZE        56


#define J1_ITMH_PPH_NOF_IPMF2_ACTIONS           9

#define J1_ITMH_PPH_NOF_IPMF2_QUALIFIERS        14

#define J1_ITMH_PPH_NOF_IPMF2_FEMS              3

#define J1_ITMH_PPH_NOF_FG                      4

#define J1_ITMH_PPH_IPMF2_FG_ID                 14

#define J1_ITMH_PPH_NOF_IPMF2_FG2_ACTIONS       4
#define J1_ITMH_PPH_NOF_IPMF2_FG3_ACTIONS       2
#define J1_ITMH_PPH_NOF_IPMF3_ACTIONS           5


#define J1_ITMH_PPH_NOF_IPMF2_FG2_QUALIFIERS    8
#define J1_ITMH_PPH_NOF_IPMF2_FG3_QUALIFIERS    7

#define J1_ITMH_PPH_NOF_IPMF3_QUALIFIERS        2

#define J1_ITMH_PPH_NOF_CMP_KEY_QUALIFIERS      3


#define J1_ITMH_PPH_PPH_TYPE_QUAL_OFFSET          0

#define  J1_ITMH_PPH_DP_DST_SNOOP_TC_QUAL_OFFSET  3

#define  J1_ITMH_PPH_EXT_PRESENT_QUAL_OFFSET  31

#define  J1_ITMH_PPH_OUT_LIF_QUAL_OFFSET    36

#define J1_ITMH_PPH_ASE_INJ_QUAL_OFFSET           4

#define J1_ITMH_PPH_MIRROR_QUAL_OFFSET            2

#define J1_ITMH_PPH_TRAP_CODE_ZEROS_QUAL_OFFSET    0

#define J1_ITMH_PPH_FWD_STR_RES_BIT_QUAL_OFFSET   16
#define J1_ITMH_PPH_FWD_STR_QUAL_OFFSET           15

#define J1_ITMH_PPH_OAM_VALID_BIT_QUAL_OFFSET      0


#define J1_ITMH_PPH_PARSING_OFFSET_QUAL_OFFSET    8

#define J1_ITMH_PPH_PARSING_TYPE_QUAL_OFFSET      4

#define J1_ITMH_PPH_FHEI_SIZE_QUAL_OFFSET         2

#define J1_ITMH_PPH_FHEI_TYPE_QUAL_OFFSET         4


#define J1_ITMH_PPH_OAM_MEP_QUAL_OFFSET           5

#define J1_ITMH_PPH_OAM_SUB_TYPE_QUAL_OFFSET      2

#define J1_ITMH_PPH_OAM_OFFSET_QUAL_OFFSET        40

#define J1_ITMH_PPH_OAM_STAMP_OFFSET_QUAL_OFFSET  0

#define J1_ITMH_PPH_PARS_OFFSET_TYPE_SH_ADJUST_QUAL_OFFSET         0

#define J1_ITMH_PPH_TTL_SET_QUAL_OFFSET      (J1_ITMH_PPH_HEADER_3_SIZE + 16)

#define J1_ITMH_PPH_IN_LIF_PROFILE_QUAL_OFFSET      (J1_ITMH_PPH_HEADER_3_SIZE + 0)

#define J1_ITMH_PPH_FWD_ADDITIONAL_INFO_QUAL_OFFSET      4

#define J1_ITMH_PPH_IN_LIF_QUAL_OFFSET         38

#define J1_ITMH_PPH_FWD_DOMAIN_QUAL_OFFSET     22

#define J1_ITMH_PPH_NETWORK_QOS_QUAL_OFFSET   (J1_ITMH_PPH_HEADER_3_SIZE + 8)


#define J1_ITMH_PPH_PPH_TYPE_QUAL_LENGTH          2

#define  J1_ITMH_PPH_DP_DST_SNOOP_TC_QUAL_LENGTH 28

#define  J1_ITMH_PPH_EXT_PRESENT_QUAL_LENGTH  1

#define  J1_ITMH_PPH_OUT_LIF_QUAL_LENGTH    20

#define J1_ITMH_PPH_ASE_INJ_QUAL_LENGTH           1

#define J1_ITMH_PPH_MIRROR_QUAL_LENGTH            1

#define J1_ITMH_PPH_TRAP_CODE_ZEROS_QUAL_LENGTH    9

#define J1_ITMH_PPH_FWD_STR_RES_BIT_QUAL_LENGTH   2
#define J1_ITMH_PPH_FWD_STR_QUAL_LENGTH           1

#define J1_ITMH_PPH_OAM_VALID_BIT_QUAL_LENGTH   1


#define  J1_ITMH_PPH_SNOOP_DATA_ACTION_LENGTH         4

#define  J1_ITMH_PPH_DST_ACTION_LENGTH                19

#define  J1_ITMH_PPH_EXT_EXIST_ACTION_LENGTH          1

#define  J1_ITMH_PPH_OUT_LIF_ACTION_LENGTH            20

#define  J1_ITMH_PPH_ASE_INJ_ACTION_LENGTH            1

#define  J1_ITMH_PPH_MIRROR_ACTION_LENGTH             1

#define J1_ITMH_PPH_FWD_STR_ACTION_LENGTH           12


#define J1_ITMH_PPH_OAM_MEP_QUAL_LENGTH           1

#define J1_ITMH_PPH_OAM_SUB_TYPE_QUAL_LENGTH      3

#define J1_ITMH_PPH_OAM_OFFSET_QUAL_LENGTH        8

#define J1_ITMH_PPH_OAM_STAMP_OFFSET_QUAL_LENGTH   8


#define J1_ITMH_PPH_PARSING_OFFSET_VALID_QUAL_LENGTH    1

#define J1_ITMH_PPH_PARSING_OFFSET_QUAL_LENGTH    7

#define J1_ITMH_PPH_PARSING_TYPE_VALID_QUAL_LENGTH    1

#define J1_ITMH_PPH_PARSING_TYPE_QUAL_LENGTH      4

#define J1_ITMH_PPH_PACKET_STRIP_QUAL_LENGTH      9

#define J1_ITMH_PPH_TTL_SET_QUAL_LENGTH     8

#define J1_ITMH_PPH_NETWORK_QOS_QUAL_LENGTH 8

#define J1_ITMH_PPH_IN_LIF_PROFILE_QUAL_LENGTH     2

#define J1_ITMH_PPH_FWD_ADDITIONAL_INFO_QUAL_LENGTH    4

#define J1_ITMH_PPH_IN_LIF_QUAL_LENGTH    18

#define J1_ITMH_PPH_FWD_DOMAIN_QUAL_LENGTH 16

#define J1_ITMH_PPH_FWD_LAYER_INDEX_QUAL_LENGTH 3


#define J1_ITMH_PPH_FHEI_SIZE_QUAL_LENGTH      2

#define J1_ITMH_PPH_FHEI_TYPE_QUAL_LENGTH      4

#define J1_ITMH_PPH_SYS_HDR_ADJUST_SIZE_QUAL_LENGTH    1


#define J1_ITMH_PPH_PARS_START_SH_ADJUST_ACTION_LENGTH     14

#define J1_ITMH_PPH_IPMF3_ZERO_ACTION_LENGTH                18

#define J1_ITMH_PPH_SYS_HDR_ADJUST_SIZE_ACTION_LENGTH       1

#define J1_ITMH_PPH_PARSING_START_OFFSET_ACTION_LENGTH       8


typedef struct
{
    char *name;
    bcm_field_qualify_t qual_id;
    uint32 qual_length;
    bcm_field_qualify_attach_info_t qual_attach_info;

} field_j1_itmh_pph_qual_info_t;


typedef struct
{
    char *name;
    bcm_field_action_t action_id;
    bcm_field_action_info_t action_info;
    uint8 action_with_valid_bit;

} field_j1_itmh_pph_action_info_t;


typedef struct
{
    int nof_conditions;
    int condition_ids[16];
    int nof_extractions;
    int nof_mapping_bits;

    
    bcm_field_array_n_position_t field_array_n_position;
    bcm_field_fem_action_info_t fem_action_info;

} field_j1_itmh_pph_fem_action_info_t;


typedef struct
{
    bcm_field_group_t fg_id;
    bcm_field_stage_t stage;
    bcm_field_group_type_t fg_type;
    int nof_quals;
    int nof_actions;
    field_j1_itmh_pph_qual_info_t *itmh_qual_info;
    field_j1_itmh_pph_action_info_t *itmh_action_info;
    int nof_fems;
    field_j1_itmh_pph_fem_action_info_t *itmh_pph_fem_action_info;
} field_j1_itmh_pph_fg_info_t;



shr_error_e appl_dnx_field_j1_itmh_pph_cb(
    int unit,
    int *dynamic_flags);


shr_error_e appl_dnx_field_j1_itmh_pph_init(
    int unit);


#endif 

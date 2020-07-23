/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 *  Testing UDH Cascading.
 *
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint ../../src/examples/dnx/field/cint_field_udh_cascading.c
 * int unit = 0;
 * cint_field_udh_cascading_ipmf2_ipmf3_main(unit);
 *
 * Configuration example end
 *
 *  Test scenario:
 *      Configure TCAM field groups in 2 different stages which allow cascading between
 *      each other. Add relevant actions and qualifiers to both. Attach them to a contexts,
 *      in relevant stage, and add an entry to them. Context Selection is done according
 *      to Forwarding type bcmFieldLayerTypeEth.
 *
 *      The test represents following scenarios:
 *          1. Ingress cascading:
 *              - IPMF1-IPMF3
 *              - IPMF2-IPMF3
 *          2. Ingress-Egress cascading:
 *              - IPMF1-EPMF
 *              - IPMF2-EPMF
 *              - IPMF3-EPMF
 *          3. Multiple cascading:
 *              - IPMF1:
 *                  bcmFieldActionUDHBase0, bcmFieldActionUDHData0
 *                  bcmFieldActionUDHBase1, bcmFieldActionUDHData1
 *                  bcmFieldActionUDHBase2, bcmFieldActionUDHData2
 *              - IPMF3 --> Qualify upon configured User Headers above.
 *          4. Exhaustive cascading:
 *              Configure bellow qualifiers with maximal values.
 *              - IPMF1:
 *                  bcmFieldActionUDHBase0, bcmFieldActionUDHData0
 *                  bcmFieldActionUDHBase1, bcmFieldActionUDHData1
 *              - IPMF2:
 *                  bcmFieldActionUDHBase2, bcmFieldActionUDHData2
 *                  bcmFieldActionUDHBase3, bcmFieldActionUDHData3
 *              - IPMF3 --> Qualify upon configured User Headers above.
 *          5. 'Train' cascading
 *              - IPMF1 --> bcmFieldActionUDHBase0, bcmFieldActionUDHData0
 *              - IPMF2 --> bcmFieldActionUDHBase2, bcmFieldActionUDHData2
 *              - IPMF3 --> bcmFieldActionUDHBase3, bcmFieldActionUDHData3
 *              - EPMF --> Qualify upon configured User Headers above.
 */

/** Offset of ETH_TYPE Qualifier for ETH Header[6:5] (2b) */
int CINT_FIELD_UDH_CASCADING_USER_QUA_ETH_TYPE_QUAL_OFFSET = 96;
/** Length of ETH_TYPE Qualifier for EHT Header[6:5] (2b) */
int CINT_FIELD_UDH_CASCADING_USER_QUA_ETH_TYPE_QUAL_LENGTH = 16;

int CINT_FIELD_UDH_CASCADING_STACKING_ACT_VALUE = 0x2;
int CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE = 0xA;
int CINT_FIELD_UDH_CASCADING_UDH1_QUAL_ACT_VALUE = 0xB;
int CINT_FIELD_UDH_CASCADING_UDH2_QUAL_ACT_VALUE = 0xC;
int CINT_FIELD_UDH_CASCADING_UDH3_QUAL_ACT_VALUE = 0xD;
int CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE = 0xFF;
int CINT_FIELD_UDH_CASCADING_16b_QUAL_ACT_VALUE = 0xFFFF;
int CINT_FIELD_UDH_CASCADING_24b_QUAL_ACT_VALUE = 0xFFFFFF;
int CINT_FIELD_UDH_CASCADING_32b_QUAL_ACT_VALUE = 0xFFFFFFFF;

int CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS = 1;
int CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS = 2;
int CINT_FIELD_UDH_CASCADING_NOF_MULTIPLE_QUALS = 3;
int CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_QUALS = 4;
int CINT_FIELD_UDH_CASCADING_NOF_TRAIN_QUALS = 8;

int CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG = 2;
int CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_FG = 3;
int CINT_FIELD_UDH_CASCADING_NOF_TRAIN_FG = 4;

int CINT_FIELD_UDH_CASCADING_NOF_CS_STAGES = 4;
int CINT_FIELD_UDH_CASCADING_NOF_CS = 1;
/**
 * Value to be used for bcmFieldActionPrioIntNew.
 * Which will be performed in case the cascading was successful.
 */
int CINT_FIELD_UDH_CASCADING_TC_ACTION_VALUE = 6;
int CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS = 1;
int CINT_FIELD_UDH_CASCADING_NOF_ACTIONS = 2;
int CINT_FIELD_UDH_CASCADING_NOF_MULTIPLE_ACTIONS = 6;
int CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_ACTIONS = 4;

/* *INDENT-OFF* */

/**
 * Structure, which contains information,
 * for creating of contexts for every stage.
 */
struct cint_field_udh_cascading_cs_info_t
{
    bcm_field_context_t context_id;      /* Context id. */
    char * context_name;      /* Context name. */
    bcm_field_presel_t presel_id;      /* Presel id. */
    bcm_field_layer_type_t fwd_layer;      /* Forwarding Layer Type. */
};

/**
 * Structure, which contains information,
 * for creating of user actions and predefined one.
 */
struct cint_field_udh_cascading_qual_info_t
{
    bcm_field_qualify_t qual_type;      /* Qualifier type. */
    uint32 qual_value;          /* Qualifier value. */
    uint32 qual_mask;           /* Qualifier mask. */
    bcm_field_qualify_attach_info_t qual_attach_info;   /* Qualifier Attach info. */
};

/**
 * Structure, which contains needed action information.
 */
struct cint_field_udh_cascading_action_info_t
{
    char * name;
    bcm_field_action_t action_type;     /* Action type. */
    uint32 action_value;        /* Action value. */
    bcm_field_action_info_t action_info;
};

/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
struct cint_field_udh_cascading_fg_info_t
{
    bcm_field_group_t fg_id;    /* Field Group ID. */
    char * fg_name;     /* Field Group Name. */
    bcm_field_stage_t stage;    /* Field stage. */
    bcm_field_group_type_t fg_type;     /* Field Group type. */
    bcm_field_entry_t entry_handle;     /* The Entry handle. */
    int nof_quals;              /* Number of qualifiers per group. */
    int nof_actions;            /* Number of actions per group. */
    cint_field_udh_cascading_cs_info_t udh_cs_info; /* Context Selection info. */
    cint_field_udh_cascading_qual_info_t * udh_qual_info;   /* Qualifier Info. */
    cint_field_udh_cascading_action_info_t * udh_action_info;       /* Action Info. */
};

/** Context Selection info per field stage (IPMF1-2-3 and EPMF). */
cint_field_udh_cascading_cs_info_t field_udh_cascading_cs_info_array[CINT_FIELD_UDH_CASCADING_NOF_CS_STAGES] = {
  /**  context_id   |   context_name   |   presel_id    |  fwd_layer  */
    {0, "udh_casc_ipmf12", 50, bcmFieldLayerTypeEth},
    {0, "udh_casc_ipmf3", 52, bcmFieldLayerTypeEth},
    {0, "udh_casc_epmf", 53, bcmFieldLayerTypeEth}
};

/** USER DEFINE Qualifiers info per field stage (IPMF1-2). */
cint_field_udh_cascading_qual_info_t field_udh_cascading_user_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_CS_STAGES] = {
 /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyCount, 0x8100, 0xFFFF, {bcmFieldInputTypeLayerAbsolute, 0, CINT_FIELD_UDH_CASCADING_USER_QUA_ETH_TYPE_QUAL_OFFSET}},
    {bcmFieldQualifyCount, 0x8100, 0xFFFF, {bcmFieldInputTypeLayerAbsolute, 0, CINT_FIELD_UDH_CASCADING_USER_QUA_ETH_TYPE_QUAL_OFFSET}}
};

/** IPMF3 USER DEFINE Qualifiers info. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_ipmf3_user_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS] = {
 /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyCount, 0, 0xFFFF, {bcmFieldInputTypeConst, 0, 0}}
};

/** BASIC Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_basic_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionPrioIntNew, CINT_FIELD_UDH_CASCADING_TC_ACTION_VALUE, {0}},
};

/** IPMF1 Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf1_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHBase3, 0x1, {0}},
    {NULL, bcmFieldActionUDHData3, CINT_FIELD_UDH_CASCADING_UDH3_QUAL_ACT_VALUE, {0}}
};

/** IPMF1 Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf1_train_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHBase0, 0x1, {0}},
    {NULL, bcmFieldActionUDHData0, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, {0}},
    {NULL, bcmFieldActionUDHBase1, 0x1, {0}},
    {NULL, bcmFieldActionUDHData1, CINT_FIELD_UDH_CASCADING_UDH1_QUAL_ACT_VALUE, {0}}
};

/** IPMF2 Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf2_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHBase3, 0x1, {0}},
    {NULL, bcmFieldActionUDHData3, CINT_FIELD_UDH_CASCADING_UDH3_QUAL_ACT_VALUE, {0}}
};

/** IPMF2 Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf2_train_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHBase2, 0x1, {0}},
    {NULL, bcmFieldActionUDHData2, CINT_FIELD_UDH_CASCADING_UDH2_QUAL_ACT_VALUE, {0}}
};

/** IPMF3 Qualifiers info. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_ipmf3_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS] = {
 /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyUDHData3, CINT_FIELD_UDH_CASCADING_UDH3_QUAL_ACT_VALUE, 0xFFFFFFFF, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** IPMF3 Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf3_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHBase, 0x40, {0}},
    {NULL, bcmFieldActionUDHData3, CINT_FIELD_UDH_CASCADING_UDH3_QUAL_ACT_VALUE, {0}}
};

/** IPMF3 Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf3_train_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHBase, 0x55, {0}},
    {NULL, bcmFieldActionUDHData3, CINT_FIELD_UDH_CASCADING_UDH3_QUAL_ACT_VALUE, {0}}
};

/** Stacking Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf1_j1_mode_stacking_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionStackingRouteNew, CINT_FIELD_UDH_CASCADING_STACKING_ACT_VALUE, {0}}
};

/** UDH/Stacking Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf1_j1_mode_udh_stack_act_info_array[CINT_FIELD_UDH_CASCADING_NOF_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionStackingRouteNew, CINT_FIELD_UDH_CASCADING_STACKING_ACT_VALUE, {0}},
    {NULL, bcmFieldActionUDHData0, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, {0}}
};

/** IPMF1 Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf1_j1_mode_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHData1, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, {0}}
};

/** IPMF1 Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf1_j1_mode_diff_sizes_act_info_array[CINT_FIELD_UDH_CASCADING_NOF_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHData1, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, {0}},
    {NULL, bcmFieldActionUDHData0, CINT_FIELD_UDH_CASCADING_UDH1_QUAL_ACT_VALUE, {0}}
};

/** IPMF2 Actions info. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf2_j1_mode_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHData0, CINT_FIELD_UDH_CASCADING_UDH1_QUAL_ACT_VALUE, {0}}
};

/** EPMF Qualifiers info. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_epmf_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS] = {
 /**  qual_type                 |   qual_value              |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyUDHData3, CINT_FIELD_UDH_CASCADING_UDH3_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHBase3, 0x1, 0x3, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** Stacking Qualifiers info. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_epmf_j1_mode_stacking_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS] = {
 /**  qual_type                 |   qual_value              |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyStackingRoute, CINT_FIELD_UDH_CASCADING_STACKING_ACT_VALUE, 0xFFFF, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** Stacking Qualifiers info. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_epmf_j1_mode_udh_stacking_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS] = {
 /**  qual_type                 |   qual_value              |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyStackingRoute, CINT_FIELD_UDH_CASCADING_STACKING_ACT_VALUE, 0xFFFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData3, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** EPMF Qualifiers info. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_epmf_j1_mode_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS] = {
 /**  qual_type                 |   qual_value              |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyUDHData3, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** EPMF Qualifiers info. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_epmf_j1_mode_combination_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS] = {
 /**  qual_type                 |   qual_value              |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyUDHData3, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData2, CINT_FIELD_UDH_CASCADING_UDH1_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** IPMF3 Qualifiers info. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_ipmf3_j1_mode_train_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS] = {
 /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyUDHData1, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, 0xFFFFFFFF, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** IPMF1 Actions info for multiple testing. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf1_multiple_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_MULTIPLE_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHBase0, 0x1, {0}},
    {"udh_0", 0, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, {0, bcmFieldActionUDHData0, 8,
        24, 0x0, {0}}},
    {NULL, bcmFieldActionUDHBase1, 0x1, {0}},
    {"udh_1", 0, CINT_FIELD_UDH_CASCADING_UDH1_QUAL_ACT_VALUE, {0, bcmFieldActionUDHData1, 8,
        24, 0x0, {0}}},
    {NULL, bcmFieldActionUDHBase2, 0x1, {0}},
    {"udh_2", 0, CINT_FIELD_UDH_CASCADING_UDH2_QUAL_ACT_VALUE, {0, bcmFieldActionUDHData2, 8,
        24, 0x0, {0}}}
};

/** IPMF3 Qualifiers info for multiple testing. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_ipmf3_multiple_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_MULTIPLE_QUALS] = {
 /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyUDHData0, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData1, CINT_FIELD_UDH_CASCADING_UDH1_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData2, CINT_FIELD_UDH_CASCADING_UDH2_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** IPMF1 Actions info for exhaustive testing. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf1_exhaustive_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHBase0, 0x1, {0}},
    {"udh_0", 0, CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE, {0, bcmFieldActionUDHData0, 8,
        24, 0x0, {0}}},
    {NULL, bcmFieldActionUDHBase1, 0x1, {0}},
    {"udh_1", 0, CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE, {0, bcmFieldActionUDHData1, 8,
        24, 0x0, {0}}}
};

/** IPMF2 Actions info for exhaustive testing. */
cint_field_udh_cascading_action_info_t field_udh_cascading_ipmf2_exhaustive_action_info_array[CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_ACTIONS] = {
 /**     action_type       |   action_value   */
    {NULL, bcmFieldActionUDHBase2, 0x1, {0}},
    {"udh_2", 0, CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE, {0, bcmFieldActionUDHData2, 8,
        24, 0x0, {0}}},
    {NULL, bcmFieldActionUDHBase3, 0x1, {0}},
    {"udh_3", 0, CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE, {0, bcmFieldActionUDHData3, 8,
        24, 0x0, {0}}}
};

/** IPMF3 Qualifiers info for exhaustive testing. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_ipmf3_exhaustive_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_QUALS] = {
 /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyUDHData0, CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData1, CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData2, CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData3, CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}}
};

/** EPMF Qualifiers info for train testing. */
cint_field_udh_cascading_qual_info_t field_udh_cascading_epmf_train_qual_info_array[CINT_FIELD_UDH_CASCADING_NOF_TRAIN_QUALS] = {
 /**  qual_type   |   qual_value   |   qual_mask    |  input_type    |    input_arg  |  qual_offset  */
    {bcmFieldQualifyUDHData0, CINT_FIELD_UDH_CASCADING_UDH0_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHBase0, 0x1, 0x3, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData1, CINT_FIELD_UDH_CASCADING_UDH1_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHBase1, 0x1, 0x3, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData2, CINT_FIELD_UDH_CASCADING_UDH2_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHBase2, 0x1, 0x3, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHData3, CINT_FIELD_UDH_CASCADING_UDH3_QUAL_ACT_VALUE, 0xFF, {bcmFieldInputTypeMetaData, 0, 0}},
    {bcmFieldQualifyUDHBase3, 0x1, 0x3, {bcmFieldInputTypeMetaData, 0, 0}}
};

/**
 * Following arrays are global, used for all test cases. They contain information per field group. Like:
 * fg_id, fg_type, context selection, quals, actions.
 */
cint_field_udh_cascading_fg_info_t field_udh_cascading_ipmf2_ipmf3_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF2 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf2",          /** Field group Name */
     bcmFieldStageIngressPMF2,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     field_udh_cascading_user_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_ipmf2_action_info_array        /** Actions info */
     },
    /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[1],     /** Context Selection info */
     field_udh_cascading_ipmf3_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_ipmf2_epmf_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF2 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf2",          /** Field group Name */
     bcmFieldStageIngressPMF2,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[1],         /** Qualifiers info */
     field_udh_cascading_ipmf2_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_ipmf3_epmf_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[1],     /** Context Selection info */
     field_udh_cascading_ipmf3_user_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_ipmf3_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_ipmf1_ipmf3_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_action_info_array        /** Actions info */
     },
    /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[1],     /** Context Selection info */
     field_udh_cascading_ipmf3_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_ipmf1_epmf_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_stacking_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "stack_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_stacking_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "stck_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_stacking_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_udh_stacking_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_stck_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_udh_stack_act_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_stck_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_udh_stacking_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_diff_sizes_act_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_combination_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_ipmf2_epmf_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF2 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf2",          /** Field group Name */
     bcmFieldStageIngressPMF2,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_ipmf3_epmf_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     field_udh_cascading_ipmf3_user_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_ipmf12_epmf_comb_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_action_info_array        /** Actions info */
     },
     /** IPMF2 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf2",          /** Field group Name */
     bcmFieldStageIngressPMF2,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[1],         /** Qualifiers info */
     field_udh_cascading_ipmf2_j1_mode_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_combination_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_ipmf13_epmf_comb_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_action_info_array        /** Actions info */
     },
     /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[1],     /** Context Selection info */
     field_udh_cascading_ipmf3_user_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_ipmf2_j1_mode_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_combination_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_ipmf23_epmf_comb_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_FG] = {
    /** IPMF2 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf2",          /** Field group Name */
     bcmFieldStageIngressPMF2,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_action_info_array        /** Actions info */
     },
     /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[1],     /** Context Selection info */
     field_udh_cascading_ipmf3_user_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_ipmf2_j1_mode_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_combination_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_j1_mode_ipmf13_epmf_train_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_j1_mode_action_info_array        /** Actions info */
     },
     /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[1],     /** Context Selection info */
     field_udh_cascading_ipmf3_j1_mode_train_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_ipmf2_j1_mode_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EPMF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_j1_mode_combination_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_ipmf1_ipmf3_multiple_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_BASIC_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_MULTIPLE_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_multiple_action_info_array        /** Actions info */
     },
    /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_MULTIPLE_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[1],     /** Context Selection info */
     field_udh_cascading_ipmf3_multiple_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_ipmf12_ipmf3_exhaustive_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_exhaustive_action_info_array        /** Actions info */
     },
     /** IPMF2 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf2",          /** Field group Name */
     bcmFieldStageIngressPMF2,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[1],         /** Qualifiers info */
     field_udh_cascading_ipmf2_exhaustive_action_info_array        /** Actions info */
     },
    /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[1],     /** Context Selection info */
     field_udh_cascading_ipmf3_exhaustive_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     }
};

cint_field_udh_cascading_fg_info_t field_udh_cascading_train_fg_info_array[CINT_FIELD_UDH_CASCADING_NOF_TRAIN_FG] = {
    /** IPMF1 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf1",          /** Field group Name */
     bcmFieldStageIngressPMF1,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_EXHAUSTIVE_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[0],         /** Qualifiers info */
     field_udh_cascading_ipmf1_train_action_info_array        /** Actions info */
     },
     /** IPMF2 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf2",          /** Field group Name */
     bcmFieldStageIngressPMF2,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                            /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[0],     /** Context Selection info */
     &field_udh_cascading_user_qual_info_array[1],         /** Qualifiers info */
     field_udh_cascading_ipmf2_train_action_info_array        /** Actions info */
     },
    /** IPMF3 info. */
    {
     0,         /** Field group ID */
     "udh_casc_ipmf3",          /** Field group Name */
     bcmFieldStageIngressPMF3,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[1],     /** Context Selection info */
     field_udh_cascading_ipmf3_user_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_ipmf3_train_action_info_array        /** Actions info */
     },
    /** EPMF info. */
    {
     0,         /** Field group ID */
     "udh_casc_epmf",          /** Field group Name */
     bcmFieldStageEgress,         /** Field stage */
     bcmFieldGroupTypeTcam,        /** Field group type */
     0,                           /* The Entry handle. */
     CINT_FIELD_UDH_CASCADING_NOF_TRAIN_QUALS,        /** Number of tested qualifiers */
     CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS,       /** Number of tested actions */
     field_udh_cascading_cs_info_array[2],     /** Context Selection info */
     field_udh_cascading_epmf_train_qual_info_array,         /** Qualifiers info */
     field_udh_cascading_basic_action_info_array        /** Actions info */
     },
};

/* *INDENT-ON* */

/** Used to configure the user defined actions. */
int
cint_field_udh_cascading_user_action_config(
    int unit,
    bcm_field_stage_t field_stage,
    cint_field_udh_cascading_action_info_t * udh_cacading_action_info)
{
    int rv = BCM_E_NONE;
    bcm_field_action_info_t action_info;
    void *dest_char;

    bcm_field_action_info_t_init(&action_info);

    action_info.action_type = udh_cacading_action_info->action_info.action_type;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, udh_cacading_action_info->name, sizeof(action_info.name));
    action_info.size = udh_cacading_action_info->action_info.size;
    action_info.stage = field_stage;
    action_info.prefix_size = udh_cacading_action_info->action_info.prefix_size;
    action_info.prefix_value = udh_cacading_action_info->action_info.prefix_value;

    rv = bcm_field_action_create(unit, 0, &action_info, &(udh_cacading_action_info->action_type));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create on action_id = '%s' \n", rv, udh_cacading_action_info->name);
        return rv;
    }

    return rv;
}

/**
 * This function creates field group, with all relevant information to it,
 * attaching it to the context and adding an entry.
 */
int
cint_field_udh_cascading_group_config(
    int unit,
    cint_field_udh_cascading_fg_info_t * udh_fg_info)
{
    int rv = BCM_E_NONE;
    int qual_index, action_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    void *dest_char;
    char *char_dest_char;
    bcm_field_entry_info_t entry_info;
    bcm_field_stage_t field_stage;

    /** In case of IPMF2 stage the context should be created in IPMF1 stage. */
    if (udh_fg_info->stage == bcmFieldStageIngressPMF2)
    {
        field_stage = bcmFieldStageIngressPMF1;
    }
    else
    {
        field_stage = udh_fg_info->stage;
    }

    /**
     * Create new context only in case there is not already created one.
     * Check is done mainly for IPMF1 and IPMF2 configurations, where we should
     * reuse the created context in IPMF1 stage in IPMF2 stage.
     */
    if (udh_fg_info->udh_cs_info.context_id == 0)
    {
        /** Create FWD context. */
        rv = field_presel_fwd_layer_main(unit, udh_fg_info->udh_cs_info.presel_id, field_stage,
                                         udh_fg_info->udh_cs_info.fwd_layer,
                                         &(udh_fg_info->udh_cs_info.context_id), udh_fg_info->udh_cs_info.context_name);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in field_presel_fwd_layer_main \n", rv);
            return rv;
        }
    }

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = udh_fg_info->fg_type;
    fg_info.stage = udh_fg_info->stage;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, udh_fg_info->fg_name, sizeof(fg_info.name));
    fg_info.nof_quals = udh_fg_info->nof_quals;
    fg_info.nof_actions = udh_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    entry_info.priority = 1;

    for (qual_index = 0; qual_index < udh_fg_info->nof_quals; qual_index++)
    {
        /**
         * Check if the current qualifier type is bcmFieldQualifyCount, it means
         * that this is an user define qualifier and should be created as follow.
         */
        if (udh_fg_info->udh_qual_info[qual_index].qual_type == bcmFieldQualifyCount)
        {
            bcm_field_qualifier_info_create_t qual_info;
            bcm_field_qualifier_info_create_t_init(&qual_info);
            dest_char = &(qual_info.name[0]);
            char_dest_char = dest_char;
            snprintf(char_dest_char, sizeof(qual_info.name), "%s_q%d", udh_fg_info->fg_name, qual_index);
            qual_info.name[sizeof(qual_info.name) - 1] = 0;
            qual_info.size = CINT_FIELD_UDH_CASCADING_USER_QUA_ETH_TYPE_QUAL_LENGTH;
            rv = bcm_field_qualifier_create(unit, 0, &qual_info, &(udh_fg_info->udh_qual_info[qual_index].qual_type));
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in bcm_field_qualifier_create on qual_id = %d (%s) \n", rv,
                       udh_fg_info->udh_qual_info[qual_index].qual_type, qual_info.name);
                return rv;
            }
        }
        fg_info.qual_types[qual_index] = udh_fg_info->udh_qual_info[qual_index].qual_type;
        attach_info.key_info.qual_types[qual_index] = fg_info.qual_types[qual_index];
        attach_info.key_info.qual_info[qual_index].input_type =
            udh_fg_info->udh_qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg =
            udh_fg_info->udh_qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset =
            udh_fg_info->udh_qual_info[qual_index].qual_attach_info.offset;
        entry_info.entry_qual[qual_index].type = fg_info.qual_types[qual_index];
        entry_info.entry_qual[qual_index].value[0] = udh_fg_info->udh_qual_info[qual_index].qual_value;
        entry_info.entry_qual[qual_index].mask[0] = udh_fg_info->udh_qual_info[qual_index].qual_mask;
    }

    for (action_index = 0; action_index < udh_fg_info->nof_actions; action_index++)
    {
        /**
         * Check if the current action name is not NULL, it means that
         * this is an user define action and should be created as follow.
         */
        if (udh_fg_info->udh_action_info[action_index].name != NULL)
        {
            rv = cint_field_udh_cascading_user_action_config(unit, fg_info.stage,
                                                             udh_fg_info->udh_action_info[action_index]);
            if (rv != BCM_E_NONE)
            {
                printf("Error (%d), in cint_field_udh_cascading_user_action_config. \n", rv);
                return rv;
            }
        }
        fg_info.action_types[action_index] = udh_fg_info->udh_action_info[action_index].action_type;
        fg_info.action_with_valid_bit[action_index] = TRUE;
        attach_info.payload_info.action_types[action_index] = fg_info.action_types[action_index];
        entry_info.entry_action[action_index].type = fg_info.action_types[action_index];
        entry_info.entry_action[action_index].value[0] = udh_fg_info->udh_action_info[action_index].action_value;
    }

    /** Create the field group. */
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(udh_fg_info->fg_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add on field_group_id = %d \n", rv, udh_fg_info->fg_id);
        return rv;
    }

    printf("Field Group ID %d was created. \n", udh_fg_info->fg_id);

    /** Attach the created field group to the context. */
    rv = bcm_field_group_context_attach(unit, 0, udh_fg_info->fg_id, udh_fg_info->udh_cs_info.context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach on contex_id = %d \n", rv,
               udh_fg_info->udh_cs_info.context_id);
        return rv;
    }

    printf("Field Group ID %d was attached to Context ID %d. \n", udh_fg_info->fg_id,
           udh_fg_info->udh_cs_info.context_id);

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, udh_fg_info->fg_id, &entry_info, &(udh_fg_info->entry_handle));
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", udh_fg_info->entry_handle,
           udh_fg_info->entry_handle, udh_fg_info->fg_id);

    return rv;
}

/** Configure UDH cascading between IPMF2 and IPMF3 stages. */
int
cint_field_udh_cascading_ipmf2_ipmf3_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF2 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf2_ipmf3_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf2_ipmf3_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Configure UDH cascading between IPMF2 and EPMF stages. */
int
cint_field_udh_cascading_ipmf2_epmf_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF2 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf2_epmf_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf2_epmf_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Configure UDH cascading between IPMF3 and EPMF stages. */
int
cint_field_udh_cascading_ipmf3_epmf_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf3_epmf_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf3_epmf_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Configure UDH cascading between IPMF1 and IPMF3 stages. */
int
cint_field_udh_cascading_ipmf1_ipmf3_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf1_ipmf3_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf1_ipmf3_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Configure UDH cascading between IPMF1 and EPMF stages. */
int
cint_field_udh_cascading_ipmf1_epmf_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf1_epmf_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf1_epmf_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1 and IPMF3 stages.
 * Using two UDH headers.
 */
int
cint_field_udh_cascading_ipmf1_ipmf3_multiple_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf1_ipmf3_multiple_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf1_ipmf3_multiple_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1-2 and IPMF3 stages.
 * Using two UDH headers with maximal values in:
 *      - IPMF1 (UDH0 and UDH1)
 *      - IPMF2 (UDH2 and UDH3)
 * Qualify upon 4 of them in IPMF3 stage.
 */
int
cint_field_udh_cascading_ipmf12_ipmf3_exhaustive_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf12_ipmf3_exhaustive_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Reuse the context of IPMF1 in IPMF2 stage. */
    field_udh_cascading_ipmf12_ipmf3_exhaustive_fg_info_array[1].udh_cs_info.context_id =
        field_udh_cascading_ipmf12_ipmf3_exhaustive_fg_info_array[0].udh_cs_info.context_id;

    /** Creating IPMF2 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf12_ipmf3_exhaustive_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_ipmf12_ipmf3_exhaustive_fg_info_array[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1-2-3 and EPMF stages.
 * Using 3 UDH headers with different values as follow:
 *      - IPMF1 (UDH1)
 *      - IPMF2 (UDH2)
 *      - IPMF3 (UDH3)
 * Qualify upon 3 of them in EPMF stage.
 */
int
cint_field_udh_cascading_train_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_train_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Reuse the context of IPMF1 in IPMF2 stage. */
    field_udh_cascading_train_fg_info_array[1].udh_cs_info.context_id =
        field_udh_cascading_train_fg_info_array[0].udh_cs_info.context_id;

    /** Creating IPMF2 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_train_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_train_fg_info_array[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_train_fg_info_array[3]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1 and EPMF stages in J1 mode.
 * Using UDH1 to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF2 and EPMF stages in J1 mode.
 * Using UDH1 to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf2_epmf_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF2 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf2_epmf_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf2_epmf_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF3 and EPMF stages in J1 mode.
 * Using UDH1 to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf3_epmf_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf3_epmf_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf3_epmf_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1-2 and EPMF stages in J1 mode.
 * Using UDH0 and UDH1 to pass values to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf12_epmf_combination_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf12_epmf_comb_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Reuse the context of IPMF1 in IPMF2 stage. */
    field_udh_cascading_j1_mode_ipmf12_epmf_comb_fg_info_array[1].udh_cs_info.context_id =
        field_udh_cascading_j1_mode_ipmf12_epmf_comb_fg_info_array[0].udh_cs_info.context_id;

    /** Creating IPMF2 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf12_epmf_comb_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf12_epmf_comb_fg_info_array[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1-3 and EPMF stages in J1 mode.
 * Using UDH0 and UDH1 to pass values to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf13_epmf_combination_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf13_epmf_comb_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf13_epmf_comb_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf13_epmf_comb_fg_info_array[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF2-3 and EPMF stages in J1 mode.
 * Using UDH0 and UDH1 to pass values to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf23_epmf_combination_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF2 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf23_epmf_comb_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf23_epmf_comb_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf23_epmf_comb_fg_info_array[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1, IPMF3 (qualify upon result of IPMF1 (UDH1))
 * and EPMF (qualify upon update UDH0 and UDH1, by ingress stages) stages in J1 mode.
 * Using UDH0 and UDH1 to pass values to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf13_epmf_train_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf13_epmf_train_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating IPMF3 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf13_epmf_train_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf13_epmf_train_fg_info_array[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1 and EPMF stages in J1 mode.
 * Using UDH1 (8b) and UDH0 (8b) to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf1_epmf_8b_main(
    int unit)
{
    int rv = BCM_E_NONE;

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[0].action_value = CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[1].action_value = CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].nof_quals = CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_value = CINT_FIELD_UDH_CASCADING_16b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_mask = 0xFFFF;

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1 and EPMF stages in J1 mode.
 * Using UDH1 (16b) and UDH0 (16b) to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf1_epmf_16b_main(
    int unit)
{
    int rv = BCM_E_NONE;

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[0].action_value = CINT_FIELD_UDH_CASCADING_16b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[1].action_value = CINT_FIELD_UDH_CASCADING_16b_QUAL_ACT_VALUE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].nof_quals = CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_value = CINT_FIELD_UDH_CASCADING_32b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_mask = 0xFFFFFFFF;

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1 and EPMF stages in J1 mode.
 * Using UDH1 (24b) and UDH0 (24b) to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf1_epmf_24b_main(
    int unit)
{
    int rv = BCM_E_NONE;

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[0].action_value = CINT_FIELD_UDH_CASCADING_24b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[1].action_value = CINT_FIELD_UDH_CASCADING_24b_QUAL_ACT_VALUE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_value = CINT_FIELD_UDH_CASCADING_32b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_mask = 0xFFFFFFFF;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[1].qual_value = CINT_FIELD_UDH_CASCADING_16b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[1].qual_mask = 0xFFFF;

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1 and EPMF stages in J1 mode.
 * Using UDH1 (8b) and UDH0 (24b) to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf1_epmf_24b_8b_main(
    int unit)
{
    int rv = BCM_E_NONE;

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[0].action_value = CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[1].action_value = CINT_FIELD_UDH_CASCADING_24b_QUAL_ACT_VALUE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].nof_quals = CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_value = CINT_FIELD_UDH_CASCADING_32b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_mask = 0xFFFFFFFF;

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1 and EPMF stages in J1 mode.
 * Using UDH1 (24b) and UDH0 (8b) to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf1_epmf_8b_24b_main(
    int unit)
{
    int rv = BCM_E_NONE;

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[0].action_value = CINT_FIELD_UDH_CASCADING_24b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[1].action_value = CINT_FIELD_UDH_CASCADING_EXHAUSTIVE_QUAL_ACT_VALUE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].nof_quals = CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_value = CINT_FIELD_UDH_CASCADING_32b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_mask = 0xFFFFFFFF;

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1 and EPMF stages in J1 mode.
 * Using UDH1 (16b) and UDH0 (0b) to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf1_epmf_0b_16b_main(
    int unit)
{
    int rv = BCM_E_NONE;

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].nof_actions = CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[0].action_value = CINT_FIELD_UDH_CASCADING_16b_QUAL_ACT_VALUE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].nof_quals = CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_value = CINT_FIELD_UDH_CASCADING_16b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_mask = 0xFFFF;

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure UDH cascading between IPMF1 and EPMF stages in J1 mode.
 * Using UDH1 (0b) and UDH0 (16b) to pass value to EPMF.
 */
int
cint_field_udh_cascading_j1_mode_ipmf1_epmf_16b_0b_main(
    int unit)
{
    int rv = BCM_E_NONE;

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].nof_actions = CINT_FIELD_UDH_CASCADING_NOF_BASIC_ACTIONS;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[0].action_type = bcmFieldActionUDHData0;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0].udh_action_info[0].action_value = CINT_FIELD_UDH_CASCADING_16b_QUAL_ACT_VALUE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].nof_quals = CINT_FIELD_UDH_CASCADING_NOF_BASIC_QUALS;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_value = CINT_FIELD_UDH_CASCADING_16b_QUAL_ACT_VALUE;
    field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1].udh_qual_info[0].qual_mask = 0xFFFF;

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure stacking cascading between IPMF1 and EPMF stages in J1 mode.
 * Using StackingRoute to pass values from Ingress to Egress.
 */
int
cint_field_udh_cascading_j1_mode_stacking_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_stacking_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_stacking_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * Configure udh/stacking cascading between IPMF1 and EPMF stages in J1 mode.
 * Using UDH/StackingRoute to pass values from Ingress to Egress.
 */
int
cint_field_udh_cascading_j1_mode_udh_stacking_main(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Creating IPMF1 Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_udh_stacking_fg_info_array[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Creating EPMF Field Group. */
    rv = cint_field_udh_cascading_group_config(unit, field_udh_cascading_j1_mode_udh_stacking_fg_info_array[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit                 - Device id
 * \param [in] udh_fg_info  - Global Array, which contains
 *  all needed information to be destroyed.
 * \param [in] is_ipmf2_exist  - Flag to indicate if we have
 * IPMF2 stage configuration, which is reusing the context
 * created in IPMF1 stage. It can be FALSE or TRUE.
 * In case of TRUE, we will not clean the created context,
 * otherwise it will be destroyed.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_field_udh_cascading_destroy(
    int unit,
    cint_field_udh_cascading_fg_info_t * udh_fg_info,
    int is_ipmf2_exist)
{
    int rv = BCM_E_NONE;
    int qual_index, action_index;
    bcm_field_stage_t field_stage;

    rv = bcm_field_entry_delete(unit, udh_fg_info->fg_id, NULL, udh_fg_info->entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, udh_fg_info->fg_id, udh_fg_info->udh_cs_info.context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    rv = bcm_field_group_delete(unit, udh_fg_info->fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_delete\n");
        return rv;
    }

    /** In case of IPMF2 stage the context was created in IPMF1 stage. */
    if (udh_fg_info->stage == bcmFieldStageIngressPMF2)
    {
        field_stage = bcmFieldStageIngressPMF1;
    }
    else
    {
        field_stage = udh_fg_info->stage;
    }

    if (is_ipmf2_exist == FALSE)
    {
        rv = field_presel_fwd_layer_destroy(unit, udh_fg_info->udh_cs_info.presel_id, field_stage,
                                            udh_fg_info->udh_cs_info.context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in field_presel_fwd_layer_destroy. \n");
            return rv;
        }
    }

    for (qual_index = 0; qual_index < udh_fg_info->nof_quals; qual_index++)
    {
        if (udh_fg_info->udh_qual_info[qual_index].qual_type >= BCM_FIELD_FIRST_USER_QUALIFIER_ID)
        {
            rv = bcm_field_qualifier_destroy(unit, udh_fg_info->udh_qual_info[qual_index].qual_type);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_field_qualifier_destroy. \n");
                return rv;
            }
        }
    }

    for (action_index = 0; action_index < udh_fg_info->nof_actions; action_index++)
    {
        if (udh_fg_info->udh_action_info[action_index].name != NULL)
        {
            rv = bcm_field_action_destroy(unit, udh_fg_info->udh_action_info[action_index].action_type);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_field_action_destroy. \n");
                return rv;
            }
        }
    }

    return rv;
}

/** Destroy cascading configuration between IPMF2 and IPMF3 stages. */
int
cint_field_udh_cascading_ipmf2_ipmf3_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF2 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf2_ipmf3_fg_info_array[0], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf2_ipmf3_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF2 and EPMF stages. */
int
cint_field_udh_cascading_ipmf2_epmf_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF2 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf2_epmf_fg_info_array[0], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf2_epmf_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF3 and EPMF stages. */
int
cint_field_udh_cascading_ipmf3_epmf_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf3_epmf_fg_info_array[0], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf3_epmf_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF1 and IPMF3 stages. */
int
cint_field_udh_cascading_ipmf1_ipmf3_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf1_ipmf3_fg_info_array[0], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf1_ipmf3_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF1 and EPMF stages. */
int
cint_field_udh_cascading_ipmf1_epmf_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf1_epmf_fg_info_array[0], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf1_epmf_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy multiple cascading configuration between IPMF1 and IPMF3 stages. */
int
cint_field_udh_cascading_ipmf1_ipmf3_multiple_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf1_ipmf3_multiple_fg_info_array[0], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf1_ipmf3_multiple_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy exhaustive cascading configuration between IPMF1-2 and IPMF3 stages. */
int
cint_field_udh_cascading_ipmf12_ipmf3_exhaustive_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf12_ipmf3_exhaustive_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF2 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf12_ipmf3_exhaustive_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_ipmf12_ipmf3_exhaustive_fg_info_array[2], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy exhaustive cascading configuration between IPMF1-2-3 and EPMF stages. */
int
cint_field_udh_cascading_train_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_train_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF2 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_train_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_train_fg_info_array[2], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_train_fg_info_array[3], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy j1_mode configuration*/
int
cint_field_udh_cascading_j1_mode_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    return rv;
}/** Destroy j1_mode configuration*/

int
cint_field_udh_cascading_j1_mode_stacking_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_stacking_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_stacking_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    return rv;
}

int
cint_field_udh_cascading_j1_mode_udh_stacking_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_udh_stacking_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_udh_stacking_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF2 and EPMF stages J1 mode. */
int
cint_field_udh_cascading_j1_mode_ipmf2_epmf_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF2 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf2_epmf_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf2_epmf_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF2 and EPMF stages J1 mode. */
int
cint_field_udh_cascading_j1_mode_ipmf3_epmf_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf3_epmf_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf3_epmf_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF1-2 and EPMF stages J1 mode combination. */
int
cint_field_udh_cascading_j1_mode_ipmf12_epmf_combination_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf12_epmf_comb_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF2 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf12_epmf_comb_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf12_epmf_comb_fg_info_array[2], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF1-3 and EPMF stages J1 mode combination. */
int
cint_field_udh_cascading_j1_mode_ipmf13_epmf_combination_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf13_epmf_comb_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf13_epmf_comb_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf13_epmf_comb_fg_info_array[2], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF2-3 and EPMF stages J1 mode combination. */
int
cint_field_udh_cascading_j1_mode_ipmf23_epmf_combination_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF2 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf23_epmf_comb_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf23_epmf_comb_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf23_epmf_comb_fg_info_array[2], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF1-3 and EPMF stages J1 mode train. */
int
cint_field_udh_cascading_j1_mode_ipmf13_epmf_train_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf13_epmf_train_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying IPMF3 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf13_epmf_train_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf13_epmf_train_fg_info_array[2], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_group_config\n", rv);
        return rv;
    }

    return rv;
}

/** Destroy cascading configuration between IPMF1 and EPMF stages J1 mode (different sizes). */
int
cint_field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_destroy(
    int unit)
{
    int rv = BCM_E_NONE;

    /** Destroying IPMF1 Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[0], TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    /** Destroying EPMF Field Configuration. */
    rv = cint_field_udh_cascading_destroy(unit, field_udh_cascading_j1_mode_ipmf1_epmf_diff_sizes_fg_info_array[1], FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_udh_cascading_destroy\n", rv);
        return rv;
    }

    return rv;
}

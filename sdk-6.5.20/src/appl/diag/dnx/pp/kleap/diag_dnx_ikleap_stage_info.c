/** \file diag_dnx_ikleap_stage_info.c
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGKLEAPDNX

/*************
 * INCLUDES  *
 */
#include "diag_dnx_ikleap.h"

/*************
 * DEFINES   *
 */

/*************
 * TYPEDEFS  *
 */
/**
 * \brief For each stage set the DBAL_PHYSICAL_TABLE according
 * to the KBR index.
 * Each KBR is Directly connected to one of the Phy. DBs
 */
dbal_physical_tables_e kbr2physical_fwd12[KLEAP_NOF_KBRS_FWD12] = {
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_KBP,
    DBAL_PHYSICAL_TABLE_KBP,
    DBAL_PHYSICAL_TABLE_LEM,
    DBAL_PHYSICAL_TABLE_SEXEM_3,
    DBAL_PHYSICAL_TABLE_SEXEM_2,
    DBAL_PHYSICAL_TABLE_KAPS_1,
    DBAL_PHYSICAL_TABLE_KAPS_1,
    DBAL_PHYSICAL_TABLE_LEM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_KAPS_2,
    DBAL_PHYSICAL_TABLE_KAPS_2,
    DBAL_PHYSICAL_TABLE_KBP,
    DBAL_PHYSICAL_TABLE_KBP
};
dbal_physical_tables_e kbr2physical_vt1[KLEAP_NOF_KBRS_VT1] = {
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_ISEM_1,
    DBAL_PHYSICAL_TABLE_ISEM_1,
    DBAL_PHYSICAL_TABLE_SEXEM_1
};
dbal_physical_tables_e kbr2physical_vt23[KLEAP_NOF_KBRS_VT23] = {
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_ISEM_2,
    DBAL_PHYSICAL_TABLE_ISEM_2,
    DBAL_PHYSICAL_TABLE_SEXEM_1,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM
};
dbal_physical_tables_e kbr2physical_vt45[KLEAP_NOF_KBRS_VT45] = {
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_ISEM_2,
    DBAL_PHYSICAL_TABLE_ISEM_2,
    DBAL_PHYSICAL_TABLE_SEXEM_1,
    DBAL_PHYSICAL_TABLE_TCAM
};
dbal_physical_tables_e kbr2physical_ipmf1[KLEAP_NOF_KBRS_IPMF1] = {
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM
};
dbal_physical_tables_e kbr2physical_ipmf1_initial[KLEAP_NOF_KBRS_IPMF1_INITIAL] = {
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM
};
dbal_physical_tables_e kbr2physical_ipmf2[KLEAP_NOF_KBRS_IPMF2] = {
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_NONE,
};
dbal_physical_tables_e kbr2physical_ipmf3[KLEAP_NOF_KBRS_IPMF3] = {
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM
};
dbal_physical_tables_e kbr2physical_epmf[KLEAP_NOF_KBRS_EPMF] = {
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_TCAM
};

/*
 * \brief For each stage set the key value signal name according to the KBR index.
 * for example: in KBP case - KBR4 uses Key0 signal, KBR5 Key1, KBR16 Key2, KBR17 Key3
 * but it's used in various lookups and shows also if a lookup has a single "key" or not at all (invalid)
 */
interfaces_index_per_phyDb_e kbr2key_sig_fwd12[KLEAP_NOF_KBRS_FWD12] = {
    INVALID_IF,
    INVALID_IF,
    IF_2,
    IF_3,
    IF_0,
    IF_1,
    IF_1,
    SINGLE_IF,
    SINGLE_IF,
    IF_0,
    IF_1,
    IF_0,
    IF_0,
    IF_1,
    IF_0,
    IF_1,
    IF_2,
    IF_3
};

interfaces_index_per_phyDb_e kbr2key_sig_vt1[KLEAP_NOF_KBRS_VT1] = {
    INVALID_IF,
    INVALID_IF,
    SINGLE_IF,
    IF_0,
    IF_1,
    SINGLE_IF
};
interfaces_index_per_phyDb_e kbr2key_sig_vt23[KLEAP_NOF_KBRS_VT23] = {
    INVALID_IF,
    INVALID_IF,
    IF_0,
    IF_1,
    SINGLE_IF,
    IF_0,
    IF_1,
};
interfaces_index_per_phyDb_e kbr2key_sig_vt45[KLEAP_NOF_KBRS_VT45] = {
    INVALID_IF,
    INVALID_IF,
    IF_0,
    IF_1,
    SINGLE_IF,
    SINGLE_IF
};
interfaces_index_per_phyDb_e kbr2key_sig_ipmf1[KLEAP_NOF_KBRS_IPMF1] = {
    INVALID_IF,
    INVALID_IF,
    INVALID_IF,
    INVALID_IF,
    INVALID_IF
};
interfaces_index_per_phyDb_e kbr2key_sig_ipmf1_initial[KLEAP_NOF_KBRS_IPMF1_INITIAL] = {
    INVALID_IF,
    INVALID_IF,
    INVALID_IF,
    INVALID_IF,
    INVALID_IF
};
int kbr2key_sig_ipmf2[KLEAP_NOF_KBRS_IPMF2] = {
    INVALID_IF,
    INVALID_IF,
    INVALID_IF,
    INVALID_IF,
    INVALID_IF
};
int kbr2key_sig_ipmf3[KLEAP_NOF_KBRS_IPMF3] = {
    INVALID_IF,
    INVALID_IF,
    INVALID_IF
};
int kbr2key_sig_epmf[KLEAP_NOF_KBRS_EPMF] = {
    INVALID_IF,
    INVALID_IF,
    INVALID_IF
};

/**************************
 * INIT STAGES PARAMETERS *
 */
stage_t ikleap_stages[KLEAP_NOF_STAGES] = {
        /** VTT_1 */
    {
     SINGLE_STAGE_TYPE,
     {"VTT1"},
     {1},
     {DBAL_FIELD_TYPE_DEF_VT1_CONTEXT_ID},
     KLEAP_NOF_KBRS_VT1,
     kbr2physical_vt1,
     kbr2key_sig_vt1,
     KLEAP_NOF_FFC_VT1,
     DBAL_NOF_TABLES,
     DBAL_TABLE_KLEAP_VT1_KBR_INFO,
     DBAL_TABLE_KLEAP_VT1_PD_INFO,
     DBAL_FIELD_KBR_IDX,
     DBAL_FIELD_CONTEXT_PROFILE,
     },

       /** VTT_2_3 */
    {
     DOUBLE_STAGE_TYPE,
     {"VTT2", "VTT3"},
     {2, 3},
     {DBAL_FIELD_TYPE_DEF_VT2_CONTEXT_ID, DBAL_FIELD_TYPE_DEF_VT3_CONTEXT_ID},
     KLEAP_NOF_KBRS_VT23,
     kbr2physical_vt23,
     kbr2key_sig_vt23,
     KLEAP_NOF_FFC_VT23,
     DBAL_TABLE_KLEAP_VT23_RESOURCES_MAPPING,
     DBAL_TABLE_KLEAP_VT23_KBR_INFO,
     DBAL_TABLE_KLEAP_VT23_PD_INFO,
     DBAL_FIELD_KBR_IDX,
     DBAL_FIELD_CONTEXT_PROFILE,
     },

    /** VTT_4_5 */
    {
     DOUBLE_STAGE_TYPE,
     {"VTT4", "VTT5"},
     {4, 5},
     {DBAL_FIELD_TYPE_DEF_VT4_CONTEXT_ID, DBAL_FIELD_TYPE_DEF_VT5_CONTEXT_ID},
     KLEAP_NOF_KBRS_VT45,
     kbr2physical_vt45,
     kbr2key_sig_vt45,
     KLEAP_NOF_FFC_VT45,
     DBAL_TABLE_KLEAP_VT45_RESOURCES_MAPPING,
     DBAL_TABLE_KLEAP_VT45_KBR_INFO,
     DBAL_TABLE_KLEAP_VT45_PD_INFO,
     DBAL_FIELD_KBR_IDX,
     DBAL_FIELD_CONTEXT_PROFILE,
     },

    /** FWD_1_2 */
    {
     DOUBLE_STAGE_TYPE,
     {"FWD1", "FWD2"},
     {6, 7},
     {DBAL_FIELD_TYPE_DEF_FWD1_CONTEXT_ID, DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID},
     KLEAP_NOF_KBRS_FWD12,
     kbr2physical_fwd12,
     kbr2key_sig_fwd12,
     KLEAP_NOF_FFC_FWD12,
     DBAL_TABLE_KLEAP_FWD12_RESOURCES_MAPPING,
     DBAL_TABLE_KLEAP_FWD12_KBR_INFO,
     DBAL_TABLE_KLEAP_FWD12_PD_INFO,
     DBAL_FIELD_FWD12_KBR_ID,
     DBAL_FIELD_CONTEXT_PROFILE,
     },

    /** IPMF1 */
    {
     SINGLE_STAGE_TYPE,
     {"IPMF1"},
     {0},
     {DBAL_FIELD_TYPE_DEF_EMPTY},
     KLEAP_NOF_KBRS_IPMF1,
     kbr2physical_ipmf1,
     kbr2key_sig_ipmf1,
     KLEAP_NOF_FFC_IPMF1,
     DBAL_NOF_TABLES,
     DBAL_TABLE_KLEAP_IPMF1_KBR_INFO,
     DBAL_NOF_TABLES,
     DBAL_FIELD_FIELD_KEY,
     DBAL_FIELD_FIELD_PMF_CTX_ID,
     },

        /** IPMF1_INITIAL */
    {
     SINGLE_STAGE_TYPE,
     {"IPMF1_INITIAL"},
     {0},
     {DBAL_FIELD_TYPE_DEF_EMPTY},
     KLEAP_NOF_KBRS_IPMF1_INITIAL,
     kbr2physical_ipmf1_initial,
     kbr2key_sig_ipmf1_initial,
     KLEAP_NOF_FFC_IPMF1_INITIAL,
     DBAL_NOF_TABLES,
     DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO,
     DBAL_NOF_TABLES,
     DBAL_FIELD_FIELD_KEY,
     DBAL_FIELD_FIELD_PMF_CTX_ID,
     },

            /** IPMF2 */
    {
     SINGLE_STAGE_TYPE,
     {"IPMF2"},
     {0},
     {DBAL_FIELD_TYPE_DEF_EMPTY},
     KLEAP_NOF_KBRS_IPMF2,
     kbr2physical_ipmf2,
     kbr2key_sig_ipmf2,
     KLEAP_NOF_FFC_IPMF2,
     DBAL_NOF_TABLES,
     DBAL_TABLE_KLEAP_IPMF2_KBR_INFO,
     DBAL_NOF_TABLES,
     DBAL_FIELD_FIELD_KEY,
     DBAL_FIELD_FIELD_PMF_CTX_ID,
     },

                /** IPMF3 */
    {
     SINGLE_STAGE_TYPE,
     {"IPMF3"},
     {0},
     {DBAL_FIELD_TYPE_DEF_EMPTY},
     KLEAP_NOF_KBRS_IPMF3,
     kbr2physical_ipmf3,
     kbr2key_sig_ipmf3,
     KLEAP_NOF_FFC_IPMF3,
     DBAL_NOF_TABLES,
     DBAL_TABLE_KLEAP_IPMF3_KBR_INFO,
     DBAL_NOF_TABLES,
     DBAL_FIELD_FIELD_KEY,
     DBAL_FIELD_FIELD_PMF_CTX_ID,
     },

                    /** EPMF */
    {
     SINGLE_STAGE_TYPE,
     {"EPMF"},
     {0},
     {DBAL_FIELD_TYPE_DEF_EMPTY},
     KLEAP_NOF_KBRS_EPMF,
     kbr2physical_epmf,
     kbr2key_sig_epmf,
     KLEAP_NOF_FFC_EPMF,
     DBAL_NOF_TABLES,
     DBAL_TABLE_KLEAP_E_PMF_KBR_INFO,
     DBAL_NOF_TABLES,
     DBAL_FIELD_FIELD_KEY,
     DBAL_FIELD_FIELD_PMF_CTX_ID,
     },
};

fixed_key_lookup_t fixed_key_lookup[NOF_FIXED_KEY_LOOKUP] = {
    /*
     * Learning fixed key
     */
    {
     KLEAP_STAGE_FWD12,
     0,
     DBAL_TABLE_INGRESS_IRPP_FWD1_CONTEXT_PROPERTIES,
     DBAL_FIELD_FWD1_CONTEXT_ID,
     DBAL_FIELD_LEARNING_FIXED_KEY_ENABLE,
     1,
     {6},
     "Learning_Key",
     "Learning_Appdb",
     },

    /*
     * OAM identification fixed key
     */
    {
     KLEAP_STAGE_FWD12,
     0,
     DBAL_TABLE_INGRESS_IRPP_FWD1_CONTEXT_PROPERTIES,
     DBAL_FIELD_FWD1_CONTEXT_ID,
     DBAL_FIELD_LEARNING_FIXED_KEY_ENABLE,
     2,
     {12, 13},
     "Traps_Key",
     "Traps_Appdb",
     },
};

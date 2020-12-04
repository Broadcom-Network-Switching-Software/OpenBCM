/*! \file diag_dnx_ikleap.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_IKLEAP_H_INCLUDED
#define DIAG_DNX_IKLEAP_H_INCLUDED

#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>

#include <appl/diag/diag.h>
#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_signals.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>

#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_pparse.h>
#include <soc/sand/sand_signals.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include "diag_dnx_ikleap_layers_translation_to_pparse_definition.h"

/*************
 * INCLUDES  *
 */

/*************
 *  DEFINES  *
 */
/*****************************
 *  General KLEAP Definitions
 */
/** \brief Number of context per stage */
#define DIAG_DNX_KLEAP_NOF_CONTEXTS                 64

/** \brief ARR index size in bits */
#define DIAG_DNX_KLEAP_ARR_INDEX_SIZE               6

/** \brief Size in bits of packet header */
#define DIAG_DNX_KLEAP_PACKET_HEADER_SIZE_IN_BITS   1152

/** \brief Max size for FFC bitmap in U32 */
#define DIAG_DNX_KLEAP_FFC_U32_BITMAP_SIZE          2

/** \brief Default value for arg context */
#define DIAG_DNX_KLEAP_DEFAULT_CONTEXT_INPUT        0xAAAA

/** \brief Default value for arg context as string */
#define DIAG_DNX_KLEAP_DEFAULT_CONTEXT_AS_STRING    "0xAAAA"

/** \brief Short string max length */
#define DIAG_DNX_KLEAP_MAX_SHORT_STRING             64

/** \brief Long string max length */
#define DIAG_DNX_KLEAP_MAX_STRING_LENGTH            1024

/** \brief Max size in words of read signals */
#define MAX_NUM_OF_WORDS_SIGNAL                     20

/** \brief Max name length in chars for signals */
#define SIGNALS_MAX_NAME_LENGTH                     30

/** \brief KBR Key bit length in units of uint32 - 160bits */
#define KEY_BUILD_SIZE_PER_KBR_IN_U32               5

#define NOF_FIXED_KEY_LOOKUP                        2

/************************************
 * General HW definitions per stage
 */
 /** FWD12 STAGE */
#define KLEAP_NOF_KBRS_FWD12            18
#define KLEAP_NOF_FFC_FWD12             38
#define KLEAP_NOF_FFC_G_FWD12           5
#define KLEAP_NOF_PD_FWD12              6
 /** VT1 STAGE */
#define KLEAP_NOF_KBRS_VT1              6
#define KLEAP_NOF_FFC_VT1               10
#define KLEAP_NOF_FFC_G_VT1             1
#define KLEAP_NOF_PD_VT1                3
 /** VT23 STAGE */
#define KLEAP_NOF_KBRS_VT23             7
#define KLEAP_NOF_FFC_VT23              20
#define KLEAP_NOF_FFC_G_VT23            4
#define KLEAP_NOF_PD_VT23               6
 /** VT45 STAGE */
#define KLEAP_NOF_KBRS_VT45             6
#define KLEAP_NOF_FFC_VT45              20
#define KLEAP_NOF_FFC_G_VT45            4
#define KLEAP_NOF_PD_VT45               6
 /** IPMF1 STAGE */
#define KLEAP_NOF_KBRS_IPMF1            5
#define KLEAP_NOF_FFC_IPMF1             64
#define KLEAP_NOF_FFC_G_IPMF1           1
#define KLEAP_NOF_PD_IPMF1              0
 /** IPMF1_INTIAL STAGE */
#define KLEAP_NOF_KBRS_IPMF1_INITIAL    5
#define KLEAP_NOF_FFC_IPMF1_INITIAL     64
#define KLEAP_NOF_FFC_G_IPMF1_INITIAL   1
#define KLEAP_NOF_PD_IPMF1_INITIAL      0
 /** IPMF2 STAGE */
#define KLEAP_NOF_KBRS_IPMF2            5
#define KLEAP_NOF_FFC_IPMF2             32
#define KLEAP_NOF_FFC_G_IPMF2           1
#define KLEAP_NOF_PD_IPMF2              0
 /** IPMF3 STAGE */
#define KLEAP_NOF_KBRS_IPMF3            3
#define KLEAP_NOF_FFC_IPMF3             32
#define KLEAP_NOF_FFC_G_IPMF3           1
#define KLEAP_NOF_PD_IPMF3              0
 /** EPMF STAGE */
#define KLEAP_NOF_KBRS_EPMF             3
#define KLEAP_NOF_FFC_EPMF              20
#define KLEAP_NOF_FFC_G_EPMF            1
#define KLEAP_NOF_PD_EPMF               0

#define KLEAP_NOF_PDS_MAX KLEAP_NOF_PD_FWD12
#define KLEAP_NOF_KBRS_MAX KLEAP_NOF_KBRS_FWD12

/*************
 *  MACROES  *
 */

/*************
 *  TYPDEFS  *
 */

/** \brief List of KLEAP stages */
typedef enum
{
    KLEAP_STAGE_VT1,
    KLEAP_STAGE_VT23,
    KLEAP_STAGE_VT45,
    KLEAP_STAGE_FWD12,
    KLEAP_STAGE_IPMF1,
    KLEAP_STAGE_IPMF1_INITIAL,
    KLEAP_STAGE_IPMF2,
    KLEAP_STAGE_IPMF3,
    KLEAP_STAGE_EPMF,

    KLEAP_NOF_STAGES
} kleap_stages_e;

/**  \brief interfaces index per PhyDb*/
typedef enum
{
    INVALID_IF = -2,
    SINGLE_IF = -1,
    IF_0 = 0,
    IF_1 = 1,
    IF_2 = 2,
    IF_3 = 3,
    MAX_NUM_INTERFACES_PER_PHY_DB
} interfaces_index_per_phyDb_e;

/**  \brief possible KBR values */
typedef enum
{
    KBR_0 = 0,
    KBR_1 = 1,
    KBR_2 = 2,
    KBR_3 = 3,
    KBR_4 = 4,
    KBR_5 = 5,
    KBR_6 = 6,
    KBR_7 = 7,
    KBR_8 = 8,
    KBR_9 = 9,
    KBR_10 = 10,
    KBR_11 = 11,
    KBR_12 = 12,
    KBR_13 = 13,
    KBR_14 = 14,
    KBR_15 = 15,
    KBR_16 = 16,
    KBR_17 = 17,
    MAX_NUM_KBR
} kbr_value_e;

/**
 * \brief Indication for PDs mapping type. float or staged
 */

 /** \brief Types of KLEAP stage. single or double stage */
typedef enum
{
    SINGLE_STAGE_TYPE,
    DOUBLE_STAGE_TYPE,

    KLEAP_NOF_STAGE_TYPES
} kleap_stage_types_e;

/** \brief Generic type of a single KLEAP stage. Contains all info per stage.
 *   The stage struct would contain 'Info' only (non advertised), 'Visibility' and mutual to both types of information.
 *   In 'Visibility' mode, 'Info' types of information would be obscured.
 */
typedef struct stage_s
{
    /**+++++++++++++ Visibility +++++++++++++++*/
    kleap_stage_types_e stage_type;
    /**+++++++++++++ Visibility +++++++++++++++*/
    /** Sub stages name list per stage */
    char *kleap_sub_stages_names[KLEAP_NOF_STAGE_TYPES];
    /**+++++++++++++ Visibility +++++++++++++++*/
    /** map stage and sub-stage to stage index in pipe */
    uint32 stage_index_in_pipe[KLEAP_NOF_STAGE_TYPES];
    /**+++++++++++++ Visibility +++++++++++++++*/
    /** Map stage and sub-stage to the enum of its context in dbal */
    dbal_field_types_defs_e dbal_context_enum[KLEAP_NOF_STAGE_TYPES];

    /**+++++++++++++ Visibility +++++++++++++++*/
    /** KLEAP stage HW properties */
    uint32 nof_kbrs;
    /**+++++++++++++ Visibility +++++++++++++++*/
    /** For each stage set the DBAL_PHYSICAL_TABLE according to the KBR index. Each KBR is Directly connected to one of the Phy. tables */
    dbal_physical_tables_e *kbr2physical;
    /**+++++++++++++ Visibility +++++++++++++++*/
    interfaces_index_per_phyDb_e *kbr_if_id;
    /**+++++++++++++ Visibility +++++++++++++++*/
    uint32 nof_ffc;
    /**+++++++++++++ Visibility +++++++++++++++*/
    dbal_tables_e dbal_table_resource_mapping;
    /**+++++++++++++ Visibility +++++++++++++++*/
    dbal_tables_e dbal_table_kbr_info;
    /**-------------- INFO -------------------*/
    dbal_tables_e dbal_table_pd_info;
    /**+++++++++++++ Visibility +++++++++++++++*/
    /** DBAL field ID which hold the kbr_idx for each stage */
    dbal_fields_e dbal_kbr_idx_field_per_stage;
    /**+++++++++++++ Visibility +++++++++++++++*/
    /** DBAL field ID which holds the context_profile for each stage */
    dbal_fields_e dbal_context_profile_field_per_stage;
} stage_t;

typedef struct fixed_key_lookup_s
{
    kleap_stages_e stage;
    int sub_stage;
    dbal_tables_e context_enablers_table;
    dbal_fields_e key_field;
    dbal_fields_e context_enablers_field;
    int nof_interfaces;
    int kbr_idx[4];
    char *key_sig_name;
    char *app_sig_name;
} fixed_key_lookup_t;

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_ikleap_man;
extern sh_sand_cmd_t sh_dnx_ikleap_cmds[];
extern sh_sand_man_t sh_dnx_ekleap_man;
extern sh_sand_cmd_t sh_dnx_ekleap_cmds[];

/**************************
 * FUNCTIONS DECLARATION  *
 */

#endif /* DIAG_DNX_KLEAP_H_INCLUDED */

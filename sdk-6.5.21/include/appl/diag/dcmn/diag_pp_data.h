/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    diag_pp_data.h
 * Purpose:    Types and structures used import/export PP data
 */

#ifndef __DIAG_PP_DATA_H
#define __DIAG_PP_DATA_H

#include <shared/utilex/utilex_rhlist.h>
#include <soc/dpp/PPC/ppc_api_fp.h>

typedef struct {
    char        name[RHNAME_MAX_SIZE];
    int         offset;
    int         size;
    int         header; /* header type for packet or LSB/MSB for signal */
    int         valid;
    uint16      ce;
    int         ce_type;
    char        ce_name[RHNAME_MAX_SIZE];
} qualifier_t;

#define PARSER_PROGRAM_SEGMENT_SIZE 64
#define PARSER_PROGRAM_SEGMENT_NUM  32
#define PARSER_PROGRAM_FIN          0x00 /* Execute instruction's command and finish */
#define PARSER_PROGRAM_END          0x7f /* End and do not execute the instruction's command */

#define PP_INVLAID_VALUE            -1

#define CE16_TYPE                   16
#define CE32_TYPE                   32

#define INTERNAL_BUFFER_CE16_SHIFT  16
#define INTERNAL_BUFFER_CE32_SHIFT  32
#define INTERNAL_BUFFER_QUALIFIER   0x8

#define VTT_QUALIFIER_NUM           12
#define FLP_QUALIFIER_NUM           32

#define BOOLSTR(mc_value)         ((mc_value == 0) ? "No" : "Yes")

/*
 * Defines
 */
#define DPP_EXPORT_MAX_STAGE_NUM        4
#define DPP_EXPORT_PP_PRESEL_MAX        48
#define DPP_EXPORT_PP_BUNDLE_MAX        DPP_EXPORT_PP_TABLE_MAX
#define DPP_EXPORT_PP_TABLE_MAX         SOC_DPP_DBAL_SW_NOF_TABLES
#define DPP_EXPORT_PP_BUNDLE_TABLE_MAX  32

#define DPP_EXPORT_CHAR_MAX_LENGTH 50

/*
 * Structs
 */
typedef struct dpp_export_pp_table_key_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
} dpp_export_pp_table_key_t;

typedef struct dpp_export_pp_table_action_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
} dpp_export_pp_table_action_t;

typedef struct dpp_export_pp_table_info_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
    char                         database[DPP_EXPORT_CHAR_MAX_LENGTH];
    dpp_export_pp_table_key_t    keys[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    dpp_export_pp_table_action_t actions[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    int                          priority;
    int                          is_static;
} dpp_export_pp_table_info_t;

typedef struct dpp_export_pp_bundle_table_s {
    dpp_export_pp_table_info_t   *table;
    int                          lookup;
    int                          is_static;
} dpp_export_pp_bundle_table_t;

typedef struct dpp_export_pp_bundle_info_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
    dpp_export_pp_bundle_table_t tables[DPP_EXPORT_PP_BUNDLE_TABLE_MAX];
    int                          nof_tables;
    int                          is_static;
    int                          selected;
} dpp_export_pp_bundle_info_t;

typedef struct dpp_presel_qual_info_s {
    rhentry_t   entry;
    uint32      data;
    uint32      mask;
    int         size;
} dpp_presel_qual_info_t;

typedef struct dpp_export_pp_presel_info_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
    char                         app_type[DPP_EXPORT_CHAR_MAX_LENGTH];
    dpp_export_pp_bundle_info_t  *bundle;
    int                          is_static;
    int                          selected;
    rhlist_t                     *qual_list;
} dpp_export_pp_presel_info_t;

typedef struct dpp_export_pp_stage_info_s {
    char                         name[DPP_EXPORT_CHAR_MAX_LENGTH];
    dpp_export_pp_presel_info_t  presels[DPP_EXPORT_PP_PRESEL_MAX];
    dpp_export_pp_bundle_info_t  bundles[DPP_EXPORT_PP_BUNDLE_MAX];
    dpp_export_pp_table_info_t   tables[DPP_EXPORT_PP_TABLE_MAX];
} dpp_export_pp_stage_info_t;

typedef struct dpp_export_pp_info_s {
    dpp_export_pp_stage_info_t   stages[SOC_PPC_NOF_FP_DATABASE_STAGES];
} dpp_export_pp_info_t;

int dpp_export_pp(int unit, char *stage, char *filename);

int dpp_dump_vt(int unit,  int core, dpp_export_pp_stage_info_t *stage);
int dpp_dump_tt(int unit,  int core, dpp_export_pp_stage_info_t *stage);
int dpp_dump_flp(int unit, int core, dpp_export_pp_stage_info_t *stage);

#endif /* __DIAG_PP_DATA_H */

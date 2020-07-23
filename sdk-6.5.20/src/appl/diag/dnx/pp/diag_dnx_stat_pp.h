/*! \file diag_dnx_stat_pp.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_STAT_PP_H_INCLUDED
#define DIAG_DNX_STAT_PP_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <appl/diag/diag.h>
#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/sand/sand_signals.h>
#include <soc/dnx/dbal/dbal.h>
#include <appl/diag/sand/diag_sand_signals.h>
#include <shared/shrextend/shrextend_debug.h>
/*************
 *  DEFINES  *
 */

#define DIAG_DNX_STAT_PP_STAT_ID_SIZE 20
#define DIAG_DNX_STAT_PP_STAT_PROFILE_SIZE 5
#define DIAG_DNX_STAT_PP_IRPP_STAT_METADATA_SIZE 16

/*************
 *  MACROES  *
 */

/*************
 * TYPEDEFS  *
 */
 /**
 * \brief
 * Typedef:    stat_pp_irpp_obj_type_idx_t
 * Purpose:    Mapping of (up to ipmf) statistic object types
 *             inside IPPC_MAP_STATISTIC_OBJECT
 */
typedef enum stat_pp_irpp_obj_type_e
{
    STAT_PP_IRPP_OBJ_IDX_VTA_LIF = 13,  /* vta terminated lif */
    STAT_PP_IRPP_OBJ_IDX_VTB_LIF = 12,  /* vtb terminated lif */
    STAT_PP_IRPP_OBJ_IDX_VTC_LIF = 11,  /* vtc terminated lif */
    STAT_PP_IRPP_OBJ_IDX_VTD_LIF = 10,  /* vtd terminated lif */
    STAT_PP_IRPP_OBJ_IDX_VTE_LIF = 9,   /* vte terminated lif */
    STAT_PP_IRPP_OBJ_IDX_VTA_TERM_FODO = 8,     /* vta terminated fodo */
    STAT_PP_IRPP_OBJ_IDX_VTE_TERM_FODO = 7,     /* vte terminated fodo */
    STAT_PP_IRPP_OBJ_IDX_FWD1_FWD_FODO = 6,     /* fwd1 forward fodo */
    STAT_PP_IRPP_OBJ_IDX_FWD2_FWD_FODO = 5,     /* fwd2 forward fodo */
    STAT_PP_IRPP_OBJ_IDX_FWD1_LOOKUP_SRC = 4,   /* fwd1 lookup src */
    STAT_PP_IRPP_OBJ_IDX_FWD2_LOOKUP_SRC = 3,   /* fwd2 lookup src */
    STAT_PP_IRPP_OBJ_IDX_FWD1_LOOKUP_DST = 2,   /* fwd1 lookup dst */
    STAT_PP_IRPP_OBJ_IDX_FWD2_LOOKUP_DST = 1,   /* fwd2 lookup dst */
    STAT_PP_IRPP_OBJ_IDX_IPMF_PP_PORT = 0,      /* iPMF in PP port */
} stat_pp_irpp_obj_type_t;

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_stat_pp_man;
extern sh_sand_cmd_t sh_dnx_stat_pp_cmds[];

/*************
 * FUNCTIONS *
 */

#endif /* DIAG_DNX_STAT_PP_H_INCLUDED */

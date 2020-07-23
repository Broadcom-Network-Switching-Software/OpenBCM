/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef INCLUDE_SOC_DNX_PP_STAGE_H_
#define INCLUDE_SOC_DNX_PP_STAGE_H_

#include <shared/shrextend/shrextend_error.h>
#include <sal/types.h>

#define MAX_DEBUG_BUS_SIZE_BITS      2560
#define MAX_DEBUG_BUS_SIZE_WORDS     BITS2WORDS(MAX_DEBUG_BUS_SIZE_BITS)

typedef struct
{
    char *name;
    char *direction;
} dnx_pp_block_info_t;

typedef enum
{

    DNX_PP_STAGE_INVALID = 0,
    DNX_PP_STAGE_FIRST = 1,

    DNX_PP_STAGE_NIF = DNX_PP_STAGE_FIRST,

    DNX_PP_STAGE_PRT,

    DNX_PP_STAGE_IRPP_FIRST,

    DNX_PP_STAGE_IPARSER = DNX_PP_STAGE_IRPP_FIRST,

    DNX_PP_STAGE_LLR,

    DNX_PP_STAGE_VTT1,

    DNX_PP_STAGE_VTT2,

    DNX_PP_STAGE_VTT3,

    DNX_PP_STAGE_VTT4,

    DNX_PP_STAGE_VTT5,

    DNX_PP_STAGE_IFWD1,

    DNX_PP_STAGE_IFWD2,

    DNX_PP_STAGE_IPMF1,

    DNX_PP_STAGE_IPMF2,

    DNX_PP_STAGE_FER,

    DNX_PP_STAGE_IPMF3,

    DNX_PP_STAGE_LBP,

    DNX_PP_STAGE_ITMR,

    DNX_PP_STAGE_ITM,

    DNX_PP_STAGE_ITPP,

    DNX_PP_STAGE_ERPP_FIRST,

    DNX_PP_STAGE_FABRIC = DNX_PP_STAGE_ERPP_FIRST,

    DNX_PP_STAGE_ERPARSER,

    DNX_PP_STAGE_ERPRP,

    DNX_PP_STAGE_FILTER,

    DNX_PP_STAGE_EPMF,

    DNX_PP_STAGE_ETMR,

    DNX_PP_STAGE_ETM,

    DNX_PP_STAGE_ETPP_FIRST,

    DNX_PP_STAGE_APPLET = DNX_PP_STAGE_ETPP_FIRST,

    DNX_PP_STAGE_ETPARSER,

    DNX_PP_STAGE_ETPRP1,

    DNX_PP_STAGE_ETPRP2,

    DNX_PP_STAGE_TERM,

    DNX_PP_STAGE_EFWD,

    DNX_PP_STAGE_ENC1,

    DNX_PP_STAGE_ENC2,

    DNX_PP_STAGE_ENC3,

    DNX_PP_STAGE_ENC4,

    DNX_PP_STAGE_ENC5,

    DNX_PP_STAGE_TRAP,

    DNX_PP_STAGE_BTC,

    DNX_PP_STAGE_ALIGNER,

    DNX_PP_STAGE_EPNI,

    DNX_PP_STAGE_ACE,

    DNX_PP_STAGE_NOF
} dnx_pp_stage_e;

typedef enum
{

    DNX_PP_DB_INVALID = 0,
    DNX_PP_DB_FIRST = 1,

    DNX_PP_DB_INLIF1 = DNX_PP_DB_FIRST,

    DNX_PP_DB_INLIF2,

    DNX_PP_DB_VSI_DB,

    DNX_PP_DB_TCAM,

    DNX_PP_DB_LEXEM,

    DNX_PP_DB_GLEM,

    DNX_PP_DB_NOF
} dnx_pp_db_e;

typedef enum
{

    DNX_PP_BLOCK_INVALID = 0,
    DNX_PP_BLOCK_FIRST = 1,

    DNX_PP_BLOCK_IRPP = DNX_PP_BLOCK_FIRST,

    DNX_PP_BLOCK_ITPP,

    DNX_PP_BLOCK_ERPP,

    DNX_PP_BLOCK_ETPP,

    DNX_PP_BLOCK_NOF
} dnx_pp_block_e;

#define DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block_m)                                                                \
    if ((asic_block_m < DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA) || (asic_block_m > DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC))  \
    {                                                                                                               \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bad asic block id:%d\n", asic_block_m);                                      \
    }

shr_error_e dnx_pp_get_block_id(
    int unit,
    int stage,
    int *block);

int dnx_pp_stage_is_kleap(
    int unit,
    dnx_pp_stage_e stage);

char *dnx_pp_stage_name(
    int unit,
    dnx_pp_stage_e pp_stage);

shr_error_e dnx_pp_stage_string_to_id(
    int unit,
    char *pp_stage_name,
    dnx_pp_stage_e * pp_stage_id);

shr_error_e dnx_pp_stage_pema_id(
    int unit,
    char *pp_stage_name,
    int *pema_id_p);

char *dnx_pp_block_name(
    int unit,
    dnx_pp_block_e pp_block);

shr_error_e dnx_pp_block_string_to_id(
    int unit,
    char *pp_block_name,
    dnx_pp_block_e * pp_block_id_p);

char *dnx_pp_block_direction(
    int unit,
    dnx_pp_block_e pp_block);

char *dnx_pp_stage_block_name(
    int unit,
    dnx_pp_stage_e stage);

shr_error_e dnx_pp_stage_kbr_resolve(
    int unit,
    char *stage_n_in,
    char *stage_n_out);

#endif

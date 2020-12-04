/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    diag_dnx_utils.h
 * Purpose: Misc utilities for the usage by dnx shell commands
 */

#ifndef _INCLUDE_DNX_UTILS_H
#define _INCLUDE_DNX_UTILS_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dnx_visibility.h>
#include <sal/compiler.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>

/*
 * Macro
 * {
 */

#define DNX_PP_BLOCK_IS_READY_CONT(core_mcr, block_mcr, log_mcr)                                                    \
        if (dnx_debug_block_is_ready(unit, core_mcr, block_mcr) != _SHR_E_NONE)                                     \
        {                                                                                                           \
            if(log_mcr)                                                                                             \
            {                                                                                                       \
                LOG_CLI(("No packet detected on %s(%s) for core=%d\n",                                              \
                        dnx_pp_block_name(unit, block_mcr), dnx_pp_block_direction(unit, block_mcr), core_mcr));    \
            }                                                                                                       \
            continue;                                                                                               \
        }

#define DNX_PP_BLOCK_IS_READY_EXIT(core_mcr, block_mcr, log_mcr)                                                    \
        if (dnx_debug_block_is_ready(unit, core_mcr, block_mcr) != _SHR_E_NONE)                                     \
        {                                                                                                           \
            if(log_mcr)                                                                                             \
            {                                                                                                       \
                LOG_CLI(("No packet detected on %s(%s) for core=%d\n",                                              \
                        dnx_pp_block_name(unit, block_mcr), dnx_pp_block_direction(unit, block_mcr), core_mcr));    \
            }                                                                                                       \
            SHR_EXIT();                                                                                             \
        }

/**
 * \brief - iterate over the relevant cores
 * if core_id is BCM_CORE_ALL will iterate over all supported cores.
 * Otherwise the iteration will be over the given core.
 */
#ifndef ADAPTER_SERVER_MODE
#define SH_DNX_CORES_ITER(unit, core_mcr, core_id_mcr) \
    DNXCMN_CORES_ITER(unit, core_mcr, core_id_mcr)
#else
#define SH_DNX_CORES_ITER(unit, core_mcr, core_id_mcr) \
    for(core_id_mcr = ((core_mcr == BCM_CORE_ALL) ? 0 : core_mcr);\
        core_id_mcr < ((core_mcr == BCM_CORE_ALL) ? 1 : (core_mcr + 1));\
        core_id_mcr++)
#endif

/*
 * Take ABS from subtraction of 2 numbers and calculate result percentage from first
 */
#define DNX_64_ABS_DEVIATION(first_u64, second_u64, result_u64)                 \
    if (COMPILER_64_GT(first_u64, second_u64))                                  \
    {                                                                           \
        COMPILER_64_COPY(result_u64, first_u64);                                \
        COMPILER_64_SUB_64(result_u64, second_u64);                             \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        COMPILER_64_COPY(result_u64, second_u64);                               \
        COMPILER_64_SUB_64(result_u64, first_u64);                              \
    }                                                                           \
    COMPILER_64_UMUL_32(result_u64, 100);                                       \
    COMPILER_64_UDIV_64(result_u64, first_u64)

/*
 * }
 */
/*
 * \brief - Check if TDM is supported and enabled in either mode
 * \return
 *      _SHR_E_UNIT - if TDM is unavailable
 *      _SHR_E_NONE - TDM Available
 */
shr_error_e sh_dnx_is_tdm_available(
    int unit,
    rhlist_t * void_list);

#endif /* _INCLUDE_DNX_UTILS_H */

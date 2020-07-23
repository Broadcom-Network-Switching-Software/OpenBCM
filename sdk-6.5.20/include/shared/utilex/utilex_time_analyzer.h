/** \file utilex_time_analyzer.h
 * 
 * Provide the a utlity that helps to run a sequence backward and forward.
 * Adittional tools provided:
 * * Sequence Logging
 * * Time tracking
 * * Time testing
 * * Tests for memory leaks
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_TIME_ANALYZER_H_INCLUDED
#define UTILEX_TIME_ANALYZER_H_INCLUDED
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/time.h>

/*
 * }
 */
/*
 * MACROs
 * {
 */
/**
 * \brief - max number of modules to be used
 */
#define UTILEX_TIME_ANALYZER_MAX_NOF_MODULES    (100)

/**
 * \brief - time analyzer state
 */
typedef enum
{
    /** do not count */
    UTILEX_TIME_ANALYZER_MODE_OFF = 0x0,
    /** clear and dump table for every step */
    UTILEX_TIME_ANALYZER_MODE_PER_INIT_STEP = 0x1,
    /** accumulate resources for all steps to be dump when init done */
    UTILEX_TIME_ANALYZER_MODE_GLOBAL = 0x2
} utilex_time_anayzer_mode_e;

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */
/**
 * \brief - statistics for diagnostic
 */
typedef struct
{
    /** name of operation */
    char *name;
    /** total time of all operation occurrences */
    uint32 time;
    /** number of operation occurrences */
    int occurences;
} utilex_time_analyzer_diag_info_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */
/**
 * \brief - allocate memory and initialize time analyzer tool
 * \param [in] unit - unit #.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_time_analyzer_init(
    int unit);
/**
 * \brief - free time analyzer tool resources
 * \param [in] unit - unit #.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_time_analyzer_deinit(
    int unit);

/**
 * \brief - add a module / operation to capture statistics for
 * \param [in] unit - unit #.
 * \param [in] module_id - module index
 * \param [in] module_name - name of the module
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_time_analyzer_module_add(
    int unit,
    int module_id,
    char *module_name);
/**
 * \brief - remove a module / operation
 * \param [in] unit - unit #.
 * \param [in] module_id - module index
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_time_analyzer_module_remove(
    int unit,
    int module_id);
/**
 * \brief - operation start - start measure time
 * \param [in] unit - unit #.
 * \param [in] module_id - module index
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void utilex_time_analyzer_module_start(
    int unit,
    int module_id);
/**
 * \brief - operation end - stop measure time and add interval interval to DB
 * \param [in] unit - unit #.
 * \param [in] module_id - module index
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
void utilex_time_analyzer_module_stop(
    int unit,
    int module_id);
/**
 * \brief - clear statistics
 * \param [in] unit - unit #.
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_time_analyzer_clear(
    int unit);
/**
 * \brief - get statistics for all modules / operations
 * \param [in] unit - unit #.
 * \param [in] max_nof_modules - max nof of modules in array 'diag_info'.
 * \param [out] diag_info - statistics per module / operation
 * \param [out] nof_modules - actual nof_modules with collected statistics
 * \return
 *   void
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_time_analyzer_diag_info_get(
    int unit,
    int max_nof_modules,
    utilex_time_analyzer_diag_info_t * diag_info,
    int *nof_modules);

/*
 * }
 */

#endif /* UTILEX_TIME_ANALYZER_H_INCLUDED */

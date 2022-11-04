/*
 * $Id: cint_logger_api.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger_api.c
 * Purpose:     API logger APIs.
 *
 */

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_APPL_CINTAPILOGGER

#include <shared/bsl.h>
#include <shared/bslnames.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/api_logger.h>
#include "cint_variables.h"
#include "cint_logger.h"

/*
 * Enable/Disable the API Logger
 *
 * Parameters
 *  [in] enable
 *    Boolean indicating enable (TRUE) or disable (FALSE)
 *
 * Returns
 *   BCM_E_NONE
 */
int
bcm_api_logger_enable_set (int enable)
{
    BCM_INIT_FUNC_DEFS_NO_UNIT;

    CINT_LOGGER_LOCK;

    Cint_logger_cfg.enabled = (enable) ? 1 : 0;

    CINT_LOGGER_UNLOCK;

    BCM_FUNC_RETURN_NO_UNIT;
}

/*
 * Get the Enabled/Disabled state of the API Logger
 *
 * Parameters
 *  [out] enable
 *    Boolean indicating Enabled (TRUE) or Disabled (FALSE)
 *
 * Returns
 *   BCM_E_NONE
 *
 */
int
bcm_api_logger_enable_get (int *enable)
{
    BCM_INIT_FUNC_DEFS_NO_UNIT;

    *enable = (Cint_logger_cfg.enabled) ? 1 : 0;

    BCM_FUNC_RETURN_NO_UNIT;
}

/*
 * Get current logging mode for the API logger
 *
 * Paramters:
 *  [out] mode
 *    bcm_api_logger_mode_t is returned. See include/bcm/api_logger.h
 *
 * Returns
 *  BCM_E_NONE on Success
 *  BCM_E_INTERNAL if mode is unknown
 *
 */
int
bcm_api_logger_mode_get (bcm_api_logger_mode_t *mode)
{
    BCM_INIT_FUNC_DEFS_NO_UNIT;

    switch (Cint_logger_cfg.mode)
    {
        case cintLoggerModeLog:
        {
            *mode = bcmApiLoggerModeLog;
            break;
        }
        case cintLoggerModeCache:
        {
            *mode = bcmApiLoggerModeCache;
            break;
        }
        case cintLoggerModeReplay:
        {
            *mode = bcmApiLoggerModeReplay;
            break;
        }
        default:
        {
            BCM_ERR_EXIT_MSG(BCM_E_INTERNAL, (BSL_META("unknown internal mode %d\n"), Cint_logger_cfg.mode));
            break;
        }
    }

exit:
    BCM_FUNC_RETURN_NO_UNIT;
}

/*
 * Set logging mode for the API logger
 *
 * Paramters
 *  [in] mode
 *    Mode to be set. See include/bcm/api_logger.h
 *
 * Returns
 *   BCM_E_NONE on Success
 *   BCM_E_PARAM is invalid mode was passed
 */
int
bcm_api_logger_mode_set (bcm_api_logger_mode_t mode)
{
    BCM_INIT_FUNC_DEFS_NO_UNIT;

    CINT_LOGGER_LOCK;

    switch (mode)
    {
        case bcmApiLoggerModeLog:
        {
            Cint_logger_cfg.mode = cintLoggerModeLog;
            break;
        }
        case bcmApiLoggerModeCache:
        {
            Cint_logger_cfg.mode = cintLoggerModeCache;
            break;
        }
        case bcmApiLoggerModeReplay:
        {
            Cint_logger_cfg.mode = cintLoggerModeReplay;
            break;
        }
        default:
        {
            BCM_ERR_EXIT_MSG(BCM_E_PARAM, (BSL_META("unknown mode %d\n"), mode));
            break;
        }
    }

exit:
    CINT_LOGGER_UNLOCK;

    BCM_FUNC_RETURN_NO_UNIT;
}

/*
 * The API logger can write logging entries to files in two places. 1. Inline
 * with the logging entry creation, where no formatting is possible and entries
 * are written as-is in raw binary format; and 2. From a dedicated SW thread
 * where formatting is possible.
 *
 * Get name of either the inline (Raw) file name, or the offloaded file, along
 * with the associated output format.
 *
 * Parameters
 *  [in] out_file
 *    Enum indicating whether inline or offloaded output log file name is being
 *    requested.
 *  [inout] file_name_size
 *    Specifies the size of the buffer file_name points to. API returns the
 *    actual size of the requested file name in this variable. Zero is returned
 *    if no file is in use. When the value returned in this parameter is bigger
 *    than its original value, caller can understand that no write was
 *    performed to file_name, and that it should be fetched again with a bigger
 *    input buffer.
 *  [out] file_name
 *    This buffer is filled up with the name of the file, if one is use. Else
 *    the buffer is not written to. If the buffer is not big enough to hold the
 *    complete file name along with its trailing NUL termination then also the
 *    buffer is not written to.
 *  [out] fmt
 *    When out_file is bcmApiLoggerOutFileOffloaded, associated output format
 *    is returned in this variable. When out_file is bcmApiLoggerOutFileInline,
 *    bcmApiLoggerFileFormatRaw is returned.
 *
 * Returns
 *   BCM_E_NONE on Success
 *   BCM_E_PARAM on invalid input
 */
int
bcm_api_logger_outfile_get (bcm_api_logger_outfile_t out_file,
                            int *file_name_size,
                            char *file_name,
                            bcm_api_logger_file_format_t *fmt)
{
    char *tgt_file = NULL;
    int tgt_len = 0;
    bcm_api_logger_file_format_t output_fmt = bcmApiLoggerFileFormatNone;

    BCM_INIT_FUNC_DEFS_NO_UNIT;

    BCM_NULL_CHECK_NO_UNIT(file_name_size);
    BCM_NULL_CHECK_NO_UNIT(file_name);
    BCM_NULL_CHECK_NO_UNIT(fmt);

    switch (out_file)
    {
        case bcmApiLoggerOutFileOffloaded:
        {

            tgt_file = Cint_logger_cfg.logfile;

            if (Cint_logger_cfg.log_to_file & CINT_LOGGER_FILE_OP_RAW) {

                output_fmt = bcmApiLoggerFileFormatRaw;

            } else if (Cint_logger_cfg.log_to_file & CINT_LOGGER_FILE_OP_CINT) {

                output_fmt = bcmApiLoggerFileFormatCintLike;

            }
            break;
        }

        case bcmApiLoggerOutFileInline:
        {
            tgt_file = Cint_logger_cfg.raw_logfile;
            output_fmt = bcmApiLoggerFileFormatRaw;
            break;
        }

        default:
        {
            BCM_ERR_EXIT_MSG(BCM_E_PARAM, (BSL_META("unknown outfile %d\n"), out_file));
            break;
        }
    }

    CINT_LOGGER_LOCK;

    if (tgt_file) {

        tgt_len = CINT_STRLEN(tgt_file) + 1;

        if (tgt_len <= *file_name_size) {

            CINT_STRNCPY(file_name, tgt_file, *file_name_size);
        }

        *file_name_size = tgt_len;

    } else {

        *file_name_size = 0;

    }

    *fmt = output_fmt;

    CINT_LOGGER_UNLOCK;

exit:
    BCM_FUNC_RETURN_NO_UNIT;
}

/*
 * Set one of the inline or offloaded output files
 *
 * Parameters
 *  [in] out_file
 *    Enum indicating whether inline or offloaded output log file is being set
 *  [in] file_name
 *    Name of the file, this file will be attempted to be opened for writing.
 *    Failure to do so will be indicated on the return value. If NULL or empty
 *    string is passed into this parameter, it has the effect of closing any
 *    previously opened file of the same type.
 *  [in] fmt
 *    When out_file is bcmApiLoggerOutFileOffloaded, associated output format
 *    is supplied in this variable. This parameter is ignored when out_file is
 *    bcmApiLoggerOutFileInline.
 *
 * Returns
 *   BCM_E_NONE on Success
 *   BCM_E_UNAVAIL if Xml/Json format is requested
 *   BCM_E_RESOURCE on failure to open the file for writing
 */
int
bcm_api_logger_outfile_set (bcm_api_logger_outfile_t out_file,
                            char *file_name,
                            bcm_api_logger_file_format_t fmt)

{
    int log_to_file = CINT_LOGGER_FILE_OP_NONE;
    char **tgt_file;
    FILE **tgt_file_fp;
    int locks_locked = 0;

    BCM_INIT_FUNC_DEFS_NO_UNIT;

    switch (out_file)
    {
        case bcmApiLoggerOutFileOffloaded:
        {
            tgt_file    = &Cint_logger_cfg.logfile;
            tgt_file_fp = (FILE**) &Cint_logger_cfg.logfile_fp;
            break;
        }

        case bcmApiLoggerOutFileInline:
        {
            tgt_file    = &Cint_logger_cfg.raw_logfile;
            tgt_file_fp = (FILE**) &Cint_logger_cfg.raw_logfile_fp;
            break;
        }

        default:
        {
            BCM_ERR_EXIT_MSG(BCM_E_PARAM, (BSL_META("unknown output file %d\n"), out_file));
            break;
        }
    }

    switch (fmt)
    {
        case bcmApiLoggerFileFormatNone:
        {
            log_to_file = CINT_LOGGER_FILE_OP_NONE;
            break;
        }

        case bcmApiLoggerFileFormatRaw:
        {
            log_to_file = CINT_LOGGER_FILE_OP_RAW;
            break;
        }

        case bcmApiLoggerFileFormatCintLike:
        {
            log_to_file = CINT_LOGGER_FILE_OP_CINT;
            break;
        }

        case bcmApiLoggerFileFormatXml:
        case bcmApiLoggerFileFormatJson:
        {
            BCM_ERR_EXIT_MSG(BCM_E_UNAVAIL, (BSL_META("Xml/Json output formats are not supported\n")));
            break;
        }

        default:
        {
            BCM_ERR_EXIT_MSG(BCM_E_PARAM, (BSL_META("unknown output format %d\n"), fmt));
            break;
        }
    }

    /*
     * pause the logger...this is so that the file manipulation we are about to
     * do does not happen while any existing file pointers are in use from
     * another thread. Ideally there would be a lock to simply lock out file
     * IO, however, we do not have that so we lock the entire logger process
     * out.
     */
    CINT_LOGGER_LOCK;

    /*
     * ...and pause all the logging to file. Following locks out the offload
     * thread from accessing files.
     */
    CINT_VARIABLES_LOCK;

    locks_locked = 1;

    /*
     * If file_name is NULL, OR it points to an empty string, OR It is
     * different from the current file name if one is in use... close the
     * current file if one is in use.
     */
    if (!file_name || !CINT_STRCMP(file_name, "") ||
        (*tgt_file && CINT_STRCMP(file_name, *tgt_file))) {

        if (*tgt_file_fp) {
            CINT_FCLOSE(*tgt_file_fp);
            *tgt_file_fp = NULL;
        }

        if (*tgt_file) {
            CINT_FREE(*tgt_file);
            *tgt_file = NULL;
        }
    }

    if (file_name && CINT_STRCMP(file_name, "") &&
        (!*tgt_file || CINT_STRCMP(file_name, *tgt_file))) {

        *tgt_file_fp = CINT_FOPEN(file_name, "a");

        if (!*tgt_file_fp) {

            BCM_ERR_EXIT_MSG(BCM_E_RESOURCE, (BSL_META("failed to open %s for writing\n"), file_name));
        }

        *tgt_file = CINT_STRDUP(file_name);
    }

    Cint_logger_cfg.log_to_file = log_to_file;

exit:

    if (locks_locked) {

        CINT_VARIABLES_UNLOCK;

        CINT_LOGGER_UNLOCK;

    }

    BCM_FUNC_RETURN_NO_UNIT;
}

/*
 * Get the registered CINT callback function names
 *
 * Parameters
 *  [in] function_type
 *    Enum specifying which one of the three supported CINT callback functions
 *    is being requested
 *  [inout] function_name_size
 *    Specifies the size of the buffer function_name points to. API returns the
 *    actual size of the requested function name in this variable. Zero is
 *    returned if no function is in use. When the value returned in this
 *    parameter is bigger than its original value, caller can understand that
 *    no write was performed to function_name, and that it should be fetched
 *    again with a bigger input buffer.
 *  [out] function_name
 *    Pointer to relevant callback function is returned in this variable. Note
 *    that the returned pointer is a reference to the same buffer that the API
 *    logger uses for holding the function names, therefore the caller should
 *    not free the returned pointer or write to it. Unpredictable results
 *    including crashes may occur if the returned pointer or buffer is
 *    manipulated in any way. If the corresponding function is not set,
 *    NULL pointer is returned.
 *
 * Returns
 *    BCM_E_NONE on Success
 *    BCM_E_PARAM if an unknown function_type is requested
 */
int
bcm_api_logger_cint_function_get (
                bcm_api_logger_cint_function_t function_type,
                int *function_name_size,
                char *function_name)
{
    char *tgt_function = NULL;
    int tgt_len = 0;
    BCM_INIT_FUNC_DEFS_NO_UNIT;

    BCM_NULL_CHECK_NO_UNIT(function_name_size);
    BCM_NULL_CHECK_NO_UNIT(function_name);

    switch (function_type)
    {
        case bcmApiLoggerCintFunctionInline:
        {
            tgt_function = Cint_logger_cfg.cint_filter_fn;
            break;
        }

        case bcmApiLoggerCintFunctionLogfile:
        {
            tgt_function = Cint_logger_cfg.cint_logfile_filter_fn;
            break;
        }

        case bcmApiLoggerCintFunctionReplay:
        {
            tgt_function = Cint_logger_cfg.cint_replay_filter_fn;
            break;
        }

        default:
        {
            BCM_ERR_EXIT_MSG(BCM_E_PARAM, (BSL_META("unknown function_type %d\n"), function_type));
            break;
        }
    }

    CINT_LOGGER_LOCK;

    CINT_VARIABLES_LOCK;

    if (tgt_function) {

        tgt_len = CINT_STRLEN(tgt_function) + 1;

        if (tgt_len <= *function_name_size) {

            CINT_STRNCPY(function_name, tgt_function, *function_name_size);

            *function_name_size = tgt_len;
        }
    } else {

        *function_name_size = 0;

    }

    CINT_VARIABLES_UNLOCK;

    CINT_LOGGER_UNLOCK;

exit:
    BCM_FUNC_RETURN_NO_UNIT;
}

/*
 * Set the registered CINT callback function names
 *
 * Parameters
 *  [in] function_type
 *    Enum indicating which of the three support CINT callback functions is
 *    being set
 *  [in] function_name
 *    Name of the CINT callback function. Passing NULL or the empty string has
 *    the effect of resetting any previously set callback. Passing a new name
 *    than current configured overrides the callback with the one supplied
 *    here. NOTE. API does not verify whether the CINT interpreter already has
 *    a function defined by the supplied name.
 *
 * Returns
 *   BCM_E_NONE on Success
 *   BCM_E_PARAM if an unknown function_type is indicated
 */
int
bcm_api_logger_cint_function_set (
                bcm_api_logger_cint_function_t function_type,
                char *function_name)
{
    char **tgt_function = NULL;
    BCM_INIT_FUNC_DEFS_NO_UNIT;

    switch (function_type)
    {
        case bcmApiLoggerCintFunctionInline:
        {
            tgt_function = &Cint_logger_cfg.cint_filter_fn;
            break;
        }

        case bcmApiLoggerCintFunctionLogfile:
        {
            tgt_function = &Cint_logger_cfg.cint_logfile_filter_fn;
            break;
        }

        case bcmApiLoggerCintFunctionReplay:
        {
            tgt_function = &Cint_logger_cfg.cint_replay_filter_fn;
            break;
        }

        default:
        {
            BCM_ERR_EXIT_MSG(BCM_E_PARAM, (BSL_META("unknown function_type %d\n"), function_type));
            break;
        }
    }

    /*
     * take locks so that CINT callback which we are about to manipulate is not
     * in use from another thread, including the offload thread.
     */
    CINT_LOGGER_LOCK;

    CINT_VARIABLES_LOCK;

    /*
     * If function_name is NULL, OR points to an empty String, OR is not the
     * same as the current function name if one is in use... then free the
     * current function name if one is in use.
     */
    if (!function_name || !CINT_STRCMP(function_name, "") ||
        (*tgt_function && CINT_STRCMP(function_name, *tgt_function))) {

        if (*tgt_function) {

            CINT_FREE(*tgt_function);
            *tgt_function = NULL;
        }
    }

    if (function_name && CINT_STRCMP(function_name, "") &&
        (!*tgt_function || CINT_STRCMP(function_name, *tgt_function))) {

        *tgt_function = CINT_STRDUP(function_name);
    }

    CINT_VARIABLES_UNLOCK;

    CINT_LOGGER_UNLOCK;

exit:
    BCM_FUNC_RETURN_NO_UNIT;
}

/* } */
#else
/* { */
/* Make ISO compilers happy. */
typedef int cint_logger_api_c_not_empty;
/* } */
#endif


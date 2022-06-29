/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_API_LOGGER_H__
#define __BCM_API_LOGGER_H__

#if defined(INCLUDE_API_LOGGING)

#include <bcm/types.h>

/* Enable/Disable the API Logger */
extern int bcm_api_logger_enable_set(
    int enable);

/* Get the Enabled/Disabled state of the API Logger */
extern int bcm_api_logger_enable_get(
    int *enable);

/* API Logger modes */
typedef enum bcm_api_logger_mode_e {
    bcmApiLoggerModeLog,    /* Invoke optional cint function, log to an
                               in-memory buffer, and optionally write to
                               raw_logfile */
    bcmApiLoggerModeCache,  /* Only log to an in-memory buffer */
    bcmApiLoggerModeReplay, /* Replaying previously generated logs, expected to
                               be used on SVK */
    bcmApiLoggerModeCount   /* Limit for input validation */
} bcm_api_logger_mode_t;

/* Get logging mode for the API logger */
extern int bcm_api_logger_mode_get(
    bcm_api_logger_mode_t *mode);

/* Set logging mode for the API logger */
extern int bcm_api_logger_mode_set(
    bcm_api_logger_mode_t mode);

/* 
 * The API logger can write logging entries to files in two places. 1.
 * Inline with the logging entry creation, where no formatting is
 * possible and entries are written as-is in raw binary format; and 2.
 * From a dedicated SW thread where formatting is possible. This enum
 * lists the various output formats
 */
typedef enum bcm_api_logger_file_format_e {
    bcmApiLoggerFileFormatNone,     /* No output is generated */
    bcmApiLoggerFileFormatRaw,      /* Logging entries are written as-is to
                                       file, as Raw binary data */
    bcmApiLoggerFileFormatCintLike, /* Logging entries are written in CINT-like
                                       format */
    bcmApiLoggerFileFormatXml,      /* Logging entries are written in XML format */
    bcmApiLoggerFileFormatJson,     /* Logging entries are written in JSON
                                       format */
    bcmApiLoggerFileFormatCount     /* Limit for input validation */
} bcm_api_logger_file_format_t;

/* API Logger output file */
typedef enum bcm_api_logger_outfile_e {
    bcmApiLoggerOutFileInline,      /* File written inline with the creation of
                                       logging entries in the circular buffer.
                                       The logging entries are written as-is, as
                                       Raw binary data */
    bcmApiLoggerOutFileOffloaded,   /* File written from a dedicated SW thread.
                                       Logging entries may be converted to other
                                       formats such as CINT-like, or may be
                                       written as-is */
    bcmApiLoggerOutFileCount        /* Limit for input validation */
} bcm_api_logger_outfile_t;

/* Get Logfile name */
extern int bcm_api_logger_outfile_get(
    bcm_api_logger_outfile_t out_file, 
    int *file_name_size, 
    char *file_name, 
    bcm_api_logger_file_format_t *fmt);

/* Set Logfile name */
extern int bcm_api_logger_outfile_set(
    bcm_api_logger_outfile_t out_file, 
    char *file_name, 
    bcm_api_logger_file_format_t fmt);

/* CINT callback functions */
typedef enum bcm_api_logger_cint_function_e {
    bcmApiLoggerCintFunctionInline,     /* CINT function invoked inline with API
                                           calls, during Log and Replay modes */
    bcmApiLoggerCintFunctionLogfile,    /* CINT function invoked from Offload
                                           thread while writing logging entries
                                           to file */
    bcmApiLoggerCintFunctionReplay,     /* CINT function invoked inline with API
                                           calls during Replay */
    bcmApiLoggerCintFunctionCount       /* Limit for input validation */
} bcm_api_logger_cint_function_t;

/* CINT callback functions */
extern int bcm_api_logger_cint_function_get(
    bcm_api_logger_cint_function_t function_type, 
    int *function_name_size, 
    char *function_name);

/* CINT callback functions */
extern int bcm_api_logger_cint_function_set(
    bcm_api_logger_cint_function_t function_type, 
    char *function_name);

#endif /* defined(INCLUDE_API_LOGGING) */

#endif /* __BCM_API_LOGGER_H__ */

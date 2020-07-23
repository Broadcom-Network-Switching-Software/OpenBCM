/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision    :   *
 *                                                                                *
 *  Description :  Internal API error functions                                   *
 */

/** @file blackhawk7_l8p2_internal_error.h
 * Internal API error functions
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLACKHAWK7_L8P2_API_INTERNAL_ERROR_H
#define BLACKHAWK7_L8P2_API_INTERNAL_ERROR_H

#include "blackhawk7_l8p2_usr_includes.h"
#include "common/srds_api_err_code.h"
#include "common/srds_api_types.h"


/**
 * Error-trapping macro.
 *
 * In other then SerDes-team post-silicon evaluation builds, simply yields
 * the error code supplied as an argument, without further action.
 */
#define blackhawk7_l8p2_error_report(sa__, err_code) \
        blackhawk7_l8p2_INTERNAL_print_err_msg_and_triage_info(sa__, (err_code), __FILE__, API_FUNCTION_NAME, (uint16_t)__LINE__)
/**@}*/
/* Prints error code, containing function, file and line number */
#define blackhawk7_l8p2_error(sa__, err_code) \
        blackhawk7_l8p2_INTERNAL_print_err_msg(sa__, (err_code), __FILE__, API_FUNCTION_NAME, (uint16_t)__LINE__)

/** Print Error messages to screen before returning.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] err_code Error Code input which is returned as well.
 * @param[in] filename filename containing the function from which error is reported.
 * @param[in] func_name function in which error is reported.
 * @param[in] line Line number of where the error occurred.
 * @return Error Code.
 */
err_code_t blackhawk7_l8p2_INTERNAL_print_err_msg(srds_access_t *sa__, uint16_t err_code, const char *filename, const char *func_name, uint16_t line);

/** Print Error messages to screen and collects and prints Triage info before returning.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] err_code Error Code input which is returned as well.
 * @param[in] filename filename containing the function from which error is reported.
 * @param[in] func_name function in which error is reported.
 * @param[in] line Line number of where the error occurred.
 * @return Error Code.
 */
err_code_t blackhawk7_l8p2_INTERNAL_print_err_msg_and_triage_info(srds_access_t *sa__, uint16_t err_code, const char *filename, const char *func_name, uint16_t line);

/** Print Convert Error code to String.
 * @param[in] err_code Error Code input which is converted to string.
 * @return String containing Error code information.
 */
const char* blackhawk7_l8p2_INTERNAL_e2s_err_code(err_code_t err_code);

#endif
#ifdef __cplusplus
}
#endif

/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2021 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision    :   *
 *                                                                                *
 *  Description :  Internal API error functions                                   *
 *                                                                                *
 **********************************************************************************
 **********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MERLIN7_PCIEG3_API_INTERNAL_ERROR_H
#define MERLIN7_PCIEG3_API_INTERNAL_ERROR_H

#include "merlin7_pcieg3_usr_includes.h"
#include "common/srds_api_err_code.h"
#include "common/srds_api_types.h"

/*! @file
 *  @brief Internal API error functions
 */

/*! @addtogroup APITag
 * @{
 */

/*! @defgroup SerdesAPIInternalErrorTag Internal Error Functions
 * Internal Serdes API functions and maros for error handling which get called by other modules and
 * should not be called directly.
 */

/*! @addtogroup SerdesAPIInternalErrorTag
 * @{
 */


/**
 * Error-trapping macro.
 *
 * In other then SerDes-team post-silicon evaluation builds, simply yields
 * the error code supplied as an argument, without further action.
 */
#define merlin7_pcieg3_error_report(sa__, err_code) \
        merlin7_pcieg3_INTERNAL_print_err_msg_and_triage_info(sa__, (err_code), __FILE__, API_FUNCTION_NAME, (uint16_t)__LINE__)
/* Prints error code, containing function, file and line number */
#define merlin7_pcieg3_error(sa__, err_code) \
        merlin7_pcieg3_INTERNAL_print_err_msg(sa__, (err_code), __FILE__, API_FUNCTION_NAME, (uint16_t)__LINE__)
/** Print Error messages to screen before returning.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] err_code Error Code input which is returned as well.
 * @param[in] filename filename containing the function from which error is reported.
 * @param[in] func_name function in which error is reported.
 * @param[in] line Line number of where the error occurred.
 * @return Error Code.
 */
err_code_t merlin7_pcieg3_INTERNAL_print_err_msg(srds_access_t *sa__, uint16_t err_code, const char *filename, const char *func_name, uint16_t line);

/** Print Error messages to screen and collects and prints Triage info before returning.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] err_code Error Code input which is returned as well.
 * @param[in] filename filename containing the function from which error is reported.
 * @param[in] func_name function in which error is reported.
 * @param[in] line Line number of where the error occurred.
 * @return Error Code.
 */
err_code_t merlin7_pcieg3_INTERNAL_print_err_msg_and_triage_info(srds_access_t *sa__, uint16_t err_code, const char *filename, const char *func_name, uint16_t line);

/** Print Convert Error code to String.
 * @param[in] err_code Error Code input which is converted to string.
 * @return String containing Error code information.
 */
const char* merlin7_pcieg3_INTERNAL_e2s_err_code(err_code_t err_code);

/*! @} SerdesAPIInternalErrorTag */
/*! @} APITag */
#endif
#ifdef __cplusplus
}
#endif

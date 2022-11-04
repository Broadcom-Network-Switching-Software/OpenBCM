/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2021 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/**************************************************************************************
 *  File Name     :  merlin7_pcieg3_usr_includes.h                                        *
 *  Created On    :  05/07/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Header file which includes all required std libraries and macros *
 *  Revision      :   *
 *                                                                                    *
 **************************************************************************************
 **************************************************************************************/

#ifndef MERLIN7_PCIEG3_API_USR_INCLUDES_H
#define MERLIN7_PCIEG3_API_USR_INCLUDES_H

/* Standard libraries that can be replaced by your custom libraries */
#ifdef _MSC_VER
/* Enclose all standard headers in a pragma to remove warings for MS compiler */
#pragma warning( push, 0 )
#endif

#include "merlin7_pcieg3_ipconfig.h"

/*! @file
 *  @brief Header file which includes all required std libraries and macros
 *
 * The user is expected to replace the macro definitions with their required implementation
 */

/*! @addtogroup APITag
 * @{
 */

/*! @defgroup SerdesAPIUsrIncludesTag User Provided Macros
 * Contains macros with default definitions which can be redefined
 * by users if desired.
 */

/*! @addtogroup SerdesAPIUsrIncludesTag
 * @{
 */

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#if defined _MSC_VER
#define API_FUNCTION_NAME __FUNCTION__
#else
#define API_FUNCTION_NAME __func__
#endif

/* Redefine macros according your compiler requirements */

#include <pciephy/pciephy.h>
#define USR_PRINTF(paren_arg_list)    (PCIEPHY_DIAG_OUT paren_arg_list)
#define USR_VPRINTF(paren_arg_list)   (PCIEPHY_DIAG_OUT paren_arg_list)
#define USR_MEMSET(mem, val, num)     PCIEPHY_MEMSET(mem, val, num)
#define USR_STRLEN(string)            PCIEPHY_STRLEN(string)
#define USR_STRNCAT(str1, str2, num)  PCIEPHY_STRNCAT(str1, str2, num)
#define USR_STRCPY(str1, str2)        PCIEPHY_STRCPY(str1, str2)
#define USR_STRCMP(str1, str2)        PCIEPHY_STRCMP(str1, str2)
#define USR_STRNCMP(str1, str2, num)  PCIEPHY_STRNCMP(str1, str2, num)
#define USR_SNPRINTF                  PCIEPHY_SNPRINTF
#define USR_CALLOC(nmemb, size)       PCIEPHY_ALLOC((nmemb * size), "bcmPcie")
#define USR_FREE(ptr)                 PCIEPHY_FREE(ptr)
#define USR_UINTPTR                   uintptr_t

#ifdef SERDES_API_FLOATING_POINT
#define USR_DOUBLE                    double
#else
#define USR_DOUBLE       int
#define double       undefined
#define float        undefined
#endif

/* Mutex Definitions */
#ifndef NO_VARIADIC_MACROS
#define USR_CREATE_LOCK
#define USR_ACQUIRE_LOCK
#define USR_RELEASE_LOCK
#define USR_DESTROY_LOCK
#endif

/* Implementation specific macros below */
#ifndef NO_VARIADIC_MACROS
/*! @def usr_logger_write
 * is an API user provided Dependency function. In practice it's return value is
 * never checked. To do so, the following define can be used. If using one of the API error checking
 * macros (like EFUN()) which calls the API error processing functons, the
 * ERR_CODE_SRDS_REG_ACCESS_FAIL error code should be returned to avoid error processing recursion.
 * The (void) cast serves to quiet compiler warnings if so configured, otherwise it can be removed.
 * usr_logger_write(...)  ((void)(logger_write(0, -1, __VA_ARGS__) ? ERR_CODE_SRDS_REG_ACCESS_FAIL : ERR_CODE_NONE))
 * ********************************************************************************************
 */
#define usr_logger_write(...) logger_write(sa__, -1,__VA_ARGS__)

/*! @def usr_logger_verbose_write
 * appears broken (no 'sa__' arg), is likely not being used, and may be removed.
 */
#define usr_logger_verbose_write(...) logger_write(1,__VA_ARGS__)
#endif
#define USR_DELAY_MS(stuff) merlin7_pcieg3_acc_delay_ms(sa__,stuff)
#define USR_DELAY_US(stuff) merlin7_pcieg3_acc_delay_us(sa__,stuff)
#define USR_DELAY_NS(stuff) merlin7_pcieg3_acc_delay_ns(sa__,stuff)
/*! @} SerdesAPIUsrIncludesTag */
/*! @} APITag */
#endif /* !SERDES_API_USR_INCLUDES_H */

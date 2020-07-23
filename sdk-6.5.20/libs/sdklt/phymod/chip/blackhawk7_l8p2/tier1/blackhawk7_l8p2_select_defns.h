/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/*********************************************************************************
 *********************************************************************************
 *  File Name  :  blackhawk7_l8p2_select_defns.h
 *  Created On :  29 Sep 2015
 *  Created By :  Brent Roberts
 *  Description:  Select header files for IP-specific definitions
 *  Revision   :  
 */

 /** @file
 * Select IP files to include for API
 */

#ifndef BLACKHAWK7_L8P2_API_SELECT_DEFNS_H
#define BLACKHAWK7_L8P2_API_SELECT_DEFNS_H

#include "blackhawk7_l8p2_ipconfig.h"
#include "blackhawk7_l8p2_field_access.h"

#   include "blackhawk7_api_uc_common.h"


/****************************************************************************
 * @name Defines for Serdes Unified APIs (UAPI)
 *
 * Unified APIs combine multiple IP variants into single UAPI.
 */

#define BLACKHAWK7_L8P2_UAPI_INIT           srds_info_t const * const blackhawk7_l8p2_info_ptr = blackhawk7_l8p2_INTERNAL_get_blackhawk7_l8p2_info_ptr_with_check(sa__);
#define BLACKHAWK7_L8P2_UAPI_SWITCH         (blackhawk7_l8p2_info_ptr->silicon_version)
#define BLACKHAWK7_L8P2_UAPI_CASE0          (AMS_VERSION_A0)
#define BLACKHAWK7_L8P2_UAPI_CASE1          (AMS_VERSION_B0)
#define BLACKHAWK7_L8P2_UAPI_INIT_AMS_A0    {                                                           \
                                     BLACKHAWK7_L8P2_UAPI_INIT                                          \
                                     if (BLACKHAWK7_L8P2_UAPI_SWITCH == AMS_VERSION_A0) { /* BLACKHAWK7_V1L8P2 */
#define BLACKHAWK7_L8P2_UAPI_INIT_AMS_B0    {                                                           \
                                     BLACKHAWK7_L8P2_UAPI_INIT                                          \
                                     if (BLACKHAWK7_L8P2_UAPI_SWITCH == AMS_VERSION_B0) { /* BLACKHAWK7_V2L8P2 */
#define BLACKHAWK7_L8P2_UAPI_ELSE_AMS_A0      }                                                         \
                                     else if (BLACKHAWK7_L8P2_UAPI_SWITCH == AMS_VERSION_A0) { /* BLACKHAWK7_V1L8P2 */
#define BLACKHAWK7_L8P2_UAPI_ELSE_AMS_B0      }                                                         \
                                     else if (BLACKHAWK7_L8P2_UAPI_SWITCH == AMS_VERSION_B0) { /* BLACKHAWK7_V2L8P2 */
#define BLACKHAWK7_L8P2_UAPI_NOT_AMS_A0_ERR   }                                                         \
                                     else if (BLACKHAWK7_L8P2_UAPI_SWITCH != AMS_VERSION_A0) {          \
                                       return(blackhawk7_l8p2_error(sa__, ERR_CODE_INVALID_UAPI_CASE)); \
                                     }                                                         \
                                   }
#define BLACKHAWK7_L8P2_UAPI_NOT_AMS_B0_ERR   }                                                         \
                                     else if (BLACKHAWK7_L8P2_UAPI_SWITCH != AMS_VERSION_B0) {          \
                                       return(blackhawk7_l8p2_error(sa__, ERR_CODE_INVALID_UAPI_CASE)); \
                                     }                                                         \
                                   }
#define BLACKHAWK7_L8P2_UAPI_ELSE_ERR         }                                                         \
                                     else {                                                    \
                                       return(blackhawk7_l8p2_error(sa__, ERR_CODE_INVALID_UAPI_CASE)); \
                                     }                                                         \
                                   }
#define BLACKHAWK7_L8P2_UAPI_TERMINATE


/****************************************************************************
 * @name Register Access Macro Inclusions
 *
 * All cores provide access to hardware control/status registers.
 */
/**@{*/

/**
 * This build includes register access macros for the BLACKHAWK7_L8P2 core.
 */
#include "blackhawk7_l8p2_fields.h"

/**@}*/


/****************************************************************************
 * @name RAM Access Macro Inclusions
 *
 * Some cores also provide access to firmware control/status RAM variables.
 */
/**@{*/

/**
 * This build includes macros to access Blackhawk microcode RAM variables.
 */
#include "blackhawk7_api_uc_vars_rdwr_defns.h"
/**@}*/

#endif

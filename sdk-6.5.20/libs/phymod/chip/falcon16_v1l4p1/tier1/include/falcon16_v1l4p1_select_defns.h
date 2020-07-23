/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*********************************************************************************
 *********************************************************************************
 *  File Name  :  falcon16_v1l4p1_select_defns.h
 *  Created On :  29 Sep 2015
 *  Created By :  Brent Roberts
 *  Description:  Select header files for IP-specific definitions
 *  Revision   :  
 */

 /** @file
 * Select IP files to include for API
 */

#ifndef FALCON16_V1L4P1_API_SELECT_DEFNS_H
#define FALCON16_V1L4P1_API_SELECT_DEFNS_H

#include "falcon16_v1l4p1_ipconfig.h"
#include "falcon16_v1l4p1_field_access.h"

#   include "falcon16_api_uc_common.h"

/****************************************************************************
 * @name Register Access Macro Inclusions
 *
 * All cores provide access to hardware control/status registers.
 */
/**@{*/

/**
 * This build includes register access macros for the FALCON16_V1L4P1 core.
 */
#include "falcon16_v1l4p1_fields.h"

/**@}*/


/****************************************************************************
 * @name RAM Access Macro Inclusions
 *
 * Some cores also provide access to firmware control/status RAM variables.
 */
/**@{*/

/**
 * This build includes macros to access Falcon16 microcode RAM variables.
 */
#include "falcon16_api_uc_vars_rdwr_defns.h"
/**@}*/

#endif

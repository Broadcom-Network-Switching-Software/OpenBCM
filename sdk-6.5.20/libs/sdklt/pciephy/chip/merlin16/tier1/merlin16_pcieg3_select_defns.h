/*********************************************************************************
 *********************************************************************************
 *  File Name  :  merlin16_pcieg3_select_defns.h
 *  Created On :  29 Sep 2015
 *  Created By :  Brent Roberts
 *  Description:  Select header files for IP-specific definitions
 *  Revision   :  
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  No portions of this material may be reproduced in any form without
 *  the written permission of:
 *  
 *      Broadcom Corporation
 *      5300 California Avenue
 *      Irvine, CA  92617
 *  
 *  All information contained in this document is Broadcom Corporation
 *  company private proprietary, and trade secret.
 */

 /** @file
 * Select IP files to include for API
 */

#ifndef MERLIN16_PCIEG3_API_SELECT_DEFNS_H
#define MERLIN16_PCIEG3_API_SELECT_DEFNS_H

#include "merlin16_pcieg3_ipconfig.h"
#include "merlin16_pcieg3_field_access.h"

#   include "merlin16_api_uc_common.h"

/****************************************************************************
 * @name Register Access Macro Inclusions
 *
 * All cores provide access to hardware control/status registers.
 */
/**@{*/

/**
 * This build includes register access macros for the MERLIN16_PCIEG3 core.
 */
#include "merlin16_pcieg3_fields.h"

/**@}*/


/****************************************************************************
 * @name RAM Access Macro Inclusions
 *
 * Some cores also provide access to firmware control/status RAM variables.
 */
/**@{*/

/**
 * This build includes macros to access Merlin16 microcode RAM variables.
 */
#include "merlin16_api_uc_vars_rdwr_defns.h"
/**@}*/

#endif

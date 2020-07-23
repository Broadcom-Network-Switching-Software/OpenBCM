/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File:    sum.h
 * Purpose: SUM definitions common to SDK and uKernel
 */

#ifndef _SOC_SHARED_SUM_H
#define _SOC_SHARED_SUM_H

#ifdef BCM_UKERNEL
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif

/*
 * SUM uC Error codes
 */
typedef enum shr_sum_uc_error_e {
    SHR_SUM_UC_E_NONE      = 0,
    SHR_SUM_UC_E_INTERNAL  = 1,
    SHR_SUM_UC_E_MEMORY    = 2,
    SHR_SUM_UC_E_UNIT      = 3,
    SHR_SUM_UC_E_PARAM     = 4,
    SHR_SUM_UC_E_EMPTY     = 5,
    SHR_SUM_UC_E_FULL      = 6,
    SHR_SUM_UC_E_NOT_FOUND = 7,
    SHR_SUM_UC_E_EXISTS    = 8,
    SHR_SUM_UC_E_TIMEOUT   = 9,
    SHR_SUM_UC_E_BUSY      = 10,
    SHR_SUM_UC_E_FAIL      = 11,
    SHR_SUM_UC_E_DISABLED  = 12,
    SHR_SUM_UC_E_BADID     = 13,
    SHR_SUM_UC_E_RESOURCE  = 14,
    SHR_SUM_UC_E_CONFIG    = 15,
    SHR_SUM_UC_E_UNAVAIL   = 16,
    SHR_SUM_UC_E_INIT      = 17,
    SHR_SUM_UC_E_PORT      = 18
} shr_sum_uc_error_t;

/* Max timeout for msging between SDK and UKERNEL */
#define SHR_SUM_MAX_UC_MSG_TIMEOUT              5000000 /* 5 secs */

/* Minimum sample interval */
#define SHR_SUM_MIN_SAMPLE_INTERVAL              2 /* 2 usecs */

/* Max number of ports in TH3 SUM supporting */
#define SHR_SUM_MAX_PORTS (256)

#endif /* _SOC_SHARED_SUM_H */

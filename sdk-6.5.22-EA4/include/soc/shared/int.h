/*
 * File:    int.h
 * Purpose: INT Turnaround definitions common to SDK and uKernel
 */

#ifndef _SOC_SHARED_INT_H
#define _SOC_SHARED_INT_H

#ifdef BCM_UKERNEL
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif

/*
 * INT uC Error codes
 */
typedef enum shr_int_uc_error_e {
    SHR_INT_UC_E_NONE      = 0,
    SHR_INT_UC_E_INTERNAL  = 1,
    SHR_INT_UC_E_MEMORY    = 2,
    SHR_INT_UC_E_UNIT      = 3,
    SHR_INT_UC_E_PARAM     = 4,
    SHR_INT_UC_E_EMPTY     = 5,
    SHR_INT_UC_E_FULL      = 6,
    SHR_INT_UC_E_NOT_FOUND = 7,
    SHR_INT_UC_E_EXISTS    = 8,
    SHR_INT_UC_E_TIMEOUT   = 9,
    SHR_INT_UC_E_BUSY      = 10,
    SHR_INT_UC_E_FAIL      = 11,
    SHR_INT_UC_E_DISABLED  = 12,
    SHR_INT_UC_E_BADID     = 13,
    SHR_INT_UC_E_RESOURCE  = 14,
    SHR_INT_UC_E_CONFIG    = 15,
    SHR_INT_UC_E_UNAVAIL   = 16,
    SHR_INT_UC_E_INIT      = 17,
    SHR_INT_UC_E_PORT      = 18
} shr_int_uc_error_t;

/* Max timeout for msging between SDK and UKERNEL */
#define SHR_INT_MAX_UC_MSG_TIMEOUT              5000000 /* 5 secs */

#endif /* _SOC_SHARED_INT_H */

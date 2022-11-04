#ifndef _SOC_SHARED_PFC_H
#define _SOC_SHARED_PFC_H

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif




/*****************************************
 * EPFC uController Error codes
 */
typedef enum shr_pfc_uc_error_e {
    SHR_PFC_UC_E_NONE = 0,
    SHR_PFC_UC_E_INTERNAL,
    SHR_PFC_UC_E_MEMORY,
    SHR_PFC_UC_E_UNIT,
    SHR_PFC_UC_E_PARAM,
    SHR_PFC_UC_E_EMPTY,
    SHR_PFC_UC_E_FULL,
    SHR_PFC_UC_E_NOT_FOUND,
    SHR_PFC_UC_E_EXISTS,
    SHR_PFC_UC_E_TIMEOUT,
    SHR_PFC_UC_E_BUSY,
    SHR_PFC_UC_E_FAIL,
    SHR_PFC_UC_E_DISABLED,
    SHR_PFC_UC_E_BADID,
    SHR_PFC_UC_E_RESOURCE,
    SHR_PFC_UC_E_CONFIG,
    SHR_PFC_UC_E_UNAVAIL,
    SHR_PFC_UC_E_INIT,
    SHR_PFC_UC_E_PORT
} shr_pfc_uc_error_t;
#endif /* _SOC_SHARED_PFC_H */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

/*
 * File:    qcm.h
 * Purpose: QCM definitions common to SDK and uKernel
 *
 * Notes:   Definition changes should be avoided in order to
 *          maintain compatibility between SDK and uKernel since
 *          both images are built and loaded separately.
 */

#ifndef _SOC_SHARED_QCM_H
#define _SOC_SHARED_QCM_H

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif

#define SHR_QCM_CONFIG_STATUS_BIT            0x1
#define SHR_QCM_INIT_STATUS_BIT              0x2
#define SHR_QCM_CCMDMA_STATUS_BIT            0x4
#define SHR_QCM_APP_STATUS_1                 0x51434D5F
#define SHR_QCM_APP_STATUS_2                 0x5F415050
#define SHR_QCM_TIME_USEC                    0x100000
#define SHR_QCM_MAX_GPORT_MONITOR            0x40
#define SHR_QCM_ERR_OFFSET                   3
#define SHR_QCM_MEM_MIN_SIZE                 0x4000

/*
 * QCM uC Error codes
 */
typedef enum shr_qcm_uc_error_e {
    SHR_QCM_UC_E_NONE      = 0,
    SHR_QCM_UC_E_INTERNAL  = 1,
    SHR_QCM_UC_E_MEMORY    = 2,
    SHR_QCM_UC_E_UNIT      = 3,
    SHR_QCM_UC_E_PARAM     = 4,
    SHR_QCM_UC_E_EMPTY     = 5,
    SHR_QCM_UC_E_FULL      = 6,
    SHR_QCM_UC_E_NOT_FOUND = 7,
    SHR_QCM_UC_E_EXISTS    = 8,
    SHR_QCM_UC_E_TIMEOUT   = 9,
    SHR_QCM_UC_E_BUSY      = 10,
    SHR_QCM_UC_E_FAIL      = 11,
    SHR_QCM_UC_E_DISABLED  = 12,
    SHR_QCM_UC_E_BADID     = 13,
    SHR_QCM_UC_E_RESOURCE  = 14,
    SHR_QCM_UC_E_CONFIG    = 15,
    SHR_QCM_UC_E_UNAVAIL   = 16,
    SHR_QCM_UC_E_INIT      = 17,
    SHR_QCM_UC_E_PORT      = 18
} shr_qcm_uc_error_t;

#endif /* _SOC_SHARED_QCM_H */



/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File:    st.h
 * Purpose: ST definitions common to SDK and uKernel
 *
 * Notes:   Definition changes should be avoided in order to
 *          maintain compatibility between SDK and uKernel since
 *          both images are built and loaded separately.
 */

#ifndef _SOC_SHARED_ST_H
#define _SOC_SHARED_ST_H

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif


/*
 * ST uC Error codes
 */
typedef enum shr_st_uc_error_e {
    SHR_ST_UC_E_NONE      = 0,
    SHR_ST_UC_E_INTERNAL  = 1,
    SHR_ST_UC_E_MEMORY    = 2,
    SHR_ST_UC_E_UNIT      = 3,
    SHR_ST_UC_E_PARAM     = 4,
    SHR_ST_UC_E_EMPTY     = 5,
    SHR_ST_UC_E_FULL      = 6,
    SHR_ST_UC_E_NOT_FOUND = 7,
    SHR_ST_UC_E_EXISTS    = 8,
    SHR_ST_UC_E_TIMEOUT   = 9,
    SHR_ST_UC_E_BUSY      = 10,
    SHR_ST_UC_E_FAIL      = 11,
    SHR_ST_UC_E_DISABLED  = 12,
    SHR_ST_UC_E_BADID     = 13,
    SHR_ST_UC_E_RESOURCE  = 14,
    SHR_ST_UC_E_CONFIG    = 15,
    SHR_ST_UC_E_UNAVAIL   = 16,
    SHR_ST_UC_E_INIT      = 17,
    SHR_ST_UC_E_PORT      = 18
} shr_st_uc_error_t;


/* Max size of the ProtoBuf packet encap */
#define SHR_ST_MAX_COLLECTOR_ENCAP_LENGTH (128)

/* Max length of the interface name */
#define SHR_ST_MAX_IF_NAME_LENGTH (32)

/* Max number of lports in all devices supporting ST */
#define SHR_ST_MAX_PORTS (160)

/* Max length of the system Id in bytes */
#define SHR_ST_MAX_SYSTEM_ID_LENGTH (32)

/* Max export length supported */
#define SHR_ST_MAX_EXPORT_LENGTH 9000

/* Max timeout for msging between SDK and UKERNEL */
#define SHR_ST_MAX_UC_MSG_TIMEOUT 5000000 /* 5 secs */

/* Stat types */
#define SHR_ST_STAT_TYPE_IF_INGRESS        (1 << 0)
#define SHR_ST_STAT_TYPE_IF_EGRESS         (1 << 1)
#define SHR_ST_STAT_TYPE_IF_INGRESS_ERRORS (1 << 2)
#define SHR_ST_STAT_TYPE_QUEUE_EGRESS      (1 << 3)

/* Supported export interval (1s) */
#define _SHR_ST_EXPORT_INTERVAL_USECS (1 * 1000 * 1000)

#endif /* _SOC_SHARED_ST_H */

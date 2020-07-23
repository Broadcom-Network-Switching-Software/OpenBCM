/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File:    ifa2.h
 * Purpose: IFA2 definitions common to SDK and uKernel
 */

#ifndef _SOC_SHARED_IFA2_H
#define _SOC_SHARED_IFA2_H

#ifdef BCM_UKERNEL
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif

/*
 * IFA2 uC Error codes
 */
typedef enum shr_ifa2_uc_error_e {
    SHR_IFA2_UC_E_NONE      = 0,
    SHR_IFA2_UC_E_INTERNAL  = 1,
    SHR_IFA2_UC_E_MEMORY    = 2,
    SHR_IFA2_UC_E_UNIT      = 3,
    SHR_IFA2_UC_E_PARAM     = 4,
    SHR_IFA2_UC_E_EMPTY     = 5,
    SHR_IFA2_UC_E_FULL      = 6,
    SHR_IFA2_UC_E_NOT_FOUND = 7,
    SHR_IFA2_UC_E_EXISTS    = 8,
    SHR_IFA2_UC_E_TIMEOUT   = 9,
    SHR_IFA2_UC_E_BUSY      = 10,
    SHR_IFA2_UC_E_FAIL      = 11,
    SHR_IFA2_UC_E_DISABLED  = 12,
    SHR_IFA2_UC_E_BADID     = 13,
    SHR_IFA2_UC_E_RESOURCE  = 14,
    SHR_IFA2_UC_E_CONFIG    = 15,
    SHR_IFA2_UC_E_UNAVAIL   = 16,
    SHR_IFA2_UC_E_INIT      = 17,
    SHR_IFA2_UC_E_PORT      = 18
} shr_ifa2_uc_error_t;

/* Max timeout for msging between SDK and UKERNEL */
#define SHR_IFA2_MAX_UC_MSG_TIMEOUT              5000000 /* 5 secs */

/* Max size of the IFA2 collector encap length */
#define SHR_IFA2_MAX_COLLECTOR_ENCAP_LENGTH      128

/* Max size of the template set encap */
#define SHR_IFA2_MAX_TEMPLATE_SET_ENCAP_LENGTH 256

/* Maximum possible size of the IFA2 metadata in bytes. */
#define SHR_IFA2_MD_MAX_SIZE (32)

/*
 * List of elements that can be part of IFA2 metadata
 */
typedef enum shr_ifa2_md_elements_e {
    /* Invalid element */
    SHR_IFA2_MD_ELEMENT_INVALID                          = 0,

    /* Device ID or (4b LNS + device ID)  */
    SHR_IFA2_MD_ELEMENT_LNS_DEVICE_ID                    = 1,

    /* IP TTL  */
    SHR_IFA2_MD_ELEMENT_IP_TTL                           = 2,

    /* Egress port speed */
    SHR_IFA2_MD_ELEMENT_EGRESS_PORT_SPEED                = 3,

    /* Congestion */
    SHR_IFA2_MD_ELEMENT_CONGESTION                       = 4,

    /* Queue ID */
    SHR_IFA2_MD_ELEMENT_QUEUE_ID                         = 5,

    /* Rx timestamp seconds portion */
    SHR_IFA2_MD_ELEMENT_RX_TIMESTAMP_SEC                 = 6,

    /* Egress system port */
    SHR_IFA2_MD_ELEMENT_EGRESS_SYSTEM_PORT               = 7,

    /* Ingress system port */
    SHR_IFA2_MD_ELEMENT_INGRESS_SYSTEM_PORT              = 8,

    /* Rx timestamp nanosecond portion */
    SHR_IFA2_MD_ELEMENT_RX_TIMESTAMP_NANO_SEC            = 9,

    /* Residence time in nanoseconds */
    SHR_IFA2_MD_ELEMENT_RESIDENCE_TIME_NANO_SEC          = 10,

    /* Opaque data 0 */
    SHR_IFA2_MD_ELEMENT_OPAQUE_DATA_0                    = 11,

    /* Opaque Data 1 */
    SHR_IFA2_MD_ELEMENT_OPAQUE_DATA_1                    = 12,

    /* Opaque Data 2 */
    SHR_IFA2_MD_ELEMENT_OPAQUE_DATA_2                    = 13,

    /* Last value, not usable */
    SHR_IFA2_MD_ELEMENT_COUNT
} shr_ifa2_md_elements_t;

/* Names of the metadata elements. */
#define SHR_IFA2_MD_ELEMENT_NAMES              \
    {                                          \
        "INVALID                ",             \
        "LNS_DEVICE_ID          ",             \
        "IP_TTL                 ",             \
        "EGRESS_PORT_SPEED      ",             \
        "CONGESTION             ",             \
        "QUEUE_ID               ",             \
        "RX_TIMESTAMP_SEC       ",             \
        "EGRESS_SYSTEM_PORT     ",             \
        "INGRESS_SYSTEM_PORT    ",             \
        "RX_TIMESTAMP_NANO_SEC  ",             \
        "RESIDENCE_TIME_NANO_SEC",             \
        "OPAQUE_DATA_0          ",             \
        "OPAQUE_DATA_1          ",             \
        "OPAQUE_DATA_2          "              \
    }

/* Maximum number of records that can be packed in a single export packet. */
#define SHR_IFA2_EXPORT_NUM_RECORDS_MAX (16)


#endif /* _SOC_SHARED_IFA2_H */


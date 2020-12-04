/*! \file mcs_mon_telemetry.h
 *
 * Telemetry messaging and definitions needed
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MCS_MON_TELEMETRY_H
#define MCS_MON_TELEMETRY_H

/*! BTE returned error codes */
typedef enum mcs_shr_st_uc_error_e {
    /*! BTE error code none */
    MCS_SHR_ST_UC_E_NONE      = 0,
    /*! BTE error code internal */
    MCS_SHR_ST_UC_E_INTERNAL  = 1,
    /*! BTE error code out of memory */
    MCS_SHR_ST_UC_E_MEMORY    = 2,
    /*! BTE error code not initialized */
    MCS_SHR_ST_UC_E_UNIT      = 3,
    /*! BTE error code wrong parameter */
    MCS_SHR_ST_UC_E_PARAM     = 4,
    /*! BTE error code empty table */
    MCS_SHR_ST_UC_E_EMPTY     = 5,
    /*! BTE error code table full */
    MCS_SHR_ST_UC_E_FULL      = 6,
    /*! BTE error code entry not found */
    MCS_SHR_ST_UC_E_NOT_FOUND = 7,
    /*! BTE error code entry exists */
    MCS_SHR_ST_UC_E_EXISTS    = 8,
    /*! BTE error code message timeout */
    MCS_SHR_ST_UC_E_TIMEOUT   = 9,
    /*! BTE error code processor busy */
    MCS_SHR_ST_UC_E_BUSY      = 10,
    /*! BTE error code message failed */
    MCS_SHR_ST_UC_E_FAIL      = 11,
    /*! BTE error code feature disabled */
    MCS_SHR_ST_UC_E_DISABLED  = 12,
    /*! BTE error code invalid ID */
    MCS_SHR_ST_UC_E_BADID     = 13,
    /*! BTE error code resource unavailable */
    MCS_SHR_ST_UC_E_RESOURCE  = 14,
    /*! BTE error code wrong configuration */
    MCS_SHR_ST_UC_E_CONFIG    = 15,
    /*! BTE error code feature unavailable */
    MCS_SHR_ST_UC_E_UNAVAIL   = 16,
    /*! BTE error code feature not initialized */
    MCS_SHR_ST_UC_E_INIT      = 17,
    /*! BTE error code invalid port */
    MCS_SHR_ST_UC_E_PORT      = 18
} mcs_shr_st_uc_error_t;


/*! Maximum size of the protobuf packet encap */
#define MCS_SHR_ST_MAX_COLLECTOR_ENCAP_LENGTH (128)

/*! Maximum length of the interface name */
#define MCS_SHR_ST_MAX_IF_NAME_LENGTH (32)

/*! Maximum number of logical ports in all devices supporting ST */
#define MCS_SHR_ST_MAX_PORTS (160)

/*! Maximum length of the system Id in bytes */
#define MCS_SHR_ST_MAX_SYSTEM_ID_LENGTH (32)

/*! Maximum export length supported */
#define MCS_SHR_ST_MAX_EXPORT_LENGTH 9000

/*! Maximum timeout for messaging between SDKLT and UKERNEL */
#define MCS_SHR_ST_MAX_UC_MSG_TIMEOUT 5000000 /* 5 secs */

/*! Telemetry ingress statistics */
#define MCS_SHR_ST_STAT_TYPE_IF_INGRESS        (1 << 0)
/*! Telemetry egress statistics */
#define MCS_SHR_ST_STAT_TYPE_IF_EGRESS         (1 << 1)
/*! Telemetry ingress error statistics */
#define MCS_SHR_ST_STAT_TYPE_IF_INGRESS_ERRORS (1 << 2)
/*! Telemetry egress queue statistics */
#define MCS_SHR_ST_STAT_TYPE_QUEUE_EGRESS      (1 << 3)
/*! Supported export interval (1s) */
#define MCS_SHR_ST_EXPORT_INTERVAL_USECS (1 * 1000 * 1000)

/*! Collector UDP checksum enable */
#define MCS_SHR_ST_COLLECTOR_FLAGS_UDP_CHECKSUM (1 << 0)

#endif /* MCS_MON_TELEMETRY_H */

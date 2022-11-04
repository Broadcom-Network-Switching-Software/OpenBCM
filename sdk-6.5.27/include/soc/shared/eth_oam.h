/*
 * File:    eth_oam.h
 * Purpose: ETH-OAM definitions common to SDK and uKernel
 *
 */

#ifndef _SOC_SHARED_ETH_OAM_H
#define _SOC_SHARED_ETH_OAM_H

#ifdef BCM_UKERNEL
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif

/*
 * ETH_OAM uC Error codes
 */
typedef enum shr_eth_oam_uc_error_e {
    SHR_ETH_OAM_UC_E_NONE      = 0,
    SHR_ETH_OAM_UC_E_INTERNAL  = 1,
    SHR_ETH_OAM_UC_E_MEMORY    = 2,
    SHR_ETH_OAM_UC_E_UNIT      = 3,
    SHR_ETH_OAM_UC_E_PARAM     = 4,
    SHR_ETH_OAM_UC_E_EMPTY     = 5,
    SHR_ETH_OAM_UC_E_FULL      = 6,
    SHR_ETH_OAM_UC_E_NOT_FOUND = 7,
    SHR_ETH_OAM_UC_E_EXISTS    = 8,
    SHR_ETH_OAM_UC_E_TIMEOUT   = 9,
    SHR_ETH_OAM_UC_E_BUSY      = 10,
    SHR_ETH_OAM_UC_E_FAIL      = 11,
    SHR_ETH_OAM_UC_E_DISABLED  = 12,
    SHR_ETH_OAM_UC_E_BADID     = 13,
    SHR_ETH_OAM_UC_E_RESOURCE  = 14,
    SHR_ETH_OAM_UC_E_CONFIG    = 15,
    SHR_ETH_OAM_UC_E_UNAVAIL   = 16,
    SHR_ETH_OAM_UC_E_INIT      = 17,
    SHR_ETH_OAM_UC_E_PORT      = 18
} shr_eth_oam_uc_error_t;

/* Max timeout for msging between SDK and UKERNEL */
#define SHR_ETH_OAM_MAX_UC_MSG_TIMEOUT (5000000) /* 5 secs */

/* PM modes. */
typedef enum shr_eth_oam_pm_mode_e {
    /* No PM enabled. */
    SHR_ETH_OAM_PM_MODE_NONE = 0,

    /* Processed stats. */
    SHR_ETH_OAM_PM_MODE_PROCESSED = 1,

    /* Raw stats. */
    SHR_ETH_OAM_PM_MODE_RAW = 2,

    /* Last, not usable. */
    SHR_ETH_OAM_PM_MODE_COUNT
} shr_eth_oam_pm_mode_t;

/* MEP Types. */
typedef enum shr_eth_oam_mep_types_e {
    /* Port MEP. */
    SHR_ETH_OAM_MEP_TYPE_INVALID = 0,

    /* Port MEP. */
    SHR_ETH_OAM_MEP_TYPE_PORT = 1,

    /* C-VLAN MEP. */
    SHR_ETH_OAM_MEP_TYPE_CVLAN = 2,

    /* SVLAN MEP. */
    SHR_ETH_OAM_MEP_TYPE_SVLAN = 3,

    /* C+SVLAN MEP. */
    SHR_ETH_OAM_MEP_TYPE_C_S_VLAN = 4,

    /* Last value, not usable. */
    SHR_ETH_OAM_MEP_TYPE_COUNT
} shr_eth_oam_mep_types_t;

/* Length of the MEG-ID. */
#define SHR_ETH_OAM_MEG_ID_LENGTH (48)

/* Maximum number of ports in the device. */
#define SHR_ETH_OAM_MAX_PORTS (68)

/* Invalid trunk ID to indicate port is not part of trunk. */
#define SHR_ETH_OAM_INVALID_TID (0xFFFF)

/* Support OAM opcodes. */
typedef enum shr_eth_oam_opcodes_e {
    /* CCM */
    SHR_ETH_OAM_OPCODE_CCM = 1,

    /* 1SL */
    SHR_ETH_OAM_OPCODE_1SL = 53,

    /* SLR */
    SHR_ETH_OAM_OPCODE_SLR = 54,

    /* SLM */
    SHR_ETH_OAM_OPCODE_SLM = 55
} shr_eth_oam_opcodes_t;

/* Supported OAM periods. */
typedef enum shr_eth_oam_periods_e {
    /* Disabled/Invalid. */
    SHR_ETH_OAM_PERIOD_INVALID = 0,

    /* 3.33 msec. */
    SHR_ETH_OAM_PERIOD_3_3_MSEC = 1,

    /* 10 msec. */
    SHR_ETH_OAM_PERIOD_10_MSEC = 2,

    /* 100 msec. */
    SHR_ETH_OAM_PERIOD_100_MSEC = 3,

    /* 1 sec. */
    SHR_ETH_OAM_PERIOD_1_SEC = 4,

    /* 10 sec. */
    SHR_ETH_OAM_PERIOD_10_SEC = 5,

    /* 1 minute. */
    SHR_ETH_OAM_PERIOD_1_MIN = 6,

    /* 10 minutes. */
    SHR_ETH_OAM_PERIOD_10_MIN = 7,
} shr_eth_oam_periods_t;

#define SHR_ETH_OAM_PERIOD_STRINGS          \
    {                                       \
        "Disabled",                         \
        "3.3 msec",                         \
        "10 msec",                          \
        "100 msec",                         \
        "1 sec",                            \
        "10 sec",                           \
        "1 min",                            \
        "10 min"                            \
   }

/* LMEP faults. */
typedef enum shr_eth_oam_lmep_faults_e {
    /* someRDIdefect */
    SHR_ETH_OAM_LMEP_FAULT_SOME_RDI = 0,

    /* someMACStatusDefect */
    SHR_ETH_OAM_LMEP_FAULT_SOME_MAC = 1,

    /* someRMEPCCMdefect */
    SHR_ETH_OAM_LMEP_FAULT_SOME_CCM = 2,

    /* errorCCMdefect */
    SHR_ETH_OAM_LMEP_FAULT_CCM_ERR = 3,

    /* xconCCMdefect */
    SHR_ETH_OAM_LMEP_FAULT_XCON = 4,

    /* Last value, not usable. */
    SHR_ETH_OAM_LMEP_FAULTS_COUNT
} shr_eth_oam_lmep_faults_t;

#define SHR_ETH_OAM_LMEP_FAULT_STRINGS          \
    {                                           \
        "Some RDI",                             \
        "Some MAC",                             \
        "Some CCM",                             \
        "CCM Error",                            \
        "XCON"                                  \
    }

/* LMEP events */
typedef enum shr_eth_oam_lmep_events_e {
    /* someRDIdefect set event. */
    SHR_ETH_OAM_LMEP_EVENT_SOME_RDI = 0,

    /* someRDIdefect clear event. */
    SHR_ETH_OAM_LMEP_EVENT_SOME_RDI_CLR = 1,

    /* someMACStatusDefect set event. */
    SHR_ETH_OAM_LMEP_EVENT_SOME_MAC = 2,

    /* someMACStatusDefect clear event. */
    SHR_ETH_OAM_LMEP_EVENT_SOME_MAC_CLR = 3,

    /* someRMEPCCMdefect set event. */
    SHR_ETH_OAM_LMEP_EVENT_SOME_CCM = 4,

    /* someRMEPCCMdefect clear event. */
    SHR_ETH_OAM_LMEP_EVENT_SOME_CCM_CLR = 5,

    /* errorCCMdefect set event. */
    SHR_ETH_OAM_LMEP_EVENT_CCM_ERR = 6,

    /* errorCCMdefect clear event. */
    SHR_ETH_OAM_LMEP_EVENT_CCM_ERR_CLR = 7,

    /* xconCCMdefect set event. */
    SHR_ETH_OAM_LMEP_EVENT_XCON = 8,

    /* xconCCMdefect clear event. */
    SHR_ETH_OAM_LMEP_EVENT_XCON_CLR = 9,

    /* Last value, not usable. */
    SHR_ETH_OAM_LMEP_EVENTS_COUNT
} shr_eth_oam_lmep_events_t;

/* RMEP faults. */
typedef enum shr_eth_oam_rmep_faults_e {
    /* rMEPCCMdefect */
    SHR_ETH_OAM_RMEP_FAULT_CCM_TIMEOUT = 0,

    /* rMEPlastRDI */
    SHR_ETH_OAM_RMEP_FAULT_RDI = 1,

    /* rMEPportStatusDefect */
    SHR_ETH_OAM_RMEP_FAULT_PORT_DOWN = 2,

    /* rMEPinterfaceStatusDefect */
    SHR_ETH_OAM_RMEP_FAULT_INTF_DOWN = 3,

    /* Last value, not usable. */
    SHR_ETH_OAM_RMEP_FAULTS_COUNT
} shr_eth_oam_rmep_faults_t;

#define SHR_ETH_OAM_RMEP_FAULT_STRINGS          \
    {                                           \
        "CCM Timeout",                          \
        "RDI",                                  \
        "Port Down",                            \
        "Intf Down",                            \
    }

/* RMEP events. */
typedef enum shr_eth_oam_rmep_events_e {
    /* rMEPCCMdefect set event. */
    SHR_ETH_OAM_RMEP_EVENT_CCM_TIMEOUT = 0,

    /* rMEPCCMdefect clear event. */
    SHR_ETH_OAM_RMEP_EVENT_CCM_TIMEOUT_CLR = 1,

    /* rMEPlastRDI set event. */
    SHR_ETH_OAM_RMEP_EVENT_RDI = 2,

    /* rMEPlastRDI clear event. */
    SHR_ETH_OAM_RMEP_EVENT_RDI_CLR = 3,

    /* rMEPportStatusDefect set event. */
    SHR_ETH_OAM_RMEP_EVENT_PORT_STATUS_DEFECT = 4,

    /* rMEPportStatusDefect clear event. */
    SHR_ETH_OAM_RMEP_EVENT_PORT_STATUS_DEFECT_CLR = 5,

    /* Interface status transition: Up --> Down. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_DOWN = 6,

    /* Interface status transition: Up --> Testing. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_TESTING = 7,

    /* Interface status transition: Up --> Unknown. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_UNKNOWN = 8,

    /* Interface status transition: Up --> Dormant. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_DORMANT = 9,

    /* Interface status transition: Up --> Not Present. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_NOT_PRESENT = 10,

    /* Interface status transition: Up --> Lower Layer Down. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_LLDOWN = 11,

    /* Interface status transition: Down --> Up. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_DOWN_TO_UP = 12,

    /* Interface status transition: Testing --> Up. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_TESTING_TO_UP = 13,

    /* Interface status transition: Unknown --> Up. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UNKNOWN_TO_UP = 14,

    /* Interface status transition: Dormant --> Up. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_DORMANT_TO_UP = 15,

    /* Interface status transition: Not Present --> Up. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_NOT_PRESENT_TO_UP = 16,

    /* Interface status transition: Lower Layer Down --> Up. */
    SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_LLDOWN_TO_UP = 17,

    /* Last value, not usable. */
    SHR_ETH_OAM_RMEP_EVENTS_COUNT
} shr_eth_oam_rmep_events_t;

/* PM Raw data version number. */
#define SHR_ETH_OAM_PM_RSTATS_VERSION (0)

/* PM Raw data stats type. */
typedef enum shr_eth_oam_pm_rstats_type_e {
    /* End marker. */
    SHR_ETH_OAM_PM_RSTATS_TYPE_END = 0,

    /* SLM. */
    SHR_ETH_OAM_PM_RSTATS_TYPE_SLM = 1,

    /* Last value, not usable. */
    SHR_ETH_OAM_PM_RSTATS_TYPE_COUNT,
} shr_eth_oam_pm_raw_stats_type_t;

/* Number of raw samples per session. */
#define SHR_ETH_OAM_PM_RSTATS_MAX_SAMPLES (10)

/*
 * Size of the PM raw stats buffer:
 * Header
 *  - Version         - 4B
 *  - Sequence number - 4B
 *
 * Per SLM session:
 *  - Stats type -  1B
 *  - Length     -  2B
 *  - ID         -  2B
 *  - Counters   -  8B (2 counters)
 *
 * End marker - 1B
 *
 */
#define SHR_ETH_OAM_PM_RSTATS_BUF_SIZE(_num_slm_)       \
    (8 + (13 * (_num_slm_) * SHR_ETH_OAM_PM_RSTATS_MAX_SAMPLES) + 1)

/* Length of CCM PDU without any TLVs. */
#define SHR_ETH_OAM_CCM_PDU_MIN_LENGTH (75)

/* Length of SLM PDU without any TLVs. */
#define SHR_ETH_OAM_SLM_PDU_MIN_LENGTH (21)

/* Length of CRC. */
#define SHR_ETH_OAM_CRC_LENGTH (4)

/* Length of MAC address. */
#define SHR_ETH_OAM_MAC_ADDR_LENGTH (6)

/*
 * Minimum length of Tx packet.
 *     - OLP L2 header   - 18B
 *     - OLP Tx header   - 16B
 *     - Min pkt length  - 64B
 */
#define SHR_ETH_OAM_TX_PKT_MIN_LENGTH (98)

/*
 * Maximum length of the Tx CCM packet.
 *   - OLP L2 header   - 18B
 *   - OLP Tx header   - 16B
 *   - Eth encap       - 22B
 *   - CCM PDU         - 75B
 *   - Port status TLV -  4B
 *   - Intf status TLV -  4B
 *   - CRC             -  4B
 *   - Alignment       -  1B
 */
#define SHR_ETH_OAM_CCM_TX_PKT_MAX_LENGTH (144)

/*
 * Maximum length of the Tx SLM packet.
 *   - OLP L2 header   - 18B
 *   - OLP Tx header   - 16B
 *   - Eth encap       - 22B
 *   - SLM PDU         - 21B
 *   - CRC             -  4B
 *   - Padding         - 17B
 *   - Alignment       -  2B
 */
#define SHR_ETH_OAM_SLM_TX_PKT_MAX_LENGTH (100)

#endif /* _SOC_SHARED_ETH_OAM_H */

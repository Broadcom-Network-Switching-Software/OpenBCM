/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_group.h
 * Purpose:     Function declarations for flowtracker group.
 */

#ifndef _BCM_INT_FT_GROUP_H_
#define _BCM_INT_FT_GROUP_H_

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)
#include <shared/bsl.h>
#include <soc/helix5.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/mspi.h>
#include <soc/format.h>
#include <bcm/module.h>
#include <soc/scache.h>
#include <bcm_int/esw/trx.h>
#include <bcm/flowtracker.h>
#include <bcm_int/esw/flowtracker/ft_alu.h>

#define BCMI_CLEANUP_IF_ERROR(rv)      \
            do {                       \
                if (BCM_FAILURE(rv)) { \
                    goto cleanup;      \
                }                      \
            } while(0)

#define BCMI_CLEANUP1_IF_ERROR(rv)     \
            do {                       \
                if (BCM_FAILURE(rv)) { \
                    goto cleanup1;     \
                }                      \
            } while(0)

/**************************************************************************
 | Start - Flowtracker group defines & structs                            |
 */

/* Refresh interval is 15.625us in HX5 and 7.8125us in HR4/FB6 */
#define BCMI_FT_METER_REFRESH_RATE_FLAG_SET(_u_, _f_)               \
    do {                                                            \
        if (SOC_IS_HELIX5(_u_)) {                                   \
            (_f_) |=  _BCM_XGS_METER_FLAG_REFRESH_RATE_15p625;      \
        }                                                           \
    } while (0)

typedef enum bcmi_ft_tcp_flags_e {
    bcmiFtTcpFlagsSYN = 0,
    bcmiFtTcpFlagsACK = 1,
    bcmiFtTcpFlagsRST = 2,
    bcmiFtTcpFlagsPSH = 3,
    bcmiFtTcpFlagsFIN = 4,
    bcmiFtTcpFlagsURG = 5,
    bcmiFtTcpFlagsECE = 6,
    bcmiFtTcpFlagsCWR = 7
} bcmi_ft_tcp_flags_t;

typedef struct bcmi_ft_state_transition_s {
    int current_state;
    int tcp_flags[2];
    int bidirectional;
    int new_learn;
    int next_state;
    int ts_triggers;
    uint32 event_flags;
} bcmi_ft_state_transition_t;

typedef enum bcmi_ft_group_param_type_e {
    bcmiFtGroupParamTypeTracking = 0,
    bcmiFtGroupParamTypeFlowchecker = 1,
    bcmiFtGroupParamTypeCollectorCopy = 2,
    bcmiFtGroupParamTypeMeter = 3,
    bcmiFtGroupParamTypeTsNewLearn = 4,
    bcmiFtGroupParamTypeTsFlowStart = 5,
    bcmiFtGroupParamTypeTsFlowEnd = 6,
    bcmiFtGroupParamTypeTsCheckEvent1 = 7,
    bcmiFtGroupParamTypeTsCheckEvent2 = 8,
    bcmiFtGroupParamTypeTsCheckEvent3 = 9,
    bcmiFtGroupParamTypeTsCheckEvent4 = 10,
    bcmiFtGroupParamTypeTsIngress = 11,
    bcmiFtGroupParamTypeTsEgress = 12
} bcmi_ft_group_param_type_t;

#define BCMI_FT_GROUP_PARAM_TYPE_STRINGS \
{ \
    "Tracking", \
    "Flowchecker", \
    "CollectorCopy", \
    "Meter", \
    "TsNewLearn",  \
    "TsFlowStart", \
    "TsFlowEnd", \
    "TsCheckEvent1", \
    "TsCheckEvent2", \
    "TsCheckEvent3", \
    "TsCheckEvent4", \
    "TsIngress", \
    "TsEgress" \
}

/* Number of tracking params for normalization */
#define BCMI_FT_GROUP_NORM_TRACKING_PARAMS         12

/**************************************************************************
 | Start - Flowtracker timestamp defines & structs                        |
 */

/* max 4 timestamps allowed in one group */
#define BCMI_FT_GROUP_MAX_TS                        4
#define BCMI_FT_GROUP_MAX_TS_TYPE                   9

/* internal group flags to manage run time timestamps */
#define BCMI_FT_GROUP_TS_NEW_LEARN                 (0)
#define BCMI_FT_GROUP_TS_FLOW_START                (1)
#define BCMI_FT_GROUP_TS_FLOW_END                  (2)
#define BCMI_FT_GROUP_TS_CHECK_EVENT1              (3)
#define BCMI_FT_GROUP_TS_CHECK_EVENT2              (4)
#define BCMI_FT_GROUP_TS_CHECK_EVENT3              (5)
#define BCMI_FT_GROUP_TS_CHECK_EVENT4              (6)
#define BCMI_FT_GROUP_TS_INGRESS                   (7)
#define BCMI_FT_GROUP_TS_EGRESS                    (8)

/* Timestamp internal flags */
#define BCMI_FT_TS_INTERNAL_IPAT              (1 << 0)
#define BCMI_FT_TS_INTERNAL_IPDT              (1 << 1)
#define BCMI_FT_TS_INTERNAL_ASSIGNED_IFT_TYPE (1 << 2)
#define BCMI_FT_TS_RESERVED_NEW_LEARN         (1 << 3)
#define BCMI_FT_TS_RESERVED_FLOW_START        (1 << 4)
#define BCMI_FT_TS_RESERVED_FLOW_END          (1 << 5)
#define BCMI_FT_TS_RESERVED_INGRESS           (1 << 6)
#define BCMI_FT_TS_RESERVED_EGRESS            (1 << 7)

/* Timestamp engine selection flags */
#define BCMI_FT_TS_ENGINE_SELECT_ANY               (0)
#define BCMI_FT_TS_ENGINE_SELECT_0            (1 << 0)
#define BCMI_FT_TS_ENGINE_SELECT_1            (1 << 1)
#define BCMI_FT_TS_ENGINE_SELECT_2            (1 << 2)
#define BCMI_FT_TS_ENGINE_SELECT_3            (1 << 3)


/* Tracking param trigger type */
#define BCMI_FT_TS_TYPE_TP                    (1 << 0)
/* Flow check trigger type */
#define BCMI_FT_TS_TYPE_FC                    (1 << 1)

/* Timestamp position in session data bus for
   upper 16bits
 */
#define BCMI_FT_TS_NTP64_UPPER16_LOC              240

#define BCMI_FT_PARAM_TS_ENGINE_SEL_GET(_u_, _flags_, _ts_eng_sel_) \
    do {                                                            \
        _ts_eng_sel_ = BCMI_FT_TS_ENGINE_SELECT_ANY;                \
        if (!soc_feature(_u_, soc_feature_flex_flowtracker_ver_2)) {\
            break;                                                  \
        }                                                           \
        if (_flags_ & BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT1){\
            _ts_eng_sel_ |= BCMI_FT_TS_ENGINE_SELECT_0;             \
        }                                                           \
        if (_flags_ & BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT2){\
            _ts_eng_sel_ |= BCMI_FT_TS_ENGINE_SELECT_1;             \
        }                                                           \
        if (_flags_ & BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT3){\
            _ts_eng_sel_ |= BCMI_FT_TS_ENGINE_SELECT_2;             \
        }                                                           \
        if (_flags_ & BCM_FLOWTRACKER_TRACKING_PARAM_TIMESTAMP_EVENT4){\
            _ts_eng_sel_ |= BCMI_FT_TS_ENGINE_SELECT_3;             \
        }                                                           \
    } while(0)


#define BCMI_FT_CHECK_TS_ENGINE_SEL_GET(_u_, _flags_, _ts_eng_sel_) \
    do {                                                            \
        _ts_eng_sel_ = BCMI_FT_TS_ENGINE_SELECT_ANY;                \
        if (!soc_feature(_u_, soc_feature_flex_flowtracker_ver_2)) {\
            break;                                                  \
        }                                                           \
        if (_flags_ & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1){\
            _ts_eng_sel_ |= BCMI_FT_TS_ENGINE_SELECT_0;             \
        }                                                           \
        if (_flags_ & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2){\
            _ts_eng_sel_ |= BCMI_FT_TS_ENGINE_SELECT_1;             \
        }                                                           \
        if (_flags_ & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT3){\
            _ts_eng_sel_ |= BCMI_FT_TS_ENGINE_SELECT_2;             \
        }                                                           \
        if (_flags_ & BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT4){\
            _ts_eng_sel_ |= BCMI_FT_TS_ENGINE_SELECT_3;             \
        }                                                           \
    } while(0)

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
#define BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(_u_, _sel_, _ft_type_, _r_)\
    do {                                                            \
        int _i_ = 0;                                                \
        (_r_) = TRUE;                                               \
        if ((_sel_) == BCMI_FT_TS_ENGINE_SELECT_ANY) {              \
            break;                                                  \
        }                                                           \
        for (_i_ = 0; _i_ < 4; _i_++) {                             \
            if ((_sel_) & (BCMI_FT_TS_ENGINE_SELECT_0 << _i_)) {    \
                if (BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(_u_, _i_)      \
                                                    != _ft_type_) { \
                    (_r_) = FALSE;                                  \
                    break;                                          \
                }                                                   \
            }                                                       \
        }                                                           \
    } while(0)
#else
#define BCMI_FT_TS_ENGINE_SEL_FT_TYPE_CHECK(_u_, _sel_, _ft_type_, _r_)\
        (_r_) = TRUE
#endif

#define BCMI_FT_TS_ENGINE_SEL_TO_GROUP_TS_TRIGGER(_sel_, _trig_)    \
    do {                                                            \
        if ((_sel_) == BCMI_FT_TS_ENGINE_SELECT_0) {                \
            (_trig_) = BCMI_FT_GROUP_TS_CHECK_EVENT1;               \
        } else if ((_sel_) == BCMI_FT_TS_ENGINE_SELECT_1) {         \
            (_trig_) = BCMI_FT_GROUP_TS_CHECK_EVENT2;               \
        } else if ((_sel_) == BCMI_FT_TS_ENGINE_SELECT_2) {         \
            (_trig_) = BCMI_FT_GROUP_TS_CHECK_EVENT3;               \
        } else /* ((_sel_) == BCMI_FT_TS_ENGINE_SELECT_3) */ {      \
            (_trig_) = BCMI_FT_GROUP_TS_CHECK_EVENT4;               \
        }                                                           \
    } while(0)

#define BCMI_FT_TS_GROUP_TRIGGER_TO_TS_ENGINE_SEL(_trig_, _sel_)    \
    do {                                                            \
        if ((_trig_) == BCMI_FT_GROUP_TS_CHECK_EVENT1) {            \
            (_sel_) = BCMI_FT_TS_ENGINE_SELECT_0;                   \
        } else if ((_trig_) == BCMI_FT_GROUP_TS_CHECK_EVENT2) {     \
            (_sel_) = BCMI_FT_TS_ENGINE_SELECT_1;                   \
        } else if ((_trig_) == BCMI_FT_GROUP_TS_CHECK_EVENT3) {     \
            (_sel_) = BCMI_FT_TS_ENGINE_SELECT_2;                   \
        } else /* (_trig_) == BCMI_FT_GROUP_TS_CHECK_EVENT4 */ {    \
            (_sel_) = BCMI_FT_TS_ENGINE_SELECT_3;                   \
        }                                                           \
    } while(0)

/* Internal MACROs for FlowExceed Profile usage */
#define BCMI_FT_GROUP_FLOW_EXCEED_PROFILE_FLOW_LIMIT    (1 << 0)
#define BCMI_FT_GROUP_FLOW_EXCEED_PROFILE_RESERVATION   (1 << 1)

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)

/* Timestamp Source related base structure. */
typedef struct bcmi_ft_group_ts_source_s {

    /* Delay mode value. */
    uint8 delay_mode;

    /* TS_PROFILE hw trigger value. */
    uint8 ts_profile_hw_trig_val;

    /* Alu16/32 hw trigger value. */
    uint8 check_hw_trig_val;

    /* Internal trigger source. */
    uint8 trig;

} bcmi_ft_group_ts_source_t;

extern
bcmi_ft_group_ts_source_t
    bcmi_ft_group_ts_source_info[bcmFlowtrackerTimestampSourceCount];

#define BCMI_FT_GROUP_TS_SOURCE_DELAY_MODE(ts_src)  \
    (bcmi_ft_group_ts_source_info[ts_src].delay_mode)

#define BCMI_FT_GROUP_TS_PROFILE_HW_SOURCE(ts_src)  \
    (bcmi_ft_group_ts_source_info[ts_src].ts_profile_hw_trig_val)

#define BCMI_FT_GROUP_TS_CHECK_HW_SOURCE(ts_src)  \
    (bcmi_ft_group_ts_source_info[ts_src].check_hw_trig_val)

#define BCMI_FT_GROUP_TS_TRIGGER(ts_src)  \
    (bcmi_ft_group_ts_source_info[ts_src].trig)

#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

/**************************************************************************
 | End - Flowtracker timestamp defines & structs                          |
 */

/**************************************************************************
 | Start - Flowtracker data engine defines & structs                      |
 */
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)

/* Index to Internal Flowchecks */
#define BCMI_FT_INT_CHECK_FLOWSTART_IDX     0
#define BCMI_FT_INT_CHECK_FLOWEND_IDX       1
#define BCMI_FT_INT_CHECK_CHECKEVENT1_IDX   2
#define BCMI_FT_INT_CHECK_CHECKEVENT2_IDX   3
#define BCMI_FT_INT_CHECK_CHECKEVENT3_IDX   4
#define BCMI_FT_INT_CHECK_CHECKEVENT4_IDX   5
#define BCMI_FT_INT_CHECK_MAX               6

/* Total Internal Group ALU info */
#define BCMI_FT_INT_ALU_INFO_MAX           10

/* Enum for Data Engine */
typedef enum bcmi_ft_data_engine_type_e {
    /* Data engine type Timestamp (48b) */
    bcmiFtDataEngineTypeTimestamp     = 0,

    /* Data engine type Metering */
    bcmiFtDataEngineTypeMeter         = 1,

    /* Data engine type ALU32 */
    bcmiFtDataEngineTypeAlu32         = 2,

    /* Data engine type LOAD16 */
    bcmiFtDataEngineTypeLoad16        = 3,

    /* Data engine type ALU16 */
    bcmiFtDataEngineTypeAlu16         = 4,

    /* Data engine type Copy to Collector */
    bcmiFtDataEngineTypeCollectorCopy = 5,

    /* Data engine type LOAD8 */
    bcmiFtDataEngineTypeLoad8         = 6,

    /* Data engine type Flow State index */
    bcmiFtDataEngineTypeFlowState     = 7,

    bcmiFtDataEngineTypeCount
} bcmi_ft_data_engine_type_t;

#define BCM_FT_DATA_ENGINE_TYPE_STRINGS \
{  \
    "Timestamp", \
    "Meter", \
    "Alu32", \
    "Load16", \
    "Alu16", \
    "CollectorCopy", \
    "Load8", \
    "FlowState", \
    "Count"  \
}

/* Next data engine in the list is linked */
#define BCMI_FT_DATA_ENGINE_NEXT_ATTACH (1 << 0)

/* Struct for data engine info */
typedef struct bcmi_ft_data_engine_info_s {
    /* Flags */
    uint32 flags;

    /* Data engine type */
    bcmi_ft_data_engine_type_t engine_type;

    /* Index to data engine */
    int index;

    /* H/w field for the data engine */
    soc_field_t field;

    /* Number of bits used by the data engine */
    int size;

    /* Offset from end in expanded form */
    int rev_pde_data_offset;

    /* Offset in PDD profile */
    int pdd_offset;

    /* Pointer to group alu info using this data engine */
    bcmi_ft_group_alu_info_t *alu_info;

    /* Is the data engine used by FT Group */
    uint8 enabled;

} bcmi_ft_data_engine_info_t;

/* Total FT Data Engines */
#define TOTAL_FT_DATA_ENGINES       \
    (TOTAL_GROUP_ALU32_MEM   +      \
     TOTAL_GROUP_ALU16_MEM   +      \
     TOTAL_GROUP_LOAD16_DATA_NUM  +      \
     TOTAL_GROUP_LOAD8_DATA_NUM   +      \
     TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM + \
     1 + 1 + 1)  /* meter + state + copyToCpu */

/* Struct for data engine order mapping */
typedef struct bcmi_ft_data_engine_per_ft_type_map_s {
    /* Number of data engine in use */
    int count;

    /* Ordered list of data engines */
    bcmi_ft_data_engine_info_t *list[TOTAL_FT_DATA_ENGINES];

} bcmi_ft_data_engine_per_ft_type_map_t;

/* Struct for data engine db */
typedef struct bcmi_ft_data_engine_db_s {
    /* Total bits in expanded form */
    int total_bits;

    /* mapping of data engines per ft type */
    bcmi_ft_data_engine_per_ft_type_map_t ft_type_map[bcmiFtTypeCount];

    /* Number of Timestamp engines */
    int ts_count;

    /* List of data engines for timestamp */
    bcmi_ft_data_engine_info_t ts[TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM];

    /* Data engine for Meter */
    bcmi_ft_data_engine_info_t meter;

    /* Number of ALU32 data engines */
    int alu32_count;

    /* List of data engines for ALU32 */
    bcmi_ft_data_engine_info_t alu32[TOTAL_GROUP_ALU32_MEM];

    /* Number of LOAD16 data engines */
    int load16_count;

    /* List of data engines for LOAD16 */
    bcmi_ft_data_engine_info_t load16[TOTAL_GROUP_LOAD16_DATA_NUM];

    /* Number of ALU16 data engines */
    int alu16_count;

    /* List of data engiens for ALU16 */
    bcmi_ft_data_engine_info_t alu16[TOTAL_GROUP_ALU16_MEM];

    /* Data engine for Copy to Collector */
    bcmi_ft_data_engine_info_t collector_copy;

    /* Number of LAOD8 data engines */
    int load8_count;

    /* List of data engines for LOAD8 */
    bcmi_ft_data_engine_info_t load8[TOTAL_GROUP_LOAD8_DATA_NUM];

    /* Data engine for Flow State */
    bcmi_ft_data_engine_info_t flow_state;

} bcmi_ft_data_engine_db_t;
#endif

/**************************************************************************
 | Ends - Flowtracker data engine defines & structs                       |
 */


typedef enum bcmi_ft_group_key_data_mode_s {
    bcmiFtGroupModeSingle = 0,
    bcmiFtGroupModeDouble = 1
} bcmi_ft_group_key_data_mode_t;

typedef uint32 bcmi_ft_hw_extractor_info_t;

/* extraction info. */
typedef struct bcmi_ft_group_extractor_info_s {
    bcmi_ft_group_key_data_mode_t ft_key_mode;
    bcmi_ft_group_key_data_mode_t ft_data_mode;
    int num_ft_key;
    bcmi_ft_hw_extractor_info_t *ft_key;
    int num_ft_data;
    bcmi_ft_hw_extractor_info_t *ft_data;
} bcmi_ft_group_extractor_info_t;

/* Structure to maintain session profiles. */
/* Populated and used by FTFP sub-module. */
typedef struct bcmi_ft_session_profiles_s {

    /* Session Key Profile Index. */
    uint32 session_key_profile_idx;

    /* Session Data Profile index. */
    uint32 session_data_profile_idx;

    /* Session Alu Data Profile index. */
    uint32 alu_data_profile_idx;

} bcmi_ft_session_profiles_t;

typedef struct bcmi_ft_group_ftfp_data_s {

    bcm_flowtracker_group_control_type_t dir_ctrl_type;

    bcm_flowtracker_direction_t direction;

    int flowtrack;

    int new_learn;

    uint32 session_key_type;

    /* Session key mode. */
    bcmi_ft_group_key_data_mode_t session_key_mode;

    /* FTFP profile information. */
    bcmi_ft_session_profiles_t profiles;

} bcmi_ft_group_ftfp_data_t;


/* template info linked list. */
typedef struct bcmi_ft_group_template_info_s {

    /* alu/load flags. */
    uint32 flags;

    /* Container offset from start of Session Data PDD */
    int cont_offset;

    /* Data shift from start of Container */
    int data_shift;

    /* Data width */
    int data_width;

    /* Width of Container */
    int cont_width;

    /* Tracking param Type */
    bcmi_ft_group_param_type_t param_type;

    /* Tracking param */
    bcm_flowtracker_tracking_param_type_t param;

    /* Flowtracker Check */
    bcm_flowtracker_check_t check_id;

    /* Index in ALU/Load Engine */
    int index;

    /* Hw Engine */
    bcmi_ft_alu_load_type_t type;

    /* Flow Direction */
    bcm_flowtracker_direction_t direction;

    /* Custom Id */
    uint16 custom_id;

} bcmi_ft_group_template_info_t;

typedef struct bcmi_ft_group_config_s {

    /* User tracking params. */
    int num_tracking_params;
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params;

    /* Ordered tracking params. */
    int num_ordered_template_params;
    bcm_flowtracker_tracking_param_info_t *list_of_ordered_template_params;

    /* Group Class. */
    uint32 class_id;

    /* State transition type */
    bcm_flowtracker_state_transition_type_t st_type;

} bcmi_ft_group_config_t;

typedef struct bcmi_ft_group_meter_s {

    uint32 sw_ckbits_sec;

    uint32 sw_ckbits_burst;

} bcmi_ft_group_meter_t;

typedef struct bcmi_ft_group_extraction_info_s {

    /* Inforamtion needed for ALU allocation scheme. */
    int num_data_info;
    bcmi_ft_group_alu_info_t *ft_data_info;

    /* Session key information for the FT. */
    int num_key_info;
    bcmi_ft_group_alu_info_t *ft_key_info;

    /* Hardware extractor inforamtion needed for FT. */
    bcmi_ft_group_extractor_info_t ft_ext_info;

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    /* VFP OpaqueCtrlId2 start bit for NTP64b war */
    int vfp_opaque2_start_bit;
#endif
} bcmi_ft_group_extraction_info_t;

/* The linked list used to form export template */
typedef struct bcmi_ft_group_template_list_s {

    bcmi_ft_group_template_info_t info;

    struct bcmi_ft_group_template_list_s *next;

} bcmi_ft_group_template_list_t;

/* Trigger info for a group */
typedef struct bcmi_ft_group_trigger_info_s {

    /* Timestamp flags. */
    uint8 ts_flags;

    /* Timestamp bank. */
    uint8 ts_bank;

    /* Timestamp reference count. */
    uint16 ts_ref_count;

    /* Timestamp flowtracker type. */
    bcmi_ft_type_t ts_ft_type;

    /* Timestamp engine selection, Sync/Recv not req */
    uint8 ts_engine_sel;

} bcmi_ft_group_trigger_info_t;

/* User entries are supported on this group */
#define BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY  (1 << 0)

/* Periodic export enabled */
#define BCMI_FT_GROUP_INFO_F_PERIODIC_EXPORT    (1 << 1)

/* Main internal group info db structure */
typedef struct bcmi_ft_group_info_s {

    /* Group config. */
    bcmi_ft_group_config_t group_config;

    /* Associated meter info. */
    bcmi_ft_group_meter_t meter_info;

    /* Group flow checker list. */
    bcmi_ft_flowchecker_list_t *head;

    /* Associated collector id. */
    bcm_flowtracker_collector_t  collector_id;

    /* Associated template id */
    bcm_flowtracker_export_template_t template_id;

    /* Group extraction information. */
    bcmi_ft_group_extraction_info_t group_ext_info;

    /* Flags */
    uint32 flags;

    /* Total number of ftfp entries pointing to this group. */
    uint32 num_ftfp_entries;

    /* FTFP data in group state. */
    bcmi_ft_group_ftfp_data_t ftfp_data;

    /* We will have a pdd profile entry here. */
    bsd_policy_action_profile_entry_t pdd_entry;

    /* ALU32 MEM allocation bitmap. */
    uint32 *ALU32_MEM_USE;

    /* ALU16 MEM allocation bitmap. */
    uint32 ALU16_MEM_USE[TOTAL_GROUP_ALU16_MEM];

    /* List for template information. */
    bcmi_ft_group_template_list_t *template_head;

    /* Timestamps triggers on this group. */
    uint32 ts_triggers;

    /* Trigger internal information. */
    bcmi_ft_group_trigger_info_t ts_info[BCMI_FT_GROUP_MAX_TS_TYPE];

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    /* Session Data Engine db */
    bcmi_ft_data_engine_db_t *engine_db;

    /* Internal Flowchecks */
    bcmi_flowtracker_flowchecker_info_t *int_fc_info_arr[BCMI_FT_INT_CHECK_MAX];
#endif

    /* Timestamp triggers for tracking params */
    uint32 param_ts_triggers;

    /* Validation status. */
    int validated;

} bcmi_ft_group_sw_info_t;

 typedef struct bcmi_ft_group_bitmap_s {
     SHR_BITDCL *ft_group_bitmap;
 } bcmi_ft_group_bitmap_t;

/* Macro for flowtracker group type */
#define BCMI_FT_GROUP_TYPE_COUNT bcmFlowtrackerGroupTypeCount

/* Flowtracker Group Info */
typedef struct bcmi_ft_group_info_db_s {
    /* Group Sw State */
    bcmi_ft_group_sw_info_t **sw_info[BCMI_FT_GROUP_TYPE_COUNT];

    /* Group Bitmap */
    bcmi_ft_group_bitmap_t bitmap[BCMI_FT_GROUP_TYPE_COUNT];

    /* Group Count */
    uint32 count[BCMI_FT_GROUP_TYPE_COUNT];

    /* Aggregate Periodic Export Interva */
    uint32 aft_periodic_interval;

} bcmi_ft_group_info_db_t;

/* Get Group Type from Flow group Id */
#define BCMI_FT_GROUP_TYPE_GET(_id_)                                    \
        (((_id_) >> BCM_FLOWTRACKER_GROUP_TYPE_SHIFT) & 0x3F)

/* Get Group Index from Flow group Id */
#define BCMI_FT_GROUP_INDEX_GET(_id_)                                   \
        ((_id_) & ((1 << BCM_FLOWTRACKER_GROUP_TYPE_SHIFT) - 1))

#define BCMI_FT_GROUP_TYPE_IS_AGGREGATE(_id_)                           \
        ((BCMI_FT_GROUP_TYPE_GET(_id_) ==                               \
          bcmFlowtrackerGroupTypeAggregateIngress) ||                   \
         (BCMI_FT_GROUP_TYPE_GET(_id_) ==                               \
          bcmFlowtrackerGroupTypeAggregateMmu)     ||                   \
         (BCMI_FT_GROUP_TYPE_GET(_id_) ==                               \
          bcmFlowtrackerGroupTypeAggregateEgress))

/* Internal flowtracker group info db*/
 extern
bcmi_ft_group_info_db_t bcmi_ft_group_info_db[BCM_MAX_NUM_UNITS];

#define BCMI_FT_GROUP_INFO(_unit_) (&bcmi_ft_group_info_db[(_unit_)])

#define BCMI_FT_GROUP_BITMAP(_unit_, _group_type_)         \
    (bcmi_ft_group_info_db[(_unit_)].bitmap[(_group_type_)].ft_group_bitmap)

#define BCMI_FT_GROUP_SW_STATE(_unit_, _group_type_)        \
    (bcmi_ft_group_info_db[(_unit_)].sw_info[_group_type_])

#define BCMI_FT_GROUP_PER_TYPE(_unit_, _group_type_, _index_)       \
    (bcmi_ft_group_info_db[(_unit_)].sw_info[(_group_type_)][(_index_)])

#define BCMI_FT_GROUP_TYPE_MAX_COUNT(_unit_, _group_type_)       \
    (bcmi_ft_group_info_db[(_unit_)].count[(_group_type_)])

#define BCMI_FT_GROUP(_unit_, _id_)                         \
    BCMI_FT_GROUP_PER_TYPE((_unit_), BCMI_FT_GROUP_TYPE_GET(_id_),  \
            BCMI_FT_GROUP_INDEX_GET(_id_))

#define BCMI_FT_GROUP_CONFIG(unit, id)  \
     ((BCMI_FT_GROUP(unit, id))->group_config)

#define BCMI_FT_GROUP_IS_BUSY(unit, id)  \
     ((BCMI_FT_GROUP(unit, id))->num_ftfp_entries != 0)

#define BCMI_FT_GROUP_IS_VALIDATED(unit, id)  \
     ((BCMI_FT_GROUP(unit, id))->validated)

#define BCMI_FT_GROUP_FTFP_DATA(unit, id) \
     ((BCMI_FT_GROUP(unit, id))->ftfp_data)

#define BCMI_FT_GROUP_TRACK_PARAM(unit, id)  \
     (((BCMI_FT_GROUP(unit, id))->group_config).list_of_tracking_params)

#define BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id)  \
     (((BCMI_FT_GROUP(unit, id))->group_config).num_tracking_params)

#define BCMI_FT_GROUP_ORDERED_TRACK_PARAM_NUM(unit, id)  \
     (((BCMI_FT_GROUP(unit, id))->group_config).num_ordered_template_params)

#define BCMI_FT_GROUP_ORDERED_TRACK_PARAM(unit, id)  \
     (((BCMI_FT_GROUP(unit, id))->group_config).list_of_ordered_template_params)

#define BCMI_FT_GROUP_CHECK_LIST(unit, id)  \
     ((BCMI_FT_GROUP(unit, id))->head)

#define BCMI_FT_GROUP_EXT_INFO(unit, id)  \
     ((BCMI_FT_GROUP(unit, id))->group_ext_info)

#define BCMI_FT_GROUP_EXT_DATA_INFO(unit, id)  \
     (((BCMI_FT_GROUP(unit, id))->group_ext_info).ft_data_info)

#define BCMI_FT_GROUP_EXT_KEY_INFO(unit, id)  \
     (((BCMI_FT_GROUP(unit, id))->group_ext_info).ft_key_info)

#define BCMI_FT_GROUP_EXT_HW_INFO(unit, id)  \
     (((BCMI_FT_GROUP(unit, id))->group_ext_info).ft_ext_info)

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
#define BCMI_FT_GROUP_EXT_HW_INFO_VFP_OPAQUEID2_START_BIT(unit, id)  \
    (((BCMI_FT_GROUP(unit, id))->group_ext_info).vfp_opaque2_start_bit)
#endif

#define BCMI_FT_GROUP_FLOWCHECKER_HEAD(unit, id)  \
     (BCMI_FT_GROUP(unit, id)->head)

#define BCMI_FT_GROUP_METER_INFO(unit, id)  \
     (BCMI_FT_GROUP(unit, id)->meter_info)

#define BCMI_FT_GROUP_ALU32_MEM_USE(unit, id) \
     (BCMI_FT_GROUP(unit, id)->ALU32_MEM_USE)

#define BCMI_FT_GROUP_ALU16_MEM_USE(unit, id) \
     (BCMI_FT_GROUP(unit, id)->ALU16_MEM_USE)

#define BCMI_FT_GROUP_ALU32_MEM_USE_GET(unit, id, mem_idx) \
     (BCMI_FT_GROUP(unit, id)->ALU32_MEM_USE[mem_idx])

#define BCMI_FT_GROUP_ALU16_MEM_USE_GET(unit, id, mem_idx) \
     (BCMI_FT_GROUP(unit, id)->ALU16_MEM_USE[mem_idx])

#define BCMI_FT_GROUP_TS_TRIGGERS(unit, id) \
     (BCMI_FT_GROUP(unit, id)->ts_triggers)

#define BCMI_FT_GROUP_TS_TRIGGER_INFO(unit, id) \
     (BCMI_FT_GROUP(unit, id)->ts_info)

#define BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, trig) \
     (BCMI_FT_GROUP(unit, id)->ts_info[trig].ts_flags)

#define BCMI_FT_GROUP_TS_TRIGGER_ENGINE_SEL(unit, id, trig) \
     (BCMI_FT_GROUP(unit, id)->ts_info[trig].ts_engine_sel)

#define BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, trig) \
     (BCMI_FT_GROUP(unit, id)->ts_info[trig].ts_bank)

#define BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, trig) \
     (BCMI_FT_GROUP(unit, id)->ts_info[trig].ts_ref_count)

#define BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_INCR(unit, id, trig) \
     (BCMI_FT_GROUP(unit, id)->ts_info[trig].ts_ref_count++)

#define BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT_DECR(unit, id, trig)    \
    if (BCMI_FT_GROUP(unit, id)->ts_info[trig].ts_ref_count > 0) { \
        (BCMI_FT_GROUP(unit, id)->ts_info[trig].ts_ref_count--);   \
    }

#define BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, trig) \
     (BCMI_FT_GROUP(unit, id)->ts_info[trig].ts_ft_type)

#define BCMI_FT_GROUP_PARAM_TS_TRIGGERS(unit, id) \
    (BCMI_FT_GROUP(unit, id)->param_ts_triggers)

#define BCMI_FT_GROUP_EXT_DATA_OR_TS_TRIGGERS_SET(unit, id) \
            ((BCMI_FT_GROUP_EXT_DATA_INFO(unit, id) != NULL) || \
             (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) != 0))

typedef enum bcmi_ft_group_param_e {
    bcmiFtGroupParamFtfpEntryNum,
    bcmiFtGroupParamCount
} bcmi_ft_group_params_t;

typedef enum bcmi_ft_group_params_operation_e {
    bcmiFtGroupParamsOprInc,
    bcmiFtGroupParamsOprDec,
    bcmiFtGroupParamsOprUpdate,
    bcmiFtGroupParamOprCount
} bcmi_ft_group_params_operation_t;

/* MAximum 4 groups can be flushed one time.*/
#define BCMI_FT_GROUP_FLUSH_MAX_SCALE             4

typedef struct bcmi_ft_group_flushinfo_s {
    uint32 group_valid;
    bcm_flowtracker_group_t group_id;
} bcmi_ft_group_flush_info_t;
/**************************************************************************/



/**************************************************************************
 | Start - Flow checker defines & structs                                 |
 */

/* Flowtracker Index generation. */
#define  BCM_FT_IDX_TYPE_CHECK               1
#define  BCM_FT_IDX_TYPE_2ND_CHECK           2
#define  BCM_FT_IDX_TYPE_INTERNAL            0x3f      /* For Internal use */
#define  BCM_FT_IDX_TYPE_SHIFT               26
#define  BCM_FT_IDX_MASK                     0x3ffffff

#define BCMI_FT_FLOWCHECKER_INDEX_SET(index, type) \
    ((type << BCM_FT_IDX_TYPE_SHIFT) | (index))

#define BCMI_FT_FLOWCHECKER_INDEX_GET(index) \
    ((index) & BCM_FT_IDX_MASK)

#define BCMI_FT_IDX_IS_CHECK(index) \
    ((((index) >> BCM_FT_IDX_TYPE_SHIFT) == BCM_FT_IDX_TYPE_CHECK) || \
     (((index) >> BCM_FT_IDX_TYPE_SHIFT) == BCM_FT_IDX_TYPE_2ND_CHECK))

#define BCMI_FT_IDX_IS_PRIMARY_CHECK(index) \
    (((index) >> BCM_FT_IDX_TYPE_SHIFT) == BCM_FT_IDX_TYPE_CHECK)

#define BCMI_FT_IDX_IS_2ND_CHECK(index) \
    (((index) >> BCM_FT_IDX_TYPE_SHIFT) == BCM_FT_IDX_TYPE_2ND_CHECK)

/* For Internal Flowcheck 6b(checktype)+16b(group idx)+10b(internal check idx)*/
#define BCMI_FT_INT_CHECK_GROUP_IDX_SHIFT    10
#define BCMI_FT_INT_CHECK_GROUP_IDX_MASK     0xffff
#define BCMI_FT_INT_CHECK_IDX_MASK           0x3ff

#define BCMI_FT_IDX_IS_INT_CHECK(index) \
    (((index) >> BCM_FT_IDX_TYPE_SHIFT) == BCM_FT_IDX_TYPE_INTERNAL)

/* Get FT Group Index (only for Internal Flowcheck) */
#define BCMI_FT_INT_CHECK_GROUP_IDX_GET(index)          \
    (((index) >> BCMI_FT_INT_CHECK_GROUP_IDX_SHIFT) &   \
            BCMI_FT_INT_CHECK_GROUP_IDX_MASK)

/* Set FT Group Index (only for Internal Flowcheck) */
#define BCMI_FT_INT_CHECK_GROUP_IDX_SET(index, group_id) \
    ((((group_id) & BCMI_FT_INT_CHECK_GROUP_IDX_MASK) << \
      BCMI_FT_INT_CHECK_GROUP_IDX_SHIFT) | index)

/* Get FT Internal check Index (only for Internal Flowcheck) */
#define BCMI_FT_INT_CHECK_IDX_GET(index) \
    ((index) & BCMI_FT_INT_CHECK_IDX_MASK)

/* Options for bcmi_ft_group_check_validate */
#define BCMI_FT_GROUP_CHECK_MATCH_GROUP      (1 << 0)
#define BCMI_FT_GROUP_CHECK_MATCH_PRIMARY    (1 << 1)

/* Group Record Traverse metadata */
typedef struct bcmi_ft_group_record_trav_data_s {
    /* Metadata flags */
    uint32 flags;

    /* Traverse group */
    bcm_flowtracker_group_t group_id;

    /* FT type bitmap for group */
    uint32 ft_type_bmp;

    /* Current traverse index */
    uint32 index;

    /* Group Record content type */
    bcm_flowtracker_group_record_type_t record_type;

    /* Pointer to group record info*/
    bcm_flowtracker_group_record_info_t *record_info;

    /* User callback function */
    bcm_flowtracker_group_record_traverse_cb cb;

    /* User data */
    void *user_data;

    /* Export record metadata offset */
    int md_offset;

    /* Export record key offset */
    int key_offset;

    /* Export record data offset */
    int data_offset;

    /* Number of bits in Flow Group */
    int group_idx_bits;

    /* uint8 buffers */
    uint8 *u8buffer1;
    uint8 *u8buffer2;

    /* uint32 buffers */
    uint32 *u32buffer1;
    uint32 *u32buffer2;
    uint32 *u32buffer3;

} bcmi_ft_group_record_trav_data_t;

#define BCMI_FT_GROUP_RECORD_TRV_F_GROUP            (1 << 0)
#define BCMI_FT_GROUP_RECORD_TRV_F_KEY              (1 << 1)
#define BCMI_FT_GROUP_RECORD_TRV_F_DATA             (1 << 2)

typedef int (*bcmi_ft_group_hw_entry_get_cb_f)(int unit,
                soc_mem_t mem, uint32 index, void *entry,
                void *user_data, int *move);


/**************************************************************************
 | End - Flow checker defines & structs                                   |
 */


/*****************************************************
 | Start - Flowtracker module utility Methods.       |
 */
extern int
ft_initialized[BCM_MAX_NUM_UNITS];

extern int
bcmi_ft_init(int unit);

extern int
bcmi_ft_cleanup(int unit);
/*****************************************************
 | End - Flowtracker module utility Methods.         |
 */



/*****************************************************
 | Start - Flow Checker utility Methods.             |
 */
/* Init routines */
extern int
bcmi_ft_flowchecker_init(int unit);

extern void
bcmi_ft_flowchecker_cleanup(int unit);

/* Create Flow check and return software ID. */
extern int
bcmi_ft_check_create(int unit,
                     uint32 options,
                     bcm_flowtracker_check_info_t check_info,
                     bcm_flowtracker_check_t *check_id);

/* Destroy flow check corresponding to this software ID. */
extern int
bcmi_ft_check_destroy(int unit,
                      bcm_flowtracker_check_t check_id);

/* Destroy all flow checks not linked to any group. */
extern int
bcmi_ft_check_destroy_all(int unit);

/* Get information of flow check. */
extern int
bcmi_ft_check_get(int unit,
                  bcm_flowtracker_check_t check_id,
                  bcm_flowtracker_check_info_t *check_info,
                  uint16 *ft_type_bmp);

/* Routine to traverse through all the flowcheckers. */
extern int
bcmi_ft_check_traverse(int unit,
                       bcm_flowtracker_check_traverse_cb cb,
                       void *user_data);

/* Get information of flow check for complete info */
extern int
bcmi_ft_flowchecker_get(int unit,
                        bcm_flowtracker_check_t flowchecker_id,
                        bcmi_flowtracker_flowchecker_info_t *flow_check_info);

extern int
bcmi_ft_check_action_info_set(int unit,
                              bcm_flowtracker_check_t check_id,
                              bcm_flowtracker_check_action_info_t info);

extern int
bcmi_ft_check_action_info_get(int unit,
                              bcm_flowtracker_check_t check_id,
                              bcm_flowtracker_check_action_info_t *info);

extern int
bcmi_ft_check_export_info_get(int unit,
                              bcm_flowtracker_check_t check_id,
                              bcm_flowtracker_check_export_info_t *info);

extern int
bcmi_ft_check_export_info_set(int unit,
                              bcm_flowtracker_check_t check_id,
                              bcm_flowtracker_check_export_info_t info);

extern int
bcmi_ft_check_action_num_operands_get(int unit,
                              bcm_flowtracker_check_action_t action,
                              int *num_operands);

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
extern int
bcmi_ftv2_check_delay_info_get(int unit,
                               bcm_flowtracker_check_t check_id,
                               bcm_flowtracker_check_delay_info_t *info);

extern int
bcmi_ftv2_check_delay_info_set(int unit,
                               bcm_flowtracker_check_t check_id,
                               bcm_flowtracker_check_delay_info_t info);
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

extern int
bcmi_ft_flowchecker_list_length_get(int unit,
                                    bcmi_ft_flowchecker_list_t **head);

extern int
bcmi_ft_flowchecker_list_add(int unit,
                             bcmi_ft_flowchecker_list_t **head,
                             bcm_flowtracker_check_t flowchecker_id);

extern int
bcmi_ft_flowchecker_list_delete(int unit,
                                bcmi_ft_flowchecker_list_t **head,
                                bcm_flowtracker_check_t flowchecker_id);

extern int
bcmi_ft_group_flowchecker_add(int unit,
                              bcm_flowtracker_group_t id,
                              bcm_flowtracker_check_t flowchecker_id);

extern int
bcmi_ft_group_flowchecker_delete(int unit,
                                 bcm_flowtracker_group_t id,
                                 bcm_flowtracker_check_t flowchecker_id);

extern int
bcmi_flowtracker_check_get_all(int unit,
                               bcm_flowtracker_group_t group_id,
                               int skip_non_primary,
                               int max_checks,
                               bcm_flowtracker_check_t *list_of_check_ids,
                               int *num_checks);

extern int
bcmi_ft_check_compare(int unit,
                      bcm_flowtracker_check_t check_id1,
                      bcm_flowtracker_check_t check_id2);

extern int
bcmi_ft_check_list_compare(int unit,
                           bcmi_ft_flowchecker_list_t **head,
                           bcm_flowtracker_check_t check_id);

extern int
bcmi_ft_group_check_validate(int unit,
                             uint32 options,
                             bcm_flowtracker_group_t id,
                             bcm_flowtracker_check_t check_id);
/*****************************************************
 | End - Flow Checker utility Methods.               |
 */


/*****************************************************
 | Start - FT Group utility Methods.                 |
 */

/* Init Routines. */
extern int
bcmi_ft_group_state_init(int unit);

extern void
bcmi_ft_group_state_clear(int unit);

extern int
bcmi_ft_group_profile_init(int unit);

extern int
bcmi_ft_group_is_invalid(int unit, bcm_flowtracker_group_t group_index);

extern int
bcmi_ft_group_collector_is_valid(int unit, bcm_flowtracker_group_t id);

/* Group Creation/Deletion Methods. */
extern int
bcmi_ft_group_create(
        int unit,
        uint32 options,
        bcm_flowtracker_group_t *id,
        bcm_flowtracker_group_info_t *flow_group_info);

extern int
bcmi_ft_group_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_info_t *flow_group_info);

extern int
bcmi_ft_group_clear(
        int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_destroy(
        int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_profiles_destroy(
        int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_get_all(
        int unit,
        int max_size,
        bcm_flowtracker_group_t *idz,
        int *list_size);

extern int
bcmi_ft_group_entry_update(
        int unit,
        bcm_flowtracker_group_t group_id,
        bcm_flowtracker_group_info_t *flow_group_info,
        int create);

extern int
bcmi_ft_group_flow_limit_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_limit);

extern int
bcmi_ft_group_flow_limit_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flow_limit);

extern int
bcmi_ft_group_age_out_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *aging_interval_ms);

extern int
bcmi_ft_group_age_out_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 aging_interval_ms);

extern int
bcmi_ft_group_age_export_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flags,
        uint32 enable);

extern int
bcmi_ft_group_age_export_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flags,
        uint32 *enable);

extern int
bcmi_ft_group_meter_profile_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_meter_info_t info);

extern int
bcmi_ft_group_meter_profile_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_meter_info_t *info);

extern int
bcmi_ft_group_collector_copy_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_collector_copy_info_t info);

extern int
bcmi_ft_group_collector_copy_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_collector_copy_info_t *info);

/* Export Triggers. */
extern int
bcmi_ft_group_export_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info);
extern int
bcmi_ft_group_export_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info);

/* Group Tracking Params. */
extern int
bcmi_ft_group_param_element_validate (
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int num_tracking_elements,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements);

extern int bcmi_ft_group_tracking_param_set(
    int unit,
    bcm_flowtracker_group_t id,
    int num_tracking_elements,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements);

extern int
bcmi_ft_group_tracking_param_get(
        int unit,
        bcm_flowtracker_group_t id,
        int max_size,
        bcm_flowtracker_tracking_param_info_t *list_of_tracking_elements,
        int *num_tracking_elements);

extern int
bcmi_ft_group_tracking_param_mask_status_get(
        int unit,
        bcm_flowtracker_tracking_param_info_t *tp_info,
        uint16 qual_size,
        int *mask_present,
        int *partial_mask);

/* Template list Addition and delettion. */
extern int
bcmi_ft_group_template_list_add(
        int unit,
        bcmi_ft_group_template_list_t **head,
        int start,
        int length,
        bcmi_ft_group_param_type_t type,
        bcmi_ft_group_alu_info_t *info);

extern void
bcmi_ft_group_template_list_delete_all(
        int unit,
        bcmi_ft_group_template_list_t **head);

extern int
bcmi_ft_group_template_data_create(
        int unit,
        bcm_flowtracker_group_t id,
        int *load16_indexes,
        int *load8_indexes);

extern int
bcmi_ft_group_flush_set(
        int unit,
        uint32 flags,
        int num_groups,
        bcm_flowtracker_group_t *idz);

extern int
bcmi_flowtracker_group_control_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_control_type_t type,
        int *arg);

extern int
bcmi_flowtracker_group_control_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_control_type_t type,
        int arg);

extern int
bcmi_ft_group_stat_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_stat_t *group_stats);

extern int
bcmi_ft_group_stat_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_stat_t *group_stats);

/* PDD profile methods. */
extern int
bcmi_ft_group_pdd_policy_profile_add(
        int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_pdd_policy_profile_delete(
        int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_pdd_to_pde_coversion(
        int unit,
        bcm_flowtracker_group_t id,
        bsc_dt_pde_profile_entry_t *pde_entry);

extern int
bcmi_ft_group_ft_mode_update(
        int unit,
        bcm_flowtracker_group_t id);

/* Group install routines */
extern int
bcmi_ft_group_hw_install(int unit,
                         bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_hw_uninstall(int unit,
                           bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_norm_tracking_params_info_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_tracking_param_info_t **norm_tracking_params_info);

extern int
bcmi_ft_group_export_checks_validate(int unit,
        bcm_flowtracker_group_t id,
        int num_in_export_elements,
        bcm_flowtracker_export_element_info_t *in_export_elements);

extern int
bcmi_ft_group_entry_add_check(int unit,
        bcm_flowtracker_group_type_t group_type,
        bcm_flowtracker_group_t id);

extern bcm_flowtracker_tracking_param_type_t
bcmi_ft_group_get_tracking_param(int unit,
        bcmi_ft_group_param_type_t pram_type);

extern int
bcmi_ft_group_check_ts_set(int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_check_t flowchecker_id,
        bcmi_flowtracker_check_set_t reset);

extern int
bcmi_ft_group_ts_triggers_set(int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_type_t ft_type,
        uint32 flags,
        int trigger_type,
        bcmi_flowtracker_check_set_t reset);

extern bcmi_ft_group_param_type_t
bcmi_ft_group_get_ts_type(int unit,
        bcm_flowtracker_group_t id,
        int ts_bank);

extern int
bcmi_ft_group_timestamp_set(int unit,
          bcm_flowtracker_group_t id);

extern void
bcmi_ft_group_timestamp_trig_info_clear(int unit,
                      bcm_flowtracker_group_t id);

extern void
bcmi_ft_group_param_ts_trig_info_clear(int unit,
                      bcm_flowtracker_group_t id,
                      uint32 *ts_triggers);

extern void
bcmi_ft_group_param_int_ts_trig_info_clear(int unit,
                      bcm_flowtracker_group_t id);

extern void
bcmi_ft_group_param_ts_checkevent_trig_set(int unit,
                      bcm_flowtracker_group_t id);

extern int
bcmi_ft_alu_load_type_resolve(int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *alu_info,
        bcmi_ft_alu_load_type_t *type);

extern int
bcmi_ft_group_ft_mode_recompute(int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_new_learn_export_trigger_set(int unit,
        bcm_flowtracker_group_t id,
        uint32 flags,
        uint32 val);

extern int
bcmi_ft_group_new_learn_export_trigger_get(int unit,
        bcm_flowtracker_group_t id,
        uint32 *flags,
        uint32 *val);

extern int
bcmi_ft_group_timer_export_trigger_set(int unit,
        bcm_flowtracker_group_t id,
        uint32 val);

extern int
bcmi_ft_group_timer_export_trigger_get(int unit,
        bcm_flowtracker_group_t id,
        uint32 *val);

extern int bcmi_ft_group_flow_exceed_profile_set(int unit,
        bcm_flowtracker_group_t id,
        uint32 *kg_entry,
        uint32 flags,
        uint32 value);

/*****************************************************
 |           End -  FT Group utility Methods.        |
 */






/*****************************************************
 | Start -  FTFP utility Methods.                    |
 */
extern int
bcmi_ft_group_extraction_alu_info_set(
        int unit,
        bcm_flowtracker_group_t id,
        int key_or_data,
        int num_alu_info,
        bcmi_ft_group_alu_info_t *alu_info);

extern int
bcmi_ft_group_extraction_alu_info_get(
        int unit,
        bcm_flowtracker_group_t id,
        int key_or_data,
        int max_alu_info,
        bcmi_ft_group_alu_info_t *alu_info,
        int *num_alu_info);

extern int
bcmi_ft_group_extraction_hw_info_set(
        int unit,
        bcm_flowtracker_group_t id,
        int key_or_data,
        int num_elements,
        bcmi_ft_hw_extractor_info_t *elements);

extern int
bcmi_ft_group_extraction_hw_info_get(
        int unit,
        int key_or_data,
        bcm_flowtracker_group_t id,
        int max_elements,
        bcmi_ft_hw_extractor_info_t *elements,
        int *num_elements);

extern int
bcmi_ft_group_extraction_mode_set(
        int unit,
        int key_or_data,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_key_data_mode_t mode);

extern int
bcmi_ft_group_extraction_mode_get(
        int unit,
        int key_or_data,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_key_data_mode_t *mode);


extern int
bcmi_ft_group_session_key_type_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 key_type);

extern int
bcmi_ft_group_session_key_type_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *key_type);

extern int
bcmi_ft_group_ftfp_profiles_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_session_profiles_t *profile_info);

extern int
bcmi_ft_group_ftfp_profiles_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_session_profiles_t *profile_info);

extern int
bcmi_ft_group_ftfp_data_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_ftfp_data_t *data);

extern int
bcmi_ft_group_param_retrieve(int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_params_t param,
        int *arg);

extern int
bcmi_ft_group_param_update(int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_params_t param,
        bcmi_ft_group_params_operation_t opr,
        int arg);

extern int
bcmi_ft_group_param_present(int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_tracking_param_type_t tparam,
        int *index);
/*****************************************************
 | End  -  FTFP utility Methods.                     |
 */



/*****************************************************
 | Start - Warmboot Methods.                         |
 */

/* Recover PDD entry for group. */
extern int
bcmi_ft_group_pdd_pde_policy_recover(
              int unit, bcm_flowtracker_group_t id);

/* Recover group profiles. */
extern int
bcmi_ft_group_profile_recover(
              int unit, bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_alu_load_wb_recover(
              int unit, bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *info,
              int *load16_indexes,
              int *load8_indexes,
              bcmi_ft_alu_load_type_t alu_load_type);

extern int
bcmi_ft_recover(int unit);

extern int
bcmi_ft_group_sw_reinstall(int unit, bcm_flowtracker_group_t id);

extern int
bcmi_ft_sync(int unit);

extern void
bcmi_ft_wb_alloc_size(int unit, int *size);

extern int
bcmi_ft_warm_boot_alloc(int unit);

/*****************************************************
 | End - Warmboot Methods.                           |
 */


/*****************************************************
 | Start - ESW layer methods.                        |
 */
extern int
bcm_esw_flowtracker_sync(int unit);

extern int
bcm_esw_flowtracker_group_stat_set(int unit,
                bcm_flowtracker_group_t id,
                bcm_flowtracker_group_stat_t *group_stat);
/*****************************************************
 | End - ESW layer methods.                          |
 */


/*****************************************************
 | Start - Flowtracker sw dump methods               |
 */
extern void
bcmi_esw_flowtracker_sw_dump(int unit);

extern void
bcmi_ft_state_dump(int unit, int brief);

extern void
bcmi_ft_group_tracking_params_list(int unit,
        uint16 ft_type_tmp);

/*****************************************************
 | End - Flowtracker sw dump methods                 |
 */



/*****************************************************
 | Start - Flowtracker diag methods                  |
 */
extern int
bcmi_ft_table_insert(int unit,
                     bcm_ip_t dip,
                     bcm_ip_t sip,
                     uint16 l4destport,
                     uint16 l4srcport,
                     uint8 proto);

extern int
bcmi_ft_table_lookup(int unit,
                     bcm_flowtracker_group_t id,
                     bcm_ip_t dip,
                     bcm_ip_t sip,
                     uint16 l4destport,
                     uint16 l4srcport,
                     uint8 proto,
                     int *index);

extern int
bcmi_ft_group_state_dump(int unit,
                         bcm_flowtracker_group_t id,
                         int brief);

extern int
bcmi_ft_group_session_data_dump(int unit,
                                bcm_flowtracker_group_t id,
                                int index);

extern int
bcmi_ft_alu32_dump(int unit,
                   bcmi_ft_alu_load_type_t alu_load_type,
                   int global_index);

extern int
bcmi_ft_alu16_dump(int unit,
                   int global_index);

extern void
bcmi_ft_group_hw_entries_decode(int unit,
        uint32 flags, bcm_flowtracker_group_t id,
        char *table_name, int index, int count);

/*****************************************************
 | End - Flowtracker diag methods                    |
 */

/*****************************************************
 | Start - ALU/LOAD HW structs & defines             |
 */
int
bcmi_ft_group_alu16_trigger_set(
    int unit,
    int mem_idx,
    int index,
    bcm_flowtracker_direction_t direction);

int
bcmi_ft_group_alu32_trigger_set(
    int unit,
    int mem_idx,
    int index,
    bcm_flowtracker_direction_t direction);
int
bcmi_ft_group_load8_trigger_set(
    int unit,
    int mem_idx,
    int index,
    bcm_flowtracker_direction_t direction);

int
bcmi_ft_group_load16_trigger_set(
    int unit,
    int mem_idx,
    int index,
    bcm_flowtracker_direction_t direction);

/*****************************************************
 | End - ALU/LOAD HW structs & defines               |
 */

extern int
bcmi_ft_tracking_param_is_timestamp(int unit,
        bcm_flowtracker_tracking_param_type_t t_param);

extern bcmi_ft_group_param_type_t
bcmi_ft_group_get_param_type(int unit,
        bcm_flowtracker_tracking_param_type_t t_param);

extern bcmi_ft_group_param_type_t
bcmi_ft_group_get_ts_type(int unit,
        bcm_flowtracker_group_t id,
        soc_field_t fid);

extern int
bcmi_ft_group_pdd_pde_policy_add(int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_template_data_add(int unit,
        bcm_flowtracker_group_t id,
        int *load16_indexes,
        int *load8_indexes);

extern int
bcmi_ft_group_stage_pde_policy_add(int unit,
        bcm_flowtracker_group_t id,
        int index,
        uint32 **pdd_entry_arr);

extern int
bcmi_ft_group_export_pde_policy_add(int unit,
        bcm_flowtracker_group_t id,
        int index,
        uint32 **pdd_entry_arr);

extern int
bcmi_ft_group_extended_alu_info_process(int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_template_data_add(int unit,
        bcm_flowtracker_group_t id,
        int *load16_indexes,
        int *load8_indexes);

extern int
bcmi_ft_group_ts_type_src_get(int unit,
        bcm_flowtracker_group_t id,
        int ts_type,
        bcm_flowtracker_timestamp_source_t *ts_src);

extern int
bcmi_ft_group_timestamp_trigger_bit_get(int unit,
        bcm_flowtracker_group_t id,
        int ts_type,
        int *bit_sel);

extern int
bcmi_ft_group_timestamp_trig_param_get(int unit,
        bcm_flowtracker_group_t id,
        int ts_type,
        bcm_flowtracker_tracking_param_type_t *param);

extern int
bcmi_ft_group_state_transition_type_get(int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_state_transition_type_t *st_type);

extern int
bcmi_ft_control_set(int unit,
        bcm_flowtracker_control_type_t type,
        int arg);

extern int
bcmi_ft_control_get(int unit,
        bcm_flowtracker_control_type_t type,
        int *arg);

extern int
bcmi_ft_group_record_traverse(int unit,
        bcm_flowtracker_group_t group_id,
        bcm_flowtracker_group_record_type_t type,
        bcm_flowtracker_group_record_traverse_cb cb,
        void *user_data);

extern int
bcmi_ft_group_checks_bitmap_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *check_bmp_data,
        uint32 *check_bmp_mask);

extern int
bcmi_ft_group_checks_list_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 check_bmp_data,
        uint32 check_bmp_mask,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *actual_num_checks);

/*****************************************************
 | Start - Flowtracker v2 methods                    |
 */
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
extern int
bcmi_ftv2_group_ft_mode_update(int unit,
                               bcm_flowtracker_group_t id);

extern int
bcmi_ftv2_group_param_ts_src_get(int unit,
                                 bcm_flowtracker_group_t id,
                                 bcm_flowtracker_tracking_param_type_t tparam,
                                 uint8 ts_flags,
                                 bcm_flowtracker_timestamp_source_t *ts_src);

extern int
bcmi_ftv2_legacy_group_pdd_policy_profile_add(int unit,
                                              bcm_flowtracker_group_t id);

extern int
bcmi_ftv2_legacy_group_template_data_create(int unit,
                                     bcm_flowtracker_group_t id,
                                     int *load16_indexes,
                                     int *load8_indexes);

extern int
bcmi_ftv2_internal_flowchecker_get(int unit,
            bcm_flowtracker_check_t flowchecker_id,
            bcmi_flowtracker_flowchecker_info_t *flow_check_info);

extern int
bcmi_ftv2_group_internal_flowcheck_create(int unit,
            bcm_flowtracker_group_t id,
            bcm_flowtracker_tracking_param_type_t param,
            bcmi_ft_group_alu_info_t *check_alu_info,
            bcm_flowtracker_check_t *int_check_id);

extern int
bcmi_ftv2_group_template_data_create(int unit,
            bcm_flowtracker_group_t id,
            int *load16_indexes,
            int *load8_indexes);

extern int
bcmi_ftv2_group_pdd_policy_profile_add(int unit,
                    bcm_flowtracker_group_t id);

extern int
bcmi_ftv2_group_pdd_to_pde_conversion(int unit,
                    bcm_flowtracker_group_t id,
                    bcmi_ft_type_t ft_type,
                    uint32 flags,
                    uint32 *pdd_entry,
                    uint32 *pde_entry);

extern int
bcmi_ftv2_data_engine_db_create(int unit,
                    bcm_flowtracker_group_t id,
                    int *load16_indexes,
                    int *load8_indexes);

extern int
bcmi_ftv2_group_internal_alu_info_add(int unit,
                    bcm_flowtracker_group_t id);

#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
/*****************************************************
 | End - Flowtracker v2 methods                      |
 */


/*****************************************************
 | Start - Flowtracker v3 methods                    |
 */
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
extern int
bcmi_ftv3_group_aggregate_entry_update(int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_info_t *info,
        int create);

extern int
bcmi_ftv3_group_aggregate_template_data_create(int unit,
        bcm_flowtracker_group_t id);

extern int
bcmi_ftv3_alu_load_info_flags_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *alu_info);

extern int
bcmi_ftv3_user_entry_entry_add_check(int unit,
        bcm_flowtracker_group_type_t group_type,
        bcm_flowtracker_group_t id,
        int flow_index);

extern int
bcmi_ftv3_flow_transition_alu_trigger_set(int unit,
        uint32 event_flag,
        int index,
        int set);

extern int
bcmi_ftv3_group_flow_transition_state_set(int unit,
        bcm_flowtracker_group_t id,
        uint32 enable);

extern int
bcmi_ftv3_aft_group_periodic_export_en(int unit,
        bcm_flowtracker_group_t group_id,
        uint32 enable);

extern int
bcmi_ftv3_group_user_entry_hw_update(int unit,
        bcm_flowtracker_group_t group_id,
        int count);

extern int
bcmi_ftv3_agg_group_record_traverse(int unit,
        bcm_flowtracker_group_t group_id,
        bcm_flowtracker_group_record_type_t type,
        bcm_flowtracker_group_record_traverse_cb cb,
        void *user_data);

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */
/*****************************************************
 | End - Flowtracker v3 methods                      |
 */

#endif /* BCM_FLOWTRACKER_SUPPORT */
#endif /* _BCM_INT_FT_GROUP_H_ */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
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
#include <bcm_int/esw/flowtracker/ft_export.h>



#define BCMI_CLEANUP_IF_ERROR(rv)      \
            do {                       \
                if (BCM_FAILURE(rv)) { \
                    goto cleanup;      \
                }                      \
            } while(0);

#define BCMI_CLEANUP1_IF_ERROR(rv)     \
            do {                       \
                if (BCM_FAILURE(rv)) { \
                    goto cleanup1;     \
                }                      \
            } while(0);


/*|*****************************************************|
  |*****************************************************|
  |       TCP Flags - Extraction control                |
  |                                                     |
  | This information is present in FTFP                 |
  |*****************************************************|
  |*****************************************************|*/

/*
 * Typedef:
 *      _field_ft_tracking_alu_data_info_t
 *
 * Purpose:
 *       ALU Data related tracking information.
 */
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


/* Internal group tracking param type. */
typedef enum bcmi_ft_group_param_type_e {
    bcmiFtGroupParamTypeTracking=0,
    bcmiFtGroupParamTypeFlowchecker=1,
    bcmiFtGroupParamTypeCollectorCopy=2,
    bcmiFtGroupParamTypeMeter=3,
    bcmiFtGroupParamTypeTsNewLearn=4,
    bcmiFtGroupParamTypeTsFlowStart=5,
    bcmiFtGroupParamTypeTsFlowEnd=6,
    bcmiFtGroupParamTypeTsCheckEvent1=7,
    bcmiFtGroupParamTypeTsCheckEvent2=8
} bcmi_ft_group_param_type_t;


typedef struct bcmi_ft_alu_data_info_s {
    uint16 param;

    int offset;

    int width;

} bcmi_ft_alu_data_info_t;

typedef struct bcmi_ft_state_transtion_s {
    int tcp_flags[2]; /* For now only 2. SYN+ACK*/
    int current_state;
    int next_state;
    int new_flow;
    int bidirectional;
    int ts_trigger;
} bcmi_ft_state_transtion_t;

/* Timestamp related base structure. */
typedef struct bcmi_ft_group_ts_s {
    bcm_flowtracker_tracking_param_type_t ts_param;
    uint32 flag_id;
    int ts_trigger_bit_sel;
    soc_field_t ts_field;
} bcmi_ft_group_ts_t;

/* Max 4 timestamps allowed in one group. */
#define BCMI_FT_GROUP_MAX_TS           4
#define BCMI_FT_GROUP_MAX_TS_TYPE      5

/* Internal Group Flags to manage run time timestamps. */
#define BCMI_FT_GROUP_TS_NEW_LEARN                 (1<<0)
#define BCMI_FT_GROUP_TS_FLOW_START                (1<<1)
#define BCMI_FT_GROUP_TS_FLOW_END                  (1<<2)
#define BCMI_FT_GROUP_TS_CHECK_EVENT1              (1<<3)
#define BCMI_FT_GROUP_TS_CHECK_EVENT2              (1<<4)

extern bcmi_ft_group_ts_t bcmi_ft_group_ts_info[BCMI_FT_GROUP_MAX_TS_TYPE];

/* Tracking param trigger type.*/
#define BCMI_FT_TS_TYPE_TP                         (1<<0)

/* Flow check trigger type. */
#define BCMI_FT_TS_TYPE_FC                         (1<<1)

extern int
bcmi_ft_group_ts_triggers_set(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 check_flags,
        int trigger_type,
        bcmi_flowtracker_check_set_t reset);

extern int
bcmi_ft_group_check_ts_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_check_t flowchecker_id,
        bcmi_flowtracker_check_set_t reset);

extern bcmi_ft_group_param_type_t
bcmi_ft_group_get_ts_type(
        int unit,
        bcm_flowtracker_group_t id,
        soc_field_t fid);

extern bcm_flowtracker_tracking_param_type_t
bcmi_ft_group_get_tracking_param(
        int unit,
        bcmi_ft_group_param_type_t pram_type);

extern int
bcmi_ft_group_timestamp_set(
        int unit,
        bcm_flowtracker_group_t id);


/* Groups internal Declarations. */
typedef struct bcmi_ft_group_config_s {
    /* User tracking params. */
    int num_tracking_params;
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params;

    /* Ordered tracking params. */
    int num_ordered_template_params;
    bcm_flowtracker_tracking_param_info_t *list_of_ordered_template_params;

    /* Group Class. */
    uint32 class_id;

} bcmi_ft_group_config_t;


/* Hardware extractor info. used by FTFP. extrators for group key and data. */
#if 0
typedef struct bcmi_ft_hw_extractor_info_s {
    int part;
    int level;
    int gran;
    int extraction_num;
} bcmi_ft_hw_extractor_info_t;
#endif

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
    bcm_flowtracker_direction_t direction;
    int flowtrack;
    int new_learn;
    uint32 session_key_type;
    /* Session key mode. */
    bcmi_ft_group_key_data_mode_t session_key_mode;
    /* FTFP profile information. */
    bcmi_ft_session_profiles_t profiles;

} bcmi_ft_group_ftfp_data_t;


typedef struct bcmi_ft_group_extraction_info_s {
    int num_data_info;
    /* Inforamtion needed for ALU allocation scheme. */
    bcmi_ft_group_alu_info_t *ft_data_info;

    /* session Key information for the FT. */
    int num_key_info;
    bcmi_ft_group_alu_info_t *ft_key_info;

    /* Hardware extractor inforamtion needed for FT. */
    bcmi_ft_group_extractor_info_t ft_ext_info;

} bcmi_ft_group_extraction_info_t;


/* template info linked list. */
typedef struct bcmi_ft_group_template_info_s {

    /* alu/load flags. */
    uint32 flags;

    /* Container offset from start of Session Data PDD */
    int cont_offset;

    /* Data shift from start of Container */
    int data_shift;

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

} bcmi_ft_group_template_info_t;

/* The linked list used to form export template */
typedef struct bcmi_ft_group_template_list_s {
    bcmi_ft_group_template_info_t info;
    struct bcmi_ft_group_template_list_s *next;
}bcmi_ft_group_template_list_t;


#if 0
typedef struct bcmi_ft_flowchecker_list_s {
    bcm_flowtracker_check_t flowchecker_id;
    struct bcmi_ft_flowchecker_list_s *next;
} bcmi_ft_flowchecker_list_t;
#endif

typedef struct bcmi_ft_group_meter_s {
    uint32 sw_ckbits_sec;
    uint32 sw_ckbits_burst;
} bcmi_ft_group_meter_t;

typedef struct bcmi_ft_group_info_s {
    bcmi_ft_group_config_t group_config;
    bcmi_ft_group_meter_t meter_info;
    bcmi_ft_flowchecker_list_t *head;
    bcm_flowtracker_collector_t  collector_id;
    bcm_flowtracker_export_template_t template_id;
    bcmi_ft_group_extraction_info_t group_ext_info;

    /* flags */
    uint32 flags;

    /* Total number of ftfp entries pointing to this group. */
    uint32 num_ftfp_entries;

    /* FTFP data in group state. */
    bcmi_ft_group_ftfp_data_t ftfp_data;

    /* We will have a pdd profile entry here. */
    bsd_policy_action_profile_entry_t pdd_entry;

    /* ALU-32 MEM allocation bitmap. */
    uint32 ALU32_MEM_USE[TOTAL_GROUP_ALU32_MEM];

    /* ALU-16 MEM allocation bitmap. */
    uint32 ALU16_MEM_USE[TOTAL_GROUP_ALU16_MEM];

    /* List for template information. */
    bcmi_ft_group_template_list_t *template_head;

    /* Timestamps triggers on this group.*/
    uint32 ts_triggers;

    /* Validation status. */
    int validated;
} bcmi_ft_group_sw_info_t;



/* Extractor information. */
typedef struct bcmi_ft_group_element_extraction_info_s {
    int location;
    uint32 length;
    int is_alu_type;
    bcm_flowtracker_check_t flow_checker_id;
    bcm_flowtracker_tracking_param_type_t inforamation_element_id;
    int location_of_update_element;
    bcm_flowtracker_check_operation_t update_operation;
} bcmi_ft_group_element_extraction_info_t;



typedef struct bcmi_ft_group_bitmap_s {
    SHR_BITDCL *ft_group_bitmap;
} bcmi_ft_group_bitmap_t;

extern bcmi_ft_group_sw_info_t **bcmi_ft_group_sw_state[BCM_MAX_NUM_UNITS];


#define BCMI_FT_GROUP_BITMAP(unit)    (bcmi_ft_group_bitmap[unit].ft_group_bitmap)
#define BCMI_FT_GROUP(unit, id)       (bcmi_ft_group_sw_state[unit][id])

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



int
bcmi_ft_group_is_invalid(int unit, bcm_flowtracker_group_t group_index);

int
bcmi_ft_group_collector_is_valid(int unit, bcm_flowtracker_group_t id);

int
bcmi_ft_flowchecker_is_invalid(int unit, int flowchecker_id);

int
bcmi_ft_group_entry_add_check(int unit, bcm_flowtracker_group_t id);

int
bcmi_ft_group_export_checks_validate(int unit,
    bcm_flowtracker_group_t id,
    int num_in_export_elements,
    bcm_flowtracker_export_element_info_t *in_export_elements);

/* Options for bcmi_ft_group_check_validate */
#define BCMI_FT_GROUP_CHECK_MATCH_GROUP      (1 << 0)
#define BCMI_FT_GROUP_CHECK_MATCH_PRIMARY    (1 << 1)
int
bcmi_ft_group_check_validate(int unit,
    uint32 options,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_check_t check_id);

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

/* Init Routines. */
extern int
bcmi_ft_group_state_init(int unit);

extern void
bcmi_ft_group_state_clear(int unit);

extern int
bcmi_ft_group_profile_init(int unit);

extern int
bcmi_ft_flowchecker_init(int unit);

extern void
bcmi_ft_flowchecker_cleanup(int unit);

extern void
bcmi_ft_group_state_clear(int unit);


extern int
bcmi_ft_init(int unit);

extern int
bcmi_ft_cleanup(int unit);

/*|*****************************************************|
  |*****************************************************|
  |             Start -  FTFP utility Methods.          |
  |*****************************************************|
  |*****************************************************|*/
int
bcmi_ft_group_extraction_alu_info_set(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int num_alu_info,
              bcmi_ft_group_alu_info_t *alu_info);


int
bcmi_ft_group_extraction_alu_info_get(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int max_alu_info,
              bcmi_ft_group_alu_info_t *alu_info,
              int *num_alu_info);


int
bcmi_ft_group_extraction_hw_info_set(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int num_elements,
              bcmi_ft_hw_extractor_info_t *elements);


int
bcmi_ft_group_extraction_hw_info_get(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              int max_elements,
              bcmi_ft_hw_extractor_info_t *elements,
              int *num_elements);

int
bcmi_ft_group_extraction_mode_set(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_key_data_mode_t mode);

int
bcmi_ft_group_extraction_mode_get(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_key_data_mode_t *mode);


int
bcmi_ft_group_session_key_type_set(
              int unit,
              bcm_flowtracker_group_t id,
              uint32 key_type);

int
bcmi_ft_group_session_key_type_get(
              int unit,
              bcm_flowtracker_group_t id,
              uint32 *key_type);


int
bcmi_ft_group_ftfp_profiles_get(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_session_profiles_t *profile_info);

int
bcmi_ft_group_ftfp_profiles_set(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_session_profiles_t *profile_info);

int
bcmi_ft_group_ftfp_data_get(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_ftfp_data_t *data);

int
bcmi_ft_group_param_retrieve(int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_params_t param,
              int *arg);

int
bcmi_ft_group_param_update(int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_params_t param,
              bcmi_ft_group_params_operation_t opr,
              int arg);

/*|*****************************************************|
  |*****************************************************|
  |            End  -  FTFP utility Methods.            |
  |*****************************************************|
  |*****************************************************|*/



/* Flow check utilities. */


/* Flowtracker Index generation. */
#define  BCM_FT_IDX_TYPE_CHECK               1
#define  BCM_FT_IDX_TYPE_2ND_CHECK           2
#define  BCM_FT_IDX_TYPE_SHIFT               26
#define  BCM_FT_IDX_MASK                     0x3ffffff


#define BCMI_FT_FLOWCHECKER_INDEX_SET(index, type) \
            ((type << BCM_FT_IDX_TYPE_SHIFT) | \
                (index))

#define BCMI_FT_FLOWCHECKER_INDEX_GET(index) \
            ((index) & BCM_FT_IDX_MASK)

#define BCMI_FT_IDX_IS_CHECK(index) \
            ((((index) >> BCM_FT_IDX_TYPE_SHIFT) == BCM_FT_IDX_TYPE_CHECK) || \
             (((index) >> BCM_FT_IDX_TYPE_SHIFT) == BCM_FT_IDX_TYPE_2ND_CHECK))

#define BCMI_FT_IDX_IS_PRIMARY_CHECK(index) \
             (((index) >> BCM_FT_IDX_TYPE_SHIFT) == BCM_FT_IDX_TYPE_CHECK)

#define BCMI_FT_IDX_IS_2ND_CHECK(index) \
             (((index) >> BCM_FT_IDX_TYPE_SHIFT) == BCM_FT_IDX_TYPE_2ND_CHECK)


/* Create Flow check and return software ID. */
int
bcmi_ft_check_create(
    int unit,
    uint32 options,
    bcm_flowtracker_check_info_t check_info,
    bcm_flowtracker_check_t *check_id);

/* Destroy flow check corresponding to this software ID. */
int
bcmi_ft_check_destroy(
    int unit,
    bcm_flowtracker_check_t check_id);

/* Destroy all flow checks not linked to any group. */
int
bcmi_ft_check_destroy_all(int unit);

/* Get information of flow check. */
int
bcmi_ft_check_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_info_t *check_info);

/* Routine to traverse through all the flowcheckers. */
int
bcmi_ft_check_traverse(
            int unit,
            bcm_flowtracker_check_traverse_cb cb,
            void *user_data);


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
        uint32 enable);

extern int
bcmi_ft_group_age_export_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *enable);

int
bcmi_ft_group_profiles_destroy(
        int unit,
         bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_hw_install(int unit, bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_hw_uninstall(int unit, bcm_flowtracker_group_t id);


extern int
bcmi_ft_flowchecker_get(
            int unit,
            bcm_flowtracker_check_t flowchecker_id,
            bcmi_flowtracker_flowchecker_info_t *flow_check_info);

/* PDD profile methods. */
int
bcmi_ft_group_pdd_policy_profile_add(
        int unit,
        bcm_flowtracker_group_t id);

int
bcmi_ft_group_pdd_policy_profile_delete(
        int unit,
        bcm_flowtracker_group_t id);

int
bcmi_ft_group_pdd_to_pde_coversion(
        int unit,
        bcm_flowtracker_group_t id,
        bsc_dt_pde_profile_entry_t *pde_entry);

int
bcmi_ft_group_ft_mode_update(
    int unit,
    bcm_flowtracker_group_t id);


int
bcmi_ft_group_meter_profile_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_meter_info_t info);

int
bcmi_ft_group_meter_profile_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_meter_info_t *info);


int
bcmi_ft_group_collector_copy_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_collector_copy_info_t info);

int
bcmi_ft_group_collector_copy_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_collector_copy_info_t *info);

/* Export Triggers. */
int
bcmi_ft_group_export_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info);
int
bcmi_ft_group_export_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info);

/* CPU notifications. */
int
bcmi_ft_group_cpu_notification_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_cpu_notification_info_t *notifications);


int
bcmi_ft_group_cpu_notification_trigger_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_cpu_notification_info_t *notifications);

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
bcmi_ft_check_action_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_action_info_t info);

extern int
bcmi_ft_check_action_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_action_info_t *info);


extern int
bcmi_ft_check_export_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_export_info_t *info);

extern int
bcmi_ft_check_export_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_export_info_t info);

extern int
bcmi_ft_group_flowchecker_add(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_check_t flowchecker_id);


extern int
bcmi_ft_group_flowchecker_delete(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_check_t flowchecker_id);

extern int
bcmi_flowtracker_check_get_all(
    int unit,
    bcm_flowtracker_group_t group_id,
    int skip_non_primary,
    int max_checks,
    bcm_flowtracker_check_t *list_of_check_ids,
    int *num_checks);

/* Template list Addition and delettion. */
extern int
bcmi_ft_group_template_list_add(
        int unit,
        bcmi_ft_group_template_list_t **head,
        int start,
        int length,
        bcmi_ft_group_param_type_t type,
        bcmi_ft_group_alu_info_t *info);


void
bcmi_ft_group_template_list_delete_all(
        int unit,
        bcmi_ft_group_template_list_t **head);

int
bcmi_ft_group_template_data_create(
        int unit,
        bcm_flowtracker_group_t id,
        int *load16_indexes,
        int *load8_indexes);


int
bcmi_ft_group_flush_set(
        int unit,
        uint32 flags,
        int num_groups,
        bcm_flowtracker_group_t *idz);

int
bcmi_flowtracker_group_control_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_control_type_t type,
        int *arg);


int
bcmi_flowtracker_group_control_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_control_type_t type,
        int arg);


int
bcmi_ft_group_stat_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_stat_t *group_stats);


int
bcmi_ft_group_stat_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_stat_t *group_stats);

/* Number of tracking params for normalization */
#define BCMI_FT_GROUP_NORM_TRACKING_PARAMS   12

int
bcmi_ft_group_norm_tracking_params_info_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_tracking_param_info_t **norm_tracking_params_info);

/*|*****************************************************|
  |*****************************************************|
  |           Start  -  Warmboot Methods.               |
  |*****************************************************|
  |*****************************************************|*/

/* Recover PDD entry for group. */
int
bcmi_ft_pdd_policy_profile_recover(
              int unit, bcm_flowtracker_group_t id);

/* Recover group profiles. */
int
bcmi_ft_group_profile_recover(
              int unit, bcm_flowtracker_group_t id);

int
bcmi_ft_group_alu_load_wb_recover(
              int unit, bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *info,
              int *load16_indexes,
              int *load8_indexes,
              bcmi_ft_alu_load_type_t alu_load_type);

int
bcmi_ft_recover(int unit);


int
bcmi_ft_group_sw_reinstall(int unit, bcm_flowtracker_group_t id);

int
bcmi_ft_sync(int unit);


void
bcmi_ft_wb_alloc_size(int unit, int *size);

int
bcmi_ft_warn_boot_alloc(int unit);


int bcmi_ft_warm_boot_alloc(int unit);

/* ESW Layer menthods. */
extern int
bcm_esw_flowtracker_sync(
    int unit);


extern int
bcmi_esw_ft_group_hw_install(
    int unit,
    bcm_flowtracker_group_t id);

extern int
bcmi_esw_ft_group_hw_uninstall(
    int unit,
    bcm_flowtracker_group_t id);

extern int
bcmi_esw_ft_group_extraction_mode_set(
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


int
bcmi_esw_ft_group_extraction_hw_info_set(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int num_elements,
              bcmi_ft_hw_extractor_info_t *elements);

extern int
bcmi_esw_ft_group_extraction_hw_info_get(
              int unit,
              int key_or_data,
              bcm_flowtracker_group_t id,
              int max_elements,
              bcmi_ft_hw_extractor_info_t *elements,
              int *num_elements);


extern int
bcmi_esw_ft_group_extraction_alu_info_set(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int num_alu_info,
              bcmi_ft_group_alu_info_t *alu_info);

extern int
bcmi_esw_ft_group_extraction_alu_info_get(
              int unit,
              bcm_flowtracker_group_t id,
              int key_or_data,
              int max_alu_info,
              bcmi_ft_group_alu_info_t *alu_info,
              int *num_alu_info);

extern int
bcmi_esw_ft_group_param_retrieve(int unit,
                        bcm_flowtracker_group_t id,
                        bcmi_ft_group_params_t param,
                        int *arg);

extern int
bcmi_esw_ft_group_param_update(int unit,
                           bcm_flowtracker_group_t id,
                           bcmi_ft_group_params_t param,
                           bcmi_ft_group_params_operation_t opr,
                           int arg);

extern int
bcm_esw_flowtracker_group_stat_set(int unit,
                bcm_flowtracker_group_t id,
                bcm_flowtracker_group_stat_t *group_stat);

/*|*****************************************************|
  |*****************************************************|
  |             END  -  Warmboot Methods.               |
  |*****************************************************|
  |*****************************************************|*/


/* Utility Macros. */
#define CLEANUP_ON_ERROR(rv) \
    do { \
	    if (BCM_FAILURE(rv)) { \
		    goto cleanup; \
	    } \
	} while(0); \

/* Dump Routine. */
extern void bcmi_esw_flowtracker_sw_dump(int unit);

extern void
bcmi_ft_state_dump(int unit);



/* Diag functions. */
extern int
bcmi_ft_table_insert(
    int unit,
    bcm_ip_t dip,
    bcm_ip_t sip,
    uint16 l4destport,
    uint16 l4srcport,
    uint8 proto);


extern int
bcmi_ft_table_lookup(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_ip_t dip,
    bcm_ip_t sip,
    uint16 l4destport,
    uint16 l4srcport,
    uint8 proto,
    int *index);


extern int
bcmi_ft_group_element_print(
    int unit,
    bcm_flowtracker_tracking_param_type_t element);

extern int
bcmi_ft_group_state_dump(
    int unit,
    bcm_flowtracker_group_t id);

extern int
bcmi_ft_group_session_data_dump(
    int unit,
    bcm_flowtracker_group_t id,
    int index);

extern int
bcmi_ft_alu32_dump(
    int unit,
    int global_index);

extern int
bcmi_ft_alu16_dump(
    int unit,
    int global_index);

/* Load trigger set methods. */
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


#endif /* BCM_FLOWTRACKER_SUPPORT */

#endif /* _BCM_INT_FT_GROUP_H_ */



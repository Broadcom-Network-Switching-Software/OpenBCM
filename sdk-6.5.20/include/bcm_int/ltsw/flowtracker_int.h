/*! \file flowtracker_int.h
 *
 * Flowtracker module internal header file.
 * This file contains Flowtracker definitions internal
 * to the flowtracker module itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_FLOWTRACKER_INT_H
#define BCMINT_LTSW_FLOWTRACKER_INT_H

#include <sal/sal_types.h>
#include <bcm/types.h>
#include <bcm/flowtracker.h>
#include <bcmltd/chip/bcmltd_mon_constants.h>
#include <bcm_int/ltsw/collector.h>

/* Get the FT_INFO struct for the unit */
#define FT_INFO_GET(_u) ft_glb_info[(_u)]

/* Max number of EM groups supported */
#define FT_MAX_EM_GROUPS    4

/* Max number of flow groups supported */
#define FT_MAX_FLOW_GROUPS  256

/* Max number of action parameters for a flow group action */
#define FT_MAX_GROUP_ACTION_PARAMS 2

/* Max number of collectors */
#define FT_MAX_COLLECTORS 1

/* Max number of export profiles */
#define FT_MAX_EXPORT_PROFILES 1

/* Max number of export templates */
#define FT_MAX_EXPORT_TEMPLATES 4

/* Max number of elephant profiles */
#define FT_MAX_ELEPHANT_PROFILES 4

/* Default value of export interval in elephant mode. 50 ms */
#define FT_ELPH_DEF_EXPORT_INTERVAL_MSECS (50)

/* Default value of export interval. 100 ms */
#define FT_DEF_EXPORT_INTERVAL_MSECS (100)

/* Default value for collector id */
#define FT_COLLECTOR_INVALID_COLLECTOR_ID 0xffffffff

/* Elephant rate thresholds must be configured in steps of 80 kbits/sec */
#define FT_ELPH_RATE_THRESHOLD_STEP 80

/* Miscellaneous Flowtracker Elephant defines. */
#define FT_ELEPHANT_MAX_PROMOTION_FILTERS 2

/* Maximum number of elephant profiles */
#define FT_MAX_ELEPHANT_PROFILES 4

/* Aging interval must be in steps of 1 sec */
#define FT_AGING_INTERVAL_STEP_MSECS (1000)

/* Aging interval can be up to 255 times scan interval */
#define FT_AGING_INTERVAL_SCAN_MULTIPLE (255)

/* Valid range of Set_ID is 256 to 65535.
 * Not checking 65535 condition since its satisfied
 * by the fact that set_id is uint16
 */
#define FT_TEMPLATE_SET_ID_VALID(set_id) (set_id >=256)

/* Max value of scan interval. 500 ms */
#define FT_MAX_SCAN_INTERVAL_USECS (500 * 1000)

/* Internal data structures */

typedef enum _ft_ha_sub_comp_id_e {
    FT_SUB_COMP_ID_GLOBAL                = 0,
    FT_SUB_COMP_ID_FLOW_GROUP_POOL       = 1,
    FT_SUB_COMP_ID_COLLECTOR_POOL        = 2,
    FT_SUB_COMP_ID_EXPORT_TEMPLATE_POOL  = 3,
    FT_SUB_COMP_ID_EXPORT_PROFILE_POOL   = 4,
    FT_SUB_COMP_ID_ELEPHANT_PROFILE_POOL = 5
}ft_ha_sub_comp_id_t;

#define FT_HA_SUB_COMP_ID_STRINGS {     \
    "bcmFtGlobal",                      \
    "bcmFtFlowGroup",                   \
    "bcmFtColloctor",                   \
    "bcmFtExportTemplate",              \
    "bcmFtExportProfile",               \
    "bcmFtElephantProfile"              \
}

typedef enum _ft_hitbit_type_e {
    FT_HITBIT_TYPE_1x = 1,
    FT_HITBIT_TYPE_2x = 2,
    FT_HITBIT_TYPE_4x = 3,
    FT_HITBIT_TYPE_MAX
}ft_hitbit_type_t;

typedef enum ft_hw_learn_flow_cmd_type_e {
    FT_HW_LEARN_FLOW_CMD_DELETE = 1,
    FT_HW_LEARN_FLOW_CMD_MODIFY = 2
}ft_hw_learn_flow_cmd_type_t;

typedef enum ft_hw_learn_type_e {
    FT_HW_LEARN_DISABLE       = 0,
    FT_HW_LEARN_ENABLE        = 1,
    FT_HW_LEARN_ENABLE_WO_EAPP = 2
}ft_hw_learn_type_t;

#define FT_GRP_ACTION_GPORT_MPLS                 (1<<0)
#define FT_GRP_ACTION_GPORT_WLAN                 (1<<1)
#define FT_GRP_ACTION_GPORT_VXLAN                (1<<2)
#define FT_GRP_ACTION_GPORT_VLAN                 (1<<3)
#define FT_GRP_ACTION_GPORT_NIV                  (1<<4)
#define FT_GRP_ACTION_GPORT_MIM                  (1<<5)
#define FT_GRP_ACTION_GPORT_FLOW                 (1<<6)
#define FT_GRP_ACTION_GPORT_MODPORT              (1<<7)
#define FT_GRP_ACTION_EGR_OBJ_OVERLAY            (1<<8)
#define FT_GRP_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY (1<<9)
#define FT_GRP_ACTION_L2_MCAST                   (1<<10)
#define FT_GRP_ACTION_L3_MCAST                   (1<<11)

/* Internal data structure to store
 * global flowtracker information.
 */
typedef struct _bcm_int_flowtracker_info_s {
    /* Flowtracker enable/disable */
    uint32_t enable;

    /* Is flowtracker initialized ? */
    bool init;

    /* Is flowtracker app using host mem ? */
    bool host_mem;

    /* Is elephant mode enabled */
    bool elph_mode;

    /* Num pipes in the devices */
    int num_pipes;

    /* IPFIX observation domain Id */
    bcm_flowtracker_ipfix_observation_domain_t observation_domain_id;

    /* Max flows */
    uint32_t max_flows[BCM_PIPES_MAX];

    /* Total max flows */
    uint32_t max_flows_total;

    /* Scan interval in microseconds */
    uint32_t scan_interval_usecs;

    /* Is flow start timestamp feature enable */
    bool flow_start_ts;

    /* Max number of flow groups. */
    uint32_t max_flow_groups;

    /* The maximum MTU size that is possible for export packets*/
    uint16_t max_export_pkt_length;

    /* Flex counter id */
    uint32_t flex_counter_id;

    /* 2x hitbit flex counter id */
    uint32_t db_wide_flex_counter_id;

    /* 4x hitbit flex counter id */
    uint32_t qd_wide_flex_counter_id;

    /* 2x hitbit flex counter group action hw idx */
    uint32_t db_wide_flex_counter_grp_act_hw_idx;

    /* 4x hitbit flex counter group action hw idx */
    uint32_t qd_wide_flex_counter_grp_act_hw_idx;

    /* Flex counter stat id */
    uint32_t flex_ctr_stat_id;

    /* 2x Hit bit flex counter stat id */
    uint32_t db_wide_hitbit_flex_ctr_stat_id;

    /* 4x Hit bit flex counter stat id */
    uint32_t qd_wide_hitbit_flex_ctr_stat_id;

    /* 2x Hit bit flex counter grp action stat id */
    uint32_t db_wide_hitbit_flex_ctr_grp_act_stat_id;

    /* 4x Hit bit flex counter grp action stat id */
    uint32_t qd_wide_hitbit_flex_ctr_grp_act_stat_id;

    /* HW learning enabled */
    uint8_t  hw_learn_en;

    /* UFT mode */
    uint8_t  uft_mode;

    /* Group flags */
    uint32_t grp_action_flags[FT_MAX_FLOW_GROUPS];
} bcm_int_ft_info_t;

/* Group actions info */
typedef struct _bcm_int_ft_group_action_info_s {
    /* Action */
    bcm_flowtracker_group_action_t action;

    /* Params */
    uint32 params[FT_MAX_GROUP_ACTION_PARAMS];
} bcm_int_ft_group_action_info_t;

/*
 * Internal data structure to store per flow_group information
 */
typedef struct _bcm_int_ft_flow_group_info_s {
    /* Software ID of the flow_group */
    bcm_flowtracker_group_t flow_group_id;
    /* Aging interval in msecs */
    uint32 aging_interval_msecs;
    /* Max flows that can be learned on this group */
    uint32  flow_limit;
    /* Collector associated with the flow group */
    int collector_id;
    /* Collector associated with the flow group */
    int export_profile_id;
    /* Template associated with the flow group */
    uint16 template_id;

    /* Elephant profle Id */
    bcm_flowtracker_elephant_profile_t elph_profile_id;

    /* QoS profile Id */
    uint32 qos_profile_id;

    /* Number of actions on the group */
    uint16 num_actions;

    bcm_int_ft_group_action_info_t action_list[FLOWTRACKER_ACTIONS_MAX];

    /*
     * Number of export triggers which will result in
     * the export of the packets
     */
    uint32    num_export_triggers;

    /*
     * List of export triggers which will result in
     * the export of the packets
     */
    bcm_flowtracker_export_trigger_t
            export_triggers[FLOWTRACKER_EXPORT_TRIGGERS_MAX];

    /* Field group corresponding to the flow group. */
    bcm_field_group_t field_group[BCM_PIPES_MAX];

    /* Number of Tracking Params on the group */
    int num_tracking_params;

    /* List of tracking params */
    bcm_flowtracker_tracking_param_info_t
                tracking_params[FLOWTRACKER_TRACKING_PARAMETERS_MAX];

    /* Flow count */
    uint32_t flow_count;

    /* Enable/disable the group */
    bool enable;
} bcm_int_ft_flow_group_info_t;

typedef struct bcm_int_ft_template_transmit_config_s {
    uint32_t export_template_id;

    bcm_ltsw_collector_transport_type_t collector_type;

    uint32_t collector_ipv4_id;

    uint32_t collector_ipv6_id;

    uint32_t transmit_interval;

    uint32_t initial_burst;

} bcm_int_ft_template_transmit_config_t;

typedef struct  bcm_int_ft_group_col_map_s {

    uint32_t group_col_map_id;

    uint32_t ft_group_id;

    bcm_ltsw_collector_transport_type_t collector_type;

    uint32_t collector_ipv4_id;


    uint32_t collector_ipv6_id;

    uint32_t export_template_id;

    uint32_t export_profile_id;
} bcm_int_ft_group_col_map_t;

/* List of supported Info elements that can be exported by app */
typedef enum ft_template_info_elem_e {
    FT_TEMPLATE_INFO_ELEM_SRC_IPV4           = 0,
    FT_TEMPLATE_INFO_ELEM_DST_IPV4           = 1,
    FT_TEMPLATE_INFO_ELEM_L4_SRC_PORT        = 2,
    FT_TEMPLATE_INFO_ELEM_L4_DST_PORT        = 3,
    FT_TEMPLATE_INFO_ELEM_IP_PROTOCOL        = 4,
    FT_TEMPLATE_INFO_ELEM_PKT_TOTAL_COUNT    = 5,
    FT_TEMPLATE_INFO_ELEM_BYTE_TOTAL_COUNT   = 6,
    FT_TEMPLATE_INFO_ELEM_FLOW_START_TS_MSEC = 7,
    FT_TEMPLATE_INFO_ELEM_OBS_TS_MSEC        = 8,
    FT_TEMPLATE_INFO_ELEM_GROUP_ID           = 9,
    FT_TEMPLATE_INFO_ELEM_VNID               = 10,
    FT_TEMPLATE_INFO_ELEM_CUSTOM             = 11,
    FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV4     = 12,
    FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV4     = 13,
    FT_TEMPLATE_INFO_ELEM_INNER_L4_SRC_PORT  = 14,
    FT_TEMPLATE_INFO_ELEM_INNER_L4_DST_PORT  = 15,
    FT_TEMPLATE_INFO_ELEM_INNER_IP_PROTOCOL  = 16,
    FT_TEMPLATE_INFO_ELEM_FLOW_VIEW_ID       = 17,
    FT_TEMPLATE_INFO_ELEM_FLOW_VIEW_START_TS = 18,
    FT_TEMPLATE_INFO_ELEM_SAMPLE_COUNT       = 19,
    FT_TEMPLATE_INFO_ELEM_FLOW_COUNT         = 20,
    FT_TEMPLATE_INFO_ELEM_DROP_PKT_COUNT     = 21,
    FT_TEMPLATE_INFO_ELEM_DROP_BYTE_COUNT    = 22,
    FT_TEMPLATE_INFO_ELEM_EGR_PORT           = 23,
    FT_TEMPLATE_INFO_ELEM_QUEUE_ID           = 24,
    FT_TEMPLATE_INFO_ELEM_QUEUE_BUF_USAGE    = 25,
    FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV6     = 26,
    FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV6     = 27,
    FT_TEMPLATE_MAX_INFO_ELEMENTS
} ft_template_info_elem_t;

/* Map export elements to internal names */
typedef struct bcm_ltsw_ft_template_support_mapping_s {
    bcm_flowtracker_export_element_type_t element;
    ft_template_info_elem_t           int_element;
    uint16_t                                data_size;
} bcm_ltsw_ft_template_support_mapping_t;

typedef struct bcm_int_ft_export_element_s {

    ft_template_info_elem_t element;

    uint16 data_size;

    uint16 id;

    uint8 enterprise;
} bcm_int_ft_export_element_t;

typedef struct bcm_int_ft_export_template_info_s {
    bcm_flowtracker_export_template_t template_id;

    uint16_t set_id;

    int interval_secs;

    int initial_burst;

    int num_export_elements;

    bcm_int_ft_export_element_t elements[FT_TEMPLATE_MAX_INFO_ELEMENTS];
} bcm_int_ft_export_template_info_t;

/* Externs to global variables */
extern bcm_int_ft_info_t *ft_glb_info[BCM_MAX_NUM_UNITS];

/* Chip common driver routines */
extern int
bcm_ltsw_flowtracker_init_common(int unit);

extern int
bcm_ltsw_flowtracker_detach_common(int unit);

extern int
bcm_ltsw_flowtracker_group_create_common(
        int unit,
        uint32 options,
        bcm_flowtracker_group_t *flow_group_id,
        bcm_flowtracker_group_info_t *flow_group_info);

extern int bcm_ltsw_flowtracker_group_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_info_t *flow_group_info);

extern int bcm_ltsw_flowtracker_group_get_all_common(
        int unit,
        int max_size,
        bcm_flowtracker_group_t *flow_group_list,
        int *list_size);

extern int bcm_ltsw_flowtracker_group_data_get_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_flow_key_t *flow_key,
        bcm_flowtracker_flow_data_t *flow_data);

extern int bcm_ltsw_flowtracker_group_flow_count_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_count);

extern int bcm_ltsw_flowtracker_group_actions_set_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flags,
        int num_actions,
        bcm_flowtracker_group_action_info_t *action_list);

extern int bcm_ltsw_flowtracker_group_age_timer_set_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 aging_interval_ms);

extern int bcm_ltsw_flowtracker_group_flow_limit_set_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flow_limit);

extern int bcm_ltsw_flowtracker_group_export_trigger_set_common(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info);

extern int
bcm_ltsw_flowtracker_group_tracking_params_set_common(
                                int unit,
                                bcm_flowtracker_group_t id,
                                int num_tracking_params,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params);

extern int bcm_ltsw_flowtracker_elephant_profile_create_common(
        int unit,
        uint32_t options,
        bcm_flowtracker_elephant_profile_info_t *profile,
        bcm_flowtracker_elephant_profile_t *profile_id);

extern int
bcm_ltsw_flowtracker_group_collector_detach_common(
                                int unit,
                                bcm_flowtracker_group_t flow_group_id,
                                bcm_collector_t collector_id,
                                int export_profile_id,
                                bcm_flowtracker_export_template_t template_id);

extern int bcm_ltsw_flowtracker_group_elephant_profile_attach_get_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_elephant_profile_t *profile_id);

extern int bcm_ltsw_flowtracker_group_elephant_profile_detach_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id);

extern int
bcm_ltsw_flowtracker_group_collector_attach_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id);

extern int
bcm_ltsw_flowtracker_template_transmit_config_set_common(
    int unit,
    bcm_flowtracker_export_template_t template_id,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_template_transmit_config_t *config);

extern int
bcm_ltsw_flowtracker_template_transmit_config_get_common(
    int unit,
    bcm_flowtracker_export_template_t template_id,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_template_transmit_config_t *config);

extern int
bcm_ltsw_flowtracker_group_elephant_profile_attach_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_elephant_profile_t profile_id);

extern int bcm_ltsw_flowtracker_group_actions_get_common(
                               int unit,
                               bcm_flowtracker_group_t flow_group_id,
                               uint32 flags,
                               int max_actions,
                               bcm_flowtracker_group_action_info_t *action_list,
                               int *num_actions);

extern int bcm_ltsw_flowtracker_group_age_timer_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *aging_interval_ms);

extern int bcm_ltsw_flowtracker_group_export_trigger_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_export_trigger_info_t *export_trigger_info);

extern int bcm_ltsw_flowtracker_group_flow_limit_get_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 *flow_limit);

extern int
bcm_ltsw_flowtracker_group_tracking_params_get_common(
    int unit,
    bcm_flowtracker_group_t id,
    int max_size,
    bcm_flowtracker_tracking_param_info_t *list_of_tracking_params,
    int *list_size);

extern int bcm_ltsw_flowtracker_elephant_profile_get_common(
        int unit,
        bcm_flowtracker_elephant_profile_t profile_id,
        bcm_flowtracker_elephant_profile_info_t *profile);

extern int bcm_ltsw_flowtracker_export_template_destroy_common(int unit,
        bcm_flowtracker_export_template_t id);

extern int bcm_ltsw_flowtracker_export_template_get_common(
        int unit,
        bcm_flowtracker_export_template_t id,
        uint16 *set_id,
        int max_size,
        bcm_flowtracker_export_element_info_t *list_of_export_elements,
        int *list_size);

extern int
bcm_ltsw_flowtracker_export_template_create_common(
        int unit,
        uint32 options,
        bcm_flowtracker_export_template_t *id,
        uint16 set_id,
        int num_export_elements,
        bcm_flowtracker_export_element_info_t *list_of_export_elements);

extern int bcm_ltsw_flowtracker_elephant_profile_destroy_common(int unit,
        bcm_flowtracker_elephant_profile_t profile_id);

extern int bcm_ltsw_flowtracker_elephant_profile_destroy_common(int unit,
        bcm_flowtracker_elephant_profile_t profile_id);

extern int bcm_ltsw_flowtracker_elephant_profile_get_all_common(
        int unit,
        int max,
        bcm_flowtracker_elephant_profile_t *profile_list,
        int *count);

extern int bcm_ltsw_flowtracker_elephant_profile_create_common(int unit,
        uint32_t options,
        bcm_flowtracker_elephant_profile_info_t *profile,
        bcm_flowtracker_elephant_profile_t *profile_id);

extern int bcm_ltsw_flowtracker_group_elephant_profile_attach_get_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_elephant_profile_t *profile_id);

extern int
bcm_ltsw_flowtracker_group_collector_detach_common(int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id);

extern int
bcm_ltsw_flowtracker_template_transmit_config_set_common(int unit,
        bcm_flowtracker_export_template_t template_id,
        bcm_flowtracker_collector_t collector_id,
        bcm_flowtracker_template_transmit_config_t *config);

extern int
bcm_ltsw_flowtracker_template_transmit_config_get_common(int unit,
        bcm_flowtracker_export_template_t template_id,
        bcm_flowtracker_collector_t collector_id,
        bcm_flowtracker_template_transmit_config_t *config);

extern int
bcm_ltsw_flowtracker_group_elephant_profile_attach_common(int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_elephant_profile_t profile_id);

extern int bcm_ltsw_flowtracker_group_elephant_profile_detach_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id);

extern int
bcm_ltsw_flowtracker_group_collector_attach_common(int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id);

extern int bcm_ltsw_flowtracker_group_collector_attach_get_all_common(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        int max_list_size,
        bcm_flowtracker_collector_t *list_of_collectors,
        int *list_of_export_profile_ids,
        bcm_flowtracker_export_template_t *list_of_templates,
        int *list_size);
extern int bcm_ltsw_flowtracker_group_destroy_common(
        int unit,
        bcm_flowtracker_group_t id);

extern int bcm_ltsw_flowtracker_group_clear_common(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 flags);

extern int bcm_ltsw_flowtracker_static_flow_get_all_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_size,
    bcm_flowtracker_flow_key_t *flow_key_arr,
    int *list_size);

extern int bcm_ltsw_flowtracker_static_flow_delete_all_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id);

extern int bcm_ltsw_flowtracker_static_flow_delete_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_flow_key_t *flow_key);

extern int bcm_ltsw_flowtracker_static_flow_add_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_flow_key_t *flow_key);

extern int bcm_ltsw_flowtracker_group_flow_delete_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_group_flow_action_info_t *action_info);

extern int bcm_ltsw_flowtracker_group_flow_action_update_common(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_group_flow_action_info_t *action_info,
    int num_actions,
    bcm_flowtracker_group_action_info_t *action_list);

/* Logical tables access routines */

extern int
bcm_ltsw_flowtracker_hw_learn_global_enable_set(int unit, bool enable);

extern int
bcm_ltsw_flowtracker_control_entry_get(int unit,
                                       bcm_int_ft_info_t *entry);
extern int
bcm_ltsw_flowtracker_control_entry_set(int unit,
                                       bool update,
                                       bcm_int_ft_info_t *entry);
extern int
bcm_ltsw_flowtracker_group_entry_get(int unit,
                                     bcm_int_ft_flow_group_info_t *entry);
extern int
bcm_ltsw_flowtracker_group_entry_set(int unit,
                                     bool update,
                                     bcm_int_ft_flow_group_info_t *entry);
extern int
bcm_ltsw_flowtracker_group_entry_delete(int unit,
                                     int flow_group_id);
extern int
bcm_ltsw_flowtracker_group_status_get(int unit,
                                     bcm_int_ft_flow_group_info_t *entry);

extern int
bcm_ltsw_flowtracker_group_flow_data_get(
                                        int unit,
                                        bcm_flowtracker_group_t flow_group_id,
                                        bcm_flowtracker_flow_key_t *flow_key,
                                        bcm_flowtracker_flow_data_t *flow_data);

extern int bcm_ltsw_flowtracker_group_collector_map_entry_set(int unit,
        bool update,
        bcm_int_ft_group_col_map_t *entry);

extern int
bcm_ltsw_flowtracker_group_collector_map_delete(int unit,
                                     int flow_group_id);

extern int bcm_ltsw_flowtracker_is_collector_attached(int unit,
                                               uint32_t flow_group_id);

extern int bcm_ltsw_flowtracker_elephant_profile_entry_set(int unit,
        bool update,
        int *id,
        uint32_t options,
        bcm_flowtracker_elephant_profile_info_t *entry);

extern int bcm_ltsw_flowtracker_template_transmit_config_entry_get(int unit,
        bcm_int_ft_template_transmit_config_t *entry);

extern int bcm_ltsw_flowtracker_export_template_entry_update(int unit,
        bcm_int_ft_template_transmit_config_t * entry);

extern int bcm_ltsw_flowtracker_group_collector_map_entry_set(int unit,
        bool update,
        bcm_int_ft_group_col_map_t *entry);

extern int
bcm_ltsw_flowtracker_group_collector_map_delete(int unit,
        int flow_group_id);

extern int bcm_ltsw_flowtracker_is_collector_attached(int unit,
        uint32_t flow_group_id);

extern int bcm_ltsw_flowtracker_elephant_profile_entry_set(int unit,
        bool update,
        int *id,
        uint32_t options,
        bcm_flowtracker_elephant_profile_info_t *entry);

extern int bcm_ltsw_flowtracker_is_elephant_profile_used( int unit, uint32_t id, bool *is_used);

extern int bcm_ltsw_flowtracker_elephant_profile_entry_delete(int unit, int id);

extern int bcm_ltsw_flowtracker_elephant_profile_entry_get(int unit,
        int id,
        bcm_flowtracker_elephant_profile_info_t *entry);

extern int bcm_ltsw_flowtracker_export_template_entry_insert(int unit,
        bcm_int_ft_export_template_info_t *entry);

extern int bcm_ltsw_flowtracker_export_template_entry_get(int unit,
        int id,
        bcm_int_ft_export_template_info_t *entry);

extern int bcm_ltsw_flowtracker_is_export_template_used( int unit, uint32_t id, bool *is_used);

extern int bcm_ltsw_flowtracker_export_template_entry_delete(
        int unit,
        bcm_flowtracker_export_template_t id);

extern int bcm_ltsw_flowtracker_group_collector_map_entry_get(int unit,
        bcm_int_ft_group_col_map_t *entry);

extern int
bcm_ltsw_flowtracker_group_flow_data_clear_all(
                                        int unit,
                                        bcm_flowtracker_group_t flow_group_id);

extern int
bcm_ltsw_flowtracker_group_static_flow_entry_delete_all(
                                int unit,
                                bcm_flowtracker_group_t flow_group_id);

extern int
bcm_ltsw_flowtracker_group_static_flow_entry_get_all(
                                int unit,
                                bcm_flowtracker_group_t flow_group_id,
                                int max_size,
                                bcm_flowtracker_flow_key_t *flow_key,
                                int *list_size);
extern int
bcm_ltsw_flowtracker_group_static_flow_entry_set(
                                int unit,
                                bool del,
                                bcm_flowtracker_group_t flow_group_id,
                                bcm_flowtracker_flow_key_t *flow_key);
extern int
bcm_ltsw_flowtracker_hw_learn_flow_action_control_set(
            int unit,
            ft_hw_learn_flow_cmd_type_t cmd_type,
            bcm_flowtracker_group_t id,
            bcm_flowtracker_group_flow_action_info_t *action_info,
            int num_actions,
            bcm_int_ft_group_action_info_t *action_list);

extern int
bcm_ltsw_flowtracker_hw_learn_flow_action_state_get(
            int unit,
            uint8_t pipe_idx);
#endif /* BCMINT_LTSW_FLOWTRACKER_INT_H */

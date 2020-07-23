/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flowtracker.h
 * Purpose:     
 */
#ifndef _BCM_INT_FLOWTRACKER_H_
#define _BCM_INT_FLOWTRACKER_H_

#if defined(INCLUDE_FLOWTRACKER)

#include <soc/shared/ft.h>
#include <soc/shared/ft_msg.h>
#include <soc/profile_mem.h>
#include <shared/idxres_fl.h>
#include <bcm/flowtracker.h>

/* Group Start Id */
#define BCM_INT_FT_GROUP_START_ID 1

/* Maximum number of flow groups
 * TH/TH2 - IFP is used for FT Group hence restricted by MATCHED_RULE 8 bit
 *          value in COPY_TO_CPU functionality.
 * MV2 - Equal to number of entries in FT_GROUP_TABLE_CONFIGm minus entry
 *       with index 0 which is reserved entry.
 */
#define BCM_INT_FT_MAX_FLOW_GROUPS \
        (soc_feature(unit, soc_feature_mv2_style_flowtracker) ? \
        (soc_mem_index_count(unit, FT_GROUP_TABLE_CONFIGm) - 1) : \
         255)

/* Default value for max flows */
#define BCM_INT_FT_DEF_MAX_FLOWS \
        soc_feature(unit, soc_feature_mv2_style_flowtracker) ? 32768 : 16384

/* Default value for max counters per flow */
#define BCM_INT_FT_DEF_MAX_COUNTERS_PER_FLOW  1

/* Default max export length is 1500 bytes */
#define BCM_INT_FT_DEF_MAX_EXPORT_LENGTH 1500

/* Maximum number of templates  */
#define BCM_INT_FT_MAX_TEMPLATES 3

/* Template start Id */
#define BCM_INT_FT_TEMPLATE_START_ID 1

/* Template end Id */
#define BCM_INT_FT_TEMPLATE_END_ID \
    (BCM_INT_FT_TEMPLATE_START_ID + BCM_INT_FT_MAX_TEMPLATES - 1)

/* Maximum number of collectors. */
#define BCM_INT_FT_MAX_COLLECTORS 1

/* Collector Start Id */
#define BCM_INT_FT_COLLECTOR_START_ID 1

/* Collector End Id */
#define BCM_INT_FT_COLLECTOR_END_ID \
    (BCM_INT_FT_COLLECTOR_START_ID + BCM_INT_FT_MAX_COLLECTORS - 1)

/* Maximum number of export_profiles. */
#define BCM_INT_FT_MAX_EXPORT_PROFILES 1

/* Export_Profile Start Id */
#define BCM_INT_FT_EXPORT_PROFILE_START_ID 1

/* Export_Profile End Id */
#define BCM_INT_FT_EXPORT_PROFILE_END_ID \
    (BCM_INT_FT_EXPORT_PROFILE_START_ID + BCM_INT_FT_MAX_EXPORT_PROFILES - 1)

/* Valid range of Set_ID is 256 to 65535. 
 * Not checking 65535 condition since its satisfied 
 * by the fact that set_id is uint16
 */
#define BCM_INT_FT_TEMPLATE_SET_ID_VALID(set_id) (set_id >=256)

/* Set Id used when transmiting template sets */
#define FT_INT_TEMPLATE_XMIT_SET_ID 2

/* Default value of export interval in elephant mode. 50 ms */
#define BCM_INT_FT_ELPH_DEF_EXPORT_INTERVAL_MSECS (50)

/* Default value of export interval. 100 ms */
#define BCM_INT_FT_DEF_EXPORT_INTERVAL_MSECS (100)

/* Default value of scan interval. 100 ms */
#define BCM_INT_FT_DEF_SCAN_INTERVAL_USECS (BCM_INT_FT_DEF_EXPORT_INTERVAL_MSECS * 1000)

/* Default value of scan interval. 100 ms */
#define BCM_INT_FT_ELPH_DEF_SCAN_INTERVAL_USECS (100 * 1000)

/* Minimum value of scan interval (2msec) */
#define BCM_INT_FT_ELPH_MIN_SCAN_INTERVAL_USECS (2 * 1000)

/* Elephant Scan interval must be in steps of 1 msec */
#define BCM_INT_FT_ELPH_SCAN_INTERVAL_STEP_USECS (1 * 1000)

/* Scan interval must be in steps of 1 msec */
#define BCM_INT_FT_SCAN_INTERVAL_STEP_USECS (1 * 1000)

/* Max value of scan interval. 500 ms */
#define BCM_INT_FT_MAX_SCAN_INTERVAL_USECS (500 * 1000)

/* Default value of aging time. 20 secs in milliseconds */
#define BCM_INT_FT_DEF_AGING_INTERVAL_MSECS (20 * 1000)

/* Aging interval must be in steps of 1 sec */
#define BCM_INT_FT_AGING_INTERVAL_STEP_MSECS (1000)

/* Aging interval can be up to 255 times scan interval */
#define BCM_INT_FT_AGING_INTERVAL_SCAN_MULTIPLE (255)

/* Size of the template set header */
#define BCM_INT_FT_SET_HDR_LENGTH 4

/* Size of the IPFIX header */
#define BCM_INT_FT_IPFIX_MSG_HDR_LENGTH 16

/* Minimum length of the collecotor encap
 * UDP_HDR  =  8 bytes
 * IPv4_HDR = 20 bytes
 * ETH_HDR  = 14 bytes
 * L2_CRC   =  4 bytes
 * 8 + 20 + 14 + 4 = 46
 */
#define BCM_INT_FT_MIN_COLLECTOR_ENCAP_LENGTH 46

/* Length of the L2 CRC field */
#define BCM_INT_FT_L2_CRC_LENGTH 4

/* Rx channel index used by FT EAPP */
#define BCM_INT_FT_EAPP_RX_CHANNEL 1

/* Maximum number of elephant profiles */
#define BCM_INT_FT_MAX_ELEPHANT_PROFILES 4

/* Elephant rate thresholds must be configured in steps of 80 kbits/sec */
#define BCM_INT_FT_ELPH_RATE_THRESHOLD_STEP 80

/* Elephant profile Start Id */
#define BCM_INT_FT_ELPH_PROFILE_START_ID 1

/* Elephant profile END Id */
#define BCM_INT_FT_ELPH_PROFILE_END_ID \
    (BCM_INT_FT_ELPH_PROFILE_START_ID + BCM_INT_FT_MAX_ELEPHANT_PROFILES - 1)

/* Max number of actions that can be applied on a group */
#define BCM_INT_FT_MAX_GROUP_ACTIONS 5

/* Max number of params per action */
#define BCM_INT_FT_MAX_GROUP_ACTION_PARAMS 2

/* Invalid EM group Id */
#define BCM_INT_FT_INVALID_EM_GROUP_ID (-1)

/* Maximum number of congestion monitor flows */
#define BCM_INT_QCM_FT_MAX_FLOWS    1000

/* Maximum number of congestion monitor flow groups */
#define BCM_INT_QCM_FT_MAX_FLOW_GROUPS  15

/* Invalid FW core */
#define BCM_INT_FT_INVALID_FW_CORE (-1)

/*
 * Macro:
 *     _BCM_FT_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_FT_ALLOC(_ptr_,_ptype_,_size_,_descr_)                     \
            do {                                                         \
                if (NULL == (_ptr_)) {                                   \
                   (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
                }                                                        \
                if((_ptr_) != NULL) {                                    \
                    sal_memset((_ptr_), 0, (_size_));                    \
                }  else {                                                \
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, \
                              (BSL_META("FT Error: Allocation failure %s\n"), \
                               (_descr_)));                              \
                }                                                        \
            } while (0)

/* Max number of parts the actions is divided in H/w */
#define BCM_INT_FT_ACTION_MAX_PARTS 2

/* Minimum of 2 values */
#define _BCM_FT_MIN(a,b) ((a) < (b) ? (a) : (b))


/* Group actions width, offset linked list */
typedef struct _bcm_int_ft_action_offset_s {
    /* Offset into the H/w buffer */
    uint16 offset[BCM_INT_FT_ACTION_MAX_PARTS];

    /* Width of the action*/
    uint8 width[BCM_INT_FT_ACTION_MAX_PARTS];

    /* Value that needs to be programmed */
    uint8 value[BCM_INT_FT_ACTION_MAX_PARTS];

} _bcm_int_ft_action_offset_t;

/* Flowtracker Internal action types */
typedef enum _bcm_ft_action_e {
    /* Change color of green packets */
    _bcmIntFtActionGpDropPrecedence = 0,

    /* Change color of yellow packets */
    _bcmIntFtActionYpDropPrecedence = 1,

    /* Change color of red packets */
    _bcmIntFtActionRpDropPrecedence = 2,

    /* Assign new Unicast cosq for Green packets */
    _bcmIntFtActionGpUcastCosQNew = 3,

    /* Assign new Unicast cosq for Yellow packets */
    _bcmIntFtActionYpUcastCosQNew = 4,

    /* Assign new Unicast cosq for Red packets */
    _bcmIntFtActionRpUcastCosQNew = 5,

    /* Assign new Multicast cosq for Green packets */
    _bcmIntFtActionGpMcastCosQNew = 6,

    /* Assign new Multicast cosq for Yellow packets */
    _bcmIntFtActionYpMcastCosQNew = 7,

    /* Assign new Multicast cosq for Red packets */
    _bcmIntFtActionRpMcastCosQNew = 8,

    /* Assign new Unicast & Multicast cosq for Green packets */
    _bcmIntFtActionGpCosQNew = 9,

    /* Assign new Unicast & Multicast cosq for Yellow packets */
    _bcmIntFtActionYpCosQNew = 10,

    /* Assign new Unicast & Multicast cosq for Red packets */
    _bcmIntFtActionRpCosQNew = 11,

    /* Change int_pri for Green packets */
    _bcmIntFtActionGpPrioIntNew = 12,

    /* Change int_pri for Yellow packets */
    _bcmIntFtActionYpPrioIntNew = 13,

    /* Change int_pri for Red packets */
    _bcmIntFtActionRpPrioIntNew = 14,

    /* First Seen INT Packet */
    _bcmIntFtFspReinjectActionIntNew = 15,

    /* Monitor flows Dropped packet */
    _bcmIntFtDropMonitorActionIntNew = 16,

    /* Congestion Monitor flows */
    _bcmIntFtCongestionMonitorActionIntNew = 17,

    /* Last value, not usable */
    _bcmIntFtActionCount
} _bcm_ft_action_t;

/* Actions info internal */
typedef struct _bcm_int_ft_action_info_s {
    /* Action element */
    _bcm_ft_action_t action;

    /* param0 */
    uint32 param0;

} _bcm_int_ft_action_info_t;

/* Group actions info */
typedef struct _bcm_int_ft_group_action_info_s {
    /* Action */
    bcm_flowtracker_group_action_t action;

    /* Number of params */
    int num_params;

    /* Params */
    uint32 params[BCM_INT_FT_MAX_GROUP_ACTION_PARAMS];
} _bcm_int_ft_group_action_info_t;

/* Per pipe information. */
typedef struct  _bcm_int_ft_pipe_info_s {
    /* Max flows */
    uint32 max_flows;

    /* Size of reserved pools */
    uint16 ctr_pool_size[SHR_FT_MAX_CTR_POOLS];

    /* Reserved Flex counter pools per pipe */
    uint8 ctr_pools[SHR_FT_MAX_CTR_POOLS];

    /* Number of reserved counter pools */
    uint8 num_ctr_pools;

    /* QoS profile mem ptr */
    soc_profile_mem_t *qos_profile_mem;
} _bcm_int_ft_pipe_info_t;

#define BCM_INT_FT_INFO_FLAGS_FLOW_START_TS             (1 << 0)
#define BCM_INT_FT_INFO_FLAGS_DROP_MONITOR              (1 << 1)
#define BCM_INT_FT_INFO_FLAGS_HOSTMEM_ENABLE            (1 << 2)
#define BCM_INT_FT_INFO_FLAGS_DROP_CONGESTION_MONITOR   (1 << 3)

/* Internal data structure to store
 * global flowtracker information.
 */
typedef struct _bcm_int_flowtracker_info_s {
    /* uController number running FT appl */
    int uc_num;
    /* Elephant mode */
    uint8 elph_mode;

    /* Flow Tracker Config Mode */
    uint8 ft_config_mode;

    /* Configuration mode */
    shr_ft_cfg_modes_t cfg_mode;

    /* Features supported by the EApp */
    uint32 uc_features;

    /* flags, See BCM_INT_FT_INFO_FLAGS_XXX */
    uint32 flags;

    /* Max number of flow groups.
     * Got from soc property flowtracker_max_flow_groups
     */
    uint32 max_flow_groups;
    /* Max number of flows for entire switch.
     * Just a summation of max_flows_per_pipe for all pipes.
     */
    uint32 max_flows_total;
    /* The maximum MTU size that is possible for export
     * packets.
     * Got from soc property flowtracker_max_export_pkt_length
     */
    uint16 max_export_pkt_length;
    /* IPFIX observation domain Id */
    bcm_flowtracker_ipfix_observation_domain_t observation_domain_id;
    /* DMA Buffer used for messaging between SDK and
     * UKERNEL.
     */
    uint8 *dma_buffer;
    /* Length of the DMA buffer */
    int dma_buffer_len;
    /* Template ID pool */
    shr_idxres_list_handle_t template_pool;
    /* Collector ID pool */
    shr_idxres_list_handle_t collector_pool;
    /* Export profile ID pool */
    shr_idxres_list_handle_t export_profile_pool;
    /* Flow group ID pool */
    shr_idxres_list_handle_t flow_group_pool;
    /* Flow group ID pool */
    shr_idxres_list_handle_t elph_profile_pool;
    /* Number of pipes. */
    uint8 num_pipes; 
    /* Per pipe information. */
    _bcm_int_ft_pipe_info_t pipe_info[BCM_PIPES_MAX];

    /* Enterprise Number */
    uint32 enterprise_number;

    /* Export interval (msecs) */
    uint32 export_interval;

    /* Scan interval (usecs) */
    uint32 scan_interval_usecs;

    /* Maximum length of the reinjected FSP packet */
    uint16 fsp_reinject_max_length;

    /* Default group ID */
    uint16 default_grp_id;

    /* Number of templates configured */
    uint16 num_templates;

    /* EM groups Mapping */
    int  em_group_id_map[SHR_FT_MAX_EM_GROUPS][BCM_PIPES_MAX];

} _bcm_int_ft_info_t;


/* Internal data structure to store the information element data, used in
 * template set export
 */
typedef struct _bcm_int_ft_info_elem_s {
    /* Export element */
    shr_ft_template_info_elem_t element;

    /* Size of each element */
    uint16 data_size;

    /* Id corresponding to the export element, IANA assigned for
     * non-enterprise
     */
    uint16 id;

    /* 1 - Enterprise element, 0 - Non enterprise element */
    uint8 enterprise;
} _bcm_int_ft_export_element_t;

/* Internal data structure to store per export template
 * information.
 */
typedef struct _bcm_int_ft_export_template_info_s {
    /* Software ID of the template */
    bcm_flowtracker_export_template_t template_id;
    /* Set ID that is put into the IPFIX packet */
    uint16 set_id;
    /* Template set transmit interval */
    int interval_secs;
    /* Number of packets sent out initially */
    int initial_burst;
    /* Number of information elements */
    int num_export_elements;
    /* Export elements */
    _bcm_int_ft_export_element_t elements[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];
} _bcm_int_ft_export_template_info_t;


/* Internal data structure to store per collector information
 */
typedef struct _bcm_int_ft_collector_info_s {
    /* Software ID of the collector */
    bcm_flowtracker_collector_t collector_id;
    /* Collector information structure */
    bcm_flowtracker_collector_info_t collector_info;
} _bcm_int_ft_collector_info_t;

typedef struct _bcm_int_ft_v2_collector_info_s {
    /* Internal Collector Id */
    int int_id;
} _bcm_int_ft_v2_collector_info_t;

typedef struct _bcm_int_ft_v2_export_profile_info_s {
    /* Internal Export_Profile Id */
    int int_id;
} _bcm_int_ft_v2_export_profile_info_t;

/* Internal data structure to store per flow_group information
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

    _bcm_int_ft_group_action_info_t *action_list;

    /* Export triggers which will result in the export of the packets */
    uint32    export_trigger;

    /* Field group corresponding to the flow group. */
    bcm_field_group_t field_group[BCM_PIPES_MAX];

    /* Number of Tracking Params on the group */
    int num_tracking_params;

    bcm_flowtracker_tracking_param_info_t *tracking_params;

} _bcm_int_ft_flow_group_info_t;

/* Internal data structure to store elephant profile info
 */
typedef struct _bcm_int_ft_elph_profile_info_s {
    /* Software Id of the profile */
    bcm_flowtracker_elephant_profile_t id;

    /* Profile information structure */
    bcm_flowtracker_elephant_profile_info_t profile;
} _bcm_int_ft_elph_profile_info_t;

/* Global data structures extern declarations */
extern _bcm_int_ft_info_t *ft_global_info[BCM_MAX_NUM_UNITS];

/* Utility functions */
extern int
_bcm_esw_flowtracker_ucast_cosq_resolve(int unit, uint32 cosq, int *hw_cosq);

extern int
_bcm_esw_flowtracker_mcast_cosq_resolve(int unit, uint32 cosq, int *hw_cosq);

/* Get the FT_INFO struct for the unit */
#define FT_INFO_GET(_u) ft_global_info[(_u)]


extern void _bcm_esw_flowtracker_sw_dump(int unit);
extern void _bcm_esw_flowtracker_dump_stats_learn(int unit);
extern void _bcm_esw_flowtracker_dump_stats_export(int unit);
extern void _bcm_esw_flowtracker_dump_stats_age(int unit);
extern void _bcm_esw_flowtracker_dump_stats_elephant(int unit);
extern void _bcm_esw_qcm_sw_dump(int unit);
extern void _bcm_esw_qcm_stats_enable(int unit, uint8 enable);
extern void _bcm_esw_qcm_dump_stats_learn(int unit);
extern void _bcm_esw_qcm_dump_stats_export(int unit);
extern void _bcm_esw_qcm_dump_stats_age(int unit);
extern void _bcm_esw_qcm_dump_stats_scan(int unit);

/* Max number of pipes across all devices */
#define BCM_INT_FT_QCM_MAX_PIPES         8

/* Max number of counter per pipes */
#define BCM_INT_FT_QCM_MAX_COUNTERS      2

/* Max number of MMU BLOCK */
#define BCM_INT_FT_QCM_NUM_MMU_BLOCK     4

typedef struct ft_qcm_port_cfg_s {
    bcm_gport_t gport;
    uint32 global_num;
    uint32 port_pipe_queue_num;
    uint16 pport;
} _bcm_esw_ft_qcm_port_cfg_t;

typedef struct ft_qcm_init_cfg_s {
    uint32 drop_index[(BCM_INT_FT_QCM_MAX_PIPES * BCM_INT_FT_QCM_MAX_COUNTERS)];
    uint32 util_index[(BCM_INT_FT_QCM_MAX_PIPES * BCM_INT_FT_QCM_MAX_COUNTERS)];
    uint32 egr_index[BCM_INT_FT_QCM_MAX_PIPES];
    uint16 max_port_num;
    uint16 queues_per_pipe;
    uint16 ports_per_pipe;
    uint8  queues_per_port;
    uint8  num_pipes;
    uint8  num_counters_per_pipe;
    uint8  acc_type[(BCM_INT_FT_QCM_MAX_PIPES * BCM_INT_FT_QCM_MAX_COUNTERS)];
} _bcm_esw_ft_qcm_init_cfg_t;

#endif /* INCLUDE_FLOWTRACKER */
extern int _bcm_esw_flowtracker_sync(int unit);
#endif  /* !_BCM_INT_FT_H_ */

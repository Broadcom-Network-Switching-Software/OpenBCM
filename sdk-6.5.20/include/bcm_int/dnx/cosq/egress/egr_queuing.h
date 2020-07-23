/** \file egr_queuing.h
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef EGR_QUEUING_H_INCLUDED
/** { */
#define EGR_QUEUING_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cosq/cosq.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>

/*************
 * DEFINES   *
 */
/* { */

/*
 * Number of FAP-data-ports.
 */
#define DNX_NOF_FAP_PORTS             256
#define DNX_MAX_FAP_PORT_ID           (DNX_NOF_FAP_PORTS-1)

#define DNX_EGR_QUEUE_DEFAULT_PROFILE_MAP       (0)
#define DNX_EGR_Q_ALMOST_EMPTY_DELAY            (3)
#define DNX_EGQ_COUNTER_INFO_GET(_info, _handle_id, _table, _field, _name, _type, _has_ovf)    \
{                                                                                              \
    sal_snprintf(_info.name, DNX_EGQ_COUNTER_NAME_LEN, "%s", _name);                           \
    _info.type = _type;                                                                        \
    _info.has_ovf = _has_ovf;                                                                  \
    dbal_tables_field_size_get(unit, _table, _field, 0, 0, 0,&_info.counter_size);             \
    dbal_value_field64_request(unit, _handle_id, _field, INST_SINGLE, &(_info.count_val));     \
}
#define DNX_EGQ_SHAPER_MODE_NOF_BITS        (1)

/**
 * PS_MODE values to load into HW to indicate priority mode for a qpair group
 * (port scheduler). See EPS_PS_MODE register (or DBAL_TABLE_
 */
#define DNX_EGQ_PS_MODE_ONE_PRIORITY_VAL    (0)
#define DNX_EGQ_PS_MODE_TWO_PRIORITY_VAL    (1)
#define DNX_EGQ_PS_MODE_FOUR_PRIORITY_VAL   (2)
#define DNX_EGQ_PS_MODE_EIGHT_PRIORITY_VAL  (3)

#define DNX_EGQ_TCG_WEIGHT_MIN              (0)
#define DNX_EGQ_TCG_WEIGHT_MAX              (255)
/**
 * TCG: Number of groups
 */
#define DNX_NOF_TCGS                      (dnx_data_egr_queuing.params.nof_tcg_get(unit))
#define DNX_TCG_MIN                       0
#define DNX_TCG_MAX                       (DNX_NOF_TCGS-1)
/*
 * This range, DNX_TCG_MIN - DNX_TCG_MAX, must not overlap BCM_COSQ_SP0 - BCM_COSQ_SP7
 * See dnx_cosq_egq_gport_egress_tc_to_tcg_map_set()
 */
#if (DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG >= BCM_COSQ_SP0)
#error "Must be smaller than"
#endif
#define DNX_EGR_NOF_Q_PAIRS               (dnx_data_egr_queuing.params.nof_q_pairs_get(unit))
#define DNX_EGR_NOF_BASE_Q_PAIRS          (DNX_EGR_NOF_Q_PAIRS)
/**
 * Number of qpairs per one port scheduler.
 */
#define DNX_EGR_NOF_Q_PAIRS_IN_PS         (dnx_data_egr_queuing.params.nof_q_pairs_in_ps_get(unit))
#define DNX_NOF_TCGS_IN_PS                (dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
/**
 * Total number of port schedulers. This is calculated by dividing the total
 * number of 'qpair's by the number of qpairs per one port scheduler.
 * See 'nof_q_pairs_in_ps' and 'nof_q_pairs' in dnx_egr_queuing.xml
 */
#define DNX_EGR_NOF_PS                    (dnx_data_egr_queuing.params.nof_port_schedulers_get(unit))
#define DNX_EGR_NOF_TCG_IDS               (DNX_EGR_NOF_PS * DNX_NOF_TCGS)
#define DNX_EGQ_COUNTER_NAME_LEN          (64)
#define DNX_EGQ_RQP_COUNTER_NUM           (18)
#define DNX_EGQ_PQP_COUNTER_NUM           (8)
#define DNX_EGQ_EPNI_COUNTER_NUM          (5)
#define DNX_EGQ_COUNTER_NUM_SUM           (DNX_EGQ_RQP_COUNTER_NUM + DNX_EGQ_PQP_COUNTER_NUM + 2*DNX_EGQ_EPNI_COUNTER_NUM)

#define DNX_EGQ_NOF_TCG_IN_BITS           (dnx_data_egr_queuing.params.nof_bits_in_nof_tcg_get(unit))

/**
 * Flag to indicate counters that can be configured.
 */
#define DNX_EGQ_PROGRAMMABLE_COUNTER         (0x1)
/**
 * Flag to indicate counters that can not be configured.
 */
#define DNX_EGQ_NON_PROGRAMMABLE_COUNTER     (0x2)
/**
 * Flag to indicate EPNI Queue counters.
 */
#define DNX_EGQ_EPNI_QUEUE_COUNTER           (0x4)
/**
 * Flag to indicate EPNI Interface counters.
 */
#define DNX_EGQ_EPNI_IF_COUNTER              (0x8)
/**
 * Flag to indicate EPNI Mirror counters.
 */
#define DNX_EGQ_EPNI_MIRROR_COUNTER          (0x10)
/**
 * Flag to indicate the configuration of counting bytes.
 */
#define DNX_EGQ_CONFIGURATION_COUNT_BYTES    (0x20)
/**
 * Flag to indicate the configuration of counting packets.
 */
#define DNX_EGQ_CONFIGURATION_COUNT_PACKETS   (0x40)
#define DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX        255

#define DNX_EGR_SINGLE_MEMBER_TCG_START       (4)
#define DNX_EGR_SINGLE_MEMBER_TCG_END         (7)

/* } */
/*************
 * TYPEDEFS   *
 */
/* { */

typedef enum
{
    /**
     * Discrete partition scheme                                                                                                      .
     * Set the thresholds independently. 
     * If the number of allocated resources exceeds the threshold of a given priority,                                                .
     * then all packets with that priority are dropped.
     */
    DNX_EGR_QUEUING_PARTITION_SCHEME_DISCRETE = 0,
    /**
     * Strict partition scheme. Set the thresholds in ascending order. 
     * If the number of allocated resources exceeds a threshold, 
     * Threshold(i) for example,
     * then all packets whose priority is less than i are dropped.  
     */
    DNX_EGR_QUEUING_PARTITION_SCHEME_STRICT = 1,
    /**
     * Total number of partition schemes.
     */
    DNX_NOF_EGR_QUEUING_PARTITION_SCHEMES = 2
} dnx_egr_queuing_partition_scheme_e;

typedef struct
{
    /**
     *  The lower the weight the higher the bandwidth.
     *  Weight of 0 for a class indicates that this class
     *  has SP over the other class.
     *  When both have equal weights it implies simple RR.
     *  Range: 0-255
     */
    uint32 tcg_weight;
    /**
     *  If set, tcg weight is valid and taking part of the
     *  WFQ policy.
     */
    uint8 tcg_weight_valid;

} dnx_egr_tcg_sch_wfq_t;

typedef enum
{
  /**
   *  Port shaper is in data mode.
   */
    DNX_EGR_PORT_SHAPER_DATA_MODE = 0,
  /**
   *  Port shaper is in packet mode.
   */
    DNX_EGR_PORT_SHAPER_PACKET_MODE = 1,
  /**
   *  Total number of Port shaper modes.
   */
    DNX_EGR_NOF_PORT_SHAPER_MODES = 2
} dnx_egr_port_shaper_mode_e;

typedef enum
{
    /**
     *  Port with one priority.
     */
    DNX_EGR_PORT_ONE_PRIORITY = 1,
    /**
     *  Port with two priorities.
     */
    DNX_EGR_PORT_TWO_PRIORITIES = 2,
    /*
     *  Port with two priorities.
     */
    DNX_EGR_PORT_FOUR_PRIORITIES = 4,
    /**
     *  Port with eight priorities.
     */
    DNX_EGR_PORT_EIGHT_PRIORITIES = 8,
    /**
     *  Total number of Port prioritie modes.
     */
    DNX_EGR_NOF_PORT_PRIORITY_MODES = 4
} dnx_egr_port_priority_mode_e;

/**
 * Port id.
 * DNX range: 0-255.
 */
typedef uint32 dnx_fap_port_id_t;

typedef struct
{
    /**
     *  Egress Traffic Class. In general, Range: 0 - 1.
     *  (Equivalent to High (0)/Low (1)). The range can be higher
     *  according to the extended queuing OFP-group
     *  configuration (Possible global maximum: 1, 3, 5 and 7).
     */
    uint32 tc;
    /**
     *  Egress Drop precedence (drop priority). Range: 0 - 3.
     */
    uint32 dp;

} dnx_egr_q_priority_t;

typedef enum
{
    FIRST_DNX_EGR_CAL_TYPE = 0,
    CAL_TYPE_FQP = FIRST_DNX_EGR_CAL_TYPE,
    CAL_TYPE_PQP,
    NUM_DNX_EGR_CAL_TYPE
} dnx_egr_cal_type_e;
/**
 * specifies the selection of Almost empty minimum gap when All Qs under an interface are AE but the interface is not.
 */
typedef enum dnx_egr_ifc_min_gap_e
{
    /**
     * selected for non-channelized interface with 4 or 8 priorities
     */
    DNX_EGR_IFC_MIN_GAP_4_8,
    /**
     * selected for channelized interface
     */
    DNX_EGR_IFC_MIN_GAP_CHANNELIZED,
    /**
     * selected for channelized interface with 1 or 2 priorities
     */
    DNX_EGR_IFC_MIN_GAP_1_2,
    DNX_EGR_IFC_MIN_GAP_MAX
} dnx_egr_ifc_min_gap_e;
/**
 * specifies the Egress multicast replication fifo's types
 */
typedef enum dnx_egr_emr_fifo_e
{
    DNX_EGR_EMR_FIFO_TDM,
    DNX_EGR_EMR_FIFO_UC,
    DNX_EGR_EMR_FIFO_MC_HIGH,
    DNX_EGR_EMR_FIFO_MC_LOW,
    DNX_EGR_EMR_FIFO_MAX
} dnx_egr_emr_fifo_t;
/**
 * specifies the egq counter type
 */
typedef enum dnx_egq_counter_type_e
{
    DNX_EGQ_COUTNER_TYPE_PACKET,
    DNX_EGQ_COUTNER_TYPE_BYTE
} dnx_egq_counter_type_t;
/**
 * specifies the egq counter filter configuration
 */
typedef enum dnx_egq_counter_filter_e
{
    DNX_EGQ_COUTNER_FILTER_BY_QP,
    DNX_EGQ_COUTNER_FILTER_BY_OTM,
    DNX_EGQ_COUTNER_FILTER_BY_IF,
    DNX_EGQ_COUTNER_FILTER_BY_MIRROR,
    DNX_EGQ_COUTNER_FILTER_BY_NONE
} dnx_egq_counter_filter_t;
/**
 * data structure for egq counter information
 */
typedef struct
{
    char name[DNX_EGQ_COUNTER_NAME_LEN];
    dnx_egq_counter_type_t type;
    uint64 count_val;
    int has_ovf;
    int count_ovf;
    int counter_size;
} dnx_egq_counter_info_t;
typedef struct
{
    /**
     * Service pool eligibility. Indicates if it can use shared resources.
     * Range: 0 - 1.
     */
    uint32 pool_eligibility;
    /**
     * Service Pool index.
     * Range: 0 - 1.
     */
    uint32 pool_id;
    /**
     * Traffic Class group for service pool thresholds.
     * Range: 0 - 7.
     */
    uint32 tc_group;
} dnx_egr_queuing_mc_cos_map_t;

typedef enum
{
    FIRST_DNX_EGR_Q_PRIO_MAPPING_TYPE = 0,
    /**
     *  Mapping from Traffic Class and Drop Precedence to Egress
     *  Queue Priority:Unicast packets to scheduled queues.
     */
    DNX_EGR_UCAST_TO_SCHED = FIRST_DNX_EGR_Q_PRIO_MAPPING_TYPE,
    /**
     *  Multicast packets to unscheduled queues (unscheduled
     *  multicast).
     */
    DNX_EGR_MCAST_TO_UNSCHED,
    /**
     *  Must be the last value. Indicates the number of [Traffic
     *  Class, Drop Precedence] to Egress Queue Priority Mapping
     *  modes.
     */
    NUM_DNX_EGR_Q_PRIO_MAPPING_TYPE
} dnx_egr_q_prio_mapping_type_e;

/*
 * See dnx_egr_q_prio_mapping_type_e
 */
#define  DNX_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAPPING_TYPES   (NUM_DNX_EGR_Q_PRIO_MAPPING_TYPE)

typedef struct
{
    /**
     * Schedule (UC) words consumed per priority 
     */
    dnx_egr_q_priority_t queue_mapping[NUM_DNX_EGR_Q_PRIO_MAPPING_TYPE][DNX_COSQ_NOF_TC][DNX_COSQ_NOF_DP];
} dnx_cosq_egress_queue_mapping_info_t;

/*
 * Set of parameters related to FQP profile
 */
typedef struct
{
    int irdy_thr;
    int txq_max_bytes;
    int is_belong_to_sub_calendar;
    int txq_tdm_irdy;
    int min_gap;
    int serve_consecutive_if;
} dnx_fqp_profile_params_t;

/* 
 * TCG Traffic class groups. Range: 0-7
 */
typedef uint32 dnx_tcg_ndx_t;
typedef enum
{
    /**
     *  Egress priority for unscheduled traffic - low
     */
    DNX_EGR_Q_PRIO_LOW = 0,
    /**
     *  Egress priority for unscheduled traffic - high
     */
    DNX_EGR_Q_PRIO_HIGH = 1,
    /**
     *  Number of egress priorities for unscheduled traffic - petra B.
     */
    DNX_EGR_NOF_Q_PRIO_PB = 2,
    /**
     *  Egress priority in DNX 0 - 7
     */
    DNX_EGR_Q_PRIO_0 = 0,
    DNX_EGR_Q_PRIO_1 = 1,
    DNX_EGR_Q_PRIO_2 = 2,
    DNX_EGR_Q_PRIO_3 = 3,
    DNX_EGR_Q_PRIO_4 = 4,
    DNX_EGR_Q_PRIO_5 = 5,
    DNX_EGR_Q_PRIO_6 = 6,
    DNX_EGR_Q_PRIO_7 = 7,
    /**
     *  Number of egress priorities for unscheduled traffic.
     */
    DNX_EGR_NOF_Q_PRIO
} dnx_egr_q_prio_e;
typedef struct
{
    /**
     *  TCG (Traffic class groups) that the q-pair is mapped to.
     *   Range: 0 - 7                                                                                                  .
     */
    dnx_tcg_ndx_t tcg_ndx[DNX_EGR_NOF_Q_PRIO];
} dnx_egr_queuing_tcg_info_t;

typedef struct
{
    /**
     *  Unscheduled, or Unscheduled weight. The lower the
     *  weight the higher the bandwidth. Weight of 0 for a class
     *  indicates that this class has SP over the other class.
     *  When both have equal weights it implies simple RR.
     *  Range: 0-255
     */
    uint32 unsched_weight;
    /**
     *  Scheduled, or Scheduled weight. Format -same as
     *  unscheduled. Range: 0 - 255.
     */
    uint32 sched_weight;

} dnx_egr_ofp_sch_wfq_t;

typedef enum
{
    FIRST_DNX_EGR_OFP_INTERFACE_PRIO = 0,
    /**
     *  Outgoing FAP Strict Priority, for egress scheduling -
     *  high. Note: this priority is only valid for ports mapped
     *  to channelized NIF port! Otherwise - ignored.
     */
    DNX_EGR_OFP_INTERFACE_PRIO_HIGH = FIRST_DNX_EGR_OFP_INTERFACE_PRIO,
    /**
     *  OFP priority - low
     */
    DNX_EGR_OFP_INTERFACE_PRIO_LOW,
    /**
     *  Must be the last value. Indicates the number of OFP
     *  scheduling priorities.
     */
    DNX_EGR_OFP_CHNIF_NOF_PRIORITIES
} dnx_egr_ofp_interface_prio_e;

typedef struct
{
    uint32 mc_or_mc_low_queue_weight;
    uint32 uc_or_uc_low_queue_weight;
} dnx_egq_dwm_tbl_data_t;

typedef struct
{
    uint32 lb_key_max;
    uint32 lb_key_min;
    uint32 second_range_lb_key_max;
    uint32 second_range_lb_key_min;
    uint32 is_stacking_port;
    uint32 peer_tm_domain_id;
    uint32 port_type;
    uint32 cnm_intrcpt_fc_vec_llfc;
    uint32 cnm_intrcpt_fc_vec_pfc;
    uint32 cnm_intrcpt_fc_en;
    uint32 cnm_intrcpt_drop_en;
    uint32 ad_count_out_port_flag;
    uint32 cgm_port_profile;    /* Threshold type */
    uint32 cgm_interface;
    uint32 base_q_pair_num;
    uint32 cos_map_profile;
    uint32 pmf_data;
    uint32 disable_filtering;
} dnx_egq_ppct_tbl_data_t;

typedef struct
{
    /**
     * 4b 
     */
    uint32 map_profile;
    /**
     * 1b 
     */
    uint32 is_egr_mc;
    /**
     * 3b 
     */
    uint32 tc;
    /**
     * 2b 
     */
    uint32 dp;

} dnx_egq_tc_dp_map_tbl_entry_t;
typedef struct
{
    uint32 tc;
    uint32 dp;
} dnx_egq_tc_dp_map_tbl_data_t;

typedef int dnx_port_t;

typedef struct
{
    /**
     *  Strict Priority, if mapped to channelized
     *  Interface. Otherwise - ignored by the HW.
     */
    dnx_egr_ofp_interface_prio_e nif_priority;
    /**
     *  WFQ weights. The WFQ is among priority traffic
     *  (scheduled/unscheduled). Each index corresponds
     *  to q-pair.
     *  Valid for DNX only.
     */
    dnx_egr_ofp_sch_wfq_t ofp_wfq_q_pair[DNX_EGR_NOF_Q_PRIO];
} dnx_egr_ofp_sch_info_t;

/* } */
/****************
 * PROTOTYPES   *
 */
/* { */

int dnx_egr_queuing_default_tc_dp_map_set(
    int unit,
    bcm_port_t port);
/**
 * \brief -
 *   Sets bandwidth for egress mirror/recycle channel
 *
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core -
 *   Core to update HW on.
 * \param [in] is_mirror -
 *   If non zero then update DBAL_FIELD_HP_MIRR_RATE in HW (HP_MIRR_TRAFFIC_SHAPER_CONFIGURATION)
 *   Else update DBAL_FIELD_RCY_RATE (RECYLED_TRAFFIC_SHAPER_CONFIGURATION)
 * \param [in] kbits_sec_speed -
 *   Required rate, in kbps.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_cosq_gport_bandwidth_set
 */
shr_error_e dnx_egr_queuing_recycle_bandwidth_set(
    int unit,
    int core,
    int is_mirror,
    uint32 kbits_sec_speed);
/**
 * \brief - gets bandwidth for egress recycle channel (mirror/recycle)
 *
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core -
 *   Core to retrieve HW info for.
 * \param [in] is_mirror -
 *   If non zero then access DBAL_FIELD_HP_MIRR_RATE in HW (HP_MIRR_TRAFFIC_SHAPER_CONFIGURATION)
 *   Else access DBAL_FIELD_RCY_RATE (RECYLED_TRAFFIC_SHAPER_CONFIGURATION)
 * \param [out] kbits_sec_speed_p -
 *   This procedure loads pointed memory by channel rate in kbps
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_cosq_gport_bandwidth_get
 */
shr_error_e dnx_egr_queuing_recycle_bandwidth_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *kbits_sec_speed_p);
/**
 * \brief -
 *   Sets burst size for egress recycle channel (mirror/recycle)
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core -
 *   Core to use for HW access.
 * \param [in] is_mirror -
 *   If non-zero, channel is mirror recycle port
 *   Else, channel is plain recycle port
 * \param [out] burst -
 *   Required channel burst size, in bytes
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_cosq_control_set
 */
shr_error_e dnx_egr_recycle_burst_set(
    int unit,
    int core,
    int is_mirror,
    uint32 burst);
/**
 * \brief -
 *   Gets burst size for egress recycle channel (mirror/recycle)
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core -
 *   Core to use for HW access.
 * \param [in] is_mirror -
 *   If non-zero, channel is mirror recycle port
 *   Else, channel is plain recycle port
 * \param [out] burst -
 *   This procedure loads pointed memory by channel burst size, in bytes
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_cosq_control_get
 */
shr_error_e dnx_egr_recycle_burst_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *burst);
/**
 * \brief -
 *   Sets weight for EIR traffic on egress recycle channel (Either mirror or recycle)
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core -
 *   Core to use for HW access.
 * \param [in] is_mirror -
 *   If non-zero, channel is WFQ mirror recycle port
 *   Else, channel is WFQ plain recycle port
 * \param [in] weight -
 *   If zero then disable traffic, as specified in 'is_mirror'.
 *   Else, enable this type of traffic and set its weight as specified.
 *     In that case, weight can be in the range 1 - 255.
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_cosq_gport_sched_set (GPORT_IS_RCY/GPORT_IS_RCY_MIRR) 
 */
shr_error_e dnx_egr_recycle_weight_set(
    int unit,
    int core,
    int is_mirror,
    uint32 weight);
/**
 * \brief -
 *   Gets weight for EIR traffic on egress recycle channel (Either mirror or recycle)
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core -
 *   Core to use to retrieve HW info from.
 * \param [in] is_mirror -
 *   If non-zero, channel is WFQ mirror recycle port
 *   Else, channel is WFQ plain recycle port
 * \param [out] weight -
 *   This procedure loads pointed memory by channel weight. Range is 1-255.
 *   If loaded by '0' then traffic is disabled on this channel.
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_recycle_weight_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *weight);
/**
 * \brief - gets port to egress recycle interface mapping
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [out] rcy_if_idx - recycle interface index
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_port_recycle_interface_map_get(
    int unit,
    bcm_port_t port,
    int *rcy_if_idx);
/**
 * \brief - egr_queuing implementation nif credit init
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_nif_credit_default_set(
    int unit,
    bcm_port_t port);

/**
 * \brief - sets the IRDY & txq_max_bytes threshold per speed. The value retrieved from dnx_data
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_if_thr_set(
    int unit,
    bcm_port_t port);

/**
 * \brief - configure in HW if the egress interface is belong to sub calendar or not, according to the interface speed.
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_if_sub_calendar_set(
    int unit,
    bcm_port_t port);

/**
 * \brief -
 *   Based on setup of rate of master ports, load interface calendars for
 *   both FQP and PQP.
 *
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] core -
 *   Core to use to access HW.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect input is the 'rate' setup for all master ports (assigned per interface).
 *     See dnx_algo_port_interface_rate_get()
 * \see
 *   * dnx_egr_queuing_nif_calendar_set
 */
shr_error_e dnx_egr_queuing_nif_calendar_set(
    int unit,
    bcm_core_t core);
/* 
 * \brief -
 *   This procedure is called, within the sequence of adding a port, to
 *   set 'packet mode' for an interface port (and for all ports under
 *   this interface).
 *   Calling this procedure is not compulsory. If not called, default
 *   value is used.
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] logical_port -
 *   bcm_port_t. Identifier of the interface to initialize (using a contained port).
 * \param [in] arg -
 *   int. Value to set for 'packet mode'. May be '1' (if 'packet mode'
 *   is required) of '0' (standard bit mode).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If 'packet mode' is required then shaping is done in units of
 *     full packets.
 *   * This procedure is only meaningful for 'egress TM' ports. It is
 *     assumed here that a validity check has been carried out by the
 *     caller.
 * \see
 *   * dnx_egr_queuing_port_add_handle
 *   * port_add_remove_sequence[]
 *   * bcm_dnx_port_add
 */
shr_error_e dnx_egr_queuing_interface_packet_mode_set(
    int unit,
    bcm_port_t logical_port,
    int arg);
/* 
 * \brief -
 *   This procedure is called to get 'packet mode' for an interface port
 *   (and for all ports under this interface).
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] logical_port -
 *   bcm_port_t. Identifier of the interface to inquire (via contained logical port).
 * \param [out] arg -
 *   Pointer to int. This procedure loads pointed memory by current value
 *   of 'packet mode'. May be '1' (if 'packet mode' is active) of '0'
 *   (standard bit mode).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If 'packet mode' is set then shaping is done in units of
 *     full packets.
 *   * This procedure is only meaningful for 'egress TM' ports. It is
 *     assumed here that a validity check has been carried out by the
 *     caller.
 * \see
 *   * dnx_egr_queuing_port_add_handle
 *   * port_add_remove_sequence[]
 *   * bcm_dnx_port_add
 *   * dnx_egr_queuing_interface_packet_mode_set
 */
shr_error_e dnx_egr_queuing_interface_packet_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *arg);

/*
 * \brief -
 *   This procedure is called, within the sequence of adding a port, to
 *   set 'packet mode' for a logical port
 *   Calling this procedure is not compulsory. If not called, default
 *   value is used.
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] logical_port -
 *   bcm_port_t. Identifier of the logical port.
 * \param [in] arg -
 *   int. Value to set for 'packet mode'. May be '1' (if 'packet mode'
 *   is required) of '0' (standard bit mode).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If 'packet mode' is required then shaping is done in units of
 *     full packets.
 *   * This procedure is only meaningful for 'egress TM' ports. It is
 *     assumed here that a validity check has been carried out by the
 *     caller.
 * \see
 */
shr_error_e dnx_egr_queuing_port_packet_mode_set(
    int unit,
    bcm_port_t logical_port,
    int arg);
/*
 * \brief -
 *   This procedure is called to get 'packet mode' for a logical port
 * \param [in] unit -
 *   int. HW identifier of unit.
 * \param [in] logical_port -
 *   bcm_port_t. Identifier of the logical port.
 * \param [out] arg -
 *   Pointer to int. This procedure loads pointed memory by current value
 *   of 'packet mode'. May be '1' (if 'packet mode' is active) of '0'
 *   (standard bit mode).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * If 'packet mode' is set then shaping is done in units of
 *     full packets.
 *   * This procedure is only meaningful for 'egress TM' ports. It is
 *     assumed here that a validity check has been carried out by the
 *     caller.
 * \see
 *   * dnx_egr_queuing_port_packet_mode_set
 */
shr_error_e dnx_egr_queuing_port_packet_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *arg);

/* 
 * \brief -
 *   This procedure is called, within the sequence of adding a port, to
 *   carry out all initializations required which are related to EGQ.
 *   (This is, then, a step in bcm_dnx_port_add())
 * \param [in] unit -
 *   HW identifier of unit.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * port_add_remove_sequence (step "EgqQueuing")
 *   * bcm_dnx_port_add
 *   * bcm_port_add API
 *   * port_add_remove_sequence[]
 */
shr_error_e dnx_egr_queuing_port_add_handle(
    int unit);
/* 
 * \brief -
 *   This procedure is called, within the sequence of removing a port, to
 *   carry out all 'deinit' operations required which are related to EGQ.
 *   (This is, then, a step in bcm_dnx_port_remove())
 * \param [in] unit -
 *   HW identifier of unit.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * port_add_remove_sequence (step "EgqQueuing")
 *   * bcm_dnx_port_remove
 *   * bcm_port_remove API
 *   * port_add_remove_sequence[]
 */
shr_error_e dnx_egr_queuing_port_remove_handle(
    int unit);
/**
 * \brief -
 *   Given logical port (corresponding to OTM port), set mode of operation:
 *   SP before WFQ or the other way round. 
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operation is to be done.
 * \param [in] enable -
 *   uint32. If non-zero then SP is done before WFQ (per OTM port). Otherwise,
 *   WFQ is done before SP.
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_prio_over_cast_set(
    int unit,
    bcm_port_t logical_port,
    int enable);
/**
 * \brief -
 *   Given logical port (corresponding to OTM port), get mode of operation:
 *   SP before WFQ or the other way round. 
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operation is to be done.
 * \param [in] enable_p -
 *   Pointer to uint32. This procedure loads pointed memory by a non-zero value if
 *   SP is done before WFQ (per OTM port). Otherwise, a zero value is loaded.
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_prio_over_cast_get(
    int unit,
    bcm_port_t logical_port,
    int *enable_p);
/*
 * \brief -
 *   enable/disable port qpairs. Flush option is available for disable mode only.
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operations are to be done.
 * \param [in] enable -
 *   uint32. Flag.
 *   If TRUE then set CGM HW to NOT return a 'disable' indication
 *     on this 'base qpair' and all its corresponding 'priority' qpairs.
 *   Else
 *     A 'disable' indication will be returned for all specified qpairs.
 *   Also, if 'enable' is FALSE then see 'flush' below.
 * \param [in] flush -
 *   uint32. Flag. Only meaningful if 'enable' is FALSE. So, if 'enable'/'flush'
 *   are set to FALSE/TRUE then the following is carried out:
 *     * Temporarily diconnect port
 *     * * Disable port shapers.
 *     * * Set egress port to ignore flow control.
 *     * Flush: Make sure that the otm port has no unicast packet left.
 *     * Flush: Make sure that the otm port has no multicast packet left.
 *     * Connect port
 *     * * Set egress port to not ignore flow control
 *     * * Enable port shapers.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Note that it is assumed that, on entry, flow control is active and port
 *     shapers are active!
 *   * In case of internal failure, flow control and port shaper are returned back to
 *     active state.
 * \see
 *   * dnx_egr_queuing_port_fc_enable_set
 *   * dnx_egr_queuing_port_disable
 */
shr_error_e dnx_egr_queuing_port_enable_and_flush_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable,
    uint32 flush);
/**
 * \brief - get the port qpairs enable status.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [out] enable -  enable status
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_port_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);
/**
 * \brief - configure egq pqp counter parameters.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [in] count_by -  indicate count by qp, hr or egq if
 * \param [in] count_place -  could be qp, hr_id or egq_if, which is decided by count_by.
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_pqp_counter_configuration_set(
    int unit,
    int core,
    int count_by,
    int count_place);
/**
 * \brief - configure egq rqp counter parameters.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [in] flag -  Flag
 * \param [in] count_by -  indicate count by qp, hr or egq if
 * \param [in] count_place -  could be qp, hr_id or egq_if, which is decided by count_by.
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_epni_counter_configuration_set(
    int unit,
    int core,
    int flag,
    int count_by,
    int count_place);

/**
 * \brief - disable egress queue port and flush it to make sure that the queue is empty
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * Called from port_remove sequence
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_port_disable(
    int unit);
/**
 * \brief - egr_queuing module init
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_init(
    int unit);
/**
 * \brief - egr_queuing module deinit
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_deinit(
    int unit);
/**
 * \brief - allocate egress interface
 *
 * \param [in] unit -  Unit ID
 * \param [in] port -  logical port
 * \param [in] master_logical_port -  master port in case channelized or DNX_ALGO_PORT_INVALID otherwise
 * \param [in] core - core ID
 * \param [in] interface_type -  interface type of port
 * \param [in] channelized_if -  if true, allocate from the pool of channelized interfaces.
 * \param [out] egr_if -  egress interface ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_interface_alloc(
    int unit,
    bcm_port_t port,
    int master_logical_port,
    bcm_core_t core,
    bcm_port_if_t interface_type,
    int channelized_if,
    int *egr_if);
/* 
 * \brief -
 *   Sets egress queue priority per traffic class and drop
 *   precedence.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] core -
 *   Core to use when accessing HW.
 * \param [in] map_type_ndx -
 *   dnx_egr_q_prio_mapping_type_e. Mapping type, defining what traffic type
 *   (unicast/multicast) is mapped to what egress queue type
 *   (scheduled/unscheduled).
 *   May be:
 *     DNX_EGR_UCAST_TO_SCHED
 *     DNX_EGR_MCAST_TO_UNSCHED
 * \param [in] tc_ndx -
 *   uint32. Traffic Class, as embedded in the packet header. Range:
 *   0 - 7.
 * \param [in] tc_ndx -
 *   uint32. Drop Precedence, as embedded in the packet header.
 *   Range: 0 - 3.
 * \param [in] map_profile_ndx -
 *   uint32. Identifier of the profile to store in HW. Range: 0 - 3.
 * \param [in] priority -
 *   Pointer to dnx_egr_q_priority_t. TC and DP to select for selected
 *   egress queue.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Each OFP is mapped to an egress queue profile by the
 *     dnx_egr_q_profile_map_set API.
 *   * Indirect output is into HW, table PQP_TC_DP_MAP
 * \see
 *   * dnx_egq_tc_dp_map_tbl_set
 */
shr_error_e dnx_egr_queuing_prio_set(
    int unit,
    int core,
    dnx_egr_q_prio_mapping_type_e map_type_ndx,
    uint32 tc_ndx,
    uint32 dp_ndx,
    uint32 map_profile_ndx,
    dnx_egr_q_priority_t * priority);
/*********************************************************************
*     Gets the configuration set by the
 *     "dnx_egr_q_prio_set" API.
 *     Refer to "dnx_egr_q_prio_set" API for details.
 */
shr_error_e dnx_egr_queuing_prio_get(
    int unit,
    int core,
    dnx_egr_q_prio_mapping_type_e map_type_ndx,
    uint32 tc_ndx,
    uint32 dp_ndx,
    uint32 map_profile_ndx,
    dnx_egr_q_priority_t * priority);
/* 
 * \brief -
 *   Given a TM port, set its TC_MAP profile.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] core_id -
 *   Core to use when accessing HW.
 * \param [in] tm_port -
 *   uint32. The port to set the profile for.
 * \param [in] map_profile_id -
 *   uint32. Traffic Class map profile. Range:
 *   0 - 7.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_cosq_gport_egress_queue_map_set
 */
shr_error_e dnx_egr_queuing_profile_map_set(
    int unit,
    int core_id,
    uint32 tm_port,
    uint32 map_profile_id);
/*********************************************************************
 *     Gets the configuration set by the
 *     "dnx_egr_q_profile_map_set" API.
 *     Refer to "dnx_egr_q_profile_map_set" API for
 *     details.
 */
int dnx_egr_queuing_profile_map_get(
    int unit,
    int core_id,
    uint32 pp_port,
    uint32 *map_profile_id);
/* 
 * \brief -
 *   Given a TM port, set its Priority mode (Essentially number of
 *   priorities: One of 1,2,4,8)
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] core_id -
 *   Core to use when accessing HW.
 * \param [in] logical_port -
 *    bcm_port_t. Logical port index to configure.
 * \param [in] priority_mode -
 *   dnx_egr_port_priority_mode_e. Priority mode to
 *   assign to this port
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_cosq_gport_egress_queue_map_set
 */
int dnx_egr_queuing_port_prio_mode_set(
    int unit,
    int core_id,
    bcm_port_t logical_port,
    dnx_egr_port_priority_mode_e priority_mode);
/*********************************************************************
 * is_high_priority indicates if port is high or low  priority
 */
/* 
 * \brief -
 *   Given 'logical_port' and 'priority' (cosq) on this port, indicate
 *   whether this qpair combination is marked as 'high priority' on attached NIF.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] core -
 *   Core to use when accessing HW.
 * \param [in] logical_port -
 *   Logical port to identify the corresponding 'base qpair' and,
 *   consequently, the qpair to investigate.
 * \param [in] cosq -
 *   uint32. Priority to use, together with 'base qpair' to indicate
 *   the qpair to investigate.
 * \param [out] is_high_priority -
 *   Pointer to int. This procedure loads pointed memory by indication
 *   on whether 'logical_port', on this 'priority', is marked as SP (TRUE)
 *   or not (FALSE).
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_cosq_gport_egress_scheduler_queue_sched_get
 *   * dnx_cosq_control_egress_port_tc_get
 */
shr_error_e dnx_egr_queuing_is_high_priority_queue_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 cosq,
    int *is_high_priority);
/**
 * \brief
 *   Schedule mode for Channelized NIF port and CPU interface.
 *   Set value of Strict priority to be used by this system. SP range: H/L
 *   Given 'gport' to identify a specific base qpair, set it priority
 *   as specified in 'mode' (high or low).
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'. Together with
 *   'cosq' the exact target qpair can be identified ('target qpair' = 'base qpair' plus
 *   'priority')
 * \param [in] cosq -
 *   bcm_cos_queue_t. Must be zero. (All priorities are set)
 * \param [in] mode -
 *   int. Priority to assign to interface corresponding to indicated qpair. Can only
 *   be high (BCM_COSQ_SP0) or low (BCM_COSQ_SP1).
 *   See dnx_egq_dbal_gport_scheduler_queue_sched_set().
 * \param [out] weight -
 *   int. Ignored by this procedure.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_QP_CFG, field QP_IS_HP
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, field DBAL_FIELD_QP_IS_HP
 * \see
 *   * dnx_cosq_gport_egress_scheduler_port_sched_get
 *   * dnx_cosq_gport_egress_scheduler_queue_sched_set
 */
shr_error_e dnx_egr_queuing_port_is_high_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);
/*
 * Purpose:
 * Schedule mode for Channelized NIF port and CPU interface.
 * Choose Strict priority to Connect to. SP range: H/L
 */
/**
 * \brief
 *   Schedule mode for Channelized NIF port and CPU interface.
 *   Get value of Strict priority to be used by this system. SP range: H/L
 *   Given 'gport' to identify a specific base qpair, Get its priority
 *   into 'mode' (high or low).
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] gport -
 *   bcm_gport_t. Gport used to identify the target 'base qpair'. Together with
 *   'cosq' the exact target qpair can be identified ('target qpair' = 'base qpair' plus
 *   'priority')
 * \param [in] cosq -
 *   bcm_cos_queue_t. Must be zero. (All priorities are set)
 * \param [out] mode -
 *   Pointer to int. This procedure loads pointed memory by priority assigned to
 *   interface corresponding to indicated qpair. Can only be high (BCM_COSQ_SP0) 
 *   or low (BCM_COSQ_SP1).
 *   See dnx_egq_dbal_gport_scheduler_queue_sched_set().
 * \param [out] weight -
 *   Pointer to int. Ignored by this procedure.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Accessing HW table EPS_QP_CFG, field QP_IS_HP
 *   * Accessing DBAL table DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, field DBAL_FIELD_QP_IS_HP
 * \see
 *   * dnx_cosq_gport_egress_scheduler_port_sched_get
 *   * dnx_cosq_gport_egress_scheduler_queue_sched_set
 */
shr_error_e dnx_egr_queuing_port_is_high_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);
/* 
 * \brief -
 *   Set new profile for egress queue mapping
 *     Given 'profile' and 'mapping info' load HW with specified new information.
 *     'profile' is a sw/hw identifier of the mapping info.
 *     'Mapping_info' indicates TC and DP to assign to unicast or multicast
 *     traffic which enters egress processing with specific TC and DP.
 *     The mapping is, then, from traffic type (unicast/multicast) plus TC and DP
 *     to new TC and DP combination.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] core -
 *   Core to use when accessing HW.
 * \param [in] profile -
 *   int. Identifier of profile (in SW data base) with indicated mapping details. range:
 *   4 bits.
 * \param [in] mapping_info -
 *   dnx_cosq_egress_queue_mapping_info_t. Structure fully loaded by info indicating
 *   all mapping combinations as explained above.
 *   All entries of the three dimensional array are assumed to have been loaded
 *   by the caller.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_egr_queuing_prio_set
 *   * dnx_cosq_egq_gport_egress_queue_map_set
 */
shr_error_e dnx_egr_queuing_profile_mapping_set(
    int unit,
    int core,
    int profile,
    const dnx_cosq_egress_queue_mapping_info_t * mapping_info);

/*********************************************************************
 *     Get calendar id from interface id. 
 *     Calender id is the interface id
 *     Except ifc ids, which do not part of the channelized pool. 
 *     For them, All goes to last calender
 */

/* 
 * \brief -
 *     Get calendar id from interface id. 
 *     Calender id is the interface id
 *     Except ifc ids, which do not part of the channelized pool. 
 *     For them, All goes to last calender
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] logical_port -
 *   logical port
 * \param [in] channelized -
 *   egress interface channelized 
 * \param [out] calendar_id -
 *   calendar id calculated by the function
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnx_egr_queuing_calendar_index_get(
    int unit,
    bcm_port_t logical_port,
    int *calendar_id);

/**
 * \brief - update total credit size for all interfaces, and verify we do not cross the capacity of ESB buffer size
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - logical port
 * \param [in] speed -  new speed
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_total_egr_if_credits_update(
    int unit,
    bcm_port_t logical_port,
    int speed);

/**
 * \brief -
 *   Given rate, in Mega Bits Per Second, find the number of blocks of
 *   12.5G that are required to contain it and, then, find the lowest power
 *   of '2' which contain this number. See 'remark' below.
 *
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] rate -
 *   Pointer to 'uint32'. Rate, in Mega Bits Per Second, to convert as per
 *   'brief' above. See 'remark' below .
 * \param [out] weight_p -
 *   Pointer to uint32. This procedure loads pointed memory as described in
 *   'brief' above. If 'rate' is '0' then '0' is loaded.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Examples:
 *       rate is '1'. The number of blocks is '1' and the calculated
 *       weight is '1'.
 *       rate is '12,500'. The number of blocks is '1' and the calculated
 *       weight is '1'.
 *       rate is '12,501'. The number of blocks is '2' and the calculated
 *       weight is '2'.
 *       rate is '25,001'. The number of blocks is '3' and the calculated
 *       weight is '4'.
 * \see
 *   * dnx_egr_q_nif_cal_set
 */
int dnx_egr_queuing_nif_rate_to_weight(
    int unit,
    uint32 rate,
    uint32 *weight_p);

/**
 * \brief
 *  Calculate the parameters for building a MUX calendar.
 *
 * \param [in] unit     - unit id
 * \param [in] weights - array of weights for each object in the calendar.
 * \param [in] max_cal_len - maximal possible calendar length
 * \param [out] cal_len_optimal - optimal calendar length
 * \param [out] nof_cal_objects - nof objects in calendar
 * \param [out] dummy_start - the point where actual entries end and padding begins
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_cal_params_calc(
    int unit,
    uint32 *weights,
    uint32 max_cal_len,
    uint32 *cal_len_optimal,
    uint32 *nof_cal_objects,
    uint32 *dummy_start);

/**
 * \brief
 *  convert HW egress interface type to bcm interface type.
 *  Any HW interface that does not belong to one of the special interfaces, will be threated as NIF_ETH
 *
 * \param [in] unit     - unit id
 * \param [in] hw_egr_if - hw egress interface, as defined in dnx_data_egr_queuing.params.
 * \param [out] bcm_egr_if - the converted value, as defined in bcm layer.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_hw_egr_if_convert(
    int unit,
    int hw_egr_if,
    bcm_port_if_t * bcm_egr_if);

/**
 * \brief -
 *   Enable/disable interface shaper correlated with specified port.
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operations are to be done.
 * \param [in] enable -
 *   uint32. Flag. If TRUE then enable interface shaper. Else, disable.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Operation is carried out only if general flag, indicating whether
 *     all inreface shapers are enabled (in HW), is asserted.
 *   * Specific interface is controlled via HW register
 *     EPS_EGRESS_CALENDAR_SHAPER_SETTINGS (field IFC_SPR_DIS)
 * \see
 *   * dnx_egr_queuing_port_all_shapers_enable_set
 *   * dnx_algo_port_egr_if_get
 */
shr_error_e dnx_egr_queuing_interface_shaper_enable_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable);

/* } */

/** } */
#endif /* EGR_QUEUING_H_INCLUDED */

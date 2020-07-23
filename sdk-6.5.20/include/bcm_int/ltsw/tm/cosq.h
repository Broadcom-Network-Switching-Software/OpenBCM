/* \file cosq.h
 *
 * COSQ headfiles to declare internal APIs for TM devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_TM_COSQ_H
#define BCMINT_LTSW_TM_COSQ_H

#include <bcm/types.h>
#include <bcm/cosq.h>

#include <bcm_int/ltsw/cosq_int.h>
#include <bcm_int/ltsw/cosq.h>

#include <sal/sal_types.h>

typedef enum {
    LTSW_SCHED_MODE_STRICT = 0,
    LTSW_SCHED_MODE_WRR,
    LTSW_SCHED_MODE_WERR
} tm_ltsw_sched_mode_e;

typedef enum {
    LTSW_COSQ_NODE_LVL_L0 = 0,
    LTSW_COSQ_NODE_LVL_UC,
    LTSW_COSQ_NODE_LVL_MC,
    LTSW_COSQ_NODE_LVL_MAX
} tm_ltsw_cosq_node_lvl_e;

/*
 * \brief cosq WRED congestion profile.
 */
typedef struct tm_ltsw_cosq_wred_cng_profile_s {
    /* Q average. */
    uint8_t q_avg;

    /* Q min. */
    uint8_t q_min;

    /* Service pool. */
    uint8_t sp;

    /* Action. */
    uint8_t action;
} tm_ltsw_cosq_wred_cng_profile_t;

typedef struct tm_ltsw_cosq_wred_config_s {
    uint32_t flags;

    /* Queue depth in bytes to begin dropping. */
    int min_thresh;

    /* Queue depth in bytes to drop all packets. */
    int max_thresh;

    /* Drop probability at max threshold. */
    int drop_probability;

    /* Determines the smoothing that should be applied. */
    int gain;

    /* Queue depth in bytes to stop marking and start dropping. */
    int ecn_thresh;

    /* Actual average refresh time. */
    int refresh_time;

    /* Use QGroupMin instead of QMin for WRED resolution. */
    uint32_t use_queue_group;

    /* Profile of discard rules. */
    uint32_t profile_id;

    /* Need Profile. */
    bool need_profile;

    /* Ignore_enable_flags. */
    int ignore_enable_flags;
} tm_ltsw_cosq_wred_config_t;

typedef struct tm_ltsw_cosq_cos_map_profile_s {
    /* Unicast COS queue value. */
    int uc_cos;

    /* Unicast COS queue value for elephant flow. */
    int uc_elephant_cos;

    /* Multicast COS queue value. */
    int mc_cos;

    /* Replication queue engine (RQE) COS queue value. */
    int rqe_cos;

    /* CPU COS queue value. */
    int cpu_cos;
} tm_ltsw_cosq_cos_map_profile_t;

/* scheduler profile queue info */
typedef struct tm_ltsw_cosq_scheduler_profile_q_info_s {
    int num_ucq;      /* Number of unicast queues. Valid values are 0-2 */
    int num_mcq;      /* Number of multicast queues. Valid values are 0-1 */
    int ucq_id[2];    /* Unicast queue IDs.  */
    int mcq_id;       /* Multicast queue ID */
} tm_ltsw_cosq_scheduler_profile_q_info_t;

typedef struct tm_ltsw_cosq_strength_profile_s {

    /* strength profile table index.*/
    uint16_t strength_profile_id;

    /* unicast cos queue strength. */
    uint8_t uc_cos_strength;

    /* multicast cos queue strength. */
    uint8_t mc_cos_strength;

    /* replication queue engine (rqe) cos queue strength. */
    uint8_t rqe_cos_strength;

    /* cpu cos queue strength. */
    uint8_t cpu_cos_strength;
} tm_ltsw_cosq_strength_profile_t;

typedef struct tm_ltsw_q_assignment_profile_s {

    /* index to queue assignment profile table. */
    const char *tm_q_assignment_profile_id;

    /* port_cos_q_map profile base index. */
    uint16_t base_port_cos_q_map_id;

    /* fp_ing_cos_q_map profile base index. */
    uint16_t base_fp_ing_cos_q_map_id;

    /* fp_destination_cos_q_map profile base index. */
    uint16_t base_fp_destination_cos_q_map_id;

    /* port_cos_q_strength_profile table index. */
    uint8_t port_cos_q_strength_profile_id;

    /* fp_ing_cos_q_strength_profile table index. */
    uint8_t fp_ing_cos_q_strength_profile_id;

    /* tm_cos_q_cpu_strength_profile table index. */
    uint8_t tm_cos_q_cpu_strength_profile_id;

    /* fp_destination_cos_q_strength_profile table index. */
    uint8_t fp_destination_cos_q_strength_profile_id;

    /* enable to use multicast cos from mirroring. */
    bool mc_cos_mirror;
} tm_ltsw_q_assignment_profile_t;

/* cut-through properties of front-panel port */
typedef struct tm_ltsw_cut_through_port_info_s {

    /* cut-through source port max-speed (mbps). */
    uint32_t src_port_max_speed;

    /* cut-through class for the port. */
    int cut_through_class;

    /* specifies the maximum credits in cells. */
    uint16_t max_credit_cells;

    /* specifies the transmit start count in bytes. */
    uint16_t egr_xmit_start_count_bytes[8];

    /* cut-through fifo threshold in cells. */
    uint32_t fifo_thd_cells;
} tm_ltsw_cut_through_port_info_t;

/*
 * \brief Get scheduler node type in symbol
 *
 * \param [in] unit      Unit Number.
 * \param [in] node_type node type.
 * \param [out] sym_val  node type in symbol.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*cosq_sched_node_type_get_t) (
    int unit,
    int node_type,
    const char **sym_val);

/*
 * \brief Get scheduler mode in symbol
 *
 * \param [in] unit      Unit Number.
 * \param [in/out] sched_mode sched mode.
 * \param [in/out] sym_val  node type in symbol.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*cosq_sched_mode_get_t) (
    int unit,
    int *sched_mode,
    const char **sym_val,
    bool get_sym);

/*
 * \brief Convert HW percent value to drop probability
 *
 * \param [in]  percent      percent.
 *
 * \retval rate              rate.
 */
typedef const char* (*cosq_wred_percent_to_drop_prob_t)(
    int percent);


/*
 * \brief Convert HW drop probability to percent value
 *
 * \param [in]  rate      rate.
 *
 * \retval percent        percent.
 */
typedef int (*cosq_wred_drop_prob_to_percent_t)(
    const char *rate);


/*
 * \brief Convert us to time domain
 *
 * \param [in]  us           us.
 *
 * \retval time_domain       time_domain.
 */
typedef const char* (*cosq_wred_us_to_time_domain_t)(
    int us);

/*
 * \brief Convert time domain to us
 *
 * \param [in]  time_domain      time_domain.
 *
 * \retval us        us.
 */
typedef int (*cosq_wred_time_domain_to_us_t)(
    const char *time_domain);

/*
 * \brief Convert ct mode to class
 *
 * \param [in]  ct_mode      ct_mode.
 *
 * \retval ct_class        ct_class.
 */
typedef int (*port_ct_mode_to_class_t)(
    const char *ct_mode);

/*
 * \brief Cosq chip driver structure.
 */
typedef struct tm_ltsw_cosq_chip_driver_s {

    /* Get scheduler node type in symbol */
    cosq_sched_node_type_get_t cosq_sched_node_type_get;

    /* Get scheduler mode in symbol */
    cosq_sched_mode_get_t cosq_sched_mode_get;

    /* Convert HW percent value to drop probability */
    cosq_wred_percent_to_drop_prob_t cosq_wred_percent_to_drop_prob;

    /* Convert HW drop probability to percent value */
    cosq_wred_drop_prob_to_percent_t cosq_wred_drop_prob_to_percent;

    /* Convert us to time domain */
    cosq_wred_us_to_time_domain_t cosq_wred_us_to_time_domain;

    /* Convert time domain to us */
    cosq_wred_time_domain_to_us_t cosq_wred_time_domain_to_us;

    /* Convert ct mode to class */
    port_ct_mode_to_class_t port_ct_mode_to_class;

} tm_ltsw_cosq_chip_driver_t;

/*
 * \brief Cosq Driver callback registration
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Driver pointer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_cosq_chip_driver_register(int unit, tm_ltsw_cosq_chip_driver_t* drv);

/*
 * \brief Cosq Driver callback deregistration
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_cosq_chip_driver_deregister(int unit);

/*
 * \brief Cosq Driver get.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_cosq_chip_driver_get(int unit, tm_ltsw_cosq_chip_driver_t **drv);

/*
 * \brief Initialize cosq module information
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_init(int unit);

/*
 * \brief Destroy cosq module information
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_detach(int unit);

/*
 * \brief Initialize cosq hierarchy information
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_hierarchy_init(int unit);

/*
 * \brief Schedule profile queue info get.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    Profile id.
 * \param [in]  node_id       Cos.
 * \param [out]  num_uc        Number of uc.
 * \param [out]  num_mc        Number of mc.
 * \param [out]  mcq_id        mc queue id.
 * \param [out]  ucq_id0       uc queue id.
 * \param [out]  ucq_id1       uc queue id.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
tm_ltsw_schedule_profile_queue_info_get(
    int unit,
    int profile_id,
    int node_id,
    tm_ltsw_cosq_scheduler_profile_q_info_t *profile);

/*
 * \brief Cosq schedq mapping set
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_index Profile index.
 * \param [in] size Array size.
 * \param [in] cosq_mapping_arr Cosq to queue mapping for scheduler.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_schedq_mapping_set(
    int unit,
    int profile_index,
    int size,
    bcm_cosq_mapping_t *cosq_mapping_arr);

/*
 * \brief cosq schedq mapping get
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_index Profile index.
 * \param [in] array_max Max size.
 * \param [out] cosq_mapping_arr Cosq to queue mapping for scheduler.
 * \param [out] size Array size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_schedq_mapping_get(
    int unit,
    int profile_index,
    int array_max,
    bcm_cosq_mapping_t *cosq_mapping_arr,
    int *size);

/*
 * \brief Set Port to Profile Mapping for Cosq profiles
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] profile_id Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_port_scheduler_profile_set(
    int unit,
    bcm_port_t port,
    int profile_id);

/*
 * \brief Get Port to Profile Mapping for Cosq profiles
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] profile_id Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_port_scheduler_profile_get(
    int unit,
    bcm_port_t port,
    int *profile_id);

/*
 * \brief Function to Set Parent(L0) for a given L1 queue of the CPU port
 *
 * \param [in] unit Unit Number.
 * \param [in] child_index child node index.
 * \param [in] child_level child node level.
 * \param [in] parent_index parent node index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_cpu_parent_set(
    int unit,
    int child_index,
    int child_level,
    int parent_index);

/*
 * \brief Function to Get Parent(L0) for a given L1 queue of the CPU port
 *
 * \param [in] unit Unit Number.
 * \param [in] child_index child node index.
 * \param [in] child_level child node level.
 * \param [out] parent_index parent node index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_cpu_parent_get(
    int unit,
    int child_index,
    int child_level,
    int *parent_index);

/*
 * \brief Retrieve a cos to CMC channel state.
 *
 * \param [in] unit Unit Number.
 * \param [in] input_cos input queue id.
 * \param [in] channel_id channel id.
 * \param [out] enable enable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_rx_queue_channel_get(
    int unit,
    int input_cos,
    int channel_id,
    bool *enable);

/*
 * \brief Associate a cos to CMC channel
 *
 * \param [in] unit Unit Number.
 * \param [in] input_cos input queue id.
 * \param [in] channel_id channel id.
 * \param [in] enable enable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_rx_queue_channel_set(
    int unit,
    int input_cos,
    int channel_id,
    bool enable);

/*
 * \brief Set port node scheduling mode and weight
 *
 * \param [in] unit Unit Number.
 * \param [in] lport Logic port number.
 * \param [in] node_id Node id.
 * \param [in] node_type Node type.
 * \param [in] sched_mode sched mode.
 * \param [in] weight weight.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_sched_set(
    int unit,
    int lport,
    int node_id,
    int node_type,
    int sched_mode,
    int weight);

/*
 * \brief Get port node scheduling mode and weight
 *
 * \param [in] unit Unit Number.
 * \param [in] lport Logic port number.
 * \param [in] node_id Node id.
 * \param [in] node_type Node type.
 * \param [out] sched_mode sched mode.
 * \param [out] weight weight.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_sched_get(
    int unit,
    int lport,
    int node_id,
    int node_type,
    int *sched_mode,
    int *weight);

/*
 * \brief cosq wred set
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] cosq cosq.
 * \param [in] wred_config wred config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_wred_set(
    int unit,
    bcm_port_t port,
    int cosq,
    tm_ltsw_cosq_wred_config_t wred_config);

/*
 * \brief cosq wred get
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] cosq cosq.
 * \param [out] wred_config wred config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_wred_get(
    int unit,
    bcm_port_t port,
    int cosq,
    tm_ltsw_cosq_wred_config_t *wred_config);

/*
 * \brief cosq wred cng profile set
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_idx Profile index.
 * \param [in] profile wred config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_wred_cng_profile_set(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_wred_cng_profile_t profile);

/*
 * \brief cosq wred cng profile get
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_idx Profile index.
 * \param [out] profile wred config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_wred_cng_profile_get(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_wred_cng_profile_t *profile);

/*
 * \brief Add default entry into LT TM_WRED_CNG_NOTIFICATION_PROFILE.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_idx Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_wred_cng_profile_reset(
    int unit,
    int profile_idx);

/*
 * \brief Displays time domain profile reference count
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_time_domain_dump(int unit);

/*
 * \brief Add a profile into LT PORT_COS_Q_MAP
 *
 * \param [in] unit Unit Number.
 * \param [in] cos_map_entries cos map profile.
 * \param [out] profile_idx Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_cos_map_profile_add(
    int unit,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries,
    int *profile_idx);

/*
 * \brief Get a profile from LT PORT_COS_Q_MAP
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_idx Profile index.
 * \param [out] cos_map_entries cos map profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_cos_map_profile_get(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries);

/*
 * \brief Delete a profile from LT PORT_COS_Q_MAP
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_idx Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_cos_map_profile_delete(
    int unit,
    int profile_idx);

/*
 * \brief Initialize the LT PORT_COS_Q_MAP
 *
 * \param [in] unit Unit Number.
 * \param [in] numq number cos.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_cos_map_profile_init(
    int unit,
    int numq);

/*
 * \brief Get shaper configure from LT TM_SHAPER_CONFIG
 *
 * \param [in] unit Unit Number.
 * \param [out] refresh_time Refresh time.
 * \param [out] itu_mode itu mode enable.
 * \param [out] shaper Shaper enable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_shaper_config_get(
    int unit,
    int *refresh_time,
    int *itu_mode,
    int *shaper);

/*
 * \brief Set shaper configure for LT TM_SHAPER_NODE
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] node_id Node id.
 * \param [in] node_type Node type.
 * \param [in] shaper_mode Shaper mode.
 * \param [in] kbits_sec_min kbits_sec_min.
 * \param [in] kbits_sec_max kbits_sec_max.
 * \param [in] burst_size_auto Enables automatic calculation of burst size.
 * \param [in] burst_min Configured size of maximum burst traffic.
 * \param [in] burst_max Configured size of maximum burst traffic.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_shaper_node_set(
    int unit,
    bcm_port_t port,
    int node_id,
    int node_type,
    int shaper_mode,
    uint32_t kbits_sec_min,
    uint32_t kbits_sec_max,
    bool burst_size_auto,
    uint32_t burst_min,
    uint32_t burst_max);

/*
 * \brief Get shaper configure from LT TM_SHAPER_NODE
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] node_id Node id.
 * \param [in] node_type Node type.
 * \param [out] shaper_mode Shaper mode.
 * \param [out] kbits_sec_min kbits_sec_min.
 * \param [out] kbits_sec_max kbits_sec_max.
 * \param [in] burst_size_auto Enables automatic calculation of burst size.
 * \param [out] burst_min Configured size of maximum burst traffic.
 * \param [out] burst_max Configured size of maximum burst traffic.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_shaper_node_get(
    int unit,
    bcm_port_t port,
    int node_id,
    int node_type,
    int *shaper_mode,
    uint32_t *kbits_sec_min,
    uint32_t *kbits_sec_max,
    bool burst_size_auto,
    uint32_t *burst_min,
    uint32_t *burst_max);

/*
 * \brief Initialize the LT FP_ING_COS_QUEUE_MAPs.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ifp_cos_map_profile_init(int unit);

/*
 * \brief  Create a cosq classifier.
 *
 * \param [in] unit Unit Number.
 * \param [out] classifier_id Classifier ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_field_classifier_id_create(
    int unit,
    int *classifier_id);

/*
 * \brief Free resource associated with this field classifier id.
 *
 * \param [in] unit Unit Number.
 * \param [in] classifier_id Classifier ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_field_classifier_id_destroy(
    int unit,
    int classifier_id);

/*
 * \brief Add a profile into LT FP_ING_COS_Q_MAP
 *
 * \param [in] unit Unit Number.
 * \param [in] cos_map_entries cos map profile.
 * \param [out] profile_idx Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ifp_cos_map_profile_add(
    int unit,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries,
    int *profile_idx);

/*
 * \brief Get a profile from LT FP_ING_COS_Q_MAP
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_idx Profile index.
 * \param [out] cos_map_entries cos map profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ifp_cos_map_profile_get(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries);

/*
 * \brief Delete a profile from LT FP_ING_COS_Q_MAP
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_idx Profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ifp_cos_map_profile_delete(
    int unit,
    int profile_idx);

/*
 * \brief Delete an internal priority to ingress field processor CoS queue override mapping profile set.
 *
 * \param [in] unit Unit Number.
 * \param [in] classifier_id Classifier ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_field_classifier_map_clear(
    int unit,
    int classifier_id);

/*
 * \brief  Create a cosq classifier.
 *
 * \param [in] unit Unit Number.
 * \param [out] classifier_id Classifier ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_field_hi_classifier_id_create(
    int unit,
    int *classifier_id);

/*
 * \brief Free resource associated with this field classifier id.
 *
 * \param [in] unit Unit Number.
 * \param [in] classifier_id Classifier ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_field_hi_classifier_id_destroy(
    int unit,
    int classifier_id);

/*
 * \brief set a profile into LT FP_ING_COS_Q_MAP_HI
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_idx Profile index.
 * \param [in] cos_map_entries cos map profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ifp_cos_map_hi_profile_set(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries);

/*
 * \brief Get a profile from LT FP_ING_COS_Q_MAP_HI
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_idx Profile index.
 * \param [out] cos_map_entries cos map profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ifp_cos_map_hi_profile_get(
    int unit,
    int profile_idx,
    tm_ltsw_cosq_cos_map_profile_t *cos_map_entries);

/*
 * \brief Clear LT FP_ING_COS_Q_MAP_HI
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ifp_cos_map_hi_clear(
    int unit);

/*
 * \brief  MC queue mode get.
 *
 * \param [in] unit Unit Number.
 * \param [out] mc_q_mode MC queue mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_mcq_mode_get(
    int unit,
    int *mc_q_mode);

/*
 * \brief Get port map information
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] info port map information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_port_map_info_get(
    int unit,
    bcm_port_t port,
    bcmi_ltsw_cosq_port_map_info_t *info);

/*
 * \brief Get device specific TM information
 *
 * \param [in] unit Unit Number.
 * \param [out] info Device specific TM information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_device_info_get(
    int unit,
    bcmi_ltsw_cosq_device_info_t *info);

/*
 * \brief Set cut-through mode for a specific port
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] enable Enable to use cut-through mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_port_ct_mode_set(
    int unit,
    bcm_port_t port,
    int enable);

/*
 * \brief Get cut-through mode for a specific port
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] enable Enable to use cut-through mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_port_ct_mode_get(
    int unit,
    bcm_port_t port,
    int *enable);

/*
 * \brief TM_CUT_THROUGH_PORT_INFOs get.
 *
 * \param [in]  unit    Unit Number.
 * \param [in]  port    Logical port ID.
 * \param [out] info    tm_ltsw_cut_through_port_info_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_tm_cut_through_port_info_get(
    int unit,
    int lport,
    tm_ltsw_cut_through_port_info_t *info);

/*
 * \brief Init PORT_COS_Q_STRENGTH_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_port_cosq_strength_profile_init(
    int unit,
    tm_ltsw_cosq_strength_profile_t *profile);

/*
 * \brief Init FP_ING_COS_Q_STRENGTH_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_ifp_cosq_strength_profile_init(
    int unit,
    tm_ltsw_cosq_strength_profile_t *profile);

/*
 * \brief Init FP_DESTINATION_COS_Q_STRENGTH_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_destination_cosq_strength_profile_init(
    int unit,
    tm_ltsw_cosq_strength_profile_t *profile);

/*
 * \brief Init TM_COS_Q_CPU_STRENGTH_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cpu_cosq_strength_profile_init(
    int unit,
    tm_ltsw_cosq_strength_profile_t *profile);

/*
 * \brief Init TM_Q_ASSIGNMENT_PROFILE table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_queue_assignment_profile_init(
    int unit,
    tm_ltsw_q_assignment_profile_t *profile);

/*
 * \brief tm_ltsw_mc_cos_mirror_set.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_mc_cos_mirror_set(
    int unit,
    tm_ltsw_q_assignment_profile_t *profile);

/*
 * \brief tm_ltsw_mc_cos_mirror_get.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_mc_cos_mirror_get(
    int unit,
    tm_ltsw_q_assignment_profile_t *profile);

#endif /* BCMINT_LTSW_TM_COSQ_H */

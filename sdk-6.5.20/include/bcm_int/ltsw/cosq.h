/*! \file cosq.h
 *
 * COSQ internal management header file.
 * This file contains the management for COSQ module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_COSQ_H
#define BCMI_LTSW_COSQ_H

#include <bcm/cosq.h>
#include <sal/sal_types.h>

/*
 *  port map information.
 */
typedef struct bcmi_ltsw_cosq_port_map_info_s {

    /* Physical port number. */
    int phy_port;

    /* Pipe number. */
    int pipe;

    /* TM pipe-local port number. */
    int local_port;

    /*
     * TM sparse global port number.
     * The discrete global port number to index ports within TM.
     */
    int mmu_port;

    /*
     * TM compact global port number.
     * The continuous global port number based on TM pipe-local port number and pipe offsets.
     */
    int global_port;

    /* Unicast queue number base. */
    int uc_base;

    /* Number of Unicast Queues. */
    int num_uc_q;

    /* Multicast queue number base. */
    int mc_base;

    /* Number of Multicast Queues. */
    int num_mc_q;

} bcmi_ltsw_cosq_port_map_info_t;

/*
 * Device specific TM information.
 */
typedef struct bcmi_ltsw_cosq_device_info_s {

    /* Maximum packet size in bytes. */
    int  max_pkt_byte;

    /* Packet header size in bytes. */
    int  mmu_hdr_byte;

    /* Jumbo packet size in bytes. */
    int  jumbo_pkt_size;

    /* Default MTU size in bytes. */
    int  default_mtu_size;

    /* Number of Cells (TM buffer memory size). */
    int  mmu_total_cell;

    /* Size of single buffer cell in bytes. */
    int  mmu_cell_size;

    /* Number of supported port-pg instances. */
    int  num_pg;

    /* Number of supported service pools. */
    int  num_service_pool;

    /*
     * Maximum number of unicast and multicast queues supported for
     * non-cpu ports.
     */
    int  num_queue;

    /*
     * Maximum number of queues supported for the CPU port.
     * The CPU port supports only multicast queues.
     */
    int  num_cpu_queue;

    /* Number of pipes. */
    int  num_pipe;

    /* Number of itms. */
    int  num_itm;

    /* Number of schedler profile. */
    int  num_sched_profile;

    /* Number of internal priority */
    int  num_int_pri;

    /* Number of internal priority */
    int  num_rqe_queue;

    /* Number of scheduler node */
    int  num_sched_node;

    /* Number of uc queue for non-cpu port */
    int  num_ucq;

    /* Number of mc queue for non-cpu port */
    int  num_mcq;
} bcmi_ltsw_cosq_device_info_t;

/*
 * \brief Get port map information.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lport         Logic port number.
 * \param [out] info          ltsw_cosq_port_map_info_t.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_port_map_info_get(int unit, bcm_port_t lport,
                                 bcmi_ltsw_cosq_port_map_info_t *info);

/*
 * \brief Get device specific TM information.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  info         bcmi_ltsw_cosq_device_info_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_device_info_get(int unit, bcmi_ltsw_cosq_device_info_t *info);

/*
 * \brief Retrieve number UC&MC queue according to multcast queue mode.
 *
 * \param [in]  unit          Unit number.
 * \param [out] num_ucq       unicast queue number.
 * \param [out] num_mcq       multcast queue number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_num_queue_get(int unit, int *num_ucq, int *num_mcq);

/*
 * \brief Resolve GPORT if it is a local port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT identifier.
 * \param [out] local_port    local port number.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_localport_resolve(int unit, bcm_gport_t gport,
                                 bcm_port_t *local_port);

/*
 * \brief Get port number and queue or scheduler identifier for the specified GPORT .
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT identifier
 * \param [out] port          Port number
 * \param [out] id            Queue or scheduler identifier.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_gport_port_resolve(int unit, bcm_gport_t gport,
                                  bcm_port_t *port, int *id);

/*!
 * \brief Validate input parameters of ingress threshold.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          port ID.
 * \param [in/out]  itm           itm.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_port_itm_validate(int unit, int port, int *itm);

/*
 * \brief Displays COS Queue information maintained by software.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern void
bcmi_ltsw_cosq_sw_dump(int unit);

/*
 * \brief COS queue bandwidth control bucket setting.
 *        If port is any form of local port, cosq is the hardware queue index.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  gport           Port number or GPORT identifier.
 * \param [in]  cosq            COS queue number.
 * \param [in]  min_quantum     kbps or packet/second.
 * \param [in]  max_quantum     kbps or packet/second.
 * \param [in]  burst_size_auto Enables automatic calculation of burst size.
 * \param [in]  kbits_burst_min Configured size of maximum burst traffic.
 * \param [in]  kbits_burst_max Configured size of maximum burst traffic.
 * \param [in]  flags           flags.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_bucket_set(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                          uint32_t min_quantum, uint32_t max_quantum, bool burst_size_auto,
                          uint32_t kbits_burst_min, uint32_t kbits_burst_max,
                          uint32_t flags);

/*
 * \brief Get COS queue bandwidth control bucket setting.
 *        If port is any form of local port, cosq is the hardware queue index.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   gport           Port number or GPORT identifier.
 * \param [in]   cosq            COS queue number.
 * \param [out]  min_quantum     kbps or packet/second.
 * \param [in]   max_quantum     kbps or packet/second.
 * \param [out]  burst_size_auto Enables automatic calculation of burst size.
 * \param [out]  kbits_burst_min Configured size of maximum burst traffic.
 * \param [out]  kbits_burst_max Configured size of maximum burst traffic.
 * \param [out]  flags           flags.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_bucket_get(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                          uint32_t *min_quantum, uint32_t *max_quantum, bool burst_size_auto,
                          uint32_t *kbits_burst_min, uint32_t *kbits_burst_max,
                          uint32_t *flags);

/*
 * \brief Get number of COSQs configured for this port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          Local port.
 *
 * \retval Per port cos num.
 */
extern int
bcmi_ltsw_cosq_port_num_cos(int unit, int port);

/*
 * \brief Get Function to return the number of child nodes and child node gport
 *        array for scheduler gport.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  sched_gport   scheduler gport.
 * \param [out] num_uc_child  number of uc child.
 * \param [out] uc_queue      uc queue array.
 * \param [out] num_mc_child  number of mc child.
 * \param [out] mc_queue      mc queue array.
 *
 * \retval Per port cos num.
 */
extern int
bcmi_ltsw_cosq_sched_node_child_get(int unit, bcm_gport_t sched_gport,
                                    int *num_uc_child, int *uc_queue,
                                    int *num_mc_child, int *mc_queue);

/* Shaper refresh timer. */
typedef enum bcmi_ltsw_sharper_refresh_time_e {
    REFRESH_TIME_INVALID = -1,
    /* Refresh time 3.90625 us. */
    REFRESH_TIME_3_90625_US = 0,
    /* Refresh time 7.8125 us. */
    REFRESH_TIME_7_8125_US = 1
} bcmi_ltsw_sharper_refresh_time_t;

/*
 * \brief Get device-level shaper configuration properties.
 *
 * \param [in]  unit          Unit Number.
 * \param [out] refresh_time  Refresh time. NULL to ignore.
 * \param [out] itu_mode      Itu mode status. NULL to ignore
 * \param [out] shaper        Shaper status. NULL to ignore
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_cosq_shaper_config_get(
    int unit,
    bcmi_ltsw_sharper_refresh_time_t *refresh_time,
    int *itu_mode,
    int *shaper);

/*
 * \brief Clear cosq configuration of port.
 *
 * \param [in]  unit        Unit Number.
 * \param [in]  port        Logical port ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_cosq_port_detach(int unit, bcm_port_t port);

/*
 * \brief Initialize Cosq for newly added port.
 *
 * \param [in]  unit        Unit Number.
 * \param [in]  port        Logical port ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_cosq_port_attach(int unit, bcm_port_t port);

/*
 * \brief Enable to use multicast COS from mirroring.
 *
 * \param [in] unit Unit Number.
 * \param [in] enable Enable
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_cosq_mc_cos_mirror_set(
    int unit,
    int enable);

/*
 * \brief Enable to use multicast COS from mirroring.
 *
 * \param [in] unit Unit Number.
 * \param [out] enable Enable status
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_cosq_mc_cos_mirror_get(
    int unit,
    int *enable);

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
bcmi_ltsw_port_ct_mode_set(
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
bcmi_ltsw_port_ct_mode_get(
    int unit,
    bcm_port_t port,
    int *enable);

/*
 * \brief Dump per-port asf configurations to diag Shell.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  Logical port number.
 * \param [in]  port_speed            Port speed in MB/s.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_port_asf_config_dump(
    int unit,
    bcm_port_t port,
    int port_speed);

/*
 * \brief Print per-port traffic forwarding mode in diag Shell.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  Logical port number.
 * \param [in]  port_speed            Port speed in MB/s.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_port_asf_config_show(
    int unit,
    bcm_port_t port,
    int port_speed);

/*
 * \brief Get the ifp cos map profile id from classifier id.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   classifier_id       Classifier ID.
 * \param [out]  flag                BCM_COSQ_CLASSIFIER_NO_INT_PRI.
 * \param [out]  index               ifp cos map profile id.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_classifier_field_get(
    int unit,
    int classifier_id,
    int *flag,
    int *index);

/*
 * \brief Set the classifier id for give ifp cos map profile id.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   index               ifp cos map profile id.
 * \param [in]   flag                BCM_COSQ_CLASSIFIER_NO_INT_PRI.
 * \param [out]  classifier_id       Classifier ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_cosq_classifier_field_set(
    int unit,
    int index,
    int flag,
    int *classifier_id);

/*
 * \brief Get the associated rx channel with a given cosq
 *
 * \param [in] unit Unit Number.
 * \param [in] queue_id input queue id
 * \param [in] chan_id channel index
 * \param [out] enable enable state
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_rx_queue_channel_get(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t chan_id,
    bool *enable);

/*
 * \brief Assign a RX channel to a cosq
 *
 * \param [in] unit Unit Number.
 * \param [in] queue_id input queue id
 * \param [in] chan_id channel index
 * \param [in] enable enable state
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_rx_queue_channel_set(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t chan_id,
    bool enable);

/*
 * \brief Tests whether the corresponding COS_BMP bit is set or not
 *
 * \param [in] unit Unit Number.
 * \param [in] queue_id input queue id
 * \param [in] chan_id channel index
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_rx_queue_channel_set_test(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t chan_id);

#endif /* BCMI_LTSW_COSQ_H */

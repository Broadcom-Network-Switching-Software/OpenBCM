/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_OOB_H__
#define __BCM_OOB_H__

#include <bcm/types.h>

#define BCM_OOB_FC_TX_FCN_EN    0x00000001 /* Fast Congestion Notification
                                              Enable Flag */
#define BCM_OOB_FC_TX_ING_EN    0x00000002 /* OOB FC Tx Ingress Congestion
                                              Notification Enable Flag */
#define BCM_OOB_FC_TX_EGR_EN    0x00000004 /* OOB FC Tx Egress Congestion
                                              Notification Enable Flag */
#define BCM_OOB_FC_TX_POOL_EN   0x00000008 /* OOB FC Tx Pool Congestion
                                              Notification Enable Flag */

/* 
 * Enum that controls the way the egress queues congetion state will be
 * reported via OOB FC Tx
 */
typedef enum bcm_oob_egress_mode_e {
    bcmOOBFCTxReportUCOnly = 0,     /* OOB FC Tx only reports unicast queues
                                       congestion state only. */
    bcmOOBFCTxReportMCOnly = 1,     /* OOB FC Tx only reports multicast queues
                                       congestion state only. */
    bcmOOBFCTxReportCos = 2,        /* OOB FC Tx only reports Cos queues level
                                       congestion state. */
    bcmOOBFCTxReportUCAndMC = 3,    /* OOB FC Tx only reports both UC and MC
                                       queues congestion state. */
    bcmOOBFCTxReportCount = 4       /* Must be last. */
} bcm_oob_egress_mode_t;

/* OOB FC TX global configuration settings. */
typedef struct bcm_oob_fc_tx_config_s {
    uint32 flags;                   /* Enable flags */
    uint8 inter_pkt_gap;            /* Inter Packet Gap in Clock cycles */
    bcm_service_pool_id_t gcs_id;   /* Global Congestion Service Pool ID */
    bcm_oob_egress_mode_t egr_mode; /* Egress Congestion Reporting Mode */
} bcm_oob_fc_tx_config_t;

/* OOB FC TX Egress Queue configuration settings. */
typedef struct bcm_oob_fc_tx_queue_config_s {
    bcm_cos_queue_t cosq;   /* UC or MC queue number */
    uint8 queue_enable;     /* Enable/disable queue congestion reporting */
    uint32 queue_offset;    /* Queue bit offset within OOBFC message */
} bcm_oob_fc_tx_queue_config_t;

/* Initializing OOB Flow Control Tx Queue configuration structure */
extern void bcm_oob_fc_tx_queue_config_t_init(
    bcm_oob_fc_tx_queue_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set / get OOB Flow control Tx Queue configuration profile */
extern int bcm_oob_fc_tx_queue_profile_set(
    int unit, 
    int profile_id, 
    int count, 
    bcm_oob_fc_tx_queue_config_t *config);

/* Set / get OOB Flow control Tx Queue configuration profile */
extern int bcm_oob_fc_tx_queue_profile_get(
    int unit, 
    int profile_id, 
    int max_count, 
    bcm_oob_fc_tx_queue_config_t *config, 
    int *count);

/* Set / Get logical port to OOB port mapping(s) */
extern int bcm_oob_fc_tx_port_mapping_set(
    int unit, 
    int count, 
    int *port_array, 
    int *oob_port_array);

/* Set / Get logical port to OOB port mapping(s) */
extern int bcm_oob_fc_tx_port_mapping_get(
    int unit, 
    int max_count, 
    int *port_array, 
    int *oob_port_array, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initializing OOB Flow Control Tx global configuration structure */
extern void bcm_oob_fc_tx_config_t_init(
    bcm_oob_fc_tx_config_t *config);

/* OOB FC TX Channel info. */
typedef struct bcm_oob_fc_tx_info_s {
    uint8 ing_base;     /* Ingress Channel Base */
    uint8 ucast_base;   /* Unicast Queue Channel Base */
    uint8 mcast_base;   /* Multicast Queue Channel Base */
    uint8 cos_base;     /* Cos Queue Channel Base */
    uint8 pool_base;    /* Pool Channel Base */
} bcm_oob_fc_tx_info_t;

/* Initializing OOB Flow Control Tx global information */
extern void bcm_oob_fc_tx_info_t_init(
    bcm_oob_fc_tx_info_t *info);

/* OOB FC RX Interface Configuration. */
typedef struct bcm_oob_fc_rx_config_s {
    uint8 channel_base; /* Channel Base of the first HCFC packet for OOB FC Rx
                           Interface */
    int enable;         /* enable=1 (Enable) and enable=0 (Disable) OOB FC Rx
                           Interface */
} bcm_oob_fc_rx_config_t;

/* Initializes the configuration for given OOB Flow Control Rx Interface. */
extern void bcm_oob_fc_rx_config_t_init(
    bcm_oob_fc_rx_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Configures the OOB Flow Control Tx global configuration */
extern int bcm_oob_fc_tx_config_set(
    int unit, 
    bcm_oob_fc_tx_config_t *config);

/* Configures the OOB Flow Control Tx global configuration */
extern int bcm_oob_fc_tx_config_get(
    int unit, 
    bcm_oob_fc_tx_config_t *config);

/* Retrieve the OOB Flow Control Tx global information */
extern int bcm_oob_fc_tx_info_get(
    int unit, 
    bcm_oob_fc_tx_info_t *info);

/* 
 * Multiple set of OOB Flow Control Rx interface side traffic class to
 * priority mapping
 */
extern int bcm_oob_fc_rx_port_tc_mapping_multi_set(
    int unit, 
    bcm_oob_fc_rx_intf_id_t intf_id, 
    bcm_gport_t gport, 
    int array_count, 
    uint32 *tc, 
    uint32 *pri_bmp);

/* 
 * Multiple get OOB Flow Control Rx interface side traffic class to
 * priority mapping
 */
extern int bcm_oob_fc_rx_port_tc_mapping_multi_get(
    int unit, 
    bcm_oob_fc_rx_intf_id_t intf_id, 
    bcm_gport_t gport, 
    int array_max, 
    uint32 *tc, 
    uint32 *pri_bmp, 
    int *array_count);

/* 
 * Set OOB Flow Control Rx interface side traffic class to priority
 * mapping
 */
extern int bcm_oob_fc_rx_port_tc_mapping_set(
    int unit, 
    bcm_oob_fc_rx_intf_id_t intf_id, 
    bcm_gport_t gport, 
    uint32 tc, 
    uint32 pri_bmp);

/* 
 * Get OOB Flow Control Rx interface side traffic class to priority
 * mapping
 */
extern int bcm_oob_fc_rx_port_tc_mapping_get(
    int unit, 
    bcm_oob_fc_rx_intf_id_t intf_id, 
    bcm_gport_t gport, 
    uint32 tc, 
    uint32 *pri_bmp);

/* Set the configuration for given OOB Flow Control Rx Interface. */
extern int bcm_oob_fc_rx_config_set(
    int unit, 
    bcm_oob_fc_rx_intf_id_t intf_id, 
    bcm_oob_fc_rx_config_t *config, 
    int array_count, 
    bcm_gport_t *gport_array);

/* Get the configuration for given OOB Flow Control Rx Interface. */
extern int bcm_oob_fc_rx_config_get(
    int unit, 
    bcm_oob_fc_rx_intf_id_t intf_id, 
    bcm_oob_fc_rx_config_t *config, 
    int array_max, 
    bcm_gport_t *gport_array, 
    int *array_count);

/* 
 * Get the flow control byte number of a port on a given OOB Flow Control
 * Rx interface.
 */
extern int bcm_oob_fc_rx_port_offset_get(
    int unit, 
    bcm_oob_fc_rx_intf_id_t intf_id, 
    bcm_gport_t gport, 
    uint32 *offset);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_OOB_STATS_TIMESTAMP_EN  0x00000001 /* Enable Flag for reporting
                                                  Timestamp */
#define BCM_OOB_STATS_POOL_EN       0x00000002 /* Enable Flag for reporting
                                                  Instantaneous service pool
                                                  size */
#define BCM_OOB_STATS_QUEUE_EN      0x00000004 /* Enable Flag for reporting
                                                  Instantaneous UC Queue size */

#define BCM_OOB_STATS_INGRESS   0x01       /* Ingress side resource */
#define BCM_OOB_STATS_EGRESS    0x02       /* Egress side resource */

/* OOB Stats global configuration settings. */
typedef struct bcm_oob_stats_config_s {
    uint32 flags;           /* Enable flags */
    uint8 inter_pkt_gap;    /* Inter Packet Gap in Clock cycles */
    uint8 config_id;        /* Global Config ID */
} bcm_oob_stats_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the OOB Stats global configuration parameters */
extern int bcm_oob_stats_config_set(
    int unit, 
    bcm_oob_stats_config_t *config);

/* Get the OOB Stats global configuration parameters */
extern int bcm_oob_stats_config_get(
    int unit, 
    bcm_oob_stats_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the OOB Stats global configuration parameters. */
extern void bcm_oob_stats_config_t_init(
    bcm_oob_stats_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Multiple set of OOB Stats service pool list configuration */
extern int bcm_oob_stats_pool_mapping_multi_set(
    int unit, 
    int array_count, 
    int *offset_array, 
    uint8 *dir_array, 
    bcm_service_pool_id_t *pool_array);

/* Multiple get of OOB Stats service pool list configuration */
extern int bcm_oob_stats_pool_mapping_multi_get(
    int unit, 
    int array_max, 
    int *offset_array, 
    uint8 *dir_array, 
    bcm_service_pool_id_t *pool_array, 
    int *array_count);

/* Set of OOB Stats service pool list configuration */
extern int bcm_oob_stats_pool_mapping_set(
    int unit, 
    int offset, 
    uint8 dir, 
    bcm_service_pool_id_t pool);

/* Get of OOB Stats service pool list configuration */
extern int bcm_oob_stats_pool_mapping_get(
    int unit, 
    int offset, 
    uint8 *dir, 
    bcm_service_pool_id_t *pool);

/* Multiple set of OOB Stats Unicast queue list configuration */
extern int bcm_oob_stats_queue_mapping_multi_set(
    int unit, 
    int array_count, 
    int *offset_array, 
    bcm_gport_t *gport_array);

/* Multiple get of OOB Stats Unicast queue list configuration */
extern int bcm_oob_stats_queue_mapping_multi_get(
    int unit, 
    int array_max, 
    int *offset_array, 
    bcm_gport_t *gport_array, 
    int *array_count);

/* Set of OOB Stats Unicast queue list configuration */
extern int bcm_oob_stats_queue_mapping_set(
    int unit, 
    int offset, 
    bcm_gport_t gport);

/* Get of OOB Stats Unicast queue list configuration */
extern int bcm_oob_stats_queue_mapping_get(
    int unit, 
    int offset, 
    bcm_gport_t *gport);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_OOB_H__ */

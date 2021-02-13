/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_SUM_H__
#define __BCM_SUM_H__

#include <bcm/types.h>

/* SUM counter types */
typedef enum bcm_sum_type_e {
    bcmSumTypeTxPktCnt = 0,         /* SUM monitors Tx packet counters. */
    bcmSumTypeRxPktCnt = 1,         /* SUM monitors Rx packet counters. */
    bcmSumTypeTxByteCnt = 2,        /* SUM monitors Tx byte counters. */
    bcmSumTypeRxByteCnt = 3,        /* SUM monitors Rx byte counters. */
    bcmSumTypeTxPktByteCnt = 4,     /* SUM monitors Tx packet, Tx byte counters. */
    bcmSumTypeRxPktByteCnt = 5,     /* SUM monitors Rx packet, Rx byte counters. */
    bcmSumTypeRxTxPktCnt = 6,       /* SUM monitors Tx/Rx packet counters. */
    bcmSumTypeRxTxByteCnt = 7,      /* SUM monitors Tx/Rx byte counters. */
    bcmSumTypeTxRxPktByteCnt = 8,   /* SUM monitors all Tx packet, Tx byte, Rx
                                       packet, Rx byte counters. */
    bcmSumTypeMax = 9               /* Maximum Counter Type. */
} bcm_sum_type_t;

/* SUM stats information types */
typedef enum bcm_sum_stat_type_e {
    bcmSumPeakUsageInfo = 0,    /* SUM Peak usage information. */
    bcmSumMinimumUsageInfo = 1, /* SUM Minimum usage information. */
    bcmSumUsageInfo = 2         /* SUM user requested number, of usage history. */
} bcm_sum_stat_type_t;

/* SUM Configuration information. */
typedef struct bcm_sum_config_s {
    uint32 usec_sample_interval;    /* sample data interval */
    uint32 max_history;             /* maximum samples number of history */
    bcm_sum_type_t sum_type;        /* sum type */
} bcm_sum_config_t;

/* SUM stats data. */
typedef struct bcm_sum_stat_info_s {
    uint64 sequence_num;        /* This is running global number update for
                                   every entry for unique identification. */
    uint64 time_stamp;          /* Sample time stamp. */
    uint64 tx_pkt_count;        /* Tx packet count. */
    uint64 rx_pkt_count;        /* Rx packet count. */
    uint64 tx_byte_count;       /* Tx byte count. */
    uint64 rx_byte_count;       /* Rx byte count. */
    bcm_sum_type_t sum_type;    /* sum type */
} bcm_sum_stat_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initializes sum subsystem. */
extern int bcm_sum_init(
    int unit);

/* Detaches sum subsystem. */
extern int bcm_sum_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a config information structure. */
extern void bcm_sum_config_t_init(
    bcm_sum_config_t *config_data);

/* Initialize SUM usage stats information structure */
extern void bcm_sum_stat_info_t_init(
    bcm_sum_stat_info_t *stat_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set a Switch Utilization Monitor configuration information. */
extern int bcm_sum_config_set(
    int unit, 
    bcm_sum_config_t *config_data, 
    bcm_pbmp_t pbmp);

/* Get a Switch Utilization Monitor Configuration information. */
extern int bcm_sum_config_get(
    int unit, 
    bcm_sum_config_t *config_data, 
    bcm_pbmp_t *pbmp);

/* Get a Switch Utilization Monitor Statics information. */
extern int bcm_sum_stat_get(
    int unit, 
    uint32 max_samples, 
    bcm_sum_stat_info_t *stats_data, 
    uint32 *num_samples, 
    bcm_sum_stat_type_t stat_type, 
    uint32 usec_sample_period);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_SUM_H__ */

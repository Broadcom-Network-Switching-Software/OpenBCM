/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rx.h
 * Purpose:     Internal structures and definitions for RX module
 */

#ifndef   _BCM_RX_H_
#define   _BCM_RX_H_

#include <sdk_config.h>
#include <soc/dma.h>
#include <bcm/error.h>
#include <bcm/rx.h>
#include <bcm_int/common/rx.h>

/* Hardware values for different redirection configuration */
#define _BCM_RX_COPYTOCPU_BUFFER_PRIORITY_LOW    0x0
#define _BCM_RX_COPYTOCPU_BUFFER_PRIORITY_MEDIUM 0x1
#define _BCM_RX_COPYTOCPU_BUFFER_PRIORITY_HIGH   0x2

/* This is the tunnel header description */
#define TUNNEL_RX_HEADER_LEN_OFS             0    /* uint16 */
#define TUNNEL_RX_HEADER_REASON_OFS          2    /* uint32 */
#define TUNNEL_RX_HEADER_RX_UNIT_OFS         6
#define TUNNEL_RX_HEADER_RX_PORT_OFS         7
#define TUNNEL_RX_HEADER_RX_CPU_COS_OFS      8
#define TUNNEL_RX_HEADER_RX_UNTAGGED_OFS     9
#define TUNNEL_RX_HEADER_COS_OFS             10
#define TUNNEL_RX_HEADER_PRIO_INT_OFS        11
#define TUNNEL_RX_HEADER_SRC_PORT_OFS        12
#define TUNNEL_RX_HEADER_SRC_MOD_OFS         13
#define TUNNEL_RX_HEADER_DEST_PORT_OFS       14
#define TUNNEL_RX_HEADER_DEST_MOD_OFS        15
#define TUNNEL_RX_HEADER_OPCODE_OFS          16
#define TUNNEL_RX_HEADER_BYTES               17

/* Sync for copy-to-cpu config */
extern sal_mutex_t  _bcmi_rx_copy_to_cpu_config_mutex[BCM_MAX_NUM_UNITS];

/* Place holder to store the copy-to-cpu configuration data */
typedef struct _bcmi_rx_CopyToCpu_config_s {
    int flags;                          /* Flags to pass with configuration data */
    int index;                          /* Index of the entry */
    bcm_pkt_drop_reasons_t drop_reason_vector; /* Array of drop reasons */
    int strength;                       /* Strength of the entry */
    int buffer_priority;                /* Buffering priority of the redirected
                                           packet */
    int cpu_cosq;                       /* CPU CoS queue to use */
} _bcmi_rx_CopyToCpu_config_t;

/* Copy to CPU config data */
extern _bcmi_rx_CopyToCpu_config_t
    *(_bcmi_rx_egr_drop_copy_to_cpu_config_data[BCM_MAX_NUM_UNITS]);

/* Place holder to store the inuse bitmap for copy-to-cpu config */
extern SHR_BITDCL
    *(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[BCM_MAX_NUM_UNITS]);

/* Get accessors for the config data */
#define BCMI_GET_COPY_TOCPU_FLAGS_GET(unit, index) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index].flags

#define BCMI_GET_COPY_TOCPU_DROP_REASONS_GET(unit, index) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index]. \
                                            drop_reason_vector

#define BCMI_GET_COPY_TOCPU_STRENGTH_GET(unit, index) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index]. \
                                            strength

#define BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_GET(unit, index) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index]. \
                                            buffer_priority

#define BCMI_GET_COPY_TOCPU_COS_Q_GET(unit, index) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index]. \
                                            cpu_cosq

/* Set accessors for the config data */
#define BCMI_GET_COPY_TOCPU_FLAGS_SET(unit, index, flags_val) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index].flags \
                                            = flags_val

#define BCMI_GET_COPY_TOCPU_DROP_REASONS_SET(unit, index, drop_reasons_val) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index].drop_reason_vector \
                                            = drop_reasons_val

#define BCMI_GET_COPY_TOCPU_STRENGTH_SET(unit, index, strength_val) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index].strength \
                                            = strength_val

#define BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_SET(unit, index, buffer_priority_val) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index].buffer_priority \
                                            = buffer_priority_val

#define BCMI_GET_COPY_TOCPU_COS_Q_SET(unit, index, cpu_cosq_val) \
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][index].cpu_cosq \
                                            = cpu_cosq_val

/* Accessors to get size for the config data fields */
#define BCMI_GET_COPY_TOCPU_FLAGS_SIZE_GET(unit) \
        sizeof(_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][0].flags)

#define BCMI_GET_COPY_TOCPU_DROP_REASONS_SIZE_GET(unit) \
        sizeof(_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][0]. \
                                                    drop_reason_vector)

#define BCMI_GET_COPY_TOCPU_STRENGTH_SIZE_GET(unit) \
        sizeof(_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][0].strength)

#define BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_SIZE_GET(unit) \
        sizeof(_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][0]. \
                                                    buffer_priority)

#define BCMI_GET_COPY_TOCPU_COS_Q_SIZE_GET(unit) \
        sizeof(_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit][0].cpu_cosq)

/*
 * RX_COPYTO_CPU_LOCK/UNLOCK should be used to synchronize
 * Copy-to-Cpu configuration.
 */

#define RX_COPY_TO_CPU_LOCK(unit)        \
    sal_mutex_take(_bcmi_rx_copy_to_cpu_config_mutex[unit], \
    sal_mutex_FOREVER)
#define RX_COPY_TO_CPU_UNLOCK(unit)      \
    sal_mutex_give(_bcmi_rx_copy_to_cpu_config_mutex[unit])

extern int _bcm_rx_shutdown(int unit);
extern int bcm_esw_rx_deinit(int unit);

extern int _bcm_esw_rx_olp_match_rule_set (int unit, int olp_match_rule);
extern int _bcm_esw_rx_olp_match_rule_get (int unit, int *olp_match_rule);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_rx_sync(int unit);
#endif

#ifdef BCM_TRX_SUPPORT
extern int _bcm_trx_rx_cosq_mapping_init(int unit);
#endif /* BCM_TRX_SUPPORT */

#if defined(INCLUDE_PKTIO)
extern int bcm_tomahawk3x_rx_deinit(int unit);
#endif /* INCLUDE_PKTIO */

#endif  /* !_BCM_RX_H_ */

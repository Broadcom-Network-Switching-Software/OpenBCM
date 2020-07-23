/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_INT_COSQ_H
#define _BCM_INT_COSQ_H

#include <bcm/cosq.h>

extern int bcm_esw_cosq_deinit(int unit);
extern int _bcm_esw_cosq_config_property_get(int unit);
extern int _bcm_esw_cpu_cosq_mapping_default_set(int unit, int numq);
extern int _bcm_esw_cosq_num_get(int unit, int *num_cos);

/* Constant definitions for Katana */

#define BCM_KA_SCHEDULER_CNT                    9
#define BCM_KA_MCAST_QUEUE_GROUP_CNT            4
#define BCM_KA_TOTAL_UCAST_QUEUE_GROUP_CNT      12
#define BCM_KA_REG_UCAST_QUEUE_GROUP_CNT        8
#define BCM_KA_EXT_UCAST_QUEUE_GROUP_MIN        8

typedef enum _bcm_cosq_op_e {
    _BCM_COSQ_OP_SET = 1,
    _BCM_COSQ_OP_ADD,
    _BCM_COSQ_OP_DELETE,
    _BCM_COSQ_OP_CLEAR,
    _BCM_COSQ_OP_GET,
    _BCM_COSQ_OP_COUNT
} _bcm_cosq_op_t;

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCMI_COSQ_WB_TAS_VERSION_1_0        SOC_SCACHE_VERSION(1, 0)
#define BCMI_COSQ_WB_SCACHE_PARTITION_TAS   (1)
extern int _bcm_esw_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_cosq_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#ifdef BCM_TRIDENT_SUPPORT
extern int _bcm_td_wred_mem_war(int unit);
#endif

/* The classifier_id used by bcm_cosq_classifier_* APIs
 * contains two fields, a 6-bit type field, followed by
 * a 26-bit value field.
 */
#define _BCM_COSQ_CLASSIFIER_TYPE_NONE      0
#define _BCM_COSQ_CLASSIFIER_TYPE_ENDPOINT  (1 << 0)
#define _BCM_COSQ_CLASSIFIER_TYPE_SERVICE   (1 << 1)
#define _BCM_COSQ_CLASSIFIER_TYPE_FIELD     (1 << 2)
#define _BCM_COSQ_CLASSIFIER_TYPE_MAX       _BCM_COSQ_CLASSIFIER_TYPE_FIELD

#define _BCM_COSQ_CLASSIFIER_TYPE_SHIFT     26
#define _BCM_COSQ_CLASSIFIER_TYPE_MASK      0x3f
#define _BCM_COSQ_CLASSIFIER_ENDPOINT_SHIFT 0
#define _BCM_COSQ_CLASSIFIER_ENDPOINT_MASK  0x3ffffff
#define _BCM_COSQ_CLASSIFIER_SERVICE_SHIFT  0
#define _BCM_COSQ_CLASSIFIER_SERVICE_MASK   0x3ffffff
#define _BCM_COSQ_CLASSIFIER_FIELD_SHIFT    (0)
#define _BCM_COSQ_CLASSIFIER_FIELD_MASK     (0x3ffffff)

/* Macros for increase/decrease share threshold limit */
#define _BCM_COSQ_INCREASE_SHARE_LIMIT      (1 << 0)
#define _BCM_COSQ_DECREASE_SHARE_LIMIT      (1 << 1)

#define _BCM_COSQ_CLASSIFIER_IS_SET(_classifier)                     \
        ((((_classifier) >> _BCM_COSQ_CLASSIFIER_TYPE_SHIFT) > 0) && \
         (((_classifier) >> _BCM_COSQ_CLASSIFIER_TYPE_SHIFT) <=      \
          _BCM_COSQ_CLASSIFIER_TYPE_MAX))

#define _BCM_COSQ_CLASSIFIER_IS_ENDPOINT(_classifier)          \
        (((_classifier) >> _BCM_COSQ_CLASSIFIER_TYPE_SHIFT) == \
         _BCM_COSQ_CLASSIFIER_TYPE_ENDPOINT)

#define _BCM_COSQ_CLASSIFIER_ENDPOINT_SET(_classifier, _endpoint)               \
        ((_classifier) = (_BCM_COSQ_CLASSIFIER_TYPE_ENDPOINT <<                 \
                          _BCM_COSQ_CLASSIFIER_TYPE_SHIFT) |                    \
                         (((_endpoint) & _BCM_COSQ_CLASSIFIER_ENDPOINT_MASK) << \
                          _BCM_COSQ_CLASSIFIER_ENDPOINT_SHIFT))

#define _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(_classifier)            \
        (((_classifier) >> _BCM_COSQ_CLASSIFIER_ENDPOINT_SHIFT) & \
         _BCM_COSQ_CLASSIFIER_ENDPOINT_MASK)

#define _BCM_COSQ_CLASSIFIER_IS_SERVICE(_classifier)          \
        (((_classifier) >> _BCM_COSQ_CLASSIFIER_TYPE_SHIFT) == \
         _BCM_COSQ_CLASSIFIER_TYPE_SERVICE)

#define _BCM_COSQ_CLASSIFIER_SERVICE_SET(_classifier, _id)               \
        ((_classifier) = (_BCM_COSQ_CLASSIFIER_TYPE_SERVICE <<                 \
                          _BCM_COSQ_CLASSIFIER_TYPE_SHIFT) |                    \
                         (((_id) & _BCM_COSQ_CLASSIFIER_SERVICE_MASK) << \
                          _BCM_COSQ_CLASSIFIER_SERVICE_SHIFT))

#define _BCM_COSQ_CLASSIFIER_SERVICE_GET(_classifier)            \
        (((_classifier) >> _BCM_COSQ_CLASSIFIER_SERVICE_SHIFT) \
            & _BCM_COSQ_CLASSIFIER_SERVICE_MASK)

#define _BCM_COSQ_CLASSIFIER_IS_FIELD(_classifier)                  \
            (((_classifier) >> _BCM_COSQ_CLASSIFIER_TYPE_SHIFT) == \
             _BCM_COSQ_CLASSIFIER_TYPE_FIELD)

#define _BCM_COSQ_CLASSIFIER_FIELD_SET(_classifier, _id)                    \
            ((_classifier) = (_BCM_COSQ_CLASSIFIER_TYPE_FIELD               \
                                << _BCM_COSQ_CLASSIFIER_TYPE_SHIFT) |       \
                                (((_id) & _BCM_COSQ_CLASSIFIER_FIELD_MASK)  \
                                << _BCM_COSQ_CLASSIFIER_FIELD_SHIFT))

#define _BCM_COSQ_CLASSIFIER_FIELD_GET(_classifier)                 \
            (((_classifier) >> _BCM_COSQ_CLASSIFIER_FIELD_SHIFT)    \
                & _BCM_COSQ_CLASSIFIER_FIELD_MASK)

/* Worst case time = 256k * 768 * 8 @ 1Gbps = 1.6 sec */
#define _BCM_COSQ_QUEUE_FLUSH_TIMEOUT_DEFAULT 1600000

/* Worst case time = 256k * 2880 *8 @ 1Gbps = 5.9 sec */
#define _BCM_COSQ_QUEUE_FLUSH_TIMEOUT_PACKING_DEFAULT 5900000

extern int bcmi_esw_cosq_handle_interrupt(int unit, bcm_cosq_event_type_t event,
                                          bcm_port_t port, bcm_cos_queue_t cosq);

#define QCM_MAX_GPORT_MONITOR    0x40
#define QCM_UC_MSG_TIMEOUT       5000000 /* 5 sec for FW response */
#define QCM_UC_STATUS_TIMEOUT    100     /* 100 usec for FW response */
typedef struct qcm_gport_port_queue_map_s {
    /* gport value */
    bcm_gport_t gport;

    /* calculate global queue based on physical port and local queue */
    uint32 global_num;

    /* Extract port number and local queue number of the port */
    uint16 pipe_num;
    uint16 port_num;
    uint16 queue_num;

    struct qcm_gport_port_queue_map_s *next;

} qcm_gport_port_queue_map_t;

typedef struct qcm_sram_memory_info_s {
    uint32 host_status;
    uint32 host_buf_start_addr;
    uint32 host_buf_size;
    uint32 total_num_gports;
    uint32 port_pipe_queue[(QCM_MAX_GPORT_MONITOR * 2)];
    uint32 uc_status;
    uint32 app_status[2];
    uint32 drop_table_index[4];
    uint32 egr_table_index;
    uint32 queues_per_pipe;
    uint32 queues_per_port;
    uint32 ports_per_pipe;
    uint32 num_pipes;
    uint32 num_counters_per_pipe;
    uint32 endianess;
} qcm_sram_memory_info_t;

#define SRAM_BASEADDR(unit)      (SOC_IS_TRIDENT3X(unit) ? 0x01200000 :  0x1b000000)
#define sram_offset(mem) ((size_t)((char *)&((qcm_sram_memory_info_t *)0)->mem - (char *)(qcm_sram_memory_info_t *)0))

#endif  /* !_BCM_INT_COSQ_H */

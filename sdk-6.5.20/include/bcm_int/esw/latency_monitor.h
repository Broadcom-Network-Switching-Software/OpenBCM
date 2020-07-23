/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        latency_monitor.h,
 * Purpose:     
 */
#ifndef _BCM_LATENCY_MONITOR_H_
#define _BCM_LATENCY_MONITOR_H_

#include <bcm/latency_monitor.h>

#define LATENCY_MONITOR_QUEUE_COUNT         12
#define LATENCY_MONITOR_BUCKET_COUNT        8
#define TIME_STEP_SET_COUNT                 10
#define COUNTERS_PER_STEP_SET               (LATENCY_MONITOR_QUEUE_COUNT * LATENCY_MONITOR_BUCKET_COUNT)
#define TIME_STEP_COUNTER_INCR              COUNTERS_PER_STEP_SET

#define LATENCY_LIMIT_QUANTA                64  /* Minimum value fo the latency is 64 nSec */
#define LATENCY_LIMIT_TABLE_SIZE            32
#define LATENCY_HISTOGRAM_BUCKET_COUNT      8
#define LATENCY_MONITOR_MAX_COUNT           4

#define LATENCY_MONITOR_SW_BUF_WIDTH        8   /* Width of Software ring buffer in Bytes */

#define LATENCY_MONITOR_STATE_IDLE          1
#define LATENCY_MONITOR_STATE_RUNNING       2
#define LATENCY_MONITOR_STATE_ERROR         3
#define LATENCY_MONITOR_STATE_CLEANUP       4

#define LATENCY_MONITOR_DEFAULT_FRAG_COUNT  2

#define LATENCY_MONITOR_MIN_TIME_STEP       100000  /* 100 mSec */

typedef struct  {
    uint8   pipe;
    uint32  *dbuf;
} _sbusdma_desc_handle_data_t;

typedef struct  _sbusdma_desc_handle_node_s{
   _sbusdma_desc_handle_data_t  *data;
   struct _sbusdma_desc_handle_node_s * next;
}_sbusdma_desc_handle_node_t;

typedef struct  {
   _sbusdma_desc_handle_node_t * head;
   _sbusdma_desc_handle_node_t * tail;
}_sbusdma_desc_handle_list_t;

typedef struct latency_monitor_data_s {
    volatile int    run[LATENCY_MONITOR_MAX_COUNT];
    int             mem_done[LATENCY_MONITOR_MAX_COUNT];

    int             big_endian;
    int             desc_mode;  /* Descriptor based SBUS DMA mode */
    int             dma_wr_en;  /* SBUS DMA write enable */
    int             dma_rd_en;  /* SBUS DMA read enable */
    int             num_mem;    /* Number of memories to be tested in parallel */
    int             init_done;
    sal_sem_t       sbusdma_run_lock;
    int             monitors_in_use_count; /* Counts the number of monitors in use */
} latency_monitor_driver_data_t;

typedef struct latency_monitor_cosq_info_s {
    uint8           bucket_count;
}latency_monitor_cosq_info_t;

typedef struct latency_monitor_mem_s {
    soc_mem_t       mem;
    int             init_done;
    int             copyno;
    int             index_min;
    int             index_max;
    int             count;
    int             frag_count;
    int             frag_index_max[LATENCY_MONITOR_DEFAULT_FRAG_COUNT];
    int             frag_index_min[LATENCY_MONITOR_DEFAULT_FRAG_COUNT];
    int             bufwords;
    int             entwords;
    sal_sem_t       latency_monitor_thread_trigger;
    sal_sem_t       dma_handle_counter_sem;
    sal_sem_t       dma_handle_queue_lock;
    sal_sem_t       latency_monitor_counter_sem;
    _sbusdma_desc_handle_list_t handle_list;
    uint64          histogram_packet64_counter[COUNTERS_PER_STEP_SET];
    uint64          histogram_byte64_counter[COUNTERS_PER_STEP_SET];
    uint8           status;
    uint32          stat_counter_id;
    uint32          time_step;           /* In Micro sec (Min: 100mS) */
    uint32          wrap_around_time;    /* In Micro sec */
    bcm_latency_monitor_mode_t   monitor_mode;
    void            *sw_buf_addr;
    uint32          sw_buf_size;
    uint32          sw_buf_wr_idx;
    int             flex_ctr_wrap_ctr;
    latency_monitor_cosq_info_t cosq_info[LATENCY_MONITOR_QUEUE_COUNT];
    uint32          dma_schedule_period;
    int             sbusdma_read_thread_pid;
    uint32          dest_pbmp_pipe_map;  /* Destination port bmp pipe map */
    uint32          pipe_map_temp;       /* Temporary pipe map */
    bcm_latency_monitor_callback_fn cb_fn; /* User callback handler */
    void           *user_data;           /* User data */
} latency_monitor_mem_t;

typedef struct cb_data_s {
    uint32 *data;
    uint8   monitor_id;
    uint8   write_op;
    uint8   pipe_num;
} cb_data_t;

typedef enum latency_monitor_id_e {
    MONITOR_ID_0 = 0,
    MONITOR_ID_1,
    MONITOR_ID_2,
    MONITOR_ID_3
} latency_monitor_id_t;

#ifdef BCM_WARM_BOOT_SUPPORT
int _bcm_th3_latency_monitor_sync(int unit);
int _bcm_th3_latency_monitor_wb_post_init(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif  /* _BCM_LATENCY_MONITOR_H_ */

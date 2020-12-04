/* 
 * $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        katana.h
 */

#ifndef _SOC_KATANA_H_
#define _SOC_KATANA_H_

#include <soc/drv.h>

#define _SOC_KT_NODE_CONFIG_MEM(n)                                         \
    (((n)==_SOC_KT_COSQ_NODE_LEVEL_ROOT) ? LLS_PORT_CONFIGm :              \
     (((n) == _SOC_KT_COSQ_NODE_LEVEL_L0) ? LLS_L0_CONFIGm :               \
      ((n) == _SOC_KT_COSQ_NODE_LEVEL_L1 ? LLS_L1_CONFIGm :  -1  )))

#define _SOC_KT_NODE_WIEGHT_MEM(n)                                          \
    (((n)==_SOC_KT_COSQ_NODE_LEVEL_L0) ? LLS_L0_CHILD_WEIGHT_CFG_CNTm  :    \
     (((n) == _SOC_KT_COSQ_NODE_LEVEL_L1) ? LLS_L1_CHILD_WEIGHT_CFG_CNTm :  \
      (((n) == _SOC_KT_COSQ_NODE_LEVEL_L2) ?  LLS_L2_CHILD_WEIGHT_CFG_CNTm : -1 )))

typedef enum {
    _SOC_KT_COSQ_NODE_LEVEL_ROOT,
    _SOC_KT_COSQ_NODE_LEVEL_L0,
    _SOC_KT_COSQ_NODE_LEVEL_L1,
    _SOC_KT_COSQ_NODE_LEVEL_L2,
    _SOC_KT_COSQ_NODE_LEVEL_MAX
}_soc_katana_cosq_node_level_t;

typedef enum {
    SOC_KT_SCHED_MODE_UNKNOWN = 0,
    SOC_KT_SCHED_MODE_STRICT,
    SOC_KT_SCHED_MODE_WRR,
    SOC_KT_SCHED_MODE_WDRR
} soc_kt_sched_mode_e;

extern soc_functions_t soc_katana_drv_funs;

extern int _soc_katana_mem_parity_control(int unit, soc_mem_t mem,
                                                       int copyno, int enable);
extern void soc_katana_stat_nack(int unit, int *fixed);
extern void soc_katana_parity_error(void *unit_vp, void *d1, void *d2,
                                     void *d3, void *d4);
extern void soc_katana_mem_nack(void *unit_vp, void *d1, void *d2,
                                 void *d3, void *d4);
extern int soc_katana_pipe_mem_clear(int unit);
extern int soc_katana_ser_mem_clear(int unit, soc_mem_t mem);
extern void soc_katana_ser_fail(int unit);
extern int soc_katana_get_egr_perq_xmt_counters_size(int unit,
                                                      int *num_entries_x,
                                                      int *num_entries_y);
extern int soc_katana_get_port_mapping(int unit, uint16 dev_id,
                                        int *bandwidth,
                                        const int **speed_max,
                                        const int **p2l_mapping,
                                        const int **p2m_mapping);
extern int soc_katana_init_port_mapping(int unit);
extern int soc_katana_num_cosq_init(int unit);
extern void _kt_lls_workaround(int unit);
extern int soc_katana_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count);
extern int soc_katana_show_ring_osc(int unit);

typedef int (*soc_kt_oam_handler_t)(int unit, soc_field_t fault_field);
extern void soc_kt_oam_handler_register(int unit, soc_kt_oam_handler_t handler);

#if defined(BCM_KATANA_SUPPORT) && defined(BCM_CMICM_SUPPORT)
extern int _soc_mem_kt_fifo_dma_start (int unit,
                                       int chan,
                                       soc_mem_t mem,
                                       int copyno,
                                       int host_entries,
                                       void * host_buf);
extern int _soc_mem_kt_fifo_dma_stop (int unit, int chan);
extern int _soc_mem_kt_fifo_dma_get_read_ptr(int unit,
                                             int chan,
                                             void **host_ptr,
                                             int *count);
extern int _soc_mem_kt_fifo_dma_advance_read_ptr (int unit,
                                                  int chan,
                                                  int count);
extern soc_reg_t _soc_kt_fifo_reg_get(int unit, int cmc, int chan, int type);
#endif

/*
 * Base queue definitions
 */
#define KATANA_BASE_QUEUES_MAX     1024

/*
 * Shaper definitions
 */
#define KATANA_SHAPER_RATE_MIN_EXPONENT                           0
#define KATANA_SHAPER_RATE_MAX_EXPONENT                           13
#define KATANA_SHAPER_RATE_MIN_MANTISSA                           0
#define KATANA_SHAPER_RATE_MAX_MANTISSA                           1023
#define KATANA_SHAPER_BURST_MIN_EXPONENT                          0
#define KATANA_SHAPER_BURST_MAX_EXPONENT                          14
#define KATANA_SHAPER_BURST_MIN_MANTISSA                          0
#define KATANA_SHAPER_BURST_MAX_MANTISSA                          127

typedef struct katanaRateInfo_s {
    uint32 rate;
    uint32 exponent;
    uint32 mantissa;
} katanaRateInfo_t;

typedef struct katanaShaperInfo_s {
    katanaRateInfo_t     *basic_rate_info;
    katanaRateInfo_t     *burst_size_info;
} katanaShaperInfo_t;

extern int
soc_katana_get_shaper_rate_info(int unit, uint32 rate,
                                 uint32 *rate_mantissa, uint32 *rate_exponent);
extern int
soc_katana_get_shaper_burst_info(int unit, uint32 rate,
                                 uint32 *burst_mantissa, uint32 *burst_exponent,
                                 uint32 flags);
extern int
soc_katana_compute_shaper_rate(int unit, uint32 rate_mantissa, 
                               uint32 rate_exponent, uint32 *rate);
extern int
soc_katana_compute_refresh_rate(int unit, uint32 rate_exp, 
                                uint32 rate_mantissa, 
                                uint32 burst_exp, uint32 burst_mantissa,
                                uint32 *cycle_sel);
extern int
soc_katana_compute_burst_rate_check(int unit, uint32 rate_exp,
                                uint32 rate_mantissa,
                                uint32 burst_exp, uint32 burst_mantissa,
                                uint32 cycle_sel);
extern int
soc_katana_compute_shaper_burst(int unit, uint32 burst_mantissa, 
                                uint32 burst_exponent, uint32 *rate);
extern void soc_kt_oam_error(void *unit_vp, void *d1, void *d2,
                                      void *d3, void *d4);
extern int soc_kt_lls_bmap_alloc(int unit);
extern int soc_kt_port_flush(int unit, int port, int enable);

extern int soc_kt_cosq_max_bucket_set(int unit, int port,
                                   int index, uint32 level);
extern int soc_kt_port_flush_state_set(int unit, int port, int enable);
extern int soc_kt_port_flush_state_get(int unit, int port, int *mode);

extern int
soc_kt_cosq_get_sched_mode(int unit, soc_port_t port, int level, int index,
                           soc_kt_sched_mode_e *pmode, int *weight);
extern int
soc_kt_sched_get_node_config(int unit, soc_port_t port, int level, int index,
                             int *pnum_spri, int *pfirst_child,
                             uint32 *pspmap);
extern int soc_kt_l3_defip_index_remap(int unit, soc_mem_t mem, int index);
extern int soc_kt_l3_defip_mem_index_get(int unit, int pindex, soc_mem_t *mem);
extern int soc_kt_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_kt_mem_config(int unit, int dev_id);

extern int _soc_kt_cosq_begin_port_flush(int unit, int port,
                                         int hw_index);
extern int _soc_kt_cosq_end_port_flush(int unit,
                                        int hw_index);
#endif	/* !_SOC_KATANA_H_ */


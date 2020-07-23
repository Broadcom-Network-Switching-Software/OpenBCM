/*
* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        instrumentation.h
 */

#ifndef _BCM_INT_INSTRUMENTATION_H
#define _BCM_INT_INSTRUMENTATION_H

#include <soc/defs.h>

#ifdef BCM_INSTRUMENTATION_SUPPORT

#include <bcm/types.h>
#include <bcm/switch.h>


/* packet trace feature support */
extern int bcm_esw_pkt_trace_init(int unit);
extern int _bcm_esw_pkt_trace_info_get(int unit, uint32 options, uint8 port, 
                                       int len, uint8 *data, 
                                       bcm_switch_pkt_trace_info_t 
                                                            *pkt_trace_info);

extern int _bcm_esw_pkt_trace_src_port_set(int unit, uint32 logical_src_port);
extern int _bcm_esw_pkt_trace_src_port_get(int unit);
extern int _bcm_esw_pkt_trace_src_pipe_get(int unit);
extern int _bcm_esw_pkt_trace_hw_reset(int unit);
extern int _bcm_esw_pkt_trace_cpu_profile_get(int unit, uint32* profile_id);
typedef struct _bcm_switch_pkt_trace_port_info_s {
    uint32 pkt_trace_src_logical_port;
    uint8  pkt_trace_src_pipe;

} _bcm_switch_pkt_trace_port_info_t;
extern int _bcm_esw_pkt_trace_raw_data_get(int unit,
                               uint32 options,
                               uint8 port,
                               int len,
                               uint8 *data,
                               uint32 raw_data_buf_size,
                               uint8 *raw_data,
                               uint32 *raw_data_size);

/* for tomahawk */
/* size is in number of double words */
#define TH_PTR_RESULTS_IVP_MAX_INDEX  2 
#define TH_PTR_RESULTS_ISW1_MAX_INDEX 4
#define TH_PTR_RESULTS_ISW2_MAX_INDEX 8

#define TH_FWD_DEST_TYPE_DGLP   0
#define TH_FWD_DEST_TYPE_NH     1
#define TH_FWD_DEST_TYPE_ECMP   2
#define TH_FWD_DEST_TYPE_L2MC   3
#define TH_FWD_DEST_TYPE_IPMC   4
#define TH_FWD_DEST_TYPE_DVP    5

#define TH_FWD_DEST_PORT_MASK   0xFF
#define TH_FWD_DEST_PORT_SHIFT  8

extern int _bcm_th_pkt_trace_info_get(int unit,
                                      bcm_switch_pkt_trace_info_t *pkt_trace_info);
extern int _bcm_th_pkt_trace_src_port_set(int unit, uint32 logical_src_port);
extern int _bcm_th_pkt_trace_src_port_get(int unit);
extern int _bcm_th_pkt_trace_src_pipe_get(int unit);
extern int _bcm_th_pkt_trace_hw_reset(int unit);
extern int _bcm_th_pkt_trace_cpu_profile_init(int unit);
extern int _bcm_th_pkt_trace_cpu_profile_set(int unit,uint32 options);
extern int _bcm_th_pkt_trace_cpu_profile_get(int unit,uint32* profile_id);
extern int _bcm_th_pkt_trace_int_lbport_set(int unit, uint8 port, int enable, int *old_values);
/* TD3 paket trace function */
extern int _bcm_td3_pkt_trace_src_port_set(int unit, uint32 logical_src_port);
extern int _bcm_td3_pkt_trace_src_port_get(int unit);
extern int _bcm_td3_pkt_trace_src_pipe_get(int unit);
extern int _bcm_td3_pkt_trace_cpu_profile_init(int unit);
extern int _bcm_td3_pkt_trace_cpu_profile_set(int unit,
                                     uint32 flags);
extern int _bcm_td3_pkt_trace_cpu_profile_get(int unit, uint32* profile_id);
extern int _bcm_td3_pkt_trace_init(int unit);
extern int _bcm_td3_pkt_trace_info_get(int unit,
                      bcm_switch_pkt_trace_info_t *pkt_trace_info);
extern int _bcm_td3_pkt_trace_raw_data_get(int unit,
                      uint32 *raw_data_size,
                      uint8 *raw_data);
extern int _bcm_td3_pkt_trace_dop_config_set(int unit);
extern int _bcm_td3_pkt_trace_source_trunk_map_pp_set(int unit,
                                              bcm_module_t mod_id,
                                              bcm_port_t pp_port,
                                              uint16 reset);
#if defined(BCM_TOMAHAWK3_SUPPORT)
/* TH3 paket trace functions */
extern int _bcm_th3_pkt_trace_hw_reset(int unit, int pipe);
extern int _bcm_th3_pkt_trace_info_retrieve_data(int unit,
                int pipe, bcm_switch_pkt_trace_info_t *pkt_trace_info);
extern int _bcm_th3_pkt_trace_info_get(int unit,
                               uint32 options, uint8 port,
                               int len, uint8 *data,
                  bcm_switch_pkt_trace_info_t *pkt_trace_info);
#endif /* BCM_TOMAHAWK3_SUPPORT */
/* end of packet trace feature support */
#endif /* BCM_INSTRUMENTATION_SUPPORT */

#endif

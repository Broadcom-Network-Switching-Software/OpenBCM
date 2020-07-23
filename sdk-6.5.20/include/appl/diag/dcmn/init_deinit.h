/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        init_deinit.h
 * Purpose:     DCMN initialization sequence header file.
 */

#ifndef _APPL_DCMN_INIT_DEINIT_H
#define _APPL_DCMN_INIT_DEINIT_H

#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING
  #define AGGRESSIVE_ALLOC_DEBUG_TESTING 0
#endif

/* appl_traffic_enable_stage options: */
#define TRAFFIC_EN_STAGE_DISABLE     0
#define TRAFFIC_EN_STAGE_AFTER_STK   0x1

typedef struct appl_dcmn_init_param_s {
    uint32 unit;
    int32 nof_devices;
    int32 repeat;
    int32 no_init;
    int32 no_deinit;
    int32 no_attach;
    int32 no_bcm;
    int32 no_soc;
    int32 no_intr;
    int32 no_rx_los;
    int32 modid;
    int32 base_modid;
    int32 flags;   
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
    int32 partial_init;
    int32 fake_bcm_start;
    int32 fake_bcm_range;
    int32 fake_soc_start;
    int32 fake_soc_range; 
#endif
    int32 no_linkscan;
    int32 warmboot;
    int32 engine_dump;
    int32 no_sync;
    int32 no_dump;
    int32 no_elk_appl;
    int32 no_elk_device;
    int32 no_elk_second_device;
    uint32 elk_ilkn_rev;
    uint32 elk_mdio_id;
    uint32 elk_mdio_second_id;

    int32 no_packet_rx;
    int32 packet_rx_cosq;

    int32 no_appl;
    int32 no_appl_stk;
    int32 appl_traffic_enable_stage;
    uint32 cosq_disable;
    uint32 appl_flags;
    int32 no_itmh_prog_mode;
    int32 rc_load;
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
    uint32 is_resources_check;
    uint32 aggressive_num_to_test;
    uint32 aggressive_alloc_debug_testing_keep_order;
    uint32 aggressive_long_free_search;
#endif
    uint32 error_on_leak;
    uint32 l2_mode;
} appl_dcmn_init_param_t;



cmd_result_t appl_dcmn_init_usage_parse(int unit, args_t *a, appl_dcmn_init_param_t *init_param);

int appl_dcmn_init(int unit, appl_dcmn_init_param_t* init_param);

int appl_dcmn_deinit(int unit, appl_dcmn_init_param_t* init_param);

#endif /* _APPL_DCMN_INIT_DEINIT_H */

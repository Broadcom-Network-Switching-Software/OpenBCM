/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        trident.h
 */

#ifndef _SOC_TRIDENT_H_
#define _SOC_TRIDENT_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>

typedef enum soc_td_drop_limit_alpha_value_e {
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_64, /* Use 1/64 as the alpha value for
                                           dynamic threshold */
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_32, /* Use 1/32 as the alpha value for
                                           dynamic threshold */
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_16, /* Use 1/16 as the alpha value for
                                           dynamic threshold */
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_8, /* Use 1/8 as the alpha value for
                                           dynamic threshold */                                          
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_4, /* Use 1/4 as the alpha value for
                                           dynamic threshold */
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_2, /* Use 1/2 as the alpha value for
                                           dynamic threshold */
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_1,  /* Use 1 as the alpha value for dynamic
                                           threshold */
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_2,  /* Use 2 as the alpha value for dynamic
                                           threshold */
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_4,  /* Use 4 as the alpha value for dynamic
                                           threshold */
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_8,  /* Use 8 as the alpha value for dynamic
                                           threshold */
    SOC_TD_COSQ_DROP_LIMIT_ALPHA_COUNT /* Must be the last entry! */
} soc_td_drop_limit_alpha_value_t;

extern soc_functions_t soc_trident_drv_funs;
extern int _soc_trident_mem_parity_control(int unit, soc_mem_t mem,
                                           int copyno, int enable);
extern int soc_trident_ser_mem_clear(int unit, soc_mem_t mem);
extern int soc_trident_pipe_select(int unit, int egress, int pipe);
extern int soc_trident_get_egr_perq_xmt_counters_size(int unit,
                                                      int *num_entries_x,
                                                      int *num_entries_y);
extern int soc_trident_port_config_init(int unit, uint16 dev_id);
extern int soc_trident_num_cosq_init(int unit);
extern int soc_trident_mmu_config_init(int unit, int test_only);
extern int soc_trident_mem_config(int unit);
extern int soc_trident_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count);
extern int soc_trident_show_material_process(int unit);
extern int soc_trident_show_ring_osc(int unit);

extern int soc_trident_cmic_rate_param_get(int unit, int *dividend, int *divisor);
extern int soc_trident_cmic_rate_param_set(int unit, int dividend, int divisor);

extern int soc_trident_ser_dual_pipe_correction(int unit, soc_mem_t reported_mem,
                                      int copyno, int index ,_soc_ser_correct_info_t *si);

#if defined(SER_TR_TEST_SUPPORT)
extern soc_error_t soc_td_ser_inject_error(int unit, uint32 flags,
                                           soc_mem_t mem, int pipeTarget,
                                           int block, int index);
extern soc_error_t soc_td_ser_mem_test(int unit, soc_mem_t mem,
                                       _soc_ser_test_t test_type, int cmd);
extern soc_error_t soc_td_ser_cmd_print(int unit, soc_mem_t mem);
extern soc_error_t soc_td_ser_test(int unit,_soc_ser_test_t test_type);
extern soc_error_t soc_td_ser_error_injection_support(int unit,
                                                      soc_mem_t mem,
                                                      int pipe);
extern int soc_td_ser_test_overlay (int unit, _soc_ser_test_t test_type);
#endif /*#if defined(SER_TR_TEST_SUPPORT)*/

#endif	/* !_SOC_TRIDENT_H_ */

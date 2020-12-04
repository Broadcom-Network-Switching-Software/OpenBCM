/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        triumph2.h
 */

#ifndef _SOC_TRIUMPH2_H_
#define _SOC_TRIUMPH2_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <shared/sram.h>

typedef int (*soc_triumph2_oam_handler_t)(int unit, soc_field_t fault_field);
typedef int (*soc_triumph2_oam_ser_handler_t)(int unit, soc_mem_t mem, int index);

extern void soc_triumph2_stat_nack(int unit, int *fixed);
extern int soc_triumph2_misc_init(int);
extern int soc_triumph2_mmu_init(int);
extern int soc_triumph2_age_timer_get(int, int *, int *);
extern int soc_triumph2_age_timer_max_get(int, int *);
extern int soc_triumph2_age_timer_set(int, int, int);
extern void soc_triumph2_parity_error(void *unit_vp, void *d1, void *d2,
                                      void *d3, void *d4);
extern void soc_triumph2_oam_events(void *unit_vp, void *d1, void *d2,
                                    void *d3, void *d4);
extern void soc_triumph2_mem_nack(void *unit_vp, void *addr_vp, void *d2,
                                     void *d3, void *d4);
extern void soc_triumph2_ser_fail(int unit);
extern int soc_triumph2_ser_mem_clear(int unit, soc_mem_t mem);
extern int _soc_triumph2_mem_parity_control(int unit, soc_mem_t mem,
                                            int copyno, int enable);
extern int _soc_triumph2_esm_process_intr_status(int unit);
extern void soc_triumph2_esm_intr_status(void *unit_vp, void *d1, void *d2,
                                         void *d3, void *d4);
extern int soc_triumph2_pipe_mem_clear(int unit);
extern soc_functions_t soc_triumph2_drv_funs;
extern void soc_triumph2_oam_handler_register(int unit,
											  soc_triumph2_oam_handler_t
											  handler);
extern void soc_triumph2_oam_ser_handler_register(int unit, soc_triumph2_oam_ser_handler_t handler);
extern int soc_triumph2_oam_ser_process(int unit, soc_mem_t mem, int index);
extern int soc_tr2_xqport_mode_change(int unit, soc_port_t port, 
                                      soc_mac_mode_t mode);
extern void soc_triumph2_mem_config(int unit);

#if defined(SER_TR_TEST_SUPPORT)
extern int soc_tr2_ser_inject_error(int unit, uint32 flags, soc_mem_t mem,
									int pipeTarget, int block, int index);
extern int soc_tr2_ser_mem_test(int unit, soc_mem_t mem,
							    _soc_ser_test_t test_type, int cmd);
extern int soc_tr2_ser_test(int unit, _soc_ser_test_t test_type);
extern int _soc_triumph2_mem_nack_error_test(int unit,
											 _soc_ser_test_t test_type,
											 int *testErrors);
#endif

#endif    /* !_SOC_TRIUMPH2_H_ */

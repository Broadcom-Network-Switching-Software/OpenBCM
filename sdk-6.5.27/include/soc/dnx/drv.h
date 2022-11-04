
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_DNX_DRV_H

#define _SOC_DNX_DRV_H

#if (!defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT))
#error "This file is for use by DNX or DNXF family only!"
#endif

#include <sal/types.h>
#include <shared/cyclic_buffer.h>
#include <shared/utilex/utilex_framework.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/scache.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnx/dnx_hbmc.h>

typedef int (
    *dnx_startup_test_function_f) (
    int unit);
extern dnx_startup_test_function_f dnx_startup_test_functions[SOC_MAX_NUM_DEVICES];

extern uint32 any_bist_performed[(SOC_MAX_NUM_DEVICES + 31) / 32];

#define SOC_DNX_RESET_ACTION_IN_RESET                               SOC_DNXC_RESET_ACTION_IN_RESET
#define SOC_DNX_RESET_ACTION_OUT_RESET                              SOC_DNXC_RESET_ACTION_OUT_RESET
#define SOC_DNX_RESET_ACTION_INOUT_RESET                            SOC_DNXC_RESET_ACTION_INOUT_RESET

#define SOC_DNX_RESET_MODE_HARD_RESET                               SOC_DNXC_RESET_MODE_HARD_RESET
#define SOC_DNX_RESET_MODE_BLOCKS_SOFT_RESET                        SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET
#define SOC_DNX_RESET_MODE_REG_ACCESS                               SOC_DNXC_RESET_MODE_REG_ACCESS
#define SOC_DNX_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET             SOC_DNXC_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET

#define SOC_DNX_RESET_MODE_FLAG_WITHOUT_ILE          (0x1)

#define SOC_DNX_ALLOW_WARMBOOT_WRITE(operation, _rv) \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv)

#define SOC_DNX_ALLOW_WARMBOOT_WRITE_NO_ERR(operation, _rv) \
        do { \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv); \
            if (_rv != _SHR_E_UNIT) { \
                _rv = _SHR_E_NONE; \
            } \
        } while(0)

#define SOC_DNX_WARMBOOT_RELEASE_HW_MUTEX(_rv)\
        do {\
            _rv = soc_schan_override_disable(unit); \
        } while(0)

int soc_dnx_info_config_deinit(
    int unit);
int soc_dnx_init_reset(
    int unit,
    int reset_action);
int soc_dnx_init_reg_reset(
    int unit,
    int reset_action);
int soc_dnx_device_reset(
    int unit,
    int mode,
    int flag,
    int action);
int soc_bist_all_jr2(
    const int unit,
    const int skip_errors);
int soc_bist_all_j2c(
    const int unit,
    const int skip_errors);
int soc_bist_all_q2a(
    const int unit,
    const int skip_errors);
int soc_bist_all_j2p(
    const int unit,
    const int skip_errors);
int soc_q2a_cpu2tap_init_mems(
    const int unit);
int soc_read_jr2_hbm_temp(
    const int unit,
    const unsigned hbm_number,
    uint32 *out_temp);
int soc_read_jr2_hbm_device_id(
    const int unit,
    const unsigned hbm_number,
    soc_dnx_hbm_device_id_t * id);
int soc_read_j2c_hbm_temp(
    const int unit,
    uint32 *out_temp);
int soc_read_j2c_hbm_device_id(
    const int unit,
    soc_dnx_hbm_device_id_t * id);

int soc_dnx_read_hbm_temp(
    const int unit,
    const unsigned hbm_number,
    uint32 *out_temp);

int soc_set_hbm_wrst_n_to_0(
    const int unit,
    const unsigned hbm_number);
int soc_dnx_init_reset_cmic_regs(
    int unit);
int soc_dnx_drv_sbus_broadcast_config(
    int unit);

shr_error_e soc_dnx_chip_type_set(
    int unit,
    uint16 dev_id);

int dnx_access_init_handle_error_skipping(
    int unit,
    int error_value);

extern shr_error_e soc_dnx_attach(
    int unit);

extern shr_error_e soc_dnx_detach(
    int unit);

shr_error_e soc_dnx_mark_not_inited(
    int unit);

shr_error_e soc_dnx_iproc_config(
    int unit);

shr_error_e soc_dnx_ring_config(
    int unit);

shr_error_e soc_dnx_soft_reset(
    int unit);

shr_error_e soc_dnx_pll_configuration(
    int unit);

shr_error_e soc_dnx_soft_init(
    int unit,
    int without_fabric,
    int without_ile);

shr_error_e soc_dnx_dma_init(
    int unit);

shr_error_e soc_dnx_dma_deinit(
    int unit);

shr_error_e soc_dnx_perform_bist_tests(
    int unit);

shr_error_e soc_dnx_unmask_mem_writes(
    int unit);

shr_error_e soc_dnx_rcpu_init(
    int unit);

int dnx_tbl_is_dynamic(
    int unit,
    soc_mem_t mem);

int dnx_redo_soft_reset_soft_init(
    int unit,
    int perform_soft_reset,
    int without_fabric,
    int without_ile);

int dnx_init_skip_mem_in_mem_init_testing(
    int unit,
    soc_mem_t mem);
shr_error_e soc_dnx_block_enable_set(
    int unit);

shr_error_e soc_dnx_ecc_indication_on_sbus_set(
    int unit,
    int enable);

#endif

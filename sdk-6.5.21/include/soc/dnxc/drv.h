
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_DNXC_DRV_H_INCLUDE

#define SOC_DNXC_DRV_H_INCLUDE

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (JR2) and DNXF (Ramon) family only!"
#endif

#include <sal/types.h>
#include <shared/cyclic_buffer.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/mem.h>
#include <soc/iproc.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/error.h>

extern uint32 test_was_run_and_suceeded[(SOC_MAX_NUM_DEVICES + 31) / 32];

int soc_dnxc_cmicx_device_hard_reset(
    int unit,
    int reset_action);

int soc_dnxc_avs_value_get(
    int unit,
    uint32 *avs_val);

void soc_dnxc_chip_dump(
    int unit,
    soc_driver_t * d);

int soc_dnxc_nof_block_instances(
    int unit,
    soc_block_types_t block_types,
    int *nof_block_instances);

shr_error_e soc_dnxc_verify_device_init(
    int unit);

shr_error_e dnxc_block_instance_validity_override(
    int unit);

int soc_dnxc_does_need_system_reset(
    int unit);

void soc_dnxc_set_need_system_reset(
    int unit,
    int needs_system_reset);

int soc_dnxc_does_support_system_reset(
    int unit);

int soc_dnxc_perform_system_reset(
    int unit);

int soc_dnxc_perform_system_reset_if_needed(
    int unit);
#endif

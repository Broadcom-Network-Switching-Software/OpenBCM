
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_DNX_DNX_INTR_H_INCLUDE

#define SOC_DNX_DNX_INTR_H_INCLUDE

#if !defined(BCM_DNX_SUPPORT)
#error "This file is for use by DNX family only!"
#endif

#include <sal/types.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/mem.h>
#include <soc/intr.h>
#include <soc/defs.h>
#include <soc/iproc.h>

typedef int (
    *dnx_intr_exceptional_get_f) (
    int unit,
    uint32 *entry_data);

shr_error_e dnx_intr_exceptional_dram_not_present(
    int unit,
    uint32 *is_exception);

int soc_dnx_interrupt_enable(
    int unit,
    int intr_id,
    int block_instance,
    const soc_interrupt_db_t * inter);

int soc_dnx_int_name_to_id(
    int unit,
    char *name);
int soc_dnx_interrupts_array_init(
    int unit);
int soc_dnx_interrupts_array_deinit(
    int unit);
int soc_dnx_interrupt_cb_init(
    int unit);
int soc_dnx_is_block_eci_intr_assert(
    int unit,
    int blk,
    soc_reg_above_64_val_t eci_interrupt);
int soc_dnx_ser_init(
    int unit);

int soc_dnx_is_interrupt_support(
    int unit,
    int intr_id,
    int *is_support);

shr_error_e dnx_ser_kaps_tcam_scan_machine_enable(
    int unit,
    int is_init,
    int enable);

#endif

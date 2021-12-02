/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement CallBacks function for J2X interrupts.
 */

#include <shared/bsl.h>
#include <soc/mcm/allenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_intr_handler.h>
#include <soc/dnxc/dnxc_intr_corr_act_func.h>

#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/intr.h>

#include <soc/dnx/drv.h>
#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>
#include <soc/dnx/intr/auto_generated/j2x/j2x_intr_cb_func.h>
#include <soc/dnx/intr/auto_generated/j2x/j2x_intr.h>

#include <soc/sand/sand_intr_corr_act_func.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR

void
j2x_interrupt_cb_init(
    int unit)
{

}

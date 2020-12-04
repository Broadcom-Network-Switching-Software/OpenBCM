/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DCMN_INTR_H_
#define _DCMN_INTR_H_

/*************
 * INCLUDES  *
 */
#include <soc/defs.h>
#include <soc/types.h>
#include <bcm/error.h>

/*************
 * TYPE DEFS *
 */

/*************
 * STRUCTURES *
 */

/*
 * static data strucure which holds the info about: 
 *             the function suppose to run for each interrupt
 *              info about storm threshold
 */
typedef struct interrupt_common_params_s {
    uint32* int_disable_on_init;
    uint32* int_active_on_init;
    uint32* int_disable_print_on_init;
    soc_mem_t* cached_mem;
} intr_common_params_t;

/*************
 * FUNCTIONS *
 */
int interrupt_appl_init(int unit);
int interrupt_appl_deinit(int unit);
int interrupt_appl_nocache_deinit(int unit);

#ifdef BCM_QUX_SUPPORT
int qux_interrupt_cmn_param_init(int unit, intr_common_params_t* interrupt_common_params);
#endif

#ifdef BCM_QAX_SUPPORT
int qax_interrupt_cmn_param_init(int unit, intr_common_params_t* interrupt_common_params);
#endif

#ifdef BCM_JERICHO_PLUS_SUPPORT
int jerp_interrupt_cmn_param_init(int unit, intr_common_params_t* interrupt_common_params);
#endif

#ifdef BCM_JERICHO_SUPPORT
int jer_interrupt_cmn_param_init(int unit, intr_common_params_t* interrupt_common_params);
#endif
#ifdef BCM_DFE_SUPPORT
int fe3200_interrupt_cmn_param_init(int unit, intr_common_params_t* interrupt_common_params);
#endif

uint32 *interrupt_active_on_intr_get(int unit);

#endif /*_DCMN_INTR_H_ */

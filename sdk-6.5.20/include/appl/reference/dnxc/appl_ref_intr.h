/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _APPL_REF_INTR_H_
#define _APPL_REF_INTR_H_

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
int appl_dnxc_interrupt_init(int unit);
int appl_dnxc_interrupt_deinit(int unit);
int interrupt_appl_nocache_deinit(int unit);

#ifdef BCM_DNX_SUPPORT
int dnx_interrupt_appl_log_defaults_set(int unit);
int dnx_interrupt_appl_mask_defaults_set(int unit);
#endif
#ifdef BCM_DNXF_SUPPORT
int dnxf_interrupt_appl_log_defaults_set(int unit);
int dnxf_interrupt_appl_mask_defaults_set(int unit);
#endif
#endif /*_DCMN_INTR_H_ */

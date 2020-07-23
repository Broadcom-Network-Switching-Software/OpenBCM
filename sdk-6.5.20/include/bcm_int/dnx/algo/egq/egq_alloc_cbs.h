/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef  EGQ_ALLOC_CBS_H_INCLUDED
#define  EGQ_ALLOC_CBS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif

#ifdef DNXC_SW_STATE_INTERNAL
#undef DNXC_SW_STATE_INTERNAL
#endif

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/*
 * \brief
 *   Print functions needed for template manager.
 */
void dnx_algo_egr_tc_dp_mapping_print_cb(
    int unit,
    const void *data);

void dnx_algo_fqp_profile_params_print_cb(
    int unit,
    const void *data);

#endif /* INCLUDE_DNX_ALLOC_MNGR_CBS_H */

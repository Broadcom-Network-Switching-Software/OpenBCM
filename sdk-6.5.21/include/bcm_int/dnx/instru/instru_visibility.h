/**
 * \file instru_visibility.h
 * Internal DNX INSTRU VIS APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef _INSTRU_VIS_H_INCLUDED__
#define _INSTRU_VIS_H_INCLUDED__

#include <soc/sand/sand_signals.h>
#include <bcm/error.h>

shr_error_e bcm_dnx_instru_vis_signal_get(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int nof_signals,
    bcm_instru_vis_signal_key_t * signal_key,
    bcm_instru_vis_signal_result_t * signal_result);

shr_error_e bcm_dnx_instru_vis_resume(
    int unit,
    bcm_core_t core_id,
    uint32 flags);

#endif /* _INSTRU_VIS_H_INCLUDED__ */

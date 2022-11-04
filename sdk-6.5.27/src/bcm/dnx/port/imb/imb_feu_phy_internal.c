/** \file imb_feu_phy_internal.c
 *
 */
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif

#include "imb_feu_phy_internal.h"
#include "imb_framer_internal.h"
#include <soc/portmod/portmod.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

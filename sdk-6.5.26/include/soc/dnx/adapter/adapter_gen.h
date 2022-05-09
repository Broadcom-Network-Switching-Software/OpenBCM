
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. $
 */

#ifndef ADAPTER_GEN_H_INCLUDED

#define ADAPTER_GEN_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <include/soc/register.h>
#include <soc/sand/shrextend/shrextend_debug.h>

shr_error_e adapter_egq_interface_port_map_set(
    int unit,
    bcm_port_t port,
    int egr_if);

#endif

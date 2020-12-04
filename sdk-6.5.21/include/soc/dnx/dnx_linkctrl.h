/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNX LINKCTRL H
 */

#ifndef _SOC_DNX_LINKCTRL_H_
#define _SOC_DNX_LINKCTRL_H_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif

#include <soc/error.h>

extern int soc_dnx_linkctrl_init(
    int unit);

#endif

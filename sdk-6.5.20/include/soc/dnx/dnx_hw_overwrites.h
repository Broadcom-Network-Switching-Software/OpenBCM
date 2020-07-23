
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */

#ifndef DNX_HW_OVERWRITES_H_INCLUDED

#define DNX_HW_OVERWRITES_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <shared/shrextend/shrextend_debug.h>

shr_error_e dnx_hw_overwrite_init(
    int unit);

#endif

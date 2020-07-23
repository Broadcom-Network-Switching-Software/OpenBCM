
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_VERIFY_H_

#define _DNX_DATA_VERIFY_H_

#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

shr_error_e dnx_data_property_unsupported_verify(
    int unit);

#endif

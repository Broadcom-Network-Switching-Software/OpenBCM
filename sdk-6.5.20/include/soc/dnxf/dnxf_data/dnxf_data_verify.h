

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_VERIFY_H_

#define _DNXF_DATA_VERIFY_H_

#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



shr_error_e
dnxf_data_property_unsupported_verify(int unit);


#endif 


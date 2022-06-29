
/*
 *                                                            $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. $
 */

#ifndef ADAPTER_REG_ACCESS_H_INCLUDED

#define ADAPTER_REG_ACCESS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#ifdef ADAPTER_SERVER_MODE

#include <soc/dnxc/dnxc_adapter_reg_access.h>

uint32 adapter_send_buffer(
    int unit,
    adapter_ms_id_e ms_id,
    uint32 src_port,
    uint32 src_channel,
    int len,
    unsigned char *buf,
    int nof_signals);

#endif

#endif

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE PORT H
 */
 
#ifndef _SOC_DFE_PORT_H_
#define _SOC_DFE_PORT_H_

#include <soc/types.h>
#include <soc/error.h>
#include <soc/dcmn/dcmn_defs.h>

typedef enum soc_dfe_port_update_type_s
{
    soc_dfe_port_update_type_sfi,
    soc_dfe_port_update_type_port,
    soc_dfe_port_update_type_all,
    soc_dfe_port_update_type_sfi_disabled,
    soc_dfe_port_update_type_port_disabled,
    soc_dfe_port_update_type_all_disabled
} soc_dfe_port_update_type_t;






soc_error_t soc_dfe_port_loopback_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback);
soc_error_t soc_dfe_port_loopback_get(int unit, soc_port_t port, soc_dcmn_loopback_mode_t* loopback);


#endif 

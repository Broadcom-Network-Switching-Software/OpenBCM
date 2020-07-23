/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXC Data Property
 */

#ifndef _SOC_DNXC_DATA_PROPERTY_H_
#define _SOC_DNXC_DATA_PROPERTY_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jr2) and DNXF family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/port.h>

shr_error_e dnxc_data_property_fw_load_method_read(
    int unit,
    uint32 *fw_load_method,
    const char *suffix);

shr_error_e dnxc_data_property_fw_crc_check_read(
    int unit,
    uint32 *fw_crc_check,
    const char *suffix);

shr_error_e dnxc_data_property_fw_load_verify_read(
    int unit,
    uint32 *fw_load_verify,
    const char *suffix);

shr_error_e dnxc_data_property_serdes_tx_taps_read(
    int unit,
    int port,
    char *propval,
    bcm_port_phy_tx_t * tx);

#endif


/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_DNXC_OTP_DRV_H_
#define _SOC_DNXC_OTP_DRV_H_

#include <soc/defs.h>
#include <soc/dnxc/dnxc_defs.h>

#define SOC_DNXC_OTP_NOF_BYTES_IN_WORD     (SHR_BITWID / 8)

typedef enum
{
    SOC_DNXC_OTP_INSTANCE_GENERAL_USE,

    SOC_DNXC_OTP_INSTANCE_HBM_0,
    SOC_DNXC_OTP_INSTANCE_HBM_1,
    SOC_DNXC_OTP_INSTANCE_COUNT
} soc_dnxc_otp_instances_e;

int soc_dnxc_otp_nof_words_in_row_get(
    int unit);

shr_error_e soc_dnxc_otp_read(
    int unit,
    soc_dnxc_otp_instances_e otp_instance,
    uint32 start_bit_addr,
    uint32 nof_bits,
    uint32 flags,
    SHR_BITDCL * output_buffer);

#endif



/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_DRAM_HBMC_HBMCOTP_H_INCLUDED

#define _BCM_DNX_DRAM_HBMC_HBMCOTP_H_INCLUDED


#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>


#define HBMC_OTP_HEADER_SIZE (8)
#define HBMC_OTP_HEADER_VER_OFFSET (0)
#define HBMC_OTP_HEADER_VER_MASK (0xf)
#define HBMC_OTP_HEADER_RESERVED_OFFSET (4)
#define HBMC_OTP_HEADER_RESERVED_MASK (0x7)
#define HBMC_OTP_HEADER_INVALID_OFFSET (7)
#define HBMC_OTP_HEADER_INVALID_MASK (0x1)
#define HBMC_OTP_HEADER_VERSION_GET(_header) ((_header >> HBMC_OTP_HEADER_VER_OFFSET) & (HBMC_OTP_HEADER_VER_MASK))
#define HBMC_OTP_HEADER_INVALID_GET(_header) \
    ((_header >> HBMC_OTP_HEADER_INVALID_OFFSET) & (HBMC_OTP_HEADER_INVALID_MASK))

soc_error_t dnx_hbmc_otp_shmoo_hbm16_restore_from_otp(
    int unit,
    int hbm_ndx,
    hbmc_shmoo_config_param_t * shmoo_config_param);

#endif 

/*! \file bcm56780_a0_bd_otp.c
 *
 * Device-specific functions for accessing OTP parameters
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmbd/bcmbd_otp.h>
#include <bcmdrd/chip/bcm56780_a0_cmic_defs.h>
#include <bcmbd/chip/bcm56780_a0_cmic_acc.h>
#include "bcm56780_a0_drv.h"

/* Local defines */
/* Offset in to OTP register array where AVS value is stored */
#define OTP_REG_OFFSET 4
/* Macros to extract AVS voltage value. Value is contained in a single word */
#define AVS_VAL_HIGH_BIT_POS 28
#define AVS_VAL_LOW_BIT_POS  21
/* AVS value is always less than 32 bits */
#define AVS_VAL_NUM_BITS ((AVS_VAL_HIGH_BIT_POS) - (AVS_VAL_LOW_BIT_POS) + 1)
#define AVS_VAL_MASK ((AVS_VAL_NUM_BITS < 32) ? \
                      ((1 << (AVS_VAL_NUM_BITS)) - 1) : ~0)

int
bcm56780_a0_bd_avs_voltage_get(int unit, uint32_t *val)
{
    DMU_PCU_OTP_CONFIGr_t otp_config;
    uint32_t reg_val;

    READ_DMU_PCU_OTP_CONFIGr(unit, OTP_REG_OFFSET, &otp_config);
    reg_val = DMU_PCU_OTP_CONFIGr_OTP_CONFIGf_GET(otp_config);
    reg_val >>= AVS_VAL_LOW_BIT_POS;
    *val = reg_val & AVS_VAL_MASK;

    return SHR_E_NONE;
}

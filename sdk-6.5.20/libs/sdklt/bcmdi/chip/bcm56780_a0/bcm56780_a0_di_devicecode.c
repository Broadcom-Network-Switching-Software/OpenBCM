/*! \file bcm56780_a0_di_devicecode.c
 *
 * BCMDI DeviceCode Loader
 *
 * This module contains the implementation of BCMDI DeviceCode loader and
 * extra device initialization not covered by DeviceCode
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_dev.h>
#include <bcmdi/bcmdi_internal.h>
#include "bcm56780_a0_di_devicecode.h"

#define BSL_LOG_MODULE BSL_LS_BCMDI_DEVICECODE

/*******************************************************************************
 * Private Functions
 */
static int
uft_hash_init(int unit)
{
    int ioerr = 0;
    IFTA30_E2T_00_SHARED_BANKS_CONTROLm_t i30_sbctl0;
    EFTA30_E2T_00_SHARED_BANKS_CONTROLm_t e30_sbctl0;
    IFTA40_E2T_00_SHARED_BANKS_CONTROLm_t i40_sbctl0;
    IFTA40_E2T_01_SHARED_BANKS_CONTROLm_t i40_sbctl1;
    IFTA80_E2T_00_SHARED_BANKS_CONTROLm_t i80_sbctl0;
    IFTA80_E2T_01_SHARED_BANKS_CONTROLm_t i80_sbctl1;
    IFTA80_E2T_02_SHARED_BANKS_CONTROLm_t i80_sbctl2;
    IFTA80_E2T_03_SHARED_BANKS_CONTROLm_t i80_sbctl3;
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_t i90_sbctl0;
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_t i90_sbctl1;
    M_E2T_00_SHARED_BANKS_CONTROLm_t      m_sbctl;

    if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
        return SHR_E_NONE;
    }

    IFTA30_E2T_00_SHARED_BANKS_CONTROLm_CLR(i30_sbctl0);
    EFTA30_E2T_00_SHARED_BANKS_CONTROLm_CLR(e30_sbctl0);
    IFTA40_E2T_00_SHARED_BANKS_CONTROLm_CLR(i40_sbctl0);
    IFTA40_E2T_01_SHARED_BANKS_CONTROLm_CLR(i40_sbctl1);
    IFTA80_E2T_00_SHARED_BANKS_CONTROLm_CLR(i80_sbctl0);
    IFTA80_E2T_01_SHARED_BANKS_CONTROLm_CLR(i80_sbctl1);
    IFTA80_E2T_02_SHARED_BANKS_CONTROLm_CLR(i80_sbctl2);
    IFTA80_E2T_03_SHARED_BANKS_CONTROLm_CLR(i80_sbctl3);
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_CLR(i90_sbctl0);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_CLR(i90_sbctl1);
    M_E2T_00_SHARED_BANKS_CONTROLm_CLR(m_sbctl);

    IFTA30_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(i30_sbctl0, 0x6);
    EFTA30_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(e30_sbctl0, 0x6);
    IFTA40_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(i40_sbctl0, 0x6);
    IFTA40_E2T_01_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(i40_sbctl1, 0x6);
    IFTA80_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(i80_sbctl0, 0x6);
    IFTA80_E2T_01_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(i80_sbctl1, 0x6);
    IFTA80_E2T_02_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(i80_sbctl2, 0x6);
    IFTA80_E2T_03_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(i80_sbctl3, 0x6);
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(i90_sbctl0, 0x6);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(i90_sbctl1, 0x6);
    M_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(m_sbctl, 0x6);

    IFTA30_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(i30_sbctl0, 0xc);
    EFTA30_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(e30_sbctl0, 0xc);
    IFTA40_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(i40_sbctl0, 0xc);
    IFTA40_E2T_01_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(i40_sbctl1, 0xc);
    IFTA80_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(i80_sbctl0, 0xc);
    IFTA80_E2T_01_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(i80_sbctl1, 0xc);
    IFTA80_E2T_02_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(i80_sbctl2, 0xc);
    IFTA80_E2T_03_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(i80_sbctl3, 0xc);
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(i90_sbctl0, 0xc);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(i90_sbctl1, 0xc);
    M_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(m_sbctl, 0xc);

    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(i90_sbctl0, 0x12);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(i90_sbctl1, 0x12);
    M_E2T_00_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(m_sbctl, 0x12);

    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(i90_sbctl0, 0x18);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(i90_sbctl1, 0x18);
    M_E2T_00_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(m_sbctl, 0x18);

    M_E2T_00_SHARED_BANKS_CONTROLm_B4_HASH_OFFSETf_SET(m_sbctl, 0x1e);
    M_E2T_00_SHARED_BANKS_CONTROLm_B5_HASH_OFFSETf_SET(m_sbctl, 0x26);
    M_E2T_00_SHARED_BANKS_CONTROLm_B6_HASH_OFFSETf_SET(m_sbctl, 0x2c);
    M_E2T_00_SHARED_BANKS_CONTROLm_B7_HASH_OFFSETf_SET(m_sbctl, 0x32);

    ioerr += bcmdi_pt_write(unit, IFTA30_E2T_00_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&i30_sbctl0);
    ioerr += bcmdi_pt_write(unit, EFTA30_E2T_00_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&e30_sbctl0);
    ioerr += bcmdi_pt_write(unit, IFTA40_E2T_00_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&i40_sbctl0);
    ioerr += bcmdi_pt_write(unit, IFTA40_E2T_01_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&i40_sbctl1);
    ioerr += bcmdi_pt_write(unit, IFTA80_E2T_00_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&i80_sbctl0);
    ioerr += bcmdi_pt_write(unit, IFTA80_E2T_01_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&i80_sbctl1);
    ioerr += bcmdi_pt_write(unit, IFTA80_E2T_02_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&i80_sbctl2);
    ioerr += bcmdi_pt_write(unit, IFTA80_E2T_03_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&i80_sbctl3);
    ioerr += bcmdi_pt_write(unit, IFTA90_E2T_00_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&i90_sbctl0);
    ioerr += bcmdi_pt_write(unit, IFTA90_E2T_01_SHARED_BANKS_CONTROLm, 0, -1,
                            (uint32_t *)&i90_sbctl1);
    if (bcmdrd_pt_index_valid(unit, M_E2T_00_SHARED_BANKS_CONTROLm, -1, 0)) {
        ioerr += bcmdi_pt_write(unit, M_E2T_00_SHARED_BANKS_CONTROLm, 0, -1,
                                (uint32_t *)&m_sbctl);
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

/*
 * Bit0 : MACDA_BC
 * Bit1 : MACDA_MC
 * Bit2 : MACDA_IPMC
 * Bit3 : MACSA_ZERO
 * Bit4 : DIP_BC
 * Bit5 : DIP_MC
 * Bit6 : DIP_RESERVED_MC
 * Bit7 : DIP_ADDRESS_ERROR (includes all martian addresses like loopback/all zeros etc)
 * Bit8 : SIP_ADDRESS_ERROR (includes all martian addresses like loopback/all zeros etc)
 * Bit9 : DIP_BFD_LOOPBACK
 * Bit10 : RESERVED
 * Bit11 : RESERVED
 * Bit12 : RESERVED or IP_SMALL_PKT
 * Bit13 : RESERVED or IP_BIG_PKT
 *
 * PT tables:
 * FIXED_HVE_IPARSER1_IPV4_DIP_TABLE
 * FIXED_HVE_IPARSER1_IPV4_SIP_TABLE
 * FIXED_HVE_IPARSER1_IPV6_DIP_TABLE
 * FIXED_HVE_IPARSER1_IPV6_SIP_TABLE
 * FIXED_HVE_IPARSER1_MACDA_TABLE
 * FIXED_HVE_IPARSER1_MACSA_TABLE
 *
 * FIXED_HVE_IPARSER2_IPV4_DIP_TABLE
 * FIXED_HVE_IPARSER2_IPV4_SIP_TABLE
 * FIXED_HVE_IPARSER2_IPV6_DIP_TABLE
 * FIXED_HVE_IPARSER2_IPV6_SIP_TABLE
 * FIXED_HVE_IPARSER2_MACDA_TABLE
 * FIXED_HVE_IPARSER2_MACSA_TABLE
 */
static int
fixed_hve_iparser_init(int unit)
{
    int ioerr = 0;
    uint32_t val[4];
    uint32_t msk[4];

    FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm_t ipv4_dip1;
    FIXED_HVE_IPARSER1_IPV4_SIP_TABLEm_t ipv4_sip1;
    FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm_t ipv6_dip1;
    FIXED_HVE_IPARSER1_IPV6_SIP_TABLEm_t ipv6_sip1;
    FIXED_HVE_IPARSER1_MACDA_TABLEm_t macda1;
    FIXED_HVE_IPARSER1_MACSA_TABLEm_t macsa1;

    FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm_t ipv4_dip2;
    FIXED_HVE_IPARSER2_IPV4_SIP_TABLEm_t ipv4_sip2;
    FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm_t ipv6_dip2;
    FIXED_HVE_IPARSER2_IPV6_SIP_TABLEm_t ipv6_sip2;
    FIXED_HVE_IPARSER2_MACDA_TABLEm_t macda2;
    FIXED_HVE_IPARSER2_MACSA_TABLEm_t macsa2;

    if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
        return SHR_E_NONE;
    }

    /* FIXED_HVE_IPARSER1 tables init */
    FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm_CLR(ipv4_dip1);
    FIXED_HVE_IPARSER1_IPV4_SIP_TABLEm_CLR(ipv4_sip1);
    FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm_CLR(ipv6_dip1);
    FIXED_HVE_IPARSER1_IPV6_SIP_TABLEm_CLR(ipv6_sip1);
    FIXED_HVE_IPARSER1_MACDA_TABLEm_CLR(macda1);
    FIXED_HVE_IPARSER1_MACSA_TABLEm_CLR(macsa1);

    FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm_CLR(ipv4_dip2);
    FIXED_HVE_IPARSER2_IPV4_SIP_TABLEm_CLR(ipv4_sip2);
    FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm_CLR(ipv6_dip2);
    FIXED_HVE_IPARSER2_IPV6_SIP_TABLEm_CLR(ipv6_sip2);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_CLR(macda2);
    FIXED_HVE_IPARSER2_MACSA_TABLEm_CLR(macsa2);

    /* Bit 0 MACDA_BC */
    val[1] = 0xffff;
    val[0] = 0xffffffff;
    FIXED_HVE_IPARSER1_MACDA_TABLEm_VALUEf_SET(macda1, val);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_VALUEf_SET(macda2, val);
    msk[1] = 0xffff;
    msk[0] = 0xffffffff;
    FIXED_HVE_IPARSER1_MACDA_TABLEm_MASKf_SET(macda1, msk);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_MASKf_SET(macda2, msk);
    FIXED_HVE_IPARSER1_MACDA_TABLEm_ADDRESS_TYPEf_SET(macda1, 0x1);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_ADDRESS_TYPEf_SET(macda2, 0x1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_MACDA_TABLEm, 0, -1,
                            (uint32_t *)&macda1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_MACDA_TABLEm, 0, -1,
                            (uint32_t *)&macda2);

    /* Bit 1 MACDA_MC */
    val[1] = 0x0100;
    val[0] = 0x00000000;
    FIXED_HVE_IPARSER1_MACDA_TABLEm_VALUEf_SET(macda1, val);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_VALUEf_SET(macda2, val);
    msk[1] = 0x0100;
    msk[0] = 0x00000000;
    FIXED_HVE_IPARSER1_MACDA_TABLEm_MASKf_SET(macda1, msk);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_MASKf_SET(macda2, msk);
    FIXED_HVE_IPARSER1_MACDA_TABLEm_ADDRESS_TYPEf_SET(macda1, 0x2);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_ADDRESS_TYPEf_SET(macda2, 0x2);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_MACDA_TABLEm, 1, -1,
                            (uint32_t *)&macda1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_MACDA_TABLEm, 1, -1,
                            (uint32_t *)&macda2);

    /* Bit 2 MACDA_IPMC (IPv4) */
    val[1] = 0x0100;
    val[0] = 0x5e000000;
    FIXED_HVE_IPARSER1_MACDA_TABLEm_VALUEf_SET(macda1, val);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_VALUEf_SET(macda2, val);
    msk[1] = 0xffff;
    msk[0] = 0xff800000;
    FIXED_HVE_IPARSER1_MACDA_TABLEm_MASKf_SET(macda1, msk);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_MASKf_SET(macda2, msk);
    FIXED_HVE_IPARSER1_MACDA_TABLEm_ADDRESS_TYPEf_SET(macda1, 0x4);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_ADDRESS_TYPEf_SET(macda2, 0x4);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_MACDA_TABLEm, 2, -1,
                            (uint32_t *)&macda1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_MACDA_TABLEm, 2, -1,
                            (uint32_t *)&macda2);

    /* Bit 2 MACDA_IPMC (IPv6) */
    val[1] = 0x3333;
    val[0] = 0x00000000;
    FIXED_HVE_IPARSER1_MACDA_TABLEm_VALUEf_SET(macda1, val);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_VALUEf_SET(macda2, val);
    msk[1] = 0xffff;
    msk[0] = 0x00000000;
    FIXED_HVE_IPARSER1_MACDA_TABLEm_MASKf_SET(macda1, msk);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_MASKf_SET(macda2, msk);
    FIXED_HVE_IPARSER1_MACDA_TABLEm_ADDRESS_TYPEf_SET(macda1, 0x4);
    FIXED_HVE_IPARSER2_MACDA_TABLEm_ADDRESS_TYPEf_SET(macda2, 0x4);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_MACDA_TABLEm, 3, -1,
                            (uint32_t *)&macda1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_MACDA_TABLEm, 3, -1,
                            (uint32_t *)&macda2);

    /* Bit 3 MACSA_ZERO */
    val[1] = 0x0000;
    val[0] = 0x00000000;
    FIXED_HVE_IPARSER1_MACSA_TABLEm_VALUEf_SET(macsa1, val);
    FIXED_HVE_IPARSER2_MACSA_TABLEm_VALUEf_SET(macsa2, val);
    msk[1] = 0xffff;
    msk[0] = 0xffffffff;
    FIXED_HVE_IPARSER1_MACSA_TABLEm_MASKf_SET(macsa1, msk);
    FIXED_HVE_IPARSER2_MACSA_TABLEm_MASKf_SET(macsa2, msk);
    FIXED_HVE_IPARSER1_MACSA_TABLEm_ADDRESS_TYPEf_SET(macsa1, 0x8);
    FIXED_HVE_IPARSER2_MACSA_TABLEm_ADDRESS_TYPEf_SET(macsa2, 0x8);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_MACSA_TABLEm, 0, -1,
                            (uint32_t *)&macsa1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_MACSA_TABLEm, 0, -1,
                            (uint32_t *)&macsa2);

    /* Bit 4 DIP_BC (IPv4) */
    val[0] = 0xffffffff;
    FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm_VALUEf_SET(ipv4_dip1, val[0]);
    FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm_VALUEf_SET(ipv4_dip2, val[0]);
    msk[0] = 0xffffffff;
    FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm_MASKf_SET(ipv4_dip1, msk[0]);
    FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm_MASKf_SET(ipv4_dip2, msk[0]);
    FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm_ADDRESS_TYPEf_SET(ipv4_dip1, 0x10);
    FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm_ADDRESS_TYPEf_SET(ipv4_dip2, 0x10);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm, 0, -1,
                            (uint32_t *)&ipv4_dip1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm, 0, -1,
                            (uint32_t *)&ipv4_dip2);

    /* Bit 5 DIP_MC (IPv4) */
    val[0] = 0xe0000000;
    FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm_VALUEf_SET(ipv4_dip1, val[0]);
    FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm_VALUEf_SET(ipv4_dip2, val[0]);
    msk[0] = 0xf0000000;
    FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm_MASKf_SET(ipv4_dip1, msk[0]);
    FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm_MASKf_SET(ipv4_dip2, msk[0]);
    FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm_ADDRESS_TYPEf_SET(ipv4_dip1, 0x20);
    FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm_ADDRESS_TYPEf_SET(ipv4_dip2, 0x20);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_IPV4_DIP_TABLEm, 1, -1,
                            (uint32_t *)&ipv4_dip1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_IPV4_DIP_TABLEm, 1, -1,
                            (uint32_t *)&ipv4_dip2);

    /* Bit 5 DIP_BC (IPv6) */
    val[3] = 0xffffffff;
    val[2] = 0xffffffff;
    val[1] = 0xffffffff;
    val[0] = 0xffffffff;
    FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm_VALUEf_SET(ipv6_dip1, val);
    FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm_VALUEf_SET(ipv6_dip2, val);
    msk[3] = 0xffffffff;
    msk[2] = 0xffffffff;
    msk[1] = 0xffffffff;
    msk[0] = 0xffffffff;
    FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm_MASKf_SET(ipv6_dip1, msk);
    FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm_MASKf_SET(ipv6_dip2, msk);
    FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm_ADDRESS_TYPEf_SET(ipv6_dip1, 0x10);
    FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm_ADDRESS_TYPEf_SET(ipv6_dip2, 0x10);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm, 0, -1,
                            (uint32_t *)&ipv6_dip1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm, 0, -1,
                            (uint32_t *)&ipv6_dip2);

    /* Bit 5 DIP_MC (IPv6) */
    val[3] = 0xff000000;
    val[2] = 0x00000000;
    val[1] = 0x00000000;
    val[0] = 0x00000000;
    FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm_VALUEf_SET(ipv6_dip1, val);
    FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm_VALUEf_SET(ipv6_dip2, val);
    msk[3] = 0xff000000;
    msk[2] = 0x00000000;
    msk[1] = 0x00000000;
    msk[0] = 0x00000000;
    FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm_MASKf_SET(ipv6_dip1, msk);
    FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm_MASKf_SET(ipv6_dip2, msk);
    FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm_ADDRESS_TYPEf_SET(ipv6_dip1, 0x20);
    FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm_ADDRESS_TYPEf_SET(ipv6_dip2, 0x20);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_IPV6_DIP_TABLEm, 1, -1,
                            (uint32_t *)&ipv6_dip1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_IPV6_DIP_TABLEm, 1, -1,
                            (uint32_t *)&ipv6_dip2);

    /* Bit 6 - 13 are not required for now */
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_IPV4_SIP_TABLEm, 1, -1,
                            (uint32_t *)&ipv4_sip1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_IPV4_SIP_TABLEm, 1, -1,
                            (uint32_t *)&ipv4_sip2);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER1_IPV6_SIP_TABLEm, 1, -1,
                            (uint32_t *)&ipv6_sip1);
    ioerr += bcmdi_pt_write(unit, FIXED_HVE_IPARSER2_IPV6_SIP_TABLEm, 1, -1,
                            (uint32_t *)&ipv6_sip2);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
ipep_misc_init(int unit)
{
    int port;
    bcmdrd_pbmp_t pbmp;
    int ioerr = 0;
    uint32_t bmp[5] = {0};
    bool sim = bcmdrd_feature_is_sim(unit);
    IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm_t ccbi_ing_dst_port;
    EPOST_EGR_PORT_MIN_PKT_SIZEm_t ep_min_pkt_sz;
    IPOST_MPB_CCBI_FIXED_UNICAST_DROP_CONFIGr_t ccbi_fixed_uc_drop_r;

    if (sim) {
        bmp[0] = 0xffffffff;
        bmp[1] = 0xffffffff;
        bmp[2] = 0xffffffff;
        bmp[3] = 0xffffffff;
        bmp[4] = 0xffffffff;
    }
    IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm_CLR(ccbi_ing_dst_port);
    IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm_PORT_BITMAPf_SET(ccbi_ing_dst_port, bmp);
    ioerr += bcmdi_pt_write(unit, IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm, 0, -1,
                            (uint32_t *)&ccbi_ing_dst_port);

    IPOST_MPB_CCBI_FIXED_UNICAST_DROP_CONFIGr_CLR(ccbi_fixed_uc_drop_r);
    IPOST_MPB_CCBI_FIXED_UNICAST_DROP_CONFIGr_DST_PORT_NUMf_SET(ccbi_fixed_uc_drop_r, 0x1);
    ioerr += bcmdi_pt_write(unit, IPOST_MPB_CCBI_FIXED_UNICAST_DROP_CONFIGr, 0, -1,
                            (uint32_t *)&ccbi_fixed_uc_drop_r);

    ioerr += bcmdrd_dev_logic_pbmp(unit, &pbmp);
    if (SHR_FAILURE(ioerr)) {
        return ioerr;
    }

    /* Set EPOST_EGR_PORT_MIN_PKT_SIZE to 0x40(64B) */
    EPOST_EGR_PORT_MIN_PKT_SIZEm_CLR(ep_min_pkt_sz);
    EPOST_EGR_PORT_MIN_PKT_SIZEm_MIN_PKT_SIZEf_SET(ep_min_pkt_sz, 0x40);
    BCMDRD_PBMP_ITER(pbmp, port) {
       ioerr += bcmdi_pt_write(unit, EPOST_EGR_PORT_MIN_PKT_SIZEm, port, -1,
                               (uint32_t *)&ep_min_pkt_sz);
    }

    return ioerr;
}

static int
ipep_urpf_bitp_botp_init(int unit)
{
    int ioerr = 0;
    URPF_BITP_PROFILEm_t bitp_buf;

    /* URPF BITP programming */
    URPF_BITP_PROFILEm_CLR(bitp_buf);
    ioerr += bcmdi_pt_read(unit, URPF_BITP_PROFILEm, 0, -1,
                           (uint32_t *)&bitp_buf);
    URPF_BITP_PROFILEm_URPF_CHECK_ENf_SET(bitp_buf, 1);
    ioerr += bcmdi_pt_write(unit, URPF_BITP_PROFILEm, 0, -1,
                            (uint32_t *)&bitp_buf);

    return ioerr;
}

static int
bcm56780_a0_di_devicecode_extra(int unit) {

    /* Call sub device init routines here if needed*/

    return SHR_E_NONE;

}

static int
bcm56780_a0_di_devicecode_extra_post(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Call sub device init routines here if needed */

    /* SF table init */
    SHR_IF_ERR_EXIT
        (fixed_hve_iparser_init(unit));
    SHR_IF_ERR_EXIT
        (ipep_misc_init(unit));
    SHR_IF_ERR_EXIT
        (ipep_urpf_bitp_botp_init(unit));
    SHR_IF_ERR_EXIT
        (uft_hash_init(unit));

exit:
    SHR_FUNC_EXIT();

}

static uint32_t
bcm56780_a0_di_devicecode_buff_base(int unit)
{
#define TD4_DEV_INFO_DATA_BASE   0x600
#define TD4_DEV_INFO_DATA_OFFSET 0x8
#define TD4_DEV_INFO_DATA_SHIFT  0x2
    return CMIC_SCHAN_MESSAGEr_OFFSET + TD4_DEV_INFO_DATA_BASE +
           (((TD4_DEV_INFO_DATA_OFFSET << (TD4_DEV_INFO_DATA_SHIFT - 1)) +
           TD4_DEV_INFO_DATA_SHIFT - 1) << TD4_DEV_INFO_DATA_SHIFT);
}


/*******************************************************************************
 * Public Functions
 */
int
bcm56780_a0_di_dvc_drv_attach(int unit, bcmdi_dvc_drv_t *drv)
{

    drv->file = &bcm56780_a0_di_devicecode_file;
    drv->extra = &bcm56780_a0_di_devicecode_extra;
    drv->extra_post = &bcm56780_a0_di_devicecode_extra_post;
    drv->buff_base = &bcm56780_a0_di_devicecode_buff_base;

    return SHR_E_NONE;
}

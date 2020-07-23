/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mcs.c
 * Purpose:     MCS initialize and load utilities
 * Requires:
 */


#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/error.h>
#include <shared/util.h>
#include <soc/uc_msg.h>

#include <soc/uc_dbg.h>
#if defined(BCM_JERICHO_SUPPORT)
#include <soc/dpp/JER/jer_drv.h>
#endif

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)

#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#define CORE1_LUT_ADDR          0xffff042c
#endif

#define _SYSTEM_UC_MSG_TIMEOUT       5000000  /* 5 seconds for FW response */

uint32
soc_uc_mem_read(int unit, uint32 addr)
{
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_mcs)) {
        return soc_pci_mcs_read(unit, addr);
    }
#endif
#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_iproc)) {
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
        if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit)) {
            return soc_pci_mcs_read(unit, addr);
        } else
#endif /* BCM_HURRICANE2_SUPPORT || BCM_HURRICANE3_SUPPORT */
        return soc_cm_iproc_read(unit, addr);
    }
#endif
    assert(0);
    return 0;
}

int
soc_uc_mem_write(int unit, uint32 addr, uint32 value)
{
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_mcs)) {
        return soc_pci_mcs_write(unit, addr, value);
    }
#endif
#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_iproc)) {
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
        if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit)) {
            return soc_pci_mcs_write(unit, addr, value);
        } else
#endif /* BCM_HURRICANE2_SUPPORT || BCM_HURRICANE3_SUPPORT */
        soc_cm_iproc_write(unit, addr, value);
        return SOC_E_NONE;
    }
#endif
    assert(0);
    return SOC_E_FAIL;
}

int
soc_uc_addr_to_pcie(int unit, int uC, uint32 addr)
{
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_mcs)) {
        /* If in TCM space */
        if (addr < 0x100000) {
            /* TCMs base addrs from PCIe perspective */
            addr += (uC == 0) ? 0x100000 : 0x200000;
        }
        return addr;
    }
#endif

#if defined(BCM_UC_MHOST_SUPPORT)
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        uint32 atcm_base = 0x01000000 + 0x100000 * uC;
        uint32 btcm_off = 0x80000;

#ifdef BCM_GREYHOUND2_SUPPORT
        if (SOC_IS_GREYHOUND2(unit)) {
            btcm_off = 0x20000;
        }
#endif

#ifdef BCM_SABER2_SUPPORT
        if (SOC_IS_SABER2(unit) && !SOC_IS_METROLITE(unit)) {
            if (uC == 2) {
                atcm_base = 0x01160000;
            }
            btcm_off = 0x20000;
        }
#endif

#if defined(BCM_APACHE_SUPPORT) || defined (BCM_TOMAHAWK2_SUPPORT) || \
    defined (BCM_TOMAHAWK3_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_APACHE(unit) || SOC_IS_TOMAHAWK2(unit) ||
            SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit)) {
            atcm_base = 0x01100000 + 0x00060000 * uC;
            btcm_off = 0x20000;
        }
#endif

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || \
    defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit) ||
            SOC_IS_FIRELIGHT(unit)) {
            atcm_base = 0x01100000 + 0x00060000 * uC;
            btcm_off = 0x20000;
        }
#endif

#if defined(BCM_DNX_SUPPORT)
        if (SOC_IS_DNX(unit)) {
            atcm_base = 0x01100000 + 0x00060000 * uC;
            btcm_off = 0x20000;
        }
#endif

#ifdef BCM_MONTEREY_SUPPORT
        /* Monterey, uc:2 is M7 */
        if (SOC_IS_MONTEREY(unit) && (2 == uC)) {
            atcm_base = 0x3260000;
            btcm_off  = 0x3264000;
        }
#endif
        /* If in TCM space */
        if (addr < 0x100000) {
            if (addr < 0x40000) {
                /* ATCM address from host perspective*/
                return atcm_base + addr;
            }
            if (addr >= 0x40000 && addr < 0x80000) {
                /* BTCM address from host perspective*/
                return atcm_base + btcm_off + addr - 0x40000;
            }
            assert(0);
        }
        return addr;
    }
#endif /* BCM_UC_MHOST_SUPPORT */

#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_iproc)) {
        return addr;
    }
#endif

    assert(0);
    return 0;
}

int
soc_uc_sram_extents(int unit, uint32 *addr, uint32 *size)
/*
 * Function: 	soc_uc_sram_extents
 * Purpose:	Get the SRAM address and length
 * Parameters:	unit - unit number
 * Returns:	SOC_E_xxx
 */
{
    *size = 0;
    *addr = 0;

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        *size = 0x00100000;
        *addr = 0x00400000;
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        *size = 0x00080000;
        *addr = 0x00400000;
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        *size = 0x00080000;
        *addr = 0x00400000;
    }
#endif
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        *size = 0x00080000;
        *addr = 0x00400000;
    }
#endif
#ifdef BCM_HELIX4_SUPPORT
    if (SOC_IS_HELIX4(unit)) {
        *size = 512 * 1024;
        *addr = 0x1b000000;
    }
#endif
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        *size = 512 * 1024;
        *addr = 0x1b000000;
    }
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        *size = 1 * 1024 * 1024;
        *addr = 0x00000000;
    }
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        *size = 1 * 1024 * 1024;
        *addr = 0x00000000;
    }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)) {
        *size = 64 * 1024;
        *addr = 0x01000000;
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK(unit)) {
        *size = 1 * 1024 * 1024;
        *addr = 0x1b000000;
    }
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TRIDENT2PLUS(unit)) {
        *size = 1 * 1024 * 1024;
        *addr = 0x00400000;
    }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        *size = 1 * 1024 * 1024;
        *addr = 0x01200000;
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        *size = 1 * 1024 * 1024;
        *addr = 0x01200000;
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)) {
        *size = 1 * 1024 * 1024;
        *addr = 0x01200000;
    }
#endif
#ifdef BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        *size = 1 * 1024 * 1024;
        *addr = 0x01200000;
    }
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        /* 128K SRAM in M0SSQ */
        *size = 128 * 1024;
        *addr = 0x10040000;
    }
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        /* 128K SRAM in M0SSQ */
        *size = 128 * 1024;
        *addr = 0x10040000;
    }
#endif
#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        *size = 1 * 1024 * 1024;
        *addr = 0x01200000;
    }
#endif
    return (SOC_E_NONE);
}


#ifdef BCM_CMICM_SUPPORT

static
int
soc_uc_mcs_init(int unit)
/*
 * Function: 	soc_uc_msc_init
 * Purpose:	Initialize the CMICm MCS
 * Parameters:	unit - unit number
 * Returns:	SOC_E_xxx
 */
{
    int rc;
    uint32 sram_size, sram_addr, i;

    rc = soc_uc_sram_extents(unit, &sram_addr, &sram_size);
    if (rc != SOC_E_NONE) {
        return (rc);
    }

    i = 0;
    while (i < sram_size) {
        soc_uc_mem_write(unit, sram_addr + i, 0x12345678);
        i += 4;
        if ((i & 0x0ffff) == 0) {
            sal_usleep(1000);
        }
    }
    return (SOC_E_NONE);
}

static
int
soc_uc_mcs_reset(int unit, int uC)
/*
 * Function: 	soc_uc_mcs_reset
 * Purpose:	Put an MCS into reset state
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_xxxx
 */
{
    uint32      config, rst_control;

    /* Put the uC in LE mode */
    if (uC == 0) {
        READ_UC_0_CONFIGr(unit, &config);
        soc_reg_field_set(unit, UC_0_CONFIGr, &config,
                          OVERWRITE_OTP_CONFIGf, 1);
        soc_reg_field_set(unit, UC_0_CONFIGr, &config,
                          CFGIEf, 1);
        soc_reg_field_set(unit, UC_0_CONFIGr, &config,
                          CFGEEf, 1);
        WRITE_UC_0_CONFIGr(unit, config);
    } else {
        READ_UC_1_CONFIGr(unit, &config);
        soc_reg_field_set(unit, UC_1_CONFIGr, &config,
                          OVERWRITE_OTP_CONFIGf, 1);
        soc_reg_field_set(unit, UC_1_CONFIGr, &config,
                          CFGIEf, 1);
        soc_reg_field_set(unit, UC_1_CONFIGr, &config,
                          CFGEEf, 1);
        WRITE_UC_1_CONFIGr(unit, config);
    }

    /* Take the processor in and out of reset but halted */
    if (uC == 0) {
        READ_UC_0_RST_CONTROLr(unit, &rst_control);
        soc_reg_field_set(unit, UC_0_RST_CONTROLr, &rst_control,
                          BYPASS_RSTFSM_CTRLf, 1);
        soc_reg_field_set(unit, UC_0_RST_CONTROLr, &rst_control,
                          CPUHALT_Nf, 0);
        WRITE_UC_0_RST_CONTROLr(unit, rst_control);
        sal_usleep(1000);
        READ_UC_0_RST_CONTROLr(unit, &rst_control);
        soc_reg_field_set(unit, UC_0_RST_CONTROLr, &rst_control,
                          SYS_PORESET_Nf, 0);
        soc_reg_field_set(unit, UC_0_RST_CONTROLr, &rst_control,
                          CORE_RESET_Nf, 0);
        WRITE_UC_0_RST_CONTROLr(unit, rst_control);
        soc_reg_field_set(unit, UC_0_RST_CONTROLr, &rst_control,
                          SYS_PORESET_Nf, 1);
        soc_reg_field_set(unit, UC_0_RST_CONTROLr, &rst_control,
                          CORE_RESET_Nf, 1);
        WRITE_UC_0_RST_CONTROLr(unit, rst_control);
    } else {
        READ_UC_1_RST_CONTROLr(unit, &rst_control);
        soc_reg_field_set(unit, UC_1_RST_CONTROLr, &rst_control,
                          CPUHALT_Nf, 0);
        WRITE_UC_1_RST_CONTROLr(unit, rst_control);
        sal_usleep(1000);
        READ_UC_1_RST_CONTROLr(unit, &rst_control);
        soc_reg_field_set(unit, UC_1_RST_CONTROLr, &rst_control,
                          SYS_PORESET_Nf, 0);
        soc_reg_field_set(unit, UC_1_RST_CONTROLr, &rst_control,
                          CORE_RESET_Nf, 0);
        WRITE_UC_1_RST_CONTROLr(unit, rst_control);
        soc_reg_field_set(unit, UC_1_RST_CONTROLr, &rst_control,
                          SYS_PORESET_Nf, 1);
        soc_reg_field_set(unit, UC_1_RST_CONTROLr, &rst_control,
                          CORE_RESET_Nf, 1);
        WRITE_UC_1_RST_CONTROLr(unit, rst_control);
    }

    return (SOC_E_NONE);
}

static
int
soc_uc_mcs_in_reset(int unit, int uC)
/*
 * Function: 	soc_uc_mcs_in_reset
 * Purpose:	Return 1 if in RESET state, 0 otherwise
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	1 or 0
 */
{
    uint32 rst, halt, ctrl;

    if (uC == 0) {
        READ_UC_0_RST_CONTROLr(unit, &ctrl);
        rst = soc_reg_field_get(unit, UC_0_RST_CONTROLr, ctrl, CORE_RESET_Nf);
        halt = soc_reg_field_get(unit, UC_0_RST_CONTROLr, ctrl, CPUHALT_Nf);
    }
    else {
        READ_UC_1_RST_CONTROLr(unit, &ctrl);
        rst = soc_reg_field_get(unit, UC_1_RST_CONTROLr, ctrl, CORE_RESET_Nf);
        halt = soc_reg_field_get(unit, UC_1_RST_CONTROLr, ctrl, CPUHALT_Nf);
    }
    /* If either RESET or HALT is asserted (0), dev is in RESET */
    return (rst == 0) || (halt == 0);
}

static
int
soc_uc_mcs_start(int unit, int uC, uint32 addr)
/*
 * Function: 	soc_uc_mcs_start
 * Purpose:	Start MCS execution
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 *              addr - address to start at
 * Returns:	SOC_E_xxx
 */
{
    uint32      rst_control;

    if (uC == 0) {
        READ_UC_0_RST_CONTROLr(unit, &rst_control);
        soc_reg_field_set(unit, UC_0_RST_CONTROLr, &rst_control,
                          CPUHALT_Nf, 1);
        WRITE_UC_0_RST_CONTROLr(unit, rst_control);
    } else {
        READ_UC_1_RST_CONTROLr(unit, &rst_control);
        soc_reg_field_set(unit, UC_1_RST_CONTROLr, &rst_control,
                          CPUHALT_Nf, 1);
        WRITE_UC_1_RST_CONTROLr(unit, rst_control);
    }

    return (SOC_E_NONE);
}

#endif /* BCM_CMICM_SUPPORT */

#ifdef BCM_IPROC_SUPPORT

/*
 * Function:    soc_uc_intr_disable
 * Purpose:  Disable UC interrupt
 * Parameters:  unit  - unit number
 *              reg   - interrupt register
 *              field - field in register
 * Returns: SOC_E_xxxx
 */
static
int
soc_uc_intr_disable(int unit,
                    soc_reg_t reg,
                    soc_field_t field)
{
    uint32 regval;
    if (soc_reg_field_valid(unit, reg, field)) {
        soc_pci_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), &regval);
        if (soc_reg_field_get(unit, reg, regval, TIMESYNC_INTRf) != 0) {
            soc_reg_field_set(unit, reg, &regval, TIMESYNC_INTRf, 0);
            soc_pci_write(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), regval);

            /* Give core time to complete processing an existing interrupt */
            sal_usleep(1000);
        }
    }

    return 0;
}

/*
 * Function: 	soc_uc_iproc_init
 * Purpose:	Initialize iProc/MCS
 * Parameters:	unit - unit number
 * Returns:	SOC_E_xxxx
 */
static
int
soc_uc_iproc_init(int unit)
{
    int rc;
    uint32 sram_size, sram_addr, i;

#ifdef BCM_IPROC_DDR_SUPPORT
    if (soc_feature(unit, soc_feature_iproc_ddr)) {
        soc_iproc_ddr_init(unit);
    }
#endif /* BCM_IPROC_DDR_SUPPORT */

    rc = soc_uc_sram_extents(unit, &sram_addr, &sram_size);
    if (rc != SOC_E_NONE) {
        return (rc);
    }

    i = 0;
    while (i < sram_size) {
        soc_uc_mem_write(unit, sram_addr + i, 0);
        i += 4;
        if ((i & 0x0ffff) == 0) {
            sal_usleep(1000);
        }
    }

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        /* JIRA CMICD-110:  The last 48 bytes of SRAM are used to cache the values for
         * CMIC_BS0_ and CMIC_BS1_ registers.  So write the current register values there
         */
        uint32 rval;

        /* store reset values in SRAM for BS0 */
        uint32 sram_saved_regs = sram_addr + 0x7ffd0;            /* end of 512K memory */
        READ_CMIC_BS0_CONFIGr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 0, rval);       /* CONFIG: mode, output enables */
        READ_CMIC_BS0_CLK_CTRLr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 4, rval);       /* CLK_CTRL: enable, bitclk high/low */
        READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 8, rval);       /* HEARBEAT_CTRL: HB enable */
        READ_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 12, 0x3ffffff); /* HEARTBEAT_DOWN_DURATION */
        READ_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 16, 0x3ffffff); /* HEARTBEAT_UP_DURATION */

        /* store reset values in SRAM for BS1 */
        sram_saved_regs += 20;
        READ_CMIC_BS1_CONFIGr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 0, rval);       /* CONFIG: mode, output enables */
        READ_CMIC_BS1_CLK_CTRLr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 4, rval);       /* CLK_CTRL: enable, bitclk high/low */
        READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 8, rval);       /* HEARBEAT_CTRL: HB enable */
        READ_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 12, 0x3ffffff); /* HEARTBEAT_DOWN_DURATION */
        READ_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, &rval);
        soc_uc_mem_write(unit, sram_saved_regs + 16, 0x3ffffff); /* HEARTBEAT_UP_DURATION */
    }
#endif

    return (SOC_E_NONE);
}

static
int
soc_uc_iproc_in_reset(int unit, int uC)
/*
 * Function: 	soc_uc_mcs_in_reset
 * Purpose:	Return 1 if in RESET state, 0 otherwise
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	1 or 0
 */
{
    uint32 rst;
    uint32 regval;

    READ_IHOST_PROC_RST_A9_CORE_SOFT_RSTNr(unit, &regval);
    if (uC == 0) {
        rst = soc_reg_field_get(unit, IHOST_PROC_RST_A9_CORE_SOFT_RSTNr,
                                regval, A9_CORE_0_SOFT_RSTNf);
    }
    else {
        rst = soc_reg_field_get(unit, IHOST_PROC_RST_A9_CORE_SOFT_RSTNr,
                                regval, A9_CORE_1_SOFT_RSTNf);
    }
    return (rst == 0);
}

/*
 * Function: 	soc_uc_iproc_reset
 * Purpose:	Put an IPROC into reset state
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_xxxx
 */
static
int
soc_uc_iproc_reset(int unit, int uC)
{
    int         rc;
    uint32      i;
    uint32      sram_base, sram_size;
    uint32      iproc_addr;
    uint32      regval;

    if (!soc_feature(unit, soc_feature_iproc)) {
        return (SOC_E_FAIL);
    }

    if ((SOC_REG_IS_VALID(unit, CRU_IHOST_PWRDWN_STATUSr)) &&
        (SOC_REG_FIELD_VALID(unit, CRU_IHOST_PWRDWN_STATUSr, LOGIC_PWRDOWN_CPU0f))) {
        READ_CRU_IHOST_PWRDWN_STATUSr(unit, &regval);
        if (soc_reg_field_get(unit, CRU_IHOST_PWRDWN_STATUSr, regval, LOGIC_PWRDOWN_CPU0f) == 1) {
            /* if the uC has been shut down, do nothing*/
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "The uC has been shut down, will not reset it.\n")));
            return (SOC_E_FAIL);
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "iproc_reset uC %d\n"), uC));

    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) {
        /* Assumes booting in QSPI mode with MDIO tied high */
        if (uC == 0) {
            rc = soc_uc_sram_extents(unit, &sram_base, &sram_size);
            if (rc != SOC_E_NONE) {
                return (rc);
            }
            iproc_addr = sram_base + sram_size - 8;

            /* load WFI loop into SRAM (ARM mode) */
            soc_cm_iproc_write(unit, iproc_addr, 0xe320f003);
            soc_cm_iproc_write(unit, iproc_addr + 4, 0xeafffffd);

            /* Update LUT to point at WFI loop */
            for (i = 0; i < 8; ++i) {
                soc_cm_iproc_write(unit, 0xffff0400 + i*4, iproc_addr);
            }
            /* core 0 should be in WFI now */
        }
    }

    /* Must ensure that core is not processing an interrupt when reset.
     * BroadSync and PTP enable the Timesync interrupt, so disable it here
     * for whichever CMC / UC it has been enabled on by the FW.
     */
    soc_uc_intr_disable(unit, CMIC_CMC0_UC0_IRQ_MASK1r, TIMESYNC_INTRf);
    soc_uc_intr_disable(unit, CMIC_CMC1_UC0_IRQ_MASK1r, TIMESYNC_INTRf);
    soc_uc_intr_disable(unit, CMIC_CMC2_UC0_IRQ_MASK1r, TIMESYNC_INTRf);
    soc_uc_intr_disable(unit, CMIC_CMC2_UC1_IRQ_MASK1r, TIMESYNC_INTRf);

    /* Now safe to reset the ARM core */
    READ_IHOST_PROC_RST_A9_CORE_SOFT_RSTNr(unit, &regval);
    if (uC == 0) {
        soc_reg_field_set(unit, IHOST_PROC_RST_A9_CORE_SOFT_RSTNr,
                          &regval, A9_CORE_0_SOFT_RSTNf, 0);
        soc_reg_field_set(unit, IHOST_PROC_RST_A9_CORE_SOFT_RSTNr,
                          &regval, A9_CORE_1_SOFT_RSTNf, 0);
    }
    else {
        soc_reg_field_set(unit, IHOST_PROC_RST_A9_CORE_SOFT_RSTNr,
                          &regval, A9_CORE_1_SOFT_RSTNf, 0);
    }
    WRITE_IHOST_PROC_RST_WR_ACCESSr(unit, 0x00a5a501);
    WRITE_IHOST_PROC_RST_A9_CORE_SOFT_RSTNr(unit, regval);
    WRITE_IHOST_PROC_RST_WR_ACCESSr(unit, 0);

    if (soc_reg_field_valid(unit, CMIC_TIMESYNC_INTERRUPT_ENABLEr, INT_ENABLEf)) {
        /* Disable all timesync interrupts in CMIC if any were enabled by FW before */
        WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, 0x0);
    }

    return (SOC_E_NONE);
}

/*
 * Function: 	soc_uc_iproc_l2cache_purge
 * Purpose:	Clean l2 cache
 * Parameters:	unit - unit number
 *		start - start address
 *              len - length
 * Returns:	SOC_E_xxx
 */
static
int
soc_uc_iproc_l2cache_purge(int unit, uint32 start, uint32 len)
{
    uint32 addr;
    uint32 regval;
    uint32 linesize = 32;
    uint32 pcie0;
    uint32 pcie1;

    /* Save config and disable cache */
    if (soc_cm_get_bus_type(unit) & SOC_DEV_BUS_ALT) {
        /* PAXB-1 */
        READ_PAXB_1_PCIE_EP_AXI_CONFIGr(unit, &pcie1);
        WRITE_PAXB_1_PCIE_EP_AXI_CONFIGr(unit, 0);
    } else {
        /* PAXB-0 */
        READ_PAXB_0_PCIE_EP_AXI_CONFIGr(unit, &pcie0);
        WRITE_PAXB_0_PCIE_EP_AXI_CONFIGr(unit, 0);
    }

    READ_IHOST_L2C_CACHE_IDr(unit, &regval);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "iproc_reset L2C_CACHE_ID 0x%08x\n"), regval));
    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
        "iproc_reset L2C_CACHE_ID 0x%08x\n", regval));

    READ_IHOST_L2C_CONTROLr(unit, &regval);
    if (regval & 0x01) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "L2 cache enabled, clean %d bytes at 0x%08x\n"),
                     len, start));
        for (addr = start; addr < (start + len); addr += linesize) {
            WRITE_IHOST_L2C_CLEAN_PAr(unit, addr);
            WRITE_IHOST_L2C_CACHE_SYNCr(unit, addr);
        }
    }
    else {
        /* L2 cache disabled, skip */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "L2 cache disabled 0x%08x\n"), regval));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
            "L2 cache disabled 0x%08x\n", regval));
    }

    /* Restore config values */
    if (soc_cm_get_bus_type(unit) & SOC_DEV_BUS_ALT) {
        WRITE_PAXB_1_PCIE_EP_AXI_CONFIGr(unit, pcie1);
    } else {
        WRITE_PAXB_0_PCIE_EP_AXI_CONFIGr(unit, pcie0);
    }

    return (SOC_E_NONE);
}

/*
 * Function: 	soc_uc_iproc_preload
 * Purpose:	Prepare to load into iProc memory
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_xxx
 */
static
int
soc_uc_iproc_preload(int unit, int uC)
{
    int rc;
    uint32 addr, size;

    rc = soc_uc_sram_extents(unit, &addr, &size);
    if (rc != SOC_E_NONE) {
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
            "%s(): soc_uc_sram_extents failed uC: %d\n",
            FUNCTION_NAME(), uC));
        return (rc);
    }
    return soc_uc_iproc_l2cache_purge(unit, addr, size);
}

/*
 * Function: 	soc_uc_iproc_start
 * Purpose:	Start IPROC execution
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_xxx
 */
static
int
soc_uc_iproc_start(int unit, int uC, uint32 iproc_address)
{
    uint32      i;
    uint32      regval;
    uint32 iproc_addr = iproc_address + (16*1024); /* first 16K is MMU L1 table */

    if (!soc_feature(unit, soc_feature_iproc)) {
        return (SOC_E_FAIL);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "iproc_start uC %d addr 0x%08x\n"), uC, iproc_addr));
    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
        "iproc_start uC %d addr 0x%08x\n", uC, iproc_addr));

    READ_IHOST_PROC_RST_A9_CORE_SOFT_RSTNr(unit, &regval);
    if (uC == 0) {
        /* Update LUT to point at SRAM */
        for (i = 0; i < 8; ++i) {
#if defined(BCM_HURRICANE3_SUPPORT)
            if (SOC_IS_HURRICANE3(unit)) {
                soc_cm_iproc_write(unit, 0xffff0fc0 + i*4, iproc_addr);
            } else
#endif /* BCM_HURRICANE3_SUPPORT */
            soc_cm_iproc_write(unit, 0xffff0400 + i*4, iproc_addr);
        }
        /* Release reset on core 0 */
        soc_reg_field_set(unit, IHOST_PROC_RST_A9_CORE_SOFT_RSTNr,
                          &regval, A9_CORE_0_SOFT_RSTNf, 1);
    }
    else {
        /* Update LUT to point at SRAM */
        soc_cm_iproc_write(unit, CORE1_LUT_ADDR, iproc_addr);
        /* Release reset on core 1 */
        soc_reg_field_set(unit, IHOST_PROC_RST_A9_CORE_SOFT_RSTNr,
                          &regval, A9_CORE_1_SOFT_RSTNf, 1);
    }
    /* Release reset */
    WRITE_IHOST_PROC_RST_WR_ACCESSr(unit, 0x00a5a501);
    WRITE_IHOST_PROC_RST_A9_CORE_SOFT_RSTNr(unit, regval);
    WRITE_IHOST_PROC_RST_WR_ACCESSr(unit, 0);

    if (uC == 1) {
        /* Wait 100ms for core 1 to set up LUT */
        sal_usleep(100 * 1000);

        /* Update LUT to point at SRAM */
        soc_cm_iproc_write(unit, CORE1_LUT_ADDR, iproc_addr);
    }
    return (SOC_E_NONE);
}

#endif /* BCM_IPROC_SUPPORT */

#ifdef BCM_UC_MHOST_SUPPORT

/*
 * Function: 	soc_uc_mhost_init
 * Purpose:	Initialize the UC
 * Parameters:	unit - unit number
 * Returns:	SOC_E_xxx
 */
static int
soc_uc_mhost_init(int unit)
{
#ifdef BCM_IPROC_DDR_SUPPORT
    if (soc_feature(unit, soc_feature_iproc_ddr)) {
        soc_iproc_ddr_init(unit);
    }
#endif /* BCM_IPROC_DDR_SUPPORT */

    return (SOC_E_NONE);
}

/*
 * Function: 	soc_uc_mhost_reset
 * Purpose:	Put the UC into reset state
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_xxxx
 */
static int
soc_uc_mhost_reset(int unit, int uC)
{
#if defined(BCM_JERICHO_SUPPORT)
    uint32 idm_mrst_control;
    uint32 idm_srst_control;
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_QUX_SUPPORT) || \
    defined(BCM_QAX_SUPPORT) || defined(BCM_DNX_SUPPORT) ||\
    defined(BCM_JERICHO_SUPPORT)
    uint32 rst_control;
#endif

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || \
    defined(BCM_DNX_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
    uint32 strap_status;
#endif

#if defined(BCM_SABER2_SUPPORT) || defined(BCM_JERICHO_SUPPORT)
    uint32 rvp = 0;
    uint32 retry_cnt;
    uint32 u_sec = 100;
#endif

#ifdef BCM_SABER2_SUPPORT
    uint16 dev_id;
    uint8  rev_id;
#endif

    if (uC < 0 || uC >= SOC_INFO(unit).num_ucs) {
        return SOC_E_UNAVAIL;
    }

    if (SOC_CONTROL(unit)->uc_hostram_buf[uC] != NULL) {
        soc_cm_sfree(unit, SOC_CONTROL(unit)->uc_hostram_buf[uC]);
        SOC_CONTROL(unit)->uc_hostram_buf[uC] = NULL;
    }

#ifdef BCM_SABER2_SUPPORT
    /* Dagger2 support 2xR5 Embedded processors
     * mHost 0 was disable and mHost 1 and 2 are valid core
     * mHost 0 is inavlid so ignore mHost 0
     */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (SOC_IS_SABER2(unit) && (dev_id == BCM56233_DEVICE_ID) && (uC == 1)) {
        return (SOC_E_NONE);
    }
    /* Lock h/w semaphores used to synchronize uC1 and uC2 */
    if (SOC_IS_SABER2(unit) && !SOC_IS_METROLITE(unit) && uC > 0) {
        /* Lock h/w Sem16 used for schan access */
        retry_cnt = 10000;
        rvp = 0;
        while (retry_cnt--) {
            READ_CMIC_SEMAPHORE_16r(unit, &rvp);
            if(rvp) {
                /* semaphore locked */
                break;
            }
            sal_usleep(u_sec);
        }
        if (!rvp) {
            return SOC_E_BUSY;
        }

        /* Lock h/w Sem17, used for CCMDMA access */
        retry_cnt = 10000; rvp=0;
        while(retry_cnt--) {
            READ_CMIC_SEMAPHORE_17r(unit, &rvp);
            if(rvp) {
                /* semaphore locked */
                break;
            }
            sal_usleep(u_sec);
        }
        if (!rvp) {
            /* Unlock the semaphore 16 */
            WRITE_CMIC_SEMAPHORE_16r(unit, 0);
            return SOC_E_BUSY;
        }
    }
#endif

    /* Must ensure that core-1 is not processing an interrupt when reset.
     * BroadSync and PTP enable the Timesync interrupt, so disable it
     * here for whichever CMC / UC it has been enabled on by the FW.
     */
    if ((1 == uC)
#ifdef BCM_TRIDENT3_SUPPORT
        && (!SOC_IS_TRIDENT3(unit))
#endif
#ifdef BCM_DNX_SUPPORT
        && (!SOC_IS_DNX(unit))
#endif
    ) {
        soc_uc_intr_disable(unit, CMIC_CMC0_UC0_IRQ_MASK1r,
                                TIMESYNC_INTRf);
        soc_uc_intr_disable(unit, CMIC_CMC1_UC0_IRQ_MASK1r,
                                TIMESYNC_INTRf);
        soc_uc_intr_disable(unit, CMIC_CMC2_UC0_IRQ_MASK1r,
                                TIMESYNC_INTRf);
        soc_uc_intr_disable(unit, CMIC_CMC2_UC1_IRQ_MASK1r,
                                TIMESYNC_INTRf);

        if (soc_reg_field_valid(unit, CMIC_TIMESYNC_INTERRUPT_ENABLEr,
                    INT_ENABLEf)) {
            /* Disable all timesync interrupts in CMIC if any were
             * enabled by FW before
             */
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, 0x0);

            /* Give core time to complete processing an existing
             * interrupt if any
             */
            sal_usleep(5000);
        }
    }

#if defined(BCM_JERICHO_SUPPORT)
    if (SOC_IS_JERICHO(unit) && (uC == 0)) {
        /* Handshaking with uC to stop CMIC activity on uC0 */
        retry_cnt = 1000; rvp=0;
        while(retry_cnt--) {
            rvp = soc_getstatus_bcm88x7x(unit, uC);
            if(!rvp) {
                /* uC is safe to reset */
                break;
            }
            sal_usleep(u_sec);
        }

        if (rvp) {
            soc_restore_bcm88x7x(unit, uC);
        }
    }
#endif

#if defined(BCM_JERICHO_SUPPORT)
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QUX(unit) && !SOC_IS_QAX(unit) && !SOC_IS_QMX(unit)) {
        /* Take the processor in and out of reset but halted */
        if (uC == 0) {
            READ_MHOST_M0_IDM_M_IDM_RESET_CONTROLr(unit, &idm_mrst_control);
            READ_MHOST_S0_IDM_S_IDM_RESET_CONTROLr(unit, &idm_srst_control);
        } else if (uC == 1) {
            READ_MHOST_M1_IDM_M_IDM_RESET_CONTROLr(unit, &idm_mrst_control);
            READ_MHOST_S1_IDM_S_IDM_RESET_CONTROLr(unit, &idm_srst_control);
        } else {
            assert(0);
            return SOC_E_UNAVAIL;
        }

        soc_reg_field_set(unit, MHOST_M0_IDM_M_IDM_RESET_CONTROLr, &idm_mrst_control,
                          RESETf, 1);
        soc_reg_field_set(unit, MHOST_S0_IDM_S_IDM_RESET_CONTROLr, &idm_srst_control,
                          RESETf, 1);

        if (uC == 0) {
            WRITE_MHOST_M0_IDM_M_IDM_RESET_CONTROLr(unit, idm_mrst_control);
            WRITE_MHOST_S0_IDM_S_IDM_RESET_CONTROLr(unit, idm_srst_control);
        } else if (uC == 1) {
            WRITE_MHOST_M1_IDM_M_IDM_RESET_CONTROLr(unit, idm_mrst_control);
            WRITE_MHOST_S1_IDM_S_IDM_RESET_CONTROLr(unit, idm_srst_control);
        }

        soc_reg_field_set(unit, MHOST_M0_IDM_M_IDM_RESET_CONTROLr, &idm_mrst_control,
                          RESETf, 0);
        soc_reg_field_set(unit, MHOST_S0_IDM_S_IDM_RESET_CONTROLr, &idm_srst_control,
                          RESETf, 0);

        if (uC == 0) {
            WRITE_MHOST_M0_IDM_M_IDM_RESET_CONTROLr(unit, idm_mrst_control);
            WRITE_MHOST_S0_IDM_S_IDM_RESET_CONTROLr(unit, idm_srst_control);
        } else if (uC == 1) {
            WRITE_MHOST_M1_IDM_M_IDM_RESET_CONTROLr(unit, idm_mrst_control);
            WRITE_MHOST_S1_IDM_S_IDM_RESET_CONTROLr(unit, idm_srst_control);
        }
        return SOC_E_NONE;
    }
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_QUX_SUPPORT) || defined(BCM_QAX_SUPPORT) || \
    defined(BCM_JERICHO_SUPPORT) || defined(BCM_DNX_SUPPORT)
    /* Take the processor in and out of reset but halted */

    if (SOC_IS_MONTEREY(unit) && (uC == 2)) {
        /* Monterey, with uc:2 is ARM-M7 */
        READ_M7SS_CONFIGr(unit, &rst_control);
        soc_reg_field_set(unit, M7SS_CONFIGr, &rst_control,
                      CPUWAITf, 1);
        WRITE_M7SS_CONFIGr(unit, rst_control);

        READ_M7SS_CTRLr(unit, &rst_control);
        soc_reg_field_set(unit, M7SS_CTRLr, &rst_control,
                      POWERUP_RESETf, 1);
        soc_reg_field_set(unit, M7SS_CTRLr, &rst_control,
                      SYS_RESETf, 1);
        WRITE_M7SS_CTRLr(unit, rst_control);

        sal_usleep(1000);

        READ_M7SS_CTRLr(unit, &rst_control);
        soc_reg_field_set(unit, M7SS_CTRLr, &rst_control,
                      POWERUP_RESETf, 0);
        soc_reg_field_set(unit, M7SS_CTRLr, &rst_control,
                      SYS_RESETf, 0);
        WRITE_M7SS_CTRLr(unit, rst_control);
        return (SOC_E_NONE);
    }

    if (uC == 0) {
        READ_MHOST_0_CR5_RST_CTRLr(unit, &rst_control);
    } else if (uC == 1) {
        if (SOC_INFO(unit).num_ucs == 3 && !SOC_IS_MONTEREY(unit)) {
            READ_RTS_MHOST_1_CR5_RST_CTRLr(unit, &rst_control);
        } else {
            READ_MHOST_1_CR5_RST_CTRLr(unit, &rst_control);
        }
    } else if (uC == 2) {
        READ_RTS_MHOST_2_CR5_RST_CTRLr(unit, &rst_control);
    } else {
        assert(0);
        return SOC_E_UNAVAIL;
    }

    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      SYS_PORESET_Nf, 0);
    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      RESET_Nf, 0);
    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      DBG_RESET_Nf, 0);
    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      PRESET_DBG_Nf, 0);
    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      CPU_HALT_Nf, 0);

    if (uC == 0) {
        WRITE_MHOST_0_CR5_RST_CTRLr(unit, rst_control);
    } else if (uC == 1) {
        if (SOC_INFO(unit).num_ucs == 3 && !SOC_IS_MONTEREY(unit)) {
            WRITE_RTS_MHOST_1_CR5_RST_CTRLr(unit, rst_control);
        } else {
            WRITE_MHOST_1_CR5_RST_CTRLr(unit, rst_control);
        }
    } else if (uC == 2) {
        WRITE_RTS_MHOST_2_CR5_RST_CTRLr(unit, rst_control);
    } else {
        assert(0);
    }

    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      SYS_PORESET_Nf, 1);
    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      RESET_Nf, 1);
    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      DBG_RESET_Nf, 1);
    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      PRESET_DBG_Nf, 1);
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || \
    defined(BCM_DNX_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
    if ((uC == 0) &&
        (SOC_IS_TRIDENT3(unit) || SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWK3(unit) ||
         SOC_IS_MAVERICK2(unit) || SOC_IS_DNX(unit) || SOC_IS_FIRELIGHT(unit))) {
        /* Check if the processor boots into Flash Firmware */
        READ_MHOST_0_MHOST_STRAP_STATUSr(unit,&strap_status);
        if (strap_status != 0) {
            /* let it unhalt for a while until the
             * Flash FW finishes its work and gets into the penholder loop
             */
            soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                              CPU_HALT_Nf, 1);
            sal_msleep(20);
            WRITE_MHOST_0_CR5_RST_CTRLr(unit, rst_control);
            sal_msleep(100);
            soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                              CPU_HALT_Nf, 0);
            /* Ensure that the uKernel FW is invalid */
            soc_uc_mem_write(unit, soc_uc_addr_to_pcie(unit, uC, 0x80), uC);

        }
    }
#endif

    if (uC == 0) {
        WRITE_MHOST_0_CR5_RST_CTRLr(unit, rst_control);
    } else if (uC == 1) {
        if (SOC_INFO(unit).num_ucs == 3 && !SOC_IS_MONTEREY(unit)) {
            WRITE_RTS_MHOST_1_CR5_RST_CTRLr(unit, rst_control);
        } else {
            WRITE_MHOST_1_CR5_RST_CTRLr(unit, rst_control);
        }
    } else if (uC == 2) {
        WRITE_RTS_MHOST_2_CR5_RST_CTRLr(unit, rst_control);
    } else {
        assert(0);
    }
#endif

#if defined(BCM_QAX_SUPPORT)
    if (SOC_IS_QAX(unit))
    {
        if (uC == 0) {
            WRITE_CHIPCOMMONG_UART0_UART_SRRr(unit,1);
        } else if (uC == 1) {
            WRITE_CHIPCOMMONG_UART1_UART_SRRr(unit,1);
        }
    }
#endif

#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit) && !SOC_IS_METROLITE(unit) && uC > 0) {
        /* Unlock the semaphores sem17 and sem16 */
        WRITE_CMIC_SEMAPHORE_17r(unit, 0);
        WRITE_CMIC_SEMAPHORE_16r(unit, 0);
    }
#endif

    return (SOC_E_NONE);
}

/*
 * Function: 	soc_uc_mhost_in_reset
 * Purpose:	Return 1 if in RESET state, 0 otherwise
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	1 or 0
 */
static int
soc_uc_mhost_in_reset(int unit, int uC)
{
    uint32 rst_control, halt;
    uint16 dev_id;
    uint8  rev_id;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || \
    defined(BCM_DNX_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
    uint32 strap_status, addr;
#endif

    if (uC < 0 || uC >= SOC_INFO(unit).num_ucs) {
        return 1; /* Actually it means no such uC */
    }

    /* Dagger2 support 2xR5 Embedded processors
     * mHost 0 was disable and mHost 1 and 2 are valid core
     * mHost 0 is inavlid so ignore mHost 0
     */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((dev_id == BCM56233_DEVICE_ID) && (uC == 1)) {
        return (1);
    }

    /* Monterey, uc:2 is M7 */
    if (SOC_IS_MONTEREY(unit) && (uC == 2)) {
        READ_M7SS_CONFIGr(unit, &rst_control);
        halt = !soc_reg_field_get(
                        unit, M7SS_CONFIGr, rst_control, CPUWAITf);
        return halt;
    }

    if (uC == 0) {
        READ_MHOST_0_CR5_RST_CTRLr(unit, &rst_control);
    } else if (uC == 1) {
        if (SOC_INFO(unit).num_ucs == 3 && !SOC_IS_MONTEREY(unit)) {
            READ_RTS_MHOST_1_CR5_RST_CTRLr(unit, &rst_control);
        } else {
            READ_MHOST_1_CR5_RST_CTRLr(unit, &rst_control);
        }
    } else if (uC == 2) {
        READ_RTS_MHOST_2_CR5_RST_CTRLr(unit, &rst_control);
    } else {
        assert(0);
        return SOC_E_UNAVAIL;
    }

    halt = soc_reg_field_get(
                unit, MHOST_0_CR5_RST_CTRLr, rst_control, CPU_HALT_Nf);

    if (halt == 0) {
        return 1; /* We are in reset */
    }

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || \
    defined(BCM_DNX_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
        if ((uC == 0) &&
            (SOC_IS_TRIDENT3(unit) || SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWK3(unit) ||
             SOC_IS_MAVERICK2(unit) || SOC_IS_DNX(unit) || SOC_IS_FIRELIGHT(unit))) {
            /* Check if the processor boots into Flash Firmware */
            READ_MHOST_0_MHOST_STRAP_STATUSr(unit,&strap_status);
            if (strap_status != 0) {
                addr = soc_uc_addr_to_pcie(unit, uC, 0x80);
                if (soc_uc_mem_read(unit, addr) != 0xbadc0de1) {
                    /* uC is not in reset, but no valid FW */
                    return 2;
                }
            }
        }
#endif
    return 0; /* Not in reset */
}

static void *_soc_uc_mhost_hostram_alloc(int unit, int uC, int size)
{
    uint32 *buf1 = NULL, *buf2 = NULL, *ret = NULL;
    sal_paddr_t paddr = 0;
    uint32 addr_lo = 0, eaddr_lo=0;

    buf1 = soc_cm_salloc(unit, size, "hostRAM buffer 1");
    if (buf1 != NULL) {
        paddr = soc_cm_l2p(unit, buf1);
        addr_lo = PTR_TO_INT(paddr);
        eaddr_lo = addr_lo + size;
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) || \
    defined(BCM_MAVERICK2_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_TRIDENT3(unit) || SOC_IS_TOMAHAWK3(unit) ||
            SOC_IS_MAVERICK2(unit) || SOC_IS_FIRELIGHT(unit)) {
            if ((addr_lo & 0xff) == (eaddr_lo & 0xff)) {
                /* Falls in the same 16M page */
                ret = buf1;
            }
        } else if(SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit)) {
            if ((addr_lo & 0xf) == (eaddr_lo & 0xf)) {
                /* Falls in the same 256M page */
                ret = buf1;
            }
        }
#endif
    }

    if (ret == NULL) {
        /* Buf1 was not usable */
        /* Free it *after* allocating a new buffer */
        buf2 = soc_cm_salloc(unit, size, "hostRAM buffer 2");
        if (buf1 != NULL) {
            soc_cm_sfree(unit, buf1);
        }
    }

    if (buf2 != NULL) {
        paddr = soc_cm_l2p(unit, buf2);
        addr_lo = PTR_TO_INT(paddr);
        eaddr_lo = addr_lo + size;
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) || \
    defined(BCM_MAVERICK2_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_TRIDENT3(unit) || SOC_IS_TOMAHAWK3(unit) ||
            SOC_IS_MAVERICK2(unit) || SOC_IS_FIRELIGHT(unit)) {
            if ((addr_lo & 0xff) == (eaddr_lo & 0xff)) {
                /* Falls in the same 16M page */
                ret = buf2;
            }
        } else if(SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit)) {
            if ((addr_lo & 0xf) == (eaddr_lo & 0xf)) {
                /* Falls in the same 256M page */
                ret = buf2;
            }
        }
#endif
    }

    if (ret == NULL) {
        /* Buf2 was also not usable. */
        if (buf2 != NULL) {
            soc_cm_sfree(unit, buf2);
        }
    }

    COMPILER_REFERENCE(addr_lo);
    COMPILER_REFERENCE(eaddr_lo);
    COMPILER_REFERENCE(paddr);

    return ret;
}

/*
 * Function:    soc_uc_mhost_prestart_config
 * Purpose: Configure the uC before starting
 * Parameters:  unit - unit number
 *      uC - microcontroller num
 * Returns: SOC_E_xxx
 */
static int
soc_uc_mhost_prestart_config(int unit, int uC)
{
    uint32 config_ver, addr, val;
    sal_paddr_t paddr = 0;
    uint32 size = 0;
    soc_control_t *soc = SOC_CONTROL(unit);

    if (soc == NULL) {
        return SOC_E_FAIL;
    }

    addr = soc_uc_addr_to_pcie(unit, uC, 0x80);
    if (soc_uc_mem_read(unit, addr) != 0xbadc0de1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "FW not configurable uC %d\n"), uC));
        return SOC_E_NONE;
    }
    addr = soc_uc_addr_to_pcie(unit, uC, 0x84);
    config_ver = soc_uc_mem_read(unit, addr);

    /* Config uC Num @ 0x88 */
    addr = soc_uc_addr_to_pcie(unit, uC, 0x88);
    soc_uc_mem_write(unit, addr, uC);

    /* Config hostRAM @ 0x8C-0x90 and its size @ 0x94 */
    if (config_ver >= 1) {
        size = soc_property_uc_get(unit, (uC + 1), spn_MCS_HOSTMEM_SIZE, 0) * 1024;
        soc->uc_hostram_buf[uC] = (size > 0) ?
            _soc_uc_mhost_hostram_alloc(unit, uC, size) : NULL;
        paddr = (soc->uc_hostram_buf[uC] != NULL ) ?
                 soc_cm_l2p(unit, soc->uc_hostram_buf[uC]) : 0;
        addr = soc_uc_addr_to_pcie(unit, uC, 0x8C);
        val = PTR_HI_TO_INT(paddr);
        soc_uc_mem_write(unit, addr, val);
        addr = soc_uc_addr_to_pcie(unit, uC, 0x90);
        val = PTR_TO_INT(paddr);
        soc_uc_mem_write(unit, addr, val);
        addr = soc_uc_addr_to_pcie(unit, uC, 0x94);
        val = size;
        soc_uc_mem_write(unit, addr, val);
    }

    /* Config the console UART @ 0xb0 */
    if (config_ver >= 2) {
        val = soc_property_uc_get(unit, (uC + 1), spn_MCS_UART_BMP, 0);
        addr = soc_uc_addr_to_pcie(unit, uC, 0xb0);
        soc_uc_mem_write(unit, addr, val);
    }
    return SOC_E_NONE;
}

/*
 * Function: 	soc_uc_mhost_start
 * Purpose:	Start UC execution
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 *              addr - address to start at
 * Returns:	SOC_E_xxx
 */
static int
soc_uc_mhost_start(int unit, int uC, uint32 addr)
{
    uint32      rst_control;
    uint16      dev_id;
    uint8       rev_id;

    if (uC < 0 || uC >= SOC_INFO(unit).num_ucs) {
        return SOC_E_UNAVAIL;
    }

    /* Dagger2 support 2xR5 Embedded processors
     * mHost 0 was disable and mHost 1 and 2 are valid core
     * mHost 0 is inavlid so ignore mHost 0
     */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((dev_id == BCM56233_DEVICE_ID) && (uC == 1)) {
        return (SOC_E_NONE);
    }

    /* Monterey, uc:2 is ARM-M7 */
    if (SOC_IS_MONTEREY(unit) && (uC == 2)) {
        READ_M7SS_CONFIGr(unit, &rst_control);
        soc_reg_field_set(unit, M7SS_CONFIGr, &rst_control,
                          CPUWAITf, 0);
        WRITE_M7SS_CONFIGr(unit, rst_control);
        return (SOC_E_NONE);
    }

    soc_uc_mhost_prestart_config(unit, uC);

    if (uC == 0) {
        READ_MHOST_0_CR5_RST_CTRLr(unit, &rst_control);
    } else if (uC == 1) {
        if (SOC_INFO(unit).num_ucs == 3 && !SOC_IS_MONTEREY(unit)) {
            READ_RTS_MHOST_1_CR5_RST_CTRLr(unit, &rst_control);
        } else {
            READ_MHOST_1_CR5_RST_CTRLr(unit, &rst_control);
        }
    } else if (uC == 2) {
        READ_RTS_MHOST_2_CR5_RST_CTRLr(unit, &rst_control);
    } else {
        assert(0);
        return SOC_E_UNAVAIL;
    }

    soc_reg_field_set(unit, MHOST_0_CR5_RST_CTRLr, &rst_control,
                      CPU_HALT_Nf, 1);

    if (uC == 0) {
        WRITE_MHOST_0_CR5_RST_CTRLr(unit, rst_control);
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if ((BCM56970_DEVICE_ID & 0xFFF0) == (dev_id & 0xFFF0)){
            soc_cm_iproc_write(unit, 0xffff0fe8, 0);
        }
#endif
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
    defined(BCM_MAVERICK2_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_TRIDENT3(unit) || SOC_IS_TOMAHAWK3(unit) ||
            SOC_IS_MAVERICK2(unit) || SOC_IS_FIRELIGHT(unit)) {
            soc_cm_iproc_write(unit, 0xffff0fe8, 0); /* PenHolder Loc */
        }
#endif
#if defined(BCM_DNX_SUPPORT)
        if (SOC_IS_DNX(unit)) {
            soc_cm_iproc_write(unit, 0xffff0fe8, 0); /* PenHolder Loc */
        }
#endif

    } else if (uC == 1) {
        if (SOC_INFO(unit).num_ucs == 3 && !SOC_IS_MONTEREY(unit)) {
            WRITE_RTS_MHOST_1_CR5_RST_CTRLr(unit, rst_control);
        } else {
            WRITE_MHOST_1_CR5_RST_CTRLr(unit, rst_control);
        }
    } else if (uC == 2) {
        WRITE_RTS_MHOST_2_CR5_RST_CTRLr(unit, rst_control);
    } else {
        assert(0);
    }

    return (SOC_E_NONE);
}

/*
 * Function: 	soc_uc_mhost_preload
 * Purpose:	Prepare to load into UC memory
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_xxx
 */
static int
soc_uc_mhost_preload(int unit, int uC)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(uC);
    return (SOC_E_NONE);
}

#endif /* BCM_UC_MHOST_SUPPORT */

int
soc_uc_init(int unit)
/*
 * Function: 	soc_uc_init
 * Purpose:	Initialize the CMICm MCS
 * Parameters:	unit - unit number
 * Returns:	SOC_E_xxx
 */
{
    if (!soc_feature(unit, soc_feature_uc)) {
        return (SOC_E_FAIL);
    }
#if defined(BCM_UC_MHOST_SUPPORT)
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        return soc_uc_mhost_init(unit);
    }
#endif /* BCM_UC_MHOST_SUPPORT */
#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_iproc)) {
        return soc_uc_iproc_init(unit);
    }
#endif /* BCM_IPROC_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_mcs)) {
        return soc_uc_mcs_init(unit);
    }
#endif /* BCM_CMICM_SUPPORT */
    return (SOC_E_FAIL);
}

int
soc_uc_in_reset(int unit, int uC)
/*
 * Function: 	soc_uc_in_reset
 * Purpose:	Return reset state of core
 * Parameters:	unit - unit number
 *              uC - core number
 * Returns:	0 or 1 or 2
 *
 * Returns 2 only for CMICx devices, when the uC is out of
 * reset but not running a valid firmware. This scenario happens
 * when booted into flash for PCIe FW, and no uKernel FW was loaded
 */
{
    if (!soc_feature(unit, soc_feature_uc)) {
        return (SOC_E_FAIL);
    }
#if defined(BCM_UC_MHOST_SUPPORT)
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        return soc_uc_mhost_in_reset(unit, uC);
    }
#endif /* BCM_UC_MHOST_SUPPORT */
#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_iproc)) {
        return soc_uc_iproc_in_reset(unit, uC);
    }
#endif /* BCM_IPROC_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_mcs)) {
        return soc_uc_mcs_in_reset(unit, uC);
    }
#endif /* BCM_CMICM_SUPPORT */
    return 1;
}

int
soc_uc_load(int unit, int uC, uint32 addr, int len, unsigned char *data)
/*
 * Function: 	soc_uc_load
 * Purpose:	Load a chunk into MCS memory
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 *              addr - Address of chunk from MCS perspective
 *              len - Length of chunk to load in bytes
 *              data - Pointer to data to load
 * Returns:	SOC_E_xxx
 */
{
    int i;
    uint32 wdata;
    int rc = SOC_E_NONE;        /* Be optimistic */
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Skip loading of Firmware for Core-1 in Dagger2 */
    if ((dev_id == BCM56233_DEVICE_ID) && (uC == 1)) {
        return SOC_E_NONE;
    }
    if (!soc_feature(unit, soc_feature_uc)) {
        return (SOC_E_FAIL);
    }

    /* Convert the UC-centric address to a PCI address */
    addr = soc_uc_addr_to_pcie(unit, uC, addr);

    for (i = 0; i < len; i += 4, addr += 4) {
#ifndef LE_HOST
        /* We swap the bytes here for the LE ARMs. */
        wdata = _shr_swap32(*((uint32 *) &data[i]));
#else
        wdata = *((uint32 *) &data[i]);
#endif
        rc = soc_uc_mem_write(unit, addr, wdata);
        if (rc != SOC_E_NONE) {
            SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
                "%s(): soc_uc_mem_write failed uC:%d addr:0x%x len:%d\n",
                FUNCTION_NAME(), uC, addr, len));
            break;
        }
    }
    return (rc);
}

int
soc_uc_preload(int unit, int uC)
/*
 * Function: 	soc_uc_preload
 * Purpose:	Prepare to load code
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_xxx
 */
{
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Skip preloading of Firmware for Core-1 in Dagger2 */
    if ((dev_id == BCM56233_DEVICE_ID) && (uC == 1)) {
        return SOC_E_NONE;
    }

    if (!soc_feature(unit, soc_feature_uc)) {
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostMcsLoad,
            "soc_feature_uc is not defined uC: %d\n", uC));
        return (SOC_E_FAIL);
    }
#if defined(BCM_UC_MHOST_SUPPORT)
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        return soc_uc_mhost_preload(unit, uC);
    }
#endif /* BCM_UC_MHOST_SUPPORT */
#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_iproc)) {
        return soc_uc_iproc_preload(unit, uC);
    }
#endif /* BCM_IPROC_SUPPORT */
#if 0
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_mcs)) {
        return soc_uc_mcs_preload(unit, uC);
    }
#endif /* BCM_CMICM_SUPPORT */
#endif
    return (SOC_E_FAIL);
}

int
soc_uc_reset(int unit, int uC)
/*
 * Function: 	soc_uc_reset
 * Purpose:	Reset the requested core
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_xxx
 */
{
    if (!soc_feature(unit, soc_feature_uc)) {
        return (SOC_E_FAIL);
    }
#if defined(BCM_UC_MHOST_SUPPORT)
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        return soc_uc_mhost_reset(unit, uC);
    }
#endif /* BCM_UC_MHOST_SUPPORT */
#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_iproc)) {
        return soc_uc_iproc_reset(unit, uC);
    }
#endif /* BCM_IPROC_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_mcs)) {
        return soc_uc_mcs_reset(unit, uC);
    }
#endif /* BCM_CMICM_SUPPORT */
    return (SOC_E_FAIL);
}

int
soc_uc_start(int unit, int uC, uint32 addr)
/*
 * Function: 	soc_uc_start
 * Purpose:	Start MCS execution
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_xxx
 */
{
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Skip starting of Firmware for Core-1 in Dagger2 */
    if ((dev_id == BCM56233_DEVICE_ID) && (uC == 1)) {
        return SOC_E_NONE;
    }    

    if (!soc_feature(unit, soc_feature_uc)) {
        return (SOC_E_FAIL);
    }
#if defined(BCM_UC_MHOST_SUPPORT)
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        return soc_uc_mhost_start(unit, uC, addr);
    }
#endif /* BCM_UC_MHOST_SUPPORT */
#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_iproc)) {
        return soc_uc_iproc_start(unit, uC, addr);
    }
#endif /* BCM_IPROC_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_mcs)) {
        return soc_uc_mcs_start(unit, uC, addr);
    }
#endif /* BCM_CMICM_SUPPORT */
    return (SOC_E_FAIL);
}

/*
 * Function: 	soc_uc_ping
 * Purpose:	Check the uKernel status
 * Parameters:	unit - unit number
 *      uC - microcontroller num
 *      timeout - Ping timeout in uS (0 to skip Ping)
 * Returns:	SOC_E_XXX
 *     SOC_E_UNAVAIL : SoC doesn't have uC / MCS support.
 *     SOC_E_PARAM : uC number given is invalid for this SoC
 *     SOC_E_DISABLED : ARM core is in reset.
 *     SOC_E_INIT : uKernel messaging is not initialized.
 *     SOC_E_TIMEOUT : Ping timed out (uKernel not responding).
 *     SOC_E_NONE : Successful.
 */
int soc_uc_ping(int unit, int uC, sal_usecs_t timeout)
{
    mos_msg_data_t send;

    if (!soc_feature(unit, soc_feature_uc)) {
        return (SOC_E_UNAVAIL);
    }
    if (uC < 0 || uC >= SOC_INFO(unit).num_ucs) {
        return SOC_E_PARAM;
    }
    if (soc_uc_in_reset(unit, uC)) {
        /* uC is in reset / Halt */
        return SOC_E_DISABLED;
    }
    if (timeout == 0) {
        return SOC_E_NONE;
    }

    send.s.mclass = MOS_MSG_CLASS_SYSTEM;
    send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_PING;
    send.s.len = 0;
    send.s.data = 0;
    return soc_cmic_uc_msg_send(unit, uC, &send, timeout);
}

char *soc_uc_firmware_version(int unit, int uC)
/*
 * Function: 	soc_uc_firmware_version
 * Purpose:	Return the firmware version string
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	char * string pointer (to be freed with soc_cm_sfree())
 */
{
    int rc;
    mos_msg_data_t send, reply;
    int len = 256;
    char *version_buffer;
    uint16 dev_id;
    uint8 rev_id;
    sal_paddr_t buf_paddr;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Skip loading of Firmware for Core-1 in Dagger2 */
    if ((dev_id == BCM56233_DEVICE_ID) && (uC == 1)) {
        return NULL;
    }

    if (!soc_feature(unit, soc_feature_uc)) {
        return NULL;
    }

    version_buffer = soc_cm_salloc(unit, len, "Version info buffer");
    if (version_buffer) {
        buf_paddr = soc_cm_l2p(unit, version_buffer);
        soc_cm_sinval(unit, version_buffer, len);

        send.s.mclass = MOS_MSG_CLASS_SYSTEM;
        send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_VERSION;
        send.s.len = soc_htons(len);
        send.s.data = soc_htonl(PTR_TO_INT(buf_paddr));

        rc = soc_cmic_uc_msg_send(unit, uC, &send, 5 * 1000 * 1000);
        if (rc != SOC_E_NONE) {
            soc_cm_sfree(unit, version_buffer);
            return NULL;
        }
        rc = soc_cmic_uc_msg_receive(unit, uC, MOS_MSG_CLASS_VERSION,
                                     &reply, 5 * 1000 * 1000);

        if (rc != SOC_E_NONE) {
            soc_cm_sfree(unit, version_buffer);
            return NULL;
        }
    }
    return version_buffer;
}

/*
 * Function: 	soc_uc_firmware_thread_info
 * Purpose:	Return the firmware thread_info string
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	char * string pointer (to be freed with soc_cm_sfree())
 */
char *soc_uc_firmware_thread_info(int unit, int uC)
{
    int rc;
    mos_msg_data_t send, reply;
    int len = 1024;
    char *thread_info_buffer;
    uint16 dev_id;
    uint8 rev_id;
    sal_paddr_t buf_paddr;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Skip loading of Firmware for Core-1 in Dagger2 */
    if ((dev_id == BCM56233_DEVICE_ID) && (uC == 1)) {
        return NULL;
    }

    if (!soc_feature(unit, soc_feature_uc)) {
        return NULL;
    }

    thread_info_buffer = soc_cm_salloc(unit, len, "thread info buffer");
    if (thread_info_buffer) {
        buf_paddr = soc_cm_l2p(unit, thread_info_buffer);
        soc_cm_sinval(unit, thread_info_buffer, len);

        send.s.mclass = MOS_MSG_CLASS_SYSTEM;
        send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_THREAD_INFO;
        send.s.len = soc_htons(len);
        send.s.data = soc_htonl(PTR_TO_INT(buf_paddr));

        rc = soc_cmic_uc_msg_send(unit, uC, &send, _SYSTEM_UC_MSG_TIMEOUT);
        if (rc != SOC_E_NONE) {
            soc_cm_sfree(unit, thread_info_buffer);
            return NULL;
        }
        rc = soc_cmic_uc_msg_receive(unit, uC, MOS_MSG_CLASS_THREAD_INFO,
                                     &reply, _SYSTEM_UC_MSG_TIMEOUT);

        if (rc != SOC_E_NONE) {
            soc_cm_sfree(unit, thread_info_buffer);
            return NULL;
        }

    }
    return thread_info_buffer;
}

/*
 * Function: 	soc_uc_console_log
 * Purpose:	Switch on/off uc console log
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_XXX
 */
int soc_uc_console_log(int unit, int uC, int on)
{
    int            rc;
    mos_msg_data_t send;

    if (!soc_feature(unit, soc_feature_uc)) {
        return SOC_E_UNAVAIL;
    }

    sal_memset(&send, 0, sizeof(send));
    send.s.mclass = MOS_MSG_CLASS_SYSTEM;
    send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_CMD_CONSOLE_LOG;
    send.s.len = soc_htons(on);
    rc = soc_cmic_uc_msg_send(unit, uC, &send, _SYSTEM_UC_MSG_TIMEOUT);

    return rc;
}

/*
 * Function: 	soc_uc_stats_reset
 * Purpose:	Reset stats info in the uC
 * Parameters:	unit - unit number
 *		uC - microcontroller num
 * Returns:	SOC_E_XXX
 */
int soc_uc_stats_reset(int unit, int uC)
{
    int            rc;
    mos_msg_data_t send;

    if (!soc_feature(unit, soc_feature_uc)) {
        return SOC_E_UNAVAIL;
    }

    sal_memset(&send, 0, sizeof(send));
    send.s.mclass = MOS_MSG_CLASS_SYSTEM;
    send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_CMD_STATS_RESET;
    rc = soc_cmic_uc_msg_send(unit, uC, &send, _SYSTEM_UC_MSG_TIMEOUT);

    return rc;
}
/*
 * Function: 	soc_uc_status
 * Purpose:	Return Status of uC
 * Parameters:	unit   - unit number
 *              uC     - core number
 *              status - 0|1. 1 - OK
 * Returns:	SOC_E_XXX
 */
int
soc_uc_status(int unit, int uC, int *status)
{
    uint32 addr = 0;
    uint32 cpsr = 0;

    *status = 0;

    if (!soc_feature(unit, soc_feature_uc)) {
        return (SOC_E_FAIL);
    }
    if (uC < 0 || uC >= SOC_INFO(unit).num_ucs) {
        return SOC_E_PARAM;
    }

    if (soc_uc_in_reset(unit, uC)) {
        /* uC is in reset */
        return SOC_E_NONE;
    }

    addr = soc_uc_addr_to_pcie(unit, uC, 0x60);

    if (soc_feature(unit, soc_feature_iproc) &&
        !soc_feature(unit, soc_feature_uc_mhost)) {
        /* iProc - L2Cache + SRAM for now. */
        if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit)) {
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
            addr = 0x00004060;
#endif /* BCM_HURRICANE2_SUPPORT || BCM_HURRICANE3_SUPPORT */
        } else {
            if (0 == uC) {
                addr = 0x1b004060;
            } else {
                addr = 0x1b034060;
            }
        }
    }

    cpsr = soc_uc_mem_read(unit, addr);
    if (!cpsr) {
        *status = 1;
    }
    return SOC_E_NONE;;
}
#endif

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Routines for accessing BCM56xx PCI memory mapped registers
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <sal/core/boot.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/devids.h>
#include <soc/error.h>
#include <soc/cmic.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <pciephy/pciephy.h>
#endif
#if defined(SOC_PCI_DEBUG) && defined(INCLUDE_CPU_I2C)
#define CMICX_OFFSET 0x3200000
#endif


/* The buffer used by soc_pci_off2name */
static char _soc_pci_off2name_buf[SOC_MAX_NUM_DEVICES][40];

/*
 * CMIC PCI Memory-Accessible registers.
 *
 * NOTE: Names must be kept in correct order to correspond with offsets.
 */

STATIC char *_soc_pci_reg_names[] = {
    "CMIC_SCHAN_CTRL",		/* Start at offset 0x50 */
    "CMIC_ARL_DMA_ADDR", "CMIC_ARL_DMA_CNT",
    "CMIC_SCHAN_ERR",
    "CMIC_COS_ENABLE_COS0", "CMIC_COS_ENABLE_COS1",
    "CMIC_COS_ENABLE_COS2", "CMIC_COS_ENABLE_COS3",
    "CMIC_COS_ENABLE_COS4", "CMIC_COS_ENABLE_COS5",
    "CMIC_COS_ENABLE_COS6", "CMIC_COS_ENABLE_COS7",
    "CMIC_ARL_MBUF00", "CMIC_ARL_MBUF01", "CMIC_ARL_MBUF02", "CMIC_ARL_MBUF03",
    0, 0, 0, 0,
    "CMIC_ARL_MBUF10", "CMIC_ARL_MBUF11", "CMIC_ARL_MBUF12", "CMIC_ARL_MBUF13",
    0, 0, 0, 0,
    "CMIC_ARL_MBUF20", "CMIC_ARL_MBUF21", "CMIC_ARL_MBUF22", "CMIC_ARL_MBUF23",
    0, 0, 0, 0,
    "CMIC_ARL_MBUF30", "CMIC_ARL_MBUF31", "CMIC_ARL_MBUF32", "CMIC_ARL_MBUF33",
    0, 0, 0, 0,
    "CMIC_DMA_CTRL", "CMIC_DMA_STAT", "CMIC_HOL_STAT", "CMIC_CONFIG",
    "CMIC_DMA_DESC0", "CMIC_DMA_DESC1", "CMIC_DMA_DESC2", "CMIC_DMA_DESC3",
    "CMIC_I2C_SLAVE_ADDR", "CMIC_I2C_DATA", "CMIC_I2C_CTRL", "CMIC_I2C_STAT",
    "CMIC_I2C_SLAVE_XADDR", "CMIC_I2C_GP0", "CMIC_I2C_GP1",
    "CMIC_I2C_RESET", "CMIC_LINK_STAT",
    "CMIC_IRQ_STAT", "CMIC_IRQ_MASK",
    "CMIC_MEM_FAIL",
    "CMIC_IGBP_WARN", "CMIC_IGBP_DISCARD",
    "CMIC_MIIM_PARAM", "CMIC_MIIM_READ_DATA",
    "CMIC_SCAN_PORTS",
    "CMIC_STAT_DMA_ADDR", "CMIC_STAT_DMA_SETUP",
    "CMIC_STAT_DMA_PORTS", "CMIC_STAT_DMA_CURRENT",
    "CMIC_ENDIAN_SELECT",
};

char *
soc_pci_off2name(int unit, uint32 offset)
{
    int			led = soc_feature(unit, soc_feature_led_proc);

    assert((offset & 3) == 0);

#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        sal_strncpy(_soc_pci_off2name_buf[unit],
                    soc_cmicm_addr_name (offset), 39);
        _soc_pci_off2name_buf[unit][39] = 0;
        return _soc_pci_off2name_buf[unit];
    }
#endif /* CMICM Support */

#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
        sal_strncpy(_soc_pci_off2name_buf[unit],
                    soc_cmicx_addr_name (unit, offset), 39);
        _soc_pci_off2name_buf[unit][39] = 0;
        return _soc_pci_off2name_buf[unit];
    }
#endif /* BCM_CMICX_SUPPORT */

    /* CMIC_SCHAN_MESSAGE begins at 0x800 on some chips */

    if (offset < 0x50) {
        sal_sprintf(_soc_pci_off2name_buf[unit], "CMIC_SCHAN_D%02d",
                    offset / 4);
    } else if ((int) offset >= CMIC_SCHAN_MESSAGE(unit, 0) &&
        (int)offset < CMIC_SCHAN_MESSAGE(unit, CMIC_SCHAN_WORDS(unit))) {
        sal_sprintf(_soc_pci_off2name_buf[unit], "CMIC_SCHAN_D%02d",
                    (offset - CMIC_SCHAN_MESSAGE(unit, 0)) / 4);
    } else if (led && offset == 0x1000) {
        sal_strncpy(_soc_pci_off2name_buf[unit], "CMIC_LED_CTRL",
                    sizeof(_soc_pci_off2name_buf[unit]));
    } else if (led && offset == 0x1004) {
        sal_strncpy(_soc_pci_off2name_buf[unit], "CMIC_LED_STATUS",
                    sizeof(_soc_pci_off2name_buf[unit]));
    } else if (led && offset >= 0x1800 && offset < 0x1c00) {
        sal_sprintf(_soc_pci_off2name_buf[unit], "CMIC_LED_PROG%02x",
                   (offset - 0x1800) / 4);
    } else if (led && offset >= 0x1c00 && offset < 0x2000) {
        sal_sprintf(_soc_pci_off2name_buf[unit], "CMIC_LED_DATA%02x",
                   (offset - 0x1c00) / 4);
    } else if ((offset - 0x50) < 4 * (uint32)COUNTOF(_soc_pci_reg_names) &&
        _soc_pci_reg_names[(offset - 0x50) / 4] != NULL) {
        sal_strncpy(_soc_pci_off2name_buf[unit],
                    _soc_pci_reg_names[(offset - 0x50) / 4], 39);
        _soc_pci_off2name_buf[unit][39] = 0;
    } else {
        sal_sprintf(_soc_pci_off2name_buf[unit], "CMIC_UNUSED_0x%04x", offset);
    }

    return _soc_pci_off2name_buf[unit];
}

/* If SOC_PCI_DEBUG not defined, then these functions are inlined in cmic.h */
#ifdef	SOC_PCI_DEBUG
/*
 * Get a CMIC register in PCI space using more "soc-like" semantics.
 * Input address is relative to the base of CMIC registers.
 */
int
soc_pci_getreg(int unit, uint32 addr, uint32 *datap)
{
	uint32 addr32 = addr;
#if defined(IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        assert(addr < 0x50000 || ((addr<7*1024*1024+128*1024) && (addr >= 7*1024*1024))); /* check that the address is in the currently supported address range of the iproc BAR in Jericho 2 and Ramon */
    }
#endif /* IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */

#if defined(SOC_PCI_DEBUG) && defined(INCLUDE_CPU_I2C)
    if (SAL_BOOT_I2C_ACCESS) {
        *datap = soc_cm_iproc_read(unit, addr | CMICX_OFFSET);
        return SOC_E_NONE;
    }
#endif
#if defined(BCM_IPROC_SUPPORT)  && defined(IPROC_NO_ATL)
	addr32 += SOC_DRIVER(unit)->cmicd_base;
#endif
    *datap = CMREAD(unit, addr32);
#if defined(CMIC_SOFT_BYTE_SWAP)
    *datap = CMIC_SWAP32(*datap);
#endif
    LOG_VERBOSE(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "PCI%d memR(0x%x)=0x%x\n"), unit, addr32, *datap));
    return SOC_E_NONE;
}

/*
 * Get a CMIC register in PCI space.
 * Input address is relative to the base of CMIC registers.
 */
uint32
soc_pci_read(int unit, uint32 addr)
{
    uint32 data, addr32 = addr;

#if defined(IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        assert(addr < 0x50000 || ((addr<7*1024*1024+128*1024) && (addr >= 7*1024*1024))); /* check that the address is in the currently supported address range of the iproc BAR in Jericho 2 and Ramon */
    }
#endif /* IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */

#if defined(SOC_PCI_DEBUG) && defined(INCLUDE_CPU_I2C)
    if (SAL_BOOT_I2C_ACCESS) {
        return soc_cm_iproc_read(unit, addr | CMICX_OFFSET);
    }
#endif

#if defined(BCM_IPROC_SUPPORT)  && defined(IPROC_NO_ATL)
	addr32 += SOC_DRIVER(unit)->cmicd_base;
#endif
	data = CMREAD(unit, addr32);
#if defined(CMIC_SOFT_BYTE_SWAP)
    data = CMIC_SWAP32(data);
#endif
    LOG_VERBOSE(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "PCI%d barR(0x%x)=0x%x\n"), unit, addr32, data));
    return data;
}

/*
 * Set a CMIC register in PCI space.
 * Input address is relative to the base of CMIC registers.
 */
int
soc_pci_write(int unit, uint32 addr, uint32 data)
{
	uint32 addr32 = addr;

#if defined(IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        assert(addr < 0x50000 || ((addr<7*1024*1024+128*1024) && (addr >= 7*1024*1024))); /* check that the address is in the currently supported address range of the iproc BAR in Jericho 2 and Ramon */
    }
#endif /* IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */

#if defined(SOC_PCI_DEBUG) && defined(INCLUDE_CPU_I2C)
    if (SAL_BOOT_I2C_ACCESS) {
        soc_cm_iproc_write(unit, addr | CMICX_OFFSET, data);
        return 0;
    }
#endif

#if defined(BCM_IPROC_SUPPORT)  && defined(IPROC_NO_ATL)
	addr32 += SOC_DRIVER(unit)->cmicd_base;
#endif
#if defined(CMIC_SOFT_BYTE_SWAP)
    data = CMIC_SWAP32(data);
#endif
    LOG_VERBOSE(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "PCI%d barW(0x%x)=0x%x\n"), unit, addr32, data));
    CMWRITE(unit, addr32, data);
    return 0;
}

/*
 * Read a register from the PCI Config Space
 */
uint32
soc_pci_conf_read(int unit, uint32 addr)
{
    uint32 data;
    data = CMCONFREAD(unit, addr);
    LOG_VERBOSE(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "PCI%d ConfigR(0x%x)=0x%x\n"), unit, addr, data));
    return data;
}

/*
 * Write a value to the PCI Config Space
 */
int
soc_pci_conf_write(int unit, uint32 addr, uint32 data)
{
    LOG_VERBOSE(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "PCI%d ConfigW(0x%x)=0x%x\n"), unit, addr, data));
    CMCONFWRITE(unit, addr, data);
    return 0;
}
#else
/* Get CMIC PCI Register */
int
soc_pci_getreg(int unit, uint32 addr, uint32 *datap)
{
    *datap = CMREAD(unit, addr);
#if defined(CMIC_SOFT_BYTE_SWAP)
    *datap = CMIC_SWAP32(*datap);
#endif
    return SOC_E_NONE;
}
#endif	/* SOC_PCI_DEBUG */

/*
 * The functions below access iproc or cmic registers, used in register access
 * macros for registers who have different types in different devices.
 * The previous macros used soc_reg_addr() so it is safe not to compile the
 * functions in builds where soc_reg_addr() is not compiled.
 */
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)
/*
 * Function:
 *      soc_cmic_or_iproc_getreg
 * Purpose:
 *      Read iProc or CMIC register
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_XXX
 */
int soc_cmic_or_iproc_getreg(int unit, soc_reg_t reg, uint32 *data)
{
    soc_regtype_t regtype = SOC_REG_TYPE(unit, reg);
    uint32 addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    if (regtype == soc_cpureg) {
        /* Read PCI register value. */
        SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, addr, data));
#ifdef BCM_IPROC_SUPPORT
    } else if (regtype == soc_iprocreg) {
        *data = soc_cm_iproc_read(unit, addr);
#endif
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Illegal register type\n")));
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_cmic_or_iproc_setreg
 * Purpose:
 *      Write iProc or CMIC register
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_XXX
 */
int soc_cmic_or_iproc_setreg(int unit, soc_reg_t reg, uint32 data)
{
    soc_regtype_t regtype = SOC_REG_TYPE(unit, reg);
    uint32 addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    if (regtype == soc_cpureg) {
        soc_pci_write(unit, addr, data);
#ifdef BCM_IPROC_SUPPORT
    } else if (regtype == soc_iprocreg) {
        soc_cm_iproc_write(unit, addr, data);
#endif
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Illegal register type\n")));
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT) */

/*
 * soc_pci_test checks PCI memory range 0x00-0x4f
 */

int
soc_pci_test(int unit)
{
    int i;
    uint32 tmp, reread;
    uint32 pat;
#ifdef BCM_CMICM_SUPPORT
    int cmc = SOC_PCI_CMC(unit);
#endif
#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
        return(soc_cmicx_pci_test(unit));
    }
#endif
    SCHAN_LOCK(unit);

    /* Check for address uniqueness */

#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            pat = 0x55555555 ^ (i << 24 | i << 16 | i << 8 | i);
            soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i), pat);
        }

        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            pat = 0x55555555 ^ (i << 24 | i << 16 | i << 8 | i);
            tmp = soc_pci_read(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i));
            if (tmp != pat) {
                goto error;
            }
        }
    } else
#endif
    {
        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            pat = 0x55555555 ^ (i << 24 | i << 16 | i << 8 | i);
            soc_pci_write(unit, CMIC_SCHAN_MESSAGE(unit, i), pat);
        }

        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            pat = 0x55555555 ^ (i << 24 | i << 16 | i << 8 | i);
            tmp = soc_pci_read(unit, CMIC_SCHAN_MESSAGE(unit, i));
            if (tmp != pat) {
                goto error;
            }
        }
    }
    if (!SAL_BOOT_QUICKTURN) {  /* Takes too long */
        /* Rotate walking zero/one pattern through each register */

        pat = 0xff7f0080;       /* Simultaneous walking 0 and 1 */

        for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
            int j;

            for (j = 0; j < 32; j++) {
#ifdef BCM_CMICM_SUPPORT
                if(soc_feature(unit, soc_feature_cmicm)) {
                    soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i), pat);
                    tmp = soc_pci_read(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i));
                } else
#endif
                {
                    soc_pci_write(unit, CMIC_SCHAN_MESSAGE(unit, i), pat);
                    tmp = soc_pci_read(unit, CMIC_SCHAN_MESSAGE(unit, i));
                }
                if (tmp != pat) {
                    goto error;
                }
                pat = (pat << 1) | ((pat >> 31) & 1);	/* Rotate left */
            }
        }
    }

    /* Clear to zeroes when done */

    for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
#ifdef BCM_CMICM_SUPPORT
        if(soc_feature(unit, soc_feature_cmicm)) {
            soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i), 0);
        } else
#endif
        {
            soc_pci_write(unit, CMIC_SCHAN_MESSAGE(unit, i), 0);
        }
    }

    SCHAN_UNLOCK(unit);
    return 0;

 error:
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        reread = soc_pci_read(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i));
    } else
#endif
    {
        reread = soc_pci_read(unit, CMIC_SCHAN_MESSAGE(unit, i));
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "FATAL PCI error testing PCIM[0x%x]:\n"
                          "Wrote 0x%x, read 0x%x, re-read 0x%x\n"),
               i, pat, tmp, reread));

    SCHAN_UNLOCK(unit);
    return SOC_E_INTERNAL;
}

/*
 * Do a harmless memory read from the address CMIC_OFFSET_TRIGGER.  This
 * can be called from error interrupts, memory test miscompare, etc. to
 * trigger a logic analyzer that is waiting for a PCI memory read of the
 * trigger address.
 */

void
soc_pci_analyzer_trigger(int unit)
{
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
        (void) soc_pci_read(unit, CMIC_OFFSET_TRIGGER);
    }
}

#define SERDES_PMI_ADDR (0x1130)
#define SERDES_PMI_WDATA (0x1134)
#define PMI_WDATA_DATA_MASK (0xFFFF)
#define PMI_WDATA_RCMD (1 << 30)
#define PMI_WDATA_WCMD (1 << 31)
#define SERDES_PMI_RDATA (0x1138)
#define PMI_RDATA_DATA_MASK (0xFFFF)
#define PMI_RDATA_VALID  (1 << 31)
#define SERDES_PMI_RD_DONE_COUNT (1000)

int
soc_pcie_phy_read(int unit, uint32 addr, uint16 *val)
{
#ifdef BCM_CMICX_SUPPORT
    uint32 data, cmd, ioerr = 0;

    if (!soc_feature(unit, soc_feature_cmicx)) {
        return SOC_E_UNAVAIL;
    }

    /*
     * Accessed indirectly via the PAXB CFG IND ADDR/DATA and
     * SERDES_PMI_ADDR/DATA registers.
     */
    ioerr += WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, SERDES_PMI_ADDR);
    sal_udelay(1000);
    ioerr += WRITE_PAXB_0_CONFIG_IND_DATAr(unit, addr);
    sal_udelay(1000);
    /* Initiate read cycle */
    ioerr += WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, SERDES_PMI_WDATA);
    sal_udelay(1000);
    ioerr += WRITE_PAXB_0_CONFIG_IND_DATAr(unit, PMI_WDATA_RCMD);
    sal_udelay(1000);
    /* Check if read is complete */
    do {
        ioerr += WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, SERDES_PMI_WDATA);
        sal_udelay(1000);
        ioerr += READ_PAXB_0_CONFIG_IND_DATAr(unit, &cmd);
        sal_udelay(1000);
        ioerr += WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, SERDES_PMI_RDATA);
        sal_udelay(1000);
        ioerr += READ_PAXB_0_CONFIG_IND_DATAr(unit, &data);
        sal_udelay(1000);
    } while (((cmd & PMI_WDATA_RCMD) != 0) || ((data & PMI_RDATA_VALID) == 0));

    *val = data;

    return (ioerr == 0) ? SOC_E_NONE : SOC_E_FAIL;
#else
    return SOC_E_UNAVAIL;
#endif
}

int
soc_pcie_phy_write(int unit, uint32 addr, uint16 val)
{
#ifdef BCM_CMICX_SUPPORT
    uint32 data, ioerr = 0;

    if (!soc_feature(unit, soc_feature_cmicx)) {
        return SOC_E_UNAVAIL;
    }

    /*
     * Accessed indirectly via the PAXB CFG IND ADDR/DATA and
     * SERDES_PMI_ADDR/DATA registers.
     */
    ioerr += WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, SERDES_PMI_ADDR);
    sal_udelay(1000);
    ioerr += WRITE_PAXB_0_CONFIG_IND_DATAr(unit, addr);
    sal_udelay(1000);
    /* Initiate write cycle */
    ioerr += WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, SERDES_PMI_WDATA);
    sal_udelay(1000);
    ioerr += WRITE_PAXB_0_CONFIG_IND_DATAr(unit, PMI_WDATA_WCMD | val);
    sal_udelay(1000);
    /* Check if write is complete */
    do {
        ioerr += WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, SERDES_PMI_WDATA);
        sal_udelay(1000);
        ioerr += READ_PAXB_0_CONFIG_IND_DATAr(unit, &data);
    } while ((data & PMI_WDATA_WCMD) != 0);

    return (ioerr == 0) ? SOC_E_NONE : SOC_E_FAIL;
#else
    return SOC_E_UNAVAIL;
#endif
}

/* Get the phy type and the iproc version */
int
soc_pcie_phy_type_get(int unit, int *type, unsigned *iproc_ver)
{
#ifdef BCM_CMICX_SUPPORT
    int rv;
    unsigned int cmic_ver, cmic_rev;

#if defined(SOC_PCI_DEBUG) && defined(INCLUDE_CPU_I2C)
    /* If there is no PCIe link to the device, we cannot access PCIe configuration space. */
    if (SAL_BOOT_I2C_ACCESS) {
        rv = *iproc_ver = get_iproc_version_from_unit(unit);
    } else
#endif
    {
        rv = soc_cmicx_iproc_version_get(unit, iproc_ver, &cmic_ver, &cmic_rev);
    }
    if (rv == 0) {
        return SOC_E_FAIL;
    }

    if (*iproc_ver == 17) {
        *type = PCIEPHY_DRIVER_TYPE_MERLIN7;
    } else if (*iproc_ver >= 14 && *iproc_ver <= 18) {
        *type = PCIEPHY_DRIVER_TYPE_MERLIN16;
    } else {
        /* All others we don't know */
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}


#ifdef BCM_CMICX_SUPPORT
static int
pcie_srds_bus_read(void *user_acc, uint32_t addr, uint16_t *val)
{
    pciephy_access_t *sa = user_acc;
    int unit = sa->unit;

    if (soc_pcie_phy_read(unit, addr, val) != SOC_E_NONE) {
        return -1;
    }

    return 0;
}

static int
pcie_srds_bus_write(void *user_acc, uint32_t addr, uint16_t val)
{
    pciephy_access_t *sa = user_acc;
    int unit = sa->unit;

    if (soc_pcie_phy_write(unit, addr, val) != SOC_E_NONE) {
        return -1;
    }

    return 0;
}

/*
 * Returns 0 - if PCIe FW is required but not loaded
 *         1 - if PCIe FW is loaded or FW is not required
 */
int
soc_pcie_fw_status_get(int unit, uint32 *valid)
{
    uint32 mode, bootdev, mhost_en = 0, rval;
    uint16 loaded;
    int rv;
    unsigned iproc_ver;

    if(soc_feature(unit, soc_feature_cmicx) && 
       (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE)) {
        pciephy_access_t sa;
        pciephy_acc_bus_t pcie_srds_bus;

        sal_memset(&pcie_srds_bus, 0, sizeof(pcie_srds_bus));
        pcie_srds_bus.name = "pcie_srds_bus";
        pcie_srds_bus.read = pcie_srds_bus_read;
        pcie_srds_bus.write = pcie_srds_bus_write;

        sal_memset(&sa, 0, sizeof(sa));
        sa.unit = unit;
        sa.bus = &pcie_srds_bus;
        SOC_IF_ERROR_RETURN(soc_pcie_phy_type_get(unit, (int *)&sa.type, &iproc_ver));
        sa.iproc_ver = (uint8_t)iproc_ver;

        /* Read PCIe link capability register and extract max link speed */
        SOC_IF_ERROR_RETURN(WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, 0xb8));
        sal_udelay(1000);
        SOC_IF_ERROR_RETURN(READ_PAXB_0_CONFIG_IND_DATAr(unit, &mode));
        mode &= 0xF; /* 1 - Gen1, 2 - Gen2, 3 - Gen3 */
        SOC_IF_ERROR_RETURN(READ_PAXB_0_GEN3_UC_LOADER_STATUSr(unit, &rval));
        /* Read SerDes register 0xd230 which has FW version loaded */
        rv = pciephy_diag_reg_read(&sa, 0xd230, &loaded);
        if(rv != 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "pciephy_diag_reg_read failed, %d\n"), rv));
        }

        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
            SOC_IF_ERROR_RETURN(READ_ICFG_ROM_S0_IDM_IO_STATUSr(unit, &rval));
            bootdev = soc_reg_field_get(unit, ICFG_ROM_S0_IDM_IO_STATUSr,
                                                       rval, STRAP_BOOT_DEVf);
        }
        else {
            SOC_IF_ERROR_RETURN(READ_ROM_S0_IDM_IO_STATUSr(unit, &rval));
            bootdev = soc_reg_field_get(unit, ROM_S0_IDM_IO_STATUSr,
                                                       rval, STRAP_BOOT_DEVf);
            SOC_IF_ERROR_RETURN(READ_ICFG_MHOST0_STRAPSr(unit, &rval));
            mhost_en = soc_reg_field_get(unit, ICFG_MHOST0_STRAPSr, rval, 
                                                            MHOST0_BOOT_DEVf);
        }
        if (mode == 3) {
            *valid = loaded;
        }
        else if (!(SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) && (mhost_en == 1) && 
                 (bootdev == 0 || bootdev == 6) &&
                 (loaded == 0)) {
           *valid = 0;
        }
        else {
           *valid = 1;
        }
    }
    else {
        *valid = 1;    
    }
    return SOC_E_NONE;
}
#endif

/* Return the iproc version of a device, given its PCIe device ID.
 * Needed for devices with iproc of version >= 14, for PCIe handling through I2C.
 * A return value of 0 means the device ID is not recognized.
 */
unsigned 
get_iproc_version_from_device_id(uint16 dev_id)
{
    switch (dev_id & DNXC_DEVID_FAMILY_MASK) {
      case JERICHO2_DEVICE_ID:/* JR2 */
      case BCM88790_DEVICE_ID: /* Ramon */
      case BCM56870_DEVICE_ID: /* TD3 */
        return 14;
      case J2C_DEVICE_ID:      /* J2C */
      case J2C_2ND_DEVICE_ID:  /* Q2C */
      case Q2A_DEVICE_ID:      /* Q2A */
      case Q2U_DEVICE_ID:
        return 15;
      case J2P_DEVICE_ID:      /* J2CP */
#ifdef BCM_LTSW_SUPPORT
      case BCM56880_DEVICE_ID: /* TD4 */
#endif
        return 17;
  }

  return 0;
}

/* Return the iproc version of a device, given its unit number
 * A return value of 0 means the device ID is not recognized.
 * known to not be composite.
 */
unsigned 
get_iproc_version_from_unit(int unit)
{
    return get_iproc_version_from_device_id(CMDEV(unit).dev.dev_id);
}

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
#include <soc/mcm/cmicx.h>
#include <soc/mcm/intr_iproc.h>
#include <soc/mcm/memregs.h>
#include <soc/intr.h>
#include <soc/cm.h>
#include <soc/cmic.h>
#include <soc/drv.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_er_threading.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <soc/iproc.h>
#include <soc/cmicx.h>
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_fwconfig.h>
#include <shared/cmicfw/cmicx_led.h>
#include <shared/cmicfw/cmicx_link.h>
#if defined(INCLUDE_IFA)
#include <bcm_int/esw/ifacc.h>
#endif /* INCLUDE_IFA */

#ifdef U0_DEBUG
/* M0 FW debug enable. */
static int fwdbg_enable = 1;
#else
/* M0 FW debug disable. */
static int fwdbg_enable = 0;
#endif
/* iProc 15 or below. */
#define SW_PROG_INTR_IRQ                73

/* iProc 16 or above. */
#define P16_SW_PROG_INTR_IRQ            69
#define P16_SELF_RESETf                 1

static int iproc_m0ssq_init_done[SOC_MAX_NUM_DEVICES] = {0};

static uint32 iproc_m0ssq_uc_fw_len[SOC_MAX_NUM_DEVICES][MAX_UCORES] = {{0,0,0,0}};
static uint32 iproc_m0ssq_uc_fw_load_crc[SOC_MAX_NUM_DEVICES][MAX_UCORES] = {{0,0,0,0}};


static const soc_reg_t control_reg[MAX_UCORES] = {
    U0_M0SS_CONTROLr,
    U1_M0SS_CONTROLr,
    U2_M0SS_CONTROLr,
    U3_M0SS_CONTROLr
};

static const soc_reg_t status_reg[MAX_UCORES] = {
    U0_M0SS_STATUSr,
    U1_M0SS_STATUSr,
    U2_M0SS_STATUSr,
    U3_M0SS_STATUSr
};

static const soc_field_t control_field[MAX_UCORES] = {
    CORTEXM0_U0f,
    CORTEXM0_U1f,
    CORTEXM0_U2f,
    CORTEXM0_U3f
};

static const soc_reg_t irq_mask[MAX_UCORES] = {
    U0_M0SS_INTR_MASK_95_64r,
    U1_M0SS_INTR_MASK_95_64r,
    U2_M0SS_INTR_MASK_95_64r,
    U3_M0SS_INTR_MASK_95_64r,
};

static const soc_reg_t p16_irq_en[MAX_UCORES] = {
    U0_SW_PROG_INTR_ENABLEr,
    U1_SW_PROG_INTR_ENABLEr,
    U2_SW_PROG_INTR_ENABLEr,
    U3_SW_PROG_INTR_ENABLEr,
};

static const soc_reg_t p16_irq_set[MAX_UCORES] = {
    U0_SW_PROG_INTR_SETr,
    U1_SW_PROG_INTR_SETr,
    U2_SW_PROG_INTR_SETr,
    U3_SW_PROG_INTR_SETr,
};

static const soc_reg_t p16_irq_clr[MAX_UCORES] = {
    U0_SW_PROG_INTR_CLRr,
    U1_SW_PROG_INTR_CLRr,
    U2_SW_PROG_INTR_CLRr,
    U3_SW_PROG_INTR_CLRr,
};

uint32 soc_iproc_percore_membase_get(int unit, int ucnum)
{
    /* Check if corresponding uC exist. */
    if (!SOC_REG_IS_VALID(unit, control_reg[ucnum])) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "ucore %d doesn't exist.\n"), ucnum));
        assert(SOC_REG_IS_VALID(unit, control_reg[ucnum]));
        return 0xFFFFFFFF;
    };

    /* Get uC TCAM address by uC's control register. */
    return soc_reg_addr(unit, control_reg[ucnum], REG_PORT_ANY, 0) & 0xFFFF0000;
}

uint32 soc_iproc_percore_memsize_get(int unit, int ucnum)
{
    /* Check if corresponding uC exist. */
    if (!SOC_REG_IS_VALID(unit, control_reg[ucnum])) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "ucore %d doesn't exist.\n"), ucnum));
        assert(SOC_REG_IS_VALID(unit, control_reg[ucnum]));
        return 0xFFFFFFFF;
    };

    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        return IPROC_M0SSQ_TCAM_SIZE_HX5;
    } else {
        return IPROC_M0SSQ_TCAM_SIZE;
    }
}

uint32 soc_iproc_sram_membase_get(int unit)
{
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        return IPROC_M0SSQ_SRAM_BASE_HX5;
    } else {
        return IPROC_M0SSQ_SRAM_BASE;
    }
}

/*
 * Function:
 *     soc_iproc_m0_self_reset_set
 * Purpose:
 *     Send a interrupt to M0 FW. Let M0 FW reset M0 itself.
 * Parameters:
 *     unit number
 *     ucore number
 *     reset Soft reset value.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0_self_reset_set(int unit, int ucore, int reset)
{
    uint32 val = 0;

    /* Device with iProc16 or above. */
    if (SOC_IS_HELIX5(unit)) {

        if (reset) {

            /* Enable interrupt mask. */
            soc_iproc_getreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), &val);
            val |= (1 << (P16_SW_PROG_INTR_IRQ - 64));
            soc_iproc_setreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), val);

            /* Enable software interrupt to M0. */
            soc_iproc_getreg(unit, soc_reg_addr(unit, p16_irq_en[ucore], REG_PORT_ANY, 0), &val);
            val |= P16_SELF_RESETf;
            soc_iproc_setreg(unit, soc_reg_addr(unit, p16_irq_en[ucore], REG_PORT_ANY, 0), val);

            /* Set software interrupt to M0. */
            val = P16_SELF_RESETf;
            soc_iproc_setreg(unit, soc_reg_addr(unit, p16_irq_set[ucore], REG_PORT_ANY, 0), val);

        } else {

            /* Disable interrupt mask. */
            soc_iproc_getreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), &val);
            val &= ~(1 << (P16_SW_PROG_INTR_IRQ - 64));
            soc_iproc_setreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), val);

            /* Clear software interrupt to M0. */
            val = P16_SELF_RESETf;
            soc_iproc_setreg(unit, soc_reg_addr(unit, p16_irq_clr[ucore], REG_PORT_ANY, 0), val);

            /* Disable interrupt.  */
            soc_iproc_getreg(unit, soc_reg_addr(unit, p16_irq_en[ucore], REG_PORT_ANY, 0), &val);
            val &= ~(P16_SELF_RESETf);
            soc_iproc_setreg(unit, soc_reg_addr(unit, p16_irq_en[ucore], REG_PORT_ANY, 0), val);
        }

    } else {

        if (reset) {

          /* Enable interrupt mask. */
          soc_iproc_getreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), &val);
          val |= (1 << (SW_PROG_INTR_IRQ - 64));
          soc_iproc_setreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), val);

          /* Set software interrupt to M0. */
          READ_ICFG_PCIE_SW_PROG_INTRr(unit, &val);
          soc_reg_field_set(unit, ICFG_PCIE_SW_PROG_INTRr, &val, control_field[ucore], 1);
          WRITE_ICFG_PCIE_SW_PROG_INTRr(unit, val);
        } else {

          /* Disable interrupt mask. */
          soc_iproc_getreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), &val);
          val &= ~(1 << (SW_PROG_INTR_IRQ - 64));
          soc_iproc_setreg(unit, soc_reg_addr(unit, irq_mask[ucore], REG_PORT_ANY, 0), val);

          /* Clear software interrupt to M0. */
          READ_ICFG_PCIE_SW_PROG_INTRr(unit, &val);
          soc_reg_field_set(unit, ICFG_PCIE_SW_PROG_INTRr, &val, control_field[ucore], 0);
          WRITE_ICFG_PCIE_SW_PROG_INTRr(unit, val);
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_iproc_m0ssq_reset_ucore
 * Purpose:
 *     Reset Cortex-M0 in Iproc subsystem quad
 * Parameters:
 *     unit number
 *     ucore number
 *     reset Soft reset value.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0ssq_reset_ucore(int unit, int ucore, int reset)
{
    uint32 val = 0;
    int cur_reset = 0;
    int offset;
    soc_timeout_t to;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit,"ucore 0x%x reset 0x%x\n"), ucore, reset));

    /* Parameter check. */
    if (ucore >= MAX_UCORES || !SOC_REG_IS_VALID(unit, control_reg[ucore])) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "ucore %d doesn't exist.\n"), ucore));
        return SOC_E_PARAM;
    }

    /* Covert reset value as boolean. */
    reset = (reset != 0);

    /* Get reset bit. */
    soc_iproc_getreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), &val);

    /* If FW is running, notify FW to do self-reset interrupt. */
    cur_reset = soc_reg_field_get(unit, control_reg[ucore], val, SOFT_RESETf);

    LOG_DEBUG(BSL_LS_SOC_M0, (BSL_META_U(unit,"ucore 0x%x cur_reset 0x%x\n"), ucore, cur_reset));

    /* Don't execute M0 self reset flow for simulator and emulator. */
    if (SAL_BOOT_SIMULATION) {
        /* Set reset bit. */
        soc_iproc_getreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), &val);
        soc_reg_field_set(unit, control_reg[ucore], &val, SOFT_RESETf, reset);
        soc_iproc_setreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), val);
        return SOC_E_NONE;
    }

    if (reset) {

        if (cur_reset == 0) {

            LOG_DEBUG(BSL_LS_SOC_M0,
                     (BSL_META_U(unit,"ucore 0x%x self reset.\n"), ucore));

            /* Set M0 to reset itself */
            soc_iproc_m0_self_reset_set(unit, ucore, 1);

            /* Check reset completion, timeout = 500ms. */
            soc_timeout_init(&to, 500000, 100);
            do {
                 soc_iproc_getreg(unit, soc_reg_addr(unit, status_reg[ucore], REG_PORT_ANY, 0), &val);
                 val = soc_reg_field_get(unit, status_reg[ucore], val, SLEEPINGf);
            } while ((val == 0) && !soc_timeout_check(&to));

            if (val == 0) {
                uint32 addr, eapps;
                /* Check if eApps by verifying "0xbadc0de1" @ 0xc0 */
                addr = soc_iproc_percore_membase_get(unit, ucore);
                eapps = soc_cm_iproc_read(unit, (addr + 0xC0));
                if (eapps != 0xbadc0de1) {
                    int rv, image_size = 0;
                    uint32 load_crc, hw_crc;
                    rv = soc_iproc_m0ssq_uc_fw_crc(unit, 0, &image_size, &load_crc, &hw_crc);
                    LOG_ERROR(BSL_LS_SOC_M0,
                            (BSL_META_U(unit,"ucore 0x%x self reset fail. image_size=%d, load_crc=0x%x, hw_crc=0x%x(rv=%d)\n"),
                             ucore, image_size, load_crc, hw_crc, rv));
                }
            } else {
                /* Wait for data purge. */
                sal_usleep(300);
            }
        }

    }

    /* Clear M0 self reset. */
    soc_iproc_m0_self_reset_set(unit, ucore, 0);

    /* For U0 only. */
    if (cur_reset == 1 && reset == 0 && ucore == 0 && fwdbg_enable != 0) {
        fwlog_t fwlog;
        uint32 *ptr = (uint32 *) &fwlog;
        uint32 addr;

        fwlog.magic = FW_LOG_MAGIC;
        fwlog.ctrl = 0;
        fwlog.base = U0_LOG_BUFFER;
        fwlog.size = U0_LOG_BUFFER_SIZE  - sizeof(fwlog_t);
        fwlog.rp = 0;
        fwlog.wp = 0;

        for (addr = U0_LOG_BUFFER_SIZE - sizeof(fwlog_t);
             addr < U0_LOG_BUFFER_SIZE; addr += 4) {
             /* Clear M0 log */
             soc_m0ssq_sram_write32(unit, U0_LOG_BUFFER + addr, *ptr);
             ptr ++;
        }

        /* Clear debug FW signature. */
        for (offset = U0_DBG; offset < (U0_DBG_SIZE + U0_DBG); offset += 4) {
             soc_m0ssq_sram_write32(unit, offset, 0);
        }

        /* Enable FW log. */
        soc_m0ssq_sram_write32(unit, FW_DBG_EN, FW_DBG_EN_MAGIC);
    }

    /* Set reset bit. */
    soc_iproc_getreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), &val);
    soc_reg_field_set(unit, control_reg[ucore], &val, SOFT_RESETf, reset);
    soc_iproc_setreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), val);

    /* For U0 only. */
    if (ucore == 0 && fwdbg_enable == 0) {

        /* Awaiting 3ms for FW running */
        sal_usleep(3000);

        /* Check FW running or stop signature */
        val = soc_m0ssq_sram_read32(unit, FW_TAG);
        if (val != FW_TAG_START && reset == 0 && cur_reset == 1) {
             LOG_ERROR(BSL_LS_SOC_M0,
                      (BSL_META_U(unit,"ucore 0x%x is not running.\n"), ucore));
        }
        if (val !=  FW_TAG_STOP && reset == 1 && cur_reset == 0) {
             LOG_ERROR(BSL_LS_SOC_M0,
                      (BSL_META_U(unit,"ucore 0x%x is not stopped.\n"), ucore));
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_iproc_m0ssq_sram_init
 * Purpose:
 *      Initialize SRAM inside M0SSQ at the first SDK run after iproc reset.
 *
 *      SRAM init includes
 *      1. Clear SRAM content as zero. If the M0 core is running, corresponding
 *          TCM won't be cleared.
 *      2. Set the done bit into ICFG_GEN_PURPOSE_REGr[0]/ICFG_GEN_PURPOSE_REG0.
 *         ICFG_GEN_PURPOSE_REG[0] only will be reset when iproc reset.
 *
 * Parameters:
 *      unit number
 * Returns:
 *      N/A
 */
static void
soc_iproc_m0ssq_sram_init(int unit)
{
    uint32 ucnum;
    uint32 addr, sram_clean_bmp, zero;
    int reset, rv;

    LOG_VERBOSE(BSL_LS_SOC_M0,
               (BSL_META_U(unit,"M0SSQ SRAM initialization start.\n")));

    /* Clear all SRAM in M0SSQ system. */
    zero = 0;
    if (SOC_REG_IS_VALID(unit, ICFG_GEN_PURPOSE_REGr)) {
        READ_ICFG_GEN_PURPOSE_REGr(unit, 0, &sram_clean_bmp);
    } else if (SOC_REG_IS_VALID(unit, ICFG_GEN_PURPOSE_REG0r)) {
        READ_ICFG_GEN_PURPOSE_REG0r(unit, &sram_clean_bmp);
    } else {
        return;
    }

    /* if the content is not zero. */
    if (sram_clean_bmp) {
        return;
    }
#if defined(SOC_PCI_DEBUG) && defined(INCLUDE_CPU_I2C)
    else if (!SAL_BOOT_I2C_ACCESS) /* don't clear SRAM when using I2C, as this is slow */
#endif
    {

    /* Clear M0SSQ 128KB SRAM .*/
    if ((sram_clean_bmp & 0x1) == 0) {
        LOG_VERBOSE(BSL_LS_SOC_M0,
                  (BSL_META_U(unit,"Initialize M0SSQ 128KB SRAM.\n")));

        for (addr = soc_iproc_sram_membase_get(unit);
             addr < soc_iproc_sram_membase_get(unit) + (128 * 1024);
             addr += 4)  {
             soc_iproc_setreg(unit, addr, zero);
        }
        if (SOC_REG_IS_VALID(unit, M0SSQ_SRAM_LL_128K_STATUS2r)) {
            WRITE_M0SSQ_SRAM_LL_128K_STATUS2r(unit, zero);
        } else if (SOC_REG_IS_VALID(unit, U0_M0SSQ_ECC_STATUSr)) {
            WRITE_U0_M0SSQ_ECC_STATUSr(unit, zero);
        }
        sram_clean_bmp |= 0x01;
    }

    /* Clear U0 LED SRAM. */
    if ((sram_clean_bmp & 0x2) == 0) {
        LOG_VERBOSE(BSL_LS_SOC_M0,
                   (BSL_META_U(unit,"Initialize LED SRAM.\n")));

        for (addr = soc_iproc_percore_membase_get(unit, 0) + 0x9000;
             addr < soc_iproc_percore_membase_get(unit, 0) + 0xB000;
             addr += 4)  {
             soc_iproc_setreg(unit, addr, zero);
        }
        WRITE_U0_LED_SRAM_ECC_STATUSr(unit, zero);
        sram_clean_bmp |= 0x02;
    }

    /* Clear M0s' TCM */
    for (ucnum = 0; ucnum < MAX_UCORES; ucnum++) {

         if (sram_clean_bmp & (0x01 << (ucnum + 2))) continue;

         /* Reserve CM0.2 for PCIe loader log. */
         if (ucnum == 2)  continue;

         rv = soc_iproc_m0ssq_reset_ucore_get(unit, ucnum, &reset);
         if (SOC_FAILURE(rv) || (reset == 0)) {
             continue;
         }
         for (addr = soc_iproc_percore_membase_get(unit, ucnum);
              addr < (soc_iproc_percore_membase_get(unit, ucnum) +
                      soc_iproc_percore_memsize_get(unit, ucnum));
              addr += 4)  {
              soc_iproc_setreg(unit, addr, 0);
         }

         LOG_VERBOSE(BSL_LS_SOC_M0,
                    (BSL_META_U(unit,"Initialize CM0.%d TCM.\n"), ucnum));

         switch (ucnum) {
             case 0:
                 WRITE_U0_M0SS_ECC_STATUSr(unit, zero);
                 break;
             case 1:
                 WRITE_U1_M0SS_ECC_STATUSr(unit, zero);
                 break;
             case 2:
                 WRITE_U2_M0SS_ECC_STATUSr(unit, zero);
                 break;
             case 3:
                 WRITE_U3_M0SS_ECC_STATUSr(unit, zero);
                 break;
             default:
                 break;

         }
         sram_clean_bmp |= (0x01 << (ucnum + 2));
    }
    }

    if (SOC_REG_IS_VALID(unit, ICFG_GEN_PURPOSE_REGr)) {
        WRITE_ICFG_GEN_PURPOSE_REGr(unit, 0, sram_clean_bmp);
    } else if (SOC_REG_IS_VALID(unit, ICFG_GEN_PURPOSE_REG0r)) {
        WRITE_ICFG_GEN_PURPOSE_REG0r(unit, sram_clean_bmp);
    }
}

/*
 * Function:
 *      soc_iproc_m0ssq_reset_ucore_get
 * Purpose:
 *      Get reset of Cortex-M0 in Iproc subsystem quad
 * Parameters:
 *      unit number
 *      ucore number
 *      reset Soft reset value.
 * Returns:
 *      SOC_E_xxx
 */
int
soc_iproc_m0ssq_reset_ucore_get(int unit, int ucore, int *reset)
{
    uint32 val = 0;

    /* Parameter check. */
    if (reset == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    if (ucore >= MAX_UCORES || !SOC_REG_IS_VALID(unit, control_reg[ucore])) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "ucore %d doesn't exist.\n"), ucore));
        return SOC_E_PARAM;
    }

    /* Get reset bit. */
    soc_iproc_getreg(unit, soc_reg_addr(unit, control_reg[ucore], REG_PORT_ANY, 0), &val);
    *reset = soc_reg_field_get(unit, control_reg[ucore], val, SOFT_RESETf);

    return SOC_E_NONE;
}

#ifdef M0SSQ_RESET_ALL
/*
 * Function:
 *      _soc_iproc_m0ssq_reset
 * Purpose:
 *      Reset of Cortex-M0 in Iproc subsystem quad
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
static int
_soc_iproc_m0ssq_reset(int unit)
{
    soc_timeout_t to;
    int rv = SOC_E_NONE;
    int flag;
    uint32 val;

    LOG_CLI((BSL_META_U(unit, "Resetting M0SSQ ...")));

    /* Reset Assertion of Master/Slave IDMs */
    SOC_IF_ERROR_RETURN
        (READ_M0SSQ_M0_IDM_IDM_RESET_CONTROLr(unit, &val));
    soc_reg_field_set(unit, M0SSQ_M0_IDM_IDM_RESET_CONTROLr, &val, RESETf, 1);
    SOC_IF_ERROR_RETURN
        (WRITE_M0SSQ_M0_IDM_IDM_RESET_CONTROLr(unit, val));

    SOC_IF_ERROR_RETURN
        (READ_M0SSQ_S0_IDM_IDM_RESET_CONTROLr(unit, &val));
    soc_reg_field_set(unit, M0SSQ_S0_IDM_IDM_RESET_CONTROLr, &val, RESETf, 1);
    SOC_IF_ERROR_RETURN
        (WRITE_M0SSQ_S0_IDM_IDM_RESET_CONTROLr(unit, val));

    /* Reset removal of Master/Slave IDMs. */
    soc_timeout_init(&to, 200000, 100);
    do {
        SOC_IF_ERROR_RETURN
            (READ_M0SSQ_M0_IDM_IDM_RESET_CONTROLr(unit, &val));
        soc_reg_field_set(unit, M0SSQ_M0_IDM_IDM_RESET_CONTROLr, &val, RESETf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_M0SSQ_M0_IDM_IDM_RESET_CONTROLr(unit, val));

        SOC_IF_ERROR_RETURN
            (READ_M0SSQ_M0_IDM_IDM_RESET_CONTROLr(unit, &val));
        flag = soc_reg_field_get(unit, M0SSQ_M0_IDM_IDM_RESET_CONTROLr, val, RESETf);
    } while (flag && !soc_timeout_check(&to));

    if (flag) {
        LOG_ERROR(BSL_LS_SOC_M0,
                 (BSL_META_U(unit,"Reset M0SSQ_M0_IDM_IDM_RESET_CONTROL fail.\n")));
        rv = SOC_E_FAIL;
    }

    soc_timeout_init(&to, 200000, 100);
    do {
        SOC_IF_ERROR_RETURN
            (READ_M0SSQ_S0_IDM_IDM_RESET_CONTROLr(unit, &val));
        soc_reg_field_set(unit, M0SSQ_S0_IDM_IDM_RESET_CONTROLr, &val, RESETf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_M0SSQ_S0_IDM_IDM_RESET_CONTROLr(unit, val));

        SOC_IF_ERROR_RETURN
            (READ_M0SSQ_S0_IDM_IDM_RESET_CONTROLr(unit, &val));
        flag = soc_reg_field_get(unit, M0SSQ_S0_IDM_IDM_RESET_CONTROLr, val, RESETf);
    } while (flag);

    if (flag) {
        LOG_ERROR(BSL_LS_SOC_M0,
                 (BSL_META_U(unit,"Reset M0SSQ_S0_IDM_IDM_RESET_CONTROL fail.\n")));
        rv = SOC_E_FAIL;
    }

    return rv;
}
#endif /* M0SSQ_RESET_ALL */

/*
 * Function:
 *      soc_iproc_m0ssq_reset
 * Purpose:
 *      Reset Cortex-M0 subsystem quad.
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
void soc_iproc_m0ssq_reset(int unit)
{
    int uc;
    uint32 dont_rst = 0;
    int rst;
    int rv;

    /* if uc2 is being used for PCIe Firmware loader,
     * exclude it from reset.
     */
    rv = soc_iproc_m0ssq_reset_ucore_get(unit, 2, &rst);
    if (rst == 0 && SOC_SUCCESS(rv)) {
        dont_rst |= (1 << 2);
    }

    for(uc = 0; uc < MAX_UCORES; uc++) {

        rv = soc_iproc_m0ssq_reset_ucore_get(unit, uc, &rst);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_M0,
                     (BSL_META_U(unit,"Getting uc%d reset status failed.\n"), uc));
            continue;
        }
#ifdef BCM_CMICX_SUPPORT

        /* Don't reset eApps while warmboot mode. */
        if (sal_boot_flags_get() & BOOT_F_WARM_BOOT) {
            if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
                uint32 addr, eapps;
                /* Check if eApps by verifying "0xbadc0de1" @ 0xc0 */
                addr = soc_iproc_percore_membase_get(unit, uc);
                eapps = soc_cm_iproc_read(unit, (addr + 0xC0));
                if (eapps == 0xbadc0de1) {
                    if (rst == 0) {
                        /* eApp is running, don't rest it as warmboot mode. */
                        dont_rst |= (1 << uc);
                    }
                    continue;
                }
            }
        }
#endif

        /* Get each M0 core into reset. */
        if ((dont_rst & (1 << uc)) == 0) {
            (void)soc_iproc_m0ssq_reset_ucore(unit, uc, M0SSQ_RESET_ENABLE);
        }
    }

#ifdef M0SSQ_RESET_ALL
    /* Reset whole M0SSQ. */
    if (dont_rst == 0) {
        _soc_iproc_m0ssq_reset(unit);
    }
#endif
    /* Initialize M0SSQ SRAM. */
    soc_iproc_m0ssq_sram_init(unit);
}

/*
 * Function:
 *     iproc_m0ssq_uc_fw_dump
 * Purpose:
 *     Dump fimrware on console.
 * Parameters:
 *     unit Unit number
 *     offset Starting pointer of firmware downloading.
 *     data Firmware array.
 *     len Length in bytes.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0ssq_uc_fw_dump(int unit, int uc, int offset, int len)
{
    uint32 addr, data32;
    uint8 *data8 = (uint8 *) &data32;
    int wlen = (len + 3) & 0xFFFFFFFC;
    int i;

    addr = soc_iproc_percore_membase_get(unit, uc);
    addr += offset;

    for (i = 0; i < wlen; i += 4) {

        soc_iproc_getreg(unit, addr + i, &data32);

        if ((i % 16) == 0) {
            LOG_CLI((BSL_META_U(unit, "%04X: "), addr + i));
        }

        LOG_CLI((BSL_META_U(unit, "%02X %02X %02X %02X "), data8[3], data8[2], data8[1], data8[0]));

        if ((i % 16) == 12) {
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     iproc_m0ssq_uc_fw_get
 * Purpose:
 *     Get fimrware for specific ucore.
 * Parameters:
 *     unit Unit number
 *     data Firmware array.
 *     len Length in bytes.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0ssq_uc_fw_get(int unit, int uc, int* len, uint8* data)
{
    uint32 addr, data32;
    int ii, wlen;

    if (uc >= MAX_UCORES) {
        return SOC_E_PARAM;
    }

    wlen = (iproc_m0ssq_uc_fw_len[unit][uc] + 3) & 0xFFFFFFFC;
    if (*len < wlen) {
        return SOC_E_PARAM;
    }
    *len = iproc_m0ssq_uc_fw_len[unit][uc];

    addr = soc_iproc_percore_membase_get(unit, uc);
    for (ii=0; ii < wlen; ii+=4)
    {
        soc_iproc_getreg(unit, addr+ii, &data32);
        data[ii] = data32 & 0xFF;
        data[ii+1] = (data32 & 0xFF00) >> 8;
        data[ii+2] = (data32 & 0xFF0000) >> 16;
        data[ii+3] = (data32 & 0xFF000000) >> 24;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_iproc_m0ssq_uc_fw_crc
 * Purpose:
 *     Get fimrware crc info for specific ucore.
 * Parameters:
 *     unit Unit number
 *     len Length in bytes.
 *     crc32 of Firmware.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0ssq_uc_fw_crc(int unit, int uc, int* len, uint32* load_crc32, uint32* hw_crc32)
{
    int rv;
    int image_size;
    uint32 memsize;
    uint8 *image_buf;
    uint32 image_crc;

    memsize = soc_iproc_percore_memsize_get(unit, uc);
    image_size = (iproc_m0ssq_uc_fw_len[unit][uc] + 3) & 0xFFFFFFFC;
    if (image_size > memsize)
    {
        return SOC_E_INTERNAL;
    }

    image_buf = sal_alloc(image_size+4, "M0FW");
    if (image_buf == NULL)
    {
        return SOC_E_MEMORY;
    }
    rv = soc_iproc_m0ssq_uc_fw_get(unit, uc, &image_size, image_buf);
    if (rv == SOC_E_NONE)
    {
        image_crc = shr_crc32(~0, image_buf, image_size);

        *len = image_size;
        *load_crc32 = iproc_m0ssq_uc_fw_load_crc[unit][uc];
        *hw_crc32 = image_crc;
    }
    sal_free(image_buf);

    return rv;
}


/*
 * Function:
 *     iproc_m0ssq_uc_fw_load
 * Purpose:
 *     Purpose
 * Parameters:
 *     unit Unit number
 *     offset Starting pointer of firmware downloading.
 *     data Firmware array.
 *     len Length in bytes.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_iproc_m0ssq_uc_fw_load(int unit, int uc, int offset, const uint8 *data, int len)
{
    uint32 addr, data32;
    int i, eapps;
    int wlen = (len + 3) & 0xFFFFFFFC;
    int rst;

    /* Check if eApps by verifying "0xbadc0de1" @ 0xc0 */
    eapps = (data[0xc3] == 0xba) && (data[0xc2] == 0xdc) &&
            (data[0xc1] == 0x0d) && (data[0xc0] == 0xe1);

    if ((offset + len) >= soc_iproc_percore_memsize_get(unit, uc)) {
        return SOC_E_PARAM;
    }
    if (uc >= MAX_UCORES) {
        return SOC_E_PARAM;
    }

    if (offset == 0){
        iproc_m0ssq_uc_fw_len[unit][uc] = len;
        iproc_m0ssq_uc_fw_load_crc[unit][uc] = shr_crc32(~0, (uint8*)data, len);
    }

    SOC_IF_ERROR_RETURN
        (soc_iproc_m0ssq_reset_ucore_get(unit, uc, &rst));

    if (eapps) {
        (void)soc_iproc_m0ssq_reset_ucore(unit, uc, M0SSQ_RESET_ENABLE);
    } else if (rst == 0 && offset == 0) {
         LOG_DEBUG(BSL_LS_SOC_M0,
                  (BSL_META_U(unit,"ucore 0x%x load firmware as m0 is reset.\n"), uc));
        (void)soc_iproc_m0ssq_reset_ucore(unit, uc, M0SSQ_RESET_ENABLE);
    }

    addr = soc_iproc_percore_membase_get(unit, uc);
    addr += offset;

    for (i = 0; i < wlen; i += 4) {
        data32 = (data[i + 3] << 24) | (data[i + 2] << 16) | (data[i + 1] << 8) | data[i];
        soc_iproc_setreg(unit, addr + i, data32);
    }

    if (eapps) {
        addr = soc_iproc_percore_membase_get(unit, uc);
        soc_iproc_setreg(unit, addr + 0xc4, uc);
        (void)soc_iproc_m0ssq_reset_ucore(unit, uc, 0);
    } else if (offset == 0 && rst == 0) {
        (void)soc_iproc_m0ssq_reset_ucore(unit, uc, 0);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_quad_event_thread
 * Purpose:
 *      Thread to handle iproc Cortex-M0 subsystem events
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
void soc_iproc_quad_event_thread(void *unit_vp)
{
    int unit = M0SSQ_DECODE_UNIT(unit_vp);
    int ucnum = M0SSQ_DECODE_UCORE(unit_vp);
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_iproc_m0ssq_control_t *iproc_m0ssq = &soc->iproc_m0ssq_ctrl[ucnum];

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_IPROC_EVENT);
    }
#endif

    while (iproc_m0ssq->thread_interval != 0) {
        LOG_DEBUG(BSL_LS_SOC_M0, (BSL_META_U(unit,"soc_iproc_quad_event_thread: sleep %d\n"),
                                                iproc_m0ssq->thread_interval));

        (void) sal_sem_take(iproc_m0ssq->event_sema, sal_sem_FOREVER);
        soc_cmic_intr_enable(unit, SW_PROG_INTR);

        /* Interrupt Handler */
        soc_iproc_msgintr_handler(unit, NULL);
    }

    sal_sem_destroy(iproc_m0ssq->event_sema);
    iproc_m0ssq->thread_interval = 0;
    iproc_m0ssq->thread_pid = NULL;

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_IPROC_EVENT);
    }
#endif

    sal_thread_exit(0);
}

/*
 * Function:
 *      soc_iproc_m0ssq_init
 * Purpose:
 *      Initialize iproc Cortex-M0 subsystem
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0ssq_init(int unit)
{
   int rv = SOC_E_NONE;
   soc_control_t       *soc = SOC_CONTROL(unit);
   soc_iproc_m0ssq_control_t *iproc_m0ssq = NULL;
   uint32 interval = IPROC_M0SSQ_THREAD_INTERVAL;
   uint32 max_ucores, ucnum;

   if (iproc_m0ssq_init_done[unit]) {
       return rv;
   }


   /* Init max number of ucores enabled */
   _soc_iproc_fw_config(unit);

   /* Get max ucores enabled */
   max_ucores = _soc_iproc_num_ucore_get(unit);

   for (ucnum = 0; ucnum < max_ucores; ucnum++) {
       iproc_m0ssq = &soc->iproc_m0ssq_ctrl[ucnum];

       iproc_m0ssq->event_sema = sal_sem_create("m0ssq_intr", sal_sem_BINARY, 0);
       if (iproc_m0ssq->event_sema == NULL) {
           LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ event sem create failed\n")));
           return SOC_E_MEMORY;
       }

       sal_snprintf(iproc_m0ssq->thread_name, sizeof(iproc_m0ssq->thread_name),
                "IPROC_M0SSQ_EVENT.%d", unit);
       iproc_m0ssq->thread_interval = interval;
       iproc_m0ssq->thread_pid = sal_thread_create(iproc_m0ssq->thread_name,
                                                  SAL_THREAD_STKSZ,
                                                  soc_property_get(unit,
                                                                   spn_LINKSCAN_THREAD_PRI,
                                                                   IPROC_M0SSQ_THREAD_PRI),
                                                  (void (*)(void*))soc_iproc_quad_event_thread,
                                                  M0SSQ_ENCODE_UNIT_UCORE(unit, ucnum));

       if (iproc_m0ssq->thread_pid == SAL_THREAD_ERROR) {
           iproc_m0ssq->thread_interval = 0;
           sal_sem_destroy(iproc_m0ssq->event_sema);
           LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ thread creation failed \n")));
           return SOC_E_MEMORY;
       }
   }

   soc_cmic_intr_enable(unit, SW_PROG_INTR);

   iproc_m0ssq_init_done[unit] = 1;

   return rv;
}

/*
 * Function:
 *      soc_iproc_m0ssq_exit
 * Purpose:
 *      Exit iproc Cortex-M0 subsystem
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0ssq_exit(int unit)
{
    int rv = SOC_E_NONE;
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_iproc_m0ssq_control_t *iproc_m0ssq;
    uint32 ucnum;

    if (!iproc_m0ssq_init_done[unit]) {
        return rv;
    }

    for (ucnum = 0; ucnum < MAX_UCORES; ucnum++) {
        iproc_m0ssq = &soc->iproc_m0ssq_ctrl[ucnum];

        iproc_m0ssq->thread_interval = 0;

        if (iproc_m0ssq->event_sema) {
            sal_sem_give(iproc_m0ssq->event_sema);
        }
    }

    soc_cmic_intr_disable(unit, SW_PROG_INTR);

    iproc_m0ssq_init_done[unit] = 0;

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0_init
 * Purpose:
 *      Initialize iproc Cortex-M0 subsystem and mbox
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0_init(int unit)
{
    int rv = SOC_E_NONE;

    if (SOC_CONTROL(unit)->iproc_m0_init_done) {
        return rv;
    }

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "IPROC M0 init\n")));

    /* Initialize Iproc ARM Cortex-M0 subsystem quad */
    rv = soc_iproc_m0ssq_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ init failed\n")));
        return rv;
    }

    /* Initialize Iproc ARM Cortex-M0 subsystem quad(M0SSQ) mailbox */
    rv = soc_iproc_mbox_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc mbox init failed\n")));
        return rv;
    }

    /* Initialize LED mailbox */
    rv = soc_cmicx_led_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "LED mbox init failed\n")));
        return rv;
    }

    rv = soc_cmicx_link_fw_config_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "LED mbox init failed\n")));
        return rv;
    }

    /* bring M0 core out of reset */
    /* uC0 for Linkscan/LED */
    (void)soc_iproc_m0ssq_reset_ucore(unit, 0, 0);

    SOC_CONTROL(unit)->iproc_m0_init_done = 1;

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0_exit
 * Purpose:
 *      Exit iproc Cortex-M0 subsystem and mbox
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0_exit(int unit)
{
    int rv = SOC_E_NONE;
    int ucnum;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "IPROC M0 exit\n")));

    if (!SOC_CONTROL(unit)->iproc_m0_init_done) {
        return rv;
    }

    /* Call LED deinit */
    rv = soc_cmicx_led_deinit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "cmicx led deinit failed\n")));
        return rv;
    }

    /* Call Linkscan deinit */
    rv = soc_cmicx_linkscan_hw_deinit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "cmicx linkscan deinit failed\n")));
        return rv;
    }

    /* Cleanup Iproc ARM Cortex-M0 subsystem quad(M0SSQ) mailbox */
    rv = soc_iproc_mbox_exit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc mbox exit failed\n")));
        return rv;
    }

    /* Cleanup Iproc ARM Cortex-M0 subsystem quad */
    rv = soc_iproc_m0ssq_exit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ exit failed\n")));
        return rv;
    }

    for(ucnum = 0; ucnum < MAX_UCORES; ucnum++) {
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
            uint32 addr, eapps;

            /* Check if eApps by verifying "0xbadc0de1" @ 0xc0 */
            addr = soc_iproc_percore_membase_get(unit, ucnum);
            eapps = soc_cm_iproc_read(unit, (addr + 0xC0));
            if (eapps == 0xbadc0de1) {
#if defined(INCLUDE_IFA)
                uint32        offset;
                uint32        app_status[2], host_status;

                /* SRAM first 256Bytes reserved */
                offset     = (addr + BASEADDR_OFFSET(unit));

                app_status[0] = soc_cm_iproc_read(unit,
                        (offset + ifa_cc_offset(app_status)));
                app_status[1] = soc_cm_iproc_read(unit,
                        (offset + ifa_cc_offset(app_status) + 4));
                host_status   = soc_cm_iproc_read(unit,
                        (offset + ifa_cc_offset(host_status)));

                /*
                 * Validation check whether M0 is loaded with IFA_CC APP
                 */
                if ((app_status[0] == BCM_IFA_CC_APP_STATUS_1) &&
                    (app_status[1] == BCM_IFA_CC_APP_STATUS_2) &&
                    (host_status & BCM_IFA_CC_CONFIG_STATUS_BIT)) {
                    continue;
                }
#endif /* INCLUDE_IFA */
            }
        }
#endif

        /* uc2 is being used for PCIe Firmware loader,
           exclude it from reset */
        if (ucnum != 2) {
            (void)soc_iproc_m0ssq_reset_ucore(unit, ucnum, M0SSQ_RESET_ENABLE);
        }
    }

    SOC_CONTROL(unit)->iproc_m0_init_done = 0;

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_free
 * Purpose:
 *      Free shared memory object.
 * Parameters:
 *      shmem Shared memory object.
 * Returns:
 *      None.
 */
void
soc_iproc_m0ssq_shmem_free(soc_iproc_m0ssq_shmem_t *pshmem)
{
    if (pshmem == NULL) {
        return;
    }
    sal_free(pshmem);
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_get
 * Purpose:
 *      Get shared memory object by name.
 * Parameters:
 *      unit Device unit number.
 *      name Name of memory object.
 *      shmem Shared memory object.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_get(int unit, char *name, soc_iproc_m0ssq_shmem_t **pshmem)
{
    if (pshmem == NULL) {
        return SOC_E_PARAM;
    }

    if (sal_strncmp(name, "linkscan", 8) == 0) {

        /* Static allocation of linkscan shmem. */
        *pshmem = sal_alloc(sizeof(soc_iproc_m0ssq_shmem_t), "SocIprocM0ssqShmem");
        if (*pshmem == NULL) {
            return SOC_E_MEMORY;
        }

        (*pshmem)->size = CMICX_LS_SHMEM_SIZE;
        (*pshmem)->base = soc_iproc_percore_membase_get(unit, 0) +
                          soc_iproc_percore_memsize_get(unit, 0) -
                          CMICX_LS_SHMEM_OFFSET_DEFAULT;
        (*pshmem)->unit = unit;

        return SOC_E_NONE;
    } else if (strncmp(name, "led", 3) == 0) {

        /* Static allocation of led shmem. */
        *pshmem = sal_alloc(sizeof(soc_iproc_m0ssq_shmem_t), "SocIprocLedM0ssqShmem");
        if (*pshmem == NULL) {
            return SOC_E_MEMORY;
        }

        (*pshmem)->size = CMICX_LED_SHMEM_SIZE;
        (*pshmem)->base = soc_iproc_percore_membase_get(unit, 0) +
                          soc_iproc_percore_memsize_get(unit, 0) -
                          CMICX_LED_SHMEM_OFFSET_DEFAULT;
        (*pshmem)->unit = unit;

        return SOC_E_NONE;
    }


    return SOC_E_FAIL;
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_write32
 * Purpose:
 *      Write 32bits data into shared memory.
 * Parameters:
 *      shmem Shared memory object.
 *      offset Offset within memory object.
 *      value 32bits data.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_write32(soc_iproc_m0ssq_shmem_t *shmem,
                              uint32 offset, uint32 value)
{
    uint32 addr;

    if (shmem == NULL) {
        return SOC_E_UNAVAIL;
    }

    if (offset >= shmem->size) {
        return SOC_E_PARAM;
    }

    addr = shmem->base + offset;
    soc_iproc_setreg(shmem->unit, addr, value);

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_iproc_m0ssq_shmem_read32
 * Purpose:
 *      Read 32bits data from shared memory.
 * Parameters:
 *      shmem Shared memory object.
 *      offset Offset within memory object.
 *      value 32bits data.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_read32(soc_iproc_m0ssq_shmem_t *shmem,
                             uint32 offset, uint32 *value)
{
    uint32 addr;

    if (shmem == NULL ||
        value == NULL)
    {
        return SOC_E_UNAVAIL;
    }

    if (offset >= shmem->size) {
        return SOC_E_PARAM;
    }

    addr = shmem->base + offset;

    soc_iproc_getreg(shmem->unit, addr, value);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_clear
 * Purpose:
 *      Clear shared memory object as zero.
 * Parameters:
 *      shmem Shared memory object.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_clear(soc_iproc_m0ssq_shmem_t *pshmem)
{
    int rv;
    uint32 addr;
    for (addr = 0; addr < pshmem->size; addr += 4) {
        rv = soc_iproc_m0ssq_shmem_write32(pshmem, addr, 0);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_m0ssq_log_dump
 * Purpose:
 *      Dump log from memory.
 * Parameters:
 *      shmem Shared memory object.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_log_dump(int unit)
{
    uint32 rp;
    uint32 addr;
    uint32 val32;
    uint8 ch;
    int byte;
    soc_iproc_m0ssq_shmem_t shmem;
    fwlog_t fwlog;
    uint32 *ptr = (uint32 *) &fwlog;

    if (fwdbg_enable == 0) {
        return SOC_E_UNAVAIL;
    }

    shmem.size = U0_LOG_BUFFER_SIZE;
    shmem.base = U0_LOG_BUFFER + soc_iproc_sram_membase_get(unit);
    shmem.unit = unit;

    sal_memset(&fwlog, 0, sizeof(fwlog_t));

    /* Get write point and magic .*/
    for (addr = U0_LOG_BUFFER_SIZE - sizeof(fwlog_t);
         addr < U0_LOG_BUFFER_SIZE; addr += 4)
    {
        SOC_IF_ERROR_RETURN
            (soc_iproc_m0ssq_shmem_read32(&shmem, addr, ptr));
        ptr++;
    }

    if (fwlog.magic != FW_LOG_MAGIC) {
        return SOC_E_UNAVAIL;
    }

    if (fwlog.ctrl != 0) {
        rp = (fwlog.wp + 1)  % fwlog.size;
    } else {
        rp = 0;
    }

    /* Cyclic buffer, free run mode. */
    while (rp != fwlog.wp) {

        SOC_IF_ERROR_RETURN
            (soc_iproc_m0ssq_shmem_read32(&shmem, rp & 0xFFFFFFFC, &val32));

        for (byte = (rp % 4); byte < 4; byte ++) {
            ch = (val32 >> (byte * 8)) & 0xFF;
            cli_out("%c", ch);
            rp ++;
            if (rp >= (fwlog.size)) {
                rp = 0;
            }
            if (rp == fwlog.wp) {
                break;
            }
        }
    };

    return SOC_E_NONE;
}

#endif /* BCM_CMICX_SUPPORT */
#endif /* BCM_ESW_SUPPORT */

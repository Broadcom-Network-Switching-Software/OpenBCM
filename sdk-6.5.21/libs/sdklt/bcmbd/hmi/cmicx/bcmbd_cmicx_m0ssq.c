/*! \file bcmbd_cmicx_m0ssq.c
 *
 * CMICx-specific M0SSQ base driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>

#include <bcmdrd/bcmdrd_feature.h>

#include <bcmbd/bcmbd_m0ssq.h>
#include <bcmbd/bcmbd_m0ssq_internal.h>
#include <bcmbd/bcmbd_cmicx_sw_intr.h>
#include <bcmbd/bcmbd_sw_intr.h>
#include <bcmbd/bcmbd_cmicx_acc.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

/* M0 Software interrupt number. */
#define UC_M0SS_SW_PROG_INTR_NUM            73

/* Number of Cortex-M0. */
#define CMICX_NUM_OF_CORTEXM0               4

/* M0 shared SRAM offset. */
#define CMICX_M0SSQ_SRAM_OFFSET             0x40000

/* M0 shared SRAM size. */
#define CMICX_M0SSQ_SRAM_SIZE               (128 * 1024)

/* M0 TCM size. */
#define CMICX_M0SSQ_UC_MEM_SIZE             (32 * 1024)

/* LED accumulation ram offset. */
#define CMICX_LED_ACCU_RAM_OFFSET           0x9000

/* LED accumulation ram size. */
#define CMICX_LED_ACCU_RAM_SIZE             4096

/* LED send ram offset. */
#define CMICX_LED_SEND_RAM_OFFSET           0xA000

/* LED send ram size. */
#define CMICX_LED_SEND_RAM_SIZE             4096

/* Index of genernel purpose register for SRAM init. */
#define CMICX_M0SSQ_SRAM_INIT_MASK_IDX      0

/* M0SSQ SRAM init mask. */
#define CMICX_M0SSQ_SRAM_INIT               (1 << 0)

/* M0SSQ LED RAM init mask. */
#define CMICX_M0SSQ_LED_RAM_INIT            (1 << 1)

/* Offset of M0SSQ M0 TCM init mask. */
#define CMICX_M0SSQ_M0_TCM_INIT_OFFSET      2

/* PCIE controllor. */
#define CMICX_PCIE_UC                       2

/*******************************************************************************
 * CMICX specific M0SSQ Driver
 */
static int
cmicx_m0ssq_uc_start_get(int unit, uint32_t uc, bool *start)
{
    UC_M0SS_CONTROLr_t uc_ctrl;

    READ_UC_M0SS_CONTROLr(unit, uc, &uc_ctrl);
    *start = !UC_M0SS_CONTROLr_SOFT_RESETf_GET(uc_ctrl);

    return SHR_E_NONE;
}

/*!
 * \brief Inform M0 to do self-reset.
 *        Send a software intr to infom M0 to do SW reset to itself.
 *
 * \param [in]  unit Unit number.
 * \param [in]  uc uC number.
 *
 * \retval SHR_E_NONE Self-reset successfully.
 * \retval SHR_E_FAIL Self-reset failed.
 */

static int
cmicx_m0ssq_self_reset(int unit, uint32_t uc)
{
    UC_M0SS_STATUSr_t uc_status;
    UC_M0SS_INTR_MASKr_t uc_intr_mask;
    ICFG_PCIE_SW_PROG_INTRr_t uc_swintr_set;
    shr_timeout_t to;
    int rv = SHR_E_NONE;
    uint32_t val;

    /* Force to turn on UC software interrupt mask. */
    READ_UC_M0SS_INTR_MASKr(unit, uc, (UC_M0SS_SW_PROG_INTR_NUM / 32), &uc_intr_mask);
    val = UC_M0SS_INTR_MASKr_GET(uc_intr_mask);
    val |= (1 << (UC_M0SS_SW_PROG_INTR_NUM % 32));
    UC_M0SS_INTR_MASKr_SET(uc_intr_mask, val);
    WRITE_UC_M0SS_INTR_MASKr(unit, uc, (UC_M0SS_SW_PROG_INTR_NUM / 32), uc_intr_mask);

    /* Send a software interrupt to M0. */
    READ_ICFG_PCIE_SW_PROG_INTRr(unit, &uc_swintr_set);
    switch (uc) {
        case 0:
            ICFG_PCIE_SW_PROG_INTRr_CORTEXM0_U0f_SET(uc_swintr_set, 1);
            break;
        case 1:
            ICFG_PCIE_SW_PROG_INTRr_CORTEXM0_U1f_SET(uc_swintr_set, 1);
            break;
        case 2:
            ICFG_PCIE_SW_PROG_INTRr_CORTEXM0_U2f_SET(uc_swintr_set, 1);
            break;
        case 3:
            ICFG_PCIE_SW_PROG_INTRr_CORTEXM0_U3f_SET(uc_swintr_set, 1);
            break;
        default:
            break;
    }
    WRITE_ICFG_PCIE_SW_PROG_INTRr(unit, uc_swintr_set);

    /* Check reset completion, timeout = 200ms. */
    shr_timeout_init(&to, 200000, 100);

    do {
        READ_UC_M0SS_STATUSr(unit, uc, &uc_status);
        val = UC_M0SS_STATUSr_SLEEPINGf_GET(uc_status);
    } while (val == 0 && !shr_timeout_check(&to));

    /* if reset fail, print warning message and reset M0 directly. */
    if (val == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "M0 self reset fail.\n")));

        rv = SHR_E_FAIL;
    }

    /* Clear software interrupt to M0. */
    READ_ICFG_PCIE_SW_PROG_INTRr(unit, &uc_swintr_set);
    switch (uc) {
        case 0:
            ICFG_PCIE_SW_PROG_INTRr_CORTEXM0_U0f_SET(uc_swintr_set, 0);
            break;
        case 1:
            ICFG_PCIE_SW_PROG_INTRr_CORTEXM0_U1f_SET(uc_swintr_set, 0);
            break;
        case 2:
            ICFG_PCIE_SW_PROG_INTRr_CORTEXM0_U2f_SET(uc_swintr_set, 0);
            break;
        case 3:
            ICFG_PCIE_SW_PROG_INTRr_CORTEXM0_U3f_SET(uc_swintr_set, 0);
            break;
        default:
            break;
    }
    WRITE_ICFG_PCIE_SW_PROG_INTRr(unit, uc_swintr_set);

    /* Turn off UC software interrupt mask while UC is in reset state. */
    READ_UC_M0SS_INTR_MASKr(unit, uc, (UC_M0SS_SW_PROG_INTR_NUM / 32), &uc_intr_mask);
    val = UC_M0SS_INTR_MASKr_GET(uc_intr_mask);
    val &= ~(1 << (UC_M0SS_SW_PROG_INTR_NUM % 32));
    UC_M0SS_INTR_MASKr_SET(uc_intr_mask, val);
    WRITE_UC_M0SS_INTR_MASKr(unit, uc, (UC_M0SS_SW_PROG_INTR_NUM / 32), uc_intr_mask);

    return rv;
}

static int
cmicx_m0ssq_uc_start_set(int unit, uint32_t uc, bool start)
{
    UC_M0SS_CONTROLr_t uc_ctrl;

    READ_UC_M0SS_CONTROLr(unit, uc, &uc_ctrl);

    /* If FW is running, notify FW to stop by FW self-reset. */
    if (UC_M0SS_CONTROLr_SOFT_RESETf_GET(uc_ctrl) == 0 &&
        start == 0 &&
        bcmdrd_feature_is_real_hw(unit))
    {
        cmicx_m0ssq_self_reset(unit, uc);
    }

    UC_M0SS_CONTROLr_SOFT_RESETf_SET(uc_ctrl, !start);
    WRITE_UC_M0SS_CONTROLr(unit, uc, uc_ctrl);

    return SHR_E_NONE;
}

/*!
 * \brief Create memory objects for CMICx-M0SSQ driver.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE Successed.
 * \retval SHR_E_FAIL Failed.
 */
int
bcmbd_cmicx_m0ssq_mem_create(int unit) {

    bcmbd_m0ssq_dev_t *dev;
    bcmdrd_dev_id_t dev_id;
    bcmbd_m0ssq_mem_t *tcm;
    bcmbd_m0ssq_mem_t *fwcfg_mem;
    bcmbd_fwconfig_t *fwcfg;
    uint32_t uc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_dev_get(unit, &dev));

    dev->num_of_uc = CMICX_NUM_OF_CORTEXM0;

    /* Initialize the shared SRAM memory object. */
    dev->sram.base = dev->base_addr + CMICX_M0SSQ_SRAM_OFFSET;
    dev->sram.size = CMICX_M0SSQ_SRAM_SIZE;
    dev->sram.write32 = bcmbd_m0ssq_iproc_mem_write32;
    dev->sram.read32 = bcmbd_m0ssq_iproc_mem_read32;
    dev->sram.unit = unit;

    /* Initialize M0 TCM memory objects. */
    for (uc = 0; uc < dev->num_of_uc; uc++) {
        tcm = &dev->m0_tcm[uc];
        fwcfg_mem = &dev->fwconfig_mem[uc];
        fwcfg = &dev->fwconfig[uc];

        sal_memcpy(tcm, &dev->sram, sizeof(bcmbd_m0ssq_mem_t));
        tcm->base = dev->base_addr;
        tcm->size = CMICX_M0SSQ_UC_MEM_SIZE;

        /* Initialize firmware configuration memory objects. */
        sal_memcpy(fwcfg_mem, &dev->sram, sizeof(bcmbd_m0ssq_mem_t));
        fwcfg_mem->base = tcm->base + tcm->size - sizeof(bcmbd_fwconfig_t);
        fwcfg_mem->size = sizeof(bcmbd_fwconfig_t);

        /* Initialize firmware configuration setting. */
        SHR_IF_ERR_EXIT
            (bcmdrd_dev_id_get(unit, &dev_id));
        fwcfg->uc = uc;
        fwcfg->devid = dev_id.device_id;
        fwcfg->magic = M0SSQ_FWCFG_MAGIC;
        fwcfg->len = sizeof(bcmbd_fwconfig_t);
    }

    /* Create LED memery objects */
    sal_memcpy(&dev->led_accu_ram, &dev->m0_tcm[0], sizeof(bcmbd_m0ssq_mem_t));
    dev->led_accu_ram.base += CMICX_LED_ACCU_RAM_OFFSET;
    dev->led_accu_ram.size = CMICX_LED_ACCU_RAM_SIZE;
    sal_memcpy(&dev->led_send_ram, &dev->m0_tcm[0], sizeof(bcmbd_m0ssq_mem_t));
    dev->led_send_ram.base += CMICX_LED_SEND_RAM_OFFSET;
    dev->led_send_ram.size = CMICX_LED_SEND_RAM_SIZE;

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Initialize M0SSQ SRAM at the first SDK run after iproc reset.
 *
 * Clear SRAM content as zero unless the corrending M0 core is running.
 *
 * Set the done bit into ICFG_GEN_PURPOSE_REGr[0].
 * ICFG_GEN_PURPOSE_REG[0] which only can be reset when iProc is reset.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE SRAM initialization successed.
 * \retval SHR_E_FAIL SRAM initialization failed.
 */
static int
cmicx_m0ssq_sram_init(int unit)
{
    uint32_t uc;
    uint32_t sram_init;
    bcmbd_m0ssq_dev_t *dev;
    ICFG_GEN_PURPOSE_REGr_t general_reg;
    U0_LED_SRAM_ECC_STATUSr_t led_sram_ecc_status;
    U0_M0SSQ_ECC_STATUSr_t u0_m0ssq_ecc_status;
    UC_M0SS_CONTROLr_t uc_ctrl;
    UC_M0SS_ECC_STATUSr_t uc_m0ss_ecc_status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_dev_get(unit, &dev));

    /* General register will only be clear to zero as port
     * on reset or iproc reset.
     */
    READ_ICFG_GEN_PURPOSE_REGr(unit,
                               CMICX_M0SSQ_SRAM_INIT_MASK_IDX,
                               &general_reg);
    sram_init = ICFG_GEN_PURPOSE_REGr_GET(general_reg);

    /* Init M0SSQ SRAM. */
    if (!(sram_init & CMICX_M0SSQ_SRAM_INIT)) {

        /* Clear M0SSQ 128KB SRAM.*/
        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_mem_clear(&dev->sram));

        /* Clear ECC status of 128KB SRAM. */
        U0_M0SSQ_ECC_STATUSr_CLR(u0_m0ssq_ecc_status);
        WRITE_U0_M0SSQ_ECC_STATUSr(unit, u0_m0ssq_ecc_status);

        /* Mark the done bit. */
        sram_init |= CMICX_M0SSQ_SRAM_INIT;
    }

    /* Init U0 LED SRAM. */
    if (!(sram_init & CMICX_M0SSQ_LED_RAM_INIT)) {

        /* Clear U0 LED SRAM. */
        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_mem_clear(&dev->led_accu_ram));
        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_mem_clear(&dev->led_send_ram));

        /* Clear ECC status of U0 LED SRAM. */
        U0_LED_SRAM_ECC_STATUSr_CLR(led_sram_ecc_status);
        WRITE_U0_LED_SRAM_ECC_STATUSr(unit, led_sram_ecc_status);

        /* Mark the done bit. */
        sram_init |= CMICX_M0SSQ_LED_RAM_INIT;
    }

    /* Init uc TCMs. */
    for (uc = 0; uc < dev->num_of_uc; uc ++) {

        /* Check if uc's TCM init is done before. */
        if (sram_init & (1 << (uc + CMICX_M0SSQ_M0_TCM_INIT_OFFSET))) {
            continue;
        }

        /* if uc is out-of-reset, then bypass uc's TCM init. */
        READ_UC_M0SS_CONTROLr(unit, uc, &uc_ctrl);
        if (!UC_M0SS_CONTROLr_SOFT_RESETf_GET(uc_ctrl)) {
            continue;
        }

        /* if uc is used by PCIe loader for log/hotswap, then bypass it. */
        if (uc == CMICX_PCIE_UC) {
            continue;
        }

        /* Clear uc's TCM by zeros. */
        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_mem_clear(&dev->m0_tcm[uc]));

        /* Clear ECC status of uc's TCM. */
        UC_M0SS_ECC_STATUSr_CLR(uc_m0ss_ecc_status);
        WRITE_UC_M0SS_ECC_STATUSr(unit, uc, uc_m0ss_ecc_status);

        /* Mark the done bit. */
        sram_init |= (1 << (uc + CMICX_M0SSQ_M0_TCM_INIT_OFFSET));
    }

    ICFG_GEN_PURPOSE_REGr_SET(general_reg, sram_init);
    WRITE_ICFG_GEN_PURPOSE_REGr(unit,
                                CMICX_M0SSQ_SRAM_INIT_MASK_IDX,
                                general_reg);

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_m0ssq_init(int unit) {

    uint32_t uc;
    bcmbd_m0ssq_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_dev_get(unit, &dev));

    SHR_IF_ERR_EXIT
        (cmicx_m0ssq_sram_init(unit));

    dev->swintr_offset = CMICX_SWI_CORTEXM0_U0;

    for (uc = 0; uc < dev->num_of_uc; uc++) {

        /* Install interrupt primary handler on software interrupt framework. */
        SHR_IF_ERR_EXIT
            (bcmbd_sw_intr_func_set(unit, dev->swintr_offset + uc,
                                    bcmbd_m0ssq_uc_swintr_isr, uc));

        /* Enable interrupt primary handler. */
        SHR_IF_ERR_EXIT
            (bcmbd_sw_intr_enable(unit, dev->swintr_offset + uc));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_m0ssq_cleanup(int unit)
{
    uint32_t uc;
    bcmbd_m0ssq_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_dev_get(unit, &dev));

    /* Disable software interrupt primary handler. */
    for (uc = 0; uc < dev->num_of_uc; uc++) {
        bcmbd_sw_intr_disable(unit, dev->swintr_offset + uc);
    }

exit:
    SHR_FUNC_EXIT();
}

/* CMICx-M0SSQ driver. */
static const bcmbd_m0ssq_drv_t bcmbd_cmicx_m0ssq_base_drv = {

    /*! Initialize M0SSQ driver. */
    .init         = cmicx_m0ssq_init,

    /*! Cleanup M0SSQ driver. */
    .cleanup      = cmicx_m0ssq_cleanup,

    /*! Stop/start uC (Cortex-M0). */
    .uc_start_set = cmicx_m0ssq_uc_start_set,

    /*! Get uC (Cortex-M0) is started or not. */
    .uc_start_get = cmicx_m0ssq_uc_start_get,
};

/*******************************************************************************
 * Public Functions.
 */
const bcmbd_m0ssq_drv_t *
bcmbd_cmicx_m0ssq_base_drv_get(int unit)
{
    return &bcmbd_cmicx_m0ssq_base_drv;
}


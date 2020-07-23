/*! \file bcmbd_cmicx2_m0ssq.c
 *
 * CMICx2-specific M0SSQ base driver.
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
#include <bcmbd/bcmbd_cmicx2_sw_intr.h>
#include <bcmbd/bcmbd_sw_intr.h>
#include <bcmbd/bcmbd_cmicx2_acc.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

/* M0 Software interrupt number. */
#define UC_M0SS_SW_PROG_INTR_NUM             73

/* Index of genernel purpose register for SRAM init. */
#define CMICX2_M0SSQ_SRAM_INIT_MASK_IDX      0

/* M0SSQ SRAM init mask. */
#define CMICX2_M0SSQ_SRAM_INIT               (1 << 0)

/* M0SSQ LED RAM init mask. */
#define CMICX2_M0SSQ_LED_RAM_INIT            (1 << 1)

/* Offset of M0SSQ M0 TCM init mask. */
#define CMICX2_M0SSQ_M0_TCM_INIT_OFFSET      2

/* PCIE controllor. */
#define CMICX2_PCIE_UC                       2

/*******************************************************************************
 * CMICX specific M0SSQ Driver
 */
static int
cmicx2_m0ssq_uc_start_get(int unit, uint32_t uc, bool *start)
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
 * \retval SHR_E_NONE Self-reset uccessfully.
 * \retval SHR_E_FAIL Self-reset failed.
 */

static int
cmicx2_m0ssq_self_reset(int unit, uint32_t uc)
{
    UC_M0SS_STATUSr_t uc_status;
    UC_M0SS_INTR_MASK_95_64r_t uc_intr_mask;
    ICFG_PCIE_SW_PROG_INTRr_t uc_swintr_set;
    shr_timeout_t to;
    int rv = SHR_E_NONE;
    uint32_t val;

    /* Force to turn on UC software interrupt mask. */
    READ_UC_M0SS_INTR_MASK_95_64r(unit, uc, &uc_intr_mask);
    val = UC_M0SS_INTR_MASK_95_64r_GET(uc_intr_mask);
    val |= (1 << (UC_M0SS_SW_PROG_INTR_NUM - 64));
    UC_M0SS_INTR_MASK_95_64r_SET(uc_intr_mask, val);
    WRITE_UC_M0SS_INTR_MASK_95_64r(unit, uc, uc_intr_mask);

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
    READ_UC_M0SS_INTR_MASK_95_64r(unit, uc, &uc_intr_mask);
    val = UC_M0SS_INTR_MASK_95_64r_GET(uc_intr_mask);
    val &= ~(1 << (UC_M0SS_SW_PROG_INTR_NUM - 64));
    UC_M0SS_INTR_MASK_95_64r_SET(uc_intr_mask, val);
    WRITE_UC_M0SS_INTR_MASK_95_64r(unit, uc, uc_intr_mask);

    return rv;
}

static int
cmicx2_m0ssq_uc_start_set(int unit, uint32_t uc, bool start)
{
    UC_M0SS_CONTROLr_t uc_ctrl;

    READ_UC_M0SS_CONTROLr(unit, uc, &uc_ctrl);

    /* If FW is running, notify FW to stop by FW self-reset. */
    if (UC_M0SS_CONTROLr_SOFT_RESETf_GET(uc_ctrl) == 0 &&
        start == 0 &&
        bcmdrd_feature_is_real_hw(unit))
    {
        cmicx2_m0ssq_self_reset(unit, uc);
    }

    UC_M0SS_CONTROLr_SOFT_RESETf_SET(uc_ctrl, !start);
    WRITE_UC_M0SS_CONTROLr(unit, uc, uc_ctrl);

    return SHR_E_NONE;
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
cmicx2_m0ssq_sram_init(int unit)
{
    uint32_t uc;
    uint32_t sram_init;
    bcmbd_m0ssq_dev_t *dev;
    ICFG_GEN_PURPOSE_REGr_t general_reg;
    U0_LED_SRAM_ECC_STATUSr_t led_sram_ecc_status;
    M0SSQ_SRAM_LL_128K_STATUS2r_t m0ssq_sram_ll_128k_status2;
    UC_M0SS_CONTROLr_t uc_ctrl;
    UC_M0SS_ECC_STATUSr_t uc_m0ss_ecc_status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_dev_get(unit, &dev));

    /* General register will only be clear to zero as port
     * on reset or iproc reset.
     */
    READ_ICFG_GEN_PURPOSE_REGr(unit,
                               CMICX2_M0SSQ_SRAM_INIT_MASK_IDX,
                               &general_reg);
    sram_init = ICFG_GEN_PURPOSE_REGr_GET(general_reg);

    /* Clear M0SSQ 128KB SRAM .*/
    if (!(sram_init & CMICX2_M0SSQ_SRAM_INIT)) {

        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_mem_clear(&dev->sram));

        M0SSQ_SRAM_LL_128K_STATUS2r_CLR(m0ssq_sram_ll_128k_status2);
        WRITE_M0SSQ_SRAM_LL_128K_STATUS2r(unit, m0ssq_sram_ll_128k_status2);
        sram_init |= CMICX2_M0SSQ_SRAM_INIT;
    }

    /* Clear U0 LED SRAM. */
    if (!(sram_init & CMICX2_M0SSQ_LED_RAM_INIT)) {

        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_mem_clear(&dev->led_accu_ram));
        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_mem_clear(&dev->led_send_ram));

        U0_LED_SRAM_ECC_STATUSr_CLR(led_sram_ecc_status);
        WRITE_U0_LED_SRAM_ECC_STATUSr(unit, led_sram_ecc_status);
        sram_init |= CMICX2_M0SSQ_LED_RAM_INIT;
    }

    /* Init uc TCMs. */
    for (uc = 0; uc < dev->num_of_uc; uc ++) {

        /* Check if uc's TCM init is done before. */
        if (sram_init & (1 << (uc + CMICX2_M0SSQ_M0_TCM_INIT_OFFSET))) {
            continue;
        }

        /* if uc is out-of-reset, then bypass uc's TCM init. */
        READ_UC_M0SS_CONTROLr(unit, uc, &uc_ctrl);
        if (!UC_M0SS_CONTROLr_SOFT_RESETf_GET(uc_ctrl)) {
            continue;
        }

        /* if uc is used by PCIe loader for log/hotswap, then bypass it. */
        if (uc == CMICX2_PCIE_UC) {
            continue;
        }

        /* Clear uc's TCM by zeros. */
        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_mem_clear(&dev->m0_tcm[uc]));

        /* Clear ECC status of uc's TCM. */
        UC_M0SS_ECC_STATUSr_CLR(uc_m0ss_ecc_status);
        WRITE_UC_M0SS_ECC_STATUSr(unit, uc, uc_m0ss_ecc_status);

        /* Mark the done bit. */
        sram_init |= (1 << (uc + CMICX2_M0SSQ_M0_TCM_INIT_OFFSET));
    }

    ICFG_GEN_PURPOSE_REGr_SET(general_reg, sram_init);
    WRITE_ICFG_GEN_PURPOSE_REGr(unit,
                                CMICX2_M0SSQ_SRAM_INIT_MASK_IDX,
                                general_reg);

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx2_m0ssq_init(int unit)
{
    uint32_t uc;
    bcmbd_m0ssq_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_dev_get(unit, &dev));

    SHR_IF_ERR_EXIT
        (cmicx2_m0ssq_sram_init(unit));

    dev->swintr_offset = CMICX2_SWI_CORTEXM0_U0;

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
cmicx2_m0ssq_cleanup(int unit)
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
static const bcmbd_m0ssq_drv_t bcmbd_cmicx2_m0ssq_base_drv = {

    /*! Initialize M0SSQ driver. */
    .init         = cmicx2_m0ssq_init,

    /*! Cleanup M0SSQ driver. */
    .cleanup      = cmicx2_m0ssq_cleanup,

    /*! Stop/start uC (Cortex-M0). */
    .uc_start_set = cmicx2_m0ssq_uc_start_set,

    /*! Get uC (Cortex-M0) is started or not. */
    .uc_start_get = cmicx2_m0ssq_uc_start_get,
};

/*******************************************************************************
 * Public Functions.
 */
const bcmbd_m0ssq_drv_t *
bcmbd_cmicx2_m0ssq_base_drv_get(int unit)
{
    return &bcmbd_cmicx2_m0ssq_base_drv;
}


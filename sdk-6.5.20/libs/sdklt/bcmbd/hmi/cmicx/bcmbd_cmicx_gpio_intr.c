/*! \file bcmbd_cmicx_gpio_intr.c
 *
 * CMICx GPIO interrupt handler framework.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <sal/sal_spinlock.h>
#include <sal/sal_sem.h>
#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_cmicx_gpio.h>
#include <bcmbd/bcmbd_cmicx_gpio_intr.h>
#include <bcmbd/bcmbd_gpio_intr_internal.h>

/*******************************************************************************
 * Local definitions
 */

typedef struct intr_vect_s {
    bcmbd_intr_f func;
    uint32_t param;
} intr_vect_t;

typedef struct intr_vect_info_s {
    intr_vect_t intr_vects[BCMBD_CMICX_NUM_GPIO];
} intr_vect_info_t;

#define INTR_MASK_LOCK(_u) sal_spinlock_lock(gpio_intr_lock[_u]);
#define INTR_MASK_UNLOCK(_u) sal_spinlock_unlock(gpio_intr_lock[_u]);

/*******************************************************************************
 * Local data
 */

static intr_vect_info_t gpio_intr_vect_info[BCMDRD_CONFIG_MAX_UNITS];
static sal_spinlock_t gpio_intr_lock[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Interrupt driver functions
 */

static int
cmicx_gpio_intr_func_set(int unit, unsigned int intr_num,
                         bcmbd_intr_f intr_func, uint32_t intr_param)
{
    intr_vect_t *vi;

    if (intr_num >= BCMBD_CMICX_NUM_GPIO) {
        return SHR_E_PARAM;
    }

    vi = &gpio_intr_vect_info[unit].intr_vects[intr_num];
    vi->func = intr_func;
    vi->param= intr_param;

    return SHR_E_NONE;
}

static void
cmicx_gpio_intr_enable(int unit, unsigned int intr_num)
{
    uint32_t mask;
    GPIO_INT_MSKr_t intr_mask;

    if (intr_num >= BCMBD_CMICX_NUM_GPIO) {
        return;
    }

    INTR_MASK_LOCK(unit);

    READ_GPIO_INT_MSKr(unit, &intr_mask);
    mask = GPIO_INT_MSKr_INT_MSKf_GET(intr_mask);
    mask |= (1 << intr_num);
    GPIO_INT_MSKr_INT_MSKf_SET(intr_mask, mask);
    WRITE_GPIO_INT_MSKr(unit, intr_mask);

    INTR_MASK_UNLOCK(unit);
}

static void
cmicx_gpio_intr_disable(int unit, unsigned int intr_num)
{
    uint32_t mask;
    GPIO_INT_MSKr_t intr_mask;

    if (intr_num >= BCMBD_CMICX_NUM_GPIO) {
        return;
    }

    INTR_MASK_LOCK(unit);

    READ_GPIO_INT_MSKr(unit, &intr_mask);
    mask = GPIO_INT_MSKr_INT_MSKf_GET(intr_mask);
    mask &= ~(1 << intr_num);
    GPIO_INT_MSKr_INT_MSKf_SET(intr_mask, mask);
    WRITE_GPIO_INT_MSKr(unit, intr_mask);

    INTR_MASK_UNLOCK(unit);
}

static void
cmicx_gpio_intr_clear(int unit, unsigned int intr_num)
{
    uint32_t mask;
    GPIO_INT_CLRr_t intr_clr;

    if (intr_num >= BCMBD_CMICX_NUM_GPIO) {
        return;
    }

    mask = (1 << intr_num);
    GPIO_INT_CLRr_SET(intr_clr, mask);
    WRITE_GPIO_INT_CLRr(unit, intr_clr);
}

static bcmbd_intr_drv_t cmicx_gpio_intr_drv = {
    .intr_func_set = cmicx_gpio_intr_func_set,
    .intr_enable = cmicx_gpio_intr_enable,
    .intr_disable = cmicx_gpio_intr_disable,
    .intr_clear = cmicx_gpio_intr_clear
};

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Initialize GPIO interrupt type.
 *
 * This function enables interrupt generation on both rising and falling edges.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to initialize.
 */
static int
cmicx_gpio_intr_type_init(int unit)
{
    int ioerr = 0;
    uint32_t val = (uint32_t)-1;
    GPIO_INT_DEr_t intr_de;
    GPIO_INT_CLRr_t intr_clr;

    GPIO_INT_DEr_SET(intr_de, val);
    ioerr = WRITE_GPIO_INT_DEr(unit, intr_de);

    GPIO_INT_CLRr_SET(intr_clr, val);
    ioerr += WRITE_GPIO_INT_CLRr(unit, intr_clr);

    return (ioerr == 0) ? SHR_E_NONE : SHR_E_IO;
}

/*!
 * \brief Initialize GPIO interrupt driver.
 *
 * Allocates locks and other resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Already initialized.
 * \retval SHR_E_FAIL Fail to initialize.
 */
static int
cmicx_gpio_intr_init(int unit)
{
    if (gpio_intr_lock[unit]) {
        return SHR_E_INTERNAL;
    }

    gpio_intr_lock[unit] = sal_spinlock_create("bcmbdCmicxGpioIntr");
    if (!gpio_intr_lock[unit]) {
        return SHR_E_FAIL;
    }

    return cmicx_gpio_intr_type_init(unit);
}

/*!
 * \brief Clean up GPIO interrupt driver.
 *
 * Free resources allocated by \ref cmicx_gpio_intr_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
cmicx_gpio_intr_cleanup(int unit)
{
    if (gpio_intr_lock[unit]) {
        sal_spinlock_destroy(gpio_intr_lock[unit]);
        gpio_intr_lock[unit] = NULL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief CMICx primary GPIO ISR.
 *
 * \param [in] unit Unit number.
 * \param [in] param Interrupt context.
 *
 * \retval SHR_E_NONE No errors.
 */
static void
cmicx_gpio_intr(int unit, uint32_t param)
{
    intr_vect_info_t *ivi = &gpio_intr_vect_info[unit];
    intr_vect_t *iv;
    GPIO_INT_MSTATr_t intr_stat;
    uint32_t stat, intr, gpio;

    /* Read the Masked Interrupt Status Register */
    READ_GPIO_INT_MSTATr(unit, &intr_stat);
    stat = GPIO_INT_MSTATr_INT_MSTATf_GET(intr_stat);

    /* Handle all received interrupts */
    for (gpio = 0; gpio < BCMBD_CMICX_NUM_GPIO; gpio++) {
        intr = (1 << gpio);
        if (stat & intr) {
            /* Received an interrupt. Invoke handler function. */
            iv = &ivi->intr_vects[gpio];
            if (iv->func) {
                iv->func(unit, iv->param);
            }
        }
    }
}

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_gpio_intr_drv_init(int unit)
{
    sal_memset(&gpio_intr_vect_info[unit], 0, sizeof(intr_vect_info_t));
    return bcmbd_gpio_intr_drv_init(unit, &cmicx_gpio_intr_drv);
}

int
bcmbd_cmicx_gpio_intr_drv_cleanup(int unit)
{
    return bcmbd_gpio_intr_drv_init(unit, NULL);
}

int
bcmbd_cmicx_gpio_intr_start(int unit)
{
    int rv;

    /* Install GPIO interrupt handler */
    rv = bcmbd_cmicx_intr_func_set(unit, IPROC_IRQ_GPIO_INTR,
                                   cmicx_gpio_intr, 0);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Initialize GPIO interrupt settings */
    rv = cmicx_gpio_intr_init(unit);
    if (SHR_FAILURE(rv)) {
        (void)cmicx_gpio_intr_cleanup(unit);
        return rv;
    }

    /* Enable GPIO interrupt */
    bcmbd_cmicx_intr_enable(unit, IPROC_IRQ_GPIO_INTR);

    return SHR_E_NONE;
}

int
bcmbd_cmicx_gpio_intr_stop(int unit)
{
    /* Disable GPIO interrupt */
    bcmbd_cmicx_intr_disable(unit, IPROC_IRQ_GPIO_INTR);

    /* Cleanup software resources */
    cmicx_gpio_intr_cleanup(unit);

    return SHR_E_NONE;
}

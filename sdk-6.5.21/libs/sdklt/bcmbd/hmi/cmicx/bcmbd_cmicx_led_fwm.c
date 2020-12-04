/*! \file bcmbd_cmicx_led_fwm.c
 *
 * CMICx host base driver for FWM LED processor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_led_internal.h>
#include <bcmbd/bcmbd_m0ssq_internal.h>
#include <bcmbd/bcmbd_fwm.h>

/*******************************************************************************
 * Local definitions
 */

/* Processor name for reference by FWM */
#define CMICX_LED_FWM_PROC_NAME      "LEDUP" /* LED micro processor */

/*******************************************************************************
 * Local variables.
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * CMICx-LED FWM base processor driver.
 */

/* LED processor driver for FWM - start/stop/reset the processor */
static int
led_fwm_proc_control(int unit, int led_uc, bcmbd_fwm_proc_control_t ctrl)
{
    const bcmbd_led_drv_t *base_drv;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    if (led_uc != 0) {
        /* There is only one LED processor. */
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Invoke CMICx LED base driver to do the actual work. */
    base_drv = bcmbd_cmicx_led_base_drv_get(unit);
    SHR_NULL_CHECK(base_drv, SHR_E_INIT);
    SHR_NULL_CHECK(base_drv->uc_start_set, SHR_E_INIT);
    switch (ctrl) {
    case BCMBD_FWM_PROC_CONTROL_RESET:
        /* Stop then start */
        SHR_IF_ERR_EXIT
            (base_drv->uc_start_set(unit, led_uc, 0));
        /* Fall through */
    case BCMBD_FWM_PROC_CONTROL_START:
    case BCMBD_FWM_PROC_CONTROL_STOP:
        SHR_IF_ERR_EXIT
            (base_drv->uc_start_set(unit, led_uc,
                                    ctrl != BCMBD_FWM_PROC_CONTROL_STOP));
        break;
    default:
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/* LED processor driver for FWM - get processor state */
static int
led_fwm_proc_state(int unit, int led_uc, bcmbd_fwm_proc_state_t *state)
{
    const bcmbd_led_drv_t *base_drv;
    int started;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    if (led_uc != 0) {
        /* There is only one LED processor. */
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(state, SHR_E_PARAM);

    /* Invoke CMICx LED base driver to do the actual work. */
    base_drv = bcmbd_cmicx_led_base_drv_get(unit);
    SHR_NULL_CHECK(base_drv, SHR_E_INIT);
    SHR_NULL_CHECK(base_drv->uc_start_get, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (base_drv->uc_start_get(unit, led_uc, &started));
    *state = started ? BCMBD_FWM_PROC_STATE_STARTED :
                       BCMBD_FWM_PROC_STATE_STOPPED;

exit:
    SHR_FUNC_EXIT();
}

/* LED processor driver for FWM - translate address to system view */
static int
led_fwm_proc_xlate_from_proc(int unit, int led_uc,
                             uint64_t proc_addr, uint64_t *sys_addr)
{
    bcmbd_m0ssq_dev_t *dev;
    bcmbd_m0ssq_mem_t *mem;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    if (led_uc != 0) {
        /* There is only one LED processor. */
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(sys_addr, SHR_E_PARAM);

    /* The processor only supports 32-bit address. */
    if (proc_addr >> 32) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the base address and size of M0 TCM memory (local to processor) */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_dev_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_INTERNAL);
    mem = &dev->m0_tcm[led_uc];

    /* If the address in the TCM range (0 ~ TCM size), it's in TCM. */
    if (proc_addr < (uint64_t)mem->size) {
        *sys_addr = proc_addr + mem->base;
    } else {
        /* It's not a processor-local address. No translation required.*/
        *sys_addr = proc_addr;
    }

exit:
    SHR_FUNC_EXIT();
}

/* LED processor driver for FWM - translate address to processor view */
static int
led_fwm_proc_xlate_to_proc(int unit, int led_uc,
                           uint64_t sys_addr, uint64_t *proc_addr)
{
    bcmbd_m0ssq_dev_t *dev;
    bcmbd_m0ssq_mem_t *mem;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    if (led_uc != 0) {
        /* There is only one LED processor. */
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(proc_addr, SHR_E_PARAM);

    /* The processor only supports 32-bit address. */
    if (sys_addr >> 32) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the base address and size of M0 TCM memory (local to processor) */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_dev_get(unit, &dev));
    SHR_NULL_CHECK(dev, SHR_E_INTERNAL);
    mem = &dev->m0_tcm[led_uc];

    /* If the address in the TCM range, it's in TCM. */
    if (sys_addr >= (uint64_t)mem->base &&
        sys_addr < (uint64_t)mem->base + mem->size) {
        *proc_addr = sys_addr - mem->base;
    } else {
        /* It's not a processor-local address. No translation required.*/
        *proc_addr = sys_addr;
    }

exit:
    SHR_FUNC_EXIT();
}

/* LED processor driver for FWM - read processor memory */
static int
led_fwm_proc_read(int unit, int led_uc, uint64_t proc_addr,
                  uint8_t *buffer, int length)
{
    uint64_t sys_addr;
    uint32_t addr;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    if (led_uc != 0) {
        /* There is only one LED processor. */
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    if (length <= 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Convert to system address */
    SHR_IF_ERR_EXIT
        (led_fwm_proc_xlate_from_proc(unit, led_uc, proc_addr, &sys_addr));
    addr = (uint32_t)sys_addr; /* For performance as it's a 32-bit processor */

    /* Handle unaligned bytes from the beginning */
    if (addr % 4) {
        uint32_t val;
        int i;

        /* Read 32-bit value from aligned address */
        SHR_IF_ERR_EXIT
            (bcmdrd_hal_iproc_read(unit, (addr & ~3), &val));

        /* Read needed bytes from the 32-bit value */
        for (i = 1; i < 4 && length > 0; i++) {
            /* Shift needed byte to the lowest. Note that M0 is little endian */
            val >>= 8;
            if (i == (int)(addr % 4)) {
                *buffer = (uint8_t)val;
                buffer++;
                addr++;
                length--;
            }
        }
    }

    /* Handle remaining data */
    while (length > 0) {
        uint32_t val;
        int i;

        /* Read 32-bit value from memory */
        SHR_IF_ERR_EXIT
            (bcmdrd_hal_iproc_read(unit, addr, &val));

        /* Convert 32-bit value to bytes */
        for (i = 0; i < 4 && length > 0; i++) {
            *buffer = (uint8_t)val;
            buffer++;
            addr++;
            length--;
            /* M0 is little endian */
            val >>= 8;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* LED processor driver for FWM - write processor memory */
static int
led_fwm_proc_write(int unit, int led_uc, uint64_t proc_addr,
                   const uint8_t *buffer, int length)
{
    uint64_t sys_addr;
    uint32_t addr;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    if (led_uc != 0) {
        /* There is only one LED processor. */
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    if (length <= 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Convert to system address */
    SHR_IF_ERR_EXIT
        (led_fwm_proc_xlate_from_proc(unit, led_uc, proc_addr, &sys_addr));
    addr = (uint32_t)sys_addr; /* For performance as it's a 32-bit processor */

    /* Handle unaligned bytes from the beginning */
    if (addr % 4) {
        uint32_t aligned = addr & ~3;
        uint32_t val;
        int i;

        /* Read original 32-bit value from aligned address */
        SHR_IF_ERR_EXIT
            (bcmdrd_hal_iproc_read(unit, aligned, &val));

        /* Replace the value with partial bytes from the buffer */
        for (i = 1; i < 4 && length > 0; i++) {
            if (i == (int)(addr % 4)) {
                int shift = 8 * i;
                /* M0 is little endian */
                val &= ~(0xffUL << shift);
                val |= (uint32_t)(*buffer) << shift;
                buffer++;
                addr++;
                length--;
            }
        }

        /* Write the 32-bit value to the original aligned address */
        SHR_IF_ERR_EXIT
            (bcmdrd_hal_iproc_write(unit, aligned, val));
    }

    /* Handle aligned data */
    while (length > 4) {
        /* Convert 32-bit value to bytes (in little endian for M0)*/
        uint32_t val = ((uint32_t)buffer[0]) |
                       ((uint32_t)buffer[1] <<  8) |
                       ((uint32_t)buffer[2] << 16) |
                       ((uint32_t)buffer[3] << 24);

        /* Write the 32-bit value to memory */
        SHR_IF_ERR_EXIT
            (bcmdrd_hal_iproc_write(unit, addr, val));

        buffer += 4;
        addr += 4;
        length -= 4;
    }

    /* Handle remaining data (less than 4 bytes) */
    if (length > 0) {
        uint32_t val;
        int i;

        /* Read original 32-bit value from aligned address */
        SHR_IF_ERR_EXIT
            (bcmdrd_hal_iproc_read(unit, addr, &val));

        /* Replace the value with partial bytes from the buffer */
        for (i = 0; i < 4 && length > 0; i++) {
            int shift = 8 * i;
            /* M0 is little endian */
            val &= ~(0xffUL << shift);
            val |= (uint32_t)(*buffer) << shift;
            buffer++;
            length--;
        }

        /* Write the 32-bit value to the original aligned address */
        SHR_IF_ERR_EXIT
            (bcmdrd_hal_iproc_write(unit, addr, val));
    }

exit:
    SHR_FUNC_EXIT();
}

/* CMICx-LED FWM processor base driver. */
static const bcmbd_fwm_proc_t led_fwm_proc_base_drv = {

    /*! Name of the processor. */
    .name = CMICX_LED_FWM_PROC_NAME,

    /*! Control (start/stop/reset) the processor. */
    .control = led_fwm_proc_control,

    /*! Get current processor state. */
    .state = led_fwm_proc_state,

    /*! Read processor memory using the address in processor view. */
    .read = led_fwm_proc_read,

    /*! Write processor memory using the address in processor view. */
    .write = led_fwm_proc_write,

    /*! Translate address from processor view to system view. */
    .xlate_from_proc = led_fwm_proc_xlate_from_proc,

    /*! Translate address from system view to processor view. */
    .xlate_to_proc = led_fwm_proc_xlate_to_proc
};

/*******************************************************************************
 * Public Functions.
 */
const bcmbd_fwm_proc_t *
bcmbd_cmicx_led_fwm_proc_base_drv_get(int unit)
{
    return &led_fwm_proc_base_drv;
}

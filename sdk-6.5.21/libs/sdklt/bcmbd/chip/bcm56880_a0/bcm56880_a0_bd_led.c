/*! \file bcm56880_a0_bd_led.c
 *
 * Device-specific LED driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmbd/bcmbd_led_internal.h>
#include <bcmbd/bcmbd_cmicx_led.h>

#include "bcm56880_a0_drv.h"

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local defines.
 */
#define SWITCH_CORE_FREQ_HZ    (1500 * 1000000)
#define M0SSQ_FREQ_HZ          (1000 * 1000000)
#define LED_REFRESH_RATE_HZ    (30)
#define LED_LAST_PORT          (0)
#define LED_CLK_HZ             (5 * 1000000)

/*******************************************************************************
 * Local variables.
 */

/* Device-specific LED driver. */
static bcmbd_led_drv_t bcm56880_a0_bd_led_drv;

/*******************************************************************************
 * Device-specific driver.
 */
static int
bcm56880_a0_bd_led_init(int unit)
{
    const bcmbd_led_drv_t *base_drv;
    const bcmbd_fwm_proc_t *base_proc;

    SHR_FUNC_ENTER(unit);

    /* Get CMICx-LED base driver. */
    base_drv = bcmbd_cmicx_led_base_drv_get(unit);
    SHR_NULL_CHECK(base_drv, SHR_E_INIT);

    /* Initialize LED refresh rate. */
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_led_refresh_freq_set(unit, SWITCH_CORE_FREQ_HZ,
                                          LED_REFRESH_RATE_HZ));

    /* Initialize LED output clock rate. */
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_led_clk_freq_set(unit, M0SSQ_FREQ_HZ, LED_CLK_HZ));

    /* Last port setting of LED. */
    SHR_IF_ERR_EXIT
        (bcmbd_cmicx_led_last_port_set(unit, LED_LAST_PORT));

    /* Invoke CMICx-LED base driver init function. */
    SHR_IF_ERR_EXIT
        (base_drv->init(unit));

    /* Get FWM processor base driver. */
    base_proc = bcmbd_cmicx_led_fwm_proc_base_drv_get(unit);
    SHR_NULL_CHECK(base_proc, SHR_E_INIT);

    /* Register the FWM processor (no override needed for this device). */
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_register(unit, base_proc, 1)); /* Only one instance */

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions.
 */
int
bcm56880_a0_bd_led_drv_init(int unit)
{
    const bcmbd_led_drv_t *base_drv;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Get CMICx-LED base driver. */
    base_drv = bcmbd_cmicx_led_base_drv_get(unit);
    SHR_NULL_CHECK(base_drv, SHR_E_INIT);

    /* Inherit driver methods from LED base driver. */
    sal_memcpy(&bcm56880_a0_bd_led_drv, base_drv, sizeof(bcmbd_led_drv_t));

    /* Override with chip-specific driver methods. */
    bcm56880_a0_bd_led_drv.init = bcm56880_a0_bd_led_init;

    /* Install driver methods. */
    SHR_IF_ERR_EXIT
        (bcmbd_led_drv_init(unit, &bcm56880_a0_bd_led_drv));

exit:
    SHR_FUNC_EXIT();
}

int bcm56880_a0_bd_led_drv_cleanup(int unit)
{
    return bcmbd_led_drv_init(unit, NULL);
}

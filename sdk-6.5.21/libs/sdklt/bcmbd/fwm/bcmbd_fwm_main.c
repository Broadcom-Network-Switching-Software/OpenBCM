/*! \file bcmbd_fwm_main.c
 *
 * Main entry of Firmware Management module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <shr/shr_util.h>
#include <bcmbd/bcmbd_fwm.h>
#include "./bcmbd_fwm_internal.h"

/* Log source for this component. */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_FWM

/*******************************************************************************
 * Public functions
 */
/* Create Firmware Management resource of certain device. */
int
bcmbd_fwm_dev_create(int unit)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Create Firmware Management resource of "
                            "unit %d.\n"), unit));

    /* Initialize each sub-feature. */
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_fpkg_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_notif_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_parser_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_validator_init(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_fw_op_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/* Destroy Firmware Management resource of certain device. */
int
bcmbd_fwm_dev_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Destory Firmware Management resource of "
                            "unit %d.\n"), unit));

    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_cleanup(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_fpkg_cleanup(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_notif_cleanup(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_parser_cleanup(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_validator_cleanup(unit));
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_fw_op_cleanup(unit));
exit:
    SHR_FUNC_EXIT();
}

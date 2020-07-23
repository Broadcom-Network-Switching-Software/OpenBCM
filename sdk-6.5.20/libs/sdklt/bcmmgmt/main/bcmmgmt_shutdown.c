/*! \file bcmmgmt_shutdown.c
 *
 * Single entry point shut-down of core SDK components.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmmgmt/bcmmgmt.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

/*******************************************************************************
 * Public functions
 */

int
bcmmgmt_shutdown(bool graceful)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmmgmt_dev_detach_all());

    SHR_IF_ERR_EXIT
        (bcmmgmt_core_stop(graceful));

    SHR_IF_ERR_CONT
        (bcmmgmt_core_cleanup());

exit:
    SHR_FUNC_EXIT();
}

/*! \file bcmbd_cmicx_schan_error.c
 *
 * Non-standard S-channel error handlers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_schan_error.h>

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_SCHAN

/*
 * This error handler only fails on the primary error indicators and
 * ignores the contents of the CMIC_SCHAN_ERR register.
 */
static int
subpipe_error(int unit, int ch, uint32_t *schan_msg, uint32_t reg_val)
{
    int rv = SHR_E_NONE;
    CMIC_SCHAN_CTRLr_t ctrl;

    CMIC_SCHAN_CTRLr_SET(ctrl, reg_val);
    if (CMIC_SCHAN_CTRLr_NACKf_GET(ctrl)) {
        rv = SHR_E_FAIL;
    }
    if (CMIC_SCHAN_CTRLr_SER_CHECK_FAILf_GET(ctrl)) {
        rv = SHR_E_FAIL;
    }
    if (CMIC_SCHAN_CTRLr_TIMEOUTf_GET(ctrl)) {
        rv = SHR_E_FAIL;
    }

    return rv;
}

int
bcmbd_cmicx_schan_subpipe_error_func_init(int unit)
{
    return bcmbd_cmicx_schan_error_func_set(unit, subpipe_error);
}


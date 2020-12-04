/*! \file bcma_sys_probe.c
 *
 * SDKLT device probe function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>

#include <bcma/sys/bcma_sys_probe_xgssim.h>
#include <bcma/sys/bcma_sys_probe_plisim.h>
#include <bcma/sys/bcma_sys_probe_ngbde.h>
#include <bcma/sys/bcma_sys_probe.h>

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_probe_cleanup(void)
{
    int rv = 0;
    /* Clean up simulators first */
#ifdef BCMA_SYS_PROBE_XGSSIM
    rv += bcma_sys_probe_xgssim_cleanup();
#endif
#ifdef BCMA_SYS_PROBE_PLISIM
    rv += bcma_sys_probe_plisim_cleanup();
#endif
#ifdef BCMA_SYS_PROBE_NGBDE
    rv += bcma_sys_probe_ngbde_cleanup();
#endif
    return rv;
}

int
bcma_sys_probe(void)
{
    int rv = 0;
    /* Probe simulators first */
#ifdef BCMA_SYS_PROBE_XGSSIM
    rv += bcma_sys_probe_xgssim();
#endif
#ifdef BCMA_SYS_PROBE_PLISIM
    rv += bcma_sys_probe_plisim();
#endif
#ifdef BCMA_SYS_PROBE_NGBDE
    rv += bcma_sys_probe_ngbde();
#endif
    return rv;
}

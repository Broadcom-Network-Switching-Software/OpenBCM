/*! \file bcma_sys_probe_override.c
 *
 * SDKLT device probe override function.
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
bcma_sys_probe_override(int unit, const char *dev_name)
{
    int rv = 0;
    /* Exclusive OR since we can only override one probe type */
#if defined(BCMA_SYS_PROBE_XGSSIM)
    rv = bcma_sys_probe_xgssim_override(unit, dev_name);
#elif defined(BCMA_SYS_PROBE_PLISIM)
    rv += bcma_sys_probe_plisim_override(unit, dev_name);
#elif defined(BCMA_SYS_PROBE_NGBDE)
    rv += bcma_sys_probe_ngbde_override(unit, dev_name);
#endif
    return rv;
}

/*! \file bcma_sys_probe_config.c
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
bcma_sys_probe_config_set(const char *config)
{
    int rv = 0;
    /* Applies to PLISIM only */
#ifdef BCMA_SYS_PROBE_PLISIM
    rv += bcma_sys_probe_plisim_config_set(config);
#endif
    return rv;
}

const char *
bcma_sys_probe_config_get(const char *prev)
{
    const char *config = NULL;
    /* Applies to PLISIM only */
#ifdef BCMA_SYS_PROBE_PLISIM
    config = bcma_sys_probe_plisim_config_get(prev);
#endif
    return config;
}

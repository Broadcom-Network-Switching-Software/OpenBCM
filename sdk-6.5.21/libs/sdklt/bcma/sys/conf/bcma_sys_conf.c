/*! \file bcma_sys_conf.c
 *
 * Initialize system configuration object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcma/sys/bcma_sys_conf.h>

#include "sys_conf_internal.h"

int
bcma_sys_conf_init(bcma_sys_conf_t *sc)
{
    if (sc == NULL) {
        return -1;
    }

    sal_memset(sc, 0, sizeof(*sc));

    /* Initialize object signature for sanity checks */
    sc->sys_conf_sig = SYS_CONF_SIGNATURE;

    return 0;
}

int
bcma_sys_conf_cleanup(bcma_sys_conf_t *sc)
{
    /* Check for valid SYS_CONF structure */
    if (sc == NULL || BAD_SYS_CONF(sc)) {
        return -1;
    }

    /* Mark object as uninitialized */
    sc->sys_conf_sig = 0;

    return 0;
}

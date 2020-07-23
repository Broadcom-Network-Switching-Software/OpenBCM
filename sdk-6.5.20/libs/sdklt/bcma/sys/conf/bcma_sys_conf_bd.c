/*! \file bcma_sys_conf_bd.c
 *
 * Initialize base driver (BD) functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmbd/bcmbd_mcs.h>
#include <bcma/io/bcma_io_term.h>

#include <bcma/bcmbd/bcma_bcmbdcmd_cmicd.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_dev.h>
#include <bcma/sys/bcma_sys_conf.h>

/*******************************************************************************
 * Local functions
 */

static void
mcs_putc(int ch)
{
    char buf[1];

    buf[0] = ch;
    bcma_io_term_write(buf, 1);
}

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_bd_init(bcma_sys_conf_t *sc)
{
    /* Check for valid sys_conf structure */
    if (sc == NULL) {
        return -1;
    }

    /* Add CLI commands for base driver to debug shell */
    bcma_bcmbdcmd_add_cmicd_cmds(sc->dsh);
    bcma_bcmbdcmd_add_cmicx_cmds(sc->dsh);
    bcma_bcmbdcmd_add_dev_cmds(sc->dsh);

    /* Configure output function for MCS console(s) */
    bcmbd_mcs_putc_func_set(mcs_putc);

    return 0;
}

int
bcma_sys_conf_bd_cleanup(bcma_sys_conf_t *sc)
{
    return 0;
}

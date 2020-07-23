/*! \file bcma_sys_conf_cli.c
 *
 * Default prompt prefix handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/sys/bcma_sys_conf.h>

#include "sys_conf_internal.h"

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Handler for customizing the prefix of CLI command prompt.
 *
 * Adding a leading asterisk '*' to CLI prompt when the device is tainted.
 *
 * \param [in] unit Unit number.
 *
 * \return "*" when the device is tainted, otherwise NULL.
 */
static const char *
prompt_prefix_cb(struct bcma_cli_s *cli)
{
    int unit = cli->cur_unit;
    bool tainted = false;

    if (SHR_FAILURE(bcmdrd_dev_tainted_get(unit, &tainted))) {
        return NULL;
    }

    return tainted ? "*" : NULL;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_cli_prompt_prefix_default_set(bcma_sys_conf_t *sc)
{
    if (BAD_SYS_CONF(sc)) {
        return -1;
    }

    sc->cli_prompt_prefix_cb = prompt_prefix_cb;

    return 0;
}


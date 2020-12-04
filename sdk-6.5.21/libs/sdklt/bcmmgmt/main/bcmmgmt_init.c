/*! \file bcmmgmt_init.c
 *
 * Default SDK initialization sequence.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmmgmt/bcmmgmt_sysm_default.h>
#include <bcmmgmt/bcmmgmt.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

int
bcmmgmt_init(bool warm, const char *conf_file)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (warm && conf_file) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Ignoring configuration file %s (warm boot)\n"),
                  conf_file));
        conf_file = NULL;
    }

    SHR_IF_ERR_EXIT
        (bcmmgmt_core_init());

    SHR_IF_ERR_EXIT
        (bcmmgmt_core_config_load(conf_file));

    SHR_IF_ERR_EXIT
        (bcmmgmt_core_start(warm, bcmmgmt_sysm_default_comp_list()));

    SHR_IF_ERR_EXIT
        (bcmmgmt_dev_attach_all(warm));

exit:
    SHR_FUNC_EXIT();
}

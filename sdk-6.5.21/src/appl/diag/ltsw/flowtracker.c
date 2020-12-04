/*! \file flow.c
 *
 * LTSW FLOW command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(BCM_LTSW_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/ltsw/cmdlist.h>

#include <bcm_int/ltsw/flowtracker.h>

#include <shared/bsl.h>


/******************************************************************************
 * Private functions
 */

static cmd_result_t
cmd_ltsw_flowtracker_sw_show(int unit)
{
    int ret = BCM_E_NONE;

    ret = bcmi_ltsw_flowtracker_sw_dump(unit);
    if (ret < 0) {
        return CMD_FAIL;
    }
    return CMD_OK;
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Flow cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
cmd_result_t
cmd_ltsw_flowtracker(int unit, args_t *a)
{
    char *subcmd = NULL;

    subcmd = ARG_GET(a);
    if(!subcmd) {
        return (CMD_USAGE);
    } else if(!sal_strcasecmp(subcmd, "help")) {
        return (CMD_USAGE);
    }

    if (!sal_strcmp(subcmd, "sw")) {
        subcmd = ARG_GET(a);
        if (subcmd) {
            if (sal_strcmp(subcmd, "show")) {
                return (CMD_USAGE);
            }
            return cmd_ltsw_flowtracker_sw_show(unit);
        } else {
            return (CMD_USAGE);
        }
    } else {
        return (CMD_NOTIMPL);
    }

    return (CMD_OK);
}

#endif /* BCM_LTSW_SUPPORT */

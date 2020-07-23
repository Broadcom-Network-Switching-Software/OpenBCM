/** \file diag_dnx_srv6.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SRV6

/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

/*************
 * TYPEDEFS  *
 */

/*************
 *   MACROS  *
 */

/*************
* FUNCTIONS *
 */
static shr_error_e
sh_diag_dnx_srv6_test_diag(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_NONE, "SRv6 diagnostic are still missing, TBD");

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_srv6_man = {
    "TBD"
};

static sh_sand_option_t sh_dnx_srv6_options[] = {
    {NULL}
};
sh_sand_cmd_t sh_dnx_srv6_diag_cmds[] = {
    /** this is a TBD command name for an SRv6 diagnostics */
    {NULL, sh_diag_dnx_srv6_test_diag, NULL, sh_dnx_srv6_options, &sh_dnx_srv6_man},
    {NULL}
};
/* *INDENT-ON* */

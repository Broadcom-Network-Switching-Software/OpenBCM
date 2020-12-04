
/*! \file diag_dnx_flow.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_FLOW_H_INCLUDED
#define DIAG_DNX_FLOW_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include "src/bcm/dnx/flow/flow_def.h"
/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_flow_man;
extern sh_sand_cmd_t sh_dnx_flow_cmds[];

/*************
 * FUNCTIONS *
 */
shr_error_e diag_flow_app_from_string(
    int unit,
    char *app_name_str,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_t * flow_handle_id,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_FLOW_H_INCLUDED */

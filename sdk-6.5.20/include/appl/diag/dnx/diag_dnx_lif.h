/*
 * ! \file diag_dnx_lif.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_LIF_H_INCLUDED
#define DIAG_DNX_LIF_H_INCLUDED

/*************
 * INCLUDES  *
 */

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_lif_man;
extern sh_sand_cmd_t sh_dnx_lif_cmds[];

/*************
 * FUNCTIONS *
 */
shr_error_e dnx_lif_diag_logger_close(
    int unit,
    bsl_severity_t * orig_severity);
shr_error_e dnx_lif_diag_logger_restore(
    int unit,
    bsl_severity_t orig_severity);

#endif /* DIAG_DNX_LIF_H_INCLUDED */

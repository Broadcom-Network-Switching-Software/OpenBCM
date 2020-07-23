
/*! \file diag_dnx_error_recovery.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_ERROR_RECOVERY_H_INCLUDED
#define DIAG_DNX_ERROR_RECOVERY_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <soc/dnx/recovery/rollback_journal_utils.h>

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */

extern sh_sand_man_t sh_dnx_err_rec_transaction_jrnl_man;
extern sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_man;
extern sh_sand_man_t sh_dnx_err_rec_man;

extern sh_sand_cmd_t sh_dnx_err_rec_transaction_cmds[];
extern sh_sand_cmd_t sh_dnx_err_rec_comparison_cmds[];
extern sh_sand_cmd_t sh_dnx_err_rec_cmds[];

#endif /* DIAG_DNX_ERROR_RECOVERY_H_INCLUDED */

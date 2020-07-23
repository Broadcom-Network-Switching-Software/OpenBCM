/*
 * ! \file diag_sand_signals.h Purpose: shell command for signal ecosystem
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_DSIG_H_INCLUDED
#define DIAG_DNX_DSIG_H_INCLUDED

#include <shared/bsl.h>
#include <appl/diag/sand/diag_sand_framework.h>

extern sh_sand_cmd_t sh_sand_signal_cmds[];
extern sh_sand_man_t sh_sand_signal_man;

extern const char cmd_sand_signal_usage[];
extern const char cmd_sand_signal_desc[];

/* Serves Legacy invocation */
cmd_result_t cmd_sand_signal(
    int unit,
    args_t * args);

/*
 * takes a hex string and converts it to a hex number
 * returns val - pointer to uint32 array
 */
shr_error_e hex2int(
    char *hex,
    uint32 *val,
    uint8 verbosity);

/**
 * \brief
 *    Finds a signal according to the specified parameters. The resulting parsed signal tree resides as
 *   the last entry in *sig_list.
 *    By default, the signals are looked up using the following flags:
 *   SIGNALS_MATCH_EXPAND
 *   SIGNALS_MATCH_EXACT
 *   SIGNALS_MATCH_RETRY
 *   If more flags are required, they can be passed as the parameter extra_flags.
 *    If a NULL pointer is passed as *sig_list, (i.e outside this function, sig_list is declared, but
 *   nulled, for example: 'rhlist_t *sig_list = NULL; sand_signal_find(..., &sig_list);'), a new list
 *   will be created. Note that the argument itself must not be NULL.
 *   Note: The list must be freed, even if the signal was not found!
 */
shr_error_e sand_signal_find(
    int unit,
    int core,
    int extra_flags,
    char *block,
    char *from,
    char *to,
    char *name,
    rhlist_t ** sig_list);

/**
 * \brief - Check if the value of the signal is equal an expected one
 *
 *   \param [in] unit             - Unit ID
 *   \param [in] in_port          - Input Port
 *   \param [in] out_port         - Output Port
 *   \param [in] block            - Block name
 *   \param [in] from             - Stage/Memory signal originated from or passed through
 *   \param [in] to               - Stage/Memory signal destined to
 *   \param [in] name             - Signals name (with exact path in case 'strict' option is set)
 *   \param [in] sig_exp_value    - Expected value of the signal
 *   \param [in] fail_on_mismatch - If set, if signal is not matched, test would fail
 *   \param [in] strict           - If set, the path of the signal need to be given exact matched
 *
 * \return
 *   \retval int - Negative in case of an error, zero in case all ok.
 */
int compare_signal_value(
    int unit,
    int in_port,
    int out_port,
    char *block,
    char *from,
    char *to,
    char *name,
    char *sig_exp_value,
    int fail_on_mismatch,
    int strict);

#endif /* DIAG_SAND_ACCESS_H_INCLUDED */

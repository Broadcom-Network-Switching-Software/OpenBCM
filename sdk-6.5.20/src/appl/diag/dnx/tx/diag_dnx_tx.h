/**
 * \file diag_dnx_tx.h
 *
 * Header for diag command 'tx'.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_TX_H_INCLUDED
#define DIAG_DNX_TX_H_INCLUDED

/*************
 * INCLUDES  *
 */
/*
 * shared
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
/*
 * appl
 */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
/*
 * bcm
 */
#include <bcm/port.h>
#include <bcm/pkt.h>
#include <bcm/trunk.h>
#include <bcm/tx.h>

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_tx_man;
extern sh_sand_option_t sh_dnx_tx_options[];

/*************
 * FUNCTIONS *
 */

/**
 * \brief - TX command implementation
 * \param [in] unit - Unit #
 * \param [in] args - Command line arguments, may be NULL if called from outside the shell
 * \param [in] sand_control - Shell command control structure
 * \return
 *      Standard error handling.
 */
shr_error_e sh_dnx_tx(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_TX_H_INCLUDED */

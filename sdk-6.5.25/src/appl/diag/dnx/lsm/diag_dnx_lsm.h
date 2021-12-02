
/** \file diag_dnx_lsm.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/**
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_LSM_H_INCLUDED
/**
 * {
 */
#define DIAG_DNX_LSM_H_INCLUDED
/**
 * INCLUDES
 * {
 */
/*
 * }
 */

/**
 * DEFINES
 * {
 */
/*
 * }
 */

/**
 * MACROES
 * {
 */
/*
 * }
 */

/*
 * GLOBALS
 * {
 */

/**
 * The tool relies on CPU sending packet from CPU with visibility feature
 * enabling it to get information relevant to specific traffic, therefore
 * the Tool will do the below configuration on init (a diagnostic command not SDK init).
 * 1. Trap codes are configured to send packets to CPU(instead of drop packets).
 * 2. All ports are configured with visibility on and unforced.
 *       In order for visibility to work on packets sent with ptch.qualifier = 2
 * 3. Traped packets are re-sent from CPU with relevant PTCH.
 *
 */
extern sh_sand_man_t Dnx_lsm_man;

/**
* \brief
* List of shell options for 'lsm' shell commands
* \remark
* * Has options to init lsm event, run event and disaplay information on state of tool
*/
extern sh_sand_cmd_t Dnx_lsm_cmds[];
/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/**
 * }
 */
#endif /* DIAG_DNX_LSM_H_INCLUDED */

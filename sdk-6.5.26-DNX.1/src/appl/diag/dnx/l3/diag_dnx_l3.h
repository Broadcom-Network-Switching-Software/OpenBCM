/** \file diag_dnx_l3.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_PP_H_INCLUDED
#define DIAG_DNX_PP_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/dbal/dbal.h>

/*************
 *  DEFINES  *
 *************/
/** Number of symbols in the printable string for a MAC address in format 01:23:45:67:89:AB*/
#define DIAG_DNX_L3_MAC_ADDRESS_PRINTABLE_STRING_SIZE 18
/** Number of symbols in the printable string for an IPv6 */
#define DIAG_DNX_L3_IPV6_ADDRESS_PRINTABLE_STRING_SIZE 40
/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/
extern sh_sand_man_t sh_dnx_l3_man;
extern sh_sand_cmd_t sh_dnx_l3_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_intf_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_route_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_host_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_egr_cmds[];
extern sh_sand_cmd_t sh_dnx_l3_ecmp_cmds[];

/*************
 * FUNCTIONS *
 *************/

#endif /* DIAG_DNX_PP_H_INCLUDED */

/** \file diag_dnx_port_mgmt.h
 * 
 * DNX PORT MGMT DIAG PACK - diagnostic pack for port mgmt module (algo port module)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_PORT_MGMT_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_PORT_MGMT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



 /*
  * LOCAL DIAG PACK:
  * {
  */
extern sh_sand_cmd_t sh_dnx_port_mgmt_cmds[];
extern sh_sand_man_t sh_dnx_port_mgmt_man;


/*
 * }
 */
#endif /** DIAG_DNX_PORT_MGMT_H_INCLUDED*/

/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cmd_cint.h
 * Purpose:     Interfaces to diag shell CINT functions
 *
 * Note - this file needs to be includable by cint_yy.h, so cannot include any
 * SDK interfaces directly.
 */

#ifndef   _CMD_CINT_H_
#define   _CMD_CINT_H_

extern int cmd_cint_initialize(void);
extern void cmd_cint_fatal_error(char *msg);


#endif /* _CMD_CINT_H_ */

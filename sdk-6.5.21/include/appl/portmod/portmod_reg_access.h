/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _PORTMOD_REG_ACCESS_H_
#define _PORTMOD_REG_ACCESS_H_

int portmod_reg_access_init(int unit);
int portmod_reg_access_deinit(int unit);
int portmod_reg_access_port_bindex_set(int unit, int port, int block_index, int internal_index);

#endif /*_PORTMOD_REG_ACCESS_H_*/

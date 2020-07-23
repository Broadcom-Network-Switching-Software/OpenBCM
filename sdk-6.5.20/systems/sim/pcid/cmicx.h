/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * cmicx.h
 *
 * header for interface proviced by cmicx module
 */


#ifndef _CMICX_H_
#define _CMICX_H_

#include "cmicx_sim_util.h"
extern unsigned char cmicx_init(void);
extern unsigned char cmicx_update(pcid_info_t *pcid_info);
extern unsigned char cmicx_pcid_register_read(addr_t addr,reg_t *reg);
extern unsigned char cmicx_pcid_register_write(addr_t addr,reg_t reg);

#endif   /* _CMICX_H */


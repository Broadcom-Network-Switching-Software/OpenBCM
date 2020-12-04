/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __LINUX_SHBDE_H__
#define __LINUX_SHBDE_H__

#include <shbde.h>
#include <shbde_pci.h>
#include <shbde_iproc.h>

extern int
linux_shbde_hal_init(shbde_hal_t *shbde, shbde_log_func_t log_func);

#endif /* __LINUX_SHBDE_H__ */

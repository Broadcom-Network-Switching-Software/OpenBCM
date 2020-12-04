/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __PLIBDE_H__
#define __PLIBDE_H__

#include <ibde.h>

extern int
plibde_create(ibde_t** bde);

#ifdef ADAPTER_SERVER_MODE
extern int
plibde_cmodel_create(ibde_t** bde);
#endif

#ifdef SINGLE_MODE
extern int
plibde_local_create(ibde_t** bde);
#endif

#endif /* __PLIBDE_H__ */

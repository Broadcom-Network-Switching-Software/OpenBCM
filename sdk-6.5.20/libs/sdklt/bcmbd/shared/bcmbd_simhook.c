/*! \file bcmbd_simhook.c
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmbd/bcmbd_simhook.h>

int (*bcmbd_simhook_read)(int unit, uint32_t addrx, uint32_t addr,
                          void *vptr, int size);
int (*bcmbd_simhook_write)(int unit, uint32_t addrx, uint32_t addr,
                           void *vptr, int size);
int (*bcmbd_simhook_schan_op)(int unit, uint32_t *schan_msg,
                              int dwc_write, int dwc_read);

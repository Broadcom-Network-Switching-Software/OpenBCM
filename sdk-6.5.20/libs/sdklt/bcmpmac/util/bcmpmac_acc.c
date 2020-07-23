/*! \file bcmpmac_acc.c
 *
 * BCM PortMAC Access Functions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmpmac/bcmpmac_util_internal.h>

int
bcmpmac_read(bcmpmac_access_t *pm_acc, bcmpmac_pport_t pm_pport,
             uint32_t addr, uint32_t idx, size_t size, uint32_t *data)
{
    void *user_acc = pm_acc->user_acc;
    bcmpmac_reg_access_t *reg_acc = pm_acc->reg_acc;

    return reg_acc->read(user_acc, pm_pport, addr, idx, size, data);
}

int
bcmpmac_write(bcmpmac_access_t *pm_acc, bcmpmac_pport_t pm_pport,
              uint32_t addr, uint32_t idx, size_t size, uint32_t *data)
{
    void *user_acc = pm_acc->user_acc;
    bcmpmac_reg_access_t *reg_acc = pm_acc->reg_acc;

    return reg_acc->write(user_acc, pm_pport, addr, idx, size, data);
}

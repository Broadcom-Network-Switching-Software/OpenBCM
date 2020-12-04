
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_DEVICE_SEM_H

#define _DNXC_DEVICE_SEM_H

typedef int (
    *dnxc_device_semaphore_create) (
    int unit);

typedef int (
    *dnxc_device_semaphore_destroy) (
    int unit);

typedef int (
    *dnxc_device_semaphore_take) (
    int unit,
    int usec,
    char *dbg_str);

typedef int (
    *dnxc_device_semaphore_give) (
    int unit,
    char *dbg_str);

int dnxc_device_sem_register_cbs(
    int unit,
    dnxc_device_semaphore_take take,
    dnxc_device_semaphore_give give,
    dnxc_device_semaphore_create create,
    dnxc_device_semaphore_destroy destroy);

#endif

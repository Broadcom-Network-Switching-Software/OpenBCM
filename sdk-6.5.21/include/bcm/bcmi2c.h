/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_BCMI2C_H__
#define __BCM_BCMI2C_H__

#if defined(INCLUDE_I2C)

#include <bcm/types.h>

#ifndef BCM_HIDE_DISPATCHABLE

/* Open I2C device. */
extern int bcm_i2c_open(
    int unit, 
    char *devname, 
    uint32 flags, 
    int speed);

/* Write to I2C device. */
extern int bcm_i2c_write(
    int unit, 
    int fd, 
    uint32 addr, 
    uint8 *data, 
    uint32 nbytes);

/* Read from I2C device. */
extern int bcm_i2c_read(
    int unit, 
    int fd, 
    uint32 addr, 
    uint8 *data, 
    uint32 *nbytes);

/* Perform IOCTL access on I2C driver. */
extern int bcm_i2c_ioctl(
    int unit, 
    int fd, 
    int opcode, 
    void *data, 
    int len);

#endif /* defined(INCLUDE_I2C) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_BCMI2C_H__ */

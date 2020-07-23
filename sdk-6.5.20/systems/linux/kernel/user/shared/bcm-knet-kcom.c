/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	kcom.c
 * Purpose: 	Provides a kcom interface using device IOCTL
 */

#ifdef INCLUDE_KNET

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <kcom.h>

#include <bcm-knet.h>

#define KNET_DEVICE_NAME "/dev/linux-bcm-knet"
#define KNET_DEVICE_MAJOR 122
#define KNET_DEVICE_MINOR 0

static int kcom_fd = -1;

char *
bcm_knet_kcom_open(char *name)
{ 
    if (kcom_fd >= 0) {
	return name;
    }

    if ((kcom_fd = open(KNET_DEVICE_NAME,
                        O_RDWR | O_SYNC | O_DSYNC | O_RSYNC)) < 0) {
	perror("open " KNET_DEVICE_NAME ": ");
	return NULL;
    }	
    
    return name;
}

int
bcm_knet_kcom_close(void *handle)
{
    close(kcom_fd);
    kcom_fd = -1;

    return 0;
}

int
bcm_knet_kcom_msg_send(void *handle, void *msg,
                       unsigned int len, unsigned int bufsz)
{
    bkn_ioctl_t io;

    io.len = len;
    io.bufsz = bufsz;

    /*
     * Pass pointer as 64-bit int in order to support 32-bit
     * applications running on a 64-bit kernel.
     */
    io.buf = (uint64_t)(unsigned long)msg;

    if (ioctl(kcom_fd, 0, &io) < 0) {
	perror("send " KNET_DEVICE_NAME ": ");
        return -1;
    }

    if (io.rc < 0) {
        return -1;
    }
    return io.len;
}

int
bcm_knet_kcom_msg_recv(void *handle, void *msg,
                       unsigned int bufsz)
{
    bkn_ioctl_t io;

    io.len = 0;
    io.bufsz = bufsz;

    /*
     * Pass pointer as 64-bit int in order to support 32-bit
     * applications running on a 64-bit kernel.
     */
    io.buf = (uint64_t)(unsigned long)msg;

    do {
        if (ioctl(kcom_fd, 0, &io) < 0) {
            perror("recv " KNET_DEVICE_NAME ": ");
            return -1;
        }

        if (io.rc < 0) {
            return -1;
        }
    } while (io.len == 0);

    return io.len;
}

#endif

int _bcm_knet_kcom_c_not_empty;

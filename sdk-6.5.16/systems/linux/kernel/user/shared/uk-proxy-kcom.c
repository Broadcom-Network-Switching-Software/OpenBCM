/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File: 	kcom.c
 * Purpose: 	Provides a kcom interface using User/Kernel proxy services
 */

#ifdef INCLUDE_KNET

#include <sal/appl/io.h>
#include <kcom.h>

#include <linux-uk-proxy.h>
#include <uk-proxy-kcom.h>

static int uk_proxy = -1;

void *
uk_proxy_kcom_open(char *name)
{
    if (uk_proxy < 0) {
        uk_proxy = linux_uk_proxy_open(); 
        if (uk_proxy < 0) {
            return NULL;
        }
    }
    return name;
}

int
uk_proxy_kcom_close(void *handle)
{
    return 0;
}

int
uk_proxy_kcom_msg_send(void *handle, void *msg,
                       unsigned int len, unsigned int bufsz)
{
    if (len > LUK_MAX_DATA_SIZE) {
        return -1;
    }
    return linux_uk_proxy_send(handle, msg, len); 
}

int
uk_proxy_kcom_msg_recv(void *handle, void *msg,
                       unsigned int bufsz)
{
    unsigned int len;

    if (bufsz > LUK_MAX_DATA_SIZE) {
        return -1;
    }
    len = bufsz;
    if (linux_uk_proxy_recv(handle, msg, &len) < 0) {
        return -1;
    }
    return len;
}

#endif

int _uk_proxy_kcom_c_not_empty;

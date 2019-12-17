/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <string.h>

#include <linux-uk-proxy.h>

static int _dev_fd = -1;

int 
linux_uk_proxy_open(void)
{ 
    if (_dev_fd >= 0) {
	return _dev_fd;
    }

    if ((_dev_fd = open(LUK_DEVICE_NAME, O_RDWR | O_SYNC | O_DSYNC | O_RSYNC)) < 0) {
	/* Try inserting it from the local directory */
	perror("open " LUK_DEVICE_NAME ": ");
	return -1;
    }	
    
    return _dev_fd;
}

int
linux_uk_proxy_close(void)
{
    close(_dev_fd);
    return 0;
}

int
linux_uk_proxy_service_create(const char* service, unsigned int q_size, unsigned int flags)
{
    luk_ioctl_t io;

    strcpy(io.service, service);
    io.args.create.q_size = q_size;
    io.args.create.flags  = flags;
    if (ioctl(_dev_fd, LUK_SERVICE_CREATE, &io) >= 0) {
        return (io.rc);
    }

    return (-1);
}

int
linux_uk_proxy_service_destroy(const char* service)
{
    luk_ioctl_t io;

    strcpy(io.service, service);
    if (ioctl(_dev_fd, LUK_SERVICE_DESTROY, &io) >= 0) {
        return (io.rc);
    }

    return (-1);
}
	
int
linux_uk_proxy_recv(const char* service, void* data, unsigned int* len)
{
    luk_ioctl_t io;

    strcpy(io.service, service);
    io.args.recv.data = (uint64_t)(unsigned long)data;
    io.args.recv.len  = *len;

    /*
     * Return code -2 means that a signal was received (e.g. SIGSTOP 
     * from GDB). In this case no message is ready, and the operation
     * should simply be retried.
     */
    do {
        if (ioctl(_dev_fd, LUK_SERVICE_RECV, &io) < 0) {
            return -1;
        }
    } while (io.rc == -2);

    *len = io.args.recv.len;
    return io.rc;
}

int
linux_uk_proxy_send(const char* service, void* data, unsigned int len)
{
    luk_ioctl_t io;

    strcpy(io.service, service);
    io.args.send.data = (uint64_t)(unsigned long)data;
    io.args.send.len  = len;

    /*
     * Return code -2 means that a signal was received (e.g. SIGSTOP 
     * from GDB). In this case the message was not sent, and the 
     * operation should simply be retried.
     */
    do {
        if (ioctl(_dev_fd, LUK_SERVICE_SEND, &io) < 0) {
            return -1;
        }
    } while (io.rc == -2);

    return io.rc;
}

int
linux_uk_proxy_suspend(const char* service)
{
    luk_ioctl_t io;

    strcpy(io.service, service);

    ioctl(_dev_fd, LUK_SERVICE_SUSPEND, &io);
    return io.rc;
}

int
linux_uk_proxy_resume(const char* service)
{
    luk_ioctl_t io;

    strcpy(io.service, service);

    ioctl(_dev_fd, LUK_SERVICE_RESUME, &io);
    return io.rc;
}

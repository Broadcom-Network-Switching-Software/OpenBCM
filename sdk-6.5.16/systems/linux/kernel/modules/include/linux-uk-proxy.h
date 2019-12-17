/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#ifndef __LINUX_UK_PROXY_H__
#define __LINUX_UK_PROXY_H__

#ifndef __KERNEL__
#include <stdint.h>
#endif

/* Device Info */
#define LUK_DEVICE_NAME "/dev/linux-uk-proxy"
#define LUK_DEVICE_MAJOR 125
#define LUK_DEVICE_MINOR 0

#define LUK_MAX_SERVICE_NAME 64
#define LUK_MAX_DATA_SIZE (5*1024)

/* Control structure passed to all ioctls */
typedef struct {
    uint32_t v;
} luk_ioctl_version_t;

typedef struct {
    uint32_t q_size;
    uint32_t flags;
} luk_ioctl_create_t;

typedef struct {
    uint32_t flags;
} luk_ioctl_destroy_t;

typedef struct {
    uint64_t data;
    uint32_t len;
} luk_ioctl_recv_t;

typedef struct {
    uint64_t data;
    uint32_t len;
} luk_ioctl_send_t;    
    
typedef struct  {
    char service[LUK_MAX_SERVICE_NAME]; 
    int rc;                               /* Operation Return Code */    
    union {
        luk_ioctl_version_t version;
        luk_ioctl_create_t  create;
        luk_ioctl_destroy_t destroy;
        luk_ioctl_recv_t    recv;
        luk_ioctl_send_t    send;
    } args;
} luk_ioctl_t;

/* Linux User/Kernel Proxy ioctls */
#define LUK_MAGIC 'L'

#define LUK_VERSION 		_IO(LUK_MAGIC, 0)
#define LUK_SERVICE_CREATE	_IO(LUK_MAGIC, 1)
#define LUK_SERVICE_DESTROY	_IO(LUK_MAGIC, 2)
#define LUK_SERVICE_RECV	_IO(LUK_MAGIC, 3)
#define LUK_SERVICE_SEND	_IO(LUK_MAGIC, 4)
#define LUK_SERVICE_SUSPEND	_IO(LUK_MAGIC, 5)
#define LUK_SERVICE_RESUME	_IO(LUK_MAGIC, 6)

#ifdef __KERNEL__

#include <linux/ioctl.h>

/* Kernel API */
extern int linux_uk_proxy_service_create(const char *service, uint32_t q_size, uint32_t flags); 
extern int linux_uk_proxy_send(const char *service, void *data, uint32_t len); 
extern int linux_uk_proxy_recv(const char *service, void *data, uint32_t *len); 
extern int linux_uk_proxy_service_destroy(const char *service); 

#else

/* User API */
extern int linux_uk_proxy_open(void); 
extern int linux_uk_proxy_service_create(const char *service, uint32_t q_size, uint32_t flags); 
extern int linux_uk_proxy_service_destroy(const char *service); 
extern int linux_uk_proxy_send(const char *service, void *data, uint32_t len); 
extern int linux_uk_proxy_recv(const char *service, void *data, uint32_t *len); 
extern int linux_uk_proxy_suspend(const char *service); 
extern int linux_uk_proxy_resume(const char *service); 
extern int linux_uk_proxy_close(void); 

#endif /* __KERNEL__ */

#endif /* __LINUX_UK_PROXY_H__ */

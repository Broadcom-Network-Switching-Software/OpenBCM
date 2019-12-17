
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <gmodule.h> /* Must be included first */

#include <linux/list.h>
#include <linux-uk-proxy.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0)
#include <linux/uaccess.h>
#endif

MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("User/Kernel Mode Proxy");
MODULE_LICENSE("Proprietary");


#define LUK_MODULE_NAME "linux-uk-proxy"
#define LUK_SERVICE_QUEUE_MAX 8

/* Service data packet */
typedef struct luk_proxy_data_s {
    struct list_head list;    
    unsigned char data[LUK_MAX_DATA_SIZE]; 
    uint32_t len; 
} luk_proxy_data_t;

typedef struct luk_proxy_list_s {
    struct list_head list; 
    struct semaphore access_s; 
    struct semaphore count_s; 
    const char *service; 
    char name[32]; 
} luk_proxy_list_t; 
    
/* Service data queue */
typedef struct luk_proxy_data_q_s {
    luk_proxy_list_t data; 
    luk_proxy_list_t free; 
} luk_proxy_data_q_t; 

/* Service Entry */
typedef struct luk_proxy_service_s {
    struct list_head list; 
    
    char service[LUK_MAX_SERVICE_NAME];     

#define LUK_SERVICE_F_LOOPBACK 0x1	/* Service is in loopback mode */
#define LUK_SERVICE_F_INACTIVE 0x2	/* Service has been deleted */
#define LUK_SERVICE_F_SUSPENDED 0x4	/* Service has been suspended */

    int  flags; 

    luk_proxy_data_q_t recv_q; 
    luk_proxy_data_q_t send_q; 
    
} luk_proxy_service_t;

/* We have a list of services */
static struct list_head _service_list; 

#define DATA_ENTRY(l) list_entry(l, luk_proxy_data_t, list)
#define SERVICE_ENTRY(l) list_entry(l, luk_proxy_service_t, list)

/*
 * Function: _find_service
 *
 * Purpose:
 *    Find proxy service in list of active proxy services.
 * Parameters:
 *    service - proxy service name
 * Returns:
 *    Proxy service if found, otherwise NULL
 */
static luk_proxy_service_t *
_find_service(const char *service)
{
    struct list_head* list;
    list_for_each(list, &_service_list) {
        luk_proxy_service_t *s = SERVICE_ENTRY(list);
        if (!strcmp(service, s->service) && !(s->flags & LUK_SERVICE_F_INACTIVE)) {
            return s;
        }
    }
    return NULL; 
}

/*
 * Function: _init_data_q
 *
 * Purpose:
 *    Initialize proxy service data queue.
 * Parameters:
 *    q - data queue
 *    service - proxy service name associated with this queue
 *    q_name - name of data queue
 *    q_size - maximum number of packets in data queue
 * Returns:
 *    0 if success, otherwise -1
 */
static int
_init_data_q(luk_proxy_data_q_t *q, const char *service, const char *q_name, int q_size)
{
    int i;

    INIT_LIST_HEAD(&q->free.list);
    sema_init(&q->free.access_s, 1);
    sema_init(&q->free.count_s, q_size);
    q->free.service = service;
    sprintf(q->free.name, "%s.free", q_name);

    INIT_LIST_HEAD(&q->data.list);
    sema_init(&q->data.access_s, 1);
    sema_init(&q->data.count_s, 0);
    q->data.service = service;
    sprintf(q->data.name, "%s.data", q_name);
    
    for (i = 0; i < q_size; i++) {
        luk_proxy_data_t *data = kmalloc(sizeof(*data), GFP_KERNEL);

	if (!data) {
	    return -1;
	}
	list_add_tail(&data->list, &q->free.list);
    }
    return 0;
}
    
/*
 * Function: _list_op
 *
 * Purpose:
 *    Move data in and out of proxy service queues.
 * Parameters:
 *    source_list - source queue list (free or data)
 *    dest_list - destination queue list (free or data)
 *    data - buffer to copy data to/from
 *    len - length of data
 *    flags - data direction and context
 *      _LIST_OP_F_PUT: copy data from buffer to queue
 *      _LIST_OP_F_GET: copy data from from queue to buffer
 *      _LIST_OP_F_USER: buffer is in user space
 *      _LIST_OP_F_KERN: buffer is in kernel space
 * Returns:
 *    0 if success, otherwise -1
 */
#define _LIST_OP_F_PUT 0x1
#define _LIST_OP_F_GET 0x0
#define _LIST_OP_F_USER 0x2
#define _LIST_OP_F_KERN 0x0

static int
_list_op(luk_proxy_list_t *source_list, luk_proxy_list_t *dest_list, void *data, uint32_t *len, int flags)
{
    luk_proxy_data_t *dp; 
    int err;

#if 1
#define LDBG(l, ptr)
#else
#define LDBG(l, str) gprintk("%d:%s:%s - %s\n", current->pid, l->service, l->name, str)
#endif

    /* We need to take an entry from the source list */
    LDBG(source_list, "waiting for an entry");
    if ((err = down_interruptible(&source_list->count_s)) < 0) {
        return err; 
    }
    LDBG(source_list, "entry ready");

    /* We know there is at least one entry for us now. Wait to modify the list */
    LDBG(source_list, "locking..."); 
    if ((err = down_interruptible(&source_list->access_s)) < 0) {
        up(&source_list->count_s); 
        return err; 
    }

    LDBG(source_list, "locked - pulling off the entry..."); 

    /* Entry is now available from the source list, Remove it. */
    dp = DATA_ENTRY(source_list->list.next);
    list_del(&dp->list);
    up(&source_list->access_s);
    LDBG(source_list, "unlocked");
    
    /* Molest it at will */
    
    if (flags & _LIST_OP_F_PUT) {
        /* The given data should be pushed into this entry */
        if (flags & _LIST_OP_F_USER) {
            /* The data is from user space */
            if (copy_from_user((void *)(unsigned long)dp->data, data, *len)) {
                return -EFAULT;
            }
	} else {
            /* The data is from the kernel */	    
            memcpy((void *)(unsigned long)dp->data, data, *len);
	}
	dp->len = *len;
    } else {
	/* Deliver the data in this entry */
	if (flags & _LIST_OP_F_USER) {
            /* Data is going to user space */
            if (copy_to_user(data, (void *)(unsigned long)dp->data, dp->len)) {
                return -EFAULT;
            }
        } else {
            /* The data is going to the kernel */
            memcpy(data, (void *)(unsigned long)dp->data, dp->len);
        }
        *len = dp->len;
    }

    /* Now we want to add it to the dest list. Just need the access sem... */
    LDBG(dest_list, "locking...");
    if (down_interruptible(&dest_list->access_s));
    LDBG(dest_list, "locked - adding entry");
    list_add_tail(&dp->list, &dest_list->list);
    LDBG(dest_list, "unlocked");
    up(&dest_list->access_s);
    LDBG(dest_list, "incrementing count...");
    up(&dest_list->count_s);
    return 0;
}

/*
 * Function: _linux_uk_proxy_recv_from_user
 *
 * Purpose:
 *    Queue data packet from user space.
 * Parameters:
 *    service - proxy service name
 *    data - data received
 *    len - length of data
 * Returns:
 *    0 if success, otherwise -1
 * Notes:
 *    This function will block if the kernel receive queue is full.
 */
static int
_linux_uk_proxy_recv_from_user(const char *service, void *data, uint32_t len)
{
    int rc;
    luk_proxy_service_t *s;
    char service0[LUK_MAX_SERVICE_NAME] = { 0 };
    char service1[LUK_MAX_SERVICE_NAME] = { 0 };

    strcpy(service0, service);

    if (strchr(service, '|')) {
        /* This data should be piped between the given services */
        *strchr(service0, '|') = 0; 
        strcpy(service1, strchr(service, '|') + 1); 
    }

    /* Should this be piped to another service? */

    if (!(s = _find_service(service0))) {
        return -1;
    }

    if (s->flags & LUK_SERVICE_F_LOOPBACK) {
        strcpy(service1, service0);
    }
    
    /* We want to push this incoming data to this service's RX queue */
    rc = _list_op(&s->recv_q.free, &s->recv_q.data, data, &len, 
                 _LIST_OP_F_PUT | _LIST_OP_F_USER); 
    if (rc == -ERESTARTSYS || rc == -EINTR) {
        return -2;
    }

    if (service1[0]) {
        /* This should be piped to the given service */
        unsigned char d[LUK_MAX_DATA_SIZE];
        uint32_t len = LUK_MAX_DATA_SIZE;

        /* Get the packet from the incoming service */
        if (linux_uk_proxy_recv(service0, d, &len) < 0) {
            return 0;
        }

        /* Try to pipe the data. If the service doesn't exist, it will just drop. */
        linux_uk_proxy_send(service1, d, len);
    }
    return 0;
}

/*
 * Function: _linux_uk_proxy_send_to_user
 *
 * Purpose:
 *    Get data packet for user space.
 * Parameters:
 *    service - proxy service name
 *    data - buffer to write data to
 *    len - length of data returned
 * Returns:
 *    0 if success, otherwise -1
 * Notes:
 *    This function will block if no data is ready.
 */
static int
_linux_uk_proxy_send_to_user(const char *service, void *data, uint32_t *len)
{
    luk_proxy_service_t *s; 
    int rc;

    if (!(s = _find_service(service))) {
        return -1;
    }
    
    /* We want to get data from this service's tx queue */
    rc = _list_op(&s->send_q.data, &s->send_q.free, data, len, 
                  _LIST_OP_F_GET | _LIST_OP_F_USER); 
    if (rc == -ERESTARTSYS || rc == -EINTR) {
        return -2;
    }
    return 0;
}

/*
 * Function: _linux_uk_proxy_suspend
 *
 * Purpose:
 *    Suspend service and flush queues.
 * Parameters:
 *    service - proxy service name
 * Returns:
 *    0 if success, otherwise -1
 */
static int
_linux_uk_proxy_suspend(const char *service)
{
    luk_proxy_service_t *s; 

    if (!(s = _find_service(service))) {
        return -1;
    }
    
    s->flags |= LUK_SERVICE_F_SUSPENDED;

    return 0;
}

/*
 * Function: _linux_uk_proxy_resume
 *
 * Purpose:
 *    Resume a suspended service.
 * Parameters:
 *    service - proxy service name
 * Returns:
 *    0 if success, otherwise -1
 */
static int
_linux_uk_proxy_resume(const char *service)
{
    luk_proxy_service_t *s; 

    if (!(s = _find_service(service))) {
        return -1;
    }
    
    s->flags &= ~LUK_SERVICE_F_SUSPENDED;

    return 0;
}

/*
 * Generic module functions
 */

/*
 * Function: _init
 *
 * Purpose:
 *    Module initialization.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
static int
_init(void)
{
    INIT_LIST_HEAD(&_service_list);
    return 0;
}

/*
 * Function: _cleanup
 *
 * Purpose:
 *    Module cleanup function.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
static int
_cleanup(void)
{
    struct list_head *list, *tmplist; 
    list_for_each_safe(list, tmplist, &_service_list) {
        luk_proxy_service_t *s = SERVICE_ENTRY(list);
        linux_uk_proxy_service_destroy(s->service);
        list_del(list);
        kfree(s);
    }
    return 0;
}

/*
 * Function: _pprint
 *
 * Purpose:
 *    Print proc filesystem information.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
static int
_pprint(void)
{	
    int i = 0; 
    struct list_head *list;
    
    pprintf("Broadcom Linux User/Kernel Mode Proxy\n");
    pprintf("\tCurrent Services:\n");

    /* Current Service List */
    list_for_each(list, &_service_list) {
	luk_proxy_service_t *s = SERVICE_ENTRY(list);
	pprintf("\t\t%s ", s->service);
	pprintf(": %s%s%s\n",
                (s->flags & LUK_SERVICE_F_INACTIVE) ? "INACTIVE" : "ACTIVE",
                (s->flags & LUK_SERVICE_F_LOOPBACK) ? " LOOPBACK" : "",
                (s->flags & LUK_SERVICE_F_SUSPENDED) ? " SUSPENDED" : "");
	i++;
    }

    if (i == 0) {
        pprintf("None\n");
    }

    return 0;
}

/*
 * Function: _ioctl
 *
 * Purpose:
 *    Handle IOCTL commands from user mode.
 * Parameters:
 *    cmd - IOCTL cmd
 *    arg - IOCTL parameters
 * Returns:
 *    Always 0
 */
static int 
_ioctl(unsigned int cmd, unsigned long arg)
{
    luk_ioctl_t io; 

    if (copy_from_user(&io, (void*)arg, sizeof(io))) {
        return -EFAULT;
    }
  
    switch(cmd)	{
    case LUK_VERSION:
        io.rc = 1;
        break;
    case LUK_SERVICE_CREATE:
        /* Client wants to create a service */
        io.rc = linux_uk_proxy_service_create(io.service, 
                                              io.args.create.q_size, 
                                              io.args.create.flags);
        break;
    case LUK_SERVICE_DESTROY:
        /* Client wants to destroy a service */
        io.rc = linux_uk_proxy_service_destroy(io.service);
        break;
    case LUK_SERVICE_SEND:
        /* Client is sending data to a service */
        io.rc = _linux_uk_proxy_recv_from_user(io.service,
                                               (void *)(unsigned long)io.args.recv.data, 
                                               io.args.recv.len);
        break;
    case LUK_SERVICE_RECV:
        /* Client wants data from a service */
        io.rc = _linux_uk_proxy_send_to_user(io.service,
                                             (void *)(unsigned long)io.args.recv.data, 
                                             &io.args.recv.len);
        break;
    case LUK_SERVICE_SUSPEND:
        /* Client wants to suspend a service */
        io.rc = _linux_uk_proxy_suspend(io.service);
        break;
    case LUK_SERVICE_RESUME:
        /* Client wants to resume a service */
        io.rc = _linux_uk_proxy_resume(io.service);
        break;
    default:
        GDBG("INVALID IOCTL");
        io.rc = -1;
        break;
    }

    if (copy_to_user((void*)arg, &io, sizeof(io))) {
        return -EFAULT;
    }

    return 0;
}

static gmodule_t _gmodule = {
    name: LUK_MODULE_NAME,
    major: LUK_DEVICE_MAJOR,
    init: _init,
    cleanup: _cleanup,
    pprint: _pprint,
    ioctl: _ioctl,
    open: NULL,
    close: NULL,
};


gmodule_t *
gmodule_get(void)
{
    return &_gmodule;
}

/*
 * Exported functions
 */

/*
 * Function: linux_uk_proxy_service_create
 *
 * Purpose:
 *    Create new proxy service
 * Parameters:
 *    service - proxy service name
 *    q_size - maximum number of packets in I/O data queues
 * Returns:
 *    0 if success, otherwise -1
 * Notes:
 *    If the service exists already, no error is reported.
 */
int
linux_uk_proxy_service_create(const char *service, unsigned int q_size, unsigned int flags)
{
    luk_proxy_service_t *ns; 
    
    /* Service already active? */
    if (_find_service(service)) {
	return 0; 
    }
    
    ns = kmalloc(sizeof(luk_proxy_service_t), GFP_KERNEL); 

    if (!ns) {
        return -1;
    }

    strcpy(ns->service, service);
    _init_data_q(&ns->recv_q, ns->service, "recv_q", q_size);
    _init_data_q(&ns->send_q, ns->service, "send_q", q_size);

    ns->flags = flags;

    list_add_tail(&ns->list, &_service_list);

    return 0; 
}

/*
 * Function: linux_uk_proxy_service_destroy
 *
 * Purpose:
 *    Destroy active proxy service
 * Parameters:
 *    service - proxy service name
 * Returns:
 *    Always 0
 * Notes:
 *    The destroyed proxy service is just marked as inactive and
 *    resources are not freed until the module is unloaded.
 */
int
linux_uk_proxy_service_destroy(const char* service)
{
    luk_proxy_service_t *s;

    if ((s = _find_service(service))) {
	/* Mark it as inactive */
	s->flags |= LUK_SERVICE_F_INACTIVE; 
    }
    return 0;
}

/*
 * Function: linux_uk_proxy_recv
 *
 * Purpose:
 *    Used by a kernel process to receive data from user space.
 * Parameters:
 *    service - proxy service name
 *    data - buffer to write data to
 *    len - length of data returned
 * Returns:
 *    0 if success, otherwise -1
 * Notes:
 *    This function will block if no data is ready.
 */
int 
linux_uk_proxy_recv(const char *service, void *data, uint32_t *len)
{
    luk_proxy_service_t *s;
    
    if (!(s = _find_service(service))) {
        return -1;
    }

    /* We want to get data from this service's rx queue */
    return _list_op(&s->recv_q.data, &s->recv_q.free, data, len, _LIST_OP_F_GET | _LIST_OP_F_KERN);
}

/*
 * Function: linux_uk_proxy_send
 *
 * Purpose:
 *    Used by a kernel process to send data to user space.
 * Parameters:
 *    service - proxy service name
 *    data - data to send
 *    len - length of data
 * Returns:
 *    0 if success, otherwise -1
 * Notes:
 *    This function will block if the user receive queue is full.
 */
int
linux_uk_proxy_send(const char *service, void *data, uint32_t len)
{      
    luk_proxy_service_t *s;

    if (!(s = _find_service(service))) {
        return -1;
    }

    if (s->flags & LUK_SERVICE_F_SUSPENDED) {
        return 0;
    }

    /* We want to push this data to the services's tx queue */
    return _list_op(&s->send_q.free, &s->send_q.data, data, &len, _LIST_OP_F_PUT | _LIST_OP_F_KERN);
}


LKM_EXPORT_SYM(linux_uk_proxy_service_create);
LKM_EXPORT_SYM(linux_uk_proxy_service_destroy);
LKM_EXPORT_SYM(linux_uk_proxy_recv);
LKM_EXPORT_SYM(linux_uk_proxy_send);

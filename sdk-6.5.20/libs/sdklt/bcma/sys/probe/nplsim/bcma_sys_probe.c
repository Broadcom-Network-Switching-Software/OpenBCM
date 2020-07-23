/*! \file bcma_sys_probe.c
 *
 * Probe function for dumb simulator.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/sys/bcma_sys_sim.h>
#include <bcma/sys/bcma_sys_probe.h>

#include <bcmbd/bcmbd_simhook.h>

#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
#error Simulation environemnt does not support direct register access
#endif

#define BSL_LOG_MODULE  BSL_LS_SYS_PCI

/*******************************************************************************
 * BM IPC
 */

#define BM_RPC_MAX_BUF_LEN (64*(sizeof(uint32_t)))

#define BM_P2L_OP_SET           4       /* BM opcode to set an entry */
#define BM_L2P_OP_GET           5       /* BM opcode to get an entry */
#define BM_TYPE_INSERT          0       /* BM type code to insert an entry */
#define BM_TYPE_LOOKUP          3       /* BM type code to lookup an entry */

typedef struct bm_rpc_intf_s {
    int active;
    const char *port;
    const char *host;
    int sockfd;
    int sockpt;
    /* RPC buffer */
    int len;
    int pack;
    int unpack;
    uint8_t buf[BM_RPC_MAX_BUF_LEN*2]; /* Header + data */
} bm_rpc_intf_t;

typedef struct bm_rpc_data_s {
    /*! Unit number.*/
    uint32_t unit;

    /*! Table symbol ID.*/
    uint32_t sid;

    /*! Table index.*/
    uint32_t index;

    /*! Table entry buffer length.*/
    uint32_t buf_len;

    /*! Table entry data.*/
    uint8_t buf[BM_RPC_MAX_BUF_LEN]; /* Data only */
} bm_rpc_data_t;

static bm_rpc_intf_t bm_srv_handle[BCMDRD_CONFIG_MAX_UNITS];

/*
 *  Write "n" bytes to a descriptor.
 */
static int
bm_drv_write(int fd, const void *ptr, int nbytes)
{
    int nleft, nwritten;
    nleft = nbytes;
    while (nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if (nwritten <= 0) {
            return (nwritten);
        }
        nleft -= nwritten;
        ptr = (char *)ptr + nwritten;
    }
    return (nbytes - nleft);
}

/*
 *  Read "n" bytes from a descriptor.
 */
static int
bm_drv_read(int fd, void *ptr, int nbytes)
{
    int nleft, nread;
    nleft = nbytes;
    while (nleft > 0) {
        nread = read(fd, ptr, nleft);
        if (nread < 0) {
            return (nread);             /* error, return < 0 */
        }
        else if (nread == 0) {
            break;                      /* EOF */
        }
        nleft -= nread;
        ptr = (char *)ptr + nread;
    }
    return (nbytes - nleft);            /* return >= 0 */
}

/*
 * Start a message packing sequence.
 */
static int
bm_drv_pack_start(bm_rpc_intf_t *handle)
{
    handle->len = 0;
    handle->pack = 0;
    handle->unpack = 0;

    return 0;
}

/*
 * Pack a uint32_t into a message.
 */
static int
bm_drv_pack_u32(bm_rpc_intf_t *handle, uint32_t u)
{
    uint32_t nu = htonl(u);
    sal_memcpy(handle->buf + handle->pack, &nu, sizeof(nu));
    handle->pack += sizeof(nu);

    return 0;
}

/*
 * Pack a byte sequence into a message.
 */
static int
bm_drv_pack_buf(bm_rpc_intf_t *handle, const uint8_t *buf, int len)
{
    sal_memcpy(handle->buf + handle->pack, buf, len);
    handle->pack += len;

    return 0;
}

/*
 * Unpack a uint32_t from a message.
 */
static int
bm_drv_unpack_u32(bm_rpc_intf_t *handle, uint32_t *u)
{
    uint32_t nu;
    sal_memcpy(&nu, handle->buf + handle->unpack, sizeof(nu));
    handle->unpack += sizeof(nu);
    *u = ntohl(nu);

    return 0;
}

/*
 * Unpack a byte sequence from a message.
 */
static int
bm_drv_unpack_buf(bm_rpc_intf_t *handle, uint8_t *buf, int len)
{
    sal_memcpy(buf, handle->buf + handle->unpack, len);
    handle->unpack += len;

    return 0;
}


/*
 * Send a BM message.
 */
static int
bm_drv_send(bm_rpc_intf_t *handle)
{
    handle->len = handle->pack;

    return bm_drv_write(handle->sockfd, handle->buf, handle->len);
}

/*
 * Receive a BM message.
 */
static int
bm_drv_rcv(bm_rpc_intf_t *handle, int len)
{
    handle->len = len;

    return bm_drv_read(handle->sockfd, handle->buf, len);
}


/*!
 * \brief Open a connection to a Behavioral Model.
 *
 * \param [out] handle          RPC interface handle.
 * \param [in]  host            Behavioral Model hostname.
 * \param [in]  port            Behavioral Model port.
 *
 * \retval 0 - OK.
 * \retval 0 - -1 Error.
 */
int bm_rpc_intf_open(struct bm_rpc_intf_s *handle)
{
    struct sockaddr_in bm_addr;
    struct hostent *host;
    int rv = SHR_E_INTERNAL;
    int err;

    handle->sockpt = sal_atoi(handle->port);
    host = gethostbyname(handle->host);
    if (host == NULL) {
        return SHR_E_UNAVAIL;
    }
    sal_memset(&bm_addr, 0, sizeof(bm_addr));
    sal_memcpy(&bm_addr.sin_addr, host->h_addr_list[0], 4);

    bm_addr.sin_family = AF_INET;
    bm_addr.sin_port = htons(handle->sockpt);

    if ((handle->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return SHR_E_FAIL;
    }

    err = connect(handle->sockfd, (struct sockaddr*)&bm_addr, sizeof(bm_addr));
    if (err < 0) {
        close(handle->sockfd);
        rv = SHR_E_FAIL;
    } else {
        handle->active = 1;
        rv = SHR_E_NONE;
    }

    return rv;
}

/*!
 * \brief Close a connection to a Behavioral Model.
 *
 * \param [in]  handle          RPC interface handle.
 *
 * \retval 0 - OK.
 * \retval 0 - -1 Error.
 */
int bm_rpc_intf_close(struct bm_rpc_intf_s *handle)
{
    close(handle->sockfd);
    handle->active = 0;

    return SHR_E_NONE;
}

/*!
 * \brief Write a Behavioral Model table.
 *
 * \param [in]  handle          RPC interface handle.
 * \param [in]  data            Entry data.
 *
 * \retval 0 - OK, !0 - error.
 */
int bm_rpc_intf_p2l_set(int unit,
                        struct bm_rpc_intf_s *handle,
                        const bm_rpc_data_t *data)
{
    uint32_t resp;
    int err;

    bm_drv_pack_start(handle);
    bm_drv_pack_u32(handle, BM_P2L_OP_SET);               /* opcode */
    bm_drv_pack_u32(handle, unit);                        /* unit */
    bm_drv_pack_u32(handle, data->sid);                   /* BM sid  */
    bm_drv_pack_u32(handle, BM_TYPE_INSERT);              /* type */
    bm_drv_pack_u32(handle, data->index);                 /* index */
    bm_drv_pack_u32(handle, data->buf_len);               /* len */
    bm_drv_pack_buf(handle, data->buf, data->buf_len);    /* buf */
    err = bm_drv_send(handle);
    if (err < 0) {
        return SHR_E_FAIL;
    }

    err = bm_drv_rcv(handle, 2*4);
    if (err < 0) {
        return SHR_E_FAIL;
    }

    bm_drv_unpack_u32(handle, &resp);                     /* unused */
    bm_drv_unpack_u32(handle, &resp);                     /* status  */

    return resp ? SHR_E_FAIL : SHR_E_NONE;
}

/*!
 * \brief Read a Behavioral Model table.
 *
 * \param [in]     handle       RPC interface handle.
 * \param [in,out] data         Entry data.
 *
 * \retval 0 - OK, !0 - error.
 */
int bm_rpc_intf_l2p_get(int unit,
                        struct bm_rpc_intf_s *handle,
                        bm_rpc_data_t *data)
{
    uint32_t resp;
    uint32_t status;
    uint32_t data_len = data->buf_len;
    int err;

    bm_drv_pack_start(handle);
    bm_drv_pack_u32(handle, BM_L2P_OP_GET);      /* opcode */
    bm_drv_pack_u32(handle, unit);               /* unit */
    bm_drv_pack_u32(handle, data->sid);          /* BM sid  */
    bm_drv_pack_u32(handle, BM_TYPE_LOOKUP);     /* type */
    bm_drv_pack_u32(handle, data->index);        /* index */
    bm_drv_pack_u32(handle, data_len);           /* len */
    err = bm_drv_send(handle);
    if (err < 0) {
        return SHR_E_FAIL;
    }

    err = bm_drv_rcv(handle, data_len + (5*4));
    if (err < 0) {
        return SHR_E_FAIL;
    }

    bm_drv_unpack_u32(handle, &resp);            /* unit */
    bm_drv_unpack_u32(handle, &resp);            /* sid  */
    bm_drv_unpack_u32(handle, &status);          /* status */
    bm_drv_unpack_u32(handle, &resp);            /* index */
    bm_drv_unpack_u32(handle, &resp);            /* len */
    bm_drv_unpack_buf(handle, data->buf, data_len);  /* buf */
    data->buf_len = data_len;

    return status ? SHR_E_FAIL : SHR_E_NONE;
}

/*******************************************************************************
 * IPC Device Management
 */

#define MAX_ENV_STR 32

static const char *
bm_drv_env(int unit, const char *name)
{
    char envstr[MAX_ENV_STR];
    const char *value = NULL;

    if (unit == 0) {
        /* Special case for unit 0 - env needs no suffix. */
        value = getenv(name);
    }

    if (value == NULL) {
        (void)sal_snprintf(envstr, sizeof(envstr), "%s%d", name, unit);
        value = getenv(name);
    }

    return value;
}

static int
bm_sdk_msg_init(int unit)
{
    int rv;
    const char *port = NULL;
    const char *host = NULL;

    sal_memset(&bm_srv_handle[unit], 0, sizeof(bm_srv_handle[unit]));

    port = bm_drv_env(unit, "SDKLT_BM_SOCK");
    if (port == NULL) {
        /* No model, so just simulate. */
        return SHR_E_NONE;
    }

    host = bm_drv_env(unit, "SDKLT_BM_HOST");
    if (host == NULL) {
        host = "localhost";
    }

    bm_srv_handle[unit].host = host;
    bm_srv_handle[unit].port = port;

    rv = bm_rpc_intf_open(&bm_srv_handle[unit]);

    return rv;
}

static int
bm_sdk_msg_cleanup(int unit)
{
    int rv = SHR_E_NONE;

    if (bm_srv_handle[unit].active) {
        rv = bm_rpc_intf_close(&bm_srv_handle[unit]);
    }

    return rv;
}

/*******************************************************************************
 * Simulator hooks
 */

static int
nplsim_read(int unit, uint32_t addrx, uint32_t addr,
            void *data, size_t size)
{
    bm_rpc_data_t c_data;
    int rv = SHR_E_FAIL;

    sal_memset(&c_data, 0, sizeof(c_data));
    c_data.unit = unit;
    c_data.sid = addr;
    c_data.index = addrx;
    c_data.buf_len = size;
    if (bm_srv_handle[unit].active != 0) {
        rv = bm_rpc_intf_l2p_get(unit, &bm_srv_handle[unit], &c_data);
    }
    if (SHR_SUCCESS(rv) &&
        (c_data.buf_len & 3) == 0 &&
        (size_t)c_data.buf_len <= size) {
        sal_memcpy(data, c_data.buf, c_data.buf_len);
    } else {
        rv = SHR_E_FAIL;
    }

    return rv;
}

static int
nplsim_write(int unit, uint32_t addrx, uint32_t addr,
             void *data, size_t size)
{
    bm_rpc_data_t c_data;
    int rv = SHR_E_FAIL;

    sal_memset(&c_data, 0, sizeof(c_data));
    c_data.unit = unit;
    c_data.sid = addr;
    c_data.index = addrx;
    c_data.buf_len = BCMDRD_WORDS2BYTES(BCMDRD_MAX_PT_WSIZE);
    sal_memcpy(c_data.buf, data, c_data.buf_len);
    if (bm_srv_handle[unit].active != 0) {
        rv = bm_rpc_intf_p2l_set(unit, &bm_srv_handle[unit], &c_data);
    }

    return rv;
}

static int
io_read(void *dvc, bcmdrd_hal_io_space_t io_space,
        uint32_t addr, void *data, size_t size)
{
    int unit = *((int*)dvc);

    return nplsim_read(unit, 0, addr, data, size);
}

static int
io_write(void *dvc, bcmdrd_hal_io_space_t io_space,
         uint32_t addr, void *data, size_t size)
{
    int unit = *((int*)dvc);

    return nplsim_write(unit, 0, addr, data, size);
}

static int
sim_read(int unit, uint32_t addrx, uint32_t addr, void *data, int size)
{
    return nplsim_read(unit, addrx, addr, data, size);
}

static int
sim_write(int unit, uint32_t addrx, uint32_t addr, void *data, int size)
{
    return nplsim_write(unit, addrx, addr, data, size);
}

/*
 * These are the successfully create devices
 */
static struct {
    int unit;
} sys_devs[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_probe_cleanup(void)
{
    int unit;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmdrd_dev_exists(unit)) {
            bcmdrd_dev_destroy(unit);
            bm_sdk_msg_cleanup(unit);
        }
    }
    return 0;
}

int
bcma_sys_probe(void)
{
    int ndevs = 0;
    int edx, unit;
    bcmdrd_dev_id_t id;
    bcmdrd_hal_io_t io;
    const char *name;

    /* Install generic simulation hooks */
    bcmbd_simhook_read = sim_read;
    bcmbd_simhook_write = sim_write;

    for (edx = 0; edx < BCMDRD_CONFIG_MAX_UNITS; edx++) {

        /* Set up ID structure for next simulated device */
        sal_memset(&id, 0, sizeof(id));
        if (bcma_sys_sim_dev_get(edx,
                                 &id.vendor_id, &id.device_id,
                                 &id.revision, NULL, &name) < 0) {
            /* No more devices */
            break;
        }

        /* Next unit number */
        unit = ndevs;

        if (bm_sdk_msg_init(unit) < 0) {
            return 0;
        }

        /* Create device */
        if (bcmdrd_dev_create(unit, &id) < 0) {
            LOG_WARN(BSL_LS_SYS_PCI,
                     (BSL_META_U(unit,
                                 "Failed to create device "
                                 "(%04x:%04x:%02x)\n"),
                      id.vendor_id, id.device_id, id.revision));
            continue;
        }

        /* Allow driver to account for simulation environment */
        bcmdrd_feature_enable(unit, BCMDRD_FT_PASSIVE_SIM);

        /* Initialize I/O structure */
        sal_memset(&io, 0, sizeof(io));

        /* Callback context is unit number */
        sys_devs[ndevs].unit = unit;
        io.devh = &sys_devs[ndevs].unit;

        io.read = io_read;
        io.write = io_write;

        if (bcmdrd_dev_hal_io_init(unit, &io) < 0) {
            LOG_ERROR(BSL_LS_SYS_PCI,
                      (BSL_META_U(unit,
                                  "Failed to initialize I/O\n")));
            continue;
        }

        if (++ndevs >= COUNTOF(sys_devs)) {
            break;
        }
    }

    return ndevs;
}

int
bcma_sys_probe_config_set(const char *config)
{
    return 0;
}

const char *
bcma_sys_probe_config_get(const char *prev)
{
    return NULL;
}

int
bcma_sys_sim_dev_get(unsigned int dev_idx,
                     uint16_t *vendor_id, uint16_t *device_id,
                     uint16_t *revision, uint16_t *model, const char **name)
{
    if (dev_idx == 0) {
        /* Return requested device information */
        if (vendor_id) {
            *vendor_id = BCM0213_VENDOR_ID;
        }
        if (device_id) {
            *device_id = BCM0213_DEVICE_ID;
        }
        if (revision) {
            *revision = BCM0213_REV_A0;
        }
        if (model) {
            *model = 0;
        }
        if (name) {
            *name = "BCM0213";
        }
        return 0;
    }

    /* Unsupported device index */
    return -1;
}

int
bcma_sys_probe_override(int unit, const char *dev_name)
{
    /* Not supported */
    return -1;
}

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      atptrans_socket.c
 *
 * Purpose:   ATP over Sockets
 *
 *            This module provides support for the ATP transport
 *            to use sockets as its underlying communication mechanism.
 *
 *            Currently, this only supported for Linux User mode
 *            chassis based systems.
 */

#include <shared/bsl.h>

#include <sal/types.h>

#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>

#include <shared/util.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/init.h>
#include <bcm/rx.h>

#include <appl/cputrans/atptrans_socket.h>
#include <appl/cputrans/atp.h>


#ifdef INCLUDE_ATPTRANS_SOCKET

/*
 * This implementation is built on top of a socket API interface.
 * This means that there must be OS dependent include files to use the
 * socket interface.  To make this truly portable, this interface should
 * be abstracted in some way.  Until this is done, the ATP over Sockets
 * implementation works for Linux user mode, but not Linux kernel mode.
 *
 * The socket APIs used are:
 *    socket, connect, accept, bind, listen, close, shutdown,
 *    read, write, setsockopt
 */

#undef ATPTRANS_SOCKET_SUPPORTED

/*
 * Linux User mode or unix 
 */
#if (defined(LINUX) || defined(unix)) && !defined(__KERNEL__)
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#define ATPTRANS_SOCKET_SUPPORTED 1
typedef socklen_t atptrans_socklen_t;
#endif /* LINUX && !__KERNEL__ */

/*
 * Linux Kernel mode (unsupported)
 */
#if defined(LINUX) && defined(__KERNEL__)
#undef ATPTRANS_SOCKET_SUPPORTED
#endif /* (LINUX || unix) && __KERNEL__ */

#if defined(ATPTRANS_SOCKET_SUPPORTED)

#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif

/* -------------------------------------------------------------------
 * Pack - Unpack 
 *
 * Macros to handle byte order
 * 
 * Note:  These macros are already defined in other
 *        header files.  Althought not all of the implementations 
 *        are the same, they accomplish the same task.
 */
#define PACK_LONG(buf, val)      _SHR_PACK_LONG(buf, val)
#define UNPACK_LONG(buf, val)    _SHR_UNPACK_LONG(buf, val)


/* -------------------------------------------------------------------
 * Packet Header
 *
 * Packet header expected for each ATP-Socket transmit/receive
 */
#define PKT_LEN_OFS             0
#define PKT_SRC_KEY_OFS         (PKT_LEN_OFS + sizeof(uint32))
#define PKT_CLIENT_ID_OFS       (PKT_SRC_KEY_OFS + CPUDB_KEY_BYTES)
#define PKT_HDR_BYTES           (PKT_CLIENT_ID_OFS + sizeof(uint32))

typedef struct atp_sock_ptk_hdr_s {
    uint32        len;       /* Number of bytes in payload (packet data) */
    cpudb_key_t   src_key;
    uint32        client_id;
} atp_sock_pkt_hdr_t;


/* -------------------------------------------------------------------
 * Semaphores - Locks
 *
 * Used for coordinating access to global resources
 */
static sal_mutex_t atp_sock_mlock = NULL;

#define ATP_SOCK_LOCK         sal_mutex_take(atp_sock_mlock, sal_mutex_FOREVER)
#define ATP_SOCK_UNLOCK       sal_mutex_give(atp_sock_mlock)
#define ATP_SOCK_INIT_DONE    (atp_sock_mlock != NULL)


/* -------------------------------------------------------------------
 * Threads
 *
 * The ATP over Sockets module may spawn the following threads,
 *   - A Server thread to service connection requests
 *   - A Connection-RX thread for each connection to service incoming packets
 */
#define ATP_SOCK_THREAD_NAME_LEN         15
#define ATP_SOCK_THREAD_PRI_DEFAULT     100
#define ATP_SOCK_STOP_RETRIES_MAX        50
#define ATP_SOCK_RETRY_WAIT_US       100000  /* 100 ms */


/* -------------------------------------------------------------------
 * Server
 *
 * The Server will have a thread that will listen for connection 
 * requests.  
 *
 * Note:  Current implementation supports only ONE server thread
 *        running on a system at a given time.
 */
#define ATP_SOCK_BACKLOG                 5
#define ATP_SOCK_SERVER_LPORT_DEFAULT 20567 /* Port to listen for connections */
#define ATP_SOCK_SERVER_NAME_DEFAULT  "bcmATPSockS"

static int  atp_sock_server_lport     = -1;
static int  atp_sock_server_lsock     = -1;
static char atp_sock_server_thread_name[ATP_SOCK_THREAD_NAME_LEN] =
                                             ATP_SOCK_SERVER_NAME_DEFAULT;
static volatile sal_thread_t atp_sock_server_thread_id = SAL_THREAD_ERROR;
static volatile int          atp_sock_server_exit = FALSE;

#define ATP_SOCK_SERVER_RUNNING  (atp_sock_server_thread_id != SAL_THREAD_ERROR)


/* Configurable parameters */
static int atp_sock_cfg_thread_pri   = ATP_SOCK_THREAD_PRI_DEFAULT;
/* Local listening port, the one we are listening on in server_conn_thread() */
static int atp_sock_cfg_local_port  = ATP_SOCK_SERVER_LPORT_DEFAULT;
/* Remote connection port */
static int atp_sock_cfg_remote_port = ATP_SOCK_SERVER_LPORT_DEFAULT;

/* Unique local and remote port. */
static int atp_sock_cfg_unique_lr_ports = 0;

/* -------------------------------------------------------------------
 * Connection
 *
 * Contains information for a given connection.  It manages the transmitting
 * and receiving of packets.  An RX-Thread is associated for each connection.
 *
 *   sock_fd         - Socket for sending/receiving data
 *   rx_thread_name  - Name of the connection receive thread
 *   rx_thread_id    - ID of the connection receive thread
 *   flags           - Indicates more information for connection
 */

#define ATP_SOCK_CONN_NAME  "bcmATPSock" /* bcmATPSockXXX, where XXX = socket */

/* Flags for Connection entry */
#define ATP_SOCK_CONN_CLEAR    0x0
#define ATP_SOCK_CONN_VALID    0x1 /* If set, a connection exists */
#define ATP_SOCK_CONN_ABORT    0x2 /* If set, connection is to be destroyed */

typedef struct atp_sock_conn_entry_s {
    int                 sock_fd;
    char                rx_thread_name[ATP_SOCK_THREAD_NAME_LEN];
    sal_thread_t        rx_thread_id;
    volatile uint32     flags;
} atp_sock_conn_entry_t;



/* -------------------------------------------------------------------
 * ATP over Socket DB
 *
 * The ATP-Socket CPU database is a link list of CPU DB entries.  Each entry
 * contains information on a given remote CPU such as CPU-key to 
 * IP mapping, connection, etc.
 *
 *   cpu_key        - Destination CPU Key
 *   ip             - Destination IP address, IPv4 format
 *   ip6            - Destination IP address, IPv6 format
 *   flags          - More information for CPU entry
 *   conn           - Connection entry for given destination CPU
 *   next           - Pointer to next entry
 */

/* Flags for the ATP-Socket database entry */
#define ATP_SOCK_DB_CLEAR        0x0
#define ATP_SOCK_DB_CPU_VALID    0x1  /* If set, CPU field is known */
#define ATP_SOCK_DB_IP6          0x2  /* If set, IPv6 is used; else, IPv4 */
#define ATP_SOCK_DB_INSTALL      0x4
#define ATP_SOCK_DB_INSTALLED    0x8

typedef struct atp_sock_db_entry_s atp_sock_db_entry_t;
struct atp_sock_db_entry_s {
    cpudb_key_t               cpu_key;
    bcm_ip_t                  ip;          /* IPv4 */
    bcm_ip6_t                 ip6;         /* IPv6 */
    atp_sock_conn_entry_t     conn;
    volatile uint32           flags;
    atp_sock_db_entry_t       *next;
};

static atp_sock_db_entry_t *atp_sock_db_list = NULL;

#define ATP_SOCK_DB_FOREACH_ENTRY(_list, _entry) \
    for (_entry = _list; _entry != NULL; _entry = _entry->next) 

static const cpudb_key_t atp_sock_cpu_key_null = {{0x0, 0x0, 0x0,
                                                   0x0, 0x0, 0x0}};
static cpudb_key_t       atp_sock_local_cpu_key;

/* Host mode ATP transport support */
static bcm_trans_ptr_t host_atp_transport;
static bcm_trans_ptr_t *original_atp_transport;



/* -------------------------------------------------------------------
 * Forward function declarations
 */

STATIC int atptrans_socket_tx_atp(cpudb_key_t dest_key,
                                  int client_id,
                                  uint8 *pkt_buf,
                                  int len,
                                  uint32 ct_flags,
                                  atp_tx_cb_f callback,
                                  void *cookie);


/* -------------------------------------------------------------------
 * Function - Implementation
 */

/*
 * Function: 
 *     ip_to_str
 * Purpose:
 *     Converts an IPv4 address from bcm_ip_t to string format.
 * Parameters:
 *     str       - (OUT) IP address in string format
 *     ip        - IP address in bcm_ip_t format
 * Returns:
 *     BCM_E_NONE    Success
 *     BCM_E_XXX     Failure
 */

STATIC int
ip_to_str(char *str, bcm_ip_t ip)
{
    sal_sprintf(str, "%d.%d.%d.%d",
                (ip >> 24) & 0xff, (ip >> 16) & 0xff, 
                (ip >> 8) & 0xff, ip & 0xff);
    
    return BCM_E_NONE;
}


/*
 * Function: 
 *     read_n
 * Purpose:
 *     Read 'n' bytes from the given file descriptor.
 * Parameters:
 *     fd        - File descriptor to read from
 *     buffer    - (OUT) Buffer where data read is returned
 *     len       - Number of bytes to read
 * Returns:
 *     > 0   Number of actual bytes read
 *       0   EOF, received FIN
 *     (-1)  On read error
 * Notes:
 *     1. The routine does not return until the specified number of bytes
 *        has been read, or, an EOF or read error has ocurred.
 *     2. If actual bytes read is less than expected, an EOF
 *        was also received.
 */

STATIC int
read_n(int fd, void *buffer, int len)
{
    int        n_left;    /* Number of bytes left to read */
    int        n_read;    /* Number of bytes read */
    char       *ptr;
    
    ptr = buffer;
    n_left = len;
    
    while (n_left > 0) {
        if ((n_read = read(fd, ptr, n_left)) < 0) {
            return (-1);
        } else if (n_read == 0) {    /* EOF */
            break;
        }
        
        n_left -= n_read;
        ptr    += n_read;
    }
    
    return (len - n_left);
}


/*
 * Function: 
 *     write_n
 * Purpose:
 *     Write 'n' bytes to the given file descriptor.
 * Parameters:
 *     fd        - File descriptor to write to
 *     buffer    - Buffer containing data
 *     len       - Number of bytes in buffer to be written
 * Returns:
 *     >= 0  Number of actual bytes written
 *     (-1)  On write error
 * Notes:
 *     The routine does not return until the specified number of bytes
 *     has been written or a write error has ocurred.
 */

STATIC int
write_n(int fd, void *buffer, int len)
{
    int         n_left;       /* Number of bytes left to write */
    int         n_written;    /* Number of bytes written */
    char        *ptr;
    
    ptr = (char *)buffer;
    n_left = len;
    
    while (n_left > 0) {
        if ((n_written = write(fd, ptr, n_left)) <= 0) {
            return (-1);
        }
        
        n_left -= n_written;
        ptr    += n_written;
    }
    
    return len;
}


/*
 * Function: 
 *     recv_pkt_hdr
 * Purpose:
 *     Read a ATP-Socket packet header from the given file descriptor.
 * Parameters:
 *     fd            - File descriptor to read packet header from
 *     hdr           - (OUT) Packet header information returned
 * Returns:
 *     BCM_E_NONE  Success, received packet header
 *     BCM_E_XXX   Failure, on read error
 */

STATIC int
recv_pkt_hdr(int fd, atp_sock_pkt_hdr_t *hdr)
{
    int          rv = BCM_E_FAIL;
    uint8        hdr_buffer[PKT_HDR_BYTES];
    int          n_recv;
    
    
    /* Read packet header buffer */
    n_recv = read_n(fd, (void *)hdr_buffer, sizeof(hdr_buffer));
    
    if (n_recv == sizeof(hdr_buffer)) {
        /* Receive complete header */
        /* Unpack Packet Header */
        UNPACK_LONG(&hdr_buffer[PKT_LEN_OFS], hdr->len);
        CPUDB_KEY_UNPACK(&hdr_buffer[PKT_SRC_KEY_OFS], hdr->src_key);
        UNPACK_LONG(&hdr_buffer[PKT_CLIENT_ID_OFS], hdr->client_id); 
        
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket: Receive packet header on socket %d, "
                            "data length %d, src_key %x:%x, client id %d\n"),
                   fd, hdr->len, hdr->src_key.key[4], hdr->src_key.key[5],
                   hdr->client_id));
        
        rv = BCM_E_NONE;
    } else if (n_recv == 0) {    /* EOF */
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP-Socket: Connection socket %d received FIN\n"),
                     fd));
    } else if (n_recv < 0) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: System error\n")));
    } else {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot read packet header\n")));
    }
    
    return rv;
}


/*
 * Function: 
 *     recv_pkt_data
 * Purpose:
 *     Read packet data from the given file descriptor.
 * Parameters:
 *     fd            - File descriptor to read data from
 *     buffer        - (OUT) Buffer where data is returned; 
 *                     buffer size must be at least 'n_expected' bytes
 *     n_expected    - Expected packet data length
 * Returns:
 *     BCM_E_NONE  Success, received expected size packet data
 *     BCM_E_XXX   Failure,
 *                 . Buffer is NULL
 *                 . Packet data returned is smaller than expected
 *                 . Read error
 */

STATIC int 
recv_pkt_data(int fd, uint8 *buffer, int n_expected)
{
    int          rv = BCM_E_FAIL;
    int          n_recv;
    
    /* Read packet data */
    n_recv = read_n(fd, buffer, n_expected);
    
    if (n_expected == n_recv) {
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket: Receive packet data on socket %d, "
                            "length %d\n"), fd, n_expected));
        rv = BCM_E_NONE;
    } else if (n_recv == 0) {    /* EOF */
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP-Socket: Connection socket %d received FIN\n"),
                     fd));
    } else if (n_recv < 0) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: System error\n")));
    } else {
        /* Length of data read is not what was expected */
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Expected packet data length %d, "
                            "received %d bytes\n"), n_expected, n_recv));
    }
    
    return rv;
}

 
/*
 * Function: 
 *     send_pkt_data
 * Purpose:
 *     Write data to given file descriptor.
 * Parameters:
 *     client_id    - Client id where to send data to
 *     fd           - File descriptor to write data to
 *     buffer       - Buffer containing packet data to be sent
 *     data_len     - Number of data bytes in 'buffer' to send (payload)
 * Returns:
 *     BCM_E_NONE  Success, specified number of bytes were written
 *     BCM_E_XXX   Failure,
 *                 . Buffer is NULL
 *                 . Write error
 */
STATIC int
send_pkt_data(int client_id, int fd, uint8 *buffer, int data_len)
{
    uint8          hdr_buffer[PKT_HDR_BYTES];
    int            n_wrote;
    int            hdr_size;

    /* Pack packet header */
    PACK_LONG(&hdr_buffer[PKT_LEN_OFS], data_len);
    CPUDB_KEY_PACK(&hdr_buffer[PKT_SRC_KEY_OFS], atp_sock_local_cpu_key);
    PACK_LONG(&hdr_buffer[PKT_CLIENT_ID_OFS], client_id);          

    /* Send packet header */
    hdr_size = sizeof(hdr_buffer);
    n_wrote = write_n(fd, (void *)hdr_buffer, hdr_size);
    if (n_wrote < hdr_size) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot send packet header\n")));
        return BCM_E_FAIL;
    }

    /* Send packet data */
    n_wrote = write_n(fd, buffer, data_len);
    if (n_wrote < data_len) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot send packet data, "
                   "payload length %d, sent %d\n"), data_len, n_wrote));
        return BCM_E_FAIL;
    }

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP-Socket: Send packet on socket %d, "
                        "payload length %d, src key " CPUDB_KEY_FMT
                        ", client id %d\n"), fd, data_len,
               CPUDB_KEY_DISP(atp_sock_local_cpu_key), client_id));

    return BCM_E_NONE;
}


/*
 * Function: 
 *     rx_conn_thread
 * Purpose:
 *     Thread that processes incoming packets for a given connection.
 *
 *     It expects an ATP-Socket packet header and the correspoding packet data.
 *     Once the payload is received, it gives the packet data to the upper
 *     layer transport protocol ATP to handle it.  It then proceeds to wait
 *     for another ATP-Socket packet to arrive.
 *
 *     Thread terminates on,
 *     . Read error
 *     . Closed connection socket
 *     . ABORT flag set
 *
 * Parameters:
 *     cookie      - Pointer to DB entry for RX thread
 * Returns:
 *     None
 */

STATIC void
rx_conn_thread(void *cookie)
{
    int                    rv = BCM_E_FAIL;
    atp_sock_pkt_hdr_t     hdr;
    uint8                  *buffer;
    int                    data_len;
    bcm_rx_t               pkt_rtn_code;
    int                    rx_sock;
    atp_sock_db_entry_t    *db_ptr;
    atp_sock_conn_entry_t  *conn_ptr;
    bcm_ip_t               dest_ip = 0;
    char                   dest_ip_str[SAL_IPADDR_STR_LEN];
    int                    lock_set = FALSE;
    char                   thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t           thread;

    db_ptr = (atp_sock_db_entry_t *)cookie;
    conn_ptr = &db_ptr->conn;

    rx_sock = conn_ptr->sock_fd;

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Start RX Connection Thread, socket %d\n"),
                 rx_sock));
    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));
    /* Main loop to service incoming data for the connection */
    while (!(conn_ptr->flags & ATP_SOCK_CONN_ABORT)) {

        /* Get packet header */
        rv = recv_pkt_hdr(rx_sock, &hdr);
        if (BCM_FAILURE(rv) || (conn_ptr->flags & ATP_SOCK_CONN_ABORT)) { 
            break;
        }
    
        /* Get packet data */
        data_len = hdr.len;
        buffer = atp_rx_data_alloc(data_len);
        if (buffer == NULL) {
            LOG_ERROR(BSL_LS_TKS_ATP,
                      (BSL_META("AbnormalThreadExit:%s ERROR: Cannot allocate receive "
                       "buffer\n"), thread_name));
            rv = BCM_E_MEMORY;
            break;
        }

        if (hdr.len != 0) {
            rv = recv_pkt_data(rx_sock, buffer, data_len);
            if (BCM_FAILURE(rv) || (conn_ptr->flags & ATP_SOCK_CONN_ABORT)) {
                atp_rx_free(buffer, NULL);
                break;
            }
        }

        /* 
         * Pass packet data to ATP receive routine to continue with
         * normal processing of packet
         */
        pkt_rtn_code = atp_rx_inject(hdr.src_key, hdr.client_id, 
                                     buffer, data_len);

        switch(pkt_rtn_code) {
        case BCM_RX_HANDLED_OWNED:
            /* 
             * Packet was handled and stolen, do not do anything.
             * New owner is responsible of deallocating the buffer.
             */
            rv = BCM_E_NONE;
            break;

        case BCM_RX_INVALID:
        case BCM_RX_NOT_HANDLED:
        case BCM_RX_HANDLED:
        default:
            /* Need to deallocate buffer */
            atp_rx_free(buffer, NULL);
            break;
        }      
    }

    close(rx_sock);


    /* Cleanup connection entry */

    /*
     * Get lock only if ABORT flag is not set
     *
     * Note:
     *   The routine that set the ABORT flag holds the lock
     *   and waits for this thread to exit.  If this
     *   thread tries to get the lock, the routine waiting
     *   for this thread to exit will timeout with an error.
     */
    if (!(conn_ptr->flags & ATP_SOCK_CONN_ABORT)) {
        ATP_SOCK_LOCK;
        lock_set = TRUE;
    }
    
    dest_ip = db_ptr->ip;

    /* Uninstall the socket mechanism from ATP transport and update flags */
    if (db_ptr->flags & ATP_SOCK_DB_INSTALLED) {
        /* Uninstall sockets from ATP */
        rv = atp_tx_override_set(db_ptr->cpu_key, NULL);
        db_ptr->flags &= ~ATP_SOCK_DB_INSTALLED;
    }

    db_ptr->conn.sock_fd = -1;
    db_ptr->conn.rx_thread_name[0] = '\0';
    db_ptr->conn.rx_thread_id = SAL_THREAD_ERROR;
    db_ptr->conn.flags = ATP_SOCK_CONN_CLEAR;

    if (lock_set) {
        ATP_SOCK_UNLOCK;
    }

    ip_to_str(dest_ip_str, dest_ip);
    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP-Socket: Destroy connection to '%s', socket %d\n"),
               dest_ip_str, rx_sock));
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("AbnormalThreadExit:%s\n"),
                   thread_name)); 
    }   
    sal_thread_exit(rv);
    
    return;
}


/*
 * Function:
 *     conn_destroy
 * Purpose:
 *     It destroys a connection as follows:
 *     - Sets the ABORT flag to indicate the RX thread to exit
 *     - Closes the connection socket
 *     - Waits for the RX thread to exit for a period of time
 * Parameters:
 *     db_ptr      - Pointer to DB entry to destroy connection
 * Returns:
 *     BCM_E_NONE  Success
 *     BCM_E_XXX   Failure,
 *                 . RX connection thread did not exit in expected wait time
 * Note:
 *     Assumes LOCK is held by caller
 */

STATIC int
conn_destroy(atp_sock_db_entry_t *db_ptr)
{
    int                 i;

    if (!(db_ptr->conn.flags & ATP_SOCK_CONN_VALID)) {
        return BCM_E_NONE;
    }

    db_ptr->conn.flags |= ATP_SOCK_CONN_ABORT;
    shutdown(db_ptr->conn.sock_fd, SHUT_RDWR);

    /* Wait until connection is clear */
    for (i = 0; i < ATP_SOCK_STOP_RETRIES_MAX; i++) {
        if (db_ptr->conn.rx_thread_id == SAL_THREAD_ERROR) {
            break;
        }
        sal_usleep(ATP_SOCK_RETRY_WAIT_US);
    }
    
    if (i >= ATP_SOCK_STOP_RETRIES_MAX) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot destroy connection on "
                   "socket %d\n"), db_ptr->conn.sock_fd));
        return BCM_E_FAIL;
    }
    
    return BCM_E_NONE;
}


/*
 * Function:
 *     conn_create
 * Purpose:
 *     It creates a connection as follows:
 *     - Checks that DB entry does not have a valid connection,
 *       if so, return failure
 *     - Creates an RX connection thread
 *     - Sets ATP override to use sockets
 *     - Updates the connection information on corresponding DB entry
 * Parameters:
 *     db_ptr      - Pointer to DB entry to create connection for
 *     sock_fd     - Connection socket
 * Returns:
 *     BCM_E_NONE  Success
 *     BCM_E_XXX   Failure,
 *                 . DB entry contains a valid connection
 *                 . Cannot create thread
 * Note:
 *     Assumes LOCK is held by caller
 */

STATIC int
conn_create(atp_sock_db_entry_t *db_ptr, int sock_fd)
{
    int                    rv;
    atp_sock_conn_entry_t  *conn_ptr;
    char                   thread_name[ATP_SOCK_THREAD_NAME_LEN];
    sal_thread_t           thread_id = SAL_THREAD_ERROR;
    const int              sock_opt_on = 1;
    char                   dest_ip_str[SAL_IPADDR_STR_LEN];


    /*
     * Set socket option to disable Nagle algorithm.
     * The purpose is to increase performance.
     *
     * Note: The Nagle algorithm prevents a connection from 
     * having multiple outstanding packets at any time.
     * It waits for the 'ack' to come back before it sends
     * any packet.
     */
    setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, 
               (char *)&sock_opt_on, sizeof(sock_opt_on));

    /* Create RX connection thread */
    sal_snprintf(thread_name, ATP_SOCK_THREAD_NAME_LEN,
                 "%s%d", ATP_SOCK_CONN_NAME, sock_fd);
    
    conn_ptr = &db_ptr->conn;
    if (conn_ptr->flags & ATP_SOCK_CONN_VALID) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR:  Connection already exists\n")));
        return BCM_E_EXISTS;
    }

    /* Set information */
    conn_ptr->flags   = ATP_SOCK_CONN_VALID;
    conn_ptr->sock_fd = sock_fd;

     /* Create thread to handle specific connection request */
    sal_strcpy(conn_ptr->rx_thread_name, thread_name);
    thread_id = sal_thread_create(conn_ptr->rx_thread_name,
                                  SAL_THREAD_STKSZ,
                                  atp_sock_cfg_thread_pri,
                                  rx_conn_thread, 
                                  (void *) db_ptr);

    if (thread_id == SAL_THREAD_ERROR) {
        conn_ptr->flags = ATP_SOCK_CONN_CLEAR;
        conn_ptr->sock_fd = -1;
        conn_ptr->rx_thread_name[0]='\0';
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot create connection thread\n")));
        return BCM_E_FAIL;
    }

    conn_ptr->rx_thread_id = thread_id;

    /* Install on ATP if needed */
    if (db_ptr->flags & ATP_SOCK_DB_INSTALL) {
         /* Set ATP to use sockets */
         rv = atp_tx_override_set(db_ptr->cpu_key, atptrans_socket_tx_atp);
         if (BCM_SUCCESS(rv)) {
             db_ptr->flags |= ATP_SOCK_DB_INSTALLED;
         }
    }

    ip_to_str(dest_ip_str, db_ptr->ip);
    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP-Socket: Created connection to '%s', socket %d, "
                        "thread '%s'\n"),
               dest_ip_str, conn_ptr->sock_fd,
               conn_ptr->rx_thread_name));

    return BCM_E_NONE;
}



/*
 * Function:
 *     conn_request
 * Purpose:
 *     It request a connection to the destination IP specified in the
 *     DB entry.  If successful, a connection is created and the DB
 *     entry is updated.
 * Parameters:
 *     db_ptr      - Pointer to DB entry of destination CPU to
 *                   connect to
 * Returns:
 *     >= 0    Socket for requested connection, if successful
 *     <  0    Failure to establish connection
 * Note:
 *     Assumes LOCK is held by caller
 */

STATIC int
conn_request(atp_sock_db_entry_t *db_ptr)
{
    struct sockaddr_in  server_addr;
    atptrans_socklen_t  addr_len;
    int                 conn_sock = -1;
    char                dest_ip_str[SAL_IPADDR_STR_LEN];
    int                 rv = -1;

    ip_to_str(dest_ip_str, db_ptr->ip);

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Connection request to '%s'\n"),
                 dest_ip_str));

    /* Open socket */
    if ((conn_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot open socket (err=%d)\n"),
                   conn_sock));
        return conn_sock;
    }

    /* Connect to specified destination */
    addr_len = (atptrans_socklen_t) sizeof(server_addr);

    sal_memset((void *)&server_addr, 0, (int)addr_len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(atp_sock_cfg_remote_port);
    server_addr.sin_addr.s_addr = htonl(db_ptr->ip);

    if ((rv = connect(conn_sock, 
                      (struct sockaddr *) &server_addr, addr_len)) < 0) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: \
                            Cannot connect to '%s'; port %d (err=%d)\n"),
                   dest_ip_str, atp_sock_cfg_remote_port, rv));
        close(conn_sock);
        return rv;
    }

    rv = conn_create(db_ptr, conn_sock);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot create TX connection to '%s', "
                   "closing socket %d\n"), dest_ip_str, conn_sock));
        close(conn_sock);
        return rv;
    }


    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Connection established with '%s' on socket "
                 "%d\n"), dest_ip_str, conn_sock));

    return conn_sock;
}


/*
 * Function:
 *     db_cpu_key_lookup
 * Purpose:
 *     It searches for a matching CPU key in the database list.
 * Parameters:
 *     list          - Database list
 *     cpu_key       - CPU key to lookup
 * Returns:
 *     non-null      Pointer to matching entry
 *     null          CPU key not found
 * Note:
 *     Assumes LOCK is held by caller
 */

STATIC atp_sock_db_entry_t *
db_cpu_key_lookup(atp_sock_db_entry_t *list, cpudb_key_t cpu_key)
{
    atp_sock_db_entry_t    *entry;

    ATP_SOCK_DB_FOREACH_ENTRY(list, entry) {
        if (entry->flags & ATP_SOCK_DB_CPU_VALID) {
            if (CPUDB_KEY_EQUAL(entry->cpu_key, cpu_key)) {
                return entry;
            }
        }
    }

    return entry;
}

/*
 * Function:
 *     db_ip_lookup
 * Purpose:
 *     It searches for a matching IP address in the database list.
 * Parameters:
 *     list          - Database list
 *     ip            - IP address to lookup
 * Returns:
 *     non-null      Pointer to matching entry
 *     null          IP address not found
 * Note:
 *     Assumes LOCK is held by caller
 */

STATIC atp_sock_db_entry_t *
db_ip_lookup(atp_sock_db_entry_t *list, bcm_ip_t ip)
{
    atp_sock_db_entry_t    *entry;

    ATP_SOCK_DB_FOREACH_ENTRY(list, entry) {
        if (entry->ip == ip) {
            return entry;
        }
    }

    return entry;
}


/*
 * Function:
 *     sock_get
 * Purpose:
 *     It gets the socket for the given CPU key.
 * Parameters:
 *     list          - Database list to search
 *     dest_key      - CPU key
 * Returns:
 *     >=0  Socket number
 *     -1   Failure, no valid connection was found for given CPU
 * Note:
 *     Assumes LOCK is held by caller
 */

STATIC int
sock_get(atp_sock_db_entry_t *list, cpudb_key_t dest_key)
{
    int                  sock_fd = -1;
    atp_sock_db_entry_t  *db_ptr;

    if ((db_ptr = db_cpu_key_lookup(list, dest_key)) != NULL) {
        if ((db_ptr->conn.flags & ATP_SOCK_CONN_VALID) &&
            !(db_ptr->conn.flags & ATP_SOCK_CONN_ABORT)) {
            sock_fd = db_ptr->conn.sock_fd;
        }
    }

    return sock_fd;
}

/*
 * Function:
 *     db_insert_front
 * Purpose:
 *     It inserts given DB entry to the front of the database list.
 * Parameters:
 *     list          - (IN/OUT) Database list
 *     entry         - Pointer to DB entry to insert to list
 * Returns:
 *     Pointer to database list
 * Note:
 *     Assumes LOCK is held by caller
 */

STATIC atp_sock_db_entry_t *
db_insert_front(atp_sock_db_entry_t **list, atp_sock_db_entry_t *entry)
{
    /* Insert entry */
    entry->next = *list;
    *list = entry;
    
    return *list;
}


/*
 * Function:
 *     db_entry_create
 * Purpose:
 *     It performs one of the following:
 *     - If entry already exists in database list, it checks that
 *       the CPU and/or IP address values are consistent with
 *       input values.
 *     - Else, it creates a new DB entry and adds it to the given ATP-Socket 
 *       database list.
 * Parameters:
 *     list          - (IN/OUT) Database list
 *     cpu_key       - CPU key
 *     ip            - IP address, IPv4 format
 *     flags         - Should be set as follows,
 *                         ATP_SOCK_DB_CPU_VALID if CPU key is provided
 *                         ATP_SOCK_DB_INSTALL   if sockets is to be installed
 * Returns:
 *     BCM_E_NONE    Success
 *     BCM_E_XXX     Failure,
 *                   . Entry already exists with unconsistent values
 *                   . Cannot allocate memory
 * Note:
 *     Assumes LOCK is held by caller
 */

STATIC atp_sock_db_entry_t *
db_entry_create(atp_sock_db_entry_t **list,
                cpudb_key_t const cpu_key, bcm_ip_t ip, uint32 flags)
{
    int                  rv;
    atp_sock_db_entry_t  *cpu_key_match;
    atp_sock_db_entry_t  *ip_match;
    atp_sock_db_entry_t  *entry;
    
    ip_match = db_ip_lookup(*list, ip);

    if (flags & ATP_SOCK_DB_CPU_VALID) {
        cpu_key_match = db_cpu_key_lookup(*list, cpu_key);
        if ((cpu_key_match != NULL) && (cpu_key_match != ip_match)) {
            return NULL;
        }
    }

    if ((atp_sock_cfg_unique_lr_ports == 1) &&
        (ip_match != NULL) &&
        (CPUDB_KEY_COMPARE(ip_match->cpu_key, cpu_key))) {
        /* If local and remote ports are not same,
         * Ignore the entry allocated for other port.
         */
        entry = NULL;
    } else {
        entry = ip_match;
    }

    if (entry != NULL) {
        if (flags & ATP_SOCK_DB_CPU_VALID) {

            /* If CPU is known, check that keys are same */
            if (entry->flags & ATP_SOCK_DB_CPU_VALID) {
                if (CPUDB_KEY_COMPARE(entry->cpu_key, cpu_key)) {
                    return NULL;
                }
            } else {
                /* Copy CPU key if needed */
                CPUDB_KEY_COPY(entry->cpu_key, cpu_key);
            }

            /* Install on ATP if needed */
            if (((flags & ATP_SOCK_DB_INSTALL) &&
                 !(entry->flags & ATP_SOCK_DB_INSTALLED)) &&
                (entry->conn.flags & ATP_SOCK_CONN_VALID)) {
                /* Set ATP to use Sockets */
                rv = atp_tx_override_set(entry->cpu_key,
                                         atptrans_socket_tx_atp);
                if (BCM_SUCCESS(rv)) {
                    entry->flags |= ATP_SOCK_DB_INSTALLED;
                }
            }
        }
        entry->flags |= flags;
        return entry;
    }


    /* Allocate new entry */
    entry = sal_alloc(sizeof(atp_sock_db_entry_t), "atp_sock_db_entry");
    if (entry == NULL) {
        return NULL;
    }
    
    /* Set information */
    CPUDB_KEY_COPY(entry->cpu_key, cpu_key);
    entry->ip = ip;
    entry->flags = flags;
    entry->conn.sock_fd = -1;
    entry->conn.rx_thread_name[0] = '\0';
    entry->conn.rx_thread_id = SAL_THREAD_ERROR;
    entry->conn.flags = ATP_SOCK_CONN_CLEAR;
    entry->next = NULL;
    
    db_insert_front(list, entry);
    
    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP-Socket: Added CPU key " CPUDB_KEY_FMT "\n"),
               CPUDB_KEY_DISP(entry->cpu_key)));
    
    return entry;
}
 

/*
 * Function:
 *     db_entry_remove
 * Purpose:
 *     Removes the given DB entry from the specified ATP-Socket database list.
 *     If a valid connection exists for given entry, this is destroyed.
 * Parameters:
 *     list          - (IN/OUT) Database list
 *     db_ptr        - Pointer to DB entry to remove
 * Returns:
 *     BCM_E_NONE    Success
 *     BCM_E_XXX     Failure
 * Note:
 *     Assumes LOCK is held by caller
 */

STATIC int
db_entry_remove(atp_sock_db_entry_t **list, atp_sock_db_entry_t *db_ptr)
{
    int                    rv;
    atp_sock_db_entry_t    *prev;
    atp_sock_db_entry_t    *cur;

    rv = conn_destroy(db_ptr);
    if (BCM_FAILURE(rv)) {
        return BCM_E_FAIL;
    }

    prev = NULL;

    ATP_SOCK_DB_FOREACH_ENTRY(*list, cur) {
        if (cur == db_ptr) {
            if (prev == NULL) {
                *list = cur->next;
            } else {
                prev->next = cur->next;
            }
            sal_free(cur);
            return BCM_E_NONE;
        }
        prev = cur;
    }

    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *     accept_connection
 * Purpose:
 *     It waits for a connection request and updates the ATP-Socket database
 *     accordingly.
 *
 *     On a incoming connection request, it searches the database
 *     list for the client IP address.  If client is not found, a 
 *     new DB entry is created and added to the database list.  A
 *     connection is created for the corresponding DB entry.
 *
 *     If client already has a valid connection, this routine
 *     will return a failure.
 *
 * Parameters:
 *     listen_sock      - Port to listen to for connection requests
 * Returns:
 *     BCM_E_NONE  Success
 *     BCM_E_XXX   Failure,
 *                 . Error on listening socket
 *                 . Could not create DB entry
 *                 . Could not create connection
 *                 . A valid connection already exists
 */

STATIC int
accept_connection(int listen_sock)
{
    int                  rv;
    atptrans_socklen_t   addr_len;
    struct sockaddr_in   client_addr;
    char                 dest_ip_str[SAL_IPADDR_STR_LEN];
    bcm_ip_t             dest_ip;
    atp_sock_db_entry_t  *db_ptr;
    int                  conn_sock;     /* Connection socket */
    
    
    addr_len = (atptrans_socklen_t) sizeof(client_addr);
    
    rv = accept(listen_sock, 
                (struct sockaddr *) &client_addr, &addr_len);
    
    if (rv < 0) {
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP-Socket:  Accept returned error on socket %d "
                     "(rv=%d)\n"), listen_sock, rv));
        return BCM_E_FAIL;
    }
    
    /* Got a connection request, process it */
    conn_sock = rv;

    /* in_addr.s_addr always in network order; bcm_ip_t in host order */
    dest_ip = ntohl(client_addr.sin_addr.s_addr);
    ip_to_str(dest_ip_str, dest_ip);

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Connection request from client '%s' on "
                 "socket %d\n"), dest_ip_str, conn_sock));
    
    if (BCM_FAILURE(rv)) {
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP-Socket WARNING: Invalid IP address '%s', closing "
                  "socket %d\n"), dest_ip_str, conn_sock));
        close(conn_sock);
        return BCM_E_NONE;
    }
    
    ATP_SOCK_LOCK;
    
    /* If destination IP is not found, create a new entry */
    if ((db_ptr = db_ip_lookup(atp_sock_db_list, dest_ip)) == NULL) {
        if ((db_ptr = db_entry_create(&atp_sock_db_list,
                                      atp_sock_cpu_key_null, dest_ip,
                                      ATP_SOCK_DB_CLEAR)) == NULL) {
            ATP_SOCK_UNLOCK;
            LOG_WARN(BSL_LS_TKS_ATP,
                     (BSL_META("ATP-Socket ERROR: Cannot create new entry, "
                      "closing socket %d\n"), conn_sock));
            close(conn_sock);
            return BCM_E_NONE;
        }
    }

    /* Check that connection does not exist */
    if (db_ptr->conn.flags & ATP_SOCK_CONN_VALID) {
        ATP_SOCK_UNLOCK;
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP-Socket ERROR: Connection already exists, closing "
                  "socket %d\n"), conn_sock));
        close(conn_sock);
        return BCM_E_NONE;
    }
    
    /* Create connection */
    rv = conn_create(db_ptr, conn_sock);
    if (BCM_FAILURE(rv)) {
        ATP_SOCK_UNLOCK;
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP-Socket WARNING: Cannot create connection to '%s', "
                  "closing socket %d\n"), dest_ip_str, conn_sock));
        close(conn_sock);
        return BCM_E_NONE;
    }
    
    ATP_SOCK_UNLOCK;
    
    return BCM_E_NONE;
}


/*
 * Function:
 *     server_conn_thread
 * Purpose:
 *     Out-Of-Band Server thread that listens for connection requests.
 *     For each connection request, it spawns a new thread that will
 *     handle incoming packets from the connection request originator.
 * Parameters:
 *     cookie      - Pointer to Server listening port
 * Returns:
 *     None
 */

STATIC void
server_conn_thread(void *cookie)
{
    int                 rv = 0;
    int                 listen_port = 0;
    int                 listen_sock = 0;   /* Listening socket */
    const int           sock_opt_on = 1;
    atptrans_socklen_t  addr_len;
    struct sockaddr_in  server_addr;


    listen_port = *((int *)cookie);

    /* Open socket */
    if ((rv = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot open Server listening socket "
                   "(err=%d)\n"), rv));
        atp_sock_server_thread_id = SAL_THREAD_ERROR;
        sal_thread_exit(rv);
        return;
    }

    listen_sock = rv;

    /* 
     * Set socket option to allow server to restart, in case
     * there are existing connections using the listening port
     */
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, 
               (char *)&sock_opt_on, sizeof(sock_opt_on));

    /* Bind socket to listen to any IP address and to a known port */
    addr_len = (atptrans_socklen_t) sizeof(server_addr);
    sal_memset((void *)&server_addr, 0, (int)addr_len);
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(listen_port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);      
    if ((rv = bind (listen_sock, 
                    (struct sockaddr *) &server_addr, addr_len)) < 0) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot bind Server listening "
                   "socket %d (err=%d)\n"), listen_sock, rv));
        close(listen_sock);
        atp_sock_server_thread_id = SAL_THREAD_ERROR;
        sal_thread_exit(rv);
        return;
    }

    /* Set the socket to listen for connection requests */
    if ((rv = listen(listen_sock, ATP_SOCK_BACKLOG)) < 0) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot set Server listening mode "
                   "on socket %d (err=%d)\n"), listen_sock, rv));
        close(listen_sock);
        atp_sock_server_thread_id = SAL_THREAD_ERROR;
        sal_thread_exit(rv);
        return;
    }

    atp_sock_server_lport = listen_port;
    atp_sock_server_lsock = listen_sock;

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Server ready, listening on socket %d\n"),
                 atp_sock_server_lsock));

    /* Main loop to service connection requests */ 
    atp_sock_server_exit = FALSE;
    while (!atp_sock_server_exit) {

        rv = accept_connection(atp_sock_server_lsock);

        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    close(atp_sock_server_lsock);
    
    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Exit Server Thread '%s', "
                 "close socket %d\n"), atp_sock_server_thread_name,
                 atp_sock_server_lsock));

    ATP_SOCK_LOCK;

    atp_sock_server_lsock = -1;
    atp_sock_server_lport = -1;
    atp_sock_server_thread_id = SAL_THREAD_ERROR;

    ATP_SOCK_UNLOCK;

    sal_thread_exit(rv);

    return;
}


/*
 * Function:
 *     atptrans_socket_tx_atp
 * Purpose:
 *     Transmit data packet to specified destination CPU using
 *     sockets.
 *
 *     When sockets is 'installed' for the specified destination CPU,
 *     this routine is called for any ATP TX operation.
 *
 *     It checks for an existing connection.  If found, existing TX
 *     socket is used.  Otherwise, a new connection request is made
 *     and the connection table is updated.
 *
 * Parameters:
 *     dest_key    - Destination CPU to send to
 *     client_id   - Client ID to send to
 *     pkt_buf     - Buffer containing data to send
 *     len         - Length of packet data in bytes
 *     ct_flags    - Bitmap of flags passed in (N/A)
 *     callback    - If not NULL, do async and callback (N/A)
 *     cookie      - Passed on callback (N/A)
 * Returns:
 *     BCM_E_NONE  Success
 *     BCM_E_XXX   Failure,
 *                 . NULL packet buffer
 *                 . Could not find valid connection for given CPU
 *                 . Error sending packet
 */

STATIC int
atptrans_socket_tx_atp(cpudb_key_t dest_key,
                       int client_id,
                       uint8 *pkt_buf,
                       int len,
                       uint32 ct_flags,
                       atp_tx_cb_f callback,
                       void *cookie)
{
    int    rv;
    int    conn_sock;

    if (pkt_buf == NULL) {
        return BCM_E_FAIL;
    }

    /* Check for flag semantics */
    if (ct_flags & CPUTRANS_NO_HEADER_ALLOC) {
        len -= CPUTRANS_HEADER_BYTES;
        pkt_buf += CPUTRANS_HEADER_BYTES;
    }

    /* Get socket */
    ATP_SOCK_LOCK;
    if ((conn_sock = sock_get(atp_sock_db_list, dest_key)) < 0) {
        ATP_SOCK_UNLOCK;
        return BCM_E_FAIL;
    }

    /* Send message */
    rv = send_pkt_data(client_id, conn_sock, pkt_buf, len);

    ATP_SOCK_UNLOCK;

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP-Socket: Send packet on socket %d\n"),
               conn_sock));
    
    if (callback != NULL) {
        callback(pkt_buf, cookie, rv);
    }
    
    return rv;
}


/*
 * Function:
 *     cleanup
 * Purpose:
 *     It cleanups the ATP over Sockets subsystem as follows,
 *     - For each entry in the database list,
 *       . Destroys connection if valid
 *       . Uninstalls the socket interface and removes entry from list
 *     - Stops the ATP-Socket Server
 * Parameters:
 *     None
 * Returns:
 *     None
 */

STATIC void
cleanup (void)
{
    atp_sock_db_entry_t    *db_ptr;

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Cleanup\n")));

    ATP_SOCK_LOCK;

    /* Uninstall the socket interface */
    ATP_SOCK_DB_FOREACH_ENTRY(atp_sock_db_list, db_ptr) {
        atptrans_socket_uninstall(db_ptr->cpu_key);
    }

    ATP_SOCK_UNLOCK;

    /* Stop the ATP-Socket Server */
    atptrans_socket_server_stop();

    return;
}


/*
 * Function:
 *     init
 * Purpose:
 *     It initializes the ATP over Sockets subsystem:
 *     - If needed, destroys current connections, uninstalls, and
 *       clears the database list
 *     - Destroys current mutex lock and creates a new one
 *     - Initializes database list
 *     - Reset configuration parameters to default values
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_NONE  Success
 *     BCM_E_XXX   Failure,
 *                 . Cannot create mutex lock
 */

STATIC int
init(void)
{
    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Init\n")));

    /* System resources cleanup, if needed */
    if (ATP_SOCK_INIT_DONE) {
        cleanup();
    }

    /* Mutex locks */
    if (atp_sock_mlock != NULL) {
        sal_mutex_destroy(atp_sock_mlock);
        atp_sock_mlock = NULL;
    }
    if ((atp_sock_mlock = sal_mutex_create("ATP_Socket_mlock")) == NULL) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot create mutex\n")));
        return BCM_E_MEMORY;
    }
    
    atp_sock_db_list = NULL;

    CPUDB_KEY_COPY(atp_sock_local_cpu_key, atp_sock_cpu_key_null);

    return BCM_E_NONE;
}

/* If atptrans_socket is started without any usable RX units,
   initialize the RX pool only, and use rx_pool allocators
   for the ATP transport allocators.
   
*/

STATIC int
_atptrans_socket_host(void)
{
    int rv = BCM_E_NONE;
    int attached;
    int rx_units;
    int i;
    bcm_trans_ptr_t *atp_transport;

    rv = bcm_attach_max(&attached);

    if (BCM_SUCCESS(rv)) {

        /* Count how many units have RX available */
        for (i=rx_units=0; i<attached; i++) {
            if (!bcm_rx_active(i)) {
                rv = bcm_rx_init(i);
                if (BCM_SUCCESS(rv)) {
                    rx_units++;
                }
            } else {
                rx_units++;
            }
        }

        if (rx_units == 0) {
            rv = atp_config_get(NULL, NULL, &atp_transport);
            if (BCM_SUCCESS(rv) && atp_transport != &host_atp_transport) {

                /* Set up RX pool if needed */
                if (!bcm_rx_pool_setup_done()) {
                    rv = bcm_rx_pool_setup(-1, -1);
                }

                /* Save original transport */
                original_atp_transport = atp_transport;

                /* Set up host transport */
                host_atp_transport = *atp_transport;
                host_atp_transport.tp_data_alloc = bcm_rx_pool_alloc;
                host_atp_transport.tp_data_free = bcm_rx_pool_free;
                rv = atp_config_set(-1, -1, &host_atp_transport);
            }
        } else {
            /* Found RX units, so host mode is not appropriate */
            rv = BCM_E_NONE;
        }
    }


    return rv;
}

/* If atptrans_socket was started in host mode above, restore the
   original ATP transport structure.
*/

STATIC int
_atptrans_socket_host_stop(void)
{
    int rv = BCM_E_NONE;
    if (original_atp_transport) {
        rv = atp_config_set(-1, -1, original_atp_transport);
        original_atp_transport = NULL;
    }

    return rv;
}

/*
 * Function:
 *     atptrans_socket_server_start
 * Purpose:
 *     Initializes the ATP over Sockets subsystem (if it has not been yet), and
 *     it creates the Server thread.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_NONE  Success, ATP-Socket Server created and ready for connection 
 *                 requests.
 *     BCM_E_XXX   Failure,
 *                 . Server thread is running
 *                 . Cannot create Server thread
 * Note:
 *     1. Only one ATP-Socket Server can be running at a time.
 *     2. Supported only in chassis based systems.
 */

int
atptrans_socket_server_start(void)
{
    int rv;

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Start Server\n")));

    rv = _atptrans_socket_host();
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (!ATP_SOCK_INIT_DONE) {
        rv = init();
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    ATP_SOCK_LOCK;

    if (ATP_SOCK_SERVER_RUNNING) {
        ATP_SOCK_UNLOCK;
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP-Socket: Server thread is already running\n")));
        return BCM_E_EXISTS;
    }

    /* Create Server thread */
    atp_sock_server_thread_id = sal_thread_create(atp_sock_server_thread_name,
                                                  SAL_THREAD_STKSZ,
                                                  atp_sock_cfg_thread_pri,
                                                  server_conn_thread, 
                                                  (void *)
                                                  &atp_sock_cfg_local_port);

    if (atp_sock_server_thread_id == SAL_THREAD_ERROR) {
        ATP_SOCK_UNLOCK;
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot create Server thread\n")));
        return BCM_E_FAIL;
    }

    ATP_SOCK_UNLOCK;

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP-Socket: Created Server Thread '%s'\n"),
               atp_sock_server_thread_name));

    return BCM_E_NONE;
}


/*
 * Function:
 *     atptrans_socket_server_stop
 * Purpose:
 *     It stops the ATP-Socket Server.  It closes Server listening socket
 *     and destroys the corresponding Server thread.  The routine
 *     only returns after the Server thread has exited or has
 *     exceeded the waiting time.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_NONE  Success, Server thread exited
 *     BCM_E_XXX   Failure, Server thread did not exit in expected wait time
 * Note:
 *     Supported only in chassis based systems
 */

int 
atptrans_socket_server_stop(void)
{
    int          i;

    if (!ATP_SOCK_INIT_DONE) {
        return BCM_E_NONE;
    }

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Stop Server\n")));

    ATP_SOCK_LOCK;

    if (!ATP_SOCK_SERVER_RUNNING) {
        ATP_SOCK_UNLOCK;
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP-Socket: Server not running\n")));
        return BCM_E_NONE;
    }

    /* Force Server thread to exit */
    atp_sock_server_exit = TRUE;
    shutdown(atp_sock_server_lsock, SHUT_RDWR);

    ATP_SOCK_UNLOCK;
    
    /* Wait for server thread to exit   */
    for (i = 0; i < ATP_SOCK_STOP_RETRIES_MAX; i++) {
        if (!ATP_SOCK_SERVER_RUNNING) {
            break;
        }
        sal_usleep(ATP_SOCK_RETRY_WAIT_US);
    }

    if (i >= ATP_SOCK_STOP_RETRIES_MAX) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot stop Server Thread in %d ms\n"),
                   (ATP_SOCK_STOP_RETRIES_MAX * ATP_SOCK_RETRY_WAIT_US)/1000));
        return BCM_E_FAIL;
    }

    _atptrans_socket_host_stop();

    /* Reset configuration parameters to default values */
    atp_sock_cfg_thread_pri = ATP_SOCK_THREAD_PRI_DEFAULT;
    atp_sock_cfg_local_port = ATP_SOCK_SERVER_LPORT_DEFAULT;
    atp_sock_cfg_remote_port = ATP_SOCK_SERVER_LPORT_DEFAULT;

    return BCM_E_NONE;
}


/*
 * Function:
 *     atptrans_socket_server_running
 * Purpose:
 *     It indicates whether the ATP-Socket Server is currently running.
 * Parameters:
 *     None
 * Returns:
 *     TRUE,  Server is running
 *     FALSE, Server is not running
 * Note:
 *     Supported only in chassis based systems
 */

int
atptrans_socket_server_running(void)
{
    return ATP_SOCK_SERVER_RUNNING;
}


/*
 * Function:
 *     atptrans_socket_config_set
 * Purpose:
 *     It sets the configuration values.
 * Parameters:
 *     priority     - Priority for new threads
 *     listen_port  - Listening port for client connection request
 *     connect_port - Connection port for server connection request
 * Returns:
 *     BCM_E_NONE  Success
 *     BCM_E_XXX   Failure
 * Note:
 *     1. Supported only in chassis based systems.
 *     2. New values will only take effect on newly create threads.
 *     3. A value of 0 (or negative) will leave the parameter unchanged.
 */

int
atptrans_socket_config_set(int priority, int listen_port, int connect_port)
{
    int         rv;

    if (!ATP_SOCK_INIT_DONE) {
        rv = init();
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    if (priority >= 0) {
        atp_sock_cfg_thread_pri = priority;
    }
    if (listen_port >= 0) {
        atp_sock_cfg_local_port = listen_port;
    }
    if (connect_port >= 0) {
        atp_sock_cfg_remote_port = connect_port;
    }

    /* Mark ports are unique if they are not same. */
    if (atp_sock_cfg_remote_port != atp_sock_cfg_local_port) {
        atp_sock_cfg_unique_lr_ports = 1;
    } else {
        atp_sock_cfg_unique_lr_ports = 0;
    }

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Configuration set to priority %d, "
                          "listen (local) port %d, connect (remote) port %d\n"),
                 atp_sock_cfg_thread_pri, atp_sock_cfg_local_port,
                 atp_sock_cfg_remote_port));

    return BCM_E_NONE;
}


/*
 * Function:
 *     atptrans_socket_install
 * Purpose:
 *     It installs the socket interface as the underlying
 *     communication mechanism for the ATP transport between the
 *     local and the given destination CPU.
 *
 *     A new DB entry is created and added to the ATP-Socket database list.
 *
 *     If the local CPU is the Client and destination CPU is the Server,
 *     a connection request is made to the destination CPU.  Otherwise,
 *     it is expected that the remote destination CPU, the Client, will
 *     be making the request.
 *
 *     In any case, the 'install' flag is set so that when a connection
 *     is finally made, the ATP override is set to use the socket interface.
 *
 * Parameters:
 *     dest_cpu    - Destination CPU key to transmit over sockets
 *     dest_ip     - Destination CPU IP address 
 *     flags       - Flags
 *
 * Returns:
 *     BCM_E_NONE  Success
 *     BCM_E_XXX   Failure,
 *                 - Initialization failure
 *                 - Entry exists with conflicting values (different key or IP)
 *                 - Cannot create entry
 *                 
 * Note:
 *     1. Supported only in chassis based systems
 *
 *     2. The installation model will allow only Clients to make
 *        connection requests.  This prevents problems caused by
 *        racing conditions.
 *
 *     3. The selection of Client and Server in a pair of CPUs is as follows,
 *            Client - CPU with LOWER key value
 *            Server - CPU with HIGHER key value
 */

int
atptrans_socket_install(cpudb_key_t dest_cpu_key,
                        bcm_ip_t dest_ip, uint32 flags)
{
    int                  rv;
    atp_sock_db_entry_t  *db_ptr;
    char                 dest_ip_str[SAL_IPADDR_STR_LEN];

    /* Initialize ATP over Sockets subsystem if needed */
    if (!ATP_SOCK_INIT_DONE) {
        rv = init();
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP-Socket: Install CPU key "
                        CPUDB_KEY_FMT_EOLN),
               CPUDB_KEY_DISP(dest_cpu_key)));

    ATP_SOCK_LOCK;

    /* Add to the database list */
    if ((db_ptr = db_entry_create(&atp_sock_db_list,
                                  dest_cpu_key, dest_ip, 
                                  ATP_SOCK_DB_CPU_VALID |
                                  ATP_SOCK_DB_INSTALL | flags )) == NULL) {
        ATP_SOCK_UNLOCK;
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot add new entry for CPU key "
                            CPUDB_KEY_FMT_EOLN),
                   CPUDB_KEY_DISP(dest_cpu_key)));
        return BCM_E_FAIL;
    }

    if (db_ptr->flags & ATP_SOCK_DB_INSTALLED) {
        ATP_SOCK_UNLOCK;
        return BCM_E_NONE;
    }

    /* 
     * Request connection 
     * 
     * Connection requests are made only by Clients. The 'Client' is 
     * the entity with the lowest value on a pair of CPU keys.  In our case,
     * this is the local and destination CPU key pair.
     */
    if (CPUDB_KEY_COMPARE(atp_sock_local_cpu_key, dest_cpu_key) < 0) {
        if ((conn_request(db_ptr)) < 0) {
            ATP_SOCK_UNLOCK;
            return BCM_E_FAIL;
        }
    }


    ATP_SOCK_UNLOCK;

    ip_to_str(dest_ip_str, db_ptr->ip);
    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP-Socket: Installed on CPU key " CPUDB_KEY_FMT "), '%s'\n"),
                 CPUDB_KEY_DISP(db_ptr->cpu_key), dest_ip_str));

    return BCM_E_NONE;
}


/*
 * Function:
 *     atptrans_socket_uninstall
 * Purpose:
 *     It uninstalls the socket interface and restores the default
 *     communication mechanism for the ATP transport between the
 *     local and the destination CPU.
 * 
 *     The corresponding entry is removed from the database list. 
 *
 *     If entry has a valid connection, this is destroyed.  The ATP
 *     override is cleared and ATP reverts to its normal transmit operation
 *     for specified CPU. 
 *
 * Parameters:
 *     dest_cpu    - Destination CPU to remove the socket interface
 * Returns:
 *     BCM_E_NONE  Success
 *     BCM_E_XXX   Failure
 * Note:
 *     Supported only in chassis based systems
 */

int
atptrans_socket_uninstall(cpudb_key_t dest_cpu_key)
{
    int                  rv;
    atp_sock_db_entry_t  *db_ptr;

    if (!ATP_SOCK_INIT_DONE) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR:  Not initialized\n")));
        return BCM_E_FAIL;
    }

    LOG_DEBUG(BSL_LS_TKS_ATP,
                 (BSL_META("ATP-Socket: Uninstall CPU key "
                           CPUDB_KEY_FMT_EOLN),
                  CPUDB_KEY_DISP(dest_cpu_key)));

    ATP_SOCK_LOCK;
    
    if ((db_ptr = db_cpu_key_lookup(atp_sock_db_list, 
                                    dest_cpu_key)) == NULL) {
        ATP_SOCK_UNLOCK;
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP-Socket ERROR: Cannot find valid entry for CPU key "
                            CPUDB_KEY_FMT_EOLN),
                   CPUDB_KEY_DISP(dest_cpu_key)));
        return BCM_E_NOT_FOUND;
    }

    rv = db_entry_remove(&atp_sock_db_list, db_ptr);

    ATP_SOCK_UNLOCK;

    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP-Socket: Uninstalled failed\n")));
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     atptrans_socket_local_cpu_key_set
 * Purpose:
 *     Sets the key for the local CPU.
 * Parameters:
 *     cpu_key    - Local CPU key
 * Returns:
 *     BCM_E_NONE  Success
 *     BCM_E_XXX   Failure
 * Note:
 *     1. The local CPU key SHOULD be set before the install is called
 *     2. Supported only in chassis based systems
 */

int
atptrans_socket_local_cpu_key_set(cpudb_key_t const cpu_key)
{
    int rv;

    if (!ATP_SOCK_INIT_DONE) {
        rv = init();
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    ATP_SOCK_LOCK;
    CPUDB_KEY_COPY(atp_sock_local_cpu_key, cpu_key);
    ATP_SOCK_UNLOCK;

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP-Socket: Set local CPU key " CPUDB_KEY_FMT "\n"),
               CPUDB_KEY_DISP(atp_sock_local_cpu_key)));

    return BCM_E_NONE;
}


/*
 * Function:
 *     atptrans_socket_show
 * Purpose:
 *     It displays detailed information for the ATP over Sockets subsystem.
 * Parameters:
 *     None
 * Returns:
 *     None
 * Note:
 *     Supported only in chassis based systems
 */

void
atptrans_socket_show(void)
{
    char                 ip_str[SAL_IPADDR_STR_LEN];
    atp_sock_db_entry_t  *db_ptr;

    /* Initialize if needed */
    if (!ATP_SOCK_INIT_DONE) {
        LOG_INFO(BSL_LS_TKS_ATP,
                 (BSL_META("ATP over Sockets not initialized\n")));
        return;
    }

    /* Server */
    LOG_INFO(BSL_LS_TKS_ATP,
             (BSL_META("ATP-Socket Server\n")));
    if (atptrans_socket_server_running()) {
        LOG_INFO(BSL_LS_TKS_ATP,
                 (BSL_META("    Listening Port    %d\n"),
                  atp_sock_server_lport));
        LOG_INFO(BSL_LS_TKS_ATP,
                 (BSL_META("    Listening Socket  %d\n"),
                  atp_sock_server_lsock));
        LOG_INFO(BSL_LS_TKS_ATP,
                 (BSL_META("    Thread-Name       %s\n"),
                  atp_sock_server_thread_name));
    } else {
        LOG_INFO(BSL_LS_TKS_ATP,
                 (BSL_META("    Not running\n")));
    }
    LOG_INFO(BSL_LS_TKS_ATP,
             (BSL_META("\n")));

    /* Database List */
    LOG_INFO(BSL_LS_TKS_ATP,
             (BSL_META("ATP-Socket Database\n")));
    LOG_INFO(BSL_LS_TKS_ATP,
             (BSL_META("    CPU Key            IP Address       Flags  Socket  "
              "Thread\n")));
    LOG_INFO(BSL_LS_TKS_ATP,
             (BSL_META("    -----------------  ---------------  -----  ------  "
              "-----------\n")));
    ATP_SOCK_DB_FOREACH_ENTRY(atp_sock_db_list, db_ptr) {
        ip_to_str(ip_str, db_ptr->ip);
        LOG_INFO(BSL_LS_TKS_ATP,
                 (BSL_META("    %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x  %-15s  0x%3.3x"),
                  SAL_MAC_ADDR_LIST(db_ptr->cpu_key.key),
                  ip_str,
                  db_ptr->flags));

        /* Connection */
        if (db_ptr->conn.flags & ATP_SOCK_CONN_VALID) {
            LOG_INFO(BSL_LS_TKS_ATP,
                     (BSL_META("  %6d  %-11s"),
                      db_ptr->conn.sock_fd,
                      db_ptr->conn.rx_thread_name));
        }

        LOG_INFO(BSL_LS_TKS_ATP,
                 (BSL_META("\n")));
    }
    LOG_INFO(BSL_LS_TKS_ATP,
             (BSL_META("\n")));

    return;
}

#else /* ATPTRANS_SOCKET_SUPPORTED */

int
atptrans_socket_server_start(void)
{
    return BCM_E_UNAVAIL;
}

int 
atptrans_socket_server_stop(void)
{
    return BCM_E_UNAVAIL;
}

int
atptrans_socket_server_running(void)
{
    return FALSE;
}

int
atptrans_socket_config_set(int priority, int listen_port, int connect_port)
{
    COMPILER_REFERENCE(priority);
    COMPILER_REFERENCE(listen_port);
    COMPILER_REFERENCE(connect_port);
    return BCM_E_UNAVAIL;
}

int
atptrans_socket_install(cpudb_key_t dest_cpu_key,
                        bcm_ip_t dest_ip, uint32 flags)
{
    COMPILER_REFERENCE(dest_cpu_key);
    COMPILER_REFERENCE(dest_ip);
    COMPILER_REFERENCE(flags);
    return BCM_E_UNAVAIL;
}

int
atptrans_socket_uninstall(cpudb_key_t dest_cpu_key)
{
    COMPILER_REFERENCE(dest_cpu_key);
    return BCM_E_UNAVAIL;
}

int
atptrans_socket_local_cpu_key_set(cpudb_key_t const cpu_key)
{
    COMPILER_REFERENCE(cpu_key);
    return BCM_E_UNAVAIL;
}

void
atptrans_socket_show(void)
{
    return;
}

#endif /* ATPTRANS_SOCKET_SUPPORTED */

#endif /* INCLUDE_ATPTRANS_SOCKET */

/*! \file verinet.h
 *
 * PLI simulation definitions (Unix PLI sim only).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef VERINET_H
#define VERINET_H

#include <stdarg.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_thread.h>

#include <sim/pli/pli.h>

#if !defined(_SOCKLEN_T) && !defined(__socklen_t_defined)
typedef int socklen_t;
#endif

#define N_VERINET_DEVICES       16

/*
 * Convenient access to different IP address formats
 */
typedef union ipaddr_u {
    struct sockaddr_storage ss;
    struct sockaddr sa;
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
} ipaddr_t;

/*
 * Register types for PLI transactions
 */

#define PCI_CONFIG              0xfeed1001
#define PCI_MEMORY              0xfeed1002
#define I2C_CONFIG              0xfeed1003
#define PLI_CONFIG              0xfeed1004
#define JTAG_CONFIG             0xfeed1005
#define DMA_MEMORY              0xfeed1006
#define SOC_INTERNAL            0xfeed1007
#define PCI_SLAVE_MEMORY        0xfeed1008
#define PHY_EXERNAL             0xfeed1009

/*
 * Returned to PLI server code when a client connection is established.
 */

typedef struct netconnection_t {
    int intsock;
    int dmasock;
    int piosock;
    ipaddr_t inetaddr;
} netconnection_t;

/*
 * PLI RPC command type: Format of RPC command messages.
 * All RPC messages from the SAL/testclient to the server (veriserv)
 * are of the following format.
 */

#define MAX_RPC_ARGS   512      /* DWORD ARG COUNT */

typedef struct rpc_cmd_t {
    uint32_t  opcode;
    /* RPC OPCODES : Requests */
#define   RPC_GETREG_REQ        0x101
#define   RPC_SETREG_REQ        0x102
#define   RPC_IOCTL_REQ         0x103

    /* For PCI Interface */
#define   RPC_PCI_CONFIG_GET    0x104
#define   RPC_PCI_CONFIG_SET    0x105
#define   RPC_PCI_MEMORY_GET    0x106
#define   RPC_PCI_MEMORY_SET    0x107
#define   RPC_DMAC_WRITE_REQ    0x108
#define   RPC_DMAC_READ_REQ     0x109
#define   RPC_SEND_INTR         0x110
#define   RPC_PCI_CONDITION     0x111
#define   RPC_REGISTER_CLIENT   0x112

    /* For I2C Interface */
#define   RPC_I2C_CONDITION     0x113
#define   RPC_I2C_BYTE_READ     0x114
#define   RPC_I2C_BYTE_WRITE    0x115
#define   RPC_I2C_ASSERT_ACK    0x116
#define   RPC_I2C_CHECK_ACK     0x117
#define   RPC_I2C_ASSERT_START  0x118
#define   RPC_I2C_CHECK_START   0x119
#define   RPC_I2C_ASSERT_STOP   0x120
#define   RPC_I2C_CHECK_STOP    0x121

    /* For simulation data transfer */
#define   RPC_PACKET_DATA_RAW   0x122
    /* First arg is data length in bytes; Data is packed into args */
    /* Currently supports up to 2 K */
#define   RPC_PACKET_DESC       0x123
    /* Data is coded (described) in args */
#define   RPC_SIM_WAIT          0x124
    /* Simulation should wait the specified number of usecs in arg 0. */

    /* Call test routine with packet data */
#define   RPC_TEST_PKT          0x125
#define   RPC_DMAC_WRITE_BYTES_REQ   0x126
#define   RPC_DMAC_READ_BYTES_REQ    0x127
#define   RPC_SCHAN_REQ         0x128

#define   RPC_SHUTDOWN          0x3fd
#define   RPC_TX_ENABLE         0x3fe
#define   RPC_SEND_PACKET       0x3ff
#define   RPC_DISCONNECT        0x400
#define   RPC_TERMINATE         0x500

    /* RPC OPCODES: Responses */
#define   RPC_GETREG_RESP       0x201
#define   RPC_SETREG_RESP       0x202
#define   RPC_IOCTL_RESP        0x203
#define   RPC_DMAC_READ_RESP    0x204
#define   RPC_DMAC_WRITE_RESP   0x205
#define   RPC_PACKET_RESP       0x206
#define   RPC_SIM_WAIT_RESP     0x207
#define   RPC_DMAC_WRITE_BYTES_RESP   0x208
#define   RPC_DMAC_READ_BYTES_RESP    0x209
#define   RPC_SCHAN_RESP        0x210

    uint32_t  argcount;
    uint32_t  args[MAX_RPC_ARGS];

    uint32_t  status;
    /* RPC STATUS CODES */
#define   RPC_OK                0x300
#define   RPC_OUTSTANDING       0x200
#define   RPC_FAIL              0x100
} rpc_cmd_t;

/* Indicate opcodes that should use rpc string packing */
#define RPC_OP_USE_STRING(op)                   \
    (((op) == RPC_DMAC_WRITE_BYTES_REQ)   ||    \
     ((op) == RPC_DMAC_READ_BYTES_REQ)    ||    \
     ((op) == RPC_DMAC_WRITE_BYTES_RESP)  ||    \
     ((op) == RPC_DMAC_READ_BYTES_RESP))

/* Connection information maintained by PLI client */

typedef struct verinet_t {

    int dev_no;

    char pli_host[128];
    int pli_port;
    int int_port;
    int dma_port;
    int skip_test_int; /* skip test intr sent by plicode */
    int sockfd; /* fd for socket conn. to simulator */
    int jobs_started;

    /*
     * pli_mutex is used by client routines so that multiple threads
     * may share the pli_setreg/pli_getreg calls.
     */
    sal_mutex_t pli_mutex;

    /*
     * DMAC
     */
    int dmac_inited;
    sal_thread_t dmac_listener;
    sal_thread_t dmac_handler;
    sal_sem_t dmac_listening;
    volatile int dmac_worker_exit;
    int dmac_fd;

    /*
     * INTR
     */
    int intr_inited;
    sal_thread_t intr_listener;
    sal_thread_t intr_dispatch;
    sal_sem_t intr_dispatch_sem;
    sal_sem_t intr_listening;
    int intr_skip_test;
    int intr_finished;
    sal_thread_t intr_handler_thread;
    volatile int intr_worker_exit;
    int intr_fd;
    pli_isr_t isr;
    void *isr_data;

} verinet_t;

extern verinet_t verinet[];

/*
 * Network utility routines.
 */

extern int
writen(int fd, void *ptr, int nbytes);

extern int
readn(int fd, void *ptr, int nbytes);

extern int
get_command(int sockfd, struct timeval *tv, rpc_cmd_t *cmd);

extern int
wait_command(int sockfd, rpc_cmd_t *cmd);

extern int
poll_command(int sockfd, rpc_cmd_t *cmd);

extern int
write_command(int sockfd, rpc_cmd_t *cmd);

extern int
get_pkt_data(int sockfd, struct timeval *tv, uint32_t *data);

extern void
make_rpc(rpc_cmd_t *cmd, uint32_t opcode, uint32_t status,
         uint32_t argcount, ...);

extern void
make_rpc_string(rpc_cmd_t *cmd, uint32_t opcode, uint32_t status,
                uint32_t addr, uint8_t *data, uint32_t len);

extern int
getsockport(int sock_fd);

extern const char *
inet_str(const ipaddr_t *ipaddr, char *dst, socklen_t size);

/*
 * RPC: requests.
 */

extern void
make_rpc_setreg_req(rpc_cmd_t *cmd,
                    uint32_t type, uint32_t regnum, uint32_t regval);

extern void
make_rpc_getreg_req(rpc_cmd_t *cmd,uint32_t type, uint32_t regnum);

extern void
make_rpc_disconnect_req(rpc_cmd_t *cmd);

extern void
make_rpc_setmem_req(rpc_cmd_t *cmd, uint32_t addr, uint32_t data);

extern void
make_rpc_getmem_req(rpc_cmd_t *cmd, uint32_t addr);

extern void
make_rpc_intr_req(rpc_cmd_t *cmd, uint32_t cause);

extern void
make_rpc_register_req(rpc_cmd_t *cmd,
                      int int_port, int dmac_port, uint32_t net_addr);

extern void
make_rpc_register_ip6_req(rpc_cmd_t *cmd,
                          int int_port, int dmac_port, uint32_t *net_addr);

/*
 * RPC: responses.
 */

extern void
make_rpc_getreg_resp(rpc_cmd_t *cmd,
                     uint32_t status, uint32_t regval, char* buf32b);

extern void
make_rpc_setreg_resp(rpc_cmd_t *cmd, uint32_t regval);

extern void
make_rpc_getmem_resp(rpc_cmd_t *cmd, uint32_t data);

extern void
make_rpc_setmem_resp(rpc_cmd_t *cmd, uint32_t data);

extern void
make_rpc_intr_resp(rpc_cmd_t *cmd, uint32_t cause);

extern void
make_rpc_register_resp(rpc_cmd_t *cmd, uint32_t status);



/*
 * Server calls.
 */

extern void
disconnect(int sockfd);

extern uint32_t
send_interrupt(int sockfd, uint32_t cause);

extern netconnection_t *
register_client(int sockfd, uint32_t int_port, uint32_t dma_port,
                int net_addrlen, uint32_t *net_addr);

extern int
unregister_client(netconnection_t *);

extern uint32_t
dma_writemem(int sockfd, uint32_t addr, uint32_t data);

extern uint32_t
dma_readmem(int sockfd, uint32_t addr);

extern uint32_t
dma_write_bytes(int sockfd, uint32_t addr, uint8_t *data, int len);

extern uint32_t
dma_read_bytes(int sockfd, uint32_t addr, uint8_t *data, int len);

/*
 * Support for genering fw packets with ncsim
 */

extern uint32_t
pli_send_packet(int dev_no, uint32_t,uint32_t,int,unsigned char *);

extern uint32_t
pli_tx_enable(int dev_no, uint32_t);

extern uint32_t
pli_shutdown(int dev_no);

extern uint32_t
pli_sim_start(int dev_no);

/*
 * DMAC: DMA Controller (emulator)
 * See: sal/unix/dmac.c
 */

extern int
dmac_init(verinet_t *v);

/*
 * INTC: Interrupt Controller
 * See: sal/unix/intr.c
 */

extern int
intr_init(verinet_t *v);

extern void
intr_set_vector(verinet_t *v, pli_isr_t isr, void *isr_data);

extern void
intr_handler(void *v_void); /* verinet_t * */

extern int
intr_int_context(void);

#endif /* VERINET_H */

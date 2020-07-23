/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pcid.h
 * Purpose:     Basic include file for pcid
 */

#ifndef _PCID_PCID_H
#define _PCID_PCID_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <soc/cmic.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <sal/appl/sal.h>
#include <bde/pli/verinet.h>

#include <sal/core/thread.h>

#include <sys/types.h>
#include <soc/types.h>
#include <soc/defs.h>
#include <soc/dcb.h>

#define PCIM_SIZE_MAX   0x40000
#define PCIC_SIZE       0x48

    /* Return values for process request */
#define PR_REQUEST_HANDLED 1
#define PR_NO_REQUEST 2
#define PR_ALL_DONE 0
#define PR_ERROR -1

#define PACKET_CHECK_INTERVAL	500000
#define COUNTER_ACT_INTERVAL	1000000

#define PKT_SIZE_MAX	(16 * 1024 + 8)

#define CPU_MAX_PACKET_QUEUE 100
#define CPU_MAX_PACKET_SIZE 16400

#define SBUS_ADDR_VERSION_1     1       /* Legacy */
#define SBUS_ADDR_VERSION_2     2       /* New */

typedef enum bcm_sim_msg_e {
    BCM_SIM_INVALID,
    BCM_SIM_ACTIVATE,
    BCM_SIM_DEACTIVATE,
    BCM_SIM_KEEP_ALIVE,
    BCM_SIM_DONE,
    BCM_SIM_TX_PKT,
    BCM_SIM_FROM_CPU,
    BCM_SIM_TO_CPU,
    BCM_SIM_SBUS_MSG_FROM_CPU,
    BCM_SIM_SBUS_MSG_TO_CPU,
    BCM_PKT_OVER_SOCKET
} bcm_sim_msg_t;

typedef enum bcmsim_opcode_e {
    BCMSIM_SCHAN_REG_READ,
    BCMSIM_SCHAN_REG_WRITE,
    BCMSIM_SCHAN_MEM_READ,
    BCMSIM_SCHAN_MEM_WRITE,
    BCMSIM_PCI_MEM_READ,
    BCMSIM_PCI_MEM_WRITE
} bcmsim_opcode_t;

typedef struct soc_datum_s soc_datum_t;

/* Keep the data part of the following structures the same */
typedef struct sim_data_s {
    uint32 data[SOC_MAX_MEM_WORDS];
} sim_data_t;

struct soc_datum_s {
    soc_datum_t     *next;
    uint32          selectors;
    uint32          addr;
    uint32          data[SOC_MAX_MEM_WORDS];
};

typedef struct packet_s {
    uint8           data[PKT_SIZE_MAX];
    int             length;
    int             consum;
#ifdef BCM_TRX_SUPPORT
    uint32          dcbd[13]; /* TR/SC : 4 + 7 words of HG + PBE */
#else
    uint32          dcbd[8];  /* FB/ER : 3 + 5 words of HG + PBE */
#endif
    struct packet_s *next;
} packet_t;

typedef struct pcid_info_s pcid_info_t;

typedef void (*_pcid_tx_cb_f)(pcid_info_t *pcid_info, dcb_t *dcb, int chan);
typedef void (*_pcid_ioctl_f)(pcid_info_t *pcid_info, uint32 *args);
typedef int (*_pcid_schan_f)(pcid_info_t *pcid_info, int unit, 
                             schan_msg_t* data);
typedef int (*_pcid_reset_f)(pcid_info_t *pcid_info, int unit);
typedef int (*_pcid_regmem_f)(pcid_info_t *pcid_info, int opcode, 
                              uint32 address, uint32 *data, uint32 nbits);

/*
 * SOC internal register/memory simulation
 */

#define SOC_DATUM_HASH_SIZE	131
#define SOC_HASH_DATUM(addr)	((addr) % SOC_DATUM_HASH_SIZE)
#define SOC_HASH_EXTENDED_DATUM(blk, addr) \
                                ((((addr) % SOC_DATUM_HASH_SIZE) + blk) % SOC_DATUM_HASH_SIZE)

extern int _soc_datum_read(pcid_info_t *pcid_info, int reg,
                           uint32 selectors, uint32 addr, int words,
                           uint32 *data);
extern int _soc_datum_write(pcid_info_t *pcid_info, int reg,
                            uint32 selectors, uint32 addr, int words,
                            uint32 *data);
extern int _soc_datum_add(pcid_info_t *pcid_info, int reg,
                          uint32 selectors, uint32 addr, int words,
                          uint32 *data);

extern int soc_datum_reg_read(pcid_info_t *pcid_info,
                              soc_regaddrinfo_t ainfo,
                              uint32 selectors, uint32 addr,
                              uint32 *data);
extern int soc_datum_reg_write(pcid_info_t *pcid_info,
                               soc_regaddrinfo_t ainfo,
                               uint32 selectors, uint32 addr,
                               uint32 *data);

extern int soc_datum_mem_read(pcid_info_t *pcid_info,
                              uint32 selectors, uint32 addr,
                              uint32 *data);
extern int soc_datum_mem_write(pcid_info_t *pcid_info,
                               uint32 selectors, uint32 addr,
                               uint32 *data);


#define SOC_DATUM_TRAVERSE_DIFF  0x1 /* Only invoke callback on datum values
                                      * which are different from the 
                                      * register reset val or
                                      * table null entry.
                                      */
typedef int (*soc_datum_traverse_cb)(pcid_info_t *pcid_info, uint32 flags,
                                     uint32 selectors, uint32 addr, int words,
                                     uint32 *data, void *user_data);
extern int soc_datum_traverse(pcid_info_t *pcid_info, uint32 flags,
                              soc_datum_traverse_cb *datum_cb);

struct pcid_info_s {
    int             opt_port;
    int             opt_verbose;
    int             opt_pli_verbose;
    int             opt_debug;
    int             opt_noreset;
    int             opt_pktloop;
    int             opt_pktfile;
    int             opt_counter;
    int             opt_gbp_mb;
    int             opt_gbp_wid;
    int             opt_gbp_banks;
    int             opt_override_devid;
    int             opt_override_revid;
    int             opt_port_config_id;
    soc_chip_types  opt_chip;
    char            *opt_chip_name;
    int             opt_i2crom;
    FILE            *i2crom_fp;
    int             opt_exit;
    int             opt_rpc_error;
    uint32          pcim[PCIM_SIZE_MAX / 4];
    uint32          pcic[PCIC_SIZE / 4];
    soc_datum_t     *reg_ht[SOC_DATUM_HASH_SIZE + 64];
    soc_datum_t     *mem_ht[SOC_DATUM_HASH_SIZE + 64];
    int             sockfd;
    int             newsockfd;
    netconnection_t *client;
    int             unit;
    int             pkt_count;
    packet_t        *pkt_list;
    sal_mutex_t     pkt_mutex; 
    sal_mutex_t     mem_mutex;
    _pcid_tx_cb_f   tx_cb;
    _pcid_ioctl_f   ioctl;
     sal_sem_t      cpu_sem;
    void            *cookie;    /* To be used by applications */
    int             pkt_sockfd;
    int             pkt_newsockfd;
    int             pkt_opt_port;
    void            *pkt_cookie;    /* To be used by applications */
    int             sbus_addr_version;
    int             cmicm;
    _pcid_schan_f   schan_cb;
    _pcid_reset_f   reset_cb;
    _pcid_regmem_f  regmem_cb;
    sal_thread_t    thread_id;
    int             single_mode;
};

typedef struct cpu_packet_desc_s {
    int             length;
    pbmp_t          port_bitmap;
    pbmp_t          untagged_port_bitmap;
    pbmp_t          l3_port_bitmap;
    int             crc_regen;
    int             pkt_num;
    int             priority;
    unsigned char   data[CPU_MAX_PACKET_SIZE];
    unsigned int    dest_bitmap;
    uint32          opcode;
    uint32          header_type;
    uint32          dst_modid;
    uint32          dst_portid;
    uint32          src_modid;
    uint32          src_tgid;
} cpu_packet_desc_t;

void           *pcid_thread(void *cookie);
void            pcid_shutdown(pcid_info_t *);

/* in util.c */
extern int pcid_x2i(int digit);
extern void pcid_type_name(char *buffer, uint32 type);

extern void pcid_test_pkt_handler(pcid_info_t *info, dcb_t *dcb, int chan);
extern void pcid_schan_cb_set(pcid_info_t *pcid_info, _pcid_schan_f f);
extern void pcid_reset_cb_set(pcid_info_t *pcid_info, _pcid_reset_f f);

/*
 * Get L3 hash table bucket size 
 * 16 entries / bucket for Raven, 8 for other FBX devices
 */
#define L3_BUCKET_SIZE(unit) (			     \
     soc_mem_index_count(unit, L3_ENTRY_ONLYm) /     \
     soc_mem_index_count(unit, L3_ENTRY_HIT_ONLYm))
     
/* Macros to Lock & Unlock PCID memory */
#define PCID_MEM_LOCK(pcid_info) \
    do { \
        if (pcid_info && pcid_info->mem_mutex) { \
            sal_mutex_take(pcid_info->mem_mutex, sal_sem_FOREVER); \
        } \
    } while (0)

#define PCID_MEM_UNLOCK(pcid_info) \
    do { \
        if (pcid_info && pcid_info->mem_mutex) { \
            sal_mutex_give(pcid_info->mem_mutex); \
        } \
    } while (0)

#endif  /* _PCID_PCID_H */

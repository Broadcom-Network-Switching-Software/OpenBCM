/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        topo_pkt.h
 * Purpose:     
 */

#ifndef   _STKTASK_TOPO_PKT_H_
#define   _STKTASK_TOPO_PKT_H_

#include <sdk_config.h>

/****************************************************************
 *
 * Topology packet format description
 *
 * See topology.txt for more information.
 */

#define TOPO_PKT_VERSION              0

/* These are relative to start of topo packet, post CPUTRANS header */
#define TOPO_VER_OFS                  0
#define TOPO_RSVD_OFS                 (TOPO_VER_OFS + sizeof(uint32))
#define TOPO_MSEQ_NUM_OFS             (TOPO_RSVD_OFS + sizeof(uint32))
#define TOPO_BASE_LEN_OFS             (TOPO_MSEQ_NUM_OFS + sizeof(uint32))
#define TOPO_FLAGS_OFS                (TOPO_BASE_LEN_OFS + sizeof(uint32))
#define TOPO_MOD_IDS_OFS              (TOPO_FLAGS_OFS + sizeof(uint32))
#define TOPO_HEADER_BYTES(_units)  \
    (TOPO_MOD_IDS_OFS + (_units) * sizeof(uint32))
#define TOPO_CMP_START_OFS            TOPO_MOD_IDS_OFS

#ifndef TOPO_PKT_BYTES_MAX
#define TOPO_PKT_BYTES_MAX (1500) /* Max supported topo pkt size */
#endif

#define BCM_ST_TOPO_CLIENT_ID SHARED_CLIENT_ID_TOPOLOGY

/* Topo packet and stack task related API functions */

extern int topo_pkt_handle_init(uint32 topo_atp_flags);
extern int topo_pkt_expect_set(int enable);
extern int bcm_stack_topo_update(cpudb_ref_t db_ref);
extern int bcm_stack_topo_create(cpudb_ref_t db_ref, topo_cpu_t *tp_cpu);

extern int topo_pkt_gen(
    cpudb_ref_t db_ref,
    cpudb_entry_t *entry,
    uint8 *pkt_data,
    int len,
    int *packed_bytes);

extern int topo_pkt_parse(
    cpudb_ref_t db_ref,
    cpudb_entry_t *entry,
    uint8 *topo_data,
    int len,
    topo_cpu_t *tp_cpu,
    int *ap_data_ofs);

extern int topo_delay_set(int delay_us);
extern int topo_delay_get(int *delay_us);

extern int topo_master_delay_set(int delay_us);
extern int topo_master_delay_get(int *delay_us);

#endif /* _STKTASK_TOPO_PKT_H_ */

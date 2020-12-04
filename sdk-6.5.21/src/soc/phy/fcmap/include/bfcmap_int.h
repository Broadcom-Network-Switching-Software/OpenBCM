/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BFCMAP_INTERNAL_H
#define BFCMAP_INTERNAL_H

#include <bfcmap_sal.h>
#include <bfcmap_util.h>
#include <bfcmap_vlan.h>
#include <bfcmap_stat.h>
#include <bfcmap_ident.h>
#include <bfcmap_drv.h>
#include <bfcmap_ctrl.h>


/* bmacsec equiv of reg/mem is in mem.h */
#define BFCMAP_REG64_READ(c,a,pv) \
            (c)->msec_io_f((c)->dev_addr, BFCMAP_PORT((c)),  \
                            BFCMAP_IO_REG_RD, (a), 2, 1, (pv))

#define BFCMAP_REG32_READ(c,a,pv) \
            (c)->msec_io_f((c)->dev_addr, BFCMAP_PORT((c)),  \
                            BFCMAP_IO_REG_RD, (a), 1, 1, (pv))

#define BFCMAP_REG64_WRITE(c,a,pv) \
            (c)->msec_io_f((c)->dev_addr, BFCMAP_PORT((c)),  \
                            BFCMAP_IO_REG_WR, (a), 2, 1, (pv))

#define BFCMAP_REG32_WRITE(c,a,pv) \
            (c)->msec_io_f((c)->dev_addr, BFCMAP_PORT((c)),  \
                            BFCMAP_IO_REG_WR, (a), 1, 1, (pv))


#define BFCMAP_WWN_HI(mac)          BMAC_TO_32_HI(mac)
#define BFCMAP_WWN_LO(mac)          BMAC_TO_32_LO(mac)
#define BFCMAP_WWN_BUILD(mac,hi,lo) BMAC_BUILD_FROM_32(mac,hi,lo)

#define BFCMAP_PID_TO_32(pid)  \
   ((((buint8_t *)(pid))[0] << 16 )|\
    (((buint8_t *)(pid))[1] << 8  )|\
    (((buint8_t *)(pid))[2] << 0  ))

#define BFCMAP_PID_BUILD(pid, val32)\
   ((buint8_t *)(pid))[0] = ((val32) >> 16) & 0xff ;\
   ((buint8_t *)(pid))[1] = ((val32) >> 8 ) & 0xff ;\
   ((buint8_t *)(pid))[2] = ((val32) >> 0 ) & 0xff ;


typedef enum {
    BFCMAP_UC_FIRMWARE_INIT = 0xA,
    BFCMAP_UC_LINK_RESET,
    BFCMAP_UC_LINK_BOUNCE,
    BFCMAP_UC_LINK_ENABLE,
    BFCMAP_UC_LINK_SPEED,
    BFCMAP_UC_RX_BBC_SET
} bfcmap_lmi_uc_cmds_t ;

typedef enum {
    BFCMAP_UC_LINK_UP = 0x1A,
    BFCMAP_UC_LINK_FAILURE
} bfcmap_uc_lmi_cmds_t ;

#if defined(INCLUDE_PHY_8806X)
/* Inband packet handling */

#ifndef COMPILER_ATTRIBUTE
#define COMPILER_ATTRIBUTE(_a)    __attribute__ (_a)
#endif /* COMPILER_ATTRIBUTE */

typedef struct bfcmap_mibs_s {
    buint64_t fc_c2_rxgoodframes;
    buint64_t fc_c2_rxinvalidcrc;  
    buint64_t fc_c2_rxframetoolong;
    buint64_t fc_c3_rxgoodframes;
    buint64_t fc_c3_rxinvalidcrc;
    buint64_t fc_c3_rxframetoolong;
    buint64_t fc_cf_rxgoodframes;
    buint64_t fc_cf_rxinvalidcrc;
    buint64_t fc_cf_rxframetoolong;
    buint64_t fc_rxdelimitererr;
    buint64_t fc_rxbbc0drop;
    buint64_t fc_rxbbcredit0;
    buint64_t fc_rxlinkfail;
    buint64_t fc_rxsyncfail;
    buint64_t fc_rxlosssig;
    buint64_t fc_rxprimseqerr;
    buint64_t fc_rxinvalidset;
    buint64_t fc_rxlpicount;
    buint64_t fc_rxencodedisparity;
    buint64_t fc_rxbadxword;
    buint64_t fc_rx_c2_runtframes;
    buint64_t fc_rx_c3_runtframes;
    buint64_t fc_rx_cf_runtframes;
    buint64_t fc_c2_rxbyt;
    buint64_t fc_c3_rxbyt;
    buint64_t fc_cf_rxbyt;
    buint64_t fc_tx_c2_frames;
    buint64_t fc_tx_c3_frames;
    buint64_t fc_tx_cf_frames;
    buint64_t fc_tx_c2_oversized_frames;
    buint64_t fc_tx_c3_oversized_frames;
    buint64_t fc_tx_cf_oversized_frames;
    buint64_t fc_txbbcredit0;
    buint64_t fc_c2_txbyt;
    buint64_t fc_c3_txbyt;
    buint64_t fc_cf_txbyt;
} COMPILER_ATTRIBUTE((packed)) bfcmap_mibs_t;

/* The order of enums below MUST match the struct bfcmap_mibs_t */
typedef enum {
    stat_fc_c2_rxgoodframes,
    stat_fc_c2_rxinvalidcrc,  
    stat_fc_c2_rxframetoolong,
    stat_fc_c3_rxgoodframes,
    stat_fc_c3_rxinvalidcrc,
    stat_fc_c3_rxframetoolong,
    stat_fc_cf_rxgoodframes,
    stat_fc_cf_rxinvalidcrc,
    stat_fc_cf_rxframetoolong,
    stat_fc_rxdelimitererr,
    stat_fc_rxbbc0drop,
    stat_fc_rxbbcredit0,
    stat_fc_rxlinkfail,
    stat_fc_rxsyncfail,
    stat_fc_rxlosssig,
    stat_fc_rxprimseqerr,
    stat_fc_rxinvalidset,
    stat_fc_rxlpicount,
    stat_fc_rxencodedisparity,
    stat_fc_rxbadxword,
    stat_fc_rx_c2_runtframes,
    stat_fc_rx_c3_runtframes,
    stat_fc_rx_cf_runtframes,
    stat_fc_c2_rxbyt,
    stat_fc_c3_rxbyt,
    stat_fc_cf_rxbyt,
    stat_fc_tx_c2_frames,
    stat_fc_tx_c3_frames,
    stat_fc_tx_cf_frames,
    stat_fc_tx_c2_oversized_frames,
    stat_fc_tx_c3_oversized_frames,
    stat_fc_tx_cf_oversized_frames,
    stat_fc_txbbcredit0,
    stat_fc_c2_txbyt,
    stat_fc_c3_txbyt,
    stat_fc_cf_txbyt,
    stat_fc_max_stat /* should be last */
} bfcmap_stat_index_t;

#define FC_INBAND_FRAME_SIGNATURE 0xFC0E
#define FC_INBAND_NUM_TLV_MAX 0x3

/* Non byte FC counters are 40 bit wide */
#define FC_COUNTER_WIDTH 40
#define FC_COUNTER_MASK 0xFFFFFFFFFFULL /* 40 bits */

typedef struct bfcmap_inb_signature_s {
    buint16_t sign;
    buint8_t num_tlv;
    buint8_t frm_seq;
} COMPILER_ATTRIBUTE((packed)) bfcmap_inb_signature_t;

typedef enum bfcmap_inb_pld_type {
    FC_INB_PLD_TYPE_FC_MIBS    = 1,
    FC_INB_PLD_TYPE_FC_EVENTS  = 2,
    FC_INB_PLD_TYPE_ETH_MIBS   = 3
} bfcmap_inb_pld_type_t;

typedef struct bfcmap_inb_tlv_s {
    buint16_t pld_type;
    buint16_t pld_len;
} COMPILER_ATTRIBUTE((packed)) bfcmap_inb_tlv_t;
         
typedef struct bfcmap_inb_pl_s {
    bfcmap_inb_tlv_t tlv;
    buint64_t word[1];
} COMPILER_ATTRIBUTE((packed)) bfcmap_inb_pl_t;

/* Inband FC frame Payload */
typedef struct bfcmap_inband_fr_payload_s {
    bfcmap_inb_signature_t fc_inb_signature;
    /* payloads */
    bfcmap_inb_pl_t pl1;
    bfcmap_inb_pl_t pl2;
    bfcmap_inb_pl_t pl3;
} COMPILER_ATTRIBUTE((packed)) bfcmap_inband_fr_payload_t;

void bfcmap88060_fc_mibs_update(bfcmap_mibs_t *mibs, bfcmap_mibs_t *prev_mibs, buint64_t *cur_fc_mibs);
int bfcmap88060_fc_event_update(buint64_t *cur_fc_evt);
void bfcmap88060_fc_stat_update(buint64_t *dst_stat, buint64_t *cur_stat, buint64_t *prev_stat, int stat_index);
#endif /* INCLUDE_PHY_8806X */

#endif /* BFCMAP_INTERNAL_H */

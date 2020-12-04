/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       phy8806x_funcs.h
 */

#ifndef   _PHY8806X_CTR_H_
#define   _PHY8806X_CTR_H_

#include <sal/types.h>


/* Make sure this structure matches the matching structure in the MT2 Firmware */
typedef struct mt2_eth_ctr_s {
    uint64 r64;
    uint64 r127;
    uint64 r255;
    uint64 r511;
    uint64 r1023;
    uint64 r1518;
    uint64 rmgv;
    uint64 r2047;
    uint64 r4095;
    uint64 r9216;
    uint64 r16383;
    uint64 rpkt;
    uint64 ruca;
    uint64 rmca;
    uint64 rbca;
    uint64 rfcs;
    uint64 rxcf;
    uint64 rxpf;
    uint64 rxpp;
    uint64 rxuo;
    uint64 rxuda;
    uint64 rxwsa;
    uint64 raln;
    uint64 rflr;
    uint64 rerpkt;
    uint64 rfcr;
    uint64 rovr;
    uint64 rjbr;
    uint64 rmtue;
    uint64 rmcrc;
    uint64 rprm;
    uint64 rvln;
    uint64 rdvln;
    uint64 rtrfu;
    uint64 rpok;
    uint64 rpfcoff0;
    uint64 rpfcoff1;
    uint64 rpfcoff2;
    uint64 rpfcoff3;
    uint64 rpfcoff4;
    uint64 rpfcoff5;
    uint64 rpfcoff6;
    uint64 rpfcoff7;
    uint64 rpfc0;
    uint64 rpfc1;
    uint64 rpfc2;
    uint64 rpfc3;
    uint64 rpfc4;
    uint64 rpfc5;
    uint64 rpfc6;
    uint64 rpfc7;
    uint64 rschcrc;
    uint64 rund;
    uint64 rfrg;
    uint64 rx_eee_lpi_event_counter;
    uint64 rx_eee_lpi_duration_counter;
    uint64 rx_llfc_phy_counter;
    uint64 rx_llfc_log_counter;
    uint64 rx_llfc_crc_counter;
    uint64 rx_hcfc_counter;
    uint64 rbyt;
    uint64 rrbyt;
    uint64 rrpkt;
    uint64 t64;
    uint64 t127;
    uint64 t255;
    uint64 t511;
    uint64 t1023;
    uint64 t1518;
    uint64 tmgv;
    uint64 t2047;
    uint64 t4095;
    uint64 t9216;
    uint64 t16383;
    uint64 tpok;
    uint64 tpkt;
    uint64 tuca;
    uint64 tmca;
    uint64 tbca;
    uint64 txpf;
    uint64 txpp;
    uint64 tjbr;
    uint64 tfcs;
    uint64 txcf;
    uint64 tovr;
    uint64 tdfr;
    uint64 tedf;
    uint64 tscl;
    uint64 tmcl;
    uint64 tlcl;
    uint64 txcl;
    uint64 tfrg;
    uint64 terr;
    uint64 tvln;
    uint64 tdvln;
    uint64 trpkt;
    uint64 tufl;
    uint64 tpfc0;
    uint64 tpfc1;
    uint64 tpfc2;
    uint64 tpfc3;
    uint64 tpfc4;
    uint64 tpfc5;
    uint64 tpfc6;
    uint64 tpfc7;
    uint64 tx_eee_lpi_event_counter;
    uint64 tx_eee_lpi_duration_counter;
    /* uint64 tx_llfc_phy_counter; */
    uint64 tx_llfc_log_counter;
    uint64 tx_hcfc_counter;
    uint64 tncl;
    uint64 tbyt;
} mt2_eth_ctr_t;

#ifdef INCLUDE_FCMAP
/* Make sure this structure matches the matching structure in the MT2 Firmware */
typedef struct mt2_fc_ctr_s {
    uint64 fc_c2_rxgoodframes;
    uint64 fc_c2_rxinvalidcrc;
    uint64 fc_c2_rxframetoolong;
    uint64 fc_c3_rxgoodframes;
    uint64 fc_c3_rxinvalidcrc;
    uint64 fc_c3_rxframetoolong;
    uint64 fc_cf_rxgoodframes;
    uint64 fc_cf_rxinvalidcrc;
    uint64 fc_cf_rxframetoolong;
    uint64 fc_rxdelimitererr;
    uint64 fc_rxbbc0drop;
    uint64 fc_rxbbcredit0;
    uint64 fc_rxlinkfail;
    uint64 fc_rxsyncfail;
    uint64 fc_rxlosssig;
    uint64 fc_rxprimseqerr;
    uint64 fc_rxinvalidset;
    uint64 fc_rxlpicount;
    uint64 fc_rxencodedisparity;
    uint64 fc_rxbadxword;
    uint64 fc_rx_c2_runtframes;
    uint64 fc_rx_c3_runtframes;
    uint64 fc_rx_cf_runtframes;
    uint64 fc_c2_rxbyt;
    uint64 fc_c3_rxbyt;
    uint64 fc_cf_rxbyt;
    uint64 fc_tx_c2_frames;
    uint64 fc_tx_c3_frames;
    uint64 fc_tx_cf_frames;
    uint64 fc_tx_c2_oversized_frames;
    uint64 fc_tx_c3_oversized_frames;
    uint64 fc_tx_cf_oversized_frames;
    uint64 fc_txbbcredit0;
    uint64 fc_c2_txbyt;
    uint64 fc_c3_txbyt;
    uint64 fc_cf_txbyt;
} mt2_fc_ctr_t;
#endif /* INCLUDE_FCMAP */

#define MT2_CTR_ALL         0x80
#define MT2_CTR_SYS         0x01
#define MT2_CTR_LINE        0x02
#define MT2_CTR_CHANGED     0x04
#define MT2_CTR_SAME        0x08
#define MT2_CTR_ZERO        0x10
#define MT2_CTR_NONZERO     0x20
#define MT2_CTR_HEX         0x40

#define MT2_CTR_DEFAULT     (MT2_CTR_SYS | MT2_CTR_LINE | MT2_CTR_CHANGED | MT2_CTR_ZERO | MT2_CTR_NONZERO)

extern int phy8806x_ctr_start(int unit);
extern int phy8806x_ctr_stop(int unit);
extern int phy8806x_ctr_show(phy_ctrl_t *pc, uint32_t flags);
extern int phy8806x_ctr_interval_set(phy_ctrl_t *pc, uint32_t val);

#endif  /* _PHY8806X_CTR_H_ */


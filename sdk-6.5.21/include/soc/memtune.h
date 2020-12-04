/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        memtune.h
 * Purpose:     External memory interface tuning routines.
 */

#ifndef   _SOC_MEMTUNE_H_
#define   _SOC_MEMTUNE_H_

#include <soc/register.h>

/* Top-level routine */
typedef enum soc_mem_interfaces_e {
    SOC_MEM_INTERFACE_RLDRAM_CH0,
    SOC_MEM_INTERFACE_RLDRAM_CH1,
    SOC_MEM_INTERFACE_SRAM,
    SOC_MEM_INTERFACE_QDR,
    SOC_MEM_INTERFACE_TCAM,
    SOC_MEM_INTERFACE_COUNT
} soc_mem_interfaces;

typedef struct soc_memtune_data_s {
    struct soc_memtune_ctrl_s *mt_ctrl;
    int                    interface;
    int                    sub_interface;
    int                    rseed;
    int                    config;
    int                    verbose;
    int                    suppress_fail;
    int                    summary;
    int                    summary_header;
    int                    show_setting;
    int                    show_matrix;
    int                    show_progress;
    int                    test_count;
    int                    max_fail_count;
    uint32                 slice_mask;
    int                    do_pvt_comp;
    int                    freq;
    int                    phase_sel_ovrd;
    int                    test_all_latency;
    int                    do_txrx_first;
    uint32                 d0r_0;
    uint32                 d0r_1;
    uint32                 d0f_0;
    uint32                 d0f_1;
    uint32                 d1r_0;
    uint32                 d1r_1;
    uint32                 d1f_0;
    uint32                 d1f_1;
    int                    adr0;
    int                    adr1;
    int                    adr_mode;
    uint32                 mask[8];
    uint32                 data[8];
    int                    tcam_loop_count;
    int                    bg_tcam_bist;
    int                    bg_tcam_loop_count;
    uint32                 bg_tcam_data[32];
    uint32                 bg_tcam_oemap;
    int                    bg_sram_bist;
    uint32                 bg_d0r_0;
    uint32                 bg_d0r_1;
    uint32                 bg_d0f_0;
    uint32                 bg_d0f_1;
    uint32                 bg_d1r_0;
    uint32                 bg_d1r_1;
    uint32                 bg_d1f_0;
    uint32                 bg_d1f_1;
    int                    bg_adr0;
    int                    bg_adr1;
    int                    bg_loop_mode;
    int                    bg_adr_mode;
    int                    manual_settings; /* =1 if manual settings */
    int                    man_phase_sel;   /* user specified value */
    int                    man_em_latency;  /* user specified value */
    int                    man_ddr_latency; /* user specified value */
    int                    man_tx_offset;   /* user specified value */
    int                    man_rx_offset;   /* user specified value */
    int                    man_w2r_nops;    /* user specified value */
    int                    man_r2w_nops;    /* user specified value */

    /* for extt2 */
    int                    loop_mode[2];
    int                    alt_adr;
    int                    tcam_data_choice;
    int                    sram_data_choice;
    int                    delta[2];
    int                    tx_offset[2];
    int                    rx_offset[2];
    int                    *fail_array[2];
    int                    ok_on_both_pass;
    int                    restore_txrx_after_test;
} soc_memtune_data_t;

/* Saved tx/rx matrix analysis results for a specific setting */
typedef struct soc_memtune_result_s {
    int          width;
    int          height;
    int          phase_sel;
    int          em_latency;
    int          ddr_latency;
    int          tx_offset;
    int          rx_offset;
    int          invert_txclk;
    int          invert_rxclk;
    int          dpeo_sync_dly;
    int          fcd_dpeo;
    int          rbus_sync_dly;
    int          fcd_rbus;
    int          fail_count;
} soc_memtune_result_t;

#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME      0x0001
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY        0x0002
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER 0x0004
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX    0x0008
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS       0x0010
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION      0x0020
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL           0x0040
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS           0x0080
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_NOPS           0x0100
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_PROGRESS       0x0200
#define SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING        0x0400

typedef struct soc_memtune_ctrl_s {
    void                    *data;
    void                    *sram_bist;
    int                     unit;
    char                    *intf_name;
    uint32                  flags;
    int                     (*prog_hw1_fn)(soc_memtune_data_t *);
    int                     (*prog_hw2_fn)(soc_memtune_data_t *);
    int                     (*prog_hw3_fn)(soc_memtune_data_t *);
    int                     (*prog_hw4_fn)(soc_memtune_data_t *);
    int                     (*per_tx_fn)(soc_memtune_data_t *);
    int                     (*per_rx_fn)(soc_memtune_data_t *);
    int                     (*test_fn)(soc_memtune_data_t *);

    /* for both SRAM and TCAM tuning */
    int                     tx_offset_cur;
    int                     tx_offset_min;
    int                     tx_offset_max;
    int                     rx_offset_cur;
    int                     rx_offset_min;
    int                     rx_offset_max;

    /* for SRAM tuning */
    int                     phase_sel_cur;
    int                     phase_sel_min;
    int                     phase_sel_max;
    int                     em_latency_cur;
    int                     em_latency_min;
    int                     em_latency_max;
    int                     ddr_latency_cur;
    int                     ddr_latency_min;
    int                     ddr_latency_max;
    int                     w2r_nops_cur;
    int                     w2r_nops_min;
    int                     w2r_nops_max;
    int                     r2w_nops_cur;
    int                     r2w_nops_min;
    int                     r2w_nops_max;

    /* for TCAM tuning */
    int                     invert_txclk_cur;
    int                     invert_txclk_min;
    int                     invert_txclk_max;
    int                     invert_rxclk_cur;
    int                     invert_rxclk_min;
    int                     invert_rxclk_max;

    /* for TCAM DBUS_out (DPEO) tuning */
    int                     dpeo_sel_cur;
    int                     dpeo_sync_dly_cur;
    int                     dpeo_sync_dly_min;
    int                     dpeo_sync_dly_max;
    int                     fcd_dpeo_cur;
    int                     fcd_dpeo_min;
    int                     fcd_dpeo_max;

    /* for TCAM RBUS tuning */
    int                     rbus_sync_dly_cur;
    int                     rbus_sync_dly_min;
    int                     rbus_sync_dly_max;
    int                     fcd_rbus_cur;
    int                     fcd_rbus_min;
    int                     fcd_rbus_max;
    int                     do_setup_for_rbus_test;

    /* for both SRAM and TCAM tuning */
    int                     dac_value;
    int                     ptr_dist;
    int                     odtres_val;
    int                     pdrive_val;
    int                     ndrive_val;
    int                     slew_val;

    int                     tx_pass_threshold;
    int                     rx_pass_threshold;
    int                     bist_poll_count;
    int                     cur_fail_count;
    uint32                  cur_fail_reason;
    uint32                  cur_fail_detail0;
    uint32                  cur_fail_detail1;
    int                     *fail_array;
    int                     result_count;
    soc_memtune_result_t    result[48];
    soc_memtune_result_t    *ps_data[4];
} soc_memtune_ctrl_t;

extern int soc_mem_interface_tune(int unit, soc_memtune_data_t *mt_data);
extern int soc_mem_interface_tune_lvl2(int unit, soc_memtune_data_t *mt_data);

/* Device specific implementations */

typedef struct soc_er_memtune_data_s {
    soc_reg_t    ddr_reg1;
    soc_reg_t    ddr_reg2;
    soc_reg_t    ddr_reg3;
    soc_reg_t    timing_reg;
    soc_reg_t    req_cmd;
    soc_reg_t    bist_ctrl;
    soc_mem_t    write_mem;
    int          channel;
} soc_er_memtune_data_t;

typedef struct soc_tr_memtune_data_s {
    soc_reg_t    config_reg1;
    soc_reg_t    config_reg2;
    soc_reg_t    config_reg3;
    soc_reg_t    status_reg2;
    soc_reg_t    mode;
    soc_reg_t    tmode0;
    soc_reg_t    tmode0_other_sram;
    soc_reg_t    sram_ctl;
    soc_reg_t    etc_ctl;
    soc_reg_t    inst_status;
    uint32       dbus_fail_mask;
    uint32       rbus_fail_mask;
    uint32       rbus_valid_mask;
} soc_tr_memtune_data_t;

#define SOC_ER_PHASE_SEL_MIN            0
#define SOC_ER_PHASE_SEL_MAX            3
#define SOC_ER_EM_LATENCY_MIN           7
#define SOC_ER_EM_LATENCY_MAX           8
#define SOC_ER_SEER_DDR_LATENCY_MIN     0
#define SOC_ER_RLDRAM_DDR_LATENCY_MIN   1
#define SOC_ER_DDR_LATENCY_MAX          2
#define SOC_ER_RLDRAM_TX_OFFSET_MIN     0
#define SOC_ER_RLDRAM_TX_OFFSET_MAX     7
#define SOC_ER_RLDRAM_RX_OFFSET_MIN     0
#define SOC_ER_RLDRAM_RX_OFFSET_MAX     7
#define SOC_ER_DDR_BIST_COUNT           10
#define SOC_ER_RLDRAM_BIST_POLL_COUNT   1000
#define SOC_ER_RLDRAM_BIST_POLL_INTERVAL 100
#define SOC_ER_RLDRAM_SLICE_MASK        0xf
#define SOC_ER_SRAM_SLICE_MASK          0xf
#define SOC_ER_QDR_SLICE_MASK           0x3

/* Triumph: SRAM */
#define SOC_TR_PHASE_SEL_MIN            0
#define SOC_TR_PHASE_SEL_MAX            3
#define SOC_TR_EM_LATENCY_MIN           4 /* skip 0, 1, 2, 3 to save time */
#define SOC_TR_EM_LATENCY_MAX           8
#define SOC_TR_DDR_LATENCY_MIN          0
#define SOC_TR_DDR_LATENCY_MAX          2
#define SOC_TR_SRAM_TX_OFFSET_MIN       0
#define SOC_TR_SRAM_TX_OFFSET_MAX       28
#define SOC_TR_SRAM_RX_OFFSET_MIN       0
#define SOC_TR_SRAM_RX_OFFSET_MAX       28
#define SOC_TR_SRAM_W2R_NOPS_MIN        0
#define SOC_TR_SRAM_W2R_NOPS_MAX        0
#define SOC_TR_SRAM_R2W_NOPS_MIN        3
#define SOC_TR_SRAM_R2W_NOPS_MAX        3
#define SOC_TR_SRAM_SLICE_MASK          0x3

/* Triumph: TCAM */
#define SOC_TR_TCAM_INVERT_TXCLK_MIN    0
#define SOC_TR_TCAM_INVERT_TXCLK_MAX    1
#define SOC_TR_TCAM_INVERT_RXCLK_MIN    0
#define SOC_TR_TCAM_INVERT_RXCLK_MAX    1
#define SOC_TR_TCAM_TX_OFFSET_MIN       0
#define SOC_TR_TCAM_VCDL_TX_OFFSET_MAX  28 /* analog clock source */
#define SOC_TR_TCAM_MIDL_TX_OFFSET_MAX  39 /* digital clock source */
#define SOC_TR_TCAM_RX_OFFSET_MIN       0
#define SOC_TR_TCAM_VCDL_RX_OFFSET_MAX  28 /* analog clock source */
#define SOC_TR_TCAM_MIDL_RX_OFFSET_MAX  39 /* digital clock source */

/* Triumph: TCAM DBUS_out (DPEO) */
#define SOC_TR_DPEO_SYNC_DLY_MIN        0
#define SOC_TR_DPEO_SYNC_DLY_MAX        31
#define SOC_TR_FCD_DPEO_MIN             0
#define SOC_TR_FCD_DPEO_MAX             1

/* Triumph: TCAM RBUS */
#define SOC_TR_RBUS_SYNC_DLY_MIN        1  /* 0 means 32 */
#define SOC_TR_RBUS_SYNC_DLY_MAX        31
#define SOC_TR_FCD_RBUS_MIN             0
#define SOC_TR_FCD_RBUS_MAX             1  /* only use 0 and 1 */


/* Triumph: config variable encoding for both SRAM and TCAM tuning result */
#define SOC_TR_MEMTUNE_CONFIG_VALID_MASK      0x1

/* Triumph: config variable encoding for SRAM tuning result */
#define SOC_TR_MEMTUNE_OVRD_SM_MASK           0x1
#define SOC_TR_MEMTUNE_OVRD_SM_SHIFT          1
#define SOC_TR_MEMTUNE_PHASE_SEL_MASK         0x3
#define SOC_TR_MEMTUNE_PHASE_SEL_SHIFT        2
#define SOC_TR_MEMTUNE_EM_LATENCY_MASK        0x7
#define SOC_TR_MEMTUNE_EM_LATENCY_SHIFT       4
#define SOC_TR_MEMTUNE_DDR_LATENCY_MASK       0x3
#define SOC_TR_MEMTUNE_DDR_LATENCY_SHIFT      7
#define SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK     0x1f
#define SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT    9
#define SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK     0x1f
#define SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT    14
#define SOC_TR_MEMTUNE_W2R_NOPS_MASK          0x3
#define SOC_TR_MEMTUNE_W2R_NOPS_SHIFT         19
#define SOC_TR_MEMTUNE_R2W_NOPS_MASK          0x3
#define SOC_TR_MEMTUNE_R2W_NOPS_SHIFT         21
#define SOC_TR_MEMTUNE_DDR_FREQ_MASK          0x1ff
#define SOC_TR_MEMTUNE_DDR_FREQ_SHIFT         23

/* Triumph: config variable encoding for TCAM tuning result */
#define SOC_TR_MEMTUNE_RBUS_SYNC_DLY_MASK     0x1f
#define SOC_TR_MEMTUNE_RBUS_SYNC_DLY_SHIFT    8
#define SOC_TR_MEMTUNE_DPEO_SYNC_DLY_MASK     0x1f
#define SOC_TR_MEMTUNE_DPEO_SYNC_DLY_SHIFT    13
#define SOC_TR_MEMTUNE_FCD_DPEO_MASK          0x1
#define SOC_TR_MEMTUNE_FCD_DPEO_SHIFT         18
#define SOC_TR_MEMTUNE_FCD_RBUS_MASK          0x3
#define SOC_TR_MEMTUNE_FCD_RBUS_SHIFT         22
#define SOC_TR_MEMTUNE_TCAM_TX_OFFSET_MASK    0x3f
#define SOC_TR_MEMTUNE_TCAM_TX_OFFSET_SHIFT   1
#define SOC_TR_MEMTUNE_TCAM_RX_OFFSET_MASK    0x3f
#define SOC_TR_MEMTUNE_TCAM_RX_OFFSET_SHIFT   7
#define SOC_TR_MEMTUNE_TCAM_FREQ_MASK         0x3ff
#define SOC_TR_MEMTUNE_TCAM_FREQ_SHIFT        13
#define SOC_TR_MEMTUNE_USE_MIDL_MASK          0x1
#define SOC_TR_MEMTUNE_USE_MIDL_SHIFT         23
#define SOC_TR_MEMTUNE_INVERT_TXCLK_MASK      0x1
#define SOC_TR_MEMTUNE_INVERT_TXCLK_SHIFT     24
#define SOC_TR_MEMTUNE_INVERT_RXCLK_MASK      0x1
#define SOC_TR_MEMTUNE_INVERT_RXCLK_SHIFT     25

/* Triumph: config variable encoding for PVT result */
#define SOC_TR_MEMTUNE_OVRD_ODTRES_MASK       0x1
#define SOC_TR_MEMTUNE_OVRD_ODTRES_SHIFT      1
#define SOC_TR_MEMTUNE_ODTRES_MASK            0xf
#define SOC_TR_MEMTUNE_ODTRES_SHIFT           2
#define SOC_TR_MEMTUNE_OVRD_DRIVER_MASK       0x1
#define SOC_TR_MEMTUNE_OVRD_DRIVER_SHIFT      6
#define SOC_TR_MEMTUNE_PDRIVE_MASK            0xf
#define SOC_TR_MEMTUNE_PDRIVE_SHIFT           7
#define SOC_TR_MEMTUNE_NDRIVE_MASK            0xf
#define SOC_TR_MEMTUNE_NDRIVE_SHIFT           11
#define SOC_TR_MEMTUNE_OVRD_SLEW_MASK         0x1
#define SOC_TR_MEMTUNE_OVRD_SLEW_SHIFT        15
#define SOC_TR_MEMTUNE_SLEW_MASK              0xf
#define SOC_TR_MEMTUNE_SLEW_SHIFT             16

/* Triumph: config variable encoding for tuning stats */
#define SOC_TR_MEMTUNE_STATS_WIDTH_MASK       0x3f
#define SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT      1
#define SOC_TR_MEMTUNE_STATS_HEIGHT_MASK      0x3f
#define SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT     7
#define SOC_TR_MEMTUNE_STATS_FAIL_MASK        0x3ff
#define SOC_TR_MEMTUNE_STATS_FAIL_SHIFT       13

#endif /* _SOC_MEMTUNE_H_ */

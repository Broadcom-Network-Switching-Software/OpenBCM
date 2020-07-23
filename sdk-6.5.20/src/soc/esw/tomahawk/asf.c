/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      asf.c
 * Purpose:   ASF feature support for Tomahawk SKUs
 * Requires:  soc/tomahawk.h
 */

#include <soc/tomahawk.h>

#if defined(BCM_TOMAHAWK_SUPPORT)

#include <appl/diag/system.h>
#include <soc/drv.h>
#include <soc/ll.h>
#include <soc/property.h>
#include <shared/bsl.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif
#include <bcm/link.h>
#include <soc/esw/portctrl.h>

/* Linkscan/PHY control Declarations */
extern int bcm_esw_linkscan_register(int, bcm_linkscan_handler_t);
extern int bcm_esw_linkscan_unregister(int, bcm_linkscan_handler_t);
extern int bcm_esw_port_phy_control_get(int,bcm_port_t,bcm_port_phy_control_t,uint32 *);

#define _SOC_TH_ASF_QUERY   0xfe
#define _SOC_TH_ASF_RETRV   0xff

#define _SOC_TH_CT_CLASS_TO_SPEED_MAP(ct_class) \
        _soc_th_asf_cfg_tbl[(ct_class)].speed

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define _SOC_TH_PORT_ASF_XMIT_START_COUNT_INIT \
        soc_th_port_asf_xmit_start_count_set

#define _SOC_TH_PORT_ASF_REINIT \
        soc_th_port_asf_init

#define _SOC_TH_ASF_SPEED_CLASS_VALIDATE(class, floor) \
        if (!(((class) >= (floor)) && ((class) <= 12)))   \
            return SOC_E_PARAM;

#define _SOC_TH_ASF_PORT_VALIDATE(unit, port) \
        if (IS_CPU_PORT(((unit)), (port)) || IS_LB_PORT(((unit)), (port)) || \
            SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), (port))) \
            return SOC_E_UNAVAIL;


/* EP credit accumulation */
typedef struct _soc_th_ep_credit_acc_s {
    uint8 line_rate;       /* Line-rate profile */
    uint8 oversub;         /* General profile */
} _soc_th_ep_credit_acc_t;

/* MMU EP Credit Thresholds */
typedef struct _soc_th_mmu_ep_credit_s {
    uint8 base_cells;      /* Baseline cells */
    uint8 pfc_op_cells;    /* PFC Optimized cells */
} _soc_th_mmu_ep_credit_t;

/* EP Credits Outstanding */
typedef struct _soc_th_egr_mmu_cell_credit_s {
    uint8 base_cells;      /* Baseline cells */
    uint8 pfc_op_cells;    /* PFC Optimized cells */
} _soc_th_egr_mmu_cell_credit_t;

/* Transmit Start Count */
typedef struct _soc_th_egr_xmit_start_count_s {
    uint8 saf;             /* Store-And-Forward */
    uint8 oversub_2_1;     /* 2:1 Oversub */
    uint8 oversub_3_2;     /* 3:2 Oversub */
    uint8 line_rate;       /* Line-rate */
} _soc_th_egr_xmit_start_count_t;

/* ASF core config */
typedef struct _soc_th_asf_core_cfg_s {
    int                              speed;
    uint8                            min_sp;                   /* Min. Src. Port Speed */
    uint8                            max_sp;                   /* Max. Src. Port Speed */
    uint8                            min_src_port_xmit_cnt;    /* MMU prebuffer */
    _soc_th_ep_credit_acc_t          ep_credit_acc;            /* EP Credit Accumulation */
    uint8                            encap_acc;                /* Encap Accumulation */
    uint8                            mmu_prebuf_depth;         /* Extra MMU Prebuffer depth */
    uint8                            fast_to_slow_acc;         /* Fast to slow CT Accumulation */
    _soc_th_mmu_ep_credit_t          mmu_ep_credit;            /* MMU EP credit thresholds */
    _soc_th_egr_mmu_cell_credit_t    egr_mmu_credit;           /* EGR MMU cell credits */
    _soc_th_egr_xmit_start_count_t   xmit_cnt_l2_latency;      /* L2 Latency Mode */
    _soc_th_egr_xmit_start_count_t   xmit_cnt_l3_full_latency; /* L3 & Full Latency Modes */
    /* NOTE: DON'T ALTER FIELD ORDER */
} _soc_th_asf_core_cfg_t;
#ifdef BCM_TOMAHAWK2_SUPPORT
/* CT FIFO Threshold and Depth */
typedef struct _soc_th2_ct_fifo_profile_s {
    uint8 line_rate;       /* Line-rate profile */
    uint8 oversub;         /* General profile */
} _soc_th2_ct_fifo_profile_t;
typedef struct _soc_th2_ct_fifo_s {
    _soc_th2_ct_fifo_profile_t ss;      /* Same-Speed */
    _soc_th2_ct_fifo_profile_t sf;      /* Slow-to-Fast */
    _soc_th2_ct_fifo_profile_t fs;      /* Fast-to-Slow */
}_soc_th2_ct_fifo_t;

/* Bubble MOP */
typedef struct _soc_th2_mop_s {
    uint8 line_rate[SOC_SWITCH_BYPASS_MODE_LOW + 1];           /* Line-rate profile */
    uint8 oversub[SOC_SWITCH_BYPASS_MODE_LOW + 1];             /* Oversub profile */
} _soc_th2_mop_t;

/* EDB Prebuffer */
typedef struct _soc_th2_edb_prebuf_profile_s {
    uint8 saf;                                  /* prebuffer for store-and-forward */
    uint8 ct[SOC_SWITCH_BYPASS_MODE_LOW + 1];   /* prebuffer for cut-through, latency none/L3/L2 */
} _soc_th2_edb_prebuf_profile_t;
typedef struct _soc_th2_edb_prebuf_s {
    _soc_th2_edb_prebuf_profile_t line_rate;
    _soc_th2_edb_prebuf_profile_t oversub_3_2;                 /* 3:2 Oversub */
    _soc_th2_edb_prebuf_profile_t oversub_2_1;                 /* 2:1 Oversub */
} _soc_th2_edb_prebuf_t;

/* MMU Credit Thresholds */
typedef struct _soc_th2_mmu_credit_threshold_profile_s {
    uint8 lo;              /* MMU High Credit Threshold */
    uint8 hi;              /* MMU Low Credit Threshold */
} _soc_th2_mmu_credit_threshold_profile_t;
typedef struct _soc_th2_mmu_credit_threshold_s {
    _soc_th2_mmu_credit_threshold_profile_t lr_os_1_5;         /* line-rate and os ratio <= 3:2 */
    _soc_th2_mmu_credit_threshold_profile_t os_2_1;            /* os ratio > 3:2 */
} _soc_th2_mmu_credit_threshold_t;

/* IFSAF Threshold */
typedef struct _soc_th2_ifsaf_threshold_s {
    uint8 ct;
    uint8 saf;
}_soc_th2_ifsaf_threshold_t;

/* ASF core config */
typedef struct _soc_th2_asf_core_cfg_s {
    int                              speed;
    uint8                            min_sp;                   /* Min. Src. Port Speed */
    uint8                            max_sp;                   /* Max. Src. Port Speed */
    uint8                            mmu_prebuffer;            /* MMU prebuffer */
    _soc_th2_ct_fifo_t               ct_fifo_threshold;        /* Same-speed CT FIFO Threshold */
    _soc_th2_ct_fifo_t               ct_fifo_depth;            /* Same-speed CT FIFO Depth */
    uint8                            fast_to_slow_acc;         /* Fast to slow CT Accumulation */
    _soc_th2_mop_t                   mop_enable;               /* Bubble MOP enable */
    _soc_th2_edb_prebuf_t            edb_prebuffer;            /* EDB prebuffer */
    _soc_th2_mmu_credit_threshold_t  mmu_credit_threshold;     /* MMU Credit Threshold */
    _soc_th_egr_mmu_cell_credit_t    ep_credit;                /* EP Credit */
    _soc_th2_ifsaf_threshold_t       ifsaf_threshold;          /* IFSAF Threshold */
    /* NOTE: DON'T ALTER FIELD ORDER */
} _soc_th2_asf_core_cfg_t;
#endif

/* ASF ctrl */
typedef struct _soc_th_asf_ctrl_s {
    uint8                     init;
    pbmp_t                    asf_ports;
    pbmp_t                    pause_restore;
    pbmp_t                    asf_ss;
    pbmp_t                    asf_sf;
    pbmp_t                    asf_fs;
    soc_th_asf_mem_profile_e  asf_mem_profile;
    int                       latency;
} _soc_th_asf_ctrl_t;

#ifdef BCM_WARM_BOOT_SUPPORT
/* ASF Warmboot */
typedef struct _soc_th_asf_wb_s {
    int unit;
    _soc_th_asf_ctrl_t asf_ctrl;
} _soc_th_asf_wb_t;
#endif

/* ASF Core Config Table  */
static const _soc_th_asf_core_cfg_t
_soc_th_asf_cfg_tbl[] = {
    {    -1,  1,  1,  0, {0,  0}, 0,  0, 0,  { 0,  0}, { 0,   0}, {18,  0,  0,  0}, {18,  0,  0,  0}},   /* SAF     */
    { 10000,  1, 10,  6, {2,  8}, 6, 10, 38, { 9,  8}, {13,  11}, {18, 51, 42, 30}, {18, 60, 48, 27}},   /* 10GE    */
    { 11000,  1, 10,  6, {2,  8}, 6, 10, 38, { 9,  8}, {13,  11}, {18, 51, 42, 30}, {18, 60, 48, 27}},   /* HG[11]  */
    { 20000,  3, 10,  7, {2,  8}, 7, 11, 29, {13, 12}, {18,  16}, {18, 63, 48, 30}, {18, 54, 42, 21}},   /* 20GE    */
    { 21000,  3, 10,  7, {2,  8}, 7, 11, 29, {13, 12}, {18,  16}, {18, 63, 48, 30}, {18, 54, 42, 21}},   /* HG[21]  */
    { 25000,  5, 10,  7, {3,  8}, 6, 11, 25, {12, 11}, {16,  15}, {18, 60, 48, 27}, {18, 51, 36, 18}},   /* 25GE    */
    { 27000,  5, 10,  7, {3,  8}, 6, 11, 25, {12, 11}, {16,  15}, {18, 60, 48, 27}, {18, 51, 36, 18}},   /* HG[27]  */
    { 40000,  7, 12,  7, {3, 12}, 8, 11, 29, {18, 14}, {25,  19}, {18, 57, 45, 24}, {18, 42, 30, 12}},   /* 40GE    */
    { 42000,  7, 12,  7, {3, 12}, 8, 11, 29, {18, 14}, {25,  19}, {18, 57, 45, 24}, {18, 42, 30, 12}},   /* HG[42]  */
    { 50000,  7, 12, 20, {1, 12}, 8, 31, 25, {20, 17}, {27,  23}, {18, 54, 42, 21}, {18, 33, 27, 12}},   /* 50GE    */
    { 53000,  7, 12,  7, {1, 12}, 8, 11, 25, {20, 17}, {27,  23}, {18, 54, 42, 21}, {18, 33, 27, 12}},   /* HG[53]  */
    {100000, 11, 12,  6, {3,  3}, 8, 10,  3, {33, 27}, {44,  36}, {18, 48, 36, 18}, {18, 33, 27, 15}},   /* 100GE   */
    {106000, 11, 12,  6, {3,  3}, 8, 10,  3, {33, 27}, {44,  36}, {18, 48, 36, 18}, {18, 33, 27, 15}}    /* HG[106] */
};
#ifdef BCM_TOMAHAWK2_SUPPORT
static const _soc_th2_asf_core_cfg_t
_soc_th2_asf_cfg_tbl[] = {
    {    -1,  0,  0,  0, {{0,  0}, { 0,  0}, {0,  0}}, {{0,  0}, { 0,  0}, { 0,  0}},  0, {{0, 0, 0}, {0, 0, 0}}, {{0, { 0,  0,  0}}, { 0, { 0,  0,  0}}, { 0, { 0,  0,  0}}}, {{0,  0}, { 0,  0}}, {10,  8}, {12, 12}},   /* SAF     */
    { 10000,  1, 10,  0, {{3, 10}, { 9, 16}, {3, 10}}, {{8, 19}, {14, 25}, {46, 57}}, 38, {{0, 0, 0}, {0, 0, 0}}, {{7, {29, 29, 29}}, {12, {40, 40, 40}}, {16, {48, 48, 48}}}, {{1,  5}, { 1,  7}}, {10,  9}, {12, 12}},   /* 10GE    */
    { 11000,  1, 10,  6, {{3, 10}, { 9, 16}, {3, 10}}, {{8, 19}, {14, 25}, {46, 57}}, 38, {{0, 0, 0}, {0, 0, 0}}, {{7, {29, 29, 29}}, {12, {40, 40, 40}}, {16, {48, 48, 48}}}, {{1,  5}, { 1,  7}}, {10,  9}, {12, 12}},   /* HG[11]  */
    { 20000,  3, 10,  0, {{3, 10}, {10, 17}, {3, 10}}, {{8, 22}, {15, 26}, {37, 51}}, 29, {{1, 1, 0}, {0, 0, 0}}, {{7, {28, 35, 30}}, {12, {41, 41, 41}}, {16, {49, 49, 49}}}, {{1,  7}, { 3, 12}}, {17, 11}, {12, 12}},   /* 20GE    */
    { 21000,  3, 10,  7, {{3, 10}, {10, 17}, {3, 10}}, {{8, 22}, {15, 26}, {37, 51}}, 29, {{1, 1, 0}, {0, 0, 0}}, {{7, {28, 35, 30}}, {12, {41, 41, 41}}, {16, {49, 49, 49}}}, {{1,  7}, { 3, 12}}, {17, 11}, {12, 12}},   /* HG[21]  */
    { 25000,  5, 10,  0, {{3, 10}, {10, 17}, {3, 10}}, {{8, 23}, {15, 26}, {33, 48}}, 25, {{1, 1, 0}, {1, 0, 0}}, {{7, {25, 35, 30}}, {12, {47, 41, 41}}, {16, {56, 49, 49}}}, {{2,  8}, { 3, 12}}, {17, 13}, {18, 18}},   /* 25GE    */
    { 27000,  5, 10,  7, {{3, 10}, {10, 17}, {3, 10}}, {{8, 23}, {15, 26}, {33, 48}}, 25, {{1, 1, 0}, {1, 0, 0}}, {{7, {25, 35, 30}}, {12, {47, 41, 41}}, {16, {56, 49, 49}}}, {{2,  8}, { 3, 12}}, {17, 13}, {18, 18}},   /* HG[27]  */
    { 40000,  7, 12,  0, {{3, 10}, {10, 17}, {3, 10}}, {{8, 25}, {15, 26}, {37, 54}}, 29, {{1, 1, 1}, {1, 1, 0}}, {{7, {13, 18, 38}}, {12, {52, 52, 44}}, {16, {44, 62, 52}}}, {{3, 12}, { 5, 21}}, {26, 17}, {12, 12}},   /* 40GE    */
    { 42000,  7, 12,  7, {{3, 10}, {10, 17}, {3, 10}}, {{8, 25}, {15, 26}, {37, 54}}, 29, {{1, 1, 1}, {1, 1, 0}}, {{7, {13, 18, 38}}, {12, {52, 52, 44}}, {16, {44, 62, 52}}}, {{3, 12}, { 5, 21}}, {26, 17}, {12, 12}},   /* HG[42]  */
    { 50000,  9, 12,  0, {{3, 10}, {10, 17}, {3, 10}}, {{8, 27}, {15, 26}, {33, 52}}, 25, {{1, 1, 1}, {1, 1, 1}}, {{7, {13, 14, 38}}, {12, {24, 52, 50}}, {16, {39, 62, 59}}}, {{3, 14}, { 6, 22}}, {28, 19}, {18, 18}},   /* 50GE    */
    { 53000,  9, 12,  7, {{3, 10}, {10, 17}, {3, 10}}, {{8, 27}, {15, 26}, {33, 52}}, 25, {{1, 1, 1}, {1, 1, 1}}, {{7, {13, 14, 38}}, {12, {24, 52, 50}}, {16, {39, 62, 59}}}, {{3, 14}, { 6, 22}}, {28, 19}, {18, 18}},   /* HG[53]  */
    {100000, 11, 12,  0, {{3, 10}, {11, 18}, {3, 10}}, {{8, 34}, {16, 27}, {11, 37}},  3, {{1, 1, 1}, {1, 1, 1}}, {{7, {13, 13, 28}}, {12, {24, 34, 64}}, {16, {31, 34, 74}}}, {{7, 25}, {11, 32}}, {48, 31}, {18, 18}},   /* 100GE   */
    {106000, 11, 12,  8, {{3, 10}, {11, 18}, {3, 10}}, {{8, 34}, {16, 27}, {11, 37}},  0, {{1, 1, 1}, {1, 1, 1}}, {{7, {13, 13, 28}}, {12, {24, 34, 64}}, {16, {31, 34, 74}}}, {{7, 25}, {11, 32}}, {48, 31}, {18, 18}}    /* HG[106] */
};
#endif

/* ASF Core Config Table with FEC/CL91 enabled */
static const _soc_th_asf_core_cfg_t
_soc_th_asf_cfg_tbl_fec[] = {
    {    -1,  1,  1,  0, {0,  0}, 0,  0, 0,  { 0,  0}, { 0,   0}, {18,  0,  0,  0}, {18,  0,  0,  0}},   /* SAF     */
    { 10000,  1, 10,  6, {2,  8}, 6, 10, 38, { 9,  8}, {13,  11}, {18, 51, 42, 30}, {18, 60, 48, 27}},   /* 10GE    */
    { 11000,  1, 10,  6, {2,  8}, 6, 10, 38, { 9,  8}, {13,  11}, {18, 51, 42, 30}, {18, 60, 48, 27}},   /* HG[11]  */
    { 20000,  3, 10,  7, {2,  8}, 7, 11, 29, {13, 12}, {18,  16}, {18, 63, 48, 30}, {18, 54, 42, 21}},   /* 20GE    */
    { 21000,  3, 10,  7, {2,  8}, 7, 11, 29, {13, 12}, {18,  16}, {18, 63, 48, 30}, {18, 54, 42, 21}},   /* HG[21]  */
    { 25000,  5, 10,  7, {3,  8}, 6, 11, 25, {12, 12}, {16,  16}, {18, 95, 83, 62}, {18, 86, 71, 53}},   /* 25GE    */
    { 27000,  5, 10,  7, {3,  8}, 6, 11, 25, {12, 12}, {16,  16}, {18, 95, 83, 62}, {18, 86, 71, 53}},   /* HG[27]  */
    { 40000,  7, 12,  7, {3, 12}, 8, 11, 29, {18, 14}, {25,  19}, {18, 57, 45, 24}, {18, 42, 30, 12}},   /* 40GE    */
    { 42000,  7, 12,  7, {3, 12}, 8, 11, 29, {18, 14}, {25,  19}, {18, 57, 45, 24}, {18, 42, 30, 12}},   /* HG[42]  */
    { 50000,  7, 12, 20, {1, 12}, 8, 31, 25, {20, 17}, {30,  24}, {18, 94, 82, 61}, {18, 73, 67, 52}},   /* 50GE    */
    { 53000,  7, 12,  7, {1, 12}, 8, 11, 25, {20, 17}, {30,  24}, {18, 94, 82, 61}, {18, 73, 67, 52}},   /* HG[53]  */
    {100000, 11, 12,  6, {3,  3}, 8, 10,  3, {33, 27}, {44,  36}, {18, 48, 36, 18}, {18, 33, 27, 15}},   /* 100GE   */
    {106000, 11, 12,  6, {3,  3}, 8, 10,  3, {33, 27}, {44,  36}, {18, 48, 36, 18}, {18, 33, 27, 15}}    /* HG[106] */
};
#ifdef BCM_TOMAHAWK2_SUPPORT
static const _soc_th2_asf_core_cfg_t
_soc_th2_asf_cfg_tbl_fec[] = {
    {    -1,  0,  0,  0, {{0,  0}, { 0,  0}, {0,  0}}, {{ 0,  0}, { 0,  0}, { 0,  0}},  0, {{0, 0, 0}, {0, 0, 0}}, {{0, { 0,  0,  0}}, { 0, { 0,  0,  0}}, { 0, { 0,  0,  0}}}, {{0,  0}, { 0,  0}}, {10,  8}, {12, 12}},   /* SAF     */
    { 10000,  1, 10,  0, {{3, 10}, { 9, 16}, {3, 10}}, {{ 8, 19}, {14, 25}, {46, 57}}, 38, {{0, 0, 0}, {0, 0, 0}}, {{7, {29, 29, 29}}, {12, {40, 40, 40}}, {16, {48, 48, 48}}}, {{1,  5}, { 1,  7}}, {10,  9}, {12, 12}},   /* 10GE    */
    { 11000,  1, 10,  6, {{3, 10}, { 9, 16}, {3, 10}}, {{ 8, 19}, {14, 25}, {46, 57}}, 38, {{0, 0, 0}, {0, 0, 0}}, {{7, {29, 29, 29}}, {12, {40, 40, 40}}, {16, {48, 48, 48}}}, {{1,  5}, { 1,  7}}, {10,  9}, {12, 12}},   /* HG[11]  */
    { 20000,  3, 10,  0, {{3, 10}, {10, 17}, {3, 10}}, {{ 8, 22}, {15, 26}, {37, 51}}, 29, {{1, 1, 0}, {0, 0, 0}}, {{7, {28, 35, 30}}, {12, {41, 41, 41}}, {16, {49, 49, 49}}}, {{1,  7}, { 3, 12}}, {17, 11}, {12, 12}},   /* 20GE    */
    { 21000,  3, 10,  7, {{3, 10}, {10, 17}, {3, 10}}, {{ 8, 22}, {15, 26}, {37, 51}}, 29, {{1, 1, 0}, {0, 0, 0}}, {{7, {28, 35, 30}}, {12, {41, 41, 41}}, {16, {49, 49, 49}}}, {{1,  7}, { 3, 12}}, {17, 11}, {12, 12}},   /* HG[21]  */
    { 25000,  5, 10,  0, {{3, 12}, {13, 20}, {3, 12}}, {{10, 27}, {20, 31}, {35, 50}}, 25, {{0, 0, 0}, {0, 0, 0}}, {{7, {64, 64, 64}}, {12, {75, 75, 75}}, {16, {83, 83, 83}}}, {{3, 10}, { 2, 12}}, {18, 16}, {10, 18}},   /* 25GE    */
    { 27000,  5, 10,  7, {{3, 12}, {13, 20}, {3, 12}}, {{10, 27}, {20, 31}, {35, 50}}, 25, {{0, 0, 0}, {0, 0, 0}}, {{7, {64, 64, 64}}, {12, {75, 75, 75}}, {16, {83, 83, 83}}}, {{3, 10}, { 2, 12}}, {18, 16}, {10, 18}},   /* HG[27]  */
    { 40000,  7, 12,  0, {{3, 10}, {10, 17}, {3, 10}}, {{ 8, 25}, {15, 26}, {37, 54}}, 29, {{1, 1, 1}, {1, 1, 0}}, {{7, {13, 18, 38}}, {12, {52, 52, 44}}, {16, {44, 62, 52}}}, {{3, 12}, { 5, 21}}, {26, 17}, {12, 12}},   /* 40GE    */
    { 42000,  7, 12,  7, {{3, 10}, {10, 17}, {3, 10}}, {{ 8, 25}, {15, 26}, {37, 54}}, 29, {{1, 1, 1}, {1, 1, 0}}, {{7, {13, 18, 38}}, {12, {52, 52, 44}}, {16, {44, 62, 52}}}, {{3, 12}, { 5, 21}}, {26, 17}, {12, 12}},   /* HG[42]  */
    { 50000,  9, 12,  0, {{3, 12}, {13, 20}, {3, 12}}, {{ 8, 29}, {20, 31}, {35, 54}}, 25, {{1, 0, 0}, {0, 0, 0}}, {{7, {78, 72, 72}}, {12, {83, 83, 83}}, {16, {91, 91, 91}}}, {{4, 14}, { 7, 24}}, {32, 22}, { 6, 18}},   /* 50GE    */
    { 53000,  9, 12,  7, {{3, 12}, {13, 20}, {3, 12}}, {{ 8, 29}, {20, 31}, {35, 54}}, 25, {{1, 0, 0}, {0, 0, 0}}, {{7, {78, 72, 72}}, {12, {83, 83, 83}}, {16, {91, 91, 91}}}, {{4, 14}, { 7, 24}}, {32, 22}, { 6, 18}},   /* HG[53]  */
    {100000, 11, 12,  0, {{3, 12}, {11, 20}, {3, 12}}, {{ 8, 36}, {18, 29}, {11, 39}},  3, {{1, 1, 1}, {1, 1, 1}}, {{7, {13, 13, 28}}, {12, {24, 34, 64}}, {16, {31, 34, 74}}}, {{7, 25}, {11, 32}}, {48, 31}, { 6, 18}},   /* 100GE   */
    {106000, 11, 12,  8, {{3, 12}, {11, 20}, {3, 12}}, {{ 8, 36}, {18, 29}, {11, 39}},  0, {{1, 1, 1}, {1, 1, 1}}, {{7, {13, 13, 28}}, {12, {24, 34, 64}}, {16, {31, 34, 74}}}, {{7, 25}, {11, 32}}, {48, 31}, { 6, 18}}    /* HG[106] */
};

/* Use FEC cfg only */
#define _SOC_TH2_ASF_CFG_TBL(unit, cl91)    (_soc_th2_asf_cfg_tbl_fec)
#endif

/* ASF Control */
static _soc_th_asf_ctrl_t*
_soc_th_asf_ctrl[SOC_MAX_NUM_DEVICES] = {NULL};

/* ASF profile billboard */
static const char *asf_profile_str[3] =
    {"removed of cut-thru capability",
     "capable of limited speed range cut-thru",
     "capable of extended speed range cut-thru"};

/*  Linkscan callback routine                                               */
/*  Reconfigures ASF settings on 25G/50G ports when FEC91/FEC108 is enabled */
STATIC void
soc_th_asf_linkscan_callback(int unit, soc_port_t port, bcm_port_info_t *info)
{
    soc_th_asf_mode_e mode;
    int port_speed;

    if (info->linkstatus != BCM_PORT_LINK_STATUS_UP) {
        return;
    }
    port_speed = info->speed;

    if (_soc_th_asf_ctrl[unit] == NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "!!! error: _soc_th_asf_ctrl[%d] is NULL pointer !!!\n"),
                 unit));
        return;
    }

    /* Reconfigure ASF settings for 25G/50G ports if CL91/CL108 is enabled on the ports */
    if (_soc_th_asf_ctrl[unit]->init) {
        if ((port_speed == 25000) || (port_speed == 27000) ||
            (port_speed == 50000) || (port_speed == 53000) ||
            (port_speed == 100000) || (port_speed == 106000)) {
                    if (SOC_E_NONE == soc_th_port_asf_mode_get(unit, port, port_speed, &mode)) {
                        soc_th_port_asf_mode_set(unit, port, port_speed, mode);
                    }
        }
    }
    return;
}

/*
 * Cut-through class encoding:
 *   0 - SAF
 *   1 - 10GE
 *   2 - HG[11]
 *   3 - 20GE
 *   4 - 21[HG}
 *   5 - 25GE
 *   6 - HG[27]
 *   7 - 40GE
 *   8 - HG[42]
 *   9 - 50GE
 *   10 - HG[53]
 *   11 - 100GE
 *   12 - HG[106]
 */
STATIC int
_soc_th_speed_to_ct_class_map(int speed)
{
    int ct_class;

    if (1000 == speed) { /* 1G ports always in SAF, use same setting as 10G ports */
        speed = 10000;
    }

    for (ct_class = 0; ct_class <= 12; ct_class++) {
        if (_soc_th_asf_cfg_tbl[ct_class].speed == speed) {
            return ct_class;
        }
    }

    return SOC_E_PARAM;
}

/******************************************************************************
 * Name: _soc_th_port_asf_class_get                                           *
 * Description:                                                               *
 *     Get Cut-through class configured for the specified port                *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 * OUT params:                                                                *
 *     - CT class                                                             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 */
STATIC int
_soc_th_port_asf_class_get(
    int             unit,
    soc_port_t      port,
    OUT int* const  class)
{
    egr_ip_cut_thru_class_entry_t entry;

    if (!class) {
        return SOC_E_PARAM;
    }

    sal_memset(&entry, 0, sizeof(egr_ip_cut_thru_class_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_EGR_IP_CUT_THRU_CLASSm(unit, MEM_BLOCK_ALL, port, &entry));
    *class = soc_mem_field32_get(unit, EGR_IP_CUT_THRU_CLASSm, &entry,
                                 CUT_THRU_CLASSf);

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_class_init                                          *
 * Description:                                                               *
 *     Initialize Cut-through class for the specified port                    *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - CT class                                                             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 */
STATIC int
_soc_th_port_asf_class_init(
    int          unit,
    soc_port_t   port,
    int          ct_class)
{
    egr_ip_cut_thru_class_entry_t entry;

    sal_memset(&entry, 0, sizeof(egr_ip_cut_thru_class_entry_t));
    soc_mem_field32_set(unit, EGR_IP_CUT_THRU_CLASSm, &entry,
                        CUT_THRU_CLASSf, ct_class);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_IP_CUT_THRU_CLASSm(unit, MEM_BLOCK_ALL, port, &entry));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_port_cl91_get                                       *
 * Description:                                                               *
 *     Retreives the cl91 fec state for given port                            *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 * OUT params:                                                                *
 *     - port_cl91_state : 1 if FEC CL91 is enabled on the port               *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th_port_asf_port_cl91_get(
    int           unit,
    soc_port_t    port,
    uint32        *port_cl91_state)
{
    int retVal;

    *port_cl91_state = 0;

    if ((!_soc_th_asf_ctrl[unit]) || 
        (IS_CPU_PORT(unit, port)) || (IS_LB_PORT(unit, port))) {
        return SOC_E_NONE;
    }

    retVal = bcm_esw_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91,
        port_cl91_state);

    if ((retVal != SOC_E_NONE) && (retVal != SOC_E_INIT) && (retVal != SOC_E_UNAVAIL)) {
        return retVal;
    }

    return SOC_E_NONE;
}


#ifdef BCM_TOMAHAWK2_SUPPORT
/******************************************************************************
 * Name: _soc_th2_port_asf_xmit_start_count_get                                *
 * Description:                                                               *
 *     Query or retrieve the XMIT Start Count for the specified source class  *
 *     of the port depending on the magic number passed in xmit_cnt           *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Source Port's Class                                                  *
 *     - Destination Port's CT Class                                          *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - xmit_cnt : IN:  carries magic number for query / retrieve            *
 *     - xmit_cnt : OUT: contains a valid XMIT START COUNT for the            *
 *       source port class, on success                                        *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th2_port_asf_xmit_start_count_get(
    int                      unit,
    soc_port_t               port,
    int                      sc,
    int                      dc,
    soc_th_asf_mode_e        mode,
    IN_OUT uint8* const      xmit_cnt)
{
    soc_info_t                  *si;
    egr_xmit_start_count_entry_t entry;
    int                          ovs_ratio, port_idx;
    soc_pbmp_t                   th_ports, line_rate_ports, ovs_3_2_ports;
    soc_port_t                   th_port;
    int                          latency = SOC_SWITCH_BYPASS_MODE_NONE;
    int                          speed;
    int                          class;
    int                          pipe;
    soc_mem_t                    config_mem = INVALIDm;
    uint32                       port_cl91_state;
    const _soc_th2_asf_core_cfg_t *_soc_th2_asf_cfg_table;

    _SOC_TH_UNIT_VALIDATE(unit);

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    pipe = si->port_pipe[port];
    config_mem = SOC_MEM_UNIQUE_ACC(unit, EGR_XMIT_START_COUNTm)[pipe];

    if (_SOC_TH_ASF_MODE_CFG_UPDATE == mode) { /* translate to valid mode */
        /* reverse map speed class to speed, to surmount speed deficit */
        SOC_IF_ERROR_RETURN(_soc_th_port_asf_class_get(unit, port, &class));
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(class, 0);
        speed = _SOC_TH_CT_CLASS_TO_SPEED_MAP(class);
        SOC_IF_ERROR_RETURN(soc_th_port_asf_mode_get(unit, port, speed, &mode));
    }
    if (!xmit_cnt ||
        !((mode >= _SOC_TH_ASF_MODE_SAF) &&
          (mode <= _SOC_TH_ASF_MODE_FAST_TO_SLOW))) {
        return SOC_E_PARAM;
    }
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(sc, 0);
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(dc, 0);

    if (_SOC_TH_ASF_RETRV == *xmit_cnt) {
        *xmit_cnt = 0;
        port_idx = ((port % 34) * 16) + sc;

        sal_memset(&entry, 0, sizeof(egr_xmit_start_count_entry_t));
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, config_mem, MEM_BLOCK_ALL,
                                                     port_idx, &entry));
        *xmit_cnt = soc_mem_field32_get(unit, config_mem,
                                        &entry, THRESHOLDf);
    } else if (_SOC_TH_ASF_QUERY == *xmit_cnt) {
        *xmit_cnt = 0;
        SOC_PBMP_CLEAR(th_ports);
        SOC_PBMP_ASSIGN(th_ports, PBMP_E_ALL(unit));
        SOC_PBMP_OR(th_ports, PBMP_HG_ALL(unit));
        SOC_PBMP_REMOVE(th_ports, PBMP_MANAGEMENT(unit));
        SOC_PBMP_CLEAR(line_rate_ports);
        SOC_PBMP_CLEAR(ovs_3_2_ports);

        PBMP_ITER(th_ports, th_port) {
            if (SOC_PBMP_MEMBER(si->oversub_pbm, th_port)) {
                SOC_IF_ERROR_RETURN(
                    soc_th_port_oversub_ratio_get(unit, th_port, &ovs_ratio));
                if (ovs_ratio <= 1500) { /* oversub ratio <= 3:2 */
                    SOC_PBMP_PORT_ADD(ovs_3_2_ports, th_port);
                }
            }
            else { /* Line-rate */
                SOC_PBMP_PORT_ADD(line_rate_ports, th_port);
            }
        }

        SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));
        _soc_th2_asf_cfg_table = _SOC_TH2_ASF_CFG_TBL(unit, port_cl91_state);

        /* Get current switch latency mode & set xmit_cnt accordingly */
        SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));

        /* if sc < dc, use SAF setting; else, use CT setting */
        if (SOC_PBMP_EQ(th_ports, line_rate_ports)) {
            *xmit_cnt = (sc < dc) ? 
                _soc_th2_asf_cfg_table[dc].edb_prebuffer.line_rate.saf :
                _soc_th2_asf_cfg_table[dc].edb_prebuffer.line_rate.ct[latency];
        } else if (SOC_PBMP_EQ(th_ports, ovs_3_2_ports)) {
            *xmit_cnt = (sc < dc) ? 
                _soc_th2_asf_cfg_table[dc].edb_prebuffer.oversub_3_2.saf :
                _soc_th2_asf_cfg_table[dc].edb_prebuffer.oversub_3_2.ct[latency];
        } else {
            *xmit_cnt = (sc < dc) ? 
                _soc_th2_asf_cfg_table[dc].edb_prebuffer.oversub_2_1.saf :
                _soc_th2_asf_cfg_table[dc].edb_prebuffer.oversub_2_1.ct[latency];
        }

    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th2_port_asf_mmu_prebuf_get                                      *
 * Description:                                                               *
 *     Query or retrieve MMU Prebuffer (Min Source Port XMIT Count) for the   *
 *     specified port depending on the magic number passed in mmu_prebuf      *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - mmu_prebuf : IN:  carries magic number for query / retrieve          *
 *     - mmu_prebuf : OUT: contains a valid Min Source Port XMIT Count on     *
 *       success                                                              *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th2_port_asf_mmu_prebuf_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    int                   min_sp,
    IN_OUT uint8* const   mmu_prebuf)
{
    uint32 rval;
    int    speed_encoding;
    uint32 port_cl91_state;
    const _soc_th2_asf_core_cfg_t *_soc_th2_asf_cfg_table;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!mmu_prebuf) {
        return SOC_E_PARAM;
    }

    if (_SOC_TH_ASF_RETRV == *mmu_prebuf) {
        *mmu_prebuf = 0;

        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *mmu_prebuf = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                        MIN_SRC_PORT_XMIT_CNTf);
    } else if (_SOC_TH_ASF_QUERY == *mmu_prebuf) {
        *mmu_prebuf = 0;
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

        SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));
        _soc_th2_asf_cfg_table = _SOC_TH2_ASF_CFG_TBL(unit, port_cl91_state);

        *mmu_prebuf = 
            _soc_th2_asf_cfg_table[speed_encoding].mmu_prebuffer;
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th2_port_asf_fifo_threshold_get                                  *
 * Description:                                                               *
 *     Query or retrieve FIFO Threshold for the specified port depending on   *
 *     the magic number passed in fifo_threshold                              *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Min Source Port Speed                                                *
 * IN/OUT params:                                                             *
 *     - fifo_threshold : IN:  carries magic number for query / retrieve      *
 *     - fifo_threshold : OUT: contains a valid FIFO Threshold on success     *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th2_port_asf_fifo_threshold_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_th_asf_mode_e     mode,
    int                   min_sp,
    IN_OUT uint8* const   fifo_threshold)
{
    uint32 rval;
    int    speed_encoding;
    uint32 oversub, port_cl91_state;
    const _soc_th2_asf_core_cfg_t *_soc_th2_asf_cfg_table;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!fifo_threshold) {
        return SOC_E_PARAM;
    }

    if (_SOC_TH_ASF_RETRV == *fifo_threshold) {
        *fifo_threshold = 0;

        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *fifo_threshold = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                            FIFO_THRESHOLDf);
    } else if (_SOC_TH_ASF_QUERY == *fifo_threshold) {
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);
        oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);

        SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));
        _soc_th2_asf_cfg_table = _SOC_TH2_ASF_CFG_TBL(unit, port_cl91_state);

        switch(mode) {
            case _SOC_TH_ASF_MODE_SAME_SPEED:
                *fifo_threshold = (oversub ?
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_threshold.ss.oversub :
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_threshold.ss.line_rate);
                break;

            case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
                *fifo_threshold = (oversub ?
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_threshold.sf.oversub :
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_threshold.sf.line_rate);
                break;

            case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
                *fifo_threshold = (oversub ?
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_threshold.fs.oversub :
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_threshold.fs.line_rate);
                break;

            default:
                *fifo_threshold = 0;
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th2_port_asf_fifo_depth_get                                      *
 * Description:                                                               *
 *     Query or retrieve FIFO Depth for the specified port depending on the   *
 *     magic number passed in buf_sz                                          *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Min Src Port Speed                                                   *
 * IN/OUT params:                                                             *
 *     - fifo_depth : IN:  carries magic number for query/retrieve            *
 *     - fifo_depth : OUT: contains a valid FIFO Depth on success             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th2_port_asf_fifo_depth_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_th_asf_mode_e     mode,
    int                   min_sp,
    IN_OUT uint8* const   fifo_depth)
{
    uint32 rval;
    int    speed_encoding;
    uint32 oversub, port_cl91_state;
    const _soc_th2_asf_core_cfg_t *_soc_th2_asf_cfg_table;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!fifo_depth) {
        return SOC_E_PARAM;
    }

    if (_SOC_TH_ASF_RETRV == *fifo_depth) {
        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        *fifo_depth = 0;
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *fifo_depth = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                        FIFO_DEPTHf);
    } else if (_SOC_TH_ASF_QUERY == *fifo_depth) {
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);
        oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);

        SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));
        _soc_th2_asf_cfg_table = _SOC_TH2_ASF_CFG_TBL(unit, port_cl91_state);

        switch(mode) {
            case _SOC_TH_ASF_MODE_SAME_SPEED:
                *fifo_depth = (oversub ?
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_depth.ss.oversub :
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_depth.ss.line_rate);
                break;

            case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
                *fifo_depth = (oversub ?
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_depth.sf.oversub :
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_depth.sf.line_rate);
                break;

            case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
                *fifo_depth = (oversub ?
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_depth.fs.oversub :
                    _soc_th2_asf_cfg_table[speed_encoding].ct_fifo_depth.fs.line_rate);
                break;

            default:
                *fifo_depth = 0;
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

#endif


/******************************************************************************
 * Name: _soc_th_port_asf_xmit_start_count_get                                *
 * Description:                                                               *
 *     Query or retrieve the XMIT Start Count for the specified source class  *
 *     of the port depending on the magic number passed in xmit_cnt           *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Source Port's Class                                                  *
 *     - Destination Port's CT Class                                          *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - xmit_cnt : IN:  carries magic number for query / retrieve            *
 *     - xmit_cnt : OUT: contains a valid XMIT START COUNT for the            *
 *       source port class, on success                                        *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th_port_asf_xmit_start_count_get(
    int                      unit,
    soc_port_t               port,
    int                      sc,
    int                      dc,
    soc_th_asf_mode_e        mode,
    IN_OUT uint8* const      xmit_cnt)
{
    soc_info_t                  *si;
    egr_xmit_start_count_entry_t entry;
    int                          ovs_ratio, port_idx;
    int                          speed;
    int                          class;
    int                          pipe;
    soc_mem_t                    config_mem = INVALIDm;
    const _soc_th_asf_core_cfg_t *_soc_th_asf_cfg_table;
    uint32 port_cl91_state;

    _SOC_TH_UNIT_VALIDATE(unit);

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl;

    /* Get the FEC/CL91 state of the port to choose correct CT parameters */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

    if (port_cl91_state) {
        _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl_fec;
    }

    pipe = si->port_pipe[port];
    config_mem = SOC_MEM_UNIQUE_ACC(unit, EGR_XMIT_START_COUNTm)[pipe];

    if (_SOC_TH_ASF_MODE_CFG_UPDATE == mode) { /* translate to valid mode */
        /* reverse map speed class to speed, to surmount speed deficit */
        SOC_IF_ERROR_RETURN(_soc_th_port_asf_class_get(unit, port, &class));
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(class, 0);
        speed = _SOC_TH_CT_CLASS_TO_SPEED_MAP(class);
        SOC_IF_ERROR_RETURN(soc_th_port_asf_mode_get(unit, port, speed, &mode));
    }
    if (!xmit_cnt ||
        !((mode >= _SOC_TH_ASF_MODE_SAF) &&
          (mode <= _SOC_TH_ASF_MODE_FAST_TO_SLOW))) {
        return SOC_E_PARAM;
    }
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(sc, 0);
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(dc, 0);

    if (_SOC_TH_ASF_RETRV == *xmit_cnt) {
        *xmit_cnt = 0;
        port_idx = ((port % 34) * 16) + sc;

        sal_memset(&entry, 0, sizeof(egr_xmit_start_count_entry_t));
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, config_mem, MEM_BLOCK_ALL,
                                                     port_idx, &entry));
        *xmit_cnt = soc_mem_field32_get(unit, config_mem,
                                        &entry, THRESHOLDf);
    } else if (_SOC_TH_ASF_QUERY == *xmit_cnt) {
        *xmit_cnt = 0;

        if ((_SOC_TH_ASF_MODE_SAF == mode) || (sc < dc)) {
            *xmit_cnt =  _soc_th_asf_cfg_table[dc].xmit_cnt_l2_latency.saf;
        } else if (SOC_PBMP_IS_NULL(si->oversub_pbm)) { /* ports are line-rate */
            if (_SOC_TH_ASF_MODE_FAST_TO_SLOW == mode) {  /* special case */
                *xmit_cnt =
                    (SOC_SWITCH_BYPASS_MODE_LOW == _soc_th_asf_ctrl[unit]->latency) ?
                        _soc_th_asf_cfg_table[dc].xmit_cnt_l2_latency.oversub_2_1 :
                        _soc_th_asf_cfg_table[dc].xmit_cnt_l3_full_latency.oversub_2_1;

            } else {
                *xmit_cnt =
                    (SOC_SWITCH_BYPASS_MODE_LOW == _soc_th_asf_ctrl[unit]->latency) ?
                        _soc_th_asf_cfg_table[dc].xmit_cnt_l2_latency.line_rate :
                        _soc_th_asf_cfg_table[dc].xmit_cnt_l3_full_latency.line_rate;
            }
         } else { /* oversub */
            SOC_IF_ERROR_RETURN(
                soc_th_port_oversub_ratio_get(unit, port, &ovs_ratio));
            if (ovs_ratio <= 1560) { /* oversub ratio <= 3:2 */
                *xmit_cnt =
                    (SOC_SWITCH_BYPASS_MODE_LOW == _soc_th_asf_ctrl[unit]->latency) ?
                        _soc_th_asf_cfg_table[dc].xmit_cnt_l2_latency.oversub_3_2 :
                        _soc_th_asf_cfg_table[dc].xmit_cnt_l3_full_latency.oversub_3_2;
            } else {
                *xmit_cnt =
                    (SOC_SWITCH_BYPASS_MODE_LOW == _soc_th_asf_ctrl[unit]->latency) ?
                        _soc_th_asf_cfg_table[dc].xmit_cnt_l2_latency.oversub_2_1 :
                        _soc_th_asf_cfg_table[dc].xmit_cnt_l3_full_latency.oversub_2_1;
            }
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_speed_limits_get                                    *
 * Description:                                                               *
 *     Query or retrieve min & max source port speeds for the specified port  *
 *     depending on the magic number passed in min_sp &                       *
 *     max_sp                                                                 *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - min_sp : IN:  carries magic number for query / retrieve              *
 *     - min_sp : OUT: contains a valid min source port speed                 *
 *       on success                                                           *
 *     - max_sp : IN:  carries magic number for query / retrieve              *
 *     - max_sp : OUT: contains a valid max source port speed                 *
 *       on success                                                           *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th_port_asf_speed_limits_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_th_asf_mode_e     mode,
    IN_OUT uint8* const   min_sp,
    IN_OUT uint8* const   max_sp)
{
    uint32 rval;
    int    speed_encoding;
    const _soc_th_asf_core_cfg_t *_soc_th_asf_cfg_table;
    uint32 port_cl91_state;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!min_sp || !max_sp) {
        return SOC_E_PARAM;
    }

    _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl;

    /* Get the FEC/CL91 state of the port to choose correct CT parameters */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

    if (port_cl91_state) {
        _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl_fec;
    }

    if ((_SOC_TH_ASF_RETRV == *min_sp) &&
        (_SOC_TH_ASF_RETRV == *max_sp)) {
        *min_sp = *max_sp = 0;
        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));

        /* MIN_SRC_PORT_SPEED */
        *min_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                    MIN_SRC_PORT_SPEEDf);
        /* MAX_SRC_PORT_SPEED */
        *max_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                    MAX_SRC_PORT_SPEEDf);
    } else if ((_SOC_TH_ASF_QUERY == *min_sp) &&
               (_SOC_TH_ASF_QUERY == *max_sp)) {
        *min_sp = *max_sp = 0;
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 0);

        switch(mode) {
            case _SOC_TH_ASF_MODE_SAF:
                return SOC_E_NONE;

            case _SOC_TH_ASF_MODE_SAME_SPEED:
                *min_sp = *max_sp = speed_encoding;
                break;

            case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
                *min_sp = _soc_th_asf_cfg_table[speed_encoding].min_sp;
#ifdef BCM_TOMAHAWK2_SUPPORT
                /* 40G->50G will be removed from TH support as well,
                   so this workaround will be deleted after TH ASF changes. */
                if (SOC_IS_TOMAHAWK2(unit)) {
                    *min_sp = _soc_th2_asf_cfg_tbl[speed_encoding].min_sp;
                }
#endif
                *max_sp = speed_encoding;
                break;

            case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
                if (_SOC_TH_ASF_MEM_PROFILE_SIMILAR ==
                        _soc_th_asf_ctrl[unit]->asf_mem_profile) {
                    if (speed_encoding % 2) { /* IEEE */
                        /* HG counterpart */
                        *max_sp = speed_encoding + 1;
                    } else { /* HG */
                        *max_sp = speed_encoding;
                    }
                } else if (_SOC_TH_ASF_MEM_PROFILE_EXTREME ==
                               _soc_th_asf_ctrl[unit]->asf_mem_profile) {
                    *max_sp = _soc_th_asf_cfg_table[speed_encoding].max_sp;
                }
                *min_sp = speed_encoding;
                break;

            default:
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_mmu_prebuf_get                                      *
 * Description:                                                               *
 *     Query or retrieve MMU Prebuffer (Min Source Port XMIT Count) for the   *
 *     specified port depending on the magic number passed in mmu_prebuf      *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - mmu_prebuf : IN:  carries magic number for query / retrieve          *
 *     - mmu_prebuf : OUT: contains a valid Min Source Port XMIT Count on     *
 *       success                                                              *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th_port_asf_mmu_prebuf_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    int                   min_sp,
    IN_OUT uint8* const   mmu_prebuf)
{
    uint32 rval;
    int    speed_encoding;
    const _soc_th_asf_core_cfg_t *_soc_th_asf_cfg_table;
    uint32 port_cl91_state;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!mmu_prebuf) {
        return SOC_E_PARAM;
    }

    _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl;

    /* Get the FEC/CL91 state of the port to choose correct CT parameters */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

    if (port_cl91_state) {
        _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl_fec;
    }

    if (_SOC_TH_ASF_RETRV == *mmu_prebuf) {
        *mmu_prebuf = 0;

        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *mmu_prebuf = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                        MIN_SRC_PORT_XMIT_CNTf);
    } else if (_SOC_TH_ASF_QUERY == *mmu_prebuf) {
        *mmu_prebuf = 0;
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

        /* special cases : 50GE & HG[53] */
        if ((9 == speed_encoding) || (10 == speed_encoding)) {
            if ((7 == min_sp) || (8 == min_sp)) {  /* 40GE or HG[42] */
                *mmu_prebuf = 20;
            } else if (9 == min_sp) {   /* HG[50] */
                *mmu_prebuf = 7;
            }
        } else {
            *mmu_prebuf =
                _soc_th_asf_cfg_table[speed_encoding].min_src_port_xmit_cnt;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_fifo_threshold_get                                  *
 * Description:                                                               *
 *     Query or retrieve FIFO Threshold for the specified port depending on   *
 *     the magic number passed in fifo_threshold                              *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Min Source Port Speed                                                *
 * IN/OUT params:                                                             *
 *     - fifo_threshold : IN:  carries magic number for query / retrieve      *
 *     - fifo_threshold : OUT: contains a valid FIFO Threshold on success     *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th_port_asf_fifo_threshold_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_th_asf_mode_e     mode,
    int                   min_sp,
    IN_OUT uint8* const   fifo_threshold)
{
    uint32 rval;
    int    ret, speed_encoding;
    uint8  mmu_prebuf = 0, oversub;
    uint32 port_cl91_state;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!fifo_threshold) {
        return SOC_E_PARAM;
    }

    if (_SOC_TH_ASF_RETRV == *fifo_threshold) {
        *fifo_threshold = 0;

        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *fifo_threshold = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                            FIFO_THRESHOLDf);
    } else if (_SOC_TH_ASF_QUERY == *fifo_threshold) {
        *fifo_threshold = 0;
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

        oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);

        /* Get port CL91 state */
        SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

        switch(mode) {
            case _SOC_TH_ASF_MODE_SAME_SPEED:
                if (oversub) {
                    *fifo_threshold = 10;
                } else {
                    *fifo_threshold = 3;
                }
/* Based on SV testing and feedback from arch, if CL91/CL108 is enabled    */
/* for same speed CT on 25G ports, fifo_threshold should be increased by 1 */
/* for 50G and 100G ports, fifo_threshold should be increased by 2         */
                if (port_cl91_state) {
                    int speed;
                    int class;

                    SOC_IF_ERROR_RETURN(_soc_th_port_asf_class_get(unit, port, &class));
                    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(class, 0);
                    speed = _SOC_TH_CT_CLASS_TO_SPEED_MAP(class);

                    if ((speed == 25000) || (speed == 27000)) {
                        *fifo_threshold = *fifo_threshold + 1;
                    }
                    else if ((speed == 50000) || (speed == 53000) ||
                        (speed == 100000) || (speed == 106000)) {
                        *fifo_threshold = *fifo_threshold + 2;
                    }
                }
                break;
            case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
                if (oversub) {
                    *fifo_threshold = 10;
                } else {
                    *fifo_threshold = 3;
                }
                break;

            case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
                if (oversub) {
                    *fifo_threshold = 10;
                } else {
                    *fifo_threshold = 3;
                }

                mmu_prebuf = _SOC_TH_ASF_QUERY;
                ret = _soc_th_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                                      min_sp, &mmu_prebuf);
                if (SOC_E_NONE != ret) {
                    return ret;
                }

                *fifo_threshold += mmu_prebuf;
                break;

            default:
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_fifo_depth_get                                      *
 * Description:                                                               *
 *     Query or retrieve FIFO Depth for the specified port depending on the   *
 *     magic number passed in buf_sz                                          *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Min Src Port Speed                                                   *
 * IN/OUT params:                                                             *
 *     - fifo_depth : IN:  carries magic number for query/retrieve            *
 *     - fifo_depth : OUT: contains a valid FIFO Depth on success             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_th_port_asf_fifo_depth_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_th_asf_mode_e     mode,
    int                   min_sp,
    IN_OUT uint8* const   fifo_depth)
{
    uint32 rval;
    int    speed_encoding;
    uint8  oversub, mmu_prebuf_depth = 0;
    uint8  same_speed_ct_depth = 0, same_speed_ct_threshold = 0;
    const _soc_th_asf_core_cfg_t *_soc_th_asf_cfg_table;
    uint32 port_cl91_state;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!fifo_depth) {
        return SOC_E_PARAM;
    }

    _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl;

    /* Get the FEC/CL91 state of the port to choose correct CT parameters */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

    if (port_cl91_state) {
        _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl_fec;
    }

    if (_SOC_TH_ASF_RETRV == *fifo_depth) {
        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        *fifo_depth = 0;
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *fifo_depth = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                        FIFO_DEPTHf);
    } else if (_SOC_TH_ASF_QUERY == *fifo_depth) {
        *fifo_depth = 0;
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

        oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);

        switch(mode) {
            case _SOC_TH_ASF_MODE_SAME_SPEED:
                if (oversub) {
                    *fifo_depth =
                    (_soc_th_asf_cfg_table[speed_encoding].ep_credit_acc.oversub +
                     _soc_th_asf_cfg_table[speed_encoding].encap_acc + 17);
                } else {
                    *fifo_depth =
                    (_soc_th_asf_cfg_table[speed_encoding].ep_credit_acc.line_rate + 6);
                }
/* Based on SV testing and feedback from arch, if CL91/CL108 is enabled */
/* for same speed CT on 25G ports, fifo_depth should be increased by 2  */
/* for 50G/100G, fifo_depth should be increased by 4.                   */
                if (port_cl91_state) {
                   int speed;
                   int class;

                   SOC_IF_ERROR_RETURN(_soc_th_port_asf_class_get(unit, port, &class));
                   _SOC_TH_ASF_SPEED_CLASS_VALIDATE(class, 0);
                   speed = _SOC_TH_CT_CLASS_TO_SPEED_MAP(class);

                    if ((speed == 25000) || (speed == 27000)) {
                        *fifo_depth = *fifo_depth + 2;
                    } else if ((speed == 50000) || (speed == 53000) ||
                        (speed == 100000) || (speed == 106000)) {
                        *fifo_depth = *fifo_depth + 4;
                    }
                }
                break;

            case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
                if (oversub) {
                    *fifo_depth =
                    (_soc_th_asf_cfg_table[speed_encoding].ep_credit_acc.oversub +
                     _soc_th_asf_cfg_table[speed_encoding].encap_acc + 17);
                } else {
                    *fifo_depth =
                    (_soc_th_asf_cfg_table[speed_encoding].ep_credit_acc.line_rate + 6);
                }

                /* Extra MMU prebuffer depth */
                if ((9 == speed_encoding) || (10 == speed_encoding)) { /* special cases : 50GE & HG[53] */
                    if ((7 == min_sp) || (8 == min_sp)) {  /* 40GE or HG[42] */
                        mmu_prebuf_depth = 31;
                    } else if (9 == min_sp) {   /* HG[50] */
                        mmu_prebuf_depth = 11;
                    }
                } else {
                    mmu_prebuf_depth = _soc_th_asf_cfg_table[speed_encoding].mmu_prebuf_depth;
                }

                *fifo_depth += mmu_prebuf_depth;
                break;

            case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
                if (oversub) {
                    same_speed_ct_depth =
                    (_soc_th_asf_cfg_table[speed_encoding].ep_credit_acc.oversub +
                     _soc_th_asf_cfg_table[speed_encoding].encap_acc + 17);

                    same_speed_ct_threshold = 10;
                } else {
                    same_speed_ct_depth =
                    (_soc_th_asf_cfg_table[speed_encoding].ep_credit_acc.line_rate + 6);

                    same_speed_ct_threshold = 3;
                }

                *fifo_depth =
                 MIN((same_speed_ct_depth + _soc_th_asf_cfg_table[speed_encoding].fast_to_slow_acc),
                     (same_speed_ct_threshold + 46) );
                break;

            default:
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_asf_obm_ca_fifo_thresh_set                                   *
 * Description:                                                               *
 *     Configure Cell Assembly CT FIFO threshold for all OBM and Pipe         *
 *     instances.                                                             *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Cell Assembly CT Threshold                                           *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 */
STATIC int
_soc_th_asf_obm_ca_fifo_thresh_set(
    int     unit,
    soc_port_t    port,
    uint8   ca_thresh)
{
    int  obm, pipe, clport;
    int port_block_base, phy_port, lane;
    uint32 rval;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 thresh_field[4] = { THRESHOLD0f, THRESHOLD1f,
           THRESHOLD2f, THRESHOLD3f };
    uint32 port_ct_sel_field[4] = { PORT0_CT_SELf, PORT1_CT_SELf,
           PORT2_CT_SELf, PORT3_CT_SELf };
    uint32 idb_obm_ca_ct_ctrl_reg[][4] = {
           {IDB_OBM0_CA_CT_CONTROL_PIPE0r, IDB_OBM0_CA_CT_CONTROL_PIPE1r,
            IDB_OBM0_CA_CT_CONTROL_PIPE2r, IDB_OBM0_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM1_CA_CT_CONTROL_PIPE0r, IDB_OBM1_CA_CT_CONTROL_PIPE1r,
            IDB_OBM1_CA_CT_CONTROL_PIPE2r, IDB_OBM1_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM2_CA_CT_CONTROL_PIPE0r, IDB_OBM2_CA_CT_CONTROL_PIPE1r,
            IDB_OBM2_CA_CT_CONTROL_PIPE2r, IDB_OBM2_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM3_CA_CT_CONTROL_PIPE0r, IDB_OBM3_CA_CT_CONTROL_PIPE1r,
            IDB_OBM3_CA_CT_CONTROL_PIPE2r, IDB_OBM3_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM4_CA_CT_CONTROL_PIPE0r, IDB_OBM4_CA_CT_CONTROL_PIPE1r,
            IDB_OBM4_CA_CT_CONTROL_PIPE2r, IDB_OBM4_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM5_CA_CT_CONTROL_PIPE0r, IDB_OBM5_CA_CT_CONTROL_PIPE1r,
            IDB_OBM5_CA_CT_CONTROL_PIPE2r, IDB_OBM5_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM6_CA_CT_CONTROL_PIPE0r, IDB_OBM6_CA_CT_CONTROL_PIPE1r,
            IDB_OBM6_CA_CT_CONTROL_PIPE2r, IDB_OBM6_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM7_CA_CT_CONTROL_PIPE0r, IDB_OBM7_CA_CT_CONTROL_PIPE1r,
            IDB_OBM7_CA_CT_CONTROL_PIPE2r, IDB_OBM7_CA_CT_CONTROL_PIPE3r},
#ifdef BCM_TOMAHAWK2_SUPPORT
           {IDB_OBM8_CA_CT_CONTROL_PIPE0r, IDB_OBM8_CA_CT_CONTROL_PIPE1r,
            IDB_OBM8_CA_CT_CONTROL_PIPE2r, IDB_OBM8_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM9_CA_CT_CONTROL_PIPE0r, IDB_OBM9_CA_CT_CONTROL_PIPE1r,
            IDB_OBM9_CA_CT_CONTROL_PIPE2r, IDB_OBM9_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM10_CA_CT_CONTROL_PIPE0r, IDB_OBM10_CA_CT_CONTROL_PIPE1r,
            IDB_OBM10_CA_CT_CONTROL_PIPE2r, IDB_OBM10_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM11_CA_CT_CONTROL_PIPE0r, IDB_OBM11_CA_CT_CONTROL_PIPE1r,
            IDB_OBM11_CA_CT_CONTROL_PIPE2r, IDB_OBM11_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM12_CA_CT_CONTROL_PIPE0r, IDB_OBM12_CA_CT_CONTROL_PIPE1r,
            IDB_OBM12_CA_CT_CONTROL_PIPE2r, IDB_OBM12_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM13_CA_CT_CONTROL_PIPE0r, IDB_OBM13_CA_CT_CONTROL_PIPE1r,
            IDB_OBM13_CA_CT_CONTROL_PIPE2r, IDB_OBM13_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM14_CA_CT_CONTROL_PIPE0r, IDB_OBM14_CA_CT_CONTROL_PIPE1r,
            IDB_OBM14_CA_CT_CONTROL_PIPE2r, IDB_OBM14_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM15_CA_CT_CONTROL_PIPE0r, IDB_OBM15_CA_CT_CONTROL_PIPE1r,
            IDB_OBM15_CA_CT_CONTROL_PIPE2r, IDB_OBM15_CA_CT_CONTROL_PIPE3r},
#endif
        };

    _SOC_TH_UNIT_VALIDATE(unit);

    /* Get lane, pipe & obm */
    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    lane = phy_port - port_block_base;
    pipe = si->port_pipe[port];
    clport = si->port_serdes[port];
    if (SOC_IS_TOMAHAWK(unit)) {
        /* obm number is reversed (mirrored) in odd pipe */
        obm = pipe & 1 ? 7 - (clport & 7) : clport & 7;
    } else {
        obm = clport & 15;
    }

    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, idb_obm_ca_ct_ctrl_reg[obm][pipe],
                      REG_PORT_ANY, 0, &rval));

    soc_reg_field_set(unit, idb_obm_ca_ct_ctrl_reg[obm][pipe], &rval,
              port_ct_sel_field[lane], lane);
    soc_reg_field_set(unit, idb_obm_ca_ct_ctrl_reg[obm][pipe], &rval,
              thresh_field[lane], ca_thresh);

    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, idb_obm_ca_ct_ctrl_reg[obm][pipe],
                              REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_asf_obm_bubble_mop_set                                       *
 * Description:                                                               *
 *     Configure Bubble MOP for all OBM and Pipe Instances                    *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Bubble MOP Flag                                                      *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 */
STATIC int
_soc_th_asf_obm_bubble_mop_set(
    int     unit,
    soc_port_t    port,
    uint8   bubble_mop_disable)
{
    int  obm, pipe, clport;
    int port_block_base, phy_port, lane;
    uint32 rval;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 port_field[4] = {
            PORT0_BUBBLE_MOP_DISABLEf, PORT1_BUBBLE_MOP_DISABLEf,
            PORT2_BUBBLE_MOP_DISABLEf, PORT3_BUBBLE_MOP_DISABLEf };
    uint32 idb_obm_ctrl_reg[][4] = {
           {IDB_OBM0_CONTROL_PIPE0r, IDB_OBM0_CONTROL_PIPE1r,
            IDB_OBM0_CONTROL_PIPE2r, IDB_OBM0_CONTROL_PIPE3r},
           {IDB_OBM1_CONTROL_PIPE0r, IDB_OBM1_CONTROL_PIPE1r,
            IDB_OBM1_CONTROL_PIPE2r, IDB_OBM1_CONTROL_PIPE3r},
           {IDB_OBM2_CONTROL_PIPE0r, IDB_OBM2_CONTROL_PIPE1r,
            IDB_OBM2_CONTROL_PIPE2r, IDB_OBM2_CONTROL_PIPE3r},
           {IDB_OBM3_CONTROL_PIPE0r, IDB_OBM3_CONTROL_PIPE1r,
            IDB_OBM3_CONTROL_PIPE2r, IDB_OBM3_CONTROL_PIPE3r},
           {IDB_OBM4_CONTROL_PIPE0r, IDB_OBM4_CONTROL_PIPE1r,
            IDB_OBM4_CONTROL_PIPE2r, IDB_OBM4_CONTROL_PIPE3r},
           {IDB_OBM5_CONTROL_PIPE0r, IDB_OBM5_CONTROL_PIPE1r,
            IDB_OBM5_CONTROL_PIPE2r, IDB_OBM5_CONTROL_PIPE3r},
           {IDB_OBM6_CONTROL_PIPE0r, IDB_OBM6_CONTROL_PIPE1r,
            IDB_OBM6_CONTROL_PIPE2r, IDB_OBM6_CONTROL_PIPE3r},
           {IDB_OBM7_CONTROL_PIPE0r, IDB_OBM7_CONTROL_PIPE1r,
            IDB_OBM7_CONTROL_PIPE2r, IDB_OBM7_CONTROL_PIPE3r},
#ifdef BCM_TOMAHAWK2_SUPPORT
           {IDB_OBM8_CONTROL_PIPE0r, IDB_OBM8_CONTROL_PIPE1r,
            IDB_OBM8_CONTROL_PIPE2r, IDB_OBM8_CONTROL_PIPE3r},
           {IDB_OBM9_CONTROL_PIPE0r, IDB_OBM9_CONTROL_PIPE1r,
            IDB_OBM9_CONTROL_PIPE2r, IDB_OBM9_CONTROL_PIPE3r},
           {IDB_OBM10_CONTROL_PIPE0r, IDB_OBM10_CONTROL_PIPE1r,
            IDB_OBM10_CONTROL_PIPE2r, IDB_OBM10_CONTROL_PIPE3r},
           {IDB_OBM11_CONTROL_PIPE0r, IDB_OBM11_CONTROL_PIPE1r,
            IDB_OBM11_CONTROL_PIPE2r, IDB_OBM11_CONTROL_PIPE3r},
           {IDB_OBM12_CONTROL_PIPE0r, IDB_OBM12_CONTROL_PIPE1r,
            IDB_OBM12_CONTROL_PIPE2r, IDB_OBM12_CONTROL_PIPE3r},
           {IDB_OBM13_CONTROL_PIPE0r, IDB_OBM13_CONTROL_PIPE1r,
            IDB_OBM13_CONTROL_PIPE2r, IDB_OBM13_CONTROL_PIPE3r},
           {IDB_OBM14_CONTROL_PIPE0r, IDB_OBM14_CONTROL_PIPE1r,
            IDB_OBM14_CONTROL_PIPE2r, IDB_OBM14_CONTROL_PIPE3r},
           {IDB_OBM15_CONTROL_PIPE0r, IDB_OBM15_CONTROL_PIPE1r,
            IDB_OBM15_CONTROL_PIPE2r, IDB_OBM15_CONTROL_PIPE3r},
#endif
        };

    _SOC_TH_UNIT_VALIDATE(unit);

    /* Get lane, pipe & obm */
    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    lane = phy_port - port_block_base;
    pipe = si->port_pipe[port];
    clport = si->port_serdes[port];
    if (SOC_IS_TOMAHAWK(unit)) {
        /* obm number is reversed (mirrored) in odd pipe */
        obm = pipe & 1 ? 7 - (clport & 7) : clport & 7;
    } else {
        obm = clport & 15;
    }

    /* BubbleMOP - At present only L3 & Full Latency Mode supported */
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, idb_obm_ctrl_reg[obm][pipe],
                      REG_PORT_ANY, 0, &rval));

    soc_reg_field_set(unit, idb_obm_ctrl_reg[obm][pipe], &rval,
                      port_field[lane], bubble_mop_disable);

    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, idb_obm_ctrl_reg[obm][pipe], REG_PORT_ANY,
                      0, rval));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_asf_obm_ct_thresh_ok_set                                     *
 * Description:                                                               *
 *     Configure IDB OBM CT FIFO threshold OK for all OBM and Pipe            *
 *     instances.                                                             *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - CT allowed if usage is less than or eq threshold                     *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 */
STATIC int
_soc_th_asf_obm_ct_thresh_ok_set(
    int     unit,
    soc_port_t    port,
    uint32   ct_ok_thresh)
{
    int  obm, pipe, clport;
    int port_block_base, phy_port, lane;
    uint64 rval64;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 idb_obm_ct_thresh_reg[][4] = {
           {IDB_OBM0_CT_THRESHOLD_PIPE0r, IDB_OBM0_CT_THRESHOLD_PIPE1r,
            IDB_OBM0_CT_THRESHOLD_PIPE2r, IDB_OBM0_CT_THRESHOLD_PIPE3r},
           {IDB_OBM1_CT_THRESHOLD_PIPE0r, IDB_OBM1_CT_THRESHOLD_PIPE1r,
            IDB_OBM1_CT_THRESHOLD_PIPE2r, IDB_OBM1_CT_THRESHOLD_PIPE3r},
           {IDB_OBM2_CT_THRESHOLD_PIPE0r, IDB_OBM2_CT_THRESHOLD_PIPE1r,
            IDB_OBM2_CT_THRESHOLD_PIPE2r, IDB_OBM2_CT_THRESHOLD_PIPE3r},
           {IDB_OBM3_CT_THRESHOLD_PIPE0r, IDB_OBM3_CT_THRESHOLD_PIPE1r,
            IDB_OBM3_CT_THRESHOLD_PIPE2r, IDB_OBM3_CT_THRESHOLD_PIPE3r},
           {IDB_OBM4_CT_THRESHOLD_PIPE0r, IDB_OBM4_CT_THRESHOLD_PIPE1r,
            IDB_OBM4_CT_THRESHOLD_PIPE2r, IDB_OBM4_CT_THRESHOLD_PIPE3r},
           {IDB_OBM5_CT_THRESHOLD_PIPE0r, IDB_OBM5_CT_THRESHOLD_PIPE1r,
            IDB_OBM5_CT_THRESHOLD_PIPE2r, IDB_OBM5_CT_THRESHOLD_PIPE3r},
           {IDB_OBM6_CT_THRESHOLD_PIPE0r, IDB_OBM6_CT_THRESHOLD_PIPE1r,
            IDB_OBM6_CT_THRESHOLD_PIPE2r, IDB_OBM6_CT_THRESHOLD_PIPE3r},
           {IDB_OBM7_CT_THRESHOLD_PIPE0r, IDB_OBM7_CT_THRESHOLD_PIPE1r,
            IDB_OBM7_CT_THRESHOLD_PIPE2r, IDB_OBM7_CT_THRESHOLD_PIPE3r},
#ifdef BCM_TOMAHAWK2_SUPPORT
           {IDB_OBM8_CT_THRESHOLD_PIPE0r, IDB_OBM8_CT_THRESHOLD_PIPE1r,
            IDB_OBM8_CT_THRESHOLD_PIPE2r, IDB_OBM8_CT_THRESHOLD_PIPE3r},
           {IDB_OBM9_CT_THRESHOLD_PIPE0r, IDB_OBM9_CT_THRESHOLD_PIPE1r,
            IDB_OBM9_CT_THRESHOLD_PIPE2r, IDB_OBM9_CT_THRESHOLD_PIPE3r},
           {IDB_OBM10_CT_THRESHOLD_PIPE0r, IDB_OBM10_CT_THRESHOLD_PIPE1r,
            IDB_OBM10_CT_THRESHOLD_PIPE2r, IDB_OBM10_CT_THRESHOLD_PIPE3r},
           {IDB_OBM11_CT_THRESHOLD_PIPE0r, IDB_OBM11_CT_THRESHOLD_PIPE1r,
            IDB_OBM11_CT_THRESHOLD_PIPE2r, IDB_OBM11_CT_THRESHOLD_PIPE3r},
           {IDB_OBM12_CT_THRESHOLD_PIPE0r, IDB_OBM12_CT_THRESHOLD_PIPE1r,
            IDB_OBM12_CT_THRESHOLD_PIPE2r, IDB_OBM12_CT_THRESHOLD_PIPE3r},
           {IDB_OBM13_CT_THRESHOLD_PIPE0r, IDB_OBM13_CT_THRESHOLD_PIPE1r,
            IDB_OBM13_CT_THRESHOLD_PIPE2r, IDB_OBM13_CT_THRESHOLD_PIPE3r},
           {IDB_OBM14_CT_THRESHOLD_PIPE0r, IDB_OBM14_CT_THRESHOLD_PIPE1r,
            IDB_OBM14_CT_THRESHOLD_PIPE2r, IDB_OBM14_CT_THRESHOLD_PIPE3r},
           {IDB_OBM15_CT_THRESHOLD_PIPE0r, IDB_OBM15_CT_THRESHOLD_PIPE1r,
            IDB_OBM15_CT_THRESHOLD_PIPE2r, IDB_OBM15_CT_THRESHOLD_PIPE3r},
#endif
        };

    _SOC_TH_UNIT_VALIDATE(unit);

    /* Get lane, pipe & obm */
    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    lane = phy_port - port_block_base;
    pipe = si->port_pipe[port];
    clport = si->port_serdes[port];
    if (SOC_IS_TOMAHAWK(unit)) {
        /* obm number is reversed (mirrored) in odd pipe */
        obm = pipe & 1 ? 7 - (clport & 7) : clport & 7;
    } else {
        obm = clport & 15;
    }

    SOC_IF_ERROR_RETURN(
        soc_reg64_get(unit, idb_obm_ct_thresh_reg[obm][pipe],
                      REG_PORT_ANY, lane, &rval64));

    soc_reg64_field32_set(unit, idb_obm_ct_thresh_reg[obm][pipe], &rval64,
              CUT_THROUGH_OKf, ct_ok_thresh);

    SOC_IF_ERROR_RETURN(
        soc_reg64_set(unit, idb_obm_ct_thresh_reg[obm][pipe],
                              REG_PORT_ANY, lane, rval64));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_asf_force_saf_config_set                                     *
 * Description:                                                               *
 *     Configure IDB_OBM_FORCE_SAF_CONFIG register for all OBM and Pipe       *
 *     instances. Also configure IDB_FORCE_SAF_CONFIG register.               *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 */
STATIC int
_soc_th_asf_force_saf_config_set(
    int     unit,
    soc_port_t    port,
    soc_th_asf_mode_e   mode)
{
    int  obm, pipe, clport;
    int port_block_base, phy_port, lane;
    uint32 port_cl91_state;
    int speed;
    int class;
    uint32 rval;
    uint32 field_a;
    uint32 field_b;
    uint32 field_c;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 idb_obm_force_saf_config_reg[][4] = {
           {IDB_OBM0_DBG_A_PIPE0r, IDB_OBM0_DBG_A_PIPE1r,
            IDB_OBM0_DBG_A_PIPE2r, IDB_OBM0_DBG_A_PIPE3r},
           {IDB_OBM1_DBG_A_PIPE0r, IDB_OBM1_DBG_A_PIPE1r,
            IDB_OBM1_DBG_A_PIPE2r, IDB_OBM1_DBG_A_PIPE3r},
           {IDB_OBM2_DBG_A_PIPE0r, IDB_OBM2_DBG_A_PIPE1r,
            IDB_OBM2_DBG_A_PIPE2r, IDB_OBM2_DBG_A_PIPE3r},
           {IDB_OBM3_DBG_A_PIPE0r, IDB_OBM3_DBG_A_PIPE1r,
            IDB_OBM3_DBG_A_PIPE2r, IDB_OBM3_DBG_A_PIPE3r},
           {IDB_OBM4_DBG_A_PIPE0r, IDB_OBM4_DBG_A_PIPE1r,
            IDB_OBM4_DBG_A_PIPE2r, IDB_OBM4_DBG_A_PIPE3r},
           {IDB_OBM5_DBG_A_PIPE0r, IDB_OBM5_DBG_A_PIPE1r,
            IDB_OBM5_DBG_A_PIPE2r, IDB_OBM5_DBG_A_PIPE3r},
           {IDB_OBM6_DBG_A_PIPE0r, IDB_OBM6_DBG_A_PIPE1r,
            IDB_OBM6_DBG_A_PIPE2r, IDB_OBM6_DBG_A_PIPE3r},
           {IDB_OBM7_DBG_A_PIPE0r, IDB_OBM7_DBG_A_PIPE1r,
            IDB_OBM7_DBG_A_PIPE2r, IDB_OBM7_DBG_A_PIPE3r},
#ifdef BCM_TOMAHAWK2_SUPPORT
           {IDB_OBM8_DBG_A_PIPE0r, IDB_OBM8_DBG_A_PIPE1r,
            IDB_OBM8_DBG_A_PIPE2r, IDB_OBM8_DBG_A_PIPE3r},
           {IDB_OBM9_DBG_A_PIPE0r, IDB_OBM9_DBG_A_PIPE1r,
            IDB_OBM9_DBG_A_PIPE2r, IDB_OBM9_DBG_A_PIPE3r},
           {IDB_OBM10_DBG_A_PIPE0r, IDB_OBM10_DBG_A_PIPE1r,
            IDB_OBM10_DBG_A_PIPE2r, IDB_OBM10_DBG_A_PIPE3r},
           {IDB_OBM11_DBG_A_PIPE0r, IDB_OBM11_DBG_A_PIPE1r,
            IDB_OBM11_DBG_A_PIPE2r, IDB_OBM11_DBG_A_PIPE3r},
           {IDB_OBM12_DBG_A_PIPE0r, IDB_OBM12_DBG_A_PIPE1r,
            IDB_OBM12_DBG_A_PIPE2r, IDB_OBM12_DBG_A_PIPE3r},
           {IDB_OBM13_DBG_A_PIPE0r, IDB_OBM13_DBG_A_PIPE1r,
            IDB_OBM13_DBG_A_PIPE2r, IDB_OBM13_DBG_A_PIPE3r},
           {IDB_OBM14_DBG_A_PIPE0r, IDB_OBM14_DBG_A_PIPE1r,
            IDB_OBM14_DBG_A_PIPE2r, IDB_OBM14_DBG_A_PIPE3r},
           {IDB_OBM15_DBG_A_PIPE0r, IDB_OBM15_DBG_A_PIPE1r,
            IDB_OBM15_DBG_A_PIPE2r, IDB_OBM15_DBG_A_PIPE3r},
#endif
        };

    uint32 idb_force_saf_config_reg[4] = {
           IDB_DBG_B_PIPE0r, IDB_DBG_B_PIPE1r,
           IDB_DBG_B_PIPE2r, IDB_DBG_B_PIPE3r};

    _SOC_TH_UNIT_VALIDATE(unit);

    /* Get the FEC/CL91 state of the port to choose correct CT parameters */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

    SOC_IF_ERROR_RETURN(_soc_th_port_asf_class_get(unit, port, &class));
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(class, 0);
    speed = _SOC_TH_CT_CLASS_TO_SPEED_MAP(class);

    field_a = 1;
    field_b = 1;
    field_c = 4;    /* Default value for speeds other than 25G/50G/100G */

    if ((speed == 25000) || (speed == 27000)) {
        if (port_cl91_state) {
            field_c = 10;
        }
        else {
            field_c = 6;
        }
    }
    else if ((speed == 50000) || (speed == 53000) ||
            (speed == 100000) || (speed == 106000)) {
            field_c= 6;
    }

    /* Get lane, pipe & obm */
    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    lane = phy_port - port_block_base;
    pipe = si->port_pipe[port];
    clport = si->port_serdes[port];
    if (SOC_IS_TOMAHAWK(unit)) {
        /* obm number is reversed (mirrored) in odd pipe */
        obm = pipe & 1 ? 7 - (clport & 7) : clport & 7;
    } else {
        obm = clport & 15;
    }

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        const _soc_th2_asf_core_cfg_t *_soc_th2_asf_cfg_table;
        _soc_th2_asf_cfg_table = _SOC_TH2_ASF_CFG_TBL(unit, port_cl91_state);
        field_c = (mode == _SOC_TH_ASF_MODE_SAF) ?
            _soc_th2_asf_cfg_table[class].ifsaf_threshold.saf :
            _soc_th2_asf_cfg_table[class].ifsaf_threshold.ct;

        /* TH2 only need adjust FIELD_C, others are set in _soc_tomahawk2_ing_fsaf_init */
        SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, idb_obm_force_saf_config_reg[obm][pipe],
                          REG_PORT_ANY, lane, &rval));
        soc_reg_field_set(unit, idb_obm_force_saf_config_reg[obm][pipe], &rval,
                  FIELD_Cf, field_c);
        SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit, idb_obm_force_saf_config_reg[obm][pipe],
                                  REG_PORT_ANY, lane, rval));

        return SOC_E_NONE;
    }
#endif

    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, idb_obm_force_saf_config_reg[obm][pipe],
                      REG_PORT_ANY, lane, &rval));

    soc_reg_field_set(unit, idb_obm_force_saf_config_reg[obm][pipe], &rval,
              FIELD_Af, field_a);
    soc_reg_field_set(unit, idb_obm_force_saf_config_reg[obm][pipe], &rval,
              FIELD_Bf, field_b);
    soc_reg_field_set(unit, idb_obm_force_saf_config_reg[obm][pipe], &rval,
              FIELD_Cf, field_c);

    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, idb_obm_force_saf_config_reg[obm][pipe],
                              REG_PORT_ANY, lane, rval));

    /* Now setup the IDB_FORCE_SAF_CONFIG register */
    switch (si->frequency) {
    case 850: field_a = 1700;
        break;
    case 765: field_a = 1532;
        break;
    case 672: field_a = 1344;
        break;
    case 645: field_a = 1290;
        break;
    case 545: field_a = 1090;
        break;
    default:
        break;
    }

    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, idb_force_saf_config_reg[pipe],
                      REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, idb_force_saf_config_reg[pipe], &rval,
              FIELD_Af, field_a);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, idb_force_saf_config_reg[pipe],
                              REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

int
soc_th_port_asf_valid(
    int unit,
    soc_port_t port)
{
    if (IS_CPU_PORT(((unit)), (port)) || IS_LB_PORT(((unit)), (port)) || \
        SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), (port))) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/*************************************
 * Init Start Routine                *
 *    - Allocates ASF Ctrl structure *
 *      for the given unit           *
 */
int
soc_th_asf_init_start(int unit)
{
    _SOC_TH_UNIT_VALIDATE(unit);

    if (!(_soc_th_asf_ctrl[unit] =
              sal_alloc(sizeof(_soc_th_asf_ctrl_t), "TH ASF Ctrl Area"))) {
        return SOC_E_MEMORY;
    }

    sal_memset(_soc_th_asf_ctrl[unit], 0, sizeof(_soc_th_asf_ctrl_t));

    /* Get current switch latency mode */
    SOC_IF_ERROR_RETURN(
        soc_th_latency_get(unit, &_soc_th_asf_ctrl[unit]->latency));

    /* ASF_MEM_PROFILE */
    _soc_th_asf_ctrl[unit]->asf_mem_profile =
        soc_property_get(unit, spn_ASF_MEM_PROFILE,
                         _SOC_TH_ASF_MEM_PROFILE_SIMILAR);
    if (!((_soc_th_asf_ctrl[unit]->asf_mem_profile >= 0) &&
          (_soc_th_asf_ctrl[unit]->asf_mem_profile < 3))) {
        _soc_th_asf_ctrl[unit]->asf_mem_profile = 2;
    }

    return SOC_E_NONE;
}

int
soc_asf_register_linkscan_cb(int unit)
{
    int retVal;

    /* Register linkscan callback routine */
    retVal = bcm_esw_linkscan_register(unit, soc_th_asf_linkscan_callback);
    return retVal;
}

/************************
 * Init Complete Marker *
 */
int
soc_th_asf_init_done(int unit)
{
    _SOC_TH_UNIT_VALIDATE(unit);

    if (_soc_th_asf_ctrl[unit]) {
        _soc_th_asf_ctrl[unit]->init = 1;
        LOG_DEBUG(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit, "*** unit %d: ports %s\n"), unit,
                     asf_profile_str[_soc_th_asf_ctrl[unit]->asf_mem_profile]));
        return SOC_E_NONE;
    } else {
        return SOC_E_INTERNAL;
    }
}

/******************************************************************************
 * Name: soc_th_port_asf_init                                                 *
 * Description:                                                               *
 *     Initialize ASF primitives for the specified port                       *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INIT on init/soc errors                                        *
 */
int
soc_th_port_asf_init(
    int                 unit,
    soc_port_t          port,
    int                 speed,
    soc_th_asf_mode_e   mode)
{
    int speed_encoding;

    _SOC_TH_UNIT_VALIDATE(unit);
    if (SOC_E_PARAM == (speed_encoding = _soc_th_speed_to_ct_class_map(speed))) {
        /* port on a speed unsupported by CT - will resort to SAF */
        speed_encoding = 0;
    }

    SOC_IF_ERROR_RETURN(soc_th_port_asf_mode_set(unit, port, speed, mode));

    return SOC_E_NONE;
}

/************************
 * Init Start Routine   *
 */
int
soc_th_asf_deinit(int unit)
{
    _SOC_TH_UNIT_VALIDATE(unit);
    /* Unregister linkscan callback routine */
    bcm_esw_linkscan_unregister(unit, soc_th_asf_linkscan_callback);

    if (_soc_th_asf_ctrl[unit]) {
        sal_free(_soc_th_asf_ctrl[unit]);
        _soc_th_asf_ctrl[unit] = NULL;
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_xmit_start_count_set                                *
 * Description:                                                               *
 *     Initialize XMIT START COUNT memory for all the source class for the    *
 *     specified port                                                         *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Extra cells (if any, to be added to xmit start count)                *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 */
int
soc_th_port_asf_xmit_start_count_set(
    int                  unit,
    soc_port_t           port,
    int                  port_speed,
    soc_th_asf_mode_e    mode,
    uint8                extra_cells)
{
    egr_xmit_start_count_entry_t entry;
    soc_info_t *si = &SOC_INFO(unit);
    uint8                        xmit_cnt = 0;
    int                          rv, src_class, dst_class = 0, port_idx;
    int                          pipe;
    soc_mem_t                    config_mem = INVALIDm;

    if (!((mode >= _SOC_TH_ASF_MODE_SAF) &&
          (mode <= _SOC_TH_ASF_MODE_CFG_UPDATE))) {
        return SOC_E_PARAM;
    }

    if (_SOC_TH_ASF_MODE_CFG_UPDATE != mode) {
        if (SOC_E_PARAM == (dst_class = _soc_th_speed_to_ct_class_map(port_speed))) {
            /* port on a speed unsupported by CT - will resort to SAF */
            dst_class = 0;
        }
    }

    sal_memset(&entry, 0, sizeof(egr_xmit_start_count_entry_t));

    pipe = si->port_pipe[port];
    config_mem = SOC_MEM_UNIQUE_ACC(unit, EGR_XMIT_START_COUNTm)[pipe];

    for (src_class = 0; src_class < 13; src_class++) {
        if (_SOC_TH_ASF_MODE_CFG_UPDATE == mode) {
            xmit_cnt = _SOC_TH_ASF_RETRV;
        } else {
            xmit_cnt = _SOC_TH_ASF_QUERY;
        }

#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            rv = _soc_th2_port_asf_xmit_start_count_get(unit, port, src_class,
                                                   dst_class, mode, &xmit_cnt);
        } else
#endif
        {
            rv = _soc_th_port_asf_xmit_start_count_get(unit, port, src_class,
                                                   dst_class, mode, &xmit_cnt);
        }
        if (SOC_FAILURE(rv)) {
            if (SOC_E_UNAVAIL == rv) {
                xmit_cnt = 18;
            } else {
                return rv;
            }
        }

        xmit_cnt += extra_cells;

        port_idx = ((port % 34) * 16) + src_class;

        soc_mem_field32_set(unit, config_mem, &entry,
                            THRESHOLDf, xmit_cnt);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, config_mem, MEM_BLOCK_ALL,
                                          port_idx, &entry));
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_mode_get                                             *
 * Description:                                                               *
 *     Retrieve the ASF/SAF mode configured on the specified port             *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 * OUT params:                                                                *
 *     - mode : OUT: contains the ASF/SAF mode confiugred on the specified    *
 *       port on success                                                      *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
int
soc_th_port_asf_mode_get(
    int                            unit,
    soc_port_t                     port,
    int                            port_speed,
    OUT soc_th_asf_mode_e* const   mode)
{
    uint32 rval;
    int    speed_encoding;
    uint8  max_sp, min_sp, enable;

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!mode) {
        return SOC_E_INTERNAL;
    }

    if (soc_th_port_asf_valid(unit, port)) {
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        enable = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, ENABLEf);
        if (!enable) {
            *mode = _SOC_TH_ASF_MODE_SAF;
            return SOC_E_NONE;
        }
    } else {
        *mode = _SOC_TH_ASF_MODE_SAF;
        return SOC_E_NONE;
    }

    if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    max_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, MAX_SRC_PORT_SPEEDf);
    min_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, MIN_SRC_PORT_SPEEDf);

    speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

    if (max_sp && min_sp) {
        if (max_sp == min_sp) {
            if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_sf, port)) {
                *mode = _SOC_TH_ASF_MODE_SLOW_TO_FAST;
            } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_fs, port)) {
                *mode = _SOC_TH_ASF_MODE_FAST_TO_SLOW;
            } else {
                *mode = _SOC_TH_ASF_MODE_SAME_SPEED;
            }
        } else if (max_sp == speed_encoding) {
            *mode = _SOC_TH_ASF_MODE_SLOW_TO_FAST;
        }
        else if (min_sp == speed_encoding) {
            *mode = _SOC_TH_ASF_MODE_FAST_TO_SLOW;
        } else {
            *mode = _SOC_TH_ASF_MODE_UNSUPPORTED;
            return SOC_E_UNAVAIL;
        }
    } else {
        *mode = _SOC_TH_ASF_MODE_UNSUPPORTED;
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_mode_set                                             *
 * Description:                                                               *
 *     Configure specified ASF/SAF modes on the specified port                *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * OUT params:                                                                *
 *     - None                                                                 *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
int
soc_th_port_asf_mode_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_th_asf_mode_e   mode)
{
    int           ret, speed_encoding = 0, pause_enable = 0, txp = 0, rxp = 0;
    uint8         min_sp = 0, max_sp = 0;
    uint8         fifo_threshold = 0, fifo_depth = 0, mmu_prebuf = 0;
    uint8         oversub = 0, ca_thresh = 0;
    uint8         bubble_mop_disable = 0;
    uint32        rval;
    soc_info_t   *si = NULL;
    mac_driver_t *macd;
    int           latency = SOC_SWITCH_BYPASS_MODE_NONE;
    int           init = 0;
    int           ct_class = 0, original_speed = 0;
    const _soc_th_asf_core_cfg_t *_soc_th_asf_cfg_table;
    uint32 port_cl91_state;
    uint32 ct_ok_thresh;

    _SOC_TH_UNIT_VALIDATE(unit);

    _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl;

    /* Get the FEC/CL91 state of the port to choose correct CT parameters */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

    if (port_cl91_state) {
        _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl_fec;
    }

    if (_SOC_TH_ASF_MODE_INIT == mode) {
        init = 1;
        mode = _SOC_TH_ASF_MODE_SAF;
    }
    if ((!init) && (!_soc_th_asf_ctrl[unit] ||
        !_soc_th_asf_ctrl[unit]->init || !(si = &SOC_INFO(unit)))) {
        return SOC_E_INTERNAL;
    }
    if (!_soc_th_asf_ctrl[unit]->asf_mem_profile) { /* no cut-thru profile */
        if ((_SOC_TH_ASF_MODE_SAF != mode) &&
            (_SOC_TH_ASF_MODE_CFG_UPDATE != mode)) {
            return SOC_E_UNAVAIL;
        }
    }
    if (!soc_th_port_asf_valid(unit, port)) {
        if ((_SOC_TH_ASF_MODE_SAF != mode) &&
            (_SOC_TH_ASF_MODE_CFG_UPDATE != mode)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(
            soc_th_port_asf_xmit_start_count_set(unit, port, port_speed,
                                                 mode, 0));
        return SOC_E_NONE;
    }

    speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 0);

    /* min. and max. src port speeds */
    if (_SOC_TH_ASF_MODE_CFG_UPDATE != mode) {
        min_sp = max_sp = _SOC_TH_ASF_QUERY;
        SOC_IF_ERROR_RETURN(
            _soc_th_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                              &min_sp, &max_sp));
    }

    /* mode specific configurations */
    switch(mode) {
        case _SOC_TH_ASF_MODE_SAF:
            break;

        case _SOC_TH_ASF_MODE_SAME_SPEED:
            /* Cannot set cut-through mode for 1G ports */
            if (1000 == port_speed) {
                return SOC_E_UNAVAIL;
            }
            /* verify mode specific conformance */
            if (min_sp != max_sp) {
                return SOC_E_PARAM;
            }
            break;

        case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
            /* Cannot set cut-through mode for 1G ports */
            if (1000 == port_speed) {
                return SOC_E_UNAVAIL;
            }
            /* verify mode specific conformance */
            if ( !((min_sp >=
                    _soc_th_asf_cfg_table[speed_encoding].min_sp) &&
                   (max_sp == speed_encoding)) ) {
                return SOC_E_PARAM;
            }
            break;

        case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
            /* Cannot set cut-through mode for 1G ports */
            if (1000 == port_speed) {
                return SOC_E_UNAVAIL;
            }
            /* verify mode specific conformance */
            if ( !((max_sp <=
                    _soc_th_asf_cfg_table[speed_encoding].max_sp) &&
                   (min_sp == speed_encoding)) ) {
                return SOC_E_PARAM;
            }
            break;

        case _SOC_TH_ASF_MODE_CFG_UPDATE:
            SOC_IF_ERROR_RETURN(
                _soc_th_port_asf_class_get(unit, port, &ct_class));
            original_speed = _soc_th_asf_cfg_table[ct_class].speed;
            ret = soc_th_port_asf_mode_get(unit, port, original_speed, &mode);
            if (SOC_E_NONE != ret) {
                mode = _SOC_TH_ASF_MODE_SAF;
            }

            /* disable CT first */
            SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
            soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                              FIFO_THRESHOLDf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));
            sal_usleep(1);
            soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, ENABLEf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

            /* update new settings & re-enable */
            ret = soc_th_port_asf_mode_set(unit, port, port_speed, mode);
            return ret;

        default:
            return SOC_E_PARAM;
    }

    if (SOC_IS_TOMAHAWK(unit)) {
        SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));
    }

    /* ASF_IPORT_CFG */
    SOC_IF_ERROR_RETURN(READ_ASF_IPORT_CFGr(unit, port, &rval));
    soc_reg_field_set(unit, ASF_IPORT_CFGr, &rval, ASF_PORT_SPEEDf,
                      port_speed == 1000 ? 0: speed_encoding);
    SOC_IF_ERROR_RETURN(WRITE_ASF_IPORT_CFGr(unit, port, rval));

    /* init CT class */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_class_init(unit, port, speed_encoding));

    if (_SOC_TH_ASF_MODE_SAF != mode) {
        /* Oversub */
        oversub =  (SOC_PBMP_MEMBER(si->oversub_pbm, port) ? 1 : 0);

#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            /* FIFO threshold */
            fifo_threshold = _SOC_TH_ASF_QUERY;
            SOC_IF_ERROR_RETURN(
                _soc_th2_port_asf_fifo_threshold_get(unit, port, port_speed, mode,
                                                    min_sp, &fifo_threshold));
            
            /* FIFO depth */
            fifo_depth = _SOC_TH_ASF_QUERY;
            SOC_IF_ERROR_RETURN(
                _soc_th2_port_asf_fifo_depth_get(unit, port, port_speed, mode,
                                                min_sp, &fifo_depth));
            
            /* MMU Prebuffer */
            mmu_prebuf = _SOC_TH_ASF_QUERY;
            SOC_IF_ERROR_RETURN(
                _soc_th2_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                                min_sp, &mmu_prebuf));
        } else
#endif
        {
            /* FIFO threshold */
            fifo_threshold = _SOC_TH_ASF_QUERY;
            SOC_IF_ERROR_RETURN(
                _soc_th_port_asf_fifo_threshold_get(unit, port, port_speed, mode,
                                                    min_sp, &fifo_threshold));
    
            /* FIFO depth */
            fifo_depth = _SOC_TH_ASF_QUERY;
            SOC_IF_ERROR_RETURN(
                _soc_th_port_asf_fifo_depth_get(unit, port, port_speed, mode,
                                                min_sp, &fifo_depth));
    
            /* MMU Prebuffer */
            mmu_prebuf = _SOC_TH_ASF_QUERY;
            SOC_IF_ERROR_RETURN(
                _soc_th_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                                min_sp, &mmu_prebuf));
        }

        /* Disable BubbleMOP for low latency mode */
        SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            const _soc_th2_asf_core_cfg_t *_soc_th2_asf_cfg_table;
            _soc_th2_asf_cfg_table = _SOC_TH2_ASF_CFG_TBL(unit, port_cl91_state);
            bubble_mop_disable = oversub ? 
                !(_soc_th2_asf_cfg_table[speed_encoding].mop_enable.oversub[latency]) :
                !(_soc_th2_asf_cfg_table[speed_encoding].mop_enable.line_rate[latency]);
        } else
#endif
        {
            /* special cases: 10G & HG[11] */
            if ((SOC_SWITCH_BYPASS_MODE_LOW == latency) &&
                ((1 == speed_encoding) || (2 == speed_encoding))) {
                bubble_mop_disable = 1;
            } else {
                bubble_mop_disable = 0;
            }
        }

        pause_enable = 0;
        ca_thresh = 4;
        ct_ok_thresh = 0;

        if (port_cl91_state) {
            if ((port_speed == 50000) || (port_speed == 53000) ||
                (port_speed == 100000) || (port_speed == 106000)) {
                ca_thresh = 6;
            }
            if ((port_speed == 25000) || (port_speed == 27000)) {
                ct_ok_thresh = (SOC_IS_TOMAHAWK2(unit) ? 15 : 5);
            }
        }

        SOC_IF_ERROR_RETURN(
            _soc_th_asf_obm_ct_thresh_ok_set(unit, port, ct_ok_thresh));

        SOC_IF_ERROR_RETURN(
            soc_th_port_asf_params_set(unit, port, port_speed, mode,
                                       bubble_mop_disable, ca_thresh,
                                       speed_encoding));

        if (SOC_IS_TOMAHAWK(unit)) {
            /* PAUSE */
            SOC_IF_ERROR_RETURN(MAC_PAUSE_GET(macd, unit, port, &txp, &rxp));
            if (rxp) {
                SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->pause_restore, port);
            }
            rxp = pause_enable;
            SOC_IF_ERROR_RETURN(MAC_PAUSE_SET(macd, unit, port, txp, rxp));
        }

        /* drain the port CT FIFO */
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, ENABLEf, 1);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, FIFO_THRESHOLDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        sal_usleep(1);

        /* enable cut-through */
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, OVERSUBf, oversub);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MAX_SRC_PORT_SPEEDf, max_sp);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MIN_SRC_PORT_SPEEDf, min_sp);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MIN_SRC_PORT_XMIT_CNTf, mmu_prebuf);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          FIFO_THRESHOLDf, fifo_threshold);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, FIFO_DEPTHf,
                          fifo_depth);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        /* bookkeeping */
        if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_ss, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_ss, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_sf, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_sf, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_fs, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_fs, port);
        }
        SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->asf_ports, port);
        if (_SOC_TH_ASF_MODE_SAME_SPEED == mode) {
            SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->asf_ss, port);
        } else if (_SOC_TH_ASF_MODE_SLOW_TO_FAST== mode) {
            SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->asf_sf, port);
        } else if (_SOC_TH_ASF_MODE_FAST_TO_SLOW== mode) {
            SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->asf_fs, port);
        }
    } else {   /* Store and Forward */
        fifo_depth = 0x40;
        fifo_threshold = 0x20;
        mmu_prebuf = 0;
        min_sp = 0;
        max_sp = 0;
        oversub = 0;
        pause_enable = 1;
        ca_thresh = 4;
        bubble_mop_disable = 0;

        if (1000 == port_speed) {
            pause_enable = 0;
        }

        if (SOC_IS_TOMAHAWK(unit)) {
            SOC_IF_ERROR_RETURN(
                _soc_th_asf_obm_ct_thresh_ok_set(unit, port, 0));
        }

        SOC_IF_ERROR_RETURN(
            soc_th_port_asf_params_set(unit, port, port_speed, mode,
                                       bubble_mop_disable, ca_thresh,
                                       speed_encoding));

        /* ASF_EPORT_CFG */
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, FIFO_THRESHOLDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        sal_usleep(1);

        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, ENABLEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        /* update SAF configs */
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, OVERSUBf, oversub);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MAX_SRC_PORT_SPEEDf, max_sp);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MIN_SRC_PORT_SPEEDf, min_sp);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MIN_SRC_PORT_XMIT_CNTf, mmu_prebuf);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          FIFO_THRESHOLDf, fifo_threshold);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, FIFO_DEPTHf,
                          fifo_depth);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        if (SOC_IS_TOMAHAWK(unit)) {
            /* restore pause */
            if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->pause_restore, port)) {
                SOC_IF_ERROR_RETURN(MAC_PAUSE_GET(macd, unit, port, &txp, &rxp));
                rxp = 1;
                SOC_IF_ERROR_RETURN(MAC_PAUSE_SET(macd, unit, port, txp, rxp));
                SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->pause_restore, port);
            }
        }

        /* bookkeeping */
        SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_ports, port);
        if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_ss, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_ss, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_sf, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_sf, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_fs, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_fs, port);
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_params_set                                           *
 * Description:                                                               *
 *     Configure ASF/SAF mode params on the specified port                    *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Bubble MOP disable flag                                              *
 *     - CA Threshold                                                         *
 *     - Speed Encoding                                                       *
 * OUT params:                                                                *
 *     - None                                                                 *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
int
soc_th_port_asf_params_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_th_asf_mode_e   mode,
    uint8               bubble_mop_disable,
    uint8               ca_thresh,
    int                 speed_encoding)
{
    uint32        mmu_ep_credits = 0, egr_mmu_cell_credits = 0;
    uint32        rval;
    soc_info_t   *si = NULL;
    uint8  oversub;
#if defined(BCM_TOMAHAWK2_SUPPORT)
    egr_mmu_cell_credit_entry_t credit_entry;
#endif
    const _soc_th_asf_core_cfg_t *_soc_th_asf_cfg_table;
    uint32 port_cl91_state;

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    /* If oversub mode, setup Ingress Force SAF registers */
    oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);
    if (oversub) {
        _soc_th_asf_force_saf_config_set(unit, port, mode);
    }

    _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl;

    /* Get the FEC/CL91 state of the port to choose correct CT parameters */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

    if (port_cl91_state) {
        _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl_fec;
    }

    /* EDB Prebuffer */
    SOC_IF_ERROR_RETURN(
        soc_th_port_asf_xmit_start_count_set(unit, port, port_speed,
                                             mode, 0));
    /* OBM Cell Assembly FIFO threshold */
    SOC_IF_ERROR_RETURN(
        _soc_th_asf_obm_ca_fifo_thresh_set(unit, port, ca_thresh));

    /* BubbleMOP - At present only L3 & Full Latency Mode supported */
    SOC_IF_ERROR_RETURN(
        _soc_th_asf_obm_bubble_mop_set(unit, port, bubble_mop_disable));

    /* MMU EP Credit Threshold */
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        int ovs_ratio = 0;
        uint8 mmu_ep_credits_lo = 0;
        const _soc_th2_asf_core_cfg_t *_soc_th2_asf_cfg_table;

        _soc_th2_asf_cfg_table = _SOC_TH2_ASF_CFG_TBL(unit, port_cl91_state);

        SOC_IF_ERROR_RETURN(
            soc_th_port_oversub_ratio_get(unit, port, &ovs_ratio));
        if ((1700 == si->frequency) && (ovs_ratio <= 1500)) {
            mmu_ep_credits_lo = 
                _soc_th2_asf_cfg_table[speed_encoding].mmu_credit_threshold.lr_os_1_5.lo;
            mmu_ep_credits = 
                _soc_th2_asf_cfg_table[speed_encoding].mmu_credit_threshold.lr_os_1_5.hi;
        } else {
            mmu_ep_credits_lo = 
                _soc_th2_asf_cfg_table[speed_encoding].mmu_credit_threshold.os_2_1.lo;
            mmu_ep_credits = 
                _soc_th2_asf_cfg_table[speed_encoding].mmu_credit_threshold.os_2_1.hi;
        }
        
        /* ASF_CREDIT_THRESH_LO */
        SOC_IF_ERROR_RETURN(READ_ASF_CREDIT_THRESH_LOr(unit, port, &rval));
        soc_reg_field_set(unit, ASF_CREDIT_THRESH_LOr, &rval, THRESHf,
                          mmu_ep_credits_lo);
        SOC_IF_ERROR_RETURN(WRITE_ASF_CREDIT_THRESH_LOr(unit, port, rval));
    } else
#endif
    {
        if (CCLK_FREQ_850MHZ <= si->frequency) {
            mmu_ep_credits =
            _soc_th_asf_cfg_table[speed_encoding].mmu_ep_credit.pfc_op_cells;
        } else {
            mmu_ep_credits =
            _soc_th_asf_cfg_table[speed_encoding].mmu_ep_credit.base_cells;
        }
    }
    /* ASF_CREDIT_THRESH_HI */
    SOC_IF_ERROR_RETURN(READ_ASF_CREDIT_THRESH_HIr(unit, port, &rval));
    soc_reg_field_set(unit, ASF_CREDIT_THRESH_HIr, &rval, THRESHf,
                      mmu_ep_credits);
    SOC_IF_ERROR_RETURN(WRITE_ASF_CREDIT_THRESH_HIr(unit, port, rval));

    /* EP credits outstanding */
    SOC_IF_ERROR_RETURN(
        soc_th_port_asf_speed_to_mmu_cell_credit(unit, port, port_speed, 
            &egr_mmu_cell_credits));

    /* EGR_MMU_CELL_CREDIT */
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_mmu_cell_credit_is_memory)) {
        SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                            si->port_l2p_mapping[port], &credit_entry));
        soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &credit_entry, CREDITf,
                                             egr_mmu_cell_credits);
        SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ALL,
                            si->port_l2p_mapping[port], &credit_entry));
    } else
#endif
    {
        if (SOC_MEM_IS_VALID(unit, EGR_MMU_CELL_CREDITm)) {
            SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, EGR_MMU_CELL_CREDITm,
                                    port, CREDITf, egr_mmu_cell_credits));
        } else {
            /* EGR_MMU_CELL_CREDIT */
            SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITr(unit, port, &rval));
            soc_reg_field_set(unit, EGR_MMU_CELL_CREDITr, &rval, CREDITf,
                              egr_mmu_cell_credits);
            SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITr(unit, port, rval));
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_speed_to_mmu_cell_credit                             *
 * Description:                                                               *
 *     Retrieve mmu_cell_credit based on port speed configured.               *
 *     Helper function for misc init.                                         *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 * OUT params:                                                                *
 *     - MMU Cell credit                                                      *
 * Returns:                                                                   *
 *     - SOC_E_XXX on success                                                 *
 */
int
soc_th_port_asf_speed_to_mmu_cell_credit(
    int        unit,
    soc_port_t port,
    int        port_speed,
    OUT uint32 *mmu_cell_credit)
{
    int          speed_encoding = 0;
    soc_info_t   *si = NULL;
    const _soc_th_asf_core_cfg_t *_soc_th_asf_cfg_table;
    uint32 port_cl91_state;

    _SOC_TH_UNIT_VALIDATE(unit);

    if (NULL == mmu_cell_credit) {
        return SOC_E_PARAM;
    }

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl;

    /* Get the FEC/CL91 state of the port to choose correct CT parameters */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

    if (port_cl91_state) {
        _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl_fec;
    }

    speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 0);
    
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        int ovs_ratio = 0;
        const _soc_th2_asf_core_cfg_t *_soc_th2_asf_cfg_table;

        _soc_th2_asf_cfg_table = _SOC_TH2_ASF_CFG_TBL(unit, port_cl91_state);
        SOC_IF_ERROR_RETURN(
            soc_th_port_oversub_ratio_get(unit, port, &ovs_ratio));
        if (IS_CPU_PORT(unit, port)) {
            speed_encoding = 3;     /* 20G for CPU */
        } else if (IS_LB_PORT(unit, port)) {
            speed_encoding = 11;    /* 100G for LB */
        }
        /* At MAX FREQ, the ovs_ratio will always be less than 3:2;
         * Still add that check
         */
        if ((1700 == si->frequency) && (ovs_ratio <= 1500)) {
            *mmu_cell_credit = 
                _soc_th2_asf_cfg_table[speed_encoding].ep_credit.pfc_op_cells;
        } else {
            *mmu_cell_credit = 
                _soc_th2_asf_cfg_table[speed_encoding].ep_credit.base_cells;
        }
    } else
#endif
    {
        if (CCLK_FREQ_850MHZ == si->frequency) {
            *mmu_cell_credit = (speed_encoding ?
                _soc_th_asf_cfg_table[speed_encoding].egr_mmu_credit.pfc_op_cells : 
                11);
        } else {
            *mmu_cell_credit = (speed_encoding ?
                _soc_th_asf_cfg_table[speed_encoding].egr_mmu_credit.base_cells :
                13);
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_mmu_cell_credit_to_port_speed                        *
 * Description:                                                               *
 *     Retrieve port speed based on mmu_cell_credit configured.               *
 *     Helper function for MAC driver speed get.                              *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - MMU Cell credit                                                      *
 * OUT params:                                                                *
 *     - Port speed                                                           *
 * Returns:                                                                   *
 *     - SOC_E_XXX on success                                                 *
 */
int
soc_th_port_asf_mmu_cell_credit_to_speed(
    int        unit,
    soc_port_t port,
    uint8      mmu_cell_credit,
    OUT int    *port_speed)
{
    int i, freq;
    uint8 cell_credit;
    int speed;
    soc_info_t   *si = NULL;
    const _soc_th_asf_core_cfg_t *_soc_th_asf_cfg_table;
    uint32 port_cl91_state;
#if defined (BCM_TOMAHAWK_SUPPORT)
    uint8  oversub;
    int port_mode;
    uint32 rval;
#endif

    _SOC_TH_UNIT_VALIDATE(unit);

    if (NULL == port_speed) {
        return SOC_E_PARAM;
    }

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

#if defined (BCM_TOMAHAWK_SUPPORT)
    oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);
#endif

    /* Special case for 1G ports since 1G ports use same setting as 10G port */
    if (1000 == (speed = si->port_init_speed[port])) {  /* Use cached speed instead */
        *port_speed = speed;
        return SOC_E_NONE;
    }

    _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl;

    /* Get the FEC/CL91 state of the port to choose correct CT parameters */
    SOC_IF_ERROR_RETURN(_soc_th_port_asf_port_cl91_get(unit, port, &port_cl91_state));

    if (port_cl91_state) {
        _soc_th_asf_cfg_table = _soc_th_asf_cfg_tbl_fec;
    }

    freq = si->frequency;
    for (i = 1; i < (COUNTOF(_soc_th_asf_cfg_tbl) - 1); i++) {
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            int ovs_ratio = 0;
            const _soc_th2_asf_core_cfg_t *_soc_th2_asf_cfg_table;

            _soc_th2_asf_cfg_table = _SOC_TH2_ASF_CFG_TBL(unit, port_cl91_state);
            SOC_IF_ERROR_RETURN(
                soc_th_port_oversub_ratio_get(unit, port, &ovs_ratio));
            /* At max frequency, use the 1.5 ratio settings.
             * For all other cases, use the 1.5 ratio as the threshold to
             * determine which settings to use.
             */
            cell_credit = ((1700 == freq) && (ovs_ratio <= 1500)) ?
                _soc_th2_asf_cfg_table[i].ep_credit.pfc_op_cells :
                _soc_th2_asf_cfg_table[i].ep_credit.base_cells;
        } else
#endif
        {
            cell_credit = (CCLK_FREQ_850MHZ == freq) ?
                _soc_th_asf_cfg_table[i].egr_mmu_credit.pfc_op_cells :
                _soc_th_asf_cfg_table[i].egr_mmu_credit.base_cells;
        }
        if (mmu_cell_credit == cell_credit) {
#if defined (BCM_TOMAHAWK_SUPPORT)
            if (oversub &&
                mmu_cell_credit == 16) { /* 16 is EDB credit value for 25G/20G */
                SOC_IF_ERROR_RETURN(READ_CLPORT_MODE_REGr(unit, port, &rval));
                port_mode = soc_reg_field_get(unit, CLPORT_MODE_REGr, rval,
                                              XPORT0_CORE_PORT_MODEf);
                if (!port_mode) {
                    *port_speed = IS_HG_PORT(unit, port) ? 27000 : 25000;
                    return SOC_E_NONE;
                }
            }
#endif
            if (IS_HG_PORT(unit, port)) {
                *port_speed = _soc_th_asf_cfg_table[i + 1].speed;
            } else {
                *port_speed = _soc_th_asf_cfg_table[i].speed;
            }
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            if (SOC_E_NONE !=
                soc_th_port_speed_supported(unit, port, *port_speed)) {
                continue;
            }
        }
#endif
            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

/******************************************************************************
 * Name: soc_th_port_asf_pause_bpmp_get                                       *
 * Description:                                                               *
 *     Get pointer of pause_restore bitmap for update                         *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 * OUT params:                                                                *
 *                                                                            *
 * Returns:                                                                   *
 *     - Pointer of pause_restore bitmap                                      *
 */
pbmp_t *
soc_th_port_asf_pause_bpmp_get(int unit)
{
    if (_soc_th_asf_ctrl[unit] == NULL) {
        return NULL;
    }
    return &_soc_th_asf_ctrl[unit]->pause_restore;
}

/************************
 * ASF Warmboot Support *
 */
#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
#define BCM_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_8

int
soc_th_asf_wb_memsz_get(
    int               unit,
    OUT uint32* const mem_sz,
    uint16            scache_ver)
{
    _SOC_TH_UNIT_VALIDATE(unit);
    if (!mem_sz) {
        return SOC_E_PARAM;
    }
    *mem_sz = 0;

    if (!SOC_WARM_BOOT(unit)) {
        if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->asf_mem_profile) {
            return SOC_E_UNAVAIL;
        }
        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }
    }

    *mem_sz = sizeof(_soc_th_asf_wb_t);

    if (scache_ver < BCM_WB_VERSION_1_8) {
        /* adjust wb area sz for lower versions */
        *mem_sz -= sizeof(int);
    }

    return SOC_E_NONE;
}

int
soc_th_asf_wb_sync(
    int    unit,
    IN_OUT uint8* const wb_data)
{
    _soc_th_asf_wb_t *wbd;

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->asf_mem_profile) {
        return SOC_E_UNAVAIL;
    }
    if (!_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_th_asf_wb_t *) wb_data;
    wbd->unit = unit;
    sal_memcpy(&wbd->asf_ctrl, _soc_th_asf_ctrl[unit],
               sizeof(_soc_th_asf_ctrl_t));

    return SOC_E_NONE;
}

int
soc_th_asf_wb_recover(
    int          unit,
    uint8* const wb_data,
    uint16       scache_ver)
{
    _soc_th_asf_wb_t *wbd;
    int adj = 0; /*scache ver adjustment */

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!SOC_WARM_BOOT(unit)) {
        return SOC_E_INTERNAL;
    }
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_th_asf_wb_t *) wb_data;
    if (wbd->unit == unit) {
        if (!(_soc_th_asf_ctrl[unit] =
                   sal_alloc(sizeof(_soc_th_asf_ctrl_t), "TH ASF Ctrl Area"))) {
            return SOC_E_MEMORY;
        }

        if (scache_ver < BCM_WB_VERSION_1_8) {
            /* adjust for lower versions */
            adj = sizeof(int);
            /* backfill latency querying latency subsystem */
            SOC_IF_ERROR_RETURN(
                soc_th_latency_get(unit, &_soc_th_asf_ctrl[unit]->latency));
        }

        sal_memcpy(_soc_th_asf_ctrl[unit], &wbd->asf_ctrl,
                   (sizeof(_soc_th_asf_ctrl_t) - adj));
    }

    LOG_CLI((BSL_META_U(unit, "*** unit %d: MMU-ASF subsystem warmbooted\n"),
                        unit));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/**********************
 * ASF Debug Support  *
 */
int
soc_th_asf_pbmp_get(int unit)
{
    char pfmt[SOC_PBMP_FMT_LEN];

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    LOG_CLI(("ASF PBMP: %s\n",
             SOC_PBMP_FMT(_soc_th_asf_ctrl[unit]->asf_ports, pfmt)));

    return SOC_E_NONE;
}

int
soc_th_port_asf_show(
    int        unit,
    soc_port_t port,
    int        port_speed)
{
    int ret;
    char mode_str[4][15] = {"SAF", "Same speed  ", "Slow to Fast",
                             "Fast to Slow"};
    char speed_str[13][10] = {"SAF", "10G", "HG[11]", "20G", "HG[21]", "25G",
                               "HG[27]", "40G", "HG[42]", "50G", "HG[53]",
                               "100G", "HG[106]"};
    soc_th_asf_mode_e mode = _SOC_TH_ASF_RETRV;
    uint8 min_speed, max_speed;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return BCM_E_NONE;
    }

    min_speed = max_speed = _SOC_TH_ASF_RETRV;
    ret = soc_th_port_asf_mode_get(unit, port, port_speed, &mode);
    if (SOC_E_UNAVAIL == ret) {
        mode = _SOC_TH_ASF_MODE_SAF;
    } else if (SOC_E_NONE != ret) {
        return ret;
    }

    LOG_CLI(("%-5s     %-11s      ", SOC_PORT_NAME(unit, port), mode_str[mode]));

    if (_SOC_TH_ASF_MODE_SAF == mode) {
        LOG_CLI(("    .. NA ..\n"));
    } else if (_SOC_TH_ASF_MODE_FAST_TO_SLOW == mode) {
        ret = _soc_th_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                                &min_speed, &max_speed);
        LOG_CLI(("%s / %s\n", speed_str[max_speed], speed_str[min_speed]));
    } else {
        ret = _soc_th_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                                &min_speed, &max_speed);
        LOG_CLI(("%s / %s\n", speed_str[min_speed], speed_str[max_speed]));
    }

    return SOC_E_NONE;
}

int soc_th_asf_config_dump(int unit)
{
    char asf_profile_str[3][25] = {"No cut-thru support",
                                   "Similar speed cut-thru",
                                   "Extreme speed cut-thru"};

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    LOG_CLI(("ASF Profile: %s\n\n",
             asf_profile_str[_soc_th_asf_ctrl[unit]->asf_mem_profile]));

    return SOC_E_NONE;
}

int
soc_th_port_asf_config_dump(
    int        unit,
    soc_port_t port,
    int        port_speed)
{
    int    txp = 0, rxp = 0, ct_class = 0;
    uint8  src_class = 0, dst_class = 0;
    uint8  min_sp = 0, max_sp = 0;
    uint8  fifo_threshold = 0, fifo_depth = 0, mmu_prebuf = 0;
    uint8  xmit_start_cnt[13] = {0};
    uint32 rval, mmu_ep_credits = 0, egr_mmu_cell_credits = 0;
    mac_driver_t *macd;
    soc_th_asf_mode_e mode = _SOC_TH_ASF_RETRV;
#if defined(BCM_TOMAHAWK2_SUPPORT)
    egr_mmu_cell_credit_entry_t credit_entry;
#endif

    _SOC_TH_UNIT_VALIDATE(unit);

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return BCM_E_NONE;
    }

    if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(
        soc_th_port_asf_mode_get(unit, port, port_speed, &mode));

    if (soc_th_port_asf_valid(unit, port)) {
        min_sp = max_sp = fifo_threshold = _SOC_TH_ASF_RETRV;
        fifo_depth = mmu_prebuf = _SOC_TH_ASF_RETRV;

        SOC_IF_ERROR_RETURN(
            _soc_th_port_asf_class_get(unit, port, &ct_class));
        SOC_IF_ERROR_RETURN(
            _soc_th_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                              &min_sp, &max_sp));
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        SOC_IF_ERROR_RETURN(
            _soc_th2_port_asf_fifo_threshold_get(unit, port, port_speed, mode,
                                                min_sp, &fifo_threshold));
        
        SOC_IF_ERROR_RETURN(
            _soc_th2_port_asf_fifo_depth_get(unit, port, port_speed, mode,
                                            min_sp, &fifo_depth));
        
        SOC_IF_ERROR_RETURN(
            _soc_th2_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                            min_sp, &mmu_prebuf));
        } else
#endif
        {
            SOC_IF_ERROR_RETURN(
                _soc_th_port_asf_fifo_threshold_get(unit, port, port_speed, mode,
                                                    min_sp, &fifo_threshold));
            SOC_IF_ERROR_RETURN(
                _soc_th_port_asf_fifo_depth_get(unit, port, port_speed, mode,
                                                min_sp, &fifo_depth));
            SOC_IF_ERROR_RETURN(
                _soc_th_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                                min_sp, &mmu_prebuf));
        }
        SOC_IF_ERROR_RETURN(READ_ASF_CREDIT_THRESH_HIr(unit, port, &rval));
        mmu_ep_credits = soc_reg_field_get(unit, ASF_CREDIT_THRESH_HIr,
                                           rval, THRESHf);
    }
        
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        dst_class = _soc_th_speed_to_ct_class_map(port_speed);
        for (src_class = 0; src_class <= 12; src_class++) {
            xmit_start_cnt[src_class] = _SOC_TH_ASF_RETRV;
            SOC_IF_ERROR_RETURN(
                _soc_th2_port_asf_xmit_start_count_get(unit, port, src_class,
                                                      dst_class, mode,
                                                      &xmit_start_cnt[src_class]));
        }
    } else
#endif
    {
        dst_class = _soc_th_speed_to_ct_class_map(port_speed);
        for (src_class = 0; src_class <= 12; src_class++) {
            xmit_start_cnt[src_class] = _SOC_TH_ASF_RETRV;
            SOC_IF_ERROR_RETURN(
                _soc_th_port_asf_xmit_start_count_get(unit, port, src_class,
                                                      dst_class, mode,
                                                      &xmit_start_cnt[src_class]));
        }
    }

    /*
     * COVERITY
     *
     * This fault is false detected. As min_sp and max_sp will be initialized to 0 inside
     * _soc_th_port_asf_speed_limits_get and will not be 255(_SOC_TH_ASF_RETRV) when code
     * runs to code paragraph.
     */
    /* coverity[overrun-local] */
    LOG_CLI(("%-5s %-3d %-3d %-6d %-6d %-3d %-3d   ", SOC_PORT_NAME(unit, port),
             mode, ct_class, _SOC_TH_CT_CLASS_TO_SPEED_MAP(min_sp),
            _SOC_TH_CT_CLASS_TO_SPEED_MAP(max_sp), fifo_threshold,
            fifo_depth));
    for (src_class = 0; src_class <= 12; src_class++) {
        LOG_CLI(("%-3d ", xmit_start_cnt[src_class]));
    }


#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_mmu_cell_credit_is_memory)) {
        SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                            SOC_INFO(unit).port_l2p_mapping[port], &credit_entry));
        egr_mmu_cell_credits = soc_EGR_MMU_CELL_CREDITm_field32_get(unit,
                               &credit_entry, CREDITf);
    } else
#endif
    {
        SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITr(unit, port, &rval));
        egr_mmu_cell_credits = soc_reg_field_get(unit, EGR_MMU_CELL_CREDITr,
                                                 rval, CREDITf);
    }

    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));
    SOC_IF_ERROR_RETURN(MAC_PAUSE_GET(macd, unit, port, &txp, &rxp));

    LOG_CLI((" %-3d %-3d %-3d %-3d\n", mmu_prebuf, mmu_ep_credits,
             egr_mmu_cell_credits, rxp));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_detach                                              *
 * Description:                                                               *
 *     Update ASF when port deleted during Flexport operation                 *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 */
int
soc_th_port_asf_detach(int unit, soc_port_t port)
{
    _SOC_TH_UNIT_VALIDATE(unit);

    if (_soc_th_asf_ctrl[unit] == NULL) {
        return SOC_E_INIT;
    }

    SOC_IF_ERROR_RETURN
        (soc_th_port_asf_mode_set(unit, port,
                                  SOC_INFO(unit).port_init_speed[port],
                                  _SOC_TH_ASF_MODE_SAF));

    if (soc_th_port_asf_valid(unit, port)) {
        SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_ports, port);
        SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->pause_restore, port);
        if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_ss, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_ss, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_sf, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_sf, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_fs, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_fs, port);
        }
    }
    return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK_SUPPORT */


/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      asf.c
 * Purpose:   ASF feature support for Trident3 SKUs
 * Requires:  soc/trident3.h
 */

#include <soc/trident3.h>

#if defined(BCM_TRIDENT3_SUPPORT)

#include <appl/diag/system.h>
#include <soc/drv.h>
#include <soc/ll.h>
#include <soc/property.h>
#include <shared/bsl.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif


#define _SOC_TD3_ASF_CLASS_CNT  13      /* 0 for SAF, 13-15 are reserved. */
#define _SOC_TD3_ASF_QUERY      0xfe
#define _SOC_TD3_ASF_RETRV      0xff

#define _SOC_TD3_CT_CLASS_TO_SPEED_MAP(ct_class) \
        _soc_td3_asf_cfg_tbl[(ct_class)].speed

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(ct_class, floor) \
        do {                                            \
            if (!(((ct_class) >= (floor)) &&                \
                  ((ct_class) < _SOC_TD3_ASF_CLASS_CNT))) \
                return SOC_E_PARAM;                         \
        } while (0)

#define _SOC_TD3_ASF_PORT_VALIDATE(unit, port)      \
        do {                                        \
            if (IS_CPU_PORT(((unit)), (port)) ||    \
                IS_LB_PORT(((unit)), (port)) ||     \
                SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), (port))) \
                return SOC_E_UNAVAIL;               \
        } while (0)

/* EP Credits Outstanding */
typedef struct _soc_td3_ep_credit_s {
    uint8 base_cells;      /* Baseline cells */
    uint8 pfc_op_cells;    /* PFC Optimized cells */
} _soc_td3_ep_credit_t;

/* CT FIFO Threshold and Depth */
typedef struct _soc_td3_ct_fifo_profile_s {
    uint8 line_rate;       /* Line-rate profile */
    uint8 oversub;         /* General profile */
} _soc_td3_ct_fifo_profile_t;

typedef struct _soc_td3_ct_fifo_s {
    _soc_td3_ct_fifo_profile_t ss;      /* Same-Speed */
    _soc_td3_ct_fifo_profile_t sf;      /* Slow-to-Fast */
    _soc_td3_ct_fifo_profile_t fs;      /* Fast-to-Slow */
}_soc_td3_ct_fifo_t;

/* Bubble MOP */
typedef struct _soc_td3_mop_s {
    uint8 line_rate;           /* Line-rate profile */
    uint8 oversub;             /* Oversub profile */
} _soc_td3_mop_t;

/* EDB Prebuffer */
typedef struct _soc_td3_edb_prebuf_profile_s {
    uint8 saf;  /* prebuffer for store-and-forward. 16B granularity. */
    uint8 ct;   /* prebuffer for cut-through. 16B granularity. */
} _soc_td3_edb_prebuf_profile_t;
typedef struct _soc_td3_edb_prebuf_s {
    _soc_td3_edb_prebuf_profile_t line_rate;
    _soc_td3_edb_prebuf_profile_t oversub_161;  /* 1.61 Oversub */
    _soc_td3_edb_prebuf_profile_t oversub_2_1;  /* 2:1 Oversub */
} _soc_td3_edb_prebuf_t;

/* MMU Credit Thresholds */
typedef struct _soc_td3_mmu_credit_threshold_profile_s {
    uint8 lo;              /* MMU High Credit Threshold */
    uint8 hi;              /* MMU Low Credit Threshold */
} _soc_td3_mmu_credit_threshold_profile_t;
typedef struct _soc_td3_mmu_credit_threshold_s {
    _soc_td3_mmu_credit_threshold_profile_t lr_os_161;  /* line-rate and os ratio <= 1.61 */
    _soc_td3_mmu_credit_threshold_profile_t os_2_1;     /* os ratio > 1.61 */
} _soc_td3_mmu_credit_threshold_t;

/* IFSAF Threshold */
typedef struct _soc_td3_ifsaf_threshold_s {
    uint8 saf;
    uint8 ct;
}_soc_td3_ifsaf_threshold_t;

/* ASF core config */
typedef struct _soc_td3_asf_core_cfg_s {
    int                              speed;
    uint8                            min_sp;                   /* Min. Src. Port Speed */
    uint8                            max_sp;                   /* Max. Src. Port Speed */
    uint8                            mmu_prebuffer;            /* MMU prebuffer */
    _soc_td3_ct_fifo_t               ct_fifo_threshold;        /* Same-speed CT FIFO Threshold */
    _soc_td3_ct_fifo_t               ct_fifo_depth;            /* Same-speed CT FIFO Depth */
    uint8                            fast_to_slow_acc;         /* Fast to slow CT Accumulation */
    _soc_td3_mop_t                   mop_enable;               /* Bubble MOP enable */
    _soc_td3_edb_prebuf_t            edb_prebuffer;            /* EDB prebuffer. xmit start cnt */
    _soc_td3_mmu_credit_threshold_t  mmu_credit_threshold;     /* MMU Credit Threshold */
    _soc_td3_ep_credit_t             ep_credit;                /* EP Credit */
    _soc_td3_ifsaf_threshold_t       ca_thresh;                /* CA_CT_CONTROL threshold0 */
    _soc_td3_ifsaf_threshold_t       ct_ok_thresh;             /* CT_THRESHOLD cut_through_ok */
    _soc_td3_ifsaf_threshold_t       unsat_thresh;             /* FORCE_SAF_CONFIG unsatisfied_threshold */
    /* NOTE: DON'T ALTER FIELD ORDER */
} _soc_td3_asf_core_cfg_t;

/* ASF ctrl */
typedef struct _soc_td3_asf_ctrl_s {
    uint8                     init;
    pbmp_t                    asf_ports;
    pbmp_t                    pause_restore;
    pbmp_t                    asf_ss;
    pbmp_t                    asf_sf;
    pbmp_t                    asf_fs;
    soc_td3_asf_mem_profile_e asf_mem_profile;
} _soc_td3_asf_ctrl_t;

#ifdef BCM_WARM_BOOT_SUPPORT
/* ASF Warmboot */
typedef struct _soc_td3_asf_wb_s {
    int unit;
    _soc_td3_asf_ctrl_t asf_ctrl;
} _soc_td3_asf_wb_t;
#endif

/* ASF Core Config Table  */
static const _soc_td3_asf_core_cfg_t
_soc_td3_asf_cfg_tbl[_SOC_TD3_ASF_CLASS_CNT] = {
    {    -1,  0,  0,  0, {{0,  0}, { 0,  0}, {0,  0}}, {{0,  0}, { 0,  0}, { 0,  0}},  0, {0, 0}, {{0,  0}, { 0,  0}, { 0,   0}}, {{0,  0}, {0,  0}}, { 0,  0}, {0, 0}, { 0,  0}, { 0,  0}},   /* SAF     */
    { 10000,  1, 10,  6, {{3, 10}, { 9, 16}, {3, 10}}, {{8, 18}, {13, 24}, {38, 48}}, 31, {0, 0}, {{7, 32}, {15, 49}, {18,  56}}, {{2,  4}, {2,  6}}, {11, 11}, {4, 4}, { 0,  0}, {12, 12}},   /* 10GE    */
    { 11000,  1, 10,  6, {{3, 10}, { 9, 16}, {3, 10}}, {{8, 18}, {13, 24}, {38, 48}}, 31, {0, 0}, {{7, 32}, {15, 49}, {18,  56}}, {{2,  4}, {2,  6}}, {11, 11}, {4, 4}, { 0,  0}, {12, 12}},   /* HG[11]  */
    { 20000,  3, 10,  6, {{3, 10}, { 9, 16}, {3, 10}}, {{8, 25}, {14, 31}, {32, 48}}, 24, {1, 1}, {{7, 13}, {15, 58}, {18,  67}}, {{2,  7}, {2, 11}}, {16, 13}, {4, 4}, { 0,  0}, {12, 12}},   /* 20GE    */
    { 21000,  3, 10,  6, {{3, 10}, { 9, 16}, {3, 10}}, {{8, 25}, {14, 31}, {32, 48}}, 24, {1, 1}, {{7, 13}, {15, 58}, {18,  67}}, {{2,  7}, {2, 11}}, {16, 13}, {4, 4}, { 0,  0}, {12, 12}},   /* HG[21]  */
    { 25000,  5, 10,  7, {{3, 12}, {11, 20}, {3, 12}}, {{8, 32}, {16, 40}, {29, 50}}, 21, {1, 1}, {{7, 72}, {15, 91}, {18, 100}}, {{3,  7}, {3, 11}}, {20, 18}, {4, 4}, {22, 22}, {26, 26}},   /* 25GE    */
    { 27000,  5, 10,  7, {{3, 12}, {11, 20}, {3, 12}}, {{8, 32}, {16, 40}, {29, 50}}, 21, {1, 1}, {{7, 72}, {15, 91}, {18, 100}}, {{3,  7}, {3, 11}}, {20, 18}, {4, 4}, {22, 22}, {26, 26}},   /* HG[27]  */
    { 40000,  7, 12,  4, {{3, 10}, { 7, 14}, {3, 10}}, {{8, 30}, {12, 34}, {32, 48}}, 24, {1, 1}, {{7, 13}, {15, 29}, {18,  36}}, {{4, 13}, {4, 21}}, {25, 19}, {4, 4}, { 0,  0}, {12, 12}},   /* 40GE    */
    { 42000,  7, 12,  4, {{3, 10}, { 7, 14}, {3, 10}}, {{8, 30}, {12, 34}, {32, 48}}, 24, {1, 1}, {{7, 13}, {15, 29}, {18,  36}}, {{4, 13}, {4, 21}}, {25, 19}, {4, 4}, { 0,  0}, {12, 12}},   /* HG[42]  */
    { 50000,  9, 12,  4, {{3, 12}, { 7, 16}, {3, 12}}, {{8, 40}, {12, 44}, {29, 50}}, 21, {1, 1}, {{7, 13}, {15, 29}, {18,  36}}, {{4, 15}, {4, 21}}, {26, 22}, {6, 6}, { 0,  0}, {18, 18}},   /* 50GE    */
    { 53000,  9, 12,  4, {{3, 12}, { 7, 16}, {3, 12}}, {{8, 40}, {12, 44}, {29, 50}}, 21, {1, 1}, {{7, 13}, {15, 29}, {18,  36}}, {{4, 15}, {4, 21}}, {26, 22}, {6, 6}, { 0,  0}, {18, 18}},   /* HG[53]  */
    {100000, 11, 12,  4, {{3, 11}, { 7, 15}, {3, 11}}, {{8, 45}, {11, 49}, {10, 49}},  3, {1, 1}, {{7, 13}, {15, 29}, {18,  36}}, {{8, 27}, {8, 29}}, {41, 38}, {5, 5}, { 0,  0}, {18, 18}},   /* 100GE   */
    {106000, 11, 12,  4, {{3, 11}, { 7, 15}, {3, 11}}, {{8, 45}, {11, 49}, {10, 49}},  3, {1, 1}, {{7, 13}, {15, 29}, {18,  36}}, {{8, 27}, {8, 29}}, {41, 38}, {5, 5}, { 0,  0}, {18, 18}}    /* HG[106] */
};

/* ASF Control */
static _soc_td3_asf_ctrl_t*
_soc_td3_asf_ctrl[SOC_MAX_NUM_DEVICES] = {NULL};

/* ASF profile billboard */
static const char *asf_profile_str[3] =
    {"removed of cut-thru capability",
     "capable of limited speed range cut-thru",
     "capable of extended speed range cut-thru"};


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
_soc_td3_speed_to_ct_class_map(int speed)
{
    int ct_class;

    if (1000 == speed) { /* 1G ports use same setting as 10G ports */
        speed = 10000;
    }

    for (ct_class = 0; ct_class < _SOC_TD3_ASF_CLASS_CNT; ct_class++) {
        if (_soc_td3_asf_cfg_tbl[ct_class].speed == speed) {
            return ct_class;
        }
    }

    return SOC_E_PARAM;
}

/******************************************************************************
 * Name: _soc_td3_port_asf_class_get                                          *
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
_soc_td3_port_asf_class_get(
    int             unit,
    soc_port_t      port,
    OUT int* const  ct_class)
{
    egr_ip_cut_thru_class_entry_t entry;

    if (!ct_class) {
        return SOC_E_PARAM;
    }

    sal_memset(&entry, 0, sizeof(egr_ip_cut_thru_class_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_EGR_IP_CUT_THRU_CLASSm(unit, MEM_BLOCK_ALL, port, &entry));
    *ct_class = soc_mem_field32_get(unit, EGR_IP_CUT_THRU_CLASSm, &entry,
                                    CUT_THRU_CLASSf);

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_td3_port_asf_class_init                                         *
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
_soc_td3_port_asf_class_init(
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
 * Name: _soc_td3_port_asf_xmit_start_count_get                               *
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
_soc_td3_port_asf_xmit_start_count_get(
    int                      unit,
    soc_port_t               port,
    int                      sc,
    int                      dc,
    soc_td3_asf_mode_e       mode,
    IN_OUT uint8* const      xmit_cnt)
{
    soc_info_t                  *si;
    egr_xmit_start_count_entry_t entry;
    int                          ovs_ratio, port_idx;
    soc_pbmp_t                   td3_ports, line_rate_ports, ovs_161_ports;
    soc_port_t                   td3_port;
    int                          speed;
    int                          ct_class;
    int                          pipe;
    soc_mem_t                    config_mem = INVALIDm;

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    pipe = si->port_pipe[port];
    config_mem = SOC_MEM_UNIQUE_ACC(unit, EGR_XMIT_START_COUNTm)[pipe];

    if (_SOC_TD3_ASF_MODE_CFG_UPDATE == mode) { /* translate to valid mode */
        /* reverse map speed class to speed, to surmount speed deficit */
        SOC_IF_ERROR_RETURN(_soc_td3_port_asf_class_get(unit, port, &ct_class));
        _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(ct_class, 0);
        speed = _SOC_TD3_CT_CLASS_TO_SPEED_MAP(ct_class);
        SOC_IF_ERROR_RETURN(soc_td3_port_asf_mode_get(unit, port, speed, &mode));
    }
    if (!xmit_cnt ||
        !((mode >= _SOC_TD3_ASF_MODE_SAF) &&
          (mode <= _SOC_TD3_ASF_MODE_FAST_TO_SLOW))) {
        return SOC_E_PARAM;
    }
    _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(sc, 0);
    _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(dc, 0);

    if (_SOC_TD3_ASF_RETRV == *xmit_cnt) {
        *xmit_cnt = 0;
        port_idx = ((port % _TD3_TDM_DEV_PORTS_PER_PIPE) * 16) + sc;

        sal_memset(&entry, 0, sizeof(egr_xmit_start_count_entry_t));
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, config_mem, MEM_BLOCK_ALL,
                                         port_idx, &entry));
        *xmit_cnt = soc_mem_field32_get(unit, config_mem,
                                        &entry, THRESHOLDf);
    } else if (_SOC_TD3_ASF_QUERY == *xmit_cnt) {
        *xmit_cnt = 0;
        SOC_PBMP_CLEAR(td3_ports);
        SOC_PBMP_ASSIGN(td3_ports, PBMP_E_ALL(unit));
        SOC_PBMP_OR(td3_ports, PBMP_HG_ALL(unit));
        SOC_PBMP_REMOVE(td3_ports, PBMP_MANAGEMENT(unit));
        SOC_PBMP_CLEAR(line_rate_ports);
        SOC_PBMP_CLEAR(ovs_161_ports);

        PBMP_ITER(td3_ports, td3_port) {
            if (SOC_PBMP_MEMBER(si->oversub_pbm, td3_port)) {
                SOC_IF_ERROR_RETURN(
                    soc_td3_port_oversub_ratio_get(unit, td3_port, &ovs_ratio));
                if (ovs_ratio < 1620) { /* oversub ratio <= 1.61 */
                    SOC_PBMP_PORT_ADD(ovs_161_ports, td3_port);
                }
            } else { /* Line-rate */
                SOC_PBMP_PORT_ADD(line_rate_ports, td3_port);
            }
        }

        /* if sc < dc, use SAF setting; else, use CT setting */
        if (SOC_PBMP_EQ(td3_ports, line_rate_ports)) {
            *xmit_cnt = (sc < dc) ?
                _soc_td3_asf_cfg_tbl[dc].edb_prebuffer.line_rate.saf :
                _soc_td3_asf_cfg_tbl[dc].edb_prebuffer.line_rate.ct;
        } else if (SOC_PBMP_EQ(td3_ports, ovs_161_ports)) {
            *xmit_cnt = (sc < dc) ?
                _soc_td3_asf_cfg_tbl[dc].edb_prebuffer.oversub_161.saf :
                _soc_td3_asf_cfg_tbl[dc].edb_prebuffer.oversub_161.ct;
        } else {
            *xmit_cnt = (sc < dc) ?
                _soc_td3_asf_cfg_tbl[dc].edb_prebuffer.oversub_2_1.saf :
                _soc_td3_asf_cfg_tbl[dc].edb_prebuffer.oversub_2_1.ct;
        }

    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_td3_port_asf_mmu_prebuf_get                                     *
 * Description:                                                               *
 *     Query or retrieve MMU Prebuffer (Min Source Port XMIT Count) for the   *
 *     specified port depending on the magic number passed in mmu_prebuf      *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
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
_soc_td3_port_asf_mmu_prebuf_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    IN_OUT uint8* const   mmu_prebuf)
{
    uint32 rval;
    int    speed_encoding;

    _SOC_TD3_ASF_PORT_VALIDATE(unit, port);
    if (!mmu_prebuf) {
        return SOC_E_PARAM;
    }

    if (_SOC_TD3_ASF_RETRV == *mmu_prebuf) {
        *mmu_prebuf = 0;

        if (!_soc_td3_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *mmu_prebuf = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                        MIN_SRC_PORT_XMIT_CNTf);
    } else if (_SOC_TD3_ASF_QUERY == *mmu_prebuf) {
        *mmu_prebuf = 0;
        speed_encoding = _soc_td3_speed_to_ct_class_map(port_speed);
        _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

        *mmu_prebuf =
            _soc_td3_asf_cfg_tbl[speed_encoding].mmu_prebuffer;
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_td3_port_asf_fifo_threshold_get                                 *
 * Description:                                                               *
 *     Query or retrieve FIFO Threshold for the specified port depending on   *
 *     the magic number passed in fifo_threshold                              *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - fifo_threshold : IN:  carries magic number for query / retrieve      *
 *     - fifo_threshold : OUT: contains a valid FIFO Threshold on success     *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_td3_port_asf_fifo_threshold_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_td3_asf_mode_e    mode,
    IN_OUT uint8* const   fifo_threshold)
{
    uint32 rval;
    int    speed_encoding;
    uint8  oversub;

    _SOC_TD3_ASF_PORT_VALIDATE(unit, port);
    if (!fifo_threshold) {
        return SOC_E_PARAM;
    }

    if (_SOC_TD3_ASF_RETRV == *fifo_threshold) {
        *fifo_threshold = 0;

        if (!_soc_td3_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *fifo_threshold = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                            FIFO_THRESHOLDf);
    } else if (_SOC_TD3_ASF_QUERY == *fifo_threshold) {
        *fifo_threshold = 0;
        speed_encoding = _soc_td3_speed_to_ct_class_map(port_speed);
        _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);
        oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);

        switch(mode) {
            case _SOC_TD3_ASF_MODE_SAME_SPEED:
                *fifo_threshold = (oversub ?
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_threshold.ss.oversub :
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_threshold.ss.line_rate);
                break;

            case _SOC_TD3_ASF_MODE_SLOW_TO_FAST:
                *fifo_threshold = (oversub ?
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_threshold.sf.oversub :
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_threshold.sf.line_rate);
                break;

            case _SOC_TD3_ASF_MODE_FAST_TO_SLOW:
                *fifo_threshold = (oversub ?
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_threshold.fs.oversub :
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_threshold.fs.line_rate);
                break;

            default:
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_td3_port_asf_fifo_depth_get                                     *
 * Description:                                                               *
 *     Query or retrieve FIFO Depth for the specified port depending on the   *
 *     magic number passed in buf_sz                                          *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - fifo_depth : IN:  carries magic number for query/retrieve            *
 *     - fifo_depth : OUT: contains a valid FIFO Depth on success             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 */
STATIC int
_soc_td3_port_asf_fifo_depth_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_td3_asf_mode_e    mode,
    IN_OUT uint8* const   fifo_depth)
{
    uint32 rval;
    int    speed_encoding;
    uint8  oversub;

    _SOC_TD3_ASF_PORT_VALIDATE(unit, port);
    if (!fifo_depth) {
        return SOC_E_PARAM;
    }

    if (_SOC_TD3_ASF_RETRV == *fifo_depth) {
        if (!_soc_td3_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        *fifo_depth = 0;
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *fifo_depth = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                        FIFO_DEPTHf);
    } else if (_SOC_TD3_ASF_QUERY == *fifo_depth) {
        *fifo_depth = 0;
        speed_encoding = _soc_td3_speed_to_ct_class_map(port_speed);
        _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);
        oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);

        switch(mode) {
            case _SOC_TD3_ASF_MODE_SAME_SPEED:
                *fifo_depth = (oversub ?
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_depth.ss.oversub :
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_depth.ss.line_rate);
                break;

            case _SOC_TD3_ASF_MODE_SLOW_TO_FAST:
                *fifo_depth = (oversub ?
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_depth.sf.oversub :
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_depth.sf.line_rate);
                break;

            case _SOC_TD3_ASF_MODE_FAST_TO_SLOW:
                *fifo_depth = (oversub ?
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_depth.fs.oversub :
                    _soc_td3_asf_cfg_tbl[speed_encoding].ct_fifo_depth.fs.line_rate);
                break;

            default:
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_td3_port_asf_speed_limits_get                                   *
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
_soc_td3_port_asf_speed_limits_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_td3_asf_mode_e    mode,
    IN_OUT uint8* const   min_sp,
    IN_OUT uint8* const   max_sp)
{
    uint32 rval;
    int    speed_encoding;

    _SOC_TD3_ASF_PORT_VALIDATE(unit, port);
    if (!min_sp || !max_sp) {
        return SOC_E_PARAM;
    }

    if ((_SOC_TD3_ASF_RETRV == *min_sp) &&
        (_SOC_TD3_ASF_RETRV == *max_sp)) {
        *min_sp = *max_sp = 0;
        if (!_soc_td3_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));

        /* MIN_SRC_PORT_SPEED */
        *min_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                    MIN_SRC_PORT_SPEEDf);
        /* MAX_SRC_PORT_SPEED */
        *max_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                    MAX_SRC_PORT_SPEEDf);
    } else if ((_SOC_TD3_ASF_QUERY == *min_sp) &&
               (_SOC_TD3_ASF_QUERY == *max_sp)) {
        *min_sp = *max_sp = 0;
        speed_encoding = _soc_td3_speed_to_ct_class_map(port_speed);
        _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 0);

        switch(mode) {
            case _SOC_TD3_ASF_MODE_SAF:
                return SOC_E_NONE;

            case _SOC_TD3_ASF_MODE_SAME_SPEED:
                *min_sp = *max_sp = speed_encoding;
                break;

            case _SOC_TD3_ASF_MODE_SLOW_TO_FAST:
                *min_sp = _soc_td3_asf_cfg_tbl[speed_encoding].min_sp;
                *max_sp = speed_encoding;
                break;

            case _SOC_TD3_ASF_MODE_FAST_TO_SLOW:
                if (_SOC_TD3_ASF_MEM_PROFILE_SIMILAR ==
                        _soc_td3_asf_ctrl[unit]->asf_mem_profile) {
                    if (speed_encoding % 2) { /* IEEE */
                        /* HG counterpart */
                        *max_sp = speed_encoding + 1;
                    } else { /* HG */
                        *max_sp = speed_encoding;
                    }
                } else if (_SOC_TD3_ASF_MEM_PROFILE_EXTREME ==
                               _soc_td3_asf_ctrl[unit]->asf_mem_profile) {
                    *max_sp = _soc_td3_asf_cfg_tbl[speed_encoding].max_sp;
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
 * Name: _soc_td3_asf_obm_ca_fifo_thresh_set                                  *
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
_soc_td3_asf_obm_ca_fifo_thresh_set(
    int     unit,
    soc_port_t    port,
    uint8   ca_thresh)
{
    int  obm, pipe;
    int  lane;
    uint32 rval;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 thresh_field[4] = { THRESHOLD0f, THRESHOLD1f,
           THRESHOLD2f, THRESHOLD3f };
    uint32 port_ct_sel_field[4] = { PORT0_CT_SELf, PORT1_CT_SELf,
           PORT2_CT_SELf, PORT3_CT_SELf };
    uint32 idb_obm_ca_ct_ctrl_reg[][2] = {
           {IDB_OBM0_CA_CT_CONTROL_PIPE0r, IDB_OBM0_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM1_CA_CT_CONTROL_PIPE0r, IDB_OBM1_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM2_CA_CT_CONTROL_PIPE0r, IDB_OBM2_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM3_CA_CT_CONTROL_PIPE0r, IDB_OBM3_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM4_CA_CT_CONTROL_PIPE0r, IDB_OBM4_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM5_CA_CT_CONTROL_PIPE0r, IDB_OBM5_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM6_CA_CT_CONTROL_PIPE0r, IDB_OBM6_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM7_CA_CT_CONTROL_PIPE0r, IDB_OBM7_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM8_CA_CT_CONTROL_PIPE0r, IDB_OBM8_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM9_CA_CT_CONTROL_PIPE0r, IDB_OBM9_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM10_CA_CT_CONTROL_PIPE0r, IDB_OBM10_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM11_CA_CT_CONTROL_PIPE0r, IDB_OBM11_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM12_CA_CT_CONTROL_PIPE0r, IDB_OBM12_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM13_CA_CT_CONTROL_PIPE0r, IDB_OBM13_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM14_CA_CT_CONTROL_PIPE0r, IDB_OBM14_CA_CT_CONTROL_PIPE1r},
           {IDB_OBM15_CA_CT_CONTROL_PIPE0r, IDB_OBM15_CA_CT_CONTROL_PIPE1r},
        };

    /* Get lane, pipe & obm */
    SOC_IF_ERROR_RETURN
        (soc_trident3_port_obm_info_get(unit, port, &obm, &lane));
    pipe = si->port_pipe[port];

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
 * Name: _soc_td3_asf_obm_bubble_mop_set                                      *
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
_soc_td3_asf_obm_bubble_mop_set(
    int     unit,
    soc_port_t    port,
    uint8   bubble_mop_disable)
{
    int  obm, pipe;
    int  lane;
    uint32 rval;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 port_field[4] = {
            PORT0_BUBBLE_MOP_DISABLEf, PORT1_BUBBLE_MOP_DISABLEf,
            PORT2_BUBBLE_MOP_DISABLEf, PORT3_BUBBLE_MOP_DISABLEf };
    uint32 idb_obm_ctrl_reg[][2] = {
           {IDB_OBM0_CONTROL_PIPE0r, IDB_OBM0_CONTROL_PIPE1r},
           {IDB_OBM1_CONTROL_PIPE0r, IDB_OBM1_CONTROL_PIPE1r},
           {IDB_OBM2_CONTROL_PIPE0r, IDB_OBM2_CONTROL_PIPE1r},
           {IDB_OBM3_CONTROL_PIPE0r, IDB_OBM3_CONTROL_PIPE1r},
           {IDB_OBM4_CONTROL_PIPE0r, IDB_OBM4_CONTROL_PIPE1r},
           {IDB_OBM5_CONTROL_PIPE0r, IDB_OBM5_CONTROL_PIPE1r},
           {IDB_OBM6_CONTROL_PIPE0r, IDB_OBM6_CONTROL_PIPE1r},
           {IDB_OBM7_CONTROL_PIPE0r, IDB_OBM7_CONTROL_PIPE1r},
           {IDB_OBM8_CONTROL_PIPE0r, IDB_OBM8_CONTROL_PIPE1r},
           {IDB_OBM9_CONTROL_PIPE0r, IDB_OBM9_CONTROL_PIPE1r},
           {IDB_OBM10_CONTROL_PIPE0r, IDB_OBM10_CONTROL_PIPE1r},
           {IDB_OBM11_CONTROL_PIPE0r, IDB_OBM11_CONTROL_PIPE1r},
           {IDB_OBM12_CONTROL_PIPE0r, IDB_OBM12_CONTROL_PIPE1r},
           {IDB_OBM13_CONTROL_PIPE0r, IDB_OBM13_CONTROL_PIPE1r},
           {IDB_OBM14_CONTROL_PIPE0r, IDB_OBM14_CONTROL_PIPE1r},
           {IDB_OBM15_CONTROL_PIPE0r, IDB_OBM15_CONTROL_PIPE1r},
        };

    /* Get lane, pipe & obm */
    SOC_IF_ERROR_RETURN
        (soc_trident3_port_obm_info_get(unit, port, &obm, &lane));
    pipe = si->port_pipe[port];

    /* BubbleMOP */
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
 * Name: _soc_td3_asf_obm_ct_thresh_ok_set                                    *
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
_soc_td3_asf_obm_ct_thresh_ok_set(
    int     unit,
    soc_port_t    port,
    uint32   ct_ok_thresh)
{
    int  obm, pipe;
    int  lane;
    uint64 rval64;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 idb_obm_ct_thresh_reg[][2] = {
           {IDB_OBM0_CT_THRESHOLD_PIPE0r, IDB_OBM0_CT_THRESHOLD_PIPE1r},
           {IDB_OBM1_CT_THRESHOLD_PIPE0r, IDB_OBM1_CT_THRESHOLD_PIPE1r},
           {IDB_OBM2_CT_THRESHOLD_PIPE0r, IDB_OBM2_CT_THRESHOLD_PIPE1r},
           {IDB_OBM3_CT_THRESHOLD_PIPE0r, IDB_OBM3_CT_THRESHOLD_PIPE1r},
           {IDB_OBM4_CT_THRESHOLD_PIPE0r, IDB_OBM4_CT_THRESHOLD_PIPE1r},
           {IDB_OBM5_CT_THRESHOLD_PIPE0r, IDB_OBM5_CT_THRESHOLD_PIPE1r},
           {IDB_OBM6_CT_THRESHOLD_PIPE0r, IDB_OBM6_CT_THRESHOLD_PIPE1r},
           {IDB_OBM7_CT_THRESHOLD_PIPE0r, IDB_OBM7_CT_THRESHOLD_PIPE1r},
           {IDB_OBM8_CT_THRESHOLD_PIPE0r, IDB_OBM8_CT_THRESHOLD_PIPE1r},
           {IDB_OBM9_CT_THRESHOLD_PIPE0r, IDB_OBM9_CT_THRESHOLD_PIPE1r},
           {IDB_OBM10_CT_THRESHOLD_PIPE0r, IDB_OBM10_CT_THRESHOLD_PIPE1r},
           {IDB_OBM11_CT_THRESHOLD_PIPE0r, IDB_OBM11_CT_THRESHOLD_PIPE1r},
           {IDB_OBM12_CT_THRESHOLD_PIPE0r, IDB_OBM12_CT_THRESHOLD_PIPE1r},
           {IDB_OBM13_CT_THRESHOLD_PIPE0r, IDB_OBM13_CT_THRESHOLD_PIPE1r},
           {IDB_OBM14_CT_THRESHOLD_PIPE0r, IDB_OBM14_CT_THRESHOLD_PIPE1r},
           {IDB_OBM15_CT_THRESHOLD_PIPE0r, IDB_OBM15_CT_THRESHOLD_PIPE1r},
        };

    /* Get lane, pipe & obm */
    SOC_IF_ERROR_RETURN
        (soc_trident3_port_obm_info_get(unit, port, &obm, &lane));
    pipe = si->port_pipe[port];

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
 * Name: _soc_td3_asf_force_saf_config_set                                    *
 * Description:                                                               *
 *     Configure IDB_OBMx_FORCE_SAF_CONFIG register for all OBM and Pipe      *
 *     instances.                                                             *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port speed                                                           *
 *     - ASF mode                                                             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 */
STATIC int
_soc_td3_asf_force_saf_config_set(
    int     unit,
    soc_port_t    port,
    int           port_speed,
    soc_td3_asf_mode_e   mode)
{
    int obm, pipe;
    int lane;
    int ct_class;
    uint32 rval;
    uint32 field_a;
    uint32 field_b;
    uint32 field_c;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 idb_obm_force_saf_config_reg[][2] = {
           {IDB_OBM0_DBG_A_PIPE0r, IDB_OBM0_DBG_A_PIPE1r},
           {IDB_OBM1_DBG_A_PIPE0r, IDB_OBM1_DBG_A_PIPE1r},
           {IDB_OBM2_DBG_A_PIPE0r, IDB_OBM2_DBG_A_PIPE1r},
           {IDB_OBM3_DBG_A_PIPE0r, IDB_OBM3_DBG_A_PIPE1r},
           {IDB_OBM4_DBG_A_PIPE0r, IDB_OBM4_DBG_A_PIPE1r},
           {IDB_OBM5_DBG_A_PIPE0r, IDB_OBM5_DBG_A_PIPE1r},
           {IDB_OBM6_DBG_A_PIPE0r, IDB_OBM6_DBG_A_PIPE1r},
           {IDB_OBM7_DBG_A_PIPE0r, IDB_OBM7_DBG_A_PIPE1r},
           {IDB_OBM8_DBG_A_PIPE0r, IDB_OBM8_DBG_A_PIPE1r},
           {IDB_OBM9_DBG_A_PIPE0r, IDB_OBM9_DBG_A_PIPE1r},
           {IDB_OBM10_DBG_A_PIPE0r, IDB_OBM10_DBG_A_PIPE1r},
           {IDB_OBM11_DBG_A_PIPE0r, IDB_OBM11_DBG_A_PIPE1r},
           {IDB_OBM12_DBG_A_PIPE0r, IDB_OBM12_DBG_A_PIPE1r},
           {IDB_OBM13_DBG_A_PIPE0r, IDB_OBM13_DBG_A_PIPE1r},
           {IDB_OBM14_DBG_A_PIPE0r, IDB_OBM14_DBG_A_PIPE1r},
           {IDB_OBM15_DBG_A_PIPE0r, IDB_OBM15_DBG_A_PIPE1r},
        };

    field_a = 1;
    field_b = 1;

    ct_class = _soc_td3_speed_to_ct_class_map(port_speed);
    _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(ct_class, 0);
    field_c = (mode == _SOC_TD3_ASF_MODE_SAF) ?
              _soc_td3_asf_cfg_tbl[ct_class].unsat_thresh.saf :
              _soc_td3_asf_cfg_tbl[ct_class].unsat_thresh.ct;

    /* Get lane, pipe & obm */
    SOC_IF_ERROR_RETURN
        (soc_trident3_port_obm_info_get(unit, port, &obm, &lane));
    pipe = si->port_pipe[port];

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

    return SOC_E_NONE;
}

int
soc_td3_port_asf_valid(
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
soc_td3_asf_init_start(int unit)
{
    if (!(_soc_td3_asf_ctrl[unit] =
              sal_alloc(sizeof(_soc_td3_asf_ctrl_t), "TD3 ASF Ctrl Area"))) {
        return SOC_E_MEMORY;
    }
    sal_memset(_soc_td3_asf_ctrl[unit], 0, sizeof(_soc_td3_asf_ctrl_t));

    /* ASF_MEM_PROFILE */
    _soc_td3_asf_ctrl[unit]->asf_mem_profile =
        soc_property_get(unit, spn_ASF_MEM_PROFILE,
                         _SOC_TD3_ASF_MEM_PROFILE_SIMILAR);
    if (!((_soc_td3_asf_ctrl[unit]->asf_mem_profile >= 0) &&
          (_soc_td3_asf_ctrl[unit]->asf_mem_profile < 3))) {
        _soc_td3_asf_ctrl[unit]->asf_mem_profile = 2;
    }

    return SOC_E_NONE;
}

/************************
 * Init Complete Marker *
 */
int
soc_td3_asf_init_done(int unit)
{
    if (_soc_td3_asf_ctrl[unit]) {
        _soc_td3_asf_ctrl[unit]->init = 1;
        LOG_CLI((BSL_META_U(unit, "*** unit %d: ports %s\n"), unit,
                 asf_profile_str[_soc_td3_asf_ctrl[unit]->asf_mem_profile]));
        return SOC_E_NONE;
    } else {
        return SOC_E_INTERNAL;
    }
}

/******************************************************************************
 * Name: soc_td3_port_asf_init                                                *
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
soc_td3_port_asf_init(
    int                 unit,
    soc_port_t          port,
    int                 speed,
    soc_td3_asf_mode_e  mode)
{
    SOC_IF_ERROR_RETURN(soc_td3_port_asf_mode_set(unit, port, speed, mode));

    return SOC_E_NONE;
}

/************************
 * Init Start Routine   *
 */
int
soc_td3_asf_deinit(int unit)
{
    if (_soc_td3_asf_ctrl[unit]) {
        sal_free(_soc_td3_asf_ctrl[unit]);
        _soc_td3_asf_ctrl[unit] = NULL;
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_td3_port_asf_xmit_start_count_set                               *
 * Description:                                                               *
 *     Initialize XMIT START COUNT memory for all the source class for the    *
 *     specified port                                                         *
 *     Support CPU and LB ports for init setting.                             *
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
soc_td3_port_asf_xmit_start_count_set(
    int                  unit,
    soc_port_t           port,
    int                  port_speed,
    soc_td3_asf_mode_e   mode,
    uint8                extra_cells)
{
    egr_xmit_start_count_entry_t entry;
    soc_info_t *si = &SOC_INFO(unit);
    uint8      xmit_cnt = 0;
    int        rv, src_class, dst_class = 0, port_idx;
    int        pipe;
    soc_mem_t  config_mem = INVALIDm;

    if (!((mode >= _SOC_TD3_ASF_MODE_SAF) &&
          (mode <= _SOC_TD3_ASF_MODE_CFG_UPDATE))) {
        return SOC_E_PARAM;
    }

    if (IS_CPU_PORT(unit, port)) {
        port_speed = 20000;
    }
    if (IS_LB_PORT(unit, port)) {
        port_speed = 100000;
    }

    if (_SOC_TD3_ASF_MODE_CFG_UPDATE != mode) {
        if (SOC_E_PARAM == (dst_class = _soc_td3_speed_to_ct_class_map(port_speed))) {
            /* port on a speed unsupported by CT - will resort to SAF */
            dst_class = 0;
        }
    }

    sal_memset(&entry, 0, sizeof(egr_xmit_start_count_entry_t));

    pipe = si->port_pipe[port];
    config_mem = SOC_MEM_UNIQUE_ACC(unit, EGR_XMIT_START_COUNTm)[pipe];

    for (src_class = 0; src_class < _SOC_TD3_ASF_CLASS_CNT; src_class++) {
        if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
            xmit_cnt = _soc_td3_asf_cfg_tbl[dst_class].edb_prebuffer.line_rate.saf;
        } else {
            if (_SOC_TD3_ASF_MODE_CFG_UPDATE == mode) {
                xmit_cnt = _SOC_TD3_ASF_RETRV;
            } else {
                xmit_cnt = _SOC_TD3_ASF_QUERY;
            }

            rv = _soc_td3_port_asf_xmit_start_count_get(unit, port, src_class,
                                                        dst_class, mode, &xmit_cnt);

            if (SOC_FAILURE(rv)) {
                if (SOC_E_UNAVAIL == rv) {
                    xmit_cnt = _soc_td3_asf_cfg_tbl[dst_class].edb_prebuffer.oversub_2_1.saf;
                } else {
                    return rv;
                }
            }

            xmit_cnt += extra_cells;
        }

        port_idx = ((port % _TD3_TDM_DEV_PORTS_PER_PIPE) * 16) + src_class;

        soc_mem_field32_set(unit, config_mem, &entry,
                            THRESHOLDf, xmit_cnt);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, config_mem, MEM_BLOCK_ALL,
                                          port_idx, &entry));
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_td3_port_asf_mode_get                                            *
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
soc_td3_port_asf_mode_get(
    int                            unit,
    soc_port_t                     port,
    int                            port_speed,
    OUT soc_td3_asf_mode_e* const  mode)
{
    uint32 rval;
    int    speed_encoding;
    uint8  max_sp, min_sp, enable;

    if (!mode) {
        return SOC_E_INTERNAL;
    }

    if (soc_td3_port_asf_valid(unit, port)) {
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        enable = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, ENABLEf);
        if (!enable) {
            *mode = _SOC_TD3_ASF_MODE_SAF;
            return SOC_E_NONE;
        }
    } else {
        *mode = _SOC_TD3_ASF_MODE_SAF;
        return SOC_E_NONE;
    }

    if (!_soc_td3_asf_ctrl[unit] || !_soc_td3_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    max_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, MAX_SRC_PORT_SPEEDf);
    min_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, MIN_SRC_PORT_SPEEDf);

    speed_encoding = _soc_td3_speed_to_ct_class_map(port_speed);
    _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

    if (max_sp && min_sp) {
        if (max_sp == min_sp) {
            if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_sf, port)) {
                *mode = _SOC_TD3_ASF_MODE_SLOW_TO_FAST;
            } else if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_fs, port)) {
                *mode = _SOC_TD3_ASF_MODE_FAST_TO_SLOW;
            } else {
                *mode = _SOC_TD3_ASF_MODE_SAME_SPEED;
            }
        } else if (max_sp == speed_encoding) {
            *mode = _SOC_TD3_ASF_MODE_SLOW_TO_FAST;
        } else if (min_sp == speed_encoding) {
            *mode = _SOC_TD3_ASF_MODE_FAST_TO_SLOW;
        } else {
            *mode = _SOC_TD3_ASF_MODE_UNSUPPORTED;
            return SOC_E_UNAVAIL;
        }
    } else {
        *mode = _SOC_TD3_ASF_MODE_UNSUPPORTED;
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_td3_port_asf_mode_set                                            *
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
soc_td3_port_asf_mode_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_td3_asf_mode_e  mode)
{
    int           ret, speed_encoding = 0;
    uint8         min_sp = 0, max_sp = 0;
    uint8         fifo_threshold = 0, fifo_depth = 0, mmu_prebuf = 0;
    uint8         oversub = 0;
    uint32        rval;
    soc_info_t   *si = NULL;
    int           init = 0;

    if (_SOC_TD3_ASF_MODE_INIT == mode) {
        init = 1;
        mode = _SOC_TD3_ASF_MODE_SAF;
    }
    if ((!init) && (!_soc_td3_asf_ctrl[unit] ||
        !_soc_td3_asf_ctrl[unit]->init || !(si = &SOC_INFO(unit)))) {
        return SOC_E_INTERNAL;
    }
    if (!_soc_td3_asf_ctrl[unit]->asf_mem_profile) { /* no cut-thru profile */
        if ((_SOC_TD3_ASF_MODE_SAF != mode) &&
            (_SOC_TD3_ASF_MODE_CFG_UPDATE != mode)) {
            return SOC_E_UNAVAIL;
        }
    }
    if (!soc_td3_port_asf_valid(unit, port)) {
        if ((_SOC_TD3_ASF_MODE_SAF != mode) &&
            (_SOC_TD3_ASF_MODE_CFG_UPDATE != mode)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(
            soc_td3_port_asf_xmit_start_count_set(unit, port, port_speed,
                                                  mode, 0));
        return SOC_E_NONE;
    }

    speed_encoding = _soc_td3_speed_to_ct_class_map(port_speed);
    _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 0);

    /* min. and max. src port speeds */
    if (_SOC_TD3_ASF_MODE_CFG_UPDATE != mode) {
        min_sp = max_sp = _SOC_TD3_ASF_QUERY;
        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                              &min_sp, &max_sp));
    }

    /* mode specific configurations */
    switch(mode) {
        case _SOC_TD3_ASF_MODE_SAF:
            break;

        case _SOC_TD3_ASF_MODE_SAME_SPEED:
            /* Cannot set cut-through mode for 1G ports */
            if (1000 == port_speed) {
                return SOC_E_UNAVAIL;
            }

            /* verify mode specific conformance */
            if (min_sp != max_sp) {
                return SOC_E_PARAM;
            }
            break;

        case _SOC_TD3_ASF_MODE_SLOW_TO_FAST:
            /* Cannot set cut-through mode for 1G ports */
            if (1000 == port_speed) {
                return SOC_E_UNAVAIL;
            }

            /* verify mode specific conformance */
            if ( !((min_sp >=
                    _soc_td3_asf_cfg_tbl[speed_encoding].min_sp) &&
                   (max_sp == speed_encoding)) ) {
                return SOC_E_PARAM;
            }
            break;

        case _SOC_TD3_ASF_MODE_FAST_TO_SLOW:
            /* Cannot set cut-through mode for 1G ports */
            if (1000 == port_speed) {
                return SOC_E_UNAVAIL;
            }

            /* verify mode specific conformance */
            if ( !((max_sp <=
                    _soc_td3_asf_cfg_tbl[speed_encoding].max_sp) &&
                   (min_sp == speed_encoding)) ) {
                return SOC_E_PARAM;
            }
            break;

        case _SOC_TD3_ASF_MODE_CFG_UPDATE:
            mode = _SOC_TD3_ASF_MODE_SAF;

            /* disable CT first */
            SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
            soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                              FIFO_THRESHOLDf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));
            sal_usleep(1);
            soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, ENABLEf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

            /* update new settings & re-enable */
            ret = soc_td3_port_asf_mode_set(unit, port, port_speed, mode);
            return ret;

        default:
            return SOC_E_PARAM;
    }

    /* ASF_IPORT_CFG */
    SOC_IF_ERROR_RETURN(READ_ASF_IPORT_CFGr(unit, port, &rval));
    soc_reg_field_set(unit, ASF_IPORT_CFGr, &rval, ASF_PORT_SPEEDf,
                      port_speed == 1000 ? 0: speed_encoding);
    SOC_IF_ERROR_RETURN(WRITE_ASF_IPORT_CFGr(unit, port, rval));

    /* init CT class */
    SOC_IF_ERROR_RETURN(_soc_td3_port_asf_class_init(unit, port, speed_encoding));

    if (_SOC_TD3_ASF_MODE_SAF != mode) {
        /* Oversub */
        oversub =  (SOC_PBMP_MEMBER(si->oversub_pbm, port) ? 1 : 0);

        /* FIFO threshold */
        fifo_threshold = _SOC_TD3_ASF_QUERY;
        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_fifo_threshold_get(unit, port, port_speed, mode,
                                                 &fifo_threshold));

        /* FIFO depth */
        fifo_depth = _SOC_TD3_ASF_QUERY;
        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_fifo_depth_get(unit, port, port_speed, mode,
                                             &fifo_depth));

        /* MMU Prebuffer */
        mmu_prebuf = _SOC_TD3_ASF_QUERY;
        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                             &mmu_prebuf));

        SOC_IF_ERROR_RETURN(
            soc_td3_port_asf_params_set(unit, port, port_speed, mode));

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
        if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_ss, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_ss, port);
        } else if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_sf, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_sf, port);
        } else if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_fs, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_fs, port);
        }
        SOC_PBMP_PORT_ADD(_soc_td3_asf_ctrl[unit]->asf_ports, port);
        if (_SOC_TD3_ASF_MODE_SAME_SPEED == mode) {
            SOC_PBMP_PORT_ADD(_soc_td3_asf_ctrl[unit]->asf_ss, port);
        } else if (_SOC_TD3_ASF_MODE_SLOW_TO_FAST== mode) {
            SOC_PBMP_PORT_ADD(_soc_td3_asf_ctrl[unit]->asf_sf, port);
        } else if (_SOC_TD3_ASF_MODE_FAST_TO_SLOW== mode) {
            SOC_PBMP_PORT_ADD(_soc_td3_asf_ctrl[unit]->asf_fs, port);
        }
    } else {   /* Store and Forward */
        fifo_depth = 0x40;
        fifo_threshold = 0x20;
        mmu_prebuf = 0;
        min_sp = 0;
        max_sp = 0;
        oversub = 0;

        SOC_IF_ERROR_RETURN(
            soc_td3_port_asf_params_set(unit, port, port_speed, mode));

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

        /* bookkeeping */
        SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_ports, port);
        if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_ss, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_ss, port);
        } else if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_sf, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_sf, port);
        } else if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_fs, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_fs, port);
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_td3_port_asf_params_set                                          *
 * Description:                                                               *
 *     Configure ASF/SAF mode params on the specified port                    *
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
soc_td3_port_asf_params_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_td3_asf_mode_e  mode)
{
    uint32        mmu_ep_credits_hi = 0, mmu_ep_credits_lo = 0;
    uint32        egr_mmu_cell_credits = 0;
    uint32        rval;
    uint32        ct_ok_thresh = 0;
    uint8         ca_thresh = 0;
    uint8         bubble_mop_disable = 0, oversub = 0;
    int           ovs_ratio = 0;
    int           speed_encoding = 0;
    soc_info_t   *si = NULL;
    egr_mmu_cell_credit_entry_t credit_entry;

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    speed_encoding = _soc_td3_speed_to_ct_class_map(port_speed);
    _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 0);
    /* EDB Prebuffer */
    SOC_IF_ERROR_RETURN(
        soc_td3_port_asf_xmit_start_count_set(unit, port, port_speed,
                                              mode, 0));
    /* OBM Cell Assembly FIFO threshold */
    ca_thresh = (mode == _SOC_TD3_ASF_MODE_SAF) ?
        _soc_td3_asf_cfg_tbl[speed_encoding].ca_thresh.saf :
        _soc_td3_asf_cfg_tbl[speed_encoding].ca_thresh.ct;
    SOC_IF_ERROR_RETURN(
        _soc_td3_asf_obm_ca_fifo_thresh_set(unit, port, ca_thresh));
    /* CT threshold */
    ct_ok_thresh = (mode == _SOC_TD3_ASF_MODE_SAF) ?
        _soc_td3_asf_cfg_tbl[speed_encoding].ct_ok_thresh.saf :
        _soc_td3_asf_cfg_tbl[speed_encoding].ct_ok_thresh.ct;
    SOC_IF_ERROR_RETURN(
        _soc_td3_asf_obm_ct_thresh_ok_set(unit, port, ct_ok_thresh));
    /* OBM ForceSAF config */
    SOC_IF_ERROR_RETURN(
        _soc_td3_asf_force_saf_config_set(unit, port, port_speed, mode));

    /* BubbleMOP */
    if (_SOC_TD3_ASF_MODE_SAF != mode) {
        oversub =  (SOC_PBMP_MEMBER(si->oversub_pbm, port) ? 1 : 0);
        bubble_mop_disable = oversub ?
            !(_soc_td3_asf_cfg_tbl[speed_encoding].mop_enable.oversub) :
            !(_soc_td3_asf_cfg_tbl[speed_encoding].mop_enable.line_rate);
    }
    SOC_IF_ERROR_RETURN(
        _soc_td3_asf_obm_bubble_mop_set(unit, port, bubble_mop_disable));

    /* MMU EP Credit Threshold */
    SOC_IF_ERROR_RETURN(
        soc_td3_port_oversub_ratio_get(unit, port, &ovs_ratio));
    if ((CCLK_FREQ_1525MHZ == si->frequency) && (ovs_ratio < 1620)) {
        mmu_ep_credits_lo =
            _soc_td3_asf_cfg_tbl[speed_encoding].mmu_credit_threshold.lr_os_161.lo;
        mmu_ep_credits_hi =
            _soc_td3_asf_cfg_tbl[speed_encoding].mmu_credit_threshold.lr_os_161.hi;
    } else {
        mmu_ep_credits_lo =
            _soc_td3_asf_cfg_tbl[speed_encoding].mmu_credit_threshold.os_2_1.lo;
        mmu_ep_credits_hi =
            _soc_td3_asf_cfg_tbl[speed_encoding].mmu_credit_threshold.os_2_1.hi;
    }

    /* ASF_CREDIT_THRESH_LO */
    SOC_IF_ERROR_RETURN(READ_ASF_CREDIT_THRESH_LOr(unit, port, &rval));
    soc_reg_field_set(unit, ASF_CREDIT_THRESH_LOr, &rval, THRESHf,
                      mmu_ep_credits_lo);
    SOC_IF_ERROR_RETURN(WRITE_ASF_CREDIT_THRESH_LOr(unit, port, rval));

    /* ASF_CREDIT_THRESH_HI */
    SOC_IF_ERROR_RETURN(READ_ASF_CREDIT_THRESH_HIr(unit, port, &rval));
    soc_reg_field_set(unit, ASF_CREDIT_THRESH_HIr, &rval, THRESHf,
                      mmu_ep_credits_hi);
    SOC_IF_ERROR_RETURN(WRITE_ASF_CREDIT_THRESH_HIr(unit, port, rval));

    /* EP credits outstanding */
    SOC_IF_ERROR_RETURN(
        soc_td3_port_asf_speed_to_mmu_cell_credit(unit, port, port_speed,
            &egr_mmu_cell_credits));

    /* EGR_MMU_CELL_CREDIT */
    SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                        si->port_l2p_mapping[port], &credit_entry));
    soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &credit_entry, CREDITf,
                                         egr_mmu_cell_credits);
    SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ALL,
                        si->port_l2p_mapping[port], &credit_entry));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_td3_port_asf_speed_to_mmu_cell_credit                            *
 * Description:                                                               *
 *     Retrieve mmu_cell_credit based on port speed configured.               *
 *     Helper function for misc init.                                         *
 *     Support CPU and LB ports.                                              *
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
soc_td3_port_asf_speed_to_mmu_cell_credit(
    int        unit,
    soc_port_t port,
    int        port_speed,
    OUT uint32 *mmu_cell_credit)
{
    int          speed_encoding = 0, ovs_ratio = 0;
    soc_info_t   *si = NULL;

    if (NULL == mmu_cell_credit) {
        return SOC_E_PARAM;
    }

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    if (IS_CPU_PORT(unit, port)) {
        port_speed = 20000;
    }
    if (IS_LB_PORT(unit, port)) {
        port_speed = 100000;
    }

    speed_encoding = _soc_td3_speed_to_ct_class_map(port_speed);
    _SOC_TD3_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 0);

    SOC_IF_ERROR_RETURN(
        soc_td3_port_oversub_ratio_get(unit, port, &ovs_ratio));

    if ((CCLK_FREQ_1525MHZ == si->frequency) && (ovs_ratio < 1620)) {
        *mmu_cell_credit =
            _soc_td3_asf_cfg_tbl[speed_encoding].ep_credit.pfc_op_cells;
    } else {
        *mmu_cell_credit =
            _soc_td3_asf_cfg_tbl[speed_encoding].ep_credit.base_cells;
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_td3_port_asf_mmu_cell_credit_to_port_speed                       *
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
soc_td3_port_asf_mmu_cell_credit_to_speed(
    int        unit,
    soc_port_t port,
    uint8      mmu_cell_credit,
    OUT int    *port_speed)
{
    int i, freq;
    uint8 cell_credit;
    int speed;
    soc_info_t   *si = NULL;

    if (NULL == port_speed) {
        return SOC_E_PARAM;
    }

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    /* Special case for 1G ports since 1G ports use same setting as 10G port */
    if (1000 == (speed = si->port_init_speed[port])) {  /* Use cached speed instead */
        *port_speed = speed;
        return SOC_E_NONE;
    }

    freq = si->frequency;
    for (i = 1; i < (COUNTOF(_soc_td3_asf_cfg_tbl) - 1); i++) {
        int ovs_ratio = 0;
        SOC_IF_ERROR_RETURN(
            soc_td3_port_oversub_ratio_get(unit, port, &ovs_ratio));

        cell_credit = ((CCLK_FREQ_1525MHZ == freq) && (ovs_ratio < 1620))?
            _soc_td3_asf_cfg_tbl[i].ep_credit.pfc_op_cells :
            _soc_td3_asf_cfg_tbl[i].ep_credit.base_cells;

        if (mmu_cell_credit == cell_credit) {
            if (IS_HG_PORT(unit, port)) {
                *port_speed = _soc_td3_asf_cfg_tbl[i + 1].speed;
            } else {
                *port_speed = _soc_td3_asf_cfg_tbl[i].speed;
            }
/* This func is not available right now. But we can skip the check since
   credit data for each speed are all different. They can be distinguished. */
#if 0
            if (SOC_E_NONE !=
                soc_td3_port_speed_supported(unit, port, *port_speed)) {
                continue;
            }
#endif
            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

/******************************************************************************
 * Name: soc_td3_port_asf_pause_bpmp_get                                      *
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
soc_td3_port_asf_pause_bpmp_get(int unit)
{
    if (_soc_td3_asf_ctrl[unit] == NULL) {
        return NULL;
    }
    return &_soc_td3_asf_ctrl[unit]->pause_restore;
}

/************************
 * ASF Warmboot Support *
 */
#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * ASF Warmboot is a part of Port module Warmboot.
 * Check Port module for scache_ver evolution.
 */
/*
#define BCM_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_8
*/
int
soc_td3_asf_wb_memsz_get(
    int               unit,
    OUT uint32* const mem_sz,
    uint16            scache_ver)
{
    if (!mem_sz) {
        return SOC_E_PARAM;
    }
    *mem_sz = 0;

    if (!SOC_WARM_BOOT(unit)) {
        if (!_soc_td3_asf_ctrl[unit] || !_soc_td3_asf_ctrl[unit]->asf_mem_profile) {
            return SOC_E_UNAVAIL;
        }
        if (!_soc_td3_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }
    }

    *mem_sz = sizeof(_soc_td3_asf_wb_t);

    return SOC_E_NONE;
}

int
soc_td3_asf_wb_sync(
    int    unit,
    IN_OUT uint8* const wb_data)
{
    _soc_td3_asf_wb_t *wbd;

    if (!_soc_td3_asf_ctrl[unit] || !_soc_td3_asf_ctrl[unit]->asf_mem_profile) {
        return SOC_E_UNAVAIL;
    }
    if (!_soc_td3_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_td3_asf_wb_t *) wb_data;
    wbd->unit = unit;
    sal_memcpy(&wbd->asf_ctrl, _soc_td3_asf_ctrl[unit],
               sizeof(_soc_td3_asf_ctrl_t));

    return SOC_E_NONE;
}

int
soc_td3_asf_wb_recover(
    int          unit,
    uint8* const wb_data,
    uint16       scache_ver)
{
    _soc_td3_asf_wb_t *wbd;

    if (!SOC_WARM_BOOT(unit)) {
        return SOC_E_INTERNAL;
    }
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_td3_asf_wb_t *) wb_data;
    if (wbd->unit == unit) {
        if (!(_soc_td3_asf_ctrl[unit] =
                   sal_alloc(sizeof(_soc_td3_asf_ctrl_t), "TD3 ASF Ctrl Area"))) {
            return SOC_E_MEMORY;
        }

        sal_memcpy(_soc_td3_asf_ctrl[unit], &wbd->asf_ctrl,
                   sizeof(_soc_td3_asf_ctrl_t));
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
soc_td3_asf_pbmp_get(int unit)
{
    char pfmt[SOC_PBMP_FMT_LEN];

    if (!_soc_td3_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    LOG_CLI(("ASF PBMP: %s\n",
             SOC_PBMP_FMT(_soc_td3_asf_ctrl[unit]->asf_ports, pfmt)));

    return SOC_E_NONE;
}

int
soc_td3_port_asf_show(
    int        unit,
    soc_port_t port,
    int        port_speed)
{
    int ret;
    char mode_str[4][15] = {"SAF", "Same speed  ", "Slow to Fast",
                             "Fast to Slow"};
    char speed_str[_SOC_TD3_ASF_CLASS_CNT][10] = {"SAF", "10G", "HG[11]", "20G",
                                "HG[21]", "25G", "HG[27]", "40G", "HG[42]",
                                "50G", "HG[53]", "100G", "HG[106]"};
    soc_td3_asf_mode_e mode = _SOC_TD3_ASF_RETRV;
    uint8 min_speed, max_speed;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return BCM_E_NONE;
    }

    min_speed = max_speed = _SOC_TD3_ASF_RETRV;
    ret = soc_td3_port_asf_mode_get(unit, port, port_speed, &mode);
    if (SOC_E_UNAVAIL == ret) {
        mode = _SOC_TD3_ASF_MODE_SAF;
    } else if (SOC_E_NONE != ret) {
        return ret;
    }

    LOG_CLI(("%-5s     %-11s      ", SOC_PORT_NAME(unit, port), mode_str[mode]));

    if (_SOC_TD3_ASF_MODE_SAF == mode) {
        LOG_CLI(("    .. NA ..\n"));
    } else if (_SOC_TD3_ASF_MODE_FAST_TO_SLOW == mode) {
        ret = _soc_td3_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                                 &min_speed, &max_speed);
        LOG_CLI(("%s / %s\n", speed_str[max_speed], speed_str[min_speed]));
    } else {
        ret = _soc_td3_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                                 &min_speed, &max_speed);
        LOG_CLI(("%s / %s\n", speed_str[min_speed], speed_str[max_speed]));
    }

    return SOC_E_NONE;
}

int soc_td3_asf_config_dump(int unit)
{
    char asf_profile_str[3][25] = {"No cut-thru support",
                                   "Similar speed cut-thru",
                                   "Extreme speed cut-thru"};

    if (!_soc_td3_asf_ctrl[unit] || !_soc_td3_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    LOG_CLI(("ASF Profile: %s\n\n",
             asf_profile_str[_soc_td3_asf_ctrl[unit]->asf_mem_profile]));

    return SOC_E_NONE;
}

int
soc_td3_port_asf_config_dump(
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
    soc_td3_asf_mode_e mode = _SOC_TD3_ASF_RETRV;
    egr_mmu_cell_credit_entry_t credit_entry;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return BCM_E_NONE;
    }

    if (!_soc_td3_asf_ctrl[unit] || !_soc_td3_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(
        soc_td3_port_asf_mode_get(unit, port, port_speed, &mode));

    if (soc_td3_port_asf_valid(unit, port)) {
        min_sp = max_sp = fifo_threshold = _SOC_TD3_ASF_RETRV;
        fifo_depth = mmu_prebuf = _SOC_TD3_ASF_RETRV;

        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_class_get(unit, port, &ct_class));
        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                               &min_sp, &max_sp));
        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_fifo_threshold_get(unit, port, port_speed, mode,
                                                 &fifo_threshold));

        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_fifo_depth_get(unit, port, port_speed, mode,
                                             &fifo_depth));

        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                             &mmu_prebuf));

        SOC_IF_ERROR_RETURN(READ_ASF_CREDIT_THRESH_HIr(unit, port, &rval));
        mmu_ep_credits = soc_reg_field_get(unit, ASF_CREDIT_THRESH_HIr,
                                           rval, THRESHf);
    }

    dst_class = _soc_td3_speed_to_ct_class_map(port_speed);
    for (src_class = 0; src_class < _SOC_TD3_ASF_CLASS_CNT; src_class++) {
        xmit_start_cnt[src_class] = _SOC_TD3_ASF_RETRV;
        SOC_IF_ERROR_RETURN(
            _soc_td3_port_asf_xmit_start_count_get(unit, port, src_class,
                                                   dst_class, mode,
                                                   &xmit_start_cnt[src_class]));
    }

    /*
     * COVERITY
     *
     * min_sp and max_sp will be updated in _soc_td3_port_asf_speed_limits_get.
     * So they won't overrun _soc_td3_asf_cfg_tbl.
     */
    /* coverity[overrun-local : FALSE] */
    LOG_CLI(("%-5s %-3d %-3d %-6d %-6d %-3d %-3d   ", SOC_PORT_NAME(unit, port),
             mode, ct_class, _SOC_TD3_CT_CLASS_TO_SPEED_MAP(min_sp),
            _SOC_TD3_CT_CLASS_TO_SPEED_MAP(max_sp), fifo_threshold,
            fifo_depth));
    for (src_class = 0; src_class < _SOC_TD3_ASF_CLASS_CNT; src_class++) {
        LOG_CLI(("%-3d ", xmit_start_cnt[src_class]));
    }

    SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                        SOC_INFO(unit).port_l2p_mapping[port], &credit_entry));
    egr_mmu_cell_credits = soc_EGR_MMU_CELL_CREDITm_field32_get(unit,
                           &credit_entry, CREDITf);

    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));
    SOC_IF_ERROR_RETURN(MAC_PAUSE_GET(macd, unit, port, &txp, &rxp));

    LOG_CLI((" %-3d %-3d %-3d %-3d\n", mmu_prebuf, mmu_ep_credits,
             egr_mmu_cell_credits, rxp));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_td3_port_asf_detach                                              *
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
soc_td3_port_asf_detach(int unit, soc_port_t port)
{
    if (_soc_td3_asf_ctrl[unit] == NULL) {
        return SOC_E_INIT;
    }

    if (soc_td3_port_asf_valid(unit, port)) {
        SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_ports, port);
        SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->pause_restore, port);
        if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_ss, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_ss, port);
        } else if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_sf, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_sf, port);
        } else if (SOC_PBMP_MEMBER(_soc_td3_asf_ctrl[unit]->asf_fs, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_td3_asf_ctrl[unit]->asf_fs, port);
        }
    }
    return SOC_E_NONE;
}

#endif /* BCM_TRIDENT3_SUPPORT */


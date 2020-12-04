/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM MacSec table operations
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif

/* Macsec calendar size */
#define FL_MS_CAL_COL_WIDTH  6
#define FL_MS_CAL_ROW_WIDTH 16
#define FL_MS_CAL_LEN       (FL_MS_MAX_CAL_LEN)

/* Macsec calendar IDLE slot token */
#define FL_MS_PORT_IDLE     (FL_MS_SLOT_IDLE)

/* Aggregated FL port */
#define FL_MS_NUM_FL_PORTS (FL_NUM_EXT_PORTS / 4)

/* Bandwidth in Mbps */
#define FL_MS_MAX_BW_950MHZ 600240


/* Macsec frequency */
typedef enum tdm_fl_macsec_freq_e {
    MACSEC_FREQ_950MHZ = 950,
    MACSEC_FREQ_380MHZ = 380,
    MACSEC_FREQ_190MHZ = 190
} tdm_fl_macsec_freq_t;

/* Macsec port group */
typedef enum tdm_fl_macsec_port_group_e {
    /*! GE port group */
    QPORT0 = 0,
    QPORT1,
    QPORT2,
    QPORT3,
    /*! XL port group */
    XLPORT0,
    XLPORT1,
    XLPORT2,
    /*! CL port group */
    CLPORT0,
    CLPORT1,
    CLPORT2,
    CLPORT3,
    /*! Port group count */
    PORT_GROUP_COUNT
} tdm_fl_macsec_port_group_t;

/* Macsec port struct */
typedef struct tdm_fl_macsec_port_s {
    /*! Pseudo Physical port or logical port*/
    int lport;

    /*! Physical port */
    int pport;

    /*! Macsec port */
    int msport;

    /*! Port group */
    int port_group;

    /*! Exclusive port group */
    int exc_port_group;
} tdm_fl_macsec_port_t;

/* chip-specific macsec port setting */
static const tdm_fl_macsec_port_t
tdm_fl_macsec_port_map[FL_NUM_EXT_PORTS] = {
/*    lgc_port | phy_port | ms_port | port_group   | exc_port */
    /* CPU IDLE slot */
    { 0,         0,        FL_MS_PORT_IDLE,  0,            0},
    /* GEPort or GPort */
    { 1,         2,          0,         QPORT0,      XLPORT0},
    { 2,         3,          1,         QPORT0,      XLPORT0},
    { 3,         4,          2,         QPORT0,      XLPORT0},
    { 4,         5,          3,         QPORT0,      XLPORT0},
    { 5,         6,          4,         QPORT0,      XLPORT0},
    { 6,         7,          5,         QPORT0,      XLPORT0},
    { 7,         8,          6,         QPORT0,      XLPORT0},
    { 8,         9,          7,         QPORT0,      XLPORT0},
    { 9,        10,          8,         QPORT0,      XLPORT0},
    {10,        11,          9,         QPORT0,      XLPORT0},
    {11,        12,         10,         QPORT0,      XLPORT0},
    {12,        13,         11,         QPORT0,      XLPORT0},
    {13,        14,         12,         QPORT0,      XLPORT0},
    {14,        15,         13,         QPORT0,      XLPORT0},
    {15,        16,         14,         QPORT0,      XLPORT0},
    {16,        17,         15,         QPORT0,      XLPORT0},
    {17,        18,         16,         QPORT1,      XLPORT1},
    {18,        19,         17,         QPORT1,      XLPORT1},
    {19,        20,         18,         QPORT1,      XLPORT1},
    {20,        21,         19,         QPORT1,      XLPORT1},
    {21,        22,         20,         QPORT1,      XLPORT1},
    {22,        23,         21,         QPORT1,      XLPORT1},
    {23,        24,         22,         QPORT1,      XLPORT1},
    {24,        25,         23,         QPORT1,      XLPORT1},
    {25,        26,         24,         QPORT1,      XLPORT1},
    {26,        27,         25,         QPORT1,      XLPORT1},
    {27,        28,         26,         QPORT1,      XLPORT1},
    {28,        29,         27,         QPORT1,      XLPORT1},
    {29,        30,         28,         QPORT1,      XLPORT1},
    {30,        31,         29,         QPORT1,      XLPORT1},
    {31,        32,         30,         QPORT1,      XLPORT1},
    {32,        33,         31,         QPORT1,      XLPORT1},
    {33,        34,         32,         QPORT2,      XLPORT2},
    {34,        35,         33,         QPORT2,      XLPORT2},
    {35,        36,         34,         QPORT2,      XLPORT2},
    {36,        37,         35,         QPORT2,      XLPORT2},
    {37,        38,         36,         QPORT2,      XLPORT2},
    {38,        39,         37,         QPORT2,      XLPORT2},
    {39,        40,         38,         QPORT2,      XLPORT2},
    {40,        41,         39,         QPORT2,      XLPORT2},
    {41,        42,         40,         QPORT2,      XLPORT2},
    {42,        43,         41,         QPORT2,      XLPORT2},
    {43,        44,         42,         QPORT2,      XLPORT2},
    {44,        45,         43,         QPORT2,      XLPORT2},
    {45,        46,         44,         QPORT2,      XLPORT2},
    {46,        47,         45,         QPORT2,      XLPORT2},
    {47,        48,         46,         QPORT2,      XLPORT2},
    {48,        49,         47,         QPORT2,      XLPORT2},
    /* XLPort */
    {49,        50,          4,        XLPORT0,       QPORT0},
    {50,        51,          5,        XLPORT0,       QPORT0},
    {51,        52,          6,        XLPORT0,       QPORT0},
    {52,        53,          7,        XLPORT0,       QPORT0},
    {53,        54,         20,        XLPORT1,       QPORT1},
    {54,        55,         21,        XLPORT1,       QPORT1},
    {55,        56,         22,        XLPORT1,       QPORT1},
    {56,        57,         23,        XLPORT1,       QPORT1},
    {57,        58,         36,        XLPORT2,       QPORT2},
    {58,        59,         37,        XLPORT2,       QPORT2},
    {59,        60,         38,        XLPORT2,       QPORT2},
    {60,        61,         39,        XLPORT2,       QPORT2},
    /* CLPort */
    {61,        62,         44,        CLPORT0,       QPORT2},
    {62,        63,         45,        CLPORT0,       QPORT2},
    {63,        64,         46,        CLPORT0,       QPORT2},
    {64,        65,         47,        CLPORT0,       QPORT2},
    {65,        66,         32,        CLPORT1,       QPORT2},
    {66,        67,         33,        CLPORT1,       QPORT2},
    {67,        68,         34,        CLPORT1,       QPORT2},
    {68,        69,         35,        CLPORT1,       QPORT2},
    {69,        70,         40,        CLPORT2,       QPORT2},
    {70,        71,         41,        CLPORT2,       QPORT2},
    {71,        72,         42,        CLPORT2,       QPORT2},
    {72,        73,         43,        CLPORT2,       QPORT2},
    {73,        74,         16,        CLPORT3,       QPORT1},
    {74,        75,         17,        CLPORT3,       QPORT1},
    {75,        76,         18,        CLPORT3,       QPORT1},
    {76,        77,         19,        CLPORT3,       QPORT1},
    /* Not in use */
    {77,        -1,         -1,              0,            0},
    {78,        -1,         -1,              0,            0},
    {79,        -1,         -1,              0,            0}
};


/**
@name: tdm_fl_macsec_init
@param:

Check validity of macsec config
 */
static int
tdm_fl_macsec_init(tdm_mod_t *_tdm)
{
    int i, result = PASS;
    int param_phy_lo, param_phy_hi;
    int *param_ms_port_en;
    enum port_speed_e *param_speeds;

    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_ms_port_en = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_port_en;
    param_speeds     = _tdm->_chip_data.soc_pkg.speed;

    /* ms_port_en[]: map physical port to pseudo physical port
     * to achieve the same indexing as speed array and state array.
     */
    for (i = param_phy_lo; i <= param_phy_hi + 1; i++) {
        if (i >= FL_PHY_PORT_NUMBER_OFFSET &&
            i < FL_NUM_EXT_PORTS) {
            param_ms_port_en[i-FL_PHY_PORT_NUMBER_OFFSET] = param_ms_port_en[i];

        }
    }

    for (i = param_phy_lo; i <= param_phy_hi; i++) {
        if (param_ms_port_en[i])
            TDM_PRINT4("ms_cfg: lport=%2d, pport=%2d, msport=%2d, speed=%0dG\n",
                   i, tdm_fl_macsec_port_map[i].pport,
                   tdm_fl_macsec_port_map[i].msport, param_speeds[i]/1000);
    }

    return result;
}

/**
@name: tdm_fl_macsec_pre_check
@param:

Check validity of macsec config
 */
static int
tdm_fl_macsec_pre_check(tdm_mod_t *_tdm)
{
    int lport, grp, result = PASS;
    int param_ms_freq, param_phy_lo, param_phy_hi;
    int *param_ms_port_en;
    enum port_speed_e *param_speeds;
    int port_group_en[PORT_GROUP_COUNT];
    int avail_bw = 0, req_bw = 0;
    int ms_port_cnt = 0;

    param_ms_freq    = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_clk_freq;
    param_ms_port_en = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_port_en;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds     = _tdm->_chip_data.soc_pkg.speed;

    /* Check frequency */
    switch (param_ms_freq) {
        case MACSEC_FREQ_950MHZ:
        case MACSEC_FREQ_380MHZ:
        case MACSEC_FREQ_190MHZ:
            break;
        default:
            result = FAIL;
            TDM_ERROR1("MacSec config: Invalid macsec frequency %0dMhz\n",
                       param_ms_freq);
            break;
    }

    /* Check mutually exclusive ports */
    TDM_MEMSET(port_group_en, 0, sizeof(int) * PORT_GROUP_COUNT);
    for (lport = param_phy_lo; lport <= param_phy_hi; lport++) {
        if (param_ms_port_en[lport] == 1) {
            grp = tdm_fl_macsec_port_map[lport].port_group;
            port_group_en[grp] = 1;
        }
    }
    for (lport = param_phy_lo; lport <= param_phy_hi; lport++) {
        if (param_ms_port_en[lport] == 1) {
            int exc_grp = tdm_fl_macsec_port_map[lport].exc_port_group;
            if (port_group_en[exc_grp] == 1) {
                int lport_2 = lport + 1;
                for (; lport_2 <= param_phy_hi; lport_2++) {
                    if (param_ms_port_en[lport_2] == 1) {
                        grp = tdm_fl_macsec_port_map[lport_2].port_group;
                        if (grp == exc_grp) break;
                    }
                }
                result = FAIL;
                TDM_ERROR2("MacSec config: Conflict pports [%0d, %0d]\n",
                           tdm_fl_macsec_port_map[lport].pport,
                           tdm_fl_macsec_port_map[lport_2].pport);
            }
        }
    }

    /* Check bandwidth */
    avail_bw = (FL_MS_MAX_BW_950MHZ * param_ms_freq) / MACSEC_FREQ_950MHZ;
    req_bw   = 0;
    for (lport = param_phy_lo; lport <= param_phy_hi; lport++) {
        if (param_ms_port_en[lport] == 1) {
            req_bw += param_speeds[lport];
            ms_port_cnt++;
        }
    }
    if (avail_bw < req_bw) {
        result = FAIL;
        TDM_ERROR3("MacSec config: %s, req_bw=%0dM, avail_bw=%0dM \n",
                   "Invalid MacSec port bandwidth",
                   req_bw, avail_bw);
    }
    TDM_PRINT1("ms_cfg: num_ms_ports=%0d\n", ms_port_cnt);
    TDM_PRINT3("ms_cfg: freq=%0dMHz, req_bw=%0dM, avail_bw=%0dM\n",
               param_ms_freq, req_bw, avail_bw);

    return result;
}


/**
@name: tdm_fl_macsec_get_cal_len
@param:

Get MacSec TDM calendar length based on macsec port info.
 */
static int
tdm_fl_macsec_get_cal_len(tdm_mod_t *_tdm)
{
    int cal_len, lport, grp;
    int param_phy_lo, param_phy_hi;
    int *param_ms_port_en;
    enum port_speed_e *param_speeds;
    int gport_en = 0, xlport_en = 0, clport_en = 0;
    int max_speed = 0;

    param_ms_port_en = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_port_en;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds     = _tdm->_chip_data.soc_pkg.speed;

    for (lport = param_phy_lo; lport <= param_phy_hi; lport++) {
        if (param_ms_port_en[lport] == 1) {
            max_speed = max_speed < param_speeds[lport] ?
                        param_speeds[lport] : max_speed;

            grp = tdm_fl_macsec_port_map[lport].port_group;
            switch (grp) {
                case QPORT0: case QPORT1: case QPORT2: case QPORT3:
                    gport_en = 1;
                    break;
                case XLPORT0: case XLPORT1: case XLPORT2:
                    xlport_en = 1;
                    break;
                case CLPORT0: case CLPORT1: case CLPORT2: case CLPORT3:
                    clport_en = 1;
                    break;
                default:
                    break;
            }
        }
    }
    /* Line Card with speed disparity between front panel and back plane */
    if (gport_en == 1 && xlport_en == 0 && clport_en == 1) {
        cal_len = 96;
    }
    /* Line Card front panel only (GPORTs only) */
    else if (gport_en == 1 && xlport_en == 0 && clport_en == 0) {
        cal_len = 48;
    }
    /* Connectivity (both XLPORT and CLPORT are used) */
    else if (gport_en == 0 && xlport_en == 1 && clport_en == 1) {
        cal_len = 48;
    }
    /* Line Card front panel only (XLPORT only) */
    else if (gport_en == 0 && xlport_en == 1 && clport_en == 0) {
        cal_len = (max_speed <= SPEED_10G) ? 24 : /* 10G needs 4x6 */
                  (max_speed <= SPEED_20G) ? 12 : /* 20G needs 2x6 */
                  6;                              /* 40G needs 1x6 */
    }
    /* Uplink card or MACsec bump-in-the-wire (CLPORT only) */
    else if (gport_en == 0 && xlport_en == 0 && clport_en == 1) {
        cal_len = (max_speed <= SPEED_25G) ? 24 : /* 25G  needs 4x6 */
                  (max_speed <= SPEED_50G) ? 12 : /* 50G  needs 2x6 */
                  6;                              /* 100G needs 1x6 */
    }
    /* all other (unspecified) cases */
    else {
        cal_len = 96;
    }

    return cal_len;
}


/**
@name: tdm_fl_macsec_gen_cal
@param:

Generate macsec tdm calendar
 */
static int
tdm_fl_macsec_gen_cal(tdm_mod_t *_tdm)
{
    int result = PASS;
    int lport, msport, avail_bw, m;
    int param_ms_freq, param_phy_lo, param_phy_hi;
    int *param_ms_port_en;
    enum port_speed_e *param_speeds;
    int cal_len, cal_rows/* , cal_cols */;
    int cal[FL_MS_MAX_CAL_LEN];
    int lport_slots[FL_NUM_EXT_PORTS];
    int fl_port, fl_port_cnt, i, j;
    int fl_port_slots[FL_MS_NUM_FL_PORTS];
    int fl_port_col[FL_MS_NUM_FL_PORTS];
    int total_slots = 0;
    int p[FL_MS_CAL_COL_WIDTH];
    int ave_idle_slots_per_row;
    int sorted_fl_ports[FL_MS_NUM_FL_PORTS];
    int col, row;
    int col_avail_slots, col_req_slots;
    int is_idle_existed;


    param_ms_freq    = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_clk_freq;
    param_ms_port_en = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_port_en;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds     = _tdm->_chip_data.soc_pkg.speed;

    /* init macsec calendar with IDLE slot token */
    for (i = 0; i < FL_MS_MAX_CAL_LEN; i++) {
        cal[i] = FL_MS_PORT_IDLE;
    }

    /* Set Macsec TDM calendar length */
    cal_len  = tdm_fl_macsec_get_cal_len(_tdm);
    TDM_PRINT1("ms_cfg: cal_len=%0d\n", cal_len);

    /* cal_cols = FL_MS_CAL_COL_WIDTH; */
    cal_rows = cal_len / FL_MS_CAL_COL_WIDTH;
    cal_rows = cal_rows <= 0 ? 1 :
               cal_rows >FL_MS_CAL_ROW_WIDTH ? FL_MS_CAL_ROW_WIDTH :
               cal_rows;

    /* Set number of slots for each Macsec port */
    TDM_MEMSET(lport_slots, 0, sizeof(int) * FL_NUM_EXT_PORTS);
    avail_bw = (FL_MS_MAX_BW_950MHZ * param_ms_freq) / MACSEC_FREQ_950MHZ;
    for (lport = param_phy_lo; lport <= param_phy_hi; lport++) {
        if (param_ms_port_en[lport] == 0) continue;
        lport_slots[lport] = param_speeds[lport] * cal_len / avail_bw + 1;
        /* round up number of slots to minimal power of 2 */
        m = 1;
        do {
            if (lport_slots[lport] <= m) {
                lport_slots[lport] = m;
                break;
            }
            m *= 2;
        } while (m < FL_MS_CAL_ROW_WIDTH);
    }

    /* Set number of slots for FL/aggregated port */
    TDM_MEMSET(fl_port_slots, 0, sizeof(int) * FL_MS_NUM_FL_PORTS);
    for (lport = param_phy_lo; lport <= param_phy_hi; lport++) {
        if (param_ms_port_en[lport] == 0) continue;

        msport  = tdm_fl_macsec_port_map[lport].msport;
        fl_port = msport / 4;

        fl_port_slots[fl_port] += lport_slots[lport];
        total_slots += lport_slots[lport];

        TDM_PRINT3("ms_slots: msport=%2d, fl_port=%2d, ms_slots=%0d\n",
                   msport, fl_port, lport_slots[lport]);
    }

    /* Determine overall row slot pattern with idle */
    TDM_MEMSET(p, 0, sizeof(int) * FL_MS_CAL_COL_WIDTH);
    ave_idle_slots_per_row = (cal_len - total_slots) / cal_rows;

    TDM_PRINT3("pattern: avail_slots=%0d, req_slots=%0d, idle_slots=%0d\n",
               cal_len, total_slots, (cal_len - total_slots));
    TDM_PRINT1("pattern: idle_slots_per_row=%0d\n", ave_idle_slots_per_row);

    switch (ave_idle_slots_per_row) {
        case 5:  p[0]=0; p[1]=1; p[2]=1; p[3]=1; p[4]=1; p[5]=1; break;
        case 4:  p[0]=0; p[1]=1; p[2]=1; p[3]=0; p[4]=1; p[5]=1; break;
        case 3:  p[0]=0; p[1]=1; p[2]=0; p[3]=1; p[4]=0; p[5]=1; break;
        case 2:  p[0]=0; p[1]=0; p[2]=1; p[3]=0; p[4]=0; p[5]=1; break;
        case 1:  p[0]=0; p[1]=0; p[2]=0; p[3]=0; p[4]=0; p[5]=1; break;
        default: p[0]=0; p[1]=0; p[2]=0; p[3]=0; p[4]=0; p[5]=0; break;
    }

    TDM_PRINT0("\n\npattern: [");
    for (col = 0; col < FL_MS_CAL_COL_WIDTH; col++)
        TDM_PRINT1("%0d, ", p[col]);
    TDM_PRINT0("]\n\n");

    /* Sort fl ports by descending order of fl port slots */
    TDM_MEMSET(sorted_fl_ports, 0, sizeof(int) * FL_MS_NUM_FL_PORTS);
    fl_port_cnt = 0;
    for (fl_port = 0; fl_port < FL_MS_NUM_FL_PORTS; fl_port++) {
        if (fl_port_slots[fl_port] > 0) {
            sorted_fl_ports[fl_port_cnt++] = fl_port;
        }
    }
    for (i = 0; i < fl_port_cnt; i++) {
        int pos = i;
        for (j = i + 1; j < fl_port_cnt; j++) {
            if (fl_port_slots[sorted_fl_ports[pos]] <
                fl_port_slots[sorted_fl_ports[j]]) {
                pos = j;
            }
        }
        if (pos != i) {
            int tmp = sorted_fl_ports[pos];
            sorted_fl_ports[pos] = sorted_fl_ports[i];
            sorted_fl_ports[i] = tmp;
        }
    }

    TDM_PRINT3("cal_len=%0d, cal_rows=%0d, cal_cols=%0d\n",
               cal_len, cal_rows, FL_MS_CAL_COL_WIDTH);

    /* Assign FL ports with pattern, or consolidate multiple of FL ports */
    TDM_MEMSET(fl_port_col, 0, sizeof(int) * FL_MS_NUM_FL_PORTS);
    col = 0;
    col_avail_slots = -1;
    for (i = 0; i < fl_port_cnt; i++) {
        int step_width;
        int prev_row = -1;

        fl_port = sorted_fl_ports[i];
        col_req_slots = fl_port_slots[fl_port];

        if (col_req_slots > col_avail_slots) {
            if (col_avail_slots != -1) col++;
            while (col < FL_MS_CAL_COL_WIDTH && p[col] == 1) col++;
            if (!(col < FL_MS_CAL_COL_WIDTH)) {
                TDM_ERROR1("Macsec cal: No enough slots for fl_port %0d\n",
                           fl_port);
                result = FAIL;
                break;
            }
            col_avail_slots = cal_rows;
        }

        step_width = cal_rows / col_req_slots;
        fl_port_col[fl_port] = col;
        TDM_PRINT4("fl_slots: fl_port=%2d, slots=%0d, col=%0d, step=%0d\n",
                   fl_port, fl_port_slots[fl_port], col, step_width);

        for (row = 0; row < cal_rows; row++) {
            int pos = row * FL_MS_CAL_COL_WIDTH + col;
            if (cal[pos] == FL_MS_PORT_IDLE) {
                int dist = (prev_row == -1) ? cal_rows : (row - prev_row);
                if (dist >= step_width) {
                    cal[pos] = fl_port;
                    prev_row = row;
                }
            }
        }
        col_avail_slots -= col_req_slots;
    }

    for (row = 0; row < cal_rows; row++) {
        TDM_PRINT1("[fl_cal] row[%2d]: ", row);
        for (col = 0; col < FL_MS_CAL_COL_WIDTH; col++) {
            int pos = row * FL_MS_CAL_COL_WIDTH + col;
            TDM_PRINT1("%2d, ", cal[pos]);
        }
        TDM_PRINT0("\n");
    }
    TDM_PRINT0("\n");

    /* Convert FL ports into MS ports */
    for (lport = param_phy_lo; lport <= param_phy_hi; lport++) {
        int step_width;
        int dist = -1;
        if (param_ms_port_en[lport] == 0) continue;
        msport = tdm_fl_macsec_port_map[lport].msport;
        fl_port = msport / 4;

        step_width = cal_rows / lport_slots[lport];
        step_width = step_width <= 0 ? 1 : step_width;
        for (row = 0; row < cal_rows; row++) {
            int pos = (row * FL_MS_CAL_COL_WIDTH + fl_port_col[fl_port]) %
                      FL_MS_MAX_CAL_LEN;
            if (cal[pos] == fl_port) {
                if (dist == -1 || dist == step_width) {
                    cal[pos] = msport;
                    dist = 0;
                }
            }
            if (dist != -1) dist++;
        }
    }

    for (row = 0; row < cal_rows; row++) {
        TDM_PRINT1("[ms_cal] row[%2d]: ", row);
        for (col = 0; col < FL_MS_CAL_COL_WIDTH; col++) {
            int pos = row * FL_MS_CAL_COL_WIDTH + col;
            TDM_PRINT1("%2d, ", cal[pos]);
        }
        TDM_PRINT0("\n");
    }
    TDM_PRINT0("\n\n");

    /* Insert one IDLE slot if there is no IDLE slot in the calendar */
    is_idle_existed = 0;
    for (i = 0; i < FL_MS_MAX_CAL_LEN && i < cal_len; i++) {
        if (cal[i] == FL_MS_PORT_IDLE) {
            is_idle_existed = 1;
            break;
        }
    }
    if (is_idle_existed == 0) {
        if (cal_len < FL_MS_MAX_CAL_LEN) {
            TDM_PRINT1("[ms_cal] Insert one IDLE slot at %0d-th pos\n",
                       cal_len+1);
            cal[cal_len++] = FL_MS_PORT_IDLE;
        }
    }

    /* copy calendar data to output */
    for (i = 0; i < FL_MS_MAX_CAL_LEN; i++) {
        if (i < cal_len) {
            _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_cal[i] = cal[i];
        }
    }
    _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_cal_len = cal_len;

    return result;
}


/**
@name: tdm_fl_macsec_post_check
@param:

Check MacSec TDM calendar content.
 */
static int
tdm_fl_macsec_post_check(tdm_mod_t *_tdm)
{
    int result = PASS;
    int j, lport, msport, avail_bw, m, slot_cnt;
    int param_ms_freq, param_phy_lo, param_phy_hi;
    int *param_ms_port_en;
    int *param_ms_cal, param_ms_cal_len;
    enum port_speed_e *param_speeds;
    int lport_slots[FL_NUM_EXT_PORTS];

    param_ms_freq    = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_clk_freq;
    param_ms_port_en = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_port_en;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds     = _tdm->_chip_data.soc_pkg.speed;
    param_ms_cal_len = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_cal_len;
    param_ms_cal     = _tdm->_chip_data.soc_pkg.soc_vars.fl.ms_cal;

    /* If one extra IDLE slot is inserted, adjust ms_cal_len */
    param_ms_cal_len = param_ms_cal_len - (param_ms_cal_len % 6);

    if (param_ms_cal != NULL && param_ms_cal_len > 0) {
        /* Set expected number of slots per port */
        TDM_MEMSET(lport_slots, 0, sizeof(int) * FL_NUM_EXT_PORTS);
        avail_bw = (FL_MS_MAX_BW_950MHZ * param_ms_freq) / MACSEC_FREQ_950MHZ;
        for (lport = param_phy_lo; lport <= param_phy_hi; lport++) {
            if (param_ms_port_en[lport] == 0) continue;
            lport_slots[lport] = param_speeds[lport] *
                                 param_ms_cal_len / avail_bw + 1;
            m = 1;
            do {
                if (lport_slots[lport] <= m) {
                    lport_slots[lport] = m;
                    break;
                }
                m *= 2;
            } while (m < FL_MS_CAL_ROW_WIDTH);
        }

        /* Check allocated numbeer of slots against expected value */
        for (lport = param_phy_lo; lport <= param_phy_hi; lport++) {
            if (param_ms_port_en[lport] == 0) continue;
            slot_cnt = 0;
            msport = tdm_fl_macsec_port_map[lport].msport;
            for (j = 0; j < param_ms_cal_len; j++) {
                if (param_ms_cal[j] == msport) slot_cnt++;
            }
            if (slot_cnt != lport_slots[lport]) {
                result = FAIL;
                TDM_ERROR4("%s pport=%2d, req_slots=%0d, act_slots=%0d\n",
                       "Macsec cal: ",
                       tdm_fl_macsec_port_map[lport].pport,
                       lport_slots[lport], slot_cnt);
            }
        }
    }

    return result;
}

/**
@name: tdm_fl_macsec_proc
@param:

Main function for MacSec TDM calendar generation.
 */
int
tdm_fl_macsec_proc(tdm_mod_t *_tdm)
{
    int result = PASS;

    TDM_PRINT0("TDM: ########################################### \n\n");
    TDM_PRINT0("TDM: MacSec TDM Calendar Generation (start) \n\n");
    TDM_PRINT0("TDM: ########################################### \n\n");

    /* init parameter */
    if (tdm_fl_macsec_init(_tdm) != PASS) {
        return FAIL;
    }

    /* check validity of macsec config */
    if (tdm_fl_macsec_pre_check(_tdm) != PASS) {
        return FAIL;
    }

    /* Generate Macsec TDM calendar */
    if (tdm_fl_macsec_gen_cal(_tdm) != PASS) {
        return FAIL;
    }

   /* Check Macsec TDM calendar result */
    if (tdm_fl_macsec_post_check(_tdm) != PASS) {
        return FAIL;
    }

    TDM_PRINT0("TDM: ########################################### \n\n");
    TDM_PRINT0("TDM: MacSec TDM Calendar Generation (end) \n");
    TDM_PRINT0("TDM: ########################################### \n\n");

    return result;
}

/*! \file bcm56780_a0_tm_e2e_settings.h
 *
 * File contains recommended E2E settings from Arch team for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_TM_E2E_SETTINGS_H
#define BCM56780_A0_TM_E2E_SETTINGS_H

#define CLK_1350MHZ 0
#define CLK_1175MHZ 1
#define CLK_1025MHZ 2
#define CLK_950MHZ  3

#define SOC_SWITCH_BYPASS_MODE_LOW  2
#define TDM_LENGTH              33
#define NULL_SLOT_COUNTER_VALUE 5000

/*******************************************************************************
 * IDB SETTINGS
 */
static const struct bcm56780_a0_obm_buffer_config_s {
    int buffer_start;
    int buffer_end;
} bcm56780_a0_obm_buffer_config_settings[9][8] = {
    /* Indexed by number of lanes used in the port */
    { /* 0 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 1 lane */
        {   0, 511},      /* buffer_config port 0 */
        { 512, 1023},     /* buffer_config port 1 */
        {1024, 1535},     /* buffer_config port 2 */
        {1536, 2047},     /* buffer_config port 3 */
        {2048, 2559},     /* buffer_config port 4 */
        {2560, 3071},     /* buffer_config port 5 */
        {3072, 3583},     /* buffer_config port 6 */
        {3584, 4095}      /* buffer_config port 7 */
    },
    { /* 2 lanes */
        {   0, 1023},     /* buffer_config port 0 */
        {   0, 1023},     /* buffer_config port 1 */
        {1024, 2047},     /* buffer_config port 2 */
        {1024, 2047},     /* buffer_config port 3 */
        {2048, 3071},     /* buffer_config port 4 */
        {2048, 3071},     /* buffer_config port 5 */
        {3072, 4095},     /* buffer_config port 6 */
        {3072, 4095}      /* buffer_config port 7 */
    },
    { /* 3 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 4 lanes */
        {   0, 2047},     /* buffer_config port 0 */
        {   0, 2047},     /* buffer_config port 1 */
        {   0, 2047},     /* buffer_config port 2 */
        {   0, 2047},     /* buffer_config port 3 */
        {2048, 4095},     /* buffer_config port 4 */
        {2048, 4095},     /* buffer_config port 5 */
        {2048, 4095},     /* buffer_config port 6 */
        {2048, 4095}      /* buffer_config port 7 */
    },
    { /* 5 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 6 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 7 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 8 lanes */
        {0, 4095},        /* buffer_config port 0 */
        {0, 4095},        /* buffer_config port 1 */
        {0, 4095},        /* buffer_config port 2 */
        {0, 4095},        /* buffer_config port 3 */
        {0, 4095},        /* buffer_config port 4 */
        {0, 4095},        /* buffer_config port 5 */
        {0, 4095},        /* buffer_config port 6 */
        {0, 4095}         /* buffer_config port 7 */
    }
};

static const struct bcm56780_a0_obm_setting_s {
    int discard_limit;
    int lossless_discard;
    int port_xoff;
    int port_xon;
    int lossless_xoff;
    int lossless_xon;
    int lossy_low_pri;
    int lossy_discard;
    int sop_discard_mode;
} bcm56780_a0_obm_settings[3][9] = {
    /* LOSSY Settings */
    { /* Indexed by number of lanes used in the port */
        { /* 0 lanes - invalid */ 0 },
        { /* 1 lane */
            509,             /* discard_limit */
            4093,            /* lossless_discard */
            4093,            /* port_xoff */
            4093,            /* port_xon */
            4093,            /* lossless_xoff */
            4093,            /* lossless_xon */
            395,             /* lossy_low_pri */
            507,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            1021,            /* discard_limit */
            4093,            /* lossless_discard */
            4093,            /* port_xoff */
            4093,            /* port_xon */
            4093,            /* lossless_xoff */
            4093,            /* lossless_xon */
            907,             /* lossy_low_pri */
            1019,            /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid */ 0 },
        { /* 4 lanes */
            2045,            /* discard_limit */
            4093,            /* lossless_discard */
            4093,            /* port_xoff */
            4093,            /* port_xon */
            4093,            /* lossless_xoff */
            4093,            /* lossless_xon */
            1931,            /* lossy_low_pri */
            2043,            /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 5 lanes - Invalid */ 0 },
        { /* 6 lanes - Invalid */ 0 },
        { /* 7 lanes - Invalid */ 0 },
        { /* 8 lanes */
            4093,            /* discard_limit */
            4093,            /* lossless_discard */
            4093,            /* port_xoff */
            4093,            /* port_xon */
            4093,            /* lossless_xoff */
            4093,            /* lossless_xon */
            3979,            /* lossy_low_pri */
            4091,            /* lossy_discard */
            1                /* sop_discard_mode */
        }
    },
    /* LOSSLESS Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 lanes - invalid */ 0 },
        { /* 1 lane */
            509,             /* discard_limit */
            4093,            /* lossless_discard */
            179,             /* port_xoff */
            171,             /* port_xon */
            81,              /* lossless_xoff */
            73,              /* lossless_xon */
            79,              /* lossy_low_pri */
            191,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            1021,            /* discard_limit */
            4093,            /* lossless_discard */
            509,             /* port_xoff */
            501,             /* port_xon */
            194,             /* lossless_xoff */
            186,             /* lossless_xon */
            79,              /* lossy_low_pri */
            191,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 lanes - invalid */ 0 },
        { /* 4 lanes */
            1837,            /* discard_limit */
            4093,            /* lossless_discard */
            1109,            /* port_xoff */
            1101,            /* port_xon */
            384,             /* lossless_xoff */
            376,             /* lossless_xon */
            79,              /* lossy_low_pri */
            191,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 5 lanes - Invalid */ 0 },
        { /* 6 lanes - Invalid */ 0 },
        { /* 7 lanes - Invalid */ 0 },
        { /* 8 lanes */
            3677,            /* discard_limit */
            4093,            /* lossless_discard */
            2364,            /* port_xoff */
            2356,            /* port_xon */
            768,             /* lossless_xoff */
            760,             /* lossless_xon */
            79,              /* lossy_low_pri */
            191,             /* lossy_discard */
            1                /* sop_discard_mode */
        }
    },
    /* LOSSY + LOSSLESS Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 lanes - invalid */ 0 },
        { /* 1 lane */
            509,             /* discard_limit */
            4093,            /* lossless_discard */
            179,             /* port_xoff */
            171,             /* port_xon */
            81,              /* lossless_xoff */
            73,              /* lossless_xon */
            79,              /* lossy_low_pri */
            191,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            1021,            /* discard_limit */
            4093,            /* lossless_discard */
            509,             /* port_xoff */
            501,             /* port_xon */
            194,             /* lossless_xoff */
            186,             /* lossless_xon */
            79,              /* lossy_low_pri */
            191,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid */ 0 },
        { /* 4 lanes */
            2045,            /* discard_limit */
            4093,            /* lossless_discard */
            1109,            /* port_xoff */
            1101,            /* port_xon */
            384,             /* lossless_xoff */
            376,             /* lossless_xon */
            79,              /* lossy_low_pri */
            191,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 5 lanes - Invalid */ 0 },
        { /* 6 lanes - Invalid */ 0 },
        { /* 7 lanes - Invalid */ 0 },
        { /* 8 lanes */
            4093,            /* discard_limit */
            4093,            /* lossless_discard */
            2364,            /* port_xoff */
            2356,            /* port_xon */
            768,             /* lossless_xoff */
            760,             /* lossless_xon */
            79,              /* lossy_low_pri */
            177,             /* lossy_discard */
            1                /* sop_discard_mode */
        }
    }
};

static const struct bcm56780_a0_idb_e2e_settings_s {
    int speed;
    uint32_t bmop_disable[SOC_SWITCH_BYPASS_MODE_LOW + 1]; /* Bubble MOP disable */
    int ca_ct_threshold;
    int obm_ct_threshold;
    int obm_unsatisfied_threshold;
    int ca_unsatisfied_threshold;
    int obm_enter_lagging_threshold;
    int obm_exit_lagging_threshold;
    int ca_enter_lagging_threshold;
    int ca_exit_lagging_threshold;
} bcm56780_a0_idb_e2e_settings_tbl[] = {
    {    -1, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20},       /* SAF */
    { 10000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 10GE */
    { 25000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 25GE */
    { 40000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 40GE */
    { 50000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 50GE */
    {100000, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20},       /* 100GE */
    {200000, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20},       /* 200GE */
    {400000, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20}        /* 400GE */
};

static const struct bcm56780_a0_idb_peek_depth_settings_s {
    int speed;
    int peek_depth;
} bcm56780_a0_peek_depth_settings_tbl[] = {
    {    -1, 1},       /* Default */
    { 10000, 1},       /* 10GE */
    { 25000, 1},       /* 25GE */
    { 40000, 1},       /* 40GE */
    { 50000, 1},       /* 50GE */
    {100000, 2},       /* 100GE */
    {200000, 3},       /* 200GE */
    {400000, 4}        /* 400GE */
};

static const struct bcm56780_a0_obm_buffer_config_s
    bcm56780_mgmt_obm_buffer_config_settings[2] = {
        {0,831},         /* buffer_config port 0 */
        {832,1663},      /* buffer_config port 1 */
};

static const struct bcm56780_a0_obm_setting_s bcm56780_mgmt_obm_settings[3] = {
    /* LOSSY Settings */
        { /* Index 0: Used for 10G port */
            828,             /* discard_limit */
            1661,            /* lossless_discard */
            1661,            /* port_xoff */
            1661,            /* port_xon */
            1661,            /* lossless_xoff */
            1661,            /* lossless_xon */
            600,             /* lossy_low_pri */
            826,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
    /* LOSSLESS Settings*/
        { /* Index 0: Used for 10G port */
            828,             /* discard_limit */
            1661,            /* lossless_discard */
            1661,            /* port_xoff */
            1661,            /* port_xon */
            1661,            /* lossless_xoff */
            1661,            /* lossless_xon */
            600,             /* lossy_low_pri */
            826,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
    /* LOSSY + LOSSLESS Settings*/
        { /* Index 0: Used for 10G port */
            828,             /* discard_limit */
            1661,            /* lossless_discard */
            1661,            /* port_xoff */
            1661,            /* port_xon */
            1661,            /* lossless_xoff */
            1661,            /* lossless_xon */
            600,             /* lossy_low_pri */
            826,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
};

/*******************************************************************************
 * TDM SETTINGS
 */
static const struct bcm56780_a0_max_spacing_settings_s {
    int all_speed;
    int special_slot;
} bcm56780_a0_max_spacing_settings_tbl[] = {
    /* all speed   special slot */
    {      69,     TDM_LENGTH}, /* 1350MHz */
    {      60,     TDM_LENGTH}, /* 1175MHz */
    {      53,     TDM_LENGTH}, /* 1025MHz */
    {      49,     TDM_LENGTH}  /*  950MHz */
};

typedef struct
bcm56780_a0_urg_pick_spacing_settings_s {
    int spacing_50;
    int spacing_100;
    int spacing_200;
    int spacing_400;
} bcm56780_a0_urg_pick_spacing_settings_t;

static const bcm56780_a0_urg_pick_spacing_settings_t
bcm56780_a0_urg_pick_spacing_settings_tbl[] = {
    /* 50G   100G   200G   400G */
    {   16,    8,     4,     2   }, /* 1350MHz */
    {   14,    7,     3,     0   }, /* 1175MHz */
    {   12,    6,     3,     0   }, /* 1025MHz */
    {   12,    6,     3,     0   }  /*  950MHz */
};

typedef struct bcm56780_a0_min_spacing_settings_s {
    uint32_t any_cell;
    uint32_t inter_pkt;
    uint32_t mid_pkt;
} bcm56780_a0_min_spacing_settings_t;

static const bcm56780_a0_min_spacing_settings_t
bcm56780_a0_min_spacing_settings_tbl[][4] = {
    /*    50G         100G        200G        400G  */
    /* a   i   m    a   i   m    a  i  m    a  i  m */
    { {8, 14, 14}, {4, 14, 14}, {2, 8, 8}, {2, 4, 4} }, /* 1350MHz */
    { {8, 14, 14}, {4, 14, 14}, {2, 6, 6}, {0, 0, 0} }, /* 1175MHz */
    { {8, 14, 14}, {4, 12, 12}, {2, 6, 6}, {0, 0, 0} }, /* 1025MHz */
    { {8, 14, 14}, {4, 12, 12}, {2, 6, 6}, {0, 0, 0} }  /*  950MHz */
};

/*******************************************************************************
 * EDB SETTINGS
 */
#define TD4_X9_EDB_MAX_CT_CLASSES 16
/* Number of valid cut thru classes: 1 SAF + 7 CT. */
#define TD4_X9_EDB_NUM_CT_CLASSES 8

/* EP2MMU credits for Aux ports. */
#define TD4_X9_EDB_EP2MMU_CRED_LBK 57
#define TD4_X9_EDB_EP2MMU_CRED_CPU 19
#define TD4_X9_EDB_EP2MMU_CRED_MGM  6

#define TD4_X9_EDB_XMIT_START_CNT_MGMT    27


/*
 * Number of PFC Optimized Config cases of xmit start count.
 *   General case + PFC_OPT cases
 */
#define TD4_X9_EDB_E2E_NUM_PFC_OPT_CFG  2


/* Transmit Start Count. */
typedef struct bcm56780_a0_edb_xmit_start_count_s {
    /*! SAF value. */
    uint32_t saf;

    /*! CT value. */
    uint32_t ct;
} bcm56780_a0_edb_xmit_start_count_t;

/* EDB E2E settings: ct_class, EP2MMU credit, xmit_cnt. */
typedef struct bcm56780_a0_edb_e2e_setting_s {
    /*! Speed in Mbps. */
    int speed;

    /*! CT class. */
    int ct_class;

    /*! EP2MMU credit. */
    int mmu_credit;
    int mmu_credit_pfc_opt;

    /*! Xmit start count. [0]->general case, [1-N]-> OPT cases. */
    bcm56780_a0_edb_xmit_start_count_t xmit_cnt[TD4_X9_EDB_E2E_NUM_PFC_OPT_CFG];
} bcm56780_a0_edb_e2e_setting_t;

/* ASF Core Config Table: SAF & CT XMIT_CNT and EP2MMU credit  */
/*
 * 1) ep2mmu credit: pick "PFC Opt 1350MHz" and "General case".
 *    Note that "PFC Opt 1350MHz" covers both 1350MHz and 1500MHz.
 * 2) xmit_start_cnt SAF: pick "PFC Opt 1350MHz/1500MHz" and "General case"
 * 3) xmit_start_cnt CT : pick "PFC Opt 1350MHz/1500MHz" and "General case"
 */

static const  bcm56780_a0_edb_e2e_setting_t
bcm56780_a0_edb_e2e_tbl[TD4_X9_EDB_NUM_CT_CLASSES] = {
   /* speed   ct     ep2mmu_credit        xmit_cnt {saf, ct}
   *          class  general opt1350    general   pfc_opt_1350/1500MHz
   */
    {     0,   0,     0,       0,      {{ 0,  0}, { 0,  0}} },   /*  SAF  */
    { 10000,   1,     7,       8,      {{ 8,  5}, { 9,  7}} },   /* 10GE  */
    { 25000,   2,    18,      18,      {{18, 13}, {24, 19}} },   /* 25GE  */
    { 40000,   3,    28,      29,      {{29, 20}, {36, 25}} },   /* 40GE  */
    { 50000,   4,    32,      32,      {{36, 25}, {46, 32}} },   /* 50GE  */
    {100000,   5,    65,      65,      {{40, 28}, {51, 36}} },   /* 100GE */
    {200000,   6,   116,      95,      {{45, 32}, {63, 44}} },   /* 200GE */
    {400000,   7,   214,     174,      {{35, 35}, {61, 61}} }    /* 400GE */
};
#endif /* BCM56780_A0_TM_E2E_SETTINGS_H */

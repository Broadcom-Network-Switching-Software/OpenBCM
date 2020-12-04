/*! \file bcm56880_a0_tm_e2e_settings.h
 *
 * File contains recommended E2E settings from Arch team for bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_E2E_SETTINGS_H
#define BCM56880_A0_TM_E2E_SETTINGS_H

#define CLK_1500MHZ 0
#define CLK_1350MHZ 1
#define CLK_950MHZ  2

#define SOC_SWITCH_BYPASS_MODE_LOW  2
#define TD4_TDM_LENGTH              33
#define TD4_NULL_SLOT_COUNTER_VALUE 5000

/*******************************************************************************
 * IDB SETTINGS
 */
static const struct td4_obm_buffer_config_s {
    int buffer_start;
    int buffer_end;
} td4_obm_buffer_config_settings[9][8] = {
    /* Indexed by number of lanes used in the port */
    { /* 0 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 1 lane */
        {0,459},         /* buffer_config port 0 */
        {460,919},       /* buffer_config port 1 */
        {920,1379},      /* buffer_config port 2 */
        {1380,1839},     /* buffer_config port 3 */
        {1840,2299},     /* buffer_config port 4 */
        {2300,2759},     /* buffer_config port 5 */
        {2760,3219},     /* buffer_config port 6 */
        {3220,3679}      /* buffer_config port 7 */
    },
    { /* 2 lanes */
        {0,919},         /* buffer_config port 0 */
        {0,919},         /* buffer_config port 1 */
        {920,1839},      /* buffer_config port 2 */
        {920,1839},      /* buffer_config port 3 */
        {1840,2759},     /* buffer_config port 4 */
        {1840,2759},     /* buffer_config port 5 */
        {2760,3679},     /* buffer_config port 6 */
        {2760,3679}      /* buffer_config port 7 */
    },
    { /* 3 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 4 lanes */
        {0,1839},        /* buffer_config port 0 */
        {0,1839},        /* buffer_config port 1 */
        {0,1839},        /* buffer_config port 2 */
        {0,1839},        /* buffer_config port 3 */
        {1840,3679},     /* buffer_config port 4 */
        {1840,3679},     /* buffer_config port 5 */
        {1840,3679},     /* buffer_config port 6 */
        {1840,3679}      /* buffer_config port 7 */
    },
    { /* 5 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 6 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 7 lanes - invalid */ {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    { /* 8 lanes */
        {0,3679},        /* buffer_config port 0 */
        {0,3679},        /* buffer_config port 1 */
        {0,3679},        /* buffer_config port 2 */
        {0,3679},        /* buffer_config port 3 */
        {0,3679},        /* buffer_config port 4 */
        {0,3679},        /* buffer_config port 5 */
        {0,3679},        /* buffer_config port 6 */
        {0,3679}         /* buffer_config port 7 */
    }
};

static const struct td4_obm_setting_s {
    int discard_limit;
    int lossless_discard;
    int port_xoff;
    int port_xon;
    int lossless_xoff;
    int lossless_xon;
    int lossy_low_pri;
    int lossy_discard;
    int sop_discard_mode;
} td4_obm_settings[3][9] = {
    /* LOSSY Settings */
    { /* Indexed by number of lanes used in the port */
        { /* 0 lanes - invalid */ 0 },
        { /* 1 lane */
            457,             /* discard_limit */
            3677,            /* lossless_discard */
            3677,            /* port_xoff */
            3677,            /* port_xon */
            3677,            /* lossless_xoff */
            3677,            /* lossless_xon */
            351,             /* lossy_low_pri */
            455,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            917,             /* discard_limit */
            3677,            /* lossless_discard */
            3677,            /* port_xoff */
            3677,            /* port_xon */
            3677,            /* lossless_xoff */
            3677,            /* lossless_xon */
            811,             /* lossy_low_pri */
            915,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid */ 0 },
        { /* 4 lanes */
            1837,            /* discard_limit */
            3677,            /* lossless_discard */
            3677,            /* port_xoff */
            3677,            /* port_xon */
            3677,            /* lossless_xoff */
            3677,            /* lossless_xon */
            1731,            /* lossy_low_pri */
            1835,            /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 5 lanes - Invalid */ 0 },
        { /* 6 lanes - Invalid */ 0 },
        { /* 7 lanes - Invalid */ 0 },
        { /* 8 lanes */
            3677,            /* discard_limit */
            3677,            /* lossless_discard */
            3677,            /* port_xoff */
            3677,            /* port_xon */
            3677,            /* lossless_xoff */
            3677,            /* lossless_xon */
            3571,            /* lossy_low_pri */
            3675,            /* lossy_discard */
            1                /* sop_discard_mode */
        }
    },
    /* LOSSLESS Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 lanes - invalid */ 0 },
        { /* 1 lane */
            457,             /* discard_limit */
            3677,            /* lossless_discard */
            153,             /* port_xoff */
            145,             /* port_xon */
            81,              /* lossless_xoff */
            73,              /* lossless_xon */
            73,              /* lossy_low_pri */
            177,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            917,             /* discard_limit */
            3677,            /* lossless_discard */
            437,             /* port_xoff */
            429,             /* port_xon */
            194,             /* lossless_xoff */
            186,             /* lossless_xon */
            73,              /* lossy_low_pri */
            177,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 lanes - invalid */ 0 },
        { /* 4 lanes */
            1837,            /* discard_limit */
            3677,            /* lossless_discard */
            962,             /* port_xoff */
            954,             /* port_xon */
            384,             /* lossless_xoff */
            376,             /* lossless_xon */
            73,              /* lossy_low_pri */
            177,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 5 lanes - Invalid */ 0 },
        { /* 6 lanes - Invalid */ 0 },
        { /* 7 lanes - Invalid */ 0 },
        { /* 8 lanes */
            3677,            /* discard_limit */
            3677,            /* lossless_discard */
            2066,            /* port_xoff */
            2058,            /* port_xon */
            768,             /* lossless_xoff */
            760,             /* lossless_xon */
            73,              /* lossy_low_pri */
            177,             /* lossy_discard */
            1                /* sop_discard_mode */
        }
    },
    /* LOSSY + LOSSLESS Settings*/
    { /* indexed by number of lanes used in the port */
        { /* 0 lanes - invalid */ 0 },
        { /* 1 lane */
            457,             /* discard_limit */
            3677,            /* lossless_discard */
            153,             /* port_xoff */
            145,             /* port_xon */
            81,              /* lossless_xoff */
            73,              /* lossless_xon */
            73,              /* lossy_low_pri */
            177,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 2 lanes */
            917,             /* discard_limit */
            3677,            /* lossless_discard */
            437,             /* port_xoff */
            429,             /* port_xon */
            194,             /* lossless_xoff */
            186,             /* lossless_xon */
            73,              /* lossy_low_pri */
            177,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 3 - invalid */ 0 },
        { /* 4 lanes */
            1837,            /* discard_limit */
            3677,            /* lossless_discard */
            962,             /* port_xoff */
            954,             /* port_xon */
            384,             /* lossless_xoff */
            376,             /* lossless_xon */
            73,              /* lossy_low_pri */
            177,             /* lossy_discard */
            1                /* sop_discard_mode */
        },
        { /* 5 lanes - Invalid */ 0 },
        { /* 6 lanes - Invalid */ 0 },
        { /* 7 lanes - Invalid */ 0 },
        { /* 8 lanes */
            3677,            /* discard_limit */
            3677,            /* lossless_discard */
            2066,            /* port_xoff */
            2058,            /* port_xon */
            768,             /* lossless_xoff */
            760,             /* lossless_xon */
            73,              /* lossy_low_pri */
            177,             /* lossy_discard */
            1                /* sop_discard_mode */
        }
    }
};

static const struct td4_e2e_settings_s {
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
} td4_e2e_settings_tbl[] = {
    {    -1, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20},       /* SAF */
    { 10000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 10GE */
    { 25000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 25GE */
    { 40000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 40GE */
    { 50000, {0, 0, 0}, 20, 8, 28, 30, 48, 8, 40, 20},       /* 50GE */
    {100000, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20},       /* 100GE */
    {200000, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20},       /* 200GE */
    {400000, {0, 0, 0}, 20, 8, 28, 30, 16, 0, 40, 20}        /* 400GE */
};

static const struct td4_peek_depth_settings_s {
    int speed;
    int peek_depth;
} td4_peek_depth_settings_tbl[] = {
    {    -1, 1},       /* Default */
    { 10000, 1},       /* 10GE */
    { 25000, 1},       /* 25GE */
    { 40000, 1},       /* 40GE */
    { 50000, 1},       /* 50GE */
    {100000, 2},       /* 100GE */
    {200000, 3},       /* 200GE */
    {400000, 4}        /* 400GE */
};

static const struct td4_max_spacing_settings_s {
    int all_speed;
    int special_slot;
} td4_max_spacing_settings_tbl[] = {
    /* all speed   special slot */
    {      60,     TD4_TDM_LENGTH}, /* 1500MHz */
    {      54,     TD4_TDM_LENGTH}, /* 1350MHz */
    {      38,     TD4_TDM_LENGTH}  /*  950MHz */
};

static const struct td4_urg_pick_spacing_settings_s {
    int spacing_50;
    int spacing_100;
    int spacing_200;
    int spacing_400;
} td4_urg_pick_spacing_settings_tbl[] = {
    /* 50G   100G   200G   400G */
    {   18,    9,     4,     2   }, /* 1500MHz */
    {   16,    8,     4,     2   }, /* 1350MHz */
    {   12,    6,     3,     2   }  /*  950MHz */
};

static const struct td4_min_spacing_settings_s {
    uint32_t any_cell;
    uint32_t inter_pkt;
    uint32_t mid_pkt;
} td4_min_spacing_settings_tbl[3][4] = {
    /*    50G         100G        200G        400G  */
    /* a   i   m    a   i   m    a  i  m    a  i  m */
    { {6, 14, 14}, {2, 14, 14}, {2, 2, 7}, {2, 2, 3} }, /* 1500MHz */
    { {6, 14, 14}, {2, 14, 14}, {2, 2, 7}, {2, 2, 3} }, /* 1350MHz */
    { {6, 14, 14}, {2,  8,  8}, {2, 2, 7}, {2, 2, 4} }  /*  950MHz */
};

/*******************************************************************************
 * EDB SETTINGS
 */
/* Number of valid cut thru classes: 1 SAF + 7 CT. */
#define TD4_EDB_NUM_CT_CLASSES 8

/* EP2MMU credits for Aux ports. */
#define TD4_EDB_EP2MMU_CRED_LBK        48
#define TD4_EDB_EP2MMU_CRED_CPU        13
#define TD4_EDB_EP2MMU_CRED_MGM         7

#define TD4_EDB_XMIT_START_CNT_MGMT    26

/*
 * Number of PFC Optimized Config cases of xmit start count.
 *   General case + PFC_OPT cases
 */
#define TD4_EDB_E2E_NUM_PFC_OPT_CFG  2


/* Transmit Start Count. */
typedef struct td4_edb_xmit_start_count_s {
    /*! SAF value. */
    uint32_t saf;

    /*! CT value. */
    uint32_t ct;
} td4_edb_xmit_start_count_t;

/* EDB E2E settings: ct_class, EP2MMU credit, xmit_cnt. */
typedef struct td4_edb_e2e_setting_s {
    /*! Speed in Mbps. */
    int speed;

    /*! CT class. */
    int ct_class;

    /*! EP2MMU credit. */
    int mmu_credit;
    int mmu_credit_pfc_opt;

    /*! Xmit start count. [0]->general case, [1-N]-> OPT cases. */
    td4_edb_xmit_start_count_t xmit_cnt[TD4_EDB_E2E_NUM_PFC_OPT_CFG];
} td4_edb_e2e_setting_t;

/* ASF Core Config Table: SAF & CT XMIT_CNT and EP2MMU credit  */
/*
 * 1) ep2mmu credit: pick "PFC Opt 1350MHz" and "General case".
 *    Note that "PFC Opt 1350MHz" covers both 1350MHz and 1500MHz.
 * 2) xmit_start_cnt SAF: pick "PFC Opt 1350MHz/1500MHz" and "General case"
 * 3) xmit_start_cnt CT : pick "PFC Opt 1350MHz/1500MHz" and "General case"
 */

static const  td4_edb_e2e_setting_t
td4_edb_e2e_tbl[TD4_EDB_NUM_CT_CLASSES] = {
   /* speed   ct     ep2mmu_credit        xmit_cnt {saf, ct}
   *          class  general opt1350    general   pfc_opt_1350/1500MHz
   */
    {     0,   0,     0,       0,      {{ 0,  0}, { 0,  0}} },   /*  SAF  */
    { 10000,   1,     9,       7,      {{ 6,  7}, { 4,  5}} },   /* 10GE  */
    { 25000,   2,    22,      17,      {{13, 18}, {10, 13}} },   /* 25GE  */
    { 40000,   3,    35,      27,      {{21, 28}, {15, 20}} },   /* 40GE  */
    { 50000,   4,    32,      32,      {{26, 36}, {19, 26}} },   /* 50GE  */
    {100000,   5,    65,      65,      {{30, 41}, {21, 29}} },   /* 100GE */
    {200000,   6,   119,     100,      {{35, 47}, {25, 38}} },   /* 200GE */
    {400000,   7,   228,     153,      {{29, 52}, {28, 51}} }    /* 400GE */
};

/* Max EB credit configurations for EB prt shced to MMU Main prt sched. */
/* Indexed by speed: 50G:0, 100G:1; 200G:2; 400G:3. */
static const int td4_tdm_mmu_eb_credit_config_vals[] = {
    47, 55, 71, 104
};


#endif /* BCM56880_A0_TM_E2E_SETTINGS_H */

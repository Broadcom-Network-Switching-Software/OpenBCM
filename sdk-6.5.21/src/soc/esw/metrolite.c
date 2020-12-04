/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        metrolite.c
 * Purpose:
 * Requires:
 */


#include <sal/core/boot.h>

#include <soc/katana2.h>
#include <soc/metrolite.h>
#include <soc/saber2.h>
#include <soc/trident.h>
#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/shmoo_and28.h>
#include <soc/phy/ddr34.h>
#include <soc/mspi.h>
#include <soc/phyreg.h>
#include <soc/iproc.h>
#include <shared/bsl.h>
#include <soc/scache.h>
#include <soc/bondoptions.h>

#ifdef BCM_METROLITE_SUPPORT

#ifdef BCM_WARM_BOOT_SUPPORT

/* Basic WB version. */
#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)

/* Default Version - Latest */
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/* Default Scache Partition - Latest */
#define BCM_FLEXIO_SCACHE_DEFAULT_PARTITION     0x1

static uint8  scache_begin_magic_number[]={0xAA,0xBB,0xCC,0xDD};
static uint8  scache_end_magic_number[]={0xDD,0xCC,0xBB,0xAA};
static bcm56270_tdm_info_t ml_scache_tdm_info={0};
static uint32 ml_scache_tdm[ML_TDM_MAX_SIZE]={0};
static uint32 ml_scache_speed[ML_MAX_PHYSICAL_PORTS]={0};
static soc_port_details_t ml_scache_port_details[ML_MAX_PHYSICAL_PORTS+1]={{0}};
static uint32 ml_port_speeds_scache[ML_MAX_PHYSICAL_PORTS] = {0};
int _soc_ml_flexio_scache_retrieve_part0(int unit);

#endif
#define _SOC_ML_DEFIP_MAX_TCAMS   4
#define _SOC_ML_DEFIP_TCAM_DEPTH  256

#define PORTGROUP_SINGLE_MODE   4
#define PORTGROUP_DUAL_MODE     2
#define PORTGROUP_QUAD_MODE     1

int ml_max_linkphy_subports_per_port;
int ml_max_linkphy_streams_per_port;
int ml_max_linkphy_streams_per_slice;

int block_ml_max_startaddr[ML_MAX_BLOCKS]={0};
int block_ml_max_endaddr[ML_MAX_BLOCKS]={0};

/* 56270_1: bcm5627x_config = 1 */
/* 4x2.5G + 4x10G + 4x11G + 2.5G LPBK   */
const  static ml_speed_t ml_speed_56270_1 = {
       2500, 2500, 2500, 2500,          /* 1 - 4  */
       10000, 10000, 10000, 10000,      /* 5 - 8  */
       11000, 11000, 11000, 11000,      /* 9 - 12 */
};
const static soc_port_details_t ml_port_details_56270_1[]={
    {1, 4, 1,GE_PORT,2500},
    {5, 8, 1,XE_PORT,10000},
    {9,12, 1,HG_PORT,11000},
    {0,0,0,0} /* End */
};

/* 56271_1: bcm5627x_config = 1 */
/* 4x1G/2.5G + 4xXFI + 4x1G/2.5G + 2.5G LPBK */
const  static ml_speed_t ml_speed_56271_1 = {
       2500, 2500, 2500, 2500,          /* 1 - 4  */
       10000, 10000, 10000, 10000,      /* 5 - 8  */
       2500, 2500, 2500, 2500,          /* 9 - 12 */
};
const static soc_port_details_t ml_port_details_56271_1[]={
    {1, 4, 1,GE_PORT,2500},
    {5, 8, 1,XE_PORT,10000},
    {9,12, 1,GE_PORT,2500},
    {0,0,0,0} /* End */
};

/* 56271_2: bcm5627x_config = 2 */
/* 4x1G/2.5G G.INT+ 2x1G/2.5G/10G */
const  static ml_speed_t ml_speed_56271_2 = {
       2500, 2500, 2500, 2500,          /* 1 - 4  */
       10000, 0, 0, 0,                  /* 5 - 8  */
       10000, 0, 10000, 0,              /* 9 - 12 */
};
const static soc_port_details_t ml_port_details_56271_2[]={
    {1, 4, 1,GE_PORT,2500},
    {5,5,1,XE_PORT,10000},
    {9,9,1,XE_PORT,10000},
    {11,11,1,XE_PORT,10000},
    {5, 8, 1,0,0},
    {10, 10, 1,0,0},
    {0,0,0,0} /* End */
};

/* 56271_3: bcm5627x_config = 3 */
/* 4x1G/2.5G G.INT + 4x1G/2.5G/10G/11G */
const  static ml_speed_t ml_speed_56271_3 = {
       2500, 2500, 2500, 2500,          /* 1 - 4  */
       10000, 0, 0, 0,                  /* 5 - 8  */
       11000, 11000, 11000, 11000,      /* 9 - 12 */
};
const static soc_port_details_t ml_port_details_56271_3[]={
    {1, 4, 1,GE_PORT,2500},
    {5,5,1,XE_PORT,10000},
    {9,12,1,HG_PORT,11000},
    {6, 8, 1,0,0},
    {0,0,0,0} /* End */
};

/* 56272_1: bcm5627x_config = 1 */
/* 8xGE + 1G LPBK */
const  static ml_speed_t ml_speed_56272_1 = {
       1000, 1000, 1000, 1000,     /* 1 - 4  */
       2500, 0, 0, 0,              /* 5 - 8  */
       1000, 1000, 1000, 1000,     /* 9 - 12 */
};
const static soc_port_details_t ml_port_details_56272_1[]={
    {1, 4, 1,GE_PORT,1000},
    {5, 5, 1,GE_PORT,2500},
    {9,12,1,GE_PORT,1000},
    {0,0,0,0} /* End */
};

/* 56272_2: bcm5627x_config = 2 */
/* 4x1G G.INT + 2x1G + 1G LPBK */
const  static ml_speed_t ml_speed_56272_2 = {
       1000, 1000, 1000, 1000,     /* 1 - 4  */
       2500, 0, 0, 0,              /* 5 - 8  */
       1000, 0, 1000, 0,           /* 9 - 12 */
};

const static soc_port_details_t ml_port_details_56272_2[]={
    {1, 4, 1,GE_PORT,1000},
    {5, 5, 1,GE_PORT,2500},
    {9, 9, 1,GE_PORT,1000},
    {11,11,1,GE_PORT,1000},
    {6, 8, 1,0,0},
    {10, 10, 1,0,0},
    {0,0,0,0} /* End */
};

soc_port_details_t  *ml_selected_port_details;
soc_port_details_t  *ml_selected_port_details_orig;
const static soc_port_details_t ml_port_details_56270_0[]={{0,0,0,0}};
const static soc_port_details_t  *ml_port_details[]={
               ml_port_details_56270_0,   /* Dummy */
	           ml_port_details_56270_1,
	           ml_port_details_56271_1,
	           ml_port_details_56271_2,
	           ml_port_details_56271_3,
	           ml_port_details_56272_1,
	           ml_port_details_56272_2,
	           ml_port_details_56270_1,
	           ml_port_details_56271_1
#ifdef BCM_WARM_BOOT_SUPPORT
               ,ml_scache_port_details           /* Dummy port details
                                                    reserved for WB */
#endif
};

ml_speed_t *bcm56270_speed[SOC_MAX_NUM_DEVICES] = {NULL};
const static ml_speed_t ml_speed_56270_0 = {0};
const static uint32 *bcm56270_speed_s[]={
               ml_speed_56270_0,          /* Dummy */
               ml_speed_56270_1,
               ml_speed_56271_1,
               ml_speed_56271_2,
               ml_speed_56271_3,
               ml_speed_56272_1,
               ml_speed_56272_2,
               ml_speed_56270_1,
               ml_speed_56271_1
#ifdef BCM_WARM_BOOT_SUPPORT
               ,ml_scache_speed /* Dummy speed reserved for WB*/
#endif
};

ml_port_to_block_subports_t
        *ml_port_to_block_subports[SOC_MAX_NUM_DEVICES] = {NULL};
ml_port_to_block_subports_t ml_port_to_block_subports_s = {
                                                 0,1,2,3, /* 1-4   mxq0 */
                                                 0,1,2,3, /* 5-8   xl0  */
                                                 0,1,2,3  /* 9-12  xl1  */
};

static ml_port_to_block_t *ml_port_to_block[SOC_MAX_NUM_DEVICES] = {NULL};
static ml_port_to_block_t ml_port_to_block_s = {
                                        0,0,0,0, /* 1-4   mxq0 */
                                        1,1,1,1, /* 5-8   xl0  */
                                        2,2,2,2  /* 9-12  xl1  */
};

ml_block_ports_t *ml_block_ports[SOC_MAX_NUM_DEVICES] = {NULL};
ml_block_ports_t ml_block_ports_s = {
                                     {1,2,3,4},
                                     {5,6,7,8},
                                     {9,10,11,12},
};

ml_port_speeds_t *ml_port_speeds[SOC_MAX_NUM_DEVICES] = {NULL};
const static  ml_port_speeds_t ml_port_speeds_56270_0 = {{0,0,0,0}};
const static  ml_port_speeds_t ml_port_speeds_56270_1 = {
              {10000,0,0,2500},{10000,10000,0,10000},{11000,11000,0,11000}};
const static  ml_port_speeds_t ml_port_speeds_56271_1 = {
              {10000,0,0,2500},{10000,10000,0,10000},{10000,0,0,2500}};
const static  ml_port_speeds_t ml_port_speeds_56271_2 = {
              {10000,0,0,2500},{10000,0,0,0},{10000,10000,0,0}};
const static  ml_port_speeds_t ml_port_speeds_56271_3 = {
              {10000,0,0,2500},{10000,0,0,0},{11000,11000,0,11000}};
const static  ml_port_speeds_t ml_port_speeds_56272_1 = {
              {1000,0,0,1000},{2500,0,0,0},{1000,1000,0,1000}};
const static  ml_port_speeds_t ml_port_speeds_56272_2 = {
              {1000,0,0,1000},{2500,0,0,0},{1000,1000,0,0}};

ml_port_speeds_t *ml_port_speeds_s[] = {
               (ml_port_speeds_t*)ml_port_speeds_56270_0,   /* Dummy */
               (ml_port_speeds_t*)ml_port_speeds_56270_1,
               (ml_port_speeds_t*)ml_port_speeds_56271_1,
               (ml_port_speeds_t*)ml_port_speeds_56271_2,
               (ml_port_speeds_t*)ml_port_speeds_56271_3,
               (ml_port_speeds_t*)ml_port_speeds_56272_1,
               (ml_port_speeds_t*)ml_port_speeds_56272_2,
               (ml_port_speeds_t*)ml_port_speeds_56270_1,
               (ml_port_speeds_t*)ml_port_speeds_56271_1
#ifdef BCM_WARM_BOOT_SUPPORT
               ,(ml_port_speeds_t*)ml_port_speeds_scache
#endif
};

ml_tdm_pos_info_t ml_tdm_pos_info[ML_MAX_BLOCKS]={{0}};

typedef struct tdm_port_slots_info_s  {
        int32  position;
        struct tdm_port_slots_info_s *prev;
        struct tdm_port_slots_info_s *next;
}tdm_port_slots_info_t;

tdm_port_slots_info_t ml_tdm_port_slots_info[ML_MAX_LOGICAL_PORTS];

uint32 ml_current_tdm[ML_TDM_MAX_SIZE]={0};
uint32 ml_current_tdm_size=0;
static uint8  old_tdm_no=0;

/* TDM_A : 0:180MHz: 209 (19 * 11) Cycles Used */
#define ML_TDM_A_SIZE   204
#define ML_TDM_A_FREQ   180 
#define ML_TDM_A_ROW    19
#define ML_TDM_A_COL    11 
const static uint32 ml_tdm_56270_A_ref[ML_TDM_A_SIZE] = {
       /*Col:1*/ /*Col:2*/ /*Col:3*/ /*Col:4*/ /*Col:5*/ /*Col:6*/ /*Col:7*/ /*Col:8*/ /*Col:9*/ /*Col:10*/ /*Col:11*/
/*1*/   5,		  6,		7,		  8,	    1,		  9,	    10,       11,       12,       5,         2,    
/*2*/	6, 		  7,		8,		  9,	    3,		  10,		11,       12,	    5,	      6,         4,
/*3*/	7,		  8,		9,		  10,	    ML_IDLE,  ML_IDLE,	11,       12,       5,        6,         7,
/*4*/	8,		  9,		10,		  11,	    1,		  12,		5,        6,        7,        8,         2,
/*5*/	9,		  10,		11,		  12,	    3,  	  5,		6,        7,        8,        9,         4,
/*6*/	10,		  11,		12,		  5,	    ML_LPBK,  6,		7,	      8,        9,        10,        ML_CMIC,
/*7*/	11,		  12,		5,		  6,	    ML_LPBK,  7,		8,        9,        10,       11,        ML_CMIC,
/*8*/	12,		  5,		6,		  7,	    1,		  8,		9,        10,       11,       12,        2,
/*9*/	5,		  6,		7,		  8,	    3,		  9,		10,       11,       12,       5,         4,
/*10*/	6,		  7,		8,		  9,	    ML_IDLE,  ML_IDLE,	10,       11,       12,       5,         6,
/*11*/	7,		  8,		9,		  10,	    1,		  11,		12,       5,        6,        7,         2,
/*12*/	8,		  9,		10,		  11,	    3,		  12,		5,        6,        7,        8,         4,
/*13*/	9,		  10,		11,		  12,	    ML_LPBK,  5,		6,        7,        8,        9,         ML_CMIC,
/*14*/	10,		  11,		12,		  5,	    ML_LPBK,  6,		7,        8,        9,        10,        ML_CMIC,
/*15*/	11,		  12,		5,		  6,	    1,		  7,		8,        9,        10,       11,        2,
/*16*/	12,		  5,		6,		  7,	    3,		  8,		9, 	      10,       11,       12,        4,
/*17*/	5,		  6,		7,		  8,	    ML_IDLE,  ML_IDLE,	9,	      10,       11,       12,        5,
/*18*/	6,		  7,		8,		  9,	    ML_LPBK,  10,		11,	      12,       5,        6,         ML_IDLE,
/*19*/	7,		  8,		9,		  10,	    11,		  12		/* ------------------ EMPTY ----------------------*/
};

/* TDM_C : 0:125MHz: 147(21 * 7) Cycles Used */
#define ML_TDM_C_SIZE   140
#define ML_TDM_C_FREQ   125 
#define ML_TDM_C_ROW    21
#define ML_TDM_C_COL    7 
const static uint32 ml_tdm_56271_C_ref[ML_TDM_C_SIZE] = {
       /*Col:1*/ /*Col:2*/ /*Col:3*/ /*Col:4*/ /*Col:5*/ /*Col:6*/ /*Col:7*/   
/*1*/	5,		  6,		7,		  8,	    1,		  9,	  	ML_CMIC, 
/*2*/	5, 		  6,		7,		  8,	    2,		  10,		ML_LPBK,
/*3*/	5,		  6,		7,		  8,	    3,     	  11,		ML_IDLE,
/*4*/	5,		  6,		7,		  8,	    4,		  12,		/* EMPTY */
/*5*/	5,		  6,		7,		  8,	    1,        9,		/* EMPTY */
/*6*/	5,		  6,		7,		  8,	    2,        10,		ML_LPBK,
/*7*/	5,		  6,		7,		  8,	    3,		  11,		ML_CMIC,
/*8*/	5,		  6,		7,		  8,	    4,		  12,		ML_IDLE,
/*9*/	5,		  6,		7,		  8,	    1,		  9,		/* EMPTY */
/*10*/	5,		  6,		7,		  8,	    2,		  10,		ML_LPBK,
/*11*/	5,		  6,		7,		  8,	    3,		  11,		ML_IDLE,
/*12*/	5,		  6,		7,		  8,	    4,		  12,		/* EMPTY */
/*13*/	5,		  6,		7,		  8,	    1,		  9,		ML_CMIC,
/*14*/	5,		  6,		7,		  8,	    2,		  10,		ML_LPBK,
/*15*/	5,		  6,		7,		  8,	    3,		  11,		ML_IDLE,
/*16*/	5,		  6,		7,		  8,	    4,		  12,		/* EMPTY */	
/*17*/	5,		  6,		7,		  8,	    1,		  9,		/* EMPTY */
/*18*/	5,		  6,		7,		  8,	    2,		  10,		ML_LPBK,
/*19*/	5,		  6,		7,		  8,	    3,		  11,		ML_CMIC,
/*20*/	5,		  6,		7,		  8,	    4,		  12,		ML_IDLE,
/*21*/	5,		  6,		7,		  8,	    ML_IDLE,  ML_IDLE	/* EMPTY */
};


/* TDM_B : 0:30MHz: 16 (4 * 4) Cycles Used */
#define ML_TDM_B_SIZE    16
#define ML_TDM_B_FREQ    30 
#define ML_TDM_B_ROW     4
#define ML_TDM_B_COL     4 
const static uint32 ml_tdm_56272_B_ref[ML_TDM_B_SIZE] = {
      /*Col:1*/ /*Col:2*/ /*Col:3*/ /*Col:4*/
/*1*/  ML_IDLE,	 1,		   9,		 5,
/*2*/  ML_CMIC,	 2,		   10,		 5,
/*3*/  ML_LPBK,	 3,		   11,		 5,
/*4*/  ML_CMIC,	 4,		   12,		 ML_IDLE
};

/* TDM related functions and structures */
static bcm56270_tdm_info_t bcm56270_tdm_info[]={
            {0, 0, 0, 0},                     /* Dummy */
            /* 56270_1 Cfg=1:IntCfg=1 */
            {ML_TDM_A_FREQ, ML_TDM_A_SIZE, ML_TDM_A_ROW, ML_TDM_A_COL},

            /* 56271_1 Cfg=1:IntCfg=2 */
            {ML_TDM_C_FREQ, ML_TDM_C_SIZE, ML_TDM_C_ROW, ML_TDM_C_COL},

            /* 56271_2 Cfg=2:IntCfg=3 */
            {ML_TDM_C_FREQ, ML_TDM_C_SIZE, ML_TDM_C_ROW, ML_TDM_C_COL},

            /* 56271_3 Cfg=3:IntCfg=4 */
            {ML_TDM_C_FREQ, ML_TDM_C_SIZE, ML_TDM_C_ROW, ML_TDM_C_COL},


            /* 56272_1 Cfg=1:IntCfg=5 */
            {ML_TDM_B_FREQ, ML_TDM_B_SIZE, ML_TDM_B_ROW, ML_TDM_B_COL},

            /* 56272_2 Cfg=1:IntCfg=6 */
            {ML_TDM_B_FREQ, ML_TDM_B_SIZE, ML_TDM_B_ROW, ML_TDM_B_COL},


            /* 53460_1 Cfg=1:IntCfg=1 */
            {ML_TDM_A_FREQ, ML_TDM_A_SIZE, ML_TDM_A_ROW, ML_TDM_A_COL},

            /* 53461_1 Cfg=1:IntCfg=2 */
            {ML_TDM_C_FREQ, ML_TDM_C_SIZE, ML_TDM_C_ROW, ML_TDM_C_COL}
#ifdef BCM_WARM_BOOT_SUPPORT
            ,{0, 0, 0, 0},
#endif
};

static uint32 ml_tdm_A_56270_0[ML_TDM_A_SIZE]={0}; /* Dummy */
static uint32 ml_tdm_A_56270_1[ML_TDM_A_SIZE]={0}; /* Filled up Runtime */
static uint32 ml_tdm_C_56271_1[ML_TDM_C_SIZE]={0}; /* Filled up Runtime */
static uint32 ml_tdm_C_56271_2[ML_TDM_C_SIZE]={0}; /* Filled up Runtime */
static uint32 ml_tdm_C_56271_3[ML_TDM_C_SIZE]={0}; /* Filled up Runtime */
static uint32 ml_tdm_B_56272_1[ML_TDM_B_SIZE]={0}; /* Filled up Runtime */
static uint32 ml_tdm_B_56272_2[ML_TDM_B_SIZE]={0}; /* Filled up Runtime */

static uint32 *bcm56270_tdm[]={
               ml_tdm_A_56270_0,   /* Dummy to match cfg_num and index */
               ml_tdm_A_56270_1,   /* 56270 Cfg=1:IntCfg=1 */
               ml_tdm_C_56271_1,   /* 56271 Cfg=1:IntCfg=2 */
               ml_tdm_C_56271_2,   /* 56271 Cfg=2:IntCfg=3 */
               ml_tdm_C_56271_3,   /* 56271 Cfg=3:IntCfg=4 */
               ml_tdm_B_56272_1,   /* 56272 Cfg=1:IntCfg=5 */
               ml_tdm_B_56272_2,   /* 56272 Cfg=2:IntCfg=6 */
               ml_tdm_A_56270_1,   /* 53460 Cfg=1:IntCfg=1 */
               ml_tdm_C_56271_1    /* 53461 Cfg=1:IntCfg=1 */
#ifdef BCM_WARM_BOOT_SUPPORT
               ,ml_scache_tdm      /*Dummy TDM reserved for WB */
#endif
};

extern int
bcm_kt2_modid_set(int unit, int *my_modid_list, int *my_modid_valid_list ,
        int *base_port_ptr_list);
int _soc_metrolite_get_cfg_num(int unit, int skip_warmboot, int *new_cfg_num);
extern int
bcm_kt2_modid_set_all(int unit, int *my_modid_list, int *my_modid_valid_list,
        int *base_port_ptr_list);

int _soc_metrolite_get_cfg_num(int unit, int skip_warmboot, int *new_cfg_num) 
{
    int cfg_num=-1;
    int max_cfg_num=0;
    int def_cfg_num=0;
    int offset_cfg_num=0;
    uint16 dev_id=0;
    uint8  rev_id=0;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) && (!skip_warmboot)) {
        cfg_num = COUNTOF(bcm56270_tdm_info) - 1;
        if (bcm56270_tdm_info[cfg_num].tdm_size != 0) {
            *new_cfg_num = cfg_num;
            return SOC_E_NONE;       
        }
        /* FAll thru */
    }
#endif
    cfg_num = soc_property_get(unit, spn_BCM5627X_CONFIG,0xFF); 

    soc_cm_get_id(unit, &dev_id, &rev_id);

    switch(dev_id) {
    case BCM56270_DEVICE_ID:
         def_cfg_num = BCM56270_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56270_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56270_DEVICE_ID_MAX_CFG;
         break;
    case BCM56271_DEVICE_ID:
         def_cfg_num = BCM56271_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56271_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56271_DEVICE_ID_MAX_CFG;
         break;
    case BCM56272_DEVICE_ID:
         def_cfg_num = BCM56272_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56272_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56272_DEVICE_ID_MAX_CFG;
         break;
    case BCM53460_DEVICE_ID:
         def_cfg_num = BCM56270_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56270_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56270_DEVICE_ID_MAX_CFG;
         break;
    case BCM53461_DEVICE_ID:
         def_cfg_num = BCM56271_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56271_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56271_DEVICE_ID_DEFAULT_CFG;
         break;

    default : 
         /* Not supported yet */
         return SOC_E_UNAVAIL; 
    }

    if (cfg_num == 0xFF) {
        *new_cfg_num = def_cfg_num;
    } else {
        if (cfg_num == 0) {
            LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Config=0 is not allowed\n")));
            return SOC_E_PARAM;
        }   
        *new_cfg_num = cfg_num + offset_cfg_num;
    }           
    if (*new_cfg_num > max_cfg_num) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Cfg_num:%d Internal cfg_num:%d \n"),cfg_num,*new_cfg_num));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Wrong %s ==> cfg_num:%d DEFAULT:%d START:%d MAX:%d \n"),
                   spn_BCM5627X_CONFIG, cfg_num,
                   def_cfg_num-offset_cfg_num,1,max_cfg_num-offset_cfg_num));
        *new_cfg_num = def_cfg_num;
        return SOC_E_NONE; 
    }              
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
               "Info Cfg_num:%d Internal cfg_num:%d \n"),cfg_num,*new_cfg_num));

    return SOC_E_NONE;
}

#if 0
soc_error_t ml_tdm_verify(int unit,
                           uint32 *tdm,
                           uint32 total_tdm_slots,
                           uint32 tdm_freq,    /* In MHHz */
                           uint32 *port_speed,
                           uint32 lpbk_port_speed,
                           uint32 *offender_port) 
{
    /* Following rules need to be met */
    /* Rule1 : Each Loopback port need 2.5G bandwidth */
    /* Rule2 : Each CMIC port need 2G bandwidth       */
    /* Rule3 : Loopback (LPBK) ports require min 3 cycle spacing among LPBK or 
               CMIC ports */ 
    /* Rule4 : Each subport in MXQPORT operates with 4 cycle TDM */
    /* Rule5 : Each 10G-XAUI ports should be able to operate in 4x2.5G with 4 
               slots allocated to each subport from the same MXQPORT */
    soc_error_t           rv=SOC_E_NONE;
    uint32                index=0;
    uint32                expected_tdm_cycles_min=0;
    uint32                expected_tdm_cycles_max=0;
    uint32                tdm_freq_index=0;
    uint32                speed_index=0;
    uint32                port=0;
    uint32                speed=0;
    uint32                worse_tdm_slot_spacing=0;
    uint32                prev_tdm_slot_spacing=0;
    uint32                next_tdm_slot_spacing=0;
#if 0
    uint32                tdm_slot_spacing=0;
#endif
    uint32                min_tdm_cycles=0; /* To Meet BW */
    uint32                count=0; 
    uint32                skip_count=0; 
    uint32                mxqblock=0;
    uint32                outer_port=0;
    uint32                inner_port=0;
    uint32                spacing=0;
    uint32                loop=0;
    tdm_port_slots_info_t *head=NULL;
    tdm_port_slots_info_t *head1=NULL;
    tdm_port_slots_info_t *current=NULL;
    tdm_port_slots_info_t *current1=NULL;
    tdm_port_slots_info_t *prev = NULL;
    tdm_port_slots_info_t *temp=NULL;
    uint32                idle_flag=0;
    uint32                idle_rule=0;
    char                  base_tdm_str[80];

   /* Possible TDM frequencies 
      80MHz(16Cycles) ,120MHz(20Cycles),155MHz(84Cycles),
      185MHz(80Cycles),185MHz(90Cycles) 
    */
    switch(tdm_freq) {
    case 130:
        if (total_tdm_slots == 284) {
            /*TDM_A : 0:130MHz: 284 (41 * 7) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_A : 0:130MHz: 284 (41 * 7) Cycles Used",
                        79);
            tdm_freq_index=0;
            expected_tdm_cycles_min=284;
            expected_tdm_cycles_max=284;
        } else if (total_tdm_slots == 141) {
            /*TDM_C,C1,C2: 7:130MHz: 141 (21 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_AA,TDM_C,C1,C2: 7:130MHz:141 (21 * 6) Cycles Used",
                        79);
            tdm_freq_index=7;
            expected_tdm_cycles_min=141;
            expected_tdm_cycles_max=141;
        } else if (total_tdm_slots == 168) {
            /*TDM_D: 8:130MHz: 168 (28 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_D: 8:130MHz: 168 (28 * 6) Cycles Used",
                        79);
            tdm_freq_index=8;
            expected_tdm_cycles_min=168;
            expected_tdm_cycles_max=168;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                      "Unsupported tdm slots:%d\n"),
                      total_tdm_slots));
            return SOC_E_FAIL;
        }
        break;
    case 24:
        /*TDM_A1: 2:24MHz: 13 (4 * 4) Cycles Used */
        sal_strncpy(base_tdm_str, 
                    "TDM_A1: 2:24MHz: 13 (4 * 4) Cycles Used ",
                    79);
        tdm_freq_index=2;
        expected_tdm_cycles_min=13;
        expected_tdm_cycles_max=13;
        break;
    case 37:
        /*TDM_A1A: 3:37MHz: 84 (21 * 4) Cycles Used */
        sal_strncpy(base_tdm_str, 
                    "TDM_A1A: 3:37MHz: 84 (21 * 4) Cycles Used ",
                    79);
        tdm_freq_index=3;
        expected_tdm_cycles_min=84;
        expected_tdm_cycles_max=84;
        break;
    case 118:
        if (total_tdm_slots == 133) {
            /*TDM_B: 4:118MHz: 133 (23 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_B: 4:118MHz: 133 (23 * 6) Cycles Used ",
                        79);
            tdm_freq_index=4;
            expected_tdm_cycles_min=133;
            expected_tdm_cycles_max=133;
        } else if (total_tdm_slots == 120) {
            /*TDM_B1: 5:118MHz: 120 (20 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_B1: 5:118MHz: 120 (20 * 6) Cycles Used ",
                        79);
            tdm_freq_index=5;
            expected_tdm_cycles_min=120;
            expected_tdm_cycles_max=120;
        } else if (total_tdm_slots == 121) {
            /*TDM_B1A: 6:118MHz: 121 (21 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_B1A: 6:118MHz: 121 (21 * 6) Cycles Used ",
                        79);
            tdm_freq_index=6;
            expected_tdm_cycles_min=121;
            expected_tdm_cycles_max=121;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                      "Unsupported tdm slots:%d\n"),
                      total_tdm_slots));
            return SOC_E_FAIL;
        }
        break;
    case 75:
        if (total_tdm_slots == 81) {
            /*TDM_E,E1,E3 9:75MHz: 81 (21 * 4) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_E,E1,E3 9:75MHz: 81 (21 * 4) Cycles Used ",
                        79);
            tdm_freq_index=9;
            expected_tdm_cycles_min=81;
            expected_tdm_cycles_max=81;
        } else if (total_tdm_slots == 16) {
            /*TDM_EA 10:75MHz: 16 (4 * 4) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_EA 10:75MHz: 16 (4 * 4) Cycles Used ",
                        79);
            tdm_freq_index=10;
            expected_tdm_cycles_min=16;
            expected_tdm_cycles_max=16;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                      "Unsupported tdm slots:%d\n"),
                      total_tdm_slots));
            return SOC_E_FAIL;
        }
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Unsupported tdm frequency:%d\n"),
                   tdm_freq));
        return SOC_E_FAIL;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
             "\nINFO: BaseTdmStr:%s :Index %d:MinCycles%d:MaxCycle%d\n"),
             base_tdm_str, 
             tdm_freq_index, expected_tdm_cycles_min, expected_tdm_cycles_max));
   if (!((total_tdm_slots >= expected_tdm_cycles_min) && 
         (total_tdm_slots <= expected_tdm_cycles_max))) {
          LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Unsupported tdm cycles:%d (min:%d max:%d\n"),
                     total_tdm_slots,
                     expected_tdm_cycles_min,expected_tdm_cycles_max));
          return SOC_E_FAIL; 
   }

    idle_flag=0;
    idle_rule = 0;
    /* Initialize list */
    for (port=0;port < ML_MAX_LOGICAL_PORTS ; port++) {
         ml_tdm_port_slots_info[port].prev=&ml_tdm_port_slots_info[port];
         ml_tdm_port_slots_info[port].next=&ml_tdm_port_slots_info[port];
         ml_tdm_port_slots_info[port].position = -1;
    }
    for (index=0; index < total_tdm_slots; index++) {
         if ((tdm[index] == ML_IDLE1) ||
             (tdm[index] == ML_IDLE)) {
             if (tdm[index] == ML_IDLE) {
                 if (idle_flag == 1) {
                     idle_rule = 1;
                 }
                 idle_flag=1;
             }
             skip_count++;
             continue;
         }
         if (((index - 1) != 0) && (index != total_tdm_slots - 1) && 
             (idle_flag == 1) && (idle_rule == 0)) {
              LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                          "### Internal Error: TDM Verification failed \n"
                          "### Two consecutive idle not found at position:%d\n"
                          "### TCAM Atomicity might get affected.\n"),
                          index - 1));
             *offender_port = ML_IDLE;
             return SOC_E_CONFIG;
         }
         idle_flag=0;
         port = tdm[index];
         if (ml_tdm_port_slots_info[port].position == -1) {
             /* First Entry */
             ml_tdm_port_slots_info[port].position = index;
             skip_count++;
             continue;
         }
         head = current = &ml_tdm_port_slots_info[port];
         while(current->next !=head) {
               current = current->next;
         } 
         current->next=sal_alloc(sizeof(tdm_port_slots_info_t),"TDM Pointer");
         count++;
         LOG_VERBOSE(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Count=%d SkipCount:%d  Port:%d  Addr:%p\n"),
                      count,skip_count,port,(void *)current->next));

         current->next->position=index;
         current->next->prev=current;
         current->next->next=head;
         head->prev=current->next;
   }
   if (idle_rule == 0) {
       /* Special case */
       if (!((tdm[0] == ML_IDLE) &&
             (tdm[total_tdm_slots-1] == ML_IDLE))) {
              LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                       "### Internal Error: TDM Verification failed \n"
                       "### Two consecutive idle not found at Edges \n"
                       "### TCAM Atomicity might get affected.\n")));
           *offender_port = ML_IDLE1;
           return SOC_E_CONFIG;
       }
   }
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Final Count=%d SkipCount:%d\n"),count,skip_count));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Verifying rule  \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Rule3: Loopback (LPBK) ports require min 3 cycle spacing "
                           "among LPBK or CMIC ports \n")));
   if (ml_tdm_port_slots_info[ML_LPBK].position == -1) {
       LOG_CLI((BSL_META_U(unit,
                           "Loopback port  missing in configuration \n")));
       return SOC_E_FAIL;
   }
   if (ml_tdm_port_slots_info[ML_CMIC].position == -1) {
       LOG_CLI((BSL_META_U(unit,
                           "CMIC port  missing in configuration \n")));
       return SOC_E_FAIL;
   }
   head = current = &ml_tdm_port_slots_info[ML_LPBK];
   do {
      prev = current->prev;
      if ( current->position != prev->position ) {
           prev_tdm_slot_spacing = 
                          (current->position - prev->position) > 0 ?
                          (current->position - prev->position)-1:
                          (total_tdm_slots - prev->position + 
                           current->position) - 1;
           next_tdm_slot_spacing = 
                          (current->next->position - current->position) >0 ?
                          (current->next->position - current->position)-1:
                          (total_tdm_slots - current->position +
                           current->next->position) - 1;
      if (prev_tdm_slot_spacing < 3) {
          LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Loopback Ports need min 3 spacing but(prev)found"
                                "(current:%d , prev:%d)=%d\n"),
                     current->position,prev->position,prev_tdm_slot_spacing));
          rv = SOC_E_FAIL;
          continue;
      } 
      if (next_tdm_slot_spacing < 3) {
          LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Loopback Ports need min 3 spacing but(next)found"
                                "(current:%d , prev:%d)=%d\n"),
                     current->position,prev->position,next_tdm_slot_spacing));
          rv = SOC_E_CONFIG;
          *offender_port = ML_LPBK;
          continue;
      } 
      } else {
          prev_tdm_slot_spacing = next_tdm_slot_spacing = 0;
      }
#if 0
      head1 = current1 = &ml_tdm_port_slots_info[ML_CMIC];
      do {
         tdm_slot_spacing = (current->position - current1->position) > 0 ?
                          (current->position - current1->position)-1:
                          (total_tdm_slots - current1->position + 
                           current->position) - 1  ;
         if (tdm_slot_spacing < 3) {
             LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                   "Loopback and CMIC ports need min 3 spacing but "
                                   "found:%u" "(Loopack current:%d , CMIC Current:%d)\n"),
                        tdm_slot_spacing,
                        current->position,current1->position));
             rv = SOC_E_CONFIG;
             *offender_port=ML_LPBK;
             continue;
         }
         current1 = current1->next;
      }while ((current1 !=head1) && (rv == SOC_E_NONE));
#endif
      current = current->next;
   } while ((current !=head) && (rv == SOC_E_NONE));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Verifying rules \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Rule1 : Each Loopback port need 2.5G bandwidth \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Rule2 : Each CMIC port need 2G bandwidth       \n")));
   for (index=0;index < ML_MAX_LOGICAL_PORTS && rv == SOC_E_NONE ;index++ ) {
        port = index ;
        if (ml_tdm_port_slots_info[port].position == -1) {
            continue;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                " %d ==>\n\n"),port));
        switch(port) {
        case ML_CMIC:
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "CMIM \n")));
            speed =2000; 
            break;
        case ML_LPBK:
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "LOOPBAKC \n")));
            speed =lpbk_port_speed; 
            break;
        default:
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    " Port:%d Speed %d ==>\n\n"),
                         port,port_speed[port-1]));
            speed = port_speed[port-1];
            break;
        }
        if ((speed == 0) && (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port))) {
            /* Eagle ports are not removed from TDM calendar 
             * Eventhough not active after lane op. Ignore those
             */
             continue;
        }

        switch(speed) {
        case 1000:
            speed_index=0;
            break;
        case 2000:
            speed_index=1;
            break;
        case 2500:
            speed_index=2;
            break;
        case 10000:
        case 10500: /* Might need ut fir HG11 */
        case 11000: /* Might need ut fir HG11 */
            speed_index=3;
            break;
        case 12500: /* Special case for LPBK port */
        case 13000:
            speed_index=4;
            break;
        case 20000:
            speed_index=5;
            break;
        case 21000:
            speed_index=6;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Unsupported speed:%d \n"),speed));
            return SOC_E_FAIL;
        }
        worse_tdm_slot_spacing=ml_tdm_cycles_info[tdm_freq_index][speed_index].
                               worse_tdm_slot_spacing;
        min_tdm_cycles=ml_tdm_cycles_info[tdm_freq_index][speed_index].
                       min_tdm_cycles;

        count=0;
        head = current = &ml_tdm_port_slots_info[port];
        do {
           prev = current->prev;
           if (current->position != prev->position ) {
               prev_tdm_slot_spacing = 
                          (current->position - prev->position)> 0 ?
                          (current->position - prev->position)-1:
                          (total_tdm_slots - prev->position + 
                           current->position) - 1  ;
               next_tdm_slot_spacing = 
                          (current->next->position - current->position) > 0 ?
                          (current->next->position - current->position)-1:
                          (total_tdm_slots - current->position + 
                           current->next->position) - 1;
           } else {
               prev_tdm_slot_spacing = next_tdm_slot_spacing = 0;
           }
           LOG_VERBOSE(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                   "Pos:%d Spacing (Prev %d - %d =  )%d"
                                   "(Next %d - %d =  )%d\n"), current->position,
                        current->position , prev->position ,
                        prev_tdm_slot_spacing, current->next->position , 
                        current->position , next_tdm_slot_spacing));
           if ((prev_tdm_slot_spacing > worse_tdm_slot_spacing) ||
               (next_tdm_slot_spacing > worse_tdm_slot_spacing)) {
                if (port == ML_CMIC) {
                    /* Exception for worst TDM slot spacing for CMIC port */
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                               "WorseSpacing:%d (prev:%d next:%d)"
                               "NOK !!!! port:%d\n"),
                                worse_tdm_slot_spacing,
                                prev_tdm_slot_spacing, next_tdm_slot_spacing,
                               port));
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,"**ATTN:SKIPPING FOR TIME-BEING \n")));
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                               "WorseSpacing:%d (prev:%d next:%d)"
                               "NOK !!!! port:%d\n"),
                                worse_tdm_slot_spacing,
                                prev_tdm_slot_spacing, next_tdm_slot_spacing,
                               port));
                    rv = SOC_E_CONFIG;
                    *offender_port=port;
                    continue;
                } 
           }
           count++;
           current = current->next;
        } while ((current !=head) && (rv == SOC_E_NONE)) ;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TDM Cycles: %d \n"),count));
        if (count < min_tdm_cycles) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "min_tdm_cycles:%dNOK  !!!! port:%d\n"),
                       min_tdm_cycles,port));
            rv =  SOC_E_CONFIG;
            *offender_port=port;
            continue;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "n== %d <==\n\n"),port));
   }

   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Verifying rule  \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Rule4 : Each subport in MXQPORT operates with 4 cycle TDM \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Verifying MXQports spacing concern\n")));
   for(mxqblock=0;mxqblock<(ML_MAX_BLOCKS-1) && rv == SOC_E_NONE ;mxqblock++) {
       for(index=0;
           index<ML_MAX_PORTS_PER_BLOCK && rv == SOC_E_NONE;
           index++) {
           outer_port = (*ml_block_ports[unit])[mxqblock][index];
           if (outer_port == 0xFF) {
               if ((mxqblock == 7)  && (index == 3)) {
                    outer_port = 40;
               } else {
                   continue;
               }
           }
           if (ml_tdm_port_slots_info[outer_port].position == -1) {
               continue;
           }
           head = current = &ml_tdm_port_slots_info[outer_port];
           do {
              for(loop=0;loop<ML_MAX_PORTS_PER_BLOCK; loop++) {
                  inner_port=(*ml_block_ports[unit])[mxqblock][loop];
                  if (inner_port == 0xFF) {
                      if ((mxqblock == 7)  && (loop == 3)) {
                           inner_port = 40;
                      } else {
                          continue;
                      }
                  }
                  if ( inner_port == outer_port) {
                       continue;
                  }
                  if (ml_tdm_port_slots_info[inner_port].position == -1) {
                      continue;
                  }
                  head1 = current1 = &ml_tdm_port_slots_info[inner_port];
                  do {
                       spacing = (current->position - current1->position) > 0 ?
                                 (current->position - current1->position)-1:
                                 (total_tdm_slots - current1->position +
                                  current->position) - 1;
                       if (spacing < 3) {
                           LOG_ERROR(BSL_LS_SOC_COMMON,
                                     (BSL_META_U(unit,
                                                 "Outer Port:%d Inner Port:%d "
                                                 "MXQSpacing Issue"
                                                 "Expected > 4 but observed %d\n"),
                                      outer_port,inner_port,spacing));
                           rv = SOC_E_CONFIG;
                           *offender_port=outer_port;
                           continue;
                       }
                       current1 = current1->next;
                  } while ((current1 !=head1) && (rv == SOC_E_NONE));
              }
              current = current->next;
         } while ((current !=head) && (rv == SOC_E_NONE));
      }
  }

  /* Free allocated resources */
  count=0;
  for (port=0;port < ML_MAX_LOGICAL_PORTS ; port++) {
       head = current = &ml_tdm_port_slots_info[port];
       do {
          temp = current->next;
          if (current != head) {
              count++;
              LOG_VERBOSE(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Freed count :%d\n"),count));
              sal_free(current);
          }          
          current = temp;
       } while (current != head);
       ml_tdm_port_slots_info[port].prev=NULL;
       ml_tdm_port_slots_info[port].next=NULL;
       ml_tdm_port_slots_info[port].position = -1;
  }
  sal_memcpy(&ml_current_tdm_cycles_info,
             &ml_tdm_cycles_info[tdm_freq_index][0],
             sizeof(tdm_cycles_info_t)*KT2_MAX_SPEEDS);
  return rv;
    return SOC_E_NONE;
}
#endif

#ifdef BCM_WARM_BOOT_SUPPORT

uint32  *iarb_tdm_ml[SOC_MAX_NUM_DEVICES];

int soc_ml_retrieve_hw_tdm(int unit)
{
    uint32                  rval = 0;
    uint8                   tdm_no = 0;
    soc_mem_t               tdm_table[]={IARB_TDM_TABLEm , IARB_TDM_TABLE_1m};
    iarb_tdm_table_entry_t  iarb_tdm_entry;
    uint32                  num_iarb_entry = 0, iarb_size=0;
    int                     index = 0;

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    tdm_no = soc_reg_field_get(unit, IARB_TDM_CONTROLr, rval, SELECT_TDMf);

    /* Allocate memory for TDM. Get size from IARB table. */
    num_iarb_entry = soc_mem_index_count(unit,IARB_TDM_TABLEm);
    iarb_size =  num_iarb_entry * sizeof(iarb_tdm_table_entry_t);

    if(iarb_tdm_ml[unit] != NULL) {
        sal_free(iarb_tdm_ml[unit]);
        iarb_tdm_ml[unit] = NULL;
    }
    iarb_tdm_ml[unit] = sal_alloc(iarb_size, "iarb_tdm_ml");
    if (iarb_tdm_ml[unit] == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(iarb_tdm_ml[unit], 0, iarb_size);

    for (index = 0; index < num_iarb_entry; index++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, tdm_table[tdm_no], SOC_BLOCK_ALL,
                    index, &iarb_tdm_entry));
        iarb_tdm_ml[unit][index] = soc_mem_field32_get(unit, tdm_table[tdm_no],
                &iarb_tdm_entry, PORT_NUMf);
    }
    return SOC_E_NONE;
}


/* Functions related to flexio scache */
int _soc_ml_flexio_scache_allocate(int unit)
{
    int                 rv=SOC_E_NONE;
    uint8               *flexio_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get=0;
    uint32 alloc_size =  sizeof(scache_begin_magic_number) +
                         sizeof(ml_scache_tdm_info) +
                         sizeof(ml_scache_tdm) +
                         sizeof(ml_scache_speed) +
                         sizeof(ml_port_speeds_scache) +
                         sizeof(ml_scache_port_details) +
                         sizeof(scache_end_magic_number);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE,
            BCM_FLEXIO_SCACHE_DEFAULT_PARTITION);

    rv = soc_scache_alloc(unit, scache_handle, alloc_size);
    if (rv  == SOC_E_CONFIG) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }

    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
    SOC_IF_ERROR_RETURN(soc_scache_ptr_get(unit, scache_handle,
                        &flexio_scache_ptr, &alloc_get));

    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }
    if (NULL == flexio_scache_ptr) {
        return SOC_E_MEMORY;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                        "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
    return SOC_E_NONE;
}

int _soc_ml_flexio_scache_retrieve(int unit)
{
    int                 rv;
    uint8               *flexio_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get=0;

    uint32 alloc_size =  sizeof(ml_scache_tdm_info) +
                         sizeof(ml_scache_tdm) +
                         sizeof(ml_scache_speed) +
                         sizeof(ml_port_speeds_scache) +
                         sizeof(ml_scache_port_details) +
                         SOC_WB_SCACHE_CONTROL_SIZE;
    uint32              scache_offset=0;
    uint16              default_ver = BCM_WB_DEFAULT_VERSION;
    uint16              recovered_ver;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE,
            BCM_FLEXIO_SCACHE_DEFAULT_PARTITION);

    rv = soc_scache_ptr_get(unit, scache_handle,
                                &flexio_scache_ptr, &alloc_get);
    if (rv == SOC_E_NOT_FOUND) {
        /* Partition 1 is not available, fall back to old retrieve procedure*/
        SOC_IF_ERROR_RETURN(_soc_ml_flexio_scache_retrieve_part0(unit));
        return SOC_E_NONE;
    }

    if (alloc_get != (alloc_size + SOC_WB_SCACHE_CONTROL_SIZE)) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }

    /* Partition 1 is found. So Extract sync-ed details */
    scache_offset = 0;
    sal_memcpy(&recovered_ver,
                  &flexio_scache_ptr[scache_offset], sizeof(uint16));

    if( default_ver > recovered_ver ) {
       LOG_VERBOSE(BSL_LS_SOC_COMMON,
                   (BSL_META_U(unit, "Scache Upgraded.\n")));
    }

    scache_offset += SOC_WB_SCACHE_CONTROL_SIZE;
    sal_memcpy(&ml_scache_tdm_info,
               &flexio_scache_ptr[scache_offset], sizeof(ml_scache_tdm_info));

    bcm56270_tdm_info[COUNTOF(bcm56270_tdm_info)-1] = ml_scache_tdm_info;

    scache_offset += sizeof(ml_scache_tdm_info);
    sal_memcpy(&ml_scache_tdm,
               &flexio_scache_ptr[scache_offset], sizeof(ml_scache_tdm));

    scache_offset += sizeof(ml_scache_tdm);
    sal_memcpy(&ml_scache_speed,
               &flexio_scache_ptr[scache_offset], sizeof(ml_scache_speed));

    scache_offset += sizeof(ml_scache_speed);

    sal_memcpy(&ml_port_speeds_scache,
               &flexio_scache_ptr[scache_offset], sizeof(ml_port_speeds_scache));

    scache_offset += sizeof(ml_port_speeds_scache);

    sal_memcpy(&ml_scache_port_details,
               &flexio_scache_ptr[scache_offset],
               sizeof(ml_scache_port_details));
    scache_offset += sizeof(ml_scache_port_details);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));
    return SOC_E_NONE;
}


int _soc_ml_flexio_scache_retrieve_part0(int unit)
{
    int                 rv;
    uint8               *flexio_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get=0;
    uint32 alloc_size =  sizeof(scache_begin_magic_number) +
                         sizeof(ml_scache_tdm_info) +
                         sizeof(ml_scache_tdm) +
                         sizeof(ml_scache_speed) +
                         sizeof(ml_port_speeds_scache) +
                         sizeof(ml_scache_port_details) +
                         sizeof(scache_end_magic_number);
    uint32              scache_offset=0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);

    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &flexio_scache_ptr, &alloc_get);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }

    if (alloc_get != (alloc_size + SOC_WB_SCACHE_CONTROL_SIZE)) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }

    if (NULL == flexio_scache_ptr) {
        return SOC_E_MEMORY;
    }

    scache_offset = 0;
    if(sal_memcmp(&flexio_scache_ptr[scache_offset],
                  &scache_begin_magic_number[0],
                  sizeof(scache_begin_magic_number)) != 0) {
        LOG_CLI((BSL_META_U(unit,
                            "Corrupt scache for flexio operation \n")));
       return SOC_E_INTERNAL;
    }

    scache_offset += sizeof(scache_begin_magic_number);
    sal_memcpy(&ml_scache_tdm_info,
               &flexio_scache_ptr[scache_offset], sizeof(ml_scache_tdm_info));

    bcm56270_tdm_info[COUNTOF(bcm56270_tdm_info)-1] = ml_scache_tdm_info;

    scache_offset += sizeof(ml_scache_tdm_info);

    /*
     * Part 0 Version has an issue in TDM sync.
     * So in part 0 version, Level 1 recovery performed to retrieve TDM.
     */
    LOG_INFO(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                        "Previous ver was synced with incorrect TDM,"
                        "Now LEVEL 1 recovery for TDM\n")));

    SOC_IF_ERROR_RETURN(soc_ml_retrieve_hw_tdm(unit));
    if(iarb_tdm_ml[unit] == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&ml_scache_tdm, iarb_tdm_ml[unit], sizeof(ml_scache_tdm));

    kt2_tdm_display(unit, (uint32 *) (&ml_scache_tdm[0]),
            ml_scache_tdm_info.tdm_size,
            ml_scache_tdm_info.row,
            ml_scache_tdm_info.col);

    scache_offset += sizeof(ml_scache_tdm);
    sal_memcpy(&ml_scache_speed,
               &flexio_scache_ptr[scache_offset], sizeof(ml_scache_speed));

    scache_offset += sizeof(ml_scache_speed);

    sal_memcpy(&ml_port_speeds_scache,
               &flexio_scache_ptr[scache_offset], sizeof(ml_port_speeds_scache));

    scache_offset += sizeof(ml_port_speeds_scache);

    sal_memcpy(&ml_scache_port_details,
               &flexio_scache_ptr[scache_offset],
               sizeof(ml_scache_port_details));
    scache_offset += sizeof(ml_scache_port_details);

    if (sal_memcmp(&flexio_scache_ptr[scache_offset],
                   &scache_end_magic_number[0],
                   sizeof(scache_end_magic_number)) != 0) {
        LOG_CLI((BSL_META_U(unit,
                            "Corrupt scache for flexio operation \n")));
        return SOC_E_INTERNAL;
    }

    /* Allocate SCACHE at part 1 Now */
    SOC_IF_ERROR_RETURN(_soc_ml_flexio_scache_allocate(unit));

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

int _soc_ml_flexio_scache_sync(int unit)
{
    int                 rv;
    uint8               *flexio_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get=0;
    uint32 alloc_size =  sizeof(ml_scache_tdm_info) +
                         sizeof(ml_scache_tdm) +
                         sizeof(ml_scache_speed) +
                         sizeof(ml_port_speeds_scache) +
                         sizeof(ml_scache_port_details) +
                         SOC_WB_SCACHE_CONTROL_SIZE;
    uint32              scache_offset=0;
    int                 cfg_num=0;
    uint32              port=0;
    uint16              default_ver = BCM_WB_DEFAULT_VERSION;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE,
            BCM_FLEXIO_SCACHE_DEFAULT_PARTITION);

    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &flexio_scache_ptr, &alloc_get);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    if (alloc_get != (alloc_size + SOC_WB_SCACHE_CONTROL_SIZE)) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }
    if (NULL == flexio_scache_ptr) {
        return SOC_E_MEMORY;
    }

    SOC_IF_ERROR_RETURN(_soc_metrolite_get_cfg_num(unit, 0, &cfg_num));

    scache_offset = 0;
    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &default_ver, sizeof(uint16));

    scache_offset += SOC_WB_SCACHE_CONTROL_SIZE;
    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &bcm56270_tdm_info[cfg_num],
               sizeof(ml_scache_tdm_info));

    scache_offset += sizeof(ml_scache_tdm_info);
    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &ml_current_tdm[0],
               sizeof(uint32)* bcm56270_tdm_info[cfg_num].tdm_size);

    scache_offset += sizeof(ml_scache_tdm);

    for (port=1 ; port <= ML_MAX_PHYSICAL_PORTS ; port++) {
         ml_scache_speed[port-1] = 0;
         /* mxqblock = (*kt2_port_to_mxqblock[unit])[port-1]; */
         ml_scache_port_details[port-1].start_port_no = port;
         ml_scache_port_details[port-1].end_port_no = port;
         ml_scache_port_details[port-1].port_incr = 1;
         ml_scache_port_details[port-1].port_type = 0;
         ml_scache_port_details[port-1].port_speed = 0xFFFF;

         if (IS_MXQ_PORT(unit,port)) {
             if ((bcm56270_speed[unit][cfg_num][port-1] == 0) || /*Converted */
                 (bcm56270_speed[unit][cfg_num][port-1] >= 2500)) {
                  ml_scache_speed[port - 1] = 2500;
                  ml_scache_port_details[port-1].port_speed = 2500;
             } else {
                  ml_scache_speed[port - 1] = 1000;
                  ml_scache_port_details[port-1].port_speed = 1000;
             }
             ml_scache_port_details[port-1].port_type = GE_PORT;
         }

         if (IS_XE_PORT(unit,port)) {
             ml_scache_speed[port - 1] = 10000;
             ml_scache_port_details[port-1].port_speed = 10000;
             ml_scache_port_details[port-1].port_type |= XE_PORT;
             if (IS_MXQ_PORT(unit,port)) {
                 ml_scache_port_details[port-1].port_incr = 4;
             }
         }

         if (IS_HG_PORT(unit,port)) {
             ml_scache_speed[port - 1] = 10000;
             ml_scache_port_details[port-1].port_speed = 10000;
             ml_scache_port_details[port-1].port_type |= HG_PORT;
             ml_scache_port_details[port-1].port_type |= STK_PORT;
         }

        if (IS_XL_PORT(unit, port) && (IS_GE_PORT(unit, port))) {
            if (bcm56270_speed[unit][cfg_num][port-1] >= 2500) {
                ml_scache_speed[port - 1] = 2500;
                ml_scache_port_details[port-1].port_speed = 2500;
            } else {
                ml_scache_speed[port - 1] = 1000;
                ml_scache_port_details[port-1].port_speed = 1000;
            }
            ml_scache_port_details[port-1].port_type |= GE_PORT;
        }
    }

    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &ml_scache_speed, sizeof(ml_scache_speed));
    scache_offset += sizeof(ml_scache_speed);

    sal_memcpy(&flexio_scache_ptr[scache_offset],
               ml_port_speeds_s[cfg_num], sizeof(ml_port_speeds_scache));
    scache_offset += sizeof(ml_port_speeds_scache);

    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &ml_scache_port_details,
               sizeof(ml_scache_port_details));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));
    return SOC_E_NONE;
}
#endif /* end of BCM_WARM_BOOT_SUPPORT */

void soc_metrolite_save_tdm_pos(int unit, uint32 new_tdm_size,uint32 *new_tdm) {
    uint32 pos=0;
    uint8 port=0;
    uint8 block=0;
    uint8 total_slots=0;
    
    sal_memset(ml_tdm_pos_info,0,sizeof(ml_tdm_pos_info));
    for (pos = 0; pos < new_tdm_size ; pos++) {
         port= new_tdm[pos];
         if ((port == ML_IDLE) || (port == ML_LPBK) || 
             (port == ML_CMIC)) {
              continue;
         }
         if(port > ML_MAX_PHYSICAL_PORTS ) {
             LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                             "tdm_size %d, pos %d, port %d \n"),
                               new_tdm_size, pos, port));

         } else {
             block = (*ml_port_to_block[unit])[port - 1];
             total_slots = ml_tdm_pos_info[block].total_slots;
             ml_tdm_pos_info[block].pos[total_slots]=pos;
             ml_tdm_pos_info[block].total_slots++;
         }
    }

    sal_memcpy(ml_current_tdm,new_tdm,new_tdm_size*sizeof(new_tdm[0]));
    ml_current_tdm_size=new_tdm_size;

}

/* This function splits the occurance of first port of the block into 4 ports */
/* Make sure that the tdm of the block is not already split */
void ml_block_tdm_split(int unit, uint32 *tdm,
        uint32 total_tdm_slots, uint32 block)
{
     uint32 index=0, count=0;
     uint32 first_port;

     first_port = (*ml_block_ports[unit])[block][0];
     for (index=0; index < total_tdm_slots ; index++ ) {
         /* Split first port occurance in to 4 */
          if (tdm[index] == first_port) {
              tdm[index] = first_port + (count % 4);
              count++;
          }

     }
}

void ml_tdm_replace(uint32 *tdm, uint32 total_tdm_slots,
                     uint32  src, uint32 dst,uint32 one_time)
{
     uint32 index=0;
     for (index=0; index < total_tdm_slots ; index++ ) {
          if (tdm[index] == src) {
              tdm[index] = dst;
              if (one_time) {
                  break;
              }
          }
     }
}

void ml_tdm_swap(uint32 *tdm, uint32 total_tdm_slots,
                  uint32  src, uint32 dst)
{
     uint32 index=0;
     uint32 temp_index=0;
     uint32 temp=0;
     for (index=0; index < total_tdm_slots ; index++ ) {
          if (tdm[index] == src) {
              temp_index=index;
          }
          if (tdm[index] == dst) {
              temp = tdm[temp_index];
              tdm[temp_index]=dst;
              tdm[index]=temp;
          }
     }
}

soc_error_t
soc_metrolite_reconfigure_tdm(int unit,uint32 new_tdm_size,uint32 *new_tdm)
{
    uint8                   new_tdm_no=0;
    soc_mem_t               tdm_table[]={IARB_TDM_TABLEm , IARB_TDM_TABLE_1m};
    soc_field_t             wrap_ptr[]={TDM_WRAP_PTRf , TDM_1_WRAP_PTRf};
    uint32                  lls_tdm_size=0;
    lls_port_tdm_entry_t    *lls_tdm=NULL;
    uint32                  iarb_size=0;
    iarb_tdm_table_entry_t  *iarb_tdm=NULL;
    int                     iter=0;
    uint32                  index;
    uint32                  rval;
    int                     cfg_num;

    SOC_IF_ERROR_RETURN(_soc_metrolite_get_cfg_num(unit, 0, &cfg_num));
    if (cfg_num >= (sizeof(bcm56270_tdm)/sizeof(bcm56270_tdm[0]))) {
        return SOC_E_FAIL;
    } 
    
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    old_tdm_no = soc_reg_field_get(unit, IARB_TDM_CONTROLr, rval, SELECT_TDMf);
    new_tdm_no = (old_tdm_no + 1)%2;

    iarb_size =  soc_mem_index_count(unit,IARB_TDM_TABLEm) *
                 sizeof(iarb_tdm_table_entry_t);
    iarb_tdm = soc_cm_salloc(unit,iarb_size, "iarb_tdm_table_entry_t");
    if (iarb_tdm == NULL) {
        return SOC_E_MEMORY;
    }            
    sal_memset(iarb_tdm,0,iarb_size);

    lls_tdm_size =  soc_mem_index_count(unit,LLS_PORT_TDMm) *
                    sizeof(lls_port_tdm_entry_t);
    lls_tdm = soc_cm_salloc(unit,lls_tdm_size, "lls_port_tdm_entry_t");
    if (lls_tdm == NULL) {
        soc_cm_sfree(unit,iarb_tdm);
        return SOC_E_MEMORY;
    }
    sal_memset(lls_tdm,0,lls_tdm_size);

    for (index = 0; index < new_tdm_size; index++) {
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm[index], PORT_NUMf,
                                        new_tdm[index]);
    }   
    soc_mem_write_range(unit, tdm_table[new_tdm_no], MEM_BLOCK_ALL, 0,
                        new_tdm_size, iarb_tdm);
    soc_cm_sfree(unit,iarb_tdm);        

    for (index = 0; index < new_tdm_size/2; index++) {
         soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                       PORT_ID_0f, new_tdm[index*2]);
         if ( new_tdm[index*2] < ML_IDLE ) {
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                            PORT_ID_0_ENABLEf, 1);
         } else {
              LOG_VERBOSE(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "IDLE SLOTs so PORT_ID_0_ENABLEf=0 \
                                       for index=%d\n"), index));
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                            PORT_ID_0_ENABLEf, 0);
         }

         soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                       PORT_ID_1f, new_tdm[(index*2) + 1]);
         if ( new_tdm[(index*2)+1] < ML_IDLE ) {
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                            PORT_ID_1_ENABLEf, 1);
         } else {
              LOG_VERBOSE(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "IDLE SLOTs so PORT_ID_1_ENABLEf=0 \
                                       for index=%d\n"), index));
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                            PORT_ID_1_ENABLEf, 0);
         } 
    } /* for index */

    if (new_tdm_size % 2) {
        LOG_CLI((BSL_META_U(unit,           
                            "ODD TDM SIZE \n")));
        soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index*2],
                                      PORT_ID_0_ENABLEf, 1);
        soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index*2],
                                      PORT_ID_0f, new_tdm[(index*2)]);
        soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[(index*2)+1],
                                      PORT_ID_1_ENABLEf, 0);
        soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[(index*2)+1],
                                      PORT_ID_1f, 0);
        index++;
    }

    soc_mem_write_range(unit, LLS_PORT_TDMm, MEM_BLOCK_ALL,
                        new_tdm_no*256, (new_tdm_no*256) + index,
                        lls_tdm);
    soc_cm_sfree(unit,lls_tdm);

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, 
                      wrap_ptr[new_tdm_no],new_tdm_size -1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, SELECT_TDMf, new_tdm_no);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        kt2_tdm_display(unit,new_tdm,new_tdm_size,
                        bcm56270_tdm_info[cfg_num].row,
                        bcm56270_tdm_info[cfg_num].col);
    }

    SOC_IF_ERROR_RETURN(WRITE_LLS_TDM_CAL_CFG_SWITCHr(unit, 1));
    if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
        SOC_IF_ERROR_RETURN(READ_LLS_TDM_CAL_CFGr(unit, &rval));
        soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval,
                          CURRENT_CALENDARf,new_tdm_no);
        SOC_IF_ERROR_RETURN(WRITE_LLS_TDM_CAL_CFGr(unit, rval));
    }

    do {
        SOC_IF_ERROR_RETURN(READ_LLS_TDM_CAL_CFGr(unit, &rval));
        sal_udelay(100);
        if (soc_reg_field_get(unit, LLS_TDM_CAL_CFGr, rval,
                              CURRENT_CALENDARf) == new_tdm_no) {
            break;
        }
    } while (iter++ < 100000);

    if (iter >= 100000) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LLS Calendar switch failed !!\n")));
        return SOC_E_INTERNAL;
    }

    old_tdm_no = new_tdm_no;
    soc_metrolite_save_tdm_pos(unit, new_tdm_size,new_tdm);

    return SOC_E_NONE;
}
#if 0
soc_error_t ml_tdm_verify(int unit,
                           uint32 *tdm,
                           uint32 total_tdm_slots,
                           uint32 tdm_freq,    /* In MHHz */
                           uint32 *port_speed,
                           uint32 lpbk_port_speed,
                           uint32 *offender_port) 
{
    /* Following rules need to be met */
    /* Rule1 : Each Loopback port need 2.5G bandwidth */
    /* Rule2 : Each CMIC port need 2G bandwidth       */
    /* Rule3 : Loopback (LPBK) ports require min 3 cycle spacing among LPBK or 
               CMIC ports */ 
    /* Rule4 : Each subport in MXQPORT operates with 4 cycle TDM */
    /* Rule5 : Each 10G-XAUI ports should be able to operate in 4x2.5G with 4 
               slots allocated to each subport from the same MXQPORT */
    soc_error_t           rv=SOC_E_NONE;
    uint32                index=0;
    uint32                expected_tdm_cycles_min=0;
    uint32                expected_tdm_cycles_max=0;
    uint32                tdm_freq_index=0;
    uint32                speed_index=0;
    uint32                port=0;
    uint32                speed=0;
    uint32                worse_tdm_slot_spacing=0;
    uint32                prev_tdm_slot_spacing=0;
    uint32                next_tdm_slot_spacing=0;
#if 0
    uint32                tdm_slot_spacing=0;
#endif
    uint32                min_tdm_cycles=0; /* To Meet BW */
    uint32                count=0; 
    uint32                skip_count=0; 
    uint32                mxqblock=0;
    uint32                outer_port=0;
    uint32                inner_port=0;
    uint32                spacing=0;
    uint32                loop=0;
    tdm_port_slots_info_t *head=NULL;
    tdm_port_slots_info_t *head1=NULL;
    tdm_port_slots_info_t *current=NULL;
    tdm_port_slots_info_t *current1=NULL;
    tdm_port_slots_info_t *prev = NULL;
    tdm_port_slots_info_t *temp=NULL;
    uint32                idle_flag=0;
    uint32                idle_rule=0;
    char                  base_tdm_str[80];

   /* Possible TDM frequencies 
      80MHz(16Cycles) ,120MHz(20Cycles),155MHz(84Cycles),
      185MHz(80Cycles),185MHz(90Cycles) 
    */
    switch(tdm_freq) {
    case 130:
        if (total_tdm_slots == 284) {
            /*TDM_A : 0:130MHz: 284 (41 * 7) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_A : 0:130MHz: 284 (41 * 7) Cycles Used",
                        79);
            tdm_freq_index=0;
            expected_tdm_cycles_min=284;
            expected_tdm_cycles_max=284;
        } else if (total_tdm_slots == 141) {
            /*TDM_C,C1,C2: 7:130MHz: 141 (21 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_AA,TDM_C,C1,C2: 7:130MHz:141 (21 * 6) Cycles Used",
                        79);
            tdm_freq_index=7;
            expected_tdm_cycles_min=141;
            expected_tdm_cycles_max=141;
        } else if (total_tdm_slots == 168) {
            /*TDM_D: 8:130MHz: 168 (28 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_D: 8:130MHz: 168 (28 * 6) Cycles Used",
                        79);
            tdm_freq_index=8;
            expected_tdm_cycles_min=168;
            expected_tdm_cycles_max=168;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                      "Unsupported tdm slots:%d\n"),
                      total_tdm_slots));
            return SOC_E_FAIL;
        }
        break;
    case 24:
        /*TDM_A1: 2:24MHz: 13 (4 * 4) Cycles Used */
        sal_strncpy(base_tdm_str, 
                    "TDM_A1: 2:24MHz: 13 (4 * 4) Cycles Used ",
                    79);
        tdm_freq_index=2;
        expected_tdm_cycles_min=13;
        expected_tdm_cycles_max=13;
        break;
    case 37:
        /*TDM_A1A: 3:37MHz: 84 (21 * 4) Cycles Used */
        sal_strncpy(base_tdm_str, 
                    "TDM_A1A: 3:37MHz: 84 (21 * 4) Cycles Used ",
                    79);
        tdm_freq_index=3;
        expected_tdm_cycles_min=84;
        expected_tdm_cycles_max=84;
        break;
    case 118:
        if (total_tdm_slots == 133) {
            /*TDM_B: 4:118MHz: 133 (23 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_B: 4:118MHz: 133 (23 * 6) Cycles Used ",
                        79);
            tdm_freq_index=4;
            expected_tdm_cycles_min=133;
            expected_tdm_cycles_max=133;
        } else if (total_tdm_slots == 120) {
            /*TDM_B1: 5:118MHz: 120 (20 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_B1: 5:118MHz: 120 (20 * 6) Cycles Used ",
                        79);
            tdm_freq_index=5;
            expected_tdm_cycles_min=120;
            expected_tdm_cycles_max=120;
        } else if (total_tdm_slots == 121) {
            /*TDM_B1A: 6:118MHz: 121 (21 * 6) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_B1A: 6:118MHz: 121 (21 * 6) Cycles Used ",
                        79);
            tdm_freq_index=6;
            expected_tdm_cycles_min=121;
            expected_tdm_cycles_max=121;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                      "Unsupported tdm slots:%d\n"),
                      total_tdm_slots));
            return SOC_E_FAIL;
        }
        break;
    case 75:
        if (total_tdm_slots == 81) {
            /*TDM_E,E1,E3 9:75MHz: 81 (21 * 4) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_E,E1,E3 9:75MHz: 81 (21 * 4) Cycles Used ",
                        79);
            tdm_freq_index=9;
            expected_tdm_cycles_min=81;
            expected_tdm_cycles_max=81;
        } else if (total_tdm_slots == 16) {
            /*TDM_EA 10:75MHz: 16 (4 * 4) Cycles Used */
            sal_strncpy(base_tdm_str, 
                        "TDM_EA 10:75MHz: 16 (4 * 4) Cycles Used ",
                        79);
            tdm_freq_index=10;
            expected_tdm_cycles_min=16;
            expected_tdm_cycles_max=16;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                      "Unsupported tdm slots:%d\n"),
                      total_tdm_slots));
            return SOC_E_FAIL;
        }
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Unsupported tdm frequency:%d\n"),
                   tdm_freq));
        return SOC_E_FAIL;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
             "\nINFO: BaseTdmStr:%s :Index %d:MinCycles%d:MaxCycle%d\n"),
             base_tdm_str, 
             tdm_freq_index, expected_tdm_cycles_min, expected_tdm_cycles_max));
   if (!((total_tdm_slots >= expected_tdm_cycles_min) && 
         (total_tdm_slots <= expected_tdm_cycles_max))) {
          LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Unsupported tdm cycles:%d (min:%d max:%d\n"),
                     total_tdm_slots,
                     expected_tdm_cycles_min,expected_tdm_cycles_max));
          return SOC_E_FAIL; 
   }

    idle_flag=0;
    idle_rule = 0;
    /* Initialize list */
    for (port=0;port < ML_MAX_LOGICAL_PORTS ; port++) {
         ml_tdm_port_slots_info[port].prev=&ml_tdm_port_slots_info[port];
         ml_tdm_port_slots_info[port].next=&ml_tdm_port_slots_info[port];
         ml_tdm_port_slots_info[port].position = -1;
    }
    for (index=0; index < total_tdm_slots; index++) {
         if ((tdm[index] == ML_IDLE1) ||
             (tdm[index] == ML_IDLE)) {
             if (tdm[index] == ML_IDLE) {
                 if (idle_flag == 1) {
                     idle_rule = 1;
                 }
                 idle_flag=1;
             }
             skip_count++;
             continue;
         }
         if (((index - 1) != 0) && (index != total_tdm_slots - 1) && 
             (idle_flag == 1) && (idle_rule == 0)) {
              LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                          "### Internal Error: TDM Verification failed \n"
                          "### Two consecutive idle not found at position:%d\n"
                          "### TCAM Atomicity might get affected.\n"),
                          index - 1));
             *offender_port = ML_IDLE;
             return SOC_E_CONFIG;
         }
         idle_flag=0;
         port = tdm[index];
         if (ml_tdm_port_slots_info[port].position == -1) {
             /* First Entry */
             ml_tdm_port_slots_info[port].position = index;
             skip_count++;
             continue;
         }
         head = current = &ml_tdm_port_slots_info[port];
         while(current->next !=head) {
               current = current->next;
         } 
         current->next=sal_alloc(sizeof(tdm_port_slots_info_t),"TDM Pointer");
         count++;
         LOG_VERBOSE(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Count=%d SkipCount:%d  Port:%d  Addr:%p\n"),
                      count,skip_count,port,(void *)current->next));

         current->next->position=index;
         current->next->prev=current;
         current->next->next=head;
         head->prev=current->next;
   }
   if (idle_rule == 0) {
       /* Special case */
       if (!((tdm[0] == ML_IDLE) &&
             (tdm[total_tdm_slots-1] == ML_IDLE))) {
              LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                       "### Internal Error: TDM Verification failed \n"
                       "### Two consecutive idle not found at Edges \n"
                       "### TCAM Atomicity might get affected.\n")));
           *offender_port = ML_IDLE1;
           return SOC_E_CONFIG;
       }
   }
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Final Count=%d SkipCount:%d\n"),count,skip_count));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Verifying rule  \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Rule3: Loopback (LPBK) ports require min 3 cycle spacing "
                           "among LPBK or CMIC ports \n")));
   if (ml_tdm_port_slots_info[ML_LPBK].position == -1) {
       LOG_CLI((BSL_META_U(unit,
                           "Loopback port  missing in configuration \n")));
       return SOC_E_FAIL;
   }
   if (ml_tdm_port_slots_info[ML_CMIC].position == -1) {
       LOG_CLI((BSL_META_U(unit,
                           "CMIC port  missing in configuration \n")));
       return SOC_E_FAIL;
   }
   head = current = &ml_tdm_port_slots_info[ML_LPBK];
   do {
      prev = current->prev;
      if ( current->position != prev->position ) {
           prev_tdm_slot_spacing = 
                          (current->position - prev->position) > 0 ?
                          (current->position - prev->position)-1:
                          (total_tdm_slots - prev->position + 
                           current->position) - 1;
           next_tdm_slot_spacing = 
                          (current->next->position - current->position) >0 ?
                          (current->next->position - current->position)-1:
                          (total_tdm_slots - current->position +
                           current->next->position) - 1;
      if (prev_tdm_slot_spacing < 3) {
          LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Loopback Ports need min 3 spacing but(prev)found"
                                "(current:%d , prev:%d)=%d\n"),
                     current->position,prev->position,prev_tdm_slot_spacing));
          rv = SOC_E_FAIL;
          continue;
      } 
      if (next_tdm_slot_spacing < 3) {
          LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Loopback Ports need min 3 spacing but(next)found"
                                "(current:%d , prev:%d)=%d\n"),
                     current->position,prev->position,next_tdm_slot_spacing));
          rv = SOC_E_CONFIG;
          *offender_port = ML_LPBK;
          continue;
      } 
      } else {
          prev_tdm_slot_spacing = next_tdm_slot_spacing = 0;
      }
#if 0
      head1 = current1 = &ml_tdm_port_slots_info[ML_CMIC];
      do {
         tdm_slot_spacing = (current->position - current1->position) > 0 ?
                          (current->position - current1->position)-1:
                          (total_tdm_slots - current1->position + 
                           current->position) - 1  ;
         if (tdm_slot_spacing < 3) {
             LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                   "Loopback and CMIC ports need min 3 spacing but "
                                   "found:%u" "(Loopack current:%d , CMIC Current:%d)\n"),
                        tdm_slot_spacing,
                        current->position,current1->position));
             rv = SOC_E_CONFIG;
             *offender_port=ML_LPBK;
             continue;
         }
         current1 = current1->next;
      }while ((current1 !=head1) && (rv == SOC_E_NONE));
#endif
      current = current->next;
   } while ((current !=head) && (rv == SOC_E_NONE));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Verifying rules \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Rule1 : Each Loopback port need 2.5G bandwidth \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Rule2 : Each CMIC port need 2G bandwidth       \n")));
   for (index=0;index < ML_MAX_LOGICAL_PORTS && rv == SOC_E_NONE ;index++ ) {
        port = index ;
        if (ml_tdm_port_slots_info[port].position == -1) {
            continue;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                " %d ==>\n\n"),port));
        switch(port) {
        case ML_CMIC:
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "CMIM \n")));
            speed =2000; 
            break;
        case ML_LPBK:
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "LOOPBAKC \n")));
            speed =lpbk_port_speed; 
            break;
        default:
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    " Port:%d Speed %d ==>\n\n"),
                         port,port_speed[port-1]));
            speed = port_speed[port-1];
            break;
        }
        if ((speed == 0) && (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port))) {
            /* Eagle ports are not removed from TDM calendar 
             * Eventhough not active after lane op. Ignore those
             */
             continue;
        }

        switch(speed) {
        case 1000:
            speed_index=0;
            break;
        case 2000:
            speed_index=1;
            break;
        case 2500:
            speed_index=2;
            break;
        case 10000:
        case 10500: /* Might need ut fir HG11 */
        case 11000: /* Might need ut fir HG11 */
            speed_index=3;
            break;
        case 12500: /* Special case for LPBK port */
        case 13000:
            speed_index=4;
            break;
        case 20000:
            speed_index=5;
            break;
        case 21000:
            speed_index=6;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Unsupported speed:%d \n"),speed));
            return SOC_E_FAIL;
        }
        worse_tdm_slot_spacing=ml_tdm_cycles_info[tdm_freq_index][speed_index].
                               worse_tdm_slot_spacing;
        min_tdm_cycles=ml_tdm_cycles_info[tdm_freq_index][speed_index].
                       min_tdm_cycles;

        count=0;
        head = current = &ml_tdm_port_slots_info[port];
        do {
           prev = current->prev;
           if (current->position != prev->position ) {
               prev_tdm_slot_spacing = 
                          (current->position - prev->position)> 0 ?
                          (current->position - prev->position)-1:
                          (total_tdm_slots - prev->position + 
                           current->position) - 1  ;
               next_tdm_slot_spacing = 
                          (current->next->position - current->position) > 0 ?
                          (current->next->position - current->position)-1:
                          (total_tdm_slots - current->position + 
                           current->next->position) - 1;
           } else {
               prev_tdm_slot_spacing = next_tdm_slot_spacing = 0;
           }
           LOG_VERBOSE(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                   "Pos:%d Spacing (Prev %d - %d =  )%d"
                                   "(Next %d - %d =  )%d\n"), current->position,
                        current->position , prev->position ,
                        prev_tdm_slot_spacing, current->next->position , 
                        current->position , next_tdm_slot_spacing));
           if ((prev_tdm_slot_spacing > worse_tdm_slot_spacing) ||
               (next_tdm_slot_spacing > worse_tdm_slot_spacing)) {
                if (port == ML_CMIC) {
                    /* Exception for worst TDM slot spacing for CMIC port */
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                               "WorseSpacing:%d (prev:%d next:%d)"
                               "NOK !!!! port:%d\n"),
                                worse_tdm_slot_spacing,
                                prev_tdm_slot_spacing, next_tdm_slot_spacing,
                               port));
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,"**ATTN:SKIPPING FOR TIME-BEING \n")));
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                               "WorseSpacing:%d (prev:%d next:%d)"
                               "NOK !!!! port:%d\n"),
                                worse_tdm_slot_spacing,
                                prev_tdm_slot_spacing, next_tdm_slot_spacing,
                               port));
                    rv = SOC_E_CONFIG;
                    *offender_port=port;
                    continue;
                } 
           }
           count++;
           current = current->next;
        } while ((current !=head) && (rv == SOC_E_NONE)) ;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TDM Cycles: %d \n"),count));
        if (count < min_tdm_cycles) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "min_tdm_cycles:%dNOK  !!!! port:%d\n"),
                       min_tdm_cycles,port));
            rv =  SOC_E_CONFIG;
            *offender_port=port;
            continue;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "n== %d <==\n\n"),port));
   }

   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Verifying rule  \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Rule4 : Each subport in MXQPORT operates with 4 cycle TDM \n")));
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "Verifying MXQports spacing concern\n")));
   for(mxqblock=0;mxqblock<(ML_MAX_BLOCKS-1) && rv == SOC_E_NONE ;mxqblock++) {
       for(index=0;
           index<ML_MAX_PORTS_PER_BLOCK && rv == SOC_E_NONE;
           index++) {
           outer_port = (*ml_block_ports[unit])[mxqblock][index];
           if (outer_port == 0xFF) {
               if ((mxqblock == 7)  && (index == 3)) {
                    outer_port = 40;
               } else {
                   continue;
               }
           }
           if (ml_tdm_port_slots_info[outer_port].position == -1) {
               continue;
           }
           head = current = &ml_tdm_port_slots_info[outer_port];
           do {
              for(loop=0;loop<ML_MAX_PORTS_PER_BLOCK; loop++) {
                  inner_port=(*ml_block_ports[unit])[mxqblock][loop];
                  if (inner_port == 0xFF) {
                      if ((mxqblock == 7)  && (loop == 3)) {
                           inner_port = 40;
                      } else {
                          continue;
                      }
                  }
                  if ( inner_port == outer_port) {
                       continue;
                  }
                  if (ml_tdm_port_slots_info[inner_port].position == -1) {
                      continue;
                  }
                  head1 = current1 = &ml_tdm_port_slots_info[inner_port];
                  do {
                       spacing = (current->position - current1->position) > 0 ?
                                 (current->position - current1->position)-1:
                                 (total_tdm_slots - current1->position +
                                  current->position) - 1;
                       if (spacing < 3) {
                           LOG_ERROR(BSL_LS_SOC_COMMON,
                                     (BSL_META_U(unit,
                                                 "Outer Port:%d Inner Port:%d "
                                                 "MXQSpacing Issue"
                                                 "Expected > 4 but observed %d\n"),
                                      outer_port,inner_port,spacing));
                           rv = SOC_E_CONFIG;
                           *offender_port=outer_port;
                           continue;
                       }
                       current1 = current1->next;
                  } while ((current1 !=head1) && (rv == SOC_E_NONE));
              }
              current = current->next;
         } while ((current !=head) && (rv == SOC_E_NONE));
      }
  }

  /* Free allocated resources */
  count=0;
  for (port=0;port < ML_MAX_LOGICAL_PORTS ; port++) {
       head = current = &ml_tdm_port_slots_info[port];
       do {
          temp = current->next;
          if (current != head) {
              count++;
              LOG_VERBOSE(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Freed count :%d\n"),count));
              sal_free(current);
          }          
          current = temp;
       } while (current != head);
       ml_tdm_port_slots_info[port].prev=NULL;
       ml_tdm_port_slots_info[port].next=NULL;
       ml_tdm_port_slots_info[port].position = -1;
  }
  sal_memcpy(&ml_current_tdm_cycles_info,
             &ml_tdm_cycles_info[tdm_freq_index][0],
             sizeof(tdm_cycles_info_t)*KT2_MAX_SPEEDS);
  return rv;
    return SOC_E_NONE;
}
#endif
#if 0
void soc_metrolite_save_tdm_pos(int unit, uint32 new_tdm_size,uint32 *new_tdm) {
    uint32 pos=0;
    uint8 port=0;
    uint8 block=0;
    uint8 total_slots=0;
    
    sal_memset(ml_tdm_pos_info,0,sizeof(ml_tdm_pos_info));
    for (pos = 0; pos < new_tdm_size ; pos++) {
         port= new_tdm[pos];
         if ((port == ML_IDLE) || (port == ML_LPBK) || 
             (port == ML_CMIC)) {
              continue;
         }
         if(port > ML_MAX_PHYSICAL_PORTS ) {
             LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                             "tdm_size %d, pos %d, port %d \n"),
                               new_tdm_size, pos, port));

         } else {
             block = (*ml_port_to_block[unit])[port - 1];
             total_slots = ml_tdm_pos_info[block].total_slots;
             ml_tdm_pos_info[block].pos[total_slots]=pos;
             ml_tdm_pos_info[block].total_slots++;
         }
    }

    sal_memcpy(ml_current_tdm,new_tdm,new_tdm_size*sizeof(new_tdm[0]));
    ml_current_tdm_size=new_tdm_size;

}

/* This function splits the occurance of first port of the block into 4 ports */
/* Make sure that the tdm of the block is not already split */
void ml_block_tdm_split(int unit, uint32 *tdm,
        uint32 total_tdm_slots, uint32 block)
{
     uint32 index=0, count=0;
     uint32 first_port;

     first_port = (*ml_block_ports[unit])[block][0];
     for (index=0; index < total_tdm_slots ; index++ ) {
         /* Split first port occurance in to 4 */
          if (tdm[index] == first_port) {
              tdm[index] = first_port + (count % 4);
              count++;
          }

     }
}

void ml_tdm_replace(uint32 *tdm, uint32 total_tdm_slots,
                     uint32  src, uint32 dst,uint32 one_time)
{
     uint32 index=0;
     for (index=0; index < total_tdm_slots ; index++ ) {
          if (tdm[index] == src) {
              tdm[index] = dst;
              if (one_time) {
                  break;
              }
          }
     }
}

void ml_tdm_swap(uint32 *tdm, uint32 total_tdm_slots,
                  uint32  src, uint32 dst)
{
     uint32 index=0;
     uint32 temp_index=0;
     uint32 temp=0;
     for (index=0; index < total_tdm_slots ; index++ ) {
          if (tdm[index] == src) {
              temp_index=index;
          }
          if (tdm[index] == dst) {
              temp = tdm[temp_index];
              tdm[temp_index]=dst;
              tdm[index]=temp;
          }
     }
}

soc_error_t
soc_metrolite_reconfigure_tdm(int unit,uint32 new_tdm_size,uint32 *new_tdm)
{
    uint8                   new_tdm_no=0;
    soc_mem_t               tdm_table[]={IARB_TDM_TABLEm , IARB_TDM_TABLE_1m};
    soc_field_t             wrap_ptr[]={TDM_WRAP_PTRf , TDM_1_WRAP_PTRf};
    uint32                  lls_tdm_size=0;
    lls_port_tdm_entry_t    *lls_tdm=NULL;
    uint32                  iarb_size=0;
    iarb_tdm_table_entry_t  *iarb_tdm=NULL;
    int                     iter=0;
    uint32                  index;
    uint32                  rval;
    int                     cfg_num;

    SOC_IF_ERROR_RETURN(_soc_metrolite_get_cfg_num(unit, 0, &cfg_num));
    if (cfg_num >= (sizeof(bcm56270_tdm)/sizeof(bcm56270_tdm[0]))) {
        return SOC_E_FAIL;
    } 
    
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    old_tdm_no = soc_reg_field_get(unit, IARB_TDM_CONTROLr, rval, SELECT_TDMf);
    new_tdm_no = (old_tdm_no + 1)%2;

    iarb_size =  soc_mem_index_count(unit,IARB_TDM_TABLEm) *
                 sizeof(iarb_tdm_table_entry_t);
    iarb_tdm = soc_cm_salloc(unit,iarb_size, "iarb_tdm_table_entry_t");
    if (iarb_tdm == NULL) {
        return SOC_E_MEMORY;
    }            
    sal_memset(iarb_tdm,0,iarb_size);

    lls_tdm_size =  soc_mem_index_count(unit,LLS_PORT_TDMm) *
                    sizeof(lls_port_tdm_entry_t);
    lls_tdm = soc_cm_salloc(unit,lls_tdm_size, "lls_port_tdm_entry_t");
    if (lls_tdm == NULL) {
        soc_cm_sfree(unit,iarb_tdm);
        return SOC_E_MEMORY;
    }
    sal_memset(lls_tdm,0,lls_tdm_size);

    for (index = 0; index < new_tdm_size; index++) {
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm[index], PORT_NUMf,
                                        new_tdm[index]);
    }   
    soc_mem_write_range(unit, tdm_table[new_tdm_no], MEM_BLOCK_ALL, 0,
                        new_tdm_size, iarb_tdm);
    soc_cm_sfree(unit,iarb_tdm);        

    for (index = 0; index < new_tdm_size/2; index++) {
         soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                       PORT_ID_0f, new_tdm[index*2]);
         if ( new_tdm[index*2] < ML_IDLE ) {
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                            PORT_ID_0_ENABLEf, 1);
         } else {
              LOG_VERBOSE(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "IDLE SLOTs so PORT_ID_0_ENABLEf=0 \
                                       for index=%d\n"), index));
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                            PORT_ID_0_ENABLEf, 0);
         }

         soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                       PORT_ID_1f, new_tdm[(index*2) + 1]);
         if ( new_tdm[(index*2)+1] < ML_IDLE ) {
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                            PORT_ID_1_ENABLEf, 1);
         } else {
              LOG_VERBOSE(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "IDLE SLOTs so PORT_ID_1_ENABLEf=0 \
                                       for index=%d\n"), index));
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],
                                            PORT_ID_1_ENABLEf, 0);
         } 
    } /* for index */

    if (new_tdm_size % 2) {
        LOG_CLI((BSL_META_U(unit,           
                            "ODD TDM SIZE \n")));
        soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index*2],
                                      PORT_ID_0_ENABLEf, 1);
        soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index*2],
                                      PORT_ID_0f, new_tdm[(index*2)]);
        soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[(index*2)+1],
                                      PORT_ID_1_ENABLEf, 0);
        soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[(index*2)+1],
                                      PORT_ID_1f, 0);
        index++;
    }

    soc_mem_write_range(unit, LLS_PORT_TDMm, MEM_BLOCK_ALL,
                        new_tdm_no*256, (new_tdm_no*256) + index,
                        lls_tdm);
    soc_cm_sfree(unit,lls_tdm);

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, 
                      wrap_ptr[new_tdm_no],new_tdm_size -1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, SELECT_TDMf, new_tdm_no);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        kt2_tdm_display(unit,new_tdm,new_tdm_size,
                        bcm56270_tdm_info[cfg_num].row,
                        bcm56270_tdm_info[cfg_num].col);
    }

    SOC_IF_ERROR_RETURN(WRITE_LLS_TDM_CAL_CFG_SWITCHr(unit, 1));
    if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
        SOC_IF_ERROR_RETURN(READ_LLS_TDM_CAL_CFGr(unit, &rval));
        soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval,
                          CURRENT_CALENDARf,new_tdm_no);
        SOC_IF_ERROR_RETURN(WRITE_LLS_TDM_CAL_CFGr(unit, rval));
    }

    do {
        SOC_IF_ERROR_RETURN(READ_LLS_TDM_CAL_CFGr(unit, &rval));
        sal_udelay(100);
        if (soc_reg_field_get(unit, LLS_TDM_CAL_CFGr, rval,
                              CURRENT_CALENDARf) == new_tdm_no) {
            break;
        }
    } while (iter++ < 100000);

    if (iter >= 100000) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LLS Calendar switch failed !!\n")));
        return SOC_E_INTERNAL;
    }

    old_tdm_no = new_tdm_no;
    soc_metrolite_save_tdm_pos(unit, new_tdm_size,new_tdm);

    return SOC_E_NONE;
}
#endif

STATIC soc_error_t      
_soc_metrolite_mmu_tdm_init(int unit) 
{
    soc_error_t     rv = SOC_E_NONE;
    uint32 *arr=NULL;
    uint32  row = 0, col = 0;
#if 0
    uint32 offender_port=0;
#endif

    int i, tdm_size = 0,tdm_freq = 0, cfg_num;
    iarb_tdm_table_entry_t iarb_tdm;
    lls_port_tdm_entry_t lls_tdm;
    uint32 rval, arr_ele;
    uint16 dev_id;
    uint8 rev_id;
    uint32 port_enable_value=0;
    /*int loop_index, block;*/

    soc_cm_get_id(unit, &dev_id, &rev_id);
    SOC_IF_ERROR_RETURN(_soc_metrolite_get_cfg_num(unit, 0, &cfg_num));
    if (cfg_num >= (sizeof(bcm56270_tdm)/sizeof(bcm56270_tdm[0]))) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Wrong cfg_num:%d exceeding max cfg_num: %d\n"),
                   cfg_num,
                   (int)(sizeof(bcm56270_tdm)/sizeof(bcm56270_tdm[0]))));
        return SOC_E_FAIL;
    } 

    switch(dev_id) {
    case BCM56270_DEVICE_ID:
    case BCM56271_DEVICE_ID:
    case BCM56272_DEVICE_ID:

    case BCM53460_DEVICE_ID:
    case BCM53461_DEVICE_ID:

        tdm_size= bcm56270_tdm_info[cfg_num].tdm_size;
        tdm_freq= bcm56270_tdm_info[cfg_num].tdm_freq;
        row= bcm56270_tdm_info[cfg_num].row;
        col= bcm56270_tdm_info[cfg_num].col;
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Cfg=%d freq=%d size=%d raw=%d col=%d \n"),
                   cfg_num,tdm_freq,tdm_size,row,col));
        arr = bcm56270_tdm[cfg_num];

        switch(cfg_num) {
        /* ------------------- */
        /* 56270 Configuration */
        /* ------------------- */
        case BCM56270_DEVICE_ID_OFFSET_CFG + 1:
             /* 56270 */
             /* 4x 1G/2.5G + 4xXFI + 4x XFI/HGs11 + 2.5G LPBK */
             sal_memcpy(arr,
                        &ml_tdm_56270_A_ref[0],sizeof(ml_tdm_56270_A_ref));
             break;

        /* ------------------- */
        /* 56271 Configuration */
        /* ------------------- */
        case BCM56271_DEVICE_ID_OFFSET_CFG + 1:
             /* 56271 */
             /* 8x 1G/2.5G + 4xXFI + 2.5G LPBK */
             sal_memcpy(arr,
                        &ml_tdm_56271_C_ref[0],sizeof(ml_tdm_56271_C_ref));
             break;

        case BCM56271_DEVICE_ID_OFFSET_CFG + 2:
             /* 56271 */
             /* 2x1G/2.5G/10G + 4x1G/2.5G G.INT */
             sal_memcpy(arr,
                        &ml_tdm_56271_C_ref[0],sizeof(ml_tdm_56271_C_ref));

             if(soc_property_get(unit, spn_SAT_ENABLE, 0) == 0){
                 ml_tdm_replace(arr,tdm_size, 6, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 8, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 9, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 10, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 11, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 12, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 5, 9, 0);
                 ml_tdm_replace(arr,tdm_size, 7, 11, 0);
             }else{
                /* SAT port (port 5) has to be enabled when SAT is enabled */
                 ml_tdm_replace(arr,tdm_size, 6, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 8, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 9, ML_IDLE_1, 0);
                 ml_tdm_replace(arr,tdm_size, 5, 9, 0);
                 ml_tdm_replace(arr,tdm_size, 10, 5, 0);
                 ml_tdm_replace(arr,tdm_size, 11, 5, 0);
                 ml_tdm_replace(arr,tdm_size, 12, 5, 0);
                 ml_tdm_replace(arr,tdm_size, ML_IDLE_1, 5, 0);
                 ml_tdm_replace(arr,tdm_size, 7, 11, 0);

             }
             break;

        case BCM56271_DEVICE_ID_OFFSET_CFG + 3:
             /* 56271 */
             /* 4x1G/2.5G/10G + 4x1G/2.5G G.INT */
             sal_memcpy(arr,
                        &ml_tdm_56271_C_ref[0],sizeof(ml_tdm_56271_C_ref));
             if(soc_property_get(unit, spn_SAT_ENABLE, 0) == 0){
                 ml_tdm_replace(arr,tdm_size, 9, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 10, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 11, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 12, ML_IDLE, 0);
                 ml_tdm_replace(arr,tdm_size, 5, 9, 0);
                 ml_tdm_replace(arr,tdm_size, 6, 10, 0);
                 ml_tdm_replace(arr,tdm_size, 7, 11, 0);
                 ml_tdm_replace(arr,tdm_size, 8, 12, 0);
             }else {
                /* SAT port (port 5) has to be enabled when SAT is enabled */
                 ml_tdm_replace(arr,tdm_size, 9, ML_IDLE_1, 0);
                 ml_tdm_replace(arr,tdm_size, 10, ML_IDLE_1, 0);
                 ml_tdm_replace(arr,tdm_size, 11, ML_IDLE_1, 0);
                 ml_tdm_replace(arr,tdm_size, 12, ML_IDLE_1, 0);

                 ml_tdm_replace(arr,tdm_size, 5, 9, 0);
                 ml_tdm_replace(arr,tdm_size, 6, 10, 0);
                 ml_tdm_replace(arr,tdm_size, 7, 11, 0);
                 ml_tdm_replace(arr,tdm_size, 8, 12, 0);

                 ml_tdm_replace(arr,tdm_size, ML_IDLE_1, 5, 0);
                 ml_tdm_replace(arr,tdm_size, ML_IDLE_1, 5, 0);
                 ml_tdm_replace(arr,tdm_size, ML_IDLE_1, 5, 0);
                 ml_tdm_replace(arr,tdm_size, ML_IDLE_1, 5, 0);
             }
             break;


        /* ------------------- */
        /* 56272 Configuration */
        /* ------------------- */
        case BCM56272_DEVICE_ID_OFFSET_CFG + 1:
             /* 56272 */
             /* 8xGE + 2.5G LPBK */
             sal_memcpy(arr,
                        &ml_tdm_56272_B_ref[0],sizeof(ml_tdm_56272_B_ref));
             break;

        case BCM56272_DEVICE_ID_OFFSET_CFG + 2:
             /* 56272 */
             /* 2x1G + 4x1G G.INT */
             sal_memcpy(arr,
                        &ml_tdm_56272_B_ref[0],sizeof(ml_tdm_56272_B_ref));
             ml_tdm_replace(arr,tdm_size, 10, ML_IDLE, 0);
             ml_tdm_replace(arr,tdm_size, 12, ML_IDLE, 0);
             break;

#ifdef BCM_WARM_BOOT_SUPPORT
        case COUNTOF(bcm56270_tdm_info) - 1:
             if (SOC_WARM_BOOT(unit)) {
                 sal_memcpy(arr,
                         &ml_scache_tdm[0], sizeof(ml_scache_tdm));
                 break;
             }
#endif
        /* Fall thru */
        default:
             LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Invalid cfg_num\n")));
             return SOC_E_FAIL;
        }
        break;
    default:
             LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,  "Invalid DevId\n")));
             return SOC_E_FAIL;
    }

    /* Reconfigure TDM based on portgroup settings */
/* portgroup will be implemented later */
#if 0
    loop_index = 0;
    while(ml_selected_port_details[loop_index].start_port_no != 0) {
        block = loop_index;

        /* Change TDM only if the portgroup has been modified */
        if (ml_selected_port_details[loop_index].port_incr ==
                ml_selected_port_details_orig[loop_index].port_incr) {
            loop_index++;
            continue;
        }

        switch (ml_selected_port_details[loop_index].port_incr){
            case PORTGROUP_SINGLE_MODE:
                if(ml_selected_port_details_orig[loop_index].port_incr !=
                        PORTGROUP_QUAD_MODE) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    "Setting TDM to Single Mode\n")));
                    for(i=1; i<ML_MAX_PORTS_PER_BLOCK; i++) {
                        ml_tdm_replace(arr,tdm_size,
                                (*ml_block_ports[unit])[block][i],
                                (*ml_block_ports[unit])[block][0], 0);
                    }
                }
                break;
            case PORTGROUP_DUAL_MODE:
                /* If original is quad mode, then we already have enough
                 * TDM slots for dual mode as well */
                if(ml_selected_port_details_orig[loop_index].port_incr !=
                        PORTGROUP_QUAD_MODE) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "Setting TDM to Dual Mode\n")));
                    /* Convert single to Quad mode and then split */
                    ml_block_tdm_split(unit, arr, tdm_size, block);
                    for(i=1; i<ML_MAX_PORTS_PER_BLOCK; i+=2) {
                        ml_tdm_replace(arr,tdm_size,
                                (*ml_block_ports[unit])[block][i],
                                (*ml_block_ports[unit])[block][i-1], 0);
                    }
                }
                break;
            case PORTGROUP_QUAD_MODE:
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "Setting TDM to Quad Mode\n")));
                /* Merge the slots and then split */
                /* This will take care of dual to quad block */
                for(i=1; i<ML_MAX_PORTS_PER_BLOCK; i++) {
                    ml_tdm_replace(arr,tdm_size,
                            (*ml_block_ports[unit])[block][i],
                            (*ml_block_ports[unit])[block][0], 0);
                }
                ml_block_tdm_split(unit, arr, tdm_size, block);
                break;
            default:
                return SOC_E_INTERNAL;
        }
        loop_index++;
    }
#endif
    /* Verify the TDM structure */
     kt2_tdm_display(unit,arr, tdm_size,  row,  col);
#if 0
     rv = ml_tdm_verify(unit, (uint32 *)bcm56270_tdm[cfg_num], tdm_size,
                        tdm_freq, (uint32 *)bcm56270_speed[unit][cfg_num],
                        lpbk_port_speed, &offender_port);
    if ((rv == SOC_E_CONFIG) && (offender_port == ML_IDLE)) {
        rv = SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(rv);
#endif

    /* Disable IARB TDM before programming... */
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf,
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    for (i = 0; i < tdm_size; i++) {
        arr_ele = arr[i];
        if (arr_ele < ML_IDLE) { /* Non Idle Slots */
            port_enable_value = 1;
        } else {
            port_enable_value = 0;
        }

        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf,
                                        arr_ele);
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i,
                                                  &iarb_tdm));

        if((i % 2) == 0) {
            /* Two entries per mem entry */
            sal_memset(&lls_tdm, 0, sizeof(lls_port_tdm_entry_t));

            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, PORT_ID_0f, arr_ele);
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm,
                    PORT_ID_0_ENABLEf, port_enable_value);
        } else {
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, PORT_ID_1f, arr_ele);
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm,
                    PORT_ID_1_ENABLEf, port_enable_value);

            SOC_IF_ERROR_RETURN(WRITE_LLS_PORT_TDMm(unit, SOC_BLOCK_ALL, (i/2),
                        &lls_tdm));
        }
    }

    if (tdm_size % 2) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Info:Odd TDM Size%d \n"),tdm_size));
        SOC_IF_ERROR_RETURN(WRITE_LLS_PORT_TDMm(unit, SOC_BLOCK_ALL, (i/2),
                                                &lls_tdm));
    }

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf,
                      tdm_size -1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, IDLE_PORT_NUM_SELf, 1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval, END_Af, tdm_size - 1);
    soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval, END_Bf, tdm_size - 1);
    soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval, ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_TDM_CAL_CFGr(unit, rval));

    soc_metrolite_save_tdm_pos(unit, tdm_size, arr);

    return rv;
}

soc_error_t soc_metrolite_get_port_block(int unit,
                                         soc_port_t port, uint8 *block)
{
    /* Return invalid port for all except physical port */
    if (!((port > 0 ) && (port < 13))) {
        return SOC_E_PORT;
    }

    *block = (*ml_port_to_block[unit])[port - 1];

    return SOC_E_NONE;
}

int soc_metrolite_hw_reset_control_init(int unit) {
    uint32              rval;
    soc_timeout_t       to;
    int                 pipe_init_usec;

    /* Reset IPITE and EPIPE block */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 0x4000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r (unit, &rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x4000);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                     "ING_HW_RESET timeout\n")));
            break;
        }
    } while (TRUE);

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                     "EGR_HW_RESET timeout\n")));
            break;
        }
    } while (TRUE);

    SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 0);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval,
                        CMIC_REQ_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    return SOC_E_NONE;
}

soc_error_t soc_metrolite_get_mxq_phy_port_mode(
        int unit, soc_port_t port,int speed, bcmMxqPhyPortMode_t *mode) {

    soc_info_t  *si = &SOC_INFO(unit);

    if (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
        return SOC_E_PORT;
    }

    if(!IS_MXQ_PORT(unit, port)){
        return SOC_E_NONE;
    }

    if (speed <= 2500) {
        *mode=bcmMxqPhyPortModeQuad;
        si->port_num_lanes[port] = 1;
        return SOC_E_NONE;
    } else if (speed >= 10000) {
        *mode=bcmMxqPhyPortModeSingle;
        si->port_num_lanes[port] = 4;
        return SOC_E_NONE;
    } else {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Invalid speed for setting phy_port_mode\n")));
        return SOC_E_CONFIG;
    }

}   

soc_error_t soc_metrolite_get_xl_phy_core_port_mode(
        int unit, soc_port_t port, 
        bcmXlPhyPortMode_t *phy_mode,
        bcmXlCorePortMode_t *core_mode) {

    uint8   block=0;
    uint8   loop=0;
    uint8   port_used[ML_MAX_PORTS_PER_BLOCK]={0, 0, 0, 0};
    uint8   num_ports=0;
    int     cfg_num;
    char    *intf_type = NULL;
    uint32  rxaui = 0;
    soc_port_t  first_port;
    soc_info_t  *si = &SOC_INFO(unit);
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32 rval = 0, oamp_status = 0;
#endif

    if (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
        return SOC_E_PORT;
    }

    if(!IS_XL_PORT(unit, port)) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(_soc_metrolite_get_cfg_num(unit, 0, &cfg_num));
    if (cfg_num >= (sizeof(bcm56270_tdm)/sizeof(bcm56270_tdm[0]))) {
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN(soc_metrolite_get_port_block(unit,port,&block));
    first_port = (*ml_block_ports[unit])[block][0];

    for(loop=0;loop<ML_MAX_PORTS_PER_BLOCK; loop++) {

        if ((bcm56270_speed[unit][cfg_num]
                    [(block*ML_MAX_PORTS_PER_BLOCK)+loop]) &&
                (SOC_PBMP_MEMBER((PBMP_ALL(unit)), 
                                 (block*ML_MAX_PORTS_PER_BLOCK)+loop+1))){
            port_used[loop] = 1;
            num_ports++;
        }
#ifdef BCM_WARM_BOOT_SUPPORT
        SOC_IF_ERROR_RETURN(READ_OAMP_ENABLEr(unit, &rval));
        oamp_status = soc_reg_field_get(unit, OAMP_ENABLEr, rval, ENABLEf);

        if ((SOC_WARM_BOOT(unit) && (block == 1))) {
            if ((bcm56270_speed[unit][cfg_num]
                        [(block*ML_MAX_PORTS_PER_BLOCK)+loop] == 0) &&
                    (oamp_status == 1)){

                port_used[loop] = 1;
                num_ports++;
            }
        }
#endif
    }

    switch(num_ports){
        case 0: return SOC_E_PARAM;
        case 1: *phy_mode=bcmXlPhyPortModeSingle;
                *core_mode=bcmXlCorePortModeSingle;
                si->port_num_lanes[first_port] = 4;
                si->port_num_lanes[first_port + 1] = 0;
                si->port_num_lanes[first_port + 2] = 0;
                si->port_num_lanes[first_port + 3] = 0;
                break;
        case 2:
                *phy_mode=bcmXlPhyPortModeDual;
                *core_mode=bcmXlCorePortModeDual;
                intf_type= soc_property_port_get_str(unit, port, spn_SERDES_IF_TYPE);

                if (intf_type != NULL) {
                    if ((sal_strcmp(intf_type, "RXAUI") == 0) ||
                            (sal_strcmp(intf_type, "rxaui") == 0)) {
                        /* mode : RXAUI */
                        si->port_num_lanes[first_port] = 2;
                        si->port_num_lanes[first_port + 1] = 0;
                        si->port_num_lanes[first_port + 2] = 2;
                        si->port_num_lanes[first_port + 3] = 0;
                    } else if ((sal_strcmp(intf_type, "XFI") == 0) ||
                            (sal_strcmp(intf_type, "xfi") == 0)) {
                        /* mode : 2xXFI */
                        si->port_num_lanes[first_port] = 1;
                        si->port_num_lanes[first_port + 1] = 0;
                        si->port_num_lanes[first_port + 2] = 1;
                        si->port_num_lanes[first_port + 3] = 0;
                    }
                }

                rxaui =  soc_property_port_get(unit, port,
                        spn_SERDES_RXAUI_MODE, 0xFF);
                if ((rxaui == 0) || (rxaui == 1)) {
                    /* mode : RXAUI */
                    si->port_num_lanes[first_port] = 2;
                    si->port_num_lanes[first_port + 1] = 0;
                    si->port_num_lanes[first_port + 2] = 2;
                    si->port_num_lanes[first_port + 3] = 0;
                } else {
                    /* mode : 2xXFI */
                    si->port_num_lanes[first_port] = 1;
                    si->port_num_lanes[first_port + 1] = 0;
                    si->port_num_lanes[first_port + 2] = 1;
                    si->port_num_lanes[first_port + 3] = 0;
                }
                break;
        case 3: if(port_used[0] && port_used[2] && port_used[3]) {
                    *phy_mode = bcmXlPhyPortModeTri023;
                    *core_mode = bcmXlCorePortModeTri023;
                    si->port_num_lanes[first_port] = 2;
                    si->port_num_lanes[first_port + 1] = 0;
                    si->port_num_lanes[first_port + 2] = 1;
                    si->port_num_lanes[first_port + 3] = 1;
                } else if (port_used[0] && port_used[1] && port_used[2]) {
                    *phy_mode = bcmXlPhyPortModeTri012;
                    *core_mode = bcmXlCorePortModeTri012;
                    si->port_num_lanes[first_port] = 1;
                    si->port_num_lanes[first_port + 1] = 1;
                    si->port_num_lanes[first_port + 2] = 2;
                    si->port_num_lanes[first_port + 3] = 0;
                } else {
                    return SOC_E_PARAM; 
                } 
                break;
        case 4: *phy_mode=bcmXlPhyPortModeQuad;   
                *core_mode=bcmXlCorePortModeQuad;
                si->port_num_lanes[first_port] = 1;
                si->port_num_lanes[first_port + 1] = 1;
                si->port_num_lanes[first_port + 2] = 1;
                si->port_num_lanes[first_port + 3] = 1;
                break;
    }

    return SOC_E_NONE;
}



soc_error_t
soc_metrolite_port_init_config(int unit, int cfg_num, soc_port_t port) 
{
    uint32      rval, port_speed;
    bcmMxqPhyPortMode_t phy_mode = -1;
    bcmXlCorePortMode_t core_mode_xl = -1;
    bcmXlPhyPortMode_t  phy_mode_xl = -1;

    port_speed = bcm56270_speed[unit][cfg_num][port-1];
   
    if(IS_MXQ_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_metrolite_get_mxq_phy_port_mode(
                    unit, port, port_speed, &phy_mode));
        if(phy_mode == -1) {
            return SOC_E_FAIL;
        }
    }
    if (SOC_REG_PORT_VALID(unit, XPORT_MODE_REGr, port)) {
        SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
        soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, PHY_PORT_MODEf,      phy_mode);
        if(port_speed < 10000) {
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                    PORT_GMII_MII_ENABLEf, 1);
        } else {
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                    PORT_GMII_MII_ENABLEf, 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
    }

    if (SOC_REG_PORT_VALID(unit, XPORT_MIB_RESETr, port)) {
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit, port, 0xf));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit, port, 0x0));
    }

    if (SOC_REG_PORT_VALID(unit, XLPORT_CONFIGr, port)) {
        if (IS_ST_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(READ_XLPORT_CONFIGr(unit, port, &rval));
            soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
             if ((soc_feature(unit, soc_feature_higig2)) &&
                     (soc_property_port_get(unit, port,
                                            spn_HIGIG2_HDR_MODE, 0))) {
                soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG2_MODEf, 1);
             } else {
                soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG2_MODEf, 0);
             }
            soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, MY_MODIDf, 0);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_CONFIGr(unit, port, rval));
        } else {
            SOC_IF_ERROR_RETURN(READ_XLPORT_CONFIGr(unit, port, &rval));
            soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG_MODEf, 0);
            soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG2_MODEf, 0);
            soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, MY_MODIDf, 0);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_CONFIGr(unit, port, rval));
        }
    }

    if (SOC_REG_PORT_VALID(unit, XLPORT_MODE_REGr, port)) {
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, 0));
    }

    if (SOC_REG_PORT_VALID(unit, XLPORT_SOFT_RESETr, port)) {
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0xF));
    }

    if(IS_XL_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_metrolite_get_xl_phy_core_port_mode(unit, port,
                &phy_mode_xl, &core_mode_xl));
        if((phy_mode_xl == -1) || (core_mode_xl == -1)){
            return SOC_E_FAIL;
        }
    }
    if (SOC_REG_PORT_VALID(unit, XLPORT_MODE_REGr, port)) {
        rval = 0;
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                EGR_1588_TIMESTAMPING_CMIC_48_ENf, 1);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                XPORT0_CORE_PORT_MODEf,core_mode_xl);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                XPORT0_PHY_PORT_MODEf, phy_mode_xl);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));
    }

    if (SOC_REG_PORT_VALID(unit, XLPORT_SOFT_RESETr, port)) {
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0x0));
    }

    if (SOC_REG_PORT_VALID(unit, XLPORT_MIB_RESETr, port)) {
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0xf));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0x0));
    }

    if (SOC_REG_PORT_VALID(unit, XLPORT_ECC_CONTROLr, port)) {
        rval = 0;
        soc_reg_field_set(unit, XLPORT_ECC_CONTROLr, &rval,
                MIB_TSC_MEM_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_ECC_CONTROLr(unit, port, rval));
    }
    
    if (SOC_REG_PORT_VALID(unit, XLPORT_CNTMAXSIZEr, port)) {
        rval = 0;
        soc_reg_field_set(unit, XLPORT_CNTMAXSIZEr, &rval, CNTMAXSIZEf, 0x5F2);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_CNTMAXSIZEr(unit, port, rval));
    }

    if (SOC_REG_PORT_VALID(unit, X_GPORT_CNTMAXSIZEr, port)) {
        rval = 0;
        soc_reg_field_set(unit, X_GPORT_CNTMAXSIZEr, &rval, CNTMAXSIZEf, 0x5F2);
        SOC_IF_ERROR_RETURN(WRITE_X_GPORT_CNTMAXSIZEr(unit, port, rval));
    }

    if (SOC_REG_PORT_VALID(unit, XLPORT_FLOW_CONTROL_CONFIGr, port)) {
        /* enable flow control for port macro ports */
        SOC_IF_ERROR_RETURN(READ_XLPORT_FLOW_CONTROL_CONFIGr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_FLOW_CONTROL_CONFIGr, &rval,MERGE_MODE_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_FLOW_CONTROL_CONFIGr(unit, port, rval));
    }

    return SOC_E_NONE;
}

int
_ml_ledup_init(int unit, uint16 dev_id)
{
    uint32 rval = 0;
    int ix;
    int freq, cycles;
    int refresh_rate=30, refresh_period;
    uint8 remap_value[ML_MAX_PHYSICAL_PORTS];

    /* Construct CMIC_LEDUP0_DATA_RAM port remapping */
    for (ix = 0; ix < ML_MAX_PHYSICAL_PORTS; ix++) {
        remap_value[ix] = ML_MAX_PHYSICAL_PORTS - ix;
    }

    /* Apply CMIC_LEDUP0_DATA_RAM port remapping */
    ix = 0;
    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval,
                      REMAP_PORT_0f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval,
                      REMAP_PORT_1f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval,
                      REMAP_PORT_2f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval,
                      REMAP_PORT_3f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval,
                      REMAP_PORT_4f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval,
                      REMAP_PORT_5f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval,
                      REMAP_PORT_6f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval,
                      REMAP_PORT_7f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval,
                      REMAP_PORT_8f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval,
                      REMAP_PORT_9f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval,
                      REMAP_PORT_10f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval,
                      REMAP_PORT_11f, remap_value[ix]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit, rval));


    /* Initialize the UP0 data ram */
    rval = 0;
    for (ix = 0; ix < 256; ix++) {
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_DATA_RAMr(unit,ix, rval));
    }

    /* From Top spec, Ring 0 LED chain is connected to 28 ports
     * Ring 1 LED chain is connected to iProc AMAC0/1 ports
     * Ring 2 is not connected.
     */

    /* Apply LEDUP delays. */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_LEDUP0_CTRLr(unit, &rval));
    soc_reg_field_set(unit,CMIC_LEDUP0_CTRLr,
                      &rval, LEDUP_SCAN_START_DELAYf, 0x6);
    soc_reg_field_set(unit,CMIC_LEDUP0_CTRLr,
                      &rval, LEDUP_SCAN_INTRA_PORT_DELAYf, 0x5);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_CTRLr(unit, rval));

    /* Program the LED clock output frequency based on core clock */
    freq = SOC_INFO(unit).frequency;

    /* For LEDCLK_HALF_PERIOD */
    /* Formula : cycles = (freq * 10^6) * (100 * 10^-9) = freq/10 */
    /* Round up the value for non-integer clocks */
    cycles = (freq + 9) / 10 - 1;
    if (cycles < 2) {
        cycles = 2;
    }

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_CLK_DIVr, &rval,
            LEDCLK_HALF_PERIODf, cycles);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_CLK_DIVr(unit, rval));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_CLK_DIVr(unit, rval));

    /* For REFRESH_CYCLE_PERIOD */
    /* Formula : refresh_period = (freq * 10^6) / 30 */
    refresh_period = freq * 1000000 / refresh_rate;

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_CLK_PARAMSr, &rval,
            REFRESH_CYCLE_PERIODf, refresh_period);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_CLK_PARAMSr(unit, rval));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_CLK_PARAMSr(unit, rval));

    return SOC_E_NONE;
}

int _soc_metrolite_misc_init(int unit)
{
    soc_info_t *si;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS], ing_entry[SOC_MAX_MEM_WORDS];
    soc_pbmp_t pbmp;
    int port;
    int blk;
    uint64 reg64;
    uint16 dev_id;
    uint8 rev_id;
    soc_field_t fields[3];
    uint32 values[3];
    int cfg_num;
    int rv;
    int parity_enable;
    int my_modid_list[4] = { 0, 0, 0, 0 };
    int my_modid_valid[4] = { 1, 0, 0, 0 };
    int my_modid_port_base_ptr[4] = { 0, 0, 0, 0 };
    soc_cm_get_id(unit, &dev_id, &rev_id);

    si = &SOC_INFO(unit);

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_MXQPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        /* Bring XMAC out of reset */
        if (SOC_IS_METROLITE(unit)) {
            SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit,port,&rval));
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, PORT_GMII_MII_ENABLEf,
                    0x1);
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, PHY_PORT_MODEf,
                    0x2);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit,port,rval));
        }
    }

    /* saber2 related hw reset control */
    if(!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_soc_saber2_hw_reset_control_init(unit));
    }

    /* core clock info in MHz updated from PRD
     * Do not modify these values until unless stated in PRD*/
    switch(dev_id) {
      case BCM56270_DEVICE_ID:
        si->frequency = 180;
        break;
      case BCM56271_DEVICE_ID:
        si->frequency = 125;
        break;
      case BCM56272_DEVICE_ID:
        si->frequency = 30;
        break;
    }

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, FALSE);

    /* WRED Configuration */
    SOC_IF_ERROR_RETURN(READ_WRED_MISCCONFIGr(unit, &rval));
    switch(dev_id) {
      case BCM56270_DEVICE_ID:
        /* Refer the formula in the Reg file
           +       * For Metrolite 56270, frequency is 180MHZ
           +       * Calculation  is
           +       * ((512 + 128)/2 * (0.00555)) + 1 = 2.778
           +       * So we are programming 3 */
        soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, BASE_UPDATE_INTERVALf, 3);
        break;
      case BCM56271_DEVICE_ID:
        /* Refer the formula in the Reg file
           +       * For Metrolite 56271, frequency is 125MHZ
           +       * Calculation  is
           +       * ((512 + 128)/2 * (0.008)) + 1 = 3.56
           +       * So we are programming 4 */
        soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, BASE_UPDATE_INTERVALf, 4);
        break;
      case BCM56272_DEVICE_ID:
        /* Refer the formula in the Reg file
           +       * For Metrolite 56272, frequency is 30MHZ
           +       * Calculation  is
           +       * ((512 + 128)/2 * (0.0333)) + 1 = 11.667
           +       * So we are programming 12 */
        soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, BASE_UPDATE_INTERVALf, 12);
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_WRED_MISCCONFIGr(unit, rval));

    soc_saber2_ser_init(unit, parity_enable ? TRUE : FALSE);

       /* initialize LED UP0 */
        SOC_IF_ERROR_RETURN(_ml_ledup_init(unit, dev_id));

    /* Some registers are implemented in memory, need to clear them in order
     * to have correct parity value */
    PBMP_PP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, 0));
        SOC_IF_ERROR_RETURN(WRITE_EGR_IPMC_CFG2r(unit, port, 0));
    }

    SOC_IF_ERROR_RETURN(
        bcm_kt2_modid_set_all(unit, my_modid_list, my_modid_valid,
                                                    my_modid_port_base_ptr));

    sal_memset(entry, 0, sizeof(cpu_pbm_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBMm, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBMm, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(cpu_pbm_2_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBM_2m, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBM_2m, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(multipass_loopback_bitmap_entry_t));
    soc_mem_pbmp_field_set(unit, MULTIPASS_LOOPBACK_BITMAPm, entry, BITMAPf,
                           &PBMP_LB(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, MULTIPASS_LOOPBACK_BITMAPm, MEM_BLOCK_ALL, 0,
                       entry));

    sal_memset(entry, 0, sizeof(egr_physical_port_entry_t));
    sal_memset(ing_entry, 0, sizeof(ing_physical_port_table_entry_t));

    soc_mem_field32_set(unit, EGR_PHYSICAL_PORTm, entry, PORT_TYPEf, 1);
    soc_mem_field32_set(unit, ING_PHYSICAL_PORT_TABLEm,
            ing_entry, PORT_TYPEf, 1);

    /* Higig ports */
    PBMP_ST_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(
                            unit, IECELL_CONFIGr, port, HIGIG_MODEf, 1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(
                            unit, XLPORT_CONFIGr, port, HIGIG_MODEf, 1));
        if (IS_HL_PORT(unit,port) ||
            soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE, 0)) {
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(
                unit,IECELL_CONFIGr, port, HIGIG2_MODEf, 1));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(
                unit, XLPORT_CONFIGr, port, HIGIG2_MODEf, 1));
            soc_mem_field32_set(unit, ING_PHYSICAL_PORT_TABLEm,ing_entry,
                                HIGIG2f,1);
            soc_mem_field32_set(unit, EGR_PHYSICAL_PORTm,entry,
                                HIGIG2f,1);
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(
                unit, IECELL_CONFIGr, port, HIGIG2_MODEf, 0));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(
                unit, XLPORT_CONFIGr, port, HIGIG2_MODEf, 0));
            soc_mem_field32_set(unit, ING_PHYSICAL_PORT_TABLEm,ing_entry,
                                HIGIG2f,0);
            soc_mem_field32_set(unit, EGR_PHYSICAL_PORTm,entry,
                                HIGIG2f,0);
        }
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_PHYSICAL_PORTm,
            MEM_BLOCK_ALL, port, entry));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_PHYSICAL_PORT_TABLEm,
            MEM_BLOCK_ALL, port, ing_entry));
    }
    
    /* CMIC higig index */
    /* Setup the cpu_hg_port specifically in HG2 mode */
    soc_mem_field32_set(unit, ING_PHYSICAL_PORT_TABLEm, ing_entry,
                        HIGIG2f, 1);
    soc_mem_field32_set(unit, EGR_PHYSICAL_PORTm,entry,
                        HIGIG2f, 1);
    SOC_IF_ERROR_RETURN(soc_mem_write( 
        unit, EGR_PHYSICAL_PORTm, MEM_BLOCK_ALL, si->cpu_hg_index, entry));
    SOC_IF_ERROR_RETURN(soc_mem_write(
        unit, ING_PHYSICAL_PORT_TABLEm, MEM_BLOCK_ALL,
        si->cpu_hg_index, ing_entry));
    
    /* Loopback port */
    soc_mem_field32_set(unit, EGR_PHYSICAL_PORTm, entry, PORT_TYPEf, 2);
    soc_mem_field32_set(unit, ING_PHYSICAL_PORT_TABLEm, ing_entry,
        PORT_TYPEf, 2);
    SOC_IF_ERROR_RETURN(soc_mem_write(
        unit, EGR_PHYSICAL_PORTm, MEM_BLOCK_ALL, si->lb_port, entry));
    SOC_IF_ERROR_RETURN(soc_mem_write(
        unit, ING_PHYSICAL_PORT_TABLEm,
        MEM_BLOCK_ALL, si->lb_port, ing_entry));
    /* ingress physical port to pp_port mapping */
    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(
        unit, ING_PHYSICAL_PORT_TABLEm,
        si->lb_port, PP_PORTf, si->lb_port)); 

    /* pp_port to egress physical port mapping */
    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(
        unit, PP_PORT_TO_PHYSICAL_PORT_MAPm,
        si->lb_port, DESTINATIONf,si->lb_port));

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    /* Enable dual hash on L2, L3 and MPLS_ENTRY tables */
    fields[0] = ENABLEf;
    values[0] = 1;
    fields[1] = HASH_SELECTf;
    values[1] = FB_HASH_CRC32_LOWER;
    fields[2] = INSERT_LEAST_FULL_HALFf;
    values[2] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L2_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L3_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MPLS_ENTRY_HASH_CONTROLr, REG_PORT_ANY,
                                INSERT_LEAST_FULL_HALFf, 1));
    /*
     * IPMC init
     */
    SOC_IF_ERROR_RETURN(READ_RQE_GLOBAL_CONFIGr(unit, &rval));
    soc_reg_field_set(unit,RQE_GLOBAL_CONFIGr, &rval, PARITY_GEN_ENf,
                      1);
    soc_reg_field_set(unit,RQE_GLOBAL_CONFIGr, &rval, PARITY_CHK_ENf,
                      1);
    SOC_IF_ERROR_RETURN(WRITE_RQE_GLOBAL_CONFIGr(unit, rval));

    /* Egress Disable */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 0);
    for (port = 0; port <= si->lb_port; port++) {
        rv = WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, entry);
        /* ****************************************************************** */
        
        /* ****************************************************************** */
        if (SOC_FAILURE(rv)) {
            if (rv == _SHR_E_PARAM) {
                continue;
            }
            return rv;
        }
    }

#if 0    
    SOC_IF_ERROR_RETURN(READ_DEQ_EFIFO_CFG_COMPLETEr(unit, &rval));
    soc_reg_field_set(unit, DEQ_EFIFO_CFG_COMPLETEr, &rval, \
            EGRESS_FIFO_CONFIGURATION_COMPLETEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFG_COMPLETEr(unit, rval));
#endif
    
    sal_memset(entry, 0, sizeof(epc_link_bmap_entry_t));
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                           &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, entry)); 

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_VALIDATION_ENf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));
    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, VT_MISS_UNTAGf, 0);

    /* Enable pri/cfi remarking on egress ports. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                      1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm, &entry, BITMAPf,
                           &pbmp);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm,
                                      MEM_BLOCK_ANY, 0, &entry));
    SOC_IF_ERROR_RETURN(READ_CMIC_TXBUF_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_TXBUF_CONFIGr, &rval,
                            FIRST_SERVE_BUFFERS_WITH_EOP_CELLSf, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_TXBUF_CONFIGr(unit, rval));

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT)));
#if 0
    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "unit %d : MSPI Init Failed\n"), unit));
    }
#endif
    SOC_IF_ERROR_RETURN(soc_mem_clear(
                        unit, MMU_RQE_QUEUE_OP_NODE_MAPm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN(soc_mem_clear(
                        unit, MMU_WRED_QUEUE_OP_NODE_MAPm, COPYNO_ALL, TRUE));
    
    SOC_IF_ERROR_RETURN(_soc_metrolite_get_cfg_num(unit, 0, &cfg_num));
    if (cfg_num >= (sizeof(bcm56270_tdm)/sizeof(bcm56270_tdm[0]))) {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Wrong cfg_num:%d exceeding max cfg_num: %d\n"),
                   cfg_num,
                   (int)(sizeof(bcm56270_tdm)/sizeof(bcm56270_tdm[0]))));
        return SOC_E_FAIL;
    }

    PBMP_ALL_ITER(unit, port) { 
        if ((port == 0 ) || (port >= 13)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_metrolite_port_init_config(unit, cfg_num, port));
    }
    
    SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, AUX_ARB_CONTROL_2r, &rval,
            TCAM_ATOMIC_WRITE_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROL_2r(unit, rval));

    /* Setup SW2_FP_DST_ACTION_CONTROL */
    fields[0] = HGTRUNK_RES_ENf;
    values[0] = 1;
    fields[1] = LAG_RES_ENf;
    values[1] = 1;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit,
                SW2_FP_DST_ACTION_CONTROLr, REG_PORT_ANY, 2, fields, values));

    /* In SB2, hardware logic for key/mask encoding and decoding is disabled */
    SOC_IF_ERROR_RETURN(READ_EGR_BYPASS_CTRLr(unit, &rval));
    soc_reg_field_set(unit, EGR_BYPASS_CTRLr, &rval,
            EFP_SW_ENC_DEC_TCAMf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_BYPASS_CTRLr(unit, rval));
#if 0    
#ifdef INCLUDE_AVS
    soc_saber2_avs_init(unit);
#endif /* INCLUDE_AVS  */

    /* initializing ts pll and bs pll */
    _soc_saber2_tspll_bspll_config(unit);
#endif

    /* Initializing subport configurations */
    si->subport_group_max = SOC_ML_SUBPORT_GROUP_MAX;
    si->subport_port_max = SOC_ML_MAX_SUBPORTS;
    si->pp_port_index_min = SOC_ML_SUBPORT_PP_PORT_INDEX_MIN;
    si->pp_port_index_max = SOC_ML_SUBPORT_PP_PORT_INDEX_MAX;
    si->lp_tx_databuf_start_addr_max = SOC_ML_LINKPHY_TX_DATA_BUF_START_ADDR_MAX;
    si->lp_tx_databuf_end_addr_min = SOC_ML_LINKPHY_TX_DATA_BUF_END_ADDR_MIN;
    si->lp_tx_databuf_end_addr_max = SOC_ML_LINKPHY_TX_DATA_BUF_END_ADDR_MAX;
    si->lp_tx_stream_start_addr_offset = SOC_ML_LINKPHY_TX_STREAM_START_ADDR_OFFSET;
    si->lp_block_max = SOC_ML_MAX_LINKPHY_BLOCK;
    si->lp_ports_max = SOC_ML_MAX_LINKPHY_PORTS;
    si->lp_streams_per_slice_max = SOC_ML_MAX_LINKPHY_STREAMS_PER_SLICE;
    si->lp_streams_per_port_max = SOC_ML_MAX_LINKPHY_STREAMS_PER_PORT;
    si->lp_streams_per_subport = SOC_ML_MAX_LINKPHY_STREAMS_PER_SUBPORT;

    return SOC_E_NONE;
}

/* *************************************************************** */
/* METROLITE MMU HELPER  --- START(As per Metrolie_MMU_Settings-5.xls */
/* *************************************************************** */
/* Dynamic/SetLater Parameter */
typedef struct _soc_ml_mmu_params {
               uint32 mmu_min_pkt_size;                   /*C5*/
               uint32 mmu_ethernet_mtu_bytes;             /*C6*/
               uint32 mmu_max_pkt_size;                   /*C7*/
               uint32 mmu_jumbo_frame_size;               /*C8*/
               uint32 mmu_int_buf_cell_size;              /*C9*/
               uint32 mmu_pkt_header_size;                /*C10*/
               uint32 lossless_mode_d_c;                  /*C11*/
               uint32 pfc_pause_mode_d_c;                 /*C12*/
               uint32 mmu_lossless_pg_num;                /*C13*/
               uint32 num_ge_ports_d;                     /*C14*/
               uint32 mmu_line_rate_ge;                   /*C15*/
               uint32 num_ge_int_ports_d;                 /*C14*/
               uint32 num_egr_queue_per_int_ge_port_d;    /*C16*/
               uint32 num_hg_ports_d;                     /*C17*/
               uint32 mmu_line_rate_hg;                   /*C18*/
               uint32 num_hg_int_ports_d;                 /*C17*/
               uint32 num_egr_queue_per_int_hg_port_d;    /*C19 */
               uint32 mmu_num_cpu_port;                   /*C20*/
               uint32 mmu_num_cpu_queue;                  /*C21*/
               uint32 mmu_num_loopback_port;              /*C22*/
               uint32 mmu_num_loopback_queue;             /*C35*/
               uint32 mmu_num_ep_redirection_queue;       /*C36*/
               uint32 mmu_num_olp_port_d;                 /*C37*/
               uint32 mmu_num_olp_queue;                  /*C38*/
               uint32 olp_port_int_ext_bounding_d_c;      /*C39:Dynamic*/
               uint32 mmu_total_egress_queue;             /*C40*/
               uint32 mmu_pipeline_lat_cpap_admission;    /*C41*/
               uint32 mmu_int_buf_size;                   /*C42:2048KB=2MB*/
               uint32 mmu_available_int_buf_size_d;       /*C43:1434KB */
               uint32 mmu_reserved_int_buf_cells;         /*C44:100 Cells */
               uint32 mmu_reserved_int_buf_ema_pool_size_d; /*C45:614KB */
               uint32 internal_buffer_reduction_d_c;        /* C46:Dynamic */
               uint32 mmu_ext_buf_size;                   /*C47:737280KB=720MB*/
               uint32 mmu_reserved_ext_buf_space_cfap;    /*C48:737280KB=720MB*/
               uint32 mmu_egress_queue_entries;           /*C49*/
               uint32 mmu_ep_redirect_queue_entries;      /*C50*/
               uint32 mmu_exp_num_of_repl_per_pkt;        /*C51*/
               uint32 mmu_repl_engine_work_queue_entries; /*C52*/
               uint32 mmu_resv_repl_engine_work_queue_entries; /*C53*/
               uint32 mmu_repl_engine_work_queue_in_device;/*C54*/
               uint32 mmu_ema_queues;                     /*C55*/
               uint32 num_cells_rsrvd_ing_ext_buf;        /* C56:Dynamic */
               uint32 per_cos_res_cells_for_int_buff_d;   /* C58:Dynamic */
               uint32 per_cos_res_cells_for_ext_buff_d;   /* C59:Dynamic */
               uint32 mmu_ing_port_dyn_thd_param;         /*C60*/
               uint32 mmu_ing_pg_dyn_thd_param;           /*C61*/
               uint32 mmu_egr_queue_dyn_thd_param_baf;        /*C62*/
               uint32 mmu_egr_queue_dyn_thd_param_baf_profile_lossy;           /*C63*/
               uint32 mmu_ing_cell_buf_reduction;         /*C64 */
               uint32 mmu_ing_pkt_buf_reduction;          /*C65 */
} _soc_ml_mmu_params_t;

typedef struct _general_info {
    uint32 max_packet_size_in_cells;                         /* C72 */
    uint32 jumbo_frame_for_int_buff;                         /* C73 */
    uint32 jumbo_frame_for_ext_buff;                         /* C74 */
    uint32 ether_mtu_cells_for_int_buff;                     /* C76 */
    uint32 ether_mtu_cells_for_ext_buff;                     /* C77 */
    uint32 total_num_of_ports;                               /* C79 */
    uint32 total_num_of_ports_excl_lpbk;                     /* C80 */
    uint32 total_num_of_ports_excl_lpbk_olp;                 /* C81 */
    uint32 total_num_of_ports_excl_lpbk_olp_cpu;             /* C82 */
    uint32 port_bw_bound_to_ext_buff;                        /* C83 */
    uint32 total_egr_queues_for_a_int_ge_ports;              /* C84 */
    uint32 total_egr_queues_for_a_ext_ge_ports;              /* C85 */
    uint32 total_egr_queues_for_a_int_hg_ports;              /* C86 */
    uint32 total_egr_queues_for_a_ext_hg_ports;              /* C87 */
    uint32 total_cpu_queues;                                 /* C88 */
    uint32 total_base_queue_int_buff;                       /* C89 */
    uint32 total_base_queue_ext_buff;                        /* C90 */
    uint32 total_ema_queues;                                 /* C91 */
    uint32 total_egr_base_queues_in_device;                  /* C93 */
    uint32 total_egr_queues_in_device;                       /* C94 */
    uint32 max_int_cell_buff_size;                           /* C95 */
    uint32 int_cell_buff_size_after_limitation;              /* C96 */
    uint32 src_packing_fifo;                                 /* C97 */
    uint32 int_buff_pool;                                    /* C98 */
    uint32 ema_pool;                                         /* C99 */
    uint32 max_ext_cell_buff_size;                           /* C100 */
    uint32 repl_engine_work_queue_entries;                   /* C101 */
    uint32 ratio_of_ext_buff_to_int_buff_size;               /* C103 */
    uint32 int_buff_cells_per_avg_size_pkt;                  /* C104 */
    uint32 ext_buff_cells_per_avg_size_pkt;                  /* C105 */
    uint32 max_prop_of_buff_used_by_one_queue_port;          /* C106 */
}_general_info_t;

typedef struct _input_port_threshold_t {
    uint32 global_hdrm_cells_for_int_buff_ingress_pool;      /* C110 */
    uint32 global_hdrm_cells_for_int_buff_ema_pool;          /* C111 */
    uint32 global_hdrm_cells_for_ext_buff_pool;              /* C112 */
    uint32 global_hdrm_cells_for_RE_WQEs;                    /* C113 */
    uint32 global_hdrm_cells_for_EQEs;                       /* C114 */

    uint32 hdrm_int_buff_cells_for_10G_PG;                   /* C115 */
    uint32 hdrm_ema_buff_cells_for_10G_PG;                   /* C116 */
    uint32 hdrm_ext_buff_cells_for_10G_PG;                   /* C117 */
    uint32 hdrm_RE_WQEs_pkts_for_10G_PG;                     /* C118 */
    uint32 hdrm_EQEs_pkts_for_10G_PG;                        /* C119 */

    uint32 hdrm_int_buff_cells_for_10G_total_PG;             /* C120 */
    uint32 hdrm_ema_buff_cells_for_10G_total_PG;             /* C121 */
    uint32 hdrm_ext_buff_cells_for_10G_total_PG;             /* C122 */
    uint32 hdrm_RE_WQEs_pkts_for_10G_total_PG;               /* C123 */
    uint32 hdrm_EQEs_pkts_for_10G_total_PG;                  /* C124 */

    uint32 hdrm_int_buff_cells_for_1G_PG;                    /* C125 */
    uint32 hdrm_ema_buff_cells_for_1G_PG;                    /* C126 */
    uint32 hdrm_ext_buff_cells_for_1G_PG;                    /* C127 */
    uint32 hdrm_RE_WQEs_pkts_for_1G_PG;                      /* C128 */
    uint32 hdrm_EQEs_pkts_for_1G_PG;                         /* C129 */

    uint32 hdrm_int_buff_cells_for_1G_total_PG;              /* C130 */
    uint32 hdrm_ema_buff_cells_for_1G_total_PG;              /* C131 */
    uint32 hdrm_ext_buff_cells_for_1G_total_PG;              /* C132 */
    uint32 hdrm_RE_WQEs_pkts_for_1G_total_PG;                /* C133 */
    uint32 hdrm_EQEs_pkts_for_1G_total_PG;                   /* C134 */

    uint32 hdrm_int_buff_cells_for_olp_port;                 /* C135 */
    uint32 hdrm_ema_buff_cells_for_olp_port;                 /* C136 */
    uint32 hdrm_ext_buff_cells_for_olp_port;                 /* C137 */
    uint32 hdrm_RE_WQEs_pkts_for_olp_port;                   /* C138 */
    uint32 hdrm_EQEs_pkts_for_olp_port;                      /* C139 */

    uint32 hdrm_int_buff_cells_for_lpbk_port;                /* C140 */
    uint32 hdrm_ema_buff_cells_for_lpbk_port;                /* C141 */
    uint32 hdrm_ext_buff_cells_for_lpbk_port;                /* C142 */
    uint32 hdrm_RE_WQEs_pkts_for_lpbk_port;                  /* C143 */
    uint32 hdrm_EQEs_pkts_for_lpbk_port;                     /* C144 */

    uint32 hdrm_int_buff_cells_for_cpu_port;                 /* C145 */
    uint32 hdrm_ema_buff_cells_for_cpu_port;                 /* C146 */
    uint32 hdrm_ext_buff_cells_for_cpu_port;                 /* C147 */
    uint32 hdrm_RE_WQEs_pkts_for_cpu_port;                   /* C148 */
    uint32 hdrm_EQEs_pkts_for_cpu_port;                      /* C149 */

    uint32 total_hdrm_int_buff_cells;                        /* C150 */
    uint32 total_hdrm_int_buff_ema_pool_cells;               /* C151 */
    uint32 total_hdrm_ext_buff_cells;                        /* C152 */
    uint32 total_hdrm_RE_WQEs_pkts;                          /* C153 */
    uint32 total_hdrm_EQEs_pkts;                             /* C154 */

    uint32 min_int_buff_cells_per_PG;                        /* C156 */
    uint32 min_int_buff_ema_pool_cells_per_PG;               /* C157 */
    uint32 min_ext_buff_cells_per_PG;                        /* C158 */
    uint32 min_RE_WQEs_pkt_per_PG;                           /* C159 */
    uint32 min_EQEs_pkt_per_PG;                              /* C160 */

    uint32 min_int_buff_cells_for_total_PG;                  /* C162 */
    uint32 min_int_buff_ema_pool_cells_for_total_PG;         /* C163 */
    uint32 min_ext_buff_cells_for_total_PG;                  /* C164 */
    uint32 min_RE_WQEs_pkts_for_total_PG;                    /* C165 */
    uint32 min_EQEs_pkts_for_total_PG;                       /* C166 */

    uint32 min_int_buff_cells_for_a_port;                    /* C168 */
    uint32 min_int_buff_ema_pool_cells_for_a_port;           /* C169 */
    uint32 min_ext_buff_cells_for_a_port;                    /* C170 */
    uint32 min_RE_WQEs_pkts_for_a_port;                      /* C171 */
    uint32 min_EQEs_pkts_for_a_port;                         /* C172 */

    uint32 min_int_buff_cells_for_total_port;                /* C174 */
    uint32 min_int_buff_ema_pool_cells_for_total_port;       /* C175 */
    uint32 min_ext_buff_cells_for_total_port;                /* C176 */
    uint32 min_RE_WQEs_pkts_for_total_port;                  /* C177 */
    uint32 min_EQEs_pkts_for_total_port;                     /* C178 */

    uint32 total_min_int_buff_cells;                         /* C180 */
    uint32 total_min_int_buff_ema_pool_cells;                /* C181 */
    uint32 total_min_ext_buff_cells;                         /* C182 */
    uint32 total_min_RE_WQEs_pkts;                           /* C183 */
    uint32 total_min_EQEs_pkts;                              /* C184 */

    uint32 total_shared_ing_buff_pool;                       /* C186 */
    uint32 total_shared_EMA_buff;                            /* C187 */
    uint32 total_shared_ext_buff;                            /* C188 */
    uint32 total_shared_RE_WQEs_buff;                        /* C189 */
    uint32 total_shared_EQEs_buff;                           /* C190 */

    uint32 ingress_burst_cells_size_for_one_port;            /* C192 */
    uint32 ingress_burst_pkts_size_for_one_port;             /* C193 */
    uint32 ingress_burst_cells_size_for_all_ports;           /* C194 */
    uint32 ingress_total_shared_cells_use_for_all_port;      /* C195 */
    uint32 ingress_burst_pkts_size_for_all_port;             /* C196 */
    uint32 ingress_total_shared_pkts_use_for_all_port;       /* C197 */
    uint32 ingress_total_shared_hdrm_cells_use_for_all_port; /* C198 */
    uint32 ingress_total_shared_hdrm_pkts_use_for_all_port;  /* C199 */

}_input_port_threshold_t;

typedef struct _output_port_threshold_t {
    uint32 min_grntd_res_queue_cells_int_buff;               /* C204 */
    uint32 min_grntd_res_queue_cells_ext_buff;               /* C205 */
    uint32 min_grntd_res_queue_cells_EQEs;                   /* C206 */
    uint32 min_grntd_res_EMA_queue_cells;                    /* C207 */
    uint32 min_grntd_res_RE_WQs_cells;                       /* C208 */
    uint32 min_grntd_res_RE_WQs_queue_cells_for_int_buff;    /* C209 */
    uint32 min_grntd_res_RE_WQs_queue_cells_for_ext_buff;    /* C210 */
    uint32 min_grntd_res_EP_redirect_queue_entry_cells;      /* C211 */

    uint32 min_grntd_tot_res_queue_cells_int_buff;           /* C213 */
    uint32 min_grntd_tot_res_queue_cells_ext_buff;           /* C214 */
    uint32 min_grntd_tot_res_queue_cells_EQEs;               /* C215 */
    uint32 min_grntd_tot_res_EMA_queue_cells;                /* C216 */
    uint32 min_grntd_tot_res_RE_WQs_cells;                   /* C217 */
    uint32 min_grntd_tot_res_RE_WQs_queue_cells_for_int_buff;/* C218 */
    uint32 min_grntd_tot_res_RE_WQs_queue_cells_for_ext_buff;/* C219 */
    uint32 min_grntd_tot_res_EP_redirect_queue_entry_cells;  /* C220 */

    uint32 min_grntd_tot_shr_queue_cells_int_buff;           /* C222 */
    uint32 min_grntd_tot_shr_queue_cells_ext_buff;           /* C223 */
    uint32 min_grntd_tot_shr_queue_cells_EQEs;               /* C224 */
    uint32 min_grntd_tot_shr_EMA_queue_cells;                /* C225 */
    uint32 min_grntd_tot_shr_RE_WQs_cells;                   /* C226 */
    uint32 min_grntd_tot_shr_RE_WQs_queue_cells_for_int_buff;/* C227 */
    uint32 min_grntd_tot_shr_RE_WQs_queue_cells_for_ext_buff;/* C228 */
    uint32 min_grntd_tot_shr_EP_redirect_queue_entry_cells;  /* C229 */

    uint32 egress_queue_dynamic_threshold_parameter;         /* C230 */
    uint32 egress_burst_cells_size_for_one_queue;            /* C231 */
    uint32 egress_burst_pkts_size_for_one_queue;             /* C232 */
    uint32 egress_burst_cells_size_for_all_ports;            /* C233 */
    uint32 egress_burst_pkts_size_for_all_ports;             /* C234 */
    uint32 egress_burst_cells_size_for_all_queues;           /* C235 */
    uint32 egress_burst_pkts_size_for_all_queues;            /* C236 */
    uint32 egress_total_use_in_cells_for_all_queues;         /* C237 */
    uint32 egress_total_use_in_pkts_for_all_queues;          /* C238 */
    uint32 egress_remaining_cells_for_all_queues;            /* C239 */
    uint32 egress_remaining_pkts_for_all_queues;             /* C240 */

}_output_port_threshold_t;

typedef struct _soc_ml_mmu_intermediate_results {
    _general_info_t           general_info;
    _input_port_threshold_t   input_port_threshold;
    _output_port_threshold_t  output_port_threshold;
}_soc_ml_mmu_intermediate_results_t;

_soc_ml_mmu_intermediate_results_t _soc_ml_mmu_intermediate_results={{0}};

void soc_metrolite_block_reset(int unit, uint8 block,int active_low) {
    uint32 rval;
    soc_field_t hot_swap_reset_fld[]={
        TOP_MXQ_HOTSWAP_RST_Lf
        };

    if (READ_TOP_SOFT_RESET_REGr(unit, &rval) != SOC_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
                        "ATTN: Reading TOP_SOFT_RESET_REG failed \n")));
        return;
    }
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval,
            hot_swap_reset_fld[block],active_low);
    /* coverity[check_return] */
    WRITE_TOP_SOFT_RESET_REGr(unit, rval);
}

STATIC
int _soc_ml_mmu_config_extra_queue(int unit, uint32 queue,
                          _soc_ml_mmu_params_t *_soc_ml_mmu_params,
                          _output_port_threshold_t *output_port_threshold)
{
     mmu_thdo_qconfig_cell_entry_t     mmu_thdo_qconfig_cell_entry={{0}};
     mmu_thdo_qoffset_cell_entry_t     mmu_thdo_qoffset_cell_entry={{0}};
     mmu_thdo_qconfig_qentry_entry_t   mmu_thdo_qconfig_qentry_entry={{0}};
     mmu_thdo_qoffset_qentry_entry_t   mmu_thdo_qoffset_qentry_entry={{0}};
     int                               temp_val;

     /* 1. MMU_THDO_QCONFIG_CELL */

     SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_CELLm(unit,
                         MEM_BLOCK_ANY,queue,
                         &mmu_thdo_qconfig_cell_entry));
     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
             &mmu_thdo_qconfig_cell_entry, Q_MIN_CELLf,
             output_port_threshold->min_grntd_res_queue_cells_int_buff);

     if (_soc_ml_mmu_params->lossless_mode_d_c) {
         temp_val = output_port_threshold->
         min_grntd_tot_shr_queue_cells_int_buff;
     } else {
         temp_val = _soc_ml_mmu_params->
                mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
     }
     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                         &mmu_thdo_qconfig_cell_entry,
                         Q_SHARED_LIMIT_CELLf,
                         temp_val);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                         &mmu_thdo_qconfig_cell_entry,
                         Q_LIMIT_DYNAMIC_CELLf,
                         _soc_ml_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                         &mmu_thdo_qconfig_cell_entry,
                         Q_LIMIT_ENABLE_CELLf,
                         _soc_ml_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry,
                        Q_COLOR_ENABLE_CELLf,0);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                         &mmu_thdo_qconfig_cell_entry,
                         Q_COLOR_LIMIT_DYNAMIC_CELLf,
                         _soc_ml_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry,
                        LIMIT_YELLOW_CELLf, 0);

     SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_CELLm(unit,
                         MEM_BLOCK_ANY,queue,
                         &mmu_thdo_qconfig_cell_entry));

     /* 2. MMU_THDO_QOFFSET_CELL */

     SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_CELLm(unit,
                         MEM_BLOCK_ANY,queue,
                         &mmu_thdo_qoffset_cell_entry));
     soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm,
                        &mmu_thdo_qoffset_cell_entry,
                        RESET_OFFSET_CELLf, 2);

     soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm,
                        &mmu_thdo_qoffset_cell_entry,
                        LIMIT_RED_CELLf, 0);

     soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm,
                        &mmu_thdo_qoffset_cell_entry,
                        RESET_OFFSET_YELLOW_CELLf,2);

     soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm,
                        &mmu_thdo_qoffset_cell_entry,
                        RESET_OFFSET_RED_CELLf,2);

     SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_CELLm(unit,
                         MEM_BLOCK_ANY,queue,
                         &mmu_thdo_qoffset_cell_entry));

     /* 3. MMU_THDO_QCONFIG_QENTRY */

     SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_QENTRYm(unit,
                        MEM_BLOCK_ANY,queue,
                        &mmu_thdo_qconfig_qentry_entry));

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                 &mmu_thdo_qconfig_qentry_entry, Q_MIN_QENTRYf,
                 output_port_threshold->min_grntd_res_queue_cells_EQEs);

     if (_soc_ml_mmu_params->lossless_mode_d_c) {
         temp_val = output_port_threshold->
         min_grntd_tot_shr_queue_cells_EQEs;
     } else {
         temp_val = _soc_ml_mmu_params->
         mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
     }
    soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                        &mmu_thdo_qconfig_qentry_entry,
                        Q_SHARED_LIMIT_QENTRYf, temp_val);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                         &mmu_thdo_qconfig_qentry_entry,
                         Q_LIMIT_DYNAMIC_QENTRYf,
                         _soc_ml_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                         &mmu_thdo_qconfig_qentry_entry,
                         Q_LIMIT_ENABLE_QENTRYf,
                         _soc_ml_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                         &mmu_thdo_qconfig_qentry_entry,
                         Q_COLOR_ENABLE_QENTRYf,0);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                         &mmu_thdo_qconfig_qentry_entry,
                         Q_COLOR_LIMIT_DYNAMIC_QENTRYf,
                         _soc_ml_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                    &mmu_thdo_qconfig_qentry_entry,
                    LIMIT_YELLOW_QENTRYf, 0);

     SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_QENTRYm(unit,
                         MEM_BLOCK_ANY,queue,
                         &mmu_thdo_qconfig_qentry_entry));

     /* 4. MMU_THDO_QOFFSET_QENTRY */

     SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_QENTRYm(unit,
                         MEM_BLOCK_ANY,queue,
                         &mmu_thdo_qoffset_qentry_entry));

     soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm,
                         &mmu_thdo_qoffset_qentry_entry,
                         RESET_OFFSET_QENTRYf, 1);

     soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm,
                         &mmu_thdo_qoffset_qentry_entry,
                         LIMIT_RED_QENTRYf, 0);

     soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm,
                         &mmu_thdo_qoffset_qentry_entry,
                         RESET_OFFSET_YELLOW_QENTRYf,1);

     soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm,
                         &mmu_thdo_qoffset_qentry_entry,
                         RESET_OFFSET_RED_QENTRYf,1);

     SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_QENTRYm(unit,
                         MEM_BLOCK_ANY,queue,
                         &mmu_thdo_qoffset_qentry_entry));
     return SOC_E_NONE;
}
/* Helper function for MMU settings based on
 * memory mode (lossless/lossy)
 * mode is selected based on config variable 'lossless_mode'
 */
STATIC
int _soc_ml_mmu_init_helper(int unit, int flex_port)
{
    soc_info_t    *si= &SOC_INFO(unit);
    uint16        dev_id=0;
    uint8         rev_id=0;
    uint32        valid_port_count = 0;
    soc_pbmp_t    pbmp_int_ge;
    uint32        pbmp_int_ge_count=0;
    soc_pbmp_t    pbmp_int_hg;
    uint32        pbmp_int_hg_count=0;
    uint32        mmu_num_olp_port=1;
    soc_pbmp_t    valid_port_cpu_pbmp;
    soc_port_t    port=0;
    uint32        available_internal_buffer=100;
    uint32        reserve_internal_buffer=0;
    uint32        temp_val;
    uint32        rval=0;
    uint32        service_pool=0;
    uint32        queue;
    uint32        op_node=0;
    uint32        op_node_offset=0;
    uint32        cos;
    uint32        idx, sub_port;
    uint32        val;

    /* Per Port  Settings */
    uint32                      mem_idx=0;
    thdi_port_sp_config_entry_t thdi_port_sp_config={{0}};
    /*
       (ML_MAX_PHYSICAL_PORTS+2) *
       ML_MAX_SERVICE_POOLS] = {{{0}}};
     */
    thdi_port_pg_config_entry_t thdi_port_pg_config={{0}};
    /*
       (ML_MAX_PHYSICAL_PORTS+2) *
       ML_MAX_PRIORITY_GROUPS] = {{{0}}};
     */

    thdirqe_thdi_port_sp_config_entry_t thdirqe_thdi_port_sp_config={{0}};
    /*
       (ML_MAX_PHYSICAL_PORTS+2) *
       ML_MAX_SERVICE_POOLS] = {{{0}}};
     */
    thdirqe_thdi_port_pg_config_entry_t thdirqe_thdi_port_pg_config={{0}};
    /*
       (ML_MAX_PHYSICAL_PORTS+2) *
       ML_MAX_PRIORITY_GROUPS] = {{{0}}};
     */
    thdiqen_thdi_port_sp_config_entry_t thdiqen_thdi_port_sp_config = {{0}};
    /*
       (ML_MAX_PHYSICAL_PORTS+2) *
       ML_MAX_SERVICE_POOLS] = {{{0}}};
     */
    thdiqen_thdi_port_pg_config_entry_t thdiqen_thdi_port_pg_config = {{0}};
    /*
       (ML_MAX_PHYSICAL_PORTS+2) *
       ML_MAX_PRIORITY_GROUPS] = {{{0}}};
     */

    mmu_thdo_opnconfig_cell_entry_t     mmu_thdo_opnconfig_cell_entry={{0}};
    mmu_thdo_qconfig_cell_entry_t       mmu_thdo_qconfig_cell_entry={{0}};
    mmu_thdo_qoffset_cell_entry_t       mmu_thdo_qoffset_cell_entry={{0}};

    mmu_thdo_opnconfig_qentry_entry_t   mmu_thdo_opnconfig_qentry_entry={{0}};
    mmu_thdo_qconfig_qentry_entry_t     mmu_thdo_qconfig_qentry_entry={{0}};
    mmu_thdo_qoffset_qentry_entry_t     mmu_thdo_qoffset_qentry_entry={{0}};

    _general_info_t *general_info =
        &_soc_ml_mmu_intermediate_results.general_info;
    _input_port_threshold_t *input_port_threshold =
        &_soc_ml_mmu_intermediate_results.input_port_threshold;
    _output_port_threshold_t *output_port_threshold =
        &_soc_ml_mmu_intermediate_results.output_port_threshold;

    _soc_ml_mmu_params_t _soc_ml_mmu_params={0};

    /********************************************************************/
    /***************** Fixed Parameter **********************************/
    /********************************************************************/
    _soc_ml_mmu_params.mmu_min_pkt_size = 64;              /*C5*/
    _soc_ml_mmu_params.mmu_ethernet_mtu_bytes=1536;        /*C6*/
    _soc_ml_mmu_params.mmu_max_pkt_size = 12288;           /*C7*/
    _soc_ml_mmu_params.mmu_jumbo_frame_size = 9216;        /*C8*/
    _soc_ml_mmu_params.mmu_int_buf_cell_size = 190;        /*C9*/
    _soc_ml_mmu_params.mmu_pkt_header_size = 62;           /*C10*/
    _soc_ml_mmu_params.mmu_lossless_pg_num = 1;            /*C13*/
    _soc_ml_mmu_params.mmu_line_rate_ge = 1;               /*C15*/
    _soc_ml_mmu_params.mmu_line_rate_hg = 10;              /*C18*/
    _soc_ml_mmu_params.mmu_num_cpu_port = 1;               /*C20*/
    _soc_ml_mmu_params.mmu_num_cpu_queue = 48;             /*C21*/
    _soc_ml_mmu_params.mmu_num_loopback_port = 1;          /*C22*/
    _soc_ml_mmu_params.mmu_num_loopback_queue = 8;
    _soc_ml_mmu_params.mmu_num_ep_redirection_queue = 16;   /**/
    _soc_ml_mmu_params.mmu_num_olp_queue = 8;               /*C26*/
    _soc_ml_mmu_params.mmu_pipeline_lat_cpap_admission = 30;/*C28*/
    _soc_ml_mmu_params.mmu_reserved_int_buf_cells = 42;     /*C30*/
    _soc_ml_mmu_params.mmu_ep_redirect_queue_entries = 480; /*C32*/
    _soc_ml_mmu_params.mmu_repl_engine_work_queue_entries = 1;/*C34*/
    _soc_ml_mmu_params.mmu_resv_repl_engine_work_queue_entries = 0;    /**/
    _soc_ml_mmu_params.mmu_repl_engine_work_queue_in_device = 12;    /*C36*/
    _soc_ml_mmu_params.mmu_ing_port_dyn_thd_param = 2; /*C39*/
    _soc_ml_mmu_params.mmu_ing_pg_dyn_thd_param = 7;  /*C40*/
    _soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf = 2; /*C41*/
    _soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy = 7;  /*C42*/
    _soc_ml_mmu_params.mmu_ing_cell_buf_reduction = 0; /*C43*/
    _soc_ml_mmu_params.mmu_ing_pkt_buf_reduction = 0; /*C44*/

    SOC_PBMP_CLEAR (pbmp_int_ge);
    SOC_PBMP_CLEAR (pbmp_int_hg);

    SOC_PBMP_CLEAR (valid_port_cpu_pbmp);
    SOC_PBMP_ASSIGN(valid_port_cpu_pbmp, PBMP_ALL(unit));
    /* SOC_PBMP_PORT_ADD(valid_port_cpu_pbmp, si->cmic_port); */

    soc_cm_get_id(unit, &dev_id, &rev_id);
    /* C11 */
    _soc_ml_mmu_params.lossless_mode_d_c =  soc_property_get(unit,
            spn_LOSSLESS_MODE, 0);
    /* C12 */
    _soc_ml_mmu_params.pfc_pause_mode_d_c =  soc_property_get(unit,
            "pfc_pause_mode", 0);
    /* C29 */
    _soc_ml_mmu_params.mmu_int_buf_size = 10922;

    SOC_PBMP_COUNT(PBMP_PORT_ALL (unit), valid_port_count);

    SOC_PBMP_ASSIGN(pbmp_int_ge,PBMP_GE_ALL(unit));
    SOC_PBMP_ASSIGN(pbmp_int_hg,PBMP_HG_ALL(unit));
    SOC_PBMP_OR(pbmp_int_hg,PBMP_XE_ALL(unit));
    SOC_PBMP_COUNT(pbmp_int_ge,pbmp_int_ge_count);
    SOC_PBMP_COUNT(pbmp_int_hg,pbmp_int_hg_count);

    /* C14 */
    _soc_ml_mmu_params.num_ge_ports_d = pbmp_int_ge_count ;

    /* C14 */
    _soc_ml_mmu_params.num_ge_int_ports_d = pbmp_int_ge_count;
    /* C16 */
    _soc_ml_mmu_params.num_egr_queue_per_int_ge_port_d = pbmp_int_ge_count ? 8:0;
    /* C17 */
    _soc_ml_mmu_params.num_hg_ports_d = pbmp_int_hg_count ;
    /* C17 */
    _soc_ml_mmu_params.num_hg_int_ports_d = pbmp_int_hg_count ;
    /* C19 */
    _soc_ml_mmu_params.num_egr_queue_per_int_hg_port_d = pbmp_int_hg_count ? 8:0;
    /* C25 */
    _soc_ml_mmu_params.mmu_num_olp_port_d = mmu_num_olp_port;
    /* */
    _soc_ml_mmu_params.olp_port_int_ext_bounding_d_c = soc_property_get(unit,
            "olp_port_bounding",0);
    /* C27 */
    _soc_ml_mmu_params.mmu_total_egress_queue = ML_MMU_MAX_EGRESS_QUEUE;

    /* C33 */
        /* C51 = C14+C17-1 */
    _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt =
                            _soc_ml_mmu_params.num_ge_ports_d +
                            _soc_ml_mmu_params.num_hg_ports_d - 1 ;

    available_internal_buffer=100;
    reserve_internal_buffer=0;

    /*  */
    _soc_ml_mmu_params.mmu_available_int_buf_size_d=available_internal_buffer;
    /* (_soc_ml_mmu_params.mmu_int_buf_size*available_internal_buffer)/100; */
    /*  */
    _soc_ml_mmu_params.mmu_reserved_int_buf_ema_pool_size_d=
        reserve_internal_buffer;
    /* (_soc_ml_mmu_params.mmu_int_buf_size*reserve_internal_buffer)/100; */

    /*  */
    _soc_ml_mmu_params.internal_buffer_reduction_d_c = soc_property_get(unit,
            "internal_buffer_reduction",0);

    /* C36 */
    _soc_ml_mmu_params.mmu_egress_queue_entries = 12;

    /* C38 = CEILING((C6*1024+C10)/C9, 1) */
    _soc_ml_mmu_params.per_cos_res_cells_for_int_buff_d =
        sal_ceil_func((_soc_ml_mmu_params.mmu_ethernet_mtu_bytes +
                    _soc_ml_mmu_params.mmu_pkt_header_size),
                _soc_ml_mmu_params.mmu_int_buf_cell_size);
    /***********************************************************************************/
    /*********************** Intermediate Results Processing****************************/
    /***********************************************************************************/

    /* C62 = CEILING((C7+C10)/C9,1) */
    general_info->max_packet_size_in_cells = sal_ceil_func(
            (_soc_ml_mmu_params.mmu_max_pkt_size +
             _soc_ml_mmu_params.mmu_pkt_header_size),
            _soc_ml_mmu_params.mmu_int_buf_cell_size);

    /* C63 = CEILING((C8+C10)/C9,1) */
    general_info->jumbo_frame_for_int_buff = sal_ceil_func(
            (_soc_ml_mmu_params.mmu_jumbo_frame_size +
             _soc_ml_mmu_params.mmu_pkt_header_size),
            _soc_ml_mmu_params.mmu_int_buf_cell_size);

    /* C64 =CEILING((C6*1024+C10)/C9, 1) */
    general_info->ether_mtu_cells_for_int_buff = sal_ceil_func(
            (_soc_ml_mmu_params.mmu_ethernet_mtu_bytes) +
            _soc_ml_mmu_params.mmu_pkt_header_size,
            _soc_ml_mmu_params.mmu_int_buf_cell_size);


    /* C66 = C14+C17+C20+C22+C25 */
    general_info->total_num_of_ports =
        _soc_ml_mmu_params.num_ge_ports_d +
        _soc_ml_mmu_params.num_hg_ports_d +
        _soc_ml_mmu_params.mmu_num_cpu_port +
        _soc_ml_mmu_params.mmu_num_loopback_port +
        _soc_ml_mmu_params.mmu_num_olp_port_d;

    /* C67 = C14+C17+C20+C25  */
    general_info->total_num_of_ports_excl_lpbk =
        _soc_ml_mmu_params.num_ge_ports_d +
        _soc_ml_mmu_params.num_hg_ports_d +
        _soc_ml_mmu_params.mmu_num_cpu_port +
        _soc_ml_mmu_params.mmu_num_olp_port_d;

    /* C68 = C14+C17+C20 */
    general_info->total_num_of_ports_excl_lpbk_olp =
        _soc_ml_mmu_params.num_ge_ports_d +
        _soc_ml_mmu_params.num_hg_ports_d +
        _soc_ml_mmu_params.mmu_num_cpu_port;

    /* C69 = C14+C17 */
    general_info->total_num_of_ports_excl_lpbk_olp_cpu =
        _soc_ml_mmu_params.num_ge_ports_d +
        _soc_ml_mmu_params.num_hg_ports_d;



    /* C71 = C16*C14 */
    general_info->total_egr_queues_for_a_int_ge_ports =
        (_soc_ml_mmu_params.num_egr_queue_per_int_ge_port_d *
         _soc_ml_mmu_params.num_ge_int_ports_d);


    /* C72 = C19*C17 */
    general_info->total_egr_queues_for_a_int_hg_ports =
        (_soc_ml_mmu_params.num_egr_queue_per_int_hg_port_d *
         _soc_ml_mmu_params.num_hg_int_ports_d);


    /* C73 = C20*C21 */
    general_info->total_cpu_queues =
        (_soc_ml_mmu_params.mmu_num_cpu_port *
         _soc_ml_mmu_params.mmu_num_cpu_queue);

    /* C74 = $C$71+$C$72+C73+C26*C25+C22*C23+C22*C24 */
    general_info->total_base_queue_int_buff =
        (general_info->total_egr_queues_for_a_int_ge_ports +
         general_info->total_egr_queues_for_a_int_hg_ports +
         general_info->total_cpu_queues);
    if (_soc_ml_mmu_params.olp_port_int_ext_bounding_d_c == 0) {
        general_info->total_base_queue_int_buff +=
            (_soc_ml_mmu_params.mmu_num_olp_queue *
             _soc_ml_mmu_params.mmu_num_olp_port_d);
    }
    general_info->total_base_queue_int_buff +=
        (_soc_ml_mmu_params.mmu_num_loopback_queue *
         _soc_ml_mmu_params.mmu_num_loopback_port);
    general_info->total_base_queue_int_buff +=
        ((_soc_ml_mmu_params.mmu_num_ep_redirection_queue *
          _soc_ml_mmu_params.mmu_num_loopback_port)/ 2);

    if (_soc_ml_mmu_params.olp_port_int_ext_bounding_d_c) {
        general_info->total_base_queue_ext_buff +=
            (_soc_ml_mmu_params.mmu_num_olp_queue *
             _soc_ml_mmu_params.mmu_num_olp_port_d);
    }

    /* C74 */
    general_info->total_egr_base_queues_in_device =
        general_info->total_base_queue_int_buff +
        general_info->total_base_queue_ext_buff;
    /* C75 = C27 */
    general_info->total_egr_queues_in_device =
        _soc_ml_mmu_params.mmu_total_egress_queue ;

    /* C95 = C42-C44*/
    general_info->max_int_cell_buff_size =
        _soc_ml_mmu_params.mmu_int_buf_size -
        _soc_ml_mmu_params.mmu_reserved_int_buf_cells ;

    /* C96 = C95 */
    general_info->int_cell_buff_size_after_limitation =
        general_info->max_int_cell_buff_size;

    general_info->int_buff_pool = general_info->max_int_cell_buff_size;

    /* C99 =IF(C17=1,FLOOR($C$45*(IF($C$46,$C$96,$C$95))-C41,1),0) */
    if (_soc_ml_mmu_params.internal_buffer_reduction_d_c) {
        temp_val = general_info->int_cell_buff_size_after_limitation;
    } else {
        temp_val = general_info->max_int_cell_buff_size;;
    }
    temp_val = (temp_val * _soc_ml_mmu_params.mmu_reserved_int_buf_ema_pool_size_d)/100;
    temp_val = temp_val - _soc_ml_mmu_params.mmu_pipeline_lat_cpap_admission;

    /* C101 =C52*1024-C53*/
    general_info->repl_engine_work_queue_entries =
        _soc_ml_mmu_params.mmu_repl_engine_work_queue_entries*1024 -
        _soc_ml_mmu_params.mmu_resv_repl_engine_work_queue_entries;


    /* C104 = 1 */
    general_info->int_buff_cells_per_avg_size_pkt = 1;


    /* C106 = 0.75 but normalized to 1 */
    general_info->max_prop_of_buff_used_by_one_queue_port = 75;


    /* Fill up input threshold info */
    /* C110 =CEILING(C7*2/C9, 1)  */
    input_port_threshold->global_hdrm_cells_for_int_buff_ingress_pool = sal_ceil_func
        (_soc_ml_mmu_params.mmu_max_pkt_size * 2 ,
         _soc_ml_mmu_params.mmu_int_buf_cell_size);

    /* C113 */
    input_port_threshold->global_hdrm_cells_for_RE_WQEs = 0 ;
    /* C114 */
    input_port_threshold->global_hdrm_cells_for_EQEs = 0 ;
    if (_soc_ml_mmu_params.lossless_mode_d_c) {
        /* C115*/
         input_port_threshold->hdrm_int_buff_cells_for_10G_PG = 178;
        /* C116*/
        input_port_threshold->hdrm_ema_buff_cells_for_10G_PG = 0;
        /* C117*/
        input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG = 66;
        /* C119 =IF(C14=0, 0, C118*C51)*/
        input_port_threshold->hdrm_EQEs_pkts_for_10G_PG =
            input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG *
            _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt ;
    } else {
        /* C115*/
        input_port_threshold->hdrm_int_buff_cells_for_10G_PG = 0;
        /* C116*/
        input_port_threshold->hdrm_ema_buff_cells_for_10G_PG = 0;
        /* C118*/
        input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG = 0;
        /* C119*/
        input_port_threshold->hdrm_EQEs_pkts_for_10G_PG = 0;
    }

    /* C120 = =C25*C16*$C$115 */
    input_port_threshold->hdrm_int_buff_cells_for_10G_total_PG =
        (_soc_ml_mmu_params.num_hg_ports_d *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_int_buff_cells_for_10G_PG);

    /* C121 = C25*C16*C116 */
    input_port_threshold->hdrm_ema_buff_cells_for_10G_total_PG =
        (_soc_ml_mmu_params.num_hg_ports_d *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_ema_buff_cells_for_10G_PG);

    /* C123 = C25*C16*C118 */
    input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_total_PG =
        (_soc_ml_mmu_params.num_hg_ports_d *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG);

    /* C124 = C25*C16*C119 */
    input_port_threshold->hdrm_EQEs_pkts_for_10G_total_PG =
        (_soc_ml_mmu_params.num_hg_ports_d *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_EQEs_pkts_for_10G_PG);

    if (_soc_ml_mmu_params.lossless_mode_d_c) {
        /* C125 */
        input_port_threshold->hdrm_int_buff_cells_for_1G_PG = 130;
        /* C126 */
        input_port_threshold->hdrm_ema_buff_cells_for_1G_PG = 0;
        /* C128*/
        input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG = 23;
        /* C129 =IF(C14=0, 0, C128*C51) */
        input_port_threshold->hdrm_EQEs_pkts_for_1G_PG =
            input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG *
            _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt;
    } else {
        /* C125 */
        input_port_threshold->hdrm_int_buff_cells_for_1G_PG = 0;
        /* C126 */
        input_port_threshold->hdrm_ema_buff_cells_for_1G_PG = 0;
        /* C128*/
        input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG = 0;
        /* C129*/
        input_port_threshold->hdrm_EQEs_pkts_for_1G_PG = 0;
    }

    /* C130 =C19*C16*$C$125 */
    input_port_threshold->hdrm_int_buff_cells_for_1G_total_PG =
        (_soc_ml_mmu_params.num_ge_ports_d *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_int_buff_cells_for_1G_PG);

    /* C131 = C19*C16*$C$126 */
    input_port_threshold->hdrm_ema_buff_cells_for_1G_total_PG =
        (_soc_ml_mmu_params.num_ge_ports_d *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_ema_buff_cells_for_1G_PG);

    /* C133 = C19*C16*$C$128 */
    input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_total_PG =
        (_soc_ml_mmu_params.num_ge_ports_d *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG);

    /* C134 = C19*C16*$C$129 */
    input_port_threshold->hdrm_EQEs_pkts_for_1G_total_PG =
        (_soc_ml_mmu_params.num_ge_ports_d *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_EQEs_pkts_for_1G_PG);

    /* C132-C135 values are zero for lossy mode */
    if (_soc_ml_mmu_params.lossless_mode_d_c) {
        /* C135 */
        input_port_threshold->hdrm_int_buff_cells_for_olp_port = 180;
        /* C136 */
        input_port_threshold->hdrm_ema_buff_cells_for_olp_port = 0;
        /* C138 */
        input_port_threshold->hdrm_RE_WQEs_pkts_for_olp_port = 7;
        /* C139 = C138*C51 */
        input_port_threshold->hdrm_EQEs_pkts_for_olp_port =
            input_port_threshold->hdrm_RE_WQEs_pkts_for_olp_port *
            _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt;
    } else {
        /* C135 */
        input_port_threshold->hdrm_int_buff_cells_for_olp_port = 0;
        /* C136 */
        input_port_threshold->hdrm_int_buff_cells_for_olp_port = 0;
        /* C138 */
        input_port_threshold->hdrm_RE_WQEs_pkts_for_olp_port = 0;
        /* C139 */
        input_port_threshold->hdrm_EQEs_pkts_for_olp_port = 0;
    }

    /* C140 = 44 */
    input_port_threshold->hdrm_int_buff_cells_for_lpbk_port =  44;

    /* C143 = =IF(C14=0, 23, 23) */
    input_port_threshold->hdrm_RE_WQEs_pkts_for_lpbk_port =  23;
   
    /* C144 = C143*C51 */
    input_port_threshold->hdrm_EQEs_pkts_for_lpbk_port =
        input_port_threshold->hdrm_RE_WQEs_pkts_for_lpbk_port *
        _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt;

    /* C145 = 44 */
    input_port_threshold->hdrm_int_buff_cells_for_cpu_port =  44;

    /* C148 =IF(C14=0, 23, 23) */
    input_port_threshold->hdrm_RE_WQEs_pkts_for_cpu_port = 23;

    /* C149 = C148*C51 */
    input_port_threshold->hdrm_EQEs_pkts_for_cpu_port =
        input_port_threshold->hdrm_RE_WQEs_pkts_for_cpu_port *
        _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt;

    /* C150 =$C$110+$C$120+$C$130+C135+C140+C145*/
    input_port_threshold->total_hdrm_int_buff_cells =
        input_port_threshold->global_hdrm_cells_for_int_buff_ingress_pool +
        input_port_threshold->hdrm_int_buff_cells_for_10G_total_PG +
        input_port_threshold->hdrm_int_buff_cells_for_1G_total_PG +
        input_port_threshold->hdrm_int_buff_cells_for_olp_port +
        input_port_threshold->hdrm_int_buff_cells_for_lpbk_port +
        input_port_threshold->hdrm_int_buff_cells_for_cpu_port;

    /* C151 = =C111+C121+C131+C136+C141+C146*/
    input_port_threshold->total_hdrm_int_buff_ema_pool_cells =
        input_port_threshold->global_hdrm_cells_for_int_buff_ema_pool +
        input_port_threshold->hdrm_ema_buff_cells_for_10G_total_PG +
        input_port_threshold->hdrm_ema_buff_cells_for_1G_total_PG +
        input_port_threshold->hdrm_ema_buff_cells_for_olp_port +
        input_port_threshold->hdrm_ema_buff_cells_for_lpbk_port +
        input_port_threshold->hdrm_ema_buff_cells_for_cpu_port;


    /* C153 = =$C$113+$C$123+$C$133+C138+C143+C148 */
    input_port_threshold->total_hdrm_RE_WQEs_pkts =
        input_port_threshold->global_hdrm_cells_for_RE_WQEs+
        input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_total_PG+
        input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_total_PG+
        input_port_threshold->hdrm_RE_WQEs_pkts_for_olp_port+
        input_port_threshold->hdrm_RE_WQEs_pkts_for_lpbk_port+
        input_port_threshold->hdrm_RE_WQEs_pkts_for_cpu_port;

    /* C154 =$C$114+$C$124+$C$134+C139+C144+C149*/
    input_port_threshold->total_hdrm_EQEs_pkts =
        input_port_threshold->global_hdrm_cells_for_EQEs+
        input_port_threshold->hdrm_EQEs_pkts_for_10G_total_PG+
        input_port_threshold->hdrm_EQEs_pkts_for_1G_total_PG+
        input_port_threshold->hdrm_EQEs_pkts_for_olp_port+
        input_port_threshold->hdrm_EQEs_pkts_for_lpbk_port+
        input_port_threshold->hdrm_EQEs_pkts_for_cpu_port;

    /* C156 = $C$73 */
    input_port_threshold->min_int_buff_cells_per_PG =
        general_info->jumbo_frame_for_int_buff;

    /* C159 */
    input_port_threshold->min_RE_WQEs_pkt_per_PG = 9;

    /* C160 = =$C$159*C51 */
    input_port_threshold->min_EQEs_pkt_per_PG =
        input_port_threshold->min_RE_WQEs_pkt_per_PG *
        _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt;

    /* C162 = =C79*C16*$C$156 */
    input_port_threshold->min_int_buff_cells_for_total_PG =
        (general_info->total_num_of_ports *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->min_int_buff_cells_per_PG) ;

    /* C163 = =C79*C16*$C$157 */
    input_port_threshold->min_int_buff_ema_pool_cells_for_total_PG =
        (general_info->total_num_of_ports *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->min_int_buff_ema_pool_cells_per_PG);

    /* C165 = C75*C16*$C$159 */
    input_port_threshold->min_RE_WQEs_pkts_for_total_PG =
        (general_info->total_num_of_ports *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->min_RE_WQEs_pkt_per_PG) ;

    /* C166 = C75*C16*$C$160 */
    input_port_threshold->min_EQEs_pkts_for_total_PG =
        (general_info->total_num_of_ports *
         _soc_ml_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->min_EQEs_pkt_per_PG) ;

    /* C168 C169 C170 C171 C172= 0 */
    input_port_threshold->min_int_buff_cells_for_a_port = 0;
    input_port_threshold->min_int_buff_ema_pool_cells_for_a_port = 0;
    input_port_threshold->min_RE_WQEs_pkts_for_a_port = 0;
    input_port_threshold->min_EQEs_pkts_for_a_port = 0;

    /* C174 =$C$79*$C$168 */
    input_port_threshold->min_int_buff_cells_for_total_port =
        (general_info->total_num_of_ports *
         input_port_threshold->min_int_buff_cells_for_a_port);

    /* C175 = $C$75*$C$169 */
    input_port_threshold->min_int_buff_ema_pool_cells_for_total_port =
        (general_info->total_num_of_ports *
         input_port_threshold->min_int_buff_ema_pool_cells_for_a_port);


    /* C177 = $C$75*$C$171 */
    input_port_threshold->min_RE_WQEs_pkts_for_total_port =
        (general_info->total_num_of_ports *
         input_port_threshold->min_RE_WQEs_pkts_for_a_port);

    /* C179 = $C$75*$C$172 */
    input_port_threshold->min_EQEs_pkts_for_total_port =
        (general_info->total_num_of_ports *
         input_port_threshold->min_EQEs_pkts_for_a_port);

    /* C180 =$C$162+$C$174 */
    input_port_threshold->total_min_int_buff_cells =
        input_port_threshold->min_int_buff_cells_for_total_PG +
        input_port_threshold->min_int_buff_cells_for_total_port;

    /* C181 = $C$163+$C$175 */
    input_port_threshold->total_min_int_buff_ema_pool_cells =
        input_port_threshold->min_int_buff_ema_pool_cells_for_total_PG +
        input_port_threshold->min_int_buff_ema_pool_cells_for_total_port;


    /* C183 = $C$165+$C$177 */
    input_port_threshold->total_min_RE_WQEs_pkts =
        input_port_threshold->min_RE_WQEs_pkts_for_total_PG +
        input_port_threshold->min_RE_WQEs_pkts_for_total_port;

    /* C184 = $C$166+$C$178 */
    input_port_threshold->total_min_EQEs_pkts =
        input_port_threshold->min_EQEs_pkts_for_total_PG +
        input_port_threshold->min_EQEs_pkts_for_total_port;


    /* Fill up output threshold info */
    /* C204 = =IF(C14=0, C58, 0) */
    if (_soc_ml_mmu_params.lossless_mode_d_c == 0) {
        output_port_threshold->min_grntd_res_queue_cells_int_buff =
            _soc_ml_mmu_params.per_cos_res_cells_for_int_buff_d;
    }
    /* C206 = =IF(C14=0, $C$58/$C$105, 0) */
    if (_soc_ml_mmu_params.lossless_mode_d_c == 0) {
        output_port_threshold->min_grntd_res_queue_cells_EQEs =
            _soc_ml_mmu_params.per_cos_res_cells_for_int_buff_d;
    }
    /* C208 = C204 */
    output_port_threshold->min_grntd_res_RE_WQs_cells =
        output_port_threshold->min_grntd_res_queue_cells_int_buff;

    /* C209 = C204 */
    output_port_threshold->min_grntd_res_RE_WQs_queue_cells_for_int_buff =
        output_port_threshold->min_grntd_res_queue_cells_int_buff;


    /* C211 = 4 */
    output_port_threshold->min_grntd_res_EP_redirect_queue_entry_cells = 4;


    /* C213 =C89*$C$204 */
    output_port_threshold->min_grntd_tot_res_queue_cells_int_buff =
        general_info->total_base_queue_int_buff *
        output_port_threshold->min_grntd_res_queue_cells_int_buff;


    /* C215  =C94*$C$206 */
    output_port_threshold->min_grntd_tot_res_queue_cells_EQEs =
        general_info->total_egr_base_queues_in_device *
        output_port_threshold->min_grntd_res_queue_cells_EQEs;

    /* C216 =$C$55*$C$207 */
    output_port_threshold->min_grntd_tot_res_EMA_queue_cells =
        _soc_ml_mmu_params.mmu_ema_queues *
        output_port_threshold->min_grntd_res_EMA_queue_cells;

    /* C217 =$C$54*$C$208 */
    output_port_threshold->min_grntd_tot_res_RE_WQs_cells =
        _soc_ml_mmu_params.mmu_repl_engine_work_queue_in_device *
        output_port_threshold->min_grntd_res_RE_WQs_cells;

    /* C218 =$C$54*$C$209 */
    output_port_threshold->min_grntd_tot_res_RE_WQs_queue_cells_for_int_buff =
        _soc_ml_mmu_params.mmu_repl_engine_work_queue_in_device *
        output_port_threshold->min_grntd_res_RE_WQs_queue_cells_for_int_buff;


    /* C220 =C211*(C36) */
    output_port_threshold->min_grntd_tot_res_EP_redirect_queue_entry_cells =
        (_soc_ml_mmu_params.mmu_num_ep_redirection_queue *
        output_port_threshold->min_grntd_res_EP_redirect_queue_entry_cells)/2;


    /* C186 =$C$98-$C$150-$C$180-IF(C14=0, 0, C213)-IF(C14=0, 0, C218) */

    input_port_threshold->total_shared_ing_buff_pool =
        general_info->int_buff_pool -
        input_port_threshold->total_min_int_buff_cells -
        input_port_threshold->total_hdrm_int_buff_cells;
    if (_soc_ml_mmu_params.lossless_mode_d_c) {
        input_port_threshold->total_shared_ing_buff_pool -=
            output_port_threshold->min_grntd_tot_res_queue_cells_int_buff;
        input_port_threshold->total_shared_ing_buff_pool -=
            output_port_threshold->min_grntd_tot_res_RE_WQs_queue_cells_for_int_buff;
    }



    /* C189 =$C$101-$C$153-$C$183-IF(C14=0, 0, C217) */
    input_port_threshold->total_shared_RE_WQEs_buff =
        general_info->repl_engine_work_queue_entries -
        input_port_threshold->total_hdrm_RE_WQEs_pkts -
        input_port_threshold->total_min_RE_WQEs_pkts;
    if (_soc_ml_mmu_params.lossless_mode_d_c) {
        input_port_threshold->total_shared_RE_WQEs_buff -=
            output_port_threshold->min_grntd_tot_res_RE_WQs_cells;
    }

    /* C190 =($C$49*1024)-$C$154-$C$184-IF(C14=0, 0, C215)-1 */
    input_port_threshold->total_shared_EQEs_buff =
        (_soc_ml_mmu_params.mmu_egress_queue_entries * 1024) -
        input_port_threshold->total_hdrm_EQEs_pkts -
        input_port_threshold->total_min_EQEs_pkts;
    if (_soc_ml_mmu_params.lossless_mode_d_c) {
        input_port_threshold->total_shared_EQEs_buff -=
            output_port_threshold->min_grntd_tot_res_queue_cells_EQEs;
    }
    input_port_threshold->total_shared_EQEs_buff -= 1;

    /* C192 =FLOOR(($C$60*$C$186)/(1+($C$60*1)),1) */
    input_port_threshold->ingress_burst_cells_size_for_one_port = sal_floor_func(
            (_soc_ml_mmu_params.mmu_ing_port_dyn_thd_param *
             input_port_threshold->total_shared_ing_buff_pool),
            (1+(_soc_ml_mmu_params.mmu_ing_port_dyn_thd_param*1)));

    /* C193 =FLOOR(($C$60*$C$190)/(1+($C$60*1)),1) */
    input_port_threshold->ingress_burst_pkts_size_for_one_port = sal_floor_func(
            (_soc_ml_mmu_params.mmu_ing_port_dyn_thd_param *
             input_port_threshold->total_shared_EQEs_buff),
            (1+(_soc_ml_mmu_params.mmu_ing_port_dyn_thd_param*1)));

    /* C194 =FLOOR(($C$60*$C$186)/(1+($C$60*$C$79)),1) */
    input_port_threshold->ingress_burst_cells_size_for_all_ports = sal_floor_func(
            (_soc_ml_mmu_params.mmu_ing_port_dyn_thd_param *
             input_port_threshold->total_shared_ing_buff_pool),
            (1+(_soc_ml_mmu_params.mmu_ing_port_dyn_thd_param*
                general_info->total_num_of_ports)));

    /* C195 =($C$194+$C$168+C156)*$C$79 */
    input_port_threshold->ingress_total_shared_cells_use_for_all_port =
        (input_port_threshold->ingress_burst_cells_size_for_all_ports +
         input_port_threshold->min_int_buff_cells_for_a_port +
         input_port_threshold->min_int_buff_cells_per_PG) *
        general_info->total_num_of_ports;

    /* C196 =FLOOR(($C$60*$C$190)/(1+($C$60*$C$79)),1) */
    input_port_threshold->ingress_burst_pkts_size_for_all_port = sal_floor_func(
            _soc_ml_mmu_params.mmu_ing_port_dyn_thd_param *
            input_port_threshold->total_shared_EQEs_buff,
            (1+(_soc_ml_mmu_params.mmu_ing_port_dyn_thd_param*
                general_info->total_num_of_ports)));

    /* C197 =($C$196+$C$172+C160)*$C$79 */
    input_port_threshold->ingress_total_shared_pkts_use_for_all_port =
        (input_port_threshold->ingress_burst_pkts_size_for_all_port+
         input_port_threshold->min_EQEs_pkts_for_a_port +
         input_port_threshold->min_EQEs_pkt_per_PG) *
        general_info->total_num_of_ports;

    /* C198 = =$C$195+C150 */
    input_port_threshold->ingress_total_shared_hdrm_cells_use_for_all_port =
        input_port_threshold->ingress_total_shared_cells_use_for_all_port +
        input_port_threshold->total_hdrm_int_buff_cells;

    /* C199 = =$C$197+C154 */
    input_port_threshold->ingress_total_shared_hdrm_pkts_use_for_all_port =
        input_port_threshold->ingress_total_shared_pkts_use_for_all_port +
        input_port_threshold->total_hdrm_EQEs_pkts;

    /* C222 =C98-$C$213-C218 */
    output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff =
        general_info->int_buff_pool -
        output_port_threshold->
        min_grntd_tot_res_queue_cells_int_buff-
        output_port_threshold->
        min_grntd_tot_res_RE_WQs_queue_cells_for_int_buff;


    /* C224 =$C$49*1024-$C$215-1 */
    output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs =
        (_soc_ml_mmu_params.mmu_egress_queue_entries*1024) -
        output_port_threshold->min_grntd_tot_res_queue_cells_EQEs - 1;

    /* C225 =$C$99-$C$216 */
    output_port_threshold->min_grntd_tot_shr_EMA_queue_cells =
        general_info->ema_pool -
        output_port_threshold->min_grntd_tot_res_EMA_queue_cells;

    /* C226 =$C$101-$C$217 */
    output_port_threshold->min_grntd_tot_shr_RE_WQs_cells =
        general_info->repl_engine_work_queue_entries -
        output_port_threshold->min_grntd_tot_res_RE_WQs_cells ;

    /* C227 =C222 */
    output_port_threshold->min_grntd_tot_shr_RE_WQs_queue_cells_for_int_buff =
        output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff ;


    /* C229 =C50-C220 */
    output_port_threshold->min_grntd_tot_shr_EP_redirect_queue_entry_cells =
        _soc_ml_mmu_params.mmu_ep_redirect_queue_entries -
        output_port_threshold->min_grntd_tot_res_EP_redirect_queue_entry_cells;

    /* C230 = 2 */
    output_port_threshold->egress_queue_dynamic_threshold_parameter = 2;

    /* C231 =FLOOR( ($C$62*$C$222)/(1+($C$62*1)), 1) */
    output_port_threshold->egress_burst_cells_size_for_one_queue = sal_floor_func(
            (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
             output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff),
            (1 + (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf*1)));

    /* C232 =FLOOR( ($C$62*$C$224)/(1+($C$62*1)), 1) */
    output_port_threshold->egress_burst_pkts_size_for_one_queue = sal_floor_func(
            (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
             output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs),
            (1 + (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf * 1)));

    /* C233 = =FLOOR( ($C$62*$C$222)/(1+($C$62*$C$79)), 1)*/
    output_port_threshold->egress_burst_cells_size_for_all_ports = sal_floor_func(
            (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
             output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff),
            (1 + (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
                  general_info->total_num_of_ports)));

    /* C234 =FLOOR( ($C$62*$C$224)/(1+($C$62*$C$79)), 1) */
    output_port_threshold->egress_burst_pkts_size_for_all_ports = sal_floor_func(
            (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
             output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs),
            (1 + (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
                  general_info->total_num_of_ports)));

    /* C235 =FLOOR( ($C$62*$C$222)/(1+($C$62*C93)), 1) */
    output_port_threshold->egress_burst_cells_size_for_all_queues = sal_floor_func(
            (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
             output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff),
            (1 + (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
                  general_info->total_egr_base_queues_in_device)));

    /* C236 =FLOOR( ($C$62*$C$224)/(1+($C$62*C93)), 1) */
    output_port_threshold->egress_burst_pkts_size_for_all_queues = sal_floor_func(
            (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
             output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs),
            (1 + (_soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
                  general_info->total_egr_base_queues_in_device)));

    /* C237 =C93*$C$235 */
    output_port_threshold->egress_total_use_in_cells_for_all_queues =
        general_info->total_egr_base_queues_in_device *
        output_port_threshold->egress_burst_cells_size_for_all_queues;

    /* C238 =C93*$C$236 */
    output_port_threshold->egress_total_use_in_pkts_for_all_queues =
        general_info->total_egr_base_queues_in_device *
        output_port_threshold->egress_burst_pkts_size_for_all_queues;

    /* C239 =$C$222-C237*/
    output_port_threshold->egress_remaining_cells_for_all_queues =
        output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff -
        output_port_threshold->egress_total_use_in_cells_for_all_queues;

    /* C240 =$C$224-C238*/
    output_port_threshold->egress_remaining_pkts_for_all_queues =
        output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs -
        output_port_threshold->egress_total_use_in_pkts_for_all_queues;


    /*======================================================================== */
    /*                        Device Wide Registers                            */
    /* ======================================================================= */

    if (flex_port == -1) {
        /* This is flex operation. so no need to re-configure */

	/* C244: CFAPICONFIG.CFAPIPOOLSIZE =C42+22 */
	SOC_IF_ERROR_RETURN(READ_CFAPICONFIGr(unit, &rval));
	soc_reg_field_set(unit, CFAPICONFIGr, &rval, CFAPIPOOLSIZEf,
		_soc_ml_mmu_params.mmu_int_buf_size + 22 );
	SOC_IF_ERROR_RETURN(WRITE_CFAPICONFIGr(unit, rval));


	/* C245: CFAPIFULLSETPOINT.CFAPIFULLSETPOINT = =C96 */
	SOC_IF_ERROR_RETURN(READ_CFAPIFULLSETPOINTr(unit, &rval));
	soc_reg_field_set(unit, CFAPIFULLSETPOINTr, &rval, CFAPIFULLSETPOINTf,
		general_info->int_cell_buff_size_after_limitation);
	SOC_IF_ERROR_RETURN(WRITE_CFAPIFULLSETPOINTr(unit, rval));

	/* C246: CFAPIFULLRESETPOINT.CFAPIFULLRESETPOINT =C244-128 */
	SOC_IF_ERROR_RETURN(READ_CFAPIFULLRESETPOINTr(unit, &rval));
	soc_reg_field_set(unit, CFAPIFULLRESETPOINTr, &rval, CFAPIFULLRESETPOINTf,
		_soc_ml_mmu_params.mmu_int_buf_size + 22 - 128 );
	SOC_IF_ERROR_RETURN(WRITE_CFAPIFULLRESETPOINTr(unit, rval));

	/* C250: MMU_ENQ_FAPCONFIG_0.FAPPOOLSIZE =C52*1024-1*/
	SOC_IF_ERROR_RETURN(READ_MMU_ENQ_FAPCONFIG_0r(unit, &rval));
	/*soc_reg_field_set(unit, MMU_ENQ_FAPCONFIG_0r, &rval, FAPPOOLSIZEf,
		(_soc_ml_mmu_params.mmu_repl_engine_work_queue_entries*1024 - 1 )); */
	soc_reg_field_set(unit, MMU_ENQ_FAPCONFIG_0r, &rval, FAPPOOLSIZEf, 1344);
	SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_FAPCONFIG_0r(unit, rval));
	temp_val = 1344;

	/* C251 MMU_ENQ_FAPFULLSETPOINT_0.FAPFULLSETPOINT =C250-64 */
	SOC_IF_ERROR_RETURN(READ_MMU_ENQ_FAPFULLSETPOINT_0r(unit, &rval));
	soc_reg_field_set(unit, MMU_ENQ_FAPFULLSETPOINT_0r, &rval, FAPFULLSETPOINTf,
	      temp_val - 64);
	SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_FAPFULLSETPOINT_0r(unit, rval));

	/* C252 MMU_ENQ_FAPFULLRESETPOINT_0.FAPFULLRESETPOINT =C250-128 */
	SOC_IF_ERROR_RETURN(READ_MMU_ENQ_FAPFULLRESETPOINT_0r(unit, &rval));
	soc_reg_field_set(unit, MMU_ENQ_FAPFULLRESETPOINT_0r, &rval, FAPFULLRESETPOINTf,
		temp_val - 128 );
	SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_FAPFULLRESETPOINT_0r(unit, rval));

	/* C253: QSTRUCT_FAPCONFIG.FAPPOOLSIZE =(C49*1024+1024*3)/4 */
	temp_val = (((_soc_ml_mmu_params.mmu_egress_queue_entries * 1024 ) + (1024 *3))/4);
	for (idx =0 ; idx < 1; idx++) {
	    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPCONFIGr(unit, idx, &rval));
	    soc_reg_field_set(unit, QSTRUCT_FAPCONFIGr, &rval, FAPPOOLSIZEf,
		    temp_val);
	    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPCONFIGr(unit, idx, rval));
	}
	/* C254 QSTRUCT_FAPFULLSETPOINT.FAPFULLSETPOINT = C253 - 8 */
	temp_val = temp_val - 8;
	for (idx =0 ; idx < 1; idx++) {
	    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPFULLSETPOINTr(unit, idx, &rval));
	    soc_reg_field_set(unit, QSTRUCT_FAPFULLSETPOINTr, &rval, FAPFULLSETPOINTf,
		    temp_val);
	    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLSETPOINTr(unit, idx, rval));
	}

	/* C255 QSTRUCT_FAPFULLRESETPOINT.FAPFULLRESETPOINT = C254 - 16*/
	temp_val = temp_val -16;
	for (idx =0 ; idx < 1; idx++) {
	    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPFULLRESETPOINTr(unit, idx, &rval));
	    soc_reg_field_set(unit, QSTRUCT_FAPFULLRESETPOINTr, &rval,
		    FAPFULLRESETPOINTf,
		    temp_val );
	    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLRESETPOINTr(unit, idx, rval));
	}

	/* C256: QSTRUCT_FAPOTPCONFIG.MAXFULLSET = C253*/
	temp_val = (((_soc_ml_mmu_params.mmu_egress_queue_entries * 1024 ) + (1024 *3))/4);
	for (idx =0 ; idx < 1; idx++) {
	    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPOTPCONFIGr(unit, idx, &rval));
	    soc_reg_field_set(unit, QSTRUCT_FAPOTPCONFIGr, &rval, MAXFULLSETf,
		    temp_val);
	    /* Its failing on Emulator */       
	    /* SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPOTPCONFIGr(unit, idx, rval)); */
	}

	/* C261: THDO_MISCCONFIG.UCMC_SEPARATION = 0 */
	SOC_IF_ERROR_RETURN(READ_THDO_MISCCONFIGr(unit, &rval));
	soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, UCMC_SEPARATIONf, 0);
	SOC_IF_ERROR_RETURN(WRITE_THDO_MISCCONFIGr(unit, rval));

	/* C262: WRED_MISCCONFIG.UCMC_SEPARATION = 0 */
	SOC_IF_ERROR_RETURN(READ_WRED_MISCCONFIGr(unit, &rval));
	soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, UCMC_SEPARATIONf, 0);
	SOC_IF_ERROR_RETURN(WRITE_WRED_MISCCONFIGr(unit, rval));

	/* C266: COLOR_AWARE.ENABLE = 0 */
	SOC_IF_ERROR_RETURN(READ_COLOR_AWAREr (unit, &rval));
	soc_reg_field_set(unit, COLOR_AWAREr, &rval, ENABLEf, 0);
	SOC_IF_ERROR_RETURN(WRITE_COLOR_AWAREr (unit, rval));

	/* C268: GLOBAL_HDRM_LIMIT.GLOBAL_HDRM_LIMIT = =$C$110 */
	SOC_IF_ERROR_RETURN(READ_GLOBAL_HDRM_LIMITr (unit, &rval));
	soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval, GLOBAL_HDRM_LIMITf,
		input_port_threshold->global_hdrm_cells_for_int_buff_ingress_pool);
	SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr (unit, rval));

	/* C269: BUFFER_CELL_LIMIT_SP.LIMIT =  =$C$186 */
	SOC_IF_ERROR_RETURN(READ_BUFFER_CELL_LIMIT_SPr (unit, service_pool, &rval));
	soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
		input_port_threshold->total_shared_ing_buff_pool);
	SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SPr (unit, service_pool, rval));

	/* C270: CELL_RESET_LIMIT_OFFSET_SP.OFFSET =CEILING(C82/4, 1)*C76 */
	SOC_IF_ERROR_RETURN(READ_CELL_RESET_LIMIT_OFFSET_SPr (unit,
		    service_pool, &rval));
	temp_val = sal_ceil_func(general_info->total_num_of_ports_excl_lpbk_olp_cpu,4) *
	    general_info->ether_mtu_cells_for_int_buff;
	soc_reg_field_set(unit, CELL_RESET_LIMIT_OFFSET_SPr, &rval,
		OFFSETf, temp_val);
	SOC_IF_ERROR_RETURN(WRITE_CELL_RESET_LIMIT_OFFSET_SPr (unit,
		    service_pool, rval));
	/* C280: THDIRQE_GLOBAL_HDRM_LIMIT.GLOBAL_HDRM_LIMIT = C113 */
	SOC_IF_ERROR_RETURN(READ_THDIRQE_GLOBAL_HDRM_LIMITr (unit, &rval));
	soc_reg_field_set(unit, THDIRQE_GLOBAL_HDRM_LIMITr, &rval,
		GLOBAL_HDRM_LIMITf,
		input_port_threshold->global_hdrm_cells_for_RE_WQEs);
	SOC_IF_ERROR_RETURN(WRITE_THDIRQE_GLOBAL_HDRM_LIMITr (unit, rval));

	/* C227:  THDIRQE_BUFFER_CELL_LIMIT_SP.LIMIT =$C$189  */
	SOC_IF_ERROR_RETURN(READ_THDIRQE_BUFFER_CELL_LIMIT_SPr (
		    unit, service_pool, &rval));
	soc_reg_field_set(unit, THDIRQE_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
		input_port_threshold->total_shared_RE_WQEs_buff);
	SOC_IF_ERROR_RETURN(WRITE_THDIRQE_BUFFER_CELL_LIMIT_SPr (
		    unit, service_pool, rval));

	/* C282: = THDIRQE_CELL_RESET_LIMIT_OFFSET_SP.OFFSET = =CEILING(C82/4, 1) */
	SOC_IF_ERROR_RETURN(READ_THDIRQE_CELL_RESET_LIMIT_OFFSET_SPr(
		    unit,service_pool,&rval));
	soc_reg_field_set(unit, THDIRQE_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
		OFFSETf,
		sal_ceil_func(general_info->total_num_of_ports_excl_lpbk_olp_cpu , 4));
	SOC_IF_ERROR_RETURN(WRITE_THDIRQE_CELL_RESET_LIMIT_OFFSET_SPr(
		    unit,service_pool,rval));

	/* C284: THDIQEN_GLOBAL_HDRM_LIMIT.GLOBAL_HDRM_LIMIT = C114 */
	SOC_IF_ERROR_RETURN(READ_THDIQEN_GLOBAL_HDRM_LIMITr (unit, &rval));
	soc_reg_field_set(unit, THDIQEN_GLOBAL_HDRM_LIMITr, &rval,
		GLOBAL_HDRM_LIMITf,
		input_port_threshold->global_hdrm_cells_for_EQEs);
	SOC_IF_ERROR_RETURN(WRITE_THDIQEN_GLOBAL_HDRM_LIMITr (unit, rval));

	/* C285:  THDIQEN_BUFFER_CELL_LIMIT_SP.LIMIT =$C$190  */
	SOC_IF_ERROR_RETURN(READ_THDIQEN_BUFFER_CELL_LIMIT_SPr (
		    unit, service_pool, &rval));
	soc_reg_field_set(unit, THDIQEN_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
		input_port_threshold->total_shared_EQEs_buff);
	SOC_IF_ERROR_RETURN(WRITE_THDIQEN_BUFFER_CELL_LIMIT_SPr (
		    unit, service_pool, rval));

	/* C286: THDIQEN_CELL_RESET_LIMIT_OFFSET_SP.OFFSET=
	   =CEILING(C82/4, 1)*C51*/
	SOC_IF_ERROR_RETURN(READ_THDIQEN_CELL_RESET_LIMIT_OFFSET_SPr(
		    unit,service_pool,&rval));
	temp_val = sal_ceil_func(general_info->total_num_of_ports_excl_lpbk_olp_cpu, 4)  *
	    _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt;
	soc_reg_field_set(unit, THDIQEN_CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf,
		temp_val);
	SOC_IF_ERROR_RETURN(WRITE_THDIQEN_CELL_RESET_LIMIT_OFFSET_SPr(
		    unit,service_pool,rval));

	/* C289: OP_BUFFER_SHARED_LIMIT_CELLI.OP_BUFFER_SHARED_LIMIT_CELLI = C222 */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_CELLIr (unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLIr, &rval,
		OP_BUFFER_SHARED_LIMIT_CELLIf,
		output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLIr (unit, rval));

	temp_val = output_port_threshold->
	    min_grntd_tot_shr_queue_cells_int_buff;

	/* C290 OP_BUFFER_SHARED_LIMIT_RESUME_CELLI.OP_BUFFER_SHARED_LIMIT_RESUME_CELLI
	   =C289-CEILING(C82/4, 1)*C76 */

	temp_val = temp_val - (sal_ceil_func(general_info->total_num_of_ports_excl_lpbk_olp_cpu,4)
		* general_info->ether_mtu_cells_for_int_buff);

	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr(unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr, &rval,
		OP_BUFFER_SHARED_LIMIT_RESUME_CELLIf, temp_val);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr(unit, rval));

	/* C292 OP_BUFFER_LIMIT_RESUME_YELLOW_CELLI.OP_BUFFER_LIMIT_RESUME_YELLOW_CELLI =
	   =CEILING(C290/8, 1) */

	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIr (
		    unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIr, &rval,
		OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIf, sal_ceil_func(temp_val,8));
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIr (
		    unit, rval));

	/* C294 OP_BUFFER_LIMIT_RESUME_RED_CELLI.OP_BUFFER_LIMIT_RESUME_RED_CELLI
	   =CEILING(C290/8, 1) */

	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_CELLIr (unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_CELLIr, &rval,
		OP_BUFFER_LIMIT_RESUME_RED_CELLIf, sal_ceil_func(temp_val,8));
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_CELLIr (unit, rval));

	/* C291: OP_BUFFER_LIMIT_YELLOW_CELLI.OP_BUFFER_LIMIT_YELLOW_CELLI =
	   =CEILING(C289/8, 1) */
	temp_val = output_port_threshold->
	    min_grntd_tot_shr_queue_cells_int_buff;
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_CELLIr (unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLIr, &rval,
		OP_BUFFER_LIMIT_YELLOW_CELLIf, sal_ceil_func(temp_val,8));
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLIr (unit, rval));

	/* C293 OP_BUFFER_LIMIT_RED_CELLI.OP_BUFFER_LIMIT_RED_CELLI
	   =CEILING(C289/8, 1) */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_CELLIr (unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLIr, &rval,
		OP_BUFFER_LIMIT_RED_CELLIf, sal_ceil_func( temp_val,8));
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLIr (unit, rval));


	/* C310:
	   OP_BUFFER_SHARED_LIMIT_THDORQEQ.OP_BUFFER_SHARED_LIMIT_CELLE = C226 */

	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_THDORQEQr (unit, &rval));
	temp_val = output_port_threshold->min_grntd_tot_shr_RE_WQs_cells;
	soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_THDORQEQr, &rval,
		OP_BUFFER_SHARED_LIMITf, temp_val);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_THDORQEQr (unit, rval));

	/* C312: OP_BUFFER_LIMIT_YELLOW_THDORQEQ.OP_BUFFER_LIMIT_YELLOW =
	   =CEILING(C310/8, 1) */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_THDORQEQr (unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_THDORQEQr, &rval,
		OP_BUFFER_LIMIT_YELLOWf, sal_ceil_func(temp_val,8));
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_THDORQEQr (unit, rval));

	/* C314: OP_BUFFER_LIMIT_RED_THDORQEQ.OP_BUFFER_LIMIT_RED =
	   =CEILING(C310/8, 1) */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_THDORQEQr (unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_THDORQEQr, &rval,
		OP_BUFFER_LIMIT_REDf, sal_ceil_func(temp_val,8));
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_THDORQEQr (unit, rval));

	/* C311: OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQ.
	   OP_BUFFER_SHARED_LIMIT_RESUME =C226-CEILING(C82/4, 1) */
	temp_val -=  sal_ceil_func(general_info->total_num_of_ports_excl_lpbk_olp_cpu,4);
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr(
		    unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr, &rval,
		OP_BUFFER_SHARED_LIMIT_RESUMEf, temp_val);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr(
		    unit, rval));

	/* C313: OP_BUFFER_LIMIT_RESUME_YELLOW_THDORQEQ.
	   OP_BUFFER_LIMIT_RESUME_YELLOW = C312-1 */
	temp_val = sal_ceil_func(output_port_threshold->min_grntd_tot_shr_RE_WQs_cells , 8);
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_THDORQEQr (
		    unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_THDORQEQr, &rval,
		OP_BUFFER_LIMIT_RESUME_YELLOWf, temp_val-1);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_THDORQEQr (
		    unit, rval));

	/* C315: OP_BUFFER_LIMIT_RESUME_RED_THDORQEQ.
	   OP_BUFFER_LIMIT_RESUME_RED==C314 -1 */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_THDORQEQr (
		    unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_THDORQEQr, &rval,
		OP_BUFFER_LIMIT_RESUME_REDf, temp_val-1);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_THDORQEQr (
		    unit, rval));

	/* C317: OP_BUFFER_SHARED_LIMIT_QENTRY.OP_BUFFER_SHARED_LIMIT_QENTRY =C221*/
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_QENTRYr (unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_QENTRYr, &rval,
		OP_BUFFER_SHARED_LIMIT_QENTRYf,
		output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_QENTRYr (unit, rval));

	temp_val = output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs;

	/* C319: OP_BUFFER_LIMIT_YELLOW_QENTRY.OP_BUFFER_LIMIT_YELLOW_QENTRY
	   = CEILING(C317/8, 1)  */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_QENTRYr(unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_QENTRYr, &rval,
			  OP_BUFFER_LIMIT_YELLOW_QENTRYf, sal_ceil_func(temp_val,8));
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_QENTRYr(unit, rval));

	/* C321: OP_BUFFER_LIMIT_RED_QENTRY.OP_BUFFER_LIMIT_RED_QENTRY
	   = CEILING(C317/8, 1)  */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_QENTRYr(unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_QENTRYr, &rval,
		OP_BUFFER_LIMIT_RED_QENTRYf, sal_ceil_func(temp_val,8));
       
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_QENTRYr(unit, rval));

	/* C318: OP_BUFFER_SHARED_LIMIT_RESUME_QENTRY.OP_BUFFER_SHARED_LIMIT_RESUME_QENTRY
	   =C224-CEILING(C82/4, 1)*C51 */
	temp_val -= (sal_ceil_func(general_info->total_num_of_ports_excl_lpbk_olp_cpu,4) *
		_soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt);
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr(
		    unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr,
		&rval,
		OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYf, temp_val);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr(
		    unit, rval));

	/* C320 OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRY.OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRY
	   =CEILING(C318/8, 1) */
	SOC_IF_ERROR_RETURN(
		READ_OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYr(unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYr, &rval,
		OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYf,
		sal_ceil_func(temp_val,8));
	SOC_IF_ERROR_RETURN(
		WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYr(unit, rval));

	/* C322 OP_BUFFER_LIMIT_RESUME_RED_QENTRY.OP_BUFFER_LIMIT_RESUME_RED_QENTRY
	   =CEILING(C318/8, 1) */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_QENTRYr(unit, &rval));
	soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_QENTRYr, &rval,
		OP_BUFFER_LIMIT_RESUME_RED_QENTRYf,
		sal_ceil_func(temp_val,8));
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_QENTRYr(unit, rval));

	/* C324 : OP_BUFFER_SHARED_LIMIT_THDORDEQ.OP_BUFFER_SHARED_LIMIT = C229 */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_THDORDEQr(unit, &rval));
	soc_reg_field_set(unit,OP_BUFFER_SHARED_LIMIT_THDORDEQr,&rval,
		OP_BUFFER_SHARED_LIMITf,
		output_port_threshold->min_grntd_tot_shr_EP_redirect_queue_entry_cells);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_THDORDEQr(unit, rval));
	temp_val= output_port_threshold->
	    min_grntd_tot_shr_EP_redirect_queue_entry_cells;

	/*C325: OP_BUFFER_SHARED_LIMIT_TRESUME_THDORDEQr.
	  OP_BUFFER_SHARED_LIMIT_RESUMEf = =C229-4 */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_THDORDEQr(
		    unit, &rval));
	soc_reg_field_set(unit,OP_BUFFER_SHARED_LIMIT_RESUME_THDORDEQr,&rval,
		OP_BUFFER_SHARED_LIMIT_RESUMEf,
		temp_val - 4);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_THDORDEQr(
		    unit, rval));

	/* C326: OP_BUFFER_LIMIT_YELLOW_THDORDEQ.OP_BUFFER_LIMIT_YELLOW
	   = CEILING(C324/8, 1) */
	temp_val =  sal_ceil_func(temp_val , 8);
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_THDORDEQr(unit, &rval));
	soc_reg_field_set(unit,OP_BUFFER_LIMIT_YELLOW_THDORDEQr,&rval,
		OP_BUFFER_LIMIT_YELLOWf, temp_val);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_THDORDEQr(unit, rval));

	/* C328: OP_BUFFER_LIMIT_RED_THDORDEQ.OP_BUFFER_LIMIT_RED
	   = CEILING(C324/8, 1) */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_THDORDEQr(unit, &rval));
	soc_reg_field_set(unit,OP_BUFFER_LIMIT_RED_THDORDEQr,&rval,
		OP_BUFFER_LIMIT_REDf, temp_val);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_THDORDEQr(unit, rval));

	/* C327: OP_BUFFER_LIMIT_RESUME_YELLOW_THDORDEQ.
	   OP_BUFFER_LIMIT_RESUME_YELLOW = =C326-1 */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_THDORDEQr(
		    unit, &rval));
	soc_reg_field_set(unit,OP_BUFFER_LIMIT_RESUME_YELLOW_THDORDEQr,&rval,
		OP_BUFFER_LIMIT_RESUME_YELLOWf, temp_val-1);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_THDORDEQr(
		    unit, rval));

	/* C329: OP_BUFFER_LIMIT_RESUME_RED_THDORDEQ.
	   OP_BUFFER_LIMIT_RESUME_RED = =C328-1 */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_THDORDEQr(unit, &rval));
	soc_reg_field_set(unit,OP_BUFFER_LIMIT_RESUME_RED_THDORDEQr,&rval,
		OP_BUFFER_LIMIT_RESUME_REDf, temp_val-1);
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_THDORDEQr(unit, rval));
    }

    SOC_PBMP_ITER (valid_port_cpu_pbmp, port) {
        if (port >= ML_MAX_LOGICAL_PORTS) {
            continue;
        }
        if ((flex_port != -1) && (flex_port != port)) {
            continue;
        }

        sal_memset(&thdi_port_sp_config,0,sizeof(thdi_port_sp_config));
        mem_idx= ML_GET_THDI_PORT(port) * ML_MAX_SERVICE_POOLS;
        /* C335 : THDI_PORT_SP_CONFIG.PORT_SP_MIN_LIMIT = =$C$168 */
        soc_mem_field32_set(unit, THDI_PORT_SP_CONFIGm,
                &thdi_port_sp_config, PORT_SP_MIN_LIMITf,
                input_port_threshold->min_int_buff_cells_for_a_port);

        /* C336 : THDI_PORT_SP_CONFIG.PORT_SP_MAX_LIMIT = =$C$269 i.e. =$C$186 */
        soc_mem_field32_set(unit, THDI_PORT_SP_CONFIGm,
                &thdi_port_sp_config, PORT_SP_MAX_LIMITf,
                input_port_threshold->total_shared_ing_buff_pool);

        /* C337 : PORT_MAX_PKT_SIZE.PORT_MAX_PKT_SIZE = =$C$72 */
        SOC_IF_ERROR_RETURN(READ_PORT_MAX_PKT_SIZEr (unit, port, &rval));
        soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval,
                PORT_MAX_PKT_SIZEf,
                general_info->max_packet_size_in_cells);
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr (unit, port, rval));

        /* C338: THDI_PORT_SP_CONFIG.PORT_SP_RESUME_LIMIT = =C336-2*$C$76 */
        soc_mem_field32_set(unit, THDI_PORT_SP_CONFIGm,
                &thdi_port_sp_config, PORT_SP_RESUME_LIMITf,
                input_port_threshold->total_shared_ing_buff_pool -
                (2 *  general_info->ether_mtu_cells_for_int_buff));

        SOC_IF_ERROR_RETURN (soc_mem_write(
                    unit, THDI_PORT_SP_CONFIGm, MEM_BLOCK_ALL,
                    mem_idx, &thdi_port_sp_config));
        sal_memset(&thdi_port_pg_config,0,sizeof(thdi_port_pg_config));
        mem_idx= ML_GET_THDI_PORT_PG(port);

        /* C339: THDI_PORT_PG_CONFIG.PG_MIN_LIMIT (PG0) = =$C$156 */
        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm,
                &thdi_port_pg_config, PG_MIN_LIMITf,
                input_port_threshold->min_int_buff_cells_per_PG);

        /* C340: THDI_PORT_PG_CONFIG.PG_SHARED_LIMIT
           =IF($C$14, $C$61, $C$269)*/
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = _soc_ml_mmu_params.mmu_ing_pg_dyn_thd_param;
        } else {
            temp_val = input_port_threshold->total_shared_ing_buff_pool;
        }
        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm,
                &thdi_port_pg_config, PG_SHARED_LIMITf, temp_val);

        /* C341: THDI_PORT_PG_CONFIG.PG_RESET_OFFSET = 2*$C$76 */
        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm,
                &thdi_port_pg_config, PG_RESET_OFFSETf,
                (2 * general_info->ether_mtu_cells_for_int_buff));

        /* C342: THDI_PORT_PG_CONFIG.PG_RESET_FLOOR = 0 */
        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm,
                &thdi_port_pg_config, PG_RESET_FLOORf, 0);

        /* C343: THDI_PORT_PG_CONFIG.PG_SHARED_DYNAMIC= =IF($C$14, 1, 0) */
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = 1;
        } else {
            temp_val = 0;
        }
        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm,
                &thdi_port_pg_config, PG_SHARED_DYNAMICf,temp_val);

        /* C344: THDI_PORT_PG_CONFIG.PG_HDRM_LIMIT=
           For Ge:$C125 For Hg:$C115 For Cpu:$C145 */
        if (IS_CPU_PORT(unit, port)) {
            temp_val = input_port_threshold->hdrm_int_buff_cells_for_cpu_port;
        } else if (IS_GE_PORT(unit, port) || IS_LB_PORT(unit,port)) {
            temp_val = input_port_threshold->hdrm_int_buff_cells_for_1G_PG;
        } else if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit,port) ) {
            temp_val = input_port_threshold->hdrm_int_buff_cells_for_10G_PG;
        }
        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm,
                &thdi_port_pg_config, PG_HDRM_LIMITf,temp_val);

        /* C345 : THDI_PORT_PG_CONFIG.PG_GBL_HDRM_EN = for lossy=1 else 0 */
        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm,
                &thdi_port_pg_config, PG_GBL_HDRM_ENf ,1);

        /* C346: THDI_PORT_PG_CONFIG.SP_SHARED_MAX_ENABLE =
           For Ge:1 For Hg,Cpu: Ge Value only!!! */

        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm,
                &thdi_port_pg_config, SP_SHARED_MAX_ENABLEf ,1);

        /* C347: THDI_PORT_PG_CONFIG.SP_MIN_PG_ENABLE =
           For Ge:1 For Hg,Cpu: Ge Value only!!! */
        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm,
                &thdi_port_pg_config, SP_MIN_PG_ENABLEf ,1);

        SOC_IF_ERROR_RETURN (soc_mem_write(
                    unit, THDI_PORT_PG_CONFIGm, MEM_BLOCK_ALL,
                    mem_idx, &thdi_port_pg_config));

        sal_memset(&thdirqe_thdi_port_sp_config , 0,
                sizeof(thdirqe_thdi_port_sp_config));

        mem_idx= ML_GET_THDI_PORT(port) * ML_MAX_SERVICE_POOLS;
        /* C380: THDIRQE_THDI_PORT_SP_CONFIG.PORT_SP_MIN_LIMIT  = $C$171 */
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_SP_CONFIGm,
                &thdirqe_thdi_port_sp_config, PORT_SP_MIN_LIMITf,
                input_port_threshold->min_RE_WQEs_pkts_for_a_port);

        /* C381: THDIRQE_THDI_PORT_SP_CONFIG.PORT_SP_MAX_LIMIT = $C$189 */
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_SP_CONFIGm,
                &thdirqe_thdi_port_sp_config, PORT_SP_MAX_LIMITf,
                input_port_threshold->total_shared_RE_WQEs_buff);

        /* C382: THDIRQE_THDI_PORT_SP_CONFIG.PORT_SP_RESUME_LIMIT =
           =C381-2 */
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_SP_CONFIGm,
                &thdirqe_thdi_port_sp_config, PORT_SP_RESUME_LIMITf,
                input_port_threshold->total_shared_RE_WQEs_buff - 2);
        SOC_IF_ERROR_RETURN(soc_mem_write(
                    unit, THDIRQE_THDI_PORT_SP_CONFIGm, MEM_BLOCK_ALL,
                    mem_idx, &thdirqe_thdi_port_sp_config));

        sal_memset(&thdirqe_thdi_port_pg_config , 0,
                sizeof(thdirqe_thdi_port_pg_config));
        mem_idx= ML_GET_THDI_PORT_PG(port);
        /* C383: THDIRQE_THDI_PORT_PG_CONFIG.PG_MIN_LIMIT(PG0) = =$C$159 */
        temp_val = input_port_threshold->min_RE_WQEs_pkt_per_PG;
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config, PG_MIN_LIMITf,temp_val);

        /* C384: THDIRQE_THDI_PORT_PG_CONFIG.PG_SHARED_LIMIT =
           =IF($C$14, $C$61, $C$189) */
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = _soc_ml_mmu_params.mmu_ing_pg_dyn_thd_param;
        } else {
            temp_val = input_port_threshold->total_shared_RE_WQEs_buff;
        }
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config,
                PG_SHARED_LIMITf, temp_val);

        /* C385: THDIRQE_THDI_PORT_PG_CONFIG.PG_RESET_OFFSET = 2 */
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config, PG_RESET_OFFSETf,2);

        /* C386: THDIRQE_THDI_PORT_PG_CONFIG.PG_RESET_FLOOR = 0 */
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config, PG_RESET_FLOORf, 0);

        /* C387: THDIRQE_THDI_PORT_PG_CONFIG.PG_SHARED_DYNAMIC=
           =IF($C$14, 1, 0) */
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = 1;
        } else {
            temp_val = 0;
        }
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config, PG_SHARED_DYNAMICf,
                temp_val);

        /* C388: THDIRQE_THDI_PORT_PG_CONFIG.PG_HDRM_LIMIT
           For Ge:$C128 For Hg:$C118 For Cpu:$C148 */
        if (IS_CPU_PORT(unit, port)) {
            temp_val = input_port_threshold->hdrm_RE_WQEs_pkts_for_cpu_port;
        } else if (IS_GE_PORT(unit, port) || IS_LB_PORT(unit,port)) {
            temp_val = input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG;
        } else if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
            temp_val= input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG;
        }
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config, PG_HDRM_LIMITf,temp_val);

        /* C389: THDIRQE_THDI_PORT_PG_CONFIG.PG_GBL_HDRM_EN = 0 */
        temp_val = 0;
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config, PG_GBL_HDRM_ENf ,
                temp_val);

        /* C390: THDIRQE_THDI_PORT_PG_CONFIG.SP_SHARED_MAX_ENABLE =
           For Ge:1 For Hg,Cpu: Ge Value only!!! */
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config, SP_SHARED_MAX_ENABLEf ,1);

        /* C391: THDIRQE_THDI_PORT_PG_CONFIG.SP_MIN_PG_ENABLE =
           For Ge:1 For Hg,Cpu: Ge Value only!!! */
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config, SP_MIN_PG_ENABLEf ,1);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                    MEM_BLOCK_ALL, mem_idx, &thdirqe_thdi_port_pg_config));

        sal_memset(&thdiqen_thdi_port_sp_config,0,
                sizeof(thdiqen_thdi_port_sp_config));
        mem_idx= ML_GET_THDI_PORT(port) * ML_MAX_SERVICE_POOLS;

        /* C394: THDIQEN_THDI_PORT_SP_CONFIG.PORT_SP_MIN_LIMIT  = $C$172 */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_SP_CONFIGm,
                &thdiqen_thdi_port_sp_config, PORT_SP_MIN_LIMITf,
                input_port_threshold->min_EQEs_pkts_for_a_port);

        /* C395: THDIQEN_THDI_PORT_SP_CONFIG.PORT_SP_MAX_LIMIT = $C$190 */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_SP_CONFIGm,
                &thdiqen_thdi_port_sp_config, PORT_SP_MAX_LIMITf,
                input_port_threshold->total_shared_EQEs_buff);

        /* C396: THDIQEN_THDI_PORT_SP_CONFIG.PORT_SP_RESUME_LIMIT =
           ==C395-2*C51 */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_SP_CONFIGm,
                &thdiqen_thdi_port_sp_config, PORT_SP_RESUME_LIMITf,
                input_port_threshold->total_shared_EQEs_buff -
                (2 * _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit,
                    THDIQEN_THDI_PORT_SP_CONFIGm, MEM_BLOCK_ALL,
                    mem_idx, &thdiqen_thdi_port_sp_config));

        sal_memset(&thdiqen_thdi_port_pg_config,0,
                sizeof(thdiqen_thdi_port_pg_config));
        mem_idx= ML_GET_THDI_PORT_PG(port);
        /* C397: THDIQEN_THDI_PORT_PG_CONFIG.PG_MIN_LIMIT(PG0) =
           =$C$160 */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, PG_MIN_LIMITf,
                input_port_threshold->min_EQEs_pkt_per_PG);


        /* C398: THDIQEN_THDI_PORT_PG_CONFIG.PG_SHARED_LIMIT =
           =IF($C$14, $C$61, $C$190) */
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = _soc_ml_mmu_params.mmu_ing_pg_dyn_thd_param;
        } else {
            temp_val = input_port_threshold->total_shared_EQEs_buff;
        }
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config,
                PG_SHARED_LIMITf, temp_val);

        /* C399: THDIQEN_THDI_PORT_PG_CONFIG.PG_RESET_OFFSET = =2*C51 */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, PG_RESET_OFFSETf,
                2 * _soc_ml_mmu_params.mmu_exp_num_of_repl_per_pkt);

        /* C400: THDIQEN_THDI_PORT_PG_CONFIG.PG_RESET_FLOOR = 0 */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, PG_RESET_FLOORf, 0);

        /* C401: THDIQEN_THDI_PORT_PG_CONFIG.PG_SHARED_DYNAMIC= 0 */
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = 1;
        } else {
            temp_val = 0;
        }
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
               &thdiqen_thdi_port_pg_config, PG_SHARED_DYNAMICf,
                temp_val);

        /* C402: THDIQEN_THDI_PORT_PG_CONFIG.PG_HDRM_LIMIT
           For Ge:$C129 For Hg:$C119 For Cpu:$C149 */
        if (IS_CPU_PORT(unit, port)) {
            temp_val = input_port_threshold->hdrm_EQEs_pkts_for_cpu_port;
        } else if (IS_GE_PORT(unit, port) || IS_LB_PORT(unit,port)) {
            temp_val = input_port_threshold->hdrm_EQEs_pkts_for_1G_PG;
        } else if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
            temp_val= input_port_threshold->hdrm_EQEs_pkts_for_10G_PG;
        }
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, PG_HDRM_LIMITf,temp_val);

        /* C403: THDIQEN_THDI_PORT_PG_CONFIG.PG_GBL_HDRM_EN =0 */
        temp_val = 0;
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, PG_GBL_HDRM_ENf ,
                temp_val);

        /* C404: THDIQEN_THDI_PORT_PG_CONFIG.SP_SHARED_MAX_ENABLE =
           For Ge:1 For Hg,Cpu: Ge Value only!!! */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, SP_SHARED_MAX_ENABLEf ,1);

        /* C405: THDIQEN_THDI_PORT_PG_CONFIG.SP_MIN_PG_ENABLE =
           For Ge:1 For Hg,Cpu: Ge Value only!!! */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, SP_MIN_PG_ENABLEf ,1);
        SOC_IF_ERROR_RETURN(soc_mem_write( unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                    MEM_BLOCK_ALL, mem_idx, &thdiqen_thdi_port_pg_config));

        if (!SOC_PBMP_MEMBER(PBMP_EXT_MEM (unit), port) &&
                (port >= si->cmic_port) && (port <= si->lb_port)) {
            op_node = ( si->port_cosq_base[port] / 8);
            for (op_node_offset = 0;
                    op_node_offset < sal_ceil_func(si->port_num_uc_cosq[port],8);
                    op_node_offset++) {
                /* C409:THDO_OPNCONFIG_CELL.OPN_SHARED_LIMIT_CELL = =$C$222 */
                soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                        &mmu_thdo_opnconfig_cell_entry,
                        OPN_SHARED_LIMIT_CELLf,
                        output_port_threshold->
                        min_grntd_tot_shr_queue_cells_int_buff);

                /* C410:THDO_OPNCONFIG_CELL.OP_SHARED_RESET_VALUE_CELL
                   = =C409-2*$C$76 */
                temp_val = output_port_threshold->
                    min_grntd_tot_shr_queue_cells_int_buff -
                    (2 * general_info->ether_mtu_cells_for_int_buff);
                soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                        &mmu_thdo_opnconfig_cell_entry,
                        OPN_SHARED_RESET_VALUE_CELLf, temp_val);

                /* C411:THDO_OPNCONFIG_CELL.PORT_LIMIT_ENABLE_CELL
                   = 0 */
                soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                        &mmu_thdo_opnconfig_cell_entry,
                        PORT_LIMIT_ENABLE_CELLf, 0);
                soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                        &mmu_thdo_opnconfig_cell_entry,
                        PIDf, port);
                SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY,op_node + op_node_offset,
                            &mmu_thdo_opnconfig_cell_entry));
            }

            for (queue = si->port_uc_cosq_base[port];
                    queue < si->port_uc_cosq_base[port] +
                    si->port_num_uc_cosq[port];
                    queue++) {
                SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY,queue,
                            &mmu_thdo_qconfig_cell_entry));
                /* C412:THDO_QCONFIG_CELL.Q_MIN_CELL
                   IF(C11=0, C38, 0)  for base queue*/
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry, Q_MIN_CELLf,
                        queue < general_info->total_egr_base_queues_in_device ?
                        output_port_threshold->min_grntd_res_queue_cells_int_buff: 0);

                /* C414:THDO_QCONFIG_CELL.Q_SHARED_LIMIT_CELL
                   =IF($C$14,$C$222,$C$63) */
                if (_soc_ml_mmu_params.lossless_mode_d_c) {
                    temp_val = output_port_threshold->
                        min_grntd_tot_shr_queue_cells_int_buff;
                } else {
                    temp_val = _soc_ml_mmu_params.
                        mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
                }
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry, Q_SHARED_LIMIT_CELLf,
                        temp_val);

                /* C416:THDO_QCONFIG_CELL.Q_LIMIT_DYNAMIC_CELL
                   =IF($C$14, 0, 1) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry, Q_LIMIT_DYNAMIC_CELLf,
                        _soc_ml_mmu_params.lossless_mode_d_c ? 0 : 1);

                /* C417:THDO_QCONFIG_CELL.Q_LIMIT_ENABLE_CELL
                   =IF($C$14, 0, 1) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry, Q_LIMIT_ENABLE_CELLf,
                        _soc_ml_mmu_params.lossless_mode_d_c ? 0 : 1);

                /* C418:THDO_QCONFIG_CELL.Q_COLOR_ENABLE_CELL = 0 */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry, Q_COLOR_ENABLE_CELLf,0);

                /* C419: THDO_QCONFIG_CELL.Q_COLOR_LIMIT_DYNAMIC_CELL
                   =IF($C$14, 0, 1) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry, Q_COLOR_LIMIT_DYNAMIC_CELLf,
                        _soc_ml_mmu_params.lossless_mode_d_c ? 0 : 1);

                /* C420: THDO_QCONFIG_CELL.LIMIT_YELLOW_CELL= 0 */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry, LIMIT_YELLOW_CELLf, 0);

                SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY,queue,
                            &mmu_thdo_qconfig_cell_entry));

                SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_CELLm(unit,
                            MEM_BLOCK_ANY,queue,
                            &mmu_thdo_qoffset_cell_entry));
                /* C415:THDO_QOFFSET_CELL.RESET_OFFSET_CELL =2 */
                soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm,
                        &mmu_thdo_qoffset_cell_entry, RESET_OFFSET_CELLf, 2);

                /* C421: THDO_QOFFSET_CELL.LIMIT_RED_CELL  = */
                soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm,
                        &mmu_thdo_qoffset_cell_entry, LIMIT_RED_CELLf, 0);

                /* C422: THDO_QOFFSET_CELL.RESET_OFFSET_YELLOW_CELL =2 */
                soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm,
                        &mmu_thdo_qoffset_cell_entry, RESET_OFFSET_YELLOW_CELLf,2);

                /* C423: THDO_QOFFSET_CELL.RESET_OFFSET_RED_CELL =2 */
                soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm,
                        &mmu_thdo_qoffset_cell_entry, RESET_OFFSET_RED_CELLf,2);

                SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_CELLm(unit,
                            MEM_BLOCK_ANY,queue,
                            &mmu_thdo_qoffset_cell_entry));
            }
        }
        for (op_node_offset = 0;
                op_node_offset < sal_ceil_func(si->port_num_uc_cosq[port],8);
                op_node_offset++) {
            SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_QENTRYm(unit,
                        MEM_BLOCK_ANY,op_node + op_node_offset,
                        &mmu_thdo_opnconfig_qentry_entry));
            /* C452:THDO_OPNCONFIG_QENTRY.OPN_SHARED_LIMIT_QENTRY = =$C$224 */
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                    &mmu_thdo_opnconfig_qentry_entry, OPN_SHARED_LIMIT_QENTRYf,
                    output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs);

            /* C453:THDO_OPNCONFIG_QENTRY.OP_SHARED_RESET_VALUE_QENTRY
               =C452-2 */
            temp_val = output_port_threshold->
                min_grntd_tot_shr_queue_cells_EQEs - 2;
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                    &mmu_thdo_opnconfig_qentry_entry,
                    OPN_SHARED_RESET_VALUE_QENTRYf, temp_val);

            /* C454:THDO_OPNCONFIG_QENTRY.PORT_LIMIT_ENABLE_QENTRY
               = 0 */
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                    &mmu_thdo_opnconfig_qentry_entry,
                    PORT_LIMIT_ENABLE_QENTRYf, 0);
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                    &mmu_thdo_opnconfig_qentry_entry,
                    PIDf, port);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_QENTRYm(unit,
                        MEM_BLOCK_ANY,op_node + op_node_offset,
                        &mmu_thdo_opnconfig_qentry_entry));
        }
        for (queue = si->port_uc_cosq_base[port];
                queue < si->port_uc_cosq_base[port] + si->port_num_uc_cosq[port];
                queue++) {


            SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_QENTRYm(unit,
                        MEM_BLOCK_ANY,queue,
                        &mmu_thdo_qconfig_qentry_entry));
            /* C455:THDO_QCONFIG_QENTRY.Q_MIN_QENTRY = =$C$206 */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                    &mmu_thdo_qconfig_qentry_entry, Q_MIN_QENTRYf,
                    queue < general_info->total_egr_base_queues_in_device ?
                    output_port_threshold->min_grntd_res_queue_cells_EQEs : 0);

            /* C456:THDO_QCONFIG_QENTRY.Q_SHARED_LIMIT_QENTRY
               =IF($C$14,$C$224,$C$63) */
            if (_soc_ml_mmu_params.lossless_mode_d_c) {
                temp_val = output_port_threshold->
                    min_grntd_tot_shr_queue_cells_EQEs;
            } else {
                temp_val = _soc_ml_mmu_params.
                    mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
            }
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                    &mmu_thdo_qconfig_qentry_entry, Q_SHARED_LIMIT_QENTRYf, temp_val);

            /* C458:THDO_QCONFIG_QENTRY.Q_LIMIT_DYNAMIC_QENTRY
               =IF($C$14, 0, 1) */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                    &mmu_thdo_qconfig_qentry_entry, Q_LIMIT_DYNAMIC_QENTRYf,
                    _soc_ml_mmu_params.lossless_mode_d_c ? 0 : 1);

            /* C459:THDO_QCONFIG_QENTRY.Q_LIMIT_ENABLE_QENTRY
               =IF($C$14, 0, 1) */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                    &mmu_thdo_qconfig_qentry_entry, Q_LIMIT_ENABLE_QENTRYf,
                    _soc_ml_mmu_params.lossless_mode_d_c ? 0 : 1);

            /* C460:THDO_QCONFIG_QENTRY.Q_COLOR_ENABLE_QENTRY = 0 */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                    &mmu_thdo_qconfig_qentry_entry, Q_COLOR_ENABLE_QENTRYf,0);

            /* C461: THDO_QCONFIG_QENTRY.Q_COLOR_LIMIT_DYNAMIC_QENTRY
               =IF($C$14, 0, 1) */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                    &mmu_thdo_qconfig_qentry_entry,
                    Q_COLOR_LIMIT_DYNAMIC_QENTRYf,
                    _soc_ml_mmu_params.lossless_mode_d_c ? 0 : 1);

            /* C462: THDO_QCONFIG_QENTRY.LIMIT_YELLOW_QENTRY= 0 */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                    &mmu_thdo_qconfig_qentry_entry, LIMIT_YELLOW_QENTRYf,
                     0);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_QENTRYm(unit,
                        MEM_BLOCK_ANY,queue,
                        &mmu_thdo_qconfig_qentry_entry));

            SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_QENTRYm(unit,
                        MEM_BLOCK_ANY,queue,
                        &mmu_thdo_qoffset_qentry_entry));
            /* C457:THDO_QOFFSET_QENTRY.RESET_OFFSET_QENTRY =1 */
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm,
                    &mmu_thdo_qoffset_qentry_entry, RESET_OFFSET_QENTRYf, 1);

            /* C463: THDO_QOFFSET_QENTRY.LIMIT_RED_QENTRY  = 0 */
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm,
                    &mmu_thdo_qoffset_qentry_entry, LIMIT_RED_QENTRYf,
                    0);

            /* C464: THDO_QOFFSET_QENTRY.RESET_OFFSET_YELLOW_QENTRY =1 */
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm,
                    &mmu_thdo_qoffset_qentry_entry,RESET_OFFSET_YELLOW_QENTRYf,1);
            /* C465: THDO_QOFFSET_QENTRY.RESET_OFFSET_RED_QENTRY =1 */
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm,
                    &mmu_thdo_qoffset_qentry_entry, RESET_OFFSET_RED_QENTRYf,1);

            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_QENTRYm(unit,
                        MEM_BLOCK_ANY,queue,
                        &mmu_thdo_qoffset_qentry_entry));
        }
    }
    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEQr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEQr(
                    unit, cos, &rval));
        /* C467:OP_QUEUE_CONFIG1_THDORQEQ.Q_MIN = C208 */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval,
                Q_MINf,
                output_port_threshold->min_grntd_res_RE_WQs_cells);

        /* C472:OP_QUEUE_CONFIG1_THDORQEQ.Q_COLOR_ENABLE  = 0 */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval,
                Q_COLOR_ENABLEf, 0);

        /* C473:OP_QUEUE_CONFIG1_THDORQEQ.Q_COLOR_DYNAMIC lossless=0 else 1
           =IF(C14=0, 1, 0) */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval,
                Q_COLOR_DYNAMICf,
                _soc_ml_mmu_params.lossless_mode_d_c? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEQr(
                    unit, cos, rval));
    }

    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEQr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEQr(
                    unit, cos, &rval));
        /* C468:OP_QUEUE_CONFIG_THDORQEQ.Q_SHARED_LIMIT
           ==IF($C$14,$C$226,$C$63) */
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = output_port_threshold->
                min_grntd_tot_shr_RE_WQs_cells ;
        } else {
            temp_val =
                _soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
        }

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval,
                Q_SHARED_LIMITf, temp_val);

        /*C470: OP_QUEUE_CONFIG_THDORQEQ.Q_LIMIT_DYNAMIC:=lossless=0 else 1
          =IF(C14=0, 1, 0) */

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval,
                Q_LIMIT_DYNAMICf,
                _soc_ml_mmu_params.lossless_mode_d_c? 0 : 1);

        /*C471:OP_QUEUE_CONFIG_THDORQEQ.Q_LIMIT_ENABLE:=lossless=0 else 1*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval,
                Q_LIMIT_ENABLEf,
                _soc_ml_mmu_params.lossless_mode_d_c? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEQr(
                    unit, cos, rval));

    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEQr); cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_THDORQEQr(unit, cos,&rval));
        /*C469: OP_QUEUE_RESET_OFFSET_THDORQEQ.Q_RESET_OFFSET == 1 */
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEQr, &rval,
                Q_RESET_OFFSETf, 1);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_THDORQEQr(unit, cos,rval));
    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEQr);
            cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_YELLOW_THDORQEQr(
                    unit, cos,&rval));
        /* C474: OP_QUEUE_LIMIT_YELLOW_THDORQEQ.Q_LIMIT_YELLOW 0 */
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEQr, &rval,
                Q_LIMIT_YELLOWf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_YELLOW_THDORQEQr(
                    unit, cos,rval));
    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDORQEQr);
            cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_RED_THDORQEQr(
                    unit, cos,&rval));
        /* C475: OP_QUEUE_LIMIT_RED_THDORQEQ.Q_LIMIT_RED =0 */
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDORQEQr, &rval,
                Q_LIMIT_REDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_RED_THDORQEQr(
                    unit, cos,rval));
    }
    for(cos=0;
            cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEQr);
            cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEQr(unit, cos,&rval));
        /*C476:OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEQ.RESUME_OFFSET_YELLOW=1*/
        soc_reg_field_set(unit,
                OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEQr, &rval,
                RESUME_OFFSET_YELLOWf, 1);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEQr(unit, cos,rval));
    }
    for(cos=0;
            cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_RED_THDORQEQr);
            cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_RED_THDORQEQr(unit, cos,&rval));
        /*C477:OP_QUEUE_RESET_OFFSET_RED_THDORQEQ.RESUME_OFFSET_RED=1*/
        soc_reg_field_set(unit,
                OP_QUEUE_RESET_OFFSET_RED_THDORQEQr, &rval,
                RESUME_OFFSET_REDf, 1);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_RED_THDORQEQr(unit, cos,rval));
    }
    /* 2.4.2 RQEI */
    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEIr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEIr(
                    unit, cos, &rval));
        /* C478:OP_QUEUE_CONFIG1_THDORQEI.Q_MIN = C209 */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval, Q_MINf,
                output_port_threshold->min_grntd_res_RE_WQs_queue_cells_for_int_buff);

        /* C483:OP_QUEUE_CONFIG1_THDORQEI.Q_COLOR_ENABLE  = 0 */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval,
                Q_COLOR_ENABLEf, 0);

        /* C484:OP_QUEUE_CONFIG1_THDORQEI.Q_COLOR_DYNAMIC lossless=0 else 1
           =IF($C$14, 0, 1)*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval,
                Q_COLOR_DYNAMICf,
                _soc_ml_mmu_params.lossless_mode_d_c? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEIr(
                    unit, cos, rval));
    }

    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEIr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEIr(
                    unit, cos, &rval));
        /* C479:OP_QUEUE_CONFIG_THDORQEI.Q_SHARED_LIMIT
           =IF($C$14,$C$227,$C$63)..Can be put outside loop */
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = output_port_threshold->min_grntd_tot_shr_RE_WQs_queue_cells_for_int_buff;
        } else {
            temp_val = _soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
        }
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval,
                Q_SHARED_LIMITf, temp_val);

        /*C481: OP_QUEUE_CONFIG_THDORQEI.Q_LIMIT_DYNAMIC:=lossless=0 else 1
          =IF($C$14, 0, 1)*/

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval,
                Q_LIMIT_DYNAMICf,
                _soc_ml_mmu_params.lossless_mode_d_c? 0 : 1);

        /*C482:OP_QUEUE_CONFIG_THDORQEI.Q_LIMIT_ENABLE:=lossless=0 else 1
          =IF($C$14, 0, 1) */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval,
                Q_LIMIT_ENABLEf,
                _soc_ml_mmu_params.lossless_mode_d_c? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEIr(
                    unit, cos, rval));

    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEIr); cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_THDORQEIr(unit, cos,&rval));
        /*C480: OP_QUEUE_RESET_OFFSET_THDORQEI.Q_RESET_OFFSET == 2 */
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEIr, &rval,
                Q_RESET_OFFSETf, 2);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_THDORQEIr(unit, cos,rval));
    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEIr);
            cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_YELLOW_THDORQEIr(
                    unit, cos,&rval));
        /* C485: OP_QUEUE_LIMIT_YELLOW_THDORQEI.Q_LIMIT_YELLOW =0 */
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEIr, &rval,
                Q_LIMIT_YELLOWf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_YELLOW_THDORQEIr(
                    unit, cos,rval));
    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDORQEIr);
            cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_RED_THDORQEIr(
                    unit, cos,&rval));
        /* C486: OP_QUEUE_LIMIT_RED_THDORQEI.Q_LIMIT_RED = 0*/
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDORQEIr, &rval,
                Q_LIMIT_REDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_RED_THDORQEIr(
                    unit, cos,rval));
    }
    for(cos=0;
            cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEIr);
            cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEIr(unit, cos,&rval));
        /*C487:OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEI.RESUME_OFFSET_YELLOW=2*/
        soc_reg_field_set(unit,
                OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEIr, &rval,
                RESUME_OFFSET_YELLOWf, 2);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEIr(unit, cos,rval));
    }
    for(cos=0;
            cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_RED_THDORQEIr);
            cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_RED_THDORQEIr(unit, cos,&rval));
        /*C488:OP_QUEUE_RESET_OFFSET_RED_THDORQEI.RESUME_OFFSET_RED=2*/
        soc_reg_field_set(unit,
                OP_QUEUE_RESET_OFFSET_RED_THDORQEIr, &rval,
                RESUME_OFFSET_REDf, 2);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_RED_THDORQEIr(unit, cos,rval));
    }
    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORDEQr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORDEQr(
                    unit, cos, &rval));
        /* C501:OP_QUEUE_CONFIG1_THDORDEQ.Q_MIN = C211 */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORDEQr, &rval, Q_MINf,
                output_port_threshold->min_grntd_res_EP_redirect_queue_entry_cells);

        /* C506:OP_QUEUE_CONFIG1_THDORDEQ.Q_COLOR_ENABLE  = 0 */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORDEQr, &rval,
                Q_COLOR_ENABLEf, 0);

        /* C507:OP_QUEUE_CONFIG1_THDORDEQ.Q_COLOR_DYNAMIC lossless=0 else 1
           =IF(C14=0, 1, 0)*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORDEQr, &rval,
                Q_COLOR_DYNAMICf,
                _soc_ml_mmu_params.lossless_mode_d_c? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORDEQr(
                    unit, cos, rval));
    }

    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORDEQr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORDEQr(
                    unit, cos, &rval));
        /* C502:OP_QUEUE_CONFIG_THDORDEQ.Q_SHARED_LIMIT
           =IF(C14=0, C63, C229)..Can be put outside loop */
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = output_port_threshold->min_grntd_tot_shr_EP_redirect_queue_entry_cells;
        } else {
            temp_val = _soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
        }
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEQr, &rval,
                Q_SHARED_LIMITf, temp_val);

        /*C504: OP_QUEUE_CONFIG_THDORDEQ.Q_LIMIT_DYNAMIC:=lossless=0 else 1i
          =IF(C14=0, 1, 0)*/

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEQr, &rval,
                Q_LIMIT_DYNAMICf,
                _soc_ml_mmu_params.lossless_mode_d_c? 0 : 1);

        /*C505:OP_QUEUE_CONFIG_THDORDEQ.Q_LIMIT_ENABLE:=lossless=0 else 1
          =IF(C14=0, 1, 0)*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEQr, &rval,
                Q_LIMIT_ENABLEf,
                _soc_ml_mmu_params.lossless_mode_d_c? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORDEQr(
                    unit, cos, rval));

    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORDEQr); cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_THDORDEQr(unit, cos,&rval));
        /*C503: OP_QUEUE_RESET_OFFSET_THDORDEQ.Q_RESET_OFFSET == 1 */
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORDEQr, &rval,
                Q_RESET_OFFSETf, 1);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_THDORDEQr(unit, cos,rval));
    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_YELLOW_THDORDEQr);
            cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_YELLOW_THDORDEQr(
                    unit, cos,&rval));
        /* C508: OP_QUEUE_LIMIT_YELLOW_THDORDEQ.Q_LIMIT_YELLOW = 0 */
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDORDEQr, &rval,
                Q_LIMIT_YELLOWf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_YELLOW_THDORDEQr(
                    unit, cos,rval));
    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDORDEQr);
            cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_RED_THDORDEQr(
                    unit, cos,&rval));
        /* C509: OP_QUEUE_LIMIT_RED_THDORDEQ.Q_LIMIT_RED = 0 */
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDORDEQr, &rval,
                Q_LIMIT_REDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_RED_THDORDEQr(
                    unit, cos,rval));
    }
    for(cos=0;
            cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEQr);
            cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEQr(unit, cos,&rval));
        /*C510:OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEQ.RESUME_OFFSET_YELLOW=1*/
        soc_reg_field_set(unit,
                OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEQr, &rval,
                RESUME_OFFSET_YELLOWf, 1);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEQr(unit, cos,rval));
    }
    for(cos=0;
            cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_RED_THDORDEQr);
            cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_RED_THDORDEQr(unit, cos,&rval));
        /*C511:OP_QUEUE_RESET_OFFSET_RED_THDORDEQ.RESUME_OFFSET_RED=1*/
        soc_reg_field_set(unit,
                OP_QUEUE_RESET_OFFSET_RED_THDORDEQr, &rval,
                RESUME_OFFSET_REDf, 1);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_RED_THDORDEQr(unit, cos,rval));
    }
    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORDEIr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORDEIr(
                    unit, cos, &rval));
        /* C512:OP_QUEUE_CONFIG1_THDORDEI.Q_MIN = 9 (C204) */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORDEIr, &rval,
                Q_MINf, output_port_threshold->min_grntd_res_queue_cells_int_buff);

        /* C517:OP_QUEUE_CONFIG1_THDORDEI.Q_COLOR_ENABLE  = 0 */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORDEIr, &rval,
                Q_COLOR_ENABLEf, 0);

        /* C518:OP_QUEUE_CONFIG1_THDORDEI.Q_COLOR_DYNAMIC = 1*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORDEIr, &rval,
                Q_COLOR_DYNAMICf, _soc_ml_mmu_params.lossless_mode_d_c? 0 :1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORDEIr(
                    unit, cos, rval));
    }

    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORDEIr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORDEIr(
                    unit, cos, &rval));
        /* C513:OP_QUEUE_CONFIG_THDORDEI.Q_SHARED_LIMIT =
           =IF(C14=0, C63, C222)*/
        if (_soc_ml_mmu_params.lossless_mode_d_c) {
            temp_val = output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff;
        } else {
            temp_val = _soc_ml_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
        }

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEIr, &rval,
                Q_SHARED_LIMITf, temp_val);

        /*C515: OP_QUEUE_CONFIG_THDORDEI.Q_LIMIT_DYNAMIC:=1
          =IF(C14=0, 1, 0)*/

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEIr, &rval,
                Q_LIMIT_DYNAMICf,
                _soc_ml_mmu_params.lossless_mode_d_c ? 0 :1);

        /*C516:OP_QUEUE_CONFIG_THDORDEI.Q_LIMIT_ENABLE:
          =IF(C14=0, 1, 0)*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEIr, &rval,
                Q_LIMIT_ENABLEf,
                _soc_ml_mmu_params.lossless_mode_d_c ? 0 :1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORDEIr(
                    unit, cos, rval));
    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORDEIr); cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_THDORDEIr(unit, cos,&rval));
        /*C514: OP_QUEUE_RESET_OFFSET_THDORDEI.Q_RESET_OFFSET == 2 */
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORDEIr, &rval,
                Q_RESET_OFFSETf, 2);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_THDORDEIr(unit, cos,rval));
    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_YELLOW_THDORDEIr);
            cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_YELLOW_THDORDEIr(
                    unit, cos,&rval));
        /* C519: OP_QUEUE_LIMIT_YELLOW_THDORDEI.Q_LIMIT_YELLOW = 0 */
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORDEIr(
                    unit, cos, &rval));
        temp_val = soc_reg_field_get(unit, OP_QUEUE_CONFIG_THDORDEIr, rval,
                Q_SHARED_LIMITf);

        soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDORDEIr, &rval,
                Q_LIMIT_YELLOWf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_YELLOW_THDORDEIr(
                    unit, cos,rval));
    }
    for(cos=0;
            cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDORDEIr);
            cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_RED_THDORDEIr(
                    unit, cos,&rval));
        /* C520: OP_QUEUE_LIMIT_RED_THDORDEI.Q_LIMIT_RED = C486 */
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORDEIr(
                    unit, cos, &rval));
        temp_val = soc_reg_field_get(unit, OP_QUEUE_CONFIG_THDORDEIr, rval,
                Q_SHARED_LIMITf);

        soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDORDEIr, &rval,
                Q_LIMIT_REDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_RED_THDORDEIr(
                    unit, cos,rval));
    }
    for(cos=0;
            cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEIr);
            cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEIr(unit, cos,&rval));
        /*C521:OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEI.RESUME_OFFSET_YELLOW=2*/
        soc_reg_field_set(unit,
                OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEIr, &rval,
                RESUME_OFFSET_YELLOWf, 2);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEIr(unit, cos,rval));
    }
    for(cos=0;
            cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_RED_THDORDEIr);
            cos++) {
        SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_RESET_OFFSET_RED_THDORDEIr(unit, cos,&rval));
        /*C522:OP_QUEUE_RESET_OFFSET_RED_THDORDEI.RESUME_OFFSET_RED=2*/
        soc_reg_field_set(unit,
                OP_QUEUE_RESET_OFFSET_RED_THDORDEIr, &rval,
                RESUME_OFFSET_REDf, 2);
        SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_RESET_OFFSET_RED_THDORDEIr(unit, cos,rval));
    }
/* Enable back pressure status from MMU for lossless mode */
    if (_soc_ml_mmu_params.lossless_mode_d_c) {
        val = 0x3ffffffe;               /* 0 to 29 ports */
        sal_memset (&rval, 0, sizeof(rval));
        soc_reg_field_set(unit, THDIQEN_PORT_PAUSE_ENABLE_64r, &rval,
                PORT_PAUSE_ENABLEf, val);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDIQEN_PORT_PAUSE_ENABLE_64r,REG_PORT_ANY,
                           0, rval));

        sal_memset (&rval, 0, sizeof(rval));
        soc_reg_field_set(unit, THDIRQE_PORT_PAUSE_ENABLE_64r, &rval,
                PORT_PAUSE_ENABLEf, val);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDIRQE_PORT_PAUSE_ENABLE_64r,REG_PORT_ANY,
                           0, rval));

        sal_memset (&rval, 0, sizeof(rval));
        soc_reg_field_set(unit, PORT_PAUSE_ENABLE_64r, &rval,
                PORT_PAUSE_ENABLEf, val);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, PORT_PAUSE_ENABLE_64r,REG_PORT_ANY,
                           0, rval));
    }
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* OpNode related config for remaining OpNodes                 */
    /* (assuming internal memory settings)                         */
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    for (op_node = soc_mem_index_min(unit,MMU_THDO_OPNCONFIG_CELLm);
         op_node <= soc_mem_index_max(unit,MMU_THDO_OPNCONFIG_CELLm);
         op_node++) {
        temp_val = 0 ;
        SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY,op_node ,
                            &mmu_thdo_opnconfig_cell_entry));
        temp_val = soc_mem_field32_get(unit, MMU_THDO_OPNCONFIG_CELLm,
                            &mmu_thdo_opnconfig_cell_entry,
                            OPN_SHARED_LIMIT_CELLf);
        if (temp_val == 0) {
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                                &mmu_thdo_opnconfig_cell_entry,
                                OPN_SHARED_LIMIT_CELLf,
                                output_port_threshold->
                                min_grntd_tot_shr_queue_cells_int_buff);
            temp_val = output_port_threshold->
                       min_grntd_tot_shr_queue_cells_int_buff -
                       (2 * general_info->ether_mtu_cells_for_int_buff);
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                                &mmu_thdo_opnconfig_cell_entry,
                                OPN_SHARED_RESET_VALUE_CELLf, temp_val);
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                                &mmu_thdo_opnconfig_cell_entry,
                                PORT_LIMIT_ENABLE_CELLf, 0);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_CELLm(unit,
                                MEM_BLOCK_ALL,op_node,
                                &mmu_thdo_opnconfig_cell_entry));
        }
    }
    for (op_node = soc_mem_index_min(unit,MMU_THDO_OPNCONFIG_QENTRYm);
         op_node <= soc_mem_index_max(unit,MMU_THDO_OPNCONFIG_QENTRYm);
         op_node++) {
         temp_val = 0 ;
         SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_QENTRYm(unit,
                             MEM_BLOCK_ANY,op_node ,
                             &mmu_thdo_opnconfig_qentry_entry));
         temp_val = soc_mem_field32_get(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                             &mmu_thdo_opnconfig_qentry_entry,
                             OPN_SHARED_LIMIT_QENTRYf);
         if (temp_val == 0) {
             soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                 &mmu_thdo_opnconfig_qentry_entry, OPN_SHARED_LIMIT_QENTRYf,
                 output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs);

             temp_val = output_port_threshold->
                        min_grntd_tot_shr_queue_cells_EQEs - 2;
             soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                                 &mmu_thdo_opnconfig_qentry_entry,
                                 OPN_SHARED_RESET_VALUE_QENTRYf, temp_val);

             soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                                 &mmu_thdo_opnconfig_qentry_entry,
                                 PORT_LIMIT_ENABLE_QENTRYf, 0);
             /*
             soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                                 &mmu_thdo_opnconfig_qentry_entry,
                                 PIDf, port);
              */
             SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_QENTRYm(unit,
                                 MEM_BLOCK_ALL,op_node ,
                                 &mmu_thdo_opnconfig_qentry_entry));
         }
    }
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* Subport queue Settings                                      */
    /* (Assuming lossless-internal memory operation)               */
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    for (sub_port = 0;
         sub_port <= SOC_ML_MAX_SUBPORTS;
         sub_port ++) {
         if (!_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, sub_port)) {
            continue;
         }
         if ( si->port_uc_cosq_base[sub_port]  != 0) {
              for (queue =  si->port_uc_cosq_base[sub_port];
                   queue <  si->port_uc_cosq_base[sub_port] +
                            si->port_num_uc_cosq[sub_port];
                   queue++) {
                   LOG_VERBOSE(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                 "sub_port:%d queue:%d"), sub_port, queue));
                   SOC_IF_ERROR_RETURN(
                   _soc_ml_mmu_config_extra_queue(unit, queue,
                                               &_soc_ml_mmu_params,
                                               output_port_threshold));
              }
         }
    }
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* Extended Queue(BCM_COSQ_GPORT_SUBSCRIBE) Settings           */
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    for (queue = si->port_uc_cosq_base[ML_MAX_PHYSICAL_PORTS] +
                 si->port_num_uc_cosq[ML_MAX_PHYSICAL_PORTS];
         queue <= soc_mem_index_max(unit,MMU_THDO_QCONFIG_CELLm);
         queue++) {
         SOC_IF_ERROR_RETURN(
            _soc_ml_mmu_config_extra_queue(unit, queue,
                                               &_soc_ml_mmu_params,
                                               output_port_threshold));
    }
    return SOC_E_NONE;
}

int _soc_ml_mmu_reconfigure(int unit, int port)
{
    SOC_IF_ERROR_RETURN(_soc_ml_mmu_init_helper(unit, port));
    LOG_CLI((BSL_META_U(unit,
                    "MMU is reconfigured \n")));
    return SOC_E_NONE;
}

STATIC int
_soc_ml_post_mmu_init(int unit) 
{
    txlp_port_addr_map_table_entry_t txlp_port_addr_map_table_entry={{0}};
    uint32      start_addr=0;
    uint32      end_addr=0;
    int         port = 0;
    int         loop;
    soc_info_t  *si;
    uint32      rval;

    si = &SOC_INFO(unit);

    if (SAL_BOOT_QUICKTURN) {
        soc_pbmp_t link_pbmp;
        epc_link_bmap_entry_t epc_link_bmap_entry={{0}};

        SOC_PBMP_ASSIGN(link_pbmp, PBMP_ALL(unit));
        soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, &epc_link_bmap_entry,
                PORT_BITMAPf,&link_pbmp);
        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(
                    unit, MEM_BLOCK_ALL, 0, &epc_link_bmap_entry));
    }

    if ((si->block_valid[TXLP0_BLOCK(unit)] == 0)) {
        /* TXLP related ports are not valid */
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(READ_TXLP_HW_RESET_CONTROL_1r(unit, 1, &rval));
    soc_reg_field_set(unit, TXLP_HW_RESET_CONTROL_1r, &rval, COUNTf, 256);
    soc_reg_field_set(unit, TXLP_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, TXLP_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TXLP_HW_RESET_CONTROL_1r(unit, 1, rval));

    /* TXLP_PORT_ADDR_MAP_TABLE */
    for(loop=0, start_addr=0,port=1 ; loop < ML_MAX_PORTS_PER_BLOCK;
            loop++, port++) {
        if (si->port_speed_max[port] <= 2500) {
            end_addr = start_addr + (( 8 * 4) - 1); /* 8 cells */
        } else if(si->port_speed_max[port] <= 13000) {
            end_addr = start_addr + (( 16 * 4) - 1);
        } else  {
            return SOC_E_PARAM;
        }

        sal_memset(&txlp_port_addr_map_table_entry,0,
                sizeof(txlp_port_addr_map_table_entry_t));
        soc_TXLP_PORT_ADDR_MAP_TABLEm_field_set(unit,
                &txlp_port_addr_map_table_entry,START_ADDRf,&start_addr);
        soc_TXLP_PORT_ADDR_MAP_TABLEm_field_set(unit,
                &txlp_port_addr_map_table_entry,END_ADDRf,&end_addr);
        start_addr = end_addr+1;

        SOC_IF_ERROR_RETURN(WRITE_TXLP_PORT_ADDR_MAP_TABLEm(
                    unit,SOC_INFO(unit).txlp_block[0], loop,
                    &txlp_port_addr_map_table_entry));
    }

    return SOC_E_NONE;
}
int _soc_ml_invalid_parent_index(int unit, int level)
{
    int index_max = 0;

    switch (level) {
        case SOC_KT2_NODE_LVL_ROOT:
            index_max = 0;
            break;
        case SOC_KT2_NODE_LVL_L0:
            index_max = (1 << soc_mem_field_length(unit, LLS_L0_PARENTm, C_PARENTf)) - 1;
            break;
        case SOC_KT2_NODE_LVL_L1:
            index_max = (1 << soc_mem_field_length(unit, LLS_L1_PARENTm, C_PARENTf)) - 1;
            break;
        case SOC_KT2_NODE_LVL_L2:
            index_max = (1 << soc_mem_field_length(unit, LLS_L2_PARENTm, C_PARENTf)) - 1;
            break;
        default:
            break;

    }

    return index_max;
}
#define INVALID_PARENT(unit, level)   _soc_ml_invalid_parent_index((unit),(level))
/* Programming all the entries with the Invalid Parent Pointer */
int
_soc_ml_mmu_set_invalid_parent(int unit)
{
    int level, index_max, index;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];

    for (level = SOC_KT2_NODE_LVL_L0; level <= SOC_KT2_NODE_LVL_L2;
            level++) {
        mem = _SOC_KT2_NODE_PARENT_MEM(level);
        /* coverity[negative_returns : FALSE] */
        index_max = soc_mem_index_max(unit, mem);
        for (index = 0; index <= index_max; index++) {
            /* During reboot, mem reinitialized. So no need to read from cache */
            SOC_IF_ERROR_RETURN
                (soc_mem_cache_invalidate(unit, mem, MEM_BLOCK_ALL, index));
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
            soc_mem_field32_set(unit, mem, entry,
                    C_PARENTf, INVALID_PARENT(unit,level));
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
        }
    }
    return SOC_E_NONE;
}
STATIC int
_soc_metrolite_mmu_init(int unit)
{
    uint32 rval;
    int                 mmu_init_usec, i;
    soc_timeout_t       to;
    mmu_aging_lmt_int_entry_t age_entry;
    int count;
    uint64 rval1;

    /* Init Link List Scheduler */
    rval = 0;
    soc_reg_field_set(unit, LLS_SOFT_RESETr, &rval, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_LLS_SOFT_RESETr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, LLS_INITr, &rval, INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_INITr(unit, rval));

    /* Wait for LLS init done. */
    if (SAL_BOOT_SIMULATION) {
        mmu_init_usec = 10000000; /* Simulation  .. 10sec */
    } else {
        mmu_init_usec = 50000;
    }
    soc_timeout_init(&to, mmu_init_usec, 0);

    do {
        SOC_IF_ERROR_RETURN(READ_LLS_INITr(unit, &rval));
        if (soc_reg_field_get(unit, LLS_INITr, rval, INIT_DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                     "LLS INIT timeout\n")));
            break;
        }
    } while (TRUE);

    /* Enable all ports */
    rval = 0xFFFFFFFF;
    COMPILER_64_ZERO(rval1);
    COMPILER_64_SET(rval1, 0, rval);
    soc_reg_field_set(unit, INPUT_PORT_RX_ENABLE_64r, &rval,
                          INPUT_PORT_RX_ENABLEf, 0x3FFFFFFF);
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLE_64r(unit, rval1));

    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_INPUT_PORT_RX_ENABLE_64r(unit, rval1));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_INPUT_PORT_RX_ENABLE_64r(unit, rval1));

    SOC_IF_ERROR_RETURN(WRITE_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDO_BYPASSr(unit, 0));


    /* Setup TDM for MMU Arb & LLS */
     SOC_IF_ERROR_RETURN(_soc_metrolite_mmu_tdm_init(unit)); 

    /* Config DEQ_EFIFO_CFG and TOQ_PORT_BW_CTRL */
    /* SOC_IF_ERROR_RETURN(_soc_metrolite_mmu_port_init(unit)); */

    /* Enable LLS */
    rval = 0;
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, DEQUEUE_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, ENQUEUE_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, FC_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, MIN_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, PORT_SCHEDULER_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, SHAPER_ENABLEf, 1);
    if (SOC_REG_FIELD_VALID(unit, LLS_CONFIG0r, SPRI_VECT_MODE_ENABLEf)) {
        if (soc_feature(unit, soc_feature_vector_based_spri)) {
            soc_reg_field_set(unit, LLS_CONFIG0r, &rval,
                              SPRI_VECT_MODE_ENABLEf, 1);
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_LLS_CONFIG0r(unit, rval));

    /* Enable shaper background refresh */
    rval = 0;
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval,                       L0_MAX_REFRESH_ENABLEf,   1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval,                       L1_MAX_REFRESH_ENABLEf,   1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval,                       L2_MAX_REFRESH_ENABLEf,   1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval,                       PORT_MAX_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval,                       S1_MAX_REFRESH_ENABLEf,   1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_MAX_REFRESH_ENABLEr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, LLS_MIN_REFRESH_ENABLEr, &rval,                       L0_MIN_REFRESH_ENABLEf,   1);
    soc_reg_field_set(unit, LLS_MIN_REFRESH_ENABLEr, &rval,                       L1_MIN_REFRESH_ENABLEf,   1);
    soc_reg_field_set(unit, LLS_MIN_REFRESH_ENABLEr, &rval,                       L2_MIN_REFRESH_ENABLEf,   1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_MIN_REFRESH_ENABLEr(unit, rval));

    /* RQE configuration */
    rval = 0;
    soc_reg_field_set(unit, RQE_SCHEDULER_CONFIGr, &rval,
                      L0_MCM_MODEf, 1);
    soc_reg_field_set(unit, RQE_SCHEDULER_CONFIGr, &rval,
                      L0_CC_MODEf, 1);
    soc_reg_field_set(unit, RQE_SCHEDULER_CONFIGr, &rval,
                      L0_UCM_MODEf, 1);
    soc_reg_field_set(unit, RQE_SCHEDULER_CONFIGr, &rval,
                      L1_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_RQE_SCHEDULER_CONFIGr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, RQE_SCHEDULER_WEIGHT_L0_QUEUEr, &rval,
                      WRR_WEIGHTf, 1);
    for(i=0; i<SOC_REG_NUMELS(unit, RQE_SCHEDULER_WEIGHT_L0_QUEUEr); i++) {
        SOC_IF_ERROR_RETURN(WRITE_RQE_SCHEDULER_WEIGHT_L0_QUEUEr(unit, i,
                                                                 rval));
    }
                      
    rval = 0;
    soc_reg_field_set(unit, RQE_SCHEDULER_WEIGHT_L1_QUEUEr, &rval,
                      WRR_WEIGHTf, 1);                           
    for(i=0; i<SOC_REG_NUMELS(unit, RQE_SCHEDULER_WEIGHT_L1_QUEUEr); i++) {
        SOC_IF_ERROR_RETURN(WRITE_RQE_SCHEDULER_WEIGHT_L1_QUEUEr(unit, i,
                                                                 rval));
    }
     
    SOC_IF_ERROR_RETURN(READ_WRED_PARITY_ERROR_MASKr(unit, &rval));
    soc_reg_field_set(unit, WRED_PARITY_ERROR_MASKr, &rval, UPDATE_INTRPT_MASKf,  0);
    SOC_IF_ERROR_RETURN(WRITE_WRED_PARITY_ERROR_MASKr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, REFRESH_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(soc_saber2_perq_flex_counters_init(unit,0)); 

    rval = 0xffffffff; /* PRIx_GRP = 0x7 */

    /* Configuring all profile 0 to use all the priority groups 0 to 7 */

    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_0_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_0_PRI_GRP1r(unit, rval));
   
    SOC_IF_ERROR_RETURN(WRITE_PROFILE0_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_PROFILE0_PRI_GRP1r(unit, rval));

    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PROFILE0_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PROFILE0_PRI_GRP1r(unit, rval));

    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PROFILE0_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PROFILE0_PRI_GRP1r(unit, rval));

    COMPILER_64_ZERO(rval1);


    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r(unit, rval1));
    /* Using the same rval1 for setting the remaining registers */
    SOC_IF_ERROR_RETURN(WRITE_PORT_PROFILE_MAPr(unit, rval1));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_PROFILE_MAPr(unit, rval1));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_PROFILE_MAPr(unit, rval1));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));

    /* Input port per-device global headroom */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_THDO_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, STAT_CLEARf, 0);
    soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, PARITY_CHK_ENf, 1);
    soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, PARITY_GEN_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_THDO_MISCCONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_THR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval, EARLY_E2E_SELECTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(_soc_ml_mmu_init_helper(unit, -1));
    
    /* Initialize MMU internal/external aging limit memory */
    count = soc_mem_index_count(unit, MMU_AGING_LMT_INTm);
    sal_memset(&age_entry, 0, sizeof(mmu_aging_lmt_int_entry_t));
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_AGING_LMT_INTm(unit,
                            MEM_BLOCK_ANY, i, &age_entry));
    }
    if (!SOC_WARM_BOOT(unit)) { 
        SOC_IF_ERROR_RETURN(_soc_ml_mmu_set_invalid_parent(unit));
    }
    SOC_IF_ERROR_RETURN(_soc_ml_post_mmu_init(unit));

    rval = 0;
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_0r, &rval, RQE_QUEUE_PRI_0f, 0);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_0r, &rval, RQE_QUEUE_PRI_1f, 1);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_0r, &rval, RQE_QUEUE_PRI_2f, 2);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_0r, &rval, RQE_QUEUE_PRI_3f, 3);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_0r, &rval, RQE_QUEUE_PRI_4f, 4);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_0r, &rval, RQE_QUEUE_PRI_5f, 5);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_0r, &rval, RQE_QUEUE_PRI_6f, 6);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_0r, &rval, RQE_QUEUE_PRI_7f, 7);
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_RQE_QUEUE_SELECT_0r(unit, rval));


    rval = 0;
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_1r, &rval, RQE_QUEUE_PRI_8f, 7);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_1r, &rval, RQE_QUEUE_PRI_9f, 7);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_1r, &rval, RQE_QUEUE_PRI_10f, 7);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_1r, &rval, RQE_QUEUE_PRI_11f, 7);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_1r, &rval, RQE_QUEUE_PRI_12f, 7);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_1r, &rval, RQE_QUEUE_PRI_13f, 7);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_1r, &rval, RQE_QUEUE_PRI_14f, 7);
    soc_reg_field_set(unit, MMU_ENQ_RQE_QUEUE_SELECT_1r, &rval, RQE_QUEUE_PRI_15f, 7);
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_RQE_QUEUE_SELECT_1r(unit, rval));
    
    return SOC_E_NONE;
}

STATIC int
_soc_metrolite_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &rval));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_metrolite_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_metrolite_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 rval;

    rval = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, rval));

    return SOC_E_NONE;
}

int ml_linkphy_port[1][4] = {
    {1, 2, 3, 4}  /* {T,R}XLP0 */
};

int 
soc_ml_linkphy_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index)
{
    int i,j;
    int blk = -1;
    int idx = -1;

    for (i=0; i<1; i++) {
        for(j=0; j<4; j++) {
            if(port == ml_linkphy_port[i][j]) {
                    blk = i;
                    idx = j;
                    break;
            }
        }
    }

    if (blktype == SOC_BLK_TXLP) {
        if (blk == 0) {
            blk = TXLP0_BLOCK(unit);
        }
    } else if (blktype == SOC_BLK_RXLP) {
        if (blk == 0) {
            blk = RXLP0_BLOCK(unit);
        }
    } else {
        return SOC_E_PARAM;
    }

    if (block != NULL) {
        *block = blk;
    }
    if (index != NULL) {
        *index = idx;
    }
    if ((blk == -1) || (idx == -1)) {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}

int
soc_ml_linkphy_port_blk_idx_get(
    int unit, int port, int *block, int *index)
{
    int i,j;
    int blk = -1;
    int idx = -1;

    for (i=0; i<1; i++) {
        for(j=0; j<4; j++) {
            if(port == ml_linkphy_port[i][j]) {
                    blk = i;
                    idx = j;
                    break;
            }
        }
    }

    if (block != NULL) {
        *block = blk;
    }
    if (index != NULL) {
        *index = idx;
    }
    if ((blk == -1) || (idx == -1)) {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}

int soc_ml_linkphy_get_portid(int unit, int block, int index) {
    if (index > 3) {
        return -1;
    }
    if ((block == TXLP0_BLOCK(unit)) || (block == RXLP0_BLOCK(unit))) {
        return ml_linkphy_port[0][index];
    }
    return -1;
}
int
soc_ml_pp_port_get_port(int unit, soc_port_t pp_port)
{
    int port;
    soc_info_t *si;
    si = &SOC_INFO(unit);

    if (!_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, pp_port)) {
        return -1;
    }

    for (port = 1; port < si->cpu_hg_index; port++) {
        if (SOC_PBMP_MEMBER(SOC_PORT_PP_BITMAP(unit, port), pp_port)) {
            return port;
        }
    }

    return -1;
}

/*
 * cpu port (mmu port 0): 48 queues (0-47)
 * loopback port (48-71)
 * hg ports : 24 queues
 * other ports : 8 queues                              
 */

int
soc_metrolite_num_cosq_init(int unit)
{
    soc_info_t *si;
    int port;
    int numq = 0, i;
    int pp_port_cos;
    int prev_port_base = 0;
    soc_pbmp_t subport_pbmp;
    int num_uc_cosq = 0, total_used_queue = 0;

    si = &SOC_INFO(unit);
    SOC_PBMP_CLEAR(subport_pbmp);

    /* For CPU port it is hardcoded to zero as base queue */
    si->port_cosq_base[si->cmic_port] = 0;
    si->port_uc_cosq_base[si->cmic_port] = 0;
    si->port_num_uc_cosq[si->cmic_port] = NUM_CPU_COSQ(unit);
    si->port_num_ext_cosq[si->cmic_port] = 0;
    total_used_queue += si->port_num_uc_cosq[si->cmic_port];

    /* For Loopback port it is hardcoded as 48 as a base queue*/
    si->port_cosq_base[si->lb_port] = 48;
    si->port_uc_cosq_base[si->lb_port] = 48;
    si->port_num_uc_cosq[si->lb_port] = 24;
    total_used_queue += si->port_num_uc_cosq[si->lb_port];

    /* first front panel port needs to start with 72 */
    numq = 72;
    prev_port_base = si->port_uc_cosq_base[si->cmic_port];

    for (port = si->cmic_port + 1 ; port < si->lb_port; port++) {
        if (SOC_PBMP_MEMBER(subport_pbmp, port)) {
            continue;
        }
        si->port_num_cosq[port] = 0;

        /* For all GE/, XE, HG Ports */
        si->port_uc_cosq_base[port] = prev_port_base +
            numq;
        si->port_cosq_base[port] = si->port_uc_cosq_base[port];
        prev_port_base = si->port_uc_cosq_base[port];

        if (IS_HG_PORT(unit, port)) {
            si->port_num_uc_cosq[port] = 24;
        } else {
            num_uc_cosq = soc_property_port_get(unit, port,
		    spn_LLS_NUM_L2UC, 8); /* DEFAULT_NUM_PHYSICAL_QUEUES */
	    if (num_uc_cosq < 8) {
		num_uc_cosq = 8;
	    }
	    si->port_num_uc_cosq[port] = num_uc_cosq;
        }
        numq = si->port_num_uc_cosq[port];

        /* This need to be validated when testing COE ports */
        pp_port_cos = soc_property_port_get(unit, port, 
                spn_NUM_SUBPORT_COS, 4); 
        if (pp_port_cos > si->port_num_uc_cosq[port]) {
            si->port_num_uc_cosq[port] = pp_port_cos;
            numq = si->port_num_uc_cosq[port];
        }
        total_used_queue += si->port_num_uc_cosq[port];
        if (total_used_queue > ML_MMU_MAX_EGRESS_QUEUE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                       "###ATTN:Physical queue assignment"
                       " for port:%d exceeded"
                       " max value:%d ###\n INFO: "
                       "Start queue:%d End Queue %d\n"),
                       port, ML_MMU_MAX_EGRESS_QUEUE,
                       si->port_cosq_base[port],
                       si->port_cosq_base[port] +
                       si->port_num_uc_cosq[port] - 1));

                       return SOC_E_CONFIG;
        }

        if (si->port_num_subport[port] > 0) {
            SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, port), i) {
                if (port != i) {
                    si->port_uc_cosq_base[i] = prev_port_base + numq;
                    si->port_cosq_base[i] = si->port_uc_cosq_base[i];
                    numq += pp_port_cos;
                } else {
                    total_used_queue -= si->port_num_uc_cosq[i];
                }
                si->port_num_uc_cosq[i] = pp_port_cos;
                SOC_PBMP_PORT_ADD(subport_pbmp, i);

                total_used_queue += si->port_num_uc_cosq[i];
                if (total_used_queue > ML_MMU_MAX_EGRESS_QUEUE) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                               "###ATTN:Subport queue assignment"
                               " for port:%d subport: %d exceeded"
                               " max value: %d ###\n INFO: "
                               "Start queue: %d End Queue: %d \n"),
                               port, i, ML_MMU_MAX_EGRESS_QUEUE,
                               si->port_cosq_base[i],
                               si->port_cosq_base[i] +
                               si->port_num_uc_cosq[i] - 1));

                               return SOC_E_CONFIG;
                }
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                    "\nport num %d cosq_base %d num_cosq %d"),
                    port, si->port_cosq_base[port], si->port_num_uc_cosq[port]));
    } 

    return SOC_E_NONE;
}

/* soc_ml_mem_config:
 *  * Over-ride the default table sizes (from regsfile) for any SKUs here
 */
int
soc_ml_mem_config(int unit, int dev_id)
{
    int rv = SOC_E_NONE;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 0;
    soc_persist_t *sop = SOC_PERSIST(unit);

    /* Modify max index of  LLS_L0_PARENT to match number of L0 nodes. */
    sop->memState[LLS_L0_PARENTm].index_max = 63;

    /* Modification of table sizes based on OTP bond options */
    if (soc_feature(unit, soc_feature_untethered_otp)) {

        if (socBondInfoFeatureL2Entry8k) {
            sop->memState[L2Xm].index_max = 8191; 
            sop->memState[L2_ENTRY_ONLYm].index_max = 8191; 
            sop->memState[L2_HITDA_ONLYm].index_max = 1023;
            sop->memState[L2_HITSA_ONLYm].index_max = 1023;
        } else if(socBondInfoFeatureL2Entry32k) {
            sop->memState[L2Xm].index_max = 32767;
            sop->memState[L2_ENTRY_ONLYm].index_max = 32767; 
        } else {
            sop->memState[L2Xm].index_max = 32767; 
            sop->memState[L2_ENTRY_ONLYm].index_max = 32767; 
        }

        if (socBondInfoFeatureIpVlanXlate1k) {
            sop->memState[VLAN_XLATEm].index_max = 1023; 
            sop->memState[VLAN_MACm].index_max = 1023; 
        }else if(socBondInfoFeatureIpVlanXlate4k) {
            sop->memState[VLAN_XLATEm].index_max = 4095;
            sop->memState[VLAN_MACm].index_max = 4095; 
        }else {
            sop->memState[VLAN_XLATEm].index_max = 4095; 
            sop->memState[VLAN_MACm].index_max = 4095; 
        }

        if (socBondInfoFeatureMpls1k) {
            sop->memState[MPLS_ENTRYm].index_max = 1023; 
        }else if(socBondInfoFeatureMpls2k) {
            sop->memState[MPLS_ENTRYm].index_max = 4095;
        }else {
            sop->memState[MPLS_ENTRYm].index_max = 4095; 
        }

        if (socBondInfoFeatureL3Entry1k) {
            sop->memState[L3_ENTRY_ONLYm].index_max = 1023; 
            sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = 1023; 
            sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = 511; 
            sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = 511; 
            sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = 255; 
        }
        if (SOC_BOND_INFO_GET(unit, vfp_slice_map) == 0x0) {
            sop->memState[VFP_TCAMm].index_max = -1; 
        }
        if (SOC_BOND_INFO_GET(unit, efp_slice_map) == 0x0) {
            sop->memState[EFP_TCAMm].index_max = -1; 
        }

        if (socBondInfoFeatureL3NextHop1k) {
            sop->memState[ING_L3_NEXT_HOPm].index_max = 1023; 
            sop->memState[EGR_L3_NEXT_HOPm].index_max = 1023; 
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 1023; 
        }else if(socBondInfoFeatureL3NextHop2k) {
            sop->memState[ING_L3_NEXT_HOPm].index_max = 2047; 
            sop->memState[EGR_L3_NEXT_HOPm].index_max = 2047; 
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 2047; 
        }else {
            sop->memState[ING_L3_NEXT_HOPm].index_max = 2047; 
            sop->memState[EGR_L3_NEXT_HOPm].index_max = 2047; 
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 2047; 
        }

        /* L3_DEFIP sizing */
        if (socBondInfoFeatureL3DefIpDisable) {
            sop->memState[L3_DEFIP_PAIR_128m].index_max = -1;
            sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = -1;
            sop->memState[L3_DEFIPm].index_max = -1;
            sop->memState[L3_DEFIP_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max = -1 ;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max = -1 ;
            SOC_CONTROL(unit)->l3_defip_index_remap = -1 ;
        }else {
            SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_ML_DEFIP_MAX_TCAMS;
            SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_ML_DEFIP_TCAM_DEPTH;
            if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
                defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

                num_ipv6_128b_entries =
                    soc_property_get(unit, spn_NUM_IPV6_LPM_128B_ENTRIES,
                            (defip_config ? 256 : 0));

                config_v6_entries = num_ipv6_128b_entries;

                if (soc_property_get(unit, spn_LPM_SCALING_ENABLE, 0)) {
                    num_ipv6_128b_entries = 0;
                    if (!soc_property_get(unit, spn_LPM_IPV6_128B_RESERVED, 1)) {
                        config_v6_entries = SOC_CONTROL(unit)->l3_defip_tcam_size *
                            ((config_v6_entries /
                              SOC_CONTROL(unit)->l3_defip_tcam_size) +
                             ((config_v6_entries %
                               SOC_CONTROL(unit)->l3_defip_tcam_size) ? 1 : 0));
                    }
                }

                sop->memState[L3_DEFIP_PAIR_128m].index_max =
                    num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max =
                    num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max =
                    num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max =
                    num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIPm].index_max =
                    (SOC_CONTROL(unit)->l3_defip_max_tcams *
                     SOC_CONTROL(unit)->l3_defip_tcam_size) -
                    (num_ipv6_128b_entries * 2) - 1;
                sop->memState[L3_DEFIP_ONLYm].index_max =
                    sop->memState[L3_DEFIPm].index_max;
                sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
                    sop->memState[L3_DEFIPm].index_max;
                sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                    sop->memState[L3_DEFIPm].index_max;
                SOC_CONTROL(unit)->l3_defip_index_remap = num_ipv6_128b_entries;
                soc_l3_defip_indexes_init(unit, config_v6_entries);
            }
        }
    }else {
        /* L3_DEFIP sizing */
        SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_ML_DEFIP_MAX_TCAMS;
        SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_ML_DEFIP_TCAM_DEPTH;
        if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
            defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

            num_ipv6_128b_entries =
                soc_property_get(unit, spn_NUM_IPV6_LPM_128B_ENTRIES,
                        (defip_config ? 256 : 0));

            config_v6_entries = num_ipv6_128b_entries;

            if (soc_property_get(unit, spn_LPM_SCALING_ENABLE, 0)) {
                num_ipv6_128b_entries = 0;
                if (!soc_property_get(unit, spn_LPM_IPV6_128B_RESERVED, 1)) {
                    config_v6_entries = SOC_CONTROL(unit)->l3_defip_tcam_size *
                        ((config_v6_entries /
                          SOC_CONTROL(unit)->l3_defip_tcam_size) +
                         ((config_v6_entries %
                           SOC_CONTROL(unit)->l3_defip_tcam_size) ? 1 : 0));
                }
            }

            sop->memState[L3_DEFIP_PAIR_128m].index_max =
                num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max =
                num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max =
                num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max =
                num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIPm].index_max =
                (SOC_CONTROL(unit)->l3_defip_max_tcams *
                 SOC_CONTROL(unit)->l3_defip_tcam_size) -
                (num_ipv6_128b_entries * 2) - 1;
            sop->memState[L3_DEFIP_ONLYm].index_max =
                sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
                sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                sop->memState[L3_DEFIPm].index_max;
            SOC_CONTROL(unit)->l3_defip_index_remap = num_ipv6_128b_entries;
            soc_l3_defip_indexes_init(unit, config_v6_entries);
        }
    }
    return rv;
}


int
_soc_metrolite_sbus_tsc_block(int unit,
            int phy_port, soc_mem_t wc_ucmem_data, int *block)
{
    int i,port_num_blktype = 0, tmp_blk=0;

    *block= -1;
    port_num_blktype = SOC_DRIVER(unit)->port_num_blktype;
    for (i = 0; i < port_num_blktype; i++){
        tmp_blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
        if (SOC_MEM_BLOCK_VALID(unit, wc_ucmem_data, tmp_blk)) {
            *block = tmp_blk;
            break;
        }
    }

    if (*block == -1){
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "_soc_metrolite_sbus_tsc_block(u=%d,pp=%d,mem=%d, blk=%d): "
                             "no valid block for TSC SBUS interface!\n"),
                  unit, phy_port, wc_ucmem_data, *block));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}
/* PHY_REG address decode :
 *  - TSC_ADDR[31:0]={DEVICE(ADVAD)[31:27],??[26:24],TSC_ID(PHYAD)[23:19],
 *      PORT_MODE(LANE)[18:16], IEEE_BIT[15], BLOCK[14:4], REG[3:0]}
 *  - QSGMII_ADDR[31:0]={DEVICE(ADVAD)[31:27], LANE[26:16], REG_HI[15],
 *      BLOCK[14:4], REG_LO[3:0]}}
 */

#define PHY_REG_ADDR_DEVAD(_phy_reg)    (((_phy_reg) >> 27) & 0x1f)
#define TSC_REG_ADDR_TSCID(_phy_reg)    (((_phy_reg) >> 19) & 0x1f)
#define TSC_REG_ADDR_PORTMODE(_phy_reg) (((_phy_reg) >> 16) & 0x7)
#define QSGMII_REG_ADDR_LANE(_phy_reg)  (((_phy_reg) >> 16) & 0x7ff)
#define PHY_REG_ADDR_BLOCK(_phy_reg)    (((_phy_reg) >> 4) & 0xfff)
#define TSC_REG_ADDR_REG(_phy_reg)      ((_phy_reg) & 0xf)
#define QSGMII_REG_ADDR_REG(_phy_reg)   ((((_phy_reg) & 0x8000) >> 11) | \
                                        ((_phy_reg) & 0xf))

#define IS_QSGMII_REGISTER(_phy_reg) (((_phy_reg) & 0xf800f000) == 0x8000)
#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad)    \
                            ((_phy_reg) |= ((_phyad) & 0x1f) << 19)
#define QSGMII_REG_ADDR_LANE_SET(_phy_reg_new, _phy_reg)    \
                            ((_phy_reg_new) |= ((_phy_reg) & 0x7ff0000))
#define SABER2_PORT_IS_QSGMII(_unit, _port) \
        (IS_GE_PORT((_unit), (_port)) && !IS_XL_PORT((_unit), (_port)))



/* Function to return the speed based on the cfg_num */
int soc_metrolite_xl_port_speed_get(int unit, int port, int *speed) {
    int                 cfg_num=0, max_speed=0;
    soc_pbmp_t          my_pbmp_xport_xe;
    soc_info_t          *si=&SOC_INFO(unit);

    SOC_PBMP_CLEAR(my_pbmp_xport_xe);
    my_pbmp_xport_xe = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_XE,
            my_pbmp_xport_xe);

    SOC_IF_ERROR_RETURN(_soc_metrolite_get_cfg_num(unit, 0, &cfg_num));
    max_speed = bcm56270_speed[unit][cfg_num][port-1];

    if (max_speed  > si->port_speed_max[port]) {
        *speed = si->port_speed_max[port];
        if (si->port_speed_max[port] == 0) {
            /* port_speed_max cannot be 0 for a valid port.
             * During warmboot sync, the port must have been merged.
             * Now after upgrade, the port_speed_max would be 0.
             * So return max speed.
             */
            *speed = max_speed;
        }
    } else if ((max_speed == 10000) && !(SOC_PBMP_MEMBER(my_pbmp_xport_xe, port))) {
        *speed = 11000;
    } else {
        *speed = (max_speed  > si->port_speed_max[port]) ? max_speed :
                 si->port_speed_max[port];
    }

    return SOC_E_NONE;
}

int
soc_metrolite_global_param_reconfigure(int unit)
{
    uint16  dev_id = 0;
    uint8   rev_id = 0;
    int cfg_num = -1;
    int max_cfg_num = 0;
    int def_cfg_num = 0;
    int offset_cfg_num = 0;
    int int_cfg_num = 0;

    /* Retreive system dev and revision identifier */
    soc_cm_get_id(unit, &dev_id, &rev_id);

    cfg_num = soc_property_get(unit, spn_BCM5627X_CONFIG,0xFF); 

    switch(dev_id) {
        case BCM56270_DEVICE_ID:
            def_cfg_num = BCM56270_DEVICE_ID_DEFAULT_CFG;
            offset_cfg_num = BCM56270_DEVICE_ID_OFFSET_CFG;
            max_cfg_num = BCM56270_DEVICE_ID_MAX_CFG;
            break;
        case BCM56271_DEVICE_ID:
            def_cfg_num = BCM56271_DEVICE_ID_DEFAULT_CFG;
            offset_cfg_num = BCM56271_DEVICE_ID_OFFSET_CFG;
            max_cfg_num = BCM56271_DEVICE_ID_MAX_CFG;
            break;
        case BCM56272_DEVICE_ID:
            def_cfg_num = BCM56272_DEVICE_ID_DEFAULT_CFG;
            offset_cfg_num = BCM56272_DEVICE_ID_OFFSET_CFG;
            max_cfg_num = BCM56272_DEVICE_ID_MAX_CFG;
            break;
        case BCM53460_DEVICE_ID:
            def_cfg_num = BCM56270_DEVICE_ID_DEFAULT_CFG;
            offset_cfg_num = BCM56270_DEVICE_ID_OFFSET_CFG;
            max_cfg_num = BCM56270_DEVICE_ID_MAX_CFG;
            break;
        case BCM53461_DEVICE_ID:
            def_cfg_num = BCM56271_DEVICE_ID_DEFAULT_CFG;
            offset_cfg_num = BCM56271_DEVICE_ID_OFFSET_CFG;
            max_cfg_num = BCM56271_DEVICE_ID_DEFAULT_CFG;
            break;
        default : 
            /* Not supported yet */
            return SOC_E_UNAVAIL; 
    }

    if (cfg_num == 0xFF) {
        int_cfg_num = def_cfg_num;
    } else {
        if (cfg_num == 0) {
            LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Config=0 is not allowed\n")));
            return SOC_E_PARAM;
        }   
        int_cfg_num = cfg_num + offset_cfg_num;
    }           
    if (int_cfg_num > max_cfg_num) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Cfg_num:%d Internal cfg_num:%d \n"),cfg_num,int_cfg_num));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Wrong %s ==> cfg_num:%d DEFAULT:%d START:%d MAX:%d \n"),
                    spn_BCM5627X_CONFIG, cfg_num,
                    def_cfg_num-offset_cfg_num,1,max_cfg_num-offset_cfg_num));
        int_cfg_num = def_cfg_num;
        return SOC_E_NONE; 
    }   
    /* Reconfigure global param based on config */
    switch(int_cfg_num) {
      case BCM56270_DEVICE_ID_OFFSET_CFG + 1:
        SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT = 4;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_PORT =  4;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_SLICE = 16;
        break;
      case BCM56271_DEVICE_ID_OFFSET_CFG + 1:
        SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT = 4;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_PORT =  4;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_SLICE = 16;
        break;
      case BCM56271_DEVICE_ID_OFFSET_CFG + 2:
        SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT = 6;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_PORT =  6;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_SLICE = 24;
        break;
      case BCM56271_DEVICE_ID_OFFSET_CFG + 3:
        SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT = 4;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_PORT =  4;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_SLICE = 16;
        break;
      case BCM56272_DEVICE_ID_OFFSET_CFG + 1:
        SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT = 4;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_PORT =  4;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_SLICE = 16;
        break;
      case BCM56272_DEVICE_ID_OFFSET_CFG + 2:
        SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT = 6;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_PORT =  6;
        SOC_ML_MAX_LINKPHY_STREAMS_PER_SLICE = 24;
        break;
      default:
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,"Invalid cfg_num\n")));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}
STATIC
int soc_metrolite_is_pp_port_reusable(int unit, int port)
{
    int                   cfg_num = 0, i;
    soc_port_details_t    *current_ml_port_details, *head;
    soc_pbmp_t            pbmp_subport, pbmp_linkphy;

    pbmp_subport = soc_property_get_pbmp(unit, spn_PBMP_SUBPORT, 0);
    pbmp_linkphy = soc_property_get_pbmp(unit, spn_PBMP_LINKPHY, 0);

    SOC_IF_ERROR_RETURN(_soc_metrolite_get_cfg_num(unit, 1, &cfg_num));
    
    current_ml_port_details = sal_alloc(
                        sizeof(soc_port_details_t) * (ML_MAX_PHYSICAL_PORTS+1),
                        "ml_block_ports_t");
    if (current_ml_port_details == NULL) { 
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
           "INTERNAL: Memory allocation failed for ml_selected_port_details\n")));
        return FALSE;
    }
    head = current_ml_port_details;
    sal_memcpy(current_ml_port_details,
            ml_port_details[cfg_num],
            sizeof(soc_port_details_t) * (ML_MAX_PHYSICAL_PORTS+1));
 
    while (current_ml_port_details->start_port_no) {
           for (i = current_ml_port_details->start_port_no; 
                i <= current_ml_port_details->end_port_no; 
                i = i + current_ml_port_details->port_incr) {
                if ((port == i) && (current_ml_port_details->port_speed != 0)) {
                    /* Check if linkphy or coe is enabled */
                    if ((!SOC_PBMP_MEMBER(pbmp_subport, port)) && 
                        (!SOC_PBMP_MEMBER(pbmp_linkphy, port))) {
                        sal_free(head); 
                        return FALSE;
                    }
                }
           }
           current_ml_port_details++; 
    }
    sal_free(head);
    return TRUE;
} 

void
soc_metrolite_pbmp_init(int unit, ml_pbmp_t ml_pbmp, soc_pbmp_t *pbmp_pp_unused) 
{
    int port;
    soc_pbmp_t          my_pbmp_xport_xe;
    soc_pbmp_t          my_pbmp_xport_ge;
    uint32              loop_index=0;
    int                 ml_port_details_index=0;
    uint8               ml_port_used_flags[14]={0};
    /*int                 old_num_lanes, num_lanes, port_incr;*/
    uint16              dev_id=0;
    uint8               rev_id=0;
    int                 local_cfg=0;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Unit Specific Allocation */
    if (ml_port_speeds[unit] != NULL) {
        sal_free(ml_port_speeds[unit]);
        ml_port_speeds[unit] = NULL;
    }
    ml_port_speeds[unit] =  sal_alloc(sizeof(ml_port_speeds_t),
            "ml_port_speeds_t");
    if (ml_port_speeds[unit] == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                   FUNCTION_NAME(),  __LINE__,
                   (int)sizeof(ml_port_speeds_t)));
        return;
    }

    if (bcm56270_speed[unit] != NULL) {
        sal_free(bcm56270_speed[unit]);
        bcm56270_speed[unit] = NULL;
    }
    bcm56270_speed[unit] =  sal_alloc(sizeof(ml_speed_t) *
                                      (sizeof(bcm56270_speed_s) /
                                       sizeof(bcm56270_speed_s[0])),
                                      "bcm56270_speed");
    if (bcm56270_speed[unit] == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                   FUNCTION_NAME(),  __LINE__, 
                   (int)sizeof(mxqspeeds_t)));
        return ;
    }

    for (loop_index = 0 ; 
         loop_index < sizeof(bcm56270_speed_s ) /
                      sizeof(bcm56270_speed_s[0]);
         loop_index++) {
         sal_memcpy(bcm56270_speed[unit][loop_index],
                    bcm56270_speed_s[loop_index],
                    sizeof(ml_speed_t));
    }    

    if (ml_port_to_block[unit] != NULL) {
        sal_free(ml_port_to_block[unit]);
        ml_port_to_block[unit] = NULL;
    }
    ml_port_to_block[unit] =  sal_alloc(sizeof(ml_port_to_block_s),
            "ml_port_to_block_t");
    if (ml_port_to_block[unit] == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                   FUNCTION_NAME(),  __LINE__, 
                   (int)sizeof(ml_port_to_block)));
        return ;
    }
    sal_memcpy(ml_port_to_block[unit],ml_port_to_block_s,
            sizeof(ml_port_to_block_s));

    if (ml_port_to_block_subports[unit] != NULL) {
        sal_free(ml_port_to_block_subports[unit]);
        ml_port_to_block_subports[unit] = NULL;
    }
    ml_port_to_block_subports[unit] = 
        sal_alloc(sizeof(ml_port_to_block_subports_s),
                "ml_port_to_block_t");
    if (ml_port_to_block_subports[unit] == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                  "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                  FUNCTION_NAME(),  __LINE__,
                  (int) sizeof(ml_port_to_block_subports)));
        return ;
    }
    sal_memcpy(ml_port_to_block_subports[unit],
            ml_port_to_block_subports_s,sizeof(ml_port_to_block_subports_s));

    if (ml_block_ports[unit] != NULL) {
        sal_free(ml_block_ports[unit]);
        ml_block_ports[unit] = NULL;
    }
    ml_block_ports[unit] =  sal_alloc(sizeof(ml_block_ports_s),
            "ml_block_ports_t");
    if (ml_block_ports[unit] == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                   FUNCTION_NAME(),  __LINE__,
                   (int)sizeof(ml_block_ports)));
        return ;
    }
    sal_memcpy(ml_block_ports[unit],ml_block_ports_s,
            sizeof(ml_block_ports_s));

    /* Initialize PBMPs */
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_gport_stack);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_mxq);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_mxq1g);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_mxq2p5g);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_mxq10g);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_xport_xe);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_xport_ge);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_xl);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_xl1g);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_xl2p5g);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_xl10g);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_pp);
    SOC_PBMP_CLEAR(*ml_pbmp.pbmp_linkphy);

    if (_soc_metrolite_get_cfg_num(unit, 0, &ml_port_details_index) != SOC_E_NONE) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Retrieved WRONG CFG VALUE =%d\n"), ml_port_details_index));
        return ;
    }

    if (ml_port_details_index >= (sizeof(ml_port_details)/
                                   sizeof(ml_port_details[0]))) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "INTERNAL: WRONG CFG VALUE =%d\n"), ml_port_details_index));
        return ;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
            "Internal Config Number : %d\n"), ml_port_details_index));
    
    switch(dev_id) {
        case BCM56270_DEVICE_ID:
        case BCM56271_DEVICE_ID:
        case BCM56272_DEVICE_ID:
	        sal_memcpy(ml_port_speeds[unit], (ml_port_speeds_s[ml_port_details_index]),
			                                                    sizeof(ml_port_speeds_t));
            break;

        default:
	        sal_memcpy(ml_port_speeds[unit], ml_port_speeds_56270_1,
			                                                    sizeof(ml_port_speeds_t));
            break;
    };

    SOC_PBMP_CLEAR(my_pbmp_xport_xe);
    SOC_PBMP_CLEAR(my_pbmp_xport_ge);
    my_pbmp_xport_xe = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_XE,
                                                     my_pbmp_xport_xe);
    my_pbmp_xport_ge = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_GE,
                                                     my_pbmp_xport_ge);

    if (ml_selected_port_details != NULL) {
        sal_free(ml_selected_port_details);
        ml_selected_port_details = NULL;
        sal_free(ml_selected_port_details_orig);
        ml_selected_port_details_orig = NULL;
    }

    /* Note : size of ml_port_details of all configs is same (72 bytes) */
    ml_selected_port_details = sal_alloc(
                        sizeof(soc_port_details_t) * (ML_MAX_PHYSICAL_PORTS+1),
                        "ml_block_ports_t");

    ml_selected_port_details_orig = sal_alloc(
                        sizeof(soc_port_details_t) * (ML_MAX_PHYSICAL_PORTS+1),
                        "ml_block_ports_t");

    if ((ml_selected_port_details == NULL) ||
        (ml_selected_port_details_orig == NULL)) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
           "INTERNAL: Memory allocation failed for ml_selected_port_details\n")));
        return ;
    }

    for (loop_index = 0; loop_index < (ML_MAX_PHYSICAL_PORTS+1); loop_index++) {
        ml_selected_port_details[loop_index] = ml_port_details[ml_port_details_index][loop_index];
        /* Keep the default config to compare later */
        ml_selected_port_details_orig[loop_index] = ml_port_details[ml_port_details_index][loop_index];
        if (ml_port_details[ml_port_details_index][loop_index].port_speed == 0) {
            break;
        }
    }

/* port group will be implemented later */
#if 0 
    if (!(SOC_WARM_BOOT(unit))) {
        /* Construct port bitmaps based on portgroup settings */
        /* In case of warmboot, maintain saved settings */
        loop_index=0;
        port_incr = 4;

        for(port = 1; port <= ML_MAX_PHYSICAL_PORTS; port += port_incr) {
            loop_index = (port / 4);

            /* Modify only if the port is not disabled by default */
            if (ml_selected_port_details[loop_index].port_speed == 0) {
                continue;
            }

            old_num_lanes = ml_selected_port_details[loop_index].port_incr;
            num_lanes = soc_property_port_get(unit, port, spn_PORTGROUP, 0xFF);

            if (num_lanes == 0xFF) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "PortGroup not specified; Setting port %d -> num lanes : %d\n"),
                            port, old_num_lanes));
                num_lanes = old_num_lanes;
            } else if(!((num_lanes == 1) || (num_lanes == 2) || (num_lanes == 4))) {
                LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Invalid PortGroup config; Setting port %d -> num lanes : %d\n"),
                            port, num_lanes));
                num_lanes = old_num_lanes;
            }

            if(old_num_lanes == num_lanes) {
                continue;
            }

            if(port < 5) {
                /* Viper ports */
                switch(num_lanes) {
                    case PORTGROUP_SINGLE_MODE:
                        if((old_num_lanes == 1) &&
                            (ml_selected_port_details[loop_index].port_speed == 2500 )) {
                            ml_selected_port_details[loop_index].port_type = XE_PORT;
                            ml_selected_port_details[loop_index].port_incr = 4;
                            ml_selected_port_details[loop_index].port_speed = 10000;
                            } else {
                        /* 4 lane or 2 lane mode would not have enough bandwidth */
                        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                           "Invalid PortGroup port %d ; BW exceeds Max port speed\n"), port));
                            }
                        break;
                    case PORTGROUP_DUAL_MODE:
                        /* Dual lane not supported for viper ports */
                        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                           "Invalid PortGroup port %d ; Dual lane not supported.\n"), port));
                        break;
                    case PORTGROUP_QUAD_MODE:
                        /* 2->4 cannot be allowed. 1->4 is possible */
                        if(old_num_lanes == 2) {
                            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                               "Invalid PortGroup config for port %d ; BW exceeds Max port speed\n"), port));
                        } else {
                            ml_selected_port_details[loop_index].port_incr = 1;
                            ml_selected_port_details[loop_index].port_speed = 2500;
                            ml_selected_port_details[loop_index].port_type = GE_PORT;
                        }
                        break;
                    default : return; /* Dummy statement */
                }
            } else {
                /* Eagle ports */
                port_incr = 2;
                if ((port == 11) || (port == 12)) {
                    loop_index += 1;
                }

                switch(num_lanes) {
                    case PORTGROUP_SINGLE_MODE:
                        if(ml_selected_port_details[loop_index].port_type & GE_PORT) {
                            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                               "Invalid PortGroup config for port %d ; BW exceeds Max port speed\n"), port));
                        } else {
                            ml_selected_port_details[loop_index].port_type = XE_PORT;
                            ml_selected_port_details[loop_index].port_speed = 10000;
                            ml_selected_port_details[loop_index].port_incr = 4;
                            ml_selected_port_details[loop_index].end_port_no = 28;
                            ml_selected_port_details[loop_index + 1].port_type = 0;
                            port_incr = 4; /* All eagle ports are initialized */
                        }
                        break;
                    case PORTGROUP_DUAL_MODE:
                        ml_selected_port_details[loop_index].port_type = XE_PORT;
                        ml_selected_port_details[loop_index].port_speed = 10000;
                        ml_selected_port_details[loop_index].port_incr = 2;
                        ml_selected_port_details[loop_index+1].port_type = XE_PORT;
                        ml_selected_port_details[loop_index+1].port_speed = 10000;
                        ml_selected_port_details[loop_index+1].port_incr = 2;
                        port_incr = 4; /* All eagle ports are initialized */
                        break;
                    case PORTGROUP_QUAD_MODE:
                        /* In Eagle, 1->4 and 2->4 is not allowed */
                        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                           "Invalid PortGroup config for port %d\n"), port));
                        break;
                    default : return; /* Dummy statement */
                }
            }
        }
        }
#endif

    /* Fill in pbmps */
    loop_index = 0;
    while(ml_selected_port_details[loop_index].port_speed != 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "start:%d end:%d incr:%d type:%d speed:%d\n"),
                 ml_selected_port_details[loop_index].start_port_no,
                 ml_selected_port_details[loop_index].end_port_no,
                 ml_selected_port_details[loop_index].port_incr,
                 ml_selected_port_details[loop_index].port_type,
                 ml_selected_port_details[loop_index].port_speed));

        for (port =  ml_selected_port_details[loop_index].start_port_no;
                port <= ml_selected_port_details[loop_index].end_port_no;
                port += ml_selected_port_details[loop_index].port_incr) {

            /* In 56271 config 2 and 3 port 5 is hidden. Available only as SAT port */
            if(dev_id == BCM56271_DEVICE_ID){
                local_cfg = soc_property_get(unit, spn_BCM5627X_CONFIG,0xFF);
                if ((local_cfg > (BCM56271_DEVICE_ID_OFFSET_CFG)) && (local_cfg < BCM56271_DEVICE_ID_MAX_CFG)) {
                    if((soc_property_get(unit, spn_SAT_ENABLE, 0) == 0) && (port == 5)) {
                        ml_selected_port_details[loop_index].port_type = 0;
                    }
                }
            }
            /* In 56272 port 5 is hidden. Available only as SAT port */
            if(dev_id == BCM56272_DEVICE_ID) {
                if((soc_property_get(unit, spn_SAT_ENABLE, 0) == 0) && (port == 5)) {
                    ml_selected_port_details[loop_index].port_type = 0;
                }
            }

            if (ml_selected_port_details[loop_index].port_type == 0) {
                break;
            }
            
            if ((port >= 1) && (port <= 4)) {
                SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_mxq, port);
            } else if (port >= 5) {
                SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xl, port);
            }

            if (ml_selected_port_details[loop_index].port_type & GE_PORT) {
                SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xport_ge, port);
                switch(ml_selected_port_details[loop_index].port_speed) {
                    case 1000:
                        if ((port >=1) && (port <= 4)) {
                            SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_mxq1g, port);
                        } else {
                            SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xl1g, port);
                        }
                        bcm56270_speed[unit][ml_port_details_index][port-1] = 1000;
                        break;
                    case 2500:
                        if ((port >=1) && (port <= 4)) {
                            SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_mxq2p5g, port);
                        } else {
                            SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xl2p5g, port);
                        }
                        bcm56270_speed[unit][ml_port_details_index][port-1] = 2500;
                        break;
                    default:
                        break;
                }
            } else if ((ml_selected_port_details[loop_index].port_type & XE_PORT) || 
                      ((ml_selected_port_details[loop_index].port_type & HG_PORT) &&
                                           (SOC_PBMP_MEMBER(my_pbmp_xport_xe, port)))) {
                SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xport_xe, port);
                if ((port >=1) && (port <= 4)) {
                    SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_mxq10g, port);
                    bcm56270_speed[unit][ml_port_details_index][port-1] = 10000;
                } else {
                    if (SOC_PBMP_MEMBER(my_pbmp_xport_ge, port)) {
                        SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xl2p5g, port);
                        SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xport_ge, port);
                        SOC_PBMP_PORT_REMOVE(*ml_pbmp.pbmp_xport_xe, port);
                        bcm56270_speed[unit][ml_port_details_index][port-1] = 2500;
                    } else {
                        SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xl10g, port);
                        bcm56270_speed[unit][ml_port_details_index][port-1] = 10000;
                    }
                }
            } else {
                if ((ml_selected_port_details[loop_index].port_type & HG_PORT) &&
                                        (SOC_PBMP_MEMBER(my_pbmp_xport_ge, port))){
                    SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xl2p5g, port);
                    SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xport_ge, port);
                    SOC_PBMP_PORT_REMOVE(*ml_pbmp.pbmp_xport_xe, port);
                    bcm56270_speed[unit][ml_port_details_index][port-1] = 2500;
                } else {   
                    if (port >= 5) {
                        SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_xl10g, port);
                        SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_gport_stack, port);
                        bcm56270_speed[unit][ml_port_details_index][port-1] = 11000;
                    }
                }
            }
            ml_port_used_flags[port-1] = 1;
        } /* for port */
        loop_index++;
    } /* while ml_selected_port_details */

    /* Update all pp ports available in system */
    for (port = 0; port < ML_MAX_PP_PORTS; port++) {
        SOC_PBMP_PORT_ADD(*ml_pbmp.pbmp_pp, port);
        SOC_PBMP_PORT_ADD(*pbmp_pp_unused, port);
    }

    /* Remove disabled ports from pbmp_valid */
    for (port = 1; port < ML_LPBK; port++) {
        SOC_PBMP_PORT_REMOVE(*pbmp_pp_unused, port);
        if (ml_port_used_flags[port-1] == 0) {
            SOC_PBMP_PORT_REMOVE(*ml_pbmp.pbmp_valid, port);
            if(soc_metrolite_is_pp_port_reusable(unit, port) == TRUE) {
                SOC_PBMP_PORT_ADD(*pbmp_pp_unused, port);
            }
        }
    }   

    SOC_PBMP_PORT_REMOVE(*pbmp_pp_unused, ML_CMIC);
    SOC_PBMP_PORT_REMOVE(*pbmp_pp_unused, ML_LPBK);
}

void
soc_metrolite_subport_init(int unit, soc_pbmp_t *pbmp_pp_unused)
{
    soc_port_t port, prev_port;
    soc_pbmp_t pbmp_subport, pbmp_linkphy;
    soc_info_t *si = &SOC_INFO(unit);
    int num_subport = 0, available_subport = 0;
    int max_subport_available = SOC_ML_MAX_SUBPORTS;
    int i = 0, pp_port_free = 0;
    
    /* Clear all pbmp */
    SOC_PBMP_CLEAR(pbmp_subport);
    SOC_PBMP_CLEAR(pbmp_linkphy);
    SOC_PBMP_CLEAR(si->linkphy_pbm);
    SOC_PBMP_CLEAR(si->lp.bitmap);
    SOC_PBMP_CLEAR(si->linkphy_allowed_pbm);
    SOC_PBMP_CLEAR(si->subtag_pbm);
    SOC_PBMP_CLEAR(si->subtag_allowed_pbm);
    SOC_PBMP_CLEAR(si->subtag.bitmap);
    SOC_PBMP_CLEAR(si->linkphy_pp_port_pbm);
    SOC_PBMP_CLEAR(si->enabled_linkphy_pp_port_pbm);
    SOC_PBMP_CLEAR(si->subtag_pp_port_pbm);
    SOC_PBMP_CLEAR(si->enabled_subtag_pp_port_pbm);
    si->subtag_enabled = 0;
    si->linkphy_enabled = 0;

    for (port = 0; port <= ML_MAX_PHYSICAL_PORTS; port++) {
        si->port_num_subport[port] = 0;
        si->port_subport_base[port] = 0;
        si->port_linkphy_s1_base[port] = 0;
        SOC_PBMP_CLEAR(SOC_PORT_PP_BITMAP(unit, port));
    }

    /* Retreive subtag/linkphy pbmp from config */
    pbmp_subport = soc_property_get_pbmp(unit, spn_PBMP_SUBPORT, 0);
    pbmp_linkphy = soc_property_get_pbmp(unit, spn_PBMP_LINKPHY, 0);

    /* Assign subtag/linkphy allowed pbmp */
    SOC_PBMP_ASSIGN(si->subtag_allowed_pbm, pbmp_subport);
    SOC_PBMP_ASSIGN(si->linkphy_allowed_pbm, pbmp_linkphy);

    if (SOC_PBMP_NOT_NULL(pbmp_subport)) {
        /* All members of pbmp_linkphy should also be
         *  member of pbmp_subport */
        if (SOC_PBMP_NOT_NULL(pbmp_linkphy)) {
            /* Check if pbmp_linkphy members are valid for
             * supporting LinkPHY.
             */
            SOC_PBMP_ITER(pbmp_linkphy, port) {
                if (!SOC_REG_PORT_VALID(unit, RXLP_PORT_ENABLEr, port)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit, "\nCONFIG ERROR\n"
                              "pbmp_linkphy member port %d is invalid for "
                              "LinkPHY support\n\n"), port));
                    SOC_PBMP_CLEAR(si->subtag_allowed_pbm);
                    SOC_PBMP_CLEAR(si->linkphy_allowed_pbm);
                    break;
                }
                SOC_PBMP_PORT_ADD(si->linkphy_allowed_pbm, port);
            }

            SOC_PBMP_ITER(pbmp_linkphy, port) {
                if (!SOC_PBMP_MEMBER(pbmp_subport, port)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit, "\nCONFIG ERROR\n"
                              "pbmp_linkphy member port %d is not member of "
                              "pbmp_subport\n\n"), port));
                    SOC_PBMP_CLEAR(si->subtag_allowed_pbm);
                    SOC_PBMP_CLEAR(si->linkphy_allowed_pbm);
                    break;
                }
                SOC_PBMP_PORT_REMOVE(si->subtag_allowed_pbm, port);
            }
        }
    } else {
        if (SOC_PBMP_NOT_NULL(pbmp_linkphy)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "\nCONFIG ERROR\n"
                      "config variable pbmp_linkphy should be sub-set of "
                      "config variable pbmp_subport\n\n")));
        }
    }

    /* Retreive available subports and max subport possible */
    SOC_PBMP_ITER(*pbmp_pp_unused, port) {
        available_subport++;
    }

    prev_port = 0;
    si->port_num_subport[prev_port] = 0;
    si->port_subport_base[prev_port] = 0;

    if (SOC_PBMP_NOT_NULL(si->linkphy_allowed_pbm)) {

        /* Iterate through LinkPHY members first and reserve subport indices */
        SOC_PBMP_ITER(si->linkphy_allowed_pbm, port) {
            if (port < SOC_MAX_NUM_PORTS) {
                num_subport =
                    soc_property_port_get(unit, port, spn_NUM_SUBPORTS, 0);

                if (num_subport > SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT) {
                    num_subport = SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT;
                }

                if (num_subport == 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit, "\nCONFIG ERROR\n"
                              "num_subports_%d should be non-zero for "
                              "LinkPHY port %d\n\n"), port, port));
                    SOC_PBMP_PORT_REMOVE(si->linkphy_allowed_pbm, port);
                    continue;
                }

                if ((num_subport - 1) > available_subport) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit, "\nCONFIG ERROR\n"
                              "num_subports_%d=%d unavailable for LinkPHY port\n\n"),
                               port, num_subport));
                    SOC_PBMP_PORT_REMOVE(si->linkphy_allowed_pbm, port);
                    continue;
                }

                if (max_subport_available <= 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                             (BSL_META_U(unit, "\nCONFIG ERROR\n"
                              "maximum subports %d are consumed already\n\n"),
                              SOC_ML_MAX_SUBPORTS));
                    SOC_PBMP_PORT_REMOVE(si->linkphy_allowed_pbm, port);
                    continue;
                }

                /* Allocate all pp ports */
                for (i = 0; i < num_subport; i++) {
                    if (i == 0) {
                        /* For the first subport, reuse pp port of parent */
                        SOC_PBMP_PORT_ADD(SOC_PORT_PP_BITMAP(unit, port), port);
                        SOC_PBMP_PORT_ADD(si->linkphy_pp_port_pbm, port);
                        SOC_PBMP_PORT_ADD(si->enabled_linkphy_pp_port_pbm, port);
                        continue;
                    }
                    SOC_PBMP_ITER(*pbmp_pp_unused, pp_port_free) {
                        SOC_PBMP_PORT_ADD(SOC_PORT_PP_BITMAP(unit, port), pp_port_free);
                        SOC_PBMP_PORT_ADD(si->linkphy_pp_port_pbm, pp_port_free);
                        SOC_PBMP_PORT_REMOVE(*pbmp_pp_unused, pp_port_free);
                        /* Set this port in the enabled bmap by default at init */
                        SOC_PBMP_PORT_ADD(si->enabled_linkphy_pp_port_pbm,
                                          pp_port_free);
                        break;
                    }
                }

                si->port_subport_base[port] = si->port_subport_base[prev_port] + si->port_num_subport[prev_port];
                si->port_num_subport[port] = num_subport;
                si->port_linkphy_s1_base[port] = si->port_subport_base[prev_port] + si->port_num_subport[prev_port];

                available_subport -= (num_subport -1);
                max_subport_available -= num_subport;

                prev_port = port;
            }
        } /* end SOC_PBMP_ITER(si->linkphy_allowed_pbm, port) */
    }

    prev_port = 0;
    si->port_subport_base[prev_port] = SOC_ML_MAX_SUBPORTS - max_subport_available;
    si->port_num_subport[prev_port] = 0;

    if (SOC_PBMP_NOT_NULL(si->subtag_allowed_pbm)) {
        /* Iterate through SubTag ports and reserve subport indices */
        SOC_PBMP_ITER(si->subtag_allowed_pbm, port) {
            num_subport =
                soc_property_port_get(unit, port, spn_NUM_SUBPORTS, 0);

            if (num_subport == 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "\nCONFIG ERROR\n"
                    "num_subports_%d should be non-zero for SubTag port        %d\n\n"),
                    port, port));
                SOC_PBMP_PORT_REMOVE(si->subtag_allowed_pbm, port);
                continue;
            }

            if ((num_subport - 1) > available_subport) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "\nCONFIG ERROR\n"
                          "num_subports_%d=%d unavailable for SubTag port\n\n"),
                          port, num_subport));
                SOC_PBMP_PORT_REMOVE(si->subtag_allowed_pbm, port);
                continue;
            }

            if (max_subport_available <= 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "\nCONFIG ERROR\n"
                         "maximum subports %d are consumed already\n\n"),
                         SOC_ML_MAX_SUBPORTS));
                SOC_PBMP_PORT_REMOVE(si->subtag_allowed_pbm, port);
                continue;
            }


            /* Allocate all pp ports */
            for (i = 0; i < num_subport; i++) {
                if (i == 0) {
                    /* For the first subport, reuse pp port of parent */
                    SOC_PBMP_PORT_ADD(SOC_PORT_PP_BITMAP(unit, port), port);
                    SOC_PBMP_PORT_ADD(si->subtag_pp_port_pbm, port);
                    SOC_PBMP_PORT_ADD(si->enabled_subtag_pp_port_pbm, port);
                    continue;
                }
                SOC_PBMP_ITER(*pbmp_pp_unused, pp_port_free) {
                    SOC_PBMP_PORT_ADD(SOC_PORT_PP_BITMAP(unit, port), pp_port_free);
                    SOC_PBMP_PORT_ADD(si->subtag_pp_port_pbm, pp_port_free);
                    SOC_PBMP_PORT_REMOVE(*pbmp_pp_unused, pp_port_free);
                    /* Set this port in the enabled bmap by default at init */
                    SOC_PBMP_PORT_ADD(si->enabled_subtag_pp_port_pbm,
                                      pp_port_free);
                    break;
                }
            }
            
            si->port_subport_base[port] = si->port_subport_base[prev_port] + si->port_num_subport[prev_port];
            si->port_num_subport[port] = num_subport;
            si->port_linkphy_s1_base[port] = 0;
            available_subport -= (num_subport - 1);
            max_subport_available -= num_subport;
            prev_port = port;
        }
    }

    if (SOC_PBMP_NOT_NULL (si->subtag_pp_port_pbm)) {
        si->subtag_enabled = 1;
    }
    if (SOC_PBMP_NOT_NULL (si->linkphy_pp_port_pbm)) {
        si->linkphy_enabled = 1;
    }
}

int ml_iecell_port[2][4] = {
    {5, 6, 7, 8},  /* IECELL0 */
    {9, 10, 11, 12}  /* IECELL1*/
};
int 
soc_ml_iecell_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index)
{
    int i,j;
    int blk = -1;
    int idx = -1;

    for (i=0; i<2; i++) {
        for(j=0; j<4; j++) {
            if(port == ml_iecell_port[i][j]) {
                    blk = i;
                    idx = j;
                    break;
            }
        }
    }

    if (blktype == SOC_BLK_IECELL) {
        switch(blk)
        {
            case 0 :
                blk = IECELL0_BLOCK(unit);
            break; 
            case 1 :
                blk = IECELL1_BLOCK(unit);
            break; 
            default :
	        return SOC_E_PARAM;
        }
    } else {
        return SOC_E_PARAM;
    }

    if (block != NULL) {
        *block = blk;
    }
    if (index != NULL) {
        *index = idx;
    }
    if ((blk == -1) || (idx == -1)) {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}

void
soc_metrolite_sbus_ring_map_config(int unit)
{
    /*
     * SBUS ring and block number
     */
    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x62034000);
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x60377531);
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x00000031);
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x0);
    WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x0);
    WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x0);
    WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x0);
    WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x0);

    WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0);

    return;
}

/*
 * Metrolite chip driver functions.
 */
soc_functions_t soc_metrolite_drv_funs = {
    _soc_metrolite_misc_init,          /* soc_misc_init_f */
    _soc_metrolite_mmu_init,           /* soc_mmu_init_f */
    _soc_metrolite_age_timer_get,      /* soc_age_timer_get_f */
    _soc_metrolite_age_timer_max_get,  /* soc_age_timer_max_get_f */
    _soc_metrolite_age_timer_set,      /* soc_age_timer_set_f */
    _soc_saber2_tsce_firmware_set,  /* soc_phy_firmware_load_f */
    _soc_saber2_mdio_reg_read,      /* soc_sbus_mdio_read_f */
    _soc_saber2_mdio_reg_write,      /* soc_sbus_mdio_write_f */
    soc_metrolite_bond_info_init      /* soc_bond_options_init_f */
};

int
soc_ml_get_max_buffer_size(int unit, int value)
{
    int cval;
    cval = _soc_ml_mmu_intermediate_results.general_info.max_int_cell_buff_size;
    if ((cval <= 0) || (cval >= value)) {
        return value;
    } else {
        return cval;
    }
}

#endif /* BCM_METROLITE_SUPPORT */

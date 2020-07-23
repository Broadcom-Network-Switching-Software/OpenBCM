/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        saber2.c
 * Purpose:
 * Requires:
 */


#include <stddef.h>
#include <sal/core/boot.h>


#include <soc/katana2.h>
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
#include <soc/uc.h>

#ifdef BCM_SABER2_SUPPORT
#define _SOC_SB2_DEFIP_MAX_TCAMS   2
#define _SOC_SB2PLUS_DEFIP_MAX_TCAMS   6
#define _SOC_SB2_DEFIP_TCAM_DEPTH  1024

#include <bcm_int/esw/subport.h>

#define SB2_IDLE1    31
#ifdef BCM_WARM_BOOT_SUPPORT

/* Basic WB version. */
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)

/* Fixed issue in TDM sync. */
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)

/* Moved from magic number based versioning to scache version based system. */
/* Fix to sync XL ge ports. */
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)

/* Fix to sync XL ge ports with pbmp_xport_ge set. */
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)

/* Default Version - Latest */
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_4

/* Default Scache Partition - Latest */
#define BCM_FLEXIO_SCACHE_DEFAULT_PARTITION     0x1

static uint8  scache_begin_magic_number[]={0xAA,0xBB,0xCC,0xDD};
static uint8  scache_end_magic_number[]={0xDD,0xCC,0xBB,0xAA};
static bcm56260_tdm_info_t sb2_scache_tdm_info={0};
static uint32 sb2_scache_tdm[SB2_TDM_MAX_SIZE]={0};
static uint32 sb2_scache_speed[SB2_MAX_PHYSICAL_PORTS]={0};
static soc_port_details_t sb2_scache_port_details[SB2_MAX_PHYSICAL_PORTS+1]={{0}};
static uint8 sb2_2p5g_ports[SB2_MAX_PHYSICAL_PORTS]={0};

int _soc_saber2_flexio_scache_retrieve_part0(int unit);

#endif

#define PORTGROUP_SINGLE_MODE   4
#define PORTGROUP_DUAL_MODE     2
#define PORTGROUP_QUAD_MODE     1

/*TDM_A : 0:130MHz: 284 (41 * 7) Cycles Used */
#define SB2_TDM_A_SIZE    284
#define SB2_TDM_A_FREQ   130 
#define SB2_TDM_A_ROW    41
#define SB2_TDM_A_COL    7

/*TDM_AA : 1:130MHz: 140 (20 * 7) Cycles Used */
#define SB2_TDM_AA_SIZE    	141
#define SB2_TDM_AA_FREQ   	130 
#define SB2_TDM_AA_ROW   	21 
#define SB2_TDM_AA_COL    	7

const static uint32 sb2_tdm_56460_AA_ref[SB2_TDM_AA_SIZE] = {
    	/*Col:1*/       /*Col:2*/       /*Col:3*/       /*Col:4*/   /*Col:5*/   /*Col:6*/  /*Col:7*/
/*1*/	SB2_IDLE,	9,		25,		26,		27,	28,		7,
/*2*/	4,		10,		25,		26,		27,	28,		8,
/*3*/	1,		11,		25,		26,		27,	28,		SB2_CMIC,
/*4*/	SB2_LPBK,	12,		25,		26,		27,	28,		21,
/*5*/	2,		13,		25,		26,		27,	28,		22,
/*6*/	3,		14,		25,		26,		27,	28,		17,
/*7*/	5,		15,		25,		26,		27,	28,		18,
/*8*/	SB2_LPBK,	16,		25,		26,		27,	28,		SB2_CMIC,
/*9*/	6,		23,		25,		26,		27,	28,		19,
/*10*/	4,		24,		25,		26,		27,	28,		20,
/*11*/	3,		9,		25,		26,		27,	28,		7,
/*12*/	SB2_LPBK,	10,		25,		26,		27,	28,		8,
/*13*/	1,		11,		25,		26,		27,	28,		SB2_CMIC,
/*14*/	4,		12,		25,		26,		27,	28,		21,
/*15*/	2,		13,		25,		26,		27,	28,		22,
/*16*/	SB2_LPBK,	14,		25,		26,		27,	28,		17,
/*17*/	5,		15,		25,		26,		27,	28,		18,
/*18*/	3,	        16,		25,		26,		27,	28,		SB2_CMIC,
/*19*/	6,		23,		25,		26,		27,	28,		19,
/*20*/	SB2_LPBK,	24,		25,		26,		27,	28,		20,
/*21*/  SB2_IDLE        /* ================================= EMPTY ============================== */
};

/*TDM_A1: 2:24MHz: 13 (4 * 4) Cycles Used */
#define SB2_TDM_A1_SIZE    13
#define SB2_TDM_A1_FREQ   24 
#define SB2_TDM_A1_ROW    4
#define SB2_TDM_A1_COL    4
const static uint32 sb2_tdm_56263_A1_ref[SB2_TDM_A1_SIZE] = {
    	/*Col:1*/ /*Col:2*/    /*Col:3*/       /*Col:4*/   
/*1*/	1,		5,	SB2_LPBK,	SB2_CMIC,
/*2*/	2,		6,	SB2_LPBK,	SB2_CMIC,
/*3*/	3,		7,	SB2_LPBK,	SB2_IDLE,
/*4*/	SB2_IDLE        /* -------- EMPTY ------------ */
};

/*TDM_F 11:75MHz: 20 (5 * 4) Cycles Used */
#define SB2_TDM_F_SIZE   20
#define SB2_TDM_F_FREQ   75
#define SB2_TDM_F_ROW    5
#define SB2_TDM_F_COL    4
const static uint32 sb2_tdm_56233_F_ref[SB2_TDM_F_SIZE] = {
       /*Col:1*/  /*Col:2*/ /*Col:3*/ /*Col:4*/
/*1*/   SB2_LPBK,   1,  21,  5,
/*2*/   SB2_CMIC,   4,  22,  5,
/*3*/   SB2_LPBK,   2,  23,  5,
/*4*/   SB2_CMIC,   3,  24,  5,
/*5*/   SB2_LPBK,   4,  SB2_IDLE, SB2_IDLE
};


/*TDM_A1A: 3:37MHz: 84 (21 * 4) Cycles Used */
#define SB2_TDM_A1A_SIZE    	84
#define SB2_TDM_A1A_FREQ    	37
#define SB2_TDM_A1A_ROW		21
#define SB2_TDM_A1A_COL    	4
const static uint32 sb2_tdm_56263_A1A_ref[SB2_TDM_A1A_SIZE] = {
   	/*Col:1*/ 	/*Col:2*/ /*Col:3*/    /*Col:4*/   
/*1*/	1,		5,		9,	SB2_LPBK,
/*2*/	13,		2,		6,	SB2_CMIC,
/*3*/	10,		14,		3,	SB2_LPBK,
/*4*/	7,		11,		15,	SB2_CMIC,
/*5*/	4,		8,		12,	SB2_LPBK,
/*6*/	16,		1,		5,	SB2_CMIC,
/*7*/	9,		13,		2,	SB2_LPBK,
/*8*/	6,		10,		14,	SB2_CMIC,
/*9*/	3,		7,		11,	SB2_LPBK,
/*10*/	15,		4,		8,		12,
/*11*/	16,		SB2_IDLE,	SB2_IDLE,	1,
/*12*/	5,		9,		SB2_LPBK,	13,
/*13*/	2,		6,		SB2_CMIC,	10,
/*14*/	14,		3,		SB2_LPBK,	7,
/*15*/	11,		15,		SB2_CMIC,	4,
/*16*/	8,		12,		SB2_LPBK,	16,
/*17*/	1,		5,		SB2_CMIC,	9,
/*18*/	13,		2,		SB2_LPBK,	6,
/*19*/	10,		14,		SB2_CMIC,	3,
/*20*/	7,		11,		SB2_LPBK,	15,
/*21*/	4,		8,		12,		16,
};

/*TDM_B: 4:118MHz: 133 (23 * 6) Cycles Used */
#define SB2_TDM_B_SIZE    133
#define SB2_TDM_B_FREQ   118
#define SB2_TDM_B_ROW    23
#define SB2_TDM_B_COL    6
const static uint32 sb2_tdm_56260_B_ref[SB2_TDM_B_SIZE] = {
	/*Col:1*/     /*Col:2*/ 	/*Col:3*/ 	/*Col:4*/   	/*Col:5*/    /*Col:6*/
/*1*/	SB2_LPBK,	1,		27,		5,		25,		21,
/*2*/	SB2_LPBK,	2,		6,		27,		SB2_LPBK,	25,
/*3*/	22,		3,		SB2_LPBK,	27,		7,		25,
/*4*/	23,		4,		SB2_LPBK,	27,		8,		25,
/*5*/	24,		SB2_CMIC,	1,		SB2_LPBK,	27,         	5,
/*6*/	25,		SB2_LPBK,	21,		2,		27,		SB2_LPBK,
/*7*/	25,		6,		22,		SB2_LPBK,	27,		3,
/*8*/	25,		7,		23,		SB2_LPBK,	27,		4,
/*9*/	25,		8,		24,		SB2_LPBK,	SB2_CMIC,	27,
/*10*/	1,		25,		SB2_LPBK,   	5,          	21,		27,
/*11*/	SB2_LPBK,	25,		2,		6,		SB2_LPBK,	27,
/*12*/	22,		25,		SB2_LPBK,	3,		7,		27,
/*13*/	23,		25,		4,		SB2_LPBK,	8,		24,
/*14*/	27,		SB2_CMIC,	25,		SB2_LPBK,	1,		5,
/*15*/	27,		SB2_LPBK,	25,		21,		2,		6,
/*16*/	27,		SB2_LPBK,	25,		22,		3,		7,
/*17*/	27,		SB2_LPBK,	25,		23,		4,		8,
/*18*/	SB2_LPBK,	27,		24,		25,		SB2_IDLE,	SB2_IDLE,
/*19*/	SB2_LPBK,	27,		1,		25,		5,		SB2_LPBK,
/*20*/	21,		27,		2,		25,		6,		SB2_LPBK,
/*21*/	22,		27,		3,		25,		7,		23,
/*22*/	SB2_LPBK,	4,		27,		8,		25,		24,
/*23*/	SB2_CMIC        /* ------------------------- EMPTY ---------------------------- */
};
/*TDM_B1: 5:118MHz: 120 (20 * 6) Cycles Used */
#define SB2_TDM_B1_SIZE    120
#define SB2_TDM_B1_FREQ   118
#define SB2_TDM_B1_ROW    20
#define SB2_TDM_B1_COL    6
const static uint32 sb2_tdm_56260_B1_ref[SB2_TDM_B1_SIZE] = {
	/*Col:1*/     /*Col:2*/ 	/*Col:3*/ 	/*Col:4*/   	/*Col:5*/    /*Col:6*/
/*1*/	SB2_IDLE,	1,		5,		25,		27,		21,
/*2*/	SB2_LPBK,	1,		5,		25,		27,		22,
/*3*/	SB2_CMIC,	1,		5,		25,		27,		23,
/*4*/	28,		1,		5,		25,		27,		24,
/*5*/	26,		1,		5,		25,		27,		21,
/*6*/	SB2_LPBK,	1,		5,		25,		27,		22,
/*7*/	SB2_IDLE1,	1,		5,		25,		27,		23,
/*8*/	SB2_CMIC,	1,		5,		25,		27,		24,
/*9*/	SB2_IDLE1,	1,		5,		25,		27,		21,
/*10*/	SB2_LPBK,	1,		5,		25,		27,		22,
/*11*/	28,		1,		5,		25,		27,		23,
/*12*/	SB2_IDLE1,	1,		5,		25,		27,		24,
/*13*/	SB2_CMIC,	1,		5,		25,		27,		21,
/*14*/	SB2_LPBK,	1,		5,		25,		27,		22,
/*15*/	26,		1,		5,		25,		27,		23,
/*16*/	SB2_CMIC,	1,		5,		25,		27,		24,
/*17*/	1,		5,		25,		27,		21,		SB2_LPBK,
/*18*/	1,		5,		25,		27,		22,		SB2_CMIC,
/*19*/	1,		5,		25,		27,		23,		28,
/*20*/	1,		5,		25,		27,		24,		SB2_IDLE
};

/*TDM_B1A: 6:118MHz: 121 (21 * 6) Cycles Used */
#define SB2_TDM_B1A_SIZE    121
#define SB2_TDM_B1A_FREQ   118
#define SB2_TDM_B1A_ROW    21
#define SB2_TDM_B1A_COL    6
const static uint32 sb2_tdm_56462_B1A_ref[SB2_TDM_B1A_SIZE] = {
	/*Col:1*/     /*Col:2*/ 	/*Col:3*/ 	/*Col:4*/   	/*Col:5*/    /*Col:6*/
/*1*/	SB2_IDLE,	1,		5,		9,		25,		27,	
/*2*/	SB2_LPBK,	1,		5,		10,		25,		27,	
/*3*/	17,		1,		5,		11,		25,		27,	
/*4*/	SB2_CMIC,	1,		5,		12,		25,		27,	
/*5*/	18,		1,		5,		13,		25,		27,	
/*6*/	SB2_LPBK,	1,		5,		14,		25,		27,	
/*7*/	19,		1,		5,		15,		25,		27,	
/*8*/	SB2_CMIC,	1,		5,		16,		25,		27,	
/*9*/	20,		1,		5,		26,		25,		27,	
/*10*/	SB2_LPBK,	1,		5,		28,		25,		27,	
/*11*/	SB2_CMIC,	1,		5,		9,		25,		27,	
/*12*/	SB2_LPBK,	1,		5,		10,		25,		27,	
/*13*/	17,		1,		5,		11,		25,		27,	
/*14*/	SB2_CMIC,	1,		5,		12,		25,		27,	
/*15*/	18,		1,		5,		13,		25,		27,	
/*16*/	SB2_LPBK,	1,		5,		14,		25,		27,	
/*17*/	19,		1,		5,		15,		25,		27,	
/*18*/	SB2_CMIC,	1,		5,		16,		25,		27,	
/*19*/	20,		1,		5,		26,		25,		27,	
/*20*/	SB2_LPBK,	1,		5,		28,		25,		27,	
/*21*/	SB2_IDLE
};

/*TDM_C,C1,C2: 7:130MHz: 141 (21 * 6) Cycles Used */
#define SB2_TDM_C_SIZE    141
#define SB2_TDM_C_FREQ   130
#define SB2_TDM_C_ROW    21
#define SB2_TDM_C_COL    7
const static uint32 sb2_tdm_56460_C_ref[SB2_TDM_C_SIZE] = {
        /*Col:1*/ 	/*Col:2*/ /*Col:3*/ /*Col:4*/ /*Col:5*/    /*Col:6*/    /*Col:7*/
/*1*/	SB2_IDLE,	1,		5,	9,	21,		25,	27,
/*2*/	SB2_LPBK,	1,		5,	10,	21,		25,	27,
/*3*/	17,		1,		5,	11,	21,		25,	27,
/*4*/	SB2_CMIC,	1,		5,	12,	21,		25,	27,
/*5*/	18,		1,		5,	13,	21,		25,	27,
/*6*/	SB2_LPBK,	1,		5,	14,	21,		25,	27,
/*7*/	19,		1,		5,	15,	21,		25,	27,
/*8*/	SB2_CMIC,	1,		5,	16,	21,		25,	27,
/*9*/	20,		1,		5,	26,	21,		25,	27,
/*10*/	SB2_LPBK,	1,		5,	28,	21,		25,	27,
/*11*/	SB2_CMIC,	1,		5,	9,	21,		25,	27,
/*12*/	SB2_LPBK,	1,		5,	10,	21,		25,	27,
/*13*/	17,		1,		5,	11,	21,		25,	27,
/*14*/	SB2_CMIC,	1,		5,	12,	21,		25,	27,
/*15*/	18,		1,		5,	13,	21,		25,	27,
/*16*/	SB2_LPBK,	1,		5,	14,	21,		25,	27,
/*17*/	19,		1,		5,	15,	21,		25,	27,
/*18*/	SB2_CMIC,	1,		5,	16,	21,		25,	27,
/*19*/	20,		1,		5,	26,	21,		25,	27,
/*20*/	SB2_LPBK,	1,		5,	28,	21,		25,	27,
/*21*/	SB2_IDLE        /* -------------------- EMPTY ------------------------- */
};
/*TDM_C,C1,C2: 7:130MHz: 141 (21 * 6) Cycles Used */
#define SB2_TDM_C1_SIZE    141
#define SB2_TDM_C1_FREQ   130
#define SB2_TDM_C1_ROW    21
#define SB2_TDM_C1_COL    7
const static uint32 sb2_tdm_56460_C1_ref[SB2_TDM_C1_SIZE] = {
        /*Col:1*/ 	/*Col:2*/ /*Col:3*/ /*Col:4*/ /*Col:5*/    /*Col:6*/    /*Col:7*/
/*1*/	SB2_IDLE,	1,		5,	9,	13,		25,	27,	
/*2*/	SB2_LPBK,	1,		5,	9,	14,		25,	27,	
/*3*/	17,		1,		5,	9,	15,		25,	27,	
/*4*/	SB2_CMIC,	1,		5,	9,	16,		25,	27,	
/*5*/	18,		1,		5,	9,	13,		25,	27,	
/*6*/	SB2_LPBK,	1,		5,	9,	14,		25,	27,	
/*7*/	26,		1,		5,	9,	15,		25,	27,	
/*8*/	SB2_CMIC,	1,		5,	9,	16,		25,	27,	
/*9*/	28,		1,		5,	9,	13,		25,	27,	
/*10*/	SB2_LPBK,	1,		5,	9,	14,		25,	27,	
/*11*/	SB2_CMIC,	1,		5,	9,	15,		25,	27,	
/*12*/	SB2_LPBK,	1,		5,	9,	16,		25,	27,	
/*13*/	17,		1,		5,	9,	13,		25,	27,	
/*14*/	SB2_CMIC,	1,		5,	9,	14,		25,	27,	
/*15*/	18,		1,		5,	9,	15,		25,	27,	
/*16*/	SB2_LPBK,	1,		5,	9,	16,		25,	27,	
/*17*/	26,		1,		5,	9,	13,		25,	27,	
/*18*/	SB2_CMIC,	1,		5,	9,	14,		25,	27,	
/*19*/	28,		1,		5,	9,	15,		25,	27,	
/*20*/	SB2_LPBK,	1,		5,	9,	16,		25,	27,	
/*21*/	SB2_IDLE
};
/*TDM_C,C1,C2: 7:130MHz: 141 (21 * 6) Cycles Used */
#define SB2_TDM_C2_SIZE    141
#define SB2_TDM_C2_FREQ   130
#define SB2_TDM_C2_ROW    21
#define SB2_TDM_C2_COL    7
const static uint32 sb2_tdm_56460_C2_ref[SB2_TDM_C2_SIZE] = {
        /*Col:1*/ 	/*Col:2*/ /*Col:3*/ /*Col:4*/ /*Col:5*/    /*Col:6*/    /*Col:7*/
/*1*/	SB2_IDLE,	1,		5,	9,	13,		17,	21,	
/*2*/	SB2_LPBK,	1,		5,	9,	14,		17,	21,	
/*3*/	25,		1,		5,	9,	15,		17,	21,	
/*4*/	SB2_CMIC,	1,		5,	9,	16,		17,	21,	
/*5*/	26,		1,		5,	9,	13,		17,	21,	
/*6*/	SB2_LPBK,	1,		5,	9,	14,		17,	21,	
/*7*/	27,		1,		5,	9,	15,		17,	21,	
/*8*/	SB2_CMIC,	1,		5,	9,	16,		17,	21,	
/*9*/	28,		1,		5,	9,	13,		17,	21,	
/*10*/	SB2_LPBK,	1,		5,	9,	14,		17,	21,	
/*11*/	SB2_CMIC,	1,		5,	9,	15,		17,	21,	
/*12*/	SB2_LPBK,	1,		5,	9,	16,		17,	21,	
/*13*/	25,		1,		5,	9,	13,		17,	21,	
/*14*/	SB2_CMIC,	1,		5,	9,	14,		17,	21,	
/*15*/	26,		1,		5,	9,	15,		17,	21,	
/*16*/	SB2_LPBK,	1,		5,	9,	16,		17,	21,	
/*17*/	27,		1,		5,	9,	13,		17,	21,	
/*18*/	SB2_CMIC,	1,		5,	9,	14,		17,	21,	
/*19*/	28,		1,		5,	9,	15,		17,	21,	
/*20*/	SB2_LPBK,	1,		5,	9,	16,		17,	21,	
/*21*/	SB2_IDLE
};

/*TDM_D 8:130MHz: 168 (28 * 6) Cycles Used */
#define SB2_TDM_D_SIZE    168
#define SB2_TDM_D_FREQ   130
#define SB2_TDM_D_ROW    28
#define SB2_TDM_D_COL    6 
const static uint32 sb2_tdm_56460_D_ref[SB2_TDM_D_SIZE] = {
        /*Col:1*/ 	/*Col:2*/ 	/*Col:3*/ 	/*Col:4*/ 	/*Col:5*/    /*Col:6*/
/*1*/	SB2_IDLE,	27,		SB2_LPBK,	25,		21,		1,
/*2*/	17,		5,		SB2_LPBK,	27,		25,		21,
/*3*/	1,		SB2_CMIC,	SB2_LPBK,	20,		27,		25,
/*4*/	21,		1,		SB2_LPBK,	28,		27,		25,
/*5*/	21,		1,		SB2_LPBK,	18,		7,		27,
/*6*/	25,		21,		SB2_LPBK,	1,		19,		6,
/*7*/	27,		25,		SB2_LPBK,	21,		1,		8,
/*8*/	26,		27,		SB2_LPBK,	25,		21,		1,
/*9*/	SB2_CMIC,	SB2_IDLE1,	SB2_LPBK,	27,		25,		21,
/*10*/	1,		SB2_IDLE1,	SB2_LPBK,	SB2_CMIC,	27,		25,
/*11*/	21,		1,		SB2_LPBK,	5,		27,		25,
/*12*/	21,		1,		SB2_LPBK,	20,		SB2_CMIC,	27,
/*13*/	25,		21,		SB2_LPBK,	1,		17,		28,
/*14*/	27,		25,		SB2_LPBK,	21,		1,		7,
/*15*/	18,		27,		SB2_LPBK,	25,		21,		1,
/*16*/	19,		6,		SB2_LPBK,	27,		25,		21,
/*17*/	1,		26,		SB2_LPBK,	8,		27,		25,
/*18*/	21,		1,		SB2_LPBK,	SB2_CMIC,	27,		25,
/*19*/	21,		1,		SB2_LPBK,	SB2_IDLE1,	SB2_CMIC,	27,
/*20*/	25,		21,		SB2_LPBK,	1,		SB2_IDLE1,	5,
/*21*/	27,		25,		SB2_LPBK,	21,		1,		SB2_CMIC,
/*22*/	20,		27,		SB2_LPBK,	25,		21,		1,
/*23*/	17,		28,		SB2_LPBK,	27,		25,		21,
/*24*/	1,		7,		SB2_LPBK,	18,		27,		25,
/*25*/	21,		1,		SB2_LPBK,	6,		27,		25,
/*26*/	21,		1,		SB2_LPBK,	SB2_CMIC,	19,		27,
/*27*/	25,		21,		SB2_LPBK,	1,		8,		26,
/*28*/	27,		25,		SB2_LPBK,	21,		1,		SB2_IDLE
};
/*TDM_E,E1,E3 9:75MHz: 81 (21 * 4) Cycles Used */
#define SB2_TDM_E_SIZE    81
#define SB2_TDM_E_FREQ   75
#define SB2_TDM_E_ROW    21
#define SB2_TDM_E_COL    4 
const static uint32 sb2_tdm_56463_E_ref[SB2_TDM_E_SIZE] = {
	/*Col:1*/  /*Col:2*/ /*Col:3*/ /*Col:4*/
/*1*/	SB2_IDLE,	1,	25,	27,	
/*2*/	SB2_LPBK,	2,	25,	27,	
/*3*/	9,		3,	25,	27,	
/*4*/	SB2_CMIC,	4,	25,	27,	
/*5*/	10,		5,	25,	27,	
/*6*/	SB2_LPBK,	6,	25,	27,	
/*7*/	11,		7,	25,	27,	
/*8*/	SB2_CMIC,	8,	25,	27,	
/*9*/	12,		26,	25,	27,	
/*10*/	SB2_LPBK,	28,	25,	27,	
/*11*/	SB2_CMIC,	1,	25,	27,	
/*12*/	SB2_LPBK,	2,	25,	27,	
/*13*/	9,		3,	25,	27,	
/*14*/	SB2_CMIC,	4,	25,	27,	
/*15*/	10,		5,	25,	27,	
/*16*/	SB2_LPBK,	6,	25,	27,	
/*17*/	11,		7,	25,	27,	
/*18*/	SB2_CMIC,	8,	25,	27,	
/*19*/	12,		26,	25,	27,	
/*20*/	SB2_LPBK,	28,	25,	27,	
/*21*/	SB2_IDLE
};
/*TDM_E,E1,E3 9:75MHz: 81 (21 * 4) Cycles Used */
#define SB2_TDM_E1_SIZE    81
#define SB2_TDM_E1_FREQ   75
#define SB2_TDM_E1_ROW    21
#define SB2_TDM_E1_COL    4 
const static uint32 sb2_tdm_56463_E1_ref[SB2_TDM_E1_SIZE] = {
	/*Col:1*/  /*Col:2*/ /*Col:3*/ /*Col:4*/
/*1*/	SB2_IDLE,	1,	11,	25,	
/*2*/	SB2_LPBK,	2,	12,	26,	
/*3*/	21,		3,	13,	27,	
/*4*/	SB2_CMIC,	4,	14,	28,	
/*5*/	22,		5,	15,	25,	
/*6*/	SB2_LPBK,	6,	16,	26,	
/*7*/	23,		7,	17,	27,	
/*8*/	SB2_CMIC,	8,	18,	28,	
/*9*/	24,		9,	19,	25,	
/*10*/	SB2_LPBK,	10,	20,	26,	
/*11*/	SB2_CMIC,	1,	11,	27,	
/*12*/	SB2_LPBK,	2,	12,	28,	
/*13*/	21,		3,	13,	25,	
/*14*/	SB2_CMIC,	4,	14,	26,	
/*15*/	22,		5,	15,	27,	
/*16*/	SB2_LPBK,	6,	16,	28,	
/*17*/	23,		7,	17,	25,	
/*18*/	SB2_CMIC,	8,	18,	26,	
/*19*/	24,		9,	19,	27,	
/*20*/	SB2_LPBK,	10,	20,	28,	
/*21*/  SB2_IDLE
};
/*TDM_E,E1,E3 9:75MHz: 81 (21 * 4) Cycles Used */
#define SB2_TDM_E3_SIZE    81
#define SB2_TDM_E3_FREQ   75
#define SB2_TDM_E3_ROW    21
#define SB2_TDM_E3_COL    4 
const static uint32 sb2_tdm_56463_E3_ref[SB2_TDM_E3_SIZE] = {
	/*Col:1*/  /*Col:2*/ /*Col:3*/ /*Col:4*/
/*1*/	SB2_IDLE,	1,	5,	25,	
/*2*/	SB2_LPBK,	1,	5,	25,	
/*3*/	21,		1,	5,	25,	
/*4*/	SB2_CMIC,	1,	5,	25,	
/*5*/	22,		1,	5,	25,	
/*6*/	SB2_LPBK,	1,	5,	25,	
/*7*/	23,		1,	5,	25,	
/*8*/	SB2_CMIC,	1,	5,	25,	
/*9*/	24,		1,	5,	25,	
/*10*/	SB2_LPBK,	1,	5,	25,	
/*11*/	SB2_CMIC,	1,	5,	25,	
/*12*/	SB2_LPBK,	1,	5,	25,	
/*13*/	21,		1,	5,	25,	
/*14*/	SB2_CMIC,	1,	5,	25,	
/*15*/	22,		1,	5,	25,	
/*16*/	SB2_LPBK,	1,	5,	25,	
/*17*/	23,		1,	5,	25,	
/*18*/	SB2_CMIC,	1,	5,	25,	
/*19*/	24,		1,	5,	25,	
/*20*/	SB2_LPBK,	1,	5,	25,	
/*21*/	SB2_IDLE
};

/*TDM_EA 10:75MHz: 16 (4 * 4) Cycles Used */
#define SB2_TDM_EA_SIZE   16 
#define SB2_TDM_EA_FREQ   75
#define SB2_TDM_EA_ROW    4
#define SB2_TDM_EA_COL    4 
const static uint32 sb2_tdm_56260_EA_ref[SB2_TDM_EA_SIZE] = {
	/*Col:1*/  /*Col:2*/ /*Col:3*/ /*Col:4*/
/*1*/	SB2_IDLE,	1,	5,	SB2_LPBK,
/*2*/	25,		2,	6,	27,
/*3*/	26,		3,	7,	28,
/*4*/	SB2_CMIC,	4,	8,	SB2_IDLE
};

uint32 sb2_current_tdm[SB2_TDM_MAX_SIZE]={0};
uint32 sb2_current_tdm_size=0;

int block_max_startaddr[SB2_MAX_BLOCKS]={0};
int block_max_endaddr[SB2_MAX_BLOCKS]={0};

const static  sb2_port_speeds_t sb2_port_speeds_56260_0 = {{0,0,0,0}};
const static  sb2_port_speeds_t sb2_port_speeds_56460_1 = {
              {10000,2500,0,2500},{10000,2500,0,2500},{10000,2500,0,2500},
              {10000,2500,0,2500},{10000,2500,0,2500},{10000,2500,0,2500},
              {10000,10000,0,10000}};

const static  sb2_port_speeds_t sb2_port_speeds_56260_1 = {
              {10000,2500,0,2500},{10000,2500,0,2500},{0,0,0,0},
              {0,0,0,0},{0,0,0,0},{10000,2500,0,2500},
              {10000,10000,0,2500}};

const static  sb2_port_speeds_t sb2_port_speeds_56233_1 = {
              {1000,1000,1000,2500},{0,0,0,0},{0,0,0,0},
              {0,0,0,0},{0,0,0,0},{1000,1000,1000,1000},
              {0,0,0,0}};

sb2_port_speeds_t *sb2_port_speeds[SOC_MAX_NUM_DEVICES]={NULL};
#define SB2_QUEUE_FLUSH_RATE_MANTISSA    1023
#define SB2_QUEUE_FLUSH_RATE_EXP         14
#define SB2_QUEUE_FLUSH_BURST_MANTISSA   124
#define SB2_QUEUE_FLUSH_BURST_EXP        14
#define SB2_QUEUE_FLUSH_CYCLE_SEL        0

static uint32 sb2_tdm_A_56260_0[SB2_TDM_A_SIZE]={0}; /* Dummy */
const static soc_port_details_t sb2_port_details_56260_0[]={{0,0,0,0}};
const  static sb2_speed_t sb2_speed_56260_0 = {0};
soc_port_details_t  *sb2_selected_port_details;
soc_port_details_t  *sb2_selected_port_details_orig;

/* TDM, Port speeds and port details for all IO  */


/* 56460_1: bcm5626x_config = 1 */
/* 24xGE + 4xHGs11  4xGE 4xGE 4xGE + 4xGE 4xGE 4xGE 4xGE N4 2.5G(LPBK) */
static uint32 sb2_tdm_A_56460_1[SB2_TDM_AA_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56460_1 = {
       1000, 1000, 1000, 1000,      /* 1  - 4  */
       1000, 1000, 1000, 1000,      /* 5  - 8  */
       1000, 1000, 1000, 1000,      /* 9  - 12 */
       1000, 1000, 1000, 1000,      /* 13 - 16 */
       1000, 1000, 1000, 1000,      /* 17 - 20 */
       1000, 1000, 1000, 1000,      /* 21 - 24 */
       10000, 10000, 10000, 10000   /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56460_1[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,1,GE_PORT,1000},
    {5,8,1,GE_PORT,1000},
    {9,12,1,GE_PORT,1000},
    {13,16,1,GE_PORT,1000},
    {17,20,1,GE_PORT,1000},
    {21,24,1,GE_PORT,1000},
    {25,28,1,HG_PORT,10000},
    {0,0,0,0},
    {0,0,0,0} /* End */
};

/* 56460_2: bcm5626x_config = 2 */
/* F.XAUI F.XAUI F.XAUI	F.XAUI F.XAUI F.XAUI 4xGE 2.5G(LPBK) */
static uint32 sb2_tdm_C2_56460_2[SB2_TDM_C2_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56460_2 = {
       10000, 0000, 0000, 0000,      /* 1  - 4  */
       10000, 0000, 0000, 0000,      /* 5  - 8  */
       10000, 0000, 0000, 0000,      /* 9  - 12 */
       10000, 0000, 0000, 0000,      /* 13 - 16 */
       10000, 0000, 0000, 0000,      /* 17 - 20 */
       10000, 0000, 0000, 0000,      /* 21 - 24 */
       1000,  1000, 1000, 1000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56460_2[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,4,XE_PORT,10000},
    {5,8,4,XE_PORT,10000},
    {9,12,4,XE_PORT,10000},
    {13,16,4,XE_PORT,10000},
    {17,20,4,XE_PORT,10000},
    {21,24,4,XE_PORT,10000},
    {25,28,1,GE_PORT,1000},
    {0,0,0,0},
    {0,0,0,0} /* End */
};

/* 56460_3: bcm5626x_config = 3 */
/* 2xF.XAUI + 12xGE + 1xF.XAUI + [2xXFI + 2xGE] + LPBK-2.5G */
static uint32 sb2_tdm_C_56460_3[SB2_TDM_C_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56460_3 = {
       10000, 0000, 0000, 0000,      /* 1  - 4  */
       10000, 0000, 0000, 0000,      /* 5  - 8  */
       1000,  1000, 1000, 1000,      /* 9  - 12 */
       1000,  1000, 1000, 1000,      /* 13 - 16 */
       1000,  1000, 1000, 1000,      /* 17 - 20 */
       10000, 0000, 0000, 0000,      /* 21 - 24 */
       10000, 1000,10000, 1000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56460_3[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,4, XE_PORT,10000},
    {5,8,4, XE_PORT,10000},
    {9,12,1,GE_PORT,1000},
    {13,16,1,GE_PORT,1000},
    {17,20,1,GE_PORT,1000},
    {21,24,4,XE_PORT,10000},
    {25,27,2,XE_PORT,10000},
    {26,28,2,GE_PORT,1000},
    {0,0,0,0} /* End */
};

/* 56460_4: bcm5626x_config = 4 */
/* 3x F.XAUI + 2x1G + .. 1xF.XAUI  + [2xXFI + 2x1G] + LPBK-2.5G */
static uint32 sb2_tdm_C1_56460_4[SB2_TDM_C1_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56460_4 = {
       10000, 0000, 0000, 0000,      /* 1  - 4  */
       10000, 0000, 0000, 0000,      /* 5  - 8  */
       10000, 0000, 0000, 0000,      /* 9  - 12 */
       1000,  0000, 1000, 0000,      /* 13 - 16 */
       0000,  0000, 0000, 0000,      /* 17 - 20 */
       10000, 0000, 0000, 0000,      /* 21 - 24 */
       10000, 1000,10000, 1000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56460_4[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,4, XE_PORT,10000},
    {5,8,4, XE_PORT,10000},
    {9,12,4, XE_PORT,10000},
    {13,16,2,GE_PORT,1000},
    {17,20,4,0,0},
    {21,24,4,XE_PORT,10000},
    {25,27,2,XE_PORT,10000},
    {26,28,2,GE_PORT,1000},
    {0,0,0,0} /* End */
};
/* 56460_5: bcm5626x_config = 5 */
/* F.XAUI F.XAUI F.XAUI	- 4xGE	F.XAUI	N2 */

static uint32 sb2_tdm_C1_56460_5[SB2_TDM_C1_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56460_5 = {
       10000, 0000, 0000, 0000,      /* 1  - 4  */
       10000, 0000, 0000, 0000,      /* 5  - 8  */
       10000, 0000, 0000, 0000,      /* 9  - 12 */
       0000,  0000, 0000, 0000,      /* 13 - 16 */
       1000,  1000, 1000, 1000,      /* 17 - 20 */
       10000, 0000, 0000, 0000,      /* 21 - 24 */
       10000, 0000,10000, 0000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56460_5[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,4, XE_PORT,10000},
    {5,8,4, XE_PORT,10000},
    {9,12,4, XE_PORT,10000},
    {13,16,1, 0, 0},
    {17,20,1,GE_PORT,1000},
    {21,24,4,XE_PORT,10000},
    {25,28,2,HG_PORT,10000},
    {0,0,0,0},
    {0,0,0,0} /* End */
};

/* 56460_6: bcm5626x_config = 6 */
/* F.XAUI	4xGE	-	-	4xGE	F.XAUI	N4s	12.5G */

static uint32 sb2_tdm_D_56460_6[SB2_TDM_D_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56460_6 = {
       10000, 0000, 0000, 0000,      /* 1  - 4  */
       1000,  1000, 1000, 1000,      /* 5  - 8  */
       0000,  0000, 0000, 0000,      /* 9  - 12 */
       0000,  0000, 0000, 0000,      /* 13 - 16 */
       1000,  1000, 1000, 1000,      /* 17 - 20 */
       10000, 0000, 0000, 0000,      /* 21 - 24 */
       10000, 1000,10000, 1000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56460_6[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,4, XE_PORT,10000},
    {5,8,1, GE_PORT,1000},
    {9,12,4, 0, 0},
    {13,16,4, 0, 0},
    {17,20,1, GE_PORT,1000},
    {21,24,4,XE_PORT,10000},
    {25,27,2,XE_PORT,10000},
    {26,28,2,GE_PORT,1000},
    {0,0,0,0} /* End */
};
/* 56462_1: bcm5626x_config = 1 */
/* F.XAUI	F.XAUI	F.XAUI	4xGE	4xGE	4xGE	F.XAUI	2.5G	B1A */
static uint32 sb2_tdm_B1A_56462_1[SB2_TDM_B1A_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56462_1 = {
       10000, 0000, 0000, 0000,      /* 1  - 4  */
       10000, 0000, 0000, 0000,      /* 5  - 8  */
       10000, 0000, 0000, 0000,      /* 9  - 12 */
       1000,  1000, 1000, 1000,      /* 13 - 16 */
       1000,  1000, 1000, 1000,      /* 17 - 20 */
       1000,  1000, 1000, 1000,      /* 21 - 24 */
       10000, 0000, 0000, 0000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56462_1[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,4, XE_PORT,10000},
    {5,8,4, XE_PORT,10000},
    {9,12,4, XE_PORT,10000},
    {13,16,1, GE_PORT,1000},
    {17,20,1, GE_PORT,1000},
    {21,24,1, GE_PORT,1000},
    {25,26,4, XE_PORT,10000},
    {0,0,0,0},
    {0,0,0,0} /* End */
};

/* 56462_2: bcm5626x_config = 2 */
/* F.XAUI	F.XAUI	-	4xGE	4xGE	4xGE	N4s	2.5G	B1A */
static uint32 sb2_tdm_B1A_56462_2[SB2_TDM_B1A_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56462_2 = {
       10000, 0000, 0000,  0000,      /* 1  - 4  */
       10000, 0000, 0000,  0000,      /* 5  - 8  */
       0000,  0000, 0000,  0000,      /* 9  - 12 */
       1000,  1000, 1000,  1000,      /* 13 - 16 */
       1000,  1000, 1000,  1000,      /* 17 - 20 */
       1000,  1000, 1000,  1000,      /* 21 - 24 */
       10000, 1000, 10000, 1000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56462_2[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,4, XE_PORT,10000},
    {5,8,4, XE_PORT,10000},
    {9,12,4, 0, 0},
    {13,16,1, GE_PORT,1000},
    {17,20,1, GE_PORT,1000},
    {20,24,1, GE_PORT,1000},
    {25,27,2,XE_PORT,10000},
    {26,28,2,GE_PORT,1000},
    {0,0,0,0} /* End */
};

/* 56463_1: bcm5626x_config = 1 */
/* 4xGE	4xGE	-	-	-	4xGE	N4s	2.5G */
static uint32 sb2_tdm_E_56463_1[SB2_TDM_E_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56463_1 = {
       1000,  1000, 1000,  1000,      /* 1  - 4  */
       1000,  1000, 1000,  1000,      /* 5  - 8  */
       0000,  0000, 0000,  0000,      /* 9  - 12 */
       0000,  0000, 0000,  0000,      /* 13 - 16 */
       0000,  0000, 0000,  0000,      /* 17 - 20 */
       1000,  1000, 1000,  1000,      /* 21 - 24 */
       10000, 1000, 10000, 1000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56463_1[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,1, GE_PORT,1000},
    {5,8,1, GE_PORT,1000},
    {9,12,4, 0, 0},
    {13,16,4, 0, 0},
    {17,20,4, 0, 0},
    {21,24,1, GE_PORT,1000},
    {25,27,2,XE_PORT,10000},
    {26,28,2,GE_PORT,1000},
    {0,0,0,0} /* End */
};


/* 56463_2: bcm5626x_config = 2 */
/* 4xGE	4xGE	4xGE	4xGE	4xGE	4xGE	4x2.5G	2.5G	E1 */
static uint32 sb2_tdm_E1_56463_2[SB2_TDM_E1_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56463_2 = {
       1000, 1000, 1000,  1000,      /* 1  - 4  */
       1000, 1000, 1000,  1000,      /* 5  - 8  */
       1000, 1000, 1000,  1000,      /* 9  - 12 */
       1000, 1000, 1000,  1000,      /* 13 - 16 */
       1000, 1000, 1000,  1000,      /* 17 - 20 */
       1000, 1000, 1000,  1000,      /* 21 - 24 */
       2500, 2500, 2500,  2500       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56463_2[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,1, GE_PORT,1000},
    {5,8,1, GE_PORT,1000},
    {9,12,1, GE_PORT,1000},
    {13,16,1, GE_PORT,1000},
    {17,20,1, GE_PORT,1000},
    {21,24,1, GE_PORT,1000},
    {25,28,1,GE_PORT,2500},
    {0,0,0,0},
    {0,0,0,0} /* End */
};

/* 56463_3: bcm5626x_config = 3 */
/* 4xGE	F.XAUI	-	-	-	F.XAUI	F.XAUI	2.5G	E3 */
static uint32 sb2_tdm_E3_56463_3[SB2_TDM_E3_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56463_3 = {
       1000,  1000, 1000,  1000,      /* 1  - 4  */
       10000, 0000, 0000,  0000,      /* 5  - 8  */
       0000,  0000, 0000,  0000,      /* 9  - 12 */
       0000,  0000, 0000,  0000,      /* 13 - 16 */
       0000,  0000, 0000,  0000,      /* 17 - 20 */
       10000, 0000, 0000,  0000,      /* 21 - 24 */
       10000, 0000, 0000,  0000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56463_3[SB2_MAX_PHYSICAL_PORTS+1]={
    {1,4,1, GE_PORT,1000},
    {5,8,4,XE_PORT,10000},
    {9,12,4, 0, 0},
    {13,16,4, 0, 0},
    {17,20,4, 0, 0},
    {21,24,4, XE_PORT,10000},
    {25,25,1,XE_PORT,10000},
    {0,0,0,0},
    {0,0,0,0} /* End */
};




/* 56260_1: bcm5626x_config = 1 */
/* 12x 2.5G + 2xXFI + 12.5G LPBK */
static uint32 sb2_tdm_B_56260_1[SB2_TDM_B_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56260_1 = {
       2500, 2500, 2500, 2500,      /* 1  - 4  */
       2500, 2500, 2500, 2500,      /* 5  - 8  */
       0,    0,    0,    0,         /* 9  - 12 */
       0,    0,    0,    0,         /* 13 - 16 */
       0,    0,    0,    0,         /* 17 - 20 */
       2500, 2500, 2500, 2500,      /* 21 - 24 */
       10000, 0, 10000,  0          /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56260_1[SB2_MAX_PHYSICAL_PORTS+1]={
    {1, 4, 1,GE_PORT,2500},
    {5, 8, 1,GE_PORT,2500},
    {9,12,4, 0, 0},
    {13,16,4, 0, 0},
    {17,20,4, 0, 0},
    {21,24,1,GE_PORT,2500},
    {25,28,2,HG_PORT,10000},
    {0,0,0,0},
    {0,0,0,0} /* End */
};

/* 56260_2: bcm5626x_config = 2 */
/* F.XAUI	F.XAUI				F.XAUI	N4s	2.5G	B1 */
static uint32 sb2_tdm_B1_56260_2[SB2_TDM_B1_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56260_2 = {
       10000, 0000, 0000,  0000,      /* 1  - 4  */
       10000, 0000, 0000,  0000,      /* 5  - 8  */
       0000,  0000, 0000,  0000,      /* 9  - 12 */
       0000,  0000, 0000,  0000,      /* 13 - 16 */
       0000,  0000, 0000,  0000,      /* 17 - 20 */
       10000, 0000, 0000,  0000,      /* 21 - 24 */
       10000, 1000, 10000, 1000          /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56260_2[SB2_MAX_PHYSICAL_PORTS+1]={
    {1, 4, 4,XE_PORT,10000},
    {5, 8, 4,XE_PORT,10000},
    {9,12,4, 0, 0},
    {13,16,4, 0, 0},
    {17,20,4, 0, 0},
    {21,24,4,XE_PORT,10000},
    {25,27,2,XE_PORT,10000},
    {26,28,2,GE_PORT,1000},
    {0,0,0,0} /* End */
};

/* 56262_1: bcm5626x_config = 1 */
/* 4x2.5G 4x2.5G - - - - - 2.5G	EA 75 MHz */
static uint32 sb2_tdm_EA_56262_1[SB2_TDM_EA_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56262_1 = {
       2500, 2500, 2500, 2500,      /* 1  - 4  */
       2500, 2500, 2500, 2500,      /* 5  - 8  */
       0000, 0000, 0000, 0000,      /* 9  - 12 */
       0000, 0000, 0000, 0000,      /* 13 - 16 */
       0000, 0000, 0000, 0000,      /* 17 - 20 */
       0000, 0000, 0000, 0000,      /* 21 - 24 */
       0000, 0000, 0000, 0000          /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56262_1[SB2_MAX_PHYSICAL_PORTS+1]={
    {1, 4, 1,GE_PORT,2500},
    {5, 8, 1,GE_PORT,2500},
    {9,12,4, 0, 0},
    {13,16,4, 0, 0},
    {17,20,4, 0, 0},
    {21,24,4, 0, 0},
    {25,28,4, 0, 0},
    {0,0,0,0},
    {0,0,0,0} /* End */
};

/* 56263_1: bcm5626x_config = 1 */
/* 4xGE	4xGE - - - 4xGE 4xGE 2.5G A1A 37 MHz */
static uint32 sb2_tdm_A1A_56263_1[SB2_TDM_A1A_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56263_1 = {
       1000, 1000, 1000, 1000,      /* 1  - 4  */
       1000, 1000, 1000, 1000,      /* 5  - 8  */
       0000, 0000, 0000, 0000,      /* 9  - 12 */
       0000, 0000, 0000, 0000,      /* 13 - 16 */
       0000, 0000, 0000, 0000,      /* 17 - 20 */
       1000, 1000, 1000, 1000,      /* 21 - 24 */
       1000, 1000, 1000, 1000          /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56263_1[SB2_MAX_PHYSICAL_PORTS+1]={
    {1, 4, 1,GE_PORT,1000},
    {5, 8, 1,GE_PORT,1000},
    {9,12,4, 0, 0},
    {13,16,4, 0, 0},
    {17,20,4, 0, 0},
    {21, 24, 1,GE_PORT,1000},
    {25, 28, 1,GE_PORT,1000},
    {0,0,0,0},
    {0,0,0,0} /* End */
};

/* 56263_2: bcm5626x_config = 2 */
/* 3xGE	3xGE	- - - -	- 2.5G	A1 24 MHz */
static uint32 sb2_tdm_A1_56263_2[SB2_TDM_A1_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56263_2 = {
       1000, 1000, 1000, 0000,      /* 1  - 4  */
       1000, 1000, 1000, 0000,      /* 5  - 8  */
       0000, 0000, 0000, 0000,      /* 9  - 12 */
       0000, 0000, 0000, 0000,      /* 13 - 16 */
       0000, 0000, 0000, 0000,      /* 17 - 20 */
       0000, 0000, 0000, 0000,      /* 21 - 24 */
       0000, 0000, 0000, 0000          /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56263_2[SB2_MAX_PHYSICAL_PORTS+1]={
    {1, 3, 1,GE_PORT,1000},
    {5, 7, 1,GE_PORT,1000},
    {9,12,4, 0, 0},
    {13,16,4, 0, 0},
    {17,20,4, 0, 0},
    {21,24,4, 0, 0},
    {25,28,4, 0, 0},
    {0,0,0,0},
    {0,0,0,0} /* End */
};

/* 56233_1: bcm56233_config = 1 */
/* 7xGE + 1x2.5GE	- - - -	- 2.5G	F 75 MHz */
static uint32 sb2_tdm_F_56233_1[SB2_TDM_F_SIZE]={0}; /* Filled up Runtime */
const  static sb2_speed_t sb2_speed_56233_1 = {
       1000, 1000, 1000, 2500,      /* 1  - 4  */
       2500, 0000, 0000, 0000,      /* 5  - 8  */
       0000, 0000, 0000, 0000,      /* 9  - 12 */
       0000, 0000, 0000, 0000,      /* 13 - 16 */
       0000, 0000, 0000, 0000,      /* 17 - 20 */
       1000, 1000, 1000, 1000,      /* 21 - 24 */
       0000, 0000, 0000, 0000       /* 25 - 28 */
};
const static soc_port_details_t sb2_port_details_56233_1[SB2_MAX_PHYSICAL_PORTS+1]={
    {1, 3, 1,GE_PORT,1000},
    {4, 4, 1,GE_PORT,2500},
    {5, 8, 4, GE_PORT, 2500},
    {9,12,4, 0, 0},
    {13,16,4, 0, 0},
    {17,20,4, 0, 0},
    {21,24,1, GE_PORT,1000},
    {25,28,4, 0, 0},
    {0,0,0,0},
    {0,0,0,0} /* End */
};


sb2_tdm_pos_info_t sb2_tdm_pos_info[SB2_MAX_BLOCKS]={{0}};
static uint8  old_tdm_no=0;

soc_field_t sb2_srcport_field_names[SB2_MAX_LOGICAL_PORTS]={
            SRCPORT_0f,
            SRCPORT_1f,  SRCPORT_2f,  SRCPORT_3f,  SRCPORT_4f,
            SRCPORT_5f,  SRCPORT_6f,  SRCPORT_7f,  SRCPORT_8f,
            SRCPORT_9f,  SRCPORT_10f, SRCPORT_11f, SRCPORT_12f,
            SRCPORT_13f, SRCPORT_14f, SRCPORT_15f, SRCPORT_16f,
            SRCPORT_17f, SRCPORT_18f, SRCPORT_19f, SRCPORT_20f,
            SRCPORT_21f, SRCPORT_22f, SRCPORT_23f, SRCPORT_24f,
            SRCPORT_25f, SRCPORT_26f, SRCPORT_27f, SRCPORT_28f,
            SRCPORT_29f
            };


static sb2_port_to_block_t sb2_port_to_block_s={
                                        0,0,0,0, /* 1-4   mxq0 */
                                        1,1,1,1, /* 5-8   mxq1 */
                                        2,2,2,2, /* 9-12  mxq2 */
                                        3,3,3,3, /* 13-16 mxq3 */
                                        4,4,4,4, /* 17-20 mxq4 */
                                        5,5,5,5, /* 21-24 mxq5 */
                                        6,6,6,6  /* 25-28 xl */
};
static sb2_port_to_block_t *sb2_port_to_block[SOC_MAX_NUM_DEVICES]={NULL};

sb2_port_to_block_subports_t sb2_port_to_block_subports_s={
                                                 0,1,2,3, /* 1-4   mxq0*/
                                                 0,1,2,3, /* 5-8   mxq1*/
                                                 0,1,2,3, /* 9-12  mxq2*/
                                                 0,1,2,3, /* 13-16 mxq3*/
                                                 0,1,2,3, /* 17-20 mxq4*/
                                                 0,1,2,3, /* 21-24 mxq5*/
                                                 0,1,2,3  /* 25-28 xl */
};
sb2_port_to_block_subports_t
        *sb2_port_to_block_subports[SOC_MAX_NUM_DEVICES]={NULL};

sb2_block_ports_t sb2_block_ports_s={
                                          {1,2,3,4},
                                          {5,6,7,8},
                                          {9,10,11,12},
                                          {13,14,15,16},
                                          {17,18,19,20},
                                          {21,22,23,24},
                                          {25,26,27,28},
};
sb2_block_ports_t *sb2_block_ports[SOC_MAX_NUM_DEVICES]={NULL};

typedef struct tdm_port_slots_info_s  {
        int32                        position;
        struct tdm_port_slots_info_s *prev;
        struct tdm_port_slots_info_s *next;
}tdm_port_slots_info_t;

tdm_port_slots_info_t sb2_tdm_port_slots_info[SB2_MAX_LOGICAL_PORTS];


static uint32 *bcm56260_tdm[]={
               sb2_tdm_A_56260_0,   /* Dummy to match cfg_num and index */
               sb2_tdm_A_56460_1,   /* 56460_1 Cfg=1:IntCfg=1 */
               sb2_tdm_C2_56460_2,   /* 56460 Cfg=2:IntCfg=2 */
               sb2_tdm_C_56460_3,   /* 56460 Cfg=3:IntCfg=3 */
               sb2_tdm_C1_56460_4,   /* 56460 Cfg=4:IntCfg=4 */
               sb2_tdm_C1_56460_5,   /* 56460 Cfg=5:IntCfg=5 */
               sb2_tdm_D_56460_6,   /* 56460 Cfg=6:IntCfg=6 */
               sb2_tdm_B1A_56462_1, /* 56462 Cfg=1:IntCfg=7 */
               sb2_tdm_B1A_56462_2, /* 56462 Cfg=2:IntCfg=8 */
               sb2_tdm_E_56463_1, /* 56463 Cfg=1:IntCfg=9 */
               sb2_tdm_E1_56463_2, /* 56463 Cfg=2:IntCfg=10 */
               sb2_tdm_E3_56463_3, /* 56463 Cfg=3:IntCfg=11 */
               sb2_tdm_B_56260_1,  /* 56260 Cfg=1:IntCfg=12 */
               sb2_tdm_B1_56260_2,    /* 56260 Cfg=2:IntCfg=13 */
               sb2_tdm_EA_56262_1,    /* 56262 Cfg=1:IntCfg=14 */
               sb2_tdm_A1A_56263_1,    /* 56263 Cfg=1:IntCfg=15 */
               sb2_tdm_A1_56263_2,    /* 56263 Cfg=2:IntCfg=16 */
               sb2_tdm_F_56233_1     /* 56233 Cfg=1:IntCfg=17 */
#ifdef BCM_WARM_BOOT_SUPPORT
               ,sb2_scache_tdm                   /*Dummy TDM reserved for WB */
#endif
};
const static uint32 *bcm56260_speed_s[]={
               sb2_speed_56260_0,               /* Dummy */
               sb2_speed_56460_1,               /* 56460 Cfg=1:IntCfg=1 */
               sb2_speed_56460_2,               /* 56460 Cfg=2:IntCfg=2 */
               sb2_speed_56460_3,               /* 56460 Cfg=3:IntCfg=3 */
               sb2_speed_56460_4,               /* 56460 Cfg=4:IntCfg=4 */
               sb2_speed_56460_5,               /* 56460 Cfg=5:IntCfg=5 */
               sb2_speed_56460_6,               /* 56460 Cfg=6:IntCfg=6 */
               sb2_speed_56462_1,               /* 56462 Cfg=1:IntCfg=7 */
               sb2_speed_56462_2,               /* 56462 Cfg=2:IntCfg=8 */
               sb2_speed_56463_1,               /* 56463 Cfg=1:IntCfg=9 */
               sb2_speed_56463_2,               /* 56463 Cfg=2:IntCfg=10 */
               sb2_speed_56463_3,               /* 56463 Cfg=3:IntCfg=11 */
               sb2_speed_56260_1,                /* 56260 Cfg=1:IntCfg=12 */
               sb2_speed_56260_2,                /* 56260 Cfg=2:IntCfg=13 */
               sb2_speed_56262_1,                /* 56262 Cfg=1:IntCfg=14 */
               sb2_speed_56263_1,                /* 56263 Cfg=1:IntCfg=15 */
               sb2_speed_56263_2,                /* 56263 Cfg=2:IntCfg=16 */
               sb2_speed_56233_1                 /* 56233 Cfg=1:IntCfg=17 */
#ifdef BCM_WARM_BOOT_SUPPORT
               ,sb2_scache_speed                /* Dummy speed reserved for WB*/
#endif
};
sb2_speed_t *bcm56260_speed[SOC_MAX_NUM_DEVICES]={NULL};

const static soc_port_details_t  *sb2_port_details[]={
               sb2_port_details_56260_0,          /* Dummy */
               sb2_port_details_56460_1,          /* 56460 Cfg=1:IntCfg=1 */
               sb2_port_details_56460_2,          /* 56460 Cfg=2:IntCfg=2 */
               sb2_port_details_56460_3,          /* 56460 Cfg=3:IntCfg=3 */
               sb2_port_details_56460_4,          /* 56460 Cfg=4:IntCfg=4 */
               sb2_port_details_56460_5,          /* 56460 Cfg=5:IntCfg=5 */
               sb2_port_details_56460_6,          /* 56460 Cfg=6:IntCfg=6 */
               sb2_port_details_56462_1,          /* 56462 Cfg=1:IntCfg=7 */
               sb2_port_details_56462_2,          /* 56462 Cfg=2:IntCfg=8 */
               sb2_port_details_56463_1,          /* 56463 Cfg=1:IntCfg=9 */
               sb2_port_details_56463_2,          /* 56463 Cfg=2:IntCfg=10 */
               sb2_port_details_56463_3,          /* 56463 Cfg=3:IntCfg=11 */
               sb2_port_details_56260_1,          /* 56260 Cfg=1:IntCfg=12 */
               sb2_port_details_56260_2,          /* 56260 Cfg=2:IntCfg=13 */
               sb2_port_details_56262_1,          /* 56262 Cfg=1:IntCfg=14 */
               sb2_port_details_56263_1,          /* 56263 Cfg=1:IntCfg=15 */
               sb2_port_details_56263_2,          /* 56263 Cfg=2:IntCfg=16 */
               sb2_port_details_56233_1           /* 56233 Cfg=1:IntCfg=17 */
#ifdef BCM_WARM_BOOT_SUPPORT
               ,sb2_scache_port_details           /* Dummy port details
                                                         reserved for WB*/
#endif
};

/* TDM related functions and structures */
static bcm56260_tdm_info_t bcm56260_tdm_info[]={
            {0, 0, 0, 0},                     /* Dummy */
            /* 56460_1 Cfg=1:IntCfg=1 */
            {SB2_TDM_AA_FREQ, SB2_TDM_AA_SIZE, SB2_TDM_AA_ROW, SB2_TDM_AA_COL},

            /* 56460_2 Cfg=2:IntCfg=2 */
            {SB2_TDM_C2_FREQ, SB2_TDM_C2_SIZE, SB2_TDM_C2_ROW, SB2_TDM_C2_COL},

            /* 56460_3 Cfg=3:IntCfg=3 */
            {SB2_TDM_C_FREQ, SB2_TDM_C_SIZE, SB2_TDM_C_ROW, SB2_TDM_C_COL},

            /* 56460_4 Cfg=4:IntCfg=4 */
            {SB2_TDM_C1_FREQ, SB2_TDM_C1_SIZE, SB2_TDM_C1_ROW, SB2_TDM_C1_COL},

            /* 56460_5 Cfg=5:IntCfg=5 */
            {SB2_TDM_C1_FREQ, SB2_TDM_C1_SIZE, SB2_TDM_C1_ROW, SB2_TDM_C1_COL},

            /* 56460_6 Cfg=6:IntCfg=6 */
            {SB2_TDM_D_FREQ, SB2_TDM_D_SIZE, SB2_TDM_D_ROW, SB2_TDM_D_COL},

            /* 56462_1 Cfg=1:IntCfg=7 */
            {SB2_TDM_B1A_FREQ, SB2_TDM_B1A_SIZE, SB2_TDM_B1A_ROW, SB2_TDM_B1A_COL},
            /* 56462_2 Cfg=2:IntCfg=8 */
            {SB2_TDM_B1A_FREQ, SB2_TDM_B1A_SIZE, SB2_TDM_B1A_ROW, SB2_TDM_B1A_COL},

            /* 56463_1 Cfg=1:IntCfg=9 */
            {SB2_TDM_E_FREQ, SB2_TDM_E_SIZE, SB2_TDM_E_ROW, SB2_TDM_E_COL},
            /* 56463_2 Cfg=2:IntCfg=10 */
            {SB2_TDM_E1_FREQ, SB2_TDM_E1_SIZE, SB2_TDM_E1_ROW, SB2_TDM_E1_COL},
            /* 56463_3 Cfg=3:IntCfg=11 */
            {SB2_TDM_E3_FREQ, SB2_TDM_E3_SIZE, SB2_TDM_E3_ROW, SB2_TDM_E3_COL},

            /* 56260_1 Cfg=1:IntCfg=12 */
            {SB2_TDM_B_FREQ, SB2_TDM_B_SIZE, SB2_TDM_B_ROW, SB2_TDM_B_COL},

            /* 56260_2 Cfg=2:IntCfg=13 */
            {SB2_TDM_B1_FREQ, SB2_TDM_B1_SIZE, SB2_TDM_B1_ROW, SB2_TDM_B1_COL},

            /* 56262_1 Cfg=1:IntCfg=14 */
            {SB2_TDM_EA_FREQ, SB2_TDM_EA_SIZE, SB2_TDM_EA_ROW, SB2_TDM_EA_COL},

            /* 56263_1 Cfg=1:IntCfg=15 */
            {SB2_TDM_A1A_FREQ, SB2_TDM_A1A_SIZE, SB2_TDM_A1A_ROW, SB2_TDM_A1A_COL},
            /* 56263_2 Cfg=1:IntCfg=16 */
            {SB2_TDM_A1_FREQ, SB2_TDM_A1_SIZE, SB2_TDM_A1_ROW, SB2_TDM_A1_COL},

            /* 56233_1 Cfg=1:IntCfg=17 */
            {SB2_TDM_F_FREQ, SB2_TDM_F_SIZE, SB2_TDM_F_ROW, SB2_TDM_F_COL}
#ifdef BCM_WARM_BOOT_SUPPORT
            ,{0, 0, 0, 0},
#endif
};

typedef enum {
    _SOC_COUNTER_TYPE_DROP_ENQ,
    _SOC_COUNTER_TYPE_ACCEPT_ENQ,
    _SOC_COUNTER_TYPE_DROP_ENQ_GREEN,
    _SOC_COUNTER_TYPE_DROP_ENQ_YELLOW,
    _SOC_COUNTER_TYPE_DROP_ENQ_RED,
    _SOC_COUNTER_TYPE_ACCEPT_ENQ_GREEN,
    _SOC_COUNTER_TYPE_ACCEPT_ENQ_YELLOW,
    _SOC_COUNTER_TYPE_ACCEPT_ENQ_RED,
    _SOC_COUNTER_TYPE_ACCEPT_DEQ,
    _SOC_COUNTER_TYPE_MAX
} _soc_saber2_counter_type_t;

typedef struct _soc_saber2_counter_info_s {
    _soc_saber2_counter_type_t type;
    int index;
    int segment;
} _soc_saber2_counter_info_t;

/* Eagle port mode */
int eagle_n4s_mode = 0;
int eagle_n2_mode = 0;
int eagle_4ge_mode = 0;

STATIC int
_soc_saber2_indacc_addr(uint32 phy_reg, int *target_select);
STATIC int
_soc_saber2_indacc_wait(int unit, soc_field_t fwait);


STATIC _soc_saber2_counter_info_t _soc_saber2_counter_info[] = {
    { _SOC_COUNTER_TYPE_DROP_ENQ, 0x0, 0 },
    { _SOC_COUNTER_TYPE_ACCEPT_ENQ, 0x0, 0 },
    /* green pkt drops, index = ACCEPT=1'b0, COLOR=2'b00 */
    { _SOC_COUNTER_TYPE_DROP_ENQ_GREEN, 0x0, 0 },
    /* yellow pkt drops, index = ACCEPT=1'b0, COLOR=2'b11 */
    { _SOC_COUNTER_TYPE_DROP_ENQ_YELLOW, 0x3, 0 },
    /* red pkt drops, index = ACCEPT=1'b0, COLOR=2'b01 */
    { _SOC_COUNTER_TYPE_DROP_ENQ_RED, 0x1, 0 },
    /* green accepted pkts, index = ACCEPT=1'b1, COLOR=2'b00 */
    { _SOC_COUNTER_TYPE_ACCEPT_ENQ_GREEN, 0x4, 0 },
    /* yellow accepted pkts, index = ACCEPT=1'b1, COLOR=2'b11 */
    { _SOC_COUNTER_TYPE_ACCEPT_ENQ_YELLOW, 0x7, 0 },
    /* red accepted pkts, index = ACCEPT=1'b1, COLOR=2'b01 */
    { _SOC_COUNTER_TYPE_ACCEPT_ENQ_RED, 0x5, 0 },
    { _SOC_COUNTER_TYPE_ACCEPT_DEQ, 0, 0 } 
};

int soc_saber2_hw_reset_control_init(int unit) {
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
    /* 'EGR_VLAN_XLATE' initialization need to clear one more entry than 
    *   its max entry number(0x4000). 
    */
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x4001);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    if((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM || SAL_BOOT_XGSSIM)) {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, DONEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    }

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ING_HW_RESET timeout\n")));
            break;
        }
    } while (TRUE);

    if((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM || SAL_BOOT_XGSSIM)) {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, DONEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));
    }

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "EGR_HW_RESET timeout\n")));
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
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, DONEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    return SOC_E_NONE;
}

_soc_sb2_mmu_params_t _soc_sb2_mmu_params={0};
_soc_sb2_mmu_intermediate_results_t _soc_sb2_mmu_intermediate_results={{0}};

extern int         
soc_tsc_xgxs_reset(int unit, soc_port_t port, int reg_idx);
extern int
bcm_kt2_modid_set_all(int unit, int *my_modid_list, int *my_modid_valid_list ,
        int *base_port_ptr_list);

int _soc_saber2_get_cfg_num(int unit, int *new_cfg_num);
int _sb2_config_id[SOC_MAX_NUM_DEVICES] = {-1};
soc_error_t
soc_saber2_port_init_config(int unit, int cfg_num, soc_port_t port);


STATIC int
soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(int unit, uint32 data, uint32 addr) {
    uint32 rval;
    int cnt;

    rval = data;
    SOC_IF_ERROR_RETURN(WRITE_TOP_CI_DDR_PHY_REG_DATAr(unit, rval));
    SOC_IF_ERROR_RETURN(READ_TOP_CI_DDR_PHY_REG_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ACKf, 1);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval,
            PHY_REG_ADDRf, addr);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_REQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CI_DDR_PHY_REG_CTRLr(unit, rval));

    cnt = 3;
    while(cnt > 0) {
        SOC_IF_ERROR_RETURN(READ_TOP_CI_DDR_PHY_REG_CTRLr(unit, &rval));

        /* Check whether PHY_REG_ACKf or PHY_REG_ERR_ACKf is set */
        if (soc_reg_field_get(unit, TOP_CI_DDR_PHY_REG_CTRLr, rval,
                PHY_REG_ACKf) ||
            soc_reg_field_get(unit, TOP_CI_DDR_PHY_REG_CTRLr, rval,
                PHY_REG_ERR_ACKf)) {
                break;
        }
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "Check count %d - 0x%x and Addr 0x%x\n"), cnt,    data, addr));
        cnt--;
        sal_usleep(1000);
    }

    /* If PHY_REG_ERR_ACKf is set, then print the error */
    if(soc_reg_field_get(unit, TOP_CI_DDR_PHY_REG_CTRLr, rval,
            PHY_REG_ERR_ACKf)) {
    LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit, "Received PHY_REG_ERR_ACK for data \
                      0x%x and Addr 0x%x\n"), data, addr));
    }
     
    return SOC_E_NONE;
}         

int
soc_sb2_update_ci_ddr3_phy_reg_ctrl(int unit, uint32 addr) {
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_TOP_CI_DDR_PHY_REG_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ACKf, 1);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval,
            PHY_REG_ADDRf, addr);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval,
            PHY_REG_ERR_ACKf, 1);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval,
            PHY_REG_RD_WR_Nf, 1);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_REQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CI_DDR_PHY_REG_CTRLr(unit, rval));

    return SOC_E_NONE;
}

STATIC int
soc_sb2_ddr3_phy_config(int unit) {
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_TOP_MMU_DDR_PHY_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_MMU_DDR_PHY_CTRLr, &rval, I_PWRONIN_PHYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_DDR_PHY_CTRLr(unit, rval));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN(READ_TOP_MMU_DDR_PHY_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_MMU_DDR_PHY_CTRLr, &rval, I_PWROKIN_PHYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_DDR_PHY_CTRLr(unit, rval));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN(READ_TOP_MMU_DDR_PHY_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_MMU_DDR_PHY_CTRLr, &rval, I_ISO_PHY_DFIf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MMU_DDR_PHY_CTRLr(unit, rval));
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0xC1,        0x1DC));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0x4001530,   0x44));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0));

    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit,              0x1C030B0B, 0x48));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0x60C080B,   0x4C));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0x94C040,    0x50));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0, 0x54));

    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x8));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0x810012,    0x8));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x8));

    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x18));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0x2030bF,    0x18));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x18));

    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0xC));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0xD0,        0x0C));

    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x10));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit,              0x9C000000, 0x10));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x10));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x8));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0x810010,    0x8));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x8));

    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x4));

    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x8));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_data_ctrl(unit, 0x810000,    0x8));
    SOC_IF_ERROR_RETURN(soc_sb2_update_ci_ddr3_phy_reg_ctrl(unit, 0x8));

    return SOC_E_NONE;
}

STATIC int
_soc_saber2_tspll_bspll_config(int unit)
{
    uint32 rval, to_usec;
    unsigned ts_ref_freq;
    unsigned ts_idx;
    /* values for 500MHz TSPLL output-f(vco)=3500, chan=0, f(out)=500*/
    static const soc_pll_param_t ts_pll[] = {
    /*     Fref,  Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,     625,         0,     2,    8,  0,  2, 3,        1},
        {20000000,     175,         0,     1,    7,  0,  2, 3,        1},
        {25000000,     140,         0,     1,    7,  0,  2, 3,        1},
        {32000000,     125,         0,     1,    8,  0,  2, 3,        1},
        {50000000,      70,         0,     1,    7,  0,  2, 3,        1},
        {       0,      70,         0,     1,    7,  0,  2, 3,        1}
    };
    unsigned bs_ref_freq;
    unsigned bs_idx;
    uint32 bs_ndiv_high, bs_ndiv_low;
    /* values for 20MHz BS0PLL output-f(vco) = 3100,channel = 1,f(out) = 20*/
    /* 25MHZ have frequency doubler on */
    static const soc_pll_param_t bs_pll[] = {
    /*     Fref,  Ndiv_int,  Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,      242,   3145728,     1,  155,  0,  0, 0,        0},
        {20000000,      155,         0,     1,  155,  0,  0, 0,        0},
        {25000000,      62,          0,     1,  155,  0,  0, 0,        0},
        {32000000,      96,   14680064,     1,  155,  0,  0, 0,        0},
        {50000000,      62,          0,     1,  155,  0,  0, 0,        0},
        {       0,      62,          0,     1,  155,  0,  0, 0,        0}
    };
    /* values for 20MHz BS1PLL output-f(vco) = 3000,channel = 1,f(out) = 20*/ 
    /* BS1 PLL always driven internaly by from BS0 PLL */
    static const soc_pll_param_t bs1_pll[] = {
    /*     Fref,  Ndiv_int,  Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
        {12800000,      234,   6291456,     1,  150,  0,  0, 0,        0},
        {20000000,      150,         0,     1,  150,  0,  0, 0,        0},
        {25000000,      60,          0,     1,  150,  0,  0, 0,        0},
        {32000000,      93,   12582912,     1,  150,  0,  0, 0,        0},
        {50000000,      60,          0,     1,  150,  0,  0, 0,        0},
        {       0,      60,          0,     1,  150,  0,  0, 0,        0}
    };

#if defined(BCM_HITLESS_RESET_SUPPORT)
    int initialize_broadsync;
#endif /* BCM_HITLESS_RESET_SUPPORT */

    /* TSPLL configuration: 500MHz TS_CLK from 20/25/32/50MHz refclk */
    /* CMICd divides by 2 on input, so this is a 250MHz clock to TS logic */

    /* 50MHz is default ->internal reference */
    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);


    /* Configure TS PLL */
    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }
    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid value for PTP_TS_PLL_REF (%u).  No default PLL params. Using internal clock \n"),
                              ts_ref_freq));
        /* resetting back to internal clock */
        ts_idx = ts_idx - 1;
    }

    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, TS_PLL_CLK_IN_SELf,
                      (ts_ref_freq != 0));
    WRITE_TOP_MISC_CONTROL_1r(unit, rval);


    if (SAL_BOOT_QUICKTURN) {
     LOG_VERBOSE(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
              "%s:%d Skipping PLL Configuration \n"), FUNCTION_NAME(),__LINE__));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
         (BSL_META_U(unit,
         "%s:%d Continuing PLL Configuration \n"), FUNCTION_NAME(),__LINE__));

        if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r)) {
            SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit,&rval));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval, KAf,
                   soc_property_get(unit, spn_PTP_TS_KA, ts_pll[ts_idx].ka));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval, KIf,
                   soc_property_get(unit, spn_PTP_TS_KI, ts_pll[ts_idx].ki));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval, KPf,
                   soc_property_get(unit, spn_PTP_TS_KP, ts_pll[ts_idx].kp));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r, &rval,
                    NDIV_FRACf, ts_pll[ts_idx].ndiv_frac);
            WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r(unit, rval);
        }

        if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r)) {
            SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit,&rval));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r, &rval, PDIVf,
                   soc_property_get(unit, spn_PTP_TS_PLL_PDIV, ts_pll[ts_idx].pdiv));
            WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit, rval);
        }

        if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r)) {
            SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit,&rval));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r, &rval, NDIV_INTf,
                   soc_property_get(unit, spn_PTP_TS_PLL_N,
                                    ts_pll[ts_idx].ndiv_int));
            WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r(unit, rval);
        }

        if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r)) {
            SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit,&rval));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r, &rval, CH0_MDIVf,
                    soc_property_get(unit, spn_PTP_TS_PLL_MNDIV,
                                     ts_pll[ts_idx].mdiv));
            WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit, rval);
        }

        if (SOC_REG_IS_VALID(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r)) {
            SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit,&rval));
            soc_reg_field_set(unit, TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r, &rval, CH1_MDIVf,
                    soc_property_get(unit, spn_PTP_TS_PLL_MNDIV,
                                     ts_pll[ts_idx].mdiv));
            WRITE_TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r(unit, rval);
        }

    }

    /* 250Mhz TS PLL implies 4ns resolution */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

#if defined(BCM_HITLESS_RESET_SUPPORT)

    SOC_IF_ERROR_RETURN(READ_CMIC_BS0_CONFIGr(unit,&rval));
    initialize_broadsync = !(soc_reg_field_get(unit, CMIC_BS0_CONFIGr, rval,
                           BS_CLK_OUTPUT_ENABLEf));

    LOG_WARN(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "BSPLL regs:  %s\n"), initialize_broadsync ? "Default" : "Configured"));

    if (!initialize_broadsync) {
        /* JIRA CMICD-110
         * On A0/B0, BroadSync Bitclock/Heartbeat divisors/enable registers get reset,
         * though the outputs themselves do not if the registers were written to an even
         * number of times.  Workaround: when the registers are changed, the register
         * should be written twice, and the same value stored in SRAM.  This code will
         * restore the register from SRAM.
         */

        /* BS0 */
        uint32 reg_cache_base = soc_cmic_bs_reg_cache(unit, 0);

        uint32 config = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, config));
        uint32 clk_ctrl = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, clk_ctrl));
        uint32 heartbeat_ctrl = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl));
        uint32 heartbeat_down = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration));
        uint32 heartbeat_up = soc_uc_mem_read(unit,
             reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration));

        if (config == 0 && clk_ctrl == 0 && heartbeat_ctrl == 0 && heartbeat_down == 0 && heartbeat_up == 0) {
            /* PLL was initialized, but there is no saved state.  Reset BroadSync. */
            initialize_broadsync = 1;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BSPLL Configured, but no BS state to restore\n")));
        } else {
            /* Double-writes, per CMICD-110 WAR */
            WRITE_CMIC_BS0_CONFIGr(unit, config);
            WRITE_CMIC_BS0_CONFIGr(unit, config);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);
            WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);

            LOG_WARN(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Restored BS0 regs:\n"
                                    "  config: %08x  clk_ctrl: %08x hb_ctrl: %08x\n"
                                    "  hb_up: %08x hb_down: %08x\n"),
                         config, clk_ctrl, heartbeat_ctrl,
                         heartbeat_down, heartbeat_up));

            /* same, but for BS1 */
            reg_cache_base = soc_cmic_bs_reg_cache(unit, 1);

            config = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, config));
            clk_ctrl = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, clk_ctrl));
            heartbeat_ctrl = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl));
            heartbeat_down = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration));
            heartbeat_up = soc_uc_mem_read(unit,
                 reg_cache_base + offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration));

            WRITE_CMIC_BS1_CONFIGr(unit, config);
            WRITE_CMIC_BS1_CONFIGr(unit, config);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, clk_ctrl);
            WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, heartbeat_ctrl);
            WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, heartbeat_down);
            WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);
            WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, heartbeat_up);

            LOG_WARN(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Restored BS1 regs:\n"
                                    "  config: %08x  clk_ctrl: %08x hb_ctrl: %08x\n"
                                    "  hb_up: %08x hb_down: %08x\n"),
                         config, clk_ctrl, heartbeat_ctrl,
                         heartbeat_down, heartbeat_up));
        }
    }

    if (initialize_broadsync)
#endif  /* BCM_HITLESS_RESET_SUPPORT */
    {

        /* Broadsync PLL initialization */
        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit,&rval));

        /* as a signal to upper-level code that the BroadSync is newly initialized
         * disable BroadSync0/1 bitclock output.  Checked in time.c / 1588 firmware
         */
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_BS0_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
        WRITE_CMIC_BS0_CONFIGr(unit, rval);

        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_BS1_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
        WRITE_CMIC_BS1_CONFIGr(unit, rval);

       /* Both BSPLLs configured the same, for 20MHz output by default */
        /* 50MHz is default.  0->internal reference */
        bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 0);
        /* Set BS_PLL_CLK_IN_SEL based on reference frequency. If it is 0, use the
         * internal reference
         */
        for (bs_idx = 0; bs_idx < sizeof(bs_pll)/sizeof(bs_pll[0]); ++bs_idx) {
            if (bs_pll[bs_idx].ref_freq == bs_ref_freq) {
                break;
            }
        }

        if (bs_idx == sizeof(bs_pll)/sizeof(bs_pll[0])) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                        "Invalid value for PTP_BS_REF (%u).  No default PLL params. Resetting to internal clock\n"),
                        bs_ref_freq));
            bs_idx = bs_idx - 1;
        }

        /* bs pll - set reference clock */

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, BS0_PLL_CLK_IN_SELf,
                          (bs_ref_freq != 0));
        /* BS1 PLL to get ref clock internally routed from BS0 PLL */
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, BS1_PLL_CLK_IN_SELf,0);
        WRITE_TOP_MISC_CONTROL_1r(unit, rval);

        /* bs pll - ndiv, mdiv, channel info */
        bs_ndiv_high = ((soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs_pll[bs_idx].ndiv_int) << 6) |
            ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) >> 18) & 0x3f));

        bs_ndiv_low = (soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac) << 14);

        /* bs pll - 0 configuration */
        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r, &rval,
                          BROAD_SYNC0_LCPLL_FBDIV_1f, bs_ndiv_high);
        WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r, &rval,
                          BROAD_SYNC0_LCPLL_FBDIV_0f, bs_ndiv_low);
        WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit, rval);

        /* for BS , use channel 1 */
        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r, &rval,
                          CH1_MDIVf, soc_property_get(unit, spn_PTP_BS_MNDIV,
                          bs_pll[bs_idx].mdiv));
        WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r(unit, rval);

        /* for 25MHz, set bit 13 to enable frequency doubler */
        if (bs_ref_freq ==  25000000) {
            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r(unit,&rval));
            rval |= (1<<13);
            WRITE_TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r(unit, rval);
        }


     
        /* bs pll - 1 configuration */
        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit,&rval));
        /*BS1 new config*/
        bs_ndiv_high = ((soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs1_pll[bs_idx].ndiv_int) << 6) |
            ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs1_pll[bs_idx].ndiv_frac) >> 18) & 0x3f));

        bs_ndiv_low = (soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs1_pll[bs_idx].ndiv_frac) << 14);

        soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r, &rval,
                          BROAD_SYNC1_LCPLL_FBDIV_1f, bs_ndiv_high);
        WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r, &rval,
                          BROAD_SYNC1_LCPLL_FBDIV_0f, bs_ndiv_low);
        WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r, &rval,
                          CH1_MDIVf, soc_property_get(unit, spn_PTP_BS_MNDIV,
                          bs1_pll[bs_idx].mdiv));
        WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r(unit, rval);

        /* for 25MHz, set bit 13 to enable frequency doubler */
        if (bs_ref_freq ==  25000000) {
            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r(unit,&rval));
            rval |= (1<<13);
            WRITE_TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r(unit, rval);
        }


        /* JIRA CMICD-110:  See above logic to restore BroadSync Bitclock/Heartbeat. */
        /* Store default values in the SRAM, so they will be restored correctly */
        /* even if SRAM is not used */
#if defined(BCM_HITLESS_RESET_SUPPORT)
        {
            /* store reset values in SRAM for BS0 */
            uint32 reg_cache_base = soc_cmic_bs_reg_cache(unit, 0);
            READ_CMIC_BS0_CONFIGr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, config), rval);
            READ_CMIC_BS0_CLK_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, clk_ctrl), rval);
            READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl), rval);
            READ_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration), rval);
            READ_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration), rval);

            /* store reset values in SRAM for BS1 */
            reg_cache_base = soc_cmic_bs_reg_cache(unit, 1);
            READ_CMIC_BS1_CONFIGr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, config), rval);
            READ_CMIC_BS1_CLK_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, clk_ctrl), rval);
            READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_ctrl), rval);
            READ_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_down_duration), rval);
            READ_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, &rval);
            soc_uc_mem_write(unit, reg_cache_base +
                             offsetof(mos_msg_bs_reg_cache_t, heartbeat_up_duration), rval);
        }
    } else {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "Leaving BroadSync in holdover\n")));
#endif  /* BCM_HITLESS_RESET_SUPPORT */
    }

    /* 16mA driving stregth */
    SOC_IF_ERROR_RETURN(READ_ICFG_IPROC_IOPAD_CTRL_21r(unit, &rval));
    soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_21r, &rval,IPROC_BS0_CLK_DRIVEf,0x7);
    soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_21r, &rval,IPROC_BS0_HB_DRIVEf,0x7);
    soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_21r, &rval,IPROC_BS0_TC_DRIVEf,0x7);
    WRITE_ICFG_IPROC_IOPAD_CTRL_21r(unit, rval);
    SOC_IF_ERROR_RETURN(READ_ICFG_IPROC_IOPAD_CTRL_20r(unit, &rval));
    soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_20r, &rval,IPROC_BS1_CLK_DRIVEf,0x7);
    soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_20r, &rval,IPROC_BS1_HB_DRIVEf,0x7);
    soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_20r, &rval,IPROC_BS1_TC_DRIVEf,0x7);
    WRITE_ICFG_IPROC_IOPAD_CTRL_20r(unit, rval);

    /* Wait for pll lock */
    to_usec = 10 * MILLISECOND_USEC;
    sal_usleep(to_usec);
    if (!SAL_BOOT_SIMULATION) {
        SOC_IF_ERROR_RETURN(READ_TOP_TIME_SYNC_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_TIME_SYNC_PLL_STATUSr, rval,
                               TOP_TIME_SYNC_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                         "TimeSync PLL not locked on unit %d "
                         "status = 0x%08x\n"), unit, rval));
        }

        if (SOC_REG_IS_VALID(unit, TOP_BROAD_SYNC0_PLL_STATUSr)) {
            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_PLL_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_BROAD_SYNC0_PLL_STATUSr, rval,
                        TOP_BROAD_SYNC0_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                          "BroadSync0 PLL not locked on unit %d "
                          "status = 0x%08x\n"), unit, rval));
            }
        }

        if (SOC_REG_IS_VALID(unit, TOP_BROAD_SYNC1_PLL_STATUSr)) {
            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_PLL_STATUSr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_BROAD_SYNC1_PLL_STATUSr, rval,
                        TOP_BROAD_SYNC1_PLL_LOCKf)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                          "BroadSync1 PLL not locked on unit %d "
                          "status = 0x%08x\n"), unit, rval));
            }
        }
    }

    return SOC_E_NONE;
}

int _soc_saber2_hw_reset_control_init(int unit)
{
    uint32 rval;
    int try_count = 0;
    int port = 1;
    int id;
    soc_mem_t mem[32] = {
        RXLP_INTERNAL_STREAM_MAP_PORT_0m,
        RXLP_INTERNAL_STREAM_MAP_PORT_1m,
        RXLP_INTERNAL_STREAM_MAP_PORT_2m,
        RXLP_INTERNAL_STREAM_MAP_PORT_3m,
        TXLP_PORT_STREAM_BITMAP_TABLEm,
        TXLP_INT2EXT_STREAM_MAP_TABLEm,
        DEVICE_STREAM_ID_TO_PP_PORT_MAPm,
        PP_PORT_TO_PHYSICAL_PORT_MAPm,
        TXLP_DEBUG_COUNTER0m,
        TXLP_DEBUG_COUNTER1m,
        TXLP_DEBUG_COUNTER2m,
        TXLP_DEBUG_COUNTER3m,
        TXLP_DEBUG_COUNTER4m,
        TXLP_DEBUG_COUNTER5m,
        TXLP_DEBUG_COUNTER6m,
        TXLP_DEBUG_COUNTER7m,
        TXLP_DEBUG_COUNTER8m,
        TXLP_DEBUG_COUNTER9m,
        TXLP_DEBUG_COUNTER10m,
        TXLP_DEBUG_COUNTER11m,
        RXLP_DEBUG_COUNTER0m,
        RXLP_DEBUG_COUNTER1m,
        RXLP_DEBUG_COUNTER2m,
        RXLP_DEBUG_COUNTER3m,
        RXLP_DEBUG_COUNTER4m,
        RXLP_DEBUG_COUNTER5m,
        RXLP_DEBUG_COUNTER6m,
        RXLP_DEBUG_COUNTER7m,
        RXLP_DEBUG_COUNTER8m,
        RXLP_DEBUG_COUNTER9m,
        RXLP_DEBUG_COUNTER10m,
        RXLP_DEBUG_COUNTER11m };

    SOC_IF_ERROR_RETURN(READ_RXLP_HW_RESET_CONTROLr(unit, port, &rval));
    soc_reg_field_set(unit, RXLP_HW_RESET_CONTROLr, &rval, VALIDf, 1);
    soc_reg_field_set(unit, RXLP_HW_RESET_CONTROLr, &rval, START_ADDRESSf, 0);
    soc_reg_field_set(unit, RXLP_HW_RESET_CONTROLr, &rval, COUNTf, 0x7D0);
    SOC_IF_ERROR_RETURN(WRITE_RXLP_HW_RESET_CONTROLr(unit, port, rval));

    if((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM || SAL_BOOT_XGSSIM)) {
        SOC_IF_ERROR_RETURN(READ_RXLP_HW_RESET_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, RXLP_HW_RESET_CONTROLr, &rval, DONEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_RXLP_HW_RESET_CONTROLr(unit, port, rval));
    }
    
    /* Now wait for HW to set DONEf */
    do {
        SOC_IF_ERROR_RETURN(READ_RXLP_HW_RESET_CONTROLr(unit, port, &rval));
        if (soc_reg_field_get(unit, RXLP_HW_RESET_CONTROLr, rval, DONEf) == 1) {
            break;
        }
        sal_usleep(1000);
        try_count++;
    } while(try_count < 3);

    if (try_count == 3) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "RXLP HW RESET Failed\n")));
        return SOC_E_TIMEOUT;
    }

    SOC_IF_ERROR_RETURN(READ_TXLP_HW_RESET_CONTROL_1r(unit, port, &rval));
    soc_reg_field_set(unit, TXLP_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, TXLP_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x84);
    soc_reg_field_set(unit, TXLP_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_TXLP_HW_RESET_CONTROL_1r(unit, port, rval));

    if((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM || SAL_BOOT_XGSSIM)) {
        SOC_IF_ERROR_RETURN(READ_TXLP_HW_RESET_CONTROL_1r(unit, port, &rval));
        soc_reg_field_set(unit, TXLP_HW_RESET_CONTROL_1r, &rval, DONEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TXLP_HW_RESET_CONTROL_1r(unit, port, rval));
    }

    /* Now wait for HW to set DONEf */
    try_count = 0;
    do {
        SOC_IF_ERROR_RETURN(READ_TXLP_HW_RESET_CONTROL_1r(unit, port, &rval));
        if (soc_reg_field_get(unit, TXLP_HW_RESET_CONTROL_1r, rval, DONEf) == 1) {
            break;
        }
        sal_usleep(1000);
        try_count++;
    } while(try_count < 3);

    if (try_count == 3) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "TXLP HW RESET Failed\n")));
        return SOC_E_TIMEOUT;
    }

    /* Clear all the debug counters */
    for (id = 0; id < 32; id++) {
        SOC_IF_ERROR_RETURN(soc_mem_clear(unit, mem[id], COPYNO_ALL, TRUE));
    }

    return SOC_E_NONE;
}
 int
_saber2_ledup_init(int unit, uint16 dev_id)
{
    uint32 rval = 0;
    int ix;
    int pval1, pval2, pval3, pval4;
    int is_5626x = 0, ii = 0, pb = 7;
    int freq, cycles;
    int refresh_rate=30, refresh_period;
    struct led_remap {
       uint32 reg_addr;
       uint32 port0;
       uint32 port1;
       uint32 port2;
       uint32 port3;
    } led0_remap[] = {
        {CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f,REMAP_PORT_1f,REMAP_PORT_2f,REMAP_PORT_3f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f,REMAP_PORT_5f,REMAP_PORT_6f,REMAP_PORT_7f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f,REMAP_PORT_9f,REMAP_PORT_10f,REMAP_PORT_11f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f,REMAP_PORT_13f,REMAP_PORT_14f,REMAP_PORT_15f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f,REMAP_PORT_17f,REMAP_PORT_18f,REMAP_PORT_19f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f,REMAP_PORT_21f,REMAP_PORT_22f,REMAP_PORT_23f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f,REMAP_PORT_25f,REMAP_PORT_26f,REMAP_PORT_27f} 
    };

    switch(dev_id) {
        case BCM56260_DEVICE_ID:
        case BCM56261_DEVICE_ID:
        case BCM56262_DEVICE_ID:
        case BCM56263_DEVICE_ID:
        case BCM56265_DEVICE_ID:
        case BCM56266_DEVICE_ID:
        case BCM56267_DEVICE_ID:
        case BCM56268_DEVICE_ID:
        case BCM56233_DEVICE_ID:
        /* Metrolite */
        case BCM56270_DEVICE_ID:
        case BCM56271_DEVICE_ID:
        case BCM56272_DEVICE_ID:
            is_5626x = 1;
            pb = 4;
            break;

        default:
            is_5626x = 0;
            pb = 7;
            break;
    };



    /* initialize the led remap register settings.
     */

    for (ix = 0; ix < sizeof(led0_remap)/sizeof(led0_remap[0]); ix++) {
      
        if (is_5626x == 1)
        {
          if ( ix >= 2 && ix <= 4) 
              continue;
        } 
        pval4 = ((pb-ii) * 4);
        pval3 = pval4 - 1;
        pval2 = pval4 - 2;
        pval1 = pval4 - 3;

        rval = 0;
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval,
                         led0_remap[ix].port0, pval1);
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval,
                         led0_remap[ix].port1, pval2);
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval,
                         led0_remap[ix].port2, pval3);
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval,
                         led0_remap[ix].port3, pval4);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN
            (soc_pci_write(unit,
                soc_reg_addr(unit, led0_remap[ix].reg_addr, REG_PORT_ANY, 0),
                rval));
        ii++;
    }

    /* initialize the UP0 data ram */
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

int _soc_saber2_misc_init(int unit)
{
    soc_info_t *si;
    uint32 rval, rval1, rval2;
    uint32 entry[SOC_MAX_MEM_WORDS], ing_entry[SOC_MAX_MEM_WORDS];
    soc_pbmp_t pbmp;
    int port;
    uint64 reg64;
    uint16 dev_id;
    uint8 rev_id;
    soc_field_t fields[3];
    uint32 values[3];
    int rv;
    int my_modid_list[4] = { 0, 0, 0, 0 };
    int my_modid_valid[4] = { 1, 0, 0, 0 };
    int my_modid_port_base_ptr[4] = { 0, 0, 0, 0 };
    int parity_enable;
    int cfg_num;
    int start_pp_port, end_pp_port, pp_port;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32 ddr3_clock_mhz = 0;
    uint32 num_rows = 32768;
    uint32 ddr3_mem_grade = 0;
    uint32 ext_pbmp_count=0;
    pbmp_t ext_pbmp;
#endif
    soc_cm_get_id(unit, &dev_id, &rev_id);

    si = &SOC_INFO(unit);

    /* saber2 related hw reset control */
    if(!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_soc_saber2_hw_reset_control_init(unit));
    }

    /* DDR Phy programming */
    if (SAL_BOOT_QUICKTURN) {
        soc_sb2_ddr3_phy_config(unit);
        sal_usleep(1000);
    }
    /* core clock info in MHz updated from PRD 
     * Do not modify these values until unless stated in PRD*/
    switch(dev_id) {
      case BCM56460_DEVICE_ID:
      case BCM56461_DEVICE_ID:
      case BCM56465_DEVICE_ID:
      case BCM56466_DEVICE_ID:
        si->frequency = 130;
        break;
      case BCM56260_DEVICE_ID:
      case BCM56261_DEVICE_ID:
      case BCM56265_DEVICE_ID:
      case BCM56266_DEVICE_ID:
      case BCM56462_DEVICE_ID:
      case BCM56467_DEVICE_ID:
        si->frequency = 118;
        break;

      case BCM56262_DEVICE_ID:
      case BCM56267_DEVICE_ID:
      case BCM56463_DEVICE_ID:
      case BCM56468_DEVICE_ID:
      case BCM56233_DEVICE_ID:
        si->frequency = 75;
        break;
      case BCM56263_DEVICE_ID:
      case BCM56268_DEVICE_ID:
        si->frequency = 37;
        break;
    }
    if(!(SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM || SAL_BOOT_XGSSIM)) {
       if (soc_feature(unit, soc_feature_ddr3)) {
#ifdef BCM_WARM_BOOT_SUPPORT
          if (SOC_WARM_BOOT(unit)) {
            ddr3_clock_mhz = 800;

            SOC_DDR3_NUM_COLUMNS(unit) = soc_property_get(unit, spn_EXT_RAM_COLUMNS,
                                                      1024);
            SOC_DDR3_NUM_BANKS(unit) = soc_property_get(unit,spn_EXT_RAM_BANKS, 8);
            SOC_DDR3_NUM_MEMORIES(unit) = soc_property_get(unit,spn_EXT_RAM_PRESENT,
                                                       0);
            SOC_DDR3_NUM_ROWS(unit) = soc_property_get(unit,spn_EXT_RAM_ROWS,
                                                       num_rows);
            SOC_DDR3_CLOCK_MHZ(unit) = soc_property_get(unit, spn_DDR3_CLOCK_MHZ,
                                                       ddr3_clock_mhz);
            SOC_DDR3_MEM_GRADE(unit) = soc_property_get(unit, spn_DDR3_MEM_GRADE,
                                                       ddr3_mem_grade);

            ext_pbmp = soc_property_get_pbmp(unit, spn_PBMP_EXT_MEM, 0);
            SOC_PBMP_COUNT(ext_pbmp, ext_pbmp_count);
        }
#endif
        if(!SOC_WARM_BOOT(unit) && SOC_DDR3_NUM_MEMORIES(unit)) {
            SOC_IF_ERROR_RETURN(soc_sb2_and28_dram_init_config(unit));
        }
      }

        parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
        soc_saber2_ser_init(unit, parity_enable ? TRUE : FALSE);
       /* initialize LED UP0 */
        SOC_IF_ERROR_RETURN(_saber2_ledup_init(unit, dev_id));
    }

    /* Some registers are implemented in memory, need to clear them in order
     * to have correct parity value */
    PBMP_PP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, 0));
        SOC_IF_ERROR_RETURN(WRITE_EGR_IPMC_CFG2r(unit, port, 0));
    }
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
         SOC_INFO(unit).linkphy_enabled) ||
         (soc_feature(unit, soc_feature_subtag_coe) &&
          SOC_INFO(unit).subtag_enabled)) {

         my_modid_list[0] = 0;
         my_modid_valid[0] = 1;
         my_modid_port_base_ptr[0] = 0;

         /* If not running in the stacking mode, setup the second module
            as the default one to host all pp_ports, else, the user will
            create the necessary modules with separate APIs */
         if(!SOC_INFO(unit).coe_stacking_mode) {
             my_modid_list[1] = 1;
             my_modid_valid[1] = 1;
             my_modid_port_base_ptr[1] = SOC_SB2_MIN_SUBPORT_INDEX;
         }
    }

    SOC_IF_ERROR_RETURN(
        bcm_kt2_modid_set_all(unit, my_modid_list, my_modid_valid,
                                my_modid_port_base_ptr));

    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r( unit,&rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, DDR3_PHY0_IDDQ_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_1r(unit, rval));
                                
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

    SOC_PBMP_CLEAR(pbmp);
    for (port = 0; port < 30; port++) {
        if (IS_LB_PORT(unit, port) ||
            (si->port_group[port] >= 2 && si->port_group[port] <= 3)) {
            SOC_PBMP_PORT_ADD(pbmp, port);
        }
    }
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

    SOC_IF_ERROR_RETURN(READ_DEQ_EFIFO_CFG_COMPLETEr(unit, &rval));
    soc_reg_field_set(unit, DEQ_EFIFO_CFG_COMPLETEr, &rval, \
            EGRESS_FIFO_CONFIGURATION_COMPLETEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFG_COMPLETEr(unit, rval));

    /* Configure MMU for the ports configured for ext mem */
    for (port = 0; port < si->lb_port; port++) {
        if (IS_EXT_MEM_PORT(unit, port)) {
            
            SOC_IF_ERROR_RETURN(READ_DEST_PORT_CFG_0r(unit, &rval));
            rval |= (1 << port);
            SOC_IF_ERROR_RETURN(WRITE_DEST_PORT_CFG_0r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_MMU_ENQ_DEST_PPP_CFG_0r(unit, &rval));
            rval |= (1 << port);
            SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_DEST_PPP_CFG_0r(unit, rval));

            if (si->port_num_subport[port] > 0) {
                start_pp_port = si->port_subport_base[port];
                end_pp_port = si->port_subport_base[port] +
                    si->port_num_subport[port];
                SOC_IF_ERROR_RETURN(READ_MMU_ENQ_DEST_PPP_CFG_0r(unit, &rval));
                SOC_IF_ERROR_RETURN(READ_MMU_ENQ_DEST_PPP_CFG_1r(unit, &rval1));
                SOC_IF_ERROR_RETURN(READ_MMU_ENQ_DEST_PPP_CFG_2r(unit, &rval2));
                for (pp_port = start_pp_port; pp_port < end_pp_port; pp_port++) {
                    if (( pp_port <= 31 )){
                        rval |= (1 << pp_port);
                    } else if (( pp_port > 31 ) && (pp_port <= 63) ){
                        rval1 |= (1 << (pp_port - 32));
                    } else if (( pp_port > 63 ) && (pp_port <= 95) ){
                        rval2 |= (1 << (pp_port - 64));
                    }
                }

                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_DEST_PPP_CFG_0r(unit, rval));
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_DEST_PPP_CFG_1r(unit, rval1));
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_DEST_PPP_CFG_2r(unit, rval2));
            }
        }
    }

    sal_memset(entry, 0, sizeof(epc_link_bmap_entry_t));
    SOC_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0,
                                             &entry));
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

    /* set to allow Mirror bit in Module Header */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_CONFIG_1r, REG_PORT_ANY,
                                RING_MODEf, 1));

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

    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "unit %d : MSPI Init Failed\n"), unit));
    }
    SOC_IF_ERROR_RETURN(soc_mem_clear(
                        unit, MMU_RQE_QUEUE_OP_NODE_MAPm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN(soc_mem_clear(
                        unit, MMU_WRED_QUEUE_OP_NODE_MAPm, COPYNO_ALL, TRUE));
  
    SOC_IF_ERROR_RETURN(_soc_saber2_get_cfg_num(unit, &cfg_num));
    if (cfg_num >= (sizeof(bcm56260_tdm)/sizeof(bcm56260_tdm[0]))) {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Wrong cfg_num:%d exceeding max cfg_num: %d\n"),
                   cfg_num,
                   (int)(sizeof(bcm56260_tdm)/sizeof(bcm56260_tdm[0]))));
        return SOC_E_FAIL;
    }
    _sb2_config_id[unit] = cfg_num;
 
    PBMP_ALL_ITER(unit, port) {
        if ((port == 0 ) || (port >= 29)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_saber2_port_init_config(unit, cfg_num, port));
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

#ifdef INCLUDE_AVS
    soc_saber2_avs_init(unit);
#endif /* INCLUDE_AVS  */

    if (!SOC_WARM_BOOT(unit)) {
    /* TS-PLL and BS-PLL needs to be initialized only during cold-boot */
    /* initializing ts pll and bs pll */
    _soc_saber2_tspll_bspll_config(unit);
    }

    /* Initializing subport configurations */
    si->subport_group_max = SOC_SB2_SUBPORT_GROUP_MAX;
    si->pp_port_index_max = SOC_SB2_SUBPORT_PP_PORT_INDEX_MAX;
    si->pp_port_index_min = SOC_SB2_SUBPORT_PP_PORT_INDEX_MIN;
    si->subport_port_max = SOC_SB2_MAX_SUBPORTS;
    si->lp_tx_databuf_start_addr_max = SOC_SB2_LINKPHY_TX_DATA_BUF_START_ADDR_MAX;
    si->lp_tx_databuf_end_addr_min = SOC_SB2_LINKPHY_TX_DATA_BUF_END_ADDR_MIN;
    si->lp_tx_databuf_end_addr_max = SOC_SB2_LINKPHY_TX_DATA_BUF_END_ADDR_MAX;
    si->lp_tx_stream_start_addr_offset = SOC_SB2_LINKPHY_TX_STREAM_START_ADDR_OFFSET;
    si->lp_block_max = SOC_SB2_LINKPHY_BLOCK_MAX;
    si->lp_ports_max = SOC_SB2_MAX_LINKPHY_PORTS;
    si->lp_streams_per_slice_max = SOC_SB2_MAX_STREAMS_PER_SLICE;
    si->lp_streams_per_port_max = SOC_SB2_MAX_LINKPHY_STREAMS_PER_PORT;
    si->lp_streams_per_subport = SOC_SB2_MAX_STREAMS_PER_SUBPORT;

    return SOC_E_NONE;
}

tdm_cycles_info_t sb2_current_tdm_cycles_info[KT2_MAX_SPEEDS];

#define SB2_MAX_TDM_FREQUENCY 16
tdm_cycles_info_t sb2_tdm_cycles_info[SB2_MAX_TDM_FREQUENCY][KT2_MAX_SPEEDS]={

                  /*TDM_A : 0:130MHz: 284 (41 * 7) Cycles Used */
                  {{78,71,4},{39/*60*/,35,8},{30,28,10},{7,6,41}, /*CMIC WRONG*/
                   {6,5,48},{0,0,0},{0,0,0}},

                  /*TDM_AA: 1:130MHz: 140 (20 * 7) Cycles Used */
                  {{71,70,2},{36,35,4},{29,28,5},{7,7,20},
                   {6,6,24},{0,0,0},{0,0,0}},

                  /*TDM_A1: 2:24MHz: 13 (4 * 4) Cycles Used */
                  {{13,13,1},{7,6,2},{5,2,3},{1,1,13}, 
                   {1,1,13},{0,0,0},{0,0,0}},

                  /*TDM_A1A: 3:37MHz: 84 (21 * 4) Cycles Used */
                  {{22,21,4},{11,10,8},{10,7,9},{2,2,42}, 
                   {1,1,84},{0,0,0},{0,0,0}},

                  /*TDM_B: 4:118MHz: 133 (23 * 6) Cycles Used */
                  {{70,67,2},{36/*53*/,34,4},{28,26,5},{7,7,19}, /*CMIC WRONG*/
                   {6,6,23},{0,0,0},{0,0,0}},

                  /*TDM_B1: 5:118MHz: 120 (20 * 6) Cycles Used */
                  {{70,60,2},{35,30,4},{28,24,5},{7,7,18}, 
                   {5,5,24},{0,0,0},{0,0,0}},

                  /*TDM_B1A: 6:118MHz: 121 (21 * 6) Cycles Used */
                  {{70,60,2},{35,30,4},{28,24,5},{7,7,18}, 
                   {5,5,25},{0,0,0},{0,0,0}},

                  /*TDM_C,C1,C2: 7:130MHz: 141 (21 * 6) Cycles Used */
                  {{77,71,2},{38,35,4},{30,28,5},{7,7,20 /*21*/},
                   {6,6,24},{0,0,0},{0,0,0}},

                  /*TDM_D 8:130MHz: 168 (28 * 6) Cycles Used */
                  {{77,56,3},{38,34,5},{30,28,6},{7,7,24}, 
                   {6,6,28},{0,0,0},{0,0,0}},

                  /*TDM_E,E1,E3 9:75MHz: 81 (21 * 4) Cycles Used */
                  {{44,40,2},{22,20,4},{17,16,5},{4,4,20}, 
                   {3,3,27},{0,0,0},{0,0,0}},

                  /*TDM_EA 10:75MHz: 16 (4 * 4) Cycles Used */
                  {{16,16,1},{16,16,1},{16,16,1},{4,4,4}, 
                   {3,3,6},{0,0,0},{0,0,0}},

                  /*TDM_F 11:75MHz: 20 (5 * 4) Cycles Used */
                  {{20,20,1},{12,12,2},{12,12,2},{12,12,2},
                   {3,3,6},{0,0,0},{0,0,0}},
                  };


soc_error_t sb2_tdm_verify(int unit,
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
        if (total_tdm_slots == 84) {
            /*TDM_A1A: 3:37MHz: 84 (21 * 4) Cycles Used */
            sal_strncpy(base_tdm_str,
                    "TDM_A1A: 3:37MHz: 84 (21 * 4) Cycles Used ",
                    79);
            tdm_freq_index=3;
            expected_tdm_cycles_min=84;
            expected_tdm_cycles_max=84;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                      "Unsupported tdm slots:%d\n"),
                      total_tdm_slots));
            return SOC_E_FAIL;
        }
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
        } else if (total_tdm_slots == 20) {
            /*TDM_F 11:75MHz: 20 (5 * 4) Cycles Used */
            sal_strncpy(base_tdm_str,
                        "TDM_F 11:75MHz: 20 (5 * 4) Cycles Used ",
                        79);
            tdm_freq_index=11;
            expected_tdm_cycles_min=20;
            expected_tdm_cycles_max=20;
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
    for (port=0;port < SB2_MAX_LOGICAL_PORTS ; port++) {
         sb2_tdm_port_slots_info[port].prev=&sb2_tdm_port_slots_info[port];
         sb2_tdm_port_slots_info[port].next=&sb2_tdm_port_slots_info[port];
         sb2_tdm_port_slots_info[port].position = -1;
    }
    for (index=0; index < total_tdm_slots; index++) {
         if ((tdm[index] == SB2_IDLE1) ||
             (tdm[index] == SB2_IDLE)) {
             if (tdm[index] == SB2_IDLE) {
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
             *offender_port = SB2_IDLE;
             return SOC_E_CONFIG;
         }
         idle_flag=0;
         port = tdm[index];
         if (sb2_tdm_port_slots_info[port].position == -1) {
             /* First Entry */
             sb2_tdm_port_slots_info[port].position = index;
             skip_count++;
             continue;
         }
         head = current = &sb2_tdm_port_slots_info[port];
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
       if (!((tdm[0] == SB2_IDLE) &&
             (tdm[total_tdm_slots-1] == SB2_IDLE))) {
              LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                       "### Internal Error: TDM Verification failed \n"
                       "### Two consecutive idle not found at Edges \n"
                       "### TCAM Atomicity might get affected.\n")));
           *offender_port = SB2_IDLE1;
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
   if (sb2_tdm_port_slots_info[SB2_LPBK].position == -1) {
       LOG_CLI((BSL_META_U(unit,
                           "Loopback port  missing in configuration \n")));
       return SOC_E_FAIL;
   }
   if (sb2_tdm_port_slots_info[SB2_CMIC].position == -1) {
       LOG_CLI((BSL_META_U(unit,
                           "CMIC port  missing in configuration \n")));
       return SOC_E_FAIL;
   }
   head = current = &sb2_tdm_port_slots_info[SB2_LPBK];
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
          *offender_port = SB2_LPBK;
          continue;
      } 
      } else {
          prev_tdm_slot_spacing = next_tdm_slot_spacing = 0;
      }
#if 0
      head1 = current1 = &sb2_tdm_port_slots_info[SB2_CMIC];
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
             *offender_port=SB2_LPBK;
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
   for (index=0;index < SB2_MAX_LOGICAL_PORTS && rv == SOC_E_NONE ;index++ ) {
        port = index ;
        if (sb2_tdm_port_slots_info[port].position == -1) {
            continue;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                " %d ==>\n\n"),port));
        switch(port) {
        case SB2_CMIC:
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "CMIM \n")));
            speed =2000; 
            break;
        case SB2_LPBK:
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
        worse_tdm_slot_spacing=sb2_tdm_cycles_info[tdm_freq_index][speed_index].
                               worse_tdm_slot_spacing;
        min_tdm_cycles=sb2_tdm_cycles_info[tdm_freq_index][speed_index].
                       min_tdm_cycles;

        count=0;
        head = current = &sb2_tdm_port_slots_info[port];
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
                if (port == SB2_CMIC) {
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
   for(mxqblock=0;mxqblock<(SB2_MAX_BLOCKS-1) && rv == SOC_E_NONE ;mxqblock++) {
       for(index=0;
           index<SB2_MAX_PORTS_PER_BLOCK && rv == SOC_E_NONE;
           index++) {
           outer_port = (*sb2_block_ports[unit])[mxqblock][index];
           if (outer_port == 0xFF) {
               if ((mxqblock == 7)  && (index == 3)) {
                    outer_port = 40;
               } else {
                   continue;
               }
           }
           if (sb2_tdm_port_slots_info[outer_port].position == -1) {
               continue;
           }
           head = current = &sb2_tdm_port_slots_info[outer_port];
           do {
              for(loop=0;loop<SB2_MAX_PORTS_PER_BLOCK; loop++) {
                  inner_port=(*sb2_block_ports[unit])[mxqblock][loop];
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
                  if (sb2_tdm_port_slots_info[inner_port].position == -1) {
                      continue;
                  }
                  head1 = current1 = &sb2_tdm_port_slots_info[inner_port];
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
  for (port=0;port < SB2_MAX_LOGICAL_PORTS ; port++) {
       head = current = &sb2_tdm_port_slots_info[port];
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
       sb2_tdm_port_slots_info[port].prev=NULL;
       sb2_tdm_port_slots_info[port].next=NULL;
       sb2_tdm_port_slots_info[port].position = -1;
  }
  sal_memcpy(&sb2_current_tdm_cycles_info,
             &sb2_tdm_cycles_info[tdm_freq_index][0],
             sizeof(tdm_cycles_info_t)*KT2_MAX_SPEEDS);
  return rv;
    return SOC_E_NONE;
}

void soc_saber2_save_tdm_pos(int unit, uint32 new_tdm_size,uint32 *new_tdm) {
    uint32 pos=0;
    uint8 port=0;
    uint8 block=0;
    uint8 total_slots=0;
    
    sal_memset(sb2_tdm_pos_info,0,sizeof(sb2_tdm_pos_info));
    for (pos = 0; pos < new_tdm_size ; pos++) {
         port= new_tdm[pos];
         if ((port == SB2_IDLE) || (port == SB2_IDLE1) ||
             (port == SB2_LPBK) || (port == SB2_CMIC)) {
              continue;
         }
         if(port > SB2_MAX_PHYSICAL_PORTS ) {
             LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                             "tdm_size %d, pos %d, port %d \n"),
                               new_tdm_size, pos, port));

         } else {
             block = (*sb2_port_to_block[unit])[port - 1];
             total_slots = sb2_tdm_pos_info[block].total_slots;
             sb2_tdm_pos_info[block].pos[total_slots]=pos;
             sb2_tdm_pos_info[block].total_slots++;
         }
    }

    sal_memcpy(sb2_current_tdm,new_tdm,new_tdm_size*sizeof(new_tdm[0]));
    sb2_current_tdm_size=new_tdm_size;

}

/* This function splits the occurance of first port of the block into 4 ports */
/* Make sure that the tdm of the block is not already split */
void sb2_block_tdm_split(int unit, uint32 *tdm,
        uint32 total_tdm_slots, uint32 block)
{
     uint32 index=0, count=0;
     uint32 first_port;

     first_port = (*sb2_block_ports[unit])[block][0];
     for (index=0; index < total_tdm_slots ; index++ ) {
         /* Split first port occurance in to 4 */
          if (tdm[index] == first_port) {
              tdm[index] = first_port + (count % 4);
              count++;
          }

     }
}

void sb2_tdm_replace(uint32 *tdm, uint32 total_tdm_slots,
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

void sb2_tdm_swap(uint32 *tdm, uint32 total_tdm_slots,
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
soc_saber2_reconfigure_tdm(int unit,uint32 new_tdm_size,uint32 *new_tdm)
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

    SOC_IF_ERROR_RETURN(_soc_saber2_get_cfg_num(unit, &cfg_num));
    if (cfg_num >= (sizeof(bcm56260_tdm)/sizeof(bcm56260_tdm[0]))) {
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
         if ( new_tdm[index*2] < SB2_IDLE ) {
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
         if ( new_tdm[(index*2)+1] < SB2_IDLE ) {
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
                        bcm56260_tdm_info[cfg_num].row,
                        bcm56260_tdm_info[cfg_num].col);
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
    soc_saber2_save_tdm_pos(unit, new_tdm_size,new_tdm);

    return SOC_E_NONE;
}

STATIC soc_error_t      
_soc_saber2_mmu_tdm_init(int unit) 
{
    soc_error_t     rv = SOC_E_NONE;
    uint32 *arr=NULL;
    uint32  row = 0, col = 0;
    uint32 offender_port=0;

    int i, tdm_size = 0,tdm_freq = 0, cfg_num;
    iarb_tdm_table_entry_t iarb_tdm;
    lls_port_tdm_entry_t lls_tdm;
    uint32 rval, arr_ele;
    uint16 dev_id;
    uint8 rev_id;
    uint32 port_enable_value=0;
    uint32 lpbk_port_speed=0;
    int loop_index, block;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    SOC_IF_ERROR_RETURN(_soc_saber2_get_cfg_num(unit, &cfg_num));
    if (cfg_num >= (sizeof(bcm56260_tdm)/sizeof(bcm56260_tdm[0]))) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Wrong cfg_num:%d exceeding max cfg_num: %d\n"),
                   cfg_num,
                   (int)(sizeof(bcm56260_tdm)/sizeof(bcm56260_tdm[0]))));
        return SOC_E_FAIL;
    } 

    switch(dev_id) {
    case BCM56260_DEVICE_ID:
    case BCM56261_DEVICE_ID:
    case BCM56265_DEVICE_ID:
    case BCM56266_DEVICE_ID:

    case BCM56262_DEVICE_ID:
    case BCM56267_DEVICE_ID:
    case BCM56263_DEVICE_ID:
    case BCM56268_DEVICE_ID:

    case BCM56460_DEVICE_ID:
    case BCM56461_DEVICE_ID:
    case BCM56465_DEVICE_ID:
    case BCM56466_DEVICE_ID:

    case BCM56462_DEVICE_ID:
    case BCM56467_DEVICE_ID:

    case BCM56463_DEVICE_ID:
    case BCM56468_DEVICE_ID:

    case BCM56233_DEVICE_ID:
        tdm_size= bcm56260_tdm_info[cfg_num].tdm_size;
        tdm_freq= bcm56260_tdm_info[cfg_num].tdm_freq;
        row= bcm56260_tdm_info[cfg_num].row;
        col= bcm56260_tdm_info[cfg_num].col;
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Cfg=%d freq=%d size=%d raw=%d col=%d \n"),
                   cfg_num,tdm_freq,tdm_size,row,col));
        arr = bcm56260_tdm[cfg_num];

        switch(cfg_num) {
        /* ------------------- */
        /* 56260 Configuration */
        /* ------------------- */
        case BCM56260_DEVICE_ID_OFFSET_CFG + 1:
             /* 56260_1 */
             /* 12x 2.5G + 2xXFI + 12.5G LPBK */
             sal_memcpy(arr,
                        &sb2_tdm_56260_B_ref[0],sizeof(sb2_tdm_56260_B_ref));
             lpbk_port_speed=2500;
             break;
        case BCM56260_DEVICE_ID_OFFSET_CFG + 2:
             /* 56260_2 */
             /* F.XAUI	F.XAUI			F.XAUI	N4s	2.5G	B1 */
             sal_memcpy(arr,
                        &sb2_tdm_56260_B1_ref[0],sizeof(sb2_tdm_56260_B1_ref));
             sb2_tdm_replace(arr,tdm_size, 22, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 23, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 24, 21, 0);
             lpbk_port_speed=2500;
             break;
        case BCM56262_DEVICE_ID_OFFSET_CFG + 1:
             /* 56262_1 */
             /* 4x2.5G	4x2.5G	- - - -	- 2.5G	EA 75 MHz */
             sal_memcpy(arr,
                        &sb2_tdm_56260_EA_ref[0],sizeof(sb2_tdm_56260_EA_ref));
             sb2_tdm_replace(arr,tdm_size, 25, SB2_IDLE1, 0);
             sb2_tdm_replace(arr,tdm_size, 26, SB2_IDLE1, 0);
             sb2_tdm_replace(arr,tdm_size, 27, SB2_IDLE1, 0);
             sb2_tdm_replace(arr,tdm_size, 28, SB2_IDLE1, 0);
             lpbk_port_speed=2500;
             break;

        case BCM56263_DEVICE_ID_OFFSET_CFG + 1:
             /* 56263_1 */
             /* 4xGE	4xGE - - - 4xGE 4xGE 2.5G A1A 37 MHz */
             sal_memcpy(arr,
                       &sb2_tdm_56263_A1A_ref[0],sizeof(sb2_tdm_56263_A1A_ref));
             sb2_tdm_replace(arr,tdm_size, 9 , 21, 0);
             sb2_tdm_replace(arr,tdm_size, 10, 22, 0);
             sb2_tdm_replace(arr,tdm_size, 11, 23, 0);
             sb2_tdm_replace(arr,tdm_size, 12, 24, 0);
             sb2_tdm_replace(arr,tdm_size, 13, 25, 0);
             sb2_tdm_replace(arr,tdm_size, 14, 26, 0);
             sb2_tdm_replace(arr,tdm_size, 15, 27, 0);
             sb2_tdm_replace(arr,tdm_size, 16, 28, 0);
             lpbk_port_speed=2500;
             break;
        case BCM56263_DEVICE_ID_OFFSET_CFG + 2:
             /* 56263_2 */
             /* 3xGE	3xGE	- - - -	- 2.5G	A1 24 MHz */
             sal_memcpy(arr,
                       &sb2_tdm_56263_A1_ref[0],sizeof(sb2_tdm_56263_A1_ref));
             lpbk_port_speed=2500;
             break;

        /* ------------------- */
        /* 56233 Configuration */
        /* ------------------- */
        case BCM56233_DEVICE_ID_OFFSET_CFG + 1:
             /* 56233_1 */
             /* 7xGE + 1x2.5GE	- - - -	- 2.5G	F 75 MHz */
             sal_memcpy(arr,
                        &sb2_tdm_56233_F_ref[0],sizeof(sb2_tdm_56233_F_ref));
             lpbk_port_speed=2500;
             break;

        /* ------------------- */
        /* 56460 Configuration */
        /* ------------------- */
        case BCM56460_DEVICE_ID_OFFSET_CFG + 1:
             /* 56460_1 */
             /* 24xGE 4xHGs11 4xGE 4xGE 4xGE 4xGE 4xGE 4xGE 4xGE N4 2.5G(LPBK)*/
             sal_memcpy(arr,
                        &sb2_tdm_56460_AA_ref[0],sizeof(sb2_tdm_56460_AA_ref));
             lpbk_port_speed=2500;
             break;
        case BCM56460_DEVICE_ID_OFFSET_CFG + 2:
             /* 56460_2 */
             /* F.XAUI F.XAUI F.XAUI F.XAUI F.XAUI F.XAUI 4xGE 2.5G(LPBK) */
             sal_memcpy(arr,
                        &sb2_tdm_56460_C2_ref[0],sizeof(sb2_tdm_56460_C2_ref));
             sb2_tdm_replace(arr,tdm_size, 14, 13, 0);
             sb2_tdm_replace(arr,tdm_size, 15, 13, 0);
             sb2_tdm_replace(arr,tdm_size, 16, 13, 0);
             lpbk_port_speed=2500;
             break;
        case BCM56460_DEVICE_ID_OFFSET_CFG + 3:
             /* 56460_3 */
             /* 2xF.XAUI + 12xGE + 1xF.XAUI + [2xXFI + 2xGE] + LPBK-2.5G */
             sal_memcpy(arr,
                        &sb2_tdm_56460_C_ref[0],sizeof(sb2_tdm_56460_C_ref));
             lpbk_port_speed=2500;
             break;
        case BCM56460_DEVICE_ID_OFFSET_CFG + 4:
             /* 56460_4 */
             /* 3x F.XAUI + 2x1G + .. 1xF.XAUI  + [2xXFI + 2x1G] + LPBK-2.5G */
             sal_memcpy(arr,
                        &sb2_tdm_56460_C1_ref[0],sizeof(sb2_tdm_56460_C1_ref));
             sb2_tdm_replace(arr,tdm_size, 13, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 14, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 15, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 16, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 17, 13, 0);
             sb2_tdm_replace(arr,tdm_size, 18, 15, 0);
             lpbk_port_speed=2500;
             break;
        case BCM56460_DEVICE_ID_OFFSET_CFG + 5:
             /* 56460_5 */
             /* F.XAUI F.XAUI F.XAUI	- 4xGE	F.XAUI	N2 */
             sal_memcpy(arr,
                        &sb2_tdm_56460_C1_ref[0],sizeof(sb2_tdm_56460_C1_ref));
             sb2_tdm_replace(arr,tdm_size, 13, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 14, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 15, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 16, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 26, 19, 0);
             sb2_tdm_replace(arr,tdm_size, 28, 20, 0);
             lpbk_port_speed=2500;
             break;
        case BCM56460_DEVICE_ID_OFFSET_CFG + 6:
             /* 56460_6 */
             /* F.XAUI	4xGE	-	-	4xGE	F.XAUI	N4s	12.5G */
             sal_memcpy(arr,
                        &sb2_tdm_56460_D_ref[0],sizeof(sb2_tdm_56460_D_ref));
             lpbk_port_speed=12500;
             break;
        case BCM56462_DEVICE_ID_OFFSET_CFG + 1:
             /* 56462_1 */
             /* F.XAUI(1) F.XAUI(5) F.XAUI(9) 4xGE 4xGE	4xGE(13..24) F.XAUI(25)	2.5G(LPBK) */
             sal_memcpy(arr,
                        &sb2_tdm_56462_B1A_ref[0],sizeof(sb2_tdm_56462_B1A_ref));
             sb2_tdm_replace(arr,tdm_size, 9, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 10, 22, 0);
             sb2_tdm_replace(arr,tdm_size, 11, 23, 0);
             sb2_tdm_replace(arr,tdm_size, 12, 24, 0);
             sb2_tdm_replace(arr,tdm_size, 27, 9, 0);
             sb2_tdm_replace(arr,tdm_size, 26, SB2_IDLE1, 0);
             sb2_tdm_replace(arr,tdm_size, 28, SB2_IDLE1, 0);
             lpbk_port_speed=2500;
             break;
        case BCM56462_DEVICE_ID_OFFSET_CFG + 2:
             /* 56462_2 */
             /* F.XAUI	F.XAUI	-	4xGE	4xGE	4xGE	N4s	2.5G */
             sal_memcpy(arr,
                       &sb2_tdm_56462_B1A_ref[0],sizeof(sb2_tdm_56462_B1A_ref));
             sb2_tdm_replace(arr,tdm_size, 9, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 10, 22, 0);
             sb2_tdm_replace(arr,tdm_size, 11, 23, 0);
             sb2_tdm_replace(arr,tdm_size, 12, 24, 0);
             lpbk_port_speed=2500;
             break;

        case BCM56463_DEVICE_ID_OFFSET_CFG + 1:
             /* 56463_1 */
             /* 4xGE	4xGE	-	-	-	4xGE	N4s	2.5G */
             sal_memcpy(arr,
                       &sb2_tdm_56463_E_ref[0],sizeof(sb2_tdm_56463_E_ref));
             sb2_tdm_replace(arr,tdm_size, 9, 21, 0);
             sb2_tdm_replace(arr,tdm_size, 10, 22, 0);
             sb2_tdm_replace(arr,tdm_size, 11, 23, 0);
             sb2_tdm_replace(arr,tdm_size, 12, 24, 0);
             lpbk_port_speed=2500;
             break;
        case BCM56463_DEVICE_ID_OFFSET_CFG + 2:
             /* 56463_2 */
             /* 4xGE 4xGE 4xGE 4xGE 4xGE 4xGE 4x2.5G 2.5G E1 */
             sal_memcpy(arr,
                       &sb2_tdm_56463_E1_ref[0],sizeof(sb2_tdm_56463_E1_ref));
             lpbk_port_speed=2500;
             break;
        case BCM56463_DEVICE_ID_OFFSET_CFG + 3:
             /* 56463_3 */
             /* 4xGE F.XAUI - -	- F.XAUI F.XAUI	2.5G	E3 */

             sal_memcpy(arr,
                       &sb2_tdm_56463_E3_ref[0],sizeof(sb2_tdm_56463_E3_ref));
             sb2_tdm_replace(arr,tdm_size, 1, 9, 0); /* 1 to 9  */
             sb2_tdm_replace(arr,tdm_size, 21, 1, 0);
             sb2_tdm_replace(arr,tdm_size, 22, 2, 0);
             sb2_tdm_replace(arr,tdm_size, 23, 3, 0);
             sb2_tdm_replace(arr,tdm_size, 24, 4, 0);
             sb2_tdm_replace(arr,tdm_size, 9, 21, 0); /* 9 to 21 */
             lpbk_port_speed=2500;
             break;
#ifdef BCM_WARM_BOOT_SUPPORT
        case COUNTOF(bcm56260_tdm_info) - 1:
             if (SOC_WARM_BOOT(unit)) {
                 sal_memcpy(arr,
                         &sb2_scache_tdm[0], sizeof(sb2_scache_tdm));
                 lpbk_port_speed=2500;                 
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
    loop_index = 0;
    while(sb2_selected_port_details[loop_index].start_port_no != 0) {
        block = loop_index;

        /* Change TDM only if the portgroup has been modified */
        if (sb2_selected_port_details[loop_index].port_incr ==
                sb2_selected_port_details_orig[loop_index].port_incr) {
            loop_index++;
            continue;
        }

        switch (sb2_selected_port_details[loop_index].port_incr){
            case PORTGROUP_SINGLE_MODE:
                if(sb2_selected_port_details_orig[loop_index].port_incr !=
                        PORTGROUP_QUAD_MODE) {
                    /* Exception for eagle port */
                    if ((sb2_selected_port_details[loop_index].start_port_no == 25) &&
                        (sb2_selected_port_details_orig[loop_index].port_incr == 2)) {
                         break;
                    }
                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                    "Setting TDM to Single Mode\n")));
                    for(i=1; i<SB2_MAX_PORTS_PER_BLOCK; i++) {
                        sb2_tdm_replace(arr,tdm_size,
                                (*sb2_block_ports[unit])[block][i],
                                (*sb2_block_ports[unit])[block][0], 0);
                    }
                }
                break;
            case PORTGROUP_DUAL_MODE:
                /* If original is quad mode, then we already have enough
                 * TDM slots for dual mode as well */
                if(sb2_selected_port_details_orig[loop_index].port_incr !=
                        PORTGROUP_QUAD_MODE) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "Setting TDM to Dual Mode\n")));
                    /* Convert single to Quad mode and then split */
                    sb2_block_tdm_split(unit, arr, tdm_size, block);
                    for(i=1; i<SB2_MAX_PORTS_PER_BLOCK; i+=2) {
                        sb2_tdm_replace(arr,tdm_size,
                                (*sb2_block_ports[unit])[block][i],
                                (*sb2_block_ports[unit])[block][i-1], 0);
                    }
                }
                break;
            case PORTGROUP_QUAD_MODE:
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "Setting TDM to Quad Mode\n")));
                /* Merge the slots and then split */
                /* This will take care of dual to quad block */
                for(i=1; i<SB2_MAX_PORTS_PER_BLOCK; i++) {
                    sb2_tdm_replace(arr,tdm_size,
                            (*sb2_block_ports[unit])[block][i],
                            (*sb2_block_ports[unit])[block][0], 0);
                }
                sb2_block_tdm_split(unit, arr, tdm_size, block);
                break;
            default:
                return SOC_E_INTERNAL;
        }
        loop_index++;
    }

    /* Verify the TDM structure */
     kt2_tdm_display(unit,arr, tdm_size,  row,  col);
     rv = sb2_tdm_verify(unit, (uint32 *)bcm56260_tdm[cfg_num], tdm_size,
                        tdm_freq, (uint32 *)bcm56260_speed[unit][cfg_num],
                        lpbk_port_speed, &offender_port);
    if ((rv == SOC_E_CONFIG) && (offender_port == SB2_IDLE)) {
        rv = SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(rv);

    /* Disable IARB TDM before programming... */
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf,
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    for (i = 0; i < tdm_size; i++) {
        arr_ele = arr[i];
        if (arr_ele < SB2_IDLE) { /* Non Idle Slots */
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

    soc_saber2_save_tdm_pos(unit, tdm_size, arr);

    return rv;
}

soc_error_t soc_saber2_get_port_block(
            int unit, soc_port_t port,uint8 *block)
{
    /* Return invalid port for all except physical port */
    if (!((port > 0 ) && (port < 29))) {
        return SOC_E_PORT;
    }

   *block = (*sb2_port_to_block[unit])[port - 1];
   return SOC_E_NONE;
}

soc_error_t soc_saber2_get_mxq_phy_port_mode(
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

soc_error_t soc_saber2_get_xl_phy_core_port_mode(
        int unit, soc_port_t port, 
        bcmXlPhyPortMode_t *phy_mode,
        bcmXlCorePortMode_t *core_mode) {
    
    uint8   block=0;
    uint8   loop=0;
    uint8   port_used[SB2_MAX_PORTS_PER_BLOCK]={0, 0, 0, 0};
    uint8   num_ports=0;
    int     cfg_num;
    char    *intf_type = NULL;
    uint32  rxaui = 0;
    soc_port_t  first_port;
    soc_info_t  *si = &SOC_INFO(unit);

    if (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
        return SOC_E_PORT;
    }

    if(!IS_XL_PORT(unit, port)) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(_soc_saber2_get_cfg_num(unit, &cfg_num));
    if (cfg_num >= (sizeof(bcm56260_tdm)/sizeof(bcm56260_tdm[0]))) {
        return SOC_E_FAIL;
    }
    
    SOC_IF_ERROR_RETURN(soc_saber2_get_port_block(unit,port,&block));
    first_port = (*sb2_block_ports[unit])[block][0];

    for(loop=0;loop<SB2_MAX_PORTS_PER_BLOCK; loop++) {
        if ((bcm56260_speed[unit][cfg_num]
                [(block*SB2_MAX_PORTS_PER_BLOCK)+loop]) &&
        (SOC_PBMP_MEMBER((PBMP_ALL(unit)), 
                         (block*SB2_MAX_PORTS_PER_BLOCK)+loop+1))){
            port_used[loop] = 1;
            num_ports++;
        }
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
soc_saber2_port_init_config(int unit, int cfg_num, soc_port_t port) {
    uint32      rval, port_speed;
    bcmMxqPhyPortMode_t phy_mode = -1;
    bcmXlCorePortMode_t core_mode_xl = -1;
    bcmXlPhyPortMode_t  phy_mode_xl = -1;

    port_speed = bcm56260_speed[unit][cfg_num][port-1];
    
    if(IS_MXQ_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_saber2_get_mxq_phy_port_mode(
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
        SOC_IF_ERROR_RETURN(soc_saber2_get_xl_phy_core_port_mode(unit, port,
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

soc_error_t
soc_saber2_port_enable_set(int unit, soc_port_t port, int enable)
{
   soc_field_t port_enable_field[SB2_MAX_PORTS_PER_BLOCK]=
                {PORT0f, PORT1f, PORT2f , PORT3f};
   uint32       rval, rval_old;
   uint8        loop;
   uint32       port_enable;
   uint32       entry[SOC_MAX_MEM_WORDS];

   loop = (port-1) % 4;

   if (SOC_REG_PORT_VALID(unit, TXLP_PORT_ENABLEr, port)) {
       SOC_IF_ERROR_RETURN(READ_TXLP_PORT_ENABLEr(unit, port, &rval));
       rval_old = rval;
       port_enable = soc_reg_field_get(unit, TXLP_PORT_ENABLEr,
               rval, PORT_ENABLEf);
       port_enable |= (1 << loop);
       soc_reg_field_set(unit, TXLP_PORT_ENABLEr, &rval,
               PORT_ENABLEf,port_enable);
       if (rval_old != rval) {
           SOC_IF_ERROR_RETURN(WRITE_TXLP_PORT_ENABLEr(unit, port, rval));
       }
   }    
        
   if (SOC_REG_PORT_VALID(unit, TXLP_MIN_STARTCNTr, port)) {
        SOC_IF_ERROR_RETURN(READ_TXLP_MIN_STARTCNTr(unit, port, &rval));
        soc_reg_field_set(unit, TXLP_MIN_STARTCNTr, &rval,
                NLP_STARTCNTf, 3);
        SOC_IF_ERROR_RETURN(WRITE_TXLP_MIN_STARTCNTr(unit, port, rval));
   }
       
   /* Bring IECELL block out of reset. */
    if (SOC_REG_PORT_VALID(unit, IECELL_CONFIGr, port)) {
        SOC_IF_ERROR_RETURN(READ_IECELL_CONFIGr(unit, port, &rval));
        rval_old = rval;
        if (enable)
            soc_reg_field_set(unit, IECELL_CONFIGr, &rval, SOFT_RESETf, 0);
        else
            soc_reg_field_set(unit, IECELL_CONFIGr, &rval, SOFT_RESETf, 1);
        if (rval_old != rval) {
            SOC_IF_ERROR_RETURN(WRITE_IECELL_CONFIGr(unit, port, rval));
        }
    }

    if (SOC_REG_PORT_VALID(unit, XPORT_PORT_ENABLEr, port)) {
        SOC_IF_ERROR_RETURN(READ_XPORT_PORT_ENABLEr(unit, port, &rval));
        rval_old = rval;
        if (enable) {
            soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &rval,
                    port_enable_field[loop], 1);
        } else {
            soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &rval,
                    port_enable_field[loop], 0);
        }
        if (rval_old != rval) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(unit, port, rval));
        }
    }
         
   if (SOC_REG_PORT_VALID(unit, XLPORT_ENABLE_REGr, port)) {
        SOC_IF_ERROR_RETURN(READ_XLPORT_ENABLE_REGr(unit, port, &rval));
        rval_old = rval;
        if (enable) {
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval,
                    port_enable_field[loop], 1);
        } else {
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval,
                    port_enable_field[loop], 0);
        }
        if (rval_old != rval) {
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, port, rval));
        }
   }

    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    if (enable) {
       soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    } else {
       soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, entry));

    return SOC_E_NONE;
}

void soc_saber2_block_reset(int unit, uint8 block,int active_low) {
    uint32 rval;
    soc_field_t hot_swap_reset_fld[]={
                TOP_MXQ0_HOTSWAP_RST_Lf,TOP_MXQ1_HOTSWAP_RST_Lf,
                TOP_MXQ2_HOTSWAP_RST_Lf,TOP_MXQ3_HOTSWAP_RST_Lf,
                TOP_MXQ4_HOTSWAP_RST_Lf,TOP_MXQ5_HOTSWAP_RST_Lf,
                TOP_PM_HOTSWAP_RST_Lf};

    if (READ_TOP_SOFT_RESET_REGr(unit, &rval) != SOC_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
                            "ATTN: Reading TOP_SOFT_RESET_REG failed \n")));
        return;
    }
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval,
                      hot_swap_reset_fld[block],active_low);
    WRITE_TOP_SOFT_RESET_REGr(unit, rval);
 
}

STATIC
int _soc_saber2_mmu_config_extra_queue(int unit, uint32 queue,
                          _soc_sb2_mmu_params_t *_soc_sb2_mmu_params,
                          _sb2_output_port_threshold_t *output_port_threshold)
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

     if (_soc_sb2_mmu_params->lossless_mode_d_c) {
         temp_val = output_port_threshold->
         min_grntd_tot_shr_queue_cells_int_buff;
     } else {
         temp_val = _soc_sb2_mmu_params->
                mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
     }
     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                         &mmu_thdo_qconfig_cell_entry, 
                         Q_SHARED_LIMIT_CELLf,
                         temp_val);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                         &mmu_thdo_qconfig_cell_entry, 
                         Q_LIMIT_DYNAMIC_CELLf,
                         _soc_sb2_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                         &mmu_thdo_qconfig_cell_entry, 
                         Q_LIMIT_ENABLE_CELLf,
                         _soc_sb2_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                        &mmu_thdo_qconfig_cell_entry, 
                        Q_COLOR_ENABLE_CELLf,0);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                         &mmu_thdo_qconfig_cell_entry, 
                         Q_COLOR_LIMIT_DYNAMIC_CELLf,
                         _soc_sb2_mmu_params->lossless_mode_d_c ? 0 : 1);

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

     if (_soc_sb2_mmu_params->lossless_mode_d_c) {
         temp_val = output_port_threshold->
         min_grntd_tot_shr_queue_cells_EQEs;
     } else {
         temp_val = _soc_sb2_mmu_params->
         mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
     }
    soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                        &mmu_thdo_qconfig_qentry_entry, 
                        Q_SHARED_LIMIT_QENTRYf, temp_val);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                         &mmu_thdo_qconfig_qentry_entry, 
                         Q_LIMIT_DYNAMIC_QENTRYf,
                         _soc_sb2_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                         &mmu_thdo_qconfig_qentry_entry, 
                         Q_LIMIT_ENABLE_QENTRYf,
                         _soc_sb2_mmu_params->lossless_mode_d_c ? 0 : 1);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                         &mmu_thdo_qconfig_qentry_entry, 
                         Q_COLOR_ENABLE_QENTRYf,0);

     soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm,
                         &mmu_thdo_qconfig_qentry_entry,
                         Q_COLOR_LIMIT_DYNAMIC_QENTRYf,
                         _soc_sb2_mmu_params->lossless_mode_d_c ? 0 : 1);

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
 * memory selection (internal/external/both) and mode (lossless/lossy)
 * The memory selection is done using config variable  'pbmp_ext_mem'
 * and the mode is selected based on config variable 'lossless_mode'
 */
STATIC 
int _soc_saber2_mmu_init_helper(int unit, int flex_port)
{
    soc_info_t    *si= &SOC_INFO(unit); 
    uint16        dev_id=0;
    uint8         rev_id=0;
    uint32        ext_mem_port_count = 0;
    uint32        valid_port_count = 0;
    soc_pbmp_t    pbmp_int_ge;
    uint32        pbmp_int_ge_count=0;
    soc_pbmp_t    pbmp_int_hg;
    uint32        pbmp_int_hg_count=0;
    soc_pbmp_t    pbmp_ext_ge;
    uint32        pbmp_ext_ge_count=0;
    soc_pbmp_t    pbmp_ext_hg;
    uint32        pbmp_ext_hg_count=0;
    uint32        mmu_num_olp_port=0;
    soc_pbmp_t    valid_port_cpu_pbmp;
    soc_port_t    port=0;
    uint32        available_internal_buffer=100;
    uint32        reserve_internal_buffer=0;
    uint32        temp_val;
    uint32        rval=0;
    uint32        service_pool=0;
    uint32        priority_group=0;
    uint32        queue;
    uint32        op_node=0;
    uint32        op_node_offset=0;
    uint32        cos;
    uint32        idx, sub_port;
    uint32        val;
    int           pg_min_configured = 0, pg_min = 0;
    char          suffix;

    /* Per Port  Settings */
    uint32                      mem_idx=0;
    thdi_port_sp_config_entry_t thdi_port_sp_config={{0}};
    /*
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_SERVICE_POOLS] = {{{0}}};
     */
    thdi_port_pg_config_entry_t thdi_port_pg_config={{0}};
    /*
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_PRIORITY_GROUPS] = {{{0}}};
     */

    thdiext_thdi_port_sp_config_entry_t thdiext_thdi_port_sp_config={{0}};
    /* 
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_SERVICE_POOLS] = {{{0}}};
     */
    thdiext_thdi_port_pg_config_entry_t thdiext_thdi_port_pg_config={{0}};
    /*
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_PRIORITY_GROUPS] = {{{0}}};
     */
       thdiema_thdi_port_sp_config_entry_t thdiema_thdi_port_sp_config = {{0}};
    /* 
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_SERVICE_POOLS] = {{{0}}};
     */
    thdiema_thdi_port_pg_config_entry_t thdiema_thdi_port_pg_config={{0}};
    /*
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_PRIORITY_GROUPS] = {{{0}}};
     */
    thdirqe_thdi_port_sp_config_entry_t thdirqe_thdi_port_sp_config={{0}};
    /*
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_SERVICE_POOLS] = {{{0}}};
     */
    thdirqe_thdi_port_pg_config_entry_t thdirqe_thdi_port_pg_config={{0}};
    /*
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_PRIORITY_GROUPS] = {{{0}}};
     */
    thdiqen_thdi_port_sp_config_entry_t thdiqen_thdi_port_sp_config = {{0}};
    /* 
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_SERVICE_POOLS] = {{{0}}};
     */
    thdiqen_thdi_port_pg_config_entry_t thdiqen_thdi_port_pg_config = {{0}};
    /*
       (SB2_MAX_PHYSICAL_PORTS+2) *
       SB2_MAX_PRIORITY_GROUPS] = {{{0}}};
     */

    mmu_thdo_opnconfig_cell_entry_t     mmu_thdo_opnconfig_cell_entry={{0}};
    mmu_thdo_qconfig_cell_entry_t       mmu_thdo_qconfig_cell_entry={{0}};
    mmu_thdo_qoffset_cell_entry_t       mmu_thdo_qoffset_cell_entry={{0}};

    mmu_thdo_opnconfig_qentry_entry_t   mmu_thdo_opnconfig_qentry_entry={{0}};
    mmu_thdo_qconfig_qentry_entry_t     mmu_thdo_qconfig_qentry_entry={{0}};
    mmu_thdo_qoffset_qentry_entry_t     mmu_thdo_qoffset_qentry_entry={{0}};

    _sb2_general_info_t *general_info = 
        &_soc_sb2_mmu_intermediate_results.general_info;
    _sb2_input_port_threshold_t *input_port_threshold =
        &_soc_sb2_mmu_intermediate_results.input_port_threshold;
    _sb2_output_port_threshold_t *output_port_threshold = 
        &_soc_sb2_mmu_intermediate_results.output_port_threshold;

    sal_memset(&_soc_sb2_mmu_intermediate_results, 0,
            sizeof(_soc_sb2_mmu_intermediate_results_t));
    sal_memset(&_soc_sb2_mmu_params,0,sizeof(_soc_sb2_mmu_params_t));

    /* Fixed Parameter */
    _soc_sb2_mmu_params.mmu_min_pkt_size = 64;              /*C5*/
    _soc_sb2_mmu_params.mmu_ethernet_mtu_bytes=1536;      /*C6*/
    _soc_sb2_mmu_params.mmu_max_pkt_size = 12288;            /*C7*/
    _soc_sb2_mmu_params.mmu_jumbo_frame_size = 9216;        /*C8*/
    _soc_sb2_mmu_params.mmu_int_buf_cell_size = 190;        /*C9*/
    /* C10  = (C9+2)*15 */
    _soc_sb2_mmu_params.mmu_ext_buf_cell_size = 
        (_soc_sb2_mmu_params.mmu_int_buf_cell_size + 2) * 15;
    _soc_sb2_mmu_params.mmu_pkt_header_size = 62;           /*C11*/
    _soc_sb2_mmu_params.mmu_lossless_pg_num = 1;            /*C16*/
    _soc_sb2_mmu_params.mmu_line_rate_ge = 1;               /*C20*/
    _soc_sb2_mmu_params.mmu_line_rate_hg = 10;              /*C26*/
    _soc_sb2_mmu_params.mmu_num_cpu_port = 1;               /*C31*/
    _soc_sb2_mmu_params.mmu_num_cpu_queue = 48;             /*C32*/
    _soc_sb2_mmu_params.mmu_num_loopback_port = 1;          /*C34*/
    _soc_sb2_mmu_params.mmu_num_loopback_queue = 8;         /*C35*/
    _soc_sb2_mmu_params.mmu_num_ep_redirection_queue = 16;  /*C36*/
    _soc_sb2_mmu_params.mmu_num_olp_queue = 8;              /*C38*/
    _soc_sb2_mmu_params.mmu_pipeline_lat_cpap_admission = 30;           /*C41*/
    _soc_sb2_mmu_params.mmu_reserved_int_buf_cells = 42;    /*C44*/
    _soc_sb2_mmu_params.mmu_ext_buf_size = 180;             /*C47*/
    _soc_sb2_mmu_params.mmu_ep_redirect_queue_entries = 480;/*C50*/
    _soc_sb2_mmu_params.mmu_repl_engine_work_queue_entries = 4;    /*C52*/
    _soc_sb2_mmu_params.mmu_resv_repl_engine_work_queue_entries = 292;    /*C53*/
    _soc_sb2_mmu_params.mmu_repl_engine_work_queue_in_device = 12;    /*C54*/
    _soc_sb2_mmu_params.mmu_ema_queues = 8;    /*C55*/
    _soc_sb2_mmu_params.mmu_ing_port_dyn_thd_param = 2; /*C60*/
    _soc_sb2_mmu_params.mmu_ing_pg_dyn_thd_param = 7;  /*C61*/
    _soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf = 2; /*C62*/
    _soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy = 7;  /*C63*/
    _soc_sb2_mmu_params.mmu_ing_cell_buf_reduction = 0; /*C64*/
    _soc_sb2_mmu_params.mmu_ing_pkt_buf_reduction = 0; /*C65*/

    SOC_PBMP_CLEAR (pbmp_int_ge);
    SOC_PBMP_CLEAR (pbmp_int_hg);
    SOC_PBMP_CLEAR (pbmp_ext_ge);
    SOC_PBMP_CLEAR (pbmp_ext_hg);

    SOC_PBMP_CLEAR (valid_port_cpu_pbmp);
    SOC_PBMP_ASSIGN(valid_port_cpu_pbmp, PBMP_ALL(unit));
    /* SOC_PBMP_PORT_ADD(valid_port_cpu_pbmp, si->cmic_port); */

    soc_cm_get_id(unit, &dev_id, &rev_id);
    /* C12 */
   if (soc_feature(unit, soc_feature_mmu_packing)) {
       _soc_sb2_mmu_params.packing_mode_d_c =  1;
   }

    /* C13 */
    _soc_sb2_mmu_params.max_pkt_size_support_packing = 10;

    /* C14 */
    _soc_sb2_mmu_params.lossless_mode_d_c =  (soc_property_get(unit,
                                                spn_LOSSLESS_MODE, 1) &
                                             soc_property_get(unit,
                                                spn_MMU_LOSSLESS, 1));
    /* C15 */
    _soc_sb2_mmu_params.pfc_pause_mode_d_c =  soc_property_get(unit, 
            "pfc_pause_mode", 0);

    SOC_PBMP_COUNT(PBMP_PORT_ALL (unit), valid_port_count);
    SOC_PBMP_COUNT(PBMP_EXT_MEM (unit), ext_mem_port_count);/*spn_PBMP_EXT_MEM*/

    /* C17 */
    if (ext_mem_port_count == 0) {
        _soc_sb2_mmu_params.extbuf_used_d_c =  0;
        if (SOC_DDR3_NUM_MEMORIES(unit)) {
            /* Support for dynamic addition of port to ext memory */
            _soc_sb2_mmu_params.extbuf_used_d_c =  1;
        }
    } else {
        _soc_sb2_mmu_params.extbuf_used_d_c =  1;
    } 

    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        _soc_sb2_mmu_params.mmu_int_buf_size = 8192;      /*C42*/
    } else {
        if (soc_feature(unit, soc_feature_mmu_reduced_internal_buffer)) {
            _soc_sb2_mmu_params.mmu_int_buf_size = 5461;     /*C42*/
        } else {
            _soc_sb2_mmu_params.mmu_int_buf_size = 10922;     /*C42*/
        }
    }
    SOC_PBMP_ASSIGN(pbmp_int_ge,PBMP_GE_ALL(unit));
    SOC_PBMP_ASSIGN(pbmp_int_hg,PBMP_HG_ALL(unit));
    SOC_PBMP_OR(pbmp_int_hg,PBMP_XE_ALL(unit));
    SOC_PBMP_ITER (PBMP_EXT_MEM (unit), port) {
        if (IS_GE_PORT(unit, port)) {
            SOC_PBMP_PORT_ADD(pbmp_ext_ge,port);
            SOC_PBMP_PORT_REMOVE(pbmp_int_ge,port);
        } 
        if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
            SOC_PBMP_PORT_ADD(pbmp_ext_hg,port);
            SOC_PBMP_PORT_REMOVE(pbmp_int_hg,port);
        }
    }
    SOC_PBMP_COUNT(pbmp_ext_ge,pbmp_ext_ge_count);
    SOC_PBMP_COUNT(pbmp_int_ge,pbmp_int_ge_count);
    SOC_PBMP_COUNT(pbmp_ext_hg,pbmp_ext_hg_count);
    SOC_PBMP_COUNT(pbmp_int_hg,pbmp_int_hg_count);

    /* C19 */
    _soc_sb2_mmu_params.num_ge_ports_d = pbmp_int_ge_count + pbmp_ext_ge_count;
    /* C21 */
    _soc_sb2_mmu_params.num_ge_int_ports_d = pbmp_int_ge_count;
    /* C22 */
    _soc_sb2_mmu_params.num_egr_queue_per_int_ge_port_d = pbmp_int_ge_count ? 8:0;
    /* C23 */
    _soc_sb2_mmu_params.num_ge_ext_ports_d = pbmp_ext_ge_count;
    /* C24 */
    _soc_sb2_mmu_params.num_egr_queue_per_ext_ge_port_d = pbmp_ext_ge_count ? 
        (_soc_sb2_mmu_params.packing_mode_d_c ? 16:8):0;
    /* C25 */
    _soc_sb2_mmu_params.num_hg_ports_d = pbmp_int_hg_count + pbmp_ext_hg_count;
    /* C27 */
    _soc_sb2_mmu_params.num_hg_int_ports_d = pbmp_int_hg_count ;
    /* C28 */
    _soc_sb2_mmu_params.num_egr_queue_per_int_hg_port_d = pbmp_int_hg_count ? 8:0;
    /* C29 */
    _soc_sb2_mmu_params.num_hg_ext_ports_d = pbmp_ext_hg_count ;
    /* C30 */
    _soc_sb2_mmu_params.num_egr_queue_per_ext_hg_port_d = pbmp_ext_hg_count ?
        (_soc_sb2_mmu_params.packing_mode_d_c ? 16:8):0;
    /* C33 */
    _soc_sb2_mmu_params.cpu_port_int_ext_bounding_d_c = soc_property_get(unit, 
            "cpu_port_bounding",0);
    /* C37 */
    _soc_sb2_mmu_params.mmu_num_olp_port_d = mmu_num_olp_port;
    /* C39 */
    _soc_sb2_mmu_params.olp_port_int_ext_bounding_d_c = soc_property_get(unit, 
            "olp_port_bounding",0);
    /* C40 */
    _soc_sb2_mmu_params.mmu_total_egress_queue = 1024;
    if ((_soc_sb2_mmu_params.lossless_mode_d_c)) {
        _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt = 10; 
    } else {
        /* C51 = C19+C25-1 */
        _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt =  
            _soc_sb2_mmu_params.num_ge_ports_d +
            _soc_sb2_mmu_params.num_hg_ports_d - 1 ;
    }
    if (((_soc_sb2_mmu_params.extbuf_used_d_c) ||
                (_soc_sb2_mmu_params.packing_mode_d_c)) && 
            (_soc_sb2_mmu_params.lossless_mode_d_c)) { 
        available_internal_buffer=60;
        reserve_internal_buffer=40;
    } else if (((_soc_sb2_mmu_params.extbuf_used_d_c) || 
                (_soc_sb2_mmu_params.packing_mode_d_c)) && 
            !(_soc_sb2_mmu_params.lossless_mode_d_c)) {
        available_internal_buffer=70;
        reserve_internal_buffer=30;
    } else {
        available_internal_buffer=100;
        reserve_internal_buffer=0;
    }
    /* C43 */
    _soc_sb2_mmu_params.mmu_available_int_buf_size_d=available_internal_buffer;
    /* (_soc_sb2_mmu_params.mmu_int_buf_size*available_internal_buffer)/100; */
    /* C45 */
    _soc_sb2_mmu_params.mmu_reserved_int_buf_ema_pool_size_d=
        reserve_internal_buffer;
    /* (_soc_sb2_mmu_params.mmu_int_buf_size*reserve_internal_buffer)/100; */

    /* C46 */
    _soc_sb2_mmu_params.internal_buffer_reduction_d_c = soc_property_get(unit, 
            "internal_buffer_reduction",0);

    /* C47 */    
    _soc_sb2_mmu_params.mmu_ext_buf_size = (64*1024*_soc_sb2_mmu_params.mmu_ext_buf_cell_size) >> 20 ;
    
    /* C48 */
    _soc_sb2_mmu_params.mmu_reserved_ext_buf_space_cfap = 64;

    /* C49 */
    _soc_sb2_mmu_params.mmu_egress_queue_entries = 64; 

    /* C58 = CEILING((C6*1024+C11)/C9, 1) */
    _soc_sb2_mmu_params.per_cos_res_cells_for_int_buff_d = 
        sal_ceil_func((_soc_sb2_mmu_params.mmu_ethernet_mtu_bytes + 
                    _soc_sb2_mmu_params.mmu_pkt_header_size),
                _soc_sb2_mmu_params.mmu_int_buf_cell_size);

    /* C59 =IF(C12=0,CEILING(C58/15,1),CEILING((C6*1024+C11+2)/C10,1)) */

    if (_soc_sb2_mmu_params.packing_mode_d_c == 0) {
        _soc_sb2_mmu_params.per_cos_res_cells_for_ext_buff_d = 
            sal_ceil_func(_soc_sb2_mmu_params.per_cos_res_cells_for_int_buff_d,15);
    } else {
        _soc_sb2_mmu_params.per_cos_res_cells_for_ext_buff_d = 
            sal_ceil_func((_soc_sb2_mmu_params.mmu_ethernet_mtu_bytes + 
                        _soc_sb2_mmu_params.mmu_pkt_header_size + 2),
                    _soc_sb2_mmu_params.mmu_ext_buf_cell_size);
    }

    /* Intermediate Results Processing Now */
    /* Fill up general info */
    /* C72 = CEILING((C7+C11)/C9,1) */
    general_info->max_packet_size_in_cells = sal_ceil_func(
            (_soc_sb2_mmu_params.mmu_max_pkt_size + 
             _soc_sb2_mmu_params.mmu_pkt_header_size),
            _soc_sb2_mmu_params.mmu_int_buf_cell_size);
    /* C73 = CEILING((C8+C11)/C9,1) */
    general_info->jumbo_frame_for_int_buff = sal_ceil_func(
            (_soc_sb2_mmu_params.mmu_jumbo_frame_size +
             _soc_sb2_mmu_params.mmu_pkt_header_size),
            _soc_sb2_mmu_params.mmu_int_buf_cell_size);
    /* C74 =IF(C12=0,CEILING(C73/15,1),CEILING((C8+C11+2)/C10,1)) */
    if (_soc_sb2_mmu_params.packing_mode_d_c == 0) {
        general_info->jumbo_frame_for_ext_buff = sal_ceil_func(
                general_info->jumbo_frame_for_int_buff,15);
    } else {
        general_info->jumbo_frame_for_ext_buff = sal_ceil_func(
                _soc_sb2_mmu_params.mmu_jumbo_frame_size +
                _soc_sb2_mmu_params.mmu_pkt_header_size + 2,
                _soc_sb2_mmu_params.mmu_ext_buf_cell_size);
    }
    /* C76 = CEILING((C6*1024+C11)/C9, 1) */
    general_info->ether_mtu_cells_for_int_buff = sal_ceil_func(
            (_soc_sb2_mmu_params.mmu_ethernet_mtu_bytes) +
            _soc_sb2_mmu_params.mmu_pkt_header_size,
            _soc_sb2_mmu_params.mmu_int_buf_cell_size);

    /* C77 = IF(C12=0,CEILING(C76/15,1),CEILING((C6*1024+C11+2)/C10,1)) */
    if (_soc_sb2_mmu_params.packing_mode_d_c == 0) {
        general_info->ether_mtu_cells_for_ext_buff = sal_ceil_func(
                general_info->ether_mtu_cells_for_int_buff,15);
    } else {
        general_info->ether_mtu_cells_for_ext_buff = sal_ceil_func(
                (_soc_sb2_mmu_params.mmu_ethernet_mtu_bytes)+
                _soc_sb2_mmu_params.mmu_pkt_header_size+2,
                _soc_sb2_mmu_params.mmu_ext_buf_cell_size);
    }

    /* C79 = C19+C25+C31+C34+C37 */
    general_info->total_num_of_ports = 
        _soc_sb2_mmu_params.num_ge_ports_d +
        _soc_sb2_mmu_params.num_hg_ports_d + 
        _soc_sb2_mmu_params.mmu_num_cpu_port + 
        _soc_sb2_mmu_params.mmu_num_loopback_port + 
        _soc_sb2_mmu_params.mmu_num_olp_port_d;

    /* C80 = C19+C25+C31+C37 ) */
    general_info->total_num_of_ports_excl_lpbk = 
        _soc_sb2_mmu_params.num_ge_ports_d +
        _soc_sb2_mmu_params.num_hg_ports_d + 
        _soc_sb2_mmu_params.mmu_num_cpu_port + 
        _soc_sb2_mmu_params.mmu_num_olp_port_d;

    /* C81 = C19+C25+C31 */
    general_info->total_num_of_ports_excl_lpbk_olp =
        _soc_sb2_mmu_params.num_ge_ports_d +
        _soc_sb2_mmu_params.num_hg_ports_d + 
        _soc_sb2_mmu_params.mmu_num_cpu_port;

    /* C82 = C19+C25 */
    general_info->total_num_of_ports_excl_lpbk_olp_cpu = 
        _soc_sb2_mmu_params.num_ge_ports_d +
        _soc_sb2_mmu_params.num_hg_ports_d;

    /* C83 = C23*C20+C29*C26 */
    general_info->port_bw_bound_to_ext_buff = 
        (_soc_sb2_mmu_params.num_ge_ext_ports_d *
         _soc_sb2_mmu_params.mmu_line_rate_ge) +
        (_soc_sb2_mmu_params.num_hg_ext_ports_d *
         _soc_sb2_mmu_params.mmu_line_rate_hg) ;

    /* C84 = C22*C21 */
    general_info->total_egr_queues_for_a_int_ge_ports = 
        (_soc_sb2_mmu_params.num_egr_queue_per_int_ge_port_d *
         _soc_sb2_mmu_params.num_ge_int_ports_d);

    /* C85 = C24*C23 */
    general_info->total_egr_queues_for_a_ext_ge_ports = 
        (_soc_sb2_mmu_params.num_egr_queue_per_ext_ge_port_d *
         _soc_sb2_mmu_params.num_ge_ext_ports_d);

    /* C86 = C28*C27 */
    general_info->total_egr_queues_for_a_int_hg_ports = 
        (_soc_sb2_mmu_params.num_egr_queue_per_int_hg_port_d *
         _soc_sb2_mmu_params.num_hg_int_ports_d);

    /* C87 = C30*C29 */
    general_info->total_egr_queues_for_a_ext_hg_ports = 
        (_soc_sb2_mmu_params.num_egr_queue_per_ext_hg_port_d *
         _soc_sb2_mmu_params.num_hg_ext_ports_d);

    /* C88 = C31*C32 */
    general_info->total_cpu_queues = 
        (_soc_sb2_mmu_params.mmu_num_cpu_port *
         _soc_sb2_mmu_params.mmu_num_cpu_queue);

    /* C89 =$C$84+$C$86+IF(C33=0,C88,0)+IF(C39=0,C38*C37,0)+C34*C35+C34*C36/2 */
    general_info->total_base_queue_int_buff = 
        (general_info->total_egr_queues_for_a_int_ge_ports +
         general_info->total_egr_queues_for_a_int_hg_ports);
    if (_soc_sb2_mmu_params.cpu_port_int_ext_bounding_d_c == 0) {
        general_info->total_base_queue_int_buff += general_info->total_cpu_queues; 
    }
    if (_soc_sb2_mmu_params.olp_port_int_ext_bounding_d_c == 0) {
        general_info->total_base_queue_int_buff += 
            (_soc_sb2_mmu_params.mmu_num_olp_queue * 
             _soc_sb2_mmu_params.mmu_num_olp_port_d); 
    }
    general_info->total_base_queue_int_buff += 
        (_soc_sb2_mmu_params.mmu_num_loopback_queue *
         _soc_sb2_mmu_params.mmu_num_loopback_port);
    general_info->total_base_queue_int_buff += 
        ((_soc_sb2_mmu_params.mmu_num_ep_redirection_queue * 
          _soc_sb2_mmu_params.mmu_num_loopback_port)/ 2);

    /* C90 = =$C$85+$C$87+IF(C33=1,C88,0)+IF(C39=1,C37*C38,0)+IF(C17=1,C34*C36/2,0) */ 
    general_info->total_base_queue_ext_buff =
        (general_info->total_egr_queues_for_a_ext_ge_ports +
         general_info->total_egr_queues_for_a_ext_hg_ports);
    if (_soc_sb2_mmu_params.cpu_port_int_ext_bounding_d_c) {
        general_info->total_base_queue_ext_buff += general_info->total_cpu_queues; 
    }
    if (_soc_sb2_mmu_params.olp_port_int_ext_bounding_d_c) {
        general_info->total_base_queue_ext_buff += 
            (_soc_sb2_mmu_params.mmu_num_olp_queue *
             _soc_sb2_mmu_params.mmu_num_olp_port_d);
    }
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        general_info->total_base_queue_ext_buff += 
            ((_soc_sb2_mmu_params.mmu_num_loopback_port *
              _soc_sb2_mmu_params.mmu_num_ep_redirection_queue)/2);
    }

    /* C56 =IF(C12=0, 0, 2*C40) */
    if (_soc_sb2_mmu_params.packing_mode_d_c) {
        _soc_sb2_mmu_params.num_cells_rsrvd_ing_ext_buf = 
            _soc_sb2_mmu_params.mmu_total_egress_queue * 2;
    }

    /* C91 = C55 */
    general_info->total_ema_queues = _soc_sb2_mmu_params.mmu_ema_queues;

    /* C93 = C89+C90 */ 
    general_info->total_egr_base_queues_in_device = 
        general_info->total_base_queue_int_buff + 
        general_info->total_base_queue_ext_buff;
    /* C94 = C40 */
    general_info->total_egr_queues_in_device =
        _soc_sb2_mmu_params.mmu_total_egress_queue ;

    /* C95 = C42-C44*/
    general_info->max_int_cell_buff_size = 
        _soc_sb2_mmu_params.mmu_int_buf_size -
        _soc_sb2_mmu_params.mmu_reserved_int_buf_cells ;

    /* C96 = C95 */
    general_info->int_cell_buff_size_after_limitation = 
        general_info->max_int_cell_buff_size;

    /* C97 =IF(C12=0, 0, C79*C13) */
    if (_soc_sb2_mmu_params.packing_mode_d_c) {
        general_info->src_packing_fifo = general_info->total_num_of_ports *
            _soc_sb2_mmu_params.max_pkt_size_support_packing;
    }

    /* C98 =FLOOR($C$43 *( IF($C$46, $C$96, $C$95)) - C41 - C97, 1) */
    if (_soc_sb2_mmu_params.internal_buffer_reduction_d_c) {
        temp_val = general_info->int_cell_buff_size_after_limitation;
    } else {
        temp_val = general_info->max_int_cell_buff_size;
    }
    temp_val = (_soc_sb2_mmu_params.mmu_available_int_buf_size_d * temp_val)/100;
    temp_val -= _soc_sb2_mmu_params.mmu_pipeline_lat_cpap_admission;
    temp_val -= general_info->src_packing_fifo;
    general_info->int_buff_pool = temp_val;

    /* C99 =IF(C17=1,FLOOR($C$45*(IF($C$46,$C$96,$C$95))-C41,1),0) */
    if (_soc_sb2_mmu_params.internal_buffer_reduction_d_c) {
        temp_val = general_info->int_cell_buff_size_after_limitation;
    } else {
        temp_val = general_info->max_int_cell_buff_size;;
    }
    temp_val = (temp_val * _soc_sb2_mmu_params.mmu_reserved_int_buf_ema_pool_size_d)/100;
    temp_val = temp_val - _soc_sb2_mmu_params.mmu_pipeline_lat_cpap_admission;
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        general_info->ema_pool = temp_val;
    }

    /* C100 =IF($C$17,$C$47*1024*1024/$C$10-C48-C41,0) */
    if  (_soc_sb2_mmu_params.extbuf_used_d_c) {
        general_info->max_ext_cell_buff_size = 
            ((_soc_sb2_mmu_params.mmu_ext_buf_size*1024*1024)/
             (_soc_sb2_mmu_params.mmu_ext_buf_cell_size)) -
            _soc_sb2_mmu_params.mmu_reserved_ext_buf_space_cfap -
            _soc_sb2_mmu_params.mmu_pipeline_lat_cpap_admission;
    } 
    /* C101 =C52*1024-C53*/
    general_info->repl_engine_work_queue_entries =
        _soc_sb2_mmu_params.mmu_repl_engine_work_queue_entries*1024 -
        _soc_sb2_mmu_params.mmu_resv_repl_engine_work_queue_entries;

    /* C103 = $C$10/($C$9+2) */
    general_info->ratio_of_ext_buff_to_int_buff_size = 
        _soc_sb2_mmu_params.mmu_ext_buf_cell_size /
        (_soc_sb2_mmu_params.mmu_int_buf_cell_size + 2);

    /* C104 = 1 */
    general_info->int_buff_cells_per_avg_size_pkt = 1;

    /* C105 = 1 */
    general_info->ext_buff_cells_per_avg_size_pkt = 1;

    /* C106 = 0.75 but normalized to 1 */
    general_info->max_prop_of_buff_used_by_one_queue_port = 75;


    /* Fill up input threshold info */
    /* C110 =CEILING(C7*2/C9, 1)  */
    input_port_threshold->global_hdrm_cells_for_int_buff_ingress_pool = sal_ceil_func
        (_soc_sb2_mmu_params.mmu_max_pkt_size * 2 ,
         _soc_sb2_mmu_params.mmu_int_buf_cell_size);

    /* C111  =IF(C17=1, C110, 0) */
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        input_port_threshold->global_hdrm_cells_for_int_buff_ema_pool
            = input_port_threshold->global_hdrm_cells_for_int_buff_ingress_pool ;
    }
    /* C112 =CEILING(IF($C$17, IF(C12=1, 0, $C$110/$C$103), 0), 1) */
    if ( _soc_sb2_mmu_params.extbuf_used_d_c &&
            (_soc_sb2_mmu_params.packing_mode_d_c == 0)) {
        input_port_threshold->global_hdrm_cells_for_ext_buff_pool = sal_ceil_func(
                input_port_threshold->global_hdrm_cells_for_int_buff_ingress_pool,
                general_info->ratio_of_ext_buff_to_int_buff_size);
    }
    /* C113 */
    input_port_threshold->global_hdrm_cells_for_RE_WQEs = 0 ;
    /* C114 */
    input_port_threshold->global_hdrm_cells_for_EQEs = 0 ;
    if (_soc_sb2_mmu_params.lossless_mode_d_c) {
        if (_soc_sb2_mmu_params.extbuf_used_d_c) {
            /* C115 =IF(C14=0, 0, IF(C17=0, 180, 76))*/
            input_port_threshold->hdrm_int_buff_cells_for_10G_PG = 76; 
            /* C116 =IF(C14=0,0,IF(C17=0,0,76))*/
            input_port_threshold->hdrm_ema_buff_cells_for_10G_PG = 76;
            /* C117 =IF(C14=0,0,IF(C17=0,0,67))*/
            input_port_threshold->hdrm_ext_buff_cells_for_10G_PG = 67;
            /* C118 =IF(C14=0, 0, IF(C17=0, 49, 21))*/
            input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG = 21;
        } else {
            /* C115*/
            input_port_threshold->hdrm_int_buff_cells_for_10G_PG = 180; 
            /* C116*/
            input_port_threshold->hdrm_ema_buff_cells_for_10G_PG = 0;
            /* C117*/
            input_port_threshold->hdrm_ext_buff_cells_for_10G_PG = 0;
            /* C118*/
            input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG = 49;
        }
        /* C119 =IF(C14=0, 0, C118*C51)*/
        input_port_threshold->hdrm_EQEs_pkts_for_10G_PG = 
            input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG *
            _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt ;
    } else {
        /* C115*/
        input_port_threshold->hdrm_int_buff_cells_for_10G_PG = 0; 
        /* C116*/
        input_port_threshold->hdrm_ema_buff_cells_for_10G_PG = 0;
        /* C117*/
        input_port_threshold->hdrm_ext_buff_cells_for_10G_PG = 0;
        /* C118*/
        input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG = 0;
        /* C119*/
        input_port_threshold->hdrm_EQEs_pkts_for_10G_PG = 0;
    } 

    /* C120 = =C25*C16*$C$115 */ 
    input_port_threshold->hdrm_int_buff_cells_for_10G_total_PG = 
        (_soc_sb2_mmu_params.num_hg_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_int_buff_cells_for_10G_PG);

    /* C121 = C25*C16*C116 */
    input_port_threshold->hdrm_ema_buff_cells_for_10G_total_PG = 
        (_soc_sb2_mmu_params.num_hg_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_ema_buff_cells_for_10G_PG);

    /* C122 = C25*C16*C117 */
    input_port_threshold->hdrm_ext_buff_cells_for_10G_total_PG = 
        (_soc_sb2_mmu_params.num_hg_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_ext_buff_cells_for_10G_PG);


    /* C123 = C25*C16*C118 */
    input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_total_PG = 
        (_soc_sb2_mmu_params.num_hg_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_RE_WQEs_pkts_for_10G_PG);

    /* C124 = C25*C16*C119 */
    input_port_threshold->hdrm_EQEs_pkts_for_10G_total_PG = 
        (_soc_sb2_mmu_params.num_hg_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_EQEs_pkts_for_10G_PG);

    if (_soc_sb2_mmu_params.lossless_mode_d_c) {
        if (_soc_sb2_mmu_params.extbuf_used_d_c) {
            /* C125 */
            input_port_threshold->hdrm_int_buff_cells_for_1G_PG = 26; 
            /* C126 */
            input_port_threshold->hdrm_ema_buff_cells_for_1G_PG = 26; 
            /* C127 */
            input_port_threshold->hdrm_ext_buff_cells_for_1G_PG =  23;
            /* C128*/
            input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG = 7; 
        } else {
            /* C125 */
            input_port_threshold->hdrm_int_buff_cells_for_1G_PG = 130; 
            /* C126 */
            input_port_threshold->hdrm_ema_buff_cells_for_1G_PG = 0; 
            /* C127 */
            input_port_threshold->hdrm_ext_buff_cells_for_1G_PG =  0;
            /* C128*/
            input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG = 35; 
        }
        /* C129 =IF(C14=0, 0, C128*C51) */
        input_port_threshold->hdrm_EQEs_pkts_for_1G_PG = 
            input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG * 
            _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt;
    } else {
        /* C125 */
        input_port_threshold->hdrm_int_buff_cells_for_1G_PG = 0; 
        /* C126 */
        input_port_threshold->hdrm_ema_buff_cells_for_1G_PG = 0; 
        /* C127 */
        input_port_threshold->hdrm_ext_buff_cells_for_1G_PG =  0;
        /* C128*/
        input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG = 0; 
        /* C129*/
        input_port_threshold->hdrm_EQEs_pkts_for_1G_PG = 0;
    }

    /* C130 =C19*C16*$C$125 */
    input_port_threshold->hdrm_int_buff_cells_for_1G_total_PG = 
        (_soc_sb2_mmu_params.num_ge_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_int_buff_cells_for_1G_PG);

    /* C131 = C19*C16*$C$126 */
    input_port_threshold->hdrm_ema_buff_cells_for_1G_total_PG = 
        (_soc_sb2_mmu_params.num_ge_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_ema_buff_cells_for_1G_PG);

    /* C132 = C19*C16*$C$127 */
    input_port_threshold->hdrm_ext_buff_cells_for_1G_total_PG = 
        (_soc_sb2_mmu_params.num_ge_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_ext_buff_cells_for_1G_PG);

    /* C133 = C19*C16*$C$128 */
    input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_total_PG = 
        (_soc_sb2_mmu_params.num_ge_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_RE_WQEs_pkts_for_1G_PG);

    /* C134 = C19*C16*$C$129 */
    input_port_threshold->hdrm_EQEs_pkts_for_1G_total_PG = 
        (_soc_sb2_mmu_params.num_ge_ports_d *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->hdrm_EQEs_pkts_for_1G_PG);

    /* C132-C135 values are zero for lossy mode */
    if (_soc_sb2_mmu_params.lossless_mode_d_c) {
        if (_soc_sb2_mmu_params.extbuf_used_d_c) {
            /* C135 */
            input_port_threshold->hdrm_int_buff_cells_for_olp_port = 76; 
            /* C136 */
            input_port_threshold->hdrm_ema_buff_cells_for_olp_port = 76; 
            /* C137 */
            input_port_threshold->hdrm_ext_buff_cells_for_olp_port = 67;
            /* C138 */
            input_port_threshold->hdrm_RE_WQEs_pkts_for_olp_port = 7;
        } else {
            /* C135 */
            input_port_threshold->hdrm_int_buff_cells_for_olp_port = 180; 
            /* C136 */
            input_port_threshold->hdrm_ema_buff_cells_for_olp_port = 0; 
            /* C137 */
            input_port_threshold->hdrm_ext_buff_cells_for_olp_port = 0;
            /* C138 */
            input_port_threshold->hdrm_RE_WQEs_pkts_for_olp_port = 35;
        }
        /* C139 = C138*C51 */
        input_port_threshold->hdrm_EQEs_pkts_for_olp_port = 
            input_port_threshold->hdrm_RE_WQEs_pkts_for_olp_port * 
            _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt;
    } else {
        /* C135 */
        input_port_threshold->hdrm_int_buff_cells_for_olp_port = 0; 
        /* C136 */
        input_port_threshold->hdrm_int_buff_cells_for_olp_port = 0; 
        /* C137 */
        input_port_threshold->hdrm_ext_buff_cells_for_olp_port = 0;
        /* C138 */
        input_port_threshold->hdrm_RE_WQEs_pkts_for_olp_port = 0;
        /* C139 */
        input_port_threshold->hdrm_EQEs_pkts_for_olp_port = 0;
    }

    /* C140 = 46 */
    input_port_threshold->hdrm_int_buff_cells_for_lpbk_port =  46;

    /* C141 = 46 for external buffer  */
    /* C142 = 40 for external buffer  */
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        input_port_threshold->hdrm_ema_buff_cells_for_lpbk_port = 46;
        input_port_threshold->hdrm_ext_buff_cells_for_lpbk_port = 40;
    }

    /* C143 = =IF(C14=0, 40, 18) */
    if (_soc_sb2_mmu_params.lossless_mode_d_c) { 
        input_port_threshold->hdrm_RE_WQEs_pkts_for_lpbk_port =  18;
    } else {
        input_port_threshold->hdrm_RE_WQEs_pkts_for_lpbk_port =  40;
    }
    /* C144 = C143*C51 */
    input_port_threshold->hdrm_EQEs_pkts_for_lpbk_port =  
        input_port_threshold->hdrm_RE_WQEs_pkts_for_lpbk_port *
        _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt;

    /* C145 = 46 */
    input_port_threshold->hdrm_int_buff_cells_for_cpu_port =  46;
    /* C146 = =IF(C17=1,46,0) */
    /* C147 = 17=1,40,0) */
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        input_port_threshold->hdrm_ema_buff_cells_for_cpu_port =  46;
        input_port_threshold->hdrm_ext_buff_cells_for_cpu_port =  40;
    }

    /* C148 =IF(C14=0, 40, 18) */
    if (_soc_sb2_mmu_params.lossless_mode_d_c) { 
        input_port_threshold->hdrm_RE_WQEs_pkts_for_cpu_port = 18;
    } else {
        input_port_threshold->hdrm_RE_WQEs_pkts_for_cpu_port = 40;
    }

    /* C149 = C148*C51 */
    input_port_threshold->hdrm_EQEs_pkts_for_cpu_port =  
        input_port_threshold->hdrm_RE_WQEs_pkts_for_cpu_port *
        _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt;

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

    /* C152 =$C$112+$C$122+$C$132++C137+C142+C147*/
    input_port_threshold->total_hdrm_ext_buff_cells = 
        input_port_threshold->global_hdrm_cells_for_ext_buff_pool+
        input_port_threshold->hdrm_ext_buff_cells_for_10G_total_PG+
        input_port_threshold->hdrm_ext_buff_cells_for_1G_total_PG+
        input_port_threshold->hdrm_ext_buff_cells_for_olp_port+
        input_port_threshold->hdrm_ext_buff_cells_for_lpbk_port+
        input_port_threshold->hdrm_ext_buff_cells_for_cpu_port;

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

    /* In lossy mode allow configuration of PG min to zero */
    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        if (_soc_sb2_mmu_params.lossless_mode_d_c == 0) {
            PBMP_ALL_ITER(unit, port) {
                for (idx = 0; idx < SB2_MAX_PRIORITY_GROUPS; idx++) {
                     pg_min = soc_property_port_obj_attr_get
                              (unit, port, spn_BUF, spn_PRIGROUP,
                              idx, spn_GUARANTEE, 0, &suffix, -1);
                     if ((pg_min_configured == 0) && (pg_min != -1)) {
                         pg_min_configured = 1;
                     }
                     if (pg_min_configured && pg_min) {
                         /* Not allow non-zero pg min configuration */
                         LOG_ERROR(BSL_LS_BCM_COSQ,
                              (BSL_META_U(unit,
                              "PG MIN configuration %d for port %d and PG %d"
                              " is not allowed.\n"), pg_min, port, idx));
                         return SOC_E_CONFIG;
                     }
                }
            }
        }
    }
    pg_min = pg_min_configured ? pg_min : general_info->jumbo_frame_for_int_buff;
    input_port_threshold->min_int_buff_cells_per_PG =  pg_min;

    /* C157 = IF($C$17, $C$156, 0) */
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        input_port_threshold->min_int_buff_ema_pool_cells_per_PG = 
            input_port_threshold->min_int_buff_cells_per_PG;
    }

    /* C158 =IF( $C$17,C74, 0) */
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        input_port_threshold->min_ext_buff_cells_per_PG =
            general_info->jumbo_frame_for_ext_buff;
    }

    /* C159 */
    input_port_threshold->min_RE_WQEs_pkt_per_PG = 9; 

    /* C160 = =$C$156/$C$104*C51 */
    input_port_threshold->min_EQEs_pkt_per_PG = 
        (input_port_threshold->min_int_buff_cells_per_PG/
         general_info->int_buff_cells_per_avg_size_pkt) *
        _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt;

    /* C162 = =C79*C16*$C$156 */
    input_port_threshold->min_int_buff_cells_for_total_PG =
        (general_info->total_num_of_ports *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->min_int_buff_cells_per_PG) ;

    /* C163 = =C79*C16*$C$157 */
    input_port_threshold->min_int_buff_ema_pool_cells_for_total_PG =
        (general_info->total_num_of_ports *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->min_int_buff_ema_pool_cells_per_PG);

    /* C164 = =C79*C16*$C$158 */
    input_port_threshold->min_ext_buff_cells_for_total_PG =
        (general_info->total_num_of_ports *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->min_ext_buff_cells_per_PG);

    /* C165 = C75*C16*$C$159 */
    input_port_threshold->min_RE_WQEs_pkts_for_total_PG =
        (general_info->total_num_of_ports *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->min_RE_WQEs_pkt_per_PG) ;

    /* C166 = C75*C16*$C$160 */
    input_port_threshold->min_EQEs_pkts_for_total_PG =
        (general_info->total_num_of_ports *
         _soc_sb2_mmu_params.mmu_lossless_pg_num *
         input_port_threshold->min_EQEs_pkt_per_PG) ;

    /* C168 C169 C170 C171 C172= 0 */
    input_port_threshold->min_int_buff_cells_for_a_port = 0;
    input_port_threshold->min_int_buff_ema_pool_cells_for_a_port = 0;
    input_port_threshold->min_ext_buff_cells_for_a_port = 0;
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

    /* C176 = $C$75*$C$170 */
    input_port_threshold->min_ext_buff_cells_for_total_port = 
        (general_info->total_num_of_ports *
         input_port_threshold->min_ext_buff_cells_for_a_port);

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

    /* C182 = $C$164+$C$176 */
    input_port_threshold->total_min_ext_buff_cells = 
        input_port_threshold->min_ext_buff_cells_for_total_PG + 
        input_port_threshold->min_ext_buff_cells_for_total_port;

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
    if (_soc_sb2_mmu_params.lossless_mode_d_c == 0) { 
        output_port_threshold->min_grntd_res_queue_cells_int_buff = 
            _soc_sb2_mmu_params.per_cos_res_cells_for_int_buff_d;
    }

    /* C205 =IF(C17=1,IF(C14=0, C77, 0), 0) */
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        if (_soc_sb2_mmu_params.lossless_mode_d_c == 0) { 
            output_port_threshold->min_grntd_res_queue_cells_ext_buff = 
                general_info->ether_mtu_cells_for_ext_buff;
        }
    }

    /* C206 = =IF(C14=0, $C$58/$C$105, 0) */
    if (_soc_sb2_mmu_params.lossless_mode_d_c == 0) { 
        output_port_threshold->min_grntd_res_queue_cells_EQEs = 
            _soc_sb2_mmu_params.per_cos_res_cells_for_int_buff_d/
            general_info->ext_buff_cells_per_avg_size_pkt;
    }

    /* C207 =IF($C$14, 0, IF($C$17, $C$58, 0)) */
    if (_soc_sb2_mmu_params.lossless_mode_d_c == 0) { 
        if (_soc_sb2_mmu_params.extbuf_used_d_c) {
            output_port_threshold->min_grntd_res_EMA_queue_cells = 
                _soc_sb2_mmu_params.per_cos_res_cells_for_int_buff_d;
        }
    }

    /* C208 = C204 */
    output_port_threshold->min_grntd_res_RE_WQs_cells =
        output_port_threshold->min_grntd_res_queue_cells_int_buff;

    /* C209 = C204 */
    output_port_threshold->min_grntd_res_RE_WQs_queue_cells_for_int_buff =
        output_port_threshold->min_grntd_res_queue_cells_int_buff;

    /* C210 =IF($C$14, 0, IF($C$17,C205, 0)) */
    if (_soc_sb2_mmu_params.lossless_mode_d_c == 0) { 
        if (_soc_sb2_mmu_params.extbuf_used_d_c) {
            output_port_threshold->
                min_grntd_res_RE_WQs_queue_cells_for_ext_buff =
                general_info->ext_buff_cells_per_avg_size_pkt;
        }
    } 

    /* C211 = 4 */
    output_port_threshold->min_grntd_res_EP_redirect_queue_entry_cells = 4;


    /* C213 =C89*$C$204 */
    output_port_threshold->min_grntd_tot_res_queue_cells_int_buff =
        general_info->total_base_queue_int_buff *
        output_port_threshold->min_grntd_res_queue_cells_int_buff;

    /* C214  =C94*$C$205 */
    output_port_threshold->min_grntd_tot_res_queue_cells_ext_buff =
        general_info->total_egr_queues_in_device *
        output_port_threshold->min_grntd_res_queue_cells_ext_buff;

    /* C215  =C94*$C$206 */
    output_port_threshold->min_grntd_tot_res_queue_cells_EQEs =
        general_info->total_egr_queues_in_device *
        output_port_threshold->min_grntd_res_queue_cells_EQEs;

    /* C216 =$C$55*$C$207 */
    output_port_threshold->min_grntd_tot_res_EMA_queue_cells = 
        _soc_sb2_mmu_params.mmu_ema_queues *
        output_port_threshold->min_grntd_res_EMA_queue_cells;

    /* C217 =$C$54*$C$208 */
    output_port_threshold->min_grntd_tot_res_RE_WQs_cells = 
        _soc_sb2_mmu_params.mmu_repl_engine_work_queue_in_device *
        output_port_threshold->min_grntd_res_RE_WQs_cells;

    /* C218 =$C$54*$C$209 */
    output_port_threshold->min_grntd_tot_res_RE_WQs_queue_cells_for_int_buff = 
        _soc_sb2_mmu_params.mmu_repl_engine_work_queue_in_device *
        output_port_threshold->min_grntd_res_RE_WQs_queue_cells_for_int_buff;

    /* C219 =$C$54*$C$210 */
    output_port_threshold->min_grntd_tot_res_RE_WQs_queue_cells_for_ext_buff = 
        _soc_sb2_mmu_params.mmu_repl_engine_work_queue_in_device *
        output_port_threshold->min_grntd_res_RE_WQs_queue_cells_for_ext_buff;

    /* C220 =C211*(C36) */
    output_port_threshold->min_grntd_tot_res_EP_redirect_queue_entry_cells = 
        _soc_sb2_mmu_params.mmu_num_ep_redirection_queue *
        output_port_threshold->min_grntd_res_EP_redirect_queue_entry_cells;

    /* C186 =$C$98-$C$150-$C$180-IF(C14=0, 0, C213)-IF(C14=0, 0, C218) */ 

    input_port_threshold->total_shared_ing_buff_pool =
        general_info->int_buff_pool - 
        input_port_threshold->total_min_int_buff_cells -
        input_port_threshold->total_hdrm_int_buff_cells;
    if (_soc_sb2_mmu_params.lossless_mode_d_c) {
        input_port_threshold->total_shared_ing_buff_pool -=
            output_port_threshold->min_grntd_tot_res_queue_cells_int_buff;
        input_port_threshold->total_shared_ing_buff_pool -=
            output_port_threshold->min_grntd_tot_res_RE_WQs_queue_cells_for_int_buff;
    }

    /* C187 =IF(C17,$C$99-$C$181-$C$151-IF(C14=0,0,C216), 0) */
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        input_port_threshold->total_shared_EMA_buff =
            general_info->ema_pool - 
            input_port_threshold->total_min_int_buff_ema_pool_cells -
            input_port_threshold->total_hdrm_int_buff_ema_pool_cells;
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
            input_port_threshold->total_shared_EMA_buff -=
                output_port_threshold->min_grntd_tot_res_EMA_queue_cells;
        }
    }

    /* C188 =$C$100-$C$152-$C$182-IF(C14=0, 0, C214) - IF(C14=0, 0, C219)-IF(C12=0, 0, C56) */ 
    input_port_threshold->total_shared_ext_buff =
        general_info->max_ext_cell_buff_size - 
        input_port_threshold->total_hdrm_ext_buff_cells -
        input_port_threshold->total_min_ext_buff_cells;
    if (_soc_sb2_mmu_params.lossless_mode_d_c) {
        input_port_threshold->total_shared_ext_buff -=
            output_port_threshold->min_grntd_tot_res_queue_cells_ext_buff;
        input_port_threshold->total_shared_ext_buff -=
            output_port_threshold->min_grntd_tot_res_RE_WQs_queue_cells_for_ext_buff;
    }
    if (_soc_sb2_mmu_params.packing_mode_d_c) {
        input_port_threshold->total_shared_ext_buff -=
            _soc_sb2_mmu_params.num_cells_rsrvd_ing_ext_buf;
    }

    /* C189 =$C$101-$C$153-$C$183-IF(C14=0, C217) */
    input_port_threshold->total_shared_RE_WQEs_buff =
        general_info->repl_engine_work_queue_entries -
        input_port_threshold->total_hdrm_RE_WQEs_pkts -
        input_port_threshold->total_min_RE_WQEs_pkts;
    if (!_soc_sb2_mmu_params.lossless_mode_d_c) {
        input_port_threshold->total_shared_RE_WQEs_buff -=
            output_port_threshold->min_grntd_tot_res_RE_WQs_cells;
    }

    /* C190 =($C$49*1024)-$C$154-$C$184-IF(C14=0, 0, C215)-1 */ 
    input_port_threshold->total_shared_EQEs_buff =
        (_soc_sb2_mmu_params.mmu_egress_queue_entries * 1024) -
        input_port_threshold->total_hdrm_EQEs_pkts - 
        input_port_threshold->total_min_EQEs_pkts; 
    if (_soc_sb2_mmu_params.lossless_mode_d_c) {
        input_port_threshold->total_shared_EQEs_buff -=
            output_port_threshold->min_grntd_tot_res_queue_cells_EQEs;
    }
    input_port_threshold->total_shared_EQEs_buff -= 1;

    /* C192 =FLOOR(($C$60*$C$186)/(1+($C$60*1)),1) */
    input_port_threshold->ingress_burst_cells_size_for_one_port = sal_floor_func(
            (_soc_sb2_mmu_params.mmu_ing_port_dyn_thd_param * 
             input_port_threshold->total_shared_ing_buff_pool),
            (1+(_soc_sb2_mmu_params.mmu_ing_port_dyn_thd_param*1)));

    /* C193 =FLOOR(($C$60*$C$190)/(1+($C$60*1)),1) */
    input_port_threshold->ingress_burst_pkts_size_for_one_port = sal_floor_func(
            (_soc_sb2_mmu_params.mmu_ing_port_dyn_thd_param *
             input_port_threshold->total_shared_EQEs_buff),
            (1+(_soc_sb2_mmu_params.mmu_ing_port_dyn_thd_param*1)));

    /* C194 =FLOOR(($C$60*$C$186)/(1+($C$60*$C$79)),1) */
    input_port_threshold->ingress_burst_cells_size_for_all_ports = sal_floor_func(
            (_soc_sb2_mmu_params.mmu_ing_port_dyn_thd_param *
             input_port_threshold->total_shared_ing_buff_pool),
            (1+(_soc_sb2_mmu_params.mmu_ing_port_dyn_thd_param*
                general_info->total_num_of_ports)));

    /* C195 =($C$194+$C$168+C156)*$C$79 */
    input_port_threshold->ingress_total_shared_cells_use_for_all_port = 
        (input_port_threshold->ingress_burst_cells_size_for_all_ports + 
         input_port_threshold->min_int_buff_cells_for_a_port +
         input_port_threshold->min_int_buff_cells_per_PG) * 
        general_info->total_num_of_ports;

    /* C196 =FLOOR(($C$60*$C$190)/(1+($C$60*$C$79)),1) */
    input_port_threshold->ingress_burst_pkts_size_for_all_port = sal_floor_func(
            _soc_sb2_mmu_params.mmu_ing_port_dyn_thd_param *
            input_port_threshold->total_shared_EQEs_buff,
            (1+(_soc_sb2_mmu_params.mmu_ing_port_dyn_thd_param*
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

    /* C223 =$C$100-$C$214-C219 */
    output_port_threshold->min_grntd_tot_shr_queue_cells_ext_buff = 
        general_info->max_ext_cell_buff_size -
        output_port_threshold->min_grntd_tot_res_queue_cells_ext_buff;
    output_port_threshold->min_grntd_tot_shr_queue_cells_ext_buff -= 
        output_port_threshold->min_grntd_tot_res_RE_WQs_queue_cells_for_ext_buff;

    /* C224 =$C$49*1024-$C$215-1 */
    output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs =
        (_soc_sb2_mmu_params.mmu_egress_queue_entries*1024) -
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

    /* C228 =$C$100-$C$219-C214 */
    output_port_threshold->
        min_grntd_tot_shr_RE_WQs_queue_cells_for_ext_buff =
        general_info->max_ext_cell_buff_size -
        output_port_threshold->min_grntd_tot_res_RE_WQs_queue_cells_for_ext_buff -
        output_port_threshold->min_grntd_tot_res_queue_cells_ext_buff ; 

    /* C229 =C50-C220 */
    output_port_threshold->min_grntd_tot_shr_EP_redirect_queue_entry_cells =
        _soc_sb2_mmu_params.mmu_ep_redirect_queue_entries - 
        output_port_threshold->min_grntd_tot_res_EP_redirect_queue_entry_cells;

    /* C230 = 2 */
    output_port_threshold->egress_queue_dynamic_threshold_parameter = 2;

    /* C231 =FLOOR( ($C$62*$C$222)/(1+($C$62*1)), 1) */
    output_port_threshold->egress_burst_cells_size_for_one_queue = sal_floor_func(
            (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf * 
             output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff),
            (1 + (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf*1)));

    /* C232 =FLOOR( ($C$62*$C$224)/(1+($C$62*1)), 1) */
    output_port_threshold->egress_burst_pkts_size_for_one_queue = sal_floor_func(
            (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf *  
             output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs),
            (1 + (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf * 1)));

    /* C233 = =FLOOR( ($C$62*$C$222)/(1+($C$62*$C$79)), 1)*/
    output_port_threshold->egress_burst_cells_size_for_all_ports = sal_floor_func(
            (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf *  
             output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff),
            (1 + (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
                  general_info->total_num_of_ports)));

    /* C234 =FLOOR( ($C$62*$C$224)/(1+($C$62*$C$79)), 1) */
    output_port_threshold->egress_burst_pkts_size_for_all_ports = sal_floor_func(
            (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf *  
             output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs),
            (1 + (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
                  general_info->total_num_of_ports)));

    /* C235 =FLOOR( ($C$62*$C$222)/(1+($C$62*C93)), 1) */
    output_port_threshold->egress_burst_cells_size_for_all_queues = sal_floor_func(
            (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf *  
             output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff),
            (1 + (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
                  general_info->total_egr_base_queues_in_device)));

    /* C236 =FLOOR( ($C$62*$C$224)/(1+($C$62*C93)), 1) */
    output_port_threshold->egress_burst_pkts_size_for_all_queues = sal_floor_func(
            (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf *  
             output_port_threshold->min_grntd_tot_shr_queue_cells_EQEs),
            (1 + (_soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf *
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


    /* ===================== */
    /* Device Wide Registers */
    /* ===================== */
    if (flex_port == -1) {
        /* This is flex operation. so no need to re-configure */
	if (!_soc_sb2_mmu_params.extbuf_used_d_c) {
	    SOC_IF_ERROR_RETURN(READ_TOP_SW_BOND_OVRD_CTRL1r(unit, &rval));
	    soc_reg_field_set(unit, TOP_SW_BOND_OVRD_CTRL1r, &rval,
		    MMU_EXT_BUFFER_ENABLEf, 0);
	    SOC_IF_ERROR_RETURN(WRITE_TOP_SW_BOND_OVRD_CTRL1r(unit, rval));
	}

	/* C244: CFAPICONFIG.CFAPIPOOLSIZE =C42+22 */
	SOC_IF_ERROR_RETURN(READ_CFAPICONFIGr(unit, &rval));
	soc_reg_field_set(unit, CFAPICONFIGr, &rval, CFAPIPOOLSIZEf,
		_soc_sb2_mmu_params.mmu_int_buf_size + 22 );
	SOC_IF_ERROR_RETURN(WRITE_CFAPICONFIGr(unit, rval));


	/* C245: CFAPIFULLSETPOINT.CFAPIFULLSETPOINT = =C96 */
	SOC_IF_ERROR_RETURN(READ_CFAPIFULLSETPOINTr(unit, &rval));
	soc_reg_field_set(unit, CFAPIFULLSETPOINTr, &rval, CFAPIFULLSETPOINTf,
		general_info->int_cell_buff_size_after_limitation);
	SOC_IF_ERROR_RETURN(WRITE_CFAPIFULLSETPOINTr(unit, rval));

	/* C246: CFAPIFULLRESETPOINT.CFAPIFULLRESETPOINT =C244-128 */
	SOC_IF_ERROR_RETURN(READ_CFAPIFULLRESETPOINTr(unit, &rval));
	soc_reg_field_set(unit, CFAPIFULLRESETPOINTr, &rval, CFAPIFULLRESETPOINTf,
		_soc_sb2_mmu_params.mmu_int_buf_size + 22 - 128 );
	SOC_IF_ERROR_RETURN(WRITE_CFAPIFULLRESETPOINTr(unit, rval));

	/* C247: CFAPECONFIG.CFAPEPOOLSIZE =C47*1024*1024/C10-1*/
	SOC_IF_ERROR_RETURN(READ_CFAPECONFIGr(unit, &rval));
	soc_reg_field_set(unit, CFAPECONFIGr, &rval, CFAPEPOOLSIZEf,
		((_soc_sb2_mmu_params.mmu_ext_buf_size * 1024 * 1024)/
		 _soc_sb2_mmu_params.mmu_ext_buf_cell_size) -1);
	SOC_IF_ERROR_RETURN(WRITE_CFAPECONFIGr(unit, rval));
	temp_val = (((_soc_sb2_mmu_params.mmu_ext_buf_size * 1024 * 1024)/
		    _soc_sb2_mmu_params.mmu_ext_buf_cell_size) -1);

	/* C248: CFAPEFULLSETPOINT.CFAPEFULLSETPOINT =C247-C48 */
	SOC_IF_ERROR_RETURN(READ_CFAPEFULLSETPOINTr(unit, &rval));
	soc_reg_field_set(unit, CFAPEFULLSETPOINTr, &rval, CFAPEFULLSETPOINTf,
		temp_val - _soc_sb2_mmu_params.mmu_reserved_ext_buf_space_cfap); 
	SOC_IF_ERROR_RETURN(WRITE_CFAPEFULLSETPOINTr(unit, rval));

	/* C249: CFAPEFULLRESETPOINT.CFAPEFULLRESETPOINT =C247-128 */
	SOC_IF_ERROR_RETURN(READ_CFAPEFULLRESETPOINTr(unit, &rval));
	soc_reg_field_set(unit, CFAPEFULLRESETPOINTr, &rval, CFAPEFULLRESETPOINTf,
		temp_val - 128 );
	SOC_IF_ERROR_RETURN(WRITE_CFAPEFULLRESETPOINTr(unit, rval));

	/* C250: MMU_ENQ_FAPCONFIG_0.FAPPOOLSIZE =C52*1024-1*/
	SOC_IF_ERROR_RETURN(READ_MMU_ENQ_FAPCONFIG_0r(unit, &rval));
	soc_reg_field_set(unit, MMU_ENQ_FAPCONFIG_0r, &rval, FAPPOOLSIZEf,
		(_soc_sb2_mmu_params.mmu_repl_engine_work_queue_entries*1024 - 1 )); 
	SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_FAPCONFIG_0r(unit, rval));
	temp_val = _soc_sb2_mmu_params.mmu_repl_engine_work_queue_entries*1024 - 1 ;

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

	/* C253: QSTRUCT_FAPCONFIG.FAPPOOLSIZE =(C49*1024+1024*6)/4 */
	temp_val = (((_soc_sb2_mmu_params.mmu_egress_queue_entries * 1024 ) + (1024 *6))/4); 
	for (idx =0 ; idx < 1; idx++) {
	    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPCONFIGr(unit, idx, &rval));
	    soc_reg_field_set(unit, QSTRUCT_FAPCONFIGr, &rval, FAPPOOLSIZEf,
		    temp_val);
	    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPCONFIGr(unit, idx, rval));
	}

	/* C254 QSTRUCT_FAPFULLSETPOINT.FAPFULLSETPOINT */
	for (idx =0 ; idx < 1; idx++) {
	    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPFULLSETPOINTr(unit, idx, &rval));
	    soc_reg_field_set(unit, QSTRUCT_FAPFULLSETPOINTr, &rval, FAPFULLSETPOINTf,
		    17912); 
	    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLSETPOINTr(unit, idx, rval));
	}

	/* C255 QSTRUCT_FAPFULLRESETPOINT.FAPFULLRESETPOINT */
	for (idx =0 ; idx < 1; idx++) {
	    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPFULLRESETPOINTr(unit, idx, &rval));
	    soc_reg_field_set(unit, QSTRUCT_FAPFULLRESETPOINTr, &rval, 
		    FAPFULLRESETPOINTf,
		    17896 );
	    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLRESETPOINTr(unit, idx, rval));
	}

	/* C258: TOP_SW_BOND_OVRD_CTRL1.MMU_PACKING_ENABLE = C12 */
	SOC_IF_ERROR_RETURN(READ_TOP_SW_BOND_OVRD_CTRL1r(unit, &rval));
	soc_reg_field_set(unit, TOP_SW_BOND_OVRD_CTRL1r, &rval, MMU_PACKING_ENABLEf, 
		_soc_sb2_mmu_params.packing_mode_d_c);
	SOC_IF_ERROR_RETURN(WRITE_TOP_SW_BOND_OVRD_CTRL1r(unit, rval));

	/* C259: MMU_ENQ_PACKING_REAS_FIFO_PROFILE_THRESH.THRESH_PROFILE_[0..3] = C13 */
	SOC_IF_ERROR_RETURN(READ_MMU_ENQ_PACKING_REAS_FIFO_PROFILE_THRESHr(unit, &rval));
	soc_reg_field_set(unit, MMU_ENQ_PACKING_REAS_FIFO_PROFILE_THRESHr, &rval, THRESH_PROFILE_0f,
		_soc_sb2_mmu_params.max_pkt_size_support_packing);
	soc_reg_field_set(unit, MMU_ENQ_PACKING_REAS_FIFO_PROFILE_THRESHr, &rval, THRESH_PROFILE_1f, 
		_soc_sb2_mmu_params.max_pkt_size_support_packing);
	soc_reg_field_set(unit, MMU_ENQ_PACKING_REAS_FIFO_PROFILE_THRESHr, &rval, THRESH_PROFILE_2f,
		_soc_sb2_mmu_params.max_pkt_size_support_packing);
	soc_reg_field_set(unit, MMU_ENQ_PACKING_REAS_FIFO_PROFILE_THRESHr, &rval, THRESH_PROFILE_3f, 
		_soc_sb2_mmu_params.max_pkt_size_support_packing);
	SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PACKING_REAS_FIFO_PROFILE_THRESHr(unit, rval));

	/* C260: MMU_ENQ_PACKING_REAS_FIFO_THRESH_PROFILE_[0/1/2].SRCPORT_[0..41] = 0 */
	rval = 0;
	SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PACKING_REAS_FIFO_THRESH_PROFILE_0r(unit, rval));
	SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PACKING_REAS_FIFO_THRESH_PROFILE_1r(unit, rval));

	/* C261: THDO_MISCCONFIG.UCMC_SEPARATION = 0 */
	SOC_IF_ERROR_RETURN(READ_THDO_MISCCONFIGr(unit, &rval));
	soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, UCMC_SEPARATIONf, 0);
	SOC_IF_ERROR_RETURN(WRITE_THDO_MISCCONFIGr(unit, rval));

	/* C262: WRED_MISCCONFIG.UCMC_SEPARATION = 0 */
	SOC_IF_ERROR_RETURN(READ_WRED_MISCCONFIGr(unit, &rval));
	soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, UCMC_SEPARATIONf, 0);
	SOC_IF_ERROR_RETURN(WRITE_WRED_MISCCONFIGr(unit, rval));

	/* C263: MISCCONFIG.THDI_ROLLOVER_COUNT = 0xB4 */
	SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
	soc_reg_field_set(unit, MISCCONFIGr, &rval, THDI_ROLLOVER_COUNTf, 0xb4);
	SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

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

	/* External Buffer */
	if (_soc_sb2_mmu_params.extbuf_used_d_c) {
	    /* C272: THDIEXT_GLOBAL_HDRM_LIMIT.GLOBAL_HDRM_LIMIT 
	       =IF(C17=0, "NA", C112)*/
	    SOC_IF_ERROR_RETURN(READ_THDIEXT_GLOBAL_HDRM_LIMITr (unit, &rval));
	    soc_reg_field_set(unit, THDIEXT_GLOBAL_HDRM_LIMITr, &rval,
		    GLOBAL_HDRM_LIMITf, 
		    input_port_threshold->global_hdrm_cells_for_ext_buff_pool);
	    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_GLOBAL_HDRM_LIMITr (unit, rval));

	    /* C273:  THDIEXT_BUFFER_CELL_LIMIT_SP.LIMIT 
	       =IF(C17=0, "NA", C188)*/
	    SOC_IF_ERROR_RETURN(READ_THDIEXT_BUFFER_CELL_LIMIT_SPr (
			unit, service_pool, &rval));
	    soc_reg_field_set(unit, THDIEXT_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
		    input_port_threshold->total_shared_ext_buff);
	    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_BUFFER_CELL_LIMIT_SPr (unit, 0,rval));

	    /* C274:THDIEXT_CELL_RESET_LIMIT_OFFSET_SP.OFFSET
	       =IF(C17=0, "NA", CEILING(C82/4, 1)*C77)*/
	    SOC_IF_ERROR_RETURN(READ_THDIEXT_CELL_RESET_LIMIT_OFFSET_SPr (
			unit, service_pool, &rval));
	    temp_val = sal_ceil_func(general_info->
		    total_num_of_ports_excl_lpbk_olp_cpu,
		    4) *  general_info->ether_mtu_cells_for_ext_buff;
	    soc_reg_field_set(unit, 
		    THDIEXT_CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf, temp_val);
	    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_CELL_RESET_LIMIT_OFFSET_SPr (
			unit, service_pool, rval));


	    /* EMA pool */
	    /* C276: THDIEMA_GLOBAL_HDRM_LIMIT.GLOBAL_HDRM_LIMIT
	       =IF(C17=0, "NA", C111)*/
	    SOC_IF_ERROR_RETURN(READ_THDIEMA_GLOBAL_HDRM_LIMITr (unit, &rval));
	    soc_reg_field_set(unit, THDIEMA_GLOBAL_HDRM_LIMITr, &rval,
		    GLOBAL_HDRM_LIMITf,
		    input_port_threshold->global_hdrm_cells_for_int_buff_ema_pool);
	    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_GLOBAL_HDRM_LIMITr (unit, rval));

	    /* C277:  THDIEMA_BUFFER_CELL_LIMIT_SP.LIMIT =IF(C17=0, "NA", C187) */
	    SOC_IF_ERROR_RETURN(READ_THDIEMA_BUFFER_CELL_LIMIT_SPr (
			unit, service_pool, &rval));
	    soc_reg_field_set(unit, THDIEMA_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
		    input_port_threshold->total_shared_EMA_buff);
	    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_BUFFER_CELL_LIMIT_SPr (unit, 0,rval));

	    /* C278:THDIEMA_CELL_RESET_LIMIT_OFFSET_SP.OFFSET=
	       =IF(C17=0, "NA", CEILING(C82/4, 1)*C76)*/
	    SOC_IF_ERROR_RETURN(READ_THDIEMA_CELL_RESET_LIMIT_OFFSET_SPr (
			unit, service_pool, &rval));
	    temp_val = sal_ceil_func(general_info->total_num_of_ports_excl_lpbk_olp_cpu,4) *
		general_info->ether_mtu_cells_for_int_buff ;
	    soc_reg_field_set(
		    unit, THDIEMA_CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf,
		    temp_val);
	    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_CELL_RESET_LIMIT_OFFSET_SPr (
			unit, service_pool, rval));
	}


	/* C280: THDIRQE_GLOBAL_HDRM_LIMIT.GLOBAL_HDRM_LIMIT = C113 */
	SOC_IF_ERROR_RETURN(READ_THDIRQE_GLOBAL_HDRM_LIMITr (unit, &rval));
	soc_reg_field_set(unit, THDIRQE_GLOBAL_HDRM_LIMITr, &rval,
		GLOBAL_HDRM_LIMITf, 
		input_port_threshold->global_hdrm_cells_for_RE_WQEs);
	SOC_IF_ERROR_RETURN(WRITE_THDIRQE_GLOBAL_HDRM_LIMITr (unit, rval));

	/* C281:  THDIRQE_BUFFER_CELL_LIMIT_SP.LIMIT =$C$189  */
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
	    _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt;
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


	/* External Buffer */
	if (_soc_sb2_mmu_params.extbuf_used_d_c) {
	    /* C296: 
	       OP_BUFFER_SHARED_LIMIT_CELLE.OP_BUFFER_SHARED_LIMIT_CELLE 
	       =IF(C17=0, "NA", C223)*/
	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r (
			unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r, &rval,
		    OP_BUFFER_SHARED_LIMIT_CELLEf, 
		    output_port_threshold->min_grntd_tot_shr_queue_cells_ext_buff);
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r (
			unit, rval));
	    temp_val = output_port_threshold->min_grntd_tot_shr_queue_cells_ext_buff;

	    /* C298: OP_BUFFER_LIMIT_YELLOW_CELLE.OP_BUFFER_LIMIT_YELLOW_CELLE =
	       =CEILING(C296/8, 1) */
	    /* C300: OP_BUFFER_LIMIT_RED_CELLE.OP_BUFFER_LIMIT_RED_CELLE =
	       =CEILING(C296/8, 1) */
	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_CELLE_POOL0r (
			unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLE_POOL0r, &rval,
		    OP_BUFFER_LIMIT_YELLOW_CELLEf, sal_ceil_func(temp_val,8));
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLE_POOL0r (
			unit, rval));

	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_CELLE_POOL0r(unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLE_POOL0r, &rval,
		    OP_BUFFER_LIMIT_RED_CELLEf, sal_ceil_func( temp_val,8));
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLE_POOL0r(unit, rval));

	    /* C297: OP_BUFFER_SHARED_LIMIT_RESUME_CELLE.
	       OP_BUFFER_SHARED_LIMIT_RESUME_CELLE 
	       =IF(C17=0, "NA", C296-CEILING(C82/4, 1)*C77)*/
	    temp_val = ((temp_val - 
		    (sal_ceil_func(general_info->total_num_of_ports_excl_lpbk_olp_cpu ,4))) *
		    general_info->ether_mtu_cells_for_ext_buff);
	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r(
			unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r, 
		    &rval,OP_BUFFER_SHARED_LIMIT_RESUME_CELLEf, temp_val);
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r(
			unit, rval));

	    /* C299: OP_BUFFER_LIMIT_RESUME_YELLOW_CELLE.
	       OP_BUFFER_LIMIT_RESUME_YELLOW_CELLE ==CEILING(C297/8, 1) */
	    /* C301: OP_BUFFER_LIMIT_RESUME_RED_CELLE.
	       OP_BUFFER_LIMIT_RESUME_RED_CELLE ==CEILING(C297/8, 1) */
	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLE_POOL0r (
			unit, &rval));
	    soc_reg_field_set(unit, 
		    OP_BUFFER_LIMIT_RESUME_YELLOW_CELLE_POOL0r, &rval,
		    OP_BUFFER_LIMIT_RESUME_YELLOW_CELLEf, sal_ceil_func(temp_val,8));
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLE_POOL0r (
			unit, rval));

	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_CELLE_POOL0r (
			unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_CELLE_POOL0r, &rval,
		    OP_BUFFER_LIMIT_RESUME_RED_CELLEf,sal_ceil_func(temp_val,8));
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_CELLE_POOL0r (
			unit, rval));

	    /* EMA pool */
	    /* C303: 
	       OP_BUFFER_SHARED_LIMIT_THDOEMA.OP_BUFFER_SHARED_LIMIT_CELLE 
	       =IF(C17=0, "NA", C225)*/
	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_THDOEMAr (unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_THDOEMAr, &rval,
		    OP_BUFFER_SHARED_LIMITf, 
		    output_port_threshold->min_grntd_tot_shr_EMA_queue_cells);
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_THDOEMAr (unit, rval));
	    temp_val = output_port_threshold->
		min_grntd_tot_shr_EMA_queue_cells;

	    /* C305: OP_BUFFER_LIMIT_YELLOW_THDOEMA.OP_BUFFER_LIMIT_YELLOW =
	       =CEILING(C303/8, 1) */
	    /* C307: OP_BUFFER_LIMIT_RED_THDOEMA.OP_BUFFER_LIMIT_RED =
	       =CEILING(C303/8, 1) */
	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_THDOEMAr (unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_THDOEMAr, &rval,
		    OP_BUFFER_LIMIT_YELLOWf,sal_ceil_func(temp_val,8));
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_THDOEMAr (unit, rval));

	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_THDOEMAr (unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_THDOEMAr, &rval,
		    OP_BUFFER_LIMIT_REDf,sal_ceil_func( temp_val,8));
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_THDOEMAr (unit, rval));



	    /* C304: OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMA.
	       OP_BUFFER_SHARED_LIMIT_RESUME_CELLE
	       =IF(C17=0, "NA", C303-CEILING(C82/4, 1)*C76) */
	    temp_val = (temp_val - (sal_ceil_func(general_info->
			    total_num_of_ports_excl_lpbk_olp_cpu,4)) *
		    general_info->ether_mtu_cells_for_int_buff);
	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr(
			unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr, &rval,
		    OP_BUFFER_SHARED_LIMIT_RESUMEf, temp_val);
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr(
			unit, rval));


	    /* C306: OP_BUFFER_LIMIT_RESUME_YELLOW_THDOEMA.
	       OP_BUFFER_LIMIT_RESUME_YELLOW==CEILING(C289/8, 1) */
	    /* C308: OP_BUFFER_LIMIT_RESUME_RED_THDOEMA.
	       OP_BUFFER_LIMIT_RESUME_RED==CEILING(C289/8, 1) */
	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_THDOEMAr (
			unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_THDOEMAr, &rval,
		    OP_BUFFER_LIMIT_RESUME_YELLOWf,sal_ceil_func(temp_val,8));
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_THDOEMAr (
			unit, rval));

	    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_THDOEMAr (
			unit, &rval));
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_THDOEMAr, &rval,
		    OP_BUFFER_LIMIT_RESUME_REDf,sal_ceil_func(temp_val,8));
	    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_THDOEMAr (
			unit, rval));
	}

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
	if (_soc_sb2_mmu_params.lossless_mode_d_c) {
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_QENTRYr, &rval,
		    OP_BUFFER_LIMIT_YELLOW_QENTRYf, sal_ceil_func(temp_val,8) - 1);
	} else {
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_QENTRYr, &rval,
		    OP_BUFFER_LIMIT_YELLOW_QENTRYf, sal_ceil_func(temp_val,8));
	}
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_QENTRYr(unit, rval));

	/* C321: OP_BUFFER_LIMIT_RED_QENTRY.OP_BUFFER_LIMIT_RED_QENTRY
	   = CEILING(C317/8, 1)  */
	SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_QENTRYr(unit, &rval));
	if (_soc_sb2_mmu_params.lossless_mode_d_c) {
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_QENTRYr, &rval,
		    OP_BUFFER_LIMIT_RED_QENTRYf, sal_ceil_func(temp_val,8) - 1);
	} else {
	    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_QENTRYr, &rval,
		    OP_BUFFER_LIMIT_RED_QENTRYf, sal_ceil_func(temp_val,8));
	}
	SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_QENTRYr(unit, rval));

	/* C318: OP_BUFFER_SHARED_LIMIT_RESUME_QENTRY.OP_BUFFER_SHARED_LIMIT_RESUME_QENTRY
	   =C224-CEILING(C82/4, 1)*C51 */
	temp_val -= (sal_ceil_func(general_info->total_num_of_ports_excl_lpbk_olp_cpu,4) * 
		_soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt);
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
        /* 10-12, 14-16,18-20  ports have 1 PG */
        if ((( port > 9) && (port < 13)) ||
                (( port > 13) && (port < 17)) ||
                (( port > 17) && (port < 21))) {
            priority_group = 0;
        } else {
            priority_group = 7;
        }
        if ((flex_port != -1) && (flex_port != port)) {
            continue;
        }
        sal_memset(&thdi_port_sp_config,0,sizeof(thdi_port_sp_config));
        mem_idx= (port * SB2_MAX_SERVICE_POOLS) + service_pool;
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
        mem_idx= (port * SB2_MAX_PRIORITY_GROUPS) + priority_group;

        /* C339: THDI_PORT_PG_CONFIG.PG_MIN_LIMIT (PG0) = =$C$156 */
        soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm, 
                &thdi_port_pg_config, PG_MIN_LIMITf, 
                input_port_threshold->min_int_buff_cells_per_PG);

        /* C340: THDI_PORT_PG_CONFIG.PG_SHARED_LIMIT
           =IF($C$14, $C$61, $C$269)*/
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
            temp_val = _soc_sb2_mmu_params.mmu_ing_pg_dyn_thd_param;
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
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
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
        /* C348 PORT_PG_SPID.PG[0..7]_SPID = 0 */

        if (_soc_sb2_mmu_params.extbuf_used_d_c) {
            sal_memset(&thdiext_thdi_port_sp_config,0,
                    sizeof(thdiext_thdi_port_sp_config));
            mem_idx= (port * SB2_MAX_SERVICE_POOLS) + service_pool;
            /* C350: THDIEXT_THDI_PORT_SP_CONFIG.PORT_SP_MIN_LIMIT  
               =IF(C17=0, "NA",C170) */
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_SP_CONFIGm,
                    &thdiext_thdi_port_sp_config, PORT_SP_MIN_LIMITf,
                    input_port_threshold->min_ext_buff_cells_for_a_port);

            /* C351: THDIEXT_THDI_PORT_SP_CONFIG.PORT_SP_MAX_LIMIT = 
               =IF(C17=0, "NA", C188)  */
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_SP_CONFIGm,
                    &thdiext_thdi_port_sp_config, PORT_SP_MAX_LIMITf,
                    input_port_threshold->total_shared_ext_buff);

            /* C352: THDIEXT_PORT_MAX_PKT_SIZE.PORT_MAX_PKT_SIZE 
               =IF(C17=0, "NA", CEILING(C72/C103, 1)) */
            SOC_IF_ERROR_RETURN(READ_THDIEXT_PORT_MAX_PKT_SIZEr (
                        unit, port, &rval));
            soc_reg_field_set(unit, THDIEXT_PORT_MAX_PKT_SIZEr, &rval,
                    PORT_MAX_PKT_SIZEf,
                    sal_ceil_func(general_info->max_packet_size_in_cells,
                        general_info->ratio_of_ext_buff_to_int_buff_size));
            SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_MAX_PKT_SIZEr (
                        unit, port, rval));

            /* C353: THDIEXT_THDI_PORT_SP_CONFIG.PORT_SP_RESUME_LIMIT =
               =IF(C17=0, "NA", C351-2*$C$77) */
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_SP_CONFIGm,
                    &thdiext_thdi_port_sp_config, PORT_SP_RESUME_LIMITf,
                    input_port_threshold->total_shared_ext_buff -
                    (2 *  general_info->ether_mtu_cells_for_ext_buff));
            SOC_IF_ERROR_RETURN (soc_mem_write(unit, 
                        THDIEXT_THDI_PORT_SP_CONFIGm, MEM_BLOCK_ALL, 
                        mem_idx, &thdiext_thdi_port_sp_config));
            sal_memset(&thdiext_thdi_port_pg_config,0,
                    sizeof(thdiext_thdi_port_pg_config));
            mem_idx= (port * SB2_MAX_PRIORITY_GROUPS) + priority_group;
            /* C354: THDIEXT_THDI_PORT_PG_CONFIG.PG_MIN_LIMIT(PG0)
               =IF(C17=0, "NA", C158) */
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm,
                    &thdiext_thdi_port_pg_config, PG_MIN_LIMITf,
                    input_port_threshold->min_ext_buff_cells_per_PG);

            /* C355: THDIEXT_THDI_PORT_PG_CONFIG.PG_SHARED_LIMIT =
               =IF(C17=0, "NA", IF($C$14, $C$61, $C$188)) */
            if (_soc_sb2_mmu_params.lossless_mode_d_c) {
                temp_val = _soc_sb2_mmu_params.mmu_ing_pg_dyn_thd_param;
            } else {
                temp_val = input_port_threshold->total_shared_ext_buff;;
            }
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm,
                    &thdiext_thdi_port_pg_config, 
                    PG_SHARED_LIMITf, temp_val);

            /* C356: THDIEXT_THDI_PORT_PG_CONFIG.PG_RESET_OFFSET 
               =IF(C17=0, "NA", 2*C77)*/
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm,
                    &thdiext_thdi_port_pg_config, PG_RESET_OFFSETf,
                    (2 * general_info->ether_mtu_cells_for_ext_buff));

            /* C357: THDI_PORT_PG_CONFIG.PG_RESET_FLOOR = 0 */
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm,
                    &thdiext_thdi_port_pg_config, PG_RESET_FLOORf, 0);

            /* C358: THDIEXT_THDI_PORT_PG_CONFIG.PG_SHARED_DYNAMIC= 
               IF($C$14, 1, 0) */
            if (_soc_sb2_mmu_params.lossless_mode_d_c) {
                temp_val = 1;
            } else {
                temp_val = 0;
            }
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm,
                    &thdiext_thdi_port_pg_config, PG_SHARED_DYNAMICf,
                    temp_val);

            /* C359: THDIEXT_THDI_PORT_PG_CONFIG.PG_HDRM_LIMIT
               For Ge:$C127 For Hg:$C117 For Cpu:$C147 */
            if (IS_CPU_PORT(unit, port)) {
                temp_val = input_port_threshold->
                    hdrm_ext_buff_cells_for_cpu_port;
            } else if (IS_GE_PORT(unit, port) || IS_LB_PORT(unit,port)) {
                temp_val = input_port_threshold->hdrm_ext_buff_cells_for_1G_PG;
            } else if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
                temp_val= input_port_threshold->hdrm_ext_buff_cells_for_10G_PG;
            }
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm,
                    &thdiext_thdi_port_pg_config, PG_HDRM_LIMITf,temp_val);

            /* C360: THDIEXT_THDI_PORT_PG_CONFIG.PG_GBL_HDRM_EN = 
               for lossy=1 else 0
               For Packed : 0
               =IF(C17=0, "NA", IF(C12=0, 1, 0))*/
            if (_soc_sb2_mmu_params.packing_mode_d_c) { 
                temp_val = 0;
            } else {
                temp_val = 1;
            }
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm, 
                    &thdiext_thdi_port_pg_config, PG_GBL_HDRM_ENf ,
                    temp_val);
            /* C361: THDIEXT_THDI_PORT_PG_CONFIG.SP_SHARED_MAX_ENABLE */
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm, 
                    &thdiext_thdi_port_pg_config, SP_SHARED_MAX_ENABLEf ,1);

            /* C362: THDIEXT_THDI_PORT_PG_CONFIG.SP_MIN_PG_ENABLE */
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm, 
                    &thdiext_thdi_port_pg_config, SP_MIN_PG_ENABLEf ,1);
            SOC_IF_ERROR_RETURN (soc_mem_write(
                        unit, THDIEXT_THDI_PORT_PG_CONFIGm, MEM_BLOCK_ALL, 
                        mem_idx, &thdiext_thdi_port_pg_config));
        } else {
            sal_memset(&thdiext_thdi_port_pg_config,0,
                    sizeof(thdiext_thdi_port_pg_config));
            mem_idx= (port * SB2_MAX_PRIORITY_GROUPS) + priority_group;


            /* C361: THDIEXT_THDI_PORT_PG_CONFIG.SP_SHARED_MAX_ENABLE */
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm, 
                    &thdiext_thdi_port_pg_config, SP_SHARED_MAX_ENABLEf ,1);

            /* C362: THDIEXT_THDI_PORT_PG_CONFIG.SP_MIN_PG_ENABLE */
            soc_mem_field32_set(unit, THDIEXT_THDI_PORT_PG_CONFIGm, 
                    &thdiext_thdi_port_pg_config, SP_MIN_PG_ENABLEf ,1);
            SOC_IF_ERROR_RETURN (soc_mem_write(
                        unit, THDIEXT_THDI_PORT_PG_CONFIGm, MEM_BLOCK_ALL, 
                        mem_idx, &thdiext_thdi_port_pg_config));
        }

        if (_soc_sb2_mmu_params.extbuf_used_d_c) {
            sal_memset(&thdiema_thdi_port_sp_config,0,
                    sizeof(thdiema_thdi_port_sp_config));
            mem_idx= (port * SB2_MAX_SERVICE_POOLS) + service_pool;
            /* C365: THDIEMA_THDI_PORT_SP_CONFIG.PORT_SP_MIN_LIMIT  
               =IF(C17=0, "NA", C169)*/
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_SP_CONFIGm,
                    &thdiema_thdi_port_sp_config, PORT_SP_MIN_LIMITf,
                    input_port_threshold->min_int_buff_ema_pool_cells_for_a_port);

            /* C366: THDIEMA_THDI_PORT_SP_CONFIG.PORT_SP_MAX_LIMIT = 
               =IF(C17=0, "NA", C187)  */
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_SP_CONFIGm,
                    &thdiema_thdi_port_sp_config, PORT_SP_MAX_LIMITf,
                    input_port_threshold->total_shared_EMA_buff);

            /* C367: THDIEMA_PORT_MAX_PKT_SIZE.PORT_MAX_PKT_SIZE 
               =IF(C17=0, "NA", $C$72)*/
            SOC_IF_ERROR_RETURN(READ_THDIEMA_PORT_MAX_PKT_SIZEr (
                        unit, port, &rval));
            soc_reg_field_set(unit, THDIEMA_PORT_MAX_PKT_SIZEr, &rval,
                    PORT_MAX_PKT_SIZEf,
                    general_info->max_packet_size_in_cells);
            SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_MAX_PKT_SIZEr (
                        unit, port, rval));
            /* C368: THDIEMA_THDI_PORT_SP_CONFIG.PORT_SP_RESUME_LIMIT =
               =IF(C17=0, "NA", C366-2*$C$76) */
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_SP_CONFIGm,
                    &thdiema_thdi_port_sp_config, PORT_SP_RESUME_LIMITf,
                    input_port_threshold->total_shared_EMA_buff -
                    (2 *  general_info->ether_mtu_cells_for_int_buff));
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, 
                        THDIEMA_THDI_PORT_SP_CONFIGm, MEM_BLOCK_ALL, 
                        mem_idx, &thdiema_thdi_port_sp_config));
            sal_memset(&thdiema_thdi_port_pg_config,0,
                    sizeof(thdiema_thdi_port_pg_config));
            mem_idx= (port * SB2_MAX_PRIORITY_GROUPS) + priority_group;

            /* C369: THDIEMA_THDI_PORT_PG_CONFIG.PG_MIN_LIMIT(PG0) 
               =IF(C17=0, "NA", C157) */
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm,
                    &thdiema_thdi_port_pg_config, PG_MIN_LIMITf,
                    input_port_threshold->min_int_buff_ema_pool_cells_per_PG);

            /* C370: THDIEMA_THDI_PORT_PG_CONFIG.PG_SHARED_LIMIT =
               =IF(C17=0, "NA", C187) */
            temp_val = input_port_threshold->total_shared_EMA_buff;
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm,
                    &thdiema_thdi_port_pg_config, 
                    PG_SHARED_LIMITf, temp_val);

            /* C371: THDIEMA_THDI_PORT_PG_CONFIG.PG_RESET_OFFSET 
               =IF(C17=0, "NA", 2*C76) */
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm,
                    &thdiema_thdi_port_pg_config, PG_RESET_OFFSETf,
                    (2 * general_info->ether_mtu_cells_for_int_buff));

            /* C372: THDIEMA_PORT_PG_CONFIG.PG_RESET_FLOOR 
               =IF(C17=0, "NA", 0)*/
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm,
                    &thdiema_thdi_port_pg_config, PG_RESET_FLOORf, 0);

            /* C373: THDIEMA_THDI_PORT_PG_CONFIG.PG_SHARED_DYNAMIC= 
               =IF(C17=0, "NA", IF($C$14, 1, 0)) */
            if (_soc_sb2_mmu_params.lossless_mode_d_c) {
                temp_val = 1;
            } else {
                temp_val = 0;
            }
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm,
                    &thdiema_thdi_port_pg_config, PG_SHARED_DYNAMICf,
                    temp_val);

            /* C374: THDIEMA_THDI_PORT_PG_CONFIG.PG_HDRM_LIMIT
               For Ge:$C126 For Hg:$C116 For Cpu:$C146 */
            if (IS_CPU_PORT(unit, port)) {
                temp_val = input_port_threshold->
                    hdrm_ema_buff_cells_for_cpu_port;
            } else if (IS_GE_PORT(unit, port) || IS_LB_PORT(unit,port)) {
                temp_val = input_port_threshold->hdrm_ema_buff_cells_for_1G_PG;
            } else if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit,port)) {
                temp_val= input_port_threshold->hdrm_ema_buff_cells_for_10G_PG;
            }
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm,
                    &thdiema_thdi_port_pg_config, PG_HDRM_LIMITf,temp_val);

            /* C375: THDIEMA_THDI_PORT_PG_CONFIG.PG_GBL_HDRM_EN = 
               for lossy=1 else 0 */
            temp_val = 1;
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm, 
                    &thdiema_thdi_port_pg_config, PG_GBL_HDRM_ENf ,
                    temp_val);
            /* C376: THDIEMA_THDI_PORT_PG_CONFIG.SP_SHARED_MAX_ENABLE */
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm, 
                    &thdiema_thdi_port_pg_config, SP_SHARED_MAX_ENABLEf ,1);

            /* C377: THDIEMA_THDI_PORT_PG_CONFIG.SP_MIN_PG_ENABLE */
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm, 
                    &thdiema_thdi_port_pg_config, SP_MIN_PG_ENABLEf ,1);
            SOC_IF_ERROR_RETURN (soc_mem_write(
                        unit, THDIEMA_THDI_PORT_PG_CONFIGm, MEM_BLOCK_ALL, 
                        mem_idx, &thdiema_thdi_port_pg_config));
            sal_memset(&thdirqe_thdi_port_sp_config , 0,
                    sizeof(thdirqe_thdi_port_sp_config));
        } else {
            sal_memset(&thdiema_thdi_port_pg_config,0,
                    sizeof(thdiema_thdi_port_pg_config));
            mem_idx= (port * SB2_MAX_PRIORITY_GROUPS) + priority_group;
            /* C376: THDIEMA_THDI_PORT_PG_CONFIG.SP_SHARED_MAX_ENABLE */
            sal_memset(&thdiema_thdi_port_pg_config,0,
                    sizeof(thdiema_thdi_port_pg_config));
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm, 
                    &thdiema_thdi_port_pg_config, SP_SHARED_MAX_ENABLEf ,1);

            /* C377: THDIEMA_THDI_PORT_PG_CONFIG.SP_MIN_PG_ENABLE */
            soc_mem_field32_set(unit, THDIEMA_THDI_PORT_PG_CONFIGm, 
                    &thdiema_thdi_port_pg_config, SP_MIN_PG_ENABLEf ,1);
            SOC_IF_ERROR_RETURN (soc_mem_write(
                        unit, THDIEMA_THDI_PORT_PG_CONFIGm, MEM_BLOCK_ALL, 
                        mem_idx, &thdiema_thdi_port_pg_config));
        }
        sal_memset(&thdirqe_thdi_port_sp_config , 0,
                sizeof(thdirqe_thdi_port_sp_config));

        mem_idx= (port * SB2_MAX_SERVICE_POOLS) + service_pool;
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
        mem_idx= (port * SB2_MAX_PRIORITY_GROUPS) + priority_group;
        /* C383: THDIRQE_THDI_PORT_PG_CONFIG.PG_MIN_LIMIT(PG0) = =$C$159 */
        temp_val = input_port_threshold->min_RE_WQEs_pkt_per_PG;
        soc_mem_field32_set(unit, THDIRQE_THDI_PORT_PG_CONFIGm,
                &thdirqe_thdi_port_pg_config, PG_MIN_LIMITf,temp_val);

        /* C384: THDIRQE_THDI_PORT_PG_CONFIG.PG_SHARED_LIMIT =
           =IF($C$14, $C$61, $C$189) */
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
            temp_val = _soc_sb2_mmu_params.mmu_ing_pg_dyn_thd_param;
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
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
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
        mem_idx= (port * SB2_MAX_SERVICE_POOLS) + service_pool;

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
                (2 * _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, 
                    THDIQEN_THDI_PORT_SP_CONFIGm, MEM_BLOCK_ALL, 
                    mem_idx, &thdiqen_thdi_port_sp_config));

        sal_memset(&thdiqen_thdi_port_pg_config,0,
                sizeof(thdiqen_thdi_port_pg_config));
        mem_idx= (port * SB2_MAX_PRIORITY_GROUPS) + priority_group;
        /* C397: THDIQEN_THDI_PORT_PG_CONFIG.PG_MIN_LIMIT(PG0) = 
           =$C$160 */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, PG_MIN_LIMITf,
                input_port_threshold->min_EQEs_pkt_per_PG);


        /* C398: THDIQEN_THDI_PORT_PG_CONFIG.PG_SHARED_LIMIT =
           =IF($C$14, $C$61, $C$190) */
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
            temp_val = _soc_sb2_mmu_params.mmu_ing_pg_dyn_thd_param;
        } else {
            temp_val = input_port_threshold->total_shared_EQEs_buff;
        }
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, 
                PG_SHARED_LIMITf, temp_val);

        /* C399: THDIQEN_THDI_PORT_PG_CONFIG.PG_RESET_OFFSET = =2*C51 */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, PG_RESET_OFFSETf,
                2 * _soc_sb2_mmu_params.mmu_exp_num_of_repl_per_pkt);

        /* C400: THDIQEN_THDI_PORT_PG_CONFIG.PG_RESET_FLOOR = 0 */
        soc_mem_field32_set(unit, THDIQEN_THDI_PORT_PG_CONFIGm,
                &thdiqen_thdi_port_pg_config, PG_RESET_FLOORf, 0);

        /* C401: THDIQEN_THDI_PORT_PG_CONFIG.PG_SHARED_DYNAMIC= 0 */

        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
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
                sal_memset(&thdiema_thdi_port_pg_config,0,
                        sizeof(thdiema_thdi_port_pg_config));
                SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY,op_node + op_node_offset,
                            &mmu_thdo_opnconfig_cell_entry));

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
                   = =$C$204 */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_MIN_CELLf,
                        output_port_threshold->min_grntd_res_queue_cells_int_buff);

                /* C414:THDO_QCONFIG_CELL.Q_SHARED_LIMIT_CELL
                   =IF($C$14,$C$222,$C$63) */
                if (_soc_sb2_mmu_params.lossless_mode_d_c) {
                    temp_val = output_port_threshold->
                        min_grntd_tot_shr_queue_cells_int_buff;
                } else {
                    temp_val = _soc_sb2_mmu_params.
                        mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
                }
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_SHARED_LIMIT_CELLf, 
                        temp_val);

                /* C416:THDO_QCONFIG_CELL.Q_LIMIT_DYNAMIC_CELL
                   =IF($C$14, 0, 1) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_LIMIT_DYNAMIC_CELLf, 
                        _soc_sb2_mmu_params.lossless_mode_d_c ? 0 : 1);

                /* C417:THDO_QCONFIG_CELL.Q_LIMIT_ENABLE_CELL
                   =IF($C$14, 0, 1) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_LIMIT_ENABLE_CELLf, 
                        _soc_sb2_mmu_params.lossless_mode_d_c ? 0 : 1);

                /* C418:THDO_QCONFIG_CELL.Q_COLOR_ENABLE_CELL = 0 */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_COLOR_ENABLE_CELLf,0); 

                /* C419: THDO_QCONFIG_CELL.Q_COLOR_LIMIT_DYNAMIC_CELL
                   =IF($C$14, 0, 1) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_COLOR_LIMIT_DYNAMIC_CELLf,
                        _soc_sb2_mmu_params.lossless_mode_d_c ? 0 : 1);

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
        if (!IS_CPU_PORT(unit, port) &&
                (port > si->cmic_port) && (port <= si->lb_port) &&
                SOC_PBMP_MEMBER(PBMP_EXT_MEM (unit), port)) {

            op_node = ( si->port_cosq_base[port] / 8) ;

            SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_CELLm(unit,
                        MEM_BLOCK_ANY,op_node , &mmu_thdo_opnconfig_cell_entry));
            /* C425:THDO_OPNCONFIG_CELL.OPN_SHARED_LIMIT_CELL = =$C$223 */
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, 
                    &mmu_thdo_opnconfig_cell_entry, 
                    OPN_SHARED_LIMIT_CELLf,
                    output_port_threshold->
                    min_grntd_tot_shr_queue_cells_ext_buff);

            /* C426:THDO_OPNCONFIG_CELL.OP_SHARED_RESET_VALUE_CELL
               = =IF(C17=0, "NA", C425-2*C77) */
            temp_val = output_port_threshold->
                min_grntd_tot_shr_queue_cells_ext_buff -
                (2 * general_info->ether_mtu_cells_for_ext_buff);
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, 
                    &mmu_thdo_opnconfig_cell_entry, 
                    OPN_SHARED_RESET_VALUE_CELLf, temp_val);

            /* C427:THDO_OPNCONFIG_CELL.PORT_LIMIT_ENABLE_CELL = 0 */
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, 
                    &mmu_thdo_opnconfig_cell_entry, 
                    PORT_LIMIT_ENABLE_CELLf, 0);
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, 
                    &mmu_thdo_opnconfig_cell_entry, 
                    PIDf, port);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_CELLm(unit,
                        MEM_BLOCK_ANY,op_node,
                        &mmu_thdo_opnconfig_cell_entry));

            for (queue = si->port_uc_cosq_base[port];
                    queue < si->port_uc_cosq_base[port] + 
                    si->port_num_uc_cosq[port];
                    queue++) {
                SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY,queue,
                            &mmu_thdo_qconfig_cell_entry));
                /* C428:THDO_QCONFIG_CELL.Q_MIN_CELL
                   =IF(C17=0, "NA", C205) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_MIN_CELLf,
                        output_port_threshold->min_grntd_res_queue_cells_ext_buff);

                /* C429:THDO_QCONFIG_CELL.Q_SHARED_LIMIT_CELL
                   =IF(C17=0, "NA", IF($C$14,$C$223,$C$63)) */
                if (_soc_sb2_mmu_params.lossless_mode_d_c) {
                    temp_val = output_port_threshold->
                        min_grntd_tot_shr_queue_cells_ext_buff;
                } else {
                    temp_val = 
                        _soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
                }
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_SHARED_LIMIT_CELLf, 
                        temp_val);

                /* C431:THDO_QCONFIG_CELL.Q_LIMIT_DYNAMIC_CELL
                   =IF(C17=0, "NA", IF($C$14, 0, 1)) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_LIMIT_DYNAMIC_CELLf, 
                        _soc_sb2_mmu_params.lossless_mode_d_c ? 0 : 1);

                /* C432:THDO_QCONFIG_CELL.Q_LIMIT_ENABLE_CELL
                   =IF(C17=0, "NA", IF(C14=0, 1, 0)) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_LIMIT_ENABLE_CELLf, 
                        _soc_sb2_mmu_params.lossless_mode_d_c ? 0 : 1);

                /* C433:THDO_QCONFIG_CELL.Q_COLOR_ENABLE_CELL 
                   =IF(C17=0, "NA", 0)*/
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_COLOR_ENABLE_CELLf,0); 

                /* C434: THDO_QCONFIG_CELL.Q_COLOR_LIMIT_DYNAMIC_CELL
                   =IF(C17=0, "NA", IF(C14=0, 1, 0)) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, Q_COLOR_LIMIT_DYNAMIC_CELLf,
                        _soc_sb2_mmu_params.lossless_mode_d_c ? 0 : 1);

                /* C435: THDO_QCONFIG_CELL.LIMIT_YELLOW_CELL=
                   =IF(C17=0, "NA", 0) */
                soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, 
                        &mmu_thdo_qconfig_cell_entry, LIMIT_YELLOW_CELLf, 
                        0);
                SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY,queue,
                            &mmu_thdo_qconfig_cell_entry));

                SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_CELLm(unit,
                            MEM_BLOCK_ANY,queue,
                            &mmu_thdo_qoffset_cell_entry));
                /* C430:THDO_QOFFSET_CELL.RESET_OFFSET_CELL 
                   =IF(C17=0,"NA",1)*/
                soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, 
                        &mmu_thdo_qoffset_cell_entry, RESET_OFFSET_CELLf, 1);

                /* C436: THDO_QOFFSET_CELL.LIMIT_RED_CELL  =
                   =IF(C17=0, "NA", 0) */
                soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, 
                        &mmu_thdo_qoffset_cell_entry, LIMIT_RED_CELLf, 
                        0);

                /* C437: THDO_QOFFSET_CELL.RESET_OFFSET_YELLOW_CELL 
                   =IF(C17=0, "NA", 1)*/
                soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, 
                        &mmu_thdo_qoffset_cell_entry, RESET_OFFSET_YELLOW_CELLf,1); 
                /* C438: THDO_QOFFSET_CELL.RESET_OFFSET_RED_CELL =
                   =IF(C17=0, "NA", 1)*/
                soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, 
                        &mmu_thdo_qoffset_cell_entry, RESET_OFFSET_RED_CELLf,1); 

                SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_CELLm(unit,
                            MEM_BLOCK_ANY,queue,
                            &mmu_thdo_qoffset_cell_entry));
            }
        }
        if (_soc_sb2_mmu_params.extbuf_used_d_c) {
            for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDOEMAr); cos++) {
                SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDOEMAr(
                            unit, cos, &rval));
                /* C440:OP_QUEUE_CONFIG1_THDOEMA.Q_MIN 
                   =IF(C17=0, "NA", C207)*/
                soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval,
                        Q_MINf, 
                        output_port_threshold->min_grntd_res_EMA_queue_cells);

                /* C445:OP_QUEUE_CONFIG1_THDOEMA.Q_COLOR_ENABLE  
                   =IF(C17=0, "NA", 0)*/
                soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval,
                        Q_COLOR_ENABLEf, 0);

                /* C446:OP_QUEUE_CONFIG1_THDOEMA.Q_COLOR_DYNAMIC 
                   =IF(C17=0, "NA", IF(C14=0, 1, 0))*/
                soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval,
                        Q_COLOR_DYNAMICf, 
                        _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
                SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDOEMAr(
                            unit, cos, rval));
            }

            for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDOEMAr); cos++) {
                SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDOEMAr(
                            unit, cos, &rval));
                /* C441:OP_QUEUE_CONFIG_THDOEMA.Q_SHARED_LIMIT
                   =IF(C17=0, "NA", IF($C$14,$C$225,$C$63)) */
                if (_soc_sb2_mmu_params.lossless_mode_d_c) {
                    temp_val = output_port_threshold->
                        min_grntd_tot_shr_EMA_queue_cells;
                } else {
                    temp_val =
                        _soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
                }
                soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval,
                        Q_SHARED_LIMITf, temp_val);

                /*C443: OP_QUEUE_CONFIG_THDOEMA.Q_LIMIT_DYNAMIC
                  =IF(C17=0, "NA", IF($C$14, 0, 1))*/

                soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval,
                        Q_LIMIT_DYNAMICf, 
                        _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);

                /*C444:OP_QUEUE_CONFIG_THDOEMA.Q_LIMIT_ENABLE:
                  =IF(C17=0, "NA", IF(C14=0, 1, 0))*/
                soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval,
                        Q_LIMIT_ENABLEf, 
                        _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
                SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDOEMAr(
                            unit, cos, rval));

            }
            for(cos=0; 
                    cos<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDOEMAr); cos++) {
                SOC_IF_ERROR_RETURN(
                        READ_OP_QUEUE_RESET_OFFSET_THDOEMAr(unit, cos,&rval));
                /*C442: OP_QUEUE_RESET_OFFSET_THDOEMA.Q_RESET_OFFSET 
                  =IF(C17=0,"NA",2) */
                soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDOEMAr, &rval,
                        Q_RESET_OFFSETf, 2);
                SOC_IF_ERROR_RETURN(
                        WRITE_OP_QUEUE_RESET_OFFSET_THDOEMAr(unit, cos,rval));
            }
            for(cos=0; 
                    cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_YELLOW_THDOEMAr); 
                    cos++) {
                SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_YELLOW_THDOEMAr(
                            unit, cos,&rval));
                /* C447: OP_QUEUE_LIMIT_YELLOW_THDOEMA.Q_LIMIT_YELLOW
                   =IF(C17=0, "NA", 0) */
                soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDOEMAr, &rval,
                        Q_LIMIT_YELLOWf, 0);
                SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_YELLOW_THDOEMAr(
                            unit, cos,rval));
            }
            for(cos=0; 
                    cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDOEMAr); 
                    cos++) {
                SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_RED_THDOEMAr(
                            unit, cos,&rval));
                /* C448: OP_QUEUE_LIMIT_RED_THDOEMA.Q_LIMIT_REDi
                   =IF(C17=0, "NA", 0) */
                soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDOEMAr, &rval,
                        Q_LIMIT_REDf, 0);
                SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_RED_THDOEMAr(
                            unit, cos,rval));
            }
            for(cos=0; 
                    cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_YELLOW_THDOEMAr); 
                    cos++) {
                SOC_IF_ERROR_RETURN(
                        READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDOEMAr(unit, cos,&rval));
                /*C449:OP_QUEUE_RESET_OFFSET_YELLOW_THDOEMA.RESUME_OFFSET_YELLOW
                  =IF(C17=0, "NA", 2)*/
                soc_reg_field_set(unit,
                        OP_QUEUE_RESET_OFFSET_YELLOW_THDOEMAr, &rval,
                        RESUME_OFFSET_YELLOWf, 2);
                SOC_IF_ERROR_RETURN(
                        WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDOEMAr(unit, cos,rval));
            }
            for(cos=0; 
                    cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_RED_THDOEMAr); 
                    cos++) {
                SOC_IF_ERROR_RETURN(
                        READ_OP_QUEUE_RESET_OFFSET_RED_THDOEMAr(unit, cos,&rval));
                /*C450:OP_QUEUE_RESET_OFFSET_RED_THDOEMA.RESUME_OFFSET_RED=
                  =IF(C17=0, "NA", 2)*/
                soc_reg_field_set(unit,
                        OP_QUEUE_RESET_OFFSET_RED_THDOEMAr, &rval,
                        RESUME_OFFSET_REDf, 2);
                SOC_IF_ERROR_RETURN(
                        WRITE_OP_QUEUE_RESET_OFFSET_RED_THDOEMAr(unit, cos,rval));
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
                    output_port_threshold->min_grntd_res_queue_cells_EQEs);

            /* C456:THDO_QCONFIG_QENTRY.Q_SHARED_LIMIT_QENTRY
               =IF($C$14,$C$224,$C$63) */
            if (_soc_sb2_mmu_params.lossless_mode_d_c) {
                temp_val = output_port_threshold->
                    min_grntd_tot_shr_queue_cells_EQEs;
            } else {
                temp_val = _soc_sb2_mmu_params.
                    mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
            }
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, 
                    &mmu_thdo_qconfig_qentry_entry, Q_SHARED_LIMIT_QENTRYf, temp_val);

            /* C458:THDO_QCONFIG_QENTRY.Q_LIMIT_DYNAMIC_QENTRY
               =IF($C$14, 0, 1) */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, 
                    &mmu_thdo_qconfig_qentry_entry, Q_LIMIT_DYNAMIC_QENTRYf, 
                    _soc_sb2_mmu_params.lossless_mode_d_c ? 0 : 1);

            /* C459:THDO_QCONFIG_QENTRY.Q_LIMIT_ENABLE_QENTRY
               =IF($C$14, 0, 1) */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, 
                    &mmu_thdo_qconfig_qentry_entry, Q_LIMIT_ENABLE_QENTRYf, 
                    _soc_sb2_mmu_params.lossless_mode_d_c ? 0 : 1);

            /* C460:THDO_QCONFIG_QENTRY.Q_COLOR_ENABLE_QENTRY = 0 */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, 
                    &mmu_thdo_qconfig_qentry_entry, Q_COLOR_ENABLE_QENTRYf,0); 

            /* C461: THDO_QCONFIG_QENTRY.Q_COLOR_LIMIT_DYNAMIC_QENTRY
               =IF($C$14, 0, 1) */
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, 
                    &mmu_thdo_qconfig_qentry_entry, 
                    Q_COLOR_LIMIT_DYNAMIC_QENTRYf,
                    _soc_sb2_mmu_params.lossless_mode_d_c ? 0 : 1);

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
                _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEQr(
                    unit, cos, rval));
    }

    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEQr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEQr(
                    unit, cos, &rval));
        /* C468:OP_QUEUE_CONFIG_THDORQEQ.Q_SHARED_LIMIT
           ==IF($C$14,$C$226,$C$63) */
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
            temp_val = output_port_threshold->
                min_grntd_tot_shr_RE_WQs_cells ;
        } else {
            temp_val =
                _soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy; 
        }	

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval,
                Q_SHARED_LIMITf, temp_val);

        /*C470: OP_QUEUE_CONFIG_THDORQEQ.Q_LIMIT_DYNAMIC:=lossless=0 else 1
          =IF(C14=0, 1, 0) */

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval,
                Q_LIMIT_DYNAMICf, 
                _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);

        /*C471:OP_QUEUE_CONFIG_THDORQEQ.Q_LIMIT_ENABLE:=lossless=0 else 1*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval,
                Q_LIMIT_ENABLEf, 
                _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
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
                _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEIr(
                    unit, cos, rval));
    }

    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEIr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEIr(
                    unit, cos, &rval));
        /* C479:OP_QUEUE_CONFIG_THDORQEI.Q_SHARED_LIMIT
           =IF($C$14,$C$227,$C$63)..Can be put outside loop */
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
            temp_val = output_port_threshold->min_grntd_tot_shr_RE_WQs_queue_cells_for_int_buff;
        } else {
            temp_val = _soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
        }	
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval,
                Q_SHARED_LIMITf, temp_val);

        /*C481: OP_QUEUE_CONFIG_THDORQEI.Q_LIMIT_DYNAMIC:=lossless=0 else 1
          =IF($C$14, 0, 1)*/

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval,
                Q_LIMIT_DYNAMICf, 
                _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);

        /*C482:OP_QUEUE_CONFIG_THDORQEI.Q_LIMIT_ENABLE:=lossless=0 else 1
          =IF($C$14, 0, 1) */
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval,
                Q_LIMIT_ENABLEf, 
                _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
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
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {

        for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEEr); cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEEr(
                        unit, cos, &rval));
            /* C489:OP_QUEUE_CONFIG1_THDORQEE.Q_MIN = 
             =IF(C17=0, "NA", C210)*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval, Q_MINf, 
                    output_port_threshold->min_grntd_res_RE_WQs_queue_cells_for_ext_buff);

            /* C494:OP_QUEUE_CONFIG1_THDORQEE.Q_COLOR_ENABLE  
             =IF(C17=0, "NA", 0)*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval,
                    Q_COLOR_ENABLEf, 0);

            /* C495:OP_QUEUE_CONFIG1_THDORQEE.Q_COLOR_DYNAMIC 
             =IF(C17=0, "NA", IF($C$14, 0, 1))*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval,
                    Q_COLOR_DYNAMICf, 
                    _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEEr(
                        unit, cos, rval));
        }

        for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEEr); cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEEr(
                        unit, cos, &rval));
            /* C490:OP_QUEUE_CONFIG_THDORQEE.Q_SHARED_LIMIT
               =IF(C17=0, "NA",IF($C$14,$C$228,$C$63)) */
            if (_soc_sb2_mmu_params.lossless_mode_d_c) {
                temp_val = output_port_threshold->
                    min_grntd_tot_shr_RE_WQs_queue_cells_for_ext_buff;
            } else {
                temp_val = _soc_sb2_mmu_params.
                    mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
            }	
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval,
                    Q_SHARED_LIMITf, temp_val);

            /*C492: OP_QUEUE_CONFIG_THDORQEE.Q_LIMIT_DYNAMIC
              =IF(C17=0, "NA", IF($C$14, 0, 1))*/

            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval,
                    Q_LIMIT_DYNAMICf, 
                    _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);

            /*C493:OP_QUEUE_CONFIG_THDORQEE.Q_LIMIT_ENABLE
              =IF(C17=0, "NA", IF($C$14, 0, 1))*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval,
                    Q_LIMIT_ENABLEf, 
                    _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEEr(
                        unit, cos, rval));

        }
        for(cos=0; 
                cos<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEEr); cos++) {
            SOC_IF_ERROR_RETURN(
                    READ_OP_QUEUE_RESET_OFFSET_THDORQEEr(unit, cos,&rval));
            /*C491: OP_QUEUE_RESET_OFFSET_THDORQEE.Q_RESET_OFFSET 
             =IF(C17=0, "NA", 1)*/
            soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEEr, &rval,
                    Q_RESET_OFFSETf, 1);
            SOC_IF_ERROR_RETURN(
                    WRITE_OP_QUEUE_RESET_OFFSET_THDORQEEr(unit, cos,rval));
        }
        for(cos=0; 
                cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEEr); 
                cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_YELLOW_THDORQEEr(
                        unit, cos,&rval));
            /* C496: OP_QUEUE_LIMIT_YELLOW_THDORQEE.Q_LIMIT_YELLOW
               =IF(C17=0, "NA", 0) */
            soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEEr, &rval,
                    Q_LIMIT_YELLOWf, 0);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_YELLOW_THDORQEEr(
                        unit, cos, rval));
        }
        for(cos=0; 
                cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDORQEEr); 
                cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_RED_THDORQEEr(
                        unit, cos,&rval));
            /* C497: OP_QUEUE_LIMIT_RED_THDORQEE.Q_LIMIT_RED
                =IF(C17=0, "NA", 0) */
            soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDORQEEr, &rval,
                    Q_LIMIT_REDf, 0);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_RED_THDORQEEr(
                        unit, cos,rval));
        }
        for(cos=0; 
                cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEEr); 
                cos++) {
            SOC_IF_ERROR_RETURN(
                    READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEEr(unit, cos,&rval));
            /*C498:OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEE.RESUME_OFFSET_YELLOW=
             =IF(C17=0, "NA", 1)*/
            soc_reg_field_set(unit,
                    OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEEr, &rval,
                    RESUME_OFFSET_YELLOWf, 1);
            SOC_IF_ERROR_RETURN(
                    WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEEr(unit, cos,rval));
        }
        for(cos=0; 
                cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_RED_THDORQEEr); 
                cos++) {
            SOC_IF_ERROR_RETURN(
                    READ_OP_QUEUE_RESET_OFFSET_RED_THDORQEEr(unit, cos,&rval));
            /*C499:OP_QUEUE_RESET_OFFSET_RED_THDORQEE.RESUME_OFFSET_RED
             =IF(C17=0, "NA", 1)*/
            soc_reg_field_set(unit,
                    OP_QUEUE_RESET_OFFSET_RED_THDORQEEr, &rval,
                    RESUME_OFFSET_REDf, 1);
            SOC_IF_ERROR_RETURN(
                    WRITE_OP_QUEUE_RESET_OFFSET_RED_THDORQEEr(unit, cos,rval));
        }
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
                _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORDEQr(
                    unit, cos, rval));
    }

    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORDEQr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORDEQr(
                    unit, cos, &rval));
        /* C502:OP_QUEUE_CONFIG_THDORDEQ.Q_SHARED_LIMIT
           =IF(C14=0, C63, C229)..Can be put outside loop */
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
            temp_val = output_port_threshold->min_grntd_tot_shr_EP_redirect_queue_entry_cells;
        } else {
            temp_val = _soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
        }	
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEQr, &rval,
                Q_SHARED_LIMITf, temp_val);

        /*C504: OP_QUEUE_CONFIG_THDORDEQ.Q_LIMIT_DYNAMIC:=lossless=0 else 1i
          =IF(C14=0, 1, 0)*/

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEQr, &rval,
                Q_LIMIT_DYNAMICf, 
                _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);

        /*C505:OP_QUEUE_CONFIG_THDORDEQ.Q_LIMIT_ENABLE:=lossless=0 else 1
          =IF(C14=0, 1, 0)*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEQr, &rval,
                Q_LIMIT_ENABLEf, 
                _soc_sb2_mmu_params.lossless_mode_d_c? 0 : 1);
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
                Q_COLOR_DYNAMICf, _soc_sb2_mmu_params.lossless_mode_d_c? 0 :1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORDEIr(
                    unit, cos, rval));
    }

    for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORDEIr); cos++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORDEIr(
                    unit, cos, &rval));
        /* C513:OP_QUEUE_CONFIG_THDORDEI.Q_SHARED_LIMIT = 
           =IF(C14=0, C63, C222)*/
        if (_soc_sb2_mmu_params.lossless_mode_d_c) {
            temp_val = output_port_threshold->min_grntd_tot_shr_queue_cells_int_buff;
        } else {
            temp_val = _soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy;
        }	

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEIr, &rval,
                Q_SHARED_LIMITf, temp_val);

        /*C515: OP_QUEUE_CONFIG_THDORDEI.Q_LIMIT_DYNAMIC:=1 
          =IF(C14=0, 1, 0)*/

        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEIr, &rval,
                Q_LIMIT_DYNAMICf,
                _soc_sb2_mmu_params.lossless_mode_d_c ? 0 :1); 

        /*C516:OP_QUEUE_CONFIG_THDORDEI.Q_LIMIT_ENABLE:
          =IF(C14=0, 1, 0)*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEIr, &rval,
                Q_LIMIT_ENABLEf, 
                _soc_sb2_mmu_params.lossless_mode_d_c ? 0 :1);
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
    if (_soc_sb2_mmu_params.extbuf_used_d_c) {
        for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORDEEr); cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORDEEr(
                        unit, cos, &rval));
            /* C523:OP_QUEUE_CONFIG1_THDORDEE.Q_MIN 
             =IF(C17=0, "NA", C205)*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORDEEr, &rval, Q_MINf, 
                    output_port_threshold->min_grntd_res_queue_cells_ext_buff);

            /* C528:OP_QUEUE_CONFIG1_THDORDEE.Q_COLOR_ENABLE  =
             =IF(C17=0, "NA", 0)*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORDEEr, &rval,
                    Q_COLOR_ENABLEf, 0);

            /* C529:OP_QUEUE_CONFIG1_THDORDEE.Q_COLOR_DYNAMIC = 
             =IF(C17=0, "NA", IF(C14=0, 1, 0))*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORDEEr, &rval,
                    Q_COLOR_DYNAMICf, _soc_sb2_mmu_params.lossless_mode_d_c ? 0 :1);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORDEEr(
                        unit, cos, rval));
        }

        for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORDEEr); cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORDEEr(
                        unit, cos, &rval));
            /* C524:OP_QUEUE_CONFIG_THDORDEE.Q_SHARED_LIMIT 
             =IF(C17=0, "NA", IF(C14=0, C63, C223)) */
            if (_soc_sb2_mmu_params.lossless_mode_d_c) {
                temp_val = output_port_threshold->min_grntd_tot_shr_queue_cells_ext_buff;
            } else {
                temp_val = _soc_sb2_mmu_params.mmu_egr_queue_dyn_thd_param_baf_profile_lossy ;
            }

            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEEr, &rval,
                    Q_SHARED_LIMITf, temp_val);

            /*C526: OP_QUEUE_CONFIG_THDORDEE.Q_LIMIT_DYNAMIC:
             =IF(C17=0, "NA", IF(C14=0, 1, 0))*/

            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEEr, &rval,
                    Q_LIMIT_DYNAMICf,_soc_sb2_mmu_params.lossless_mode_d_c? 0 :1); 

            /*C527:OP_QUEUE_CONFIG_THDORDEE.Q_LIMIT_ENABLE:
             =IF(C17=0, "NA", IF(C14=0, 1, 0))*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEEr, &rval,
                    Q_LIMIT_ENABLEf, _soc_sb2_mmu_params.lossless_mode_d_c? 0 :1);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORDEEr(
                        unit, cos, rval));
        }
        for(cos=0; 
                cos<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORDEEr); cos++) {
            SOC_IF_ERROR_RETURN(
                    READ_OP_QUEUE_RESET_OFFSET_THDORDEEr(unit, cos,&rval));
            /*C525:OP_QUEUE_RESET_OFFSET_THDORDEE.Q_RESET_OFFSET
              =IF(C17=0, "NA", 1) */
            soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORDEEr, &rval,
                    Q_RESET_OFFSETf, 1) ; 
            SOC_IF_ERROR_RETURN(
                    WRITE_OP_QUEUE_RESET_OFFSET_THDORDEEr(unit, cos,rval));
        }
        for(cos=0; 
                cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_YELLOW_THDORDEEr); 
                cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_YELLOW_THDORDEEr(
                        unit, cos,&rval));
            /* C530: OP_QUEUE_LIMIT_YELLOW_THDORDEE.Q_LIMIT_YELLOW 
               =IF(C17=0, "NA", 0) */

            soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDORDEEr, &rval,
                    Q_LIMIT_YELLOWf, 0);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_YELLOW_THDORDEEr(
                        unit, cos,rval));
        }
        for(cos=0; 
                cos<SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDORDEEr); 
                cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_RED_THDORDEEr(
                        unit, cos,&rval));
            /* C531: OP_QUEUE_LIMIT_RED_THDORDEE.Q_LIMIT_RED 
             =IF(C17=0, "NA", 0)*/
            soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDORDEEr, &rval,
                    Q_LIMIT_REDf, 0);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_RED_THDORDEEr(
                        unit, cos,rval));
        }
        for(cos=0; 
                cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEEr); 
                cos++) {
            SOC_IF_ERROR_RETURN(
                    READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEEr(unit, cos,&rval));
            /*C532:OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEE.RESUME_OFFSET_YELLOW
             =IF(C17=0, "NA", 1)*/
            soc_reg_field_set(unit,
                    OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEEr, &rval,
                    RESUME_OFFSET_YELLOWf, 1);
            SOC_IF_ERROR_RETURN(
                    WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDORDEEr(unit, cos,rval));
        }
        for(cos=0; 
                cos < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_RED_THDORDEEr); 
                cos++) {
            SOC_IF_ERROR_RETURN(
                    READ_OP_QUEUE_RESET_OFFSET_RED_THDORDEEr(unit, cos,&rval));
            /*C533:OP_QUEUE_RESET_OFFSET_RED_THDORDEE.RESUME_OFFSET_RED
             =IF(C17=0, "NA", 1)*/
            soc_reg_field_set(unit,
                    OP_QUEUE_RESET_OFFSET_RED_THDORDEEr, &rval,
                    RESUME_OFFSET_REDf, 1);
            SOC_IF_ERROR_RETURN(
                    WRITE_OP_QUEUE_RESET_OFFSET_RED_THDORDEEr(unit, cos,rval));
        }
    }
/* Enable back pressure status from MMU for lossless mode */
    if (_soc_sb2_mmu_params.lossless_mode_d_c) {
        val = 0x1ffffffe;		/* 0 to 29 ports */
        sal_memset (&rval, 0, sizeof(rval));
        soc_reg_field_set (unit, THDIEXT_PORT_PAUSE_ENABLE_64r, &rval,
                PORT_PAUSE_ENABLEf, val);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDIEXT_PORT_PAUSE_ENABLE_64r,REG_PORT_ANY,
                           0, rval));


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
        soc_reg_field_set(unit, THDIEMA_PORT_PAUSE_ENABLE_64r, &rval,
                PORT_PAUSE_ENABLEf, val);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDIEMA_PORT_PAUSE_ENABLE_64r,REG_PORT_ANY,
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
    for (sub_port = SOC_SB2_MIN_SUBPORT_INDEX;
         sub_port <= SOC_SB2_MAX_SUBPORTS;
         sub_port ++) {
         if ( si->port_uc_cosq_base[sub_port]  != 0) {
              for (queue =  si->port_uc_cosq_base[sub_port];
                   queue <  si->port_uc_cosq_base[sub_port] +
                            si->port_num_uc_cosq[sub_port];
                   queue++) {
                   LOG_VERBOSE(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                 "sub_port:%d queue:%d"), sub_port, queue));
                   SOC_IF_ERROR_RETURN(
                   _soc_saber2_mmu_config_extra_queue(unit, queue,
                                               &_soc_sb2_mmu_params,
                                               output_port_threshold)); 
              }
         }
    }
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* Extended Queue(BCM_COSQ_GPORT_SUBSCRIBE) Settings           */
    /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    for (queue = si->port_uc_cosq_base[SB2_MAX_PHYSICAL_PORTS] +
                 si->port_num_uc_cosq[SB2_MAX_PHYSICAL_PORTS];
         queue <= soc_mem_index_max(unit,MMU_THDO_QCONFIG_CELLm);
         queue++) {
         SOC_IF_ERROR_RETURN(
            _soc_saber2_mmu_config_extra_queue(unit, queue,
                                               &_soc_sb2_mmu_params,
                                               output_port_threshold));
    } 
    return SOC_E_NONE;
}

int _soc_saber2_mmu_init_extn(int unit) {
    int    port=0;
    uint32 rval=0;
    
    for (port = SB2_CMIC; port < SB2_LPBK; port++) {
         rval = 0;
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
             soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval, 
                               PORT_MAX_PKT_SIZEf,49);
         }
         SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_MAX_PKT_SIZEr(unit, port,
                                                              rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_MAX_PKT_SIZEr(unit, port, 
                                                              rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_MAX_PKT_SIZEr(unit, port, 
                                                              rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_MAX_PKT_SIZEr(unit, port, 
                                                              rval));
         rval = 0xffff;
         SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_XON_ENABLEr(unit, port,
                                                                rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_XON_ENABLEr(unit, port,
                                                                rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_PRI_XON_ENABLEr(unit, port,
                                                                rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_PRI_XON_ENABLEr(unit, port,
                                                                rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_PRI_XON_ENABLEr(unit, port,
                                                                rval));
    }

    rval = 0;
    for (port = SB2_CMIC; port < SB2_LPBK; port++) {
    if (((*sb2_port_to_block_subports[unit])[port - 1] == 0) ||
        (IS_HG_PORT(unit, port)) || (IS_XE_PORT(unit, port)) ||
        (IS_LB_PORT(unit,port))) {
             if (soc_reg_field_valid(unit, MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r,
                                     sb2_srcport_field_names[port])) {
                 /* PRIx_GRP = 0x7 */
                 soc_reg_field_set(unit, MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r, 
                                   &rval, sb2_srcport_field_names[port], 3);
             }
         }           
    }                         
    SOC_IF_ERROR_RETURN(
            WRITE_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r_REG32(unit, rval));
    
    return SOC_E_NONE;
}

int _soc_saber2_mmu_reconfigure(int unit, int port)
{
  SOC_IF_ERROR_RETURN(_soc_saber2_mmu_init_helper(unit, port));
  LOG_CLI((BSL_META_U(unit,
                      "MMU is reconfigured \n")));
  return SOC_E_NONE;
}

STATIC int
_soc_saber2_mmu_port_init(int unit)
{
    uint32      rval =0 ;
    soc_info_t  *si = &SOC_INFO(unit);
    uint32      max_depth_limit  = 512;
    uint32      fifo_start_addr = 0;
    int         egr_fifo_depth, threshold, port_bw, start_threshold, padding;
    int         block_port, first_port = -1, last_port = -1, block = 0, loop;
    int         port, port_incr = 1;
    int         valid = FALSE;
    int         cfg_num = 0; /* Internal Cfg number */

    for (port = SB2_CMIC; port < SB2_MAX_LOGICAL_PORTS; port += port_incr) {
        port_incr = 1;
        valid = FALSE;
        if (!(IS_CPU_PORT(unit,port) || IS_LB_PORT(unit,port))) {
            /* coverity[overrun-local : FALSE] */
            block = (*sb2_port_to_block[unit])[port - 1];
        }

        if (IS_CPU_PORT(unit, port)) {
            egr_fifo_depth = 2;
            threshold = 1;
            padding = 0;
            port_bw = 63;
            start_threshold = 1;
        } else if (IS_LB_PORT(unit, port)) {
            /* For following 2 configs LB Port B/w is 12.5G else 2.5G
               1) 56260 Cfg=1 (IntCfg=12)
               2) 56460 Cfg=6 (IntCfg=6)
            */
            SOC_IF_ERROR_RETURN(_soc_saber2_get_cfg_num(unit, &cfg_num));
            if ((12 == cfg_num) || (6 == cfg_num)) {
                egr_fifo_depth = (IS_EXT_MEM_PORT(unit, port)) ? 53 : 10;
                threshold = (IS_EXT_MEM_PORT(unit, port)) ? 49 : 1;
                port_bw = 325;
                start_threshold = 48;
            } else {
                egr_fifo_depth = (IS_EXT_MEM_PORT(unit, port)) ? 14 : 2;
                threshold = 1;
                port_bw = 63;
                start_threshold = 13;
            }
            padding = 0;
        } else if (IS_GE_PORT(unit, port)) {
            if (si->port_speed_max[port] == 2500) {
                egr_fifo_depth = (IS_EXT_MEM_PORT(unit, port)) ? 19 : 2;
                threshold = (IS_EXT_MEM_PORT(unit, port)) ? 15 : 1;
                padding =  19 - egr_fifo_depth;
                port_bw = 63;
                start_threshold = 14;
            } else {
                egr_fifo_depth = (IS_EXT_MEM_PORT(unit, port)) ? 10 : 2;
                threshold = (IS_EXT_MEM_PORT(unit, port)) ? 8 : 1;
                padding =  10 - egr_fifo_depth;
                port_bw = 25;
                start_threshold = 7;
            }
        } else if ((si->port_speed_max[port] != 0) &&
                (si->port_speed_max[port] <= 13000)) {
            egr_fifo_depth = (IS_EXT_MEM_PORT(unit, port)) ? 53 : 10;
            threshold = (IS_EXT_MEM_PORT(unit, port)) ? 49 : 1;
            if (IS_XL_PORT(unit, port)) {
                /* Eagle core will always be initialized to worst-case
                 * conditions, so no need for extra padding */
                padding = 53 - egr_fifo_depth;
            } else {
                /* Actual required FIFO size is 53,
                 * but padded to 76 to support 4x2.5G flex */
                padding = 76 - egr_fifo_depth;
                port_incr = 4;
            }
            port_bw = 325;
            start_threshold = 48;
        } else if(SOC_WARM_BOOT(unit) && (block == 6) &&
                (si->port_speed_max[port] == 0)) {
            if((eagle_n2_mode) && ((port == 26) || (port == 28))) {
                continue;
            }
            if((eagle_n4s_mode) && ((port == 26) || (port == 28))) {
                egr_fifo_depth = (IS_EXT_MEM_PORT(unit, port)) ? 10 : 2;
                threshold = (IS_EXT_MEM_PORT(unit, port)) ? 8 : 1;
                padding =  10 - egr_fifo_depth;
                port_bw = 25;
                start_threshold = 7;
                valid = TRUE;
            } else if(((eagle_n4s_mode) && (port == 27)) ||
                    (!eagle_n4s_mode)){
                egr_fifo_depth = (IS_EXT_MEM_PORT(unit, port)) ? 53 : 10;
                threshold = (IS_EXT_MEM_PORT(unit, port)) ? 49 : 1;
                padding = 53 - egr_fifo_depth;
                port_bw = 325;
                start_threshold = 48;
                valid = TRUE;
            } else {
                /* Ideally should not hit this block. */
                return SOC_E_INTERNAL;
            }
        } else { /* > 13G */
            if (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
                continue;
            }
            return SOC_E_PARAM;
        }

        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
            SOC_IF_ERROR_RETURN(READ_DEQ_EFIFO_CFGr(unit, port, &rval));
            soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                    EGRESS_FIFO_START_ADDRESSf, fifo_start_addr);
            soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                    EGRESS_FIFO_DEPTHf, egr_fifo_depth);
            soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                    EGRESS_FIFO_XMIT_THRESHOLDf, threshold);
        }

        if (!(IS_CPU_PORT(unit,port) || IS_LB_PORT(unit,port))) {
            loop = 0; first_port = -1; last_port = -1;
            while(loop < 4) {
                block_port = (*sb2_block_ports[unit])[block][loop];
                if((eagle_n2_mode) &&
                        ((block_port == 26) || (block_port == 28))) {
                    loop++;
                    continue;
                }
                if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), block_port) ||
                        (valid == TRUE)) {
                    if(first_port == -1) {
                        first_port = block_port;
                    }
                    last_port = block_port;
                }
                loop++;
            }
            if(port == first_port) {
                block_max_startaddr[block] = fifo_start_addr;
            }
            if (port == last_port) {
                block_max_endaddr[block] =
							fifo_start_addr + egr_fifo_depth + padding - 1;
            }
        }
        fifo_start_addr += egr_fifo_depth + padding;

        if(fifo_start_addr > max_depth_limit) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "DEQ_EFIFO_CFG limit crossed: Available limit %d"
                    "port:%d depth:%d fifo_start_addr=%d\n"),
                    max_depth_limit, port, egr_fifo_depth, fifo_start_addr));
            return SOC_E_RESOURCE;
        }

        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
            SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFGr(unit, port, rval));

            /* Port BW Ctrl */
            SOC_IF_ERROR_RETURN(READ_TOQ_PORT_BW_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval,
                    PORT_BWf, port_bw);
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval,
                    START_THRESHOLDf, start_threshold);
            SOC_IF_ERROR_RETURN(WRITE_TOQ_PORT_BW_CTRLr(unit, port, rval));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_sb2_post_mmu_init(int unit) {

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
        SOC_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0,
                                                &epc_link_bmap_entry));
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
    soc_reg_field_set(unit, TXLP_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x84);
    soc_reg_field_set(unit, TXLP_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TXLP_HW_RESET_CONTROL_1r(unit, 1, rval));

    /* TXLP_PORT_ADDR_MAP_TABLE */
    for(loop=0, start_addr=0,port=1 ; loop < SB2_MAX_PORTS_PER_BLOCK;
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


int
soc_saber2_perq_flex_counters_init(int unit, uint32 drop_mask)
{
    _soc_saber2_counter_info_t *counter_info;
    uint32 rval;
    int index, field_index, i, j, i_max = 2, j_max = 16;
    int flags = 0;
    int counter_index = 0, total_num_elem = 6, elem_size = 2;
    int num_elem = 0;
    _soc_saber2_counter_type_t type = _SOC_COUNTER_TYPE_MAX;
    soc_counter_non_dma_id_t non_dma_id;
    int enq_stats = 0;
    _sb2_cosq_counter_mem_map_t *mem_map;
    uint16 dev_id;
    uint8 rev_id;
    soc_reg_t   dtype_reg[] = {
        CTR_DEQ_DTYPE_TBL0r,
                                CTR_DEQ_DTYPE_TBL1r
                              };
    soc_field_t dtype_field[] = {
                                  DTYPE0f, DTYPE1f, DTYPE2f, 
                                  DTYPE3f, DTYPE4f, DTYPE5f, 
                                  DTYPE6f, DTYPE7f, DTYPE8f, 
                                  DTYPE9f, DTYPE10f, DTYPE11f,
                                  DTYPE12f, DTYPE13f,DTYPE14f,
                                  DTYPE15f
                                };
    soc_field_t act_field[3] = {ACTIVE0f, ACTIVE1f, ACTIVE2f};
    soc_field_t seg_field[3] = {SEG0f, SEG1f, SEG2f};
    
    soc_sb2_cosq_counter_mem_map_get(unit, &num_elem, &mem_map);

    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((dev_id == BCM56233_DEVICE_ID) ||
        (SOC_IS_METROLITE(unit))) {
        elem_size = 1;
        total_num_elem = 3;
    }

    /* Setup MMU counter pool segment start address, consider each 512 block
       as separate segment */
    for (i = 0; i < total_num_elem; i++) {
        if ((dev_id == BCM56233_DEVICE_ID) ||
            (SOC_IS_METROLITE(unit))) {
            rval = 0;
            soc_reg_field_set(unit, CTR_SEGMENT_STARTr, &rval,
                    SEG_STARTf, (i * 512));
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, CTR_SEGMENT_STARTr,REG_PORT_ANY,
                               i, rval));
        } else {
            if (!soc_feature(unit, soc_feature_cosq_gport_stat_ability) &&
                    ((i % 2) != 0)) {
                /*
                 * Create 3 segments each with 1K counters
                 * Seg0(total enq drop pkts) seg_start address 0
                 * Seg2(red enq drop pkts)  seg_start address 2 * 512
                 * Seg4(deq stats)) seg_start_address 3 * 512
                 */
                continue;
            }
            rval = 0;
            soc_reg_field_set(unit, CTR_SEGMENT_STARTr, &rval,
                    SEG_STARTf, (i * 512));
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, CTR_SEGMENT_STARTr,REG_PORT_ANY,
                               i, rval));
        }
    }

    /* Default configuration */
    /* Cosq_stats -
     * CTR_FLEX_COUNT_0 = Total ENQ discarded
     * CTR_FLEX_COUNT_2  = Red ENQ discarded
     * CTR_FLEX_COUNT_4  = Total DEQ
     */

    /* Cosq_gport_stats -
     * CTR_FLEX_COUNT_0 = Total ENQ discarded
     * CTR_FLEX_COUNT_1 = Total ENQ Accepted
     * CTR_FLEX_COUNT_2 = Green ENQ discarded
     * CTR_FLEX_COUNT_3 = Yellow ENQ discarded
     * CTR_FLEX_COUNT_4 = Red packet ENQ discarded
     * CTR_FLEX_COUNT_5 = Total DEQ
     */
    counter_info = _soc_saber2_counter_info;


    /* reset the previously configured registers*/ 
    rval = 0;
    for ( i = 0; i < 3; i++) {
        soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, act_field[i], 0);
        soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, seg_field[i], 0);
        SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, CTR_ENQ_STATS_CFGr,REG_PORT_ANY,0, rval));
    }  
    
    rval = 0;
    soc_reg_field_set(unit, CTR_DEQ_STATS_CFGr, &rval, ACTIVE0f, 0);
    soc_reg_field_set(unit, CTR_DEQ_STATS_CFGr, &rval, SEG0f, 0);
    SOC_IF_ERROR_RETURN 
            (soc_reg32_set(unit, CTR_DEQ_STATS_CFGr, REG_PORT_ANY,0 , rval));
 
    SOC_IF_ERROR_RETURN (READ_CTR_SYS_CONTROLr (unit, &rval));
    soc_reg_field_set (unit, CTR_SYS_CONTROLr, &rval, DEQ_FIRST_BRICKf, 0);
    soc_reg_field_set (unit, CTR_SYS_CONTROLr, &rval, DEQ_LAST_BRICKf, 0);
    SOC_IF_ERROR_RETURN (WRITE_CTR_SYS_CONTROLr (unit, rval));
    
    /*intializing the segment in counter info */
    for ( i =0; i < 6; i++) {
        _soc_saber2_counter_info[i].segment = 0;
    }

    for (counter_index = 0; counter_index < num_elem; counter_index+=elem_size) {

        non_dma_id = mem_map[counter_index].non_dma_id;
        field_index = counter_index/elem_size;
        rval = 0;
        if ((non_dma_id == SOC_COUNTER_NON_DMA_COSQ_DROP_PKT)||
                (non_dma_id == SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT)) {
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, act_field[field_index], 1);
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, seg_field[field_index], counter_index);
        } 
        switch (non_dma_id){
            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
                type = _SOC_COUNTER_TYPE_DROP_ENQ; 
                for ( i = _SOC_COUNTER_TYPE_DROP_ENQ_GREEN; 
                        i <= _SOC_COUNTER_TYPE_DROP_ENQ_RED; i++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, CTR_ENQ_STATS_CFGr,
                                       REG_PORT_ANY,
                                       counter_info[i].index,
                                       rval));
                    flags |= (1 << i); 
                }
                break;
            case SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT:
                type = _SOC_COUNTER_TYPE_ACCEPT_ENQ; 
                for ( i = _SOC_COUNTER_TYPE_ACCEPT_ENQ_GREEN; 
                        i <= _SOC_COUNTER_TYPE_ACCEPT_ENQ_RED; i++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, CTR_ENQ_STATS_CFGr,
                                       REG_PORT_ANY,
                                       counter_info[i].index,
                                       rval));
                    flags |= (1 << i); 
                }
                break;
            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN:
                type = _SOC_COUNTER_TYPE_DROP_ENQ_GREEN; 
                break;

            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED:
                type = _SOC_COUNTER_TYPE_DROP_ENQ_RED; 
                break;

            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW:
                type = _SOC_COUNTER_TYPE_DROP_ENQ_YELLOW; 
                break;

            case SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_GREEN:
                type = _SOC_COUNTER_TYPE_ACCEPT_ENQ_GREEN; 
                break;

            case SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_RED:
                type = _SOC_COUNTER_TYPE_ACCEPT_ENQ_RED; 
                break;

            case SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_YELLOW:
                type = _SOC_COUNTER_TYPE_ACCEPT_ENQ_YELLOW; 
                break;

            case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT: 
                type = _SOC_COUNTER_TYPE_ACCEPT_DEQ;
                /* enable deq stats */
                if (SOC_IS_METROLITE(unit)) {
                    i_max = 1;
                    j_max = 14;
                } 
                for (i = 0; i < i_max; i++) {
                    rval = 0; 
                    for (j = 0; j < j_max; j++) {
                        if ((dtype_reg[i] == CTR_DEQ_DTYPE_TBL1r) && (j > 13)) {
                            continue;
                        }
                        soc_reg_field_set(unit, dtype_reg[i], &rval, dtype_field[j], 2);
                    }   
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, dtype_reg[i], REG_PORT_ANY, 0, rval));
                }
                /* set deq stats config */
                rval = 0;
                index = counter_info[type].index;
                soc_reg_field_set(unit, CTR_DEQ_STATS_CFGr, &rval, ACTIVE0f, 1);
                soc_reg_field_set(unit, CTR_DEQ_STATS_CFGr, &rval, SEG0f, counter_index);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, CTR_DEQ_STATS_CFGr, REG_PORT_ANY, index, rval));

                SOC_IF_ERROR_RETURN (READ_CTR_SYS_CONTROLr (unit, &rval));
                soc_reg_field_set (unit, CTR_SYS_CONTROLr, &rval, DEQ_FIRST_BRICKf, counter_index);
                soc_reg_field_set (unit, CTR_SYS_CONTROLr, &rval, DEQ_LAST_BRICKf, counter_index+ 1);
                SOC_IF_ERROR_RETURN (WRITE_CTR_SYS_CONTROLr (unit, rval));
                enq_stats++;
                break;
            default:
                /*should not reach here*/
                break;
        }

        _soc_saber2_counter_info[type].segment = counter_index;
        if ((type >= _SOC_COUNTER_TYPE_DROP_ENQ_GREEN) 
                && (type <= _SOC_COUNTER_TYPE_ACCEPT_ENQ_RED)){ 

            rval = 0;
            index = counter_info[type].index;
            if (flags & (1 << type)) {
               SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, CTR_ENQ_STATS_CFGr,
                               REG_PORT_ANY, index, &rval));
            } 
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, act_field[field_index-enq_stats], 1);
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, seg_field[field_index-enq_stats], counter_index);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, CTR_ENQ_STATS_CFGr,
                               REG_PORT_ANY,
                               counter_info[type].index,
                               rval));
        }
    }
    /* This is set to 1 so that we count only thdo drops in the out drops*/
    SOC_IF_ERROR_RETURN(READ_THDO_DROP_CTR_CONFIGr(unit, &rval));
    
    /*drop mask eliminates the packet drop counts for masked out drop types.*/
    soc_reg_field_set(unit, THDO_DROP_CTR_CONFIGr, &rval, OP_DROP_MASKf,drop_mask);
    soc_reg_field_set(unit, THDO_DROP_CTR_CONFIGr, &rval, OP_CNT_CFGf, 1);
    
    SOC_IF_ERROR_RETURN(WRITE_THDO_DROP_CTR_CONFIGr(unit, rval));

    return SOC_E_NONE;
}

int _soc_sb2_invalid_parent_index(int unit, int level)
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

#define INVALID_PARENT(unit, level)   _soc_sb2_invalid_parent_index((unit),(level))

/* Programming all the entries with the Invalid Parent Pointer */
int
_soc_saber2_mmu_set_invalid_parent(int unit)
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
_soc_saber2_mmu_init(int unit)
{
    uint32 rval;
    int                 mmu_init_usec, i;
    soc_info_t *si;
    soc_timeout_t       to;
    mmu_aging_lmt_int_entry_t age_entry;
    int count;
    uint16 dev_id;
    uint8 rev_id;
    uint64 rval1;
    int port;
    uint32 ext_pbmp_count=0;
    pbmp_t ext_pbmp;
    int rd_phase, wr_phase , freq_loc;

    si = &SOC_INFO(unit);
    soc_cm_get_id(unit, &dev_id, &rev_id);

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

    /* In sim, INIT_DONE will not be set. So manually setting it. */
    if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
        SOC_IF_ERROR_RETURN(READ_LLS_INITr(unit, &rval));
        soc_reg_field_set(unit, LLS_INITr, &rval, INIT_DONEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_LLS_INITr(unit, rval));
    }

    do {
        SOC_IF_ERROR_RETURN(READ_LLS_INITr(unit, &rval));
        if (soc_reg_field_get(unit, LLS_INITr, rval, INIT_DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "LLS INIT timeout\n")));
            break;
        }
    } while (TRUE);

    /* Enable all ports */
    rval = 0xFFFFFFFF;
    soc_reg_field_set(unit, INPUT_PORT_RX_ENABLE_64r, &rval,
                          INPUT_PORT_RX_ENABLEf, 0x3FFFFFFF);
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLE_64r_REG32(unit, rval));

    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_INPUT_PORT_RX_ENABLE_64r_REG32(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_INPUT_PORT_RX_ENABLE_64r_REG32(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_INPUT_PORT_RX_ENABLE_64r_REG32(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_INPUT_PORT_RX_ENABLE_64r_REG32(unit, rval));

    SOC_IF_ERROR_RETURN(WRITE_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDO_BYPASSr(unit, 0));

    ext_pbmp = soc_property_get_pbmp(unit, spn_PBMP_EXT_MEM, 0);
    SOC_PBMP_COUNT(ext_pbmp, ext_pbmp_count);
    if (soc_feature(unit, soc_feature_ddr3) && SOC_DDR3_NUM_MEMORIES(unit)) {
        /* Configure EMC */
        SOC_IF_ERROR_RETURN(READ_EMC_CFGr(unit, &rval));
        soc_reg_field_set(unit, EMC_CFGr, &rval, NUM_CISf,
                          SOC_DDR3_NUM_MEMORIES(unit));
        soc_reg_field_set(unit, EMC_CFGr, &rval, DRAM_SIZEf,
                          SOC_DDR3_NUM_ROWS(unit) / 8192 - 1);
        SOC_IF_ERROR_RETURN(WRITE_EMC_CFGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_EMC_FREE_POOL_SIZESr(unit, &rval));
        soc_reg_field_set(unit, EMC_FREE_POOL_SIZESr, &rval, WTFP_SIZEf, 0x200);
        soc_reg_field_set(unit, EMC_FREE_POOL_SIZESr, &rval, RSFP_SIZEf,
                          (SOC_DDR3_NUM_MEMORIES(unit) == 1) ? 64 : 128);
        SOC_IF_ERROR_RETURN(WRITE_EMC_FREE_POOL_SIZESr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_EMC_IRRB_THRESHOLDSr(unit, &rval));
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval, ALL_Q_XOFF_THRESHOLDf,   0x10);
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval,                          ALL_UNDERRUN_Q_XOFF_THRESHOLDf, 0x28);
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval,                          ALL_UNDERRUN_Q_XON_THRESHOLDf, 0x18);
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval, ALL_Q_XON_THRESHOLDf, 8);
        SOC_IF_ERROR_RETURN(WRITE_EMC_IRRB_THRESHOLDSr(unit, rval));
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFG_COMPLETEr(unit, rval));

    /* Setup TDM for MMU Arb & LLS */
    SOC_IF_ERROR_RETURN(_soc_saber2_mmu_tdm_init(unit));

    /* Config DEQ_EFIFO_CFG and TOQ_PORT_BW_CTRL */
    SOC_IF_ERROR_RETURN(_soc_saber2_mmu_port_init(unit));
    if((soc_feature(unit, soc_feature_ddr3)) &&
                (!(SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM || SAL_BOOT_XGSSIM)) &&
               SOC_DDR3_NUM_MEMORIES(unit)) {
        freq_loc = _soc_ddr_sb2_phy_freqtoloc(SOC_DDR3_CLOCK_MHZ(unit));

        if (freq_loc == SB2_FREQ_667) {
            wr_phase = 119;
            rd_phase = 122;
        } else if (freq_loc == SB2_FREQ_800) {
            wr_phase = 120;
            rd_phase = 124;
        } else {
             LOG_DEBUG(BSL_LS_SOC_MII,
                     (BSL_META_U(unit,
                                "_soc_ddr_sb2_phy_freqtoloc() " 
                                "returned invalid frequency")));
            return SOC_E_INTERNAL;
        }


        for(i=0; i<16; i++) {
            SOC_IF_ERROR_RETURN(READ_TOQ_EXT_MEM_BW_MAP_TABLEr(unit, i, &rval));
            soc_reg_field_set(unit, TOQ_EXT_MEM_BW_MAP_TABLEr, &rval,
                                                GBL_GUARENTEE_BW_LIMITf, 0x1C2);
            soc_reg_field_set(unit, TOQ_EXT_MEM_BW_MAP_TABLEr, &rval,
                                                WR_PHASEf, wr_phase);
            soc_reg_field_set(unit, TOQ_EXT_MEM_BW_MAP_TABLEr, &rval,
                                                RD_PHASEf, rd_phase);
            SOC_IF_ERROR_RETURN(
                WRITE_TOQ_EXT_MEM_BW_MAP_TABLEr(unit,i,rval));
        }
    }

    rval = 0;
    soc_reg_field_set(unit, TOQ_BW_LIMITING_MIDPKT_ENr,
             &rval, MIDPKT_SHAPE_ENf, 0x1FFFFFFE);
    SOC_IF_ERROR_RETURN(WRITE_TOQ_BW_LIMITING_MIDPKT_ENr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, DEQ_EFIFO_CFG_COMPLETEr, &rval,
            EGRESS_FIFO_CONFIGURATION_COMPLETEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFG_COMPLETEr(unit, rval));
    
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
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval,                       S0_MAX_REFRESH_ENABLEf,   1);
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

    if ((dev_id == BCM56268_DEVICE_ID) || (dev_id == BCM56263_DEVICE_ID)) {
        /* Refer the formula in the Reg file
           For Saber2 56268, frequency is 37MHZ
           Calculation  is
           ((1024 +256)/2 * (0.027027)) + 1 = 18.29
           So we are programming 19 */
        /* WRED Configuration */
        SOC_IF_ERROR_RETURN(READ_WRED_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, BASE_UPDATE_INTERVALf, 19);
        SOC_IF_ERROR_RETURN(WRITE_WRED_MISCCONFIGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_WRED_PARITY_ERROR_BITMAPr(unit, &rval));
        soc_reg_field_set(unit, WRED_PARITY_ERROR_BITMAPr, &rval, UPDATE_INTRPT_STATUSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_WRED_PARITY_ERROR_BITMAPr(unit, rval));
    } else if ((dev_id == BCM56262_DEVICE_ID) || (dev_id == BCM56463_DEVICE_ID)
               || (dev_id == BCM56267_DEVICE_ID) || (dev_id== BCM56468_DEVICE_ID)
               || (dev_id == BCM56233_DEVICE_ID)) {
        /* Refer the formula in the Reg file
           For Saber2 56262, frequency is 75MHZ
           Calculation  is
           ((1024 +256)/2 * (0.013)) + 1 = 9.32
           So we  program value 10 */
        /* WRED Configuration */
        SOC_IF_ERROR_RETURN(READ_WRED_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, BASE_UPDATE_INTERVALf, 10);
        SOC_IF_ERROR_RETURN(WRITE_WRED_MISCCONFIGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_WRED_PARITY_ERROR_BITMAPr(unit, &rval));
        soc_reg_field_set(unit, WRED_PARITY_ERROR_BITMAPr, &rval, UPDATE_INTRPT_STATUSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_WRED_PARITY_ERROR_BITMAPr(unit, rval));
    } else {
        /* WRED Configuration */
        SOC_IF_ERROR_RETURN(READ_WRED_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, BASE_UPDATE_INTERVALf, 7);
        SOC_IF_ERROR_RETURN(WRITE_WRED_MISCCONFIGr(unit, rval));
    }
    
    SOC_IF_ERROR_RETURN(READ_WRED_PARITY_ERROR_MASKr(unit, &rval));
    soc_reg_field_set(unit, WRED_PARITY_ERROR_MASKr, &rval, UPDATE_INTRPT_MASKf,  0);
    SOC_IF_ERROR_RETURN(WRITE_WRED_PARITY_ERROR_MASKr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, REFRESH_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    /*
     * By default drop mask has been configured 0. It configures OP_DROP_MASK as 0 in 
     * THDO_DROP_CTR_CONFIG thus enabling all THDO drop counters
     */
    SOC_IF_ERROR_RETURN(soc_saber2_perq_flex_counters_init(unit,0));

    rval = 0xffffffff; /* PRIx_GRP = 0x7 */

    /* Configuring all profile 3 to use all the priority groups 0 to 7 */

    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_3_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_3_PRI_GRP1r(unit, rval));
   
    SOC_IF_ERROR_RETURN(WRITE_PROFILE3_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_PROFILE3_PRI_GRP1r(unit, rval));

    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PROFILE3_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PROFILE3_PRI_GRP1r(unit, rval));

    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PROFILE3_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PROFILE3_PRI_GRP1r(unit, rval));

    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PROFILE3_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PROFILE3_PRI_GRP1r(unit, rval));

    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PROFILE3_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PROFILE3_PRI_GRP1r(unit, rval));

    COMPILER_64_ZERO(rval1);

    /* Setting profile ID=3 for all the ports which support 8 PG */ 
    for (port = si->cmic_port + 1 ; port <= si->lb_port; port++) {
        /* 10-12, 14-16,18-20  ports have 1 PG */
        if ((( port > 9) && (port < 13)) ||
                (( port > 13) && (port < 17)) ||
                (( port > 17) && (port < 21))) {
            continue;
        }
        soc_reg64_field32_set(unit, MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r, 
                &rval1 ,sb2_srcport_field_names[port], 3);
    }

    WRITE_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r(unit, rval1);
    /* Using the same rval1 for setting the remaining registers */
    WRITE_PORT_PROFILE_MAPr(unit, rval1);
    WRITE_THDIEXT_PORT_PROFILE_MAPr(unit, rval1);
    WRITE_THDIEMA_PORT_PROFILE_MAPr(unit, rval1);
    WRITE_THDIRQE_PORT_PROFILE_MAPr(unit, rval1);
    WRITE_THDIQEN_PORT_PROFILE_MAPr(unit, rval1);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
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

    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_soc_saber2_mmu_init_helper(unit, -1));
    }

    
    /* Initialize MMU internal/external aging limit memory */
    count = soc_mem_index_count(unit, MMU_AGING_LMT_INTm);
    sal_memset(&age_entry, 0, sizeof(mmu_aging_lmt_int_entry_t));
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_AGING_LMT_INTm(unit,
                            MEM_BLOCK_ANY, i, &age_entry));
    }
    
    count = soc_mem_index_count(unit, MMU_AGING_LMT_EXTm);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_AGING_LMT_EXTm(unit,
                            MEM_BLOCK_ANY, i, &age_entry));
    }
    
    SOC_IF_ERROR_RETURN(_soc_sb2_post_mmu_init(unit));

    /* RQE_QUEUE_PRI_x field is valid for all revisions of all SKUs of SB2 */

    if ((rev_id == BCM56260_A0_REV_ID) || (rev_id == BCM56260_B0_REV_ID)) {
        /* Allow the RQE to use all eight queues on B0 */
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
    }

    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_soc_saber2_mmu_set_invalid_parent(unit));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_saber2_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &rval));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_saber2_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_saber2_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 rval;

    rval = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, rval));

    return SOC_E_NONE;
}

int sb2_linkphy_port[1][4] = {
    {1, 2, 3, 4}  /* {T,R}XLP0 */
};

int 
soc_sb2_linkphy_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index)
{
    int i,j;
    int blk = -1;
    int idx = -1;

    for (i=0; i<1; i++) {
        for(j=0; j<4; j++) {
            if(port == sb2_linkphy_port[i][j]) {
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
soc_sb2_linkphy_port_blk_idx_get(
    int unit, int port, int *block, int *index)
{
    int i,j;
    int blk = -1;
    int idx = -1;

    for (i=0; i<1; i++) {
        for(j=0; j<4; j++) {
            if(port == sb2_linkphy_port[i][j]) {
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

int soc_sb2_linkphy_get_portid(int unit, int block, int index) {
    if (index > 3) {
        return -1;
    }
    if ((block == TXLP0_BLOCK(unit)) || (block == RXLP0_BLOCK(unit))) {
        return sb2_linkphy_port[0][index];
    }
    return -1;
}

int sb2_iecell_port[1][4] = {
    {25, 26, 27, 28}  /* IECELL0 */
};

int 
soc_sb2_iecell_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index)
{
    int i,j;
    int blk = -1;
    int idx = -1;

    for (i=0; i<1; i++) {
        for(j=0; j<4; j++) {
            if(port == sb2_iecell_port[i][j]) {
                    blk = i;
                    idx = j;
                    break;
            }
        }
    }

    if (blktype == SOC_BLK_IECELL) {
        if (blk == 0) {
            blk = IECELL0_BLOCK(unit);
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
soc_sb2_iecell_port_blk_idx_get(
    int unit, int port, int *block, int *index)
{
    int i,j;
    int blk = -1;
    int idx = -1;

    for (i=0; i<1; i++) {
        for(j=0; j<4; j++) {
            if(port == sb2_iecell_port[i][j]) {
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

int soc_sb2_iecell_get_portid(int unit, int block, int index) {
    if (index > 3) {
        return -1;
    }
    if (block == IECELL0_BLOCK(unit)) {
        return sb2_iecell_port[0][index];
    }
    return -1;
}
int
soc_saber2_port_is_unused(int unit, int port_no)
{
    uint32 port_blk = 0;
    soc_port_t first_port;

    if (NULL == sb2_block_ports[unit]) {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                  "Port blocks not initialized\n")));
        return 1;
    }

    /* If LB Port return port valid */
    if ((port_no == LB_PORT(unit)) || (port_no == CMIC_PORT(unit))) {
        return 0;
    }

    /* Get port block index*/
    port_blk = (port_no + 3)/4 - 1;
    if (port_blk > SB2_MAX_BLOCKS) {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                  "Invalid Port block\n")));
        return 1;
    }

    /* Check if first port in block is not valid return false */
    /* coverity[overrun-local : FALSE] */
    first_port = (*sb2_block_ports[unit])[port_blk][0];
    if (!SOC_PORT_VALID(unit, first_port)) {
        return 1;
    }

    return 0;
}
/*
 * cpu port (mmu port 0): 48 queues (0-47)
 * loopback port (48-71)
 * hg ports : 24 queues
 * other ports : 8 queues                              
 */

int
soc_saber2_num_cosq_init(int unit)
{
    soc_info_t *si;
    int port, prev_port;
    int numq = 0, i;
    int pp_port_cos;
    int start_pp_port, end_pp_port;
    int packing_mode;
    int num_uc_cosq = 0;

    si = &SOC_INFO(unit);
    packing_mode = soc_property_get(unit, spn_MMU_MULTI_PACKETS_PER_CELL, 0);

    /* For CPU port it is hardcoded to zero as base queue */
    si->port_cosq_base[si->cmic_port] = 0;
    si->port_uc_cosq_base[si->cmic_port] = 0;
    si->port_num_uc_cosq[si->cmic_port] = NUM_CPU_COSQ(unit);
    si->port_num_ext_cosq[si->cmic_port] = 0;

    /* For Loopback port it is hardcoded as 48 as a base queue*/
    si->port_cosq_base[si->lb_port] = 48;
    si->port_uc_cosq_base[si->lb_port] = 48;
    si->port_num_uc_cosq[si->lb_port] = 24;

    /* first front panel port needs to start with 72 */
    prev_port = si->cmic_port;
    numq = 72;
    for (port = si->cmic_port + 1 ; port < si->lb_port; port++) {
        if (soc_saber2_port_is_unused(unit, port)) {
            continue;
        }
        si->port_num_ext_cosq[port] = 0;
        si->port_num_cosq[port] = 0;

        /* For all GE/, XE, HG Ports */
        si->port_uc_cosq_base[port] = si->port_uc_cosq_base[prev_port] +
            numq;
        si->port_cosq_base[port] = si->port_uc_cosq_base[port];

        prev_port = port;
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

        /* In Packing mode we have seperate queues for multicast and unicast 
           So using first half of the table for unicats and second half for multicast*/
        if (packing_mode) {
            if (!IS_CPU_PORT(unit, port)) {
                si->port_cosq_base[port] += 512;
            }
            si->port_num_cosq[port] = si->port_num_uc_cosq[port];    
        }

        /* This need to be validated when testing COE ports */
        pp_port_cos = soc_property_port_get(unit, port, 
                spn_NUM_SUBPORT_COS, 4); 
        if (si->port_num_subport[port] > 0) {
            start_pp_port = si->port_subport_base[port];
            end_pp_port = si->port_subport_base[port] + 
                si->port_num_subport[port];
            for (i = start_pp_port; i < end_pp_port; i++) {
                si->port_uc_cosq_base[i] = si->port_uc_cosq_base[port] + numq;
                si->port_cosq_base[i] = si->port_uc_cosq_base[i];
                si->port_num_uc_cosq[i] = pp_port_cos;
                numq += pp_port_cos;
                if (packing_mode) {
                    si->port_cosq_base[i] += 512;
                    if (si->port_cosq_base[i] >= 1024) {
                        LOG_ERROR(BSL_LS_BCM_COSQ,
                          (BSL_META_U(unit,
                          "Packing and subport configuration together causing "
                          "total queue allocation more than 1024.\n")));
                        return SOC_E_CONFIG;
                    } 
                    si->port_num_cosq[i] = si->port_num_uc_cosq[i];    
                }
            }
        }

        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, 
                    "\nport num %d cosq_base %d num_cosq %d"),
                    port, si->port_cosq_base[port], si->port_num_uc_cosq[port]));
    }

    return SOC_E_NONE;
}

/* soc_sb2_mem_config:
 * Over-ride the default table sizes (from regsfile) for any SKUs here
 */
int
soc_sb2_mem_config(int unit, int dev_id)
{
    int rv = SOC_E_NONE;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 0;
    soc_persist_t *sop = SOC_PERSIST(unit);

    /* L3_DEFIP sizing */

    switch (dev_id) {
    case BCM56268_DEVICE_ID:
    case BCM56260_DEVICE_ID:
    case BCM56262_DEVICE_ID:
    case BCM56263_DEVICE_ID:
    case BCM56265_DEVICE_ID:
    case BCM56267_DEVICE_ID:
    case BCM56233_DEVICE_ID:
        SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_SB2_DEFIP_MAX_TCAMS;
        break;
    default:
        SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_SB2PLUS_DEFIP_MAX_TCAMS;
        break;

    }
    SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_SB2_DEFIP_TCAM_DEPTH;
    if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

        if(dev_id == BCM56233_DEVICE_ID) {
            num_ipv6_128b_entries =
                soc_property_get(unit, spn_NUM_IPV6_LPM_128B_ENTRIES,
                        (defip_config ? 512 : 0));
        } else {
            num_ipv6_128b_entries =
                soc_property_get(unit, spn_NUM_IPV6_LPM_128B_ENTRIES,
                        (defip_config ? 1024 : 0));
        }

        config_v6_entries = num_ipv6_128b_entries;

        if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {

            num_ipv6_128b_entries = 0;
            if (!soc_feature(unit, soc_feature_l3_lpm_128b_entries_reserved)) {
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

    if (!(SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
        switch (dev_id) {
        case BCM56260_DEVICE_ID:
        case BCM56261_DEVICE_ID:
        case BCM56262_DEVICE_ID:
        case BCM56263_DEVICE_ID:
        case BCM56265_DEVICE_ID:
        case BCM56266_DEVICE_ID:
        case BCM56267_DEVICE_ID:
        case BCM56268_DEVICE_ID:
        case BCM56233_DEVICE_ID:
             sop->memState[VLAN_MACm].index_max = 4095;
             sop->memState[VLAN_XLATEm].index_max = 4095;
             /* OAM tables scaled down for SB2 */
             sop->memState[LMEPm].index_max = 511;
             sop->memState[LMEP_1m].index_max = 511;
             sop->memState[RMEPm].index_max = 2047;
             sop->memState[MA_INDEXm].index_max = 1535;
             sop->memState[EGR_MA_INDEXm].index_max = 1535;
             sop->memState[MA_STATEm].index_max = 511;
             sop->memState[MAID_REDUCTIONm].index_max = 511;
             sop->memState[ING_OAM_LM_COUNTERS_0m].index_max = 1023;
             sop->memState[EGR_OAM_LM_COUNTERS_0m].index_max = 1023;
             sop->memState[ING_OAM_LM_COUNTERS_1m].index_max = 511;
             sop->memState[EGR_OAM_LM_COUNTERS_1m].index_max = 511;
             sop->memState[ING_OAM_LM_COUNTERS_2m].index_max = 511;
             sop->memState[EGR_OAM_LM_COUNTERS_2m].index_max = 511;

            
             sop->memState[L3_ENTRY_ONLYm].index_max = 4095;
             sop->memState[L3_ENTRY_HIT_ONLYm].index_max = 255;
             sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = 4095;
             sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = 2047;
             sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = 2047;
             sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = 1023;
             if (dev_id == BCM56233_DEVICE_ID) {
                 /* L2 MAC Address : 16K */
                 sop->memState[L2Xm].index_max = 16383;
                 sop->memState[L2_ENTRY_ONLYm].index_max = 16383;
                 sop->memState[L2_HITDA_ONLYm].index_max = 2047;
                 sop->memState[L2_HITSA_ONLYm].index_max = 2047;

                 /* MPLS Labels : 2K */
                 sop->memState[MPLS_ENTRYm].index_max = 2047;

                 sop->memState[FP_TCAMm].index_max = 2047;
                 sop->memState[FP_GLOBAL_MASK_TCAMm].index_max = 2047;
                 sop->memState[FP_COUNTER_TABLEm].index_max = 2047;
                 sop->memState[FP_METER_TABLEm].index_max = 2047;
                 sop->memState[FP_POLICY_TABLEm].index_max = 2047;
                 sop->memState[ING_QUEUE_MAPm].index_max = 511;
                 sop->memState[LLS_L0_PARENTm].index_max = 63;
                 sop->memState[LLS_L0_CONFIGm].index_max = 63;
                 sop->memState[LLS_L0_MIN_CONFIG_Cm].index_max = 63;
                 sop->memState[LLS_L0_SHAPER_CONFIG_Cm].index_max = 63;
                 sop->memState[LLS_L1_PARENTm].index_max = 127;
                 sop->memState[LLS_L1_CONFIGm].index_max = 127;
                 sop->memState[LLS_L1_SHAPER_CONFIG_Cm].index_max = 127;
                 sop->memState[LLS_L2_PARENTm].index_max = 511;
                 sop->memState[LLS_L2_SHAPER_CONFIG_Cm].index_max = 127;
             }

             sop->memState[SOURCE_VPm].index_max = 4095; 
             sop->memState[EGR_DVP_ATTRIBUTEm].index_max = 4095; 
             sop->memState[ING_DVP_TABLEm].index_max = 4095; 


             sop->memState[ING_FLEX_CTR_COUNTER_TABLE_0m].index_max = 1023; 
             sop->memState[ING_FLEX_CTR_COUNTER_TABLE_1m].index_max = 1023; 
             sop->memState[ING_FLEX_CTR_COUNTER_TABLE_2m].index_max = 1023; 
             sop->memState[ING_FLEX_CTR_COUNTER_TABLE_3m].index_max = 1023; 
             sop->memState[ING_FLEX_CTR_COUNTER_TABLE_4m].index_max = 1023; 
             sop->memState[ING_FLEX_CTR_COUNTER_TABLE_5m].index_max = 1023; 
             sop->memState[ING_FLEX_CTR_COUNTER_TABLE_6m].index_max = 1023; 
             sop->memState[ING_FLEX_CTR_COUNTER_TABLE_7m].index_max = 1023; 
             sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_0m].index_max = 511; 
             sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_1m].index_max = 511; 
             sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_2m].index_max = 511; 
             sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_3m].index_max = 511; 
             sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_4m].index_max = 511; 
             sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_5m].index_max = 511; 
             sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_6m].index_max = 511; 
             sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_7m].index_max = 511; 


             sop->memState[VLAN_OR_VFI_MAC_COUNTm].index_max = 5119;
             sop->memState[VLAN_OR_VFI_MAC_LIMITm].index_max = 5119;
             sop->memState[VFIm].index_max = 1023;
             sop->memState[VFI_1m].index_max = 1023;
             sop->memState[L2MCm].index_max = 2047;


             if (dev_id == BCM56233_DEVICE_ID) {
                 /* IPMc Groups : 256 */
                 sop->memState[EGR_IPMCm].index_max = 255;
                 sop->memState[L3_IPMCm].index_max = 255;
                 sop->memState[L3_IPMC_1m].index_max = 255;
                 sop->memState[L3_IPMC_REMAPm].index_max = 255;
                 sop->memState[MMU_IPMC_GROUP_TBL0m].index_max = 255;
                 sop->memState[MMU_IPMC_GROUP_TBL1m].index_max = 255;
                 sop->memState[MMU_IPMC_GROUP_TBL2m].index_max = 255;
                 sop->memState[MMU_IPMC_GROUP_TBL3m].index_max = 255;
                 sop->memState[MMU_IPMC_GROUP_TBLm].index_max = 255;
             } else {
                 sop->memState[L3_IPMCm].index_max = 2047;
                 sop->memState[L3_IPMC_1m].index_max = 2047;
                 sop->memState[L3_IPMC_REMAPm].index_max = 2047;
                 sop->memState[MMU_IPMC_GROUP_TBLm].index_max = 2047;
             }
             sop->memState[MMU_REPL_HEAD_TBLm].index_max = 16383;
             sop->memState[MMU_REPL_LIST_TBLm].index_max = 4095;
             if (dev_id == BCM56233_DEVICE_ID) {
                 sop->memState[MMU_REPL_GRP_TBL0m].index_max = 256;
                 sop->memState[MMU_REPL_GRP_TBL1m].index_max = 256;
             } else {
                 sop->memState[MMU_REPL_GRP_TBL0m].index_max = 2047;
                 sop->memState[MMU_REPL_GRP_TBL1m].index_max = 2047;
             }
             sop->memState[MMU_EXT_MC_GROUP_MAPm].index_max = 2047;
             sop->memState[MMU_EXT_MC_QUEUE_LIST0m].index_max = 4095;
             sop->memState[MMU_EXT_MC_QUEUE_LIST4m].index_max = 4095;

             sop->memState[L3_ECMP_COUNTm].index_max = 127;
             sop->memState[INITIAL_L3_ECMP_GROUPm].index_max = 127;
             sop->memState[INITIAL_L3_ECMPm].index_max = 255;
             sop->memState[L3_ECMPm].index_max = 255;

             /* L3 Next Hop : 4K */
             sop->memState[EGR_L3_NEXT_HOPm].index_max = 4095;
             sop->memState[ING_L3_NEXT_HOPm].index_max = 4095;
             sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 4095;
             sop->memState[INITIAL_PROT_NHI_TABLEm].index_max = 4095;

             break;
        }
    }
    return rv;
}

int
_soc_saber2_tsce_firmware_set(int unit, int port, uint8 *array, int datalen)
{
    return soc_warpcore_firmware_set(unit, port, array, datalen, 0,
                                     XLPORT_WC_UCMEM_DATAm,
                                     XLPORT_WC_UCMEM_CTRLr); 
}

STATIC int
_soc_saber2_mdio_addr_to_port(int unit, uint32 phy_addr)
{
#if 0
    if (PHY_ID_BUS_NUM(phy_addr) == 1) {
        return phy_addr & 0x1f;
    } else {
        return SOC_E_PARAM;
    }
#else
if(SOC_IS_METROLITE(unit))
        return (phy_addr & 0x1f) + 4;
else
        return (phy_addr & 0x1f) + 24;

#endif
}

int
_soc_saber2_sbus_tsc_block(int unit,
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
                             "_soc_saber2_sbus_tsc_block(u=%d,pp=%d,mem=%d, blk=%d): "
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


/* to check if this PHY register access sbus MDIO request is for
 * QSGMII register.
 */
STATIC int
_soc_saber2_qsgmii_reg_access_check(int unit,
                            uint32 port, uint32 phyreg, int *to_qsgmii)
{
    *to_qsgmii = 0;
    if (PHY_REG_ADDR_DEVAD(phyreg) == 0){  /* DEVAD==0 for PCS */
        if (IS_QSGMII_REGISTER(phyreg)) {
            *to_qsgmii = 1;
        } else if ((QSGMII_REG_ADDR_REG(phyreg) < 0x10) &&
                    SABER2_PORT_IS_QSGMII(unit, port)) {
            /* means QSGMII port on accessing IEEE registers */
            *to_qsgmii = 1;
        }
    }

    return SOC_E_NONE;
}
STATIC int
_soc_saber2_indacc_read(int unit, uint32 phy_addr,
                           uint32 phy_reg, uint32 *phy_data)
{
    uint32 ctlsts;
    int ts;
    uint32  reg_addr = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_indacc_read"
                          "(%d,0x%x,0x%08x,*phy_data)..\n"),
               unit, phy_addr, phy_reg));

    SOC_IF_ERROR_RETURN(
        _soc_saber2_indacc_addr(phy_reg, &ts));

    ctlsts = 0;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, TARGET_SELECTf, ts);
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, RD_REQf, 1);

    /* assigning the reg_addr in Clause22 format */
    reg_addr = phy_reg & 0x1f;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, ADDRESSf, reg_addr);

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_indacc_read:"
                          "INDACC_CTLSTSr=0x%08x\n"), ctlsts));
    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_CTLSTSr(unit, REG_PORT_ANY, ctlsts));

    SOC_IF_ERROR_RETURN(
        _soc_saber2_indacc_wait(unit, RD_RDYf));

    SOC_IF_ERROR_RETURN(
        READ_CHIP_INDACC_RDATAr(unit, REG_PORT_ANY, phy_data));

    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_CTLSTSr(unit, REG_PORT_ANY, 0));

    return SOC_E_NONE;
}


STATIC int
_soc_saber2_indacc_wait(int unit, soc_field_t fwait)
{
    uint32 ctlsts;
    int poll = 1000;

    do {
        SOC_IF_ERROR_RETURN(
            READ_CHIP_INDACC_CTLSTSr(unit, REG_PORT_ANY, &ctlsts));
        if (soc_reg_field_get(unit, CHIP_INDACC_CTLSTSr, ctlsts, fwait)) {
            break;
        }
    } while (--poll > 0);

    if (poll <= 0) {
        return SOC_E_TIMEOUT;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_saber2_indacc_addr(uint32 phy_reg, int *target_select)
{
    if (QSGMII_REG_ADDR_LANE(phy_reg) <= 15) {
        *target_select = (QSGMII_REG_ADDR_LANE(phy_reg) < 8) ? 0 : 1;
        return SOC_E_NONE;
    }

    return SOC_E_INTERNAL;
}

STATIC int
_soc_saber2_indacc_write(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 phy_data)
{
    uint32 ctlsts;
    int ts;
    uint32  reg_addr = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_indacc_write"
                          "(%d,0x%x,0x%08x,0x%04x)..\n"),
               unit, phy_addr, phy_reg, phy_data));
    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_WDATAr(unit, REG_PORT_ANY, phy_data));

    SOC_IF_ERROR_RETURN(
        _soc_saber2_indacc_addr(phy_reg, &ts));

    ctlsts = 0;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, TARGET_SELECTf, ts);
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, WR_REQf, 1);
    /* assigning the reg_addr in Clause22 format */
    reg_addr = phy_reg & 0x1f;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, ADDRESSf, reg_addr);

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_indacc_write:"
                          "INDACC_CTLSTSr=0x%08x\n"), ctlsts));
    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_CTLSTSr(unit, REG_PORT_ANY, ctlsts));

    SOC_IF_ERROR_RETURN(
        _soc_saber2_indacc_wait(unit, WR_RDYf));

    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_CTLSTSr(unit, REG_PORT_ANY, 0));

    return SOC_E_NONE;
}

STATIC int
_soc_saber2_sbus_qsgmii_write(int unit,
                uint32 phy_addr, uint32 phy_reg, uint32 phy_data)
{
    uint32  reg_addr, reg_data;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_sbus_qsgmii_write"
                          "(%d,0x%x,0x%08x,0x%04x)..\n"),
               unit, phy_addr, phy_reg, phy_data));

    /* The "phy_reg" in Sbus MDIO access is expected in 32 bits PHY address
     *  format with AER information included. Since this interface allows
     *  MDIO access in Claue22 only, the AER process must be applied.
     */

    /* AER process : AER block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = 0xffd0;
    SOC_IF_ERROR_RETURN(_soc_saber2_indacc_write(unit,
            phy_addr, reg_addr, reg_data));

    /* AER process : lane control */
    reg_addr = 0x1e;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    /* lane control to indicated lane 0~7 for each QSGMIMI core */
    reg_data = QSGMII_REG_ADDR_LANE(phy_reg) & 0x7;
    SOC_IF_ERROR_RETURN(_soc_saber2_indacc_write(unit,
            phy_addr, reg_addr, reg_data));

    /* target register block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = phy_reg & 0xfff0;
    SOC_IF_ERROR_RETURN(_soc_saber2_indacc_write(unit,
            phy_addr, reg_addr, reg_data));

    /* write data */
    reg_addr = QSGMII_REG_ADDR_REG(phy_reg);
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    SOC_IF_ERROR_RETURN(_soc_saber2_indacc_write(unit,
            phy_addr, reg_addr, phy_data));

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_sbus_qsgmii_write()... Done!")));

    return SOC_E_NONE;
}


STATIC int
_soc_saber2_sbus_qsgmii_read(int unit,
                uint32 phy_addr, uint32 phy_reg, uint32 *phy_data)
{
    uint32  reg_addr, reg_data;

    *phy_data = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_sbus_qsgmii_read"
                          "(%d,0x%x,0x%08x,*phy_data)..\n"),
               unit, phy_addr, phy_reg));

    /* The "phy_reg" in Sbus MDIO access is expected in 32 bits PHY address
     *  format with AER information included. Since this interface allows
     *  MDIO access in Claue22 only, the AER process must be applied.
     */

    /* AER process : AER block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = 0xffd0;
    SOC_IF_ERROR_RETURN(_soc_saber2_indacc_write(unit,
            phy_addr, reg_addr, reg_data));

    /* AER process : lane control */
    reg_addr = 0x1e;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    /* lane control to indicated lane 0~7 for each QSGMIMI core */
    reg_data = QSGMII_REG_ADDR_LANE(phy_reg) & 0x7;
    SOC_IF_ERROR_RETURN(_soc_saber2_indacc_write(unit,
            phy_addr, reg_addr, reg_data));

    /* target register block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = phy_reg & 0xfff0;
    SOC_IF_ERROR_RETURN(_soc_saber2_indacc_write(unit,
            phy_addr, reg_addr, reg_data));

    /* read data */
    reg_addr = QSGMII_REG_ADDR_REG(phy_reg);
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    SOC_IF_ERROR_RETURN(_soc_saber2_indacc_read(unit,
            phy_addr, reg_addr, &reg_data));

    *phy_data = reg_data;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_sbus_qsgmii_read()...data=0x%04x\n"),
               *phy_data));

    return SOC_E_NONE;
}

int
_soc_saber2_mdio_reg_write(int unit, uint32 phy_addr,
                              uint32 phy_reg, uint32 phy_data)
{
    int rv = SOC_E_NONE;
    int port, phy_port, blk;
    int qsgmii_reg = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_mdio_reg_write(%d,0x%x,0x%08x,0x%04x)..\n"),
               unit, phy_addr, phy_reg,phy_data));
    /* Physical port based on MDIO address */
    phy_port = _soc_saber2_mdio_addr_to_port(unit, phy_addr);
    /* No Physical to Logical Port Mapping so direct 1 to 1 */
    port = phy_port;
    SOC_IF_ERROR_RETURN(_soc_saber2_sbus_tsc_block(unit,
                        phy_port, XLPORT_WC_UCMEM_DATAm, &blk));

    /* access interface selection */
    SOC_IF_ERROR_RETURN(_soc_saber2_qsgmii_reg_access_check(unit,
                        port, phy_reg, &qsgmii_reg));
    if (qsgmii_reg) {
        MIIM_LOCK(unit);
        rv = _soc_saber2_sbus_qsgmii_write(unit, phy_addr,
                                        phy_reg, phy_data);
        MIIM_UNLOCK(unit);
    } else {
        TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
        rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr,
                                    phy_reg, phy_data);
    }
    return rv;
}

int
_soc_saber2_mdio_reg_read(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 *phy_data)
{
    int rv = SOC_E_NONE;
    int port, phy_port, blk;
    int qsgmii_reg = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_saber2_mdio_reg_read(%d,0x%x,0x%08x,*phy_data)..\n"),
               unit, phy_addr, phy_reg));
    /* Physical port based on MDIO address */
    phy_port = _soc_saber2_mdio_addr_to_port(unit, phy_addr);
    /* No Physical to Logical Port Mapping so direct 1 to 1 */
    port = phy_port;
    SOC_IF_ERROR_RETURN(_soc_saber2_sbus_tsc_block(unit,
                        phy_port, XLPORT_WC_UCMEM_DATAm, &blk));

    /* access interface selection */
    SOC_IF_ERROR_RETURN(_soc_saber2_qsgmii_reg_access_check(unit,
                        port, phy_reg, &qsgmii_reg));
    if (qsgmii_reg) {
        MIIM_LOCK(unit);
#if 0
        /* WARMBOOT STUFF */
        SOC_ALLOW_WB_WRITE(unit, _soc_saber2_sbus_qsgmii_read(unit, phy_addr,
                                        phy_reg, phy_data), rv);
#endif
        SOC_IF_ERROR_RETURN(_soc_saber2_sbus_qsgmii_read(unit, phy_addr,
                                        phy_reg, phy_data));
        MIIM_UNLOCK(unit);
    } else {
        TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
        rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr,
                                   phy_reg, phy_data);
    }
    return rv;
}



/*
 * Saber chip driver functions.
 */
soc_functions_t soc_saber2_drv_funs = {
    _soc_saber2_misc_init,          /* soc_misc_init_f */
    _soc_saber2_mmu_init,           /* soc_mmu_init_f */
    _soc_saber2_age_timer_get,      /* soc_age_timer_get_f */
    _soc_saber2_age_timer_max_get,  /* soc_age_timer_max_get_f */
    _soc_saber2_age_timer_set,      /* soc_age_timer_set_f */
    _soc_saber2_tsce_firmware_set,  /* soc_phy_firmware_load_f */
    _soc_saber2_mdio_reg_read,      /* soc_sbus_mdio_read_f */
    _soc_saber2_mdio_reg_write      /* soc_sbus_mdio_write_f */
};



/* Saber2 pbmp init sequence */
int _soc_saber2_get_cfg_num(int unit, int *new_cfg_num) 
{
    int cfg_num=-1;
    int max_cfg_num=0;
    int def_cfg_num=0;
    int offset_cfg_num=0;
    uint16 dev_id=0;
    uint8  rev_id=0;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        cfg_num = COUNTOF(bcm56260_tdm_info) - 1;
        if (bcm56260_tdm_info[cfg_num].tdm_size != 0) {
            *new_cfg_num = cfg_num;
            return SOC_E_NONE;       
        }
        /* FAll thru */
    }
#endif
    cfg_num = soc_property_get(unit, spn_BCM5626X_CONFIG,0xFF); 
    
    soc_cm_get_id(unit, &dev_id, &rev_id);
    switch(dev_id) {
    case BCM56260_DEVICE_ID:
    case BCM56261_DEVICE_ID:
    case BCM56265_DEVICE_ID:
    case BCM56266_DEVICE_ID:
         def_cfg_num = BCM56260_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56260_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56260_DEVICE_ID_MAX_CFG;
         break;
    case BCM56262_DEVICE_ID:
    case BCM56267_DEVICE_ID:
         def_cfg_num = BCM56262_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56262_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56262_DEVICE_ID_MAX_CFG;
         break;

    case BCM56263_DEVICE_ID:
    case BCM56268_DEVICE_ID:
         def_cfg_num = BCM56263_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56263_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56263_DEVICE_ID_MAX_CFG;
         break;

    case BCM56460_DEVICE_ID:
    case BCM56461_DEVICE_ID:
    case BCM56465_DEVICE_ID:
    case BCM56466_DEVICE_ID:
         def_cfg_num = BCM56460_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56460_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56460_DEVICE_ID_MAX_CFG;
         break;
    case BCM56462_DEVICE_ID:
    case BCM56467_DEVICE_ID:
         def_cfg_num = BCM56462_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56462_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56462_DEVICE_ID_MAX_CFG;
         break;
    case BCM56463_DEVICE_ID:
    case BCM56468_DEVICE_ID:
         def_cfg_num = BCM56463_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56463_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56463_DEVICE_ID_MAX_CFG;
         break;

    case BCM56233_DEVICE_ID:
         def_cfg_num = BCM56233_DEVICE_ID_DEFAULT_CFG;
         offset_cfg_num = BCM56233_DEVICE_ID_OFFSET_CFG;
         max_cfg_num = BCM56233_DEVICE_ID_MAX_CFG;
         break;

         /* Not supported yet */
         return SOC_E_UNAVAIL; 
    }

    if (cfg_num == 0xFF) {
        *new_cfg_num = def_cfg_num;
    } else {
        if (cfg_num == 0) {
   LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Config=0 is not allowed \n")));
            return SOC_E_PARAM;
        }   
        *new_cfg_num = cfg_num + offset_cfg_num;
    }           
    if (*new_cfg_num > max_cfg_num) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Cfg_num:%d Internal cfg_num:%d \n"),cfg_num,*new_cfg_num));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, 
                   "Wrong %s ==> cfg_num:%d DEFAULT:%d START:%d MAX:%d \n"),
                   spn_BCM5626X_CONFIG, cfg_num,
                   def_cfg_num-offset_cfg_num,1,max_cfg_num-offset_cfg_num));
        *new_cfg_num = def_cfg_num;
        return SOC_E_NONE; 
    }              
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit, 
               "Info Cfg_num:%d Internal cfg_num:%d \n"),cfg_num,*new_cfg_num));

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

uint32  *iarb_tdm[SOC_MAX_NUM_DEVICES];

int soc_saber2_retrieve_hw_tdm(int unit)
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

    if(iarb_tdm[unit] != NULL) {
        sal_free(iarb_tdm[unit]);
        iarb_tdm[unit] = NULL;
    }
    iarb_tdm[unit] = sal_alloc(iarb_size, "iarb_tdm");
    if (iarb_tdm[unit] == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(iarb_tdm[unit], 0, iarb_size);

    for (index = 0; index < num_iarb_entry; index++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, tdm_table[tdm_no], SOC_BLOCK_ALL,
                    index, &iarb_tdm_entry));
        iarb_tdm[unit][index] = soc_mem_field32_get(unit, tdm_table[tdm_no],
                &iarb_tdm_entry, PORT_NUMf);
    }
    return SOC_E_NONE;
}


/* Functions related to flexio scache */
int _soc_saber2_flexio_scache_allocate(int unit)
{
    int                 rv=SOC_E_NONE;
    uint8               *flexio_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get=0;
    uint32 alloc_size =  sizeof(scache_begin_magic_number) +
                         sizeof(sb2_scache_tdm_info) +
                         sizeof(sb2_scache_tdm) +
                         sizeof(sb2_scache_speed) +
                         sizeof(sb2_scache_port_details) +
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

int _soc_saber2_flexio_scache_retrieve(int unit)
{
    int                 rv, port;
    uint8               *flexio_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get=0;
    uint32 alloc_size =  sizeof(sb2_scache_tdm_info) +
                         sizeof(sb2_scache_tdm) +
                         sizeof(sb2_scache_speed) +
                         sizeof(sb2_scache_port_details) +
                         SOC_WB_SCACHE_CONTROL_SIZE;
    uint32              scache_offset=0;
    uint16              default_ver = BCM_WB_DEFAULT_VERSION;
    uint16              recovered_ver;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE,
            BCM_FLEXIO_SCACHE_DEFAULT_PARTITION);

    rv = soc_scache_ptr_get(unit, scache_handle,
                                &flexio_scache_ptr, &alloc_get);
    if (rv == SOC_E_NOT_FOUND) {
        /* Partition 1 is not available, fall back to old retrieve procedure */
        SOC_IF_ERROR_RETURN(_soc_saber2_flexio_scache_retrieve_part0(unit));
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
    sal_memcpy(&sb2_scache_tdm_info,
               &flexio_scache_ptr[scache_offset], sizeof(sb2_scache_tdm_info));

    bcm56260_tdm_info[COUNTOF(bcm56260_tdm_info)-1] = sb2_scache_tdm_info;

    scache_offset += sizeof(sb2_scache_tdm_info);
    sal_memcpy(&sb2_scache_tdm,
               &flexio_scache_ptr[scache_offset], sizeof(sb2_scache_tdm));

    scache_offset += sizeof(sb2_scache_tdm);
    sal_memcpy(&sb2_scache_speed,
               &flexio_scache_ptr[scache_offset], sizeof(sb2_scache_speed));

    scache_offset += sizeof(sb2_scache_speed);
    sal_memcpy(&sb2_scache_port_details,
               &flexio_scache_ptr[scache_offset],
               sizeof(sb2_scache_port_details));
    scache_offset += sizeof(sb2_scache_port_details);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    if ((default_ver >= BCM_WB_VERSION_1_4) &&
        (recovered_ver < BCM_WB_VERSION_1_4)) {
        for (port=25 ; port <= SB2_MAX_PHYSICAL_PORTS ; port++) {
            if ((sb2_scache_port_details[port-1].port_type & GE_PORT) &&
                sb2_2p5g_ports[port-1]) {
                sb2_scache_speed[port - 1] = 2500;
                sb2_scache_port_details[port-1].port_speed = 2500;
            }
        }
    }

    return SOC_E_NONE;
}


int _soc_saber2_flexio_scache_retrieve_part0(int unit)
{
    int                 rv;
    uint8               *flexio_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get=0;
    uint32 alloc_size =  sizeof(scache_begin_magic_number) +
                         sizeof(sb2_scache_tdm_info) +
                         sizeof(sb2_scache_tdm) +
                         sizeof(sb2_scache_speed) +
                         sizeof(sb2_scache_port_details) +
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
    sal_memcpy(&sb2_scache_tdm_info,
               &flexio_scache_ptr[scache_offset], sizeof(sb2_scache_tdm_info));

    bcm56260_tdm_info[COUNTOF(bcm56260_tdm_info)-1] = sb2_scache_tdm_info;

    scache_offset += sizeof(sb2_scache_tdm_info);

    /*
     * Part 0 Version has an issue in TDM sync.
     * So in part 0 version, Level 1 recovery performed to retrieve TDM.
     */
    LOG_INFO(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                        "Previous ver was synced with incorrect TDM,"
                        "Now LEVEL 1 recovery for TDM\n")));

    SOC_IF_ERROR_RETURN(soc_saber2_retrieve_hw_tdm(unit));
    if(iarb_tdm[unit] == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&sb2_scache_tdm, iarb_tdm[unit], sizeof(sb2_scache_tdm));

    kt2_tdm_display(unit, (uint32 *) (&sb2_scache_tdm[0]),
            sb2_scache_tdm_info.tdm_size,
            sb2_scache_tdm_info.row,
            sb2_scache_tdm_info.col);

    scache_offset += sizeof(sb2_scache_tdm);
    sal_memcpy(&sb2_scache_speed,
               &flexio_scache_ptr[scache_offset], sizeof(sb2_scache_speed));

    scache_offset += sizeof(sb2_scache_speed);
    sal_memcpy(&sb2_scache_port_details,
               &flexio_scache_ptr[scache_offset],
               sizeof(sb2_scache_port_details));
    scache_offset += sizeof(sb2_scache_port_details);
    
    if (sal_memcmp(&flexio_scache_ptr[scache_offset],
                   &scache_end_magic_number[0], 
                   sizeof(scache_end_magic_number)) != 0) {
        LOG_CLI((BSL_META_U(unit,
                            "Corrupt scache for flexio operation \n")));
        return SOC_E_INTERNAL;
    }              
    
    /* Allocate SCACHE at part 1 Now */
    SOC_IF_ERROR_RETURN(_soc_saber2_flexio_scache_allocate(unit));

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));
        
    return SOC_E_NONE;
}

int _soc_saber2_flexio_scache_sync(int unit)
{
    int                 rv;
    uint8               *flexio_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get=0;
    uint32 alloc_size =  sizeof(sb2_scache_tdm_info) +
                         sizeof(sb2_scache_tdm) +
                         sizeof(sb2_scache_speed) +
                         sizeof(sb2_scache_port_details) +
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
    
    SOC_IF_ERROR_RETURN(_soc_saber2_get_cfg_num(unit, &cfg_num));
    _sb2_config_id[unit] = cfg_num;

    scache_offset = 0;
    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &default_ver, sizeof(uint16));

    scache_offset += SOC_WB_SCACHE_CONTROL_SIZE;
    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &bcm56260_tdm_info[cfg_num],
               sizeof(sb2_scache_tdm_info));

    scache_offset += sizeof(sb2_scache_tdm_info);
    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &sb2_current_tdm[0],
               sizeof(uint32)* bcm56260_tdm_info[cfg_num].tdm_size);

    scache_offset += sizeof(sb2_scache_tdm);

    for (port=1 ; port <= SB2_MAX_PHYSICAL_PORTS ; port++) {
        sb2_scache_speed[port-1] = 0;

        if (IS_MXQ_PORT(unit,port)) {
            if ((bcm56260_speed[unit][cfg_num][port-1] == 0) || /*Converted */
                (bcm56260_speed[unit][cfg_num][port-1] >= 2500)) {
                    sb2_scache_speed[port - 1] = 2500;
            } else {
                sb2_scache_speed[port - 1] = 1000;
            }
        }

        if (IS_XE_PORT(unit,port)) {
            sb2_scache_speed[port - 1] = 10000;
        }  
        
        if (IS_HG_PORT(unit,port)) {
             sb2_scache_speed[port - 1] = 10000;
        }

        if (IS_XL_PORT(unit, port) && (IS_GE_PORT(unit, port))) {
            if (bcm56260_speed[unit][cfg_num][port-1] >= 2500) {
                sb2_scache_speed[port - 1] = 2500;
            } else {
                sb2_scache_speed[port - 1] = 1000;
            }
        }
    }

    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &sb2_scache_speed[0], sizeof(sb2_scache_speed));
    scache_offset += sizeof(sb2_scache_speed);

    for (port=1 ; port <= SB2_MAX_PHYSICAL_PORTS ; port++) {
         /* mxqblock = (*kt2_port_to_mxqblock[unit])[port-1]; */
         sb2_scache_port_details[port-1].start_port_no = port;
         sb2_scache_port_details[port-1].end_port_no = port;
         sb2_scache_port_details[port-1].port_incr = 1;
         sb2_scache_port_details[port-1].port_type = 0;
         sb2_scache_port_details[port-1].port_speed = 0xFFFF;

         if (IS_MXQ_PORT(unit,port)) {
             if ((bcm56260_speed[unit][cfg_num][port-1] == 0) || /*Converted */
                 (bcm56260_speed[unit][cfg_num][port-1] >= 2500)) {
                  sb2_scache_port_details[port-1].port_speed = 2500;
             } else {
                  sb2_scache_port_details[port-1].port_speed = 1000;
             }
             sb2_scache_port_details[port-1].port_type = GE_PORT; 
         }        

         if (IS_XE_PORT(unit,port)) {
             sb2_scache_port_details[port-1].port_speed = 10000;
             sb2_scache_port_details[port-1].port_type |= XE_PORT;
             if (IS_MXQ_PORT(unit,port)) {
                 sb2_scache_port_details[port-1].port_incr = 4;
             }
         }   

         if (IS_HG_PORT(unit,port)) {
             sb2_scache_port_details[port-1].port_speed = 10000;
             sb2_scache_port_details[port-1].port_type |= HG_PORT;
             sb2_scache_port_details[port-1].port_type |= STK_PORT;
         }

        if (IS_XL_PORT(unit, port) && (IS_GE_PORT(unit, port))) {
            if (bcm56260_speed[unit][cfg_num][port-1] >= 2500) {
                sb2_scache_port_details[port-1].port_speed = 2500;
            } else {
                sb2_scache_port_details[port-1].port_speed = 1000;
            }
            sb2_scache_port_details[port-1].port_type |= GE_PORT;
        }
    }
    sal_memcpy(&flexio_scache_ptr[scache_offset],
               &sb2_scache_port_details,
               sizeof(sb2_scache_port_details));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));
    return SOC_E_NONE;
}
#endif

/* Function to return the speed based on the cfg_num */
int soc_saber2_xl_port_speed_get(int unit, int port, int *speed) {
    int                 cfg_num=0, max_speed=0;
    soc_pbmp_t          my_pbmp_xport_xe;
    soc_info_t          *si=&SOC_INFO(unit);

    SOC_PBMP_CLEAR(my_pbmp_xport_xe);
    my_pbmp_xport_xe = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_XE,
                                                 my_pbmp_xport_xe);

    SOC_IF_ERROR_RETURN(_soc_saber2_get_cfg_num(unit, &cfg_num));
    max_speed = bcm56260_speed[unit][cfg_num][port-1];

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
    }

    if ((max_speed == 10000) && !(SOC_PBMP_MEMBER(my_pbmp_xport_xe, port))) {
        *speed = 11000;
        if(eagle_n4s_mode) {
            /* In mixed mode, the hg ports would come up in single
             * or dual mode. Either way xe and hg ports would be 10G. */
            *speed = 10000;
        }
    } else {
        *speed = (max_speed  > si->port_speed_max[port]) ? max_speed :
            si->port_speed_max[port];
    }

    return SOC_E_NONE;
}

/* Functions related to initlializing sb2 port bitmap */
void
soc_saber2_pbmp_init(int unit, sb2_pbmp_t sb2_pbmp) 
{
    int port;
    soc_pbmp_t          my_pbmp_xport_xe;
    soc_pbmp_t          my_pbmp_xport_ge;
    uint32              loop_index=0, port_speed;
    int                 sb2_port_details_index=0;
    uint8               sb2_port_used_flags[32]={0};
    int                 old_num_lanes, num_lanes, port_incr;
    uint16              dev_id=0;
    uint8               rev_id=0;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Unit Specific Allocation */
    if (sb2_port_speeds[unit] != NULL) {
        sal_free(sb2_port_speeds[unit]);
        sb2_port_speeds[unit] = NULL;
    }
    sb2_port_speeds[unit] =  sal_alloc(sizeof(sb2_port_speeds_t),
            "sb2_port_speeds_t");
    if (sb2_port_speeds[unit] == NULL) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                   FUNCTION_NAME(),  __LINE__,
                   (int)sizeof(sb2_port_speeds_t)));
        return;
    }

    if (bcm56260_speed[unit] != NULL) {
        sal_free(bcm56260_speed[unit]);
        bcm56260_speed[unit] = NULL;
    }
    bcm56260_speed[unit] =  sal_alloc(sizeof(sb2_speed_t) *
                                      (sizeof(bcm56260_speed_s) /
                                       sizeof(bcm56260_speed_s[0])),
                                      "bcm56260_speed");
    if (bcm56260_speed[unit] == NULL) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                   FUNCTION_NAME(),  __LINE__, 
                   (int)sizeof(mxqspeeds_t)));
        return ;
    }

    for (loop_index = 0 ; 
         loop_index < sizeof(bcm56260_speed_s ) /
                      sizeof(bcm56260_speed_s[0]);
         loop_index++) {
         sal_memcpy(bcm56260_speed[unit][loop_index],
                    bcm56260_speed_s[loop_index],
                    sizeof(sb2_speed_t));
    }    

    if (sb2_port_to_block[unit] != NULL) {
        sal_free(sb2_port_to_block[unit]);
        sb2_port_to_block[unit] = NULL;
    }
    sb2_port_to_block[unit] =  sal_alloc(sizeof(sb2_port_to_block_s),
            "sb2_port_to_block_t");
    if (sb2_port_to_block[unit] == NULL) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                   FUNCTION_NAME(),  __LINE__, 
                   (int)sizeof(sb2_port_to_block)));
        return ;
    }
    sal_memcpy(sb2_port_to_block[unit],sb2_port_to_block_s,
            sizeof(sb2_port_to_block_s));

    if (sb2_port_to_block_subports[unit] != NULL) {
        sal_free(sb2_port_to_block_subports[unit]);
        sb2_port_to_block_subports[unit] = NULL;
    }
    sb2_port_to_block_subports[unit] = 
        sal_alloc(sizeof(sb2_port_to_block_subports_s),
                "sb2_port_to_block_t");
    if (sb2_port_to_block_subports[unit] == NULL) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                   FUNCTION_NAME(),  __LINE__,
                   (int) sizeof(sb2_port_to_block_subports)));
        return ;
    }
    sal_memcpy(sb2_port_to_block_subports[unit],
            sb2_port_to_block_subports_s,sizeof(sb2_port_to_block_subports_s));

    if (sb2_block_ports[unit] != NULL) {
        sal_free(sb2_block_ports[unit]);
        sb2_block_ports[unit] = NULL;
    }
    sb2_block_ports[unit] =  sal_alloc(sizeof(sb2_block_ports_s),
            "sb2_block_ports_t");
    if (sb2_block_ports[unit] == NULL) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Resource issue %s:%d..Couldn't allocate memory=%d \n"),
                   FUNCTION_NAME(),  __LINE__,
                   (int)sizeof(sb2_block_ports)));
        return ;
    }
    sal_memcpy(sb2_block_ports[unit],sb2_block_ports_s,
            sizeof(sb2_block_ports_s));

    /* Initialize PBMPs */
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_gport_stack);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_mxq);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_mxq1g);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_mxq2p5g);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_mxq10g);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_xport_xe);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_xport_ge);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_xl);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_xl1g);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_xl2p5g);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_xl10g);
    /* SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_valid); */
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_pp);
    SOC_PBMP_CLEAR(*sb2_pbmp.pbmp_linkphy);

    if (_soc_saber2_get_cfg_num(unit, &sb2_port_details_index) != SOC_E_NONE) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "Retrieved WRONG CFG VALUE =%d\n"), sb2_port_details_index));
        return ;
    }

    if (sb2_port_details_index >= (sizeof(sb2_port_details)/
                                   sizeof(sb2_port_details[0]))) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                   "INTERNAL: WRONG CFG VALUE =%d\n"), sb2_port_details_index));
        return ;
    }
    LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
            "Internal Config Number : %d\n"), sb2_port_details_index));

    _sb2_config_id[unit] = sb2_port_details_index;
    
    switch(dev_id) {
        case BCM56260_DEVICE_ID:
        case BCM56261_DEVICE_ID:
        case BCM56262_DEVICE_ID:
        case BCM56263_DEVICE_ID:
        case BCM56265_DEVICE_ID:
        case BCM56266_DEVICE_ID:
        case BCM56267_DEVICE_ID:
        case BCM56268_DEVICE_ID:
            sal_memcpy(sb2_port_speeds[unit], sb2_port_speeds_56260_1,
                        sizeof(sb2_port_speeds_t));
            break;

        case BCM56460_DEVICE_ID:
        case BCM56461_DEVICE_ID:
        case BCM56462_DEVICE_ID:
        case BCM56463_DEVICE_ID:
        case BCM56465_DEVICE_ID:
        case BCM56466_DEVICE_ID:
        case BCM56467_DEVICE_ID:
        case BCM56468_DEVICE_ID:
            sal_memcpy(sb2_port_speeds[unit], sb2_port_speeds_56460_1,
                        sizeof(sb2_port_speeds_t));
            break;

        case BCM56233_DEVICE_ID:
            sal_memcpy(sb2_port_speeds[unit], sb2_port_speeds_56233_1,
                        sizeof(sb2_port_speeds_t));
            break;

        default:
            sal_memcpy(sb2_port_speeds[unit], sb2_port_speeds_56260_0,
                        sizeof(sb2_port_speeds_t));
            break;
    };

    SOC_PBMP_CLEAR(my_pbmp_xport_xe);
    SOC_PBMP_CLEAR(my_pbmp_xport_ge);
    if (!(SOC_WARM_BOOT(unit))) {
        eagle_n4s_mode = 0;
        eagle_n2_mode = 0;
        eagle_4ge_mode = 0;
        my_pbmp_xport_xe = soc_property_get_pbmp_default(unit,
                                      spn_PBMP_XPORT_XE, my_pbmp_xport_xe);
        my_pbmp_xport_ge = soc_property_get_pbmp_default(unit,
                                      spn_PBMP_XPORT_GE, my_pbmp_xport_ge);
    }

    if (sb2_selected_port_details != NULL) {
        sal_free(sb2_selected_port_details);
        sb2_selected_port_details = NULL;
        sal_free(sb2_selected_port_details_orig);
        sb2_selected_port_details_orig = NULL;
    }

    /* Note : size of sb2_port_details of all configs is same (72 bytes) */
    sb2_selected_port_details = sal_alloc(
                        sizeof(soc_port_details_t) * (SB2_MAX_PHYSICAL_PORTS+1),
                        "sb2_block_ports_t");

    sb2_selected_port_details_orig = sal_alloc(
                        sizeof(soc_port_details_t) * (SB2_MAX_PHYSICAL_PORTS+1),
                        "sb2_block_ports_t");

    if ((sb2_selected_port_details == NULL) ||
            (sb2_selected_port_details_orig == NULL)) {
        LOG_DEBUG(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
           "INTERNAL: Memory allocation failed for sb2_selected_port_details\n")));
        return ;
    }

    sal_memcpy(sb2_selected_port_details,
            sb2_port_details[sb2_port_details_index],
            sizeof(soc_port_details_t) * (SB2_MAX_PHYSICAL_PORTS+1));

    /* Keep the default config to compare later */
    sal_memcpy(sb2_selected_port_details_orig,
            sb2_port_details[sb2_port_details_index],
            sizeof(soc_port_details_t) * (SB2_MAX_PHYSICAL_PORTS+1));

    if (!(SOC_WARM_BOOT(unit))) {
        /* Construct port bitmaps based on portgroup settings */
        /* In case of warmboot, maintain saved settings */
        loop_index=0;
        port_incr = 4;

        for(port = 1; port <= SB2_MAX_PHYSICAL_PORTS; port += port_incr) {
            loop_index = (port / 4);

            /* Modify only if the port is not disabled by default */
            if (sb2_selected_port_details[loop_index].port_speed == 0) {
                continue;
            }
 
            old_num_lanes = sb2_selected_port_details[loop_index].port_incr;
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

            if(port < 25) {
                /* Viper ports */
                switch(num_lanes) {
                    case PORTGROUP_SINGLE_MODE:
                        if((old_num_lanes == 1) &&
                            (sb2_selected_port_details[loop_index].port_speed == 2500 )) {
                            sb2_selected_port_details[loop_index].port_type = XE_PORT;
                            sb2_selected_port_details[loop_index].port_incr = 4;
                            sb2_selected_port_details[loop_index].port_speed = 10000;
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
                            sb2_selected_port_details[loop_index].port_incr = 1;
                            sb2_selected_port_details[loop_index].port_speed = 2500;
                            sb2_selected_port_details[loop_index].port_type = GE_PORT;
                        }
                        break;
                    default : return; /* Dummy statement */
                }
            } else {
                /* Eagle ports */
                port_incr = 2;
                if ((port == 27) || (port == 28)) {
                    loop_index += 1;
                }

                switch(num_lanes) {
                    case PORTGROUP_SINGLE_MODE:
                        if(sb2_selected_port_details[loop_index].port_type & GE_PORT) {
                            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                               "Invalid PortGroup config for port %d ; BW exceeds Max port speed\n"), port));
                            break;
                        } else if (SOC_PBMP_MEMBER(my_pbmp_xport_xe, port)) {
                            sb2_selected_port_details[loop_index].port_type = XE_PORT;
                        } else {
                            sb2_selected_port_details[loop_index].port_type = HG_PORT;
                        }
                        sb2_selected_port_details[loop_index].port_speed = 10000;
                        sb2_selected_port_details[loop_index].port_incr = 4;
                        sb2_selected_port_details[loop_index].end_port_no = 28;
                        sb2_selected_port_details[loop_index + 1].port_type = 0;
                        port_incr = 4; /* All eagle ports are initialized */
                        break;
                    case PORTGROUP_DUAL_MODE:
                        sb2_selected_port_details[loop_index].port_type = XE_PORT;
                        sb2_selected_port_details[loop_index].port_speed = 10000;
                        sb2_selected_port_details[loop_index].port_incr = 2;
                        sb2_selected_port_details[loop_index+1].port_type = XE_PORT;
                        sb2_selected_port_details[loop_index+1].port_speed = 10000;
                        sb2_selected_port_details[loop_index+1].port_incr = 2;
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

    /* Fill in pbmps */
    loop_index=0;
    while(sb2_selected_port_details[loop_index].start_port_no != 0) {
          LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, 
                       "start:%d end:%d incr:%d type:%d speed:%d\n"),
                       sb2_selected_port_details[loop_index].start_port_no,
                       sb2_selected_port_details[loop_index].end_port_no,
                       sb2_selected_port_details[loop_index].port_incr,
                       sb2_selected_port_details[loop_index].port_type,
                       sb2_selected_port_details[loop_index].port_speed));

        /*
         * Understanding eagle port details:
         * - N4 and 4xGE - port_incr = 1,
         * - N2 - port_incr = 2,
         * - N4s - 2 entries(25,27 and 26,28) each with port_incr = 2.
         */
        for (port =  sb2_selected_port_details[loop_index].start_port_no;
           port <= sb2_selected_port_details[loop_index].end_port_no;
           port += sb2_selected_port_details[loop_index].port_incr) {

            port_speed = sb2_selected_port_details[loop_index].port_speed;
            if((dev_id == BCM56233_DEVICE_ID) && (port == 4)) {
                port_speed = 2500;
            }
            if((dev_id == BCM56233_DEVICE_ID) &&
                    (soc_property_get(unit, spn_SAT_ENABLE, 0) == 0) &&
                    (port == 5)) {
                /* In Dagger2, port 5 is hidden. Available only as SAT port */
                sb2_selected_port_details[loop_index].port_type = 0;
            }

            if (sb2_selected_port_details[loop_index].port_type == 0) {
                break;
            }

            if(!SOC_WARM_BOOT(unit)) {
                if(port == 25) {
                    if(sb2_selected_port_details[loop_index+1].port_type != 0) {
                        /* There are 2 entries for eagle port. */
                        /* Refer : "Understanding eagle port details" above */
                        eagle_n4s_mode = 1;
                    } else if(sb2_selected_port_details[loop_index].port_incr == 2) {
                        /* Port incr = 2 and eagle has only one entry */
                        eagle_n2_mode = 1;
                    } else if ((sb2_selected_port_details[loop_index].port_incr == 1) &&
                        (sb2_selected_port_details[loop_index].port_type & GE_PORT)) {
                        /* Port incr = 1 and 4GE ports */
                        eagle_4ge_mode = 1;
                    }
                }
            }

            if ((port >=1) && (port <= 24)) {
                SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_mxq, port);
            } else if (port >=25) {
                SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xl, port);
            }

            if (sb2_selected_port_details[loop_index].port_type & GE_PORT) {
                SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xport_ge, port);
                switch(port_speed) {
                    case 1000:
                        if ((port >=1) && (port <= 24)) {
                            SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_mxq1g, port);
                        } else {
                            SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xl1g, port);
                        }
                        bcm56260_speed[unit][sb2_port_details_index][port-1] = 1000;
                        break;
                    case 2500:
                        if ((port >=1) && (port <= 24)) {
                            SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_mxq2p5g, port);
                        } else {
                            SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xl2p5g, port);
                        }
                        bcm56260_speed[unit][sb2_port_details_index][port-1] = 2500;
                        break;
                    default:
                        break;
                }
            }

            if ((sb2_selected_port_details[loop_index].port_type & XE_PORT) || 
                ((sb2_selected_port_details[loop_index].port_type & HG_PORT) &&
                 (SOC_PBMP_MEMBER(my_pbmp_xport_xe, port)))) {
                SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xport_xe, port);
                bcm56260_speed[unit][sb2_port_details_index][port-1] = 10000;
                if ((port >=1) && (port <= 24)) {
                     SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_mxq10g, port);
                } else {
                     if (SOC_PBMP_MEMBER(my_pbmp_xport_ge, port)) {
                         SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xl2p5g, port);
                         SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xport_ge, port);
                         SOC_PBMP_PORT_REMOVE(*sb2_pbmp.pbmp_xport_xe, port);
                     } else if (port_speed == 2500) {
                         SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xl2p5g, port);
                     } else if (port_speed == 1000) {
                         SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xl1g, port);
                     } else {
                         SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xl10g, port);
                     }
                }
            } 
            if ((sb2_selected_port_details[loop_index].port_type & HG_PORT) &&
                (SOC_PBMP_MEMBER(my_pbmp_xport_ge, port))){
                 SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xl2p5g, port);
                 SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xport_ge, port);
                 SOC_PBMP_PORT_REMOVE(*sb2_pbmp.pbmp_xport_xe, port);
            } else {
                if (port >= 25) {
                    SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_xl10g, port);
                    SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_gport_stack, port);
                }
            }
            sb2_port_used_flags[port-1]=1;

        } /* for port */
        

        loop_index++;
    } /* while sb2_selected_port_details */

    /* Eagle port support 2.5G speed when pbmp_xport_ge is set
     * Prior to 1.4 version speed sync as 1G
     * But from 1.4 version sync function is fixed
     * To retrive properly for older to 1.4 version by using sb2_2p5g_ports
     * we are fixing the speed to 2.5G instead of 1G
     */
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!(SOC_WARM_BOOT(unit))) {
        for(port = 25; port <= SB2_MAX_PHYSICAL_PORTS; ++port) {
            if (bcm56260_speed[unit][_sb2_config_id[unit]][port-1] >= 2500) {
                sb2_2p5g_ports[port-1] = 1;
            }
        }
    }
#endif

    for (port = 1; port < SB2_LPBK ; port++) {
        if (sb2_port_used_flags[port-1] == 0) {
            SOC_PBMP_PORT_REMOVE(*sb2_pbmp.pbmp_valid, port);
        }
    }   
    
#if 1
    for (port = 29; port <= 31; port++) {
        SOC_PBMP_PORT_REMOVE(*sb2_pbmp.pbmp_valid, port);
    }   
#endif

    for (port = 0; port < 96; port++) {
        SOC_PBMP_PORT_ADD(*sb2_pbmp.pbmp_pp, port);
    }
}

void
soc_saber2_subport_init(int unit)
{
    soc_port_t port, lp_port, prev_port, pp_port;
    soc_pbmp_t pbmp_subport, pbmp_linkphy;
    soc_pbmp_t pbmp_linkphy_one_stream_per_subport;
    soc_info_t *si=&SOC_INFO(unit);
    int num_subport = 0, available_subport = 0;
    int available_lp_subport = 0;
    int port_subport_index_start = 0;
    int s1_xlp0_index = 0;

    SOC_PBMP_CLEAR(pbmp_subport);
    SOC_PBMP_CLEAR(pbmp_linkphy);
    SOC_PBMP_CLEAR(pbmp_linkphy_one_stream_per_subport);
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
    SOC_PBMP_CLEAR(si->general_pp_port_pbm);
    SOC_PBMP_CLEAR(si->enabled_general_pp_port_pbm);
    si->subtag_enabled = 0;
    si->linkphy_enabled = 0;

    for (port = 1; port <= SB2_MAX_PHYSICAL_PORTS; port++) {
        si->port_num_subport[port] = 0;
        si->port_subport_base[port] = 0;
        si->port_linkphy_s1_base[port] = 0;
    }

    pbmp_subport = soc_property_get_pbmp(unit, spn_PBMP_SUBPORT, 0);
    pbmp_linkphy = soc_property_get_pbmp(unit, spn_PBMP_LINKPHY, 0);
    pbmp_linkphy_one_stream_per_subport =
    soc_property_get_pbmp(unit, spn_PBMP_LINKPHY_ONE_STREAM_PER_SUBPORT, 0);

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
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,  "\nCONFIG ERROR\n"
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
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,  "\nCONFIG ERROR\n"
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
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,  "\nCONFIG ERROR\n"
                "config variable pbmp_linkphy should be sub-set of "
                "config variable pbmp_subport\n\n")));
        }
    }

    available_subport = SOC_SB2_MAX_SUBPORTS;
    available_lp_subport = SOC_SB2_MAX_LINKPHY_SUBPORTS;
    prev_port = 0;
    si->port_num_subport[prev_port] = 0;
    si->port_subport_base[prev_port] = SOC_SB2_MIN_SUBPORT_INDEX;

    if (SOC_PBMP_NOT_NULL(si->linkphy_allowed_pbm)) {

        /* Iterate through LinkPHY members first and reserve subport indices */
        SOC_PBMP_ITER(si->linkphy_allowed_pbm, port) {
            if (port < SOC_MAX_NUM_PORTS) {
                num_subport =
                    soc_property_port_get(unit, port, spn_NUM_SUBPORTS, 0);

                if (num_subport > SOC_SB2_MAX_LINKPHY_SUBPORTS_PER_PORT) {
                    num_subport = SOC_SB2_MAX_LINKPHY_SUBPORTS_PER_PORT;
                }

                if (num_subport == 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,  "\nCONFIG ERROR\n"
                        "num_subports_%d should be non-zero for "
                        "LinkPHY port %d\n\n"), port, port));
                    SOC_PBMP_PORT_REMOVE(si->linkphy_allowed_pbm, port);
                    continue;
                }

                if ((num_subport > available_subport) ||
                    (num_subport > available_lp_subport)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,  "\nCONFIG ERROR\n"
                        "num_subports_%d=%d unavailable for LinkPHY port\n\n"),
                            port, num_subport));
                    SOC_PBMP_PORT_REMOVE(si->linkphy_allowed_pbm, port);
                    continue;
                }

                port_subport_index_start =
                    si->port_subport_base[prev_port] +
                    si->port_num_subport[prev_port];

                si->port_subport_base[port] = port_subport_index_start;
                si->port_num_subport[port] = num_subport;
                si->port_linkphy_s1_base[port] = s1_xlp0_index;
                s1_xlp0_index += num_subport *
                (SOC_PBMP_MEMBER(pbmp_linkphy_one_stream_per_subport, port) ? 1 : 2);

                available_subport -= num_subport;
                available_lp_subport -= num_subport;

                prev_port = port;
            }
        } /* end SOC_PBMP_ITER(si->linkphy_allowed_pbm, port) */
    }

    prev_port = 0;
    if (SOC_PBMP_NOT_NULL(si->subtag_allowed_pbm)) {
        /* Iterate through SubTag ports and reserve subport indices */
        SOC_PBMP_ITER(si->subtag_allowed_pbm, port) {
            num_subport =
                soc_property_port_get(unit, port, spn_NUM_SUBPORTS, 0);

            if (num_subport == 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,  "\nCONFIG ERROR\n"
                    "num_subports_%d should be non-zero for SubTag port        %d\n\n"),
                    port, port));
                SOC_PBMP_PORT_REMOVE(si->subtag_allowed_pbm, port);
                continue;
            }

            if (num_subport > available_subport) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,  "\nCONFIG ERROR\n"
                    "num_subports_%d=%d unavailable for SubTag port\n\n"),
                    port, num_subport));
                SOC_PBMP_PORT_REMOVE(si->subtag_allowed_pbm, port);
                continue;
            }

            port_subport_index_start = si->port_subport_base[prev_port] +
                                       si->port_num_subport[prev_port];

            /* Iterate the LinkPHY ports.
             * Skip the LinkPHY ports for which subports are allocated
             * until we get start index of required free subports
             * or reach end of LinkPHY ports. */

            SOC_PBMP_ITER(si->linkphy_allowed_pbm, lp_port) {
                if (lp_port < SOC_MAX_NUM_PORTS) {
                    if (si->port_num_subport[lp_port]) {
                        if (si->port_subport_base[lp_port] ==
                                                    port_subport_index_start) {
                            /* skip the allocated LinkPHY ports */
                            port_subport_index_start =
                                si->port_subport_base[lp_port] +
                                si->port_num_subport[lp_port];
                            continue;
                        } else if (si->port_subport_base[lp_port] >
                                                    port_subport_index_start) {
                            /* check if there is a big enough subport index
                             * chunk unused, between 2 consecutive LinkPHY
                             * enabled ports, to accomodate required           subports */
                            if ((si->port_subport_base[lp_port] -
                                 port_subport_index_start) < num_subport) {
                                /* skip the allocated LinkPHY ports */
                                port_subport_index_start =
                                    si->port_subport_base[lp_port]
                                    + si->port_num_subport[lp_port];
                                continue;
                            }
                        }
                    }
                }
            }

            if (port < SOC_MAX_NUM_PORTS) {
                si->port_subport_base[port] = port_subport_index_start;
                si->port_num_subport[port] = num_subport;

                available_subport -= num_subport;
                prev_port = port;
            }
        } /* end SOC_PBMP_ITER(si->subtag_allowed_pbm, port) */
    }

    /* populate LinkPHY pp_port bitmap */
    SOC_PBMP_ITER(si->linkphy_allowed_pbm, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            for (pp_port = 0; pp_port < si->port_num_subport[port]; pp_port++) {
                SOC_PBMP_PORT_ADD(si->linkphy_pp_port_pbm,
                        pp_port + si->port_subport_base[port]);
                /* Set this port in the enabled bmap by default at init */
                SOC_PBMP_PORT_ADD(si->enabled_linkphy_pp_port_pbm,
                        pp_port + si->port_subport_base[port]);
            }
        }
    }

    /* populate SubTag pp_port bitmap */
    SOC_PBMP_ITER(si->subtag_allowed_pbm, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            for (pp_port = 0; pp_port < si->port_num_subport[port]; pp_port++) {
                SOC_PBMP_PORT_ADD(si->subtag_pp_port_pbm,
                        pp_port + si->port_subport_base[port]);
                /* Set this port in the enabled bmap by default at init */
                SOC_PBMP_PORT_ADD(si->enabled_subtag_pp_port_pbm,
                        pp_port + si->port_subport_base[port]);
            }
        }
    }
    if (SOC_PBMP_NOT_NULL (si->subtag_pp_port_pbm)) {
        si->subtag_enabled = 1;
    }
    if (SOC_PBMP_NOT_NULL (si->linkphy_pp_port_pbm)) {
        si->linkphy_enabled = 1;
    }

    /* Update General Purpose pp port bitmap */
    if (soc_feature(unit, soc_feature_general_cascade)) {
        for (pp_port = 0; pp_port < SOC_SB2_MAX_SUBPORTS; pp_port++) {
            if (SOC_PBMP_MEMBER(si->subtag_pp_port_pbm,
                     (pp_port + SOC_SB2_MIN_SUBPORT_INDEX))) {
                continue;
            }
            if (SOC_PBMP_MEMBER(si->linkphy_pp_port_pbm,
                     (pp_port + SOC_SB2_MIN_SUBPORT_INDEX))) {
                continue;
            }
            SOC_PBMP_PORT_ADD(si->general_pp_port_pbm,
                 (pp_port + SOC_SB2_MIN_SUBPORT_INDEX));
        }
    }

    /* Initialize the number of modules */
    si->num_coe_modules = soc_property_get(unit, spn_MODULE_NUM_MODIDS, 1);

    /* Check if the num_mods exceeds max allowed, if so flag an error,
       note that the default of '1' is reserved for front-panel physical
       ports */
    if(si->num_coe_modules > (SOC_SB2_MAX_COE_MODULES + 1)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "\nCONFIG ERROR\n"
                              "Number of modules %d exceeds maximum: %d,"
                              "setting to max \n\n"),
                   si->num_coe_modules, SOC_SB2_MAX_COE_MODULES));
        si->num_coe_modules = SOC_SB2_MAX_COE_MODULES;
    }

    /* If it is a non default value, note that we are in a
       'coe-stacking-mode', wherein we will be supporing stacking for CoE */
    if(si->num_coe_modules != 1) {
        si->coe_stacking_mode = 1;
        /* Since the first module is reserved for physical-ports, reduce the
           num_coe_modules by 1 */
        si->num_coe_modules--;
    }

}

int 
soc_sb2_cosq_min_clear (int unit, int port, 
                        int index, uint32 level)
{
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l2_shaper_config_lower_entry_t l2_entry;
    int idx;

    static const soc_field_t rate_exp_fields[] = {
        C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
        C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
    };
    static const soc_field_t rate_mant_fields[] = {
        C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
        C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };
    int l2_queues = -1;
    int l2_shaper_bucket = -1;
    
    l2_queues     = soc_mem_index_count(unit,LLS_L2_PARENTm);
    l2_shaper_bucket = l2_queues /
            soc_mem_index_count(unit,
                    LLS_L2_SHAPER_CONFIG_Cm);

    switch(level){
        case _SOC_KT2_COSQ_NODE_LVL_L0:
            sal_memset(&l0_entry, 0,
                    sizeof(lls_l0_shaper_config_c_entry_t));
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L0_MIN_CONFIG_Cm, MEM_BLOCK_ALL,
                              index, &l0_entry));
            soc_mem_field32_set(unit, LLS_L0_MIN_CONFIG_Cm , &l0_entry,
                    C_MIN_REF_RATE_EXPf, 0);  
            soc_mem_field32_set(unit, LLS_L0_MIN_CONFIG_Cm, &l0_entry,
                    C_MIN_REF_RATE_MANTf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L0_MIN_CONFIG_Cm,
                               MEM_BLOCK_ALL, index, &l0_entry));
            break;
        case _SOC_KT2_COSQ_NODE_LVL_L1:
            sal_memset(&l1_entry, 0,
                    sizeof(lls_l1_shaper_config_c_entry_t));
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L1_MIN_CONFIG_Cm, MEM_BLOCK_ALL,
                              index, &l1_entry));
            soc_mem_field32_set(unit, LLS_L1_MIN_CONFIG_Cm, &l1_entry,
                    C_MIN_REF_RATE_EXPf, 0);
            soc_mem_field32_set(unit, LLS_L1_MIN_CONFIG_Cm, &l1_entry,
                    C_MIN_REF_RATE_MANTf,0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L1_MIN_CONFIG_Cm,
                               MEM_BLOCK_ALL, index, &l1_entry));
            break;
        case _SOC_KT2_COSQ_NODE_LVL_L2:
            idx = (index % 4);
            sal_memset(&l2_entry, 0,
                    sizeof(lls_l2_shaper_config_lower_entry_t));
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L2_MIN_CONFIG_Cm, MEM_BLOCK_ALL,
                              index/l2_shaper_bucket, &l2_entry));
            soc_mem_field32_set(unit, LLS_L2_MIN_CONFIG_Cm, &l2_entry,
                    rate_exp_fields[idx], 0);
            soc_mem_field32_set(unit, LLS_L2_MIN_CONFIG_Cm, &l2_entry,
                    rate_mant_fields[idx], 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L2_MIN_CONFIG_Cm,
                               MEM_BLOCK_ALL, index/l2_shaper_bucket, &l2_entry));
            break;

        default:
            break;
    }
    return SOC_E_NONE;
}

int
soc_sb2_cosq_min_bucket_get(int unit, int port, int index, int level,
                        uint32 *min_quantum, uint32 *mantissa,
                        uint32 *exp, uint32 *cycle)
{
    int rv;
    uint32 idx = 0;
    uint32 i = 0;
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l2_shaper_config_lower_entry_t l2_entry;
    soc_mem_t config_mem[2];
    uint32 rate_exp[2], rate_mantissa[2];
    uint32 cycle_sel[2];

    soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
       C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
       C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
    };
    soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
       C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
       C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };
    soc_field_t cycle_sel_fields[] = {
        C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
        C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f,
        C_MIN_CYCLE_SEL_0f, C_MIN_CYCLE_SEL_1f,
        C_MIN_CYCLE_SEL_2f, C_MIN_CYCLE_SEL_3f
    };

    soc_field_t rate_exp_f[] = {
        C_MAX_REF_RATE_EXPf, C_MIN_REF_RATE_EXPf
    };
    soc_field_t rate_mant_f[] = {
        C_MAX_REF_RATE_MANTf, C_MIN_REF_RATE_MANTf
    };
    soc_field_t cycle_sel_f[] = {
        C_MAX_CYCLE_SELf, C_MIN_CYCLE_SELf
    };
    int shaper_bucket_index = 0 ;
    int max_l1_shaper_bucket = 0;
    int max_l2_shaper_bucket = 0 ;

    switch (level) {

        case _SOC_KT2_COSQ_NODE_LVL_L0:
            config_mem[0] = LLS_L0_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L0_MIN_CONFIG_Cm;

            for (i = 1; i < 2; i++) {
                SOC_IF_ERROR_RETURN
                       (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                 index, &l0_entry));
                rate_exp[i] = soc_mem_field32_get(unit, config_mem[i],
                                                  &l0_entry,
                                                  rate_exp_f[i]);
                rate_mantissa[i] = soc_mem_field32_get(unit, config_mem[i],
                                                       &l0_entry,
                                                       rate_mant_f[i]);
                cycle_sel[i] = soc_mem_field32_get(unit, config_mem[i], &l0_entry,
                                cycle_sel_f[i]);
            }
            break;

        case _SOC_KT2_COSQ_NODE_LVL_L1:
            config_mem[0] = LLS_L1_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L1_MIN_CONFIG_Cm;
            max_l1_shaper_bucket = soc_mem_index_count(unit,LLS_L1_PARENTm) /
                                               soc_mem_index_count(unit,
                                                LLS_L1_SHAPER_CONFIG_Cm);

                shaper_bucket_index = index % max_l1_shaper_bucket;
            for (i = 1; i < 2; i++) {
                 idx = (i * 4) + shaper_bucket_index;
                SOC_IF_ERROR_RETURN
                       (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                     index / max_l1_shaper_bucket, &l1_entry));
                rate_exp[i] = soc_mem_field32_get(unit, config_mem[i],
                                                  &l1_entry,
                                                  rate_exp_fields[idx]);
                rate_mantissa[i] = soc_mem_field32_get(unit, config_mem[i],
                                                       &l1_entry,
                                                       rate_mant_fields[idx]);
                cycle_sel[i] = soc_mem_field32_get(unit, config_mem[i], &l1_entry,
                                cycle_sel_f[i]);
             }
            break;

        case _SOC_KT2_COSQ_NODE_LVL_L2:

            config_mem[0] = LLS_L2_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L2_MIN_CONFIG_Cm;
             max_l2_shaper_bucket = soc_mem_index_count(unit,LLS_L2_PARENTm)/
              soc_mem_index_count(unit, LLS_L2_SHAPER_CONFIG_Cm);
            for (i = 1; i < 2; i++) {
                idx = (i * 4) + (index % 4);
                SOC_IF_ERROR_RETURN
                       (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                 index/max_l2_shaper_bucket, &l2_entry));
                rate_exp[i] = soc_mem_field32_get(unit, config_mem[i],
                                                  &l2_entry,
                                                  rate_exp_fields[idx]);
                rate_mantissa[i] = soc_mem_field32_get(unit, config_mem[i],
                                                       &l2_entry,
                                                       rate_mant_fields[idx]);
                cycle_sel[i] = soc_mem_field32_get(unit, config_mem[i], &l2_entry,
                                cycle_sel_fields[idx]);
            }
            break;

        case _SOC_KT2_COSQ_NODE_LVL_S0:
        case _SOC_KT2_COSQ_NODE_LVL_S1:
        case _SOC_KT2_COSQ_NODE_LVL_ROOT:
             return SOC_E_INTERNAL;
        default:
            return SOC_E_INTERNAL;
    }


   rv =  soc_katana_compute_shaper_rate(unit, rate_mantissa[1], rate_exp[1],
                                        min_quantum);
   if(!rv)
   {
      return rv;
   }
   *mantissa = rate_mantissa[1];
   *exp = rate_exp[1];
   *cycle = cycle_sel[1];

    return SOC_E_NONE;

}


int
soc_sb2_cosq_max_bucket_set(int unit, int port,
                        int index, uint32 level)
{
    uint32 idx = 0;
    uint32 i = 0;
    lls_port_shaper_config_c_entry_t port_entry;
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l2_shaper_config_lower_entry_t l2_entry;
    soc_mem_t config_mem[2];
    lls_s0_shaper_config_c_entry_t s0_entry;
    soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
       C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
       C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
    };
    soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
       C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
       C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };
    soc_field_t burst_exp_fields[] = {
       C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
       C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
       C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
       C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f
    };
    soc_field_t burst_mant_fields[] = {
       C_MAX_THLD_MANT_0f, C_MAX_THLD_MANT_1f,
       C_MAX_THLD_MANT_2f, C_MAX_THLD_MANT_3f,
       C_MIN_THLD_MANT_0f, C_MIN_THLD_MANT_1f,
       C_MIN_THLD_MANT_2f, C_MIN_THLD_MANT_3f
    };
    soc_field_t cycle_sel_fields[] = {
        C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
        C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f,    
        C_MIN_CYCLE_SEL_0f, C_MIN_CYCLE_SEL_1f,
        C_MIN_CYCLE_SEL_2f, C_MIN_CYCLE_SEL_3f
    };

    soc_field_t rate_exp_f[] = {
        C_MAX_REF_RATE_EXPf, C_MIN_REF_RATE_EXPf
    };
    soc_field_t rate_mant_f[] = {
        C_MAX_REF_RATE_MANTf, C_MIN_REF_RATE_MANTf
    };
    soc_field_t burst_exp_f[] = {
        C_MAX_THLD_EXPf, C_MIN_THLD_EXPf
    };
    soc_field_t burst_mant_f[] = {
        C_MAX_THLD_MANTf, C_MIN_THLD_MANTf
    };
    soc_field_t cycle_sel_f[] = {
        C_MAX_CYCLE_SELf, C_MIN_CYCLE_SELf
    };
    int l2_queues = -1;
    int l2_shaper_bucket = -1;

    l2_queues     = soc_mem_index_count(unit,LLS_L2_PARENTm);
    l2_shaper_bucket = l2_queues /
            soc_mem_index_count(unit,
                    LLS_L2_SHAPER_CONFIG_Cm);

    /* set Min/Max threshold shapers to 33Gbps */
    switch (level)  {
        case _SOC_KT2_COSQ_NODE_LVL_ROOT:
            config_mem[0] = LLS_PORT_SHAPER_CONFIG_Cm;

            sal_memset(&port_entry, 0,
                       sizeof(lls_port_shaper_config_c_entry_t));
            SOC_IF_ERROR_RETURN
               (soc_mem_read(unit, config_mem[0], MEM_BLOCK_ALL,
                             index, &port_entry));
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                rate_exp_f[0],
                                SB2_QUEUE_FLUSH_RATE_EXP);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                rate_mant_f[0],
                                SB2_QUEUE_FLUSH_RATE_MANTISSA);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                burst_exp_f[0],
                                SB2_QUEUE_FLUSH_BURST_EXP);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                burst_mant_f[0],
                                SB2_QUEUE_FLUSH_BURST_MANTISSA);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                cycle_sel_f[0],
                                SB2_QUEUE_FLUSH_CYCLE_SEL);
            SOC_IF_ERROR_RETURN
                   (soc_mem_write(unit, config_mem[0],
                                       MEM_BLOCK_ALL, index, &port_entry));
           break;
        case _SOC_KT2_COSQ_NODE_LVL_S0:
        case _SOC_KT2_COSQ_NODE_LVL_S1:
            config_mem[0] = (level ==_SOC_KT2_COSQ_NODE_LVL_S0) ?
                            LLS_S0_SHAPER_CONFIG_Cm :
                            LLS_S1_SHAPER_CONFIG_Cm;
            SOC_IF_ERROR_RETURN
               (soc_mem_read(unit, config_mem[0], MEM_BLOCK_ALL,
                             index, &s0_entry));
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                rate_exp_f[0], SB2_QUEUE_FLUSH_RATE_EXP);
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                rate_mant_f[0], SB2_QUEUE_FLUSH_RATE_MANTISSA);
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                burst_exp_f[0], SB2_QUEUE_FLUSH_BURST_EXP);
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                burst_mant_f[0], SB2_QUEUE_FLUSH_BURST_MANTISSA);
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                cycle_sel_f[0], SB2_QUEUE_FLUSH_CYCLE_SEL);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, config_mem[0],
                               MEM_BLOCK_ALL, index, &s0_entry));
            break;
        case _SOC_KT2_COSQ_NODE_LVL_L0:
            config_mem[0] = LLS_L0_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L0_MIN_CONFIG_Cm;

            for (i = 0; i < 2; i++) {
                sal_memset(&l0_entry, 0,
                             sizeof(lls_l0_shaper_config_c_entry_t));
                SOC_IF_ERROR_RETURN
                       (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                     index, &l0_entry));
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    rate_exp_f[i],
                                    SB2_QUEUE_FLUSH_RATE_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    rate_mant_f[i],
                                    SB2_QUEUE_FLUSH_RATE_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    burst_exp_f[i],
                                    SB2_QUEUE_FLUSH_BURST_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    burst_mant_f[i],
                                    SB2_QUEUE_FLUSH_BURST_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    cycle_sel_f[i],
                                    SB2_QUEUE_FLUSH_CYCLE_SEL);
                SOC_IF_ERROR_RETURN
                         (soc_mem_write(unit, config_mem[i],
                                    MEM_BLOCK_ALL, index, &l0_entry));
            }
            break;
        case _SOC_KT2_COSQ_NODE_LVL_L1:
            config_mem[0] = LLS_L1_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L1_MIN_CONFIG_Cm;

            for (i = 0; i < 2; i++) {
                sal_memset(&l1_entry, 0,
                            sizeof(lls_l1_shaper_config_c_entry_t));
                SOC_IF_ERROR_RETURN
                       (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                   index, &l1_entry));
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    rate_exp_f[i],
                                    SB2_QUEUE_FLUSH_RATE_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    rate_mant_f[i],
                                    SB2_QUEUE_FLUSH_RATE_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    burst_exp_f[i],
                                    SB2_QUEUE_FLUSH_BURST_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    burst_mant_f[i],
                                    SB2_QUEUE_FLUSH_BURST_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    cycle_sel_f[i],
                                    SB2_QUEUE_FLUSH_CYCLE_SEL);
                SOC_IF_ERROR_RETURN
                     (soc_mem_write(unit, config_mem[i],
                                    MEM_BLOCK_ALL, 
                                    index, &l1_entry));
            }
            break;
        case _SOC_KT2_COSQ_NODE_LVL_L2:
            config_mem[0] = LLS_L2_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L2_MIN_CONFIG_Cm;

            for ( i = 0; i < 2; i++) {
                idx = (i * 4) + (index % 4);

                sal_memset(&l2_entry, 0,
                           sizeof(lls_l2_shaper_config_lower_entry_t));
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                  index/l2_shaper_bucket, &l2_entry));
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    rate_exp_fields[idx],
                                    SB2_QUEUE_FLUSH_RATE_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    rate_mant_fields[idx],
                                    SB2_QUEUE_FLUSH_RATE_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    burst_exp_fields[idx],
                                    SB2_QUEUE_FLUSH_BURST_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    burst_mant_fields[idx],
                                    SB2_QUEUE_FLUSH_BURST_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    cycle_sel_fields[idx],
                                    SB2_QUEUE_FLUSH_CYCLE_SEL); 
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, config_mem[i],
                                   MEM_BLOCK_ALL, 
                                   index/l2_shaper_bucket, &l2_entry));
            }
            break;
        default:
            return SOC_E_NONE;
    }

    return SOC_E_NONE;

}

STATIC int
_soc_sb2_mmu_config_shared_update_check(int unit, int val1, int val2, int flags)
{
    int rv = 0;
    if (flags == 1) {
        if (val1 > val2) {
            rv = 1;
        }
    } else {
        if (val1 < val2) {
            rv = 1;
        }
    }
    return rv;
}

int
soc_sb2_mmu_config_shared_buf_recalc(int unit, int shared_size, 
                                     bcm_kt2_cosq_recalc_type_t recalc_type)
{
    soc_info_t    *si= &SOC_INFO(unit); 
    soc_pbmp_t    valid_port_cpu_pbmp;
    soc_port_t    port=0;
    uint32        temp_val;
    uint32        rval=0;
    uint32        op_node=0;
    uint32        op_node_offset=0;
    uint32        cos;
    int           cur_limit, resume_limit, flags;
    uint32        ext_mem_type;

    mmu_thdo_opnconfig_cell_entry_t     mmu_thdo_opnconfig_cell_entry={{0}};

    ext_mem_type = ((recalc_type == BCM_KT2_RECALC_SHARED_EXT_DECREMENT) ||
                    (recalc_type == BCM_KT2_RECALC_SHARED_EXT_INCREMENT)) ? 1 : 0;

    flags = ((recalc_type == BCM_KT2_RECALC_SHARED_INT_DECREMENT) ||
             (recalc_type == BCM_KT2_RECALC_SHARED_EXT_DECREMENT)) ? 1 : 0;

    if ((recalc_type == BCM_KT2_RECALC_SHARED_EXT_DECREMENT) || 
        (recalc_type == BCM_KT2_RECALC_SHARED_EXT_INCREMENT)) {

        SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r (
                            unit, &rval));
        cur_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r, rval,
                                         OP_BUFFER_SHARED_LIMIT_CELLEf);
        if (_soc_sb2_mmu_config_shared_update_check(unit, cur_limit, shared_size, flags)) {
            if (recalc_type == BCM_KT2_RECALC_SHARED_EXT_DECREMENT) {
                temp_val = cur_limit - shared_size;
            } else {
                temp_val = shared_size - cur_limit;
            }
            soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r, &rval,
                            OP_BUFFER_SHARED_LIMIT_CELLEf, shared_size);
            SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r (
                                unit, rval));

            SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r(
                                unit, &rval));
            cur_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r, 
                            rval,OP_BUFFER_SHARED_LIMIT_RESUME_CELLEf);
            if (recalc_type == BCM_KT2_RECALC_SHARED_EXT_DECREMENT) {
                resume_limit = cur_limit - temp_val;
                if (resume_limit < 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\nGlobal external"
                                                             " shared buffer exhausted")));
                    return SOC_E_RESOURCE;
                }
            } else {
                resume_limit = cur_limit + temp_val;
            }

            soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r, 
                            &rval,OP_BUFFER_SHARED_LIMIT_RESUME_CELLEf, resume_limit);
            SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r(
                                unit, rval));
        }
    } else {
        SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_CELLIr (unit, &rval));
        cur_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMIT_CELLIr, rval,
                                         OP_BUFFER_SHARED_LIMIT_CELLIf);
        if (_soc_sb2_mmu_config_shared_update_check(unit, cur_limit, shared_size, flags)) {
            if (recalc_type == BCM_KT2_RECALC_SHARED_INT_DECREMENT) {
                temp_val = cur_limit - shared_size;
            } else {
                temp_val = shared_size - cur_limit;
            }
            soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLIr, &rval,
                    OP_BUFFER_SHARED_LIMIT_CELLIf, shared_size);
            SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLIr (unit, rval));

            SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr(unit, &rval));
            cur_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr, rval,
                                        OP_BUFFER_SHARED_LIMIT_RESUME_CELLIf);
            if (recalc_type == BCM_KT2_RECALC_SHARED_INT_DECREMENT) {
                resume_limit = cur_limit - temp_val;
                if (resume_limit < 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\nGlobal internal"
                                                             " shared buffer exhausted")));
                    return SOC_E_RESOURCE;
                }
            } else {
                resume_limit = cur_limit + temp_val;
            }

            soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr, &rval,
                            OP_BUFFER_SHARED_LIMIT_RESUME_CELLIf, resume_limit);
            SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr(unit, rval));
        }
    }

    SOC_PBMP_CLEAR (valid_port_cpu_pbmp);
    SOC_PBMP_ASSIGN(valid_port_cpu_pbmp, PBMP_ALL(unit));
    SOC_PBMP_ITER (valid_port_cpu_pbmp, port) {
        /* True only when recalc type (int or ext) is same as port type */
        if (((!ext_mem_type && !SOC_PBMP_MEMBER(PBMP_EXT_MEM (unit), port)) ||
            (ext_mem_type && SOC_PBMP_MEMBER(PBMP_EXT_MEM (unit), port))) &&
            (port >= si->cmic_port) && (port <= si->lb_port)) {

            op_node = ( si->port_cosq_base[port] / 8);
            for (op_node_offset = 0; 
                    op_node_offset < sal_ceil_func(si->port_num_uc_cosq[port],8); 
                    op_node_offset++) {

                SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY,op_node + op_node_offset,
                            &mmu_thdo_opnconfig_cell_entry));
                cur_limit = soc_mem_field32_get(unit, MMU_THDO_OPNCONFIG_CELLm, 
                                    &mmu_thdo_opnconfig_cell_entry, 
                                    OPN_SHARED_LIMIT_CELLf);
                if (_soc_sb2_mmu_config_shared_update_check(
                        unit, cur_limit, shared_size, flags)) {
                    if ((recalc_type == BCM_KT2_RECALC_SHARED_INT_DECREMENT) ||
                        (recalc_type == BCM_KT2_RECALC_SHARED_EXT_DECREMENT)) {
                        temp_val = cur_limit - shared_size;
                    } else {
                        temp_val = shared_size - cur_limit;
                    }
                    soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, 
                            &mmu_thdo_opnconfig_cell_entry, 
                            OPN_SHARED_LIMIT_CELLf, shared_size);

                    cur_limit = soc_mem_field32_get(unit, MMU_THDO_OPNCONFIG_CELLm, 
                                        &mmu_thdo_opnconfig_cell_entry, 
                                        OPN_SHARED_RESET_VALUE_CELLf);
                    if ((recalc_type == BCM_KT2_RECALC_SHARED_INT_DECREMENT) ||
                        (recalc_type == BCM_KT2_RECALC_SHARED_EXT_DECREMENT)) {
                        resume_limit = cur_limit - temp_val;
                    } else {
                        resume_limit = cur_limit + temp_val;
                    }

                    soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, 
                            &mmu_thdo_opnconfig_cell_entry, 
                            OPN_SHARED_RESET_VALUE_CELLf, resume_limit);

                    SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_CELLm(unit,
                                MEM_BLOCK_ANY,op_node + op_node_offset,
                                &mmu_thdo_opnconfig_cell_entry));
                }
            }
        }
    }
    if (ext_mem_type) {
        for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORDEEr); cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORDEEr(
                        unit, cos, &rval));

            temp_val = soc_reg_field_get(unit, OP_QUEUE_CONFIG_THDORDEEr, rval,
                                Q_LIMIT_DYNAMICf);
            if (!temp_val) {
                cur_limit = soc_reg_field_get(unit, OP_QUEUE_CONFIG_THDORDEEr, rval,
                                                Q_SHARED_LIMITf);
                if (_soc_sb2_mmu_config_shared_update_check(
                        unit, cur_limit, shared_size, flags)) {
                    soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEEr, &rval,
                            Q_SHARED_LIMITf, shared_size);
                    SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORDEEr(
                                unit, cos, rval));
                }
            }
        }
    } else {
        for(cos=0; cos<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORDEIr); cos++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORDEIr(
                        unit, cos, &rval));

            temp_val = soc_reg_field_get(unit, OP_QUEUE_CONFIG_THDORDEIr, rval,
                                Q_LIMIT_DYNAMICf);
            if (!temp_val) {
                cur_limit = soc_reg_field_get(unit, OP_QUEUE_CONFIG_THDORDEIr, rval,
                                                Q_SHARED_LIMITf);
                if (_soc_sb2_mmu_config_shared_update_check(
                        unit, cur_limit, shared_size, flags)) {
                    soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORDEIr, &rval,
                            Q_SHARED_LIMITf, shared_size);
                    SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORDEIr(
                                unit, cos, rval));
                }
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_sb2_xport_type_update
 * Purpose:
 *      Rewrite the SOC control port structures to reflect the current port
 *      mode, and reinitialize the port
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (IEEE/HG/HG2/HG2-LITE)
 * Returns:
 *      None.
 * Notes:
 *      Must pause linkscan and take COUNTER_LOCK before calling this.
 */
void
soc_sb2_xport_type_update(int unit, soc_port_t port, int mode)
{
    soc_info_t          *si;
    soc_port_t          it_port;

    si = &SOC_INFO(unit);

    /* We need to lock the SOC structures until we finish the update */
    SOC_CONTROL_LOCK(unit);

    if (mode != SOC_ENCAP_IEEE) {
        SOC_PBMP_PORT_ADD(si->st.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
        /* special process for HG-Lite support to avoid wrong PBMP assignment.
         *
         * Note :
         *  the 'to_hg_port' for this special design must carry proper encap mode
         *  instead of TRUE/FALSE only.
         */
        if (mode == SOC_ENCAP_HIGIG2_LITE) {
            SOC_PBMP_PORT_ADD(si->hl.bitmap, port);
        } else {
            /* for HG/HG2 case (GE port expected not allowed here) */
            SOC_PBMP_PORT_ADD(si->hg.bitmap, port);
            SOC_PBMP_PORT_REMOVE(si->hl.bitmap, port);
        }
    } else {
        /* To IEEE mode */
        SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
        SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->hl.bitmap, port);
    }
#define RECONFIGURE_PORT_TYPE_INFO(ptype) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, it_port) { \
        si->ptype.port[si->ptype.num++] = it_port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = it_port; \
        } \
        if (it_port > si->ptype.max) { \
            si->ptype.max = it_port; \
        } \
    }

    /* Recalculate port type data */
    RECONFIGURE_PORT_TYPE_INFO(ether);
    RECONFIGURE_PORT_TYPE_INFO(st);
    RECONFIGURE_PORT_TYPE_INFO(hg);
    RECONFIGURE_PORT_TYPE_INFO(xe);
#undef  RECONFIGURE_PORT_TYPE_INFO

    soc_dport_map_update(unit);

    /* Release SOC structures lock */
    SOC_CONTROL_UNLOCK(unit);
}

/*
 * soc_sb2_5626x_devtype
 *
 * Verify saber2 device is 5626x
 */
int
soc_sb2_5626x_devtype (int unit)
{
    uint16      dev_id;
    uint8       rev_id;
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((dev_id == BCM56260_DEVICE_ID)
          || (dev_id == BCM56261_DEVICE_ID)
          || (dev_id == BCM56262_DEVICE_ID)
          || (dev_id == BCM56263_DEVICE_ID)
          || (dev_id == BCM56265_DEVICE_ID)
          || (dev_id == BCM56266_DEVICE_ID)
          || (dev_id == BCM56267_DEVICE_ID)
          || (dev_id == BCM56268_DEVICE_ID)
          || (dev_id == BCM56233_DEVICE_ID)) {
        return TRUE;
    } else {
        return FALSE;
    }
}
int
soc_sb2_get_max_buffer_size(int unit, int external, int value)
{
    int cval;
    cval= (external ?
           _soc_sb2_mmu_intermediate_results.general_info.max_ext_cell_buff_size:
           _soc_sb2_mmu_intermediate_results.general_info.max_int_cell_buff_size);
    if ((cval <= 0) || (cval >= value)) {
        return value;
    } else {
        return cval;
    }
}
void
soc_sb2_get_queue_min_size(int unit, int *internal_size, int *external_size)
{
   _sb2_output_port_threshold_t *output_port_threshold =
        &_soc_sb2_mmu_intermediate_results.output_port_threshold;
   if (internal_size) {
       *internal_size = output_port_threshold->min_grntd_res_queue_cells_int_buff;
   }
   if (external_size) {
       *external_size = output_port_threshold->min_grntd_res_queue_cells_ext_buff;
   }
   return;
}
#endif /* BCM_SABER_SUPPORT */

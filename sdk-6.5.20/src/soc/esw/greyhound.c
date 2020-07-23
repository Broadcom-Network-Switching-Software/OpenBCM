/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        greyhound.c
 * Purpose:
 * Requires:    
 */


#include <sal/core/boot.h>

#include <soc/firebolt.h>
#include <soc/bradley.h>
#include <soc/greyhound.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/lpm.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/er_tcam.h>
#include <soc/memtune.h>
#include <soc/devids.h>
#include <soc/defs.h>
#include <soc/mspi.h>
#include <soc/iproc.h>
#include <soc/esw/portctrl.h>

#include <shared/util.h>
#include <shared/l3.h>
#include <shared/bsl.h>
#include <soc/soc_ser_log.h>

#ifdef BCM_GREYHOUND_SUPPORT
#if defined(INCLUDE_L3)
#include <soc/greyhound.h>
#endif

#define SOC_MAX_PHY_PORTS            38

/* Port config related : p2l, max_speed and TDM */
/* Embedded Mode */   
/* 16x10G : 10G Embedded*/
static const int p2l_mapping_embedded_16x10g[] = {
     0, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,  2,  3,
     4,  5,  6,  7,  8,  9, 10, 11, 
    12, 13, 14, 15, 16, 17
};
static const int port_speed_max_embedded_16x10g[] = {
    -1,   -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,   -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,   -1,  -1,  -1,  -1,  -1, 110, 110, 
    110, 110, 110, 110, 110, 110, 110, 110, 
    110, 110, 110, 110, 110, 110
};

/* 8x10G: Low Port Count 10G Embedded */
static const int p2l_mapping_embedded_8x10g[] = {
     0, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,  2,  3,
     4,  5,  6,  7,  8,  9, -1, -1,
    -1, -1, -1, -1, -1, -1
};
static const int port_speed_max_embedded_8x10g[] = {
    -1,   -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,   -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,   -1,  -1,  -1,  -1,  -1, 110, 110, 
    110, 110, 110, 110, 110, 110,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1
};

/* Mixed Embeded Mode */    
/* 12x10G + 8x2.5G + 4x5G */   
/* 24x2.5G */
static const int p2l_mapping_mixed_embeded[] = {
     0, -1,  2,  3,  4,  5, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  6,  7,  8,  9, 10, 11, 
    12, 13, 14, 15, 16, 17, 18, 19, 
    20, 21, 22, 23, 24, 25
};

/* 24x2.5G */   
static const int port_speed_max_mixed_embeded_24x2p5[] = {
    -1,  -1,  25,  25,  25,  25,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
    -1,  -1,  25,  25,  25,  25,  25,  25,  
    25,  25,  25,  25,  25,  25,  25,  25,
    25,  25,  25,  25,  25,  25        
};
/* 12x10G + 8x2.5G + 4x5G */   
static const int port_speed_max_mixed_embeded[] = {
    -1,  -1,  50,  50,  50,  50,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
    -1,  -1,  25,  25,  25,  25,  25,  25,
    25,  25, 110, 110, 110, 110, 110, 110,
   110, 110, 110, 110, 110, 110
};
/* 16x10G */    
static const int p2l_mapping_mixed_embeded_16x10g[] = {
     0, -1,  2, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  3, -1, -1, -1,  4,  5,  
     6,  7,  8,  9, 10, 11, 12, 13, 
    14, 15, 16, 17, -1, -1
};
static const int port_speed_max_mixed_embeded_16x10g[] = {
    -1,  -1, 100,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1, 100,  -1,  -1,  -1, 110, 110,       
   110, 110, 110, 110, 110, 110, 110, 110,
   110, 110, 110, 110,  -1,  -1        
};        
/* Cascade Mode, Non-Cascade */
/* 24x1GE + 2x10G + 2x13G*/
static const int p2l_mapping_cascade[] = {
       0, -1,  2,  3,  4,  5,  6,  7, 
       8,  9, 10, 11, 12, 13, 14, 15, 
      16, 17, 18, 19, 20, 21, 22, 23, 
      24, 25, 26, -1, 27, -1, 28, -1, 
      29, -1, -1, -1, -1, -1
};

static const int port_speed_max_cascade[] = {
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10,  10,  10,  10,  10,  10,  10,  
      10,  10,  10,  10,  10,  10,  10,  10,  
      10,  10, 110,  -1, 110,  -1, 130,  -1, 
      130, -1,  -1,  -1,  -1,  -1
};

/* 16x1G(4xQSGMII) + 8x2.5G + 2x10G + 2x13G*/
static const int port_speed_max_cascade_2p5[] = {
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10,  10,  10,  10,  10,  10,  10,  
      10,  10,  25,  25,  25,  25,  25,  25,  
      25,  25, 110,  -1, 110,  -1, 130,  -1, 
      130, -1,  -1,  -1,  -1,  -1
};

/* Non-Cascade Mode */
/* 24x1GE + 4x10GE */
static const int port_speed_max_non_cascade[] = {
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10,  10,  10,  10,  10,  10,  10,
      10,  10,  10,  10,  10,  10,  10,  10,  
      10,  10, 110,  -1, 110,  -1, 110,  -1, 
     110,  -1,  -1,  -1,  -1,  -1
};

/* 16x1GE(QSGMII)+ 8x2.5G + 4x10GE */
static const int port_speed_max_non_cascade_2p5[] = {
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10,  10,  10,  10,  10,  10,  10,
      10,  10,  25,  25,  25,  25,  25,  25,  
      25,  25, 110,  -1, 110,  -1, 110,  -1, 
     110,  -1,  -1,  -1,  -1,  -1
};
/* 53401 option 3/56062/5345x/5342x */
static const int p2l_mapping_cascade_1[] = {
       0, -1,  2,  3,  4,  5,  6,  7, 
       8,  9, -1, -1, -1, -1, -1, -1,
      -1, -1, 10, 11, 12, 13, 14, 15, 
      16, 17, 18, 19, 20, 21, 22, 23, 
      24, 25, 26, 27, 28, 29 
};
static const int port_speed_max_cascade_1[] = {
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10, 110, 110, 110, 110,  10,  10,  
      10,  10,  10,  10,  10,  10
};
/* 8x1GE + 16x2.5G + 4x10G */
static const int port_speed_max_56062_op1[] = {
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  25,  25,  25,  25,  25,  25,  
      25,  25,  25,  25,  25,  25,  25,  25,  
      25,  25, 110, 110, 110, 110
};
/* 8x1GE + 8x2.5G + 6x10G */
static const int port_speed_max_56062_op2[] = {
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  25,  25,  25,  25,  25,  25,  
      25,  25, 100,  -1,  -1,  -1, 100,  -1,  
      -1,  -1, 110, 110, 110, 110
};
/* 20x2.5G + 4x10G */
static const int port_speed_max_53454[] = {
      -1,  -1,  25,  25,  25,  25,  -1,  -1,  
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  25,  25,  25,  25,  25,  25,  
      25,  25, 110, 110, 110, 110,  25,  25,  
      25,  25,  25,  25,  25,  25
};
/* 8x1GE + 16x2.5G + 4x10G */
static const int port_speed_max_53456[] = {
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  25,  25,  25,  25,  25,  25,  
      25,  25, 110, 110, 110, 110,  25,  25,  
      25,  25,  25,  25,  25,  25
};
/* 8x1G + 2x10G */
static const int port_speed_max_53422[] = {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10, 110, 110,  -1,  -1,  -1,  -1,  
      -1,  -1,  -1,  -1,  -1,  -1
};
/* 8x1GE + 16x1G + 4x10G */
static const int port_speed_max_53424[] = {
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10, 110, 110, 110, 110,  10,  10,  
      10,  10,  10,  10,  10,  10
};
/* 20x1G + 4x10G */
static const int port_speed_max_53426[] = {
      -1,  -1,  10,  10,  10,  10,  -1,  -1,  
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  10,  10,  10,  10,  10,  10,  
      10,  10, 110, 110, 110, 110,  10,  10,  
      10,  10,  10,  10,  10,  10
};
/* 16x1G + 4x2.5G */
static const int port_speed_max_56066[] = {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  10,  10,  10,  10,  10,  10,
      10,  10,  25,  25,  25,  25,  10,  10,
      10,  10,  10,  10,  10,  10
};
/* 53403 */
/* 13x1GE + 5x10G-KR + 1xXAUI*/
static const int p2l_mapping_53403[] = {
     0, -1,  2, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  3,  4,  5,  6,  7,  8,  
     9, 10, 11, 12, 13, 14, 15, -1, 
    16, -1, 17, 18, 19, 20
};

static const int port_speed_max_53403[] = {
     -1,  -1, 100,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  10,  10,  10,  10,  10,  10,       
     10,  10,  10,  10,  10,  10,  10,  -1,
    100,  -1, 100, 100, 100, 100 
};

/* 53404 */
static const int p2l_mapping_53404[] = {
     0, -1,  2, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  3,  4,  5,  6,  7,  8,  
     9, 10, 11, 12, 13, -1, 14, -1, 
    -1, -1, -1, -1, -1, -1
};
static const int port_speed_max_53404_op1[] = {
     -1,  -1, 100,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  10,  10,  10,  10,  10,  10,       
     10,  10, 100,  -1,  -1,  -1,  10,  -1,
     -1,  -1,  -1,  -1,  -1,  -1 
};
static const int port_speed_max_53404_op2[] = {
     -1,  -1, 100,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  10,  10,  10,  10,  10,  10,
     10,  -1, 100, 100, 100,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1 
};
/* 53406 option 2&3 */
static const int p2l_mapping_53406_op2[] = {
     0, -1,  2, -1,  4, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1,  6, -1, -1, -1, 10, -1, 
    -1, -1, 14, 15, 16, 17, 18, 19, 
    20, 21, 22, 23, 24, 25
};
static const int port_speed_max_53406_op2[] = {
     -1,  -1, 100,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 100,  -1,  -1,  -1, 100,  -1,
     -1,  -1, 100,  -1,  -1,  -1, 110, 110,
    110, 110, 110, 110, 110, 110 
};

/* 53406 option 4 */
static const int port_speed_max_53406_op4[] = {
     -1,  -1,  10,  10,  10,  10,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  10,  10,  10,  10,  10, 110,
    110, 110, 110, 110, 110, 110, 110, 110,
    110, 110, 110, 110, 110, 110 
};

static const int port_blk_gport[] = {
      -1, -1,
      0,  0,  0,  0,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
};
static const int port_blk_xlport[] = {
      -1, -1,
       1,  1,  1,  1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
};

static const int speed_limit_56060_op2[] = {
    0x24,  0x3,  0x4,  0x5, 
    0x25, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19,
    0x1a, 0x1b, 0x1c, 0x1d,
    0x1e, 0x1f, 0x20, 0x21,
    0x22, 0x23,  0x2, 0x12
};
static const int speed_limit_53406_op2[] = {
     0x2,  0x3,  0x4,  0x5, 
    0x1b, 0x13, 0x14, 0x15,
    0x1c, 0x17, 0x18, 0x19,
    0x1a, 0x12, 0x16, 0x1d,
    0x1e, 0x1f, 0x20, 0x21,
    0x22, 0x23, 0x24, 0x25
};
static const int speed_limit_53406_op4[] = {
    0x16, 0x17, 0x18, 0x19, 
    0x12, 0x13, 0x14, 0x15,
     0x2,  0x3,  0x4,  0x5,
    0x1a, 0x1b, 0x1c, 0x1d,
    0x1e, 0x1f, 0x20, 0x21,
    0x22, 0x23, 0x24, 0x25
};
static const int speed_limit_53401_op3[] = {
     0x2,  0x3,  0x4,  0x5,
    0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19,
    0x1a, 0x1e, 0x1c, 0x20,
    0x1b, 0x1f, 0x1d, 0x21,
    0x22, 0x23, 0x24, 0x25
};
static const int speed_limit_53456_op1[] = {
     0x2,  0x3,  0x4,  0x5, 
    0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19,
    0x1a, 0x1e, 0x1c, 0x20,
    0x1b, 0x1f, 0x1d, 0x21,
    0x22, 0x23, 0x24, 0x25
};

/* Embedded Mode TDM */    
/* 56060 option 1, 53405/53415 */
uint32 gh_tdm_10g_embedded[68] = {
        22, 23, 24, 25, 26, 27,
        28, 29, 30, 31, 32, 33,
        34, 35, 36, 37,  0, 22,
        23, 24, 25, 26, 27, 28,
        29, 30, 31, 32, 33, 34,
        35, 36, 37, 63, 22, 23,
        24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35,
        36, 37, 63, 22, 23, 24,
        25, 26, 27, 28, 29, 30,
        31, 32, 33, 34, 35, 36,
        37, 63};    
/*  Mixed Embeded Mode TDM */
/* 53406/53416 option 1*/
uint32 gh_tdm_mixed_embeded_4x5g[68] = {
        26, 27, 28, 29, 30, 31,
        32, 33,  2, 18, 22, 34,
        35, 36, 37,  0,  3, 26,
        27, 28, 29, 30, 31, 32,
        33,  4, 19, 23, 34, 35,
        36, 37, 63,  5, 26, 27,
        28, 29, 30, 31, 32, 33,
         2, 20, 24, 34, 35, 36,
        37, 63,  3, 26, 27, 28,
        29, 30, 31, 32, 33,  4,
        21, 25, 34, 35, 36, 37,
        63,  5};
/* 53406/53416 option 1*/    
uint32 gh_tdm_mixed_embeded_2xrxaui[68] = {
        26, 27, 28, 29, 30, 31,
        32, 33,  2, 18, 22, 34,
        35, 36, 37,  0,  4, 26,
        27, 28, 29, 30, 31, 32,
        33,  2, 19, 23, 34, 35,
        36, 37, 63,  4, 26, 27,
        28, 29, 30, 31, 32, 33,
         2, 20, 24, 34, 35, 36,
        37, 63,  4, 26, 27, 28,
        29, 30, 31, 32, 33,  2,
        21, 25, 34, 35, 36, 37,
        63,  4};        

/* Cascade Mode TDM */
/* 56064 option 1/2, 53401/53411 option 1/2 */
uint32 gh_tdm_cascade[96] = {
         2, 18, 26, 28, 30, 32,
         3, 19, 26, 28, 30, 32,
         4, 20, 26, 28, 30, 32,
         5, 21, 26, 28, 30, 32,
         6, 22, 26, 28, 30, 32,
         7, 23, 26, 28, 30, 32,
         8, 24, 26, 28, 30, 32,
         9, 25, 26, 28, 30, 32,             
        10,  0, 26, 28, 30, 32,
        11, 63, 26, 28, 30, 32,
        12, 63, 26, 28, 30, 32,
        13, 63, 26, 28, 30, 32,
        14, 63, 26, 28, 30, 32,
        15, 63, 26, 28, 30, 32,
        16, 63, 26, 28, 30, 32,
        17, 63, 26, 28, 30, 32 
    };            
/* cascade mode for 53401 op3 */
uint32 gh_tdm_cascade_1[78] = {
        26, 27, 28, 29,  2, 18,
        26, 27, 28, 29,  3, 19,
        26, 27, 28, 29,  4, 20,
        26, 27, 28, 29,  5, 21,
        26, 27, 28, 29,  6, 22,
        26, 27, 28, 29,  7, 23,
        26, 27, 28, 29,  8, 24,
        26, 27, 28, 29,  9, 25,
        26, 27, 28, 29, 30, 34,
        26, 27, 28, 29, 31, 35,
        26, 27, 28, 29, 32, 36,
        26, 27, 28, 29, 33, 37,
        26, 27, 28, 29,  0, 63
    };            
/* 53456/53457 op1 op2*/    
uint32 gh_tdm_cascade_2p5[98] = {
    26,  28,  30,  18,  22,   2,
    32,  26,  28,  19,  23,   3,
    30,  32,  26,  20,  24,   4,
    28,  30,  32,  21,  25,   5,
    26,  28,  30,  18,  22,   6,
    32,  26,  28,  19,  23,   7,
    30,  32,  26,  20,  24,   8,
    28,  30,  32,  21,  25,   9,
    26,  28,  30,  18,  22,  10,
    32,  26,  28,  19,  23,  11,
    30,  32,  26,  20,  24,  12,
    28,  30,  32,  21,  25,  13,
    26,  28,  30,  18,  22,  14,
    32,  26,  28,  19,  23,  15,
    30,  32,  26,  20,  24,  16,
    28,  30,  32,  21,  25,  17,
     0,   63,
};

/* low port count - 10G embeded */
/* 53402/53412 */
uint32 gh_tdm_low_port_count_10g_embeded[36] = {
        22, 23, 24, 25, 26, 27,
        28, 29,  0, 22, 23, 24,
        25, 26, 27, 28, 29, 63,
        22, 23, 24, 25, 26, 27,
        28, 29, 63, 22, 23, 24,
        25, 26, 27, 28, 29, 63};

/* mixed embeded -2.5G */
/* 53408/53418 */
uint32 gh_tdm_mixed_embeded_2p5g[28] = {
         2, 18, 22, 26, 30, 34,
         0,  3, 19, 23, 27, 31,
        35, 63,  4, 20, 24, 28,
        32, 36, 63,  5, 21, 25,
        29, 33, 37, 63};

/* mixed embeded - 10G */
/* 56060 option 2 */
uint32 gh_tdm_mixed_embeded_10g[68] = {
        22, 23, 24, 25, 26, 27,
        28, 29, 30, 31, 32, 33,
        34, 35,  2, 18,  0, 22,
        23, 24, 25, 26, 27, 28,
        29, 30, 31, 32, 33, 34,
        35,  2, 18, 63, 22, 23,
        24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35,
         2, 18, 63, 22, 23, 24,
        25, 26, 27, 28, 29, 30,
        31, 32, 33, 34, 35,  2,
        18, 63};
/* 53403 */
uint32 gh_tdm_mixed_embeded_53403[80] = {
        18, 22,  2, 32, 34, 35,
        36, 37, 19, 23,  2, 32,
        34, 35, 36, 37, 20, 24,
         2, 32, 34, 35, 36, 37,
        21, 25,  2, 32, 34, 35,
        36, 37, 26, 63,  2, 32,
        34, 35, 36, 37, 27, 63,
         2, 32, 34, 35, 36, 37,
        28, 63,  2, 32, 34, 35,
        36, 37, 29, 63,  2, 32,
        34, 35, 36, 37, 30, 63,
         2, 32, 34, 35, 36, 37,
         0, 63,  2, 32, 34, 35,
        36, 37};
/* 53404 */
uint32 gh_tdm_mixed_embeded_53404[36] = {
        18, 22,  2, 26, 27, 28,
        19, 23,  2, 26, 27, 28,
        20, 24,  2, 26, 27, 28,
        21, 25,  2, 26, 27, 28,
        30, 63,  2, 26, 27, 28,
         0, 63,  2, 26, 27, 28,
};
/* 53406 option 2&3 */
uint32 gh_tdm_mixed_embeded_53406_op2[68]={
        26, 27, 28, 29, 30, 31,
        32, 33,  2, 18, 22, 34,
        35, 36, 37,  0,  4, 26,
        27, 28, 29, 30, 31, 32,
        33,  2, 18, 22, 34, 35,
        36, 37, 63,  4, 26, 27,
        28, 29, 30, 31, 32, 33,
         2, 18, 22, 34, 35, 36,
        37, 63,  4, 26, 27, 28,
        29, 30, 31, 32, 33,  2,
        18, 22, 34, 35, 36, 37,
        63,  4,             
};
/* 53406 option 4 */
uint32 gh_tdm_mixed_embeded_53406_op4[101]={
        26,  27,  28,  29,  34,  35,
        36,  37,  30,  31,  32,  33,
        23,  24,  25,  26,  27,  28,
        29,  34,  35,  36,  37,  30,
        31,  32,  33,  23,  24,  25,
        26,  27,  28,  29,  34,  35,
        36,  37,  30,  31,  32,  33,
        23,  24,  25,  26,  27,  28,
        29,  34,  35,  36,  37,  30,
        31,  32,  33,  23,  24,  25,
        26,  27,  28,  29,  34,   2,
        35,  36,  37,  30,  31,   3,
        32,  33,  23,  24,  25,   4,
        26,  27,  28,  29,  34,   5,
        35,  36,  37,  30,  31,  18,
        32,  33,  23,  24,  25,  22,
        19,  20,  21,   0,  63
};

/* 56062 */
uint32 gh_tdm_mixed_embeded_56062[60]={
        34, 35, 18, 22, 26,  2,
        36, 37, 19, 23, 27,  3,
        34, 35, 20, 24, 28,  4,
        36, 37, 21, 25, 29,  5,
         0, 63, 30, 31, 32, 33,
        34, 35, 18, 22, 26,  6,
        36, 37, 19, 23, 27,  7,
        34, 35, 20, 24, 28,  8,
        36, 37, 21, 25, 29,  9,
        63, 63, 30, 31, 32, 33,
};
uint32 gh_tdm_mixed_embeded_56062_op2[58]={
        34, 35, 36, 37, 30,  2,
        26, 18, 20, 22, 24,  3,
        34, 35, 36, 37, 30,  4,
        26, 18, 20, 22, 24,  5,
        19, 21, 23, 25,  0, 63,
        34, 35, 36, 37, 30,  6,
        26, 18, 20, 22, 24,  7,
        34, 35, 36, 37, 30,  8,
        26, 18, 20, 22, 24,  9,
        19, 21, 23, 25,
};
/* Elkhound -53455 */
/* 53454/53455/53456/53457 */
uint32 gh_tdm_elkhound_53455[48]={
        26, 27, 18, 22,  2,  0,
        28, 29, 30, 34,  3, 63,
        26, 27, 19, 23,  4, 63,
        28, 29, 31, 35,  5, 63,
        26, 27, 20, 24,  6, 63,
        28, 29, 32, 36,  7, 63,
        26, 27, 21, 25,  8, 63,
        28, 29, 33, 37,  9, 63,
};
/* Bloodhound -53422 */
/* 53422/53424/53426 */
uint32 gh_tdm_bloodhound_53422[78]={
        26, 27, 28, 29, 18,  2,
        26, 27, 28, 29, 19,  3,
        26, 27, 28, 29, 20,  4,
        26, 27, 28, 29, 21,  5,
        26, 27, 28, 29, 22,  6,
        26, 27, 28, 29, 23,  7,
        26, 27, 28, 29, 24,  8,
        26, 27, 28, 29, 25,  9,
        26, 27, 28, 29, 30, 34,
        26, 27, 28, 29, 31, 35,
        26, 27, 28, 29, 32, 36,
        26, 27, 28, 29, 33, 37,
        26, 27, 28, 29,  0, 63,
};
#define _GH_MAX_TSC_COUNT   (6)

/* TSC lane mode */
typedef enum _gh_tsc_lane_mode_e {
    _GH_TSC_LANE_MODE_SINGLE = 0,
    _GH_TSC_LANE_MODE_XAUI = 1,
    _GH_TSC_LANE_MODE_RXAUI = 2,
    _GH_TSC_LANE_MODE_RXAUI13G = 3, /* special mode for HG-Duo[13](RXAUI) */
    _GH_TSC_LANE_MODE_CNT
} _gh_tsc_lane_mode_t;

/* Flexible Lane Mode bitmap (special mode for HG-Duo[13] is not included */
#define _GH_FLEX_LANE_MODE_SINGLE   (1 << _GH_TSC_LANE_MODE_SINGLE)
#define _GH_FLEX_LANE_MODE_XAUI     (1 << _GH_TSC_LANE_MODE_XAUI)
#define _GH_FLEX_LANE_MODE_RXAUI    (1 << _GH_TSC_LANE_MODE_RXAUI)

#define _GH_FLEX_LANE_MODE_ALL    \
        (_GH_FLEX_LANE_MODE_SINGLE | _GH_FLEX_LANE_MODE_XAUI | \
        _GH_FLEX_LANE_MODE_RXAUI)

#define _GH_FLEX_LANE_MODE_IS_SUPPORT(lane_mode_bmp, lane_mode) \
        (((lane_mode_bmp) != 0) && ((lane_mode) != -1) && \
        ((lane_mode_bmp) & (1 << (lane_mode))))

#define _GH_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define _GH_MAX_LPORT_COUNT (24)

static soc_reg_t speed_limit_regs[_GH_MAX_LPORT_COUNT] = {
    SPEED_LIMIT_ENTRY0_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY1_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY2_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY3_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY4_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY5_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY6_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY7_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY8_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY9_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY10_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY11_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY12_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY13_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY14_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY15_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY16_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY17_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY18_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY19_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY20_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY21_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY22_PHYSICAL_PORT_NUMBERr,
    SPEED_LIMIT_ENTRY23_PHYSICAL_PORT_NUMBERr
};
static const int lane_single_override[]={1, 1, 1, 1};
static const int lane_rxaui_override[]={1, -1, 1, -1};
static const int lane_xaui_override[]={1, -1, -1, -1};
static const int lane_rxaui_speed_override[]={100, -1, 100, -1};
static const int lane_xaui_speed_override[]={100, -1, -1, -1};
static const int lane_disable_override[]={-1, -1, -1, -1};
static const int tsc_phy_port[] = {18, 22, 26, 30, 34, 2};

static int matched_devid_idx = -1;
static int _gh_port_config_id = -1;

typedef struct _gh_tsc_info_s {
    int     lane_mode;
    uint8   valid;
    int     phy_port_base;
}_gh_tsc_info_t; 

typedef struct _gh_sku_speed_limit_info_s {
    uint16      dev_id;
    int         config_op; /* sku option */
    const int   *phy_port_mapping;
}_gh_sku_speed_limit_info_t;

typedef struct _gh_sku_info_s {
    uint16      dev_id;
    int         config_op; /* sku option */
    int         dual_port_idx; /* 0/1:tsc5 is n/a or exist */
    const int   *port_blk_type; /* tsc5 is gxport or xlport */
    const int   *p2l_mapping;
    const int   *speed_max;
    uint32      *tdm_table;
    int         tdm_table_size;
    uint32      flex_tsc_bmp; /* 6 bits: [tsc5...tsc0] */
    uint32      disabled_tsc_bmp; 
    int         default_lane_mode[_GH_MAX_TSC_COUNT];
    uint32      flex_lane_modes_bmp[_GH_MAX_TSC_COUNT];
}_gh_sku_info_t;
    
static _gh_tsc_info_t _gh_tsc[_GH_MAX_TSC_COUNT];

STATIC _gh_sku_speed_limit_info_t _gh_sku_speed_limit[] = {
{BCM56060_DEVICE_ID, 2, speed_limit_56060_op2},
{BCM53406_DEVICE_ID, 2, speed_limit_53406_op2},
{BCM53406_DEVICE_ID, 3, speed_limit_53406_op2},
{BCM53406_DEVICE_ID, 4, speed_limit_53406_op4},
{BCM53369_DEVICE_ID, 2, speed_limit_53406_op2},
{BCM53369_DEVICE_ID, 3, speed_limit_53406_op2},
{BCM53369_DEVICE_ID, 4, speed_limit_53406_op4},
{BCM53369_DEVICE_ID, 5, speed_limit_53406_op4},
{BCM53456_DEVICE_ID, 1, speed_limit_53456_op1},
{BCM53456_DEVICE_ID, 2, speed_limit_53456_op1},
{BCM53424_DEVICE_ID, 1, speed_limit_53456_op1},
{BCM53424_DEVICE_ID, 2, speed_limit_53456_op1},
{BCM53401_DEVICE_ID, 3, speed_limit_53401_op3},
{0,0,0}
};

STATIC _gh_sku_info_t _gh_sku_port_config[] = {
{BCM53401_DEVICE_ID,  1, 1, port_blk_gport, 
    p2l_mapping_cascade, port_speed_max_non_cascade, 
    gh_tdm_cascade, _GH_ARRAY_SIZE(gh_tdm_cascade),
    0, 0x10,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     -1, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}},
{BCM53401_DEVICE_ID,  2, 1, port_blk_gport, 
    p2l_mapping_cascade, port_speed_max_cascade, 
    gh_tdm_cascade, _GH_ARRAY_SIZE(gh_tdm_cascade),
    0, 0x10,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_RXAUI13G,
     -1, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}},
{BCM53401_DEVICE_ID,  3, 1, port_blk_gport, 
    p2l_mapping_cascade_1, port_speed_max_cascade_1, 
    gh_tdm_cascade_1, _GH_ARRAY_SIZE(gh_tdm_cascade_1),
    0, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}},
{BCM53402_DEVICE_ID, -1, 0, NULL, 
    p2l_mapping_embedded_8x10g, port_speed_max_embedded_8x10g, 
    gh_tdm_low_port_count_10g_embeded, _GH_ARRAY_SIZE(gh_tdm_low_port_count_10g_embeded),
    0x6, 0x39,
    {-1, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, -1,
     -1, -1},
    {0, _GH_FLEX_LANE_MODE_ALL, 
     _GH_FLEX_LANE_MODE_ALL, 0,
     0, 0}}, 
{BCM53403_DEVICE_ID, -1, 1, port_blk_xlport, 
    p2l_mapping_53403, port_speed_max_53403, 
    gh_tdm_mixed_embeded_53403, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_53403),
    0, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_XAUI},
    {0, 0, 0, 0, 0, 0}}, 
{BCM53404_DEVICE_ID,  1, 1, port_blk_xlport, 
    p2l_mapping_53404, port_speed_max_53404_op1, 
    gh_tdm_mixed_embeded_53404, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_53404),
    0, 0x10,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_SINGLE,
     -1, _GH_TSC_LANE_MODE_XAUI},
    {0, 0, 0, 0, 0, 0}}, 
{BCM53404_DEVICE_ID,  2, 1, port_blk_xlport, 
    p2l_mapping_53404, port_speed_max_53404_op2, 
    gh_tdm_mixed_embeded_53404, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_53404),
    0, 0x18,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, -1,
     -1, _GH_TSC_LANE_MODE_XAUI},
    {0, 0, 0, 0, 0, 0}}, 
{BCM53405_DEVICE_ID, -1, 0, NULL, 
    p2l_mapping_embedded_16x10g, port_speed_max_embedded_16x10g, 
    gh_tdm_10g_embedded, _GH_ARRAY_SIZE(gh_tdm_10g_embedded),
    0x1e, 0x21,
    {-1, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, -1}, 
    {0, _GH_FLEX_LANE_MODE_ALL, 
     _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL, 
     _GH_FLEX_LANE_MODE_ALL, 0}},
{BCM53365_DEVICE_ID, -1, 0, NULL, 
    p2l_mapping_embedded_16x10g, port_speed_max_embedded_16x10g, 
    gh_tdm_10g_embedded, _GH_ARRAY_SIZE(gh_tdm_10g_embedded),
    0x1e, 0x21,
    {-1, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, -1}, 
    {0, _GH_FLEX_LANE_MODE_ALL, 
     _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL, 
     _GH_FLEX_LANE_MODE_ALL, 0}}, 
{BCM53406_DEVICE_ID,  1, 1, port_blk_xlport, 
    p2l_mapping_mixed_embeded, port_speed_max_mixed_embeded, 
    gh_tdm_mixed_embeded_4x5g, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_4x5g),
    0x3c, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE}, 
    {0, 0, 
     _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL, 
     _GH_FLEX_LANE_MODE_ALL, (_GH_FLEX_LANE_MODE_SINGLE | _GH_FLEX_LANE_MODE_XAUI)}}, 
{BCM53406_DEVICE_ID,  2, 1, port_blk_xlport, 
    p2l_mapping_53406_op2, port_speed_max_53406_op2, 
    gh_tdm_mixed_embeded_53406_op2, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_53406_op2), 
    0x18, 0,
    {_GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_XAUI,
     _GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_XAUI},
    {0, 0, 0, _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL, 0}}, 
{BCM53406_DEVICE_ID,  3, 1, port_blk_xlport, 
    p2l_mapping_53406_op2, port_speed_max_53406_op2, 
    gh_tdm_mixed_embeded_53406_op2, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_53406_op2), 
    0, 0x10,
    {_GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_XAUI,
     _GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_SINGLE,
     -1, _GH_TSC_LANE_MODE_XAUI},
    {0, 0, 0, 0, 0, 0}},      
{BCM53406_DEVICE_ID,  4, 1, port_blk_xlport, 
    p2l_mapping_mixed_embeded, port_speed_max_53406_op4, 
    gh_tdm_mixed_embeded_53406_op4, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_53406_op4), 
    0x1c, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 
     _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL, 
     _GH_FLEX_LANE_MODE_ALL, 0}},
{BCM53369_DEVICE_ID,  1, 1, port_blk_xlport,
    p2l_mapping_mixed_embeded, port_speed_max_mixed_embeded,
    gh_tdm_mixed_embeded_4x5g, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_4x5g),
    0x3c, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0,
     _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL,
     _GH_FLEX_LANE_MODE_ALL, (_GH_FLEX_LANE_MODE_SINGLE | _GH_FLEX_LANE_MODE_XAUI)}},
{BCM53369_DEVICE_ID,  2, 1, port_blk_xlport,
    p2l_mapping_53406_op2, port_speed_max_53406_op2,
    gh_tdm_mixed_embeded_53406_op2, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_53406_op2),
    0x18, 0,
    {_GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_XAUI,
     _GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_XAUI},
    {0, 0, 0, _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL, 0}},
{BCM53369_DEVICE_ID,  3, 1, port_blk_xlport,
    p2l_mapping_53406_op2, port_speed_max_53406_op2,
    gh_tdm_mixed_embeded_53406_op2, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_53406_op2),
    0, 0x10,
    {_GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_XAUI,
     _GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_SINGLE,
     -1, _GH_TSC_LANE_MODE_XAUI},
    {0, 0, 0, 0, 0, 0}},
{BCM53369_DEVICE_ID,  4, 1, port_blk_xlport,
    p2l_mapping_mixed_embeded, port_speed_max_53406_op4,
    gh_tdm_mixed_embeded_53406_op4, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_53406_op4),
    0x1c, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0,
     _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL,
     _GH_FLEX_LANE_MODE_ALL, 0}},
{BCM53369_DEVICE_ID, 5, 0, NULL,
    p2l_mapping_embedded_16x10g, port_speed_max_embedded_16x10g,
    gh_tdm_10g_embedded, _GH_ARRAY_SIZE(gh_tdm_10g_embedded),
    0x1e, 0x21,
    {-1, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, -1},
    {0, _GH_FLEX_LANE_MODE_ALL, 
     _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL,
     _GH_FLEX_LANE_MODE_ALL, 0}},
{BCM53408_DEVICE_ID, -1, 1, port_blk_xlport, 
    p2l_mapping_mixed_embeded, port_speed_max_mixed_embeded_24x2p5, 
    gh_tdm_mixed_embeded_2p5g, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_2p5g),
    0, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}}, 
{BCM56060_DEVICE_ID,  1, 0, NULL, 
    p2l_mapping_embedded_16x10g, port_speed_max_embedded_16x10g, 
    gh_tdm_10g_embedded, _GH_ARRAY_SIZE(gh_tdm_10g_embedded),
    0x4, 0x21,
    {-1, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, -1},
    {0, 0, _GH_FLEX_LANE_MODE_ALL, 0, 0, 0}}, 
{BCM56060_DEVICE_ID,  2, 1, port_blk_xlport, 
    p2l_mapping_mixed_embeded_16x10g, port_speed_max_mixed_embeded_16x10g, 
    gh_tdm_mixed_embeded_10g, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_10g),
    0x4, 0,
    {_GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_XAUI},
    {0, 0, _GH_FLEX_LANE_MODE_ALL, 0, 0, 0}}, 
{BCM56062_DEVICE_ID, 1, 1, port_blk_gport, 
    p2l_mapping_cascade_1, port_speed_max_56062_op1, 
    gh_tdm_mixed_embeded_56062, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_56062),
    0, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}},
{BCM56062_DEVICE_ID, 2, 1, port_blk_gport, 
    p2l_mapping_cascade_1, port_speed_max_56062_op2, 
    gh_tdm_mixed_embeded_56062_op2, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_56062_op2),
    0x3, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_XAUI, _GH_TSC_LANE_MODE_XAUI,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {(_GH_FLEX_LANE_MODE_SINGLE | _GH_FLEX_LANE_MODE_RXAUI), 
     (_GH_FLEX_LANE_MODE_SINGLE | _GH_FLEX_LANE_MODE_RXAUI), 
     0, 0, 
     0, 0}},
{BCM56062_DEVICE_ID,  3, 1, port_blk_gport, 
    p2l_mapping_cascade_1, port_speed_max_53456, 
    gh_tdm_elkhound_53455, _GH_ARRAY_SIZE(gh_tdm_elkhound_53455),
    0x4, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, _GH_FLEX_LANE_MODE_ALL, 0, 0, 0}},
{BCM56063_DEVICE_ID, -1, 1, port_blk_gport, 
    p2l_mapping_cascade, port_speed_max_non_cascade, 
    gh_tdm_cascade, _GH_ARRAY_SIZE(gh_tdm_cascade),
    0, 0x13,
    {-1, -1,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     -1, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}}, 
{BCM56064_DEVICE_ID,  1, 1, port_blk_gport, 
    p2l_mapping_cascade, port_speed_max_non_cascade, 
    gh_tdm_cascade, _GH_ARRAY_SIZE(gh_tdm_cascade),
    0, 0x10,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     -1, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}}, 
{BCM56064_DEVICE_ID,  2, 1, port_blk_gport, 
    p2l_mapping_cascade, port_speed_max_cascade, 
    gh_tdm_cascade, _GH_ARRAY_SIZE(gh_tdm_cascade),
    0, 0x10,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_RXAUI13G,
     -1, _GH_TSC_LANE_MODE_SINGLE}},
{BCM56065_DEVICE_ID, -1, 1, port_blk_xlport, 
    p2l_mapping_mixed_embeded, port_speed_max_mixed_embeded, 
    gh_tdm_mixed_embeded_4x5g, _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_4x5g),
    0x3c, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE}, 
    {0, 0, 
     _GH_FLEX_LANE_MODE_ALL, _GH_FLEX_LANE_MODE_ALL, 
     _GH_FLEX_LANE_MODE_ALL, (_GH_FLEX_LANE_MODE_SINGLE | _GH_FLEX_LANE_MODE_XAUI)}},          
{BCM56066_DEVICE_ID, -1, 0, NULL,
    p2l_mapping_cascade_1, port_speed_max_56066,
    gh_tdm_bloodhound_53422, _GH_ARRAY_SIZE(gh_tdm_bloodhound_53422),
    0x4, 0x20,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, -1},
    {0, 0,
     (_GH_FLEX_LANE_MODE_SINGLE | _GH_FLEX_LANE_MODE_XAUI), 0, 0, 0}},
{BCM53454_DEVICE_ID, -1, 1, port_blk_xlport, 
    p2l_mapping_cascade_1, port_speed_max_53454, 
    gh_tdm_elkhound_53455, _GH_ARRAY_SIZE(gh_tdm_elkhound_53455),
    0x4, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, _GH_FLEX_LANE_MODE_ALL, 0, 0, 0}},
{BCM53456_DEVICE_ID,  1, 1, port_blk_gport, 
    p2l_mapping_cascade, port_speed_max_non_cascade_2p5, 
    gh_tdm_cascade_2p5, _GH_ARRAY_SIZE(gh_tdm_cascade_2p5),
    0, 0x10,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     -1, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}},
{BCM53456_DEVICE_ID,  2, 1, port_blk_gport, 
    p2l_mapping_cascade, port_speed_max_cascade_2p5, 
    gh_tdm_cascade_2p5, _GH_ARRAY_SIZE(gh_tdm_cascade_2p5),
    0, 0x10,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_RXAUI13G,
     -1, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}},
{BCM53456_DEVICE_ID,  3, 1, port_blk_gport, 
    p2l_mapping_cascade_1, port_speed_max_53456, 
    gh_tdm_elkhound_53455, _GH_ARRAY_SIZE(gh_tdm_elkhound_53455),
    0x4, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, _GH_FLEX_LANE_MODE_ALL, 0, 0, 0}},
{BCM53422_DEVICE_ID, -1, 0, NULL, 
    p2l_mapping_cascade_1, port_speed_max_53422, 
    gh_tdm_bloodhound_53422, _GH_ARRAY_SIZE(gh_tdm_bloodhound_53422),
    0, 0x38,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, -1,
     -1, -1},
    {0, 0, 0, 0, 0, 0}},
{BCM53424_DEVICE_ID,  1, 1, port_blk_gport, 
    p2l_mapping_cascade, port_speed_max_non_cascade, 
    gh_tdm_cascade, _GH_ARRAY_SIZE(gh_tdm_cascade),
    0, 0x10,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     -1, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}},
{BCM53424_DEVICE_ID,  2, 1, port_blk_gport, 
    p2l_mapping_cascade, port_speed_max_cascade, 
    gh_tdm_cascade, _GH_ARRAY_SIZE(gh_tdm_cascade),
    0, 0x10,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_RXAUI13G,
     -1, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, 0, 0, 0, 0}},        
{BCM53424_DEVICE_ID, 3, 1, port_blk_gport, 
    p2l_mapping_cascade_1, port_speed_max_53424, 
    gh_tdm_bloodhound_53422, _GH_ARRAY_SIZE(gh_tdm_bloodhound_53422),
    0x4, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, _GH_FLEX_LANE_MODE_ALL, 0, 0, 0}},
{BCM53426_DEVICE_ID, -1, 1, port_blk_xlport, 
    p2l_mapping_cascade_1, port_speed_max_53426, 
    gh_tdm_bloodhound_53422, _GH_ARRAY_SIZE(gh_tdm_bloodhound_53422),
    0x4, 0,
    {_GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE,
     _GH_TSC_LANE_MODE_SINGLE, _GH_TSC_LANE_MODE_SINGLE},
    {0, 0, _GH_FLEX_LANE_MODE_ALL, 0, 0, 0}},
{0,0,0,0,0,0,0,0,0,0,{-1,-1,-1,-1,-1,-1},{0, 0, 0, 0, 0, 0}},
};

/* MMU related */
/* Greyhound uses 2MB/1MB of internal Buffers */
#define GH_MMU_BUFFER_SIZE_2MB    (2 * 1024 * 1024) 
#define GH_MMU_BUFFER_SIZE_1MB    (1 * 1024 * 1024) 

/* 24GE + 4HG +1CPU ports */ 
#define GH_MMU_TOTAL_PORTS      29 

#define GH_MMU_NUM_COS          8
#define GH_MMU_CELL_SIZE        128


#define GH_MMU_IN_COS_MIN_CELLS     16
#define GH_MMU_IN_COS_MIN_XQS       16

/* cos(8) * reserved cells */
#define GH_MMU_IN_PORT_STATIC_CELLS (GH_MMU_NUM_COS * GH_MMU_IN_COS_MIN_CELLS) 

/* Every port has 6K packet pointers(XQs) */
#define GH_MMU_IN_PORT_TOTAL_XQS    6144 
/* number of cos(8) * minimum xqs per cos */
#define GH_MMU_IN_PORT_STATIC_XQS   (GH_MMU_NUM_COS * GH_MMU_IN_COS_MIN_XQS) 
/* total xqs (8192)- static xqs */
#define GH_MMU_IN_PORT_DYNAMIC_XQS  (GH_MMU_IN_PORT_TOTAL_XQS - GH_MMU_IN_PORT_STATIC_XQS) 

/* SER related */
#define     GREYHOUND_SER_PARITY_ENABLED              (1)
#define     GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED    (0)

/* EEE reference count related  :
 * - The clock devider to generate ~1us reference pulses used by EEE timers.
 * - It is normally chip independent and should be delivered by chip team.
 *
 * GH's EEE_REF_COUNT :
 * 1. XLMAC : 'TSC_CLK' is the reference clock.
 * 2. UNIMAC : 'TS_CLK' is the reference clock.
 */
#define GH_TSC_CLK  645     /* for 644.531 MHz */
#define GH_TS_CLK   250     /* for 250 MHz */
#define GH_XLMAC_EEE_REF_CNT    GH_TSC_CLK
#define GH_UNIMAC_EEE_REF_CNT   GH_TS_CLK

typedef enum {
    _SOC_PARITY_INFO_TYPE_GENERIC,
    _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
    _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
    _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
    _SOC_PARITY_INFO_TYPE_COUNTER,
    _SOC_PARITY_INFO_TYPE_MMU_ECC,
    _SOC_PARITY_INFO_TYPE_MMU_XQ,
    _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
    _SOC_PARITY_INFO_TYPE_MMU_IPMCVLAN,
    _SOC_PARITY_INFO_TYPE_MMU_WRED,
    _SOC_PARITY_INFO_TYPE_MMU_E2EFC,
    _SOC_PARITY_INFO_TYPE_MMU_CBPPKTHDR,
    _SOC_PARITY_INFO_TYPE_PORT_MACRO,
    _SOC_PARITY_INFO_TYPE_OAM, /* Not parity, but same interrupt */
    _SOC_PARITY_INFO_TYPE_NUM
} _soc_parity_info_type_t;

#define _GH_SER_F_L3  0x1

typedef struct _soc_parity_info_s {
    soc_field_t             enable_field;
    soc_field_t             error_field;
    char                    *msg;
    soc_mem_t               mem;
    _soc_parity_info_type_t type;
    soc_reg_t               control_reg;
    soc_reg_t               intr_status0_reg;
    soc_reg_t               intr_status1_reg; /* Also SBE force for ECC */
    soc_reg_t               nack_status0_reg;
    soc_reg_t               nack_status1_reg; /* Also DBE force for ECC */
    soc_field_t             clear_field; /* For level 1 */
    soc_reg_t               level2_error_reg; /* For MMU level 2 */
    soc_reg_t               level2_error_vld_reg;
    soc_reg_t               level2_error_multi_reg;
    soc_reg_t               level2_error_clear_reg;
    uint32                  feature;
} _soc_gh_parity_info_t;

/*
 *    _SOC_PARITY_INFO_TYPE_SINGLE_PARITY
 *      PARITY_EN
 *      ENTRY_IDX, MULTIPLE_ERR, PARITY_ERR
 *
 *    _SOC_PARITY_INFO_TYPE_SINGLE_ECC
 *      ECC_EN
 *      ENTRY_IDX, DOUBLE_BIT_ERR, MULTIPLE_ERR, ECC_ERR
 *
 *    _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER
 *      PARITY_EN
 *      PORT_IDX, COUNTER_IDX, MULTIPLE_ERR, PARITY_ERR
 *
 *    _SOC_PARITY_INFO_TYPE_DUAL_PARITY
 *      PARITY_EN
 *      BUCKET_IDX, MULTIPLE_ERR, PARITY_ERR_BM
 */

STATIC _soc_gh_parity_info_t _soc_rg2_ip0_parity_info[] = {
    { PARITY_ENf, VFP_POLICY_PAR_ERRf, "VFP_POLICY table parity error",
      VFP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFP_POLICY_PARITY_CONTROLr,
      VFP_POLICY_PARITY_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_gh_parity_info_t _soc_gh_ip0_parity_info[] = {
    /* { PARITY_ENf, VXLT_PAR_ERRf,"VLAN_XLATE table parity error",
      VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      VLAN_XLATE_PARITY_CONTROLr,
      VLAN_XLATE_PARITY_STATUS_0r, VLAN_XLATE_PARITY_STATUS_1r,
      INVALIDr, INVALIDr }, */
 /* { PPA_CMD_COMPLETEf }, */
 /* { MEM_RESET_COMPLETEf }, */
 /* { AGE_CMD_COMPLETEf }, */
    { PARITY_ENf, VLAN_SUBNET_DATA_PARITY_ERRf, "VLAN_SUBNET_DATA table parity error",
      VLAN_SUBNET_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_SUBNET_DATA_PARITY_CONTROLr,
      VLAN_SUBNET_DATA_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PROTOCOL_DATA_PARITY_ERRf, "VLAN_PROTOCOL_DATA table parity error",
      VLAN_PROTOCOL_DATAm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PROTOCOL_DATA_PARITY_CONTROLr,
      VLAN_PROTOCOL_DATA_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PARITY_ERRf, "VLAN table parity error",
      VLAN_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PARITY_CONTROLr,
      VLAN_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_STG_PARITY_ERRf, "VLAN_STG table parity error",
      STG_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_STG_PARITY_CONTROLr,
      VLAN_STG_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { L3_TUNNEL_RAM_PARITY_ERRf }, */
    { PARITY_ENf, SOURCE_TRUNK_MAP_PARITY_ERRf, "SOURCE_TRUNK_MAP table parity error",
      SOURCE_TRUNK_MAP_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SOURCE_TRUNK_MAP_PARITY_CONTROLr,
      SOURCE_TRUNK_MAP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { PARITY_ENf, VFP_POLICY_PAR_ERRf, "VFP_POLICY table parity error",
      VFP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFP_POLICY_PARITY_CONTROLr,
      VFP_POLICY_PARITY_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr }, */
    { PARITY_ENf, LMEP_PAR_ERRf, "LMEP table parity error",
      LMEPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      LMEP_PARITY_CONTROLr,
      LMEP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, LMEP_DA_PAR_ERRf, "LMEP_DA table parity error",
      LMEP_DAm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      LMEP_DA_PARITY_CONTROLr,
      LMEP_DA_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { IARB_PKT_ECC_INTR} */
    { ECC_ENf, IARB_PKT_ECC_INTRf, "Iarb packet ecc error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      IARB_PKT_ECC_CONTROLr,
      IARB_PKT_ECC_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    /* {PORT_TABLE_1BIT_ERR_INTR} */
    { ECC_ENf, PORT_TABLE_ECC_INTRf, "PORT table ecc error",
      PORT_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PORT_TABLE_ECC_CONTROLr,
      PORT_TABLE_ECC_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, SYSTEM_CONFIG_PAR_ERRf, "SYSTEM_CONFIG table error",
      SYSTEM_CONFIG_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SYSTEM_CONFIG_TABLE_PARITY_CONTROLr,
      SYSTEM_CONFIG_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, UDF_OFFSET_PAR_ERRf, "UDF_OFFSET table error",
      FP_UDF_OFFSETm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      UDF_OFFSET_PARITY_CONTROLr,
      UDF_OFFSET_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, ING_MOD_MAP_PAR_ERRf, "ING_MOD_MAP table error",
      ING_MOD_MAP_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      ING_MOD_MAP_PARITY_CONTROLr,
      ING_MOD_MAP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* {LPORT_TABLE_1BIT_ERR_INTR} */
    { ECC_ENf, LPORT_TABLE_ECC_INTRf, "LPORT table error",
      LPORT_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      LPORT_TABLE_ECC_CONTROLr,
      LPORT_TABLE_ECC_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, ING_ETAG_PCP_MAPPING_PAR_ERRf, "ING_ETAG_PCP_MAPPING table error",
      ING_ETAG_PCP_MAPPINGm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      ING_ETAG_PCP_MAPPING_PARITY_CONTROLr,
      ING_ETAG_PCP_MAPPING_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_gh_parity_info_t _soc_rg2_ip1_parity_info[] = {
    { PARITY_ENf, INITIAL_L3_ECMP_GROUP_PAR_ERRf, 
      "INITIAL_L3_ECMP_GROUP table parity error",
      INITIAL_L3_ECMP_GROUPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      INITIAL_L3_ECMP_GROUP_PARITY_CONTROLr,
      INITIAL_L3_ECMP_GROUP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, INITIAL_L3_ECMP_PAR_ERRf, "INITIAL_L3_ECMP table parity error",
      INITIAL_L3_ECMPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      INITIAL_L3_ECMP_PARITY_CONTROLr,
      INITIAL_L3_ECMP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};


STATIC _soc_gh_parity_info_t _soc_gh_ip1_parity_info[] = {
    /* { VFI_PAR_ERRf, }, */
    /* { SVP_PAR_ERRf, }, */
    /* { L3_IIF_PAR_ERRf, }, */
    /* { MPLS_ENTRY_PAR_ERRf, }, */
    { PARITY_ENf, L2_ENTRY_PAR_ERRf, "L2_ENTRY table parity error",
      L2Xm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      L2_ENTRY_PARITY_CONTROLr,
      L2_ENTRY_PARITY_STATUS_0r, L2_ENTRY_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L2MC_PAR_ERRf, "L2MC table parity error",
      L2MCm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L2MC_PARITY_CONTROLr,
      L2MC_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_ENTRY_PAR_ERRf, "L3_ENTRY table parity error",
      L3_ENTRY_ONLYm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      L3_ENTRY_PARITY_CONTROLr,
      L3_ENTRY_PARITY_STATUS_0r, L3_ENTRY_PARITY_STATUS_1r,
      INVALIDr, INVALIDr, 
      INVALIDf, INVALIDr, INVALIDr, INVALIDr, INVALIDr,
      _GH_SER_F_L3},
    { PARITY_ENf, L3_DEFIP_DATA_PAR_ERRf, "L3_DEFIP_DATA_ONLY table parity error",
      L3_DEFIP_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_DEFIP_PARITY_CONTROLr,
      L3_DEFIP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDf, INVALIDr, INVALIDr, INVALIDr, INVALIDr,
      _GH_SER_F_L3},
    { PARITY_ENf, L3MC_PAR_ERRf, "L3MC table parity error",
      L3_IPMCm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3MC_PARITY_CONTROLr,
      L3MC_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDf, INVALIDr, INVALIDr, INVALIDr, INVALIDr,
      _GH_SER_F_L3},
    { PARITY_ENf, MA_STATE_PAR_ERRf, "MA_STATE table parity error",
      MA_STATEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MA_STATE_PARITY_CONTROLr,
      MA_STATE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, RMEP_PAR_ERRf, "RMEP table parity error",
      RMEPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      RMEP_PARITY_CONTROLr,
      RMEP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MAID_REDUCTION_PAR_ERRf, "MAID_REDUCTION table parity error",
      MAID_REDUCTIONm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MAID_REDUCTION_PARITY_CONTROLr,
      MAID_REDUCTION_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MA_INDEX_PAR_ERRf, "MA_INDEX table parity error",
      MA_INDEXm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MA_INDEX_PARITY_CONTROLr,
      MA_INDEX_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { PORT_CBL_TABLE_PAR_ERRf, }, */
    { PARITY_ENf, INITIAL_NHOP_PAR_ERRf, "INITIAL_NHOP table parity error",
      INITIAL_ING_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      INITIAL_NHOP_PARITY_CONTROLr,
      INITIAL_NHOP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDf, INVALIDr, INVALIDr, INVALIDr, INVALIDr,
       _GH_SER_F_L3},
    /* Start of parity-unrelated OAM faults */

    { ANY_RMEP_TLV_PORT_DOWN_INTRf, ANY_RMEP_TLV_PORT_DOWN_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ANY_RMEP_TLV_PORT_UP_INTRf, ANY_RMEP_TLV_PORT_UP_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ANY_RMEP_TLV_INTERFACE_DOWN_INTRf, ANY_RMEP_TLV_INTERFACE_DOWN_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ANY_RMEP_TLV_INTERFACE_UP_INTRf, ANY_RMEP_TLV_INTERFACE_UP_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { XCON_CCM_DEFECT_INTRf, XCON_CCM_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ERROR_CCM_DEFECT_INTRf, ERROR_CCM_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { SOME_RDI_DEFECT_INTRf, SOME_RDI_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { SOME_RMEP_CCM_DEFECT_INTRf, SOME_RMEP_CCM_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    /* End of parity-unrelated OAM faults */
    /* { VFI_1_PAR_INTRf, }, */
    { PARITY_ENf, L2_USER_ENTRY_DATA_ONLY_PAR_ERRf, 
      "L2_USER_ENTRY_DATA_ONLY table parity error",
      L2_USER_ENTRY_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L2_USER_ENTRY_DATA_ONLY_PARITY_CONTROLr,
      L2_USER_ENTRY_DATA_ONLY_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_gh_parity_info_t _soc_rg2_ip2_parity_info[] = {
    { PARITY_ENf, L3_ECMP_GROUP_PAR_ERRf, "L3_ECMP_GROUP table parity error",
      L3_ECMP_COUNTm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_ECMP_GROUP_PARITY_CONTROLr,
      L3_ECMP_GROUP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_ECMP_PAR_ERRf, "L3_ECMP table parity error",
      L3_ECMPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_ECMP_PARITY_CONTROLr,
      L3_ECMP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};


STATIC _soc_gh_parity_info_t _soc_gh_ip2_parity_info[] = {
    { PARITY_ENf, ING_NHOP_PAR_ERRf, "ING_L3_NEXT_HOP table parity error",
      ING_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      ING_L3_NEXT_HOP_PARITY_CONTROLr,
      ING_L3_NEXT_HOP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDf, INVALIDr, INVALIDr, INVALIDr, INVALIDr,
      _GH_SER_F_L3},
    { PARITY_ENf, IFP_METER_PAR_ERRf, "FP_METER_TABLE table parity error",
      FP_METER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_METER_PARITY_CONTROLr,
      IFP_METER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_COUNTER_PAR_ERRf, "FP_COUNTER_TABLE table parity error",
      FP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_COUNTER_PARITY_CONTROLr,
      IFP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_POLICY_PAR_ERRf, "FP_POLICY_TABLE table parity error",
      FP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_POLICY_PARITY_CONTROLr,
      IFP_POLICY_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_STORM_CONTROL_PAR_ERRf, 
      "FP_STORM_CONTROL_METERS table parity error",
      FP_STORM_CONTROL_METERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_STORM_CONTROL_PARITY_CONTROLr,
      IFP_STORM_CONTROL_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_MTU_VALUES_PAR_ERRf, "L3_MTU_VALUES table parity error",
      L3_MTU_VALUESm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_MTU_VALUES_PARITY_CONTROLr,
      L3_MTU_VALUES_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDf, INVALIDr, INVALIDr, INVALIDr, INVALIDr,
      _GH_SER_F_L3},
    { PARITY_ENf, EGR_MASK_PAR_ERRf, "EGR_MASK table parity error",
      EGR_MASKm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_MASK_PARITY_CONTROLr,
      EGR_MASK_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MODPORT_MAP_SW_PAR_ERRf, "MODPORT_MAP_SW table parity error",
      MODPORT_MAP_SWm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_SW_PARITY_CONTROLr,
      MODPORT_MAP_SW_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },   
    { PARITY_ENf, MODPORT_MAP_IM_PAR_ERRf, "MODPORT_MAP_IM table parity error",
      MODPORT_MAP_IMm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_IM_PARITY_CONTROLr,
      MODPORT_MAP_IM_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MODPORT_MAP_EM_PAR_ERRf, "MODPORT_MAP_EM table parity error",
      MODPORT_MAP_EMm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_EM_PARITY_CONTROLr,
      MODPORT_MAP_EM_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, OAM_LM_COUNTERS_PAR_ERRf, " OAM_LM_COUNTERS table parity error",
      OAM_LM_COUNTERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      OAM_LM_COUNTERS_PARITY_CONTROLr,
      OAM_LM_COUNTERS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, TRUNK_EGR_MASK_PAR_ERRf, "TRUNK_EGR_MASK table parity error",
      TRUNK_EGR_MASKm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      TRUNK_EGR_MASK_PARITY_CONTROLr,
      TRUNK_EGR_MASK_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, TRUNK_GROUP_PAR_ERRf, "TRUNK_GROUP table parity error",
      TRUNK_GROUPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      TRUNK_GROUP_PARITY_CONTROLr,
      TRUNK_GROUP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, TRUNK_BITMAP_TABLE_PAR_ERRf, 
      "TRUNK_BITMAP table parity error",
      TRUNK_BITMAPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      TRUNK_BITMAP_TABLE_PARITY_CONTROLr,
      TRUNK_BITMAP_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MAC_BLOCK_TABLE_PAR_ERRf, "MAC_BLOCK table parity error",
      MAC_BLOCKm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MAC_BLOCK_TABLE_PARITY_CONTROLr,
      MAC_BLOCK_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, NONUCAST_TRUNK_BLOCK_MASK_PAR_ERRf, 
      "NONUCAST_TRUNK_BLOCK_MASK table parity error",
      NONUCAST_TRUNK_BLOCK_MASKm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      NONUCAST_TRUNK_BLOCK_MASK_PARITY_CONTROLr,
      NONUCAST_TRUNK_BLOCK_MASK_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, SRC_MODID_BLOCK_PAR_ERRf, 
      "SRC_MODID_BLOCK table parity error",
      SRC_MODID_BLOCKm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SRC_MODID_BLOCK_PARITY_CONTROLr,
      SRC_MODID_BLOCK_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PROFILE_2_PAR_ERRf, "VLAN_PROFILE_2 table parity error",
      VLAN_PROFILE_2m, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PROFILE_2_PARITY_CONTROLr,
      VLAN_PROFILE_2_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, ALTERNATE_EMIRROR_BITMAP_PAR_ERRf, 
      "ALTERNATE_EMIRROR_BITMAP table parity error",
      ALTERNATE_EMIRROR_BITMAPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      ALTERNATE_EMIRROR_BITMAP_PARITY_CONTROLr,
      ALTERNATE_EMIRROR_BITMAP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};


STATIC _soc_gh_parity_info_t _soc_rg2_ep_parity_info[] = {
    { PARITY_ENf, EFP_POLICY_TABLE_PAR_ERRf, "Parity enable for EFP_POLICY_TABLE memory",
      EFP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EFP_POLICY_TABLE_PARITY_CONTROLr,
      EFP_POLICY_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EFP_METER_TABLE_PAR_ERRf, "Parity enable for EFP_METER_TABLE memory",
      EFP_METER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EFP_METER_TABLE_PARITY_CONTROLr,
      EFP_METER_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { EFPCTR_PAR_ENf, EGR_FP_COUNTERf, "Parity enable for efp counter memory",
      EFP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_FP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_gh_parity_info_t _soc_gh_ep_parity_info[] = {
    { PARITY_ENf, EGR_NHOP_PAR_ERRf, NULL,
      EGR_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_L3_NEXT_HOP_PARITY_CONTROLr,
      EGR_L3_NEXT_HOP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDf, INVALIDr, INVALIDr, INVALIDr, INVALIDr,
      _GH_SER_F_L3},
    { PARITY_ENf, EGR_L3_INTF_PAR_ERRf, NULL,
      EGR_L3_INTFm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_L3_INTF_PARITY_CONTROLr,
      EGR_L3_INTF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDf, INVALIDr, INVALIDr, INVALIDr, INVALIDr,
      _GH_SER_F_L3},
    { PARITY_ENf, EGR_VLAN_PAR_ERRf, NULL,
      EGR_VLANm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_VLAN_PARITY_CONTROLr,
      EGR_VLAN_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_VLAN_STG_PAR_ERRf, NULL,
      EGR_VLAN_STGm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_VLAN_STG_PARITY_CONTROLr,
      EGR_VLAN_STG_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_VXLT_PAR_ERRf, NULL,
      EGR_VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      EGR_VLAN_XLATE_PARITY_CONTROLr,
      EGR_VLAN_XLATE_PARITY_STATUS_0r, EGR_VLAN_XLATE_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
    { STATS_PAR_ENf, EGR_STATS_COUNTER_TABLE_PAR_ERRf, "Parity enable for egress stats counter memory",
      INVALIDm, _SOC_PARITY_INFO_TYPE_COUNTER,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_STATS_COUNTER_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, EP_INITBUF_DBEf, "EP_INITBUF double-bit ECC error",
      INVALIDm,_SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      EGR_INITBUF_ECC_CONTROLr,
      EGR_INITBUF_ECC_STATUS_DBEr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, EP_INITBUF_SBEf, "EP_INITBUF single-bit ECC error (corrected)",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      EGR_INITBUF_ECC_CONTROLr,
      EGR_INITBUF_ECC_STATUS_SBEr, INVALIDr,
      INVALIDr, INVALIDr },
    { PERQ_PAR_ENf, EGR_PERQ_COUNTERf, "Parity enable for egress perq xmt counter memory",
      EGR_PERQ_XMT_COUNTERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_PERQ_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { EFPCTR_PAR_ENf, EGR_FP_COUNTERf, "Parity enable for efp counter memory",
      EFP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_FP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
    { PARITY_ENf, EGR_MOD_MAP_TABLE_PAR_ERRf, "Parity enable for EGR_MOD_MAP_TABLE memory",
      EGR_MOD_MAP_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_MOD_MAP_PARITY_CONTROLr,
      EGR_MOD_MAP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { PARITY_ENf, EFP_POLICY_TABLE_PAR_ERRf, "Parity enable for EFP_POLICY_TABLE memory",
      EFP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EFP_POLICY_TABLE_PARITY_CONTROLr,
      EFP_POLICY_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EFP_METER_TABLE_PAR_ERRf, "Parity enable for EFP_METER_TABLE memory",
      EFP_METER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EFP_METER_TABLE_PARITY_CONTROLr,
      EFP_METER_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
    { PARITY_ENf, EGR_ETAG_PCP_DE_MAPPING_PAR_ERRf, "Parity enable for EGR_ETAG_PCP_MAPPING memory",
      EGR_ETAG_PCP_MAPPINGm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_ETAG_PCP_DE_MAPPING_PARITY_CONTROLr,
      EGR_ETAG_PCP_DE_MAPPING_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_GPP_ATTRIBUTES_PAR_ERRf, "Parity enable for EGR_GPP_ATTRIBUTES memory",
      EGR_GPP_ATTRIBUTESm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_GPP_ATTRIBUTES_PARITY_CONTROLr,
      EGR_GPP_ATTRIBUTES_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_PER_Q_ECN_MARKED_PAR_ERRf, "Parity enable for EGR_PER_Q_ECN_MARKED memory",
      EGR_PER_Q_ECN_MARKEDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_PER_Q_ECN_MARKED_PARITY_CONTROLr,
      EGR_PER_Q_ECN_MARKED_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { RESI_ECC_ENf, EGR_RESI_DBUFf, "Parity enable for P_RESI_DATABUF memory",
      INVALIDm,_SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_RESI_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { CM_ECC_ENf, EGR_CM_DBUFf, "ECC enable for cm interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_CM_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { GP_ECC_ENf, EGR_GP_DBUFf, "ECC enable for gp interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_GP_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP0_ECC_ENf, EGR_XP0_DBUFf, "ECC enable for xp0 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP0_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP1_ECC_ENf, EGR_XP1_DBUFf, "ECC enable for xp1 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP1_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP2_ECC_ENf, EGR_XP2_DBUFf, "ECC enable for xp2 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP2_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP3_ECC_ENf, EGR_XP3_DBUFf, "ECC enable for xp3 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP3_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP4_ECC_ENf, EGR_XP4_DBUFf, "ECC enable for xp4 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP4_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP5_ECC_ENf, EGR_XP5_DBUFf, "ECC enable for xp5 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP5_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP6_ECC_ENf, EGR_XP6_DBUFf, "ECC enable for xp6 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP6_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP7_ECC_ENf, EGR_XP7_DBUFf, "ECC enable for xp7 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP7_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP8_ECC_ENf, EGR_XP8_DBUFf, "ECC enable for xp8 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP8_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP9_ECC_ENf, EGR_XP9_DBUFf, "ECC enable for xp9 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP9_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP10_ECC_ENf, EGR_XP10_DBUFf, "ECC enable for xp10 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP10_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP11_ECC_ENf, EGR_XP11_DBUFf, "ECC enable for xp11 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP11_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP12_ECC_ENf, EGR_XP12_DBUFf, "ECC enable for xp12 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP12_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP13_ECC_ENf, EGR_XP13_DBUFf, "ECC enable for xp13 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP13_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP14_ECC_ENf, EGR_XP14_DBUFf, "ECC enable for xp14 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP14_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP15_ECC_ENf, EGR_XP15_DBUFf, "ECC enable for xp15 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP15_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP16_ECC_ENf, EGR_XP16_DBUFf, "ECC enable for xp16 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP16_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP17_ECC_ENf, EGR_XP15_DBUFf, "ECC enable for xp17 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP17_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP18_ECC_ENf, EGR_XP15_DBUFf, "ECC enable for xp18 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP18_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP19_ECC_ENf, EGR_XP15_DBUFf, "ECC enable for xp19 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP19_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};


STATIC _soc_gh_parity_info_t _soc_gh_xl_parity_info[] = {
    { PM_INTRf, PM_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_PORT_MACRO,
      INVALIDr,
      XLPORT_INTR_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO3_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_TXFIFO0_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO2_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_TXFIFO1_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO1_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_TXFIFO2_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO0_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_TXFIFO3_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { RXFIFO_ECC_ENABLEf, RXFIFO_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_RXFIFO_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
#if GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED
    { TXFIFO_ECC_ENf, TXFIFO3_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_TXFIFO0_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO2_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_TXFIFO1_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO1_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_TXFIFO2_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO0_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_TXFIFO3_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { RXFIFO_EN_COR_ERR_RPTf, RXFIFO_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_XL_ECC_CONTROLr,
      PGW_XL_RXFIFO_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
#endif /* GREYHOUND_SER_ECC_SINGLE_ERR_RPT_ENABLED */
    /* { BOD_XLP0_ERRf,}, */
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_gh_parity_info_t _soc_gh_gx_parity_info[] = {
    { PM_INTRf, PM_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_PORT_MACRO,
      INVALIDr,
      XLPORT_INTR_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO3_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_TXFIFO0_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO2_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_TXFIFO1_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO1_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_TXFIFO2_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO0_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_TXFIFO3_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { RXFIFO_ECC_ENABLEf, RXFIFO1_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_RXFIFO1_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { RXFIFO_ECC_ENABLEf, RXFIFO0_DBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_RXFIFO0_ECC_DBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
#if GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED
    { TXFIFO_ECC_ENf, TXFIFO3_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_TXFIFO0_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO2_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_TXFIFO1_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO1_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_TXFIFO2_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO0_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_TXFIFO3_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { RXFIFO_EN_COR_ERR_RPTf, RXFIFO_SBEf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      PGW_GX_ECC_CONTROLr,
      PGW_GX_RXFIFO0_ECC_SBE_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
#endif /* GREYHOUND_SER_ECC_SINGLE_ERR_RPT_ENABLED */
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_gh_parity_info_t _soc_gh_mmu_parity_info[] = {
    { CELLECCERRORINTMASKf, CELLECCERRORf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMU_ECC,
      INVALIDr,
      CBPCELLERRPTRr, INVALIDr,
      INVALIDr, INVALIDr,  
      CELLECCERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { CBPPKTHDRPARITYERRORINTMASKf, CBPPKTHDRPARITYERRORf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMU_CBPPKTHDR,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      CBPPKTHDRPARITYERROR_CLRf, 
      CBPPKTHDR_ERR_CLRr, INVALIDr, INVALIDr, INVALIDr }, 
    { CBPCELLHDRPARITYERRORINTMASKf, CBPCELLHDRPARITYERRORf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMU_ECC,
      INVALIDr,
      CBPCELLHDRPARITYERRPTRr, INVALIDr,
      INVALIDr, INVALIDr, 
      CBPCELLHDRPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr },
    { XQPARITYERRORINTMASKf, XQPARITYERRORf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMU_XQ,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      XQPARITYERROR_CLRf, 
      XQPARITYERRORPTRr, XQ_ERR_VLDr, XQ_MULTI_ERRr, XQ_ERR_CLRr },
    { CFAPPARITYERRORINTMASKf, CFAPPARITYERRORf, "CBP Free Address Pool RAM",
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMU_ECC,
      INVALIDr,
      CFAPPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr, 
      CFAPPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr },
    { CCPPARITYERRORINTMASKf, CCPPARITYERRORf, "Copy Count Pool RAM",
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMU_ECC,
      INVALIDr,
      CCPPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr, 
      CCPPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr },      
    { WREDPARITYERRORINTMASKf, WREDPARITYERRORf, NULL,
      MMU_WRED_MARK_THDm, _SOC_PARITY_INFO_TYPE_MMU_WRED,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      WREDPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { WREDPARITYERRORINTMASKf, WREDPARITYERRORf, NULL,
      MMU_WRED_DROP_PROFILE_GREENm, _SOC_PARITY_INFO_TYPE_MMU_WRED,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      WREDPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { WREDPARITYERRORINTMASKf, WREDPARITYERRORf, NULL,
      MMU_WRED_DROP_PROFILE_YELLOWm, _SOC_PARITY_INFO_TYPE_MMU_WRED,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      WREDPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { WREDPARITYERRORINTMASKf, WREDPARITYERRORf, NULL,
      MMU_WRED_DROP_PROFILE_REDm, _SOC_PARITY_INFO_TYPE_MMU_WRED,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      WREDPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { WREDPARITYERRORINTMASKf, WREDPARITYERRORf, NULL,
      MMU_WRED_MARK_PROFILE_GREENm, _SOC_PARITY_INFO_TYPE_MMU_WRED,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      WREDPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { WREDPARITYERRORINTMASKf, WREDPARITYERRORf, NULL,
      MMU_WRED_MARK_PROFILE_YELLOWm, _SOC_PARITY_INFO_TYPE_MMU_WRED,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      WREDPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { WREDPARITYERRORINTMASKf, WREDPARITYERRORf, NULL,
      MMU_WRED_MARK_PROFILE_REDm, _SOC_PARITY_INFO_TYPE_MMU_WRED,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      WREDPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { WREDPARITYERRORINTMASKf, WREDPARITYERRORf, NULL,
      MMU_WRED_AVG_QSIZEm, _SOC_PARITY_INFO_TYPE_MMU_WRED,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      WREDPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { WREDPARITYERRORINTMASKf, WREDPARITYERRORf, NULL,
      MMU_WRED_CONFIGm, _SOC_PARITY_INFO_TYPE_MMU_WRED,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      WREDPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { XQFLLPARITYERRORINTMASKf, XQFLLPARITYERRORf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMU_XQ,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      XQFLLPARITYERROR_CLRf, 
      XQFLLPARITYERRORPTRr, XQFLL_ERR_VLDr, XQFLL_MULTI_ERRr, XQFLL_ERR_CLRr },
    { E2EFCPARITYERRORINTMASKf, E2EFCPARITYERRORf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMU_E2EFC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, 
      E2EFCPARITYERROR_CLRf, 
      INVALIDr, INVALIDr, INVALIDr, INVALIDr },
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL2m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL3m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},  
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL4m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL5m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL6m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL7m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL8m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL9m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL10m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL11m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL12m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL13m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL14m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL15m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL16m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL17m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL18m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL19m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL20m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL21m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL22m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL23m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL24m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL25m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL26m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL27m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL28m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCGROUPPARITYERRORINTMASKf, IPMCGROUPPARITYERRORf, NULL,
      MMU_IPMC_GROUP_TBL29m, _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCGROUPPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { IPMCVLANXPORTPARITYERRORINTMASKf, IPMCVLANXPORTPARITYERRORf, 
      "MMU_IPMV_VLAN_TBL memory",
      MMU_IPMC_VLAN_TBLm, _SOC_PARITY_INFO_TYPE_MMU_IPMCVLAN,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr, IPMCVLANXPORTPARITYERROR_CLRf,
      INVALIDr, INVALIDr, INVALIDr, INVALIDr},
    { INVALIDf, INVALIDf }, /* table terminator */
};


#define CMIC_PARITY_MMU_TO_CMIC_MEMFAIL_INTR_BIT    0x0001
#define CMIC_PARITY_EP_TO_CMIC_PERR_INTR_BIT        0x0002
#define CMIC_PARITY_IP0_TO_CMIC_PERR_INTR_BIT       0x0004
#define CMIC_PARITY_IP1_TO_CMIC_PERR_INTR_BIT       0x0008
#define CMIC_PARITY_IP2_TO_CMIC_PERR_INTR_BIT       0x0010
#define CMIC_PARITY_XLP0_TO_CMIC_PERR_INTR_BIT      0x0040


typedef struct {
    uint32             cpi_bit;
    soc_block_t        blocktype;
    soc_reg_t          enable_reg;
    soc_reg_t          status_reg;
    soc_reg_t          clear_reg;
    _soc_gh_parity_info_t *info;
} _soc_gh_parity_group_info_t; 
static _soc_gh_parity_group_info_t _soc_rg2_parity_group_info_template[] = {
    { CMIC_PARITY_MMU_TO_CMIC_MEMFAIL_INTR_BIT, SOC_BLK_MMU, MEMFAILINTMASKr, 
        MEMFAILINTSTATUSr, MEMFAILINT_CLRr, _soc_gh_mmu_parity_info},
    { CMIC_PARITY_EP_TO_CMIC_PERR_INTR_BIT, SOC_BLK_EPIPE, EGR_INTR_ENABLE_64r, 
        EGR_INTR_STATUS_64r, INVALIDr, _soc_gh_ep_parity_info },
    { CMIC_PARITY_EP_TO_CMIC_PERR_INTR_BIT, SOC_BLK_EPIPE, EGR_INTR_ENABLE_64r, 
        EGR_INTR_STATUS_64r, INVALIDr, _soc_rg2_ep_parity_info },
    { CMIC_PARITY_IP0_TO_CMIC_PERR_INTR_BIT, SOC_BLK_IPIPE, IP0_INTR_ENABLEr, 
        IP0_INTR_STATUSr, INVALIDr, _soc_gh_ip0_parity_info },
    { CMIC_PARITY_IP0_TO_CMIC_PERR_INTR_BIT, SOC_BLK_IPIPE, IP0_INTR_ENABLEr, 
        IP0_INTR_STATUSr, INVALIDr, _soc_rg2_ip0_parity_info },
    { CMIC_PARITY_IP1_TO_CMIC_PERR_INTR_BIT, SOC_BLK_IPIPE, IP1_INTR_ENABLEr, 
        IP1_INTR_STATUSr, INVALIDr, _soc_gh_ip1_parity_info },
    { CMIC_PARITY_IP1_TO_CMIC_PERR_INTR_BIT, SOC_BLK_IPIPE, IP1_INTR_ENABLEr, 
        IP1_INTR_STATUSr, INVALIDr, _soc_rg2_ip1_parity_info },
    { CMIC_PARITY_IP2_TO_CMIC_PERR_INTR_BIT, SOC_BLK_IPIPE, IP2_INTR_ENABLEr, 
        IP2_INTR_STATUSr, INVALIDr, _soc_gh_ip2_parity_info },
    { CMIC_PARITY_IP2_TO_CMIC_PERR_INTR_BIT, SOC_BLK_IPIPE, IP2_INTR_ENABLEr, 
        IP2_INTR_STATUSr, INVALIDr, _soc_rg2_ip2_parity_info },
 /* { 0x00020, PCIE_REPLAY_PERR, }, */
    { CMIC_PARITY_XLP0_TO_CMIC_PERR_INTR_BIT, SOC_BLK_XLPORT, 
        PGW_XL_INTR_ENABLEr, PGW_XL_INTR_STATUSr, 
        INVALIDr, _soc_gh_xl_parity_info },        
    { 0 }, /* table terminator */
};

static _soc_gh_parity_group_info_t _soc_gh_parity_group_info_template[] = {
    { CMIC_PARITY_MMU_TO_CMIC_MEMFAIL_INTR_BIT, SOC_BLK_MMU, MEMFAILINTMASKr, 
        MEMFAILINTSTATUSr, MEMFAILINT_CLRr, _soc_gh_mmu_parity_info},
    { CMIC_PARITY_EP_TO_CMIC_PERR_INTR_BIT, SOC_BLK_EPIPE, EGR_INTR_ENABLE_64r, 
        EGR_INTR_STATUS_64r, INVALIDr, _soc_gh_ep_parity_info },
    { CMIC_PARITY_IP0_TO_CMIC_PERR_INTR_BIT, SOC_BLK_IPIPE, IP0_INTR_ENABLEr, 
        IP0_INTR_STATUSr, INVALIDr, _soc_gh_ip0_parity_info },
    { CMIC_PARITY_IP1_TO_CMIC_PERR_INTR_BIT, SOC_BLK_IPIPE, IP1_INTR_ENABLEr, 
        IP1_INTR_STATUSr, INVALIDr, _soc_gh_ip1_parity_info },
    { CMIC_PARITY_IP2_TO_CMIC_PERR_INTR_BIT, SOC_BLK_IPIPE, IP2_INTR_ENABLEr, 
        IP2_INTR_STATUSr, INVALIDr, _soc_gh_ip2_parity_info },
 /* { 0x00020, PCIE_REPLAY_PERR, }, */
    { CMIC_PARITY_XLP0_TO_CMIC_PERR_INTR_BIT, SOC_BLK_XLPORT, 
        PGW_XL_INTR_ENABLEr, PGW_XL_INTR_STATUSr, 
        INVALIDr, _soc_gh_xl_parity_info },
    { 0 }, /* table terminator */
};


STATIC _soc_generic_ser_info_t *_soc_gh_tcam_ser_info[SOC_MAX_NUM_DEVICES] = {NULL};
STATIC _soc_gh_parity_group_info_t *_soc_gh_parity_group_info[SOC_MAX_NUM_DEVICES] = {NULL};

STATIC soc_ser_functions_t _gh_ser_functions[SOC_MAX_NUM_DEVICES];


STATIC soc_greyhound_oam_handler_t gh_oam_handler[SOC_MAX_NUM_DEVICES] = {NULL};
STATIC soc_greyhound_oam_ser_handler_t gh_oam_ser_handler[SOC_MAX_NUM_DEVICES] = {NULL};


#define _SOC_GREYHOUND_SER_REG 1
#define _SOC_GREYHOUND_SER_MEM 0

typedef union _greyhound_ser_nack_reg_mem_u {
    soc_reg_t reg;
    soc_mem_t mem;
} _greyhound_ser_nack_reg_mem_t;

STATIC int
_soc_greyhound_ser_mem_skip(int unit, int skip_feature)
{
    int     skip = 0;

    if (!soc_feature(unit, soc_feature_l3)){
        if (skip_feature & _GH_SER_F_L3) {
            skip = 1;
        }
    }

    return skip;
}

STATIC int
_soc_greyhound_parity_block_port(int unit, soc_block_t block, soc_port_t *port)
{
    *port = SOC_BLOCK_PORT(unit, block);
    if ((*port != REG_PORT_ANY) && !SOC_PORT_VALID(unit, *port)) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

soc_field_t _soc_gh_oam_interrupt_fields[] = {
    SOME_RDI_DEFECT_INTRf,
    SOME_RMEP_CCM_DEFECT_INTRf,
    ERROR_CCM_DEFECT_INTRf,
    ANY_RMEP_TLV_PORT_DOWN_INTRf,
    ANY_RMEP_TLV_PORT_UP_INTRf,
    ANY_RMEP_TLV_INTERFACE_DOWN_INTRf,
    ANY_RMEP_TLV_INTERFACE_UP_INTRf,
    XCON_CCM_DEFECT_INTRf,
    INVALIDf
};

STATIC int
_soc_gh_process_oam_interrupt(int unit)
{
    uint32 rval;
    int found = 0, fidx = 0;
    soc_greyhound_oam_handler_t oam_handler_snapshot = gh_oam_handler[unit];


    SOC_IF_ERROR_RETURN(READ_IP1_INTR_STATUSr(unit, &rval));
    
    while (_soc_gh_oam_interrupt_fields[fidx] != INVALIDf) {
        if (soc_reg_field_get(unit, IP1_INTR_STATUSr, rval,
                          _soc_gh_oam_interrupt_fields[fidx])) {
        
            if (oam_handler_snapshot != NULL)
            {
                (void)(oam_handler_snapshot(unit,
                           _soc_gh_oam_interrupt_fields[fidx]));
            }
            found++;
        }
        fidx++;
    }
    if (!found) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Unexpected interrupt received for OAM !!\n")));
    }
    return SOC_E_NONE;
}

void
soc_greyhound_oam_ser_handler_register(int unit, 
                                soc_greyhound_oam_ser_handler_t handler) 
{
    gh_oam_ser_handler[unit] = handler;
}

int
soc_greyhound_oam_ser_process(int unit, soc_mem_t mem, int index) 
{
    void *null_entry = NULL;
    int rv = SOC_E_NONE;
    
    if (gh_oam_ser_handler[unit]) {
        return gh_oam_ser_handler[unit](unit, mem, index);
    } else {
        /* Clear the entry of MA_STATE and REMP memories */
        null_entry = soc_mem_entry_null(unit, mem);
        if ((rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, null_entry)) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "OAM SER ENTRY_CLEAR failed for %s[%d]: %s\n"),
                       SOC_MEM_NAME(unit, mem),
                       index, soc_errmsg(rv)));
            return rv;
        }

        if (soc_property_get(unit, spn_ENHANCED_SER_CORRECTION_EVENT_REPORT, 0)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY_CORRECTED, mem, index);
        } else {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_CORRECTED, mem, index);
        }

        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "OAM SER ENTRY_CLEAR: %s[%d] index %d\n"),
                   SOC_MEM_NAME(unit, mem), mem, index));
        return rv;
    }
}



#if GREYHOUND_SER_PARITY_ENABLED            
STATIC int
_soc_greyhound_parity_enable(int unit, int enable)
{
    int group, table, reg_is_64, blk_num = 0;
    uint32 group_enable, regval, cmic_enable = 0, cpi_blk_bit;
    _soc_gh_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_field_t enable_field;
    soc_block_t blk;
    uint64 group_enable64;
    _soc_gh_parity_group_info_t *parity_group_info;

    parity_group_info = _soc_gh_parity_group_info[unit];
    if (parity_group_info == NULL) {
        return SOC_E_UNAVAIL;
    }
    
    /* loop through each group */
    for (group = 0; parity_group_info[group].cpi_bit; group++) {
        info = parity_group_info[group].info;
        group_reg = parity_group_info[group].enable_reg;
        group_enable = 0;
        COMPILER_64_ZERO(group_enable64);
        reg_is_64 = SOC_REG_IS_64(unit, group_reg) ? 1 : 0;
        cpi_blk_bit = parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       parity_group_info[group].blocktype) {
            if (_soc_greyhound_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }
            if (!SOC_REG_IS_VALID(unit, group_reg)) {
                break;
            }
            
            if (reg_is_64) {
                SOC_IF_ERROR_RETURN(soc_reg64_get(unit, group_reg, block_port, 
                    0, &group_enable64));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, group_reg, block_port, 
                    0, &group_enable));
            }
        
            if (parity_group_info[group].blocktype == SOC_BLK_XLPORT) {
                blk_num = SOC_BLOCK_INFO(unit, blk).number;
                if (blk_num == 5) {
                    info = _soc_gh_gx_parity_info;
                    group_reg = PGW_GX_INTR_ENABLEr;
                    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, group_reg, 
                        block_port, 0, &group_enable));
                }
            }           
            
            /* loop through each table in the group */
            for (table = 0; info[table].enable_field != INVALIDf; table++) {
                if (_soc_greyhound_ser_mem_skip(unit, info[table].feature)) {
                    continue;
                }
                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                case _SOC_PARITY_INFO_TYPE_MMU_ECC:
                case _SOC_PARITY_INFO_TYPE_MMU_XQ:
                case _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP:
                case _SOC_PARITY_INFO_TYPE_MMU_IPMCVLAN:
                case _SOC_PARITY_INFO_TYPE_MMU_E2EFC:
                case _SOC_PARITY_INFO_TYPE_MMU_WRED:
                case _SOC_PARITY_INFO_TYPE_MMU_CBPPKTHDR:
                case _SOC_PARITY_INFO_TYPE_OAM:
                case _SOC_PARITY_INFO_TYPE_PORT_MACRO:
                    enable_field = info[table].enable_field;
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                case _SOC_PARITY_INFO_TYPE_COUNTER:
                    reg = info[table].control_reg;
                    if (!SOC_REG_IS_VALID(unit, reg)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, 
                        block_port, 0, &regval));
                    soc_reg_field_set(unit, reg, &regval,
                                      info[table].enable_field,
                                      enable ? 1 : 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, 
                        block_port, 0, regval));
                    enable_field = info[table].error_field;
                    break;
                default:
                    enable_field = INVALIDf;
                    break;
                } /* handle different parity error reporting style */
                if (enable_field != INVALIDf) {
                    if (reg_is_64) {
                        soc_reg64_field32_set(unit, group_reg, &group_enable64,
                                      enable_field, enable ? 1 : 0);
                    } else {
                        soc_reg_field_set(unit, group_reg, &group_enable,
                                      enable_field, enable ? 1 : 0);
                    }
                }
            } /* loop through each table in the group */

            if (parity_group_info[group].blocktype == 
                SOC_BLK_XLPORT) {
                /* Enable XLPORT interrupts */
                reg = XLPORT_INTR_ENABLEr;
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, 
                                reg, block_port, 0, &regval));
                soc_reg_field_set(unit, reg, &regval,
                              TSC_ERRf,
                              enable ? 1 : 0);
                soc_reg_field_set(unit, reg, &regval,
                              MAC_RX_CDC_MEM_ERRf,
                              enable ? 1 : 0);
                soc_reg_field_set(unit, reg, &regval,
                              MAC_TX_CDC_MEM_ERRf,
                              enable ? 1 : 0);
                soc_reg_field_set(unit, reg, &regval,
                              MIB_RX_MEM_ERRf,
                              enable ? 1 : 0);
                soc_reg_field_set(unit, reg, &regval,
                              MIB_TX_MEM_ERRf,
                              enable ? 1 : 0);
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, 
                                reg, block_port, 0, regval));
                
            }

            if (reg_is_64) {
                SOC_IF_ERROR_RETURN(soc_reg64_set(unit, group_reg, block_port, 
                    0, group_enable64));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, group_reg, block_port, 
                    0, group_enable));
            }
            if (parity_group_info[group].blocktype == 
                SOC_BLK_XLPORT) {
                cmic_enable |= (cpi_blk_bit << blk_num);
            } else {
                cmic_enable |= cpi_blk_bit;
            }
            
        }
    } /* loop through each group */
    
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    if (enable) {
        /* MMU enables */
        soc_reg_field_set(unit, MISCCONFIGr, &regval, PARITY_CHECK_ENf, 1);
#if GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED        
        soc_reg_field_set(unit, MISCCONFIGr, &regval, 
                                ECC_ONE_BIT_ERROR_INT_ENf, 1);
#endif /* GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED */
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regval));
        /* Write CMIC enable register */
        (void)soc_cmicm_intr3_enable(unit, cmic_enable);
    } else {
        /* MMU disables */
        soc_reg_field_set(unit, MISCCONFIGr, &regval, PARITY_CHECK_ENf, 0);
#if GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED        
        soc_reg_field_set(unit, MISCCONFIGr, &regval, 
                                ECC_ONE_BIT_ERROR_INT_ENf, 0);
#endif /* GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED */
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regval));
        /* Write CMIC disable register */
        (void)soc_cmicm_intr3_disable(unit, cmic_enable);
    }
    return SOC_E_NONE;
}
#endif /* GREYHOUND_SER_PARITY_ENABLED */


/* SER processing for TCAMs */
/* For Ranger2 */
STATIC _soc_generic_ser_info_t _soc_rg2_tcam_ser_info_template[] = {
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2, 
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0 },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0 },
    { FP_GLOBAL_MASK_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 37}, {1, 37}, {38, 74}, {39, 74} }, 0, 0, 0 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0 },
    { FP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0 },
    { INVALIDm },
};

/* For Greyhound */
STATIC _soc_generic_ser_info_t _soc_gh_tcam_ser_info_template[] = {
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0 },
    { FP_GLOBAL_MASK_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 37}, {1, 37}, {38, 74}, {39, 74} }, 0, 0, 0 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0 },
    { FP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0 },
    { INVALIDm },
};

/* For Greyhound */
STATIC _soc_generic_ser_info_t _soc_gh_no_l3_tcam_ser_info_template[] = {
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0 },
    { FP_GLOBAL_MASK_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 37}, {1, 37}, {38, 74}, {39, 74} }, 0, 0, 0 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0 },
    { FP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0 },
    { INVALIDm },
};


STATIC int
_soc_greyhound_tcam_ser_init(int unit)
{
    if (soc_feature(unit, soc_feature_field_multi_stage)) {
        _soc_gh_tcam_ser_info[unit] = _soc_rg2_tcam_ser_info_template;
    } else if (soc_feature(unit, soc_feature_l3)){
        _soc_gh_tcam_ser_info[unit] = _soc_gh_tcam_ser_info_template;
    } else {
        _soc_gh_tcam_ser_info[unit] = _soc_gh_no_l3_tcam_ser_info_template;
    }
    return soc_generic_ser_init(unit, _soc_gh_tcam_ser_info[unit]);
}

void
soc_greyhound_ser_fail(int unit)

{
    soc_generic_ser_process_error(unit, _soc_gh_tcam_ser_info[unit], 0);
}

int
soc_greyhound_ser_mem_clear(int unit, soc_mem_t mem)
{
    uint32  range_enable;
    int info_ix, hw_enable_ix, i;
    _soc_generic_ser_info_t *cur_spi;
    ser_memory_entry_t ser_mem;
    _soc_generic_ser_info_t *tcam_ser_info;
    
    /* Check if enable */
    SOC_IF_ERROR_RETURN
        (READ_SER_RANGE_ENABLEr(unit, &range_enable));

    if (!range_enable) {
        return SOC_E_NONE;
    }

    info_ix = 0;
    tcam_ser_info = _soc_gh_tcam_ser_info[unit];

    while (tcam_ser_info[info_ix].mem != INVALIDm) {
        if (tcam_ser_info[info_ix].mem == mem) {
            break;
        }
        info_ix++;
    }
    hw_enable_ix = tcam_ser_info[info_ix].ser_hw_index ;

    if ((tcam_ser_info[info_ix].mem != INVALIDm) &&
        (range_enable & (1 << hw_enable_ix))) {
        cur_spi = &(tcam_ser_info[info_ix]);

        range_enable &= ~(1 << hw_enable_ix);
        /* Disable SER protection on this memory */
        SOC_IF_ERROR_RETURN
            (WRITE_SER_RANGE_ENABLEr(unit, range_enable));
         
        /* Flush SER memory segment for the table */
        sal_memset(&ser_mem, 0, sizeof(ser_mem));
        for (i = cur_spi->ser_section_start;
             i <= cur_spi->ser_section_end;
             i++) {
            SOC_IF_ERROR_RETURN
                    (WRITE_SER_MEMORYm(unit, MEM_BLOCK_ALL, i, &ser_mem));
        }

        range_enable |= (1 << hw_enable_ix);
        /* Enable SER protection on this memory */
        SOC_IF_ERROR_RETURN
            (WRITE_SER_RANGE_ENABLEr(unit, range_enable));

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "\t%s: SER[%d-%d]\n"),
                     SOC_MEM_NAME(unit, cur_spi->mem),
                     cur_spi->ser_section_start, cur_spi->ser_section_end));
    }

    return SOC_E_NONE;
    
}


STATIC void
_soc_gh_mem_parity_info(int unit, int block_info_idx, int pipe,
                         soc_field_t field_enum, uint32 *minfo)
{
    *minfo = (SOC_BLOCK2SCH(unit, block_info_idx) << SOC_ERROR_BLK_BP)
        | ((pipe & 0xff) << SOC_ERROR_PIPE_BP)
        | (field_enum & SOC_ERROR_FIELD_ENUM_MASK);
}


int
_soc_greyhound_mem_parity_control(int unit, soc_mem_t mem,
                                 int copyno, int enable)
{
    int group, table;
    uint32 misc_cfg;
    _soc_gh_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_field_t enable_field;
    soc_block_t blk;
    _soc_gh_parity_group_info_t *parity_group_info;

    if (!soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        /* Parity checking is not enabled, nothing to do */
        return SOC_E_NONE;
    }
    parity_group_info = _soc_gh_parity_group_info[unit];
    if (parity_group_info == NULL) {
        return SOC_E_UNAVAIL;
    }

    /* Convert component/aggregate memories to the table for which
     * the parity registers correspond. */
    switch(mem) {
    case VLAN_SUBNETm:
        mem = VLAN_SUBNET_DATA_ONLYm; /* Should be VLAN_SUBNET? */
        break;
    case L2_ENTRY_ONLYm:
        mem = L2Xm;
        break;
    case L2_USER_ENTRY_ONLYm:
    case L2_USER_ENTRY_DATA_ONLYm:
        mem = L2_USER_ENTRYm;
        break;
    case L3_DEFIPm:
        mem = L3_DEFIP_DATA_ONLYm;
        break;
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        mem = L3_ENTRY_ONLYm;
        break;
    case VLAN_MACm:
        mem = VLAN_XLATEm;
        break;
    default:
        /* Do nothing, keep memory as provided */
        break;
    }


    /* loop through each group */
    for (group = 0; parity_group_info[group].cpi_bit; group++) {
        info = parity_group_info[group].info;
        group_reg = parity_group_info[group].enable_reg;

        SOC_BLOCK_ITER(unit, blk,
                       parity_group_info[group].blocktype) {
            if (_soc_greyhound_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }
            if ((copyno != MEM_BLOCK_ANY) && (blk != copyno)) {
                continue;
            }

            /* loop through each table in the group */
            for (table = 0; info[table].enable_field != INVALIDf; table++) {
                if (mem != info[table].mem) {
                    continue;
                }
                if (_soc_greyhound_ser_mem_skip(unit, info[table].feature)) {
                    continue;
                }

                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                case _SOC_PARITY_INFO_TYPE_MMU_ECC:
                case _SOC_PARITY_INFO_TYPE_MMU_XQ:
                case _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP:
                case _SOC_PARITY_INFO_TYPE_MMU_IPMCVLAN:
                case _SOC_PARITY_INFO_TYPE_MMU_WRED:
                case _SOC_PARITY_INFO_TYPE_MMU_E2EFC:
                case _SOC_PARITY_INFO_TYPE_MMU_CBPPKTHDR:
                    enable_field = info[table].enable_field;
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, group_reg, block_port,
                                                enable_field,
                                                enable ? 1 : 0));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                case _SOC_PARITY_INFO_TYPE_COUNTER:
                    reg = info[table].control_reg;
                    if (!SOC_REG_IS_VALID(unit, reg)) {
                        return SOC_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, reg, block_port,
                                                info[table].enable_field,
                                                enable ? 1 : 0));
                    break;
                default:
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */
        }
    } /* loop through each group */

    /* MMU controls */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
    if (enable) {
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHECK_ENf, 1);
#if GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED        
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, 
                                        ECC_ONE_BIT_ERROR_INT_ENf, 1);
#endif /* GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED */
    } else {
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHECK_ENf, 0);
#if GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED        
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, 
                                        ECC_ONE_BIT_ERROR_INT_ENf, 0);
#endif /* GREYHOUND_SER_ECC_1BIT_ERR_RPT_ENABLED */
    }
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));
    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_process_single_parity_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       int schan, char *msg, soc_block_t block)
{
    _soc_gh_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 reg_val;
    int index, multiple, error;
    uint32 minfo;
    _soc_ser_correct_info_t spci;

    info = _soc_gh_parity_group_info[unit][group].info;

    status_reg = schan ? info[table].nack_status0_reg :
        info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit,status_reg, 
                                    block_port, 0, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_IDXf);
    multiple = soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
    error = soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERRf);
 
    if (error) {
        _soc_gh_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                               index, minfo);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d parity error\n"),
                   unit, msg, index));
        if (multiple) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s has multiple parity errors\n"),
                       unit, msg));
        }

        if (info[table].mem != INVALIDm) {
            sal_memset(&spci, 0, sizeof(spci));
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;         
            spci.reg = INVALIDr;
            spci.mem = info[table].mem;
            spci.blk_type = -1;
            spci.index = index;
            (void)soc_ser_correction(unit, &spci);
        }
        
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "unit %d %s: parity error is handled.\n"),
                 unit, msg));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit,status_reg,  block_port, 0, 0));
    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_process_single_ecc_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       int schan, char *msg, soc_block_t block)
{
    _soc_gh_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 reg_val;
    int index, double_bit, multiple, error;
    uint32 minfo;
    _soc_ser_correct_info_t spci;

    info = _soc_gh_parity_group_info[unit][group].info;

    status_reg = schan ? info[table].nack_status0_reg :
        info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit,status_reg,  block_port, 0, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_IDXf);
    double_bit = soc_reg_field_get(unit, status_reg,
                                   reg_val, DOUBLE_BIT_ERRf);
    multiple = soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
    error = soc_reg_field_get(unit, status_reg, reg_val, ECC_ERRf);
 
    if (error) {
        _soc_gh_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                               index, minfo);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d %s ECC error\n"),
                   unit, msg, index, double_bit ? "double-bit" : ""));
        if (multiple) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s has multiple ECC errors\n"),
                       unit, msg));
        }
        sal_memset(&spci, 0, sizeof(spci));
        if (double_bit) {
            spci.double_bit = 1;
        }
        if (info[table].mem != INVALIDm) {
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = info[table].mem;
            spci.blk_type = -1;
            spci.index = index;
            spci.parity_type = SOC_PARITY_TYPE_ECC;
            (void)soc_ser_correction(unit, &spci);
        }
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "unit %d %s: parity error is handled.\n"),
                 unit, msg));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit,status_reg,  block_port, 0, 0));
    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_process_mmu_ecc(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{
    _soc_gh_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 reg_val;
    int index, double_bit;
    uint32 minfo;

    info = _soc_gh_parity_group_info[unit][group].info;

    status_reg = info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit,status_reg,  
        block_port, 0, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_INDEXf);
    double_bit = soc_reg_field_get(unit, status_reg,
                                   reg_val, DOUBLE_BIT_ERRf);
 
    _soc_gh_mem_parity_info(unit, block, 0,
                             info[table].error_field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                           SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                           index, minfo);
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d %s entry %d %s ECC error\n"),
               unit, msg, index, double_bit ? "double-bit" : ""));

    return SOC_E_NONE;
}


STATIC int
_soc_greyhound_process_dual_parity_error(int unit, int group,
                                        soc_port_t block_port, int table,
                                        int schan, char *msg, soc_block_t block)
{
    _soc_gh_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 reg_val, bitmap = 0;
    int index, bucket_index = 0, multiple = 0, ix, bits, size = 0;
    _soc_ser_correct_info_t spci;
    uint32 minfo;

    info = _soc_gh_parity_group_info[unit][group].info;

    for (ix = 0; ix < 2; ix ++) {
        if (ix == 1) {
            status_reg = schan ? info[table].nack_status1_reg :
                info[table].intr_status1_reg;
        } else {
            status_reg = schan ? info[table].nack_status0_reg :
                info[table].intr_status0_reg;
        }

        if (status_reg == INVALIDr) {
            continue;
        }

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit,status_reg,  
            block_port, 0, &reg_val));
        if (soc_reg_field_valid(unit, status_reg, BUCKET_IDXf)) {
            bucket_index =
                soc_reg_field_get(unit, status_reg, reg_val, BUCKET_IDXf);
            multiple =
                soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
            bitmap =
                soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BMf);
            size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BMf);
        } else if (soc_reg_field_valid(unit, status_reg, BUCKET_IDX_0f)) {
            bucket_index =
            soc_reg_field_get(unit, status_reg, reg_val, BUCKET_IDX_0f);
            multiple =
                soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERR_0f);
            bitmap =
                soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BM_0f);
            size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BM_0f);         
        } else if (soc_reg_field_valid(unit, status_reg, BUCKET_IDX_1f)) {
            bucket_index =
                soc_reg_field_get(unit, status_reg, reg_val, BUCKET_IDX_1f);
            multiple =
               soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERR_1f);
            bitmap =
                soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BM_1f);
            size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BM_1f);
        }


        if (multiple) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s has multiple parity errors\n"),
                       unit, msg));
        }

        if (bitmap != 0) {
            for (bits = 0; bits < size; bits++) {
                if (bitmap & (1 << bits)) {
                    index =
                        bucket_index * size * 2 + bits + (ix * size);
                    _soc_gh_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                               index, minfo);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s entry %d parity error\n"),
                               unit, msg, index));
                    if (info[table].mem != INVALIDm) {
                        sal_memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
                        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                        spci.reg = INVALIDr;
                        spci.mem = info[table].mem;
                        spci.blk_type = block;
                        spci.index = index;
                        (void)soc_ser_correction(unit, &spci);
                    }
                }
            }
        }

        /* Clear parity status */
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit,status_reg,  block_port, 0, 0));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_process_mmu_xq(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{
    _soc_gh_parity_info_t *info;
    soc_reg_t err_reg;
    uint32 reg_val, valid_val, entry_index, double_err, multiple;
    uint32  minfo;
    soc_port_t  port;

    info = _soc_gh_parity_group_info[unit][group].info;

    /* CHECK if the error is valid */
    err_reg = info[table].level2_error_vld_reg;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                        block_port, 0, &reg_val));
    valid_val =
        soc_reg_field_get(unit, err_reg, reg_val, ERR_VALIDf);
    if (valid_val == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s: parity hardware inconsistency\n"),
                   unit, msg));
        return SOC_E_NONE;
    }
    
    port = 0;
    while (!(valid_val & (0x1 << port))) {
        port++;
    }
    
    err_reg = info[table].level2_error_reg;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                port, 0, &reg_val));
    entry_index =
            soc_reg_field_get(unit, err_reg, reg_val, ENTRY_INDEXf);
    double_err =
            soc_reg_field_get(unit, err_reg, reg_val, DOUBLE_BIT_ERRf);
    err_reg = info[table].level2_error_multi_reg;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                port, 0, &reg_val));
    multiple =
            soc_reg_field_get(unit, err_reg, reg_val, MULTIPLE_ERRf);

    _soc_gh_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                               entry_index, minfo);

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d %s port %d entry %d %s ECC error\n"),
               unit, msg, port, entry_index, double_err ? "double-bit" : ""));
    if (multiple) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s has multiple ECC errors\n"),
                   unit, msg));
    }
    
    /* Clear interrupt status */
    err_reg = info[table].level2_error_clear_reg;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                block_port, 0, &reg_val));
    soc_reg_field_set(unit, err_reg, &reg_val,
                            CLR_ERRf, valid_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, err_reg, 
                                block_port, 0, reg_val));


    return SOC_E_NONE;
}

#define _SOC_GH_MMU_IPMC_GROUP_MAX     30
static soc_mem_t ipmc_mems[_SOC_GH_MMU_IPMC_GROUP_MAX] = {
        INVALIDm,  INVALIDm, 
        MMU_IPMC_GROUP_TBL2m,  MMU_IPMC_GROUP_TBL3m,
        MMU_IPMC_GROUP_TBL4m,  MMU_IPMC_GROUP_TBL5m, 
        MMU_IPMC_GROUP_TBL6m,  MMU_IPMC_GROUP_TBL7m,
        MMU_IPMC_GROUP_TBL8m,  MMU_IPMC_GROUP_TBL9m, 
        MMU_IPMC_GROUP_TBL10m,  MMU_IPMC_GROUP_TBL11m,
        MMU_IPMC_GROUP_TBL12m,  MMU_IPMC_GROUP_TBL13m, 
        MMU_IPMC_GROUP_TBL14m,  MMU_IPMC_GROUP_TBL15m,
        MMU_IPMC_GROUP_TBL16m,  MMU_IPMC_GROUP_TBL17m, 
        MMU_IPMC_GROUP_TBL18m,  MMU_IPMC_GROUP_TBL19m,
        MMU_IPMC_GROUP_TBL20m,  MMU_IPMC_GROUP_TBL21m, 
        MMU_IPMC_GROUP_TBL22m,  MMU_IPMC_GROUP_TBL23m,
        MMU_IPMC_GROUP_TBL24m,  MMU_IPMC_GROUP_TBL25m, 
        MMU_IPMC_GROUP_TBL26m,  MMU_IPMC_GROUP_TBL27m,
        MMU_IPMC_GROUP_TBL28m,  MMU_IPMC_GROUP_TBL29m
};

STATIC int
_soc_greyhound_process_mmu_ipmcgroup_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{
    _soc_gh_parity_info_t *info;
    soc_reg_t err_reg;
    uint32 reg_val, valid_val, entry_index, double_err, multiple, multi_regval;
    uint32  minfo;
    int i;
    _soc_ser_correct_info_t spci;

    info = _soc_gh_parity_group_info[unit][group].info;

    err_reg = IPMC_GROUP_ERR_VLDr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                        block_port, 0, &reg_val));
    valid_val =
            soc_reg_field_get(unit, err_reg, reg_val, ERR_VALIDf);

    /* CHECK if the error is valid */
    if (valid_val == 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "unit %d %s: parity error is handled.\n"),
                 unit, msg));
        return SOC_E_NONE;
    }

    /* Get the Multiple error register value */
    err_reg = IPMC_GROUP_MULTI_ERRr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                REG_PORT_ANY, 0, &multi_regval));

    for (i = 0; i < _SOC_GH_MMU_IPMC_GROUP_MAX; i++) {
        if (valid_val & (1 << i)) {
            err_reg = IPMC_GROUP_ERR_PTRr; 
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                i, 0, &reg_val));
            entry_index =
                soc_reg_field_get(unit, err_reg, reg_val, ENTRY_INDEXf);
            double_err =
                soc_reg_field_get(unit, err_reg, reg_val, DOUBLE_BIT_ERRf);
            
            multiple = multi_regval & (1 << i);
            _soc_gh_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                               entry_index, minfo);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d IPMC GROUP%d entry %d %s ECC error\n"),
                       unit, i, entry_index, double_err ? "double-bit" : ""));
            if (multiple) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d IPMC GROUP%d has multiple ECC errors\n"),
                           unit, i));
            }
            if (ipmc_mems[i] != INVALIDm) {
                sal_memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = ipmc_mems[i];
                spci.blk_type = SOC_BLK_MMU;
                spci.index = entry_index;
                (void)soc_ser_correction(unit, &spci);
            }
        }
    }
    
    
    /* Clear interrupt status */
    err_reg = IPMC_GROUP_ERR_CLRr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                block_port, 0, &reg_val));
    soc_reg_field_set(unit, err_reg, &reg_val,
                            CLR_ERRf, valid_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, err_reg, 
                                block_port, 0, reg_val));


    return SOC_E_NONE;
}


STATIC int
_soc_greyhound_process_mmu_ipmcvlan_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{

    _soc_gh_parity_info_t *info;
    soc_reg_t err_reg;
    uint32 reg_val, entry_index, double_err;
    uint32  minfo;
    _soc_ser_correct_info_t spci;

    info = _soc_gh_parity_group_info[unit][group].info;

    err_reg = IPMCPARITYERRORPTRr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                        block_port, 0, &reg_val));
    entry_index =
            soc_reg_field_get(unit, err_reg, reg_val, ENTRY_INDEXf);
    double_err =
            soc_reg_field_get(unit, err_reg, reg_val, DOUBLE_BIT_ERRf);

    _soc_gh_mem_parity_info(unit, block, 0,
                         info[table].error_field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                       SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                       entry_index, minfo);
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d MMU_IPMC VLAN entry %d %s ECC error\n"),
               unit, entry_index, double_err ? "double-bit" : ""));
    sal_memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = MMU_IPMC_VLAN_TBLm;
                spci.blk_type = SOC_BLK_MMU;
                spci.index = entry_index;
                (void)soc_ser_correction(unit, &spci);

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_process_mmu_wred_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{
#define _SOC_GH_MMU_WRED_MAX     9
    static soc_mem_t wred_mems[_SOC_GH_MMU_WRED_MAX] = {
          MMU_WRED_MARK_THDm,  
          MMU_WRED_DROP_PROFILE_GREENm,
          MMU_WRED_DROP_PROFILE_YELLOWm,
          MMU_WRED_DROP_PROFILE_REDm,
          MMU_WRED_MARK_PROFILE_GREENm,
          MMU_WRED_MARK_PROFILE_YELLOWm,
          MMU_WRED_MARK_PROFILE_REDm,
          MMU_WRED_AVG_QSIZEm,
          MMU_WRED_CONFIGm
    };
    static soc_reg_t wred_regs[_SOC_GH_MMU_WRED_MAX] = {
          WRED_MARK_THD_PARITY_ERR_PTRr,  
          WRED_DROP_PROFILE_GREEN_PARITY_ERR_PTRr,
          WRED_DROP_PROFILE_YELLOW_PARITY_ERR_PTRr,
          WRED_DROP_PROFILE_RED_PARITY_ERR_PTRr,
          WRED_MARK_PROFILE_GREEN_PARITY_ERR_PTRr,
          WRED_MARK_PROFILE_YELLOW_PARITY_ERR_PTRr,
          WRED_MARK_PROFILE_RED_PARITY_ERR_PTRr,
          WRED_AVG_QSIZE_PARITY_ERR_PTRr,
          WRED_CONFIG_PARITY_ERR_PTRr
    };


    _soc_gh_parity_info_t *info;
    soc_reg_t err_reg;
    uint32 reg_val, valid_val, entry_index, multi_err, multiple;
    uint32  minfo;
    _soc_ser_correct_info_t spci;
    int i;

    info = _soc_gh_parity_group_info[unit][group].info;

    err_reg = WRED_PARITY_ERR_STATUSr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                        block_port, 0, &reg_val));
    valid_val =
            soc_reg_field_get(unit, err_reg, reg_val, ERR_VALIDf);
    /* CHECK if the error is valid */
    if (valid_val == 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "unit %d %s: parity error is handled.\n"),
                 unit, msg));
        return SOC_E_NONE;
    }
    multiple =
            soc_reg_field_get(unit, err_reg, reg_val, MULTIPLE_ERRf);

    for (i = 0; i < _SOC_GH_MMU_WRED_MAX; i++) {
        if (valid_val & (1 << i)) {
            err_reg = wred_regs[i]; 
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                block_port, 0, &reg_val));
            entry_index =
                soc_reg_field_get(unit, err_reg, reg_val, ENTRY_INDEXf);
            multi_err = (multiple & (i << 1));
            _soc_gh_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                               entry_index, minfo);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s entry %d parity error\n"),
                       unit, SOC_MEM_NAME(unit, wred_mems[i]), 
                       entry_index));
            if (multi_err) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d %s has multiple parity errors\n"),
                           unit, SOC_MEM_NAME(unit, wred_mems[i])));
            }
            
            sal_memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = wred_mems[i];
            spci.blk_type = SOC_BLK_MMU;
            spci.index = entry_index;
            (void)soc_ser_correction(unit, &spci);
        }
    }

    /* Clear level 2 error status */
    err_reg = WRED_ERR_CLRr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                block_port, 0, &reg_val));
    soc_reg_field_set(unit, err_reg, &reg_val,
                            CLR_ERRf, valid_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, err_reg, 
                                block_port, 0, reg_val));

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_process_mmu_e2efc_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{
#define _SOC_GH_MMU_E2EFC_MAX     4
    static soc_reg_t e2efc_regs[_SOC_GH_MMU_E2EFC_MAX] = {
          INVALIDr,  
          E2EFC_CNT_SET_LIMITr,
          E2EFC_CNT_RESET_LIMITr,
          E2EFC_CNT_DISC_LIMITr
    };


    _soc_gh_parity_info_t *info;
    soc_reg_t err_reg;
    uint32 reg_val, entry_index, mem_id;
    uint32  minfo;
    _soc_ser_correct_info_t spci;

    info = _soc_gh_parity_group_info[unit][group].info;

    err_reg = E2EFC_PARITYERRORPTRr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                        block_port, 0, &reg_val));
    mem_id =
            soc_reg_field_get(unit, err_reg, reg_val, MEM_IDf);
    /* CHECK if the error is valid */
    if (mem_id == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s: parity hardware inconsistency\n"),
                   unit, msg));
        return SOC_E_NONE;
    }
    entry_index =
            soc_reg_field_get(unit, err_reg, reg_val, ENTRY_INDEXf);

    _soc_gh_mem_parity_info(unit, block, 0,
                         info[table].error_field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                       entry_index, minfo);
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d MMU E2EFC entry %d parity error\n"),
               unit, entry_index));
    sal_memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
                spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_KNOWN;
                spci.reg = e2efc_regs[mem_id];
                spci.mem = INVALIDm;
                spci.blk_type = SOC_BLK_MMU;
                spci.index = entry_index;
                (void)soc_ser_correction(unit, &spci);

    return SOC_E_NONE;
}


STATIC int
_soc_greyhound_process_mmu_cbppkthdr_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{
#define _SOC_GH_MMU_CBPPKTHDR_MAX     4
    static soc_reg_t cbppkthdr_regs[_SOC_GH_MMU_CBPPKTHDR_MAX] = {
          CBPPKTHDR_ERRPTR0r,  
          CBPPKTHDR_ERRPTR1r,
          CBPPKTHDR_ERRPTR2r,
          CBPPKTHDR_ERRPTR3r
    };

    _soc_gh_parity_info_t *info;
    soc_reg_t err_reg;
    uint32 reg_val, valid_val, entry_index, double_err, multiple, multi_err;
    uint32  minfo;
    int i;

    info = _soc_gh_parity_group_info[unit][group].info;

    /* CHECK if the error is valid */
    err_reg = CBPPKTHDR_ERR_VLDr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                    block_port, 0, &reg_val));
    valid_val =
        soc_reg_field_get(unit, err_reg, reg_val, ERR_VALIDf);
    if (valid_val == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s: parity hardware inconsistency\n"),
                   unit, msg));
        return SOC_E_NONE;
    }

    multiple =
            soc_reg_field_get(unit, err_reg, reg_val, MULTIPLE_ERRf);

    for (i = 0; i < _SOC_GH_MMU_CBPPKTHDR_MAX; i++) {
        if (valid_val & (1 << i)) {
            err_reg = cbppkthdr_regs[i]; 
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                block_port, 0, &reg_val));
            entry_index =
                soc_reg_field_get(unit, err_reg, reg_val, ENTRY_INDEXf);
            double_err =
                soc_reg_field_get(unit, err_reg, reg_val, DOUBLE_BIT_ERRf);
            multi_err = (multiple & (i << 1));
            _soc_gh_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                               entry_index, minfo);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d CBPPKTHDR %d entry %d %s ECC error\n"),
                       unit, i, entry_index, double_err ? "double-bit" : ""));
            if (multi_err) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d CBPPKTHDR %d has multiple ECC errors\n"),
                           unit, i));
            }
        }
    }
    
    
    /* Clear level 2 status */
    err_reg = CBPPKTHDR_ERR_CLRr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                block_port, 0, &reg_val));
    soc_reg_field_set(unit, err_reg, &reg_val,
                            CLR_ERRf, valid_val);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, err_reg, 
                                block_port, 0, reg_val));


    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_process_port_macro_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{

    _soc_gh_parity_info_t *info;
    soc_reg_t err_reg;
    uint32 reg_val, err;
    uint32  minfo;

    info = _soc_gh_parity_group_info[unit][group].info;

    err_reg = XLPORT_INTR_STATUSr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                        block_port, 0, &reg_val));

    /* Check errors */
    err = soc_reg_field_get(unit, err_reg, reg_val, TSC_ERRf);
    if (err) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d  port macro ECC error on TSC.\n"),
                   unit));
    }
    err = soc_reg_field_get(unit, err_reg, reg_val, MAC_RX_CDC_MEM_ERRf);
    if (err) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d  port macro ECC error on MAC RX CDC memory.\n"),
                   unit));
    }
    err = soc_reg_field_get(unit, err_reg, reg_val, MAC_TX_CDC_MEM_ERRf);
    if (err) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d  port macro ECC error on MAC TX CDC memory.\n"),
                   unit));
    }
    err = soc_reg_field_get(unit, err_reg, reg_val, MIB_RX_MEM_ERRf);
    if (err) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d  port macro ECC error on MIB RX STAT counter memory.\n"),
                   unit));
    }
    err = soc_reg_field_get(unit, err_reg, reg_val, MIB_TX_MEM_ERRf);
    if (err) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d  port macro ECC error on MIB TX STAT counter memory.\n"),
                   unit));
    }

    /* report error */
    _soc_gh_mem_parity_info(unit, block, 0,
                         info[table].error_field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                       SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                       0, minfo);

    return SOC_E_NONE;
}


STATIC int
_soc_greyhound_process_counter_error(int unit, int group,
                                    soc_port_t block_port, int table,
                                    char *msg, soc_block_t block)
{
    soc_cmap_t *cmap;
    soc_reg_t reg, counter_reg;
    uint32 reg_val, minfo;
    uint32 multiple, counter_idx, port_idx, entry_idx;
    char *counter_name;
    _soc_ser_correct_info_t spci;

    _soc_gh_parity_info_t *info;

    
    info = _soc_gh_parity_group_info[unit][group].info;
    reg = info[table].intr_status0_reg;
    
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, 
                                    block_port, 0, &reg_val));

    if (soc_reg_field_get(unit, reg, reg_val, PARITY_ERRf)) {
        multiple = soc_reg_field_get(unit, reg, reg_val, MULTIPLE_ERRf);
        counter_idx = soc_reg_field_get(unit, reg, reg_val, COUNTER_IDXf);
        port_idx = soc_reg_field_get(unit, reg, reg_val, PORT_IDXf);
        entry_idx = soc_reg_field_get(unit, reg, reg_val, ENTRY_IDXf);

        /* TDBGC starts at index 0x20 of counter DMA table */
        if (info[table].error_field == EGR_STATS_COUNTER_TABLE_PAR_ERRf) {
            counter_idx += 0x20;
        }
        cmap = soc_port_cmap_get(unit, port_idx);
        counter_reg = cmap->cmap_base[counter_idx].reg;
        if (SOC_REG_IS_VALID(unit, counter_reg)) {
            _soc_gh_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                               entry_idx, minfo);
            counter_name = SOC_REG_NAME(unit, counter_reg);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "EGR STAT COUNTER port %d %s entry %d parity error\n"),
                       port_idx, counter_name,
                       entry_idx));
            sal_memset(&spci, 0, sizeof(spci));
            spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_KNOWN;
            spci.mem = INVALIDm;
            spci.port = port_idx;
            spci.index = 0;
            spci.reg = counter_reg;
            spci.blk_type = block;
            (void)soc_ser_correction(unit, &spci);    
            if (multiple) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "EGR STAT COUNTER has multiple parity errors\n")));
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "EGR STAT COUNTER parity hardware inconsistency\n")));
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "EGR_STAT_COUNTER parity hardware inconsistency\n")));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg,  block_port, 0, 0));

    return SOC_E_NONE;
}


STATIC int
_soc_greyhound_process_parity_error(int unit)
{
    int group, table, reg_is_64, blk_num = 0;
    uint32 cmic_rval, group_status,
        group_enable, cpi_blk_bit, group_clr;
    _soc_gh_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_block_t blk;
    char *msg;
    uint64 group_status64, group_enable64, temp64;
    uint32 minfo;
    _soc_gh_parity_group_info_t *parity_group_info;

    /* Read CMIC parity status register */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN
        (READ_CMIC_CMC0_IRQ_STAT3r(unit, &cmic_rval));
    if (cmic_rval == 0) {
        return SOC_E_NONE;
    }

    parity_group_info = _soc_gh_parity_group_info[unit];
    if (parity_group_info == NULL) {
        LOG_CLI((BSL_META_U(unit,
                "unit %d the parity error can't be handled.\n"), unit));
        return SOC_E_UNAVAIL;
    }

    /* loop through each group */
    for (group = 0; parity_group_info[group].cpi_bit; group++) {
        info = parity_group_info[group].info;
        group_reg = parity_group_info[group].status_reg;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d %s parity processing\n"),
                                unit, SOC_REG_NAME(unit, group_reg)));
        reg_is_64 = SOC_REG_IS_64(unit,group_reg) ? 1 : 0;
        cpi_blk_bit = parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       parity_group_info[group].blocktype) {
            if (_soc_greyhound_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }

            if (parity_group_info[group].blocktype == SOC_BLK_XLPORT) {
                blk_num = SOC_BLOCK_INFO(unit, blk).number;
                if (blk_num == 5) {
                    info = _soc_gh_gx_parity_info;
                    group_reg = PGW_GX_INTR_STATUSr;
                }
                cpi_blk_bit = (cpi_blk_bit << blk_num);
            } 
            
            if (!(cmic_rval & cpi_blk_bit)) {
                continue;
            }

            if (reg_is_64) {
                SOC_IF_ERROR_RETURN(soc_reg64_get(
                    unit, group_reg, block_port, 0, &group_status64));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg32_get(
                    unit, group_reg, block_port, 0, &group_status));
            }

            /* loop through each table in the group */
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                if (_soc_greyhound_ser_mem_skip(unit, info[table].feature)) {
                    continue;
                }
                if (!soc_reg_field_valid(unit, group_reg,
                                         info[table].error_field)) {
                    if (info[table].mem != INVALIDm) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit %d %s has bad error field\n"),
                                              unit, SOC_MEM_NAME(unit, info[table].mem)));
                    }
                    continue;
                }

                if (reg_is_64) {
                    if (!soc_reg64_field32_get(unit, group_reg, group_status64,
                                           info[table].error_field)) {
                        continue;
                    }
                } else {
                    if (!soc_reg_field_get(unit, group_reg, group_status,
                                           info[table].error_field)) {
                        continue;
                    }
                }
                if (info[table].msg) {
                    msg = info[table].msg;
                } else if (info[table].mem != INVALIDm) {
                    msg = SOC_MEM_NAME(unit, info[table].mem);
                } else {
                    msg = SOC_FIELD_NAME(unit, info[table].error_field);
                }

                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "unit %d %s analysis\n"),
                                        unit, msg));

                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                    _soc_gh_mem_parity_info(unit, blk,
                        0, info[table].error_field , &minfo);
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 0,
                               minfo);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s asserted\n"),
                                          unit, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_single_parity_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_single_ecc_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_dual_parity_error(unit,
                                  group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_mmu_ecc(unit,
                              group, block_port, table, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_XQ:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_mmu_xq(unit,
                              group, block_port, table, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_IPMCGROUP:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_mmu_ipmcgroup_error(unit,
                              group, block_port, table, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_IPMCVLAN:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_mmu_ipmcvlan_error(unit,
                              group, block_port, table, msg, blk));
                    break;
                    
                case _SOC_PARITY_INFO_TYPE_MMU_WRED:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_mmu_wred_error(unit,
                              group, block_port, table, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_E2EFC:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_mmu_e2efc_error(unit,
                              group, block_port, table, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_CBPPKTHDR:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_mmu_cbppkthdr_error(unit,
                              group, block_port, table, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_COUNTER:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_counter_error(unit,
                              group, block_port, table, msg, blk));
                    break;

                case _SOC_PARITY_INFO_TYPE_OAM:
                    SOC_IF_ERROR_RETURN
                         (_soc_gh_process_oam_interrupt(unit));
                    break;
                case _SOC_PARITY_INFO_TYPE_PORT_MACRO:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_port_macro_error(unit,
                              group, block_port, table, msg, blk));
                    break;

                default:
                    break;
                } /* handle different parity error reporting style */

                /* Level 1 interrupt status clear */
                if (parity_group_info[group].clear_reg != INVALIDr) {
                    /* Clean interrupt status */
                    reg = parity_group_info[group].clear_reg;
                    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg,  block_port, 
                        0, &group_clr));
                    soc_reg_field_set(unit, reg, &group_clr,
                                           info[table].clear_field, 1);
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg,  block_port, 0, group_clr));
                }
            } /* loop through each table in the group */

            

            reg = parity_group_info[group].enable_reg;
             if ((parity_group_info[group].blocktype == SOC_BLK_XLPORT) && 
                    (blk_num == 5)) {
                reg = PGW_GX_INTR_ENABLEr;
            } 

            if (reg_is_64) {
                SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, block_port, 0, &group_enable64));
                temp64 = group_status64;
                COMPILER_64_NOT(temp64);
                COMPILER_64_AND(group_enable64, temp64);
                SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg,  
                    block_port, 0, group_enable64));
                COMPILER_64_OR(group_enable64, group_status64);
                SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg,  
                    block_port, 0, group_enable64));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg,  block_port, 0, &group_enable));
                group_enable &= ~group_status;
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg,  block_port, 0, group_enable));
                group_enable |= group_status;
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg,  block_port, 0, group_enable));
            }

        } /* loop through each block for the group */

    } /* loop through each group */

    return SOC_E_NONE;
}

void
soc_greyhound_parity_error(void *unit_vp, void *d1, void *d2,
                         void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    (void)_soc_greyhound_process_parity_error(unit);
    sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 1000); /* Don't reenable too soon */
    if (d2 != NULL) {
        (void)soc_cmicm_intr3_enable(unit, PTR_TO_INT(d2));
    }
}

STATIC int
_soc_greyhound_mem_nack_error_process(int unit, 
        _greyhound_ser_nack_reg_mem_t nack_reg_mem, 
        soc_block_t block, int reg_mem)
{
    int group, table;
    _soc_gh_parity_info_t *info;
    soc_port_t block_port;
    soc_block_t blk;
    char *msg;
    _soc_gh_parity_group_info_t *parity_group_info;

    if (nack_reg_mem.mem == INVALIDm) {
        return SOC_E_PARAM;
    }

    parity_group_info = _soc_gh_parity_group_info[unit];
    if (parity_group_info == NULL) {
        LOG_CLI((BSL_META_U(unit,
                "unit %d the NACK parity error can't be handled.\n"), unit));
        return SOC_E_UNAVAIL;
    }

    /* loop through each group */
    for (group = 0; parity_group_info[group].cpi_bit; group++) {
        info = parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                       parity_group_info[group].blocktype) {
            if (block != SOC_BLOCK_INFO(unit, blk).schan) {
                continue;
            }
            if (_soc_greyhound_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }
            /* loop through each table in the group */
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                if (_soc_greyhound_ser_mem_skip(unit, info[table].feature)) {
                    continue;
                }
                if (info[table].mem != nack_reg_mem.mem) {
                    continue;
                }                
                if (info[table].msg) {
                    msg = info[table].msg;
                } else { /* mem must be valid to get her */
                    msg = SOC_MEM_NAME(unit, info[table].mem);
                }

                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_single_parity_error(unit,
                              group, block_port, table, FALSE, msg, block));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_single_ecc_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_greyhound_process_dual_parity_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;                   
                default:
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */
        } /* loop through each block of the group */
    } /* loop through each group */

    return SOC_E_NONE;
}


void
soc_greyhound_mem_nack(void *unit_vp, void *addr_vp, void *blk_vp, 
                     void *d3, void *d4)
{
    soc_mem_t mem = INVALIDm;
    int reg_mem = PTR_TO_INT(d3);
    int rv, unit = PTR_TO_INT(unit_vp);
    uint32 address = PTR_TO_INT(addr_vp);
    uint32 block = PTR_TO_INT(blk_vp);
    uint32 offset = 0, min_addr = 0;
    soc_regaddrinfo_t ainfo;
    _greyhound_ser_nack_reg_mem_t nack_reg_mem;
    soc_field_t     error_fld = INVALIDf;
    uint32 minfo;
    _soc_ser_correct_info_t spci;
    char *msg;

    if (reg_mem == _SOC_GREYHOUND_SER_REG) {
        nack_reg_mem.reg = INVALIDr;
        if (address) {
            soc_regaddrinfo_get(unit, &ainfo, address);
            nack_reg_mem.reg = ainfo.reg;
        }
        if (nack_reg_mem.reg == INVALIDr) {
            /* Invalid register */
            return;
        }
    } else {
        offset = address;
        
        mem = soc_addr_to_mem_extended(unit, block, 0, address);
        if (mem == INVALIDm) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d mem decode failed, "
                                  "SCHAN NACK analysis failure\n"), unit));
            return;
        }
        
        nack_reg_mem.mem = mem;

        min_addr = SOC_MEM_INFO(unit, mem).base;
        min_addr += SOC_MEM_INFO(unit, mem).index_min;
    }

    if (reg_mem == _SOC_GREYHOUND_SER_MEM) {
        if ((mem == EGR_L3_NEXT_HOPm) || (mem == EGR_L3_INTFm) ||
            (mem == EGR_VLANm) || (mem == EGR_VLAN_STGm) ||
            (mem == EFP_COUNTER_TABLEm) || (mem == EGR_ETAG_PCP_MAPPINGm) ||
            (mem == EGR_PERQ_XMT_COUNTERSm) || (mem == EGR_GPP_ATTRIBUTESm) ||
            (mem == EGR_MOD_MAP_TABLEm) || 
            (mem == EGR_PER_Q_ECN_MARKEDm) || 
            (mem == L2_USER_ENTRY_DATA_ONLYm)) {
            /* 
             * The inetrupt of parity error for these memories will not be triggered.
             * And the parity error status register will not be updated.
             * Use the offset value of S-channel msg to instead of.
             */
            switch (mem) {
                case EGR_L3_NEXT_HOPm:
                    error_fld = EGR_NHOP_PAR_ERRf;
                    break;
                case EGR_L3_INTFm:
                    error_fld = EGR_L3_INTF_PAR_ERRf;
                    break;
                case EGR_VLANm:
                    error_fld = EGR_VLAN_PAR_ERRf;
                    break;
                case EGR_VLAN_STGm:
                    error_fld = EGR_VLAN_STG_PAR_ERRf;
                    break;
                case EFP_COUNTER_TABLEm:
                    error_fld = EGR_FP_COUNTERf;
                    break;
                case EGR_ETAG_PCP_MAPPINGm:
                    error_fld = EGR_ETAG_PCP_DE_MAPPING_PAR_ERRf;
                    break;
                case EGR_GPP_ATTRIBUTESm:
                    error_fld = EGR_GPP_ATTRIBUTES_PAR_ERRf;
                    break;
                case EGR_MOD_MAP_TABLEm:
                    error_fld = EGR_MOD_MAP_TABLE_PAR_ERRf;
                    break;
                case EGR_PER_Q_ECN_MARKEDm:
                    error_fld = EGR_PER_Q_ECN_MARKED_PAR_ERRf;
                    break;
                case L2_USER_ENTRY_DATA_ONLYm:
                    mem = L2_USER_ENTRYm;
                    error_fld = L2_USER_ENTRY_DATA_ONLY_PAR_ERRf;
                    break;
                default:
                    /* EGR_PERQ_XMT_COUNTERSm */
                    error_fld = EGR_PERQ_COUNTERf;
                    break;
            }
            _soc_gh_mem_parity_info(unit, block, 0,
                                     error_fld, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                                   SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                                   (offset - min_addr), minfo);
            msg = SOC_MEM_NAME(unit, mem);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s entry %d parity error\n"),
                       unit, msg, (offset - min_addr)));
            sal_memset(&spci, 0, sizeof(spci));
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;         
            spci.reg = INVALIDr;
            spci.mem = mem;
            spci.blk_type = -1;
            spci.index = (offset - min_addr);
            (void)soc_ser_correction(unit, &spci);
        } else {

            if ((rv = _soc_greyhound_mem_nack_error_process(unit, nack_reg_mem, 
                (soc_block_t)block, reg_mem)) < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d %s entry %d SCHAN NACK analysis failure\n"),
                           unit, SOC_MEM_NAME(unit, mem),
                           offset - min_addr));
            }
        }

        LOG_CLI((BSL_META_U(unit,
                            "unit %d %s entry %d SCHAN NACK analysis\n"),
                 unit, SOC_MEM_NAME(unit, mem),
                 offset - min_addr));
    } else {
        /* Regitser */
        LOG_CLI((BSL_META_U(unit,
                            "unit %d %s REG SCHAN NACK analysis\n"),
                 unit, SOC_REG_NAME(unit, nack_reg_mem.reg)));
        if ((ainfo.reg >= TDBGC0r) && (ainfo.reg <= TDBGC11r)) {
            error_fld = EGR_STATS_COUNTER_TABLE_PAR_ERRf;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s REG unexpected NACK failure!\n"),
                       unit, SOC_REG_NAME(unit, nack_reg_mem.reg)));
            return;
        }
        _soc_gh_mem_parity_info(unit, block, 0,
                                 error_fld, &minfo);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                               ainfo.idx, minfo);
        msg = SOC_REG_NAME(unit, nack_reg_mem.reg);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d port %d %s index %d parity error\n"),
                   unit, ainfo.port, msg, ainfo.idx));
        sal_memset(&spci, 0, sizeof(spci));
        spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_KNOWN;         
        spci.reg = ainfo.reg;
        spci.port = ainfo.port;
        spci.mem = INVALIDm;
        spci.blk_type = SOC_BLOCK_ANY;
        spci.index = ainfo.idx;
        (void)soc_ser_correction(unit, &spci);
    }
}

static int
soc_greyhound_pipe_mem_clear(int unit)
{
    uint32              rval;
    int                 pipe_init_usec, index;
    soc_timeout_t       to;
    static const struct {
        soc_mem_t mem;
        uint32 skip_flags; /* always skip on QUICKTURN or XGSSIM */
    } cam_list[] = {
        { CPU_COS_MAPm,                     BOOT_F_PLISIM },
        { EFP_TCAMm,                        BOOT_F_PLISIM },
        { FP_GLOBAL_MASK_TCAMm,             BOOT_F_PLISIM },
        { FP_TCAMm,                         BOOT_F_PLISIM },
        { L2_USER_ENTRYm,                   BOOT_F_PLISIM },
        { L2_USER_ENTRY_ONLYm,              BOOT_F_PLISIM },
        { L3_DEFIPm,                        BOOT_F_PLISIM },
        { L3_DEFIP_ONLYm,                   BOOT_F_PLISIM },
        { VFP_TCAMm,                        BOOT_F_PLISIM },
        { VLAN_SUBNETm,                     0 },/* VLAN API needs all 0 mask */
        { VLAN_SUBNET_ONLYm,                0 }
    };

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries in largest IPIPE table, L2_ENTRYm */
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 0x4000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table, EGR_VLANm */
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x1000);
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
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
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
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM) {
        /* TCAM tables are not handled by hardware reset control */
        for (index = 0; index < sizeof(cam_list) / sizeof(cam_list[0]);
             index++) {
            if (sal_boot_flags_get() & cam_list[index].skip_flags) {
                continue;
            }
            if (((cam_list[index].mem == EFP_TCAMm) || 
                (cam_list[index].mem == VFP_TCAMm)) && 
                (!soc_feature(unit, soc_feature_field_multi_stage))) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, cam_list[index].mem, COPYNO_ALL, TRUE));
        }

        /* MMU_IPMC_VLAN_TBL */
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_VLAN_TBLm, COPYNO_ALL, TRUE));

        /* MMU_IPMC_GROUP_TBLn : n is 2 ~ 29 in Greyhound */
        for (index = 0; index < _SOC_GH_MMU_IPMC_GROUP_MAX; index++) {
            if (ipmc_mems[index] != INVALIDm) {
                SOC_IF_ERROR_RETURN(soc_mem_clear(unit, 
                        ipmc_mems[index], COPYNO_ALL, TRUE));
            }
        }

    }
    return SOC_E_NONE;
}


#define  GH_NUM_COS  8
int soc_gh_xq_mem(int unit, soc_port_t port, soc_mem_t *xq)
{
    soc_mem_t   xq_mems[] = {   MMU_XQ0m,
                                MMU_XQ1m,
                                MMU_XQ2m,
                                MMU_XQ3m,
                                MMU_XQ4m,
                                MMU_XQ5m,
                                MMU_XQ6m,
                                MMU_XQ7m,
                                MMU_XQ8m,
                                MMU_XQ9m,
                                MMU_XQ10m,
                                MMU_XQ11m,
                                MMU_XQ12m,
                                MMU_XQ13m,
                                MMU_XQ14m,
                                MMU_XQ15m,
                                MMU_XQ16m,
                                MMU_XQ17m,
                                MMU_XQ18m,
                                MMU_XQ19m,
                                MMU_XQ20m,
                                MMU_XQ21m,
                                MMU_XQ22m,
                                MMU_XQ23m,
                                MMU_XQ24m,
                                MMU_XQ25m,
                                MMU_XQ26m,
                                MMU_XQ27m,
                                MMU_XQ28m,
                                MMU_XQ29m
                            };
    int max_port;
    max_port = sizeof(xq_mems) / sizeof(xq_mems[0]);
    if (port >= max_port) {
        return(SOC_E_PORT);
    }
    *xq = xq_mems[port];
    return(SOC_E_NONE);
}


int
soc_greyhound_init_port_mapping(int unit)
{
    soc_info_t *si;
    soc_mem_t mem;
    uint32 rval;
    ing_physical_to_logical_port_number_mapping_table_entry_t entry;
    int port, phy_port;
    int num_port, num_phy_port;
    int speed;

    si = &SOC_INFO(unit);

    /* Ingress physical to logical port mapping */
    mem = ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm;
    num_phy_port = soc_mem_index_count(unit, mem);
    sal_memset(&entry, 0, sizeof(entry));
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        soc_mem_field32_set(unit, mem, &entry, LOGICAL_PORT_NUMBERf,
                            port == -1 ? 0x1f : port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port, &entry));
    }
    num_port = soc_mem_index_count(unit, PORT_TABm);

    /* Egress logical to physical port mapping */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        rval = 0;
        soc_reg_field_set(unit, EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr,
                          &rval, PHYSICAL_PORT_NUMBERf,
                          phy_port == -1 ? 0x3f : phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr(unit, port,
                                                                rval));
    }
    /* MMU logical to physical port mapping */
    /*(Here, Same as Egress logical to physical port mapping) */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_m2p_mapping[port];
        if (phy_port != -1) {
            rval = 0;
            soc_reg_field_set(unit, LOG_TO_PHY_PORT_MAPPINGr, &rval,
                              PHY_PORTf, phy_port);
            SOC_IF_ERROR_RETURN
                (WRITE_LOG_TO_PHY_PORT_MAPPINGr(unit, port, rval));
        }
    }

    /* Initialize the si->port_init_speed */
    PBMP_ALL_ITER(unit, port) {
        /*
         * Let the port_init_speed as the max supported speed of xe
         * while the port_speed_max would be the max supported speed of hg
         */
        switch(si->port_speed_max[port]) {
            case 11000:
                speed = 10000;
                break;
            default:
                speed = si->port_speed_max[port];
                break;
        }
        si->port_init_speed[port] = speed;
    }
    return SOC_E_NONE;
}

/* Provide the max speed mapping from xe to hg */
int
soc_greyhound_port_hg_speed_get(int unit, soc_port_t port, int *speed)
{
    soc_info_t *si;
    si = &SOC_INFO(unit);

    switch (si->port_init_speed[port]) {
        case 10000:
            *speed = 11000;
            break;
        default:
            *speed = si->port_init_speed[port];
            break;
    }

    if (*speed > si->port_speed_max[port]) {
        *speed = si->port_speed_max[port];
    }
    return SOC_E_NONE;
}

/*
 *  GH port mapping
 * cpu port number is fixed: physical 0, logical 0, mmu 0
 */
int
soc_greyhound_port_config_init(int unit, uint16 dev_id)
{

    soc_info_t *si;
    int rv = SOC_E_NONE;
    int phy_port, dual_port_idx, blk,port_idx;
    const int *p2l_mapping = 0,*speed_max = 0;
    const int *valid_port_idx = 0;
    char *propval,*option;
    int match = -1, i, j, match_option;
    int p2l_mapping_override[SOC_MAX_PHY_PORTS];
    int port_speed_max_override[SOC_MAX_PHY_PORTS];    
    _gh_sku_info_t *matched_port_config;


    option = soc_property_get_str(unit, spn_BCM53400_INIT_PORT_CONFIG);
    if(option == NULL) {
        _gh_port_config_id = 1;
    } else {
        _gh_port_config_id = sal_ctoi(option, NULL);
    }
    match_option = -1;
    for (i=0; _gh_sku_port_config[i].dev_id; i++){
        if(dev_id == _gh_sku_port_config[i].dev_id) {
            match = i;
            if (_gh_sku_port_config[i].config_op != -1 ){
                match_option = 0;
                if (_gh_sku_port_config[i].config_op == _gh_port_config_id ){
                    match = i;                
                    match_option = _gh_port_config_id;
                    break;
                }                 
            } 
        }
    }
    for (i = 0; i < _GH_MAX_TSC_COUNT; i++) {
        _gh_tsc[i].lane_mode = -1;
        _gh_tsc[i].valid = 0;
        _gh_tsc[i].phy_port_base = tsc_phy_port[i];
        propval = soc_property_suffix_num_str_get(unit, i, spn_BCM53400_INIT_PORT_CONFIG, "tsc");    
        if (propval) {
            if (sal_strcmp(propval, "SINGLE") == 0) {
                _gh_tsc[i].lane_mode = _GH_TSC_LANE_MODE_SINGLE;
                _gh_tsc[i].valid = 1;
            } else if (sal_strcmp(propval, "XAUI") == 0) {
                _gh_tsc[i].lane_mode = _GH_TSC_LANE_MODE_XAUI;
                _gh_tsc[i].valid = 1;
            } else if (sal_strcmp(propval, "RXAUI") == 0) {
                _gh_tsc[i].lane_mode = _GH_TSC_LANE_MODE_RXAUI;
                _gh_tsc[i].valid = 1;
            } else {
                if (match == -1) {
                    /* take 53401 option 1 as default config */
                    _gh_tsc[i].lane_mode = _gh_sku_port_config[0].default_lane_mode[i];

                } else {
                    _gh_tsc[i].lane_mode = _gh_sku_port_config[match].default_lane_mode[i];
                }
            }
        } else {
            if (match == -1) {
                _gh_tsc[i].lane_mode = _gh_sku_port_config[0].default_lane_mode[i];
            } else {
                _gh_tsc[i].lane_mode = _gh_sku_port_config[match].default_lane_mode[i];
            }
        }    
    }     
    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        p2l_mapping_override[phy_port] = 0;
        port_speed_max_override[phy_port] = 0;
    }
    dual_port_idx = 0;
    if (match == -1) {
        /* take 53401 option 1 as default config */
        p2l_mapping = p2l_mapping_cascade;
        speed_max = port_speed_max_non_cascade;
        dual_port_idx = 1;
        valid_port_idx = port_blk_gport;
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "no device_id matched\n")));
    } else {
        matched_devid_idx = match;
        matched_port_config = &_gh_sku_port_config[match];
        if (((option != NULL) && (matched_port_config->config_op == -1)) ||
            (match_option == 0)){
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Warning: bcm53400_init_port_config=%d config "
                                 "is not supported in %s ! \n"),_gh_port_config_id,
                      soc_dev_name(unit)));
        }

        p2l_mapping = matched_port_config->p2l_mapping;
        speed_max = matched_port_config->speed_max;

        if (matched_port_config->dual_port_idx != 0) {
            dual_port_idx = 1;
            valid_port_idx = matched_port_config->port_blk_type;
        }
        for (i = 0; i <_GH_MAX_TSC_COUNT; i++) {
            if(_gh_tsc[i].valid) {
                if((1 << i) & matched_port_config->flex_tsc_bmp){
                /* support flex tsc settings */
                    /* validate the assigned flexible lane mode */
                    if (!_GH_FLEX_LANE_MODE_IS_SUPPORT(
                            matched_port_config->flex_lane_modes_bmp[i], 
                            _gh_tsc[i].lane_mode)) {
                        LOG_WARN(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                                "Warning: bcm53400_init_port_config_tsc%d " 
                                "config is not supported in %s ! \n"), 
                                i, soc_dev_name(unit)));
                        _gh_tsc[i].lane_mode = 
                                matched_port_config->default_lane_mode[i];
                    }
                    if(_gh_tsc[i].lane_mode == _GH_TSC_LANE_MODE_XAUI) {
                        for (j = 0; j < 4; j++) {
                            p2l_mapping_override[tsc_phy_port[i]+j] = 
                                lane_xaui_override[j];
                            port_speed_max_override[tsc_phy_port[i]+j] = 
                                lane_xaui_speed_override[j];
                        }
                    } else if(_gh_tsc[i].lane_mode == _GH_TSC_LANE_MODE_RXAUI) {
                        for (j = 0; j < 4; j++) {
                            p2l_mapping_override[tsc_phy_port[i]+j] = 
                                lane_rxaui_override[j];
                            port_speed_max_override[tsc_phy_port[i]+j] = 
                                lane_rxaui_speed_override[j];
                        }
                    } else {
                        /* expected for LANEMODE_SIGNLE */
                        for (j = 0; j < 4; j++) {
                            p2l_mapping_override[tsc_phy_port[i]+j] = 
                                lane_single_override[j];
                        }
                    }
                } else {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                             (BSL_META_U(unit,
                                         "Warning: bcm53400_init_port_config_tsc%d config "
                                         "is not supported in %s ! \n"),i,soc_dev_name(unit)));
                    _gh_tsc[i].lane_mode = 
                        matched_port_config->default_lane_mode[i];
                }
            }
            if((1 << i) & matched_port_config->disabled_tsc_bmp){
                /* Disable TSC here */
                for (j = 0; j < 4; j++) {
                    p2l_mapping_override[tsc_phy_port[i]+j] = 
                        lane_disable_override[j];
                    port_speed_max_override[tsc_phy_port[i]+j] = 
                        lane_disable_override[j];
                }
            }
        }
    }

    si = &SOC_INFO(unit);

    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        if (p2l_mapping_override[phy_port] == -1){
            si->port_p2l_mapping[phy_port] = p2l_mapping_override[phy_port];
            /* Same as Egr P2L */
            si->port_p2m_mapping[phy_port] = p2l_mapping_override[phy_port]; 
            /* For COSQ programming */
            si->max_port_p2m_mapping[phy_port] = p2l_mapping_override[phy_port]; 
        } else {
            /* speed_max reflects the valid port */
            /* For the same TDM, the phy_port should be existed in p2l_mapping table */
            if((speed_max[phy_port] == -1) && (phy_port != 0)){
                si->port_p2l_mapping[phy_port] = -1;
                si->port_p2m_mapping[phy_port] = -1;
                si->max_port_p2m_mapping[phy_port] = -1;
            } else {
                si->port_p2l_mapping[phy_port] = p2l_mapping[phy_port];
                si->port_p2m_mapping[phy_port] = p2l_mapping[phy_port];
                si->max_port_p2m_mapping[phy_port] = p2l_mapping[phy_port];
            }
        }
        if (port_speed_max_override[phy_port] != 0){
            if (port_speed_max_override[phy_port] != -1) {
                si->port_speed_max[si->port_p2l_mapping[phy_port]] = 
                    port_speed_max_override[phy_port] * 100;
            }
        } else {
            if (speed_max[phy_port] != -1) {
                si->port_speed_max[si->port_p2l_mapping[phy_port]] = 
                    speed_max[phy_port] * 100;
            }
        }
        if (dual_port_idx && (valid_port_idx[phy_port] != -1)) {
            for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
                 port_idx++) {
                if (valid_port_idx[phy_port] != port_idx){
                    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
                    SOC_BLOCK_INFO(unit, blk).number |= 0xf00;
                }
            }
        }    
    }
    return rv;
}


int
_soc_greyhound_tdm_init(int unit, uint16 dev_id)
{   
    uint32              *arr = NULL;
    int                 tdm_size;
    uint32              rval; 
    iarb_tdm_table_entry_t iarb_tdm;
    mmu_arb_tdm_table_entry_t mmu_arb_tdm;
    int i, port, phy_port;
    _gh_sku_info_t *matched_sku_info;
    soc_info_t *si;
    int tsc5_phy_port = 2;

    si = &SOC_INFO(unit);


    
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, 83);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));
    
    if (matched_devid_idx == -1) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Warning: soc_greyhound_port_config_init should "
                             "be invoked first! Choose bcm534x1 port config.\n")));
        matched_devid_idx = 0;
    }
    matched_sku_info = &_gh_sku_port_config[matched_devid_idx];

    arr = matched_sku_info->tdm_table;
    tdm_size = matched_sku_info->tdm_table_size;
    if((matched_sku_info->dev_id == BCM53406_DEVICE_ID && matched_sku_info->config_op == 1)||
       (matched_sku_info->dev_id == BCM53369_DEVICE_ID && matched_sku_info->config_op == 1)){
        if( si->port_speed_max[si->port_p2l_mapping[tsc5_phy_port]] > 5000){
            /* Re check after support 5G */
            arr = gh_tdm_mixed_embeded_2xrxaui;
            tdm_size = _GH_ARRAY_SIZE(gh_tdm_mixed_embeded_2xrxaui);
        }
    }

    if (arr == NULL) {
        return SOC_E_CONFIG;
    }

    for (i = 0; i < tdm_size; i++) {
        phy_port = arr[i];
        port = (phy_port != 63) ? matched_sku_info->p2l_mapping[phy_port]:63;
        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        sal_memset(&mmu_arb_tdm, 0, sizeof(mmu_arb_tdm_table_entry_t));

        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf, 
                                        phy_port);

        soc_MMU_ARB_TDM_TABLEm_field32_set(unit, &mmu_arb_tdm, PORT_NUMf, 
                                       port); 
        if (i == tdm_size - 1) {
            soc_MMU_ARB_TDM_TABLEm_field32_set(unit, &mmu_arb_tdm, WRAP_ENf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                  &iarb_tdm));
        SOC_IF_ERROR_RETURN(WRITE_MMU_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                 &mmu_arb_tdm));
    }
    rval = 0;
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, 
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));
    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_mmu_init(int unit)
{
    int         cos;
    soc_port_t  port;
    uint32      val, oval, cfap_max_idx;
    int         age[GH_NUM_COS], max_age, min_age;
    int         age_enable, disabled_age;
    int         skid_mark;
    uint32  dyn_xq_per_port;
    uint32      limit; /* E2EFC configuration */
    uint32  xq_limit, cell_limit;
    uint32  cell_resume_delta;
    uint32      cos_min_xqs, cos_min_cells;
    uint16      dev_id;
    uint8       rev_id;
    int mmu_total_cells;
    int mmu_total_dyn_cells;
    int mmu_total_static_cells;
    int total_ports;
    uint32 max_cell_limit_pri0;
    uint32 max_cell_limit_pri1_7;
    uint32 dyn_cell_limit;
    uint32 xoff_cell_limit;
    uint32 reset_cell_limit;
    uint32 xoff_pkt_limit;
    uint32 reset_pkt_limit;
    uint32 dyn_cell_reset_delta;
    _gh_sku_info_t *matched_sku_info;
    int config_op = 1;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    SOC_IF_ERROR_RETURN(_soc_greyhound_tdm_init(unit, dev_id));

    matched_sku_info = &_gh_sku_port_config[matched_devid_idx];
    config_op = matched_sku_info->config_op;

    cfap_max_idx = soc_mem_index_max(unit, MMU_CFAPm); 
    SOC_IF_ERROR_RETURN(READ_CFAPCONFIGr(unit, &val));
    oval = val;
    soc_reg_field_set(unit, CFAPCONFIGr, &val, CFAPPOOLSIZEf, cfap_max_idx);
    if (oval != val) {
        SOC_IF_ERROR_RETURN(WRITE_CFAPCONFIGr(unit, val));
    }

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    /* set dyn_xq_en on */
    soc_reg_field_set(unit, MISCCONFIGr, &val, DYN_XQ_ENf, 1);
    /* set HOL_CELL_SOP_DROP_EN on */
    soc_reg_field_set(unit, MISCCONFIGr, &val, HOL_CELL_SOP_DROP_ENf, 1);
    /* set skidmarker to 3 */
    soc_reg_field_set(unit, MISCCONFIGr, &val, SKIDMARKERf, 3);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));

    /* Every inactive COSQ consumes skid mark + 4 XQs; remove them from pool */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    skid_mark = soc_reg_field_get(unit, MISCCONFIGr, val, SKIDMARKERf) + 4;

    /* Enable IP to CMICM credit transfer */
    val = 0;
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr, &val, TRANSFER_ENABLEf, 1);
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr, &val, NUM_OF_CREDITSf, 32);
    SOC_IF_ERROR_RETURN(WRITE_IP_TO_CMICM_CREDIT_TRANSFERr(unit, val));

    if ((dev_id & 0x0060) == 0x0020) {
        /* Bloodhound: BCM5342x */
        mmu_total_cells = GH_MMU_BUFFER_SIZE_1MB / GH_MMU_CELL_SIZE;
    } else {
        mmu_total_cells = GH_MMU_BUFFER_SIZE_2MB / GH_MMU_CELL_SIZE;
        /* For SKU with 1MB buffer pool, use the following line instead */
        /*    mmu_total_cells = GH_MMU_BUFFER_SIZE_1MB / GH_MMU_CELL_SIZE; */
    }

    if ((dev_id == BCM53403_DEVICE_ID) || (dev_id == BCM53413_DEVICE_ID) || \
        (dev_id == BCM53404_DEVICE_ID) || (dev_id == BCM53414_DEVICE_ID) || \
        (dev_id == BCM53406_DEVICE_ID) || (dev_id == BCM53416_DEVICE_ID) || \
       (dev_id == BCM53408_DEVICE_ID) || (dev_id == BCM53418_DEVICE_ID) || \
       (dev_id == BCM53454_DEVICE_ID) || (dev_id == BCM53455_DEVICE_ID) || \
       (dev_id == BCM53426_DEVICE_ID) || (dev_id == BCM56065_DEVICE_ID) || \
       (dev_id == BCM56066_DEVICE_ID) ||
       ((dev_id == BCM53369_DEVICE_ID) && (config_op != 5))) {
        total_ports = 25;
    } else if ((dev_id == BCM56060_DEVICE_ID) || \
        (dev_id == BCM53405_DEVICE_ID) || (dev_id == BCM53415_DEVICE_ID) || \
        (dev_id == BCM53402_DEVICE_ID) || (dev_id == BCM53412_DEVICE_ID) || \
        (dev_id == BCM53422_DEVICE_ID) || (dev_id == BCM53365_DEVICE_ID) ||
        ((dev_id == BCM53369_DEVICE_ID) && (config_op == 5))) {
        total_ports = 17;
    } else {
        /* BCM56062,BCM56063,BCM56064,BCM53401,BCM53411,BCM53456,BCM53457,BCM53424 */
        total_ports = 29;
    }
    mmu_total_static_cells = total_ports * GH_MMU_IN_PORT_STATIC_CELLS;
    mmu_total_dyn_cells = mmu_total_cells - mmu_total_static_cells;

    /* total dynamic xqs - skid mark - 9(reserved xqs) */   
    dyn_xq_per_port = GH_MMU_IN_PORT_DYNAMIC_XQS -  skid_mark - 9;
    /* limit includes the static minimum cells as well */
    xq_limit = dyn_xq_per_port + GH_MMU_IN_COS_MIN_XQS;
    cos_min_xqs = GH_MMU_IN_COS_MIN_XQS;
    cos_min_cells = GH_MMU_IN_COS_MIN_CELLS;

    cell_resume_delta = 12;
    dyn_cell_reset_delta = 24;
    if ((dev_id == BCM53403_DEVICE_ID) || (dev_id == BCM53413_DEVICE_ID) || \
        (dev_id == BCM53404_DEVICE_ID) || (dev_id == BCM53414_DEVICE_ID) || \
        (dev_id == BCM53406_DEVICE_ID) || (dev_id == BCM53416_DEVICE_ID) || \
       (dev_id == BCM53408_DEVICE_ID) || (dev_id == BCM53418_DEVICE_ID) || \
       (dev_id == BCM53454_DEVICE_ID) || (dev_id == BCM53455_DEVICE_ID) || \
       (dev_id == BCM53426_DEVICE_ID) || (dev_id == BCM56065_DEVICE_ID) || \
       (dev_id == BCM56066_DEVICE_ID) ||
       ((dev_id == BCM53369_DEVICE_ID) && (config_op != 5))) {
        max_cell_limit_pri0 = 3456;
        max_cell_limit_pri1_7 = 48;
        xoff_pkt_limit = 80;
        reset_pkt_limit = 0;
    } else if ((dev_id == BCM56060_DEVICE_ID) || \
        (dev_id == BCM53405_DEVICE_ID) || (dev_id == BCM53415_DEVICE_ID) || \
        (dev_id == BCM53402_DEVICE_ID) || (dev_id == BCM53412_DEVICE_ID) || \
        (dev_id == BCM53422_DEVICE_ID) || (dev_id == BCM53365_DEVICE_ID) ||
        ((dev_id == BCM53369_DEVICE_ID) && (config_op == 5))) {
        max_cell_limit_pri0 = 4096;
        max_cell_limit_pri1_7 = 4096;
        xoff_pkt_limit = 80;
        reset_pkt_limit = 1;
        cell_resume_delta = 16;
        dyn_cell_reset_delta = 32;
    } else {
        /* BCM56062,BCM56063,BCM56064,BCM53401,BCM53411,BCM53456,BCM53457,BCM53424 */
        max_cell_limit_pri0 = 7776;
        max_cell_limit_pri1_7 = 48;
        xoff_pkt_limit = 12;
        reset_pkt_limit = 0;
    }
    xoff_cell_limit = 80;
    reset_cell_limit = 40;

    dyn_cell_limit = max_cell_limit_pri0 + \
                     ((GH_MMU_NUM_COS -1) * 4 * (1536 / GH_MMU_CELL_SIZE)); 

    PBMP_ALL_ITER(unit, port) {

        for (cos = 0; cos < NUM_COS(unit); cos++) {
            /* 
             * The HOLCOSPKTSETLIMITr register controls BOTH the XQ 
             * size per cosq AND the HOL set limit for that cosq.
             */
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTSETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &val,
                              PKTSETLIMITf, 
                              xq_limit);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, val));

            /* reset limit is set limit - 4 */
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTRESETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &val,
                              PKTRESETLIMITf, 
                              xq_limit-4);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTRESETLIMITr(unit, port, cos, val));

            val = 0;
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &val,
                              HOLCOSMINXQCNTf, cos_min_xqs);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSMINXQCNTr(unit, port, cos, val)); 

            val = 0;
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLSETLIMITf, cos_min_cells);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLRESETLIMITf, cos_min_cells);
            SOC_IF_ERROR_RETURN
                (WRITE_LWMCOSCELLSETLIMITr(unit, port, cos, val)); 
              
            cell_limit = 0;
            if(!cos) {
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                  CELLMAXLIMITf, max_cell_limit_pri0);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                  CELLMAXRESUMELIMITf, max_cell_limit_pri0 - cell_resume_delta);
            } else {
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                  CELLMAXLIMITf, max_cell_limit_pri1_7);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                  CELLMAXRESUMELIMITf, max_cell_limit_pri1_7 - cell_resume_delta);
            }
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSCELLMAXLIMITr(unit, port, cos, cell_limit)); 

            val = 0;
            soc_reg_field_set(unit, PGCELLLIMITr, &val,
                              CELLSETLIMITf, xoff_cell_limit);
            soc_reg_field_set(unit, PGCELLLIMITr, &val,
                              CELLRESETLIMITf, reset_cell_limit);
            SOC_IF_ERROR_RETURN
                (WRITE_PGCELLLIMITr(unit, port, cos, val)); 

            val = 0;
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &val,
                              DISCARDSETLIMITf, mmu_total_cells-1);
            SOC_IF_ERROR_RETURN
                (WRITE_PGDISCARDSETLIMITr(unit, port, cos, val)); 
        }

        val = 0;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &val,
                          DYNXQCNTPORTf, dyn_xq_per_port);

        SOC_IF_ERROR_RETURN(WRITE_DYNXQCNTPORTr(unit, port, val)); 

        val = 0;
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &val,
                          DYNRESETLIMPORTf, dyn_xq_per_port - 16);
        SOC_IF_ERROR_RETURN(WRITE_DYNRESETLIMPORTr(unit, port, val)); 

        val = 0;
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLSETLIMITf, dyn_cell_limit);
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLRESETLIMITf, dyn_cell_limit - 24);
        SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, val)); 

        cell_limit = 0;
        soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                          &cell_limit, TOTALDYNCELLSETLIMITf, mmu_total_dyn_cells);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, cell_limit));
        cell_limit = 0;
        soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                          &cell_limit, TOTALDYNCELLRESETLIMITf, mmu_total_dyn_cells - 
                          dyn_cell_reset_delta);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, cell_limit));

        val = 0;   
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val,   
                        PKTSETLIMITf, xoff_pkt_limit);   
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &val, RESETLIMITSELf, reset_pkt_limit);   
        SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, val)); 


        /* E2EFC configuration */
        if (IS_HG_PORT(unit, port)){
            SOC_IF_ERROR_RETURN(READ_IE2E_CONTROLr(unit, port, &val));
            soc_reg_field_set(unit, IE2E_CONTROLr, &val, IBP_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_IE2E_CONTROLr(unit, port, val));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit, 
                                PKT_SET_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit, 
                                CELL_SET_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_SET_LIMITr(unit, port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit, 
                                PKT_RESET_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit, 
                                CELL_RESET_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_RESET_LIMITr(unit, port, limit));
        
            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit, 
                                PKT_DISC_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit, 
                                CELL_DISC_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_DISC_LIMITr(unit, port, limit));
        } else {
            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit, 
                                PKT_SET_LIMITf, 12);
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit, 
                                CELL_SET_LIMITf, 80);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_SET_LIMITr(unit, port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit, 
                                PKT_RESET_LIMITf, 9);
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit, 
                                CELL_RESET_LIMITf, 60);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_RESET_LIMITr(unit, port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit, 
                                PKT_DISC_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit, 
                                CELL_DISC_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_DISC_LIMITr(unit, port, limit));
        }
    }

    /* E2EFC configuration */
    SOC_IF_ERROR_RETURN(READ_E2EFC_IBP_ENr(unit, &val));
    soc_reg_field_set(unit, E2EFC_IBP_ENr, &val, ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_E2EFC_IBP_ENr(unit, val));

    /* 
     * Configure per-XQ packet aging for the various COSQs. 
     *
     * The shortest age allowed by H/W is 250 microseconds.
     * The longest age allowed is 7.162 seconds (7162 msec).
     * The maximum ratio between the longest age and the shortest 
     * (nonzero) age is 7:2.
     */
    age_enable = disabled_age = max_age = 0;
    min_age = 7162;
    for (cos = 0; cos < NUM_COS(unit); cos++) {
        if ((age[cos] = 
             soc_property_suffix_num_get(unit, cos, spn_MMU_XQ_AGING,
                                         "cos",  0)) > 0) {
            age_enable = 1;
            if (age[cos] > 7162) {
                age[cos] = 7162;
            }
            if (age[cos] < min_age) {
                min_age = age[cos];
            }
        } else {
            disabled_age = 1;
            age[cos] = 0;
        }
        if (age[cos] > max_age) {
            max_age = age[cos];
        }
    }
    if (!age_enable) {
        /* Disable packet aging on all COSQs */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, 0));
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, 0)); /* Aesthetic */
    } else {
        uint32 regval = 0;
        uint32 timerval;

        /* Enforce the 7:2 ratio between min and max values */
        if ((((max_age * 2)+6) / 7) > min_age) {
            /* Keep requested max age; make min_age comply */ 
            min_age = ((max_age * 2) + 6) / 7; 
        }

        /* 
         * Give up granularity for range, if we need to
         * "disable" (max out) aging for any COSQ(s).
         */
        if (disabled_age) {
            /* Max range */
            max_age = min_age * 7 / 2;
        } 

        /* 
         * Compute shortest duration of one PKTAGINGTIMERr cycle. 
         * This duration is 1/7th of the longest packet age. 
         * This duration is in units of 125 usec (msec * 8).
         */
        timerval = ((8 * max_age) + 6) / 7; 
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, timerval));

        for (cos = 0; cos < NUM_COS(unit); cos++) {
            if (!age[cos]) {
                /* 
                 * Requested to be disabled, but cannot disable individual
                 * COSQs once packet aging is enabled. Therefore, mark
                 * this COSQ's aging duration as maxed out.
                 */
                age[cos] = -1;
            } else if (age[cos] < min_age) {
                age[cos] = min_age;
            }

            /* Normalize each "age" into # of PKTAGINGTIMERr cycles. */
        if (age[cos] > 0) {
            /* coverity[divide_by_zero : FALSE] */
            age[cos] = ((8 * age[cos]) + timerval - 1) / timerval;
        }
        else {
            age[cos] = 7;
        }
            /* Format each "age" for its appropriate field */
            regval |= ((7 - age[cos]) << (cos * 3));
        }
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, regval));
    }

    /*
     * Port enable
     */
    val = 0;
    soc_reg_field_set(unit, MMUPORTENABLEr, &val, MMUPORTENABLEf,
                      SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLEr(unit, val));

    return SOC_E_NONE;
}

/* _soc_gh_sbus_tsc_block()
 *  - to report the valid block of this port for wc_ucmem_data
 *      (i.e. XLPORT_WC_UCMEM_DATAm )access process.
 *
 *  - Parameters :
 *      1. phy_port(IN) : physical port.
 *      2. block(OUT) : the valid block
 * 
 *  Note :
 *  - the port in TSC0Q(xlport5) reports one more associated block type.
 */
int 
_soc_gh_sbus_tsc_block(int unit, 
            int phy_port, soc_mem_t wc_ucmem_data, int *block)
{
    int i,port_num_blktype = 0, tmp_blk=0;

    *block= -1;
    port_num_blktype = SOC_DRIVER(unit)->port_num_blktype;
    for (i = 0; i < port_num_blktype; i++){
        tmp_blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
        if ((SOC_MEM_INFO(unit, wc_ucmem_data).blocks & (1 << tmp_blk)) &&
            SOC_INFO(unit).block_valid[tmp_blk]) {
            *block = tmp_blk;
            break;
        }
    }

    if (*block == -1){
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "_soc_gh_sbus_tsc_block(u=%d,pp=%d,mem=%d, blk=%d): "
                             "no valid block for TSC SBUS interface!\n"), 
                  unit, phy_port, wc_ucmem_data, *block));
        return SOC_E_INTERNAL;
    }
    
    return SOC_E_NONE;
}

#if defined(SER_TR_TEST_SUPPORT)
soc_ser_test_functions_t ser_gh_test_fun;
#endif

extern int (*_phy_tsce_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);

STATIC int
_soc_greyhound_tsce_firmware_set(int unit, int port, uint8 *array, int datalen)
{
    /* TSC firmware set is handled by PortMod internally */
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        return SOC_E_INTERNAL;
    }

    return soc_warpcore_firmware_set(unit, port, array, datalen, 0,
                                     XLPORT_WC_UCMEM_DATAm,
                                     XLPORT_WC_UCMEM_CTRLr);
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

/* to check if this PHY register access sbus MDIO request is for 
 * QSGMII register. 
 */
STATIC int 
_soc_greyhound_qsgmii_reg_access_check(int unit, 
                            uint32 port, uint32 phyreg, int *to_qsgmii)
{
    *to_qsgmii = 0;
    if (PHY_REG_ADDR_DEVAD(phyreg) == 0){  /* DEVAD==0 for PCS */
        if (IS_QSGMII_REGISTER(phyreg)) {
            *to_qsgmii = 1;
        } else if ((QSGMII_REG_ADDR_REG(phyreg) < 0x10) &&
                    GH_PORT_IS_QSGMII(unit, port)) {
            /* means QSGMII port on accessing IEEE registers */
            *to_qsgmii = 1; 
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_indacc_addr(uint32 phy_reg, int *target_select)
{
    if (QSGMII_REG_ADDR_LANE(phy_reg) <= 15) {
        *target_select = (QSGMII_REG_ADDR_LANE(phy_reg) < 8) ? 0 : 1;
        return SOC_E_NONE;
    }
    
    return SOC_E_INTERNAL;
}

STATIC int
_soc_greyhound_indacc_wait(int unit, soc_field_t fwait)
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
_soc_greyhound_indacc_read(int unit, uint32 phy_addr,
                           uint32 phy_reg, uint32 *phy_data)
{
    uint32 ctlsts;
    int ts;
    uint32  reg_addr = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_indacc_read"
                          "(%d,0x%x,0x%08x,*phy_data)..\n"), 
               unit, phy_addr, phy_reg));

    SOC_IF_ERROR_RETURN(
        _soc_greyhound_indacc_addr(phy_reg, &ts));

    ctlsts = 0;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, TARGET_SELECTf, ts);
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, RD_REQf, 1);

    /* assigning the reg_addr in Clause22 format */
    reg_addr = phy_reg & 0x1f;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, ADDRESSf, reg_addr);

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_indacc_read:"
                          "INDACC_CTLSTSr=0x%08x\n"), ctlsts));
    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_CTLSTSr(unit, REG_PORT_ANY, ctlsts));

    SOC_IF_ERROR_RETURN(
        _soc_greyhound_indacc_wait(unit, RD_RDYf));

    SOC_IF_ERROR_RETURN(
        READ_CHIP_INDACC_RDATAr(unit, REG_PORT_ANY, phy_data));

    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_CTLSTSr(unit, REG_PORT_ANY, 0));

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_indacc_write(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 phy_data)
{
    uint32 ctlsts;
    int ts;
    uint32  reg_addr = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_indacc_write"
                          "(%d,0x%x,0x%08x,0x%04x)..\n"), 
               unit, phy_addr, phy_reg, phy_data));
    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_WDATAr(unit, REG_PORT_ANY, phy_data));

    SOC_IF_ERROR_RETURN(
        _soc_greyhound_indacc_addr(phy_reg, &ts));

    ctlsts = 0;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, TARGET_SELECTf, ts);
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, WR_REQf, 1);

    /* assigning the reg_addr in Clause22 format */
    reg_addr = phy_reg & 0x1f;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, ADDRESSf, reg_addr);

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_indacc_write:"
                          "INDACC_CTLSTSr=0x%08x\n"), ctlsts));
    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_CTLSTSr(unit, REG_PORT_ANY, ctlsts));

    SOC_IF_ERROR_RETURN(
        _soc_greyhound_indacc_wait(unit, WR_RDYf));

    SOC_IF_ERROR_RETURN(
        WRITE_CHIP_INDACC_CTLSTSr(unit, REG_PORT_ANY, 0));

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_sbus_qsgmii_read(int unit, 
                uint32 phy_addr, uint32 phy_reg, uint32 *phy_data)
{
    uint32  reg_addr, reg_data;

    *phy_data = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_sbus_qsgmii_read"
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
    SOC_IF_ERROR_RETURN(_soc_greyhound_indacc_write(unit, 
            phy_addr, reg_addr, reg_data));

    /* AER process : lane control */
    reg_addr = 0x1e;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    /* lane control to indicated lane 0~7 for each QSGMIMI core */
    reg_data = QSGMII_REG_ADDR_LANE(phy_reg) & 0x7;
    SOC_IF_ERROR_RETURN(_soc_greyhound_indacc_write(unit, 
            phy_addr, reg_addr, reg_data));

    /* target register block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = phy_reg & 0xfff0;
    SOC_IF_ERROR_RETURN(_soc_greyhound_indacc_write(unit, 
            phy_addr, reg_addr, reg_data));

    /* read data */
    reg_addr = QSGMII_REG_ADDR_REG(phy_reg);
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    SOC_IF_ERROR_RETURN(_soc_greyhound_indacc_read(unit, 
            phy_addr, reg_addr, &reg_data));

    *phy_data = reg_data;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_sbus_qsgmii_read()...data=0x%04x\n"),
               *phy_data));

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_sbus_qsgmii_write(int unit, 
                uint32 phy_addr, uint32 phy_reg, uint32 phy_data)
{
    uint32  reg_addr, reg_data;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_sbus_qsgmii_write"
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
    SOC_IF_ERROR_RETURN(_soc_greyhound_indacc_write(unit, 
            phy_addr, reg_addr, reg_data));

    /* AER process : lane control */
    reg_addr = 0x1e;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    /* lane control to indicated lane 0~7 for each QSGMIMI core */
    reg_data = QSGMII_REG_ADDR_LANE(phy_reg) & 0x7;
    SOC_IF_ERROR_RETURN(_soc_greyhound_indacc_write(unit, 
            phy_addr, reg_addr, reg_data));

    /* target register block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = phy_reg & 0xfff0;
    SOC_IF_ERROR_RETURN(_soc_greyhound_indacc_write(unit, 
            phy_addr, reg_addr, reg_data));

    /* write data */
    reg_addr = QSGMII_REG_ADDR_REG(phy_reg);
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    SOC_IF_ERROR_RETURN(_soc_greyhound_indacc_write(unit, 
            phy_addr, reg_addr, phy_data));

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_sbus_qsgmii_write()... Done!")));

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_mdio_addr_to_port(uint32 phy_addr)
{
    if (PHY_ID_BUS_NUM(phy_addr) == 1) {
        return (phy_addr & 0x1f) + 17;
    } else {
        return (phy_addr & 0x1f) + 1;
    }
}

STATIC int
_soc_greyhound_mdio_reg_read(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 *phy_data)
{
    int rv;
    int port, phy_port, blk;
    int qsgmii_reg = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_mdio_reg_read(%d,0x%x,0x%08x,*phy_data)..\n"), 
               unit, phy_addr, phy_reg));
    /* Physical port based on MDIO address */
    phy_port = _soc_greyhound_mdio_addr_to_port(phy_addr);
    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    SOC_IF_ERROR_RETURN(_soc_gh_sbus_tsc_block(unit, 
                        phy_port, XLPORT_WC_UCMEM_DATAm, &blk));

    /* access interface selection */
    SOC_IF_ERROR_RETURN(_soc_greyhound_qsgmii_reg_access_check(unit, 
                        port, phy_reg, &qsgmii_reg));

    if (qsgmii_reg) {
        MIIM_LOCK(unit);
        SOC_ALLOW_WB_WRITE(unit, _soc_greyhound_sbus_qsgmii_read(unit, phy_addr,
                                        phy_reg, phy_data), rv);
        MIIM_UNLOCK(unit);
    } else {
        TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
        rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr, 
                                   phy_reg, phy_data);
    }
    return rv;
}

STATIC int
_soc_greyhound_mdio_reg_write(int unit, uint32 phy_addr,
                              uint32 phy_reg, uint32 phy_data)
{
    int rv;
    int port, phy_port, blk;
    int qsgmii_reg = 0;

    LOG_DEBUG(BSL_LS_SOC_MII,
              (BSL_META_U(unit,
                          "_soc_greyhound_mdio_reg_write(%d,0x%x,0x%08x,0x%04x)..\n"), 
               unit, phy_addr, phy_reg,phy_data));
    /* Physical port based on MDIO address */
    phy_port = _soc_greyhound_mdio_addr_to_port(phy_addr);
    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    SOC_IF_ERROR_RETURN(_soc_gh_sbus_tsc_block(unit, 
                        phy_port, XLPORT_WC_UCMEM_DATAm, &blk));

    /* access interface selection */
    SOC_IF_ERROR_RETURN(_soc_greyhound_qsgmii_reg_access_check(unit, 
                        port, phy_reg, &qsgmii_reg));

    if (qsgmii_reg) {
        MIIM_LOCK(unit);
        rv = _soc_greyhound_sbus_qsgmii_write(unit, phy_addr, 
                                        phy_reg, phy_data);
        MIIM_UNLOCK(unit);
    } else {
        TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
        rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr, 
                                    phy_reg, phy_data);
    }
    return rv;
}

#define _gh_get_pval(_map_, _idx_, _max_num_) \
    ((_idx_ < _max_num_) ? _map_[_idx_]: 0)

/* The number of ports per TSCQ */
#define _GH_PORTS_NUM_PER_TSCQ      (16)
/* The number of ports per TSC */
#define _GH_PORTS_NUM_PER_TSC       (4)
/* The first physcial port number of TSC0Q */
#define _GH_FIRST_PHY_PORT_TSC0Q    (2)
/* The first physcial port number of TSC0 */
#define _GH_FIRST_PHY_PORT_TSC0     (_GH_FIRST_PHY_PORT_TSC0Q + _GH_PORTS_NUM_PER_TSCQ)

STATIC int
_greyhound_ledup_init(int unit)
{
    int i, j, pval1, pval2, pval3, pval4, port_idx, is_qmode = 0;
    uint32 led_remap[64] = {0}, led_cnt = 0, rval = 0, disable_4x10 = 0, disable_4x10q = 0;
    int freq, cycles;
    int refresh_rate = 30, refresh_period;

    struct led_remap_t {
       uint32 reg_addr;
       uint32 port0;
       uint32 port1;
       uint32 port2;
       uint32 port3;
    } ledup0_remap_r[] = {
        {CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r,
         REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r,
         REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r,
         REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f}
    };

    /* Program the LED clock output frequency based on core clock */
    freq = SOC_INFO(unit).frequency;

    /* For LEDCLK_HALF_PERIOD */
    /* Formula : cycles = (freq * 10^6) * (100 * 10^-9) = freq/10 */
    /* Round up the value for non-integer clocks */
    cycles = (freq + 9) / 10 - 1;

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_CLK_DIVr, &rval,
                      LEDCLK_HALF_PERIODf, cycles);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_CLK_DIVr(unit, rval));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_CLK_DIVr(unit, rval));

    /* For REFRESH_CYCLE_PERIOD */
    /* Formula : refresh_period = (freq * 10^6) / 30 */
    /* refresh_rate = 30Hz */
    refresh_period = freq * 1000000 / refresh_rate;

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_CLK_PARAMSr, &rval,
                      REFRESH_CYCLE_PERIODf, refresh_period);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_CLK_PARAMSr(unit, rval));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_CLK_PARAMSr(unit, rval));

    /* The LED scanning out order is
     *  TSC4(37->34) -> TSC3(33->30) -> TSC2(29->26) -> TSC1(25->22) -> TSC0(21->18) -> TSC0Q(2->17(QSGMII)/5->2(Non-QSGMII))
     *  If a TSC is ON, it will output 4 bytes LED status to the LED chain
     *  If a TSCQ is ON, it will output 4 bytes for Non-QSGMII mode, or 16 bytes for QSGMII mode.
     *  If a TSC/TSCQ is OFF, it will output 1 byte, bit[7:0] are all zero, to the LED chain
     */

    /* Construct the LED reamp by TSC status */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_PGW_CTRL_0r(unit, &rval));
    disable_4x10 = soc_reg_field_get(unit, PGW_CTRL_0r, rval, SW_PM4X10_DISABLEf);
    disable_4x10q = soc_reg_field_get(unit, PGW_CTRL_0r, rval, SW_PM4X10Q_DISABLEf);

    if (!disable_4x10q) {
        SOC_IF_ERROR_RETURN(READ_CHIP_CONFIGr(unit, REG_PORT_ANY, &rval));
        is_qmode = soc_reg_field_get(unit, CHIP_CONFIGr, rval, QMODEf);
    }

    /* TSC4 LED data is shifted out first and TSC0 is the last */
    for (i = 4; i >= 0; i--) {
        if (disable_4x10 & (1 << i)) {
            led_remap[led_cnt++] = 0;
        } else {
            /* For each TSC, the status of port 3 is shifted out first and port 0 is the last */
            for (j = 3; j >= 0; j--) {
                led_remap[led_cnt++] = _GH_FIRST_PHY_PORT_TSC0 + _GH_PORTS_NUM_PER_TSC * i + j;
            }
        }
    }

    /* TSCQ LED data is shifted out after TSCs */
    if (disable_4x10q) {
        led_remap[led_cnt++] = 0;
    } else {
        if (is_qmode) {
            /* For QSGMII mode, the status of port 0 is shifted out first and port 15 is the last */
            for (i = 0; i <= 15; i++) {
                led_remap[led_cnt++] = _GH_FIRST_PHY_PORT_TSC0Q + i;
            }
        } else {
            /* For Non-QSGMII mode, the status of port 3 is shifted out first and port 0 is the last */
            for (i = 3; i >= 0; i--) {
                led_remap[led_cnt++] = _GH_FIRST_PHY_PORT_TSC0Q + i;
            }
        }
    }

    /* Setup the led remap registers */
    for (i = 0; i < sizeof(ledup0_remap_r)/sizeof(ledup0_remap_r[0]); i++) {
        port_idx = i * 4;
        pval1 = _gh_get_pval(led_remap, port_idx, led_cnt);
        pval2 = _gh_get_pval(led_remap, port_idx + 1, led_cnt);
        pval3 = _gh_get_pval(led_remap, port_idx + 2, led_cnt);
        pval4 = _gh_get_pval(led_remap, port_idx + 3, led_cnt);

        rval = 0;
        soc_reg_field_set(unit, ledup0_remap_r[i].reg_addr, &rval,
                         ledup0_remap_r[i].port0, pval1);
        soc_reg_field_set(unit, ledup0_remap_r[i].reg_addr, &rval,
                         ledup0_remap_r[i].port1, pval2);
        soc_reg_field_set(unit, ledup0_remap_r[i].reg_addr, &rval,
                         ledup0_remap_r[i].port2, pval3);
        soc_reg_field_set(unit, ledup0_remap_r[i].reg_addr, &rval,
                         ledup0_remap_r[i].port3, pval4);
        SOC_IF_ERROR_RETURN
            (soc_pci_write(unit,
                soc_reg_addr(unit, ledup0_remap_r[i].reg_addr, REG_PORT_ANY, 0),
                rval));
    }

    /* Initialize the UP0 data ram */
    rval = 0;
    for (i = 0; i < 256; i++) {
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_DATA_RAMr(unit, i, rval));
    }

    /* Apply LEDUP delays. */
    SOC_IF_ERROR_RETURN(READ_AMAC_IDM0_IO_CONTROL_DIRECTr(unit, &rval));

    /* GH-1651. For QSGMII mode, set LEDSCAN_FLOP_STAGESf to 0x3. O/W, set to 0x4 */
    if (is_qmode) {
        soc_reg_field_set(unit, AMAC_IDM0_IO_CONTROL_DIRECTr,
                       &rval, LEDSCAN_FLOP_STAGESf, 0x3);
    } else {
        soc_reg_field_set(unit, AMAC_IDM0_IO_CONTROL_DIRECTr,
                       &rval, LEDSCAN_FLOP_STAGESf, 0x4);
    }
    SOC_IF_ERROR_RETURN(WRITE_AMAC_IDM0_IO_CONTROL_DIRECTr(unit, rval));

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_qsgmii_mode_check(int unit, int *qsgmii_mode)
{
    uint16  dev_id;
    uint8   rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((dev_id & 0xF000) == 0x8000) {
        if ((dev_id & 0x0060) != 0x0000) { /* Elkhound and Bloodhound */
            if ((dev_id & 0x00F6) == 0x0054) { /* 53454/53455 */
                dev_id &= 0xFFF4; /* use 53454 */
            }
            if ((dev_id & 0x00F6) == 0x0056) { /* 53456/53457 */
                dev_id &= 0xFFF6; /* use 53456 */
            }
             /* no change on Bloodhound */
        } else {
            dev_id &= 0xFF0F;
        }
        /* no change on Bloodhound */
    }
    if (dev_id == BCM56064_DEVICE_ID || dev_id == BCM56063_DEVICE_ID || 
        dev_id == BCM53401_DEVICE_ID || dev_id == BCM56062_DEVICE_ID ||
        dev_id == BCM53456_DEVICE_ID || dev_id == BCM53424_DEVICE_ID) {
        *qsgmii_mode = 1;
    }    
    return SOC_E_NONE;
}

STATIC int
soc_greyhound_freq_get(int unit, int *frequency, int *change_freq){

    uint16              dev_id;
    uint8               rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (((dev_id & 0xF000) == 0x8000) &&
        (dev_id != BCM53365_DEVICE_ID)){
        if ((dev_id & 0x0060) != 0x0000) { /* Elkhound and Bloodhound */
            if ((dev_id & 0x00F6) == 0x0054) { /* 53454/53455 */
                dev_id &= 0xFFF4; /* use 53454 */
            }
            if ((dev_id & 0x00F6) == 0x0056) { /* 53456/53457 */
                dev_id &= 0xFFF6; /* use 53456*/
            }
            /* no change on Bloodhound */
        } else {
            dev_id &= 0xFF0F;
        }
    }
    *change_freq = 0;

    switch (dev_id) {
        case BCM56064_DEVICE_ID:
        case BCM56063_DEVICE_ID:
        case BCM53408_DEVICE_ID:
        case BCM53402_DEVICE_ID:
        case BCM53401_DEVICE_ID:
        case BCM53422_DEVICE_ID:
        case BCM53424_DEVICE_ID:
        case BCM53426_DEVICE_ID:
        case BCM56066_DEVICE_ID:
            *frequency = 176;
            break;
        case BCM53403_DEVICE_ID:
        case BCM53404_DEVICE_ID:
            *frequency = 176;
            *change_freq = 1;
            break;
        case BCM56060_DEVICE_ID:
        case BCM53405_DEVICE_ID:
        case BCM53406_DEVICE_ID:
        case BCM56062_DEVICE_ID:
        case BCM53454_DEVICE_ID:
        case BCM53456_DEVICE_ID:
        case BCM56065_DEVICE_ID:
        case BCM53365_DEVICE_ID:
        case BCM53369_DEVICE_ID:
            *frequency = 300;
            break;
        default:
            *frequency = 0;
            break;
    };
    return SOC_E_NONE;
}


STATIC int
_soc_greyhound_misc_init(int unit)
{
#define NUM_XLPORT 4
    static const soc_field_t port_field[] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    uint32              rval, fval; 
    uint32              prev_reg_addr;
    uint64              reg64;
    int                 port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t         fields[3];
    uint32              values[3];
    int blk, bindex, mode;
    int phy_port_base;
    soc_info_t *si = &SOC_INFO(unit);
    int parity_enable, blk_port, phy_port, delay;
    int freq, target_freq, divisor, dividend;
    char *board_name;
    int temp;

#ifdef INCLUDE_MEM_SCAN
    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
#endif /* INCLUDE_MEM_SCAN */

    
    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_greyhound_pipe_mem_clear(unit));

        /* Reset XLPORT MIB counter (registers implemented in memory). The clear
         * function is implemented with read-modify-write, parity needs to be
         * disabled */
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
            blk_port = SOC_BLOCK_PORT(unit, blk);
            if (blk_port < 0) {
                continue;
            }
            phy_port = si->port_l2p_mapping[blk_port];
            fval = 0;
            for (bindex = 0; bindex < 4; bindex++) {
                if (si->port_p2l_mapping[phy_port + bindex] != -1) {
                    fval |= 1 << bindex;
                }
            }
            rval = 0;
            soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, fval);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, blk_port, rval));
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, blk_port, 0));
        }
    }

    SOC_IF_ERROR_RETURN(soc_greyhound_init_port_mapping(unit));


    /* Turn on ingress/egress/mmu parity */
    _soc_gh_tcam_ser_info[unit] = NULL;
    _soc_gh_parity_group_info[unit] = NULL;
    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (soc_feature(unit, soc_feature_field_multi_stage)) {
        _soc_gh_parity_group_info[unit] = _soc_rg2_parity_group_info_template;
    } else {
        _soc_gh_parity_group_info[unit] = _soc_gh_parity_group_info_template;
    }
    if (parity_enable) {
        (void)_soc_greyhound_tcam_ser_init(unit);        
#if defined(SER_TR_TEST_SUPPORT)
        soc_gh_ser_test_register(unit);
#endif /*defined(SER_TR_TEST_SUPPORT*/
#ifdef INCLUDE_MEM_SCAN
        soc_mem_scan_ser_list_register(unit, TRUE, _soc_gh_tcam_ser_info[unit]);
#endif /* INCLUDE_MEM_SCAN */
         memset(&_gh_ser_functions[unit], 0, sizeof(soc_ser_functions_t));
        _gh_ser_functions[unit]._soc_ser_fail_f = &soc_greyhound_ser_fail;
        _gh_ser_functions[unit]._soc_ser_parity_error_cmicm_intr_f = 
            &soc_greyhound_parity_error;
        _gh_ser_functions[unit]._soc_ser_mem_nack_f = 
            &soc_greyhound_mem_nack;
        soc_ser_function_register(unit, &_gh_ser_functions[unit]);
#if GREYHOUND_SER_PARITY_ENABLED
        /* Enabling the packet drop when parity error occurred */
        SOC_IF_ERROR_RETURN(READ_IPIPE_PERR_CONTROLr(unit, &rval));
        fval = 1;
        soc_reg_field_set(unit, IPIPE_PERR_CONTROLr, &rval, DROPf, fval);
        SOC_IF_ERROR_RETURN(WRITE_IPIPE_PERR_CONTROLr(unit, rval));
       (void)_soc_greyhound_parity_enable(unit, TRUE);
#endif /* GREYHOUND_SER_PARITY_ENABLED */
    } else {
#if GREYHOUND_SER_PARITY_ENABLED  
       (void)_soc_greyhound_parity_enable(unit, FALSE); 
#endif /* GREYHOUND_SER_PARITY_ENABLED */
    }

    /* set QMODE enable for TSC0Q at QSGMII mode before unimac init process 
     *  - GH's QSGMII in TSC0Q only and check proper SKU to enable QMODE. 
     */
    
    SOC_IF_ERROR_RETURN(READ_PGW_CTRL_0r(unit, &rval));
    fval = soc_reg_field_get(unit, PGW_CTRL_0r, rval, SW_PM4X10Q_DISABLEf);
    /* ensure the TSC0Q is enabled */
    if (fval == 0) {
        int qsgmii_mode = 0;
        (void)_soc_greyhound_qsgmii_mode_check(unit, &qsgmii_mode);        
        SOC_IF_ERROR_RETURN(READ_CHIP_CONFIGr(unit, REG_PORT_ANY, &rval));
        fval = 0;
        /* only 56064|56063|53401(also 53411)|56062|53456(also 53457)|53424 
            support TSC0Q at QSGMII mode */
        if (qsgmii_mode == 1){
            fval = 1;
        }
        soc_reg_field_set(unit, CHIP_CONFIGr, &rval, QMODEf, fval);
        SOC_IF_ERROR_RETURN(WRITE_CHIP_CONFIGr(unit, REG_PORT_ANY, rval));
    }

    /* GMAC init should be moved to mac */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);
    prev_reg_addr = 0xffffffff;
    PBMP_E_ITER(unit, port) {
        uint32  reg_addr;
        if (IS_XL_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }
    prev_reg_addr = 0xffffffff;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_E_ITER(unit, port) {
        uint32  reg_addr;
        if (IS_XL_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }

    PBMP_PORT_ITER(unit, port) {
        if (!IS_HG_PORT(unit, port)) {
            continue;
        }

        /* Section below is used to config XLPORT and PGW related HiGig encap 
         * setting for normal init process.
         */
        SOC_IF_ERROR_RETURN(READ_XLPORT_CONFIGr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_CONFIGr(unit, port, rval));

        /* only HG ports allows PGW encap setting below */
        SOC_IF_ERROR_RETURN(soc_greyhound_pgw_encap_field_modify(unit, 
                port, HIGIG_MODEf, 1));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        int lane_mode, i;
        port = SOC_BLOCK_PORT(unit, blk);
        if (port == -1 || port == REG_PORT_ANY) {
            continue;
        }
        phy_port_base = si->port_l2p_mapping[port];


        lane_mode = -1;

        for(i=0; i <_GH_MAX_TSC_COUNT ; i++){
            if(_gh_tsc[i].phy_port_base == phy_port_base){
                lane_mode = _gh_tsc[i].lane_mode;
                break;
            }
        }
        if (lane_mode == _GH_TSC_LANE_MODE_SINGLE) {
            mode = SOC_GH_PORT_MODE_QUAD;
            si->port_num_lanes[port] = 1;          
            si->port_num_lanes[port+1] = 1;
            si->port_num_lanes[port+2] = 1;
            si->port_num_lanes[port+3] = 1;
        } else if (lane_mode == _GH_TSC_LANE_MODE_XAUI) {
            mode = SOC_GH_PORT_MODE_SINGLE;
            si->port_num_lanes[port] = 4;
        } else if (lane_mode == _GH_TSC_LANE_MODE_RXAUI){
            mode = SOC_GH_PORT_MODE_DUAL;
            si->port_num_lanes[port] = 2;
            /* logical port_id in this case will not in serial list */
            si->port_num_lanes[port+2]= 2;
        } else if (lane_mode == _GH_TSC_LANE_MODE_RXAUI13G){
            mode = SOC_GH_PORT_MODE_DUAL;
            si->port_num_lanes[port] = 2;
            si->port_num_lanes[port+1]= 2;
        } else {
            mode = SOC_GH_PORT_MODE_QUAD;
            si->port_num_lanes[port] = 1;
            si->port_num_lanes[port+1] = 1;
            si->port_num_lanes[port+2] = 1;
            si->port_num_lanes[port+3] = 1;            
        }
        rval = 0;
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                          XPORT0_CORE_PORT_MODEf, mode);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                          XPORT0_PHY_PORT_MODEf, mode);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

        /* Enable XLPORT */
        rval = 0;
        for (bindex = 0; bindex < NUM_XLPORT; bindex++) {
            if (si->port_p2l_mapping[phy_port_base + bindex] != -1) {
                soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval,
                                  port_field[bindex], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, port, rval));
    }

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    /* Enable dual hash on L2 and L3 tables */
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

    /*
     * Egress Enable
     */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, SOC_INFO(unit).port_l2p_mapping[port], entry));
    }

    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, EPC_LINK_BMAP_64r, &reg64, PORT_BITMAP_LOf,
                          SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAP_64r(unit, reg64));

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

    SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, RING_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));
   
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

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT)));

    /* Setup SW2_FP_DST_ACTION_CONTROL */
    fields[0] = HGTRUNK_RES_ENf;
    fields[1] = LAG_RES_ENf;
    values[0] = values[1] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, SW2_FP_DST_ACTION_CONTROLr,
                                 REG_PORT_ANY, 2, fields, values));

    freq = si->frequency;
    if (freq == 0) {
        (void)soc_greyhound_freq_get(unit, &si->frequency, &temp);
        freq = si->frequency;
    }

    /*
     * Set external MDIO freq to around 10MHz
     * target_freq = core_clock_freq * DIVIDEND / DIVISOR / 2
     */
    if (!SOC_WARM_BOOT(unit)) {
        target_freq = 10;
        divisor = (freq + (target_freq * 2 - 1)) / (target_freq * 2);
        divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, divisor);
        dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND, 1);

        rval = 0;
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVIDENDf, dividend);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));
    }

    /*
     * Set internal MDIO freq to around 10MHz
     * Valid range is from 2.5MHz to 12.5MHz
     * target_freq = core_clock_freq * DIVIDEND / DIVISOR / 2
     * or
     * DIVISOR = core_clock_freq * DIVIDENT / (target_freq * 2)
     */
    target_freq = 10;
    divisor = (freq + (target_freq * 2 - 1)) / (target_freq * 2);
    divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, divisor);
    dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 1);
    rval = 0;
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf,
                       divisor);
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf,
                       dividend);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    delay = soc_property_get(unit, spn_MDIO_OUTPUT_DELAY, -1);
    if (delay >= 1  && delay <= 15) {
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_MIIM_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_MIIM_CONFIGr, &rval, MDIO_OUT_DELAYf,
                          delay);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_MIIM_CONFIGr(unit, rval));
    }

    /* Directed Mirroring ON by default except for CPU port */
    /* The src_port info will be changed as egress port if EM_SRCMOD_CHANGEf = 1 */
    PBMP_PORT_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(READ_EGR_PORT_64r(unit, port, &reg64));
        soc_reg64_field32_set(unit, EGR_PORT_64r, &reg64, EM_SRCMOD_CHANGEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_PORT_64r(unit, port, reg64));
        SOC_IF_ERROR_RETURN(READ_IEGR_PORT_64r(unit, port, &reg64));
        soc_reg64_field32_set(unit, IEGR_PORT_64r, &reg64, EM_SRCMOD_CHANGEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IEGR_PORT_64r(unit, port, reg64));
    }

    _phy_tsce_firmware_set_helper[unit] = _soc_greyhound_tsce_firmware_set;

    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "unit %d : MSPI Init Failed\n"), unit)); 
    }

    /* I2C set to Master Mode through Override Strap */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_OVERRIDE_STRAPr(unit, &rval));
    soc_reg_field_set(unit, CMIC_OVERRIDE_STRAPr, &rval, ENABLE_OVERRIDE_I2C_MASTER_SLAVE_MODEf, 1);
    soc_reg_field_set(unit, CMIC_OVERRIDE_STRAPr, &rval, I2C_MASTER_SLAVE_MODEf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_OVERRIDE_STRAPr(unit, rval));

    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_greyhound_ledup_init(unit));
    }

    /* Add board properity which can be used in rc.soc */
    board_name = soc_property_get_str(unit, spn_BOARD_NAME);
    if (board_name &&
        ((sal_strcmp(board_name, "BCM953411K") == 0) ||
         (sal_strcmp(board_name, "BCM953411R") == 0))) {
        if (soc_mem_config_set) {
            soc_mem_config_set("bcm953411", "1");
        } else {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : soc_mem_config_set() is NULL\n"), unit));
        }
    }

#ifdef INCLUDE_MEM_SCAN
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
    }
#endif /* INCLUDE_MEM_SCAN */    

    return SOC_E_NONE;
}



/* soc_greyhound_mem_config:
 * Over-ride the default table sizes (from regsfile) for any SKUs here
 */
int
soc_greyhound_mem_config(int unit, int dev_id)
{
    int rv = SOC_E_NONE;
    soc_persist_t *sop = SOC_PERSIST(unit);

    switch (dev_id) {
        case BCM53400_DEVICE_ID:
        case BCM53401_DEVICE_ID:
        case BCM53402_DEVICE_ID:
        case BCM53403_DEVICE_ID:
        case BCM53404_DEVICE_ID:
        case BCM53405_DEVICE_ID:
        case BCM53406_DEVICE_ID:
        case BCM53408_DEVICE_ID:
        case BCM53454_DEVICE_ID:
        case BCM53456_DEVICE_ID:
        case BCM53369_DEVICE_ID:
            sop->memState[L2MCm].index_max = 511;
            sop->memState[L3_IPMCm].index_max = 63;
            sop->memState[L3_DEFIPm].index_max = 63;
            sop->memState[L3_DEFIP_ONLYm].index_max = 63;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 63;
            sop->memState[ING_L3_NEXT_HOPm].index_max = 767;
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 767;
            sop->memState[EGR_L3_NEXT_HOPm].index_max = 767;
            sop->memState[EGR_L3_INTFm].index_max = 255;
            sop->memState[EFP_COUNTER_TABLEm].index_max = -1;
            sop->memState[EFP_METER_TABLEm].index_max = -1;
            sop->memState[EFP_POLICY_TABLEm].index_max = -1;
            sop->memState[EFP_TCAMm].index_max = -1;
            sop->memState[VFP_POLICY_TABLEm].index_max = -1;
            sop->memState[VFP_TCAMm].index_max = -1;
            sop->memState[L3_ECMP_COUNTm].index_max = -1;
            sop->memState[L3_ECMPm].index_max = -1;
            sop->memState[INITIAL_L3_ECMP_GROUPm].index_max = -1;
            sop->memState[INITIAL_L3_ECMPm].index_max = -1;
            SOC_CONTROL(unit)->l3_defip_max_tcams = 1;
            SOC_CONTROL(unit)->l3_defip_tcam_size = 64;
            break;
        case BCM53422_DEVICE_ID:
        case BCM53424_DEVICE_ID:
        case BCM53426_DEVICE_ID:
            sop->memState[L2MCm].index_max = 511;
            sop->memState[L3_IPMCm].index_max = -1;
            sop->memState[L3_DEFIPm].index_max = -1;
            sop->memState[L3_DEFIP_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max = -1;
            sop->memState[ING_L3_NEXT_HOPm].index_max = -1;
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = -1;
            sop->memState[EGR_L3_NEXT_HOPm].index_max = -1;
            sop->memState[EGR_L3_INTFm].index_max = -1;
            sop->memState[L3_ENTRY_HIT_ONLYm].index_max = -1;
            sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = -1;
            sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = -1;
            sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = -1;
            sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = -1;
            sop->memState[L3_ENTRY_ONLYm].index_max = -1;
            sop->memState[L3_ENTRY_VALID_ONLYm].index_max = -1;
            sop->memState[L3_MTU_VALUESm].index_max = -1;
            sop->memState[EFP_COUNTER_TABLEm].index_max = -1;
            sop->memState[EFP_METER_TABLEm].index_max = -1;
            sop->memState[EFP_POLICY_TABLEm].index_max = -1;
            sop->memState[EFP_TCAMm].index_max = -1;
            sop->memState[VFP_POLICY_TABLEm].index_max = -1;
            sop->memState[VFP_TCAMm].index_max = -1;
            sop->memState[L3_ECMP_COUNTm].index_max = -1;
            sop->memState[L3_ECMPm].index_max = -1;
            sop->memState[INITIAL_L3_ECMP_GROUPm].index_max = -1;
            sop->memState[INITIAL_L3_ECMPm].index_max = -1;
            sop->memState[FP_TCAMm].index_max = 1023;
            sop->memState[FP_GLOBAL_MASK_TCAMm].index_max = 1023;
            sop->memState[FP_COUNTER_TABLEm].index_max = 1023;
            sop->memState[FP_METER_TABLEm].index_max = 1023;
            sop->memState[FP_POLICY_TABLEm].index_max = 1023;
            sop->memState[MMU_CBPCELLHEADERm].index_max = 8191;
            sop->memState[MMU_CBPDATA0m].index_max = 8191;
            sop->memState[MMU_CBPDATA1m].index_max = 8191;
            sop->memState[MMU_CBPDATA2m].index_max = 8191;
            sop->memState[MMU_CBPDATA3m].index_max = 8191;
            sop->memState[MMU_CBPDATA4m].index_max = 8191;
            sop->memState[MMU_CBPDATA5m].index_max = 8191;
            sop->memState[MMU_CBPDATA6m].index_max = 8191;
            sop->memState[MMU_CBPDATA7m].index_max = 8191;
            sop->memState[MMU_CBPPKTHEADER0m].index_max = 8191;
            sop->memState[MMU_CBPPKTHEADER1m].index_max = 8191;
            sop->memState[MMU_CBPPKTHEADER2m].index_max = 8191;
            sop->memState[MMU_CBPPKTHEADER_EXTm].index_max = 8191;
            SOC_CONTROL(unit)->l3_defip_max_tcams = 1;
            SOC_CONTROL(unit)->l3_defip_tcam_size = 64;
            break;
        case BCM53365_DEVICE_ID:
            sop->memState[L2MCm].index_max = 511;
            /* no L3, VFP, EFP */
            sop->memState[L3_IPMCm].index_max = -1;
            sop->memState[L3_DEFIPm].index_max = -1;
            sop->memState[L3_DEFIP_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max = -1;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max = -1;
            sop->memState[ING_L3_NEXT_HOPm].index_max = -1;
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = -1;
            sop->memState[EGR_L3_NEXT_HOPm].index_max = -1;
            sop->memState[EGR_L3_INTFm].index_max = -1;
            sop->memState[L3_ENTRY_HIT_ONLYm].index_max = -1;
            sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = -1;
            sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = -1;
            sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = -1;
            sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = -1;
            sop->memState[L3_ENTRY_ONLYm].index_max = -1;
            sop->memState[L3_ENTRY_VALID_ONLYm].index_max = -1;
            sop->memState[L3_MTU_VALUESm].index_max = -1;
            sop->memState[L3_ECMP_COUNTm].index_max = -1;
            sop->memState[L3_ECMPm].index_max = -1;
            sop->memState[INITIAL_L3_ECMP_GROUPm].index_max = -1;
            sop->memState[INITIAL_L3_ECMPm].index_max = -1;
            sop->memState[EFP_COUNTER_TABLEm].index_max = -1;
            sop->memState[EFP_METER_TABLEm].index_max = -1;
            sop->memState[EFP_POLICY_TABLEm].index_max = -1;
            sop->memState[EFP_TCAMm].index_max = -1;
            sop->memState[VFP_POLICY_TABLEm].index_max = -1;
            sop->memState[VFP_TCAMm].index_max = -1;
            break;
        default:
            sop->memState[ING_L3_NEXT_HOPm].index_max = 1535;
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 1535;
            sop->memState[EGR_L3_NEXT_HOPm].index_max = 1535;
            SOC_CONTROL(unit)->l3_defip_max_tcams = 8;
            SOC_CONTROL(unit)->l3_defip_tcam_size = 64;
            break;
    }
    if (SAL_BOOT_QUICKTURN) {
        /* QuickTurn with limited TCAM entries */
        sop->memState[L2_USER_ENTRYm].index_max = 15;
        sop->memState[L2_USER_ENTRY_ONLYm].index_max = 15;
        sop->memState[CPU_COS_MAPm].index_max = 15;
        sop->memState[CPU_COS_MAP_ONLYm].index_max = 15;
        sop->memState[FP_GLOBAL_MASK_TCAMm].index_max = 15;
        sop->memState[VLAN_SUBNETm].index_max = 15;
        sop->memState[VLAN_SUBNET_ONLYm].index_max = 15;
        sop->memState[VLAN_SUBNET_ONLYm].index_max = 15;
        sop->memState[L3_DEFIPm].index_max = 15;
        sop->memState[L3_DEFIP_ONLYm].index_max = 15;
        sop->memState[VFP_TCAMm].index_max = 15;
        sop->memState[EFP_TCAMm].index_max = 15;
        sop->memState[FP_TCAMm].index_max = 15;
    }
    return rv;
}

/*
 * Function:
 *  _soc_greyhound_gpio_set
 * Purpose:
 *  Write value to the specified GPIO pin
 */
STATIC int
_soc_greyhound_gpio_set(int unit, int pin, int val)
{
    uint32 rval = 0;
    uint32 fval = 0;
    uint8 mask = 0xFF;

    mask &= ~(1 << pin);

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_GP_OUT_ENr(unit, &rval));
    fval = soc_reg_field_get(unit, CMIC_GP_OUT_ENr, rval, OUT_ENABLEf);
    fval |= 1 << pin;
    soc_reg_field_set(unit, CMIC_GP_OUT_ENr, &rval, OUT_ENABLEf, fval);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_GP_OUT_ENr(unit, rval));

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_GP_DATA_OUTr(unit, &rval));
    fval = soc_reg_field_get(unit, CMIC_GP_DATA_OUTr, rval, DATA_OUTf);
    fval &= mask;
    fval |= (val << pin);

    soc_reg_field_set(unit, CMIC_GP_DATA_OUTr, &rval, DATA_OUTf, fval);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_GP_DATA_OUTr(unit, rval));

    return SOC_E_NONE;
}

#define TOP_STRAP_STATUS_TSC_FIELD(val)    \
    (((val) >> 26) & 0x3F)
#define TOP_STRAP_STATUS_LCPLL1_REFCLK_SEL(val)    \
    (((val) >> 9) & 0x1)

/*
 * Function:
 *      _soc_greyhound_tsc_core_enable
 * Purpose:
 *      Enable the TSC core and reconfig the port info
 */
STATIC int
_soc_greyhound_tsc_core_enable(int unit, uint32 disabled_tsc_bmp)
{
    int i, j, idx;
    int phy_port, port, blk, qsgmii_mode, num_port;
    uint32 disable_4x10,disable_4x10q, rval;        
    soc_info_t *si;
    
    si = &SOC_INFO(unit);    
    disable_4x10 = 0;
    disable_4x10q = 0;    
    qsgmii_mode = 0;
    (void)_soc_greyhound_qsgmii_mode_check(unit, &qsgmii_mode);
    num_port = _GH_PORTS_NUM_PER_TSC;    
    for (i = 0; i < _GH_MAX_TSC_COUNT; i++) {
        if((1 << i) & disabled_tsc_bmp){           
            if (i == 5){
                disable_4x10q = 1;
                if (qsgmii_mode == 1) {
                    num_port = _GH_PORTS_NUM_PER_TSCQ;
                }
            } else {
                disable_4x10 |= (1 << i);
            }
            for (j = 0; j < num_port; j++) {
                phy_port = tsc_phy_port[i]+j;
                port = si->port_p2l_mapping[phy_port];
                for (idx = 0; idx < SOC_DRIVER(unit)->port_num_blktype;
                    idx++) {
                    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, idx);
                    if (blk < 0) { /* end of block list of each port */
                        break;
                    }
                    if(si->block_port[blk] == port){
                        si->block_port[blk] = -1;
                    }
                    si->block_valid[blk]=0;                    
                    SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk],port);
                }
                si->port_p2l_mapping[phy_port]= -1;                    
                si->port_p2m_mapping[phy_port] = -1;                
                si->max_port_p2m_mapping[phy_port] = -1;                    
                si->port_l2p_mapping[port] = -1;
                si->port_m2p_mapping[port] = -1;
                
                SOC_PBMP_PORT_REMOVE(si->ether.bitmap,port);
                SOC_PBMP_PORT_REMOVE(si->ge.bitmap,port);
                SOC_PBMP_PORT_REMOVE(si->xe.bitmap,port);
                SOC_PBMP_PORT_REMOVE(si->hg.bitmap,port);
                SOC_PBMP_PORT_REMOVE(si->st.bitmap,port);
                SOC_PBMP_PORT_REMOVE(si->gx.bitmap,port);
                SOC_PBMP_PORT_REMOVE(si->xl.bitmap,port);
                SOC_PBMP_PORT_REMOVE(si->all.bitmap,port);
                SOC_PBMP_PORT_REMOVE(si->port.bitmap,port);
                sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                         "?%d", port);
                si->port_offset[port] = port;
            }
        }
    }    
    
#define RECONFIGURE_PORT_CONFIG_INFO(ptype,str) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, port) { \
        if(sal_strcmp(str,"")){\
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),\
            "%s%d", str, si->ptype.num);\
        }\
        si->ptype.port[si->ptype.num++] = port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = port; \
        } \
        if (port > si->ptype.max) { \
            si->ptype.max = port; \
        } \
    }

    /* coverity[pointless_string_compare] */  
    RECONFIGURE_PORT_CONFIG_INFO(ether,"");
    /* coverity[pointless_string_compare] */  
    RECONFIGURE_PORT_CONFIG_INFO(st,"");
    RECONFIGURE_PORT_CONFIG_INFO(hg,"hg");
    RECONFIGURE_PORT_CONFIG_INFO(xe,"xe");
    RECONFIGURE_PORT_CONFIG_INFO(ge,"ge");
#undef RECONFIGURE_PORT_CONFIG_INFO
    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(READ_PGW_CTRL_0r(unit, &rval));
        soc_reg_field_set(unit, PGW_CTRL_0r, &rval, SW_PM4X10_DISABLEf, disable_4x10);
        soc_reg_field_set(unit, PGW_CTRL_0r, &rval, SW_PM4X10Q_DISABLEf, disable_4x10q);
        SOC_IF_ERROR_RETURN(WRITE_PGW_CTRL_0r(unit, rval));
    }
    SOC_IF_ERROR_RETURN(soc_dport_map_update(unit));
    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      soc_greyhound_chip_warmboot_reset
 * Purpose:
 *      Special re-init sequencing for BCM53400 during warmboot
 */
int
soc_greyhound_chip_warmboot_reset(int unit)
{
    uint32 strap_sts;
    _gh_sku_info_t *matched_sku_info;
    uint32 disabled_tsc_bmp;

    /* The pbmp need to remap according to strap pin */
    if (matched_devid_idx == -1) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Warning: soc_greyhound_port_config_init should "
                             "be invoked first! Choose bcm534x1 port config.\n")));
        matched_devid_idx = 0;
    }

    matched_sku_info = &_gh_sku_port_config[matched_devid_idx];
    SOC_IF_ERROR_RETURN(READ_TOP_STRAP_STATUSr(unit, &strap_sts));

    disabled_tsc_bmp = (TOP_STRAP_STATUS_TSC_FIELD(strap_sts)) |
                       matched_sku_info->disabled_tsc_bmp;

    _soc_greyhound_tsc_core_enable(unit, disabled_tsc_bmp);

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

STATIC int _soc_greyhound_disable_usb(int unit)
{
    uint16  dev_id;
    uint8   rev_id;
    uint32  rval;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    /* Lite sku (sku w/o A9) */
    if (((dev_id & 0x00F0) == 0x0000) || ((dev_id & 0x0061) == 0x0040) ||
        ((dev_id & 0x0060) == 0x0020) || (dev_id == BCM53365_DEVICE_ID)) {
        SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_USBPHY_CTRL_2r(unit, &rval));
        soc_reg_field_set(unit, IPROC_WRAP_USBPHY_CTRL_2r,
                          &rval, PHY_ISOf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_USBPHY_CTRL_2r(unit, rval));
        SOC_IF_ERROR_RETURN(READ_USBH_UTMI_P0CTLr(unit, &rval));
        soc_reg_field_set(unit, USBH_UTMI_P0CTLr, &rval, DFE_POWERUP_FSMf, 0);
        SOC_IF_ERROR_RETURN(WRITE_USBH_UTMI_P0CTLr(unit, rval));
    }
    return SOC_E_NONE;
}

void
soc_greyhound_sbus_ring_map_config(int unit)
{
    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x11112200); /* block 7  - 0 */
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x00430001); /* block 15 - 8 */
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x00005064); /* block 23 - 16 */
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x00000000); /* block 31 - 24 */
    WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x00002222); /* block 39 - 32 */

    WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0);

    return;
}

/*
 * Function:
 *      soc_greyhound_chip_reset
 * Purpose:
 *      Special reset sequencing for BCM53400
 */

int
soc_greyhound_chip_reset(int unit)
{
    uint32 rval, to_usec, strap_sts;
    _gh_sku_info_t *matched_sku_info;
    uint32 disabled_tsc_bmp;    
    int i, change_core_clk = 0;
    soc_info_t *si;
    char *board_name;

    uint32 strap_status_1;

    unsigned ts_ref_freq;
    unsigned ts_idx;
    static const soc_pll_param_t ts_pll[] = {  /* values for 500MHz TSPLL output */
      /*     Fref,  Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_DIV2 */
        {25000000,       100,          0,     1,    5,  2,  2,  3,        1},
        {50000000,        50,          0,     1,    5,  2,  2,  3,        1},
        {       0,       100,          0,     1,    5,  2,  2,  3,        1}  /* 25MHz, from internal reference */
    };
    unsigned bs_ref_freq;
    unsigned bs_idx;
    uint32 bs0_ndiv_high, bs0_ndiv_low;
    uint32 bs1_ndiv_high, bs1_ndiv_low;

    /* For performance reasons, use different settings for BSPLL0 and BSPLL1 */
    static const soc_pll_param_t bs0_pll[] = {  /* values for 20MHz BSPLL output */
      /*     Fref,  Ndiv_int,  Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_FB_DIV2 */
        {25000000,        64,          0,     1,  160,  4,  1,  8,        1},
        {50000000,       128,          0,     1,  160,  4,  1,  8,        1},
        {       0,        64,          0,     1,  160,  4,  1,  8,        1}  /* 25MHz, from internal reference */
    };
    static const soc_pll_param_t bs1_pll[] = {  /* values for 20MHz BSPLL output */
      /*     Fref,  Ndiv_int,  Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_FB_DIV2 */
        {25000000,        62,          0,     1,  155,  4,  1,  8,        1},
        {50000000,       124,          0,     1,  155,  4,  1,  8,        1},
        {       0,        62,          0,     1,  155,  4,  1,  8,        1}  /* 25MHz, from internal reference */
    };

    /* Parameters for BSPLL particular to GH.  Same indexing as above */
    static const unsigned bs_cp1[] = {1, 0, 1};
    static const unsigned bs_cp[] = {3, 0, 3};
    static const unsigned bs_cz[] = {3, 3, 3};
    static const unsigned bs_rp[] = {7, 0, 7};
    static const unsigned bs_rz[] = {7, 8, 7};
    static const unsigned bs_icp[] = {16, 32, 16};

    si = &SOC_INFO(unit);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    soc_greyhound_sbus_ring_map_config(unit);
    sal_usleep(to_usec);
    

    /* PCIe Serdes workaround */
    READ_RESCAL_STATUS_0r(unit, &rval);
    if (soc_reg_field_get(unit, RESCAL_STATUS_0r, rval, RESCAL_DONEf)) {
        SOC_IF_ERROR_RETURN(
            WRITE_CHIPCOMMONG_MII_MANAGEMENT_CONTROLr(unit, 0x88));
        SOC_IF_ERROR_RETURN(
            WRITE_CHIPCOMMONG_MII_MANAGEMENT_COMMAND_DATAr(unit, 0x517e1000));
        SOC_IF_ERROR_RETURN(
            WRITE_CHIPCOMMONG_MII_MANAGEMENT_COMMAND_DATAr(unit, (0x5106c030 | 
                soc_reg_field_get(unit, RESCAL_STATUS_0r, rval, RESCAL_PONf))));
        sal_usleep(to_usec);
        SOC_IF_ERROR_RETURN (
            WRITE_CHIPCOMMONG_MII_MANAGEMENT_CONTROLr(unit, 0));
    }
    /* Power down USB PHY in Lite sku */
    _soc_greyhound_disable_usb(unit);

    /* AVS PVTMON settings fixed in temperature mode*/
    SOC_IF_ERROR_RETURN (
        WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, 0x7e));    
    SOC_IF_ERROR_RETURN (
        soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_0r,
                    REG_PORT_ANY, BG_ADJf, 0x1));

    SOC_IF_ERROR_RETURN(READ_TOP_STRAP_STATUSr(unit, &strap_sts));
    SOC_IF_ERROR_RETURN(READ_TOP_STRAP_STATUS_1r(unit, &strap_status_1));

    SOC_IF_ERROR_RETURN(
        soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr, REG_PORT_ANY,
                               TOP_LCPLL_SOFT_RESETf, 1));

    if (TOP_STRAP_STATUS_LCPLL1_REFCLK_SEL(strap_sts) == 0){
        /* strap_sts = 0:  from LCPLL1 differential pad */
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            TOP_XG_PLL1_CTRL_1r, REG_PORT_ANY, PDIVf, 3));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            TOP_XG1_LCPLL_FBDIV_CTRL_0r, REG_PORT_ANY, XG1_LCPLL_FBDIV_0f, 0));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            TOP_XG1_LCPLL_FBDIV_CTRL_1r, REG_PORT_ANY, XG1_LCPLL_FBDIV_1f, 0xf00));

        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL1_CTRL_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, CP1f, 1);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, CPf,  1);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, CZf,  3);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, RPf,  0);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, RZf,  8);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, ICPf, 32);
        WRITE_TOP_XG_PLL1_CTRL_5r(unit, rval);

        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            TOP_XG_PLL1_CTRL_6r, REG_PORT_ANY, MSC_CTRLf, 0xcbd4));

    } else {
        /* strap_lcpll1_refclk_sel == 1 */
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            TOP_XG_PLL1_CTRL_1r, REG_PORT_ANY, PDIVf, 1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            TOP_XG1_LCPLL_FBDIV_CTRL_0r, REG_PORT_ANY, XG1_LCPLL_FBDIV_0f, 0));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            TOP_XG1_LCPLL_FBDIV_CTRL_1r, REG_PORT_ANY, XG1_LCPLL_FBDIV_1f, 0xfa0));

        if (strap_status_1 & 1) {
            /* 50MHz ref */
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_XG_PLL1_CTRL_7r, REG_PORT_ANY, FREQ_DOUBLER_ONf, 0));
        } else {
            /* 25MHz ref */
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_XG_PLL1_CTRL_7r, REG_PORT_ANY, FREQ_DOUBLER_ONf, 1));
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            TOP_XG_PLL1_CTRL_0r, REG_PORT_ANY, CH0_MDIVf, 0x14));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
            TOP_XG_PLL1_CTRL_6r, REG_PORT_ANY, MSC_CTRLf, 0x0192));

        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL1_CTRL_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, CP1f, 1);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, CPf,  3);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, CZf,  3);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, RPf,  7);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, RZf,  7);
        soc_reg_field_set(unit, TOP_XG_PLL1_CTRL_5r, &rval, ICPf, 16);
        WRITE_TOP_XG_PLL1_CTRL_5r(unit, rval);
    }

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
        TOP_XG_PLL1_CTRL_5r, REG_PORT_ANY, VCO_GAINf, 0x3));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
        TOP_XG_PLL1_CTRL_3r, REG_PORT_ANY, VCO_CURf, 0));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
        TOP_XG_PLL1_CTRL_7r, REG_PORT_ANY, VCO_CONT_ADJf, 0x1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
        TOP_MISC_CONTROL_1r, REG_PORT_ANY, CMIC_TO_XG_PLL1_SW_OVWRf, 1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
        TOP_XG_PLL1_CTRL_7r, REG_PORT_ANY, CPPf, 0x80));

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr, REG_PORT_ANY, TOP_LCPLL_SOFT_RESETf, 0));
    sal_usleep(to_usec);

    soc_greyhound_freq_get(unit, &si->frequency, &change_core_clk);

    if(change_core_clk != 0){
        /* set 53403/53404 core clock to 176MHz */
        if (si->frequency == 176) {
            rval = 0x11;
        } else {
            rval = 0xa;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_CORE_PLL_CTRL4r,
                                     REG_PORT_ANY, MSTR_CH0_MDIVf, rval));

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    CMIC_TO_CORE_PLL_LOADf, 1));
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
             REG_PORT_ANY, TOP_LCPLL_SOFT_RESETf, 0x1));

    if (strap_status_1 & 1) {
        /* 50 MHz clock source. */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_7r,
                    REG_PORT_ANY, FREQ_DOUBLER_ONf, 0x0));

        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, CP1f, 1);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, CPf,  3);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, CZf,  3);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, RPf,  7);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, RZf,  7);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, ICPf, 16);
        WRITE_TOP_XG_PLL0_CTRL_5r(unit, rval);
    } else {
        /* 25 MHz clock source */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_7r,
                    REG_PORT_ANY, FREQ_DOUBLER_ONf, 0x1));
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, CP1f, 1);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, CPf,  1);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, CZf,  3);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, RPf,  0);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, RZf,  8);
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval, ICPf, 32);
        WRITE_TOP_XG_PLL0_CTRL_5r(unit, rval);
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_1r,
             REG_PORT_ANY, PDIVf, 0x1));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_0r,
             REG_PORT_ANY, CH0_MDIVf, 0x18));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_6r,
         REG_PORT_ANY, MSC_CTRLf, 0x0012));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_7r,
             REG_PORT_ANY, VCO_CONT_ADJf, 0x1));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_3r,
             REG_PORT_ANY, VCO_CURf, 0x0));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_5r,
             REG_PORT_ANY, VCO_GAINf, 0x3));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r,
             REG_PORT_ANY, CMIC_TO_XG_PLL0_SW_OVWRf, 0x1));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_XG0_LCPLL_FBDIV_CTRL_0r,
             REG_PORT_ANY, XG0_LCPLL_FBDIV_0f, 0x0));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_XG0_LCPLL_FBDIV_CTRL_1r,
             REG_PORT_ANY, XG0_LCPLL_FBDIV_1f, 0xf00));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_7r,
             REG_PORT_ANY, CPPf, 0x80));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
             REG_PORT_ANY, TOP_LCPLL_SOFT_RESETf, 0x0));
    sal_usleep(to_usec);

    /* BCM953411 needs enable LCPLL0 for external PHY */
    board_name = soc_property_get_str(unit, spn_BOARD_NAME);
    if (board_name &&
        ((sal_strcmp(board_name, "BCM953411K") == 0) ||
         (sal_strcmp(board_name, "BCM953411R") == 0))) {
        SOC_IF_ERROR_RETURN(_soc_greyhound_gpio_set(unit, 3, 0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_6r,
                    REG_PORT_ANY, MSC_CTRLf, 0x1012));
        SOC_IF_ERROR_RETURN(_soc_greyhound_gpio_set(unit, 3, 1));
    }

    /* TSPLL configuration: 500MHz TS_CLK from 20/25/32/50MHz refclk */
    /* CMICd divides by 2 on input, so this is a 250MHz clock to TS logic */

    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);  /* 0->internal reference */

    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }
    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid value for PTP_TS_PLL_REF (%u).  No default PLL params.\n"), ts_ref_freq));
        /* Could check for existence of SOC properties for VCO_DIV2, KA, KI, KP, NDIV_INT, NDIV_FRAC, MDIV, PDIV, and if
           all exist, use them.  For now, just fail.
        */
    } else {
        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_0r, &rval, VCO_DIV2f,
                          soc_property_get(unit, spn_PTP_TS_VCO_DIV2, ts_pll[ts_idx].vco_div2));
        WRITE_TOP_TS_PLL_CTRL_0r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_4r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_TS_KA, ts_pll[ts_idx].ka));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_TS_KI, ts_pll[ts_idx].ki));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_TS_KP, ts_pll[ts_idx].kp));
        WRITE_TOP_TS_PLL_CTRL_4r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_3r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_TS_PLL_N, ts_pll[ts_idx].ndiv_int));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_FRACf,
                          ts_pll[ts_idx].ndiv_frac);
        WRITE_TOP_TS_PLL_CTRL_3r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_PDIV, ts_pll[ts_idx].pdiv));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, ts_pll[ts_idx].mdiv));
        WRITE_TOP_TS_PLL_CTRL_2r(unit, rval);

        /* Enable software overwrite of TimeSync PLL settings. */
        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_TS_PLL_LOADf, 1);
        WRITE_TOP_MISC_CONTROL_1r(unit, rval);
    }

    /* 500Mhz TSPLL -> 250MHz ref at timestamper, implies 4ns resolution */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

    if (!soc_feature(unit, soc_feature_time_v3_no_bs)) {
        /* as a signal to upper-level code that the BroadSync is newly initialized
         * disable BroadSync0/1 bitclock output.  Checked in time.c / 1588 firmware    */
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_BS0_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
        WRITE_CMIC_BS0_CONFIGr(unit, rval);

        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_BS1_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &rval, BS_CLK_OUTPUT_ENABLEf, 0);
        WRITE_CMIC_BS1_CONFIGr(unit, rval);

        /* BSPLL0 has not been configured, so reset/configure both BSPLL0 and BSPLL1 */
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_POST_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_POST_RST_Lf, 0);
        WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

        /* Different BSPLL configurations, but with same Fref */

        bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 0);  /* 0->internal reference */

        for (bs_idx = 0; bs_idx < sizeof(bs0_pll)/sizeof(bs0_pll[0]); ++bs_idx) {
            if (bs0_pll[bs_idx].ref_freq == bs_ref_freq) {
                break;
            }
        }
        if (bs_idx == sizeof(bs0_pll)/sizeof(bs0_pll[0])) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Invalid value for PTP_BS_REF (%u).  No default PLL params.\n"), bs_ref_freq));
            /* Could check for existence of SOC properties for VCO_DIV2, KA, KI, KP, NDIV_INT,
               NDIV_FRAC, MDIV, PDIV, and if all exist, use them.  For now, just fail.
            */
        } else {
            /* BSPLL0 */
            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, VCO_FB_DIV2f,
                              soc_property_get(unit, spn_PTP_BS_VCO_DIV2, bs0_pll[bs_idx].vco_div2));
            WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, KAf,
                              soc_property_get(unit, spn_PTP_BS_KA, bs0_pll[bs_idx].ka));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, KIf,
                              soc_property_get(unit, spn_PTP_BS_KI, bs0_pll[bs_idx].ki));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, KPf,
                              soc_property_get(unit, spn_PTP_BS_KP, bs0_pll[bs_idx].kp));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, PDIVf,
                              soc_property_get(unit, spn_PTP_BS_PDIV, bs0_pll[bs_idx].pdiv));
            WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval);

            bs0_ndiv_high = ((soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs0_pll[bs_idx].ndiv_int) << 6) |
                            ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs0_pll[bs_idx].ndiv_frac) >> 26) & 0x3f));
            bs0_ndiv_low = ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs0_pll[bs_idx].ndiv_frac) << 6) & 0xffffc000);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r, &rval, BROAD_SYNC0_LCPLL_FBDIV_0f, bs0_ndiv_low);
            WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC0_LCPLL_FBDIV_1f, bs0_ndiv_high);
            WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_0r, &rval, CH0_MDIVf,
                              soc_property_get(unit, spn_PTP_BS_MNDIV, bs0_pll[bs_idx].mdiv));
            WRITE_TOP_BS_PLL0_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL0_CTRL_6r, REG_PORT_ANY, MSC_CTRLf, 0x12));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL0_CTRL_7r, REG_PORT_ANY, VCO_CONT_ADJf, 1));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL0_CTRL_3r, REG_PORT_ANY, VCO_CURf, 0));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL0_CTRL_3r, REG_PORT_ANY, VCO_CURf, 0));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL0_CTRL_5r, REG_PORT_ANY, VCO_GAINf, 0));

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_5r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_5r, &rval, CP1f, bs_cp1[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_5r, &rval, CPf,  bs_cp[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_5r, &rval, CZf,  bs_cz[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_5r, &rval, RPf,  bs_rp[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_5r, &rval, RZf,  bs_rz[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_5r, &rval, ICPf, bs_icp[bs_idx]);
            WRITE_TOP_BS_PLL0_CTRL_5r(unit, rval);

            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL0_CTRL_7r, REG_PORT_ANY, CPPf, 0x80));

            /* BSPLL1 */
            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_3r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, VCO_FB_DIV2f,
                              soc_property_get(unit, spn_PTP_BS_VCO_DIV2, bs1_pll[bs_idx].vco_div2));
            WRITE_TOP_BS_PLL1_CTRL_3r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, KAf,
                              soc_property_get(unit, spn_PTP_BS_KA, bs1_pll[bs_idx].ka));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, KIf,
                              soc_property_get(unit, spn_PTP_BS_KI, bs1_pll[bs_idx].ki));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, KPf,
                              soc_property_get(unit, spn_PTP_BS_KP, bs1_pll[bs_idx].kp));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, PDIVf,
                              soc_property_get(unit, spn_PTP_BS_PDIV, bs1_pll[bs_idx].pdiv));
            WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval);

            bs1_ndiv_high = ((soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs1_pll[bs_idx].ndiv_int) << 6) |
                            ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs1_pll[bs_idx].ndiv_frac) >> 26) & 0x3f));
            bs1_ndiv_low = ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs1_pll[bs_idx].ndiv_frac) << 6) & 0xffffc000);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r, &rval, BROAD_SYNC1_LCPLL_FBDIV_0f, bs1_ndiv_low);
            WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC1_LCPLL_FBDIV_1f, bs1_ndiv_high);
            WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_0r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_0r, &rval, CH0_MDIVf,
                              soc_property_get(unit, spn_PTP_BS_MNDIV, bs1_pll[bs_idx].mdiv));
            WRITE_TOP_BS_PLL1_CTRL_0r(unit, rval);

            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL1_CTRL_6r, REG_PORT_ANY, MSC_CTRLf, 0x12));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL1_CTRL_7r, REG_PORT_ANY, VCO_CONT_ADJf, 1));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL1_CTRL_3r, REG_PORT_ANY, VCO_CURf, 0));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL1_CTRL_3r, REG_PORT_ANY, VCO_CURf, 0));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL1_CTRL_5r, REG_PORT_ANY, VCO_GAINf, 0));

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_5r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_5r, &rval, CP1f, bs_cp1[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_5r, &rval, CPf,  bs_cp[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_5r, &rval, CZf,  bs_cz[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_5r, &rval, RPf,  bs_rp[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_5r, &rval, RZf,  bs_rz[bs_idx]);
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_5r, &rval, ICPf, bs_icp[bs_idx]);
            WRITE_TOP_BS_PLL1_CTRL_5r(unit, rval);

            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                TOP_BS_PLL1_CTRL_7r, REG_PORT_ANY, CPPf, 0x80));

            /*
             * Set frequency doubler based on reference clock.
             * Greyhound supports 25MHz/50MHz reference clocks.
             * LCPLL frequency doubler Boolean in PLL control
             * register 7 scales 25MHz so effective reference
             * frequency equals 50MHz.
             */            
            if (strap_status_1 & 1) {
                SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_7r(unit, &rval));
                soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_7r, &rval, FREQ_DOUBLER_ONf, 0);
                WRITE_TOP_BS_PLL0_CTRL_7r(unit, rval);

                SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_7r(unit, &rval));
                soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_7r, &rval, FREQ_DOUBLER_ONf, 0);
                WRITE_TOP_BS_PLL1_CTRL_7r(unit, rval);
            }

            /* Enable software overwrite of BroadSync 0/1 PLL settings. */
            SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
            soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_BS_PLL0_SW_OVWRf, 1);
            soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_BS_PLL1_SW_OVWRf, 1);
            WRITE_TOP_MISC_CONTROL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, LOAD_EN_CHf, 0);
            WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval);
            soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, LOAD_EN_CHf, 1);
            WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval);

            SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit,&rval));
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, LOAD_EN_CHf, 0);
            WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval);
            soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, LOAD_EN_CHf, 1);
            WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval);
        }

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf, 1);
        WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_POST_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_POST_RST_Lf, 1);
        WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

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
                        
    }

    /*
     * Bring port blocks out of reset
     */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP3_RST_Lf, 1);        
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP4_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GXP0_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);
    
    /* Bring network sync out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);
    
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* speed limit settings for some SKUs option */
    
    /* TSCx should be disabled in some sku. */ 
    /* pgw_ctrl_0 */
    if (matched_devid_idx == -1) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Warning: soc_greyhound_port_config_init should "
                             "be invoked first! Choose bcm534x1 port config.\n")));
        matched_devid_idx = 0;
    }

    matched_sku_info = &_gh_sku_port_config[matched_devid_idx];
    disabled_tsc_bmp = (TOP_STRAP_STATUS_TSC_FIELD(strap_sts)) |
        matched_sku_info->disabled_tsc_bmp;

    _soc_greyhound_tsc_core_enable(unit, disabled_tsc_bmp);

    for (i = 0; _gh_sku_speed_limit[i].dev_id; i++){
        int j;
        if ((matched_sku_info->dev_id == _gh_sku_speed_limit[i].dev_id) &&
            (matched_sku_info->config_op == _gh_sku_speed_limit[i].config_op)){
                for (j = 0; j < _GH_MAX_LPORT_COUNT; j++ ){
                    soc_reg32_set(unit, speed_limit_regs[j], 0, 0, 
                        _gh_sku_speed_limit[i].phy_port_mapping[j]);
                }
                break;
            }            
    }    
    return SOC_E_NONE;
}


int
soc_greyhound_tsc_reset(int unit)
{
    int blk, port;
    uint32 rval; 

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_reset(unit, port, 0));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (port == -1 || port == REG_PORT_ANY) {
            continue;
        }
        SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
    }
    return SOC_E_NONE;
}


STATIC int
_soc_greyhound_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_greyhound_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}


void soc_greyhound_oam_handler_register(int unit, soc_greyhound_oam_handler_t handler)
{
    uint32 rval;
    int rv, fidx = 0;
    gh_oam_handler[unit] = handler;
    rv = READ_IP1_INTR_ENABLEr(unit, &rval);
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error reading %s reg !!\n"),
                              unit, SOC_REG_NAME(unit, IP1_INTR_ENABLEr)));
    }
    while (_soc_gh_oam_interrupt_fields[fidx] != INVALIDf) {
        soc_reg_field_set(unit, IP1_INTR_ENABLEr, &rval,
                          _soc_gh_oam_interrupt_fields[fidx], 1);
        fidx++;
    }
    rv = WRITE_IP1_INTR_ENABLEr(unit, rval);
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error writing %s reg !!\n"),
                              unit, SOC_REG_NAME(unit, IP1_INTR_ENABLEr)));
    }
}

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r
};

int
soc_gh_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count)
{
    soc_reg_t reg;
    int index;
    uint32 rval;
    int fval, cur, peak;
    int num_entries_out;

    *temperature_count = 0;
    if (COUNTOF(pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(pvtmon_result_reg);
    }
        
    for (index = 0; index < num_entries_out; index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        cur = (41016000 - (48517 * fval)) / 10000;
        fval = soc_reg_field_get(unit, reg, rval, PEAK_TEMP_DATAf);
        peak = (41016000 - (48517 * fval)) / 10000;
        (temperature_array + index)->curr = cur;
        (temperature_array + index)->peak = peak;
    }
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    *temperature_count=num_entries_out;
    return SOC_E_NONE;
}
 
typedef struct _gh_pgw_reg_info_s {
    soc_reg_t   pgw_reg;
    uint32      valid_bm;
    uint32      invalid_bm; 
    uint32      invalid_blk_num_bm;
}_gh_pgw_reg_info_t;

STATIC _gh_pgw_reg_info_t _gh_pgw_reg_info[] = {
    {PGW_GX_CONFIGr, 0x3c, 0, 0x1f},
    {PGW_GX_TXFIFO_CTRLr, 0x3c, 0, 0x1f},
    {PGW_GX_SPARE0_REGr, 0x3c, 0, 0x1f},
    {PGW_GX_RXFIFO_SOFT_RESETr, 0, 0, 0x1f},
    {PGW_GX_ECC_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_RXFIFO0_ECC_SBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_RXFIFO0_ECC_DBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_RXFIFO1_ECC_SBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_RXFIFO1_ECC_DBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_TXFIFO0_ECC_SBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_TXFIFO0_ECC_DBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_TXFIFO1_ECC_SBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_TXFIFO1_ECC_DBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_TXFIFO2_ECC_SBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_TXFIFO2_ECC_DBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_TXFIFO3_ECC_SBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_TXFIFO3_ECC_DBE_STATUSr, 0, 0, 0x1f},
    {PGW_GX_TXFIFO_OVERFLOWr, 0, 0, 0x1f},
    {PGW_GX_RXFIFO0_OVERFLOW_ERRORr, 0, 0, 0x1f},
    {PGW_GX_RXFIFO1_OVERFLOW_ERRORr, 0, 0, 0x1f},
    {PGW_GX_TM0_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_TM1_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_TM2_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_TM3_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_TM4_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_TM5_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_TM6_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_TM7_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_TM8_CONTROLr, 0, 0, 0x1f},
    {PGW_GX_BYPASS_LED_CHAIN_CONFIGr, 0, 0, 0x1f},
    {PGW_GX_INTR_STATUSr, 0, 0, 0x1f},
    {PGW_GX_INTR_ENABLEr, 0, 0, 0x1f},
    {PGW_XL_CONFIGr, 0, 0x3c, 0x20},
    {PGW_XL_TXFIFO_CTRLr, 0, 0x3c, 0x20},
    {PGW_XL_SPARE0_REGr, 0, 0x3c, 0x20},
    {PGW_XL_RXFIFO_SOFT_RESETr, 0, 0, 0x20},
    {PGW_XL_ECC_CONTROLr, 0, 0, 0x20},        
    {PGW_XL_RXFIFO_ECC_SBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_RXFIFO_ECC_DBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_TXFIFO0_ECC_SBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_TXFIFO0_ECC_DBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_TXFIFO1_ECC_SBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_TXFIFO1_ECC_DBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_TXFIFO2_ECC_SBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_TXFIFO2_ECC_DBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_TXFIFO3_ECC_SBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_TXFIFO3_ECC_DBE_STATUSr, 0, 0, 0x20},
    {PGW_XL_TXFIFO_OVERFLOWr, 0, 0, 0x20},
    {PGW_XL_RXFIFO_OVERFLOW_ERRORr, 0, 0, 0x20},
    {PGW_XL_TM0_CONTROLr, 0, 0, 0x20},
    {PGW_XL_TM1_CONTROLr, 0, 0, 0x20},
    {PGW_XL_TM2_CONTROLr, 0, 0, 0x20},
    {PGW_XL_TM3_CONTROLr, 0, 0, 0x20},
    {PGW_XL_TM4_CONTROLr, 0, 0, 0x20},
    {PGW_XL_TM5_CONTROLr, 0, 0, 0x20},
    {PGW_XL_TM6_CONTROLr, 0, 0, 0x20},
    {PGW_XL_TM7_CONTROLr, 0, 0, 0x20},
    {PGW_XL_TM8_CONTROLr, 0, 0, 0x20},
    {PGW_XL_BYPASS_LED_CHAIN_CONFIGr, 0, 0, 0x20},
    {PGW_XL_INTR_STATUSr, 0, 0, 0x20},
    {PGW_XL_INTR_ENABLEr, 0, 0, 0x20},
    {INVALIDr, 0, 0},
};

/*
 * Function:
 *  soc_greyhound_pgw_reg_blk_index_get
 * Purpose:
 *  Deal with the blk, blknum for PGW_GX and PGW_XL registers
 * Parameters:
 *  reg (IN): register.
 *  port (IN): logical port if PGW_GX's port register.
 *  bm (IN/OUT): bm should be added for the reg.
 *  block (IN/OUT): blk used to access the reg.
 *  index (OUT): port index of the reg.
 *  invalid_blk_check (IN): perform the blk check.
 *                          skip the block if the return value = 1.
 * Return:
 *   SOC_E_NOT_FOUND : w/o bm,blk,invalid_blk_check case, register not matched.
 *   1 : register matched and the block/index has been override.
 *   1 : register matched and the valid bm has been override.
 *   1 : skip the block when the invalid_blk_check=1 
 *   SOC_E_PARAM : invalid paramter 
 */
int 
soc_greyhound_pgw_reg_blk_index_get(int unit, 
    soc_reg_t reg, soc_port_t port, 
    pbmp_t *bm, int *block, int *index, int invalid_blk_check) {
    
    int i, p, phy_port, blk_num, pgw_blk;    
    soc_reg_info_t *reginfo;
    _gh_pgw_reg_info_t *pgw_reg = NULL;
    pbmp_t pgw_pbm, pgw_invalid_pbm;

    pgw_blk = -1;
    SOC_PBMP_CLEAR(pgw_pbm);
 
    for (i = 0; _gh_pgw_reg_info[i].pgw_reg != INVALIDr; i ++) {
        if (reg == _gh_pgw_reg_info[i].pgw_reg){
            pgw_reg = &_gh_pgw_reg_info[i];
            break;
        }
    }
    
    if (!pgw_reg) {
        return SOC_E_NOT_FOUND;
    }

    reginfo = &SOC_REG_INFO(unit, reg);
    phy_port = 0;            
    if (reginfo->regtype == soc_portreg) {
        if (port < 0) {
            return SOC_E_PARAM;
        }
        SOC_PBMP_WORD_SET(pgw_pbm, 0, pgw_reg->valid_bm);
        SOC_PBMP_WORD_SET(pgw_invalid_pbm, 0, pgw_reg->invalid_bm);
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    }        

    if (bm != NULL) {
        SOC_PBMP_ITER(pgw_pbm, p) {
            SOC_PBMP_PORT_ADD(*bm, p);
        }
        return 1;
    }
    if (block) {
        if (reginfo->regtype == soc_portreg) {
            if (port < 0) {
                return SOC_E_PARAM;
            }
            if( SOC_PORT_BLOCK_TYPE(unit,phy_port) == SOC_BLK_GXPORT ){
                if(SOC_PBMP_MEMBER(pgw_pbm, port)) {
                    /* get the blk from phy_port=2 bindex=1 i.e. XLPORT5*/
                    pgw_blk = SOC_PORT_IDX_BLOCK(unit, 2, 1);
                    if (index) {
                        *index = phy_port - 2;
                    }
                    if (!invalid_blk_check) {
                        *block = pgw_blk;
                        return 1;
                    }
                }
                if(SOC_PBMP_MEMBER(pgw_invalid_pbm, port)) {
                    *block = 0;
                    return 1;
                }
            }
        }        
    }
    if (invalid_blk_check){
        if(!block){
            return SOC_E_PARAM;        
        }        
        /* check if the *block is valid for the reg */

        if (pgw_blk == -1) {
            pgw_blk = *block;
        }
        blk_num = SOC_BLOCK_NUMBER(unit, pgw_blk) & 0xff;
        if (pgw_reg->invalid_blk_num_bm & (1 << blk_num)){
            return 1;
        }
    }
    return SOC_E_NONE;
}

int
_soc_greyhound_features(int unit, soc_feature_t feature){

    uint16              dev_id;
    uint8               rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    switch (feature) {
        case soc_feature_tsce:
        case soc_feature_arl_hashed:
        case soc_feature_arl_insert_ovr:
        case soc_feature_cfap_pool:
        case soc_feature_cos_rx_dma:
        case soc_feature_dcb_type31:
        case soc_feature_ingress_metering:
        case soc_feature_egress_metering:
        case soc_feature_l3_ip6:
        case soc_feature_l3_lookup_cmd:
        case soc_feature_led_proc:                                                              
        case soc_feature_led_data_offset_a0:
        case soc_feature_schmsg_alias:                                                             
        case soc_feature_stack_my_modid:                                                                
        case soc_feature_stat_dma:                                                               
        case soc_feature_cpuport_stat_dma:
        case soc_feature_table_dma:                                                                        
        case soc_feature_tslam_dma:                                                                
        case soc_feature_stg:                                                                
        case soc_feature_stg_xgs:                                                                                        
        case soc_feature_remap_ut_prio:
        case soc_feature_xgxs_v7:
        case soc_feature_phy_cl45:
        case soc_feature_aging_extended:
        case soc_feature_modmap:
        case soc_feature_l3_sgv:
        case soc_feature_l3_dynamic_ecmp_group:
        case soc_feature_l2_hashed:                                                              
        case soc_feature_l2_lookup_cmd:
        case soc_feature_l2_lookup_retry:
        case soc_feature_l2_user_table:
        case soc_feature_schan_hw_timeout:
        case soc_feature_lpm_tcam:
        case soc_feature_mdio_enhanced:                                               
        case soc_feature_dodeca_serdes:
        case soc_feature_rxdma_cleanup:
        case soc_feature_fe_maxframe:
        case soc_feature_l2x_parity:
        case soc_feature_l3x_parity: 
        case soc_feature_l2_modfifo:
        case soc_feature_vlan_mc_flood_ctrl:
        case soc_feature_vlan_translation:
        case soc_feature_parity_err_tocpu:        
        case soc_feature_nip_l3_err_tocpu:
        case soc_feature_l3mtu_fail_tocpu:
        case soc_feature_meter_adjust:
        case soc_feature_xgxs_power:
        case soc_feature_src_modid_blk:
        case soc_feature_src_modid_blk_ucast_override:
        case soc_feature_src_modid_blk_opcode_override:
        case soc_feature_egress_blk_ucast_override:
        case soc_feature_stat_jumbo_adj:
        case soc_feature_stat_xgs3:
        case soc_feature_port_trunk_index:                                        
        case soc_feature_cpuport_switched:
        case soc_feature_cpuport_mirror:                                                        
        case soc_feature_higig2:
        case soc_feature_color:
        case soc_feature_color_inner_cfi:
        case soc_feature_color_prio_map:
        case soc_feature_untagged_vt_miss:
        case soc_feature_module_loopback:
        case soc_feature_dscp_map_per_port:
        case soc_feature_egr_dscp_map_per_port:
        case soc_feature_dscp_map_mode_all:
        case soc_feature_higig_lookup:
        case soc_feature_egr_mirror_path:
        case soc_feature_trunk_extended:
        case soc_feature_hg_trunking:
        case soc_feature_hg_trunk_override:
        case soc_feature_egr_vlan_check:
        case soc_feature_cpu_proto_prio:
        case soc_feature_hg_trunk_failover:
        case soc_feature_trunk_egress:
        case soc_feature_force_forward:
        case soc_feature_port_egr_block_ctl:
        case soc_feature_bucket_support:
        case soc_feature_remote_learn_trust:
        case soc_feature_src_mac_group:
        case soc_feature_storm_control:
        case soc_feature_hw_stats_calc:
        case soc_feature_mac_learn_limit:
        case soc_feature_linear_drr_weight:
        case soc_feature_igmp_mld_support:
        case soc_feature_basic_dos_ctrl:
        case soc_feature_enhanced_dos_ctrl:
        case soc_feature_proto_pkt_ctrl:
        case soc_feature_vlan_ctrl:
        case soc_feature_big_icmpv6_ping_check:
        case soc_feature_trunk_group_overlay:
        case soc_feature_xport_convertible:
        case soc_feature_dual_hash:
        case soc_feature_dscp:
        case soc_feature_unimac:
        case soc_feature_ifg_wb_include_unimac:
        case soc_feature_xlmac:
        case soc_feature_generic_table_ops:
        case soc_feature_vlan_translation_range:
        case soc_feature_static_pfm:
        case soc_feature_sgmii_autoneg:
#ifdef INCLUDE_RCPU
        case soc_feature_rcpu_1:
        case soc_feature_rcpu_priority:
        case soc_feature_rcpu_tc_mapping:
#endif
        case soc_feature_mem_push_pop:
        case soc_feature_dcb_reason_hi:
        case soc_feature_multi_sbus_cmds:
        case soc_feature_new_sbus_format:
        case soc_feature_new_sbus_old_resp:
        case soc_feature_sbus_format_v4:
        case soc_feature_fifo_dma:
        case soc_feature_fifo_dma_active:
        case soc_feature_l3_entry_key_type:
        case soc_feature_l2_pending:
        case soc_feature_internal_loopback:
        case soc_feature_vlan_action:
        case soc_feature_mac_based_vlan:
        case soc_feature_ip_subnet_based_vlan:            
        case soc_feature_packet_rate_limit:
        case soc_feature_system_mac_learn_limit:
        case soc_feature_field:
        case soc_feature_field_meter_pools8:
        case soc_feature_field_mirror_ovr:
        case soc_feature_field_udf_higig:
        case soc_feature_field_udf_ethertype:
        case soc_feature_field_comb_read:
        case soc_feature_field_wide:
        case soc_feature_field_slice_enable:
        case soc_feature_field_cos:
        case soc_feature_field_color_indep:
        case soc_feature_field_qual_drop:
        case soc_feature_field_qual_IpType:
        case soc_feature_field_qual_Ip6High:
        case soc_feature_field_ingress_global_meter_pools:
        case soc_feature_field_ingress_ipbm:
        case soc_feature_field_egress_flexible_v6_key:
        case soc_feature_field_egress_global_counters:
        case soc_feature_field_ing_egr_separate_packet_byte_counters:
        case soc_feature_field_egress_metering:
        case soc_feature_field_intraslice_double_wide:
        case soc_feature_field_virtual_slice_group:
        case soc_feature_field_action_timestamp:
        case soc_feature_field_action_l2_change:
        case soc_feature_field_virtual_queue:
        case soc_feature_field_action_redirect_nexthop:
        case soc_feature_field_slice_dest_entity_select:
        case soc_feature_field_packet_based_metering:
        case soc_feature_lport_tab_profile:
        case soc_feature_ignore_cmic_xgxs_pll_status:
        case soc_feature_use_double_freq_for_ddr_pll:
        case soc_feature_counter_parity:
        case soc_feature_extended_pci_error:
        case soc_feature_priority_flow_control:
        case soc_feature_gh_style_pfc_config:
        case soc_feature_qos_profile:
        case soc_feature_rx_timestamp:
        case soc_feature_rx_timestamp_upper:
        case soc_feature_logical_port_num:
        case soc_feature_timestamp_counter:
        case soc_feature_generic_counters:
        case soc_feature_modport_map_profile:
        case soc_feature_modport_map_dest_is_hg_port_bitmap:
        case soc_feature_eee:
        case soc_feature_xy_tcam:
        case soc_feature_xy_tcam_direct:
        case soc_feature_xy_tcam_28nm:
        case soc_feature_vlan_egr_it_inner_replace:
        case soc_feature_cmicm:
        case soc_feature_iproc:
        case soc_feature_iproc_7:
        case soc_feature_unified_port:
        case soc_feature_sbusdma:
        case soc_feature_mirror_encap_profile:
        case soc_feature_higig_misc_speed_support:
        case soc_feature_vpd_profile:
        case soc_feature_color_prio_map_profile:
        case soc_feature_port_extension:
        case soc_feature_niv:
        case soc_feature_mem_parity_eccmask:
        case soc_feature_discard_ability:
        case soc_feature_wred_drop_counter_per_port:
        case soc_feature_l2_no_vfi:
        case soc_feature_time_support:
        case soc_feature_time_v3:
        case soc_feature_timesync_support:
        case soc_feature_timesync_v3:
        case soc_feature_time_synce_divisor_setting:
        case soc_feature_gmii_clkout:
        case soc_feature_l3mc_use_egress_next_hop:
        case soc_feature_field_action_pfc_class:
        case soc_feature_fifo_dma_hu2:
        case soc_feature_proxy_port_property:
        case soc_feature_system_reserved_vlan:
        case soc_feature_ser_parity:
        case soc_feature_mem_cache:
        case soc_feature_mem_wb_cache_reload:
        case soc_feature_ser_engine:
        case soc_feature_ser_system_scrub:
        case soc_feature_regs_as_mem:
        case soc_feature_cmicd_v2:
        case soc_feature_cmicm_extended_interrupts:
        case soc_feature_int_common_init:
        case soc_feature_inner_tpid_enable:
        case soc_feature_no_tunnel:
        case soc_feature_ecn_wred:
        case soc_feature_eee_bb_mode:
        case soc_feature_field_oam_actions:
        case soc_feature_hg_no_speed_change:
        case soc_feature_hr2_dual_hash:
        case soc_feature_uc:
        case soc_feature_uc_mhost:
        case soc_feature_iproc_ddr:
        case soc_feature_ptp:
        case soc_feature_field_qual_vlanformat_reverse:
        case soc_feature_cpureg_dump:
        case soc_feature_ignore_mem_write_nak:
        case soc_feature_field_udf_offset_hg_114B:
        case soc_feature_field_udf_offset_hg2_110B:
        case soc_feature_eee_stat_clear_directly:
        case soc_feature_pktpri_as_dot1p:
        case soc_feature_cosq_hol_drop_packet_count:
        case soc_feature_hg2_light_in_portmacro:
        case soc_feature_sync_port_lport_tab:
        case soc_feature_port_encap_speed_max_config:
        case soc_feature_serdes_firmware_pos_by_host_endian:
        case soc_feature_dport_update_hl:
        case soc_feature_no_mirror_truncate:
            return TRUE;
        case soc_feature_field_multi_stage: /* include VFP, EFP */
            /* Ranger 2 only */
            if ((dev_id & 0xF000) == 0xB000) {
                return TRUE;
            } else {
                return FALSE;
            }
        case soc_feature_field_slice_size128: /* Half set FP */
        case soc_feature_l3_no_ecmp:        /* no ecmp in l3 */
            /* other than Ranger2 */
            if ((dev_id & 0xF000) == 0xB000) {
                return FALSE;
            } else {
                return TRUE;
            }
        case soc_feature_time_v3_no_bs:  
            /* Lite sku (sku w/o A9) */
            if (((dev_id & 0x00F0) == 0x0000) || ((dev_id & 0x0061) == 0x0040) ||
                ((dev_id & 0x0060) == 0x0020) || 
                (dev_id == BCM53365_DEVICE_ID)||
                (dev_id == BCM53369_DEVICE_ID)) {
                /* No BroadSync interfaces for lite SKUs */
                return TRUE;
            } else {
                return FALSE;
            }
        case soc_feature_field_slices8:
            /* other than Bloodhound */
            if ((dev_id & 0x0060) == 0x0020) {
                return FALSE;
            } else {
                return TRUE;
            }
        case soc_feature_field_slices4:
            /* Bloodhound only */
            if ((dev_id & 0x0060) == 0x0020) {
                return TRUE;
            } else {
                return FALSE;
            }
        case soc_feature_l3:
        case soc_feature_ip_mcast:
        case soc_feature_ip_mcast_repl:
        case soc_feature_ipmc_unicast:
        case soc_feature_ipmc_use_configured_dest_mac:
        case soc_feature_oam:
            /* l3 related*/
            /* other than Bloodhound and 53365 */
            if (((dev_id & 0x0060) == 0x0020) || (dev_id == BCM53365_DEVICE_ID)) {
                return FALSE;
            } else {
                return TRUE;
            }

        case soc_feature_core_clock_300m:
            if ((dev_id == BCM56060_DEVICE_ID) ||
                (dev_id == BCM53405_DEVICE_ID) ||
                (dev_id == BCM53415_DEVICE_ID) ||
                (dev_id == BCM53406_DEVICE_ID) ||
                (dev_id == BCM53416_DEVICE_ID) ||
                (dev_id == BCM56062_DEVICE_ID) ||
                (dev_id == BCM53454_DEVICE_ID) ||
                (dev_id == BCM53455_DEVICE_ID) ||
                (dev_id == BCM53456_DEVICE_ID) ||
                (dev_id == BCM53457_DEVICE_ID) ||
                (dev_id == BCM53365_DEVICE_ID) ||
                (dev_id == BCM53369_DEVICE_ID)){
                return TRUE;
            } else {
                return FALSE;
            }
        case soc_feature_timesync_timestampingmode:
            return TRUE;
        case soc_feature_portmod:
            return FALSE;
        case soc_feature_uc_image_name_with_no_chip_rev:
            return TRUE;
        default:
            return FALSE;
    }
}

#if defined(SER_TR_TEST_SUPPORT)

/* Overlay memory */
const soc_ser_overlay_test_t soc_ser_gh_overlay_mems[] = {
    {L3_ENTRY_IPV4_UNICASTm, L3_ENTRY_PARITY_CONTROLr, PARITY_ENf, L3_ENTRY_ONLYm,  _SOC_ACC_TYPE_PIPE_ANY},
    {L3_ENTRY_IPV4_MULTICASTm, L3_ENTRY_PARITY_CONTROLr, PARITY_ENf, L3_ENTRY_ONLYm,  _SOC_ACC_TYPE_PIPE_ANY},
    {L3_ENTRY_IPV6_MULTICASTm, L3_ENTRY_PARITY_CONTROLr, PARITY_ENf, L3_ENTRY_ONLYm,  _SOC_ACC_TYPE_PIPE_ANY},
    {L3_ENTRY_IPV6_UNICASTm, L3_ENTRY_PARITY_CONTROLr, PARITY_ENf, L3_ENTRY_ONLYm,  _SOC_ACC_TYPE_PIPE_ANY},
    {MODPORT_MAPm, MODPORT_MAP_SW_PARITY_CONTROLr, PARITY_ENf, MODPORT_MAP_SWm,  _SOC_ACC_TYPE_PIPE_ANY},
    {INVALIDm}
};



/*
 * Function:
 *      soc_gh_ser_mem_test
 * Purpose:
 *      Performs a SER test on a single Greyhound memory
 * Parameters:
 *      unit               - (IN) Device Number
 *      mem                - (IN) The memory to test
 *      test_type        - (IN) How many indices in the memory to test
 *      cmd         - (IN) TRUE if a command-line test is desired.
 */
int soc_gh_ser_mem_test(int unit, soc_mem_t mem, 
                                _soc_ser_test_t test_type, int cmd) {
    int group, table, i, rv = SOC_E_NONE, found_mem = FALSE;
    _soc_gh_parity_info_t *info;
    soc_mem_t memTable;
    soc_port_t block_port;
    soc_block_t blk;
    int testErrors = 0;
    ser_test_data_t test_data;
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], fieldData[SOC_MAX_REG_FIELD_WORDS];
    uint32 reg_val, wred_refresh = 0;
    int overlay_mem = 0;
    _soc_generic_ser_info_t *tcam_ser_info;
    _soc_gh_parity_group_info_t *parity_group_info;

    soc_ser_test_long_sleep = TRUE;
    soc_ser_test_long_sleep_time_us = 500000;

    tcam_ser_info = _soc_gh_tcam_ser_info[unit];
    
    if (tcam_ser_info != NULL) {
        /*TCAM_test*/
        for (i = 0; tcam_ser_info[i].mem != INVALIDm; i++) {
            if (tcam_ser_info[i].mem == mem) {
                found_mem = TRUE;
                soc_ser_create_test_data(unit, tmp_entry, fieldData,
                             SER_RANGE_ENABLEr, i, INVALIDf, mem,
                             VALIDf, MEM_BLOCK_ANY, REG_PORT_ANY,
                             _SOC_ACC_TYPE_PIPE_ANY, 0, &test_data);
                if (cmd) {
                    ser_test_cmd_generate(unit, &test_data);
                } else {
                    rv = ser_test_mem(unit, 0, &test_data, 
                        test_type, &testErrors);
                    if (rv != SOC_E_NONE) {
                        LOG_CLI((BSL_META_U(unit,
                            "Error during TCAM test.  Aborting.\n")));
                        soc_ser_test_long_sleep = FALSE;
                        return rv;
                    }
                }
            }
        }
    }

    parity_group_info = _soc_gh_parity_group_info[unit];
    
    if ((!found_mem) && (parity_group_info != NULL)) {
        /*H/W memory Test*/
        for (group = 0; parity_group_info[group].cpi_bit; group++) {
            info = parity_group_info[group].info;

            SOC_BLOCK_ITER(unit, blk,
                    parity_group_info[group].blocktype) {
                if (_soc_greyhound_parity_block_port(unit, 
                                        blk, &block_port) < 0) {
                    continue;
                }

                if (parity_group_info[group].blocktype == 
                                                        SOC_BLK_XLPORT) {
                    /* Skip */
                    continue;
                }

                for (table = 0; info[table].error_field != INVALIDf; table++) {
                    memTable = info[table].mem;
                    if (memTable == INVALIDm) {
                        continue;
                    }
                    if (_soc_greyhound_ser_mem_skip(unit, info[table].feature)) {
                        continue;
                    }
                    /* Check if overlay memory */
                    for (i=0; soc_ser_gh_overlay_mems[i].mem != INVALIDm; i++) {
                        if ((soc_ser_gh_overlay_mems[i].mem == mem) &&
                            (soc_ser_gh_overlay_mems[i].base_mem == memTable)) {
                            overlay_mem = 1;
                            break;
                        }
                    }
                    
                    if ((memTable == mem) || (overlay_mem)) {
                        found_mem = TRUE;
                        if (parity_group_info[group].blocktype 
                                            == SOC_BLK_MMU) {
                            soc_ser_create_test_data(unit, tmp_entry, fieldData,
                                MISCCONFIGr,
                                SOC_INVALID_TCAM_PARITY_BIT,
                                PARITY_CHECK_ENf,
                                mem, INVALIDf, blk,
                                block_port, _SOC_ACC_TYPE_PIPE_ANY, 0, 
                                &test_data);
                            wred_refresh = 0;
                            if ((mem == MMU_WRED_AVG_QSIZEm) || 
                                (mem == MMU_WRED_MARK_THDm)) {
                                /* Save WRED refresh configuration */
                                rv = soc_reg32_get(unit,
                                    FUNCTIONAL_REFRESH_ENr, block_port, 0,
                                    &reg_val);
                                if (rv != SOC_E_NONE) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "Error of getting WRED "
                                                        "refresh function  "
                                                        "Aborting.\n")));
                                    soc_ser_test_long_sleep = FALSE;
                                    return rv;
                                }
                                wred_refresh = soc_reg_field_get(unit, 
                                    FUNCTIONAL_REFRESH_ENr,
                                    reg_val, WRED_REFRESH_ENf);
                                if (wred_refresh) {
                                    soc_reg_field_set(unit,
                                        FUNCTIONAL_REFRESH_ENr, &reg_val,
                                        WRED_REFRESH_ENf, 0);
                                    rv = soc_reg32_set(unit,
                                        FUNCTIONAL_REFRESH_ENr, block_port, 0,
                                        reg_val);
                                    if (rv != SOC_E_NONE) {
                                        LOG_CLI((BSL_META_U(unit,
                                                            "Error of disabling WRED "
                                                            "refresh function  "
                                                            "Aborting.\n")));
                                        soc_ser_test_long_sleep = FALSE;
                                        return rv;
                                    }
                                }
                            }
                            if (cmd) {
                                ser_test_cmd_generate(unit, &test_data);
                            } else {
                                rv = ser_test_mem(unit, 0, &test_data,
                                                  test_type, &testErrors);
                            }
                            /* restore WRED refresh configuration */
                            if (wred_refresh) {
                                soc_reg_field_set(unit,
                                    FUNCTIONAL_REFRESH_ENr, &reg_val,
                                    WRED_REFRESH_ENf, 1);
                                rv = soc_reg32_set(unit,
                                    FUNCTIONAL_REFRESH_ENr, block_port, 0,
                                    reg_val);
                                if (rv != SOC_E_NONE) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "Error of enabling WRED "
                                                        "refresh function  "
                                                        "Aborting.\n")));
                                    soc_ser_test_long_sleep = FALSE;
                                    return rv;
                                }
                            }
                        } else {
                            if (mem == TRUNK_GROUPm) {
                                /* 
                                 * Since the RTAG field of TRUNK GROUP memories is implemented 
                                 * by register. We need to assign the test field for this test
                                 */
                                 test_data.test_field = EVEN_PARITYf;
                            }
                            soc_ser_create_test_data(unit, tmp_entry, fieldData,
                                         info[table].control_reg,
                                         SOC_INVALID_TCAM_PARITY_BIT,
                                         info[table].enable_field,
                                         mem, EVEN_PARITYf, blk,
                                         block_port, _SOC_ACC_TYPE_PIPE_ANY, 0,
                                         &test_data);
                            if (cmd) {
                                ser_test_cmd_generate(unit, &test_data);
                            } else {
                                rv = ser_test_mem(unit, 0, &test_data,
                                                  test_type, &testErrors);
                            }
                        }
                        if (rv != SOC_E_NONE) {
                            LOG_CLI((BSL_META_U(unit,
                                                "Error during H/W test.  Aborting.\n")));
                            soc_ser_test_long_sleep = FALSE;
                            return rv;
                        }
                        break;
                    }
                }
                if (found_mem) {
                    break;
                }

            }

            if (found_mem) {
                break;
            }
        }
    }

    if (!found_mem) {
        LOG_CLI((BSL_META_U(unit,
                 "SER Test is unavaible on unit: %d for memory %s\n"), unit, 
                 SOC_MEM_NAME(unit,mem)));
        return SOC_E_UNAVAIL;
    }
    
    if (testErrors == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "SER Test passed on unit: %d for memory %s\n"), unit, 
                 SOC_MEM_NAME(unit,mem)));
    }
    else {
        LOG_CLI((BSL_META_U(unit,
                            "SER Test failed on unit: %d for memory %s\n"), unit, 
                 SOC_MEM_NAME(unit,mem)));
        soc_ser_test_long_sleep = FALSE;
        return SOC_E_MEMORY;
    }
    /* restore to default */
    soc_ser_test_long_sleep = FALSE;
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_gh_ser_test
 * Purpose:
 *      Performs a SER test on all Greyhound Memories
 * Parameters:
 *      unit               - (IN) Device Number
 *      test_type        - (IN) Determines how comprehensive of a test to run
 */
int soc_gh_ser_test(int unit, _soc_ser_test_t test_type) {
    int i, group, rv, table;
    _soc_gh_parity_info_t *info;
    soc_port_t block_port;
    soc_mem_t memTable;
    soc_field_t testfield;
    soc_block_t blk;
    int numTCAMErr   = 0;
    int numHwMemErr   = 0;
    uint32 reg_val, wred_refresh = 0;
    _soc_generic_ser_info_t *tcam_ser_info;
    _soc_gh_parity_group_info_t *parity_group_info;

    soc_ser_test_long_sleep = TRUE;
    soc_ser_test_long_sleep_time_us = 500000;

    tcam_ser_info = _soc_gh_tcam_ser_info[unit];
    
    if (tcam_ser_info != NULL) {
        /*Test each TCAM memory*/
        for(i = 0; tcam_ser_info[i].mem != INVALIDm; i++) {
            rv = ser_test_mem_pipe(unit, SER_RANGE_ENABLEr, i, -1,
                    tcam_ser_info[i].mem, VALIDf, test_type, 
                    MEM_BLOCK_ANY, REG_PORT_ANY,
                    _SOC_ACC_TYPE_PIPE_ANY, &numTCAMErr);
            if (rv != SOC_E_NONE) {
                LOG_CLI((BSL_META_U(unit,
                                    "TR 144 test failed. "
                                    "Error during TCAM test.  Aborting.\n")));
                soc_ser_test_long_sleep = FALSE;
                return rv;
            }
        }
    }

    parity_group_info = _soc_gh_parity_group_info[unit];

    if (parity_group_info != NULL) {
        /*H/W memory Test*/
        for (group = 0; parity_group_info[group].cpi_bit; group++) {
            info = parity_group_info[group].info;

            SOC_BLOCK_ITER(unit, blk,
                    parity_group_info[group].blocktype) {
                if (_soc_greyhound_parity_block_port(unit, blk, &block_port) < 0) {
                    continue;
                }

                if (parity_group_info[group].blocktype == SOC_BLK_XLPORT) {
                    /* no memoy can be tested */
                    continue;
                }

                for (table = 0; info[table].error_field != INVALIDf; table++) {
                    memTable = info[table].mem;
                    if (memTable == INVALIDm) {
                        continue;
                    }
                    if (_soc_greyhound_ser_mem_skip(unit, info[table].feature)) {
                        continue;
                    }
                    testfield = INVALIDf;
                    if (parity_group_info[group].blocktype == 
                                 SOC_BLK_MMU) {
                        if ((memTable == MMU_WRED_AVG_QSIZEm) || 
                            (memTable == MMU_WRED_MARK_THDm)) {
                            /* Save WRED refresh configuration */
                            rv = soc_reg32_get(unit,
                                FUNCTIONAL_REFRESH_ENr, block_port, 0,
                                &reg_val);
                            if (rv != SOC_E_NONE) {
                                LOG_CLI((BSL_META_U(unit,
                                    "TR 144 test failed. "
                                    "Error of getting WRED "
                                    "refresh function  Aborting.\n")));
                                soc_ser_test_long_sleep = FALSE;
                                return rv;
                            }
                            wred_refresh = soc_reg_field_get(unit, 
                                FUNCTIONAL_REFRESH_ENr,
                                reg_val, WRED_REFRESH_ENf);
                            if (wred_refresh) {
                                soc_reg_field_set(unit,
                                    FUNCTIONAL_REFRESH_ENr, &reg_val,
                                    WRED_REFRESH_ENf, 0);
                                rv = soc_reg32_set(unit,
                                    FUNCTIONAL_REFRESH_ENr, block_port, 0,
                                    reg_val);
                                if (rv != SOC_E_NONE) {
                                    LOG_CLI((BSL_META_U(unit,
                                        "TR 144 test failed. "
                                        "Error of disabling WRED "
                                        "refresh function  Aborting.\n")));
                                    soc_ser_test_long_sleep = FALSE;
                                    return rv;
                                }
                            }
                        }
                        
                        rv = ser_test_mem_pipe(unit, MISCCONFIGr, -1, 
                                 PARITY_CHECK_ENf,
                                 info[table].mem, testfield, test_type, 
                                 blk, block_port, 
                                 _SOC_ACC_TYPE_PIPE_ANY, &numHwMemErr);
                        
                        if ((memTable == MMU_WRED_AVG_QSIZEm) || 
                            (memTable == MMU_WRED_MARK_THDm)) {
                            /* restore WRED refresh configuration */
                            if (wred_refresh) {
                                soc_reg_field_set(unit,
                                    FUNCTIONAL_REFRESH_ENr, &reg_val,
                                    WRED_REFRESH_ENf, 1);
                                rv = soc_reg32_set(unit,
                                    FUNCTIONAL_REFRESH_ENr, block_port, 0,
                                    reg_val);
                                if (rv != SOC_E_NONE) {
                                    LOG_CLI((BSL_META_U(unit,
                                        "TR 144 test failed. "
                                        "Error of enabling WRED "
                                        "refresh function  Aborting.\n")));
                                    soc_ser_test_long_sleep = FALSE;
                                    return rv;
                                }
                            }
                        }
                    } else {
                        if (memTable == TRUNK_GROUPm) {
                            testfield = EVEN_PARITYf;
                        }
                        rv = ser_test_mem_pipe(unit,info[table].control_reg, -1, 
                                 info[table].enable_field,
                                 info[table].mem, testfield, test_type, 
                                 blk, block_port, 
                                 _SOC_ACC_TYPE_PIPE_ANY, &numHwMemErr);
                    }
                    if (rv != SOC_E_NONE) {
                        LOG_CLI((BSL_META_U(unit,
                                  "TR 144 test failed. "  
                                  "Error during H/W test.  Aborting.\n")));
                        soc_ser_test_long_sleep = FALSE;
                        return rv;
                    }
                }

            }
        }
    }

    if((numTCAMErr != 0) || (numHwMemErr != 0)) {
        LOG_CLI((BSL_META_U(unit,
            "TR 144 test failed.\n")));		
    }

    LOG_CLI((BSL_META_U(unit,
              "Total TCAM errors on unit %d: %d\n"), unit, numTCAMErr));
    LOG_CLI((BSL_META_U(unit,
              "Total H/W parity errors on unit %d: %d\n"),unit, numHwMemErr));
    soc_ser_test_long_sleep = FALSE;
    return SOC_E_NONE;
}

STATIC int soc_gh_tcam_entry_is_invalid(int unit, 
                                                    soc_mem_t mem, int index)
{
    if (soc_feature(unit, soc_feature_field_slice_size128)) {
        if (mem == FP_GLOBAL_MASK_TCAMm ||  mem == FP_TCAMm) {
            if ((index / 64) % 2) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*
 * Function:
 *      soc_gh_ser_inject_error
 * Purpose:
 *       Injects an error into a single GH memory
 * Parameters:
 *      unit               - (IN) Device Number
 *      mem                - (IN) The memory to test
 *      pipeTarget  - (IN) The pipe (x/y) to use when injecting the error
 *      block - (IN) The index into which block will be injected.
 *      index  - (IN) The index into which the error will be injected.
 */
int soc_gh_ser_inject_error(int unit, uint32 flags, soc_mem_t mem,
                            int pipeTarget, int block, int index)
{
    int group, table, i;
    _soc_gh_parity_info_t *info;
    soc_mem_t memTable;
    soc_port_t block_port;
    soc_block_t blk;
    ser_test_data_t test_data;
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], fieldData[SOC_MAX_REG_FIELD_WORDS];
    int overlay_mem = 0;
    _soc_generic_ser_info_t *tcam_ser_info;
    _soc_gh_parity_group_info_t *parity_group_info;

    tcam_ser_info = _soc_gh_tcam_ser_info[unit];
    
    if (tcam_ser_info != NULL) {
        /*TCAM_test*/
        for (i = 0; tcam_ser_info[i].mem != INVALIDm; i++) {
            if (tcam_ser_info[i].mem == mem) {
                /* Check if this entry is invalid or not */
                if (soc_gh_tcam_entry_is_invalid(unit, mem, index)) {
                    return (SOC_E_PARAM);
                }
                soc_ser_create_test_data(unit, tmp_entry, fieldData,
                                 SER_RANGE_ENABLEr, i, VALIDf, mem,
                                 VALIDf, MEM_BLOCK_ANY, REG_PORT_ANY,
                                 _SOC_ACC_TYPE_PIPE_ANY, index, &test_data);
                /*Disable parity*/
                SOC_IF_ERROR_RETURN(
                    _ser_test_parity_control(unit, &test_data, 0));
                /*Read the memory for successful injection*/
                SOC_IF_ERROR_RETURN(ser_test_mem_read(unit, 0, &test_data));
                /*Inject error*/            
                SOC_IF_ERROR_RETURN(soc_ser_test_inject_full(unit, flags,
                                                             &test_data));
                /*Enable parity*/
                SOC_IF_ERROR_RETURN(
                    _ser_test_parity_control(unit, &test_data, 1));
                return SOC_E_NONE;

            }
        }
    }

    parity_group_info = _soc_gh_parity_group_info[unit];

    if (parity_group_info != NULL) {
        /*H/W memory Test*/
        for (group = 0; parity_group_info[group].cpi_bit; group++) {
            info = parity_group_info[group].info;

            SOC_BLOCK_ITER(unit, blk,
                    parity_group_info[group].blocktype) {
                if (_soc_greyhound_parity_block_port(unit, blk, 
                        &block_port) < 0) {
                    continue;
                }

                for (table = 0; info[table].error_field != INVALIDf; table++) {
                    memTable = info[table].mem;
                    if (memTable == INVALIDm) {
                        continue;
                    }
                    if (_soc_greyhound_ser_mem_skip(unit, info[table].feature)) {
                        continue;
                    }
                    /* Check if overlay memory */
                    for (i=0; soc_ser_gh_overlay_mems[i].mem != INVALIDm; i++) {
                        if ((soc_ser_gh_overlay_mems[i].mem == mem) &&
                            (soc_ser_gh_overlay_mems[i].base_mem == memTable)) {
                            overlay_mem = 1;
                            break;
                        }
                    }
                    
                    if ((memTable == mem) || (overlay_mem)) {
                        if((blk == block) || (block == MEM_BLOCK_ANY))
                        {
                            /*Inject error*/
                            test_data.mem = mem;
                            test_data.tcam_parity_bit = -1;
                            if (parity_group_info[group].blocktype 
                                            == SOC_BLK_MMU) {
                                soc_ser_create_test_data(unit, tmp_entry, 
                                         fieldData,
                                         MISCCONFIGr,
                                         SOC_INVALID_TCAM_PARITY_BIT,
                                         PARITY_CHECK_ENf,
                                         mem, EVEN_PARITYf, blk,
                                         block_port, _SOC_ACC_TYPE_PIPE_ANY,
                                         index, &test_data);
                            } else {
                                soc_ser_create_test_data(unit, tmp_entry, 
                                         fieldData,
                                         info[table].control_reg,
                                         SOC_INVALID_TCAM_PARITY_BIT,
                                         info[table].enable_field,
                                         mem, EVEN_PARITYf, blk,
                                         block_port, _SOC_ACC_TYPE_PIPE_ANY,
                                         index, &test_data);
                            }
                            /*Disable parity*/
                            SOC_IF_ERROR_RETURN(_ser_test_parity_control(
                                                unit, &test_data, 0));
                            /*Read the memory for successful injection*/
                            SOC_IF_ERROR_RETURN(ser_test_mem_read(
                                        unit, 0, &test_data));
                            /*Inject error*/
                            SOC_IF_ERROR_RETURN(soc_ser_test_inject_full(
                                        unit, flags, &test_data));
                            /*Enable parity*/
                            SOC_IF_ERROR_RETURN(_ser_test_parity_control(
                                        unit, &test_data, 1));
                        }

                        return SOC_E_NONE;
                    }
                }
            }
        }
    }

    /* The tested memory is not found in SER enabled list, can't inject error of it */
    return SOC_E_UNAVAIL;
}

STATIC soc_error_t
_ser_gh_ser_error_injection_support(int unit, soc_mem_t mem,
                                    int pipe_target)
{
    int rv = SOC_E_UNAVAIL;
    int i = 0, hw_enable_ix = 0;
    int group = 0, table = 0, overlay_mem = 0;
    uint32 range_enable;
    soc_mem_t memTable;
    soc_port_t block_port;
    soc_block_t blk;
    _soc_generic_ser_info_t *tcam_ser_info;
    _soc_gh_parity_group_info_t *parity_group_info;
    _soc_gh_parity_info_t *info;

    LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "unit %d, mem %s, pipe_target %d\n"),
                     unit, SOC_MEM_NAME(unit,mem), pipe_target));

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d, mem %d is INVALID or not valid "
                              "for this unit !!\n"),
                   unit, mem));
        return rv;
    }

    tcam_ser_info = _soc_gh_tcam_ser_info[unit];

    /* Search TCAMs */
    if (tcam_ser_info != NULL) {

        /* Check if enable */
        SOC_IF_ERROR_RETURN
            (READ_SER_RANGE_ENABLEr(unit, &range_enable));
        LOG_DEBUG(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "Search TCAMs: SER_RANGE_ENABLE 0x%X\n"),
                         range_enable));
        for (i = 0; tcam_ser_info[i].mem != INVALIDm; i++) {
            if (tcam_ser_info[i].mem == mem) {
                hw_enable_ix = tcam_ser_info[i].ser_hw_index;
                LOG_DEBUG(BSL_LS_SOC_SER,
                                (BSL_META_U(unit,
                                            "SER Range HW Enable index %d\n"),
                                 hw_enable_ix));
                if (((range_enable >> hw_enable_ix) & 0x1) == 0) {
                    LOG_WARN(BSL_LS_SOC_SER,
                                    (BSL_META_U(unit,
                                                "matched mem but SER detection is disabled\n"
                                                "SER_RANGE_ENABLE 0x%X, SER Range HW index %d\n"),
                                     range_enable,hw_enable_ix));
                    return rv; /* matched mem but ser_detection is disabled */
                }
                LOG_VERBOSE(BSL_LS_SOC_SER,
                                (BSL_META_U(unit,
                                            "TCAM found\n")));
                return SOC_E_NONE; /* found */
            }
        }
    }

    parity_group_info = _soc_gh_parity_group_info[unit];

    /* Search H/W memory */
    if (parity_group_info != NULL) {
        for (group = 0; parity_group_info[group].cpi_bit; group++) {
            info = parity_group_info[group].info;

            SOC_BLOCK_ITER(unit, blk,
                    parity_group_info[group].blocktype) {
                if (_soc_greyhound_parity_block_port(unit, blk,
                        &block_port) < 0) {
                    continue;
                }

                for (table = 0; info[table].error_field != INVALIDf; table++) {
                    memTable = info[table].mem;
                    if (memTable == INVALIDm) {
                        continue;
                    }
                    if (_soc_greyhound_ser_mem_skip(unit, info[table].feature)) {
                        continue;
                    }
                    /* Check if overlay memory */
                    for (i=0; soc_ser_gh_overlay_mems[i].mem != INVALIDm; i++) {
                        if ((soc_ser_gh_overlay_mems[i].mem == mem) &&
                            (soc_ser_gh_overlay_mems[i].base_mem == memTable)) {
                            overlay_mem = 1;
                            LOG_DEBUG(BSL_LS_SOC_SER,
                                            (BSL_META_U(unit,
                                                        "mem %s base_mem %s\n"),
                                             SOC_MEM_NAME(unit,mem),SOC_MEM_NAME(unit,memTable)));
                            break;
                        }
                    }
                    if ((memTable == mem) || (overlay_mem)) {
                        LOG_VERBOSE(BSL_LS_SOC_SER,
                                        (BSL_META_U(unit,
                                                    "H/W memory found\n")));
                        return SOC_E_NONE; /* found */
                    }
                }
            }
        }
    }

    /* The tested memory is not found in SER enabled list */
    return rv;
}

void
soc_gh_ser_test_register(int unit)
{
    /*Initialize chip-specific functions for SER testing*/
    memset(&ser_gh_test_fun, 0, sizeof(soc_ser_test_functions_t));
    ser_gh_test_fun.inject_error_f = &soc_gh_ser_inject_error;
    ser_gh_test_fun.test_mem = &soc_gh_ser_mem_test;
    ser_gh_test_fun.test     = &soc_gh_ser_test;
    ser_gh_test_fun.injection_support = &_ser_gh_ser_error_injection_support;

    soc_ser_test_functions_register(unit, &ser_gh_test_fun);
}
#endif /* SER_TR_TEST_SUPPORT */

/* To write to PGW_GX_CONFIG or PGW_XL_CONFIG register on indicated field */
int 
soc_greyhound_pgw_encap_field_modify(int unit, 
                                soc_port_t lport, 
                                soc_field_t field, 
                                uint32 val) 
{
    int     chk_pgwgx = 0,reg_blk = 0,pindex = -1;
    int     pport = SOC_INFO(unit).port_l2p_mapping[lport];

    if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_GXPORT) {
        /* need to avoid the set been applied to QSGMII port!
         * - QSGMII p2-p5 belong to GXPORT block but not proper for encap set.
         */
        chk_pgwgx = soc_greyhound_pgw_reg_blk_index_get(unit, 
                    PGW_GX_CONFIGr, lport, NULL, &reg_blk, &pindex, 0);
        if (chk_pgwgx > 0){
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, 
                        PGW_GX_CONFIGr, lport, field, val));
        } else {
            return SOC_E_UNAVAIL;
        }
    } else if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_XLPORT) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, 
                    PGW_XL_CONFIGr, lport, field, val));
    } else {
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/* To get from PGW_GX_CONFIG or PGW_XL_CONFIG register on indicated field */
int 
soc_greyhound_pgw_encap_field_get(int unit, 
                                soc_port_t lport, 
                                soc_field_t field, 
                                uint32 *val) 
{
    int     chk_pgwgx = 0,reg_blk = 0,pindex = -1;
    uint32  reg_val = 0;
    int     pport = SOC_INFO(unit).port_l2p_mapping[lport];

    *val = 0;
    if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_GXPORT) {
        /* need to avoid the get to QSGMII port!
         * - QSGMII p2-p5 belong to GXPORT block but not proper for encap get.
         */
        chk_pgwgx = soc_greyhound_pgw_reg_blk_index_get(unit, 
                    PGW_GX_CONFIGr, lport, NULL, &reg_blk, &pindex, 0);
        if (chk_pgwgx > 0){
            SOC_IF_ERROR_RETURN(READ_PGW_GX_CONFIGr(unit, lport, &reg_val));
            *val = soc_reg_field_get(unit, PGW_GX_CONFIGr, reg_val, field);
        } else {
            return SOC_E_UNAVAIL;
        }
    } else if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_XLPORT) {
        SOC_IF_ERROR_RETURN(READ_PGW_XL_CONFIGr(unit, lport, &reg_val));
        *val = soc_reg_field_get(unit, PGW_XL_CONFIGr, reg_val, field);
    } else {
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}


/*
 * soc_gh_eee_timers_setup :
 * - EEE timers in different MAC noramlly has it own reset default but need
 *    other proper value per device.
 * - EEE timer is speed dependent.
 */
int
soc_gh_port_eee_timers_setup(int unit, soc_port_t port, int speed)
{
    uint32  wake_timer = 0, lpi_timer = 0;
 
    /* Native EEE feature supporting for UNIMAC/XLMAC:
     *  - Assigning proper EEE timer to work with Ext-PHY
     *      - Wake timer :
     *          a. Speed 100 : 30 us
     *          b. Speed 1G : 16.5 us
     *          c. Speed 10G : 7.6 us
     *      - Delay Entry timer :
     *          a. Speed 00 : 40 us
     *          b. Speed 1G : 4 us
     *          c. Speed 10G : f0.4 us
     *
     * NOTE :
     *  1. Ext-PHY with EEE support speed in 100/1G/10G currently.
     */
 
    /* EEE wake timer (unit of uSec.) */
    wake_timer = ((speed >= 10000) ? 7 :
            ((speed >= 1000) ? 17 : 30));
    /* EEE dealy entry timer (unit of uSec.) */
    lpi_timer = ((speed >= 10000) ? 1 :
            ((speed >= 1000) ? 4 : 40));
 
    if (IS_XL_PORT(unit, port)) {
        /* TSC-E mode : for XLMAC seting */
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                XLMAC_EEE_TIMERSr, port, EEE_WAKE_TIMERf, wake_timer));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                XLMAC_EEE_TIMERSr, port, EEE_DELAY_ENTRY_TIMERf, lpi_timer));
    } else {
        /* QSGMII-E mode : for UNIMAC setting
            *
            * INFO : UNIMAC EEE timer per
            */
        /* GE speed EEE timer (unit of uSec.) */
        wake_timer = 17;
        lpi_timer = 4;
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                GMII_EEE_WAKE_TIMERr, port,
                GMII_EEE_WAKE_TIMERf, wake_timer));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                GMII_EEE_DELAY_ENTRY_TIMERr, port,
                GMII_EEE_LPI_TIMERf, lpi_timer));
        /* FE speed EEE timer (unit of uSec.) */
        wake_timer = 30;
        lpi_timer = 40;
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                MII_EEE_WAKE_TIMERr, port,
                MII_EEE_WAKE_TIMERf, wake_timer));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                MII_EEE_DELAY_ENTRY_TIMERr, port,
                MII_EEE_LPI_TIMERf, lpi_timer));
    }
    return SOC_E_NONE;
}
 
/* EEE reference count init :
 * - The assignment of EEE_REF_COUNT and EEE Wake/LPI timer
 */
int
soc_gh_port_eee_ref_count_init(int unit, soc_port_t port)
{
    /* assigning EEE_REF_COUNT */
    if (IS_XL_PORT(unit, port)) {
        /* TSC-E mode : for XLMAC EEE_REF_COUNT */
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                XLMAC_EEE_TIMERSr, port, EEE_REF_COUNTf,
                GH_XLMAC_EEE_REF_CNT));
    } else {
        /* QSGMII-E mode : for UNIMAC EEE_REF_COUNT */
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                UMAC_EEE_REF_COUNTr, port, EEE_REF_COUNTf,
                GH_UNIMAC_EEE_REF_CNT));
    }
    return SOC_E_NONE;
}

/*
 * Greyhound chip driver functions.  
 */
soc_functions_t soc_greyhound_drv_funs = {
    _soc_greyhound_misc_init,
    _soc_greyhound_mmu_init,
    _soc_greyhound_age_timer_get,
    _soc_greyhound_age_timer_max_get,
    _soc_greyhound_age_timer_set,
    _soc_greyhound_tsce_firmware_set,
    _soc_greyhound_mdio_reg_read,
    _soc_greyhound_mdio_reg_write,
};

#endif /* BCM_GREYHOUND_SUPPORT */

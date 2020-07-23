/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.  
 */
#ifndef __DINO_TYPES_H__
#define __DINO_TYPES_H__

/* 
 * Includes
 */
#include <phymod/phymod_types.h>
/*
 *  Defines
 */
#define ALL_PORTS ((int)-1)
#define ALL_LANES ((int)-1)
#define ALL_DIES  ((int)-1)
#define BOTH_SYS_LINE ((CHIP_SYSEN_E)-1)


#define DINO_FW_ALREADY_DOWNLOADED      0xFAD

/* 
 *  Types
 */
typedef enum {
    DEV_PMA = 1,
    DEV_AN = 7
} DEVID_E;

typedef enum {
    LINE=0, 
    SYS=1
} CHIP_SYSEN_E;

/* Furia Transmiter/Receiver Types. */
typedef enum {
  DUPLEX_XCVR,  /* Duplex transceiver */
  SIMPLEX_TX,   /* Simplex transmitter */
  SIMPLEX_RX    /* Simplex Receiver */
} DINO_XCVR_TYPE_E;

/* Furia PHY Types. */
typedef enum {
  PHY_BP   = 0x000, /* Phy Back Plane */
  PHY_FP   = 0x001, /* Phy Front Plane */
  PHY_OTN  = 0x002, /* Phy OTN Plane */
  PHY_FC   = 0x003, /* Phy Fiber Channel Plane */
  PHY_PROP = 0x004, /* Phy Proprietary */
  PHY_INVALID = -1  /* Invalid value */
} DINO_PHY_TYPE_E;


/* Reference Clock Options. */
typedef enum {
    REF_CLK_106p25MHz = 10,
    REF_CLK_174p703125MHz = 20,
    REF_CLK_156p25Mhz = 30,
    REF_CLK_125Mhz = 40,
    REF_CLK_156p637Mhz = 50,
    REF_CLK_161p1328125Mhz = 60,
    REF_CLK_168p04Mhz = 70,
    REF_CLK_172p64Mhz = 80,
    REF_CLK_INVALID = -1
} DINO_REF_CLK_E;


/* AVDD TX Driver */
typedef enum {
    AVDDTXDRV_1p25 = 0,
    AVDDTXDRV_1p0 = 1 
} FALCON_AVDDTXDRV_E;


/** Falcon PRBS Mode.
*/
typedef enum {
    PRBS7      =  0,    
    PRBS9      =  1,    
    PRBS11     =  2,    
    PRBS15     =  3,    
    PRBS23     =  4,    
    PRBS31     =  5,    
    PRBS58     =  6    
} FALCON_PRBS_E;

	
/** Furia Package Sides Type.
 * Package side is defined as:
 * For Duplex Chips:
 *   Side A = Line Side
 *   Side B = System Side
 * For Simplex Chips:
 *   Side A = TX Side
 *   Side B = RX Side
*/
typedef enum {
    SIDE_A   = 0,
    SIDE_B   = 1,
    ALL_SIDES = 10
} DINO_PKG_SIDE_E;


/** Furia Physical Lane Descriptor
 *  Port configuration set by package.
*/
struct _DINO_PKG_LANE_CFG_S {
    uint32_t chip_id;              /* Chip id lane belongs to */    
    uint16_t pkg_lane_num;         /* Package lane number. */ 
    DINO_XCVR_TYPE_E  lane_type;  /* Transceiver, Receiver, Transmitter. */
    uint16_t  die_addr;            /* Die Address used in Multiple die packages. */
    uint16_t  die_lane_num;        /* Die Internal Physical lane number. */
    uint16_t  slice_rd_val;        /* Value to be written to 
                                      Slice.rd_lane register field. */
    uint16_t  slice_wr_val;        /* Value to be written to 
                                      Slice.wr_lane register field. */
    CHIP_SYSEN_E  sideB;           /* Duplex System Side / Simplex RX side */
    CHIP_SYSEN_E  sideA;           /* Duplex Line Side / Simplex TX side */
    uint16_t  inv_in_data;         /* 0: Input Data is not inverted in PKG. 
                                      1: Input Data is inverted in PKG */
    uint16_t  inv_out_data;        /* 0: Output Data is not inverted in PKG.
                                      1: Output Data is inverted in PKG */
};
typedef struct _DINO_PKG_LANE_CFG_S DINO_PKG_LANE_CFG_t;

/** Furia PHY ID list
 * Database for PHYs 
*/
struct _DINO_PHY_LIST_S {
    uint32_t phy_id;               
    uint32_t valid;
    uint32_t tx_lanes;
};
typedef struct _DINO_PHY_LIST_S DINO_PHY_LIST_t;
/** Falcon PLL Mode.
*/
typedef enum {
  PLL_MODE_64  = 0x0, /*5'b00000, */
  PLL_MODE_66  = 0x1, /*5'b00001, */
  PLL_MODE_80  = 0x2, /*5'b00010, */
  PLL_MODE_96  = 0x10, /*5'b10000, */
  PLL_MODE_120 = 0x11, /*5'b1001, */
  PLL_MODE_128 = 0x3, /*5'b00011, */
  PLL_MODE_132 = 0x4, /*5'b00100, */
  PLL_MODE_140 = 0x5, /*5'b00101, */
  PLL_MODE_160 = 0x6, /*5'b00110, */
  PLL_MODE_165 = 0x7, /*5'b00111, */
  PLL_MODE_168 = 0x8, /*5'b01000, */
  PLL_MODE_170 = 0x9, /*5'b01001, */
  PLL_MODE_175 = 0xa, /*5'b01010, */
  PLL_MODE_180 = 0xb, /*5'b01011, */
  PLL_MODE_184 = 0xc, /*5'b01100, */
  PLL_MODE_200 = 0xd, /*5'b01101, */
  PLL_MODE_224 = 0xe, /*5'b01110, */
  PLL_MODE_264 = 0xf, /*5'b01111 */
  PLL_MODE_INVALID = -1
} FALCON_PLL_MODE_E;

/** Falcon OSX Mode.
*/
typedef enum {
     F_OSX1      =  0,
     F_OSX2      =  1,
     F_OSX4      =  2,
     F_OSX16P5   =  8,
     F_OSX20P625 =  12
} FALCON_OSX_MODE_E;


typedef enum {
  UNICAST=0x0,
  MULTICAST01=0x3,
  MULTICAST23=0xC,
  BROADCAST=0xF
} CHIP_BROADCAST_E;

typedef enum {
    SPEED_1G = 1000,
    SPEED_4P25G = 4250,
    SPEED_6G = 6000,
    SPEED_7P5G = 7500,
    SPEED_8P5G = 8500,
    SPEED_10G = 10000,
    SPEED_11G = 11000,
    SPEED_11P5G = 11500,
    SPEED_12P5G = 12500,
    SPEED_14G = 14000,
    SPEED_15G = 15000,
    SPEED_20G = 20000,
    SPEED_21G = 21000,
    SPEED_23G = 23000,
    SPEED_25G = 25000,
    SPEED_27G = 27000,
    SPEED_28G = 28000,
    SPEED_40G = 40000,
    SPEED_42G = 42000,
    SPEED_50G = 50000,
    SPEED_53G = 53000,
    SPEED_100G = 100000,
    SPEED_106G = 106000
} DINO_SPEED_E;

typedef struct {
    uint32_t rptr_rtmr_mode;
    uint32_t avdd_txdrv;
    uint32_t ull_dp;
    uint32_t an_master_lane;
}DINO_DEVICE_AUX_MODE_T;
/** Module controller I2C master commands */
typedef enum {
  DINO_FLUSH   = 0,
  DINO_RANDOM_ADDRESS_READ = 1,
  DINO_CURRENT_ADDRESS_READ = 2,
  DINO_I2C_WRITE = 3
} DINO_I2CM_CMD_E;

/** Furia AN - HCD link criteria
 */
typedef enum {
    DINO_HCD_LINK_REG_WR         = 0x0,
    DINO_HCD_LINK_KR_BLOCK_LOCK  = 0x1,
    DINO_HCD_LINK_KR4_BLOCK_LOCK = 0x2,
    DINO_HCD_LINK_KR4_PMD_LOCK   = 0x3,
    DINO_HCD_LINK_EXT_PCS        = 0x4,
    DINO_HCD_LINK_DEFAULT        = 0x5
} DINO_HCD_LINK_CRITERIA_E;

typedef enum {
    PLL_LOCK_FOUND = 1<<0,
    PLL_LOCK_LOST = 1<<1,
    SIGDET_FOUND = 1<<2,
    SIGDET_LOST = 1<<3,
    PMD_LOCK_FOUND = 1<<4,
    PMD_LOCK_LOST = 1<<5,
    AN_COMPLETE = 1<<6,
    AN_RESTARTED = 1<<7,
    INT_DEMUX_FIFOERR = 1<<8,
    INT_MUX_FIFOERR = 1<<9
} DINO_INTR_TYPE_E;

typedef enum {
    LINE_LINK_CORE_0,
    LINE_LINK_CORE_1,
    LINE_LINK_CORE_2,
    SYS_LINK_CORE_0,
    SYS_LINK_CORE_1,
    SYS_LINK_CORE_2,
    AN_CORE_0_1_2,
    DP_MUX_DEMUX_FIFO
} DINO_INTR_GROUP_E;

typedef enum  {
    DINO_TX_REPEATER      = 0, /**< Tx repeater mode, chip default  */
    DINO_TX_REPEATER1     = 1, /**< Alternative value for Tx repeater mode  */
    DINO_TX_RETIMER       = 2, /**< Tx retimer mode  */
    DINO_TX_REPEATER_ULL  = 3  /**< Tx repeater mode with ULL data path  */
} DINO_TX_TIMING_MODE_E;

#endif

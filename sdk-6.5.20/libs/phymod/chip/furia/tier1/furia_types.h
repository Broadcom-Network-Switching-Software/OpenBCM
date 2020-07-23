/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.  
 */
#ifndef __FURIA_TYPES_H__
#define __FURIA_TYPES_H__

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


#define FURIA_FW_ALREADY_DOWNLOADED      0xFAD

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
} FURIA_XCVR_TYPE_E;

typedef struct {
    unsigned short otn_type;
}FURIA_DEVICE_OTN_MODE_T;


/* Furia PHY Types. */
typedef enum {
  PHY_BP   = 0x000, /* Phy Back Plane */
  PHY_FP   = 0x001, /* Phy Front Plane */
  PHY_OTN  = 0x002, /* Phy OTN Plane */
  PHY_FC   = 0x003, /* Phy Fiber Channel Plane */
  PHY_PROP = 0x004, /* Phy Proprietary */
  PHY_INVALID = -1  /* Invalid value */
} FURIA_PHY_TYPE_E;


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
} FURIA_REF_CLK_E;


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
} FURIA_PKG_SIDE_E;


/** Furia Physical Lane Descriptor
 *  Port configuration set by package.
*/
struct _FURIA_PKG_LANE_CFG_S {
    uint32_t chip_id;              /* Chip id lane belongs to */    
    uint16_t pkg_lane_num;         /* Package lane number. */ 
    FURIA_XCVR_TYPE_E  lane_type;  /* Transceiver, Receiver, Transmitter. */
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
typedef struct _FURIA_PKG_LANE_CFG_S FURIA_PKG_LANE_CFG_t;

/** Furia PHY ID list
 * Database for PHYs 
*/
struct _FURIA_PHY_LIST_S {
    uint32_t phy_id;               
    uint32_t valid;
    uint32_t tx_lanes;
};
typedef struct _FURIA_PHY_LIST_S FURIA_PHY_LIST_t;
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

typedef enum PHY82381_INTERRUPT {
            INT_TIMESYNC_FRAMESYNC,
            INT_TIMESYNC_TIMESTAMP,
            INT_M0_SLV_MISC,
            INT_M0_MST_MISC,
            INT_M0_SLV_MSGOUT,
            INT_M0_MST_MSGOUT,
            INT_MUX_PMDLOCKLOST,
            INT_MUX_PMDLOCKFOUND,
            INT_MUX_SIGDETLOST,
            INT_MUX_SIGDETFOUND,
            INT_MUX_PCSMONLOCKLOST,
            INT_MUX_PCSMONLOCKFOUND,
            INT_MUX_FIFOERR,
            INT_MUX_CL73_RESTARTED,
            INT_MUX_CL73_COMPLETE,
            INT_MUX_CL73_RESTARTEDP2,
            INT_MUX_CL73_RESTARTEDP1,
            INT_MUX_CL73_COMPLETEP2,
            INT_MUX_CL73_COMPLETEP1,
            INT_CL73_AN_RESTARTED_L0,
            INT_CL73_AN_RESTARTED_L1,
            INT_CL73_AN_RESTARTED_L2,
            INT_CL73_AN_RESTARTED_L3,
            INT_CL73_AN_COMPLETE_L0,
            INT_CL73_AN_COMPLETE_L1,
            INT_CL73_AN_COMPLETE_L2,
            INT_CL73_AN_COMPLETE_L3,
            INT_DEMUX_PMDLOCKLOST,
            INT_DEMUX_PMDLOCKFOUND,
            INT_DEMUX_SIGDETLOST,
            INT_DEMUX_SIGDETFOUND,
            INT_DEMUX_PCSMONLOCKLOST,
            INT_DEMUX_PCSMONLOCKFOUND,
            INT_DEMUX_FIFOERR,
            INT_DEMUX_CL73_RESTARTED,
            INT_DEMUX_CL73_COMPLETE,
            INT_PLL_10G_LOCKLOST,
            INT_PLL_10G_LOCKFOUND,
            INT_PLL_25G_LOCKLOST,
            INT_PLL_25G_LOCKFOUND,
            INT_SYS_PLL_LOCKLOST,
            INT_SYS_PLL_LOCKFOUND,
            INT_LINE_PLL_LOCKLOST,
            INT_LINE_PLL_LOCKFOUND,
            PHY82381_MAX_INT
}PHY82381_INTERRUPT_T;

typedef enum {
        INT_M0_SLV_SYSRESET_REQ = 0,
        INT_M0_MST_SYSRESET_REQ,
        INT_M0_SLV_LOCKUP,
        INT_M0_MST_LOCKUP,
        INT_M0_SLV_MISC_INTR,
        INT_M0_MST_MISC_INTR,
        INT_M0_SLV_MSGOUT_INTR,
        INT_M0_MST_MSGOUT_INTR,
        INT_M0_SLV_DED,
        INT_M0_SLV_SEC,
        INT_M0_MST_DED,
        INT_M0_MST_SEC,
        INT_SYS_1GCDR_LOCK_LOST,
        INT_SYS_1GCDR_LOCK_FOUND,
        INT_SYS_PMD_LOCK_LOST,
        INT_SYS_PMD_LOCK_FOUND,
        INT_SYS_RX_SIGDET_LOST,
        INT_SYS_RX_SIGDET_FOUND,
        INT_SYS_PLL_LOCK_LOST,
        INT_SYS_PLL_LOCK_FOUND,
        INT_SYS_PMI_ARB_WDOG_EXP,
        INT_SYS_PMI_M0_WDOG_EXP,
        INT_LINE_1GCDR_LOCK_LOST,
        INT_LINE_1GCDR_LOCK_FOUND,
        INT_LINE_PMD_LOCK_LOST,
        INT_LINE_PMD_LOCK_FOUND,
        INT_LINE_RX_SIGDET_LOST,
        INT_LINE_RX_SIGDET_FOUND,
        INT_LINE_PLL_LOCK_LOST,
        INT_LINE_PLL_LOCK_FOUND,
        INT_LINE_PMI_ARB_WDOG_EXP,
        INT_LINE_PMI_M0_WDOG_EXP,
        INT_CL91_RX_ALIGN_LOST,
        INT_CL91_RX_ALIGN_FOUND,
        INT_CL73_AN_RESTARTED,
        INT_CL73_AN_COMPLETE,
        INT_LINE_RX_PCSFC_MON_LOCK_LOST,
        INT_LINE_RX_PCSFC_MON_LOCK_FOUND,
        INT_SYS_TX_FIFOERR,
        INT_LINE_RX_FIFOERR,
        INT_CL73_AN_RESTARTED_0,
        INT_CL73_AN_RESTARTED_1,
        INT_CL73_AN_RESTARTED_2,
        INT_CL73_AN_RESTARTED_3,
        INT_CL73_AN_COMPLETE_0,
        INT_CL73_AN_COMPLETE_1,
        INT_CL73_AN_COMPLETE_2,
        INT_CL73_AN_COMPLETE_3,
        INT_CL91_TX_ALIGN_LOST,
        INT_CL91_TX_ALIGN_FOUND,
        INT_ONEG_INBAND_MSG_LOST,
        INT_ONEG_INBAND_MSG_FOUND,
        INT_SYS_RX_PCSFC_MON_LOCK_LOST,
        INT_SYS_RX_PCSFC_MON_LOCK_FOUND,
        INT_LINE_TX_FIFOERR,
        INT_SYS_RX_FIFOERR
} FURIA_INTR_TYPE_E;
typedef enum {
       PCS49_1x10G   = 0x0, /* 10G-KR/LR/SR */
       PCS82_4x10G   = 0x1, /* 40G-KR4/LR4/SR4/CR4 */
       PCS82_2x25G   = 0x2, /* 50G-KR2 */
       PCS82_4x25G   = 0x3, /* 100G-KR4/LR4/CR4 */
       FC4           = 0x4,
       FC8           = 0x5,
       FC16          = 0x6,
       FC32          = 0x7,
       DISABLE_FCPCS_CHKR = -1
} FURIA_FCPCS_CHKR_E;

typedef enum {
    SPEED_1G = 1000,
    SPEED_4P25G = 4250,
    SPEED_6G = 6000,
    SPEED_7P5G = 7500,
    SPEED_8P5G = 8500,
    SPEED_9P95G= 9950,
    SPEED_10G = 10000,
    SPEED_10P5G = 10500,
    SPEED_10P518G = 10518,
    SPEED_10P7G = 10700,
    SPEED_11G = 11000,
    SPEED_11P18G = 11180,
    SPEED_11P25G = 11250,
    SPEED_11P5G = 11500,
    SPEED_12P5G = 12500,
    SPEED_14G = 14000,
    SPEED_14P025G = 14025,
    SPEED_15G = 15000,
    SPEED_20G = 20000,
    SPEED_21G = 21000,
    SPEED_23G = 23000,
    SPEED_25G = 25000,
    SPEED_27G = 27000,
    SPEED_28G = 28000,
    SPEED_28P05G = 28050,
    SPEED_40G = 40000,
    SPEED_42G = 42000,
    SPEED_43G = 43000,
    SPEED_44P57G = 44570,
    SPEED_44P583G = 44583,
    SPEED_50G = 50000,
    SPEED_53G = 53000,
    SPEED_100G = 100000,
    SPEED_106G = 106000,
    SPEED_111G = 111000
} FURIA_SPEED_E;

typedef struct {
    uint32_t rptr_rtmr_mode;
    uint32_t avdd_txdrv;
    uint32_t ull_dp;
    uint32_t an_master_lane;
}FURIA_DEVICE_AUX_MODE_T;
/** Module controller I2C master commands */
typedef enum {
  FURIA_FLUSH   = 0,
  FURIA_RANDOM_ADDRESS_READ = 1,
  FURIA_CURRENT_ADDRESS_READ = 2,
  FURIA_I2C_WRITE = 3
} FURIA_I2CM_CMD_E;

/** Furia AN - HCD link criteria
 */
typedef enum {
    FURIA_HCD_LINK_REG_WR         = 0x0,
    FURIA_HCD_LINK_KR_BLOCK_LOCK  = 0x1,
    FURIA_HCD_LINK_KR4_BLOCK_LOCK = 0x2,
    FURIA_HCD_LINK_KR4_PMD_LOCK   = 0x3,
    FURIA_HCD_LINK_EXT_PCS        = 0x4,
    FURIA_HCD_LINK_DEFAULT        = 0x5
} FURIA_HCD_LINK_CRITERIA_E;

#endif

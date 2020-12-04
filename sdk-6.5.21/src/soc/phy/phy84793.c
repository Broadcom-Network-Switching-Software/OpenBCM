/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy84793.c
 * Purpose:    Phys Driver support for Broadcom 84793 100Gig
 *             gearbox.
 * Note:
 */

#if defined(INCLUDE_PHY_84793)
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <shared/bsl.h>
#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phydefs.h"      /* Must include before other phy related includes */

#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyxehg.h"
#include "phy84793.h"

#define MSGOUT_BIT_POS                        (1<< 5)
#define PHY84793_ID_84793                     0x84793
#define PHY84793_ID_84790                     0x84790
#define PHY84793_ID_84794                     0x84794
#define PHY84793_DC_HC_RD_WIT_OUT_CLEAR       0
#define PHY84793_DC_HC_RD_WIT_CLEAR           1
#define PHY84793_FW_DLOAD_OFFSET              0
#define PHY84793_FW_DLOAD_RETRY_CNT           5
#define PHY84793_CDR_LOCK                     1
#define PHY84793_CDR_UNLOCK                   0
#define PHY84793_DC_LANE0                     0
#define PHY84793_DC_LANE9                     9 
#define PHY84793_HC_LANE0                     0
#define PHY84793_HC_LANE5                     5 
#define PHY84793_VSR40_LANE0                  0 
#define PHY84793_VSR40_LANE3                  3 
#define PHY84793_LN_MAX_NO_LANE               10 
#define PHY84793_IF_CAUI                      0x2A	       
#define PHY84793_IF_XLAUI                     0x22
#define PHY84793_IF_SR10                      0x29
#define PHY84793_IF_SR                        0x7
#define PHY84793_IF_OTL                       0x4
#define PHY84793_POL_DND                      0xFFFF  
#define PHY84793_ENABLE_SET_DND               0xFFFF   
#define PHY84793_SLICE_0                      0   
#define PHY84793_SLICE_1                      0x1   
#define VSR40_TX_ANA_LANE0_BASE               0x9940 
#define VSR40_TX_ANA_LANE1_BASE               0x9950 
#define VSR40_TX_ANA_LANE2_BASE               0x9960 
#define VSR40_TX_ANA_LANE3_BASE               0x9970 
#define PHY84793_MODE1_SPD                    100000 
#define PHY84793_MODE2_SPD                    40000 
#define PHY84793_MODE3_SPD                    100000 
#define PHY84793_MODE4_SPD                    10000
#define PHY84793_MODE5_SPD                    100000
#define PHY84793_ENABLE_ALL_LANES             0x3ff
#define PHY84793_DISABLE_ALL_LANES            0x0
#define POL_LANE_MASK(lane)                  (1 << (lane))
#define PHY84793_16BIT_MSB                   0xFFFF0000
#define PHY84793_16BIT_SHIFT                 16
#define PHY84793_16BIT_LSB                   0x0000FFFF
#define PHY84793_HC_RX_BIT                   2
#define PHY84793_DC_ENABLE                   1
#define PHY84793_HC_READ_ENABLE              0x12
#define PHY84793_HC_WRITE_ENABLE             0xA
#define PHY84793_HC_PCB_CLEAR                0x32
#define PHY84793_DC_PCB_CLEAR                0x31
#define PHY84793_DC_READ_ENABLE              0x11
#define PHY84793_DC_WRITE_ENABLE             0x9
#define VSR40_RX_ANA_LANE0_BASE              0x9480 
#define VSR40_RX_ANA_LANE1_BASE              0x9580 
#define VSR40_RX_ANA_LANE2_BASE              0x9680 
#define VSR40_RX_ANA_LANE3_BASE              0x9780
#define PHY84793_PRBS_INV_BIT_MASK           (1<<13)
#define PHY84793_PRBS_INV_BIT_POS            13
#define PHY84793_EYESCAN_MAX_LEFT_HOFFSET    -32
#define PHY84793_EYESCAN_MAX_RIGHT_HOFFSET    31
#define PHY84793_EYESCAN_MIN_VOFFSET         -31
#define PHY84793_EYESCAN_MAX_VOFFSET          31
#define PHY84793_DC_HC_BROADCAST              0x1FF
#define PHY84793_REF_CLK_156_25               0 
#define PHY84793_REF_CLK_161_132              1
#define PHY84793_REF_CLK_174_7                2  
#define PHY84793_REF_CLK_312_5                3
#define PHY84793_REF_CLK_322_265              4
#define PHY84793_REF_CLK_349_4                5
#define PHY84793_REF_CLK_625_0                6
#define PHY84793_REF_CLK_644_531              7
#define PHY84793_REF_CLK_698_8                8
#define VSR40_RX_ANA_LANE0_BASE               0x9480
#define VSR40_RX_ANA_IC_CTRL_REG              0xC
#define VSR40_RX_ANA_CTRL1_REG                0xA
#define HS_RX_CDR_LANE_BASE                   0x9400
#define HS_RX_CDR_STAT_INTG_REG               0x15
#define HS_RX_CDR_STAT_NCO_RDP                0x16
 
/* macros for Eye monitor start */
#define DECA_RT 1
#define HEXA_RX 2
#define VSR_RX  3
#define HEXA_TX 4

#define ENABLE_EYE_MON  2
#define RUN_EYE_MON     4
#define DISABLE_EYE_MON 6

#define BAD_EYE_MON_CORE 3
#define BAD_EYE_MON_CMD  5
#define BAD_EYE_MON_MODE 7

#define ALL_LANES_IN_CORE 0x1FF

/* macros for Eye monitor Ends */

#define PHY_84793_HCTX_PCB_WRITE(UNIT, PC, ADDR, VALUE) \
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC, \
                    CTL_REGS_PCB_ADR_MSW_TYPE, (((ADDR) & PHY84793_16BIT_MSB)>>16)));\
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC,\
                    CTL_REGS_PCB_ADR_LSW_TYPE, ((ADDR) & PHY84793_16BIT_LSB)));\
    sal_usleep(50);\
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC,\
                    CTL_REGS_PCB_DATA_IN_TYPE, (VALUE)));\
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC,\
                    CTL_REGS_PCB_CTRL_TYPE, 0x4));\
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC, \
                    CTL_REGS_PCB_CTRL_TYPE, 0xC));\
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC, \
                    CTL_REGS_PCB_CTRL_TYPE, 0x0));

#define PHY_84793_HCTX_PCB_READ(UNIT, PC, ADDR, VAL) \
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC, \
                    CTL_REGS_PCB_ADR_MSW_TYPE, (((ADDR) & PHY84793_16BIT_MSB)>>16)));\
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC,\
                    CTL_REGS_PCB_ADR_LSW_TYPE, ((ADDR) & PHY84793_16BIT_LSB)));\
    sal_usleep(50);\
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC, \
                    CTL_REGS_PCB_CTRL_TYPE, 0x4));\
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC, \
                    CTL_REGS_PCB_CTRL_TYPE, 0x14));\
    SOC_IF_ERROR_RETURN \
         (READ_PHY84793_PMA_PMD_REG(UNIT, PC, CTL_REGS_PCB_DATA_OUT_TYPE, VAL));\
    SOC_IF_ERROR_RETURN (WRITE_PHY84793_PMA_PMD_REG(UNIT, PC, \
                    CTL_REGS_PCB_CTRL_TYPE, 0x0));


#define GET_BIT_VAL(val, msb, lsb)  ((val) & (((int32)power(2, \
			(msb +1))) - ((int32)power(2,lsb))))/((int32)power(2,lsb))

#define SET_PHY84793_SLICE(_unit, _phy_ctrl, _slice) \
           WRITE_PHY84793_PMA_PMD_REG(_unit, _phy_ctrl, SLICE_REGS_SLICE , \
			              _slice)

#define PHY84793_GET_PORT_MODE(_pc, MODE) \
    if ((((phy84793_dev_desc_t *)((_pc) + 1))->intf.type == SOC_PORT_IF_SR) &&  \
	    (((phy84793_dev_desc_t *)((_pc) + 1))->intf.speed == PHY84793_MODE4_SPD))\
        MODE = PHY84793_MODE_10X10G_TO_10X10G_ASYNC;\
    else if ((((phy84793_dev_desc_t *)((_pc) + 1))->intf.type == SOC_PORT_IF_SR4) &&  \
            (((phy84793_dev_desc_t *)((_pc) + 1))->intf.speed == PHY84793_MODE1_SPD))\
        MODE = PHY84793_MODE_10X10G_TO_4X25G;\
    else if ((((phy84793_dev_desc_t *)((_pc) + 1))->intf.type == SOC_PORT_IF_SR4) && \
  	     (((phy84793_dev_desc_t *)((_pc) + 1))->intf.speed == PHY84793_MODE2_SPD))\
        MODE = PHY84793_MODE_4X10G_TO_4X10G;\
    else if ((((phy84793_dev_desc_t *)((_pc) + 1))->intf.type == SOC_PORT_IF_SR10) && \
             (((phy84793_dev_desc_t *)((_pc) + 1))->intf.speed == PHY84793_MODE3_SPD))\
        MODE = PHY84793_MODE_10X10G_TO_10X10G_SYNC;\
    else if ((((phy84793_dev_desc_t *)((_pc) + 1))->intf.type == SOC_PORT_IF_OTL) && \
             (((phy84793_dev_desc_t *)((_pc) + 1))->intf.speed == PHY84793_MODE5_SPD))\
        MODE = PHY84793_MODE_10X10G_TO_4X25G_OTL;

#define PHY84793_EYE_MON_PARAMS(core, command, lane_mask, PARAMS) \
	do { \
                PARAMS = core; \
	        PARAMS = PARAMS << 3; \
	        PARAMS = PARAMS | command; \
	        PARAMS = PARAMS <<10; \
	        PARAMS = PARAMS | lane_mask; \
          }while(0)

extern unsigned char phy84793_ucode_bin[];
extern unsigned int phy84793_ucode_bin_len;
static const char *phy84793_intf_names[] = SOC_PORT_IF_NAMES_INITIALIZER;

soc_port_if_t phy_84793_sys_to_port_if[] = {
    0,  
    SOC_PORT_IF_SR4,
    SOC_PORT_IF_SR10, 
    SOC_PORT_IF_SR,
    SOC_PORT_IF_OTL,    
};

typedef struct phy84793_polarity_cfg {
    uint16  tx_pol_cfg;
    uint16  rx_pol_cfg;
}phy84793_polarity_cfg;

typedef struct phy84793_intf_cfg_t {
    int32	             speed;
    soc_port_if_t    type;
} phy84793_intf_cfg_t ;

typedef struct {
    uint32                            devid;
    phy84793_intf_cfg_t               intf;
    phy84793_polarity_cfg             ln_pol_cfg;
    phy84793_polarity_cfg             sys_pol_cfg;
    uint16                            lb_stat;
    char                              dev_name_84793[15];
    uint16                            pol_tx_cfg;
    uint16                            pol_rx_cfg;
} phy84793_dev_desc_t;

#define DEVID(_pc)           (((phy84793_dev_desc_t *)((_pc) + 1))->devid)
#define INTERFACE(_pc)       (((phy84793_dev_desc_t *)((_pc) + 1))->intf)
#define SYS_POLARITY(_pc)    (((phy84793_dev_desc_t *)((_pc) + 1))->sys_pol_cfg)
#define LN_POLARITY(_pc)     (((phy84793_dev_desc_t *)((_pc) + 1))->ln_pol_cfg)
#define LOOPBACK(_pc)        (((phy84793_dev_desc_t *)((_pc) + 1))->lb_stat)
#define DEV_NAME(_pc)        (((phy84793_dev_desc_t *)((_pc) + 1))->dev_name_84793)
#define POL_TX_CFG(_pc)	     (((phy84793_dev_desc_t *)((_pc) + 1))->pol_tx_cfg)
#define POL_RX_CFG(_pc)      (((phy84793_dev_desc_t *)((_pc) + 1))->pol_rx_cfg)

#define CHECK_LANE(_l)       \
	if (_l > PHY84793_DC_LANE9)\
           return SOC_E_PARAM;

typedef enum PHY84793_CORES {
    PHY84793_DECA_CORE = 0,
    PHY84793_HEXA_CORE
}PHY84793_CORES;

typedef enum PHY84793_INTF_SIDE {
    PHY84793_SYS_SIDE = 0,
    PHY84793_LINE_SIDE
}PHY84793_INTF_SIDE;

typedef enum PHY84793_MODES {
    PHY84793_MODE_10X10G_TO_4X25G = 0,  /* 10X10G to 4X25G (1 Port)*/
    PHY84793_MODE_4X10G_TO_4X10G,      /* 4X10G to 4X10G  (2 port)*/
    PHY84793_MODE_10X10G_TO_10X10G_SYNC,      /* 10X10G to 10X10G (1 port)*/
    PHY84793_MODE_10X10G_TO_10X10G_ASYNC,      /* 10X10G to 10X10G (10 ports)*/
    PHY84793_MODE_10X10G_TO_4X25G_OTL 
}PHY84793_MODES;

typedef enum PHY84793_PRBS_PATT {
    PHY84793_PRBS_7,
    PHY84793_PRBS_15,
    PHY84793_PRBS_23,
    PHY84793_PRBS_31,
    PHY84793_PRBS_9,
    PHY84793_PRBS_11,
    PHY84793_PRBS_FIXED_PATT
}PHY84793_PRBS_PATT;
		   
typedef enum PHY84793_PRBS_LOCK {
    PHY84793_PRBS_LOCKED = 0,
    PHY84793_PRBS_NOT_LOCKED,
    PHY84793_PRBS_LL_SET,
    PHY84793_PRBS_LL_NOT_SET
}PHY84793_PRBS_LOCK;

/* dsc values */
struct phy84793_dsc_cb {
    int32 tx_pre_cursor;
    int32 tx_main;
    int32 tx_post_cursor;
    int32 postc_metric;
    int32 pf_ctrl;
    int32 vga_sum;
    int32 dfe1_bin;
    int32 dfe2_bin;
    int32 dfe3_bin;
    int32 dfe4_bin;
    int32 dfe5_bin;
    int32 integ_reg;
    int32 integ_reg_xfer;
    int32 clk90_offset;
    int32 slicer_target;
    int32 sig_det;
    int32 offset_pe;
    int32 offset_ze;
    int32 offset_me;
    int32 offset_po;
    int32 offset_zo;
    int32 offset_mo;
};

/*Functions forward declarations*/
STATIC int32 
_phy_84793_prbs_gen_get(int32 unit, soc_port_t port, 
                             int32 lane, uint16 *tx_gen, uint16 *rx_gen);
STATIC int32
_phy_84793_prbs_chkr_get(int32 unit, soc_port_t port,
                             int32 lane, uint16 *tx_chk, uint16 *rx_chk);
STATIC int32 
_phy_84793_prbs_gen_enable(int32 unit, soc_port_t port,
                                   int32 lane, uint16 en_tx, uint16 en_rx);
STATIC int32 
_phy_84793_prbs_chkr_enable(int32 unit, soc_port_t port,
                                    int32 lane, uint16 en_tx, uint16 en_rx);
STATIC int32 
_phy_84793_control_prbs_tx_invert_data_set(int32 unit, 
                                       soc_port_t port, 
                                       PHY84793_INTF_SIDE intf_side,
                                       int32 invert);
STATIC int32 
_phy_84793_control_prbs_rx_invert_data_set(int32 unit, 
                                        soc_port_t port, 
                                        PHY84793_INTF_SIDE intf_side,
                                        int32 invert);
STATIC int32 
_phy_84793_per_lane_control_prbs_polynomial_set(int32 unit, 
                           soc_port_t port, int32 lane, int32 poly_ctrl);
STATIC int32 
_phy_84793_control_prbs_polynomial_set(int32 unit, 
                                       soc_port_t port,int32 poly_ctrl);
STATIC int32 
_phy_84793_per_lane_control_prbs_enable_set(int32 unit, 
                           soc_port_t port, int32 lane, int32 enable);
STATIC int32 
_phy_84793_control_prbs_enable_set(int32 unit, soc_port_t port,
                                   int32 enable);
STATIC int32 
_phy_84793_per_lane_control_prbs_enable_get(int32 unit,
                            soc_port_t port, int32 lane, uint32 *value);
STATIC int32 
_phy_84793_control_prbs_enable_get(int32 unit, soc_port_t port,
                                   uint32 *value);
STATIC int32 
_phy_84793_per_lane_control_prbs_polynomial_get(int32 unit, 
                               soc_port_t port, int32 lane, uint32 *value);
STATIC int32 
_phy_84793_control_prbs_polynomial_get(int32 unit, 
                                        soc_port_t port, uint32 *value);
STATIC int32 
_phy_84793_control_prbs_tx_invert_data_get(int32 unit, soc_port_t port, 
                                           PHY84793_INTF_SIDE intf_side,
                                           uint32 *value);
STATIC int32 
_phy_84793_control_prbs_rx_status_get(int32 unit, 
                                      soc_port_t port, uint32 *value);
STATIC int32 
_phy_84793_per_lane_control_prbs_rx_status_get(int32 unit, 
                                      soc_port_t port, int32 lane, 
                                      uint32 *value);
STATIC int32 
phy_84793_diag_ctrl(int32 unit, soc_port_t port, uint32 inst,
                    int32 op_type,  int32 op_cmd, void *arg);
STATIC int32 
_phy_84793_dsc_diag(int32 unit, soc_port_t port,
                    int32 intf, int32 lane);
STATIC int32 phy_84793_control_set(int32 unit, soc_port_t port,
                                   soc_phy_control_t type, uint32 value);
STATIC int32 phy_84793_control_get(int32 unit, soc_port_t port, 
                                    soc_phy_control_t type, uint32 *value);
STATIC int32
_phy_84793_speed_set(int32 unit, soc_port_t port, int32 speed);
STATIC int32
phy_84793_per_lane_control_set(int32 unit, soc_port_t port, int32 lane,
                     soc_phy_control_t type, uint32 value);
STATIC int32
phy_84793_per_lane_control_get(int32 unit, soc_port_t port, int32 lane,
                     soc_phy_control_t type, uint32 *value);
STATIC int32 
_phy_84793_modify_pma_pmd_reg (int32 unit, int32 port, phy_ctrl_t *pc, 
	uint16 address, uint8 msb_pos, uint8 lsb_pos, uint16 data);
STATIC int32 
_phy_84793_rom_firmware_download (int32 unit, int32 port, int32 offset,
                          phy_ctrl_t *pc, uint8 *new_fw, uint32 datalen);
STATIC int32
_phy_84793_control_tx_preemphsis_set (int32 unit, soc_port_t port, 
                                      soc_phy_control_t type, 
                                      PHY84793_INTF_SIDE intf_side, 
                                      uint32 value);
STATIC int32
_phy_84793_control_tx_driver_set (int32 unit, soc_port_t port, 
                                  soc_phy_control_t type, 
                                  PHY84793_INTF_SIDE intf_side, uint32 value);
STATIC int32
_phy_84793_per_lane_control_tx_preemphasis_set (int32 unit, soc_port_t port, 
		                             PHY84793_INTF_SIDE intf_side, 
					                 uint16 lane, uint32 value);
STATIC int32
phy_84793_lb_set (int32 unit, soc_port_t port, int32 enable);

STATIC int32
_phy_84793_per_lane_control_tx_driver_set (int32 unit, soc_port_t port,  
                                           soc_phy_control_t type, 
                                           PHY84793_INTF_SIDE intf_side, 
                                           uint16 lane, uint32 value);
STATIC int32
_phy_84793_per_lane_control_tx_driver_get (int32 unit, soc_port_t port,  
                                           soc_phy_control_t type, 
                                           PHY84793_INTF_SIDE intf_side, 
                                           uint16 lane, uint32 *value);
STATIC int32
_phy_84793_control_tx_driver_get (int32 unit, soc_port_t port, 
                                  soc_phy_control_t type, 
                                  PHY84793_INTF_SIDE intf_side, 
                                  uint32 *value);
STATIC int32
_phy_84793_control_tx_preemphsis_get (int32 unit, soc_port_t port, 
                                      PHY84793_INTF_SIDE intf_side, 
                                      uint32 *value);
STATIC int32
_phy_84793_per_lane_control_tx_preemphasis_get (int32 unit, soc_port_t port, 
		                             PHY84793_INTF_SIDE intf_side, 
                                     uint16 lane, uint32 *value);
STATIC int32
phy_84793_lb_get(int32 unit, soc_port_t port, int32 *enable);

STATIC int32 
_phy_84793_per_lane_lb_get(int32 unit, soc_port_t port, uint16 lane,
                           uint32 *enable);
STATIC int32 
phy_84793_per_lane_link_get(int32 unit, soc_port_t port, uint16 lane,
                            int32 *link);
STATIC
int32 _phy_84793_dc_hc_pcb_read (int32 unit, soc_port_t port, phy_ctrl_t *pc,
                 uint16 dc_hc, uint32 addr, uint16 clear_reg, uint16 *rddata);
STATIC
int32 _phy_84793_system_polarity_flip (int32 unit, soc_port_t port, 
		phy_ctrl_t *pc, uint16 cfg_tx_pol, uint16 cfg_rx_pol);
STATIC
int32 _phy_84793_line_polarity_flip (int32 unit, soc_port_t port, 
                                     phy_ctrl_t *pc, 
                                     uint16 cfg_tx_pol, uint16 cfg_rx_pol);
STATIC int32
_phy_84793_remote_loopback_set(int32 unit, soc_port_t port, int32 enable);

STATIC int32
_phy_84793_squelch_enable(int32 unit, soc_port_t port, 
                          PHY84793_INTF_SIDE intf_side, 
                          uint8 enable);
STATIC int32 
_phy_84793_per_lane_remote_lb_get(int32 unit, soc_port_t port, 
                                  uint16 lane, uint32 *enable);
STATIC int32
_phy_84793_sys_lane_squelch_enable (int32 unit, soc_port_t port, uint8 lane, 
                                    uint8 enable);
STATIC int32
_phy_84793_line_lane_squelch_enable (int32 unit, soc_port_t port, uint8 lane, 
                                    uint8 enable);
STATIC int32
phy_84793_interface_set(int32 unit, soc_port_t port, soc_port_if_t pif);

STATIC int32
phy_84793_speed_set(int32 unit, soc_port_t port, int32 speed);

STATIC int32
_phy_84793_div_change(int32 unit, soc_port_t port, uint16 setmode, 
                     uint16 refclkin, uint16 mode);

STATIC int32
_phy_84793_rx_seq_done_get(int unit, soc_port_t port, int32 intf, uint32 *value);

STATIC int32 
phy_84793_link_get(int32 unit, soc_port_t port, int32 *link);

STATIC
int32 _phy_84793_dc_hc_pcb_write (int32 unit, soc_port_t port, phy_ctrl_t *pc, 
                                  uint16 dc_hc, uint32 addr, uint16 value);
STATIC
int32 _phy_84793_pcb_wr_bits(int32 unit, soc_port_t port, phy_ctrl_t *pc,
                  uint16 core_sel, uint16 lane, uint16 addr, 
                  uint16 msb, uint16 lsb, uint16 value);
STATIC int
_phy_84793_debug_info(int unit, soc_port_t port);


STATIC int
_phy_84793_per_lane_rx_seq_done_get(int unit, soc_port_t port, 
                                    int32 intf, 
                                    int lane, uint32 *value);

uint32 power(uint16 x, uint16 y)
{
    uint32 temp;
    if( y == 0)
        return 1;
    temp = power(x, y/2);
    if (y%2 == 0)
        return temp*temp;
    else
        return x*temp*temp;
}

/*
 * Function:
 *      phy_84793_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion.

 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - New setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
phy_84793_control_set(int32 unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{

    phy_ctrl_t    *pc;       /* PHY software state */
    int rv = SOC_E_UNAVAIL;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);
    switch (type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
            /* fall through */
            rv = _phy_84793_control_tx_preemphsis_set (unit, port, type, 
                                  (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                                  PHY84793_SYS_SIDE : PHY84793_LINE_SIDE, 
                                  value);
        break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
            /* fall through */
            rv = _phy_84793_control_tx_driver_set (unit, port, type,
                                  (pc->flags & PHYCTRL_SYS_SIDE_CTRL)?
                                  PHY84793_SYS_SIDE:PHY84793_LINE_SIDE,
                                  value);
            break;
        case SOC_PHY_CONTROL_EDC_MODE:
            return SOC_E_UNAVAIL;
        case SOC_PHY_CONTROL_CLOCK_ENABLE:
            return SOC_E_UNAVAIL;
        case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
            return SOC_E_UNAVAIL;
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
            rv = _phy_84793_control_prbs_polynomial_set(unit, port, value);
            break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
           rv = _phy_84793_control_prbs_tx_invert_data_set(unit, port, 
                (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE :
                PHY84793_LINE_SIDE, value);
           if (SOC_SUCCESS(rv)) {
               rv = _phy_84793_control_prbs_rx_invert_data_set(unit, port, 
                    (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE :
                    PHY84793_LINE_SIDE, value);
           }
           break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
            /* fall through */
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
            /* tx/rx is enabled at the same time. no seperate control */
            rv = _phy_84793_control_prbs_enable_set(unit, port, value);
            break;
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
            rv = _phy_84793_remote_loopback_set(unit, port, value);
            break;
        case SOC_PHY_CONTROL_PORT_PRIMARY:
            rv = soc_phyctrl_primary_set(unit, port, (soc_port_t)value);
            break;
        case SOC_PHY_CONTROL_PORT_OFFSET:
            rv = soc_phyctrl_offset_set(unit, port, (int)value);
            break;
        case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
            return SOC_E_UNAVAIL;
        case SOC_PHY_CONTROL_DUMP:
            rv = _phy_84793_debug_info(unit, port);
            break;
        case SOC_PHY_CONTROL_RX_POLARITY:
            if ((pc->flags & PHYCTRL_SYS_SIDE_CTRL)) {
                rv = _phy_84793_system_polarity_flip (unit, port, 
		     pc, PHY84793_POL_DND, (uint16)value);
	    } else {
                rv = _phy_84793_line_polarity_flip (unit, port, 
		     pc, PHY84793_POL_DND, (uint16)value);
	    }
        break;
        case SOC_PHY_CONTROL_TX_POLARITY:
            if ((pc->flags & PHYCTRL_SYS_SIDE_CTRL)) {
                rv = _phy_84793_system_polarity_flip (unit, port, 
		     pc, (uint16)value, PHY84793_POL_DND);
	    } else {
                rv = _phy_84793_line_polarity_flip (unit, port, 
		              pc, (uint16)value, PHY84793_POL_DND);
	    }
        break;
        case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
	    rv = _phy_84793_squelch_enable(unit, port, 
                            (pc->flags & PHYCTRL_SYS_SIDE_CTRL)?
                            PHY84793_SYS_SIDE:PHY84793_LINE_SIDE, 
                            (uint8)value);
        break;
        case SOC_PHY_CONTROL_LOOPBACK_INTERNAL: 
            rv = phy_84793_lb_set(unit, port, value); 
        break; 
        default:
            rv = SOC_E_UNAVAIL;
        break;
            
    }
    return rv;
}

STATIC int
_phy_84793_debug_info(int unit, soc_port_t port)
{
	phy_ctrl_t			*pc;     
	phy_ctrl_t			*int_pc; 
	phy84793_intf_cfg_t *intf;
    PHY84793_INTF_SIDE side;
	soc_port_if_t		iif;
	int					att =0, ispeed = 0, ilink = 0;
	uint16				link_sts[PHY84793_LN_MAX_NO_LANE];
	uint16				tx_pol_sts[PHY84793_LN_MAX_NO_LANE];
	uint16				rx_pol_sts[PHY84793_LN_MAX_NO_LANE];
	uint16				drv_cur[PHY84793_LN_MAX_NO_LANE];
	uint16				preemph_main[PHY84793_LN_MAX_NO_LANE];
	uint16				preemph_post1[PHY84793_LN_MAX_NO_LANE];
	uint16				preemph_post2[PHY84793_LN_MAX_NO_LANE];
	uint16				data0, data1, data2, data3, data4, lane_idx;
    uint16              mode = PHY84793_MODE_10X10G_TO_10X10G_ASYNC;
	uint32				temp = 0, reg_addr = 0;
    uint16              start_lane = 0; 
    uint16              end_lane = PHY84793_LN_MAX_NO_LANE; 
    uint16              *lane_val = NULL;

	pc = EXT_PHY_SW_STATE(unit, port);
	int_pc = INT_PHY_SW_STATE(unit, port);
	
	intf = &(INTERFACE(pc));
    PHY84793_GET_PORT_MODE(pc, mode);
	/* firware rev: 1.F417  */
	SOC_IF_ERROR_RETURN
		(READ_PHY84793_PMA_PMD_REG(unit, pc,
									   CTL_REGS_GPREG0, &data0));
	/* Mode select 1.F403 */
	SOC_IF_ERROR_RETURN
		(READ_PHY84793_PMA_PMD_REG(unit, pc,
									   CTL_REGS_USER_MODESEL_REG, &data1));
	/* REF clk in 1.F404 */
	SOC_IF_ERROR_RETURN
		(READ_PHY84793_PMA_PMD_REG(unit, pc,
									   CTL_REGS_REFCLK_SEL_REG, &data2));
    LOG_CLI((BSL_META_U(unit, 
	             "Port %-2d: micro ver(1.%04x)=%04x,Mode Sel(1.%04x)=%04x\n"
				 "        Ref Clk(1.%04x)=%04x, mdio=0x%x Mode:%04x \n"
                 "        speed: %d interface type: %s\n"),
				 port, CTL_REGS_GPREG0, data0,
				 CTL_REGS_USER_MODESEL_REG, data1,
                 CTL_REGS_REFCLK_SEL_REG, data2,
				 pc->phy_id, mode, intf->speed, phy84793_intf_names[intf->type]));

	/* Interfaces: software, hardware, internal serdes */
	for (side = PHY84793_SYS_SIDE; side <= PHY84793_LINE_SIDE; side++) {
  		/* control: 1.0000 */
		SOC_IF_ERROR_RETURN(READ_PHY84793_PMA_PMD_REG(unit, pc,
										   IEEE_PMA_PMD_REGS_PMA_PMD_CONTROL_1, &data0));
        if (side == PHY84793_SYS_SIDE) {
            /* Loopback 1.F457 */
		    SOC_IF_ERROR_RETURN
                (READ_PHY84793_PMA_PMD_REG(unit, pc, 
										  CTL_REGS_SYS_LOOPBACK_EN, &data1));
		   /*DC Tx disable 1.F449 */
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_DCTX_PD, &data2));
            data2  &= 0x3ff;

		   /*DC Rx disable 1.F447 */
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_DCRX_PD, &data3));
            data3  &= 0x3ff;
        } else {
            /* Loopback 1.F458 */
		    SOC_IF_ERROR_RETURN
                (READ_PHY84793_PMA_PMD_REG(unit, pc, 
       								  CTL_REGS_LINE_LOOPBACK_EN, &data1));

		    /*VSR Tx disable 1.F44f HC TX 1.F44D */
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_VSR40_TX_PD, 
                                         &data2));
            data2 &= 0xf;
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_HCTX_PD, &data4));
            data2 |= ((data4 & 0x3f) << 4);

		    /*VSR Rx disable 1.F451 HC TX 1.F44b */
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_VSR40_RX_PD, 
                                         &data3));
            data3 &= 0xf;
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_HCRX_PD, &data4));
            data3 |= ((data4 & 0x3f) << 4);
        }
        /* Single PORT Mode */
		if (mode == PHY84793_MODE_10X10G_TO_4X25G || mode == PHY84793_MODE_10X10G_TO_10X10G_SYNC 
            || mode == PHY84793_MODE_10X10G_TO_4X25G_OTL || mode == PHY84793_MODE_4X10G_TO_4X10G) {
            if (mode == PHY84793_MODE_4X10G_TO_4X10G) {
                start_lane = (pc->lane_num) ? 5 : 0;
                end_lane = start_lane + 4;
            }
            if (side == PHY84793_SYS_SIDE) {
    			for (lane_idx = start_lane; lane_idx < end_lane; lane_idx++) {
                     /* DC.80b0 :: Link sts*/
                     SOC_IF_ERROR_RETURN
                          (_phy_84793_dc_hc_pcb_read (unit, port, pc,
                          PHY84793_DECA_CORE, (lane_idx << 16)|0x80b1, 0, &data4));
                     data4 &= (~0x7);
                     SOC_IF_ERROR_RETURN
                          (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                          PHY84793_DECA_CORE, 
                          (lane_idx << 16), data4));
                     SOC_IF_ERROR_RETURN
                          (_phy_84793_dc_hc_pcb_read (unit, port, pc,
                          PHY84793_DECA_CORE, (lane_idx << 16)|0x80b0, 0, &(link_sts[lane_idx])));
                     
                     /* 1. F516 TX polarity*/
                     SOC_IF_ERROR_RETURN
                      (READ_PHY84793_PMA_PMD_REG (unit, pc, 
                       RG_RX_RX_GB_IO_BIT_CTRL, &tx_pol_sts[lane_idx]));
    
                     /* DC.0x80BA RX polarity*/
                     SOC_IF_ERROR_RETURN
                     (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
                           (lane_idx << 16) | 0x80BA, 0, &rx_pol_sts[lane_idx]));
                     SOC_IF_ERROR_RETURN
                      (_phy_84793_per_lane_control_tx_preemphasis_get (unit, port, 
		                             side, lane_idx, &temp));
                     preemph_main[lane_idx]  = (temp & 0x3f00);
                     preemph_post1[lane_idx] = (temp & 0x01f);
                     preemph_post2[lane_idx] = (temp & 0xe0);

                     reg_addr = 0x8065 | (lane_idx << 16);
                     SOC_IF_ERROR_RETURN
                     (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
                       reg_addr, 0, &drv_cur[lane_idx]));
                }
            } else {
    			for (lane_idx = start_lane; lane_idx < end_lane; lane_idx++) {
                    if (lane_idx <= PHY84793_VSR40_LANE3)  {
                        /* 1.9811 : RX pol*/
                        SOC_IF_ERROR_RETURN
                            (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                             VSR40_RX_DIG_REGS_DP_CTRL, &rx_pol_sts[lane_idx])); 
                        
                        /* 1.9800 : link status*/
                        SOC_IF_ERROR_RETURN
                            (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                             VSR40_RX_DIG_REGS_CDR_CONTROL, &(link_sts[lane_idx])));
                        SOC_IF_ERROR_RETURN
                            (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                            (VSR40_TX_ANA_LANE0_BASE | (lane_idx << 4)), &drv_cur[lane_idx]));

                    } else {
                        /* HC.80ba :: rx_pol_sts*/
                        reg_addr = ((lane_idx-4) << 16) | 0x80BA; 
                        SOC_IF_ERROR_RETURN
                          (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE,
                           reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rx_pol_sts[lane_idx]));

                        /* HC.80b0 :: Link sts*/
                        SOC_IF_ERROR_RETURN
                          (_phy_84793_dc_hc_pcb_read (unit, port, pc,
                          PHY84793_HEXA_CORE, ((lane_idx-4) << 16)|0x80b1, 0, &data4));
                        data4 &= (~0x7);
                        SOC_IF_ERROR_RETURN
                          (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                          PHY84793_HEXA_CORE, 
                          ((lane_idx-4) << 16), data4));
                        SOC_IF_ERROR_RETURN
                          (_phy_84793_dc_hc_pcb_read (unit, port, pc,
                          PHY84793_HEXA_CORE, ((lane_idx-4) << 16)|0x80b0, 0, &(link_sts[lane_idx])));
                        
                        reg_addr = 0x8065 | ((lane_idx - 4) << 16);
                        PHY_84793_HCTX_PCB_READ(unit, pc, reg_addr, &drv_cur[lane_idx]);
                    }
                    SOC_IF_ERROR_RETURN
                       (READ_PHY84793_PMA_PMD_REG(unit, pc,
                         RG_TX_TX_GB_IO_BIT_CONTROL, &tx_pol_sts[lane_idx]));
                    SOC_IF_ERROR_RETURN
                      (_phy_84793_per_lane_control_tx_preemphasis_get (unit, port, 
		                             side, lane_idx, &temp));
                     preemph_main[lane_idx]  = (temp & 0x3f00);
                     preemph_post1[lane_idx] = (temp & 0x01f);
                     preemph_post2[lane_idx] = (temp & 0xe0);
                     if ((mode == PHY84793_MODE_10X10G_TO_4X25G ||
                          mode == PHY84793_MODE_10X10G_TO_4X25G_OTL) && 
                          lane_idx >  PHY84793_VSR40_LANE3)  {
                         link_sts[lane_idx] = 0;
                         rx_pol_sts[lane_idx] = 0;
                         tx_pol_sts[lane_idx] = 0;
                         preemph_main[lane_idx]  = 0;
                         preemph_post1[lane_idx] = 0;
                         preemph_post2[lane_idx] = 0;
                         drv_cur[lane_idx] = 0;
                     }
                 }
            }
        } else {
            if (side == PHY84793_SYS_SIDE) {
                /* DC.80b0 :: Link sts*/
                SOC_IF_ERROR_RETURN
                    (_phy_84793_dc_hc_pcb_read (unit, port, pc,
                     PHY84793_DECA_CORE, (pc->lane_num << 16)|0x80b1, 0, &data4));
                data4 &= (~0x7);
                SOC_IF_ERROR_RETURN
                    (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                     PHY84793_DECA_CORE, (pc->lane_num << 16), data4));
                SOC_IF_ERROR_RETURN
                    (_phy_84793_dc_hc_pcb_read (unit, port, pc,
                     PHY84793_DECA_CORE, (pc->lane_num << 16)|0x80b0, 0, 
                     &(link_sts[pc->lane_num])));
                     
                /* 1. F516 TX polarity*/
                SOC_IF_ERROR_RETURN
                    (READ_PHY84793_PMA_PMD_REG (unit, pc, 
                     RG_RX_RX_GB_IO_BIT_CTRL, &tx_pol_sts[pc->lane_num]));
    
                     /* DC.0x80BA RX polarity*/
                SOC_IF_ERROR_RETURN
                  (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
                        (pc->lane_num << 16) | 0x80BA, 0, &rx_pol_sts[pc->lane_num]));
                SOC_IF_ERROR_RETURN
                  (_phy_84793_per_lane_control_tx_preemphasis_get (unit, port, 
		                             side, pc->lane_num, &temp));
                preemph_main[pc->lane_num]  = (temp & 0x3f00);
                preemph_post1[pc->lane_num] = (temp & 0x01f);
                preemph_post2[pc->lane_num] = (temp & 0xe0);

                reg_addr = 0x8065 | (pc->lane_num << 16);
                SOC_IF_ERROR_RETURN
                   (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
                    reg_addr, 0, &drv_cur[pc->lane_num]));
            } else {
                if (pc->lane_num <= PHY84793_VSR40_LANE3)  {
                    /* 1.9811 : RX pol*/
                    SOC_IF_ERROR_RETURN
                        (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                         VSR40_RX_DIG_REGS_DP_CTRL, &rx_pol_sts[pc->lane_num])); 
                        
                    /* 1.9800 : link status*/
                    SOC_IF_ERROR_RETURN
                        (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                         VSR40_RX_DIG_REGS_CDR_CONTROL, &(link_sts[pc->lane_num])));

                    SOC_IF_ERROR_RETURN
                        (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                          (VSR40_TX_ANA_LANE0_BASE | (pc->lane_num << 4)), &drv_cur[pc->lane_num]));
                } else {
                    /* HC.80ba :: rx_pol_sts*/
                    reg_addr = ((pc->lane_num-4) << 16) | 0x80BA; 
                    SOC_IF_ERROR_RETURN
                       (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE,
                        reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rx_pol_sts[pc->lane_num]));

                    /* HC.80b0 :: Link sts*/
                    SOC_IF_ERROR_RETURN
                      (_phy_84793_dc_hc_pcb_read (unit, port, pc,
                      PHY84793_HEXA_CORE, ((pc->lane_num-4) << 16)|0x80b1, 0, &data4));
                    data4 &= (~0x7);
                    SOC_IF_ERROR_RETURN
                      (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                      PHY84793_HEXA_CORE, 
                      ((pc->lane_num-4) << 16), data4));
                    SOC_IF_ERROR_RETURN
                      (_phy_84793_dc_hc_pcb_read (unit, port, pc,
                      PHY84793_HEXA_CORE, ((pc->lane_num-4)<< 16)|0x80b0, 0, 
                      &(link_sts[pc->lane_num])));
    
                    reg_addr = 0x8065 | ((pc->lane_num - 4) << 16);
                    PHY_84793_HCTX_PCB_READ(unit, pc, reg_addr, &drv_cur[pc->lane_num]);
                }
                SOC_IF_ERROR_RETURN
                   (READ_PHY84793_PMA_PMD_REG(unit, pc,
                     RG_TX_TX_GB_IO_BIT_CONTROL, &tx_pol_sts[pc->lane_num]));
                SOC_IF_ERROR_RETURN
                  (_phy_84793_per_lane_control_tx_preemphasis_get (unit, port, 
		                         side, pc->lane_num, &temp));
                preemph_main[pc->lane_num]  = (temp & 0x3f00);
                preemph_post1[pc->lane_num] = (temp & 0x01f);
                preemph_post2[pc->lane_num] = (temp & 0xe0);
            }
        }
        start_lane = 0;
        end_lane = PHY84793_LN_MAX_NO_LANE;
        if (mode == PHY84793_MODE_10X10G_TO_4X25G || mode == PHY84793_MODE_10X10G_TO_10X10G_SYNC 
            || mode == PHY84793_MODE_10X10G_TO_4X25G_OTL || mode == PHY84793_MODE_4X10G_TO_4X10G) {
            if (mode == PHY84793_MODE_4X10G_TO_4X10G) {
                start_lane = (pc->lane_num) ? 5 : 0;
                end_lane = start_lane + 4;
            }
            LOG_CLI((BSL_META_U(unit, 
        		" %s: pmdctrl(1.%04x)=%04x, lb(1.%04x)=%04x\n"),
             	(side == PHY84793_LINE_SIDE) ? "Line    " : "System  ",
        		IEEE_PMA_PMD_REGS_PMA_PMD_CONTROL_1, data0,
        		(side == PHY84793_LINE_SIDE) ? CTL_REGS_LINE_LOOPBACK_EN : CTL_REGS_SYS_LOOPBACK_EN, data1));
            if (side == PHY84793_SYS_SIDE) {
                LOG_CLI((BSL_META_U(unit, 
                "          txdis(1.%04x)=%04x  rxDis(1.%04x) = %04x\n"),
                        CTL_REGS_DCTX_PD, data2, CTL_REGS_DCRX_PD, data3));
            } else {
                LOG_CLI((BSL_META_U(unit, 
                "          txdis(1.%04x(HC)/1.%04x(VSR))=%04x  rxDis(1.%04x(HC)/1.%04x(VSR)) = %04x\n"),
        	        		CTL_REGS_HCTX_PD, CTL_REGS_VSR40_TX_PD, data2,
        		            CTL_REGS_HCRX_PD, CTL_REGS_VSR40_RX_PD, data3));
            }
            for (att = 0; att < 7; att++) {
               if (att == 0) {
                   if (side == PHY84793_SYS_SIDE) {
                       LOG_CLI((BSL_META_U(unit, 
                        "          linksts(DC:80b0)     : ")));
                   } else {
                       LOG_CLI((BSL_META_U(unit, 
                        " linksts(VSR:9800 HC: 80b0)    : ")));
                   }
                   lane_val = link_sts;
               } else if(att == 1) {
                   if (side == PHY84793_SYS_SIDE) {
                       LOG_CLI((BSL_META_U(unit, 
                        "          tx_pol(1.f516)       : ")));
                   } else {
                       LOG_CLI((BSL_META_U(unit, 
                        "          tx_pol(1.f633)       : ")));
                   }
                   lane_val = tx_pol_sts;
               } else if(att == 2) {
                   if (side == PHY84793_SYS_SIDE) {
                       LOG_CLI((BSL_META_U(unit, 
                        "          rx_pol(DC:80BA)      : ")));
                   } else {
                       LOG_CLI((BSL_META_U(unit, 
                        "   rx_pol(HC:80BA/9811)        : ")));
                   }
                   lane_val = rx_pol_sts;
               } else if(att == 3) {
                   LOG_CLI((BSL_META_U(unit, 
                    "          pre_main             : ")));
                   lane_val = preemph_main;
               } else if(att == 4) {
                   LOG_CLI((BSL_META_U(unit, 
                   "          pre_post1            : ")));
                   lane_val = preemph_post1;
               } else if(att == 5) {
                   LOG_CLI((BSL_META_U(unit, 
                   "          pre_post2            : ")));
                   lane_val = preemph_post2;
               } else if(att == 6) {
                   if (side == PHY84793_SYS_SIDE) {
                       LOG_CLI((BSL_META_U(unit, 
                       "          drv_cur(DC:8065)     : ")));
                   } else {
                       LOG_CLI((BSL_META_U(unit, 
                        "   drv_cur(HC:8065/%04x)       : "),
                               VSR40_TX_ANA_LANE0_BASE));
                   }
                   lane_val = drv_cur;
               }

               for (lane_idx = start_lane; lane_idx < end_lane; lane_idx++) {
                   if ((lane_idx % 4 == 0) && (lane_idx != 0) &&
                           (mode != PHY84793_MODE_4X10G_TO_4X10G)) {
                       LOG_CLI((BSL_META_U(unit, "\n")));
                       LOG_CLI((BSL_META_U(unit, 
                          "                                 ")));
                   }
                   if ((mode == PHY84793_MODE_10X10G_TO_4X25G ||
                          mode == PHY84793_MODE_10X10G_TO_4X25G_OTL) && 
                          lane_idx >  PHY84793_VSR40_LANE3 && 
                          side == PHY84793_LINE_SIDE)  {
                       LOG_CLI((BSL_META_U(unit, 
                             "(ln%d=-NA-)"),lane_idx));
                   } else {
                       LOG_CLI((BSL_META_U(unit, 
                          "(ln%d=%04x)"),lane_idx, lane_val[lane_idx]));
                   }
               }
               LOG_CLI((BSL_META_U(unit, "\n")));
            }
        } else {
            LOG_CLI((BSL_META_U(unit, 
        		"Lane:%d: %s: pmdctrl(1.%04x)=%04x, lb(1.%04x)=%04x\n"),
             	 pc->lane_num, (side == PHY84793_LINE_SIDE) ? "Line    " : "System  ",
        		 IEEE_PMA_PMD_REGS_PMA_PMD_CONTROL_1, data0,
        		 (side == PHY84793_LINE_SIDE) ? CTL_REGS_LINE_LOOPBACK_EN : CTL_REGS_SYS_LOOPBACK_EN, data1));
            if (side == PHY84793_SYS_SIDE) {
                LOG_CLI((BSL_META_U(unit, 
                "         txdis(1.%04x)=%04x  rxDis(1.%04x) = %04x\n"),
                        CTL_REGS_DCTX_PD, data2, CTL_REGS_DCRX_PD, data3));
                LOG_CLI((BSL_META_U(unit, 
                    "           link(0x80b0)=%04x, txpol(1.0xf516)=%04x, rxpol(0x80BA)=%04x"), 
                       link_sts[pc->lane_num],tx_pol_sts[pc->lane_num] ,rx_pol_sts[pc->lane_num]));

            } else {
                LOG_CLI((BSL_META_U(unit, 
                "         txdis(1.%04x(HC)/1.%04x(VSR))=%04x  rxDis(1.%04x(HC)/1.%04x(VSR)) = %04x\n"),
        	        		CTL_REGS_HCTX_PD, CTL_REGS_VSR40_TX_PD, data2,
        		            CTL_REGS_HCRX_PD, CTL_REGS_VSR40_RX_PD, data3));
                LOG_CLI((BSL_META_U(unit, 
                  "           link(1.0x80b0(HC)/1.0x9800(VSR))=%04x, txpol(1.0xF633)=%04x, rxpol(1.0x80ba(HC)/1.0x9811(VSR))=%04x"), 
                  link_sts[pc->lane_num],tx_pol_sts[pc->lane_num] ,rx_pol_sts[pc->lane_num]));
            }
            LOG_CLI((BSL_META_U(unit, 
              "\n"
              "           prem_main=%04x, prem_post1=%04x prem_post2=%04x drv_cur=%04x\n"),
              preemph_main[pc->lane_num], preemph_post1[pc->lane_num],
              preemph_post2[pc->lane_num], drv_cur[pc->lane_num]));
		}
	}
	if (int_pc) {
		SOC_IF_ERROR_RETURN(PHY_INTERFACE_GET(int_pc->pd, unit, port, &iif));
		SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &ilink));
		SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &ispeed));
        LOG_CLI((BSL_META_U(unit, 
		  " Internal: type=%s, speed=%d, link=%d \n===\n"), 
					 phy84793_intf_names[iif], ispeed, ilink));
	}
	return SOC_E_NONE;
}


STATIC int
phy_84793_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int32 rv = 0;
    phy_ctrl_t    *pc;       /* PHY software state */
    soc_port_t offset = 0;
    soc_port_t primary;


    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);
    switch (type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
            /* fall through */
            rv = _phy_84793_control_tx_preemphsis_get (unit, port, 
                   (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                   PHY84793_SYS_SIDE : PHY84793_LINE_SIDE, 
                   value);
        break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
            /* fall through */
            rv = _phy_84793_control_tx_driver_get(unit, port, type, 
                    (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                    PHY84793_SYS_SIDE : PHY84793_LINE_SIDE, 
                    value);
        break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:   
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
            rv = _phy_84793_control_prbs_enable_get(unit, port, value);
        break;
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
            rv = _phy_84793_control_prbs_polynomial_get(unit, port, value);
        break;
        case  SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
            rv = _phy_84793_control_prbs_tx_invert_data_get(unit, port,
                    (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                    PHY84793_SYS_SIDE : PHY84793_LINE_SIDE,
                    value);
        break;
        case SOC_PHY_CONTROL_PRBS_RX_STATUS:
	        rv = _phy_84793_control_prbs_rx_status_get(unit, port, value);
        break;
        case SOC_PHY_CONTROL_PORT_PRIMARY:
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_primary_get(unit, port, &primary));
            *value = (uint32) primary;
            rv = SOC_E_NONE;
        break;
        case SOC_PHY_CONTROL_PORT_OFFSET:
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_offset_get(unit, port, &offset));
            *value = (uint32) offset;
	        rv = SOC_E_NONE;
        break;
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
            rv = _phy_84793_per_lane_remote_lb_get(unit, port, pc->lane_num, 
                    value);
        break;
        case SOC_PHY_CONTROL_LOOPBACK_INTERNAL: 
            rv = phy_84793_lb_get(unit, port, (int32*)value); 
        break;
        case SOC_PHY_CONTROL_RX_SEQ_DONE:
            rv = _phy_84793_rx_seq_done_get(unit, port, 
             (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
             PHY84793_SYS_SIDE : PHY84793_LINE_SIDE, 
             value);
        break;

        default:
            rv = SOC_E_UNAVAIL;
        break;
    }
    return rv;
}
/*
 * Function:
 *      phy_84793_per_lane_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_84793_per_lane_control_set(int unit, soc_port_t port , int lane, 
		               soc_phy_control_t type, uint32 value)
{
    int32 rv = SOC_E_UNAVAIL;
    phy_ctrl_t    *pc;       /* PHY software state */

    CHECK_LANE (lane);
    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);
    
    switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
            /* fall through */
            rv = _phy_84793_per_lane_control_tx_preemphasis_set (unit, port, 
                 (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE : 
                 PHY84793_LINE_SIDE, lane, value);
        break; 
        case SOC_PHY_CONTROL_DRIVER_CURRENT:           /* fall through */
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
            /* fall through */
            rv = _phy_84793_per_lane_control_tx_driver_set (unit, port,  
                 type, (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE :
                 PHY84793_LINE_SIDE, lane, value);
        break;
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
            rv = _phy_84793_per_lane_control_prbs_polynomial_set(unit, port,
                 lane, value);
        break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
            rv = _phy_84793_control_prbs_tx_invert_data_set(unit, port, 
                 (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE :
                 PHY84793_LINE_SIDE, value);
            if (SOC_SUCCESS(rv)) {
                rv = _phy_84793_control_prbs_rx_invert_data_set(unit, port,
                     (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE :
                     PHY84793_LINE_SIDE, value);
            }
        break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:   
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
            rv = _phy_84793_per_lane_control_prbs_enable_set(unit, port,
                 lane, value);
        break;
        case SOC_PHY_CONTROL_RX_POLARITY:
            if ((pc->flags & PHYCTRL_SYS_SIDE_CTRL)) {
                rv = _phy_84793_system_polarity_flip (unit, port, 
		         pc, PHY84793_POL_DND, (uint16)value);
	        } else {
                rv = _phy_84793_line_polarity_flip (unit, port, 
		        pc, PHY84793_POL_DND, (uint16)value);
	        }
        break;
        case SOC_PHY_CONTROL_TX_POLARITY:
            if ((pc->flags & PHYCTRL_SYS_SIDE_CTRL)) {
                rv = _phy_84793_system_polarity_flip (unit, port, 
		             pc, (uint16)value, PHY84793_POL_DND);
            } else {
                rv = _phy_84793_line_polarity_flip (unit, port, 
		               pc, (uint16)value, PHY84793_POL_DND);
	        }
        break;
        case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
            if (pc->flags & PHYCTRL_SYS_SIDE_CTRL) {
               rv = _phy_84793_sys_lane_squelch_enable (unit, port, lane, value);
            } else {
               rv = _phy_84793_line_lane_squelch_enable (unit, port, lane, value);
            }
        break;
        case SOC_PHY_CONTROL_LOOPBACK_INTERNAL: 
            rv = phy_84793_lb_set(unit, port, value); 
        break; 
        default:
            rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_84793_per_lane_control_get
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_84793_per_lane_control_get(int unit, soc_port_t port , int lane,
                               soc_phy_control_t type, uint32 *value)
{
    int32 rv = 0;
    phy_ctrl_t    *pc;       /* PHY software state */
    soc_port_t offset = 0;
    soc_port_t primary;

    CHECK_LANE (lane);
    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);

    rv = SOC_E_UNAVAIL;
    switch(type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
            rv = _phy_84793_per_lane_control_tx_preemphasis_get (unit, port, 
                       (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                       PHY84793_SYS_SIDE : PHY84793_LINE_SIDE, 
                       lane, value);
        break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
             rv = _phy_84793_per_lane_control_tx_driver_get(unit, port, type, 
                                            (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                                            PHY84793_SYS_SIDE : PHY84793_LINE_SIDE, 
                                            lane, value);
        break;
        case SOC_PHY_CONTROL_EDC_MODE:
            return SOC_E_UNAVAIL;
        case SOC_PHY_CONTROL_CLOCK_ENABLE:
            return SOC_E_UNAVAIL;
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
             rv = _phy_84793_per_lane_control_prbs_polynomial_get(unit, port, 
            		                                         lane, value);
        break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
            rv = _phy_84793_control_prbs_tx_invert_data_get(unit, port,
                    (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                     PHY84793_SYS_SIDE : PHY84793_LINE_SIDE,
                     value);
        break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
            /* fall through */
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
            rv = _phy_84793_per_lane_control_prbs_enable_get(unit, port,
            		                                     lane, value);
        break;
        case SOC_PHY_CONTROL_PRBS_RX_STATUS:
	        rv = _phy_84793_per_lane_control_prbs_rx_status_get(unit, port, lane,
                    value);
            break;
        case SOC_PHY_CONTROL_PORT_PRIMARY:
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_primary_get(unit, port, &primary));
            *value = (uint32) primary;
            rv = SOC_E_NONE;
        break;
        case SOC_PHY_CONTROL_PORT_OFFSET:
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_offset_get(unit, port, &offset));
            *value = (uint32) offset;
            rv = SOC_E_NONE;
        break;
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
            rv = _phy_84793_per_lane_remote_lb_get(unit, port, lane, value);
        break;
        case SOC_PHY_CONTROL_LOOPBACK_INTERNAL: 
            rv = _phy_84793_per_lane_lb_get(unit, port, lane, value);
        break; 
        case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
            return SOC_E_UNAVAIL;
        case SOC_PHY_CONTROL_RX_SEQ_DONE:
            rv = _phy_84793_per_lane_rx_seq_done_get(unit, port, 
                     (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ?
                     PHY84793_SYS_SIDE : PHY84793_LINE_SIDE, 
                     lane, value);
        break;
        default:
            rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

/*
 * Function:
 *      _phy_84793_dsc_diag
 * Purpose:
 *      Diag function for DSC .
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      intf - line/system
 *      lane - lane number 
 *      type  - Control to update
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_dsc_diag(int32 unit, soc_port_t port, int32 intf, int32 lane)
{
    int32 i = 0;
    int32 lanes = 0;
    uint32 reg_addr = 0;
    uint16 reg_val = 0;
    phy_ctrl_t *pc;
    int32 side = 0;
    uint16 hc_dc = 0, prev_val=0;
    struct phy84793_dsc_cb *dsc_cbp;
    struct phy84793_dsc_cb dsc_cb[PHY84793_LN_MAX_NO_LANE];

    pc = EXT_PHY_SW_STATE(unit, port);
    side =  (intf == PHY_DIAG_INTF_LINE) ? PHY84793_LINE_SIDE :
		PHY84793_SYS_SIDE;
	
    if (side == PHY84793_LINE_SIDE) {
        lanes = PHY84793_HC_LANE5 + 1;
        hc_dc = PHY84793_HEXA_CORE;
    } else {
        lanes = PHY84793_LN_MAX_NO_LANE;
        hc_dc = PHY84793_DECA_CORE;
    }	    
    sal_memset((char *)&dsc_cb[0], 0, (sizeof(dsc_cb)));
    /* display */
    LOG_CLI((BSL_META_U(unit,
                        "\nDSC parameters for port %d\n"), port));
    if (intf == PHY84793_LINE_SIDE) {
        LOG_CLI((BSL_META_U(unit,
                        "\nVSR core \n")));
        LOG_CLI((BSL_META_U(unit,
                        "LaneN  PPM clk90_ofs PF SL_TRGT LA  "
                 "PREC MAIN POSTC SIG_DET P1 Z1 P0 Z0\n")));
        for (i = 0; i <= PHY84793_VSR40_LANE3; i++) {
            dsc_cbp = &(dsc_cb[i]);
            /* TX pre MAIN and post tap*/
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, 
               ((VSR40_TX_ANA_LANE0_BASE | (i << 4)) + VSR40_TX_ANA25G_CONTROL5), 
               &reg_val));
            dsc_cb[i].tx_pre_cursor = (reg_val & 0x7100) >> 11;
            dsc_cb[i].tx_post_cursor = (reg_val & 0x7c0) >> 6; 
            dsc_cb[i].tx_main = (reg_val & 0x3f);
            
            /* Peeking filter control*/
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, 
              ((VSR40_RX_ANA_LANE0_BASE | (i << 8)) + VSR40_RX_ANA_IC_CTRL_REG), 
               &reg_val));
            dsc_cb[i].pf_ctrl = (reg_val & 0xf);
            
            /* Limiting Amplifier*/
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, 
              ((VSR40_RX_ANA_LANE0_BASE | (i << 8)) + VSR40_RX_ANA_CTRL1_REG), 
               &reg_val));
            dsc_cb[i].vga_sum = (reg_val & 0x71) >> 3;
            
            /* CDR Inter. status*/
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, 
              ((HS_RX_CDR_LANE_BASE | (i <<8)) + HS_RX_CDR_STAT_INTG_REG),
              &reg_val));
            dsc_cb[i].integ_reg = reg_val;
            
            /* CLK90 offset*/
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, 
              ((HS_RX_CDR_LANE_BASE | (i <<8)) + HS_RX_CDR_STAT_NCO_RDP),
              &reg_val));
            dsc_cb[i].clk90_offset = (reg_val & 0x7f00) >> 8;
            
            /* Slicer target*/
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, 
              (VSR40_RX_ANA_LANE0_BASE | (i << 8)), &reg_val));
            dsc_cb[i].slicer_target = ((reg_val & 0x1c) >> 2);
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, 
              ((VSR40_RX_ANA_LANE0_BASE | (i << 8)) + 1), &reg_val));
            dsc_cb[i].offset_po = (reg_val & 0xF);
            dsc_cb[i].offset_pe = (reg_val & 0xF0)>>4;
            dsc_cb[i].offset_zo = (reg_val & 0xF00)>>8;
            dsc_cb[i].offset_ze = (reg_val & 0xF000)>>12;
            
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, 
               VSR40_RX_DIG_REGS_STATUS, &reg_val));
            dsc_cb[i].sig_det = (reg_val & 
                (1 << (VSR40_RX_DIG_REGS_STATUS_ALOSB_SHIFT + i))) >>
                (VSR40_RX_DIG_REGS_STATUS_ALOSB_SHIFT + i);

            LOG_CLI((BSL_META_U(unit,
             "  %02d  %05d %04d    %04d "
             "%04d  %04d %04d %04d %04d    %04d" 
             "  %02d %02d %02d %02d \n"),
             i, dsc_cbp->integ_reg, dsc_cbp->clk90_offset,
            dsc_cbp->pf_ctrl, dsc_cbp->slicer_target, dsc_cbp->vga_sum,
            dsc_cbp->tx_pre_cursor, dsc_cbp->tx_main,
            dsc_cbp->tx_post_cursor,dsc_cbp->sig_det, dsc_cbp->offset_pe,
            dsc_cbp->offset_ze, dsc_cbp->offset_po, dsc_cbp->offset_zo));

        }
    }    
    sal_memset((char *)&dsc_cb[0], 0, (sizeof(dsc_cb)));
    if (hc_dc == PHY84793_DECA_CORE) {
        LOG_CLI((BSL_META_U(unit, "DECA CORE\n")));
    } else {
        LOG_CLI((BSL_META_U(unit, "HEXA CORE\n")));
    }
    LOG_CLI((BSL_META_U(unit,
                 "LaneN  PPM_XFR clk90_ofs PF SL_TRGT VGA DFE1 DFE2 "
                 "DFE3 DFE4 DFE5 POST1 MAIN POST2 SIGDET MTRC PE ZE ME PO ZO MO\n")));
    for (i = 0; i < lanes; i++) {
        dsc_cbp = &(dsc_cb[i]);
        /* vga_sum , dfe1_bin */
        reg_addr = (i << 16) | 0x8225;   
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
                      reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->vga_sum = (reg_val &0x003f); 
        dsc_cbp->dfe1_bin = ((reg_val >> 6) & 0x003f);

        /* dfe2_bin, dfe3_bin */
        reg_addr = (i << 16) | 0x8226;
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
                     reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->dfe2_bin = (reg_val & 0x003f); 
        dsc_cbp->dfe3_bin = ((reg_val >> 6) & 0x003f);

       
        /* dfe4_bin, dfe5_bin */
        reg_addr = (i << 16) | 0x8227;
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
                   reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->dfe4_bin = (reg_val & 0x001f);
        dsc_cbp->dfe5_bin = ((reg_val >> 5) & 0x001f);

        /* integ_reg_xfer */
        reg_addr = (i << 16) | 0x8220;
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
                 reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->integ_reg_xfer = reg_val & 0xffff;

        /* postc_metric */ 
        reg_addr = (i << 16) | 0x8220;
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
                     reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->postc_metric = (reg_val & 0x07ff);

        /* pf_ctrl */
        reg_addr = (i << 16) | 0x821c;
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
                       reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->pf_ctrl = (reg_val & 0x000f);
        
        /* clk90_offset */
        reg_addr = (i << 16) | 0x8223;
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
                   reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->clk90_offset = ((reg_val >> 7) & 0x007f);
        
        /* offset_pe, offset_po */
        reg_addr = (i << 16) | 0x822c;
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
                   reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->offset_pe = (reg_val & 0x003f);
        dsc_cbp->offset_po = ((reg_val >> 6) & 0x003f);
        
        /* offset_ze, offset_zo */
        reg_addr = (i << 16) | 0x822d;
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc, 
                   reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->offset_ze = (reg_val & 0x003f);
        dsc_cbp->offset_zo = ((reg_val >> 6) & 0x003f);
        
        /* offset_me, offset_zo */
        reg_addr = (i << 16) | 0x822e;
        SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
                   reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        dsc_cbp->offset_me = (reg_val & 0x003f);
        dsc_cbp->offset_mo = ((reg_val >> 6) & 0x003f);

        /* SIGGDET*/
        reg_addr = (i << 16) | 0x80b1;
        SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
            reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
        prev_val = reg_val;
        reg_val  &= ~0x7;
        reg_val |= 0x1;
        SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                   hc_dc, reg_addr, reg_val));
        reg_addr = (i << 16) | 0x80b0;
        SOC_IF_ERROR_RETURN(
          _phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
              reg_addr, PHY84793_DC_HC_RD_WIT_CLEAR, &reg_val));
        dsc_cbp->sig_det = (reg_val & 0x20) >> 5;
        
        reg_addr = (i << 16) | 0x80b1;
        SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                   hc_dc, reg_addr, prev_val));
        /* TX post, pre and main cursor*/
        if (hc_dc == PHY84793_DECA_CORE) {
            reg_addr = (i << 16) | 0x8067;
	        SOC_IF_ERROR_RETURN
             (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
             reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
            dsc_cbp->tx_main = ((reg_val & 0x1F8) >> 3);
            dsc_cbp->tx_pre_cursor = (reg_val & 0x7)<<2;
            
            reg_addr = (i << 16) | 0x8066;
            SOC_IF_ERROR_RETURN
             (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
              reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &reg_val));
            dsc_cbp->tx_post_cursor = (reg_val & 0x3800) >> 11;
            dsc_cbp->tx_pre_cursor |= (reg_val & 0xc000)>>14;
        } else {
            reg_addr = (i << 16) | 0x8067;
            PHY_84793_HCTX_PCB_READ (unit, pc, reg_addr, &reg_val);
            dsc_cbp->tx_main = ((reg_val & 0x1F8) >> 3);
            dsc_cbp->tx_pre_cursor = (reg_val & 0x7)<<2;
            
            reg_addr = (i << 16) | 0x8066;
            PHY_84793_HCTX_PCB_READ (unit, pc, reg_addr, &reg_val);
            dsc_cbp->tx_post_cursor = (reg_val & 0x3800) >> 11;
            dsc_cbp->tx_pre_cursor |= (reg_val & 0xc000)>>14;
        }
        LOG_CLI((BSL_META_U(unit,
             "  %02d    %05d    %04d    %02d   %02d    "
                "%03d %04d %04d" 
             " %04d %04d %04d %04d  %04d %04d   %04d  %04d %02d %02d %02d %02d %02d %02d\n"),
           i,dsc_cbp->integ_reg_xfer,dsc_cbp->clk90_offset,
           dsc_cbp->pf_ctrl,dsc_cbp->slicer_target,dsc_cbp->vga_sum,
           dsc_cbp->dfe1_bin,dsc_cbp->dfe2_bin,dsc_cbp->dfe3_bin,
           dsc_cbp->dfe4_bin,
           dsc_cbp->dfe5_bin,dsc_cbp->tx_pre_cursor,dsc_cbp->tx_main,
           dsc_cbp->tx_post_cursor, dsc_cbp->sig_det, 
           dsc_cbp->postc_metric,dsc_cbp->offset_pe,dsc_cbp->offset_ze,
           dsc_cbp->offset_me,
           dsc_cbp->offset_po,dsc_cbp->offset_zo,dsc_cbp->offset_mo));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *  phy_84793_cmd_eye_monitor 
 * Purpose:
 *     To execute the command for eyemonitor 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      core  - deca/hexa/VSR
 *      command - enable/run/disable
 *      lane_mask - lane nos for which eyemonitor 
 *                  command to be executed
 * Returns:
 *      SOC_E_NONE
 */
int32 phy_84793_cmd_eye_monitor(int32 unit, int32 port, uint8 core, uint8 command,
                                uint16 lane_mask)
{
	uint16 params;
	uint16 rddata = 0;
	uint16 count = 0;
    phy_ctrl_t *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

	PHY84793_EYE_MON_PARAMS(core, command, lane_mask, params);

    /* Write the command and params to GPReg5 */
    SOC_IF_ERROR_RETURN
       (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG5, params));

    /* Read GPReg5 to check lane bits are cleared by Firmware */
    SOC_IF_ERROR_RETURN
       (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG5, &rddata));
        
    while ((rddata & 0x03ff) && count <100) {
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN
          (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG5, &rddata));
	    count ++;	
    }

    if (count > 100) {
        return SOC_E_INTERNAL;
    }            

    /* Read GPReg5 for command status */
    SOC_IF_ERROR_RETURN
       (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG5, &rddata));

	/* return 0 for success , return error for failure */
    return ((rddata >> 10) & 0x7);
}

/*
 * Function:
 *  _phy_84793_diag_eyescan_livelink_en    
 * Purpose:
 *     To enable eyescan livelink 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      en - 1-enable, 0-disable
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_livelink_en(int unit, soc_port_t port, int16 intf, 
                                    int16 lane, int en)
{
    uint16 lane_mask = 0;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    uint8 core_id = 0; /*1.Deca 2.Hexa 3.VSR */
    int32 rv = 0;
    uint16 mode = 0;

    PHY84793_GET_PORT_MODE(pc, mode);
    if (intf == PHY84793_SYS_SIDE) {
        core_id = DECA_RT;	    
    }
    
    if (mode == PHY84793_MODE_10X10G_TO_4X25G) {
        if (intf == PHY84793_LINE_SIDE) {
            if (lane<=3) {
                core_id = VSR_RX;		
	        } else {
                return SOC_E_PARAM;
            }	
        }
    } else {
        if (intf == PHY84793_LINE_SIDE) {
            if (lane <= 3) {
                core_id = VSR_RX;		
	        } else {
                lane -= 4;
                core_id = HEXA_RX;
            }
        }
    }

    lane_mask = 1<<lane;

    if (en) {
        rv = phy_84793_cmd_eye_monitor(unit, port, core_id, ENABLE_EYE_MON, lane_mask);
    } else {
	    rv = phy_84793_cmd_eye_monitor(unit, port, core_id, DISABLE_EYE_MON, lane_mask);
    }

    return rv;
}

/*
 * Function:
 *      _phy_84793_diag_eyescan_livelink_clear    
 * Purpose:
 *      To clear the eyescan live link 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      unused -
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_livelink_clear(int unit, soc_port_t port, int unused)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *  _phy_84793_diag_eyescan_livelink_start    
 * Purpose:
 *     To start the eyescan livelink 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      unused -
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_livelink_start(int unit, soc_port_t port, int16 intf, 
                                       int16 lane)
{
    uint16 lane_mask = 0;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    uint8 core_id = 0; 
    int32 rv = 0;
    uint16 mode = 0;

    PHY84793_GET_PORT_MODE(pc, mode);
    if (intf == PHY84793_SYS_SIDE) {
        core_id = DECA_RT;	    
    }
    if (mode == PHY84793_MODE_10X10G_TO_4X25G) {
        if (intf == PHY84793_LINE_SIDE) {
            if (lane<=3) {
                core_id = VSR_RX;		
            } else {
         	    return SOC_E_PARAM;
	        }	
        }
    } else {
        if (intf == PHY84793_LINE_SIDE) {
            if (lane<=3) {
                core_id = VSR_RX;		
            } else {
                lane -= 4;
                core_id = HEXA_RX;		
            }
        }
    }
    lane_mask = 1 << lane;

    rv = phy_84793_cmd_eye_monitor(unit, port, core_id, RUN_EYE_MON, lane_mask);

    return rv;
}


/*
 * Function:
 *      _phy_84793_diag_eyescan_livelink_stop    
 * Purpose:
 *     To stop eyescan live link 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      unused -
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_livelink_stop(int unit, soc_port_t port, int unused)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_diag_eyescan_livelink_read 
 * Purpose:
 *      To read the result/error count 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      err_counter - error counter 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_livelink_read(int unit, soc_port_t port, int16 intf, 
                                      int16 lane, uint32 *err_counter)
{
    uint16 rdaddr = 0;
    uint16 rddata = 0;
    uint16 rddata1 = 0;
    int32 core_id = 0; /* 0:VSR 1:Deca core 2: Hexacore */
    uint32 reg_addr = 0;
    uint16 hc_dc = 0;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);
    uint16 mode = 0;
    
    PHY84793_GET_PORT_MODE(pc, mode);
    if (intf == PHY84793_SYS_SIDE) {
        core_id = DECA_RT;	    
        hc_dc = PHY84793_DECA_CORE;
    }
    
    if (mode == PHY84793_MODE_10X10G_TO_4X25G) {
        if (intf == PHY84793_LINE_SIDE) {
            if (lane<=3) {
                core_id = VSR_RX;		
            } else {
         	    return SOC_E_PARAM;
	        }	
        }	    
    } else {
        if (intf == PHY84793_LINE_SIDE) {
            if (lane <= 3) {
                core_id = VSR_RX;		
	        } else {
                lane -= 4;
                core_id = HEXA_RX;		
                hc_dc = PHY84793_HEXA_CORE;
	        }
        }
    }
    /* For Deca/Hexa core */
    if ((core_id == DECA_RT) || (core_id == HEXA_RX)) {
        reg_addr = (lane<<16) | 0x8252;
        SOC_IF_ERROR_RETURN
           (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
           reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
        
        reg_addr = (lane<<16) | 0x8251;
        SOC_IF_ERROR_RETURN
           (_phy_84793_dc_hc_pcb_read (unit, port, pc, hc_dc,
           reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata1));

        *err_counter = (rddata << 16) | rddata1;
    } else {
        rdaddr = 0x9418 | (lane << 8);
        SOC_IF_ERROR_RETURN
            (READ_PHY84793_PMA_PMD_REG(unit, pc, rdaddr, &rddata));
        SOC_IF_ERROR_RETURN
            (READ_PHY84793_PMA_PMD_REG(unit, pc, rdaddr+1, &rddata1));

        *err_counter = (rddata << 16) + rddata1; 
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_diag_eyescan_init_voffset_get 
 * Purpose:
 *      To get the initial value of the voffset 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      voffset - vertical offset 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_init_voffset_get(int unit, soc_port_t port, int *voffset)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_diag_eyescan_hoffset_set 
 * Purpose:
 *      To set the horizontal offset 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      hoffset - horizontal offset 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_hoffset_set(int unit, soc_port_t port, int *hoffset)
{
    uint16 hor_ver_offset = 0; 
    uint16 rddata = 0;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    hor_ver_offset = (uint8)(*hoffset);
    hor_ver_offset <<= 8;

    SOC_IF_ERROR_RETURN
           (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG6, &rddata));
    hor_ver_offset |= (rddata & 0x00FF);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG6, hor_ver_offset));
	
    return SOC_E_NONE;
}

/*
 * Function:
 *     _phy_84793_diag_eyescan_voffset_set 
 * Purpose:
 *      To set the vertical offset 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      voffset - vertical offset 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_voffset_set(int unit, soc_port_t port, int *voffset)
{
    uint16 rddata = 0;
    uint16 hor_ver_offset = 0; 
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
           (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG6, &rddata));
    hor_ver_offset  = ((uint8)(*voffset)); 
    hor_ver_offset |= (rddata & 0xFF00);

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG6, hor_ver_offset));

    return SOC_E_NONE;
}

/*
 * Function:
 *     _phy_84793_diag_eyescan_max_left_hoffset_get 
 * Purpose:
 *      To get the left horizontal offset 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      left_hoffset - left horizontal offset 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_max_left_hoffset_get(int unit, soc_port_t port, 
                                              int *left_hoffset)
{
    *left_hoffset = PHY84793_EYESCAN_MAX_LEFT_HOFFSET;
    return SOC_E_NONE;
}

/*
 * Function:
 *     _phy_84793_diag_eyescan_max_right_hoffset_get 
 * Purpose:
 *      To get the max right horizontal offset 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      right_hoffset - right horizontal offset 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_max_right_hoffset_get(int unit, soc_port_t port, 
                                              int *right_hoffset)
{
    *right_hoffset = PHY84793_EYESCAN_MAX_RIGHT_HOFFSET;
    return SOC_E_NONE;
}

/*
 * Function:
 *     _phy_84793_diag_eyescan_min_voffset_get 
 * Purpose:
 *      To get the minimum vertical offset 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      min_voffset - minimum vertical offset 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_min_voffset_get(int unit, soc_port_t port, 
                                        int *min_voffset)
{
    *min_voffset = PHY84793_EYESCAN_MIN_VOFFSET;
    return SOC_E_NONE;
}

/*
 * Function:
 *     _phy_84793_diag_eyescan_max_voffset_get 
 * Purpose:
 *      To get the maximum vertical offset 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      max_voffset - maximum vertical offset 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan_max_voffset_get(int unit, soc_port_t port, 
                                        int *max_voffset)
{
    *max_voffset = PHY84793_EYESCAN_MAX_VOFFSET;
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_diag_eyescan 
 * Purpose:
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      inst - instance
 *      op_cmd - operational command 
 *      arg - arguments to the command
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_diag_eyescan(int32 unit, soc_port_t port, uint32 inst, int32 op_cmd, 
                        void *arg)
{
    int32 rv = SOC_E_NONE;
    int16 intf = 0;
    int16 lane = 0, mode = 0;
    phy_ctrl_t *pc;
    intf = PHY_DIAG_INST_INTF(inst);
    lane = PHY_DIAG_INST_LN(inst);
    intf = (intf == PHY_DIAG_INTF_LINE)?PHY84793_LINE_SIDE:PHY84793_SYS_SIDE;
    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);
     
    if (mode == PHY84793_MODE_10X10G_TO_10X10G_ASYNC) {
        lane = pc->lane_num;
    }
    switch (op_cmd) {
        case PHY_DIAG_CTRL_EYE_ENABLE_LIVELINK:
            rv = _phy_84793_diag_eyescan_livelink_en(unit, port, intf , lane, 1);
        break;
	    case PHY_DIAG_CTRL_EYE_START_LIVELINK:
            rv = _phy_84793_diag_eyescan_livelink_start(unit, port, intf, lane);
        break;
        case PHY_DIAG_CTRL_EYE_DISABLE_LIVELINK:
            rv = _phy_84793_diag_eyescan_livelink_en(unit, port, intf, lane, 0);
        break;
        case PHY_DIAG_CTRL_EYE_STOP_LIVELINK:
            rv = _phy_84793_diag_eyescan_livelink_stop(unit, port, 0);
        break;
        case PHY_DIAG_CTRL_EYE_CLEAR_LIVELINK:
            rv = _phy_84793_diag_eyescan_livelink_clear(unit, port, 0);
        break;
        case PHY_DIAG_CTRL_EYE_READ_LIVELINK:
            rv = _phy_84793_diag_eyescan_livelink_read(unit, port, intf, lane, (uint32 *) arg);
        break;
        case PHY_DIAG_CTRL_EYE_GET_INIT_VOFFSET:
            rv = _phy_84793_diag_eyescan_init_voffset_get(unit, port, (int *)arg);
        break; 
        case PHY_DIAG_CTRL_EYE_SET_HOFFSET:
            rv = _phy_84793_diag_eyescan_hoffset_set(unit, port, (int*)arg);
        break;
        case PHY_DIAG_CTRL_EYE_SET_VOFFSET:
            rv = _phy_84793_diag_eyescan_voffset_set(unit, port, (int*)arg);
        break;
        case PHY_DIAG_CTRL_EYE_GET_MAX_LEFT_HOFFSET:
            rv = _phy_84793_diag_eyescan_max_left_hoffset_get(unit, port, (int *)arg);
        break;
        case PHY_DIAG_CTRL_EYE_GET_MAX_RIGHT_HOFFSET:
            rv = _phy_84793_diag_eyescan_max_right_hoffset_get(unit, port, (int *)arg);
        break;
        case PHY_DIAG_CTRL_EYE_GET_MIN_VOFFSET:
            rv = _phy_84793_diag_eyescan_min_voffset_get(unit, port, (int *)arg);
        break;
        case PHY_DIAG_CTRL_EYE_GET_MAX_VOFFSET:
            rv = _phy_84793_diag_eyescan_max_voffset_get(unit, port, (int *)arg);
        break;
    }
    return rv;
}

/*
 * Function:
 *      phy_84793_diag_ctrl 
 * Purpose:
 *      Diagnostics control function.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      inst  -
 *      op_type -
 *      op_cmd -
 *      arg -
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
phy_84793_diag_ctrl(int32 unit, soc_port_t port, uint32 inst, int32 op_type,
                                                     int32 op_cmd, void *arg)
{
    int32 lane;
    int32 intf;
    int32 rv = SOC_E_NONE;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "84793 diag_ctrl: u=%d p=%d ctrl=0x%x\n"), unit, 
                         port, op_cmd));
    lane = PHY_DIAG_INST_LN(inst);
    intf = PHY_DIAG_INST_INTF(inst);

    if (intf == PHY_DIAG_INTF_SYS) {
        pc->flags |= PHYCTRL_SYS_SIDE_CTRL;	    
    } 	    

    switch (op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            SOC_IF_ERROR_RETURN(_phy_84793_dsc_diag(unit, port, intf, lane));
        break;
        case PHY_DIAG_CTRL_EYE_ENABLE_LIVELINK:
        case PHY_DIAG_CTRL_EYE_DISABLE_LIVELINK:
        case PHY_DIAG_CTRL_EYE_CLEAR_LIVELINK:
        case PHY_DIAG_CTRL_EYE_START_LIVELINK:
        case PHY_DIAG_CTRL_EYE_STOP_LIVELINK:
        case PHY_DIAG_CTRL_EYE_READ_LIVELINK:
        case PHY_DIAG_CTRL_EYE_GET_INIT_VOFFSET:
        case PHY_DIAG_CTRL_EYE_SET_HOFFSET:
        case PHY_DIAG_CTRL_EYE_SET_VOFFSET:
        case PHY_DIAG_CTRL_EYE_GET_MAX_LEFT_HOFFSET:
        case PHY_DIAG_CTRL_EYE_GET_MAX_RIGHT_HOFFSET:
        case PHY_DIAG_CTRL_EYE_GET_MIN_VOFFSET:
        case PHY_DIAG_CTRL_EYE_GET_MAX_VOFFSET:
	        rv = _phy_84793_diag_eyescan(unit, port, inst, op_cmd, arg);
        break;
        case PHY_DIAG_CTRL_EYE_MARGIN_VEYE:
        case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_LEFT:
        case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_RIGHT:
	        rv = SOC_E_UNAVAIL;
        break;
        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                SOC_IF_ERROR_RETURN
                (phy_84793_control_set(unit, port,
                                op_cmd, PTR_TO_INT(arg)));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                SOC_IF_ERROR_RETURN
                (phy_84793_control_get(unit, port,
                                op_cmd, (uint32 *)arg));
            } else {
                LOG_INFO(BSL_LS_SOC_PHY, \
                         (BSL_META_U(unit, \
                                     "84793 diag_ctrl bad operation:\
                                     u=%d p=%d ctrl=0x%x\n"), unit, port, op_cmd));
                rv = SOC_E_UNAVAIL;
            }
        break;
    }
    return rv;

}
/*
 * Function:
 *      _phy_84793_prbs_gen_get  
 * Purpose:
 *      To check the status of IEEE PRBS generator whether or not enabled .
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      lane - lane number 
 *      tx_gen - tx gen enable/disable (0/1)
 *      rx_gen - rx gen enable /disable (0/1)
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_prbs_gen_get(int32 unit, soc_port_t port, int32 lane,
                                         uint16 *tx_gen, uint16 *rx_gen)
{ 
    uint16 rddata = 0;
    uint16 mode = 0;
    int32 reg_addr = 0;
    phy_ctrl_t     *pc;
    uint16 intf = 0;
    pc = EXT_PHY_SW_STATE(unit, port);

    if ((!tx_gen) || (!rx_gen)) {
        return SOC_E_PARAM;
    }
    PHY84793_GET_PORT_MODE(pc, mode);
    if ((mode == PHY84793_MODE_4X10G_TO_4X10G) && (lane == 4 || lane == 9)) {
        return SOC_E_PARAM;
    }
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE : PHY84793_LINE_SIDE;
    if (intf == PHY84793_SYS_SIDE) {
        reg_addr = (lane << 16) | 0x8069;  
        SOC_IF_ERROR_RETURN
           (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE, 
                                       reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
     
        if ((rddata & 0x0060) == 0x0060) {
            *rx_gen = 1;
        } else {
            *rx_gen = 0;
        }
    } else {
        if (lane <= PHY84793_VSR40_LANE3) {
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG (unit, pc, RG_TX_TX_GB_PRBS_DB_CTRL,
                                          &rddata));
            if (rddata & (1 << (lane + RG_TX_TX_GB_PRBS_DB_CTRL_PRBS_GEN_EN_SHIFT))) {
                *tx_gen = 1;
            } else {
                *tx_gen = 0;
            }
        } else {
            if ((mode == PHY84793_MODE_10X10G_TO_4X25G) || 
                    (mode == PHY84793_MODE_10X10G_TO_4X25G_OTL)) {
                return SOC_E_NONE;
            }

            reg_addr = ((lane-4) << 16) | 0x8069; 
            PHY_84793_HCTX_PCB_READ (unit, pc, reg_addr, &rddata);
            if ((rddata & 0x0060) == 0x0060) {
                *tx_gen = 1;
            } else {
                *tx_gen = 0;
            }
        }
    }
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_prbs_chkr_get
 * Purpose:
 *      To check the status of PRBS whether or not enabled 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      lane  - lane number 
 *      tx_chk - tx checker enable/disable
 *      rx_chk - rx checker enable/disable 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_prbs_chkr_get(int32 unit, soc_port_t port, int32 lane,
                                          uint16 *tx_chk, uint16 *rx_chk)
{
    uint16 rddata = 0;
    uint16 mode = 0;
    phy_ctrl_t     *pc;
    int32 reg_addr = 0;
    uint16 intf = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);

    if ((!tx_chk) || (!rx_chk)) {
        return SOC_E_PARAM;
    }
    PHY84793_GET_PORT_MODE(pc, mode);
    
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE : PHY84793_LINE_SIDE;
    if ((mode == PHY84793_MODE_4X10G_TO_4X10G) && (lane == 4 || lane == 9)) {
        return SOC_E_PARAM;
    }
    if (intf == PHY84793_SYS_SIDE) {
        reg_addr = (lane << 16) | 0x80b6;  
        SOC_IF_ERROR_RETURN
           (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE, 
                               reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
     
        if ((rddata & 0x0060) == 0x0060) {
            *tx_chk = 1;
        } else {
            *tx_chk = 0;
        }
    } else {
        if (lane <= PHY84793_VSR40_LANE3) {
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG (unit, pc, RG_RX_RX_GB_TEST_CTRL_LN03,
                                          &rddata));
            if (rddata & (1 << lane)) {
                *rx_chk = 1;
            } else {
                *rx_chk = 0;
            }
        } else {
            if ((mode == PHY84793_MODE_10X10G_TO_4X25G) || 
                    (mode == PHY84793_MODE_10X10G_TO_4X25G_OTL)) {
                return SOC_E_NONE;
            }

            reg_addr = ((lane-4) << 16) | 0x80b6;  
            SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE, 
                               reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
 
            if ((rddata & 0x0060) == 0x0060) {
                *rx_chk = 1;
            } else {
                *rx_chk = 0;
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_prbs_gen_enable   
 * Purpose:
 *      To enable PRBS generator enable .
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      lane  - lane number 
 *      en_tx - enable/disable PRBS generator at tx side
 *      en_rx - enable/disable PRBS generator at rx side 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_prbs_gen_enable(int32 unit, soc_port_t port, int32 lane, 
                                                 uint16 en_tx, uint16 en_rx)
{
    phy_ctrl_t     *pc;
    uint16 mode = 0, rddata = 0; 
    int32 reg_addr = 0;
    uint16 intf = 0;
    pc = EXT_PHY_SW_STATE(unit, port);

    PHY84793_GET_PORT_MODE(pc, mode);
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE : PHY84793_LINE_SIDE;
    if ((mode == PHY84793_MODE_4X10G_TO_4X10G) && (lane == 4 || lane == 9)) {
        return SOC_E_PARAM;
    }
    if (intf == PHY84793_SYS_SIDE) {
        reg_addr = (lane << 16) | 0x8069;  
        SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE, 
                      reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
        rddata &= (~0x60);
        rddata |= (en_rx ? (3 << 5):0);
        SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE, 
             reg_addr, rddata));
    } else {
        if (lane <= PHY84793_VSR40_LANE3) {
            SOC_IF_ERROR_RETURN
             (_phy_84793_modify_pma_pmd_reg(unit, port, pc, RG_TX_TX_GB_PRBS_DB_CTRL ,
             RG_TX_TX_GB_PRBS_DB_CTRL_PRBS_GEN_EN_SHIFT + lane,
             RG_TX_TX_GB_PRBS_DB_CTRL_PRBS_GEN_EN_SHIFT + lane, 
             en_tx));
        } else {
            if (mode == PHY84793_MODE_10X10G_TO_4X25G || 
                    mode == PHY84793_MODE_10X10G_TO_4X25G_OTL) {
                 return SOC_E_NONE;
            }
            reg_addr = ((lane-4) << 16) | 0x8069;  
            PHY_84793_HCTX_PCB_READ (unit, pc, reg_addr, &rddata);
            rddata &= (~0x60);
            rddata |= (en_tx ? (3<< 5):0);
            PHY_84793_HCTX_PCB_WRITE (unit, pc, reg_addr, rddata);
        }
    }
    
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_prbs_chkr_enable
 * Purpose:
 *      To enable PRBS checker .
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      lane  - lane number
 *      en_tx - tx enable/disable
 *      en_rx - rx enable/disable 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_prbs_chkr_enable(int32 unit, soc_port_t port, int32 lane,
                                 uint16 en_tx, uint16 en_rx)
{
    phy_ctrl_t     *pc;
    uint16 mode = 0, rddata = 0;
    int32 reg_addr = 0;
    uint16 intf = 0;
    
    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE : PHY84793_LINE_SIDE;

    if ((mode == PHY84793_MODE_4X10G_TO_4X10G) && (lane == 4 || lane == 9)) {
        return SOC_E_PARAM;
    }
    if (intf == PHY84793_SYS_SIDE) {
        reg_addr = (lane << 16) | 0x80b6;  
        SOC_IF_ERROR_RETURN
           (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE, 
                           reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));

        rddata &= (~0x60);
        rddata |= (en_tx ? (3 << 5):0);
        SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE, 
             reg_addr, rddata));
    } else {
        if (lane <= PHY84793_VSR40_LANE3) {
            SOC_IF_ERROR_RETURN
             (_phy_84793_modify_pma_pmd_reg(unit, port, pc, RG_RX_RX_GB_TEST_CTRL_LN03,
             RG_RX_RX_GB_TEST_CTRL_LN03_PRBS_CHK_EN_SHIFT + lane,
             RG_RX_RX_GB_TEST_CTRL_LN03_PRBS_CHK_EN_SHIFT + lane, en_rx));
        } else {
            if (mode == PHY84793_MODE_10X10G_TO_4X25G || 
                mode == PHY84793_MODE_10X10G_TO_4X25G_OTL) {
                return SOC_E_NONE;
            }
            reg_addr = ((lane-4) << 16) | 0x80b6;  
            SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE, 
                        reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
            rddata &= (~0x60);
            rddata |= (en_rx ? (3 << 5):0);

            SOC_IF_ERROR_RETURN
             (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_HEXA_CORE, 
             reg_addr,rddata));
        }
        /* Setting bit to make LOS of lock work*/
        /* JIRA PHY-1465*/
        SOC_IF_ERROR_RETURN
             (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_FW_ENABLE_TYPE,
               1, 0, en_rx));

    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_control_prbs_tx_invert_data_set
 * Purpose:
 *      To invert the PRBS data at tx side.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      invert - invert data (0/1) 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_control_prbs_tx_invert_data_set(int32 unit, soc_port_t port, 
                                    PHY84793_INTF_SIDE intf_side, int32 invert)
{
    phy_ctrl_t     *pc;
    uint16 lane = 0, rddata = 0, mode = 0;
    uint32 reg_addr = 0;
    
    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);
    if (intf_side == PHY84793_SYS_SIDE) {
        for (lane =0; lane <= PHY84793_DC_LANE9; lane ++) {
            /* For Systemside(DC) */
            reg_addr = (lane << 16) | 0x80b6;
            SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE, 
                reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
            rddata &= (~0x10);
            rddata |= ((invert & (1<<lane)) ? (1<<4) :0);

            SOC_IF_ERROR_RETURN
             (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE, 
             reg_addr, rddata));
        }
    } else {
        for (lane =0; lane < PHY84793_LN_MAX_NO_LANE; lane ++) {
            if (lane < PHY84793_VSR40_LANE3) {
                /* For VSR*/
                SOC_IF_ERROR_RETURN
                 (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                  RG_TX_TX_GB_TEST_GEN_CTRL, 
                  RG_TX_TX_GB_TEST_GEN_CTRL_PRBS_INV_SHIFT +  lane,
                  RG_TX_TX_GB_TEST_GEN_CTRL_PRBS_INV_SHIFT + lane , 
                  (invert & (1<<lane)?1:0)));
            } else {
                if (mode == PHY84793_MODE_10X10G_TO_4X25G || 
                        mode == PHY84793_MODE_10X10G_TO_4X25G_OTL) {
                   return SOC_E_NONE;
                }
                reg_addr = ((lane-4) << 16) | 0x8069;
                PHY_84793_HCTX_PCB_READ (unit, pc, reg_addr, &rddata);
                rddata &= (~0x10);
                rddata |= ((invert & (1<<lane)) ? (1<<4) :0);

                PHY_84793_HCTX_PCB_WRITE (unit, pc, reg_addr, rddata);
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_control_prbs_rx_invert_data_set
 * Purpose:
 *      To invert the PRBS data at rx side
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      invert - invert data (0/1) 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32  
_phy_84793_control_prbs_rx_invert_data_set(int32 unit, soc_port_t port,
                               PHY84793_INTF_SIDE intf_side, int32 invert)
{
    phy_ctrl_t     *pc;
    uint16 lane = 0, rddata = 0, mode =0;
    uint32 reg_addr = 0;
    pc = EXT_PHY_SW_STATE(unit, port);
    
    PHY84793_GET_PORT_MODE(pc, mode);
    if (intf_side == PHY84793_SYS_SIDE) {
        for (lane =0; lane <= PHY84793_DC_LANE9; lane ++) {
            /* For Systemside(DC) */
            reg_addr = (lane << 16) | 0x8069;
            SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE, 
                reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
            rddata &= (~0x10);
            rddata |= ((invert & (1<<lane)) ? (1<<4) :0);

            SOC_IF_ERROR_RETURN
             (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE, 
             reg_addr, rddata));
        }
    } else {
        for (lane =0; lane < PHY84793_LN_MAX_NO_LANE; lane ++) {
            if (lane < PHY84793_VSR40_LANE3) {
                /* For VSR*/
                SOC_IF_ERROR_RETURN
                 (_phy_84793_modify_pma_pmd_reg (unit, port, pc, RG_RX_RX_GB_PRBS_INV_IN,
                    RG_RX_RX_GB_PRBS_INV_IN_PRBS_INV_IN_SHIFT + lane,
                    RG_RX_RX_GB_PRBS_INV_IN_PRBS_INV_IN_SHIFT + lane, 
                    (invert & (1<<lane)?1:0)));
            } else {
                if (mode == PHY84793_MODE_10X10G_TO_4X25G || 
                        mode == PHY84793_MODE_10X10G_TO_4X25G_OTL) {
                   return SOC_E_NONE;
                }
                reg_addr = ((lane-4) << 16) | 0x80b6;
                SOC_IF_ERROR_RETURN
                   (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE, 
                   reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
                rddata &= (~0x10);
                rddata |= ((invert & (1<<lane)) ? (1<<4) :0);

                SOC_IF_ERROR_RETURN
                 (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_HEXA_CORE, 
                  reg_addr, rddata));
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_per_lane_control_prbs_polynomial_set
 * Purpose:
 *      To set the polynomial per lane basis. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      lane  - lane number 
 *      poly_ctrl - polynomial type
 *                  currently PRBS31 ie poly_ctrl=3 is supported
 *                  There is no PRBS9 support available in SDK 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_per_lane_control_prbs_polynomial_set(int32 unit, soc_port_t port,
                                                int32 lane, int32 poly_ctrl)
{
    uint16 mode = 0;
    uint16 rddata = 0, reg_bit = 0, wrdata = 0;
    int32 reg_addr = 0;
    uint16 intf = 0;
	
    phy_ctrl_t     *pc;
    pc = EXT_PHY_SW_STATE(unit, port);
    
    PHY84793_GET_PORT_MODE(pc, mode);
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE : PHY84793_LINE_SIDE;

    if ((mode == PHY84793_MODE_4X10G_TO_4X10G) && (lane == 4 || lane == 9)) {
        return SOC_E_PARAM;
    }
    switch (poly_ctrl) {
        case PHY84793_PRBS_7:
            reg_bit &= (~0x10c);
        break;
        case PHY84793_PRBS_15:
            reg_bit &= (~0x108);
            reg_bit |= 0x4;
        break;
        case PHY84793_PRBS_23:
            reg_bit &= (~0x104);
            reg_bit |= 0x8;
        break;
        case PHY84793_PRBS_31:
            reg_bit |= 0x000c;
            reg_bit &= (~0x100);
        break;
        case PHY84793_PRBS_9:
            reg_bit &= (~0x0c);
            reg_bit |= 0x100;
        break;
        case PHY84793_PRBS_11:
            reg_bit &= (~0x08);
            reg_bit |= 0x104;
        break;
        /*
         * COVERITY
         * This default is unreachable. It is kept intentionally as a defensive 
         * default for future development.
         */
        /* coverity[dead_error_begin] */
        case PHY84793_PRBS_FIXED_PATT:
            return SOC_E_UNAVAIL;
    }
    if ( intf == PHY84793_SYS_SIDE) {
        /* System Side polynomial Set*/
        /* Deca Core prbs generator */
        reg_addr = (lane << 16) | 0x8069;
        SOC_IF_ERROR_RETURN
          (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
           reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
        
        rddata &= ~0x10c ;
        wrdata = rddata | reg_bit;
    	
        SOC_IF_ERROR_RETURN
          (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE, 
           reg_addr, wrdata));
    
        /* enable checker */
        reg_addr = (lane << 16) | 0x80b6;
        SOC_IF_ERROR_RETURN
          (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE, 
            reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
    
        rddata &= ~0x10c ;
        wrdata = rddata | reg_bit;
    
        SOC_IF_ERROR_RETURN
          (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE, 
            reg_addr, wrdata));
    } else {
        /* Line Side polynomial Set */
        if (lane <= PHY84793_VSR40_LANE3) {  /* VSR */
            switch(poly_ctrl) {
                case PHY84793_PRBS_7:
                    wrdata = 0x1;
                break;
                case PHY84793_PRBS_15:
                    wrdata = 0x4;
                break;
                case PHY84793_PRBS_23:
                    wrdata = 0x5;
                break;
                case PHY84793_PRBS_31:
                    wrdata = 0x6;
                break;
                case PHY84793_PRBS_9:
                    wrdata = 0x2;
                break;
                case PHY84793_PRBS_11:
                    wrdata = 0x3;
                break;
                /*
                 * COVERITY
                 * This default is unreachable. It is kept intentionally as a defensive 
                 * default for future development.
                 */
                /* coverity[dead_error_begin] */
                case PHY84793_PRBS_FIXED_PATT:
                    return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN
              (_phy_84793_modify_pma_pmd_reg (unit, port, pc, RG_TX_TX_GB_TEST_GEN_CTRL,
              (lane* 3) +2 , (lane *3), wrdata));
    
            SOC_IF_ERROR_RETURN
              (_phy_84793_modify_pma_pmd_reg (unit, port, pc, RG_RX_RX_GB_PRBS_INV_IN ,
              (lane* 3) +2 , (lane *3), wrdata));
        } else {
            if (mode == PHY84793_MODE_10X10G_TO_4X25G || 
                    mode == PHY84793_MODE_10X10G_TO_4X25G_OTL) {
                return SOC_E_NONE;
            }
    
            /* Hexa Core prbs generator */
            reg_addr = ((lane -4) << 16) | 0x8069;
            PHY_84793_HCTX_PCB_READ (unit, pc, reg_addr, &rddata);
         
            rddata &= ~0x10c ;
            wrdata = rddata | reg_bit;
    	
            PHY_84793_HCTX_PCB_WRITE (unit, pc, reg_addr, wrdata);
    
            /* enable checker */
            reg_addr = ((lane - 4)<< 16) | 0x80b6;
            SOC_IF_ERROR_RETURN
             (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE, 
                reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
    
            rddata &= ~0x10c ;
            wrdata = rddata | reg_bit;
    
            SOC_IF_ERROR_RETURN
             (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_HEXA_CORE, 
              reg_addr, wrdata));
        } 
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_control_prbs_polynomial_set
 * Purpose:
 *      To set the polynomial type.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      poly_ctrl - polynomial type 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_control_prbs_polynomial_set(int32 unit, soc_port_t port,
                                       int32 poly_ctrl)
{
    uint16 mode = 0;
    uint16 start_lane = 0, end_lane =0;
    uint32 lane = 0;
	
    phy_ctrl_t     *pc;
    pc = EXT_PHY_SW_STATE(unit, port);
    
    /* Only PRBS 7, 15,23, 31,9,11 */
    if (poly_ctrl != PHY84793_PRBS_7 && poly_ctrl != PHY84793_PRBS_15 &&
        poly_ctrl != PHY84793_PRBS_23 && poly_ctrl != PHY84793_PRBS_31 &&
        poly_ctrl != PHY84793_PRBS_9 && poly_ctrl != PHY84793_PRBS_11) {
        return SOC_E_PARAM;
    }
	
    PHY84793_GET_PORT_MODE(pc, mode);
 
    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:			
        case PHY84793_MODE_10X10G_TO_4X25G_OTL:
            for (lane = 0; lane < PHY84793_LN_MAX_NO_LANE; lane ++) {
                SOC_IF_ERROR_RETURN
                 (_phy_84793_per_lane_control_prbs_polynomial_set(unit, port,
                  lane, poly_ctrl));
            }
        break;
        case PHY84793_MODE_4X10G_TO_4X10G:
            if(pc->lane_num == 0) {
                start_lane = 0;
                end_lane = PHY84793_VSR40_LANE3;
            } else {
                start_lane = 5;
                end_lane = 8;
            }
            for (lane = start_lane; lane <= end_lane; lane ++) {
                SOC_IF_ERROR_RETURN
                 (_phy_84793_per_lane_control_prbs_polynomial_set(unit, port,
                  lane, poly_ctrl));
            }
        break; 
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
            for (lane = 0; lane <= PHY84793_LN_MAX_NO_LANE; lane ++) {
                SOC_IF_ERROR_RETURN
                  (_phy_84793_per_lane_control_prbs_polynomial_set(unit, port,
                   lane, poly_ctrl));
            }
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:		
            SOC_IF_ERROR_RETURN
              (_phy_84793_per_lane_control_prbs_polynomial_set(unit, port,
               pc->lane_num, poly_ctrl));
        break;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     _phy_84793_per_lane_control_prbs_enable_set 
 * Purpose:
 *      To enable/disable PRBS per lane basis.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      lane  - lane number 
 *      enable - enable/disable 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_per_lane_control_prbs_enable_set(int32 unit, soc_port_t port,
                                             int32 lane, int32 enable)
{
    uint16 en_tx = 0;
    uint16 en_rx = 0;
    if (enable) {
        en_tx = 1;
        en_rx = 1;
    } else {
        en_tx = 0;
        en_rx = 0;
    }
	
    SOC_IF_ERROR_RETURN	
            (_phy_84793_prbs_gen_enable(unit, port, lane, en_tx, en_rx));
    SOC_IF_ERROR_RETURN	
            (_phy_84793_prbs_chkr_enable(unit, port, lane, en_tx, en_rx));
	
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_control_prbs_enable_set
 * Purpose:
 *      To enable/disable PRBS.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      enable - enable /disable 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_control_prbs_enable_set(int32 unit, soc_port_t port, int32 enable)
{
    uint16 mode = 0;
    int32 lane = 0, start_lane = 0, end_lane = 0;
	
    phy_ctrl_t     *pc;
    pc = EXT_PHY_SW_STATE(unit, port);
	
    PHY84793_GET_PORT_MODE(pc, mode)

    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:			
        case PHY84793_MODE_10X10G_TO_4X25G_OTL:
       	    for (lane = 0; lane < PHY84793_LN_MAX_NO_LANE; lane ++) {
                SOC_IF_ERROR_RETURN
                 (_phy_84793_per_lane_control_prbs_enable_set(unit, port, lane,
                 enable));
            }
        break;
        case PHY84793_MODE_4X10G_TO_4X10G:   
            if(pc->lane_num == 0) {
                start_lane = 0;
                end_lane = PHY84793_VSR40_LANE3;
            } else {
                start_lane = 5;
                end_lane = 8;
            }
            for (lane = start_lane; lane <= end_lane; lane ++) {
                SOC_IF_ERROR_RETURN
                  (_phy_84793_per_lane_control_prbs_enable_set(unit, port, lane,
                     enable));
            }
        break; 
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
            for (lane = start_lane; lane <= end_lane; lane ++) {
                SOC_IF_ERROR_RETURN
                (_phy_84793_per_lane_control_prbs_enable_set(unit, port, pc->lane_num,
                                                                     enable));
            }
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:		
            SOC_IF_ERROR_RETURN
                (_phy_84793_per_lane_control_prbs_enable_set(unit, port, pc->lane_num,
                                                                       enable));
        break;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_per_lane_control_prbs_enable_get
 * Purpose:
 *      Check whether or not PRBS is enable for given lane.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      lane  - lane number
 *      value - enable/disable 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_per_lane_control_prbs_enable_get(int32 unit, soc_port_t port,
                                               int32 lane, uint32 *value)
{
    uint16 tx_gen = 0;
    uint16 rx_gen = 0;
    uint16 tx_chk = 0;
    uint16 rx_chk = 0;
    
    SOC_IF_ERROR_RETURN
            (_phy_84793_prbs_gen_get(unit, port, lane, &tx_gen, &rx_gen));
    SOC_IF_ERROR_RETURN
            (_phy_84793_prbs_chkr_get(unit, port, lane, &tx_chk, &rx_chk));	
			
    if (tx_gen == 1 || rx_gen == 1 || tx_chk == 1 || rx_chk == 1) {
        *value = TRUE;
    } else {
        *value = FALSE;
    }	
	
    return SOC_E_NONE;
}

/*
 * Function:
 *     _phy_84793_control_prbs_enable_get 
 * Purpose:
 *      To check whether or not prbs checker/gen
 *      is enabled.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      value - enable / disale 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_control_prbs_enable_get(int32 unit, soc_port_t port, uint32 *value)
{
    uint16 mode = 0, start_lane = 0, end_lane =0;
    int32 lane = 0;
    uint32 prbs_sts = 0;
	
    phy_ctrl_t     *pc;
    pc = EXT_PHY_SW_STATE(unit, port);
	
    PHY84793_GET_PORT_MODE(pc, mode)
    *value = 0;
    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:			
        case PHY84793_MODE_10X10G_TO_4X25G_OTL:
            for (lane = 0; lane <PHY84793_LN_MAX_NO_LANE; lane ++) {
                SOC_IF_ERROR_RETURN
                 (_phy_84793_per_lane_control_prbs_enable_get(unit, port, lane, 
                 &prbs_sts));
                *value |= prbs_sts;
            }
        break;
        case PHY84793_MODE_4X10G_TO_4X10G:
            if (pc->lane_num == 0) {
                start_lane = PHY84793_VSR40_LANE0;
                end_lane = PHY84793_VSR40_LANE3;
            } else {
                start_lane = 5;
                end_lane = 8;
            }
            for (lane = start_lane; lane <= end_lane; lane ++) {
                SOC_IF_ERROR_RETURN
                  (_phy_84793_per_lane_control_prbs_enable_get(unit, port, lane,
                  &prbs_sts));
                *value |= prbs_sts ;
       	    }
        break; 
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
            for (lane = 0; lane < PHY84793_LN_MAX_NO_LANE; lane ++) {
                SOC_IF_ERROR_RETURN
                (_phy_84793_per_lane_control_prbs_enable_get(unit, port, lane,
                 &prbs_sts));
                *value |= prbs_sts;
            }
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:		
            SOC_IF_ERROR_RETURN
              (_phy_84793_per_lane_control_prbs_enable_get(unit, port, pc->lane_num,
                 &prbs_sts));
            *value = prbs_sts;
        break;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_per_lane_control_prbs_polynomial_get
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      value - PRBS polynomial type 
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32 
_phy_84793_per_lane_control_prbs_polynomial_get(int32 unit, soc_port_t port, 
                                                    int32 lane, uint32 *value)
{    
    uint16 rddata = 0, poly_ctrl = 0;
    uint16 mode = 0;
    int32 reg_addr = 0;
    phy_ctrl_t     *pc;
    uint16 intf = 0;
      	
    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE : PHY84793_LINE_SIDE;

    if ((mode == PHY84793_MODE_4X10G_TO_4X10G) && (lane == 4 || lane == 9)) {
        return SOC_E_PARAM;
    }
    
    /* Since Both line and system side configured with same polynomial
     * its sufficient to get system side alone*/
    if (intf == PHY84793_SYS_SIDE) {
        reg_addr = (lane << 16) | 0x8069;
        SOC_IF_ERROR_RETURN
         (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE, 
             reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
            
        switch ((rddata & 0xc)) {
            case 0xC:
                *value = PHY84793_PRBS_31;
            break;
            case 0:
                if ((rddata & 0x100) == 0x100) {
                   *value = PHY84793_PRBS_9;
                } else {
                   *value = PHY84793_PRBS_7;
                }
            break;
            case 4:
                if ((rddata & 0x100) == 0x100) {
                   *value = PHY84793_PRBS_11;
                } else {
                   *value = PHY84793_PRBS_15;
                }
            break;
            case 8:
                *value = PHY84793_PRBS_23;
            break;
        }
    } else {
        if (lane < PHY84793_VSR40_LANE3) {
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG 
                (unit, pc, RG_TX_TX_GB_TEST_GEN_CTRL, &rddata));
            poly_ctrl = (rddata >> (lane *3)) & 7;
            switch(poly_ctrl) {
                case 0x1:
                    *value = PHY84793_PRBS_7;
                break;
                case 0x4:
                    *value = PHY84793_PRBS_15;
                break;
                case 0x5:
                    *value = PHY84793_PRBS_23;
                break;
                case 0x6:
                    *value = PHY84793_PRBS_31;
                break;
                case 0x2:
                    *value = PHY84793_PRBS_9;
                break;
                case 0x3:
                    *value = PHY84793_PRBS_11;
                break;
            }
        } else {
            reg_addr = ((lane - 4) << 16) | 0x8069;
            SOC_IF_ERROR_RETURN
             (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE, 
                 reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
                
            switch ((rddata & 0xc)) {
                case 0xC:
                    *value = PHY84793_PRBS_31;
                break;
                case 0:
                    if ((rddata & 0x100) == 0x100) {
                       *value = PHY84793_PRBS_9;
                    } else {
                       *value = PHY84793_PRBS_7;
                    }
                break;
                case 4:
                    if ((rddata & 0x100) == 0x100) {
                       *value = PHY84793_PRBS_11;
                    } else {
                       *value = PHY84793_PRBS_15;
                    }
                break;
                case 8:
                    *value = PHY84793_PRBS_23;
                break;
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_control_prbs_polynomial_get
 * Purpose:
 *      Get PRBS polynomial type
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      value - PRBS polynomial type
 * Returns:
 *      SOC_E_NONE
 */
STATIC int32
_phy_84793_control_prbs_polynomial_get(int32 unit, soc_port_t port, 
                                                        uint32 *value)
{
    uint16 mode = 0, lane = 0;
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
	
    PHY84793_GET_PORT_MODE(pc, mode)

    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:			
        case PHY84793_MODE_10X10G_TO_4X25G_OTL:
            SOC_IF_ERROR_RETURN
             (_phy_84793_per_lane_control_prbs_polynomial_get(unit, port,
                                                       pc->lane_num, value));
  
        break;
        case PHY84793_MODE_4X10G_TO_4X10G:            
            if (pc->lane_num == 0) {
                lane = PHY84793_VSR40_LANE0;
            } else {
                lane = 5;
            }
            SOC_IF_ERROR_RETURN
             (_phy_84793_per_lane_control_prbs_polynomial_get(unit, port,
                                                       lane, value));
        break; 
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:		
            SOC_IF_ERROR_RETURN
             (_phy_84793_per_lane_control_prbs_polynomial_get(unit, port,
                                                            pc->lane_num, value));
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
           SOC_IF_ERROR_RETURN
             (_phy_84793_per_lane_control_prbs_polynomial_get(unit, port,
                                                            pc->lane_num, value));

        break;
    }
    return SOC_E_NONE;
}
/*
 * Function:
 *      _phy_84793_control_prbs_tx_invert_data_get
 * Purpose:
 *      To check whether or not invert data enabled.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      value - enable/disable 
 * Returns:
 *      SOC_E_NONE
 */

STATIC int32 
_phy_84793_control_prbs_tx_invert_data_get(int32 unit, soc_port_t port, 
                                           PHY84793_INTF_SIDE intf,
                                           uint32 *value)
{
    uint16 rddata = 0, mode = 0;
    phy_ctrl_t     *pc;
    int32 reg_addr = 0;
    uint32 lane = 0;
    pc = EXT_PHY_SW_STATE(unit, port);	
	
    PHY84793_GET_PORT_MODE(pc, mode);

    *value = 0;
    if (intf == PHY84793_SYS_SIDE) {
        for (lane =0; lane <= PHY84793_DC_LANE9; lane ++) {
            /* For Systemside(DC) */
            reg_addr = (lane << 16) | 0x80b6;
            SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE, 
                reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
            *value |= (rddata & 0x10)?(1<<lane):0;

        }
    } else {
        for (lane =0; lane < PHY84793_LN_MAX_NO_LANE; lane ++) {
            if (lane < PHY84793_VSR40_LANE3) {
                /* For VSR*/
                SOC_IF_ERROR_RETURN
                 (READ_PHY84793_PMA_PMD_REG (unit, pc, RG_TX_TX_GB_TEST_GEN_CTRL, 
                   &rddata));
                *value = (rddata >> 12) & 0xF;
            } else {
                if (mode == PHY84793_MODE_10X10G_TO_4X25G || 
                        mode == PHY84793_MODE_10X10G_TO_4X25G_OTL) {
                   return SOC_E_NONE;
                }
                reg_addr = ((lane-4) << 16) | 0x8069;
                PHY_84793_HCTX_PCB_READ (unit, pc, reg_addr, &rddata);
                *value |= (rddata & 0x10)?(1<<lane):0;
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84793_control_prbs_rx_status_get
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      lane  - lane number 
 *      value - status of the PRBS lock 
 * Returns:
 *      SOC_E_NONE
 */

STATIC int32 
_phy_84793_control_prbs_rx_status_get(int32 unit, soc_port_t port, 
		                                    uint32 *value)
{
    uint16 mode = 0, start_lane = 0, end_lane=0;
    uint32 prbs_sts = 0;
    int32 lane = 0;
    phy_ctrl_t     *pc;      	
    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);
    *value = 0;
    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:			
	    case PHY84793_MODE_10X10G_TO_4X25G_OTL:
       	    for (lane = 0; lane <= PHY84793_VSR40_LANE3; lane ++) {
                SOC_IF_ERROR_RETURN
                (_phy_84793_per_lane_control_prbs_rx_status_get(unit, port,
                                                          lane, &prbs_sts));
                *value |= prbs_sts;
            }
	    break;
        case PHY84793_MODE_4X10G_TO_4X10G:            
            if (pc->lane_num == 0) {
                start_lane = PHY84793_VSR40_LANE0;
                end_lane = PHY84793_VSR40_LANE3;
            } else {
                start_lane = 5;
                end_lane = 8;
            }
       	    for (lane = start_lane; lane < end_lane; lane ++) {
                SOC_IF_ERROR_RETURN
                (_phy_84793_per_lane_control_prbs_rx_status_get(unit, port,
                                                              lane, &prbs_sts));
                *value |= prbs_sts;
            }
        break; 
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:		
            SOC_IF_ERROR_RETURN
                (_phy_84793_per_lane_control_prbs_rx_status_get(unit, port,
                                                    pc->lane_num, &prbs_sts));
                *value = prbs_sts;
             break;
	    case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
            for (lane = 0; lane < PHY84793_LN_MAX_NO_LANE; lane ++) {
                SOC_IF_ERROR_RETURN
                (_phy_84793_per_lane_control_prbs_rx_status_get(unit, port,
                                                          lane, &prbs_sts));
                *value |= prbs_sts;
	        }
        break;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_84793_per_lane_control_prbs_rx_status_get
 * Purpose:
 *      Configure PHY device specific control fucntion.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      lane  - lane number 
 *      value - status of the PRBS lock 
 * Returns:
 *      SOC_E_NONE
 */

STATIC int32 
_phy_84793_per_lane_control_prbs_rx_status_get(int32 unit, soc_port_t port,
                                                  int32 lane, uint32 *value)
{
    uint16 rddata = 0, prev_val = 0;
    uint16 mode = 0;
    uint16 err_lsb = 0, err_msb=0;
    phy_ctrl_t     *pc;
    int32 reg_addr = 0;
    uint16 intf = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY84793_SYS_SIDE : PHY84793_LINE_SIDE;

    if ((mode == PHY84793_MODE_4X10G_TO_4X10G) && (lane == 4 || lane == 9)) {
        return SOC_E_PARAM;
    }
    
    *value = 0;
    if (intf == PHY84793_SYS_SIDE) {
        reg_addr = (lane << 16) | 0x80b1;
        SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
            reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
        prev_val = rddata;
        rddata  &= ~0x7;
        rddata |= 0x7;
        SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                                 PHY84793_DECA_CORE, reg_addr, rddata));
        reg_addr = (lane << 16) | 0x80b0;
        
        SOC_IF_ERROR_RETURN(
          _phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
              reg_addr, PHY84793_DC_HC_RD_WIT_CLEAR, &rddata));
        if (rddata & 0x8000) {
            if (rddata & 0x4000) {
                /* LOcked Now But lost before*/
                /* LOSS OF LOCK occured once, so one error at least */
                *value = (rddata & 0x3fff);
                if (*value == 0) {
                    *value = -2;   /* No Error BUT LOL*/
                }
            } else if (rddata & 0x3fff) {
                /* Check For Error, but no loss of lock */
                *value = (rddata & 0x3fff);
            } else {
                *value = 0;
            }
        } else { 
            *value = (rddata & 0x3fff);
            if (*value == 0) {
                *value = -1;   /* No Error And NO Lock*/
            }
        }
        reg_addr = (lane << 16) | 0x80b1;
        SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                               PHY84793_DECA_CORE, reg_addr, prev_val));
    } else {
        uint16 l_lock = 0;
        switch (mode) {
            case PHY84793_MODE_10X10G_TO_4X25G:			
            case PHY84793_MODE_10X10G_TO_4X25G_OTL:
                if (lane > PHY84793_VSR40_LANE3) {
                    return SOC_E_PARAM;
                }
                SOC_IF_ERROR_RETURN
                   (READ_PHY84793_PMA_PMD_REG(unit, pc, RG_RX_RX_GB_PRBS_LOCK, &rddata));
                SOC_IF_ERROR_RETURN
                   (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                      RG_RX_RX_GB_TEST_MISC_CTRL, 11, 8 , lane));
                SOC_IF_ERROR_RETURN
                   (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                                          RG_RX_RX_GB_LSB_ERR_CNT, &err_lsb));
                SOC_IF_ERROR_RETURN
                   (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                                           RG_RX_RX_GB_MSB_ERR_CNT, &err_msb));
                if (rddata & (1 << lane)) { 
                    SOC_IF_ERROR_RETURN
                       (READ_PHY84793_PMA_PMD_REG(unit, pc, RG_RX_RX_GB_PRBS_LOCK_LL, &l_lock));
                    if (l_lock & (1 << lane)) {
                        /* LOcked Now But lost before*/
                        /* LOSS OF LOCK occured once, so one error at least */
                        *value = ((err_lsb | (err_msb<<16)) & 0xffffffff);
                        if (*value == 0) {
                            *value = -2;   /* No Error but LOL*/
                        }
                    } else if (err_lsb | (err_msb << 16)) {
                        *value = ((err_lsb | (err_msb<<16)) & 0xffffffff);
                    } else {
                        *value = 0;
                    }
                } else {
                   *value = ((err_lsb | (err_msb<<16)) & 0xffffffff);
                    if (*value == 0) {
                        *value = -1;   /* No Error And NO Lock*/
                    }
                }
            break;
            case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
            case PHY84793_MODE_4X10G_TO_4X10G:      
    	    case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:		
            {
                if (lane <= PHY84793_VSR40_LANE3) {
                    SOC_IF_ERROR_RETURN
                       (READ_PHY84793_PMA_PMD_REG(unit, pc, RG_RX_RX_GB_PRBS_LOCK, 
                            &rddata));
                    SOC_IF_ERROR_RETURN
                       (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                       RG_RX_RX_GB_TEST_MISC_CTRL, 11, 8 , lane));

                    SOC_IF_ERROR_RETURN
                       (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                                           RG_RX_RX_GB_LSB_ERR_CNT,&err_lsb));
                    SOC_IF_ERROR_RETURN
                       (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                                           RG_RX_RX_GB_MSB_ERR_CNT,&err_msb));
                    if (rddata & (1 << lane)) {
                        SOC_IF_ERROR_RETURN
                           (READ_PHY84793_PMA_PMD_REG(unit, pc, RG_RX_RX_GB_PRBS_LOCK_LL, &l_lock));
                        if (l_lock & (1 << lane)) {
                            /* LOcked Now But lost before*/
                            /* LOSS OF LOCK occured once, so one error at least */
                            *value = ((err_lsb | (err_msb<<16)) & 0xffffffff);
                            if (*value == 0) {
                                *value = -2;   /* No Error BUT LOL*/
                            }
                        } else if (err_lsb | (err_msb << 16)) {
                            *value = ((err_lsb | (err_msb<<16)) & 0xffffffff);
                        } else {
                            *value = 0;
                        }
                    } else {
                        *value = ((err_lsb | (err_msb<<16)) & 0xffffffff);
                        if (*value == 0) {
                            *value = -1;   /* No Error And NO Lock*/
                        }
                    }
                } else {
                    reg_addr = ((lane-4) << 16) | 0x80b1;
                    SOC_IF_ERROR_RETURN
                        (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE,
                        reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
                    prev_val = rddata;
                    rddata  &= ~0x7;
                    rddata |= 0x7;
                    SOC_IF_ERROR_RETURN
                        (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                                             PHY84793_HEXA_CORE, reg_addr, rddata));
                    reg_addr = ((lane-4) << 16) | 0x80b0;
                    
                    SOC_IF_ERROR_RETURN(
                      _phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE,
                          reg_addr, PHY84793_DC_HC_RD_WIT_CLEAR, &rddata));
                    if (rddata & 0x8000) {
                        if (rddata & 0x4000) {
                            /* LOcked Now But lost before*/
                           /* LOSS OF LOCK occured once, so one error at least */
                            *value = (rddata & 0x3fff);
                            if (*value == 0) {
                                *value = -2;   /* No Error BUT LOL*/
                            }
                        } else if(rddata & 0x3fff) {
                            *value = (rddata & 0x3fff);
                        } else {
                            *value = 0;
                        }
                    } else { 
                        *value = (rddata & 0x3fff);
                        if (*value == 0) {
                            *value = -1;   /* No Error And NO Lock*/
                        }
                    }
                    reg_addr = ((lane-4) << 16) | 0x80b1;
                    SOC_IF_ERROR_RETURN
                        (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                                           PHY84793_HEXA_CORE, reg_addr, prev_val));
                }
                break;
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_dev_id_get(int32 unit, soc_port_t port, phy_ctrl_t *pc,
	              uint32 *chip_id)
{
    uint16 chip_id_msb = 0, chip_id_lsb = 0;
    
    SOC_IF_ERROR_RETURN
            (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_COMMON_REV_ID,
                                       &chip_id_msb));
    chip_id_msb = (chip_id_msb & 0xF000) >> 12;
    
    SOC_IF_ERROR_RETURN
            (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_COMMON_CHIP_ID,
                                       &chip_id_lsb));
    if (chip_id_msb == 0x8) {
        if (chip_id_lsb == 0x4793) {
            *chip_id = PHY84793_ID_84793;
        } else if(chip_id_lsb == 0x4790) {
            *chip_id = PHY84793_ID_84790;
        } else if(chip_id_lsb == 0x4794) {
            *chip_id = PHY84793_ID_84794;
        } 
    }

    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_dc_hc_pcb_read (int32 unit, soc_port_t port, phy_ctrl_t *pc, 
                 uint16 dc_hc, uint32 addr, uint16 clear_reg, uint16 *rddata) 
{
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_ADR_MSW_TYPE, 
                                    (uint16)((addr & PHY84793_16BIT_MSB)>>16)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_ADR_LSW_TYPE,
                                    (uint16)(addr & PHY84793_16BIT_LSB)));
    
    if (dc_hc == PHY84793_HEXA_CORE) {/* For Rx HC*/
        SOC_IF_ERROR_RETURN
          (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
                                      PHY84793_HC_RX_BIT));
        SOC_IF_ERROR_RETURN
          (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
                                      PHY84793_HC_READ_ENABLE));
    } else {  /* FOR DC */
        SOC_IF_ERROR_RETURN
          (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
                                      PHY84793_DC_ENABLE));
        SOC_IF_ERROR_RETURN
          (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
                                      PHY84793_DC_READ_ENABLE));
    }

    SOC_IF_ERROR_RETURN
     (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_DATA_OUT_TYPE, rddata));

    if (clear_reg) {
        if (dc_hc == PHY84793_HEXA_CORE) {
            SOC_IF_ERROR_RETURN
               (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE,
                                           PHY84793_HC_PCB_CLEAR));
            SOC_IF_ERROR_RETURN
               (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE,
                                           PHY84793_HC_READ_ENABLE));
        } else {
            SOC_IF_ERROR_RETURN
               (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE,
                                           PHY84793_DC_PCB_CLEAR));
            SOC_IF_ERROR_RETURN
               (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE,
                                           PHY84793_DC_READ_ENABLE));
        }
    }
    SOC_IF_ERROR_RETURN
          (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 0x0));

    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_dc_hc_pcb_write (int32 unit, soc_port_t port, phy_ctrl_t *pc, 
		                  uint16 dc_hc, uint32 addr, uint16 value)
{
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_ADR_MSW_TYPE, 
                                    (uint16)((addr & PHY84793_16BIT_MSB)>>16)));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_ADR_LSW_TYPE,
                                    (uint16)(addr & PHY84793_16BIT_LSB)));
      
    SOC_IF_ERROR_RETURN
      (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_DATA_IN_TYPE, value));
    if (dc_hc == PHY84793_HEXA_CORE) { /*for RX HC*/
        SOC_IF_ERROR_RETURN
           (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
				       PHY84793_HC_RX_BIT));
        SOC_IF_ERROR_RETURN
           (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
				       PHY84793_HC_WRITE_ENABLE));
    } else { /* For DC*/
        SOC_IF_ERROR_RETURN
           (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
				       PHY84793_DC_ENABLE));
        SOC_IF_ERROR_RETURN
           (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
				       PHY84793_DC_WRITE_ENABLE));
    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 0x0));

    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_set_refclk_code(int32 unit, soc_port_t port, phy_ctrl_t *pc,
                                 uint16 refclk_freq)
{
    uint8 refclk_code = 0;

    if (refclk_freq == PHY84793_REF_CLK_156_25) {
        refclk_code = 0x0;
    } else if(refclk_freq == PHY84793_REF_CLK_161_132) {
        refclk_code = 0x5;
    } else if(refclk_freq == PHY84793_REF_CLK_174_7) {
        refclk_code = 0x3;
    } else if(refclk_freq == PHY84793_REF_CLK_312_5) {
        refclk_code = 0x1;
    } else if(refclk_freq == PHY84793_REF_CLK_322_265) {
        refclk_code = 0x6;
    } else if(refclk_freq ==  PHY84793_REF_CLK_349_4) {
        refclk_code = 0x2;
    } else if(refclk_freq == PHY84793_REF_CLK_625_0) {
        refclk_code = 0x8;
    } else if(refclk_freq == PHY84793_REF_CLK_644_531) {
        refclk_code = 0x7;
    } else if(refclk_freq == PHY84793_REF_CLK_698_8) {
        refclk_code = 0x4;
    }
    
    /* Setting refclk_sel_reg.refclk_code */
    SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_REFCLK_SEL_REG,
          CTL_REGS_REFCLK_SEL_REG_REFCLK_CODE_SHIFT + 3,
          CTL_REGS_REFCLK_SEL_REG_REFCLK_CODE_SHIFT, refclk_code));
    
    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_config_mode (int32 unit, soc_port_t port, phy_ctrl_t *pc,
                              uint16 mode)
{
    uint16 reg_clk = 0;
    
    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:
            reg_clk = PHY84793_REF_CLK_156_25;
            mode = 0;
        break;
        case PHY84793_MODE_4X10G_TO_4X10G:
            reg_clk = PHY84793_REF_CLK_161_132;
            mode = 1;
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
            reg_clk = PHY84793_REF_CLK_161_132;
            mode = 2;
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:
            reg_clk = PHY84793_REF_CLK_161_132;
            mode = 3;
        break;
        case PHY84793_MODE_10X10G_TO_4X25G_OTL: 
            reg_clk = PHY84793_REF_CLK_174_7;
            mode = 4;
        break;

        default:
            return SOC_E_FAIL;
    }
    if (mode == PHY84793_MODE_10X10G_TO_4X25G) {
        SOC_IF_ERROR_RETURN
          (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_USER_MODESEL_REG,
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT, 
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT, 0));

        SOC_IF_ERROR_RETURN
          (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_USER_MODESEL_REG,
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_SHIFT + 2,
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_SHIFT, mode));
        SOC_IF_ERROR_RETURN
          (_phy_84793_set_refclk_code(unit, port, pc, reg_clk));
   
        SOC_IF_ERROR_RETURN(_phy_84793_div_change(unit, port, 0, 0, mode));

         SOC_IF_ERROR_RETURN
          (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_USER_MODESEL_REG,
            CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT,
            CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT, 0));
         SOC_IF_ERROR_RETURN
          (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_USER_MODESEL_REG,
            CTL_REGS_USER_MODESEL_REG_USER_MODESEL_SHIFT + 2,
            CTL_REGS_USER_MODESEL_REG_USER_MODESEL_SHIFT, mode));
    } else {
        SOC_IF_ERROR_RETURN
          (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_USER_MODESEL_REG,
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT, 
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT, 1));
        SOC_IF_ERROR_RETURN
          (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_USER_MODESEL_REG,
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_SHIFT + 2,
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_SHIFT, mode));

        SOC_IF_ERROR_RETURN(_phy_84793_div_change(unit, port, 1, 0, mode));

	    SOC_IF_ERROR_RETURN
          (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_USER_MODESEL_REG,
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT, 
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT, 1));

        SOC_IF_ERROR_RETURN
          (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_USER_MODESEL_REG,
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_SHIFT + 2,
          CTL_REGS_USER_MODESEL_REG_USER_MODESEL_SHIFT, mode));
    }
	
    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_system_polarity_flip (int32 unit, soc_port_t port, 
		phy_ctrl_t *pc, uint16 cfg_tx_pol, uint16 cfg_rx_pol)
{
    uint16 lane = 0 , rddata = 0;
    uint8 flip = 0;
    uint32 reg_addr = 0;

    if (PHY84793_POL_DND != cfg_rx_pol) {
        /*The function enables system side input (Rx) polarity invert*/
        for (lane = PHY84793_DC_LANE0; lane <= PHY84793_DC_LANE9; lane ++) {
            reg_addr = (lane << 16) | 0x80BA; 
            SOC_IF_ERROR_RETURN
            (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
                       reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));

            if ((cfg_rx_pol & POL_LANE_MASK(lane)) == POL_LANE_MASK(lane)) { 
               rddata |= (1 << 2);
            } else {
                rddata &= ~(1 << 2);
            }
            rddata |= (1 << 3);      /* Force Rx polarity bit 3*/ 

	    SOC_IF_ERROR_RETURN
	    (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE, 
                                         reg_addr, rddata)); 
        }
    }
    if (PHY84793_POL_DND != cfg_tx_pol) {
        /*The function enables system side output (Tx) polarity invert*/
        for (lane = PHY84793_DC_LANE0; lane <= PHY84793_DC_LANE9; lane ++) {
            if ((cfg_tx_pol & POL_LANE_MASK(lane)) == POL_LANE_MASK(lane)) {
                flip = 1;
            } else { 
                flip = 0;
            }           
            SOC_IF_ERROR_RETURN
               (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                 RG_RX_RX_GB_IO_BIT_CTRL,
                 RG_RX_RX_GB_IO_BIT_CTRL_OUTPUT_INVERT_SHIFT + lane,
                 RG_RX_RX_GB_IO_BIT_CTRL_OUTPUT_INVERT_SHIFT + lane, flip)); 
        }
    }
    return SOC_E_NONE; 
}

STATIC
int32 _phy_84793_line_polarity_flip (int32 unit, soc_port_t port, 
                                     phy_ctrl_t *pc, 
                                     uint16 cfg_tx_pol, uint16 cfg_rx_pol)
{
    uint16 lane = 0, rddata = 0;
    uint8 flip = 0;
    uint32 reg_addr = 0;

    if (PHY84793_POL_DND != cfg_rx_pol) {
        /*The function enables system side input (Rx) polarity invert*/
        for (lane = PHY84793_HC_LANE0; lane < PHY84793_LN_MAX_NO_LANE; lane ++) {
            if (lane <= PHY84793_VSR40_LANE3)  {
                if ((cfg_rx_pol & POL_LANE_MASK(lane)) == POL_LANE_MASK(lane)) {
                    flip = 1;
                } else {
                    flip = 0;
                }
                SOC_IF_ERROR_RETURN
                  (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                  VSR40_RX_DIG_REGS_DP_CTRL, 
                  VSR40_RX_DIG_REGS_DP_CTRL_INVERT_DP_SHIFT + lane, 
                  VSR40_RX_DIG_REGS_DP_CTRL_INVERT_DP_SHIFT + lane, flip)); 
            } else {

                reg_addr = ((lane-4) << 16) | 0x80BA; 
                SOC_IF_ERROR_RETURN
                (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_HEXA_CORE,
                 reg_addr, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
                if ((cfg_rx_pol & POL_LANE_MASK(lane)) == 
                    POL_LANE_MASK(lane)) { 
                   rddata |= (1 << 2);
                } else {
                    rddata &= ~(1 << 2);
                }
                rddata |= (1 << 3);      /* Force Rx polarity bit 3*/ 
	            SOC_IF_ERROR_RETURN
	             (_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                   PHY84793_HEXA_CORE, reg_addr, rddata)); 
            }
        }
    }
    if (PHY84793_POL_DND != cfg_tx_pol) {
        /*The function enables line side output (Tx) polarity invert*/
        for (lane = PHY84793_HC_LANE0; lane < PHY84793_LN_MAX_NO_LANE; lane ++) {
            if ((cfg_tx_pol & POL_LANE_MASK(lane)) == POL_LANE_MASK(lane)) {
                flip = 1;
            } else  {
                flip = 0;
           }
           SOC_IF_ERROR_RETURN
             (_phy_84793_modify_pma_pmd_reg(unit, port, pc,
             RG_TX_TX_GB_IO_BIT_CONTROL, 
             RG_TX_TX_GB_IO_BIT_CONTROL_OUTPUT_INVERT_SHIFT + lane, 
             RG_TX_TX_GB_IO_BIT_CONTROL_OUTPUT_INVERT_SHIFT + lane, flip));
        }
    }
    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_sys_enable_set (int32 unit, soc_port_t port, phy_ctrl_t *pc,
                                 uint16 cfg_tx_set, uint16 cfg_rx_set)
{
    uint16 enable = 0, lane = 0, mode = 0;

    PHY84793_GET_PORT_MODE(pc, mode);
    if (cfg_rx_set != PHY84793_ENABLE_SET_DND) {
        for (lane = PHY84793_DC_LANE0; lane <= PHY84793_DC_LANE9; lane ++) {
            if ((cfg_rx_set & POL_LANE_MASK(lane)) == POL_LANE_MASK(lane)) {
                enable = 0;
            } else {
                enable = 1;
            }
            if (mode == PHY84793_MODE_4X10G_TO_4X10G) {
                if (lane == 4 || lane ==9) {
                    return SOC_E_NONE;
                }
            }
            SOC_IF_ERROR_RETURN
               (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_FRC_DCRX_PD,
                CTL_REGS_FRC_DCRX_PD_FRC_DCRX_PWRDN_SHIFT + lane, 
                CTL_REGS_FRC_DCRX_PD_FRC_DCRX_PWRDN_SHIFT + lane, 1)); 
            SOC_IF_ERROR_RETURN
               (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_DCRX_PD,
                 CTL_REGS_DCRX_PD_DCRX_PWRDN_SHIFT + lane, 
                 CTL_REGS_DCRX_PD_DCRX_PWRDN_SHIFT + lane, enable));
        }
    }
    if (cfg_tx_set != PHY84793_ENABLE_SET_DND) {
        for (lane = PHY84793_DC_LANE0; lane <= PHY84793_DC_LANE9; lane ++) {
            if ((cfg_tx_set & POL_LANE_MASK(lane)) == POL_LANE_MASK(lane)) {
                enable = 0;
            } else {
                enable = 1;
            }
            if (mode == PHY84793_MODE_4X10G_TO_4X10G) {
                if (lane == 4 || lane ==9) {
                    return SOC_E_NONE;
                }
            }

            SOC_IF_ERROR_RETURN
               (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_FRC_DCTX_PD,
                CTL_REGS_FRC_DCTX_PD_FRC_DCTX_PWRDN_SHIFT + lane, 
                CTL_REGS_FRC_DCTX_PD_FRC_DCTX_PWRDN_SHIFT + lane, 1)); 
            SOC_IF_ERROR_RETURN
               (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_DCTX_PD, 
                CTL_REGS_DCTX_PD_DCTX_PWRDN_SHIFT + lane,
                CTL_REGS_DCTX_PD_DCTX_PWRDN_SHIFT + lane, enable));
        }
    }
    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_line_enable_set (int32 unit, soc_port_t port, phy_ctrl_t *pc, 
                                  uint16 cfg_tx_set, uint16 cfg_rx_set)
{
    uint16 enable = 0, lane = 0, mode = 0;
    
    PHY84793_GET_PORT_MODE(pc, mode);
    if (cfg_rx_set != PHY84793_ENABLE_SET_DND) {
        for (lane = PHY84793_HC_LANE0; lane < PHY84793_LN_MAX_NO_LANE;
		       	lane ++) {
            if ((cfg_rx_set & POL_LANE_MASK(lane)) == POL_LANE_MASK(lane)) {
                enable = 0;
            } else {
                enable = 1;
            }
            if (lane <= PHY84793_VSR40_LANE3) {
                SOC_IF_ERROR_RETURN
                   (_phy_84793_modify_pma_pmd_reg(unit, port, pc,
                   CTL_REGS_FRC_VSR40_RX_PD, 
                   CTL_REGS_FRC_VSR40_RX_PD_FRC_VSR40_RX_PWRDN_SHIFT + lane, 
                   CTL_REGS_FRC_VSR40_RX_PD_FRC_VSR40_RX_PWRDN_SHIFT + lane, 1)); 
                SOC_IF_ERROR_RETURN
                   (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                    CTL_REGS_VSR40_RX_PD, 
                    CTL_REGS_VSR40_RX_PD_VSR40_RX_PWRDN_SHIFT + lane, 
                    CTL_REGS_VSR40_RX_PD_VSR40_RX_PWRDN_SHIFT + lane, enable)); 
            } else {
                if (mode == PHY84793_MODE_4X10G_TO_4X10G) {
                    if (lane == 4 || lane ==9) {
                        return SOC_E_NONE;
                    }
                }
                SOC_IF_ERROR_RETURN
                   (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                     CTL_REGS_FRC_HCRX_PD, 
                     (CTL_REGS_FRC_HCRX_PD_FRC_HCRX_PWRDN_SHIFT + (lane-4)),
                     (CTL_REGS_FRC_HCRX_PD_FRC_HCRX_PWRDN_SHIFT + (lane-4)), 1)); 
                SOC_IF_ERROR_RETURN
                   (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_HCRX_PD, 
                    (CTL_REGS_HCRX_PD_HCRX_PWRDN_SHIFT + (lane-4)), 
                    (CTL_REGS_HCRX_PD_HCRX_PWRDN_SHIFT + (lane -4)), enable)); 
            }
        }
    }
    if (cfg_tx_set != PHY84793_ENABLE_SET_DND) {
        for (lane = PHY84793_HC_LANE0; lane < PHY84793_LN_MAX_NO_LANE; 
			                                          lane ++) {
            if ((cfg_tx_set & POL_LANE_MASK(lane)) == POL_LANE_MASK(lane)) {
                enable = 0;
            } else {
                enable = 1;
            }
            if (lane <= PHY84793_VSR40_LANE3) {
                SOC_IF_ERROR_RETURN
                   (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                           CTL_REGS_FRC_VSR40_TX_PD, lane, lane, 1)); 
                SOC_IF_ERROR_RETURN
                   (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                   CTL_REGS_VSR40_TX_PD, 
                   (CTL_REGS_VSR40_TX_PD_VSR40_TX_PWRDN_SHIFT + lane),
                   (CTL_REGS_VSR40_TX_PD_VSR40_TX_PWRDN_SHIFT + lane), enable));
            } else {
                if (mode == PHY84793_MODE_4X10G_TO_4X10G) {
                    if (lane == 4 || lane ==9) {
                        return SOC_E_NONE;
                    }
                }
                SOC_IF_ERROR_RETURN
                   (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                    CTL_REGS_FRC_HCTX_PD, 
                    (CTL_REGS_FRC_HCTX_PD_FRC_HCTX_PWRDN_SHIFT + (lane-4)), 
		            (CTL_REGS_FRC_HCTX_PD_FRC_HCTX_PWRDN_SHIFT + (lane-4)), 1));
                SOC_IF_ERROR_RETURN
                   (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                   CTL_REGS_HCTX_PD, (CTL_REGS_HCTX_PD_HCTX_PWRDN_SHIFT + (lane-4)), 
		           (CTL_REGS_HCTX_PD_HCTX_PWRDN_SHIFT + (lane-4)), enable));
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Squelch enable/disable
 */
STATIC int32
_phy_84793_sys_lane_squelch_enable (int32 unit, soc_port_t port, uint8 lane, 
                                    uint8 enable)
{
    phy_ctrl_t     *pc;
    uint16 mode = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);
    
    if(mode == PHY84793_MODE_4X10G_TO_4X10G) {
        if (lane == 4 || lane == 9) {
            return SOC_E_NONE;
        }
    }
    
    /* SYStem side Tx squelch */
    SOC_IF_ERROR_RETURN
           (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_FRC_DCTX_PD,
                              CTL_REGS_FRC_DCTX_PD_FRC_DCTX_PWRDN_SHIFT + lane, 
				    CTL_REGS_FRC_DCTX_PD_FRC_DCTX_PWRDN_SHIFT + lane, 1)); 
    SOC_IF_ERROR_RETURN
           (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_DCTX_PD, 
                 CTL_REGS_DCTX_PD_DCTX_PWRDN_SHIFT + lane,
                 CTL_REGS_DCTX_PD_DCTX_PWRDN_SHIFT + lane, enable));
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_line_lane_squelch_enable (int32 unit, soc_port_t port, uint8 lane, 
                                uint8 enable)
{
    phy_ctrl_t     *pc;
    uint16 mode = 0;

    CHECK_LANE(lane);
    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);
    
    if(mode == PHY84793_MODE_4X10G_TO_4X10G) {
        if (lane == 4 || lane == 9) {
            return SOC_E_NONE;
        }
    }
    
    if (lane <= PHY84793_VSR40_LANE3) {
        SOC_IF_ERROR_RETURN
           (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_FRC_VSR40_TX_PD,
                 CTL_REGS_FRC_VSR40_TX_PD_FRC_VSR40_TX_PWRDN_SHIFT + lane, 
                 CTL_REGS_FRC_VSR40_TX_PD_FRC_VSR40_TX_PWRDN_SHIFT + lane, 1)); 
        SOC_IF_ERROR_RETURN
           (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_VSR40_TX_PD, 
                 CTL_REGS_VSR40_TX_PD_VSR40_TX_PWRDN_SHIFT + lane,
                 CTL_REGS_VSR40_TX_PD_VSR40_TX_PWRDN_SHIFT + lane, enable));
    } else {
        if (mode != PHY84793_MODE_10X10G_TO_4X25G) { 
            lane -=4;
            SOC_IF_ERROR_RETURN
               (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_FRC_HCTX_PD,
                 CTL_REGS_FRC_HCTX_PD_FRC_HCTX_PWRDN_SHIFT + lane, 
                 CTL_REGS_FRC_HCTX_PD_FRC_HCTX_PWRDN_SHIFT + lane, 1));
            SOC_IF_ERROR_RETURN
               (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_HCTX_PD, 
                 CTL_REGS_HCTX_PD_HCTX_PWRDN_SHIFT + lane,
                 CTL_REGS_HCTX_PD_HCTX_PWRDN_SHIFT + lane, enable));
        }
    }

    return SOC_E_NONE;
}

STATIC int32
_phy_84793_squelch_enable(int32 unit, soc_port_t port, 
                          PHY84793_INTF_SIDE intf_side, 
                          uint8 enable)
{
    uint16 lane = 0;
    if (intf_side == PHY84793_SYS_SIDE) {
        for (lane = PHY84793_DC_LANE0; lane<= PHY84793_DC_LANE9; lane ++) {
            SOC_IF_ERROR_RETURN
              (_phy_84793_sys_lane_squelch_enable (unit, port, lane, enable));
        }
    } else if (intf_side == PHY84793_LINE_SIDE) {
       for (lane = 0; lane<= 9; lane ++) {
           SOC_IF_ERROR_RETURN
             (_phy_84793_line_lane_squelch_enable (unit, port, lane, enable));
       }
    } else {
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

STATIC int32
_phy_84793_control_tx_preemphsis_get (int32 unit, soc_port_t port, 
                                      PHY84793_INTF_SIDE intf_side, 
                                      uint32 *value)
{
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_84793_per_lane_control_tx_preemphasis_get (unit, port, intf_side,
                                                         pc->lane_num, value));
    return SOC_E_NONE;
}



STATIC int32
_phy_84793_per_lane_control_tx_preemphasis_get (int32 unit, soc_port_t port, 
		                             PHY84793_INTF_SIDE intf_side, 
                                   uint16 lane, uint32 *value)
{
    uint32 tx_lane_base_add = 0;
    uint16 temp = 0;
    uint16 rddata = 0, post_cursor1 = 0;
    phy_ctrl_t     *pc;

    CHECK_LANE(lane);
    pc = EXT_PHY_SW_STATE(unit, port);
    
    if (intf_side == PHY84793_LINE_SIDE) {
        if (lane <= PHY84793_VSR40_LANE3) {   
             /* Value format: bit 14:11: pre_tap bit 10:06: post_tap  
             bit 05:00: main_tap  */
             switch (lane) {
                case 0:
                    tx_lane_base_add = VSR40_TX_ANA_LANE0_BASE;
                break;
                case 1:
                    tx_lane_base_add = VSR40_TX_ANA_LANE1_BASE;
                break;
                case 2:
                    tx_lane_base_add = VSR40_TX_ANA_LANE2_BASE;
                break;
                case 3:
                    tx_lane_base_add = VSR40_TX_ANA_LANE3_BASE;
                break;
             }
             SOC_IF_ERROR_RETURN
             (READ_PHY84793_PMA_PMD_REG(unit, pc,(uint16) (tx_lane_base_add + 
			                     VSR40_TX_ANA25G_CONTROL5) , &rddata));
             *value = (rddata & 0x3FFF);
        } else {
            /* Value format: bit 4:0 - post cursor1 bit7:5 - post cursor2  
             bit 13:8-main_tap  */
            lane = lane -4;
            tx_lane_base_add = (lane<<16) | 0x8066;
            PHY_84793_HCTX_PCB_READ(unit, pc, tx_lane_base_add, &rddata);

            tx_lane_base_add = (lane<<16) | 0x8067;
            PHY_84793_HCTX_PCB_READ(unit, pc, tx_lane_base_add, &temp);
            post_cursor1 = ((temp & 0x7)<<2)|((rddata & 0xc000) >> 14);
            *value = ((temp & 0x1f8) << 5) | ((rddata & 0x3800)>>6) | post_cursor1; 
        }
    } else {
         /* Value format: bit 4:0 - post cursor1 bit7:5 - post cursor2  
             bit 13:8-main_tap  */
        tx_lane_base_add = (lane<<16) | 0x8066;
        SOC_IF_ERROR_RETURN
         (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
            tx_lane_base_add, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
        
        tx_lane_base_add = (lane << 16) | 0x8067;
	    SOC_IF_ERROR_RETURN
         (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
            tx_lane_base_add, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &temp));

        post_cursor1 = ((temp & 0x7)<<2)|((rddata & 0xc000) >> 14);

        *value = ((temp & 0x1f8) << 5) | ((rddata & 0x3800)>>6) | post_cursor1; 
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_per_lane_control_tx_preemphasis_set (int32 unit, soc_port_t port, 
		                             PHY84793_INTF_SIDE intf_side, 
                                   uint16 lane, uint32 value)
{
    uint32 tx_lane_base_add = 0;
    uint16 post_cursor1 = 0, post_cursor2 = 0, main_tap = 0;
    uint16 rddata = 0;
    phy_ctrl_t     *pc;

    CHECK_LANE(lane);
    pc = EXT_PHY_SW_STATE(unit, port);
    
    if (intf_side == PHY84793_LINE_SIDE) {
        if (lane <= PHY84793_VSR40_LANE3) {   
             /* Value format: bit 14:11: pre_tap bit 10:06: post_tap  
            bit 05:00: main_tap  */
            switch (lane) {
                case 0:
                    tx_lane_base_add = VSR40_TX_ANA_LANE0_BASE;
                break;
                case 1:
                    tx_lane_base_add = VSR40_TX_ANA_LANE1_BASE;
                break;
                case 2:
                    tx_lane_base_add = VSR40_TX_ANA_LANE2_BASE;
                break;
                case 3:
                    tx_lane_base_add = VSR40_TX_ANA_LANE3_BASE;
                break;
            }
            SOC_IF_ERROR_RETURN
                 (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                   (uint16)tx_lane_base_add + VSR40_TX_ANA25G_CONTROL5, 
                   VSR40_TX_ANA25G_CONTROL5_PRE_TAP_SHIFT+3,
                   VSR40_TX_ANA25G_CONTROL5_MAIN_TAP_SHIFT, (uint16)value));
        } else {
            /* Value format: bit 4:0 - post cursor1 bit7:5 - post cursor2  
             bit 13:8-main_tap  */
            post_cursor1 = (value & 0x1F); 
            post_cursor2 = (value & 0xE0) >> 5; 
            main_tap = (value & 0x3F00) >> 8;
            lane = lane-4;
            tx_lane_base_add = (lane<<16) | 0x8066;
            
            PHY_84793_HCTX_PCB_READ(unit,pc,tx_lane_base_add, &rddata);
           
            rddata = (rddata & 0x07ff) | ((post_cursor2 & 0x7) <<11)|
                                         ((post_cursor1 & 0x3) << 14);
            
            PHY_84793_HCTX_PCB_WRITE(unit, pc, tx_lane_base_add, rddata); 
            tx_lane_base_add = (lane<<16) | 0x8067;
            
            PHY_84793_HCTX_PCB_READ(unit,pc,tx_lane_base_add, &rddata); 
	        rddata = (rddata & 0xFE00) | ((main_tap & 0x3F) << 3) | 
                     ((post_cursor1>>2) & 0x7);
            
            PHY_84793_HCTX_PCB_WRITE(unit, pc, tx_lane_base_add, rddata); 
        }
    } else {
         /* Value format: bit 4:0 - post cursor1 bit7:5 - post cursor2  
             bit 13:8-main_tap  */
        post_cursor1 = (value & 0x1F); 
        post_cursor2 = (value & 0xE0) >> 5; 
        main_tap = (value & 0x3F00) >> 8;
        tx_lane_base_add = (lane<<16) | 0x8066;
        SOC_IF_ERROR_RETURN
          (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
           tx_lane_base_add, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
        rddata = (rddata & 0x07ff) | ((post_cursor2 & 0x7) <<11)| 
	          ((post_cursor1 & 0x3) << 14);
        
        SOC_IF_ERROR_RETURN
	   (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE, 
                                    tx_lane_base_add, rddata));
        
        tx_lane_base_add = (lane<<16) | 0x8067;
        SOC_IF_ERROR_RETURN
         (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
           tx_lane_base_add, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
        rddata = (rddata & 0xFE00) | ((main_tap & 0x3F) << 3) | 
                 ((post_cursor1>>2) & 0x7);
       
        SOC_IF_ERROR_RETURN
	      (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE, 
                                         tx_lane_base_add, rddata));
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_control_tx_preemphsis_set (int32 unit, soc_port_t port, 
                                      soc_phy_control_t type, 
                                      PHY84793_INTF_SIDE intf_side, 
                                      uint32 value)
{
    uint16 lane = 0;

    for (lane = 0; lane < PHY84793_LN_MAX_NO_LANE;lane ++) {
        SOC_IF_ERROR_RETURN
        (_phy_84793_per_lane_control_tx_preemphasis_set (unit, port, intf_side,
                                                         lane, value));
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_control_tx_driver_set (int32 unit, soc_port_t port, 
                                  soc_phy_control_t type, 
                                  PHY84793_INTF_SIDE intf_side, uint32 value)
{
    uint16 lane = 0;

    for (lane = 0; lane < PHY84793_LN_MAX_NO_LANE;lane ++) {
        SOC_IF_ERROR_RETURN
          (_phy_84793_per_lane_control_tx_driver_set (unit, port, type,
                                                      intf_side, lane, value));
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_control_tx_driver_get (int32 unit, soc_port_t port, 
                                  soc_phy_control_t type, 
                                  PHY84793_INTF_SIDE intf_side, 
                                  uint32 *value)
{
    phy_ctrl_t     *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_84793_per_lane_control_tx_driver_get (unit, port, type, intf_side,
                                                    pc->lane_num, value));
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_per_lane_control_tx_driver_get (int32 unit, soc_port_t port,  
                                           soc_phy_control_t type, 
                                           PHY84793_INTF_SIDE intf_side, 
                                           uint16 lane, uint32 *value)
{
    uint16 start_bit = 0, rddata = 0;
    uint32 tx_lane_base_add = 0;
    phy_ctrl_t     *pc;

    CHECK_LANE(lane);
    pc = EXT_PHY_SW_STATE(unit, port); 

    switch (type) {
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            start_bit = 0;
            type = SOC_PHY_CONTROL_DRIVER_CURRENT;
        break;
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
            type = SOC_PHY_CONTROL_PRE_DRIVER_CURRENT;	    
            start_bit = VSR40_TX_ANA25G_CONTROL1_IPREDRIVER_SHIFT;
        break;
        default:
            return SOC_E_PARAM;
    }
    switch (lane) {
        case 0:
            tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                VSR40_TX_ANA_LANE0_BASE:((lane<<16)|0x8065);
        break;
        case 1:
            tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                VSR40_TX_ANA_LANE1_BASE:((lane<<16)|0x8065);
        break;
        case 2:
            tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                VSR40_TX_ANA_LANE2_BASE:((lane<<16)|0x8065);
        break;
        case 3:
            tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                VSR40_TX_ANA_LANE3_BASE:((lane<<16)|0x8065);
        break;
        case 4:
        case 5:
        case 6: /* FALL THROUGH*/
        case 7:
        case 8:
        case 9:
             tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                (((lane-4)<<16)|0x8065):((lane<<16)|0x8065);
        break;	    
    }
    if (intf_side == PHY84793_LINE_SIDE) {
        if (lane <= PHY84793_VSR40_LANE3) {
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc,  (uint16)(tx_lane_base_add 
                + VSR40_TX_ANA25G_CONTROL1), &rddata));
            *value = GET_BIT_VAL(rddata,  start_bit + 3, start_bit);
        } else {
            uint16 rddata = 0;
            PHY_84793_HCTX_PCB_READ(unit, pc, tx_lane_base_add, &rddata);
            
            /* coverity[mixed_enums : FALSE] */
            if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
                *value = (rddata & 0xf000) >> 12; /*Bit 15:12*/
            } else {
                *value = (rddata & 0x0f00) >> 8; /*Bit 11:8*/
            }
        }
    } else {
        uint16 rddata = 0;
        SOC_IF_ERROR_RETURN
           (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
            tx_lane_base_add, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
        
        /* coverity[mixed_enums : FALSE] */
        if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
            *value = (rddata & 0xf000) >> 12; /*Bit 15:12*/
        } else {
            *value = (rddata & 0x0f00) >> 8; /*Bit 11:8*/
        }
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_per_lane_control_tx_driver_set (int32 unit, soc_port_t port,  
                                           soc_phy_control_t type, 
                                           PHY84793_INTF_SIDE intf_side, 
                                           uint16 lane, uint32 value)
{
    uint16 tmp = 0, start_bit = 0;
    uint32 tx_lane_base_add = 0;
    phy_ctrl_t     *pc;

    CHECK_LANE(lane);
    pc = EXT_PHY_SW_STATE(unit, port); 

    switch (type) {
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            tmp = (uint16) (value & VSR40_TX_ANA25G_CONTROL1_IDRIVER_MASK) ;
            start_bit = VSR40_TX_ANA25G_CONTROL1_IDRIVER_SHIFT;
            type = SOC_PHY_CONTROL_DRIVER_CURRENT;
        break;
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
            tmp = (uint16) value & 0xf;
	    type = SOC_PHY_CONTROL_PRE_DRIVER_CURRENT;	    
            start_bit = VSR40_TX_ANA25G_CONTROL1_IPREDRIVER_SHIFT;
        break;
        default:
            return SOC_E_PARAM;
    }
    switch (lane) {
        case 0:
            tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                VSR40_TX_ANA_LANE0_BASE:((lane<<16)|0x8065);
        break;
        case 1:
            tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                VSR40_TX_ANA_LANE1_BASE:((lane<<16)|0x8065);
        break;
        case 2:
            tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                VSR40_TX_ANA_LANE2_BASE:((lane<<16)|0x8065);
        break;
        case 3:
            tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                VSR40_TX_ANA_LANE3_BASE:((lane<<16)|0x8065);
        break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            tx_lane_base_add = (intf_side == PHY84793_LINE_SIDE)?
                                (((lane-4)<<16)|0x8065):((lane<<16)|0x8065);
        break;	    
    }
    if (intf_side == PHY84793_LINE_SIDE) {
        if (lane <= PHY84793_VSR40_LANE3) {
            SOC_IF_ERROR_RETURN
            (_phy_84793_modify_pma_pmd_reg(unit, port, pc, (uint16)tx_lane_base_add 
             + VSR40_TX_ANA25G_CONTROL1, start_bit + 3, start_bit, tmp));
        } else {
           uint16 rddata = 0;
           PHY_84793_HCTX_PCB_READ(unit, pc, tx_lane_base_add, &rddata);

           /* coverity[mixed_enums:FALSE] */
           if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
               rddata &= 0x0fff;
               rddata |= tmp << 12;  /*Bit 15:12*/
           } else {
               rddata &= 0xf0ff;
               rddata |= tmp << 8;  /*Bit 11:8*/
           }
           PHY_84793_HCTX_PCB_WRITE(unit, pc, tx_lane_base_add, rddata);
        }
    } else {
        uint16 rddata = 0;
        SOC_IF_ERROR_RETURN
           (_phy_84793_dc_hc_pcb_read (unit, port, pc, PHY84793_DECA_CORE,
            tx_lane_base_add, PHY84793_DC_HC_RD_WIT_OUT_CLEAR, &rddata));
        
        /* coverity[mixed_enums:FALSE] */
        if (type == SOC_PHY_CONTROL_DRIVER_CURRENT) {
            rddata &= 0x0fff;
            rddata |= tmp << 12;  /*Bit 15:12*/
        } else {
            rddata &= 0xf0ff;
            rddata |= tmp << 8;  /*Bit 11:8*/
        }
        SOC_IF_ERROR_RETURN
	     (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_DECA_CORE,
                                         tx_lane_base_add, rddata));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84793_rx_seq_done_get(int unit, soc_port_t port, int32 intf, uint32 *value)
{
    uint16 lane = 0, rxseqdone = 0, rddata = 0;
    uint16 mode = 0;
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);

    if (intf == PHY84793_LINE_SIDE) {
        SOC_IF_ERROR_RETURN(phy_84793_link_get(unit, port,  (int32*)value));
    } else {
        for (lane = 0; lane <=9; lane ++) {
            SOC_IF_ERROR_RETURN(_phy_84793_dc_hc_pcb_read (unit, port,pc,
              PHY84793_DECA_CORE , (lane<<16)|0x80b1, 0, &rddata));
            rddata = rddata & (~(0x0007));
       
            SOC_IF_ERROR_RETURN(_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                        PHY84793_DECA_CORE, (lane<<16), rddata));
            sal_usleep(1000);
            SOC_IF_ERROR_RETURN(_phy_84793_dc_hc_pcb_read (unit, port,pc,
               PHY84793_DECA_CORE , (lane << 16)|0x80b0, 0, &rddata));
            if ((rddata & 0x1000) == 0x1000) {
                rxseqdone ++;
            }
        }
        if (mode == PHY84793_MODE_4X10G_TO_4X10G) { 
            *value = (rxseqdone == (lane-2));
        } else {
            *value = (rxseqdone == lane);
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84793_per_lane_rx_seq_done_get(int unit, soc_port_t port, 
                                    int32 intf, 
                                    int lane, uint32 *value)
{
    uint16 rddata = 0;
    uint16 mode = 0;
    phy_ctrl_t    *pc;       /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);

    if (mode == PHY84793_MODE_4X10G_TO_4X10G) {
        if (pc->lane_num) {
            /* If it is 2nd port */
            /* lane should be between 5 to 8 */
            lane = 5 + lane;
        }
    }
    if ((mode == PHY84793_MODE_10X10G_TO_4X25G) && (intf == PHY84793_LINE_SIDE)
            && (lane > PHY84793_VSR40_LANE3)) {
        return SOC_E_PARAM;
    }
    if (intf == PHY84793_LINE_SIDE) {
        if (lane <= PHY84793_VSR40_LANE3) {
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, 
               VSR40_RX_DIG_REGS_CDR_CONTROL, &rddata));
            *value = (rddata &  
                    (VSR40_RX_DIG_REGS_CDR_CONTROL_CDRLOCK_STATUS_LN0_MASK << lane)) ? 1 : 0;
        } else {
            SOC_IF_ERROR_RETURN(_phy_84793_dc_hc_pcb_read (unit, port,pc,
              PHY84793_HEXA_CORE , (lane<<16)|0x80b1, 0, &rddata));
            rddata = rddata & (~(0x0007));
           
            SOC_IF_ERROR_RETURN(_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                        PHY84793_HEXA_CORE, (lane<<16), rddata));
            sal_usleep(1000);
            SOC_IF_ERROR_RETURN(_phy_84793_dc_hc_pcb_read (unit, port,pc,
               PHY84793_HEXA_CORE , (lane << 16)|0x80b0, 0, &rddata));
            *value = (rddata & 0x1000) ? 1 : 0;
        }
    } else {
        SOC_IF_ERROR_RETURN(_phy_84793_dc_hc_pcb_read (unit, port,pc,
          PHY84793_DECA_CORE , (lane<<16)|0x80b1, 0, &rddata));
        rddata = rddata & (~(0x0007));
       
        SOC_IF_ERROR_RETURN(_phy_84793_dc_hc_pcb_write (unit, port, pc, 
                    PHY84793_DECA_CORE, (lane<<16), rddata));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN(_phy_84793_dc_hc_pcb_read (unit, port,pc,
           PHY84793_DECA_CORE , (lane << 16)|0x80b0, 0, &rddata));
        *value = (rddata & 0x1000) ? 1 : 0;
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_remote_loopback_set(int32 unit, soc_port_t port, int32 enable)
{
    phy_ctrl_t *pc; 
    uint16 mode = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);

    /* Squelch System side */
    SOC_IF_ERROR_RETURN
        (_phy_84793_squelch_enable (unit, port, PHY84793_SYS_SIDE, enable));

    /*Enable Remote loopback*/
    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:
	        /* Fall Thru */
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
	        /* Fall Thru */
        case PHY84793_MODE_10X10G_TO_4X25G_OTL:  
            SOC_IF_ERROR_RETURN
                (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                 CTL_REGS_LINE_LOOPBACK_EN, 
                 CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT + 9,
                 CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT, 
                  enable?0x3ff:0));

        break;
        case PHY84793_MODE_4X10G_TO_4X10G:
            if (pc->lane_num == 0) {
                SOC_IF_ERROR_RETURN
                (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                 CTL_REGS_LINE_LOOPBACK_EN, 
                 CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT + 3,
                 CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT, 
                  enable?0xf:0)); 
            } else {
                SOC_IF_ERROR_RETURN
                (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                 CTL_REGS_LINE_LOOPBACK_EN, 
                 CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT+8,
                 CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT+ 5, 
                  enable?0xf:0)); 
            }
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:
          SOC_IF_ERROR_RETURN
            (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
             CTL_REGS_LINE_LOOPBACK_EN,
             CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT + pc->lane_num,
             CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT + pc->lane_num, enable));
        break;
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_local_loopback_set(int32 unit, soc_port_t port, int32 enable)
{
    phy_ctrl_t *pc; 
    uint16 mode = 0;
    uint16 start_lane = 0, end_lane = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);
   
    /*Enable Line side(Remote)loopback*/
    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:
	        /* Fall Thru */
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
	        /* Fall Thru */
        case PHY84793_MODE_10X10G_TO_4X25G_OTL:  
           /* Disable(Enable) Squelch when we  Enable(Disable) loopback */
            SOC_IF_ERROR_RETURN
               (_phy_84793_squelch_enable (unit, port, PHY84793_LINE_SIDE, enable));

            SOC_IF_ERROR_RETURN
              (_phy_84793_modify_pma_pmd_reg(unit, port, pc, CTL_REGS_SYS_LOOPBACK_EN,
                CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT + 9 ,
                CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT , enable?0x3ff:0));
        break;

        case PHY84793_MODE_4X10G_TO_4X10G:
            if (pc->lane_num == PHY84793_VSR40_LANE0) {
                start_lane = 0;
                end_lane = 3;
            } else {
                start_lane = 5;
                end_lane=8;
            }
            for (;start_lane <= end_lane; start_lane++) {
               /* Disable(Enable) Squelch when we  Enable(Disable) loopback */
                SOC_IF_ERROR_RETURN
                  (_phy_84793_line_lane_squelch_enable (unit, port, start_lane,
                                                        enable));
 
                SOC_IF_ERROR_RETURN
                  (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                  CTL_REGS_SYS_LOOPBACK_EN,
                  CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT + start_lane ,
                  CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT + start_lane, 
                  enable));
            }
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:
              /* Disable(Enable) Squelch when we  Enable(Disable) loopback */
              SOC_IF_ERROR_RETURN
                  (_phy_84793_line_lane_squelch_enable (unit, port, pc->lane_num,
                                                        enable));

              SOC_IF_ERROR_RETURN
              (_phy_84793_modify_pma_pmd_reg(unit, port, pc, 
                CTL_REGS_SYS_LOOPBACK_EN,
                CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT + pc->lane_num,
                CTL_REGS_SYS_LOOPBACK_EN_SYS_LPBK_EN_SHIFT + pc->lane_num, 
                enable));
        break;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84793_lb_set
 * Purpose:
 *    Put 84793 in PHY PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *      enable - binary value for on/off (1/0)
 * Returns:
 *    SOC_E_NONE
 */

STATIC int32
phy_84793_lb_set (int32 unit, soc_port_t port, int32 enable)
{

    SOC_IF_ERROR_RETURN
        (_phy_84793_local_loopback_set (unit, port, enable));
    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_init_pass2 (int32 unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    phy84793_intf_cfg_t *interface;
    uint16 if_sys_idx = 0;
    uint16 force_dload = 0, data16 = 0;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84793 init pass2: u=%d p=%d\n"), unit, port));
    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * - interfaces are XAUI/XLAUI/SR/SR10
     * - line side mode can be changed dynamically
     *   - configured specifically with interface set and/or by matching speed
     *
     *    port mode            system side          
     * ----------------  ------------------------  
     * 100G                     dflt:  CAUI
     * 40G                      XLAUI   
     * 10G(sync)                SR10 
     * 10G(async)               SR
     */
	
    interface = &(INTERFACE(pc));
    sal_memset(interface, 0, sizeof(phy84793_intf_cfg_t));

    pc->flags |= PHYCTRL_INIT_DONE;
    force_dload = soc_property_port_get(unit, port,
                     spn_PHY_FORCE_FIRMWARE_LOAD, FALSE);
    if (force_dload) {
        SOC_IF_ERROR_RETURN
           (_phy_84793_rom_firmware_download (unit, port, 
            PHY84793_FW_DLOAD_OFFSET, 
            pc, phy84793_ucode_bin, phy84793_ucode_bin_len));
    } else {
        SOC_IF_ERROR_RETURN(
            READ_PHY84793_PMA_PMD_REG(unit, pc, COMMON_POR_REGS_BOOT_POR, &data16));
        if ((data16 & 0x2000) != 0x2000) {
            SOC_IF_ERROR_RETURN
               (_phy_84793_rom_firmware_download (unit, port, 
                PHY84793_FW_DLOAD_OFFSET, 
                pc, phy84793_ucode_bin, phy84793_ucode_bin_len));
        } else {
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "FW is already downloaded!!. Use spn_PHY_FORCE_FIRMWARE_LOAD"
                            "to force download the micro\n")));
        }
    }
    /* Get configured system interface */
    if_sys_idx = soc_property_port_get(unit, port, spn_PHY_SYS_INTERFACE, 0);
    if (if_sys_idx >= (sizeof(phy_84793_sys_to_port_if)/sizeof(soc_port_if_t))) {
        LOG_ERROR(BSL_LS_SOC_PHY, \
                  (BSL_META_U(unit, \
                              "PHY84793 invalid system side\
                              interface: u=%d p=%d intf=%d\n"
                              "Using default interface\n"),
                   unit, port, if_sys_idx));
        if_sys_idx = 0;
    }
    
    interface->type = phy_84793_sys_to_port_if[if_sys_idx];
    if (interface->type == 0)  {
        /* Set the Default system interface */
        interface->type = SOC_PORT_IF_SR4;
        interface->speed = PHY84793_MODE1_SPD;  
    } else {
        switch (interface->type) {
            case SOC_PORT_IF_SR4:
                /* If interface is SR4 use the configured speed
                 * dont modify the speed, because SR4 shares 100 and 40G*/
            break;
            case SOC_PORT_IF_SR10:  
                interface->speed = PHY84793_MODE3_SPD;
            break;
            case SOC_PORT_IF_SR:
                interface->speed = PHY84793_MODE4_SPD; 
            break;
            case SOC_PORT_IF_OTL: 
                interface->speed = PHY84793_MODE5_SPD; 
            break;
            default:
            break;
        }
        SOC_IF_ERROR_RETURN(
               _phy_84793_speed_set(unit, port, interface->speed));
    }
    return SOC_E_NONE;
}

STATIC int32
_phy_84793_config_update(int32 unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    pc = EXT_PHY_SW_STATE(unit, port);
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84793 INIT: u=%d p=%d TX Pol=0x%x RX Pol:0x%x\n"), 
                         unit, port,POL_TX_CFG(pc), POL_RX_CFG(pc)));
    return _phy_84793_line_polarity_flip(unit, port, pc, POL_TX_CFG(pc), POL_RX_CFG(pc));
}

STATIC
int32 phy_84793_init(int32 unit, soc_port_t port)
{
    uint32     devid = 0;
    phy_ctrl_t *pc;
    uint16 len = 0;
    soc_phy_info_t *pi;
    char *dev_name;
    soc_info_t *si;
    int phy_port;  /* physical port number */


    pc = EXT_PHY_SW_STATE(unit, port);
    dev_name = DEV_NAME(pc);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84793 init: u=%d p=%d state=%d\n"), 
                         unit, port,PHYCTRL_INIT_STATE(pc)));
    
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
        PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45);
        
	    /* READ DEVICE IDENTIFICATION REG TO IDENTIFY THE REG */
        SOC_IF_ERROR_RETURN(_phy_84793_dev_id_get(unit, port, pc, &devid));
        DEVID(pc) = devid;
        /* indicate second pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
        return SOC_E_NONE;
    }
    
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84793 init pass2: u=%d p=%d\n"), unit, port));
        SOC_IF_ERROR_RETURN(_phy_84793_init_pass2(unit, port));
    }
    si = &SOC_INFO(unit);
    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = si->port_l2p_mapping[port];
    } else {
        phy_port = port;
    }

    if (SOC_IS_TRIUMPH3(unit)) {
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
    } else {
       /* For trident  and other switch*/
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port) + (4 * SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, port)));

        pc->lane_num  %= 10;
    }
    /* convert to phy_ctrl macros */
    if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), pc->port)) {
        pc->phy_mode = PHYCTRL_MULTI_CORE_PORT;
        pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
    } else if (si->port_num_lanes[port] == 4) {
        pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
    } else if (si->port_num_lanes[port] == 1) {
        pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
        pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
    } else if (si->port_num_lanes[port] >= 10) {
        pc->phy_mode = PHYCTRL_MULTI_CORE_PORT;
        pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
    }

    pi = &SOC_PHY_INFO(unit, port);
    if (DEVID(pc) == PHY84793_ID_84793) { 
        /* coverity[secure_coding] */
        sal_strcpy (dev_name, "BCM84793");
    } else if (DEVID(pc) == PHY84793_ID_84790) {
        /* coverity[secure_coding] */
        sal_strcpy (dev_name, "BCM84790");
    } else if (DEVID(pc) == PHY84793_ID_84794) {
        /* coverity[secure_coding] */
        sal_strcpy (dev_name, "BCM84794");
    } 
    len = sal_strlen(dev_name);
        
    /* phy_mode: 1 Single port mode, port uses 1 lanes
     *           4 quad port mode, port uses 4 lane
     */
    if (pc->phy_mode == PHYCTRL_ONE_LANE_PORT) {
        dev_name[len++] = '/';
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        dev_name[len++] = pc->lane_num + '0';
        SOC_IF_ERROR_RETURN(phy_84793_speed_set (unit, port, PHY84793_MODE4_SPD));
    } else if (pc->phy_mode == PHYCTRL_QUAD_LANE_PORT) {
        dev_name[len++] = '/';
        dev_name[len++] = pc->lane_num + '0';
        SOC_IF_ERROR_RETURN(phy_84793_speed_set (unit, port, PHY84793_MODE2_SPD));
    } else {
        /*pc->dev_name[len++] = '4';*/
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
    }
    dev_name[len] = 0;
    pc->dev_name = dev_name;  /* string terminator */
    pi->phy_name = dev_name;

    /* Polarity configuration is applied every time interface is configured */
	POL_TX_CFG(pc) = (uint16) soc_property_port_get(unit, port, 
            spn_PHY_TX_POLARITY_FLIP, PHY84793_POL_DND);
	POL_RX_CFG(pc) = (uint16) soc_property_port_get(unit, port, 
            spn_PHY_RX_POLARITY_FLIP, PHY84793_POL_DND);
    
    /* Push configuration to the device */
	SOC_IF_ERROR_RETURN (_phy_84793_config_update(unit, port));
    
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84793_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #
 *    .
 *    link - address of memory to store link up/down state.
 * Returns:
 *    SOC_E_NONE
 */
STATIC int32 
phy_84793_link_get(int32 unit, soc_port_t port, int32 *link)
{
    phy_ctrl_t *pc;
    int16 mode = 0, lane = 0;
    phy_ctrl_t *int_pc;
    int32 int_phy_link;

    if (link == NULL) {
        return SOC_E_PARAM;
    }
    
    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }
    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    PHY84793_GET_PORT_MODE(pc, mode);

    if (mode == PHY84793_MODE_4X10G_TO_4X10G) {
        if(pc->lane_num == 0) {
            lane = 0;
        }
        else {
            lane = 5;
        } 
    } else {
        lane = pc->lane_num;
    }
    /*
     * PCS from the internal PHY is used to determine link.
     */
    if (int_pc != NULL) {
		SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &int_phy_link));
    } else {
		*link = 0;
	}

    SOC_IF_ERROR_RETURN
            (phy_84793_per_lane_link_get(unit, port, lane, link));

     /* When CDR lock of 84793 is not Set, Leave the port link as
     * CDR status of 84793. When CDR lock of 84793 is set use
     * PCS status of internal serdes as port link status.
     * When internal serdes is not there use 84793 CDR as link status*/
    if (*link && int_pc) {
        *link = int_phy_link;
    }
   
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_84793_link_get: u=%d port%d: link:%s\n"),
                 unit, port, *link ? "Up": "Down"));

    return SOC_E_NONE;
}

STATIC int32 
phy_84793_per_lane_link_get(int32 unit, soc_port_t port, uint16 lane, int32 *link)
{
    uint16 mode = 0, link_stat = 0;
    uint16 rx_seq_done = 0, hclane = 0;
    phy_ctrl_t *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    PHY84793_GET_PORT_MODE(pc, mode);

    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:
	        /* Fall Thru */
        case PHY84793_MODE_10X10G_TO_4X25G_OTL:  
            SOC_IF_ERROR_RETURN(SET_PHY84793_SLICE(unit, pc, PHY84793_SLICE_0));
            SOC_IF_ERROR_RETURN
               (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                IEEE_PMA_PMD_REGS_PMA_PMD_STATUS_1, &link_stat));
    
            *link = GET_BIT_VAL(link_stat, 2, 2);
        break;
        case PHY84793_MODE_4X10G_TO_4X10G:
            if (pc->lane_num <= 3) {
                SOC_IF_ERROR_RETURN
                  (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                  VSR40_RX_DIG_REGS_CDR_CONTROL, &link_stat));
                *link = (((link_stat & 0xF0) == 0xF0)? 1 : 0);

            } else {
                for (hclane = 1; hclane <=4; hclane ++) {
                    SOC_IF_ERROR_RETURN
                      (_phy_84793_dc_hc_pcb_read (unit, port,pc,
                      PHY84793_HEXA_CORE , (hclane<<16)|0x80b1, 0, &link_stat));
                    link_stat = link_stat & (~(0x0007));
                    SOC_IF_ERROR_RETURN
                      (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_HEXA_CORE, 
                      (hclane<<16), link_stat));
                    SOC_IF_ERROR_RETURN
                      (_phy_84793_dc_hc_pcb_read (unit, port,pc,
                       PHY84793_HEXA_CORE , (hclane << 16)|0x80b0, 0, &link_stat));
                    if ((link_stat & 0x1000) == 0x1000) {
                       rx_seq_done ++;
                    }
                }
                 *link = (rx_seq_done == 4);
            }
            
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
            *link =1;
            for (lane = 0; lane <=9; lane ++) {
                if (lane <4) {
                   SOC_IF_ERROR_RETURN
                    (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                    VSR40_RX_DIG_REGS_CDR_CONTROL, &link_stat));
                   *link &= ((link_stat & 
                      (1 << (lane+VSR40_RX_DIG_REGS_CDR_CONTROL_CDRLOCK_STATUS_LN0_SHIFT)))
                       ? 1 : 0);
                } else {
                    SOC_IF_ERROR_RETURN
                     (_phy_84793_dc_hc_pcb_read (unit, port,pc,
                     PHY84793_HEXA_CORE , ((lane-4)<<16)|0x80b1, 0, &link_stat));
                    link_stat = link_stat & (~(0x0007));
                    SOC_IF_ERROR_RETURN
                      (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_HEXA_CORE, 
                     ((lane-4)<<16), link_stat));
                    SOC_IF_ERROR_RETURN
                     (_phy_84793_dc_hc_pcb_read (unit, port,pc,
                     PHY84793_HEXA_CORE , ((lane-4) << 16)|0x80b0, 0, &link_stat));
                    if ((link_stat & 0x1000) == 0x1000) {
                        *link &= 1;
                    } else {
                        *link &= 0;
                    }
                }
            }
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:
            if (lane < 4) {
                SOC_IF_ERROR_RETURN
                  (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                  VSR40_RX_DIG_REGS_CDR_CONTROL, &link_stat));
                *link = ((link_stat & 
                  (1 << (lane + VSR40_RX_DIG_REGS_CDR_CONTROL_CDRLOCK_STATUS_LN0_SHIFT)))
                          ? 1 : 0);
            } else {
                SOC_IF_ERROR_RETURN
                  (_phy_84793_dc_hc_pcb_read (unit, port,pc,
                  PHY84793_HEXA_CORE , ((lane-4)<<16)|0x80b1, 0, &link_stat));
                link_stat = link_stat & (~(0x0007));
                SOC_IF_ERROR_RETURN
                  (_phy_84793_dc_hc_pcb_write (unit, port, pc, PHY84793_HEXA_CORE, 
                  ((lane-4)<<16), link_stat));
                SOC_IF_ERROR_RETURN
                  (_phy_84793_dc_hc_pcb_read (unit, port,pc,
                  PHY84793_HEXA_CORE , ((lane-4) << 16)|0x80b0, 0, &link_stat));
                if ((link_stat & 0x1000) == 0x1000) {
                    *link = 1;
                } else {
                    *link = 0;
                }
            }
        break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84793_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int32
phy_84793_reg_read(int32 unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data16;
    phy_ctrl_t          *pc;      /* PHY software state */
    int32 rv = SOC_E_NONE;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (flags & SOC_PHY_I2C_DATA8) {
    } else if (flags & SOC_PHY_I2C_DATA16) {
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY84793_PMA_PMD_REG(unit, pc, phy_reg_addr, &data16));
        *phy_data = data16;
    }

    return rv;
}

/*
 * Function:
 *      phy_84793_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int32
phy_84793_reg_write(int32 unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    phy_ctrl_t          *pc;      /* PHY software state */
    int32 rv = SOC_E_NONE;

    pc = EXT_PHY_SW_STATE(unit, port);
 
    if (flags & SOC_PHY_I2C_DATA8) {
    } else if (flags & SOC_PHY_I2C_DATA16) {
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84793_PMA_PMD_REG (unit, pc, phy_reg_addr, 
                                         (uint16)phy_data));
    }
    return rv;
}

STATIC
int32 _phy_84793_modify_pma_pmd_reg (int32 unit, int32 port, phy_ctrl_t *pc, 
                               uint16 address, uint8 msb_pos, 
                               uint8 lsb_pos, uint16 data)
{
    uint16 mask = 0, i =0;

    for (i = lsb_pos; i<=msb_pos; i++) {
        mask |= (1 << i);
    }
   
    data = data << lsb_pos; 
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84793_PMA_PMD_REG(unit, pc, address,
                                        data, mask));
    return 0;
}

STATIC 
int32 _phy_84793_enable_msgout_int (int32 unit, int32 port, phy_ctrl_t *pc)
{
    uint16 rddata = 0, wrdata = 0;

    SOC_IF_ERROR_RETURN
           (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_MSGOUT, &rddata));

    SOC_IF_ERROR_RETURN
           (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_EISR, &rddata));
    wrdata = rddata | MSGOUT_BIT_POS;
   
    SOC_IF_ERROR_RETURN 
         (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_EISR, wrdata));

    SOC_IF_ERROR_RETURN
        (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_EIMR, &rddata));
    wrdata = rddata | MSGOUT_BIT_POS;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_EIMR, wrdata));

    SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_MSCR,
                                 CTL_REGS_MSCR_GLOBAL_INTR_MASK_SHIFT, 
                                 CTL_REGS_MSCR_GLOBAL_INTR_MASK_SHIFT, 1));
    return SOC_E_NONE;
}

STATIC 
int32 _phy_84793_wait_micro_msgout (int32 unit, int32 port, phy_ctrl_t *pc)
{
    uint16 tmp_data = 0, wrdata = 0;
    uint16 retry_cnt = PHY84793_FW_DLOAD_RETRY_CNT; 

    SOC_IF_ERROR_RETURN
       (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_EISR, &tmp_data));

    while (((tmp_data & MSGOUT_BIT_POS) == 0) && retry_cnt) { 
       SOC_IF_ERROR_RETURN
            (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_EISR, &tmp_data));
       retry_cnt --;
    }
    if (!retry_cnt) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN
         (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_EISR, &tmp_data));
    wrdata = tmp_data | MSGOUT_BIT_POS;

    SOC_IF_ERROR_RETURN
         (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_EISR, wrdata));

    return SOC_E_NONE;
}

STATIC 
int32 _phy_84793_check_micro_msgout_next (int32 unit, int32 port, phy_ctrl_t *pc)
{
    uint16 tmp_data=0;

    SOC_IF_ERROR_RETURN
           (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_MSGOUT, &tmp_data));
	 
    return SOC_E_NONE;
}

STATIC 
int32 _phy_84793_micro_soft_reset (int32 unit, int32 port, phy_ctrl_t *pc)
{
	 
    SOC_IF_ERROR_RETURN
    (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_COMMON_CTRL1,
                              CTL_REGS_COMMON_CTRL1_UC_RSTB_SHIFT, 
                              CTL_REGS_COMMON_CTRL1_UC_RSTB_SHIFT, 0));
    SOC_IF_ERROR_RETURN
    (_phy_84793_modify_pma_pmd_reg (unit, port, pc, CTL_REGS_COMMON_CTRL1, 
                              CTL_REGS_COMMON_CTRL1_UC_RSTB_SHIFT, 
                              CTL_REGS_COMMON_CTRL1_UC_RSTB_SHIFT, 1));
    return SOC_E_NONE;
}

STATIC 
int32 _phy_84793_rom_firmware_download (int32 unit, int32 port, int32 offset, 
                               phy_ctrl_t *pc, uint8 *new_fw, uint32 datalen)
{
    uint32 wr_data = 0, j = 0;
    uint16 data16 = 0;
    phy84793_intf_cfg_t *interface;
    
    SOC_IF_ERROR_RETURN
         (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_COMMON_CTRL1, 0xFFFE));
    SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg(unit, port, pc, COMMON_POR_REGS_BOOT_POR, 
				 COMMON_POR_REGS_BOOT_POR_SPI_PORT_USED_SHIFT,
                                 COMMON_POR_REGS_BOOT_POR_SPI_PORT_USED_SHIFT, 0)); 
    SOC_IF_ERROR_RETURN
            (_phy_84793_modify_pma_pmd_reg(unit, port, pc, COMMON_POR_REGS_BOOT_POR, 
                                     COMMON_POR_REGS_BOOT_POR_SER_BOOT_SHIFT, 
				     COMMON_POR_REGS_BOOT_POR_SER_BOOT_SHIFT, 1));
    SOC_IF_ERROR_RETURN
            (_phy_84793_enable_msgout_int(unit, port, pc));
    SOC_IF_ERROR_RETURN
            (_phy_84793_micro_soft_reset (unit, port, pc));
    SOC_IF_ERROR_RETURN
            (_phy_84793_wait_micro_msgout (unit, port, pc));
    SOC_IF_ERROR_RETURN
            (_phy_84793_check_micro_msgout_next (unit, port, pc));

    wr_data = 0x8000; /*DEST_RAM_ADDR*/
    SOC_IF_ERROR_RETURN
            (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_MSGIN, wr_data));

    SOC_IF_ERROR_RETURN
            (_phy_84793_wait_micro_msgout (unit, port, pc));
    SOC_IF_ERROR_RETURN
            (_phy_84793_check_micro_msgout_next (unit, port, pc));
    
    wr_data = (datalen)/2;

    SOC_IF_ERROR_RETURN
            (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_MSGIN, wr_data));
    
	 /* Fill in the SRAM */
    wr_data = (datalen - 1);
    for (j = 0; j < wr_data; j+=2) {
      
       /*Make sure the word is read by the Micro */
       SOC_IF_ERROR_RETURN
          (_phy_84793_wait_micro_msgout (unit, port, pc));
       SOC_IF_ERROR_RETURN
          (_phy_84793_check_micro_msgout_next (unit, port, pc));

       data16 = (new_fw[j+1] << 8) | new_fw[j];
       SOC_IF_ERROR_RETURN
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_MSGIN,
               data16));
    }
	
    SOC_IF_ERROR_RETURN
         (_phy_84793_wait_micro_msgout (unit, port, pc));
    SOC_IF_ERROR_RETURN
         (_phy_84793_check_micro_msgout_next (unit, port, pc));

    SOC_IF_ERROR_RETURN 
         (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG4, &data16));
 
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "Check Sum: 0x%X \n"), data16));
    if (data16 == 0x600D) {
        interface = &(INTERFACE(pc));
        interface->type = SOC_PORT_IF_SR4; 
        interface->speed = PHY84793_MODE1_SPD;
    }
    SOC_IF_ERROR_RETURN 
           (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_GPREG0, &data16));
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "Version: 0x%X\n"), data16));
    return SOC_E_NONE;
}

STATIC
int32 phy_84793_firmware_set(int32 unit, int32 port, int32 offset,
                            uint8 *array, int32 datalen)
{
    phy_ctrl_t *pc;

    if (array == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        return SOC_E_NONE;
    }
    pc = EXT_PHY_SW_STATE(unit, port);
    
    SOC_IF_ERROR_RETURN
    (_phy_84793_rom_firmware_download(unit, port, offset, pc, array, datalen));

    return SOC_E_NONE;
}

STATIC int32 
phy_84793_probe(int32 unit, phy_ctrl_t *pc)
{
    uint32 devid = 0;
    
    SOC_IF_ERROR_RETURN(
        _phy_84793_dev_id_get(pc->unit, pc->port, pc,  &devid));
    
    if (devid == PHY84793_ID_84793 || devid == PHY84793_ID_84790 ||
            devid == PHY84793_ID_84794) {
        /*pc->dev_name = dev_name_84793;*/
    } else {  /* not found */
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "port %d: BCM84xxx type"
                             "PHY device detected, please use"
                             "phy_84<xxx> config variable to select"
                             "the specific type\n"),
                  pc->port));
        return SOC_E_NOT_FOUND;
    }
    pc->size = sizeof(phy84793_dev_desc_t);

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84793_ability_advert_set
 * Purpose:
 *      Set the current advertisement for mode1.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int32
phy_84793_ability_advert_set(int32 unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
    /* PHY84793 Doesnt support AN */
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84793_ability_advert_get
 * Purpose:
 *      Set the current advertisement for mode1.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int32
phy_84793_ability_advert_get(int32 unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
    if (ability == NULL) {
        return SOC_E_PARAM;
    }
	sal_memset(ability, 0, sizeof(soc_port_ability_t));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84793_enable_set
 * Purpose:
 *    Enable/Disable phy 
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *    enable - on/off state to set
 * Returns:    
 *    SOC_E_NON    
 */

STATIC int32
phy_84793_enable_set(int32 unit, soc_port_t port, int32 enable)
{
    uint16 mode = 0, tx_data = 0, rx_data = 0;
    uint16 tx_sys_data = 0, rx_sys_data = 0;
    uint16 vsr_tx_data = 0, vsr_rx_data = 0;
    uint16 start_lane =0, end_lane =0, lane = 0;

    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
 
    PHY84793_GET_PORT_MODE(pc, mode);
    if (mode == PHY84793_MODE_10X10G_TO_10X10G_ASYNC) {
        SOC_IF_ERROR_RETURN 
          (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_VSR40_TX_PD, &vsr_tx_data));
        vsr_tx_data = (~vsr_tx_data) & 0xf;
        SOC_IF_ERROR_RETURN 
          (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_VSR40_RX_PD, &vsr_rx_data));
        vsr_rx_data = (~vsr_rx_data) & 0xf;
        SOC_IF_ERROR_RETURN 
          (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_HCTX_PD, &tx_data));
        tx_data = (((~tx_data) & 0x3f) << 4) | vsr_tx_data;
        SOC_IF_ERROR_RETURN 
          (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_HCRX_PD, &rx_data));
        rx_data = (((~rx_data) & 0x3f) << 4) | vsr_rx_data;

        SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_DCTX_PD, &tx_sys_data));
        tx_sys_data = (~tx_sys_data) & 0x3ff;
        SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_DCRX_PD, &rx_sys_data));
        rx_sys_data = (~rx_sys_data) & 0x3ff;
        
        tx_data &= ~(1 << pc->lane_num);
        rx_data &= ~(1 << pc->lane_num);
        tx_sys_data &= ~(1 << pc->lane_num);
        rx_sys_data &= ~(1 << pc->lane_num);
        if (enable) {
            tx_data |= (1 << pc->lane_num);
            rx_data |= (1 << pc->lane_num);

            tx_sys_data |= (1 << pc->lane_num);
            rx_sys_data |= (1 << pc->lane_num);
        }
        SOC_IF_ERROR_RETURN(_phy_84793_sys_enable_set (unit, port, pc, 
			tx_sys_data, rx_sys_data)); 

        SOC_IF_ERROR_RETURN(_phy_84793_line_enable_set (unit, port, pc, 
			tx_data, rx_data)); 
    } else if (mode == PHY84793_MODE_4X10G_TO_4X10G) {
        if(pc->lane_num == 0) {
            start_lane = PHY84793_VSR40_LANE0;
            end_lane = PHY84793_VSR40_LANE3;
        } else {
            start_lane = 5;
            end_lane = 6;
        }
        for (lane = start_lane; lane <= end_lane ; lane ++) {
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_VSR40_TX_PD, 
                                         &vsr_tx_data));
            vsr_tx_data = (~vsr_tx_data) & 0xf;
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_VSR40_RX_PD, 
                                         &vsr_rx_data));
            vsr_rx_data = (~vsr_rx_data) & 0xf;
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_HCTX_PD, &tx_data));
            tx_data = (((~tx_data) & 0x3f) << 4) | vsr_tx_data;
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_HCRX_PD, &rx_data));
            rx_data = (((~rx_data) & 0x3f) << 4) | vsr_rx_data;

            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_DCTX_PD, 
                                         &tx_sys_data));
            tx_sys_data = (~tx_sys_data) & 0x3ff;
            SOC_IF_ERROR_RETURN 
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_DCRX_PD, 
                                         &rx_sys_data));
            rx_sys_data = (~rx_sys_data) & 0x3ff;
        
            tx_data &= ~(1 << lane);
            rx_data &= ~(1 << lane);
            tx_sys_data &= ~(1 << lane);
            rx_sys_data &= ~(1 << lane);
            if (enable) {
                tx_data |= (1 << lane);
                rx_data |= (1 << lane);

                tx_sys_data |= (1 << lane);
                rx_sys_data |= (1 << lane);
            }
            SOC_IF_ERROR_RETURN(_phy_84793_sys_enable_set (unit, port, pc, 
			    tx_sys_data, rx_sys_data)); 

            SOC_IF_ERROR_RETURN(_phy_84793_line_enable_set (unit, port, pc, 
                tx_data, rx_data)); 
        }
    } else {
        /* Manually power channel up/down in Tx and RX dir of line and system side*/
        SOC_IF_ERROR_RETURN(_phy_84793_sys_enable_set (unit, port, pc, 
			enable ? PHY84793_ENABLE_ALL_LANES : 0, 
			enable ? PHY84793_ENABLE_ALL_LANES : 0));

        SOC_IF_ERROR_RETURN(_phy_84793_line_enable_set (unit, port, pc, 
			enable ? PHY84793_ENABLE_ALL_LANES : 0, 
			enable ? PHY84793_ENABLE_ALL_LANES : 0));
    }
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84793_interface_set
 * Purpose:
 *      Set either CR4 interface or SR4/LR4. The valid interface are SOC_PORT_IF_CR4
 *      and SOC_PORT_IF_SR4 which put device in default mode: SR4/LR4 mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_UNAVAIL - unsupported interface
 */

STATIC int32
phy_84793_interface_set(int32 unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t           *pc;
    phy84793_intf_cfg_t  *intf;

    if (pif == SOC_PORT_IF_MII || pif == SOC_PORT_IF_XGMII || pif == SOC_PORT_IF_GMII) {
		return phy_null_interface_set(unit, port, pif);
	}

    pc = EXT_PHY_SW_STATE(unit, port);
    intf = &(INTERFACE(pc));
   
    if (pif > SOC_PORT_IF_COUNT) {
        return SOC_E_PARAM;
    }

    /* translate to the mode of operation that supported */
    if (pif==SOC_PORT_IF_CAUI || pif==SOC_PORT_IF_XLAUI) {
        pif = SOC_PORT_IF_SR4;
    } else if (pif==SOC_PORT_IF_SFI || pif==SOC_PORT_IF_XFI) {
        pif = SOC_PORT_IF_SR;
    }


    switch(pif) 
    {
        case SOC_PORT_IF_SR4:
            if (intf->speed == PHY84793_MODE2_SPD) {
                LOG_CLI((BSL_META_U(unit,
                                    "PHY84793 MODE2 Configured\n")));
                SOC_IF_ERROR_RETURN
                 (_phy_84793_config_mode (unit, port, pc, 
                                          PHY84793_MODE_4X10G_TO_4X10G));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "PHY84793 MODE1 Configured\n")));
                SOC_IF_ERROR_RETURN
                 (_phy_84793_config_mode (unit, port, pc, 
                                          PHY84793_MODE_10X10G_TO_4X25G));
                intf->speed = PHY84793_MODE1_SPD;
            }
        break;
        case SOC_PORT_IF_SR10:
            LOG_CLI((BSL_META_U(unit,
                                "PHY84793 MODE3 Configured\n")));
            SOC_IF_ERROR_RETURN
             (_phy_84793_config_mode (unit, port, pc, 
                                      PHY84793_MODE_10X10G_TO_10X10G_SYNC));
            intf->speed = PHY84793_MODE3_SPD;

        break;
        case SOC_PORT_IF_SR:
            LOG_CLI((BSL_META_U(unit,
                                "PHY84793 MODE4 Configured\n")));
            SOC_IF_ERROR_RETURN
             (_phy_84793_config_mode (unit, port, pc, 
                                      PHY84793_MODE_10X10G_TO_10X10G_ASYNC));
            intf->speed = PHY84793_MODE4_SPD;
        break;
        case SOC_PORT_IF_OTL:
            LOG_CLI((BSL_META_U(unit,
                                "PHY84793 MODE5 Configured\n")));
            SOC_IF_ERROR_RETURN
             (_phy_84793_config_mode (unit, port, pc, 
                                      PHY84793_MODE_10X10G_TO_4X25G_OTL));
            intf->speed = PHY84793_MODE5_SPD;
        break;
        default:
            return SOC_E_PARAM;
    }
    intf->type = pif;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84793_ability_local_get
 * Purpose:
 *      Get the device's complete abilities.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - return device's abilities.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int32
phy_84793_ability_local_get(int32 unit, soc_port_t port, soc_port_ability_t *ability)
{
    uint32		pa_speed = 0;
    phy84793_intf_cfg_t *intf;

    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);
    if (ability == NULL) {
        return SOC_E_PARAM;
    }
	sal_memset(ability, 0, sizeof(soc_port_ability_t));

    intf = &(INTERFACE(pc));
    if (intf->speed == PHY84793_MODE1_SPD) {
        pa_speed = SOC_PA_SPEED_100GB;
    } else if (intf->speed == PHY84793_MODE2_SPD) {
        pa_speed = SOC_PA_SPEED_40GB;
    } else if (intf->speed == PHY84793_MODE4_SPD) {
        pa_speed = SOC_PA_SPEED_10GB;
    }
    ability->speed_full_duplex = pa_speed;
	ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
	ability->pause = 0; 
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->loopback  = SOC_PA_LB_PHY;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84793_ability_local_get: u=%d p=%d speed=0x%x\n"),
              unit, port, ability->speed_full_duplex));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84793_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - current link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int32
phy_84793_speed_get(int32 unit, soc_port_t port, int32 *speed)
{
    phy84793_intf_cfg_t *intf;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    intf = &(INTERFACE(pc));
    
    *speed = intf->speed;

    return SOC_E_NONE;
}
      
/*
 * Function:
 *      phy_84793_interface_get
 * Purpose:
 *      Get the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */

STATIC int32
phy_84793_interface_get(int32 unit, soc_port_t port, soc_port_if_t *pif)
{
    phy84793_intf_cfg_t *intf;
    phy_ctrl_t  *pc = EXT_PHY_SW_STATE(unit, port);

    intf = &(INTERFACE(pc));
    
    *pif = intf->type;
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84793_lb_get
 * Purpose:
 *    Get 84793 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *    enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int32
phy_84793_lb_get(int32 unit, soc_port_t port, int32 *enable)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
      (_phy_84793_per_lane_lb_get(unit, port, pc->lane_num , (uint32*)enable));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84793_lb_get: u=%d port%d: loopback:%s\n"),
              unit, port, *enable ? "Enabled": "Disabled"));
    
    return SOC_E_NONE;
}

STATIC int32 
_phy_84793_per_lane_lb_get(int32 unit, soc_port_t port, uint16 lane, uint32 *enable)
{
    uint16 mode = 0, tmp = 0;
    phy_ctrl_t *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    PHY84793_GET_PORT_MODE(pc, mode);

    /*Get local loopback*/
    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:
	        /* Fall Thru */
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
	        /* Fall Thru */
        case PHY84793_MODE_10X10G_TO_4X25G_OTL:  
            SOC_IF_ERROR_RETURN
             (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                        CTL_REGS_SYS_LOOPBACK_EN, &tmp));

            *enable = (tmp & 0x3ff) ? TRUE : FALSE;
        break;
        case PHY84793_MODE_4X10G_TO_4X10G:
            if (lane <=3) {
                SOC_IF_ERROR_RETURN
                   (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                              CTL_REGS_SYS_LOOPBACK_EN, &tmp));
                *enable = ( tmp & 0xF)?TRUE:FALSE;

            } else {
                SOC_IF_ERROR_RETURN
                   (READ_PHY84793_PMA_PMD_REG(unit, pc, 
                              CTL_REGS_SYS_LOOPBACK_EN, &tmp));

                *enable = (tmp & 0x1e0)?TRUE:FALSE;
            }
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:
            SOC_IF_ERROR_RETURN
              (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_SYS_LOOPBACK_EN, &tmp));
            *enable = (tmp & (1 <<lane)) ? TRUE : FALSE;
        break;
    }
    return SOC_E_NONE;
}

STATIC int32 
_phy_84793_per_lane_remote_lb_get(int32 unit, soc_port_t port, uint16 lane, 
                                  uint32 *enable)
{
    uint16 mode = 0, tmp = 0;
    phy_ctrl_t *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    PHY84793_GET_PORT_MODE(pc, mode);

    /*Enable Line side(Remote)loopback*/
    switch (mode) {
        case PHY84793_MODE_10X10G_TO_4X25G:
	        /* Fall Thru */
        case PHY84793_MODE_10X10G_TO_4X25G_OTL:  
            SOC_IF_ERROR_RETURN
             (READ_PHY84793_PMA_PMD_REG(unit, pc,
                CTL_REGS_LINE_LOOPBACK_EN , &tmp));
            
            *enable = (tmp & 0xf)? TRUE : FALSE;
        break;
        case PHY84793_MODE_4X10G_TO_4X10G:
            if (lane <=3) {
                SOC_IF_ERROR_RETURN
                     (READ_PHY84793_PMA_PMD_REG(unit, pc,
                             CTL_REGS_LINE_LOOPBACK_EN , &tmp));
                *enable = (tmp & 0xf) == 0xf? TRUE:FALSE;
            } else {
                SOC_IF_ERROR_RETURN
                     (READ_PHY84793_PMA_PMD_REG(unit, pc,
                             CTL_REGS_LINE_LOOPBACK_EN , &tmp));
                *enable = (tmp & 0x1e0) == 0x1e0? TRUE:FALSE;
            }
        break;
        case PHY84793_MODE_10X10G_TO_10X10G_SYNC:
        case PHY84793_MODE_10X10G_TO_10X10G_ASYNC:
            SOC_IF_ERROR_RETURN
             (READ_PHY84793_PMA_PMD_REG(unit, pc,
                    CTL_REGS_LINE_LOOPBACK_EN , &tmp));
            *enable = (tmp & 0x3ff) ? TRUE : FALSE;

        break;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84793_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int32
phy_84793_speed_set(int32 unit, soc_port_t port, int32 speed)
{
    int32 rv = 0;
    phy_ctrl_t  *pc;
    phy84793_intf_cfg_t  *intf;

    pc = EXT_PHY_SW_STATE(unit, port);
    intf = &(INTERFACE(pc));
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84793_speed_set: u=%d p=%d speed=%d\n"), 
                         unit, port, speed));
    switch (speed) {
        case PHY84793_MODE1_SPD:
        break;
        case PHY84793_MODE4_SPD:
            /* Line side 10GX10 system 10 X10G */
            intf->type = SOC_PORT_IF_SR;
        break;
        case PHY84793_MODE2_SPD:
	        /* Line side 10GX4X2port System 10GX4X2port*/
            intf->type = SOC_PORT_IF_SR4;
	    break;
        default:
            return SOC_E_FAIL;
    }
    intf->speed = speed;  
    rv =  _phy_84793_speed_set(unit, port, speed);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "84793  %s failed: u=%d p=%d\n"), 
                              FUNCTION_NAME(), unit, port));
    }
    return rv;
}

STATIC int32
_phy_84793_speed_set(int32 unit, soc_port_t port, int32 speed)
{
    phy_ctrl_t  *pc;
    phy84793_intf_cfg_t  *intf;

    pc = EXT_PHY_SW_STATE(unit, port);
    intf = &(INTERFACE(pc));

    intf->speed = speed;
    
    switch (speed) {
        case PHY84793_MODE1_SPD:
            if (intf->type == SOC_PORT_IF_SR4) {
                LOG_CLI((BSL_META_U(unit,
                                    "PHY84793 MODE1 Configured\n")));
                SOC_IF_ERROR_RETURN
                 (_phy_84793_config_mode (unit, port, pc, 
                                          PHY84793_MODE_10X10G_TO_4X25G));
	        }
            if (intf->type == SOC_PORT_IF_OTL) {
                LOG_CLI((BSL_META_U(unit,
                                    "PHY84793 MODE5 Configured\n")));
                SOC_IF_ERROR_RETURN
                (_phy_84793_config_mode (unit, port, pc, 
                                         PHY84793_MODE_10X10G_TO_4X25G_OTL));
            } 
            if (intf->type == SOC_PORT_IF_SR10 ) {
                LOG_CLI((BSL_META_U(unit,
                                    "PHY84793 MODE3 Configured\n")));
                SOC_IF_ERROR_RETURN
                (_phy_84793_config_mode (unit, port, pc, 
                                         PHY84793_MODE_10X10G_TO_10X10G_SYNC));
            }
        break;
        case PHY84793_MODE4_SPD:
            if (intf->type == SOC_PORT_IF_SR) {
                LOG_CLI((BSL_META_U(unit,
                                    "PHY84793 MODE4 Configured\n")));
                SOC_IF_ERROR_RETURN
                (_phy_84793_config_mode (unit, port, pc, 
                                         PHY84793_MODE_10X10G_TO_10X10G_ASYNC));
            }
         break;
         case PHY84793_MODE2_SPD:
             if (intf->type == SOC_PORT_IF_SR4) {
                LOG_CLI((BSL_META_U(unit,
                                    "PHY84793 MODE2 Configured\n")));
                SOC_IF_ERROR_RETURN
                (_phy_84793_config_mode (unit, port, pc, 
                                         PHY84793_MODE_4X10G_TO_4X10G));
             }
         break;
    }	 
    return SOC_E_NONE; 
}

/*
 * Function:
 *      phy_84793_an_set
 * Purpose:
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int32
phy_84793_an_set(int32 unit, soc_port_t port, int32 an)
{
    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_dev_mode_config (int32 unit, soc_port_t port, phy_ctrl_t *pc,
                        uint16 modesel, uint16 refclk_code, uint16 sysPllRst)
{
	uint8 frcMode = 0;

	if (modesel == 0) {
		frcMode = 0;
    } else {
		frcMode = 1;
    }

	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
        CTL_REGS_USER_MODESEL_REG, CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT,
        CTL_REGS_USER_MODESEL_REG_USER_MODESEL_FRC_SHIFT, frcMode));

	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
         CTL_REGS_USER_MODESEL_REG, 2, CTL_REGS_USER_MODESEL_REG_USER_MODESEL_SHIFT,
         modesel));
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc,
         CTL_REGS_REFCLK_SEL_REG, CTL_REGS_REFCLK_SEL_REG_REFCLK_CODE_SHIFT + 3,
         CTL_REGS_REFCLK_SEL_REG_REFCLK_CODE_SHIFT, refclk_code));

	if (sysPllRst == 1) {
        SOC_IF_ERROR_RETURN(_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                     CTL_REGS_DC_RESET, CTL_REGS_DC_RESET_FRC_DC_PLL_RSTB_SHIFT,
                     CTL_REGS_DC_RESET_DC_PLL_RSTB_SHIFT, 0x3));
	    SOC_IF_ERROR_RETURN(_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                     CTL_REGS_DC_RESET, CTL_REGS_DC_RESET_FRC_DC_PLL_RSTB_SHIFT,
                     CTL_REGS_DC_RESET_DC_PLL_RSTB_SHIFT, 0x2));
	    SOC_IF_ERROR_RETURN(_phy_84793_modify_pma_pmd_reg (unit, port, pc,
                     CTL_REGS_DC_RESET, CTL_REGS_DC_RESET_FRC_DC_PLL_RSTB_SHIFT,
                     CTL_REGS_DC_RESET_DC_PLL_RSTB_SHIFT, 0x3));
	    SOC_IF_ERROR_RETURN(_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                   CTL_REGS_DC_RESET, CTL_REGS_DC_RESET_FRC_DC_PLL_RSTB_SHIFT, 
                    CTL_REGS_DC_RESET_DC_PLL_RSTB_SHIFT, 0x1));
    }
    return SOC_E_NONE;
}

STATIC
uint16 _phy84793_get_val(uint16 regval, uint16 msb, uint16 lsb)
{
    uint16 mask =0;
    uint16 status = 0;

    mask = (power(2,(msb+1))) - (power (2,lsb));
    status = (regval & mask)/(power(2,lsb));
    return status;
}

STATIC
int32 _phy84793_pcb_bus_read (int32 unit, soc_port_t port, phy_ctrl_t *pc,
                    uint16 core_sel,uint16 lane, uint16 addr, 
                    uint16 clr_reg, uint16 *data)
{
    uint16 msb = 0, lsb = 0, rddata = 0;
		
    msb = _phy84793_get_val(lane,15,0);
    lsb = _phy84793_get_val(addr,15,0);
		
	if (core_sel >= 3) {
        return SOC_E_FAIL;
    }
    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_ADR_MSW_TYPE, msb));
    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_ADR_LSW_TYPE, lsb)); 
    sal_udelay(10);
		
    /*assert read bit F452.4 and enable bit F452.0*/
    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
                                    power(2,core_sel)));
    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
                                   (0x10 + power(2,core_sel))));

    SOC_IF_ERROR_RETURN
     (READ_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_DATA_OUT_TYPE, &rddata));

    if (clr_reg) {
         /*clear, read And enable bits asserted*/
        SOC_IF_ERROR_RETURN 
           (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 0x31));
        /* read and enable bits asserted*/
        SOC_IF_ERROR_RETURN
           (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 0x11));
    }
    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 0x0));
    sal_udelay(10);
    *data = rddata;

    return SOC_E_NONE;
}

STATIC
int32 pcb_bus_write (int32 unit, soc_port_t port, phy_ctrl_t *pc,
                     uint16 core_sel, uint16 lane, uint16 addr, uint16 value)
{
    uint16 msb = 0, lsb = 0;
		
    msb = _phy84793_get_val(lane,15,0);
    lsb = _phy84793_get_val(addr,15,0);

    if (core_sel >= 3) {
	    return SOC_E_FAIL; 
    }
		
    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_ADR_MSW_TYPE, msb));
    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_ADR_LSW_TYPE, lsb));
    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_DATA_IN_TYPE, value));

    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
                                    power(2, core_sel)));
    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 
                                    (0x8 + power(2,core_sel))));

    SOC_IF_ERROR_RETURN 
        (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_PCB_CTRL_TYPE, 0x0));
    sal_udelay(10);

    return SOC_E_NONE;
}

STATIC
int32 _phy_84793_pcb_wr_bits(int32 unit, soc_port_t port, phy_ctrl_t *pc,
                  uint16 core_sel, uint16 lane, uint16 addr, 
                  uint16 msb, uint16 lsb, uint16 value)
{
    uint16 regval = 0, mask = 0, maskr = 0, shift_val = 0;

    SOC_IF_ERROR_RETURN 
        (_phy84793_pcb_bus_read(unit, port, pc, core_sel, lane, 
                                addr, 0, &regval));

    mask = (power(2,(msb+1)) - power(2,lsb));
    maskr = 65535 & ((-1) ^ mask);
    shift_val = (power(2,lsb)) * value;
    regval = (regval & maskr)  + (shift_val & mask);
    
    SOC_IF_ERROR_RETURN(pcb_bus_write(unit, port, pc, 
                core_sel, lane, addr, regval));

    return SOC_E_NONE;
}

STATIC int32
_phy_84793_div_change(int32 unit, soc_port_t port, uint16 setmode, 
                     uint16 refclkin, uint16 mode)
{
	uint16 rxRefDiv = 0;
	uint16 rxFbDiv = 0;
	uint16 rx25gMode = 0;
	uint16 initRefclkMode = 0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (refclkin == 1) {
        rxRefDiv = 1;
        initRefclkMode = 6;
    } else if(refclkin == 0){
        rxRefDiv = 0;
		initRefclkMode = 5;
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "Error\n")));
		return SOC_E_FAIL;
    }
		
    rxFbDiv  = 2;
    rx25gMode = 0;
    if(mode == PHY84793_MODE_10X10G_TO_4X25G) {
        initRefclkMode = 0;
    }


    /*
	 * Set mode to Mode 2, 3 or 4 first with 161M REFCLK
	 */
     SOC_IF_ERROR_RETURN
         (_phy_84793_dev_mode_config(unit,port, pc, mode, initRefclkMode, 1));

	/*Overwrite the RX PLL divider settings to support Div 132*/
    /* Force for refDiv*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_RX_PLL_CONTROL1, 
                  VSR40_RX_PLL_CONTROL1_SUSM_RX_REF_DIV_FRC_SHIFT, 
                  VSR40_RX_PLL_CONTROL1_SUSM_RX_REF_DIV_FRC_SHIFT, setmode ));
    
    SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_RX_PLL_CONTROL1, 
                  VSR40_RX_PLL_CONTROL1_SUSM_RX_REF_DIV_SHIFT+1,
                  VSR40_RX_PLL_CONTROL1_SUSM_RX_REF_DIV_SHIFT, rxRefDiv));
    /* Force for rxFbDiv(1)*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_RX_PLL_CONTROL4,
                  VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_E100G_FRC_SHIFT,
                  VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_E100G_FRC_SHIFT, setmode)); 

    /* Force for rxFbDiv(0)*/
    SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_RX_PLL_CONTROL4, 
                  VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_25G_FRC_SHIFT, 
                  VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_25G_FRC_SHIFT, setmode)); 
    SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_RX_PLL_CONTROL4, 
                  VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_E100G_SHIFT, 
                  VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_E100G_SHIFT, 
                  _phy84793_get_val(rxFbDiv, 1, 1)));
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_RX_PLL_CONTROL4,  
                  VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_25G_SHIFT,  
                  VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_25G_SHIFT, 
                  _phy84793_get_val(rxFbDiv, 0, 0)));
    /* Force for 20g mode*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_RX_PLL_CONTROL2, 
                 VSR40_RX_PLL_CONTROL2_SUSM_RX_VCO_EN25G_FRC_SHIFT,
                 VSR40_RX_PLL_CONTROL2_SUSM_RX_VCO_EN25G_FRC_SHIFT, setmode)); 	
    /* Force for 20g mode*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_RX_PLL_CONTROL2, 
                 VSR40_RX_PLL_CONTROL2_SUSM_RX_VCO_EN25G_SHIFT, 
                 VSR40_RX_PLL_CONTROL2_SUSM_RX_VCO_EN25G_SHIFT, rx25gMode)); 
	/* Assert RX CMU Reset*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                 VSR40_RX_DIG_REGS_RESET_CONTROL,
                 VSR40_RX_DIG_REGS_RESET_CONTROL_CAL_RSTB_SHIFT, 
                 VSR40_RX_DIG_REGS_RESET_CONTROL_CAL_RSTB_SHIFT, 0x0)); 
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                 VSR40_RX_DIG_REGS_RESET_CONTROL,  
                 VSR40_RX_DIG_REGS_RESET_CONTROL_CAL_RSTB_SHIFT, 
                 VSR40_RX_DIG_REGS_RESET_CONTROL_CAL_RSTB_SHIFT, 0x1));
    /* Assert RX CMU Lkdet Reset*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                 VSR40_RX_DIG_REGS_RESET_CONTROL,
                 VSR40_RX_DIG_REGS_RESET_CONTROL_LKDT_RSTB_SHIFT,  
                 VSR40_RX_DIG_REGS_RESET_CONTROL_LKDT_RSTB_SHIFT, 0x0)); 
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                 VSR40_RX_DIG_REGS_RESET_CONTROL,
                 VSR40_RX_DIG_REGS_RESET_CONTROL_LKDT_RSTB_SHIFT,  
                 VSR40_RX_DIG_REGS_RESET_CONTROL_LKDT_RSTB_SHIFT, 0x1));

	/* Overwrite the TX PLL divider settings*/
    /* Force for refDiv*/
	SOC_IF_ERROR_RETURN(_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                VSR40_TX_PLL_CONTROL1, 
                VSR40_RX_PLL_CONTROL1_SUSM_RX_REF_DIV_FRC_SHIFT, 
                VSR40_RX_PLL_CONTROL1_SUSM_RX_REF_DIV_FRC_SHIFT, setmode));  	
    SOC_IF_ERROR_RETURN(_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                VSR40_TX_PLL_CONTROL1, 
                VSR40_RX_PLL_CONTROL1_SUSM_RX_REF_DIV_SHIFT+1 , 
                VSR40_RX_PLL_CONTROL1_SUSM_RX_REF_DIV_SHIFT, rxRefDiv));
    /*Force for rxFbDiv(1)*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_TX_PLL_CONTROL4, 
                VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_E100G_FRC_SHIFT,
                VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_E100G_FRC_SHIFT, setmode));  
    /*Force for rxFbDiv(0)*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_TX_PLL_CONTROL4, 
                VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_25G_FRC_SHIFT, 
                VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_25G_FRC_SHIFT, setmode));  
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_TX_PLL_CONTROL4,
                VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_E100G_SHIFT, 
                VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_E100G_SHIFT, 
                _phy84793_get_val(rxFbDiv, 1, 1)));
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_TX_PLL_CONTROL4, 
                VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_25G_SHIFT,  
                VSR40_RX_PLL_CONTROL4_SUSM_RX_EN_25G_SHIFT, 
                _phy84793_get_val(rxFbDiv, 0, 0)));
    /*Force for 20g mode*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_TX_PLL_CONTROL2, 
                VSR40_RX_PLL_CONTROL2_SUSM_RX_VCO_EN25G_FRC_SHIFT, 
                VSR40_RX_PLL_CONTROL2_SUSM_RX_VCO_EN25G_FRC_SHIFT, setmode));  
    /*Force for 20g mode*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, VSR40_TX_PLL_CONTROL2, 
                VSR40_RX_PLL_CONTROL2_SUSM_RX_VCO_EN25G_SHIFT, 
                VSR40_RX_PLL_CONTROL2_SUSM_RX_VCO_EN25G_SHIFT, rx25gMode)); 
	
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                VSR40_TX_DIG_REGS_RESET_CONTROL, 
                VSR40_TX_DIG_REGS_RESET_CONTROL_CAL_RSTB_SHIFT,  
                VSR40_TX_DIG_REGS_RESET_CONTROL_CAL_RSTB_SHIFT, 
                0x0 )); /*Assert TX CMU Reset*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                VSR40_TX_DIG_REGS_RESET_CONTROL, 
                VSR40_TX_DIG_REGS_RESET_CONTROL_CAL_RSTB_SHIFT,  
                VSR40_TX_DIG_REGS_RESET_CONTROL_CAL_RSTB_SHIFT, 0x1 ));
    /*Assert TX CMU Lkdet Reset*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                VSR40_TX_DIG_REGS_RESET_CONTROL, 
                VSR40_TX_DIG_REGS_RESET_CONTROL_LKDT_RSTB_SHIFT,
                VSR40_TX_DIG_REGS_RESET_CONTROL_LKDT_RSTB_SHIFT, 0x0 ));
    SOC_IF_ERROR_RETURN
        (_phy_84793_modify_pma_pmd_reg (unit, port, pc, 
                VSR40_TX_DIG_REGS_RESET_CONTROL,  
                VSR40_TX_DIG_REGS_RESET_CONTROL_LKDT_RSTB_SHIFT,  
                VSR40_TX_DIG_REGS_RESET_CONTROL_LKDT_RSTB_SHIFT, 0x1 ));	
	/*Set input divider*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_pcb_wr_bits(unit, port, pc, 0, PHY84793_DC_HC_BROADCAST, 
                                 0x805e,  11,  10,  rxRefDiv));	
	/* Set /66 divider*/
    SOC_IF_ERROR_RETURN
        (_phy_84793_pcb_wr_bits(unit, port, pc, 0, PHY84793_DC_HC_BROADCAST, 
                                 0x805b, 10, 9, 0x2));	
    /* Set /66 divider*/
    SOC_IF_ERROR_RETURN
        (_phy_84793_pcb_wr_bits( unit, port, pc, 0, PHY84793_DC_HC_BROADCAST, 
                                 0x805a, 14, 12, 0x4));	
    /* Select CTRL bus input*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_pcb_wr_bits( unit, port, pc, 0, PHY84793_DC_HC_BROADCAST, 
                                 0x8059, 9, 9, 0x1));	
    /* PLL Restart*/
	SOC_IF_ERROR_RETURN
        (_phy_84793_pcb_wr_bits( unit, port, pc, 0, 0, 
                                 0x8051, 15, 15, 0x0));		
	SOC_IF_ERROR_RETURN
        (_phy_84793_pcb_wr_bits(unit, port, pc, 0, 0, 
                                0x8051, 15, 15, 0x1));

    return SOC_E_NONE;
}

int32 phy_84793_hw_reset(int32 unit, soc_port_t port, void *not_used)
{
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "84793 HW Reset.....\n")));
   
    /* Hard Reset */
    SOC_IF_ERROR_RETURN
         (WRITE_PHY84793_PMA_PMD_REG(unit, pc, CTL_REGS_COMMON_CTRL1, 0xFFFE));
    
    SOC_IF_ERROR_RETURN
       (_phy_84793_rom_firmware_download (unit, port, 
           PHY84793_FW_DLOAD_OFFSET, pc, 
           phy84793_ucode_bin, phy84793_ucode_bin_len));

	SOC_IF_ERROR_RETURN (_phy_84793_config_update(unit, port));

    return SOC_E_NONE;
}

/*
 * Variable:
 *    phy_84793_drv
 * Purpose:
 *    Phy Driver for 100G PHY.
 */
phy_driver_t phy_84793drv_ce = {
    "84793 100-Gigabit PHY Driver",
    phy_84793_init,                /* Init */
    phy_84793_hw_reset,            /* Reset */
    phy_84793_link_get,            /* Link get   */
    phy_84793_enable_set,          /* Enable set */
    phy_null_enable_get,           /* Enable get */
    phy_null_set,                  /* Duplex set */
    phy_null_one_get,              /* Duplex get */
    phy_84793_speed_set,           /* Speed set  */
    phy_84793_speed_get,           /* Speed get  */
    phy_null_set,                  /* Master set */
    phy_null_zero_get,             /* Master get */
    phy_84793_an_set,              /* ANA set */   /* Return NONE*/
    phy_null_an_get,               /* ANA get */
    NULL,                          /* Local Advert set */
    NULL,                          /* Local Advert get */
    phy_null_mode_get,             /* Remote Advert get */
    phy_84793_lb_set,              /* PHY loopback set */
    phy_84793_lb_get,              /* PHY loopback get */
    phy_84793_interface_set,       /* IO Interface set */
    phy_84793_interface_get,        /* IO Interface get */
    NULL,                          /* PHY abilities mask */
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                          /* phy_cable_diag  */
    NULL,                          /* phy_link_change */
    phy_84793_control_set,         /* phy_control_set */
    phy_84793_control_get,         /* phy_control_get */
    phy_84793_reg_read,            /* phy_reg_read */
    phy_84793_reg_write,           /* phy_reg_write */
    NULL,                          /* phy_reg_modify */
    NULL,                          /* phy_notify */
    phy_84793_probe,               /* pd_probe  */
    phy_84793_ability_advert_set,  /* pd_ability_advert_set */  /* Return NONE*/
    phy_84793_ability_advert_get,  /* pd_ability_advert_get */   /* Return NONE*/
    NULL,                          /* pd_ability_remote_get */
    phy_84793_ability_local_get,   /* pd_ability_local_get  */   /* Return NONE*/
    phy_84793_firmware_set,        /* pd_firmware_set */
    NULL,                          /* pd_timesync_config_set */
    NULL,                          /* pd_timesync_config_get */
    NULL,                          /* pd_timesync_control_set */
    NULL,                          /* pd_timesync_control_get */
    phy_84793_diag_ctrl,            /* .pd_diag_ctrl */
    phy_84793_per_lane_control_set, /* .pd_lane_control_set*/ 
    phy_84793_per_lane_control_get  /* .pd_lane_control_get*/ 
    
};
#else /* INCLUDE_PHY_84793 */
typedef int _phy_84793_not_empty; /* Make ISO compilers happy. */
#endif /*INCLUDE_PHY_84793*/

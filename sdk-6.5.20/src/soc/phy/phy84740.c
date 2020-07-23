/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy84740.c
 * Purpose:    Phys Driver support for Broadcom 84740 40Gig
 *             transceiver :XLAUI system side and PPI Line interface.
 * Note:       
 */
#include "phydefs.h"            /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_84740)
#include "phyconfig.h"          /* Must be the first phy include after phydefs.h */

#include <sal/types.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>
#include <shared/bsl.h>

#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyxehg.h"
#include "phy84740.h"
/*
 * SPI-ROM Program related defs
 */
#define SPI_CTRL_1_L        0xC000
#define SPI_CTRL_1_H        0xC002
#define SPI_CTRL_2_L        0xC400
#define SPI_CTRL_2_H        0xC402
#define SPI_TXFIFO          0xD000
#define SPI_RXFIFO          0xD400
#define WR_CPU_CTRL_REGS    0x11
#define RD_CPU_CTRL_REGS    0xEE
#define WR_CPU_CTRL_FIFO    0x66

/*
 * SPI Controller Commands(Messages).
 */
#define MSGTYPE_HWR          0x40
#define MSGTYPE_HRD          0x80
#define WRSR_OPCODE          0x01
#define WR_OPCODE            0x02
#define WRDI_OPCODE          0x04
#define RDSR_OPCODE          0x05
#define WREN_OPCODE          0x06
#define WR_BLOCK_SIZE        0x40
#define TOTAL_WR_BYTE        0x4000

#define WR_TIMEOUT   1000000

/* CL73 autoneg */
#define AN_ADVERT_40GCR4 (1 << 9)
#define AN_ADVERT_40GKR4 (1 << 8)
#define AN_ADVERT_10GKR  (1 << 7)
#define AN_ADVERT_1GKX   (1 << 5)

/* PRBS */
#define PHY84740_FORTYG_PRBS_PATTERN_TESTING_CONTROL_STATUS         0x0135
#define PHY84740_FORTYG_SQ_WAVE_TESTING_CONTROL_REGISTER            0x05dd
#define PHY84740_UDRCD_PRBS20_CONTROL_REGISTER                      0xcd60
#define PHY84740_FORTYG_PRBS_RX_TESTING_ERROR_COUNTER_LANE0         0x06a4

#define PHY84740_40G_PRBS31             (1 << 7)
#define PHY84740_40G_PRBS9              (1 << 6)

#define PHY84740_40G_SYS_PRBS31             (0x6)
#define PHY84740_40G_SYS_PRBS23             (0x5)
#define PHY84740_40G_SYS_PRBS15             (0x4)
#define PHY84740_40G_SYS_PRBS11             (0x3)
#define PHY84740_40G_SYS_PRBS9              (0x2)
#define PHY84740_40G_SYS_PRBS7              (0x1)
#define PHY84740_40G_SYS_PRBS_POLY_MASK     (0x7007)
#define PHY84740_40G_SYS_PRBS_MASK          (0x7)
#define PHY84740_40G_SYS_PRBS_ENABLE        (0x0005)
#define PHY84740_40G_SYS_PRBS_ENABLE_MASK   (0x0005)
#define PHY84740_40G_SYS_PRBS_LOCK_VALUE    (0x8000)
#define PHY84740_40G_SYS_PRBS_LOCK_MASK     (0xf000)
#define PHY84740_40G_SYS_PRBS_ERR_MASK      (0x0fff)
#define PHY84740_40G_SYS_REP_MODE_ENABLE    (0x6)
#define PHY84740_RX_REPEATER_MASK        (1 << 1)
#define PHY84740_TX_REPEATER_MASK        (1 << 2)
#define PHY84740_TX_REPEATER_ENABLE      (1 << 2)


#define PHY84740_40G_PRBS_TX_ENABLE     (1 << 3)
#define PHY84740_40G_PRBS_RX_ENABLE     (1 << 0)
#define PHY84740_40G_PRBS_ENABLE     (PHY84740_40G_PRBS_RX_ENABLE | \
        PHY84740_40G_PRBS_TX_ENABLE)

#define PHY84740_FORTYG_PRBS_RECEIVE_ERROR_COUNTER_LANE0            0x0140
#define PHY84740_USER_PRBS_CONTROL_0_REGISTER                       0xCD14
#define PHY84740_USER_PRBS_TX_INVERT   (1 << 4)
#define PHY84740_USER_PRBS_RX_INVERT   (1 << 15)
#define PHY84740_USER_PRBS_INVERT      (1 << 4 | 1 << 15)
#define PHY84740_USER_PRBS_ENABLE      (1 << 7)
#define PHY84740_USER_PRBS_TYPE_MASK   (0x7)

#define PHY84740_USER_PRBS_STATUS_0_REGISTER                        0xCD15
#define PHY84740_GENSIG_8071_REGISTER                               0xCD16
#define PHY84740_RESET_CONTROL_REGISTER                             0xCD17

/* Module auto detect status */
#define PHY84740_MOD_AUTO_DETECT_STS_REG                            0xc82b
#define PHY84740_MOD_AUTO_DETECT_STS_NOT_DONE                       (1 << 15)
#define PHY84740_MOD_AUTO_DETECT_STS_SFP_EN                         (1 << 10)
#define PHY84740_MOD_AUTO_DETECT_STS_I2C_FAIL                       (1 << 9)
#define PHY84740_MOD_AUTO_DETECT_STS_NOT_RES                        (1 << 0)

/* Optical config */
#define PHY84740_PMAD_OPTICAL_CFG_MOD_PRESENCE                      (1 << 3)

/* General purpose register */
#define PHY84740_PMAD_GEN_PURPOSE_REG                               0xc710
#define PHY84740_PMAD_GEN_PURPOSE_MOD_AUTO_DETECT_EN                (1 << 2)

/* Polarity config */
#define POL_CONFIG_LANE_WIDTH     0x4
#define POL_CONFIG_LANE_MASK(_ln)   (0xf << ((_ln) * POL_CONFIG_LANE_WIDTH))
#define POL_CONFIG_ALL_LANES    1
/*
 * When a port is being flexed between 40G->10G, 
 * the polarity config syntax can be in
 * 40G syntax for first lane, so there is a need to parse 
 * both 10G and 40G polarity config syntax in 10G.
 */
#define PHY84740_LN(_pc)         ((_pc)->phy_id & 0x3)
#define PHY84740_POL_MASK(_pc)   POL_CONFIG_LANE_MASK(PHY84740_LN(_pc))
#define PHY84740_10G_POL_CFG_GET(_pc, _pol) \
(((_pol) == POL_CONFIG_ALL_LANES) ||  \
(((_pol) & PHY84740_POL_MASK(_pc)) ==  PHY84740_POL_MASK(_pc)))

/* I2C related defines */
#define PHY84740_BSC_XFER_MAX     0x1F9
#define PHY84740_BSC_WR_MAX       16
#define PHY84740_WRITE_START_ADDR 0x8007
#define PHY84740_READ_START_ADDR  0x8007
#define PHY84740_WR_FREQ_400KHZ   0x0100
#define PHY84740_2W_STAT          0x000C
#define PHY84740_2W_STAT_IDLE     0x0000
#define PHY84740_2W_STAT_COMPLETE 0x0004
#define PHY84740_2W_STAT_IN_PRG   0x0008
#define PHY84740_2W_STAT_FAIL     0x000C
#define PHY84740_BSC_WRITE_OP     0x22
#define PHY84740_BSC_READ_OP      0x2
#define PHY84740_I2CDEV_WRITE     0x1
#define PHY84740_I2CDEV_READ      0x0
#define PHY84740_I2C_8BIT         0
#define PHY84740_I2C_16BIT        1
#define PHY84740_I2C_TEMP_RAM     0xE
#define PHY84740_I2C_OP_TYPE(access_type,data_type) \
        ((access_type) | ((data_type) << 8))
#define PHY84740_I2C_ACCESS_TYPE(op_type) ((op_type) & 0xff)
#define PHY84740_I2C_DATA_TYPE(op_type)   (((op_type) >> 8) & 0xff)

/* preemphasis control */
#define PHY84740_PREEMPH_CTRL_POST_TAP2_SHFT 16
#define PHY84740_PREEMP_GET_POST_TAP2(_val)\
        (((_val) >> PHY84740_PREEMPH_CTRL_POST_TAP2_SHFT) & 0x7)
#define PHY84740_PREEMPH_REG_POST_TAP2_MASK (0x7 << PHY84740_PREEMPH_REG_POST_TAP2_SHFT)
#define PHY84740_PREEMPH_REG_POST_TAP2_SHFT  0
#define PHY84740_PREEMPH_CTRL_FORCE_SHFT       15
#define PHY84740_PREEMPH_GET_FORCE(_val)  \
        (((_val) >> PHY84740_PREEMPH_CTRL_FORCE_SHFT) & 1)
#define PHY84740_PREEMPH_CTRL_POST_TAP_SHFT    10
#define PHY84740_PREEMPH_GET_POST_TAP(_val) \
        (((_val) >> PHY84740_PREEMPH_CTRL_POST_TAP_SHFT) & 0x1f)
#define PHY84740_PREEMPH_CTRL_PRE_TAP_SHFT    0
#define PHY84740_PREEMPH_GET_PRE_TAP(_val) \
        (((_val) >> PHY84740_PREEMPH_CTRL_PRE_TAP_SHFT) & 0xf)
#define PHY84740_PREEMPH_CTRL_MAIN_TAP_SHFT    4
#define PHY84740_PREEMPH_GET_MAIN_TAP(_val) \
        (((_val) >> PHY84740_PREEMPH_CTRL_MAIN_TAP_SHFT) & 0x3f)
#define PHY84740_PREEMPH_REG_FORCE_MASK     (1 << 8)
#define PHY84740_PREEMPH_REG_POST_TAP_SHFT  4
#define PHY84740_PREEMPH_REG_POST_TAP_MASK  (0xf << PHY84740_PREEMPH_REG_POST_TAP_SHFT)
#define PHY84740_PREEMPH_REG_PRE_TAP_SHFT  0
#define PHY84740_PREEMPH_REG_PRE_TAP_MASK  (0x7 << PHY84740_PREEMPH_REG_PRE_TAP_SHFT)
#define PHY84740_PREEMPH_REG_MAIN_TAP_SHFT  11
#define PHY84740_PREEMPH_REG_MAIN_TAP_MASK  (0x1f << PHY84740_PREEMPH_REG_MAIN_TAP_SHFT)
#define PHY84740_PREEMPH_REG_TX_PWRDN_MASK  (0x1 << 10)

/* Misc. */
#define PHY84740_AN_SPEED_1G_MASK                 0x11
#define PHY84740_AN_SPEED_10G_MASK                0x44
#define PHY84740_UCODE_DEVID_ADDR              0x7FF9

#define PHY84740_SINGLE_PORT_MODE(_pc)  \
(SOC_INFO((_pc)->unit).port_num_lanes[(_pc)->port] >= 4)
#define PHY84740_EDC_MODE_MASK          0xff
#define PHY84740_ID_84740 0x84740
#define PHY84740_ID_84064 0x84064
#define PHY84740_ID_84754 0x84754
#define PHY84740_ID_84753 0x84753
#define PHY84740_ID_84752 0x84752

/* Other variants */
#define PHY84740_ID_84780 0x84780
#define PHY84740_ID_84784 0x84784
#define PHY84740_ID_84758 0x84758
#define PHY84740_ID_84164 0x84164
#define PHY84740_ID_84168 0x84168
#define PHY84740_ID_84318 0x84318

#define PHY84740_IS_QUADRA(devid)\
((devid) == PHY84740_ID_84780 ||\
 (devid) == PHY84740_ID_84784 ||\
 (devid) == PHY84740_ID_84758 ||\
 (devid) == PHY84740_ID_84164 ||\
 (devid) == PHY84740_ID_84168 ||\
 (devid) == PHY84740_ID_84318)

#define PHY84740_IS_QUADRA_BACKPLANE(devid)\
((devid) == PHY84740_ID_84164 ||\
 (devid) == PHY84740_ID_84168 )

#define NUM_LANES  4

/* typedefs */
typedef struct {
    int rx_los;
    int rx_los_invert;
    int mod_abs;
    int mod_abs_invert;
    int p2l_map[NUM_LANES];     /* index: physical lane, array element: logical lane */
    int pcs_repeater;
    int reverse_mode;
    int mod_auto_detect;
    int mod_auto_detect_msg;
    int software_rx_los;
    int fcmap_passthrough;
    int single_lpmode;
    int skip_fw_dload;
} PHY84740_DEV_DESC_t;

/* device field macros */
#define RX_LOS(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->rx_los)
#define RX_LOS_INVERT(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->rx_los_invert)
#define MOD_ABS(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->mod_abs)
#define MOD_ABS_INVERT(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->mod_abs_invert)
#define P2L_MAP(_pc,_ix) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->p2l_map[(_ix)])
#define PCS_REPEATER(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->pcs_repeater)
#define REVERSE_MODE(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->reverse_mode)
#define MOD_AUTO_DETECT(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->mod_auto_detect)
#define MOD_AUTO_DETECT_MSG(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->mod_auto_detect_msg)
#define SOFTWARE_RX_LOS(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->software_rx_los)
#define FCMAP_PASSTHROUGH(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->fcmap_passthrough)
#define SINGLE_LPMODE(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->single_lpmode)
#define SKIP_FW_DLOAD(_pc) (((PHY84740_DEV_DESC_t *)((_pc) + 1))->skip_fw_dload)

#define PHY84740_FCS_ERROR_WAR

/* callback to deliever the firmware by application 
 * return value: TRUE  successfully delivered
 *               FALSE fail to deliver
 *
 * Example:
 *
 * int user_firmware_send (int unit,int port,int flag,unsigned char **data,int *len)
 * {
 *     *data = phy84740_ucode_bin;
 *     *len  = phy84740_ucode_bin_len;
     LOG_WARN(BSL_LS_SOC_PHY,
              (BSL_META_U(unit,
                          "port %d: user firmware hook routine called\n"),port));
 *     return TRUE;
 * }
 * Then initialize the function pointer before the BCM port initialization: 
 * phy_84740_uc_firmware_hook = user_firmware_send;
 */
int (*phy_84740_uc_firmware_hook) (int unit,    /* switch number */
                                   int port,    /* port number */
                                   int flag,    /* any information need to ba passed. Not used for now */
                                   unsigned char **addr,        /* starting address of delivered firmware */
                                   int *len     /* length of the firmware */
    ) = NULL;

static int write_message(int unit, phy_ctrl_t *pc, uint16 wrdata,
                         uint16 *rddata);

STATIC int phy_84740_speed_get(int, soc_port_t, int *);
STATIC int
_phy_84740_firmware_to_rom(int unit, int port, int offset, uint8 *array,
                           int datalen);
STATIC int _phy84740_mdio_lane_firmware_download(int unit, int port,
                                                 phy_ctrl_t *pc,
                                                 uint8 *new_fw,
                                                 uint32 fw_length);
STATIC void _phy_84740_decode_syncout_mode(uint16 value,
                                           soc_port_phy_timesync_syncout_mode_t
                                           *mode);
STATIC void _phy_84740_encode_syncout_mode(soc_port_phy_timesync_syncout_mode_t
                                           mode, uint16 *value);
STATIC void _phy_84740_decode_framesync_mode(uint16 value,
                                             soc_port_phy_timesync_framesync_mode_t
                                             *mode);
STATIC void
_phy_84740_encode_framesync_mode(soc_port_phy_timesync_framesync_mode_t mode,
                                 uint16 *value);
STATIC void _phy_84740_decode_gmode(uint16 value,
                                    soc_port_phy_timesync_global_mode_t *mode);
STATIC void _phy_84740_encode_gmode(soc_port_phy_timesync_global_mode_t mode,
                                    uint16 *value);
STATIC void _phy_84740_decode_ingress_message_mode(uint16 value, int offset,
                                                   soc_port_phy_timesync_event_message_ingress_mode_t
                                                   *mode);
STATIC void _phy_84740_decode_egress_message_mode(uint16 value, int offset,
                                                  soc_port_phy_timesync_event_message_egress_mode_t
                                                  *mode);

STATIC void
_phy_84740_encode_ingress_message_mode
(soc_port_phy_timesync_event_message_ingress_mode_t mode, int offset, uint16 *value);
STATIC void
_phy_84740_encode_egress_message_mode
(soc_port_phy_timesync_event_message_egress_mode_t mode, int offset, uint16 *value);
STATIC int phy_84740_tx_polarity_flip_set(int unit, int port, uint32 value);
STATIC int phy_84740_rx_polarity_flip_set(int unit, int port, uint32 value);
STATIC int phy_84740_enable_set(int unit, soc_port_t port, int enable);
extern unsigned char phy84740_ucode_bin[];
extern unsigned int phy84740_ucode_bin_len;
extern unsigned char phy84754_ucode_bin[];
extern unsigned int phy84754_ucode_bin_len;
extern unsigned char phy84753_ucode_bin[];
extern unsigned int phy84753_ucode_bin_len;
extern unsigned char phy84064_ucode_bin[];
extern unsigned int phy84064_ucode_bin_len;
extern unsigned char phy84752_ucode_bin[];
extern unsigned int phy84752_ucode_bin_len;
extern unsigned char phy84758_ucode_bin[];
extern unsigned int phy84758_ucode_bin_len;
extern unsigned char phy84780_ucode_bin[];
extern unsigned int phy84780_ucode_bin_len;
extern unsigned char phy84784_ucode_bin[];
extern unsigned int phy84784_ucode_bin_len;
extern unsigned char phy84318_ucode_bin[];
extern unsigned int phy84318_ucode_bin_len;

static char *dev_name_84740 = "BCM84740";
static char *dev_name_84752 = "BCM84752";
static char *dev_name_84753 = "BCM84753";
static char *dev_name_84754 = "BCM84754";
static char *dev_name_84064 = "BCM84064";
static char *dev_name_84784 = "BCM84784";
STATIC int
_phy_84740_config_devid(int unit, soc_port_t port, phy_ctrl_t *pc,
                        uint32 *devid)
{
    uint16 data16;

    if (soc_property_port_get(unit, port, spn_PHY_84740, FALSE)) {
        *devid = PHY84740_ID_84740;
    } else if (soc_property_port_get(unit, port, spn_PHY_84752, FALSE)) {
        *devid = PHY84740_ID_84752;
    } else if (soc_property_port_get(unit, port, spn_PHY_84753, FALSE)) {
        *devid = PHY84740_ID_84753;
    } else if (soc_property_port_get(unit, port, spn_PHY_84754, FALSE)) {
        *devid = PHY84740_ID_84754;
    } else if (soc_property_port_get(unit, port, spn_PHY_84064, FALSE)) {
        *devid = PHY84740_ID_84064;
    } else {

        /* read Other chip id */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc802, &data16));

        if (PHY84740_IS_QUADRA(0x80000 | data16)) {
            *devid = 0x80000 | data16;
        } else {
            *devid = 0;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_single_to_quad_mode(int unit, soc_port_t port, phy_ctrl_t *pc)
{
    int ix;
    uint16 saved_phy_addr;
    uint16 mask16;
    uint16 data16;
    uint32 devid;

    if (pc == NULL) {
        pc = EXT_PHY_SW_STATE(unit, port);
    }

    /* probe routine should block any invalid devids. 
     * devids should be always valid here
     */
    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    mask16 = PHY84740_PMAD_DAC_MODE_MASK | PHY84740_PMAD_BKPLANE_MODE_MASK;
    if (devid == PHY84740_ID_84064) {
        data16 = PHY84740_PMAD_BKPLANE_MODE_MASK;
    } else {
        data16 = 0;
    }

    /* config DAC mode/backplane mode first. This is a bcst register in single mode */
    SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                        (pc->unit, pc, PHY84740_PMAD_CHIP_MODE_REG, data16,
                         mask16));

    /* then configure quad chip mode */
    SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                        (pc->unit, pc, PHY84740_PMAD_CHIP_MODE_REG, 0,
                         PHY84740_PMAD_CHIP_MODE_MASK));

    /* once this mode register is configured with quad mode, each lane is
     * accessable by the lane specific MDIO address
     */
    saved_phy_addr = pc->phy_id;
    /* Reset all 4 lanes. Only after reset, the mode is actually switched */
    for (ix = 0; ix < 4; ix++) {
        pc->phy_id = (saved_phy_addr & (~0x3)) + ix;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_PMA_PMD_CTRLr(pc->unit, pc,
                                           MII_CTRL_RESET, MII_CTRL_RESET));
    }
    pc->phy_id = saved_phy_addr;

    return SOC_E_NONE;
}

STATIC int _phy_84740_quad_to_single_mode(int unit, soc_port_t port)
{
    int ix;
    uint16 saved_phy_addr;
    phy_ctrl_t *pc;
    uint16 mask16;
    uint16 data16;
    uint32 devid;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    saved_phy_addr = pc->phy_id;
    /* configure all four lanes to bcst mode */
    for (ix = 0; ix < 4; ix++) {
        pc->phy_id = (saved_phy_addr & (~0x3)) + ix;
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(pc->unit, pc,
                                            PHY84740_PMAD_BCST_REG, 0xffff));
    }
    pc->phy_id = saved_phy_addr;

    /* then configure single mode */
    SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                        (pc->unit, pc, PHY84740_PMAD_CHIP_MODE_REG,
                         PHY84740_PMAD_MODE_40G, PHY84740_PMAD_CHIP_MODE_MASK));

    mask16 = PHY84740_PMAD_DAC_MODE_MASK | PHY84740_PMAD_BKPLANE_MODE_MASK;
    if (devid == PHY84740_ID_84064) {
        data16 = PHY84740_PMAD_BKPLANE_MODE_MASK;
    } else {
        data16 = 0;
    }

    /* config DAC mode and backplane mode for LR4/SR4 */
    SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                        (pc->unit, pc, PHY84740_PMAD_CHIP_MODE_REG, data16,
                         mask16));

    /* do a soft reset to switch to the configured mode
     * The reset should also clear the bcst register configuration
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_PMA_PMD_CTRLr(pc->unit, pc,
                                       MII_CTRL_RESET, MII_CTRL_RESET));

    return SOC_E_NONE;
}

STATIC int
_phy84740_mdio_firmware_download(int unit, int port, phy_ctrl_t *pc,
                                 uint8 *new_fw, uint32 fw_length)
{
    uint16 data16;
    uint16 mask16;
    int lane;
    int rv = SOC_E_NONE;

    if (new_fw == NULL || fw_length == 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d MDIO firmware download: invalid firmware\n"),
                  unit, port));
        return SOC_E_NONE;
    }

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* point to lane 0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    }

    /* clear SPA ctrl reg bit 15 and bit 13. 
     * bit 15, 0-use MDIO download to SRAM, 1 SPI-ROM download to SRAM 
     * bit 13, 0 clear download done status, 1 skip download 
     */
    mask16 = (1 << 13) | (1 << 15);
    data16 = 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_PMAD_SPA_CTRL_REG,
                                         data16, mask16));

    /* set SPA ctrl reg bit 14, 1 RAM boot, 0 internal ROM boot */
    mask16 = 1 << 14;
    data16 = mask16;
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_PMAD_SPA_CTRL_REG,
                                         data16, mask16));

    /* misc_ctrl1 reg bit 3 to 1 for 32k downloading size */
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        for (lane = 0; lane < NUM_LANES; lane++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_AER_ADDR_REG, lane));
            mask16 = 1 << 3;
            data16 = mask16;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_MISC_CTRL1_REG, data16, mask16));
        }
    } else {
        mask16 = 1 << 3;
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_MISC_CTRL1_REG, data16, mask16));
    }
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* apply bcst Reset to start download code from MDIO */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    }
    SOC_IF_ERROR_RETURN(WRITE_PHY84740_PMA_PMD_CTRLr(unit, pc, 0x8000));

    /* wait for 2ms for M8051 start and 5ms to initialize the RAM */
    sal_usleep(10000);          /* Wait for 10ms */

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        for (lane = 0; lane < NUM_LANES; lane++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_AER_ADDR_REG, lane));
            /* download the firmware to the specific lane */
            rv = _phy84740_mdio_lane_firmware_download(unit, port, pc, new_fw,
                                                       fw_length);
        }

        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy84740_mdio_lane_firmware_download
             (unit, port, pc, new_fw, fw_length));
    }
    return rv;
}

STATIC int
_phy84740_mdio_lane_firmware_download(int unit, int port, phy_ctrl_t *pc,
                                      uint8 *new_fw, uint32 fw_length)
{
    uint16 data16;
    int j;
    uint16 num_words;

    /* Write Starting Address, where the Code will reside in SRAM */
    data16 = 0x8000;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG
         (unit, pc, PHY84740_PMAD_M8051_MSGIN_REG, data16));

    /* make sure address word is read by the micro */
    sal_udelay(10);             /* Wait for 10us */

    /* Write SPI SRAM Count Size */
    data16 = (fw_length) / 2;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG
         (unit, pc, PHY84740_PMAD_M8051_MSGIN_REG, data16));

    /* make sure read by the micro */
    sal_udelay(10);             /* Wait for 10us */

    /* Fill in the SRAM */
    num_words = (fw_length - 1);
    for (j = 0; j < num_words; j += 2) {
        /* Make sure the word is read by the Micro */
        sal_udelay(10);

        data16 = (new_fw[j] << 8) | new_fw[j + 1];
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_M8051_MSGIN_REG, data16));
    }

    /* make sure last code word is read by the micro */
    sal_udelay(20);

    /* Read Hand-Shake message (Done) from Micro */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG
         (unit, pc, PHY84740_PMAD_M8051_MSGOUT_REG, &data16));

    /* Download done message */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "u=%d p=%d MDIO firmware download done message: 0x%x\n"),
              unit, port, data16));

    /* Clear LASI status */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &data16));

    /* Wait for LASI to be asserted when M8051 writes checksum to MSG_OUTr */
    sal_udelay(100);            /* Wait for 100 usecs */

    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG
         (unit, pc, PHY84740_PMAD_M8051_MSGOUT_REG, &data16));

    /* Need to check if checksum is correct */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1C, &data16));

    if (data16 != 0x600D) {
        /* Bad CHECKSUM */
        LOG_CLI((BSL_META_U(unit,
                            "MDIO Firmware downlad failure:"
                 "Incorrect Checksum %x\n"), data16));
        return SOC_E_FAIL;
    }

    /* read Rev-ID */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1A, &data16));
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "u=%d p=%d MDIO Firmware download revID: 0x%x\n"),
                         unit, port, data16));

    return SOC_E_NONE;
}

STATIC int _phy84740_rom_firmware_download(int unit, int port, phy_ctrl_t *pc)
{
    int lane;
    uint16 data16;
    uint16 mask16;
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        for (lane = 3; lane >= 0; lane--) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                lane));

            /* 0xca85[3]=1, 32K download */
            mask16 = (1 << 3);
            data16 = mask16;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, 0xCA85, data16, mask16));
        }

        /* 0xc848[15]=1, SPI-ROM downloading to RAM, 0xc848[14]=1, serial boot */
        mask16 = (1 << 15) | (1 << 14);
        data16 = mask16;

        /* 0xc848[13]=0, SPI-ROM downloading not done, 0xc848[2]=0, spi port enable */
        mask16 |= (1 << 13) | (1 << 2);
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc848, data16, mask16));

        /* apply software reset to download code from SPI-ROM */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_PMA_PMD_CTRLr(unit, pc, MII_CTRL_RESET));

        for (lane = 0; lane < NUM_LANES; lane++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                lane));
            /* wait for at least 50ms */
            sal_usleep(100000);
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "u=%d p=%d lane%d SPI-ROM download done msg 0x%x\n"),
                      unit, port, lane, data16));
            SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc, 0xCA1C, &data16));
            SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc, 0xCA1A, &data16));
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, 0));
    } else {                    /* single lane */
        /* 0xca85[3]=1, 32K download */
        mask16 = (1 << 3);
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA85, data16, mask16));

        /* 0xc848[15]=1, SPI-ROM downloading to RAM, 0xc848[14]=1, serial boot */
        mask16 = (1 << 15) | (1 << 14);
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc848, data16, mask16));

        /* 0xc848[13]=0, SPI-ROM downloading not done, 0xc848[2]=0, spi port enable */
        mask16 = (1 << 13) | (1 << 2);
        data16 = 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc848, data16, mask16));

        /* apply software reset to download code from SPI-ROM */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_PMA_PMD_CTRLr(unit, pc,
                                           MII_CTRL_RESET, MII_CTRL_RESET));

        /* wait for at least 50ms */
        sal_usleep(100000);
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d SPI-ROM download done msg 0x%x\n"),
                             unit, port, data16));

    }
    return SOC_E_NONE;
}

/*
 * Flip the tx/rx polarities based on the configuration.
 */
STATIC int _phy_84740_polarity_flip_set(int unit, soc_port_t port)
{
    int lane;
    phy_ctrl_t *pc;
    uint16 tx_pol, rx_pol;
    uint16 data16, mask16;

    pc = EXT_PHY_SW_STATE(unit, port);

    tx_pol = 0;
    rx_pol = 0;

    /* get the reset default value. The 84740 device has the TX polarity flipped
     * by default. Construct the init default value 
     */

    if ((PHY84740_SINGLE_PORT_MODE(pc))) {
        for (lane = 0; lane < NUM_LANES; lane++) {
            /* point to the each lane starting at lane0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                P2L_MAP(pc, lane)));

            /* read both TX/RX polarities */
            SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc, PHY84740_DIG_CTRL_REG, &data16));
            if (data16 & PHY84740_TX_POLARITY_FLIP_MASK) {
                tx_pol |= POL_CONFIG_LANE_MASK(lane);
            }
            if (data16 & PHY84740_RX_POLARITY_FLIP_MASK) {
                rx_pol |= POL_CONFIG_LANE_MASK(lane);
            }
        }
    } else {                    /* quad mode */
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_DIG_CTRL_REG, &data16));
        if (data16 & PHY84740_TX_POLARITY_FLIP_MASK) {
            tx_pol = POL_CONFIG_ALL_LANES;
        }
        if (data16 & PHY84740_RX_POLARITY_FLIP_MASK) {
            rx_pol = POL_CONFIG_ALL_LANES;
        }
    }

    tx_pol = soc_property_port_get(unit, port,
                                   spn_PHY_PCS_TX_POLARITY_FLIP, tx_pol);
    rx_pol = soc_property_port_get(unit, port,
                                   spn_PHY_PCS_RX_POLARITY_FLIP, rx_pol);

    mask16 = PHY84740_TX_POLARITY_FLIP_MASK | PHY84740_RX_POLARITY_FLIP_MASK;

    if ((PHY84740_SINGLE_PORT_MODE(pc))) {
        for (lane = 0; lane < NUM_LANES; lane++) {
            data16 = 0;

            /* point to the each lane starting at lane0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                P2L_MAP(pc, lane)));

            if ((tx_pol == POL_CONFIG_ALL_LANES) ||
                ((tx_pol & POL_CONFIG_LANE_MASK(lane)) ==
                 POL_CONFIG_LANE_MASK(lane))) {
                data16 = PHY84740_TX_POLARITY_FLIP_MASK;
            }
            if ((rx_pol == POL_CONFIG_ALL_LANES) ||
                ((rx_pol & POL_CONFIG_LANE_MASK(lane)) ==
                 POL_CONFIG_LANE_MASK(lane))) {
                data16 |= PHY84740_RX_POLARITY_FLIP_MASK;
            }
            SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                                PHY84740_DIG_CTRL_REG,
                                                                data16,
                                                                mask16));
        }
        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    } else {                    /* quad-port mode */
        data16 = 0;
        tx_pol = PHY84740_10G_POL_CFG_GET(pc, tx_pol);
        if (tx_pol) {
            data16 = PHY84740_TX_POLARITY_FLIP_MASK;
        }
        rx_pol = PHY84740_10G_POL_CFG_GET(pc, rx_pol);
        if (rx_pol) {
            data16 |= PHY84740_RX_POLARITY_FLIP_MASK;
        }
        SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_DIG_CTRL_REG, data16, mask16));
    }

    return SOC_E_NONE;
}

/*
 * Squelch enable/disable
 */
STATIC int _phy_84740_squelch_enable(int unit, soc_port_t port,
                                     int intf, int enable, int force)
{
    phy_ctrl_t *pc;
    int ix;
    int repeater_mode;
    uint16 data16, mask16,bit,speed_val;
    uint32 devid;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    if (PHY84740_IS_QUADRA(devid)) { 
        if ((PHY_DIAG_INTF_LINE == intf || REVERSE_MODE(pc)) &&
            PHY_DIAG_INTF_SYS != intf) {
            if(PHY84740_SINGLE_PORT_MODE(pc)) {
                mask16 = (1 << 6);
                data16 = enable ? mask16 : 0;
                for (ix = 0; ix < NUM_LANES; ix++) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    ix));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     0xcd18, data16, mask16));
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     0xcd16,
                                                     enable ? (1 << 4) : 0,
                                                     (1 << 4)));
                }
               /* point back to lane0 */
               SOC_IF_ERROR_RETURN
                   (WRITE_PHY84740_MMF_PMA_PMD_REG
                   (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0)); 

            } else {/*quad ports**/
                SOC_IF_ERROR_RETURN
                    (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81d, &data16));
                repeater_mode = ((data16 & 0x6) == 0x6) ? TRUE : FALSE;
                if (repeater_mode) {
                   SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc81f, &speed_val));
                   bit = (speed_val & (1 << 2)) ? 3 : 4; 
                    mask16 = (1 << bit) | (1 << 6);
                    data16 = enable ? mask16 : 0;
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     0xcd18, data16, mask16));
                }                
            }
        } else { /*system side or not reverse mode*/
            if (PHY84740_SINGLE_PORT_MODE(pc)) {
                mask16 = (1 << 6) | (1 << 11);
                data16 = enable ? mask16 : 0;
                for (ix = 0; ix < NUM_LANES; ix++) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                   PHY84740_PMAD_AER_ADDR_REG,
                                                   ix));
                    SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                    SOC_IF_ERROR_RETURN(MODIFY_PHY84740_XFI_PMA_PMD_REG(unit,
                                                                   pc,
                                                                   0xcd18,
                                                                   data16,
                                                                   mask16));
                    SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                }
                 /* point back to lane0 */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG
                    (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
            } else {/* quad port*/
                SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81d, &data16));
                repeater_mode = ((data16 & 0x6) == 0x6) ? TRUE : FALSE;
                if (repeater_mode) {
                    SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc81f, &speed_val));
                    bit = (speed_val & (1 << 2)) ? 3 : 4; 
                    mask16 = (1 << bit) | (1 << 6);
                    data16 = enable ? mask16 : 0;
                    SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                    SOC_IF_ERROR_RETURN(MODIFY_PHY84740_XFI_PMA_PMD_REG(unit, pc,
                                                                  0xcd18,
                                                                  data16,
                                                                  mask16));
                    SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
               }
            }
        }
    } else { 
        if ((PHY_DIAG_INTF_LINE == intf || REVERSE_MODE(pc)) &&
            PHY_DIAG_INTF_SYS != intf) {
            if (PHY84740_SINGLE_PORT_MODE(pc)) {

                /* Note: following line needed as workaround */
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));

                mask16 = (1 << 6);
                data16 = enable ? mask16 : 0;

                for (ix = 0; ix < NUM_LANES; ix++) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    ix));

                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     0xcd18, data16, mask16));

                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     0xcd16,
                                                     enable ?
                                                     (force ?((1 << 3)|(1 << 4))
                                                     :(1 << 4))
                                                     : 0,
                                                     (1 << 3)|(1 << 4)));
               }
               /* point back to lane0 */
               SOC_IF_ERROR_RETURN
                   (WRITE_PHY84740_MMF_PMA_PMD_REG
                   (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     0xcd16,
                                                     force ? (1 << 3) : 0,
                                                     (1 << 3)));
                SOC_IF_ERROR_RETURN
                    (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81d, &data16));

                repeater_mode = ((data16 & 0x6) == 0x6) ? TRUE : FALSE;
                if (repeater_mode) {
                    mask16 = (1 << 4) | (1 << 6);
                    data16 = enable ? mask16 : 0;

                    SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     0xcd18, data16, mask16));
                }
            }

          } else { /*system side or not reverse mode*/
              if (PHY84740_SINGLE_PORT_MODE(pc)) {

                 mask16 = (1 << 6) | (1 << 11);
                 data16 = enable ? mask16 : 0;

                 for (ix = 0; ix < NUM_LANES; ix++) {
                     SOC_IF_ERROR_RETURN
                         (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    ix));

                     SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                     SOC_IF_ERROR_RETURN(MODIFY_PHY84740_XFI_PMA_PMD_REG(unit,
                                                                    pc,
                                                                    0xcd18,
                                                                    data16,
                                                                    mask16));

                     SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                 }
                 /* point back to lane0 */
                 SOC_IF_ERROR_RETURN
                     (WRITE_PHY84740_MMF_PMA_PMD_REG
                     (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
              } else {
                  SOC_IF_ERROR_RETURN
                      (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81d, &data16));

                  repeater_mode = ((data16 & 0x6) == 0x6) ? TRUE : FALSE;
                  if (repeater_mode) {
                      mask16 = (1 << 4) | (1 << 6);
                      data16 = enable ? mask16 : 0;

                      SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                      SOC_IF_ERROR_RETURN(MODIFY_PHY84740_XFI_PMA_PMD_REG(unit, pc,
                                                                    0xcd18,
                                                                    data16,
                                                                    mask16));

                      SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                 }
              }
          }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_firmware_is_downloaded(int unit, soc_port_t port, int ext_rom)
{
    int rv = 0;
    uint16 data16;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (soc_property_port_get(unit, port,spn_PHY_FORCE_FIRMWARE_LOAD, TRUE)) {
        return 0;
    }

    /*
     * To determine whether the firmware has been downloaded, it must
     * pass the following:
     * 1. firmware is in RAM: 1.c848.13==1
     * 2. expected checksum:  1.ca1c==600d
     * 3. version is non-zero:  1.ca1a!=0
     */

    /* 1. Firmware download status */
    rv = READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xC848, &data16);
    if (rv != SOC_E_NONE) {
        return 0;
    }
    if (ext_rom) {
        if ((data16 & 0xa000) != 0xa000) {
            return 0;
        }
    } else if ((data16 & 0xf000) != 0x7000) {
        return 0;
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "FW download status=%x: u=%d p=%d\n"), data16, unit, port));

    /* 2. Expected checksum */
    rv = READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1C, &data16);
    if ((rv != SOC_E_NONE) || (data16 != 0x600D)) {
        return 0;
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "FW checksum=%x: u=%d p=%d\n"), data16, unit, port));

    /* 3. non-zero version */
    rv = READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1A, &data16);
    if ((rv != SOC_E_NONE) || (data16 == 0)) {
        return 0;
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "FW version=%x: u=%d p=%d\n"), data16, unit, port));

    return 1;
}

/* Function:
 *    phy_84740_init
 * Purpose:    
 *    Initialize 84740 phys
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int phy_84740_init(int unit, soc_port_t port)
{
    soc_timeout_t to;
    int rv = SOC_E_NONE;
    uint16 data16, ucode_ver;
    uint16 chip_mode;
    phy_ctrl_t *pc;
    int ix;
    uint32 phy_ext_rom_boot = 0;
    uint16 mask16;
    uint32 devid;
    unsigned char *pdata;
    int data_len;
    uint32 l2p, l2p_map;

    pc = EXT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY84740 init: u=%d p=%d desc=0x%x\n"),
              unit, port, PTR_TO_INT(pc + 1)));
    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    /* initialize default p2l map */
    for (ix = 0; ix < NUM_LANES; ix++) {
        P2L_MAP(pc, ix) = ix;
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
        PCS_REPEATER(pc) = FALSE;
        if (PHY84740_SINGLE_PORT_MODE(pc)) {
            /* Always set repeater flag in single port mode */
            PHY_FLAGS_SET(unit, port,
                          PHY_FLAGS_FIBER | PHY_FLAGS_C45 | PHY_FLAGS_REPEATER);
        } else {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER | PHY_FLAGS_C45);
        }

        FCMAP_PASSTHROUGH(pc) =
            soc_property_port_get(unit, port, spn_PHY_FCMAP_PASSTHROUGH, 0);

        if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84740 init pass1: u=%d p=%d\n"), unit, port));

            /* check if to use the bcst mode for RAM uC download */
            phy_ext_rom_boot = soc_property_port_get(unit, port,
                                                     spn_PHY_EXT_ROM_BOOT, 1);

            SKIP_FW_DLOAD(pc) = _phy_84740_firmware_is_downloaded(unit, port,
                                                                  phy_ext_rom_boot);

            /* 
               if (SKIP_FW_DLOAD(pc)) {
               LOG_CLI((BSL_META_U(unit,
                                   "u=%d, p=%d skipping firmware download\n"),
                        unit, port));
               }
             */

            if (!phy_ext_rom_boot) {
                if (!SKIP_FW_DLOAD(pc)) {
                    pc->flags |= PHYCTRL_MDIO_BCST;
                }
            } else {
                /*
                 * To enable firmware to be downloaded to ports which are out-of-order, 
                 * 1.c848.13 must be set on all ports first. The first port within the core that
                 * has 1.c848.13 clear has control over the SPI-ROM, so later when the 
                 * firmware is actually being downloaded, that port will first clear 1.c848.13 
                 * and obtain control over the SPI-ROM.
                 */
                data16 = (1 << 15) | (1 << 14) | (1 << 13);
                mask16 = data16;
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG
                     (unit, pc, 0xc848, data16, mask16));
            }

            /* configure to quad-port mode to prepare for the broadcast mode */
            SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG(pc->unit, pc,
                                                              PHY84740_PMAD_CHIP_MODE_REG,
                                                              &chip_mode));

            /* switch to quad-port mode if not already in */
            if ((chip_mode & PHY84740_PMAD_CHIP_MODE_MASK) ==
                PHY84740_PMAD_MODE_40G) {
                SOC_IF_ERROR_RETURN(_phy_84740_single_to_quad_mode
                                    (pc->unit, pc->port, pc));
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "PHY84740 init pass1 mode S to Q switch: u=%d p=%d\n"),
                          unit, port));
            }

            /* indicate second pass of init is needed */
            PHYCTRL_INIT_STATE_SET(pc, PHYCTRL_INIT_STATE_PASS2);
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84740 init pass2: u=%d p=%d\n"), unit, port));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_PMAD_CHIP_MODE_REG,
                             &chip_mode));

        /* check the current chip mode */
        if ((chip_mode & PHY84740_PMAD_CHIP_MODE_MASK) ==
            PHY84740_PMAD_MODE_40G) {
            /* 
             * switch to configured mode(quad mode) if current chip mode is single mode 
             * In quad mode, the supported speed modes are 10G and 1G forced, 1G autoneg.
             * However 1G currently is only working with QSFP fiber cable not Cu cable.
             */
            if (!(PHY84740_SINGLE_PORT_MODE(pc))) {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "PHY84740 init pass2 mode switch: u=%d p=%d\n"),
                          unit, port));
                SOC_IF_ERROR_RETURN(_phy_84740_single_to_quad_mode
                                    (unit, port, NULL));

            } else {            /* configured mode is single mode */
                /* if configured mode(single mode) matches current chip mode. Make sure 
                 * it is SR4/LR4. If a optical module is plugged in when the device is 
                 * configured in CR4 mode, it may damage the optical module because of big
                 * voltage sway in CR4 mode. For now, the device is configured in
                 * LR4/SR4 mode by default. If a QSFP Cu is used, EDC mode should be set to
                 * 0x55.  In case there is a need to go to CR4 mode, user can use interface
                 * set function to change to the CR4 mode. In this mode, both autoneg and 
                 * forced 40G speed is supported.
                 */
                /* config DAC mode and backplane mode for LR4/SR4  */
                mask16 =
                    PHY84740_PMAD_DAC_MODE_MASK |
                    PHY84740_PMAD_BKPLANE_MODE_MASK;
                if (devid == PHY84740_ID_84064) {
                    data16 = PHY84740_PMAD_BKPLANE_MODE_MASK;
                } else {
                    data16 = 0;
                }

                SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                    (unit, pc, PHY84740_PMAD_CHIP_MODE_REG,
                                     data16, mask16));

                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_PMA_PMD_CTRLr(unit, pc,
                                                   MII_CTRL_RESET,
                                                   MII_CTRL_RESET));
            }
        } else {                /* current chip mode is quad mode */
            /* switch to configured mode(single mode) if current chip mode is quad mode */
            if (PHY84740_SINGLE_PORT_MODE(pc)) {

                SOC_IF_ERROR_RETURN(_phy_84740_quad_to_single_mode(unit, port));
            } else {            /* configured mode is quad mode */

                /* chip already in configured mode. config DAC/backplane mode */
                mask16 =
                    PHY84740_PMAD_DAC_MODE_MASK |
                    PHY84740_PMAD_BKPLANE_MODE_MASK;
                if (devid == PHY84740_ID_84064) {
                    data16 = PHY84740_PMAD_BKPLANE_MODE_MASK;
                } else {
                    data16 = 0;
                }

                SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                    (unit, pc, PHY84740_PMAD_CHIP_MODE_REG,
                                     data16, mask16));
            }
        }

        /* should be done before reset */
        l2p = soc_property_port_get(unit, port, spn_PHY_LANE0_L2P_MAP, 0);
        if (l2p >= NUM_LANES) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "PHY84740 invalid L2P lane configuration: u=%d p=%d, l2p=%d\n"),
                       unit, port, l2p));
            return SOC_E_PARAM;
        }

        SINGLE_LPMODE(pc) =
            soc_property_port_get(unit, port, spn_PHY_TX_DISABLE_NO_LPMODE, 0);

        /*
         * 1.c701
         *    - Quebec devices specifies the physical channel number designated as
         *      logical channel 0 
         *    - Quadra devices specifies the mapping of all physical channels to 
         *      logical channels 
         */
        data16 = 0;
        l2p_map = 0;
        if (PHY84740_IS_QUADRA(devid)) {
            for (ix = 0; ix < NUM_LANES; ix++) {
                l2p_map |= ((l2p + ix) % NUM_LANES) << (ix * 4);
            }
            data16 = l2p_map;
            mask16 = 0xffff;
            for (ix = 0; ix < NUM_LANES; ix++) {
                P2L_MAP(pc, ((l2p_map >> (ix * 4)) & 0xf)) = ix;
            }
        } else {
            data16 = l2p;
            mask16 = PHY84740_L2P_PHYSICAL_LANE_MASK;

            /* physical to logical lane map, index is physical lane */
            for (ix = 0; ix < NUM_LANES; ix++) {
                P2L_MAP(pc, (l2p + ix) % NUM_LANES) = ix;
            }
        }

        SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_PMAD_LANE0_L2P_REG, data16,
                             mask16));

        /* do a soft reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_PMA_PMD_CTRLr(unit, pc,
                                           MII_CTRL_RESET, MII_CTRL_RESET));

        /* Wait for device to come out of reset */
        soc_timeout_init(&to, 10000, 0);
        while (!soc_timeout_check(&to)) {
            rv = READ_PHY84740_PMA_PMD_CTRLr(unit, pc, &data16);
            if (((data16 & MII_CTRL_RESET) == 0) || SOC_FAILURE(rv)) {
                break;
            }
        }
        if (((data16 & MII_CTRL_RESET) != 0) || SOC_FAILURE(rv)) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84740 reset failed: u=%d p=%d\n"),
                      unit, port));
            return SOC_E_FAIL;
        }

        if (pc->flags & PHYCTRL_UCODE_BCST_DONE) {
            /* ucode is already loaded thru bcst */

            /* Not clearing PHYCTRL_UCODE_BCST_DONE  -->
               This bit can be checked again, in case init is called again,
               to avoid an additional firmware download  */

            phy_ext_rom_boot = 0;
        } else {
            phy_ext_rom_boot = soc_property_port_get(unit, port,
                                                     spn_PHY_EXT_ROM_BOOT, 1);

            if (SKIP_FW_DLOAD(pc)) {
                /* do nothing */
            } else if (phy_ext_rom_boot) {
                SOC_IF_ERROR_RETURN
                    (_phy84740_rom_firmware_download(unit, port, pc));
            } else {            /* MDIO download to RAM */
                /* check if the firmware is delivered by user */
                if (phy_84740_uc_firmware_hook &&
                    phy_84740_uc_firmware_hook(unit, port, devid, &pdata,
                                               &data_len)) {
                    SOC_IF_ERROR_RETURN(_phy84740_mdio_firmware_download
                                        (unit, port, pc, pdata, data_len));
                } else {
                    if (devid == PHY84740_ID_84754) {
                        SOC_IF_ERROR_RETURN
                            (_phy84740_mdio_firmware_download
                             (unit, port, pc, phy84754_ucode_bin,
                              phy84754_ucode_bin_len));
                    } else if (devid == PHY84740_ID_84740) {
                        /* do not use 84753/84754 ucode for 84740. There is a voltage differece */
                        SOC_IF_ERROR_RETURN
                            (_phy84740_mdio_firmware_download
                             (unit, port, pc, phy84740_ucode_bin,
                              phy84740_ucode_bin_len));
                    } else if (devid == PHY84740_ID_84064) {
                        SOC_IF_ERROR_RETURN
                            (_phy84740_mdio_firmware_download
                             (unit, port, pc, phy84064_ucode_bin,
                              phy84064_ucode_bin_len));
                    } else if (devid == PHY84740_ID_84753) {
                        SOC_IF_ERROR_RETURN
                            (_phy84740_mdio_firmware_download
                             (unit, port, pc, phy84753_ucode_bin,
                              phy84753_ucode_bin_len));
                    } else if (devid == PHY84740_ID_84752) {
                        SOC_IF_ERROR_RETURN
                            (_phy84740_mdio_firmware_download
                             (unit, port, pc, phy84752_ucode_bin,
                              phy84752_ucode_bin_len));
                    } else if (devid == PHY84740_ID_84784) {
                        SOC_IF_ERROR_RETURN
                            (_phy84740_mdio_firmware_download
                             (unit, port, pc, phy84784_ucode_bin,
                              phy84784_ucode_bin_len));
                    } else if (devid == PHY84740_ID_84780) {
                        SOC_IF_ERROR_RETURN
                            (_phy84740_mdio_firmware_download
                             (unit, port, pc, phy84780_ucode_bin,
                              phy84780_ucode_bin_len));
                    } else if (devid == PHY84740_ID_84318) {
                        SOC_IF_ERROR_RETURN
                             (_phy84740_mdio_firmware_download(unit,port,pc,phy84318_ucode_bin,
                               phy84318_ucode_bin_len));
                    } else if (PHY84740_IS_QUADRA(devid)) {
                        SOC_IF_ERROR_RETURN
                            (_phy84740_mdio_firmware_download
                             (unit, port, pc, phy84758_ucode_bin,
                              phy84758_ucode_bin_len));
                    } else {
                        LOG_WARN(BSL_LS_SOC_PHY,
                                 (BSL_META_U(unit,
                                             "84740: p=%d unknown device id: 0x%x\n"),
                                             port,devid));
                    }
                }
            }
        }

        /* reset will restart the uC. wait a few ms for checksum to be written */
        sal_udelay(12000);

        REVERSE_MODE(pc) =
            soc_property_port_get(unit, port, spn_PORT_PHY_MODE_REVERSE, 0);
        if (!SOC_WARM_BOOT(unit)) {
            /* Disable Tx */
            SOC_IF_ERROR_RETURN(phy_84740_enable_set(unit, port, FALSE));
        }

        /* read the microcode revision */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xce00, &ucode_ver));

        /* check the completion of the microcode download. */
        if (PHY84740_SINGLE_PORT_MODE(pc)) {
            uint16 cksum[4];
            int ret = SOC_E_NONE;

            for (ix = 0; ix < NUM_LANES; ix++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG
                     (unit, pc, PHY84740_PMAD_AER_ADDR_REG, ix));

                /* check the completion of the microcode download. */
                rv = READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1C,
                                                   &cksum[ix]);
                if (cksum[ix] != 0x600D || SOC_FAILURE(rv)) {
                    ret = SOC_E_FAIL;
                }
            }
            if (ret == SOC_E_FAIL) {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "84740: p=%d SPI-ROM load: Bad Checksum lane 0,1,2,3 (0x%x),"
                                     "(0x%x), (0x%x), (0x%x)\n"), port, cksum[0],
                          cksum[1], cksum[2], cksum[3]));
                return SOC_E_FAIL;
            }
        } else {
            rv = READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1C, &data16);
            if (data16 != 0x600D || SOC_FAILURE(rv)) {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "84740: p=%d SPI-ROM load: Bad Checksum (0x%x)\n"),
                          port, data16));
                return SOC_E_FAIL;
            }
        }

        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "%x init u=%d port=%d rom code ver. 0x%x: %s boot\n"),
                  devid, unit, port, ucode_ver,
                  phy_ext_rom_boot ? "SPI-ROM" : "MDIO_TO_RAM"));

        if (!(PHY84740_SINGLE_PORT_MODE(pc))) {
            /* clear 1.0xcd17 to enable the PCS */
            SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                               PHY84740_RESET_CONTROL_REGISTER,
                                                               0));
        } else {
            /* 40G no autoneg support */
            /* EDC mode for SR4/LR4 0x44, done in ucode, no need to set for CR4 */
            /* disable cl72 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x0096, 0x0000));
            /* disable AN */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG(unit, pc, AN_CTRL_REG, 0x0));
        }


        mask16 = PHY84740_RXLOS_OVERRIDE_MASK | PHY84740_MOD_ABS_OVERRIDE_MASK;
        data16 = 0;

        RX_LOS(pc) = soc_property_port_get(unit, port, spn_PHY_RX_LOS, 0);
        MOD_ABS(pc) = soc_property_port_get(unit, port, spn_PHY_MOD_ABS, 0);
        RX_LOS_INVERT(pc) =
            soc_property_port_get(unit, port, spn_PHY_RX_LOS_INVERT, 0);
        MOD_ABS_INVERT(pc) =
            soc_property_port_get(unit, port, spn_PHY_MOD_ABS_INVERT, 0);
        MOD_AUTO_DETECT(pc) =
            soc_property_port_get(unit, port, spn_PHY_MOD_AUTO_DETECT, 0);

        /* default: override RX_LOS signalling */
        if (!RX_LOS(pc)) {
            data16 |= PHY84740_RXLOS_OVERRIDE_MASK;
        }
        /* default: override MOD_ABS signalling */
        if (!MOD_ABS(pc)) {
            data16 |= PHY84740_MOD_ABS_OVERRIDE_MASK;
        }

        if (PHY84740_SINGLE_PORT_MODE(pc)) {
            for (ix = 0; ix < NUM_LANES; ix++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    ix));
                SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                    (unit, pc, PHY84740_PMAD_OPTICAL_CFG_REG,
                                     data16, mask16));
            }
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                 PHY84740_PMAD_OPTICAL_CFG_REG,
                                                 data16, mask16));
        }

        mask16 = PHY84740_RXLOS_LVL_MASK | PHY84740_MOD_ABS_LVL_MASK;
        data16 = 0;
        /* default: RX_LOS level high */
        if (!RX_LOS_INVERT(pc)) {
            data16 |= PHY84740_RXLOS_LVL_MASK;
        }
        /* default: MOD_ABS level high */
        if (!MOD_ABS_INVERT(pc)) {
            data16 |= PHY84740_MOD_ABS_LVL_MASK;
        }

        if (PHY84740_SINGLE_PORT_MODE(pc)) {
            for (ix = 0; ix < NUM_LANES; ix++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    ix));
                SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                    (unit, pc,
                                     PHY84740_PMAD_OPTICAL_SIG_LVL_REG, data16,
                                     mask16));
            }
            /* point back to lane0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                 PHY84740_PMAD_OPTICAL_SIG_LVL_REG,
                                                 data16, mask16));
        }

        if (soc_property_port_get(unit, port, spn_PHY_PCS_REPEATER, 0)) {
            if (!PHY84740_SINGLE_PORT_MODE(pc)) {
                /* put device in repeat mode. Single port is already in repeat mode */
                SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                    (unit, pc, 0xc806, 0x6, 0x6));
                /* in repeater mode, 84740 will use autoneg from internal serdes. But
                 * 84064 will still use cl73/cl72 from ifself.
                 */
                if (devid != PHY84740_ID_84064) {
                    PCS_REPEATER(pc) = TRUE;
                    PHY_FLAGS_SET(unit, port,
                                  PHY_FLAGS_FIBER | PHY_FLAGS_C45 |
                                  PHY_FLAGS_REPEATER);
                }
            }
        }

        SOC_IF_ERROR_RETURN(_phy_84740_squelch_enable(unit, port, -1, TRUE, FALSE));

        if (FCMAP_PASSTHROUGH(pc)) {
            if ((!PHY84740_SINGLE_PORT_MODE(pc)) && (PHY84740_IS_QUADRA(devid))) {
                /* put device in the fcmap passthrough mode. */
                SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                    (unit, pc, PHY84740_PMAD_CHIP_MODE_REG, 0x3,
                                     0x3));
                SOC_IF_ERROR_RETURN(MODIFY_PHY84740_PMA_PMD_CTRLr
                                    (unit, pc, MII_CTRL_RESET, MII_CTRL_RESET));
            } else {
                FCMAP_PASSTHROUGH(pc) = FALSE;
            }
        }

        /* Polarity configuration */
        SOC_IF_ERROR_RETURN(_phy_84740_polarity_flip_set(unit, port));

        /* check if module auto detect enabled */
        if (MOD_AUTO_DETECT(pc)) {
            if ((ucode_ver & 0x0fff) < 0x105) {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84740 module auto detect requires ucode ver 0x105 or newer: "
                                      "u=%d p%d\n"), unit, port));
            } else if (!(RX_LOS(pc) && MOD_ABS(pc))) {
                MOD_AUTO_DETECT(pc) = 0;
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "84740 module auto detect requires mod_abs and rx_lost "
                                      "to be configured: u=%d p%d\n"), unit, port));
            } else if (PHY84740_SINGLE_PORT_MODE(pc)) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "PHY84740 QSFP module auto detect enabled: u=%d p=%d\n"),
                          unit, port));
                rv = MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     PHY84740_PMAD_GEN_PURPOSE_REG,
                                                     PHY84740_PMAD_GEN_PURPOSE_MOD_AUTO_DETECT_EN,
                                                     PHY84740_PMAD_GEN_PURPOSE_MOD_AUTO_DETECT_EN);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_PHY,
                              (BSL_META_U(unit,
                                          "PHY84740 setting QSP module auto detect failed: u=%d p%d\n"),
                               unit, port));
                } else {
                    MOD_AUTO_DETECT_MSG(pc) = 1;
                }
            } else if (devid == PHY84740_ID_84754) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "PHY84740 SFP module auto detect on: u=%d p=%d\n"),
                          unit, port));
                rv = MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     PHY84740_MOD_AUTO_DETECT_STS_REG,
                                                     PHY84740_MOD_AUTO_DETECT_STS_SFP_EN,
                                                     PHY84740_MOD_AUTO_DETECT_STS_SFP_EN);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_PHY,
                              (BSL_META_U(unit,
                                          "PHY84740 setting SFP module auto detect failed: u=%d p%d\n"),
                               unit, port));
                } else {
                    MOD_AUTO_DETECT_MSG(pc) = 1;
                }
            } else {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "PHY84740 module auto detect not supported: u=%d p%d\n"),
                           unit, port));
            }
        }
    }

    if (PHY84740_IS_QUADRA_BACKPLANE(devid)) {
        mask16 = PHY84740_PMAD_DAC_MODE_MASK | PHY84740_PMAD_BKPLANE_MODE_MASK;
        data16 = PHY84740_PMAD_BKPLANE_MODE_MASK;
        SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_PMAD_CHIP_MODE_REG, data16,
                             mask16));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_an_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int phy_84740_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t *pc;
    uint16 an_status;
    uint32 devid;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (FCMAP_PASSTHROUGH(pc)) {
        *an = TRUE;
        *an_done = TRUE;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                        (unit, pc, PHY84740_PMAD_INTR_VALUE_REG, &an_status));
    *an_done = (an_status & PHY84740_AUTONEG_COMPLETE) ? TRUE : FALSE;

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_AN_REG(unit, pc, AN_CTRL_REG, &an_status));
        *an = (an_status & PHY84740_AN_ENABLE) ? TRUE : FALSE;
    } else {
        SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                            (pc->unit, pc->port, pc, &devid));
        if (PHY84740_IS_QUADRA_BACKPLANE(devid)) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_AN_REG(unit, pc, AN_CTRL_REG, &an_status));

        } else {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_AN_REG(unit, pc, 0xFFE0, &an_status));
        }
        *an = (an_status & PHY84740_AN_ENABLE) ? TRUE : FALSE;

        /* Confirmed value from 7.0x8309[5]  */
        if (*an == FALSE) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_AN_REG(unit, pc, 0x8309, &an_status));
            *an = (an_status & (1 << 5)) ?  FALSE : TRUE ;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_an_set
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
STATIC int phy_84740_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t *pc;
    phy_ctrl_t *int_pc;
    uint16 mode = 0;
    uint32 devid = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_an_set: u=%d p=%d an=%d\n"),
                         unit, port, an));
    /* make sure internal autoneg is turned off always */
    int_pc = INT_PHY_SW_STATE(unit, port);

    if (FCMAP_PASSTHROUGH(pc)) {
        if (NULL != int_pc) {
            SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_SET
                                (int_pc->pd, unit, port, FALSE));
        }
        return SOC_E_NONE;
    }

    if (REVERSE_MODE(pc) && IS_HG_PORT(unit, port)) {
        /* Both WC and external phy need to set AN off @ 10G or 40G */
        an = 0;
        if (NULL != int_pc) {
            if (PHY84740_SINGLE_PORT_MODE(pc)) {
                /* 
                 * Internal Serdes needs to be in forced KR4 (a misnomer), meaning
                 * DFE is enabled but cl73 (or cl37) are turned off. This enables
                 * the equalizer as required for KR backplane interfaces
                 */
                SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_SET
                                    (int_pc->pd, unit, port, 0));
                SOC_IF_ERROR_RETURN(PHY_INTERFACE_SET
                                    (int_pc->pd, unit, port, SOC_PORT_IF_KR4));
                SOC_IF_ERROR_RETURN(PHY_SPEED_SET
                                    (int_pc->pd, unit, port, 40000));

                /* disable cl72: 1.96=0 (device 1, register 0x96) */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x0096, 0));
                /* disable AN: 7.0=0 (device 7, register 0) */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_AN_REG(unit, pc, AN_CTRL_REG, 0));
            }
        }
    }

    if (NULL != int_pc) {
        if (REVERSE_MODE(pc)) {
            SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_SET
                                (int_pc->pd, unit, port, an));
        } else {
            /* if in passthru mode, enable autoneg on internal serdes */
            if (PCS_REPEATER(pc)) {
                SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_SET
                                    (int_pc->pd, unit, port, an));
            } else {
                SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_SET
                                    (int_pc->pd, unit, port, FALSE));
            }
        }
    }

    /* 0xc700(0xc710 once firmware fixed), PCS link status overide for CR4 autoneg.
     * bit 4 enable override, bit 1 status indication
     * system side link status check in 40G 0xcd04, this is per lane register 
     */
    SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                        (unit, pc, PHY84740_PMAD_CHIP_MODE_REG, &mode));

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* make sure internal speed is set to 40G */
        if (an) {
            if (NULL != int_pc) {
                if (!REVERSE_MODE(pc)) {
                    if (soc_property_port_get(unit, port, spn_PHY_LONG_XFI, 0)) {
                        PHY_INTERFACE_SET(int_pc->pd, unit, port,
                                          SOC_PORT_IF_CR);
                    }
                    SOC_IF_ERROR_RETURN(PHY_SPEED_SET
                                        (int_pc->pd, unit, port, 40000));
                }
            }
        }

        /* autoneg only supported in CR4 mode */
        /* no autoneg if in SR4/LR4 mode */
        if (!(mode &
              (PHY84740_PMAD_DAC_MODE_MASK |
               PHY84740_PMAD_BKPLANE_MODE_MASK)) ||
               (REVERSE_MODE(pc) && IS_HG_PORT(unit, port))) {
            an = 0;
        }

        /* enable/disable cl72 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x0096, an ? 2 : 0));
        /* enable/disable AN */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_AN_REG(unit, pc, AN_CTRL_REG,
                                       an ? AN_ENABLE | AN_RESTART : 0x0));

        return SOC_E_NONE;
    } else {                    /* quad mode, cl73 for backplane */
        if (mode & PHY84740_PMAD_BKPLANE_MODE_MASK) {
            /* make sure internal speed is set to 10G */
            if (an) {
                if (NULL != int_pc) {
                    if (soc_property_port_get(unit, port, spn_PHY_LONG_XFI, 0)) {
                        PHY_INTERFACE_SET(int_pc->pd, unit, port,
                                          SOC_PORT_IF_CR);
                    }
                    SOC_IF_ERROR_RETURN(PHY_SPEED_SET
                                        (int_pc->pd, unit, port, 10000));
                }
            }

            /* enable/disable cl72 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x0096, an ? 2 : 0));
            /* enable/disable AN */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG(unit, pc, AN_CTRL_REG,
                                           an ? AN_ENABLE | AN_RESTART : 0x0));
        }
    }

    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    if (an) {
        /* Enable Clause 37 AN */
        if (!(PHY84740_IS_QUADRA_BACKPLANE(devid))) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG
                 (unit, pc, 0xFFE0, MII_CTRL_AE | MII_CTRL_RAN));
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_AN_REG(unit, pc, 0x8309, 0, (1 << 5)));
    } else {
        /* disable Clause 37 AN */
        if (!(PHY84740_IS_QUADRA_BACKPLANE(devid))) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG(unit, pc, 0xFFE0, 0x0));
        }
        /* Enable encoded forced speed derived from ieeeControl1 and ieeeControl2 */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_AN_REG(unit, pc, 0x8309, (1 << 5), (1 << 5)));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_84740_ability_advert_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    uint16 an_adv;
    soc_port_mode_t mode;
    phy_ctrl_t *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    if (FCMAP_PASSTHROUGH(pc)) {
        ability->speed_full_duplex = SOC_PA_SPEED_10GB;
        ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        return SOC_E_NONE;
    }

    if (PCS_REPEATER(pc)) {
        phy_ctrl_t *int_pc = INT_PHY_SW_STATE(unit, port);
        if (NULL != int_pc) {
            SOC_IF_ERROR_RETURN(PHY_ABILITY_ADVERT_GET
                                (int_pc->pd, unit, port, ability));
        }
        return SOC_E_NONE;
    }

    mode = 0;
    ability->pause = 0;
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* advert register 1 */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_AN_REG(unit, pc, AN_ADVERT_1_REG, &an_adv));

        mode |= (an_adv & AN_ADVERT_40GCR4) ? SOC_PA_SPEED_40GB : 0;
        mode |= (an_adv & AN_ADVERT_40GKR4) ? SOC_PA_SPEED_40GB : 0;
        mode |= (an_adv & AN_ADVERT_10GKR) ? SOC_PA_SPEED_10GB : 0;
        mode |= (an_adv & AN_ADVERT_1GKX) ? SOC_PA_SPEED_1000MB : 0;
        ability->speed_full_duplex = mode;

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_AN_REG(unit, pc, AN_ADVERT_0_REG, &an_adv));

        switch (an_adv & (AN_ADVERT_PAUSE | AN_ADVERT_PAUSE_ASYM)) {
            case AN_ADVERT_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case AN_ADVERT_PAUSE_ASYM:
                ability->pause = SOC_PA_PAUSE_TX;
                break;
            case AN_ADVERT_PAUSE | AN_ADVERT_PAUSE_ASYM:
                ability->pause = SOC_PA_PAUSE_RX;
                break;
        }
    } else {
        /* check Full Duplex advertisement on Clause 37 */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_AN_REG(unit, pc, 0xFFE4, &an_adv));
        mode |= (an_adv & MII_ANA_C37_FD) ? SOC_PA_SPEED_1000MB : 0;

        ability->speed_full_duplex = mode;

        switch (an_adv & (MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE)) {
            case MII_ANA_C37_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANA_C37_ASYM_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX;
                break;
            case MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE:
                ability->pause = SOC_PA_PAUSE_RX;
                break;
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_ability_advert_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    return SOC_E_NONE;
}

/*
* Function:
*      phy_84740_ability_remote_get
* Purpose:
*      Get the device's complete abilities.
* Parameters:
*      unit - StrataSwitch unit #.
*      port - StrataSwitch port #.
*      ability - return device's abilities.
* Returns:
*      SOC_E_XXX
*/

STATIC int
phy_84740_ability_remote_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    uint16 lp_abil;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (FCMAP_PASSTHROUGH(pc)) {
        ability->speed_full_duplex = SOC_PA_SPEED_10GB;
        ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        return SOC_E_NONE;
    }

    if (PCS_REPEATER(pc)) {
        phy_ctrl_t *int_pc = INT_PHY_SW_STATE(unit, port);
        if (NULL != int_pc) {
            SOC_IF_ERROR_RETURN(PHY_ABILITY_REMOTE_GET
                                (int_pc->pd, unit, port, ability));
        }
        return SOC_E_NONE;
    }

    ability->speed_half_duplex = SOC_PA_ABILITY_NONE;

    /* check CL73 first */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_AN_REG(unit, pc, AN_LP_ABILITY_1_REG, &lp_abil));
    ability->speed_full_duplex =
        (lp_abil & AN_ADVERT_40GCR4) ? SOC_PA_SPEED_40GB : 0;
    ability->speed_full_duplex |=
        (lp_abil & AN_ADVERT_40GKR4) ? SOC_PA_SPEED_40GB : 0;
    ability->speed_full_duplex |=
        (lp_abil & AN_ADVERT_10GKR) ? SOC_PA_SPEED_10GB : 0;
    ability->speed_full_duplex |=
        (lp_abil & AN_ADVERT_1GKX) ? SOC_PA_SPEED_1000MB : 0;

    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_AN_REG(unit, pc, AN_LP_ABILITY_0_REG, &lp_abil));

    ability->pause = 0;
    switch (lp_abil & (AN_ADVERT_PAUSE | AN_ADVERT_PAUSE_ASYM)) {
        case AN_ADVERT_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case AN_ADVERT_PAUSE_ASYM:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case AN_ADVERT_PAUSE | AN_ADVERT_PAUSE_ASYM:
            ability->pause = SOC_PA_PAUSE_RX;
            break;
    }

    /* check CL37 */
    SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_AN_REG(unit, pc, 0xFFE5, &lp_abil));
    ability->speed_full_duplex |= (lp_abil & MII_ANA_C37_FD) ?
        SOC_PA_SPEED_1000MB : 0;

    switch (lp_abil & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
        case MII_ANP_C37_PAUSE:
            ability->pause |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANP_C37_ASYM_PAUSE:
            ability->pause |= SOC_PA_PAUSE_TX;
            break;
        case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
            ability->pause |= SOC_PA_PAUSE_RX;
            break;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_ability_remote_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_84740_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
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

STATIC int
phy_84740_ability_advert_set(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    uint16 an_adv, an_adv_cl37, mode;
    phy_ctrl_t *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    if (FCMAP_PASSTHROUGH(pc)) {
        return SOC_E_NONE;
    }

    if (PCS_REPEATER(pc)) {
        phy_ctrl_t *int_pc = INT_PHY_SW_STATE(unit, port);
        if (NULL != int_pc) {
            SOC_IF_ERROR_RETURN(PHY_ABILITY_ADVERT_SET
                                (int_pc->pd, unit, port, ability));
        }
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                        (unit, pc, PHY84740_PMAD_CHIP_MODE_REG, &mode));

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        if (mode & PHY84740_PMAD_BKPLANE_MODE_MASK) {
            an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_40GB) ?
                AN_ADVERT_40GKR4 : 0;
            an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ?
                AN_ADVERT_10GKR : 0;
        } else {

            /* only advertise 40G CR4 */
            an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_40GB) ?
                AN_ADVERT_40GCR4 : 0;
        }

        /* CL73 advert register 1 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_AN_REG(unit, pc, AN_ADVERT_1_REG, an_adv));

    } else {                    /* quad-port mode */
        if (mode & PHY84740_PMAD_BKPLANE_MODE_MASK) {
            an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ?
                AN_ADVERT_10GKR : 0;
            an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ?
                AN_ADVERT_1GKX : 0;

            /* CL73 advert register 1 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG(unit, pc, AN_ADVERT_1_REG, an_adv));
        }

        /* only 1G CL37 autoneg */
        an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ?
            AN_ADVERT_1G : 0;

        /* write Full Duplex advertisement on Clause 37 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_AN_REG(unit, pc, 0xFFE4, an_adv));
    }

    an_adv = an_adv_cl37 = 0;
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
        case SOC_PA_PAUSE_TX:
            an_adv = AN_ADVERT_PAUSE_ASYM;
            an_adv_cl37 = MII_ANA_C37_ASYM_PAUSE;
            break;
        case SOC_PA_PAUSE_RX:
            an_adv = AN_ADVERT_PAUSE_ASYM | AN_ADVERT_PAUSE;
            an_adv_cl37 = MII_ANA_C37_ASYM_PAUSE | MII_ANA_C37_PAUSE;
            break;
        case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
            an_adv_cl37 = MII_ANA_C37_PAUSE;
            an_adv = AN_ADVERT_PAUSE;
            break;
    }
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_AN_REG(unit, pc, AN_ADVERT_0_REG, an_adv,
                                        AN_ADVERT_PAUSE |
                                        AN_ADVERT_PAUSE_ASYM));
    } else {
        if (mode & PHY84740_PMAD_BKPLANE_MODE_MASK) {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_AN_REG(unit, pc, AN_ADVERT_0_REG, an_adv,
                                            AN_ADVERT_PAUSE |
                                            AN_ADVERT_PAUSE_ASYM));
        }

        /* write Pause advertisement on Clause 37 */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_AN_REG(unit, pc, 0xFFE4, an_adv_cl37,
                                        MII_ANA_C37_ASYM_PAUSE |
                                        MII_ANA_C37_PAUSE));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_8072_ability_advert_set: u=%d p=%d pause=0x%08x adv_reg1=0x%04x\n"),
              unit, port, ability->pause, an_adv));
        
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_ability_local_get
 * Purpose:
 *      Get the device's complete abilities.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ability - return device's abilities.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_84740_ability_local_get(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    phy_ctrl_t *int_pc;
    uint32 pa_speed = 0;
    
    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_ability_local_get: u=%d p=%d\n"),
              unit, port));
                                                                               
    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    if (FCMAP_PASSTHROUGH(pc)) {
        ability->speed_full_duplex = SOC_PA_SPEED_10GB;
        ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        return SOC_E_NONE;
    }

    if (NULL != int_pc && PCS_REPEATER(pc)) {
        SOC_IF_ERROR_RETURN(PHY_ABILITY_LOCAL_GET
                            (int_pc->pd, unit, port, ability));
        return SOC_E_NONE;
    }
    ability->speed_half_duplex = SOC_PA_ABILITY_NONE;

    pa_speed = PHY84740_SINGLE_PORT_MODE(pc) ? SOC_PA_SPEED_40GB : 
                SOC_PA_SPEED_10GB | SOC_PA_SPEED_1000MB;
    ability->speed_full_duplex = pa_speed;
    
    ability->pause = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium = SOC_PA_MEDIUM_FIBER;
    ability->loopback = SOC_PA_LB_PHY;
    ability->flags = SOC_PA_AUTONEG;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_ability_local_get: u=%d p=%d speed=0x%x\n"),
              unit, port, ability->speed_full_duplex));

    return (SOC_E_NONE);
}

STATIC int _phy84740_mod_auto_detect_update(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    uint16 data;
    uint16 status;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* point to lane 0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    }

    /* check for presence */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                       PHY84740_PMAD_OPTICAL_CFG_REG, &data));
    if (data & PHY84740_PMAD_OPTICAL_CFG_MOD_PRESENCE) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                           PHY84740_MOD_AUTO_DETECT_STS_REG,
                                           &status));
        /* make sure auto detection completed */
        if (!(status & PHY84740_MOD_AUTO_DETECT_STS_NOT_DONE)) {
            if (MOD_AUTO_DETECT_MSG(pc)) {
                /* present and must print message */
                MOD_AUTO_DETECT_MSG(pc) = 0;
                if (status & PHY84740_MOD_AUTO_DETECT_STS_NOT_RES) {
                    uint16 mod_130 = 0, mod_131 = 0;
                    (void)(READ_PHY84740_MMF_PMA_PMD_REG
                           (unit, pc, 0x81f2, &mod_130));
                    (void)(READ_PHY84740_MMF_PMA_PMD_REG
                           (unit, pc, 0x81f3, &mod_131));
                    LOG_ERROR(BSL_LS_SOC_PHY,
                              (BSL_META_U(unit,
                                          "PHY84740 could not detect module, "
                                          "defaulting to SR(4): u=%d p=%d sts=0x%x (0x%x:0x%x)\n"),
                               unit, port, status, mod_130, mod_131));
                } else {
                    /* auto detect done and successfully resolved */
                    SOC_IF_ERROR_RETURN
                        (READ_PHY84740_MMF_PMA_PMD_REG
                         (unit, pc, PHY84740_PMAD_CHIP_MODE_REG, &data));
                    LOG_VERBOSE(BSL_LS_SOC_PHY,
                              (BSL_META_U(unit,
                                          "PHY84740 %s module detected: u=%d p=%d\n"),
                               (data & PHY84740_PMAD_DAC_MODE_MASK) ?
                               "CR4" : "SR4", unit, port));
                }
            }
        } else if (!(status & PHY84740_MOD_AUTO_DETECT_STS_I2C_FAIL) &&
                   (MOD_AUTO_DETECT_MSG(pc))) {
            uint16 mod_130 = 0, mod_131 = 0;
            (void)(READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x81f2, &mod_130));
            (void)(READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x81f3, &mod_131));
            MOD_AUTO_DETECT_MSG(pc) = 0;
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "PHY84740 i2c failed while detecting module, "
                                  "defaulting to SR(4): u=%d p=%d sts=0x%x (0x%x:0x%x)\n"),
                       unit, port, status, mod_130, mod_131));
        }
    } else {
        if (!MOD_AUTO_DETECT_MSG(pc)) {
            /* module not present */
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "PHY84740 module removed u=%d p=%d\n"),
                       unit, port));
            MOD_AUTO_DETECT_MSG(pc) = 1;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84740_link_get
 * Purpose:
 *    Get layer2 connection status.
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:    
 *    SOC_E_NONE
 */
STATIC int phy_84740_link_get(int unit, soc_port_t port, int *link)
{
    uint16 pma_mii_stat, pcs_mii_stat, link_stat;
    phy_ctrl_t *pc;
    phy_ctrl_t *int_pc;
    int cur_speed = 0;
    int an = 0, an_done = 0;
    uint16 speed_val;
    uint32 devid;
    int rv;

    if (link == NULL) {
        return SOC_E_NONE;
    }

    if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    if (!REVERSE_MODE(pc)) {
        if (PCS_REPEATER(pc)) {
            rv = PHY_LINK_GET(int_pc->pd, unit, port, link);
            return rv;
        }
        if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_REPEATER)) {
            rv = PHY_LINK_GET(int_pc->pd, unit, port, link);
        }
    }
    if (FCMAP_PASSTHROUGH(pc)) {
        rv = PHY_LINK_GET(int_pc->pd, unit, port, link);
        return rv;
    }

    phy_84740_an_get(unit, port, &an, &an_done);

    /* only supports 40G. PHY PCS is not used. Check PMD link only  */
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        int serdes_link = TRUE;

        if (REVERSE_MODE(pc)) {
            SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_PMA_PMD_STATr(unit, pc, &pma_mii_stat));
        *link = pma_mii_stat == 0xffff ? FALSE :
            ((pma_mii_stat & MII_STAT_LA) ? TRUE : FALSE);

        if (REVERSE_MODE(pc)) {
            SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
        }

        if ((NULL != int_pc) && an) {
            PHY_LINK_GET(int_pc->pd, unit, port, &serdes_link);
        }

        if (MOD_AUTO_DETECT(pc)) {
            _phy84740_mod_auto_detect_update(unit, port);
        }

        *link = (*link) && serdes_link;
        return SOC_E_NONE;
    }

    /* return link false if in the middle of autoneg */
    if (an == TRUE && an_done == FALSE) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    /* AN only supports 1G */
    if (!an) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_PMAD_CTRL2_REG,
                                           &speed_val));
        if ((speed_val & PHY84740_PMAD_CTRL2r_PMA_TYPE_MASK) !=
            PHY84740_PMAD_CTRL2r_PMA_TYPE_1G_KX) {
            cur_speed = 10000;
        }
    } else {
        if (PHY84740_IS_QUADRA(devid)) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_SPEED_LINK_DETECT_STAT_REG, &speed_val));

            if (speed_val & PHY84740_AN_SPEED_1G_MASK) {
                cur_speed = 1000;
            } else if (speed_val & PHY84740_AN_SPEED_10G_MASK) {
                cur_speed = 10000;
            } else {
                cur_speed = 1000;
            }
        } else {
            cur_speed = 1000;
        }
    }

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
    }

    if (cur_speed == 10000) {   /* check all 3 device's link status if 10G */

        /* MMF Link status */
        /* Read Status 2 register to get rid of Local faults */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_PMAD_STAT2_REG,
                                           &pma_mii_stat));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_PMAD_STAT2_REG,
                                           &pma_mii_stat));
        /* Receive Link status */
        SOC_IF_ERROR_RETURN(READ_PHY84740_PCS_STATr(unit, pc, &pcs_mii_stat));

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_PMA_PMD_STATr(unit, pc, &pma_mii_stat));

        /* MMF link status */
        link_stat = pma_mii_stat & pcs_mii_stat;
        *link = link_stat == 0xffff ? FALSE :
            ((link_stat & MII_STAT_LA) ? TRUE : FALSE);
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_AN_REG(unit, pc, 0xffe1, &link_stat));
        *link = link_stat == 0xffff ? FALSE :
            ((link_stat & MII_STAT_LA) ? TRUE : FALSE);
    }

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
    }

	if (MOD_AUTO_DETECT(pc)) {
		_phy84740_mod_auto_detect_update(unit, port);
	}
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "phy_84740_link_get: u=%d port%d: link:%s\n"),
                 unit, port, *link ? "Up": "Down"));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84740_enable_set
 * Purpose:
 *    Enable/Disable phy 
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int phy_84740_enable_set(int unit, soc_port_t port, int enable)
{
    uint16 data;                /* Holder for new value to write to PHY reg */
    uint16 mask;                /* Holder for bit mask to update in PHY reg */
    int ix;
    uint16 tx_ctrl;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    if (REVERSE_MODE(pc) || (pc->flags & PHYCTRL_SYS_SIDE_CTRL)) {
        SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
    }

    if (SINGLE_LPMODE(pc) && !(PHY84740_SINGLE_PORT_MODE(pc))) {
        /* Use 1.cd16 to disable tx */
        data = enable ? 0 : (1 << 3);
        mask = 1 << 3;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_GENSIG_8071_REGISTER, data, mask));
    } else {
        /* TX transmitter enable/disable. BCST register in single-port mode */
        data = enable ? 0 : (1 << 0);
        mask = 1 << 0;          /* Global PMD transmit disable */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_PMA_PMD_TX_DISABLEr(unit, pc, data, mask));
    }

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
    }

    for (ix = 0; ix < NUM_LANES; ix++) {
        if (PHY84740_SINGLE_PORT_MODE(pc)) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                ix));
        }

        /* TxOn control */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                             PHY84740_PMAD_OPTICAL_CFG_REG,
                                             enable ? 0 : (1 << 12),
                                             (1 << 12)));
        if (enable) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                               PHY84740_PMAD_OPTICAL_CFG_REG,
                                               &data));

            /* c8e4[4] indicates the TX active state when c8e4[12]=0. 0 means TX 
             * active, 1 means low power mode. This state = TxOnOff pin XOR c800[7]
             * Toggle 0xc800[7] to enable the Tx when c8e4[4]=1.
             * The default value of c800[7] is 1, if user straps the TxOnOff to 0,
             * Then the Tx won't be enabled until this function is called.
             */

            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                               PHY84740_PMAD_OPTICAL_SIG_LVL_REG,
                                               &tx_ctrl));
            tx_ctrl &= (1 << 7);
            if (data & (1 << 4)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     PHY84740_PMAD_OPTICAL_SIG_LVL_REG,
                                                     tx_ctrl ? 0 : (1 << 7),
                                                     (1 << 7)));
            }
        }
        /* power on/off the lane */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                             PHY84740_PMAD_OPTICAL_CFG_REG,
                                             enable ? 0 : (1 << 4), (1 << 4)));
        if (!PHY84740_SINGLE_PORT_MODE(pc)) {
            break;
        }
    }
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *    phy_84740_lb_set
 * Purpose:
 *    Put 84740 in PHY PMA/PMD loopback
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int phy_84740_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    uint16 tmp;
    uint16 lane;
    uint16 data16 = 0, mask16 = 0;
    int intf_tx;
    int intf_rx;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    intf_tx = (REVERSE_MODE(pc) || (pc->flags & PHYCTRL_SYS_SIDE_CTRL)) ?
        PHY_DIAG_INTF_SYS : PHY_DIAG_INTF_LINE;

    intf_rx = ( intf_tx ==  PHY_DIAG_INTF_LINE ) ?
        PHY_DIAG_INTF_SYS : PHY_DIAG_INTF_LINE;

    /* TX Enable(Disable) Squelch(FORCE) when we  Enable(Disable) loopback */
    SOC_IF_ERROR_RETURN
        (_phy_84740_squelch_enable(unit, port,
        intf_tx, enable ? TRUE : FALSE,  enable ? TRUE : FALSE ));

    /* RX Disable(Enable) Squelch when we  Enable(Disable) loopback */
    SOC_IF_ERROR_RETURN
        (_phy_84740_squelch_enable(unit, port,
        intf_rx, enable ? FALSE : TRUE, /* force = */ FALSE));

    tmp = enable ? PHY84740_MII_CTRL_PMA_LOOPBACK : 0;

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_PMA_PMD_CTRLr(unit, pc,
                                       tmp, PHY84740_MII_CTRL_PMA_LOOPBACK));

    if (PCS_REPEATER(pc)) {
        if ((PHY84740_SINGLE_PORT_MODE(pc))) {
            for (lane = 0; lane < NUM_LANES; lane++) {
                data16 = 0;

                /* point to the each lane starting at lane0 */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    lane));

                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xcd0a,
                                                     enable ? 1 : 0, 1));

                SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                                  PHY84740_DIG_CTRL_REG,
                                                                  &data16));

                /* 1.cd08.10 = 1.cd08.10  XOR 1.cd08.9 */
                tmp = (data16 & PHY84740_RX_POLARITY_FLIP_MASK) << 1;
                data16 = (data16 & PHY84740_TX_POLARITY_FLIP_MASK) ^ tmp;
                mask16 = PHY84740_TX_POLARITY_FLIP_MASK;

                SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                                    PHY84740_DIG_CTRL_REG,
                                                                    data16,
                                                                    mask16));
            }
            /* point back to lane0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
        } else {                /* quad-port mode */
            data16 = 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xcd0a,
                                                 enable ? 1 : 0, 1));

            SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                              PHY84740_DIG_CTRL_REG,
                                                              &data16));

            /* 1.cd08.10 = 1.cd08.10  XOR 1.cd08.9 */
            tmp = (data16 & PHY84740_RX_POLARITY_FLIP_MASK) << 1;
            data16 = (data16 & PHY84740_TX_POLARITY_FLIP_MASK) ^ tmp;
            mask16 = PHY84740_TX_POLARITY_FLIP_MASK;

            SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc, PHY84740_DIG_CTRL_REG, data16,
                                 mask16));
        }
    }

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_84740_lb_get
 * Purpose:
 *    Get 84740 PHY loopback state
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:    
 *    SOC_E_NONE
 */

STATIC int phy_84740_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16 tmp;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
    }

    SOC_IF_ERROR_RETURN(READ_PHY84740_PMA_PMD_CTRLr(unit, pc, &tmp));
    /* ctrl != 0xffff check is to handle removable PHY daughter cards */
    *enable = ((tmp != 0xffff) &&
               (tmp & PHY84740_MII_CTRL_PMA_LOOPBACK)) ? TRUE : FALSE;

    if (REVERSE_MODE(pc)) {
        SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_lb_get: u=%d port%d: loopback:%s\n"),
              unit, port, *enable ? "Enabled": "Disabled"));
    
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_tx_driver_set(int unit, soc_port_t port, int interface,
                                 soc_phy_control_t type, uint32 value)
{
    uint16 data;                /* Temporary holder of reg value to be written */
    uint16 mask;                /* Bit mask of reg value to be updated */
    int ix;
    int lane;
    int start;
    int end;
    uint16 post_pre_tap = 0, force_preemph = 0;
    uint16 main_tap = 0;
    phy_ctrl_t *pc;             /* PHY software state */
    int reg;
    uint16 post_tap2 = 0;
    uint32 devid = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch (type) {
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
            data = (uint16) (value & 0xf);
            data = data << 12;
            mask = 0xf000;
            reg = (interface == PHY_DIAG_INTF_SYS) ? 0xc90a : 0xca01;
            if ((soc_phy_control_t)type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) {
                lane = 0;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) {
                lane = 1;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) {
                lane = 2;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) {
                lane = 3;
            } else {            /* SOC_PHY_CONTROL_DRIVER_CURRENT */
                lane = 4;       /* all lanes */
            }
            if (PHY84740_SINGLE_PORT_MODE(pc)) {
                start = lane == 4 ? 0 : lane;
                end = lane == 4 ? 4 : lane + 1;
                for (ix = start; ix < end; ix++) {

                    PHY84740_MMF_IF_INTF(unit, pc, interface,
                                         PHY_DIAG_INTF_SYS);

                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                        PHY84740_PMAD_AER_ADDR_REG,
                                                        P2L_MAP(pc, ix)));

                    PHY84740_XFI_IF_INTF(unit, pc, interface,
                                         PHY_DIAG_INTF_SYS);

                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG
                         (unit, pc, reg, data, mask));
                }
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG
                     (unit, pc, reg, data, mask));
            }
            break;

        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
            data = (uint16) (value & 0xf);
            data = data << 8;
            mask = 0x0f00;
            reg = (interface == PHY_DIAG_INTF_SYS) ? 0xc90a : 0xca01;
            if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0) {
                lane = 0;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1) {
                lane = 1;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2) {
                lane = 2;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3) {
                lane = 3;
            } else {            /* SOC_PHY_CONTROL_PRE_DRIVER_CURRENT */
                lane = 4;       /* all lanes */
            }
            if (PHY84740_SINGLE_PORT_MODE(pc)) {
                start = lane == 4 ? 0 : lane;
                end = lane == 4 ? 4 : lane + 1;
                for (ix = start; ix < end; ix++) {

                    PHY84740_MMF_IF_INTF(unit, pc, interface,
                                         PHY_DIAG_INTF_SYS);
                    SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                                        (unit, pc, PHY84740_PMAD_AER_ADDR_REG,
                                         P2L_MAP(pc, ix)));

                    PHY84740_XFI_IF_INTF(unit, pc, interface,
                                         PHY_DIAG_INTF_SYS);
                    SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                        (unit, pc, reg, data, mask));
                }
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG
                     (unit, pc, reg, data, mask));
            }
            break;
        case SOC_PHY_CONTROL_PREEMPHASIS:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
            /* The value format:
             * bit 16:18: post_tap2 value
             * bit 15:  1 enable forced preemphasis ctrl, 0 auto config
             * bit 14:10:  post_tap value
             * bit 09:04:  main tap value
             * bit 03:00:  pre_tap value 
             * 
             * set 1.CA07[8] = 0, 1.CA02[15:11] for main_tap and 1.CA05[7:4] for post tap.
             * main tap control is in the register field 0xCA02[15:11]. 
             * If a wrong value is written to the same register, field 0xCA02[9:7],
             *  it may DAMAGE the device. User must avoid writing to 0xCA02[9:7] 
             */
            /* Register bit8, 0 force preemphasis, 1 auto config, 1.CA07 */
            force_preemph = PHY84740_PREEMPH_GET_FORCE(value) ? 0 :
                PHY84740_PREEMPH_REG_FORCE_MASK;

            /* add post tap value */
            post_pre_tap = PHY84740_PREEMPH_GET_POST_TAP(value) <<
                PHY84740_PREEMPH_REG_POST_TAP_SHFT;

            /* add pre tap value */
            post_pre_tap |= PHY84740_PREEMPH_GET_PRE_TAP(value) <<
                PHY84740_PREEMPH_REG_PRE_TAP_SHFT;

            /* main tap in 1.CA02 */
            main_tap = PHY84740_PREEMPH_GET_MAIN_TAP(value) <<
                PHY84740_PREEMPH_REG_MAIN_TAP_SHFT;

            post_tap2 = PHY84740_PREEMP_GET_POST_TAP2(value) <<
                PHY84740_PREEMPH_REG_POST_TAP2_SHFT;

            reg = (interface == PHY_DIAG_INTF_SYS) ? 0xc90c : 0xca06;

            if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {
                lane = 0;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) {
                lane = 1;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) {
                lane = 2;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) {
                lane = 3;
            } else {            /* SOC_PHY_CONTROL_PREEMPHASIS_LANE */
                lane = 4;       /* all lanes */
            }
            if (PHY84740_SINGLE_PORT_MODE(pc)) {
                start = lane == 4 ? 0 : lane;
                end = lane == 4 ? 4 : lane + 1;
            } else {
                /* just make sure execute once */
                start = 0;
                end = 1;
            }
            for (ix = start; ix < end; ix++) {
                if (PHY84740_SINGLE_PORT_MODE(pc)) {

                    PHY84740_MMF_IF_INTF(unit, pc, interface,
                                         PHY_DIAG_INTF_SYS);
                    SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                                        (unit, pc, PHY84740_PMAD_AER_ADDR_REG,
                                         P2L_MAP(pc, ix)));
                    PHY84740_XFI_IF_INTF(unit, pc, interface,
                                         PHY_DIAG_INTF_SYS);
                }
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, reg,
                                                     post_pre_tap,
                                                     PHY84740_PREEMPH_REG_POST_TAP_MASK
                                                     |
                                                     PHY84740_PREEMPH_REG_PRE_TAP_MASK));

                /* N/A on system side */
                if (interface != PHY_DIAG_INTF_SYS) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xca07,
                                                         force_preemph,
                                                         PHY84740_PREEMPH_REG_FORCE_MASK));
                }

                /* always clear tx_pwrdn bit. The read/modify/write may accidently
                 * set the bit 0xCA02[10]. Because the CA02[10] read value is: 
                 * CA02[10]_read_value = CA02[10] OR Digital_State_Machine 
                 * The problem will occur when the Digital_State_Machine output is 1
                 * at the time of reading this register.
                 */
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                     (interface ==
                                                      PHY_DIAG_INTF_SYS) ?
                                                     0xc90b : 0xca02, main_tap,
                                                     PHY84740_PREEMPH_REG_MAIN_TAP_MASK
                                                     |
                                                     PHY84740_PREEMPH_REG_TX_PWRDN_MASK));

                SOC_IF_ERROR_RETURN
                    (_phy_84740_config_devid(pc->unit, pc->port, pc, &devid));

                /* Only Quadra devices have post tap 2 */
                /* Initialize to zero if i) 40G ii) Backplane package iii) system side */
                if (PHY84740_IS_QUADRA(devid)) {
                    if (PHY84740_SINGLE_PORT_MODE(pc) ||
                        (interface == PHY_DIAG_INTF_SYS) ||
                        (PHY84740_IS_QUADRA_BACKPLANE(devid))) {
                        SOC_IF_ERROR_RETURN
                            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                             0xca08,
                                                             0x0,
                                                             PHY84740_PREEMPH_REG_POST_TAP2_MASK));
                    } else {
                        SOC_IF_ERROR_RETURN
                            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                             0xca08,
                                                             post_tap2,
                                                             PHY84740_PREEMPH_REG_POST_TAP2_MASK));
                    }
                }
            }
            break;

        default:
            /* should never get here */
            return SOC_E_PARAM;
    }

#ifdef PHY84740_FCS_ERROR_WAR
    /* Toggle Transmit lane output so that the link partner
       resets itself to a good signal. Othwerise, it could 
       sync to a sub optimal signal and can see FCS errors. 
     */
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        start = lane == 4 ? 0 : lane;
        end = lane == 4 ? 4 : lane + 1;
    } else {
        /* just make sure execute once */
        start = 0;
        end = 1;
    }
    for (ix = start; ix < end; ix++) {
        if (PHY84740_SINGLE_PORT_MODE(pc)) {

            PHY84740_MMF_IF_INTF(unit, pc, interface, PHY_DIAG_INTF_SYS);
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                P2L_MAP(pc, ix)));
            PHY84740_XFI_IF_INTF(unit, pc, interface, PHY_DIAG_INTF_SYS);

            data = mask = 0x1 << (12 + P2L_MAP(pc, ix));        /* per lane */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_AN_REG(unit, pc, 0x800e, data, mask));

            data = 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_AN_REG(unit, pc, 0x800e, data, mask));
        } else {
            data = mask = (lane == 4) ? 0x1 << 11 :     /* Global */
                0x1 << (12 + P2L_MAP(pc, lane));        /* per lane */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_AN_REG(unit, pc, 0x800e, data, mask));
            data = 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_AN_REG(unit, pc, 0x800e, data, mask));
        }
    }
#endif                          /* PHY84740_FCS_ERROR_WAR */

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        PHY84740_MMF_IF_INTF(unit, pc, interface, PHY_DIAG_INTF_SYS);
        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, 0));
        PHY84740_XFI_IF_INTF(unit, pc, interface, PHY_DIAG_INTF_SYS);
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_tx_driver_get(int unit, soc_port_t port, int interface,
                                 soc_phy_control_t type, uint32 *value)
{
    uint16 data16;              /* Temporary holder of a reg value */
    uint16 main_tap;
    uint16 post_tap;
    int lane;
    phy_ctrl_t *pc;             /* PHY software state */
    uint16 post_tap2 = 0;
    uint16 pre_tap = 0;
    uint16 force_preemph = 0;
    uint32 devid = 0;
    int reg;

    pc = EXT_PHY_SW_STATE(unit, port);

    data16 = 0;
    switch (type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
            if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {
                lane = 0;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) {
                lane = 1;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) {
                lane = 2;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) {
                lane = 3;
            } else {
                lane = 0;
            }
            /* 1.CA05[15] = 0, set 1.CA02[15:11] for main_tap and 
             * 1.CA05[7:4] for postcursor 
             */
            if (PHY84740_SINGLE_PORT_MODE(pc)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    P2L_MAP(pc, lane)));
            }
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                               (interface ==
                                                PHY_DIAG_INTF_SYS) ? 0xc90b :
                                               0xca02, &main_tap));
            SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc,
                                 (interface ==
                                  PHY_DIAG_INTF_SYS) ? 0xc90c : 0xca06,
                                 &post_tap));
            SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc,
                                 (interface ==
                                  PHY_DIAG_INTF_SYS) ? 0xc90c : 0xca06,
                                 &pre_tap));

            if (interface != PHY_DIAG_INTF_SYS) {
                SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc,
                                 0xca07,
                                 &force_preemph));
                /* bit15 force flag, 14:10 post_tap, 09:04 main tap */
                *value = (force_preemph & PHY84740_PREEMPH_REG_FORCE_MASK) ? 0 :
                    (1 << PHY84740_PREEMPH_CTRL_FORCE_SHFT);
            } else {
                *value = 0;
            } 
            *value |= ((post_tap & PHY84740_PREEMPH_REG_POST_TAP_MASK) >>
                       PHY84740_PREEMPH_REG_POST_TAP_SHFT) <<
                PHY84740_PREEMPH_CTRL_POST_TAP_SHFT;
            *value |= ((main_tap & PHY84740_PREEMPH_REG_MAIN_TAP_MASK) >>
                       PHY84740_PREEMPH_REG_MAIN_TAP_SHFT) <<
                PHY84740_PREEMPH_CTRL_MAIN_TAP_SHFT;
            *value |= ((pre_tap & PHY84740_PREEMPH_REG_PRE_TAP_MASK) >>
                       PHY84740_PREEMPH_REG_PRE_TAP_SHFT) <<
                PHY84740_PREEMPH_CTRL_PRE_TAP_SHFT;
            SOC_IF_ERROR_RETURN
                (_phy_84740_config_devid(pc->unit, pc->port, pc, &devid));

            /* Only Quadra devices have post tap 2 */
            /* Initialize to zero if i) 40G ii) Backplane package iii) system side */
            if (PHY84740_IS_QUADRA(devid)) {
                if (PHY84740_SINGLE_PORT_MODE(pc) ||
                    (interface == PHY_DIAG_INTF_SYS) ||
                    (PHY84740_IS_QUADRA_BACKPLANE(devid))) {
                    post_tap2 = 0;
                } else {
                    SOC_IF_ERROR_RETURN
                        (READ_PHY84740_MMF_PMA_PMD_REG
                         (unit, pc, 0xca08, &post_tap2));
                }
                *value |= ((post_tap2 & PHY84740_PREEMPH_REG_POST_TAP2_MASK) >>
                           PHY84740_PREEMPH_REG_POST_TAP2_SHFT) <<
                    PHY84740_PREEMPH_CTRL_POST_TAP2_SHFT;
            }
            break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
            reg = (interface == PHY_DIAG_INTF_SYS) ? 0xc90a : 0xca01;
            if ((soc_phy_control_t)type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) {
                lane = 0;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) {
                lane = 1;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) {
                lane = 2;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) {
                lane = 3;
            } else {
                lane = 0;
            }
            if (PHY84740_SINGLE_PORT_MODE(pc)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    P2L_MAP(pc, lane)));
            }
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
            *value = (data16 & 0xf000) >> 12;
            break;

        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
            reg = (interface == PHY_DIAG_INTF_SYS) ? 0xc90a : 0xca01;
            if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0) {
                lane = 0;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1) {
                lane = 1;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2) {
                lane = 2;
            } else if ((soc_phy_control_t)type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3) {
                lane = 3;
            } else {
                lane = 0;
            }
            if (PHY84740_SINGLE_PORT_MODE(pc)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    P2L_MAP(pc, lane)));
            }
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, reg, &data16));
            *value = (data16 & 0x0f00) >> 8;
            break;

        default:
            /* should never get here */
            return SOC_E_PARAM;
    }

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_edc_mode_set(int unit, soc_port_t port, uint32 value)
{
    uint16 data16;
    uint16 mask16;
    phy_ctrl_t *pc;             /* PHY software state */
    int ix;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* clear software overrides */
    mask16 = 0;
    if (!RX_LOS(pc)) {
        mask16 |= PHY84740_RXLOS_OVERRIDE_MASK;
    }
    /* default: override MOD_ABS signalling */
    if (!MOD_ABS(pc)) {
        mask16 |= PHY84740_MOD_ABS_OVERRIDE_MASK;
    }

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        for (ix = 0; ix < NUM_LANES; ix++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                ix));
            SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc, PHY84740_PMAD_OPTICAL_CFG_REG, 0,
                                 mask16));
        }
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                             PHY84740_PMAD_OPTICAL_CFG_REG, 0,
                                             mask16));
    }

    for (ix = 0; ix < NUM_LANES; ix++) {
        if (PHY84740_SINGLE_PORT_MODE(pc)) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                ix));
        }

        /* EDC mode programming sequence */
        mask16 = 1 << 9;

        /* induce LOS condition: toggle register bit 0xc800.9 */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                           PHY84740_PMAD_OPTICAL_SIG_LVL_REG,
                                           &data16));

        /* only change toggled bit 9 */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                             PHY84740_PMAD_OPTICAL_SIG_LVL_REG,
                                             ~data16, mask16));

        /* program EDC mode */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1A, (uint16) value,
                                             PHY84740_EDC_MODE_MASK));

        /* remove LOS condition: restore back original value of bit 0xc800.9 */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                             PHY84740_PMAD_OPTICAL_SIG_LVL_REG,
                                             data16, mask16));

        if (!PHY84740_SINGLE_PORT_MODE(pc)) {
            break;
        }
    }

    /* restore software override */
    data16 = 0;
    mask16 = PHY84740_RXLOS_OVERRIDE_MASK | PHY84740_MOD_ABS_OVERRIDE_MASK;
    if (!RX_LOS(pc)) {
        data16 |= PHY84740_RXLOS_OVERRIDE_MASK;
    }
    /* default: override MOD_ABS signalling */
    if (!MOD_ABS(pc)) {
        data16 |= PHY84740_MOD_ABS_OVERRIDE_MASK;
    }

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        for (ix = 0; ix < NUM_LANES; ix++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                ix));
            SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc, PHY84740_PMAD_OPTICAL_CFG_REG,
                                 data16, mask16));
        }

        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    } else {
        /* XXX temp 0xc0c0: RXLOS override: 0x0808 MOD_ABS override */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                             PHY84740_PMAD_OPTICAL_CFG_REG,
                                             data16, mask16));
    }

    return SOC_E_NONE;
}

/*
 * enable/disable syncE recoverd clock for 10G/40G modes.
 */
STATIC int
_phy_84740_control_recovery_clock_set(int unit, soc_port_t port, int enable)
{
    uint16 data16;
    uint16 mask16;
    int lane;
    phy_ctrl_t *pc;             /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    /* 
     *  1.CB27[3]        = 0   to disable FC recovered clock to appear at TESTP/N.
     *  1.CA1D[11]       = 0   to disable PLL clock to appear at TESTP/N.
     *  1.CA1D[1:0]      = 00  to disable bandgap voltage and vco_control voltage appear at TESTP/N.
     *  1.CB27[0]        = 1   enables rx recovered clock to come out of TESTP/N pads.
     *  1.CB26[14:13]    = 11  for 156.25Mhz syncE divider select
     *  1.CB26[12]       = 1   for syncE clock out.
     */

    for (lane = 0; lane < NUM_LANES; lane++) {
        if (PHY84740_SINGLE_PORT_MODE(pc)) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                lane));
        }
        /* set to 0 in both enable/disable cases */

        /* disable FC recovered clock to appear at TESTP/N. */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_RECOVERY_CLK_REG, 0, (1 << 3)));
        /* disable PLL clock to appear at TESTP/N
         * disable bandgap voltage and vco_control voltage appear at TESTP/N
         */
        mask16 = (1 << 11) | 0x3;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1D, 0, mask16));

        /* enables rx recovered clock to come out of TESTP/N pads */
        mask16 = PHY84740_RECOVERY_CLK_ENABLE_MASK;
        data16 = enable ? mask16 : 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_RECOVERY_CLK_REG, data16, mask16));

        /* always use bits 14:13 to select the sync clock frequency */
        mask16 = (0x1 << 12);
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_RECOVERY_CLK_FREQ_REG, data16, mask16));
        if (!PHY84740_SINGLE_PORT_MODE(pc)) {
            break;
        }
    }
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    }
    return SOC_E_NONE;
}

/*
 * set the frequency of the syncE recoverd clock.
 */
STATIC int
_phy_84740_control_recovery_clock_freq_set(int unit, soc_port_t port, int freq)
{
    phy_ctrl_t *pc;             /* PHY software state */
    int lane;
    uint16 freq_ctrl;

    pc = EXT_PHY_SW_STATE(unit, port);

    /*  
     * 1.CB26[14:13]
     * Select the frequency of SyncE clock to be sent to the test port: 
     * 00 :  Power down syncE clock dividers
     * 01 :  125 MHz            (divide by 82.5) 
     * 10 :  195.3125 MHz       (divide by 52.8)
     * 11 :  156.25 MHz         (divide by 66)
     */

    switch (freq) {
        case 125000:
            freq_ctrl = 0x1;
            break;
        case 195312:
            freq_ctrl = 0x2;
            break;
        case 156250:
            freq_ctrl = 0x3;
            break;
        default:
            freq_ctrl = 0x0;
            break;
    }

    for (lane = 0; lane < NUM_LANES; lane++) {
        if (PHY84740_SINGLE_PORT_MODE(pc)) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                lane));
        }

        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_RECOVERY_CLK_FREQ_REG,
              PHY84740_RECOVERY_CLK_FREQf(freq_ctrl),
              PHY84740_RECOVERY_CLK_FREQ_MASK));

        if (!PHY84740_SINGLE_PORT_MODE(pc)) {
            break;
        }
    }
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_prbs_tx_invert_data_set(int unit, soc_port_t port,
                                           int intf, int invert)
{
    phy_ctrl_t *pc;             /* PHY software state */
    uint16 data16, speed_val, mask16 = 0x1 << 2;
    uint32 devid;
    int lane = 0;
    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                       (pc->unit, pc->port, pc, &devid));
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        if (PHY_DIAG_INTF_SYS == intf) {
            data16 = invert ? PHY84740_USER_PRBS_TX_INVERT : 0;
            mask16 = PHY84740_USER_PRBS_TX_INVERT;
            for (lane = 0; lane < NUM_LANES; lane++) {
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, lane));
                SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                            PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                                 data16, mask16));
            }
            SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                 PHY84740_PMAD_AER_ADDR_REG, 0));
            SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
        } else {
            if(PHY84740_IS_QUADRA(devid)) {
           /*
            * In Quadra PRBS TX is inverted in PACKAGE itself,
            * by default bit no. 4 and bit no.3 in reg 0xcd60 is set to
            * 1 (in this mode),which means twice inversion of PRBS TX
            * (means no inversion in TX)
            * only PRBS RX is inverted,so prbs lock won't happen,to avoid
            * this situation we are clearing bit no.4 in 0xcd60,means
            * both TX and RX inverted leads to zero prbs error.
 
            * NOTE:Bit no.6 in 0xcd60 is not considered in 40G mode.
            *      The autodetect is forced to disable.
            */
               data16 = invert ? 0 : PHY84740_USER_PRBS_TX_INVERT;
               mask16 = PHY84740_USER_PRBS_TX_INVERT;
               for (lane = 0; lane < NUM_LANES; lane++) {
                  SOC_IF_ERROR_RETURN
                     (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                      PHY84740_PMAD_AER_ADDR_REG, lane));
                  SOC_IF_ERROR_RETURN
                     (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                      PHY84740_UDRCD_PRBS20_CONTROL_REGISTER,
                      data16,mask16));
               }
               SOC_IF_ERROR_RETURN
                  (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                   PHY84740_PMAD_AER_ADDR_REG, 0));
            } else {
                return SOC_E_UNAVAIL;
            }
       }
    } else {                    /* quad-port mode */
        SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                            (pc->unit, pc->port, pc, &devid));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc81f, &speed_val));
        if (PHY84740_IS_QUADRA(devid) && (speed_val & (1 << 2))) {
            data16 = invert ? mask16 : 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_AN_REG(unit, pc, 0x8019, data16, mask16));
        } else {
            /* tx inversion */
            data16 = invert ? PHY84740_USER_PRBS_TX_INVERT : 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                 PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                                 data16,
                                                 PHY84740_USER_PRBS_TX_INVERT));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_prbs_rx_invert_data_set(int unit, soc_port_t port,
                                           int intf, int invert)
{
    phy_ctrl_t *pc;             /* PHY software state */
    uint16 data16;
    uint16 mask16;
    int lane = 0;
    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        if (PHY_DIAG_INTF_SYS == intf) {
            data16 = invert ? PHY84740_USER_PRBS_RX_INVERT : 0;
            mask16 = PHY84740_USER_PRBS_RX_INVERT;
 
            for (lane = 0; lane < NUM_LANES; lane++) {
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, lane));
                SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                            PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                            data16, mask16));
            }
            SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                 PHY84740_PMAD_AER_ADDR_REG, 0));
       } else {
             return SOC_E_UNAVAIL;
       }
    } else {                    /* quad-port mode */
        /* rx inversion */
        data16 = invert ? PHY84740_USER_PRBS_RX_INVERT : 0;
        mask16 = PHY84740_USER_PRBS_RX_INVERT;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                             PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                             data16, mask16));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_prbs_polynomial_set(int unit, soc_port_t port, int poly_ctrl,
                                       int intf, int tx)
{                               /* tx or rx */
    phy_ctrl_t *pc;             /* PHY software state */
    uint16 data16;
    uint16 mask16, speed_val;
    uint32 devid;
    int lane=0; 
    /*
     * poly_ctrl:
     *  0x0 = prbs7
     *  0x1 = prbs15
     *  0x2 = prbs23
     *  0x3 = prbs31
     *  0x4 = prbs9
     *  0x5 = prbs11
     */
    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN(
        _phy_84740_config_devid(pc->unit,pc->port, pc,  &devid));
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        if (PHY_DIAG_INTF_SYS == intf) {
            mask16 = PHY84740_40G_SYS_PRBS_MASK;
            if (poly_ctrl == SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1) {
                data16 = PHY84740_40G_SYS_PRBS31;
            } else if (poly_ctrl == SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1) {
                data16 = PHY84740_40G_SYS_PRBS9; 
            } else if (poly_ctrl == SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1) {
                data16 = PHY84740_40G_SYS_PRBS7;
            } else if (poly_ctrl == SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1) {
                data16 = PHY84740_40G_SYS_PRBS15;
            } else if (poly_ctrl == SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1) {
                data16 = PHY84740_40G_SYS_PRBS23;
            } else if (poly_ctrl == SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1) {
                data16 = PHY84740_40G_SYS_PRBS11;
            } else {
                return SOC_E_PARAM;
            }
            if (!tx) {
                data16 <<= 12;
                mask16 <<= 12;
            }
            for (lane = 0; lane < NUM_LANES; lane++) {
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, lane));
                SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                            PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                                 data16,mask16));
            }
            SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                 PHY84740_PMAD_AER_ADDR_REG, 0));
            SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));

        } else {
            if(PHY84740_IS_QUADRA(devid)) {
               mask16 = PHY84740_40G_PRBS31 | PHY84740_40G_PRBS9 ;
               if (poly_ctrl == SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1) {
                   data16 = PHY84740_40G_PRBS31;
               } else if (poly_ctrl == SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1) {
                   data16 = PHY84740_40G_PRBS9;
               } else {
                   return SOC_E_PARAM;
               }
               SOC_IF_ERROR_RETURN
                   (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                     PHY84740_FORTYG_SQ_WAVE_TESTING_CONTROL_REGISTER,
                     data16,mask16));
            } else {
                    /* only two types available PRBS9 and PRBS31 */
                if (poly_ctrl == 3) {
                    data16 = PHY84740_40G_PRBS31;
                } else {
                    data16 = PHY84740_40G_PRBS9;
                }
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                      PHY84740_FORTYG_PRBS_PATTERN_TESTING_CONTROL_STATUS,
                      data16, PHY84740_40G_PRBS9 | PHY84740_40G_PRBS31));
            }
       }
    } else {                    /* quad-port mode */
        SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                            (pc->unit, pc->port, pc, &devid));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc81f, &speed_val));

        if (PHY84740_IS_QUADRA(devid) && (speed_val & (1 << 2))) {
            if (poly_ctrl == 0) {
                poly_ctrl = 0;
            } else if (poly_ctrl == 3) {
                poly_ctrl = 3;
            } else if (poly_ctrl == 4) {
                poly_ctrl = 1;
            } else if (poly_ctrl == 5) {
                poly_ctrl = 2;
            } else {
                return SOC_E_PARAM;
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG(unit, pc, 0x8015, 0x0010));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG(unit, pc, 0x8000, 0x2c22));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG(unit, pc, 0x8017, 0x0));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_AN_REG
                 (unit, pc, 0x8019, poly_ctrl, 0x0003));
        } else {
            /* 001 : prbs7 
               010 : prbs9
               011 : prbs11
               100 : prbs15
               101 : prbs23
               110 : prbs31
             */
            if (poly_ctrl == 0) {
                data16 = 1;
            } else if (poly_ctrl == 1) {
                data16 = 4;
            } else if (poly_ctrl == 2) {
                data16 = 5;
            } else if (poly_ctrl == 3) {
                data16 = 6;
            } else if (poly_ctrl == 4) {
                data16 = 2;
            } else if (poly_ctrl == 5) {
                data16 = 3;
            } else {
                return SOC_E_PARAM;
            }
            mask16 = PHY84740_USER_PRBS_TYPE_MASK;

            /* default is tx */
            if (!tx) {          /* rx */
                data16 = data16 << 12;
                mask16 = mask16 << 12;
            }

            /* both tx/rx types */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                 PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                                 data16, mask16));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_prbs_enable_set(int unit, soc_port_t port,
                                   int intf, int enable)
{
    phy_ctrl_t *pc;             /* PHY software state */
    int lane;
    int repeater_mode;
    uint16 data16, speed_val, regval=0,mask16;
    uint32 devid;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN(
        _phy_84740_config_devid(pc->unit,pc->port, pc,  &devid));
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        if (PHY_DIAG_INTF_SYS == intf) {
            for (lane = 0; lane < NUM_LANES; lane++) {
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, lane));
                SOC_IF_ERROR_RETURN
                    (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81d, &data16));
                if ((data16 & PHY84740_40G_SYS_REP_MODE_ENABLE ) ==
                    PHY84740_40G_SYS_REP_MODE_ENABLE ) {
                    regval = enable ? PHY84740_40G_SYS_PRBS_ENABLE : 0;
                    SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                    SOC_IF_ERROR_RETURN
                        (WRITE_PHY84740_MMF_AN_REG(unit, pc, 0x810c,regval));
                }
            }
            SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                0));
            SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
        } else {
            if(PHY84740_IS_QUADRA(devid)) {
               mask16 = PHY84740_40G_PRBS_ENABLE;
               data16 = enable ? PHY84740_40G_PRBS_ENABLE : 0;
               SOC_IF_ERROR_RETURN
                   (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                       PHY84740_FORTYG_SQ_WAVE_TESTING_CONTROL_REGISTER,
                       data16,mask16));
           /*
            * In Quadra PRBS TX is inverted in PACKAGE itself,
            * by default bit no. 4 and bit no.3 in reg 0xcd60 is set to
            * 1 in this mode,which means twice inversion of PRBS TX
            * (means no inversion in TX)
            * only PRBS RX is inverted,so prbs lock won't happen,to avoid
            * this situation we are clearing bit no.4 in 0xcd60,means
            * both TX and RX inverted leads to zero prbs error.
 
            * NOTE:Bit no.6 in 0xcd60 is not considered in 40G mode.
            *      The autodetect is forced to disable.
            */
               data16 = enable ? 0 : PHY84740_USER_PRBS_TX_INVERT;
               mask16 = PHY84740_USER_PRBS_TX_INVERT;
               for (lane = 0; lane < NUM_LANES; lane++) {
                  SOC_IF_ERROR_RETURN
                      (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                            PHY84740_PMAD_AER_ADDR_REG, lane));
                  SOC_IF_ERROR_RETURN
                      (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                         PHY84740_UDRCD_PRBS20_CONTROL_REGISTER,
                         data16,mask16));
               }
            } else {
                for (lane = 0; lane < NUM_LANES; lane++) {
                   SOC_IF_ERROR_RETURN
                       (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    lane));

                   /* GENSIG_8071_REGISTER Clocks enable */
                   SOC_IF_ERROR_RETURN
                       (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_GENSIG_8071_REGISTER,
                                                    enable ? 0x3 : 0));
                   /* RESET_CONTROL_REGISTER */
                   SOC_IF_ERROR_RETURN
                       (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_RESET_CONTROL_REGISTER,
                                                    0x0));

                   /* DATA_MUX_SEL_CNTRL_REGISTER */
                   SOC_IF_ERROR_RETURN
                       (WRITE_PHY84740_MMF_PMA_PMD_REG
                       (unit, pc, 0xcd58, enable ? 0x2 : 0));

                   /* 40G_PRBS_PATTERN_TESTING_CONTROL_STATUS */
                   SOC_IF_ERROR_RETURN
                       (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                        PHY84740_FORTYG_PRBS_PATTERN_TESTING_CONTROL_STATUS,
                        enable ?
                        PHY84740_40G_PRBS_ENABLE : 0,
                        0xf));
                 }
              }
        }
        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    } else {                    /* quad-port mode */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81d, &data16));

        repeater_mode = ((data16 & 0x6) == 0x6) ? TRUE : FALSE;
        if (repeater_mode) {
            /* Enable retimer prbs clocks inside SFI */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                 PHY84740_GENSIG_8071_REGISTER,
                                                 enable ? 0x3 : 0, 0x3));

            /* Enable the retimer datapath */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xcd58,
                                                 enable ? 0x2 : 0, 0x3));
        }
        SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                            (pc->unit, pc->port, pc, &devid));

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81f, &speed_val));
        if (PHY84740_IS_QUADRA(devid) && (speed_val & (1 << 2))) {
            SOC_IF_ERROR_RETURN
               (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81d, &data16));
            if (data16 & PHY84740_RX_REPEATER_MASK) {
               SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc806,
                 enable ? 0 : PHY84740_TX_REPEATER_ENABLE,
                 PHY84740_TX_REPEATER_MASK));
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG(unit, pc, 0x80b1, 0x0007));
            if (enable) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_AN_REG
                     (unit, pc, 0x8019, 0x0008, 0x0008));
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_AN_REG
                     (unit, pc, 0x8019, 0x0000, 0x0008));
            }

        } else {
            /* clear PRBS control if disabled */
            if (!enable) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 
                                                     PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                                     0,
                                                     PHY84740_USER_PRBS_TYPE_MASK | PHY84740_USER_PRBS_TYPE_MASK << 12));    /* rx type */
            }
            /*  program the PRBS enable */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                 PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                                 enable ?
                                                 PHY84740_USER_PRBS_ENABLE : 0,
                                                 PHY84740_USER_PRBS_ENABLE));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_prbs_enable_get(int unit, soc_port_t port,int intf, uint32 *value)
{
    uint16 data16, speed_val,temp=0;
    phy_ctrl_t *pc;             /* PHY software state */
    uint32 devid;
    int lane = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN(
        _phy_84740_config_devid(pc->unit,pc->port, pc,  &devid));
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        if (PHY_DIAG_INTF_SYS == intf) {
            temp = PHY84740_40G_SYS_PRBS_ENABLE_MASK;
            for (lane = 0; lane < NUM_LANES; lane++) {
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, lane));
                SOC_IF_ERROR_RETURN
                    (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81d, &data16));
                if ((data16 & PHY84740_40G_SYS_REP_MODE_ENABLE) ==
                     PHY84740_40G_SYS_REP_MODE_ENABLE) {
                    SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                    SOC_IF_ERROR_RETURN
                        (READ_PHY84740_MMF_AN_REG(unit, pc, 0x810c, &data16));
                    if ((temp & data16) != PHY84740_40G_SYS_PRBS_ENABLE) {
                         *value  = FALSE;
                         break;
                    }
                    *value = TRUE ;
                }
            }
            SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                 PHY84740_PMAD_AER_ADDR_REG, 0));
            SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
        } else {
            if(PHY84740_IS_QUADRA(devid)) {
               SOC_IF_ERROR_RETURN
                   (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                    PHY84740_FORTYG_SQ_WAVE_TESTING_CONTROL_REGISTER,
                    &data16));
               data16 = data16 & PHY84740_40G_PRBS_ENABLE;
               *value = (data16 == PHY84740_40G_PRBS_ENABLE) ? TRUE : FALSE;
            } else {
                SOC_IF_ERROR_RETURN
                   (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                    PHY84740_FORTYG_PRBS_PATTERN_TESTING_CONTROL_STATUS,
                    &data16));
                if ((data16 & PHY84740_40G_PRBS_ENABLE) == PHY84740_40G_PRBS_ENABLE) {
                   *value = TRUE;
                } else {
                    *value = FALSE;
                }
            }
        }
    } else {                    /* quad-port mode */
        SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                            (pc->unit, pc->port, pc, &devid));

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81f, &speed_val));

        if (PHY84740_IS_QUADRA(devid) && (speed_val & (1 << 2))) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_AN_REG(unit, pc, 0x8019, &data16));
            *value = (data16 & 0x0008) ? TRUE : FALSE;
        } else {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                               PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                               &data16));
            if (data16 & PHY84740_USER_PRBS_ENABLE) {
                *value = TRUE;
            } else {
                *value = FALSE;
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_prbs_polynomial_get(int unit, soc_port_t port,
                                       int intf, uint32 *value)
{
    phy_ctrl_t *pc;             /* PHY software state */
    uint16 poly_ctrl, speed_val, data16=0;
    uint32 devid;
    int lane = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    /*
     * poly_ctrl:
     *  0x0 = prbs7
     *  0x1 = prbs15
     *  0x2 = prbs23
     *  0x3 = prbs31
     *  0x4 = prbs9
     *  0x5 = prbs11
     */
    SOC_IF_ERROR_RETURN(
       _phy_84740_config_devid(pc->unit,pc->port, pc,  &devid));
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        if (PHY_DIAG_INTF_SYS == intf) {
            poly_ctrl = 0;
            for (lane = 0; lane < NUM_LANES; lane++) {
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, lane));
                SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                SOC_IF_ERROR_RETURN
                    (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                            PHY84740_USER_PRBS_CONTROL_0_REGISTER,&data16));
                poly_ctrl |= (data16 & PHY84740_40G_SYS_PRBS_POLY_MASK) ;
            }
            SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG, 0));
            SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));

            if (poly_ctrl == (PHY84740_40G_SYS_PRBS31 |
                              (PHY84740_40G_SYS_PRBS31 << 12))) {
                *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
            } else if (poly_ctrl == (PHY84740_40G_SYS_PRBS23 |
                                    (PHY84740_40G_SYS_PRBS23 << 12))) {
                *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
            } else if (poly_ctrl == (PHY84740_40G_SYS_PRBS15 |
                                    (PHY84740_40G_SYS_PRBS15 << 12))) {
                *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
            } else if (poly_ctrl == (PHY84740_40G_SYS_PRBS11 |
                                    (PHY84740_40G_SYS_PRBS11 << 12))) {
                *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
            } else if (poly_ctrl == (PHY84740_40G_SYS_PRBS9 |
                                    (PHY84740_40G_SYS_PRBS9 << 12))) {
                *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
            } else if (poly_ctrl == (PHY84740_40G_SYS_PRBS7 |
                                    (PHY84740_40G_SYS_PRBS7 << 12))) {
                *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
            } else {
                return SOC_E_INTERNAL;
            }
        } else {
           if(PHY84740_IS_QUADRA(devid)) {
              SOC_IF_ERROR_RETURN
                  (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                   PHY84740_FORTYG_SQ_WAVE_TESTING_CONTROL_REGISTER,
                   &poly_ctrl));
              poly_ctrl &= (PHY84740_40G_PRBS31 | PHY84740_40G_PRBS9);
              if (poly_ctrl == PHY84740_40G_PRBS31) {
                  *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
              } else if (poly_ctrl == PHY84740_40G_PRBS9) {
                  *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
              } else {
                  return SOC_E_INTERNAL;
              }
           } else {
               SOC_IF_ERROR_RETURN
                   (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                   PHY84740_FORTYG_PRBS_PATTERN_TESTING_CONTROL_STATUS,
                   &poly_ctrl));
               if (poly_ctrl & PHY84740_40G_PRBS31) {
                   *value = 3;
               } else {
                   *value = 0;         /* 0 indicates PRBS9 */
               }
           }
       }
    } else {                    /* quad-port mode */
        SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                            (pc->unit, pc->port, pc, &devid));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc81f, &speed_val));

        if (PHY84740_IS_QUADRA(devid) && (speed_val & (1 << 2))) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_AN_REG(unit, pc, 0x8019, &poly_ctrl));
            *value = poly_ctrl & (uint16) 0x0003;
            if (*value == 1)
                *value = 4;
            if (*value == 2)
                *value = 5;

        } else {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                               PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                               &poly_ctrl));
            poly_ctrl &= PHY84740_USER_PRBS_TYPE_MASK;

            if (poly_ctrl == 1) {
                *value = 0;
            } else if (poly_ctrl == 4) {
                *value = 1;
            } else if (poly_ctrl == 5) {
                *value = 2;
            } else if (poly_ctrl == 6) {
                *value = 3;
            } else if (poly_ctrl == 2) {
                *value = 4;
            } else if (poly_ctrl == 3) {
                *value = 5;
	    } else {
	        *value = poly_ctrl;
	    }
        }
    }

    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_prbs_tx_invert_data_get(int unit, soc_port_t port,
                                           int intf, uint32 *value)
{
    phy_ctrl_t *pc;             /* PHY software state */
    uint16 data16, speed_val;
    uint32 devid;
    int lane = 0;   

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                       (pc->unit, pc->port, pc, &devid));
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
      if (PHY_DIAG_INTF_SYS == intf) {
            *value = 0;
            for (lane = 0; lane < NUM_LANES; lane++) {
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, lane));
                SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                SOC_IF_ERROR_RETURN
                    (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                            PHY84740_USER_PRBS_CONTROL_0_REGISTER,&data16));
                *value |=  data16 &(uint16)PHY84740_USER_PRBS_TX_INVERT;
            }
            *value >>= 4;
            SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                 PHY84740_PMAD_AER_ADDR_REG, 0));
            SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
       } else {
           if(PHY84740_IS_QUADRA(devid)) {
              *value = 0;
           /*
            * In Quadra PRBS TX is inverted in PACKAGE itself,
            * by default bit no. 4 and bit no.3 in reg 0xcd60 is set to
            * 1 in this mode,which means twice inversion of PRBS TX
            * (means no inversion in TX)
            * only PRBS RX is inverted,so prbs lock won't happen,to avoid
            * this situation we are clearing bit no.4 in 0xcd60,means
            * both TX and RX inverted leads to zero prbs error.
 
            * NOTE:Bit no.6 in 0xcd60 is not considered in 40G mode.
            *      The autodetect is forced to disable.
            */
              for (lane = 0; lane < NUM_LANES; lane++) {
                  SOC_IF_ERROR_RETURN
                     (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                      PHY84740_PMAD_AER_ADDR_REG, lane));
                  SOC_IF_ERROR_RETURN
                     (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                      PHY84740_UDRCD_PRBS20_CONTROL_REGISTER,
                      &data16));
                  *value |= data16;
              }
              *value &= PHY84740_USER_PRBS_TX_INVERT;
              if( *value == PHY84740_USER_PRBS_TX_INVERT) {
                  *value = FALSE;
              } else {
                  *value = TRUE;
              }
          } else {
              return SOC_E_UNAVAIL;
          }
       }
    } else {                    /* quad-port mode */
        SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                            (pc->unit, pc->port, pc, &devid));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc81f, &speed_val));

        if (PHY84740_IS_QUADRA(devid) && (speed_val & (1 << 2))) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_AN_REG(unit, pc, 0x8019, &data16));
            *value = (data16 & (uint16) 0x0004) ? TRUE : FALSE;
        } else {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                               PHY84740_USER_PRBS_CONTROL_0_REGISTER,
                                               &data16));
            if (data16 & PHY84740_USER_PRBS_TX_INVERT) {
                *value = TRUE;
            } else {
                *value = FALSE;
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_prbs_rx_status_get(int unit, soc_port_t port,
                                      int intf, uint32 *value)
{
    phy_ctrl_t *pc;             /* PHY software state */
    uint16 data16, speed_val;
    int lane;
    uint32 devid;

    pc = EXT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                       (pc->unit, pc->port, pc, &devid));
    *value = 0;
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        if (PHY_DIAG_INTF_SYS == intf) {
            for (lane = 0; lane < NUM_LANES; lane++) {
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG, lane));
                SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
                SOC_IF_ERROR_RETURN
                    (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                            PHY84740_USER_PRBS_STATUS_0_REGISTER,&data16));
                if ((data16 & PHY84740_40G_SYS_PRBS_LOCK_MASK ) ==
                      PHY84740_40G_SYS_PRBS_LOCK_VALUE ) {
                    *value += data16 & PHY84740_40G_SYS_PRBS_ERR_MASK ;
                } else{
                    *value = -1;
                    break;
                }
            }
            SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                 PHY84740_PMAD_AER_ADDR_REG, 0));
            SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
        } else {
            if(PHY84740_IS_QUADRA(devid)) {
               *value = 0;
               for (lane = 0; lane < 4; lane++) {
                  SOC_IF_ERROR_RETURN
                      (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                       PHY84740_FORTYG_PRBS_RX_TESTING_ERROR_COUNTER_LANE0 + lane,
                       &data16));
                       *value |= data16;
               }
            } else {
                for (lane = 0; lane < 4; lane++) {
                   SOC_IF_ERROR_RETURN
                       (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                        PHY84740_FORTYG_PRBS_RECEIVE_ERROR_COUNTER_LANE0
                        + lane, &data16));
                   if (!(data16 & 0xfff)) {
                      /* PRBS is in sync */
                       continue;
                   } else {
                       /* Get errors */
                       *value += (data16 & 0xfff);
                   }
                }
            }
        }
     } else {                    /* quad-port mode */
        SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                            (pc->unit, pc->port, pc, &devid));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc81f, &speed_val));
        if (PHY84740_IS_QUADRA(devid) && (speed_val & (1 << 2))) {
            data16 = 0;
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_AN_REG(unit, pc, 0x80b1, 0x0007));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_AN_REG(unit, pc, 0x80b0, &data16));
            if (data16 & (uint16) 0x8000) {
                *value = data16 & (uint16) 0x3fff;
            } else {
                *value = -1;
            }
        } else {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                               PHY84740_USER_PRBS_STATUS_0_REGISTER,
                                               &data16));
            if (data16 == 0x8000) {     /* PRBS lock and error free */
                *value = 0;
            } else if (!(data16 & 0x8000)) {
                *value = -1;
            } else {
                *value = data16 & 0x7fff;
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_control_edc_mode_get(int unit, soc_port_t port, uint32 *value)
{
    uint16 data16;
    phy_ctrl_t *pc;             /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    /* get EDC mode */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1A, &data16));

    *value = data16 & PHY84740_EDC_MODE_MASK;

    return SOC_E_NONE;
}

STATIC int
_phy_84740_remote_loopback_set(int unit, soc_port_t port, int intf,
                               uint32 enable)
{
    phy_ctrl_t *pc;             /* PHY software state */
    int lane;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (intf == PHY_DIAG_INTF_SYS) {
        if ((PHY84740_SINGLE_PORT_MODE(pc))) {
            for (lane = 0; lane < NUM_LANES; lane++) {
                /* targeted to the system side */
                SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));

                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                    PHY84740_PMAD_AER_ADDR_REG,
                                                    lane));
                SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                                    (unit, pc, 0xcd0a, enable ? 1 : 0, 1));

                /* targeted to the system side, switch back */
                SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
            }
            /* point back to lane0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xcd0a,
                                                 enable ? 1 : 0, 1));
        }
    }
    return SOC_E_NONE;
}

STATIC int _phy_84740_debug_info(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    phy_ctrl_t *int_pc;
    int lane;
    const char *if_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
    struct dbgi {
        uint16 ucode_rev;
        uint16 pl2_reg;
        uint16 pma_mii_stat;
        uint16 tx_polarity_reg[NUM_LANES];
        uint16 rx_polarity_reg[NUM_LANES];
        soc_port_if_t iif;
        int ilink;
        int ispeed;
        int ian;
        int ian_done;
    } dbgi;

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xce00, &dbgi.ucode_rev));

    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG
         (unit, pc, PHY84740_PMAD_LANE0_L2P_REG, &dbgi.pl2_reg));

    for (lane = 0; lane < NUM_LANES; lane++) {
        /* point to the each lane starting at lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            PHY84740_PMAD_AER_ADDR_REG,
                                            P2L_MAP(pc, lane)));

        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_DIG_CTRL_REG,
                             &dbgi.tx_polarity_reg[lane]));
        SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_DIG_CTRL_REG,
                             &dbgi.rx_polarity_reg[lane]));
        SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
    }

   LOG_CLI((BSL_META_U(unit,
                       "Port %-3d(mdio=0x%x): ver(1.%04x)=0x%x, single lpmode=%d, "
                       "p2lreg(1.%04x)=0x%04x, p2lmap=%d%d%d%d\n"),
            port, pc->phy_id, 0xce00, dbgi.ucode_rev,
            SINGLE_LPMODE(pc),
            PHY84740_PMAD_LANE0_L2P_REG, dbgi.pl2_reg,
            P2L_MAP(pc, 3), P2L_MAP(pc, 2), P2L_MAP(pc, 1), P2L_MAP(pc,
            0)));

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_PMA_PMD_STATr(unit, pc, &dbgi.pma_mii_stat));
        SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &dbgi.ilink));
       LOG_CLI((BSL_META_U(unit,
                           "   pma_link(1.0x%04x)=0x%0x(%s), serdes_link=%s\n"),
                1, dbgi.pma_mii_stat,
                (dbgi.pma_mii_stat & MII_STAT_LA) ? "up" : "down",
                dbgi.ilink ? "up" : "down"));
        LOG_CLI((BSL_META_U(unit,
                            "   line polarity(1.%04x)=(ln0=%04x, "
                            "ln1=%04x, ln2=%04x, ln3=%04x)\n"),
                 PHY84740_DIG_CTRL_REG, dbgi.tx_polarity_reg[0],
                 dbgi.tx_polarity_reg[1], dbgi.tx_polarity_reg[2],
                 dbgi.tx_polarity_reg[3]));
        LOG_CLI((BSL_META_U(unit,
                            "   sys  polarity(1.%04x)=(ln0=%04x, "
                            "ln1=%04x, ln2=%04x, ln3=%04x)\n"),
                 PHY84740_DIG_CTRL_REG, dbgi.rx_polarity_reg[0],
                 dbgi.rx_polarity_reg[1], dbgi.tx_polarity_reg[2],
                 dbgi.rx_polarity_reg[3]));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "   line polarity(1.%04x)=%04x\n"), PHY84740_DIG_CTRL_REG,
                 dbgi.tx_polarity_reg[0]));
        LOG_CLI((BSL_META_U(unit,
                            "   sys  polarity(1.%04x)=%04x\n"), PHY84740_DIG_CTRL_REG,
                 dbgi.rx_polarity_reg[0]));
    }
    if (int_pc) {
        SOC_IF_ERROR_RETURN(PHY_INTERFACE_GET
                            (int_pc->pd, unit, port, &dbgi.iif));
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET
                            (int_pc->pd, unit, port, &dbgi.ispeed));
        SOC_IF_ERROR_RETURN(PHY_AUTO_NEGOTIATE_GET
                            (int_pc->pd, unit, port, &dbgi.ian,
                             &dbgi.ian_done));
       LOG_CLI((BSL_META_U(unit,
                           "   Internal: type=%s, speed=%d, an=%d\n"),
                if_names[dbgi.iif], dbgi.ispeed, dbgi.ian));
    }
   LOG_CLI((BSL_META_U(unit,
                       "=======\n")));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_control_set
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
STATIC int
_phy_84740_control_set(int unit, soc_port_t port, int intf, int lane,
                       soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t *pc;             /* PHY software state */
    phy_ctrl_t *int_pc;
    int rv;
    uint32 devid;

    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);

    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    rv = SOC_E_UNAVAIL;
    if (intf == PHY_DIAG_INTF_SYS) {
        /* if is targeted to the system side */
        SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
    }
    switch (type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
            rv = _phy_84740_control_tx_driver_set(unit, port, intf, type,
                                                  value);
            break;
        case SOC_PHY_CONTROL_EDC_MODE:
            rv = _phy_84740_control_edc_mode_set(unit, port, value);
            break;
        case SOC_PHY_CONTROL_CLOCK_ENABLE:
            rv = _phy_84740_control_recovery_clock_set(unit, port, value);
            break;
        case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
            rv = _phy_84740_control_recovery_clock_freq_set(unit, port, value);
            break;
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
            /* on line side, rx poly type is auto-detected. On system side,
             * need to enable the rx type as well.
             */
            rv = _phy_84740_control_prbs_polynomial_set(unit, port,
                                                        value, intf,
                                                        TRUE);
            if (intf == PHY_DIAG_INTF_SYS) {
                rv = _phy_84740_control_prbs_polynomial_set(unit, port, value,
                                                            intf,
                                                            FALSE);
            }
            break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
            /* on line side, rx invertion is auto-detected. On system side,
             * need to enable the rx as well.
             */
            rv = _phy_84740_control_prbs_tx_invert_data_set(unit,
                                                            port,
                                                            intf,
                                                            value);
            if (intf == PHY_DIAG_INTF_SYS) {
                rv = _phy_84740_control_prbs_rx_invert_data_set(unit, port,
                                                                intf, value);
            }
            break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
            /* fall through */
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
            /* tx/rx is enabled at the same time. no seperate control */
            rv = _phy_84740_control_prbs_enable_set(unit, port, intf, value);
            break;
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
            rv = _phy_84740_remote_loopback_set(unit, port, PHY_DIAG_INTF_SYS,
                                                value);
            break;
        case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
            if (int_pc) {
                SOC_IF_ERROR_RETURN
                    (PHY_CONTROL_SET(int_pc->pd, unit, port, type, value));
                SOFTWARE_RX_LOS(pc) = value ? 1 : 0;
                rv = SOC_E_NONE;
            }
            break;
        case SOC_PHY_CONTROL_DUMP:
            rv = _phy_84740_debug_info(unit, port);
            break;
        case SOC_PHY_CONTROL_TX_POLARITY:
            if (PHY84740_IS_QUADRA(devid)) {    /* Quadra */
                rv = phy_84740_tx_polarity_flip_set(unit, port, value);
            } else {
                rv = SOC_E_UNAVAIL;
            }
            break;
        case SOC_PHY_CONTROL_RX_POLARITY:
            if (PHY84740_IS_QUADRA(devid)) {    /* Quadra */
                rv = phy_84740_rx_polarity_flip_set(unit, port, value);
            } else {
                rv = SOC_E_UNAVAIL;
            }
            break;
        case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
            /* inside the _phy_84740_squelch_enable function, check for Quadra */
            rv = _phy_84740_squelch_enable(unit, port, intf, value, FALSE);
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }
    if (intf == PHY_DIAG_INTF_SYS) {
        /* if it is targeted to the system side, switch back */
        SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
    }
    return rv;
}

STATIC int phy_84740_tx_polarity_flip_set(int unit, int port, uint32 value)
{
    int lane;
    phy_ctrl_t *pc;
    uint16 tx_pol;
    uint16 data16, mask16;

    pc = EXT_PHY_SW_STATE(unit, port);
    tx_pol = value;

    /* get the reset default value. The 84740 device has the TX polarity flipped
     * by default. Construct the init default value 
     */

    mask16 = PHY84740_TX_POLARITY_FLIP_MASK;

    if ((PHY84740_SINGLE_PORT_MODE(pc))) {
        for (lane = 0; lane < NUM_LANES; lane++) {
            data16 = 0;

            /* point to the each lane starting at lane0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                P2L_MAP(pc, lane)));

            if ((tx_pol == POL_CONFIG_ALL_LANES) ||
                ((tx_pol & POL_CONFIG_LANE_MASK(lane)) ==
                 POL_CONFIG_LANE_MASK(lane))) {
                data16 = PHY84740_TX_POLARITY_FLIP_MASK;
            }
            SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                                PHY84740_DIG_CTRL_REG,
                                                                data16,
                                                                mask16));
        }
        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    } else {                    /* quad-port mode */
        data16 = 0;
        tx_pol = PHY84740_10G_POL_CFG_GET(pc, tx_pol);
        if (tx_pol) {
            data16 = PHY84740_TX_POLARITY_FLIP_MASK;
        }
        SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_DIG_CTRL_REG, data16, mask16));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_DIG_CTRL_REG, &data16));
    }

    return SOC_E_NONE;
}

STATIC int phy_84740_rx_polarity_flip_set(int unit, int port, uint32 value)
{
    int lane;
    phy_ctrl_t *pc;
    uint16 rx_pol;
    uint16 data16, mask16;

    pc = EXT_PHY_SW_STATE(unit, port);

    rx_pol = value;
    /* get the reset default value. The 84740 device has the TX polarity flipped
     * by default. Construct the init default value 
     */
    mask16 = PHY84740_RX_POLARITY_FLIP_MASK;

    if ((PHY84740_SINGLE_PORT_MODE(pc))) {
        for (lane = 0; lane < NUM_LANES; lane++) {
            data16 = 0;

            /* point to the each lane starting at lane0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                P2L_MAP(pc, lane)));
            if ((rx_pol == POL_CONFIG_ALL_LANES)
                || ((rx_pol & POL_CONFIG_LANE_MASK(lane)) ==
                    POL_CONFIG_LANE_MASK(lane))) {
                data16 |= PHY84740_RX_POLARITY_FLIP_MASK;
            }
            SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                                PHY84740_DIG_CTRL_REG,
                                                                data16,
                                                                mask16));
        }
        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    } else {                    /* quad-port mode */
        data16 = 0;
        rx_pol = PHY84740_10G_POL_CFG_GET(pc, rx_pol);
        if (rx_pol) {
            data16 = PHY84740_RX_POLARITY_FLIP_MASK;
        }
        SOC_IF_ERROR_RETURN(MODIFY_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_DIG_CTRL_REG, data16, mask16));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_DIG_CTRL_REG, &data16));
    }
    return SOC_E_NONE;
}

STATIC int
phy_84740_control_set(int unit, soc_port_t port,
                      soc_phy_control_t type, uint32 value)
{
    phy_ctrl_t *pc;             /* PHY software state */
    int intf;

    pc = EXT_PHY_SW_STATE(unit, port);

    intf = (REVERSE_MODE(pc) || (pc->flags & PHYCTRL_SYS_SIDE_CTRL)) ?
        PHY_DIAG_INTF_SYS : PHY_DIAG_INTF_LINE;
    SOC_IF_ERROR_RETURN
        (_phy_84740_control_set(unit, port, intf,
                                PHY_DIAG_LN_DFLT, type, value));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_control_get
 * Purpose:
 *      Get current control settign of the PHY.
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 *      type  - Control to update
 *      value - (OUT)Current setting for the control
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
_phy_84740_control_get(int unit, soc_port_t port, int intf, int lane,
                       soc_phy_control_t type, uint32 *value)
{
    phy_ctrl_t *pc;             /* PHY software state */
    int rv;
    uint16 data16;

    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);

    if (intf == PHY_DIAG_INTF_SYS) {
        /* targeted to the system side */
        SOC_IF_ERROR_RETURN(PHY84740_XFI(unit, pc));
    }
    rv = SOC_E_UNAVAIL;
    switch (type) {
        case SOC_PHY_CONTROL_PREEMPHASIS:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
            /* fall through */
        case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
            rv = _phy_84740_control_tx_driver_get(unit, port, intf, type,
                                                  value);
            break;
        case SOC_PHY_CONTROL_EDC_MODE:
            rv = _phy_84740_control_edc_mode_get(unit, port, value);
            break;
        case SOC_PHY_CONTROL_CLOCK_ENABLE:
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_RECOVERY_CLK_REG, &data16));
            *value =
                (data16 & PHY84740_RECOVERY_CLK_ENABLE_MASK) ? TRUE : FALSE;
            rv = SOC_E_NONE;
            break;
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
            rv = _phy_84740_control_prbs_polynomial_get(unit,
                                                        port,
                                                        intf,
                                                        value);
            break;
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
            rv = _phy_84740_control_prbs_tx_invert_data_get(unit,
                                                            port,
                                                            intf,
                                                            value);
            break;
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
            /* fall through */
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
            rv = _phy_84740_control_prbs_enable_get(unit, port, intf, value);
            break;
        case SOC_PHY_CONTROL_PRBS_RX_STATUS:
            rv = _phy_84740_control_prbs_rx_status_get(unit, port, intf, value);
            break;
        case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_RECOVERY_CLK_FREQ_REG, &data16));
            switch ((data16 >> PHY84740_RECOVERY_CLK_FREQ_SHIFT) & 0x3) {
                case 0x1:
                    *value = 125000;
                    break;
                case 0x2:
                    *value = 195312;
                    break;
                case 0x3:
                    *value = 156250;
                    break;
                default:
                    *value = 0x0;
                    break;
            }
            rv = SOC_E_NONE;
            break;
        case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
            *value = SOFTWARE_RX_LOS(pc);
            rv = SOC_E_NONE;
            break;

        default:
            rv = SOC_E_UNAVAIL;
            break;
    }

    if (intf == PHY_DIAG_INTF_SYS) {
        /* if it is targeted to the system side, switch back */
        SOC_IF_ERROR_RETURN(PHY84740_MMF(unit, pc));
    }
    return rv;
}

STATIC int
phy_84740_control_get(int unit, soc_port_t port,
                      soc_phy_control_t type, uint32 *value)
{
    int intf;
    phy_ctrl_t *pc;             /* PHY software state */

    pc = EXT_PHY_SW_STATE(unit, port);

    intf = (REVERSE_MODE(pc) || (pc->flags & PHYCTRL_SYS_SIDE_CTRL)) ?
        PHY_DIAG_INTF_SYS : PHY_DIAG_INTF_LINE;

    SOC_IF_ERROR_RETURN
        (_phy_84740_control_get(unit, port, intf,
                                PHY_DIAG_LN_DFLT, type, value));
    return SOC_E_NONE;
}

STATIC int phy_84740_diag_ctrl(int unit,        /* unit */
                               soc_port_t port, /* port */
                               uint32 inst,     /* the specific device block the control action directs to */
                               int op_type,     /* operation types: read,write or command sequence */
                               int op_cmd,      /* command code */
                               void *arg)
{                               /* command argument  */
    int lane;
    int intf;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_diag_ctrl: u=%d p=%d ctrl=0x%x\n"),
              unit, port, op_cmd));

    lane = PHY_DIAG_INST_LN(inst);
    intf = PHY_DIAG_INST_INTF(inst);
    if (intf == PHY_DIAG_INTF_DFLT) {
        intf = PHY_DIAG_INTF_LINE;
    }

    if (op_type == PHY_DIAG_CTRL_GET) {
        SOC_IF_ERROR_RETURN
            (_phy_84740_control_get(unit, port, intf, lane,
                                    op_cmd, (uint32 *) arg));
    } else if (op_type == PHY_DIAG_CTRL_SET) {
        SOC_IF_ERROR_RETURN
            (_phy_84740_control_set(unit, port, intf, lane,
                                    op_cmd, PTR_TO_INT(arg)));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int phy_84740_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc;
    phy_ctrl_t *int_pc;
    int rv = SOC_E_NONE;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    int_pc = INT_PHY_SW_STATE(unit, port);
    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_speed_set: u=%d p=%d speed=%d\n"),
                         unit, port, speed));

    if (FCMAP_PASSTHROUGH(pc)) {
        return SOC_E_NONE;
    }

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        if (speed != 40000) {
            return SOC_E_PARAM;
        }

        /* single port mode: doesn't use PCS. should be already in repeater mode */
        /* it is always in 40G speed, make sure internal device is in 40G */
        if (NULL != int_pc) {
            if (REVERSE_MODE(pc)) {
                if (IS_HG_PORT(unit, port)) {
                    SOC_IF_ERROR_RETURN
                        (PHY_INTERFACE_SET
                         (int_pc->pd, unit, port, SOC_PORT_IF_KR4));
                } else {
                    SOC_IF_ERROR_RETURN
                        (PHY_INTERFACE_SET
                         (int_pc->pd, unit, port, SOC_PORT_IF_SR4));
                }
            } else {
                if (soc_property_port_get(unit, port, spn_PHY_LONG_XFI, 0)) {
                    rv = PHY_INTERFACE_SET(int_pc->pd, unit, port,
                                           SOC_PORT_IF_CR);
                }
            }

            rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
        }
        return rv;
    }

    /* quad lane mode */
    if ((!(speed == 10000 || speed == 1000)) && (!PCS_REPEATER(pc))) {
        return SOC_E_PARAM;
    }

    if (speed == 10000) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_PMAD_CTRL_REG,
                                            MII_CTRL_SS_MSB | MII_CTRL_SS_LSB,
                                            MII_CTRL_SS_MSB | MII_CTRL_SS_LSB));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_PMAD_CTRL2_REG,
                                             PHY84740_PMAD_CTRL2r_PMA_TYPE_10G_LRM,
                                             PHY84740_PMAD_CTRL2r_PMA_TYPE_MASK));

    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_PMAD_CTRL2_REG,
                                             PHY84740_PMAD_CTRL2r_PMA_TYPE_1G_KX,
                                             PHY84740_PMAD_CTRL2r_PMA_TYPE_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, MII_CTRL_REG,
                                             0, MII_CTRL_SS_LSB));
    }
    /* need to set the internal phy's speed accordingly */

    if (NULL != int_pc) {
        if (REVERSE_MODE(pc)) {
            SOC_IF_ERROR_RETURN
                (PHY_INTERFACE_SET(int_pc->pd, unit, port, SOC_PORT_IF_SR));
        } else {
            /* For long XFI channel, it is best to use SFP+ DAC mode 
             * in the serdes side to provide adequate Rx equalization
             */
            if (soc_property_port_get(unit, port, spn_PHY_LONG_XFI, 0)) {
                if (!PHY84740_SINGLE_PORT_MODE(pc)) {
                    rv = PHY_INTERFACE_SET(int_pc->pd, unit, port,
                                           SOC_PORT_IF_CR);
                }
            }
        }
        rv = PHY_SPEED_SET(int_pc->pd, unit, port, speed);
    }
    return rv;
}

/*
 * Function:
 *      phy_84740_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - current link speed in Mbps
 * Returns:
 *      SOC_E_NONE
 */

STATIC int phy_84740_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;
    uint16 speed_val;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PCS_REPEATER(pc)) {
        phy_ctrl_t *int_pc;

        int_pc = INT_PHY_SW_STATE(unit, port);
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, speed));
        return SOC_E_NONE;
    } else if (FCMAP_PASSTHROUGH(pc)) {
        *speed = 10000;
        return SOC_E_NONE;
    }

    /* this register contains the speed info for both single/quad modes */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc81f, &speed_val));

    if (speed_val & (1 << 5)) {
        *speed = 40000;
    } else if (speed_val & (1 << 4)) {
        *speed = 10000;
    } else if (speed_val & (1 << 2)) {
        *speed = 1000;
    } else {                    /* default */
        if (PHY84740_SINGLE_PORT_MODE(pc)) {
            *speed = 40000;
        } else {
            *speed = 10000;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_interface_set
 * Purpose:
 *      Set either CR4 interface or SR4/LR4. The valid interface are SOC_PORT_IF_CR4
 *      and SOC_PORT_IF_XLAUI which put device in default mode: SR4/LR4 mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_UNAVAIL - unsupported interface
 */

STATIC int phy_84740_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t *pc;
    phy_ctrl_t *int_pc;
    uint16 ucode_ver;
    uint16 sig_lvl_reg[4];
    uint16 rxlos_reg[4];
    uint16 pol_reg[4];
    int lane;
    uint32 devid;
    soc_port_ability_t ability;

    int_pc = INT_PHY_SW_STATE(unit, port);
    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    if (int_pc != NULL) {
        if (REVERSE_MODE(pc)) {
            SOC_IF_ERROR_RETURN(PHY_INTERFACE_SET(int_pc->pd, unit, port, pif));
            SOC_IF_ERROR_RETURN(PHY_ABILITY_ADVERT_GET
                                (int_pc->pd, unit, port, &ability));
            SOC_IF_ERROR_RETURN(PHY_ABILITY_ADVERT_SET
                                (int_pc->pd, unit, port, &ability));
        }
    }

    if (PHY84740_IS_QUADRA(devid)) {    /* Quadra */
        if (!(PHY84740_SINGLE_PORT_MODE(pc))) { /*1G/10G mode */
            switch (pif) {
                case SOC_PORT_IF_CR:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG
                         (unit, pc, PHY84740_PMAD_CHIP_MODE_REG,
                          PHY84740_PMAD_DAC_MODE_MASK,
                          PHY84740_PMAD_DAC_MODE_MASK));
                    break;
                case SOC_PORT_IF_SR:
                case SOC_PORT_IF_LR:
                case SOC_PORT_IF_XAUI:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG
                         (unit, pc, PHY84740_PMAD_CHIP_MODE_REG, 0,
                          PHY84740_PMAD_DAC_MODE_MASK));
                    break;
                default:
                    break;
            }
        } else {                /* 40G mode */
            switch (pif) {
                case SOC_PORT_IF_CR4:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG
                         (unit, pc, PHY84740_PMAD_CHIP_MODE_REG,
                          PHY84740_PMAD_DAC_MODE_MASK,
                          PHY84740_PMAD_DAC_MODE_MASK));
                    break;
                case SOC_PORT_IF_SR4:
                case SOC_PORT_IF_LR4:
                case SOC_PORT_IF_XLAUI:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG
                         (unit, pc, PHY84740_PMAD_CHIP_MODE_REG, 0,
                          PHY84740_PMAD_DAC_MODE_MASK));
                    break;
                default:
                    break;
            }
        }
    } else {                    /* Quebec */
        if (!(PHY84740_SINGLE_PORT_MODE(pc))) { /*1G/10G mode */
            return phy_null_interface_set(unit, port, pif);
        } else {                /* 40G mode */
            switch (pif) {
                case SOC_PORT_IF_CR4:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG
                         (unit, pc, PHY84740_PMAD_CHIP_MODE_REG,
                          PHY84740_PMAD_DAC_MODE_MASK,
                          PHY84740_PMAD_DAC_MODE_MASK));
                    break;

                case SOC_PORT_IF_XLAUI:
                    SOC_IF_ERROR_RETURN
                        (MODIFY_PHY84740_MMF_PMA_PMD_REG
                         (unit, pc, PHY84740_PMAD_CHIP_MODE_REG, 0,
                          PHY84740_PMAD_DAC_MODE_MASK));
                    break;
                default:
                    break;
            }
        }
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xce00, &ucode_ver));

    if (ucode_ver >= 0x0104) {
        /* For 40G mode, Do not reset the PHY if ucode is D104 on newer */
        return SOC_E_NONE;
    }

    /* After switching mode, user need to redo the configuration */
    if (pif == SOC_PORT_IF_CR4 || pif == SOC_PORT_IF_XLAUI) {

        /* save the register settings */
        for (lane = 0; lane < NUM_LANES; lane++) {
            /* point to the each lane starting at lane0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                lane));
            SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc, PHY84740_DIG_CTRL_REG,
                                 &pol_reg[lane]));

            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                               PHY84740_PMAD_OPTICAL_CFG_REG,
                                               &rxlos_reg[lane]));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                               PHY84740_PMAD_OPTICAL_SIG_LVL_REG,
                                               &sig_lvl_reg[lane]));
        }

        /* perform IEEE reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_PMA_PMD_CTRLr(pc->unit, pc,
                                           MII_CTRL_RESET, MII_CTRL_RESET));

        sal_udelay(100);        /* Wait for 100us */

        /* restore back saved register values */
        for (lane = 0; lane < NUM_LANES; lane++) {
            /* point to the each lane starting at lane0 */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_AER_ADDR_REG,
                                                lane));
            SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc, PHY84740_DIG_CTRL_REG,
                                 pol_reg[lane]));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_OPTICAL_CFG_REG,
                                                rxlos_reg[lane]));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                PHY84740_PMAD_OPTICAL_SIG_LVL_REG,
                                                sig_lvl_reg[lane]));
        }

        /* redo Polarity configuration */
        SOC_IF_ERROR_RETURN(_phy_84740_polarity_flip_set(unit, port));

        /* point back to lane0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_interface_get
 * Purpose:
 *      Get the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_84740_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t *pc;
    uint16 mode;
    uint16 speed_val;
    uint32 devid;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG
         (unit, pc, PHY84740_PMAD_CHIP_MODE_REG, &mode));

    if (PHY84740_IS_QUADRA(devid)) {
        if (!(PHY84740_SINGLE_PORT_MODE(pc))) { /*10G mode */
            if (mode & PHY84740_PMAD_DAC_MODE_MASK) {
                *pif = SOC_PORT_IF_CR;
            } else if (mode & PHY84740_PMAD_BKPLANE_MODE_MASK) {
                SOC_IF_ERROR_RETURN
                    (READ_PHY84740_MMF_PMA_PMD_REG
                     (unit, pc, 0xc81f, &speed_val));
                if ((speed_val & (1 << 2))) {   /*10G/1G mode */
                    *pif = SOC_PORT_IF_KX;
                } else {
                    *pif = SOC_PORT_IF_KR;
                }
            } else {
                *pif = SOC_PORT_IF_XAUI;
            }
        } else {                /*40G mode */
            if (mode & PHY84740_PMAD_DAC_MODE_MASK) {
                *pif = SOC_PORT_IF_CR4;
            } else if (mode & PHY84740_PMAD_BKPLANE_MODE_MASK) {
                *pif = SOC_PORT_IF_KR4;
            } else {
                *pif = SOC_PORT_IF_XLAUI;
            }
        }
    } else {                    /*Quebec */
        if (!(PHY84740_SINGLE_PORT_MODE(pc))) {
            *pif = SOC_PORT_IF_XAUI;
        } else {                /*40G mode */
            if (mode & PHY84740_PMAD_DAC_MODE_MASK) {
                *pif = SOC_PORT_IF_CR4;
            } else {
                *pif = SOC_PORT_IF_XLAUI;
            }
        }
    }
    return SOC_E_NONE;
}

/*
 *  Function:  *      phy_84740_rom_wait
 *
 *  Purpose:
 *      Wait for data to be written to the SPI-ROM.
 *  Input:
 *      unit, port
 */
STATIC int phy_84740_rom_wait(int unit, int port)
{
    uint16 rd_data;
    uint16 wr_data;
    uint16 data16;
    int count;
    phy_ctrl_t *pc;
    soc_timeout_t to;
    int SPI_READY;

    pc = EXT_PHY_SW_STATE(unit, port);

    rd_data = 0;
    soc_timeout_init(&to, WR_TIMEOUT, 0);
    do {
        /* Write SPI Control Register Read Command. */
        count = 1;
        wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* extra reads to clear the status */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &data16));

        if (rd_data & 0x0100) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if (!(rd_data & 0x0100)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "phy_84740_rom_program: u = %d p = %d "
                              "timeout 1\n"), unit, port));
        return SOC_E_TIMEOUT;
    }

    SPI_READY = 1;
    while (SPI_READY == 1) {
        /* Set-up SPI Controller To Receive SPI EEPROM Status. */
        count = 1;
        wr_data = ((WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = SPI_CTRL_2_H;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Fill-up SPI Transmit Fifo To check SPI Status. */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write Tx Fifo Register Address. */
        wr_data = SPI_TXFIFO;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-1. */
        wr_data = ((1 * 0x0100) | MSGTYPE_HRD);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Tx Fifo Control Word-2. */
        wr_data = RDSR_OPCODE;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control Register Write Command. */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-1. */
        wr_data = 0x0101;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control Register Write Command. */
        count = 1;
        wr_data = ((WR_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = SPI_CTRL_1_H;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0103;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Wait For 64 bytes To be written.   */
        rd_data = 0x0000;
        soc_timeout_init(&to, WR_TIMEOUT, 0);
        do {
            count = 1;
            wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
            SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
            wr_data = SPI_CTRL_1_L;
            SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

            /* extra reads to clear the status */
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &data16));

            if (rd_data & 0x0100) {
                break;
            }
        } while (!soc_timeout_check(&to));
        if (!(rd_data & 0x0100)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "phy_84740_rom_program: u = %d p = %d "
                                  "timeout 2\n"), unit, port));
            return SOC_E_TIMEOUT;
        }
        /* Write SPI Control Register Read Command. */
        count = 1;
        wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        /* Write SPI Control -1 Register Address. */
        wr_data = SPI_RXFIFO;

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA12, wr_data));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &rd_data));

        /* Clear LASI Message Out Status. */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &wr_data));

        if ((rd_data & 0x1) == 0) {
            SPI_READY = 0;
        }
        /* extra reads to clear the status */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &data16));
    }                           /* SPI_READY  */
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_rom_write_enable_set
 *
 * Purpose:
 *      Enable disable protection on SPI_EEPROM
 *
 * Input:
 *      unit
 *      port
 *      enable
 * Output:
 *      SOC_E_xxx
 *
 * Notes:
 *          25AA256 256Kbit Serial EEPROM
 *          STATUS Register
 *          +------------------------------------------+
 *          | WPEN | x | x | x | BP1 | BP0 | WEL | WIP |
 *          +------------------------------------------+
 *      BP1 BP0  :   Protected Blocks
 *       0   0   :  Protect None
 *       1   1   :  Protect All
 *
 *      WEL : Write Latch Enable
 *       0  : Do not allow writes
 *       1  : Allow writes
 */

STATIC int
phy_84740_rom_write_enable_set(int unit, int port, int enable)
{
    uint16 rd_data;
    uint16 wr_data;
    uint8 wrsr_data;
    int count;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Write SPI Control Register Write Command.
     */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Address.
     */
    wr_data = SPI_CTRL_2_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Word-1.
     */
    wr_data = 0x8200;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -2 Register Word-2.
     */
    wr_data = 0x0100;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Fill-up SPI Transmit Fifo With SPI EEPROM Messages.
     * Write SPI Control Register Write Command.
     */
    count = 4;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write Tx Fifo Register Address.
     */
    wr_data = SPI_TXFIFO;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-1.
     */
    wr_data = ((1 * 0x0100) | MSGTYPE_HWR);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-2.
     */
    wr_data = ((MSGTYPE_HWR * 0x0100) | WREN_OPCODE);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-3.
     */
    wr_data = ((WRSR_OPCODE * 0x100) | (0x2));
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Tx Fifo Control Word-4.
     */
    wrsr_data = enable ? 0x2 : 0xc;
    wr_data = ((wrsr_data * 0x0100) | wrsr_data);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control Register Write Command.
     */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Address.
     */
    wr_data = SPI_CTRL_1_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Word-1.
     */
    wr_data = 0x0101;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Write SPI Control -1 Register Word-2.
     */
    wr_data = 0x0003;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /*
     * Wait For WRSR Command To be written.
     */
    SOC_IF_ERROR_RETURN(phy_84740_rom_wait(unit, port));

    return SOC_E_NONE;
}

STATIC int _phy84740_init_ucode_bcst(int unit, int port, int cmd)
{
    uint16 data16;
    uint16 mask16;
    uint16 phy_id;
    int j;
    uint16 num_words;
    int i;
    uint16 chip_mode;
    int num_lanes;
    uint8 *fw_ptr;
    int fw_length;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    num_lanes = PHY84740_SINGLE_PORT_MODE(pc) ? 4 : 1;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }
#endif

    if (cmd == PHYCTRL_UCODE_BCST_SETUP) {
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, PHY84740_PMAD_CHIP_MODE_REG,
                             &chip_mode));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84740 BCST start: u=%d p=%d\n"), unit, port));
        /* check the current chip mode, switch to quad if in single port mode */
        if ((chip_mode & PHY84740_PMAD_CHIP_MODE_MASK) ==
            PHY84740_PMAD_MODE_40G) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "PHY84740 BCST mode switch: u=%d p=%d\n"),
                                 unit, port));
            SOC_IF_ERROR_RETURN(_phy_84740_single_to_quad_mode
                                (unit, port, NULL));
        }

        /* program the bcst register. do 4 lanes for single port configuration */
        phy_id = pc->phy_id;
        /* configure all four ports  */
        for (i = 0; i < num_lanes; i++) {
            if (num_lanes > 1) {
                pc->phy_id = (phy_id & (~0x3)) + i;     /* Port 3, 2, 1 and 0 */
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc8fe, 0xffff));
        }
        /* restore the original phy_id */
        pc->phy_id = phy_id;
        return SOC_E_NONE;
    } else if (cmd == PHYCTRL_UCODE_BCST_uC_SETUP) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84740 BCST1: u=%d p=%d\n"), unit, port));
        /* clear SPA ctrl reg bit 15 and bit 13.
         * bit 15, 0-use MDIO download to SRAM, 1 SPI-ROM download to SRAM
         * bit 13, 0 clear download done status, 1 skip download
         */
        mask16 = (1 << 13) | (1 << 15);
        data16 = 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_SPA_CTRL_REG, data16, mask16));

        /* set SPA ctrl reg bit 14, 1 RAM boot, 0 internal ROM boot */
        mask16 = 1 << 14;
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_SPA_CTRL_REG, data16, mask16));

        /* misc_ctrl1 reg bit 3 to 1 for 32k downloading size */
        mask16 = 1 << 3;
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_MISC_CTRL1_REG, data16, mask16));

        /* apply bcst Reset to start download code from MDIO */
        SOC_IF_ERROR_RETURN(WRITE_PHY84740_PMA_PMD_CTRLr(unit, pc, 0x8000));
        return SOC_E_NONE;
    } else if (cmd == PHYCTRL_UCODE_BCST_ENABLE) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84740 BCST2: u=%d p=%d\n"), unit, port));

        /* reset clears bcst register */
        /* restore bcst register after reset */
        phy_id = pc->phy_id;
        /* configure all four ports  */
        for (i = 0; i < num_lanes; i++) {
            if (num_lanes > 1) {
                pc->phy_id = (phy_id & (~0x3)) + i;     /* Port 3, 2, 1 and 0 */
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc8fe, 0xffff));
        }
        /* restore the original phy_id */
        pc->phy_id = phy_id;
        return SOC_E_NONE;
    } else if (cmd == PHYCTRL_UCODE_BCST_LOAD) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_bcst,device name %s: u=%d p=%d\n"),
                             pc->dev_name ? pc->dev_name : "NULL", unit, port));
        /* find the right firmware */
        if (pc->dev_name == dev_name_84740) {
            fw_ptr = phy84740_ucode_bin;
            fw_length = phy84740_ucode_bin_len;
        } else if (pc->dev_name == dev_name_84752) {
            fw_ptr = phy84752_ucode_bin;
            fw_length = phy84752_ucode_bin_len;
        } else if (pc->dev_name == dev_name_84753) {
            fw_ptr = phy84753_ucode_bin;
            fw_length = phy84753_ucode_bin_len;
        } else if (pc->dev_name == dev_name_84754) {
            fw_ptr = phy84754_ucode_bin;
            fw_length = phy84754_ucode_bin_len;
        } else if (pc->dev_name == dev_name_84064) {
            fw_ptr = phy84064_ucode_bin;
            fw_length = phy84064_ucode_bin_len;
        } else if (pc->dev_name == dev_name_84784) {
            fw_ptr = phy84784_ucode_bin;
            fw_length = phy84784_ucode_bin_len;
        } else {
            /* invalid device name */
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "firmware_bcst,invalid device name %s: u=%d p=%d\n"),
                      pc->dev_name ? pc->dev_name : "NULL", unit, port));
            return SOC_E_NONE;
        }

        /* wait for 2ms for M8051 start and 5ms to initialize the RAM */
        sal_usleep(10000);      /* Wait for 10ms */

        /* Write Starting Address, where the Code will reside in SRAM */
        data16 = 0x8000;
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_M8051_MSGIN_REG, data16));

        /* make sure address word is read by the micro */
        sal_udelay(10);         /* Wait for 10us */

        /* Write SPI SRAM Count Size */
        data16 = (fw_length) / 2;
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_M8051_MSGIN_REG, data16));

        /* make sure read by the micro */
        sal_udelay(10);         /* Wait for 10us */

        /* Fill in the SRAM */
        num_words = (fw_length - 1);
        for (j = 0; j < num_words; j += 2) {
            /* Make sure the word is read by the Micro */
            sal_udelay(10);

            data16 = (fw_ptr[j] << 8) | fw_ptr[j + 1];

            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_M8051_MSGIN_REG, data16));
        }
        return SOC_E_NONE;
    } else if (cmd == PHYCTRL_UCODE_BCST_END) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY84740 BCST end: u=%d p=%d\n"), unit, port));

        /* make sure last code word is read by the micro */
        sal_udelay(20);

        /* first disable bcst mode */
        phy_id = pc->phy_id;
        for (i = 0; i < num_lanes; i++) {
            if (num_lanes > 1) {
                pc->phy_id = (phy_id & (~0x3)) + i;     /* Port 3, 2, 1 and 0 */
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc8fe, 0x0));

            /* Read Hand-Shake message (Done) from Micro */
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_M8051_MSGOUT_REG, &data16));

            /* Download done message */
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "u=%d p=%d MDIO firmware download done message: 0x%x\n"),
                      unit, port, data16));

            /* Clear LASI status */
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &data16));
        }
        /* restore the original phy_id */
        pc->phy_id = phy_id;

        /* Wait for LASI to be asserted when M8051 writes checksum to MSG_OUTr */
        sal_udelay(100);        /* Wait for 100 usecs */

        phy_id = pc->phy_id;
        for (i = 0; i < num_lanes; i++) {
            if (num_lanes > 1) {
                pc->phy_id = (phy_id & (~0x3)) + i;     /* Port 3, 2, 1 and 0 */
            }
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, PHY84740_PMAD_M8051_MSGOUT_REG, &data16));

            /* Need to check if checksum is correct */
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1C, &data16));

            if (data16 != 0x600D) {
                /* Bad CHECKSUM */
               LOG_CLI((BSL_META_U(unit,
                                   "firmware_bcst downlad failure: port %d "
                                   "Incorrect Checksum %x\n"), port, data16));
                return SOC_E_FAIL;
            }

            /* read Rev-ID */
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA1A, &data16));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "u=%d p=%d MDIO Firmware download revID: 0x%x\n"),
                      unit, port, data16));
        }
        /* restore the original phy_id */
        pc->phy_id = phy_id;

        return SOC_E_NONE;
    } else {
         LOG_WARN(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "u=%d p=%d firmware_bcst: invalid cmd 0x%x\n"),
                              unit, port, cmd));
    }

    return SOC_E_FAIL;
}

/*
 * Function:
 *      phy_84740_firmware_set
 * Purpose:
 *      program the given firmware into the SPI-ROM 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      offset - offset to the data stream
 *      array  - the given data
 *      datalen- the data length  
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_84740_firmware_set(int unit, int port, int offset, uint8 *array,
                       int datalen)
{
    uint16 saved_phy_addr;
    int ix;
    phy_ctrl_t *pc;
    uint32 devid;
    uint32 uCode_devid;

    /* overload this function a littl bit if array == NULL 
     * special handling for init. uCode broadcast. Internal use only 
     */
    if (array == NULL) {
        return _phy84740_init_ucode_bcst(unit, port, offset);
    }

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    /* first validate the firmware is for the configured device
     * 84740 has 1.5V from TXDRV pin, 84754 is internally wired at 1.0V.
     * If the 84754 firmware which programs chip for 1.0V is loaded
     * to the 84740, then it may damage the device and module.
     * 
     *  Chip ID is at addresses 0x7FF9, 0x7FFA, 0x7FFB.e.g. 
     *  for 84740, [0x7FF9] = 0x08, [0x7FFA] = 0x47, [0x7FFB] = 0x40. 
     *  Version number is at 0x7FFC-0x7FFD
     */

    /* length should be at least 0xFFFE */
    if (datalen < PHY84740_UCODE_DEVID_ADDR+3) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_set, wrong firmware len=0x%x : u=%d p=%d\n"),
                             datalen, unit, port));
        return SOC_E_PARAM;
    }

    /* check the firmware devid matches the configured devid */
    ix = PHY84740_UCODE_DEVID_ADDR;
    uCode_devid = (array[ix] << 16) | (array[ix + 1] << 8) | array[ix + 2];

    if (uCode_devid != devid) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "firmware_set,u=%d p=%d:  Firmware device "
                             "type(0x%x) does not match configured device type(0x%x)\n"),
                  unit, port,uCode_devid,devid));
        return SOC_E_PARAM;
    }

    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* switch to quad-mode */
        SOC_IF_ERROR_RETURN(_phy_84740_single_to_quad_mode(unit, port, NULL));
    }
    saved_phy_addr = pc->phy_id;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "firmware_set init,quad port : u=%d p=%d\n"),
                         unit, port));
    /* configure all four lanes to bcst mode */
    for (ix = 0; ix < 4; ix++) {
        pc->phy_id = (saved_phy_addr & (~0x3)) + ix;
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_BCST_REG, 0xffff));
    }
    pc->phy_id = saved_phy_addr;

    /* place all uC in reset */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA10, 0x018f));

    /* disable all port's SPI */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xC843, 0x000f));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xC840, 0x000c));

    /* Set bit 0 (spa enable) in C848 for master channel ? */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xC848, 0x2000, 0x2000));

    /* disable the bcst mode for all channels */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG
         (unit, pc, PHY84740_PMAD_BCST_REG, 0xfffe));

    SOC_IF_ERROR_RETURN
        (_phy_84740_firmware_to_rom(unit, port, offset, array, datalen));

    /* switch back to single mode */
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        /* switch to quad-mode */
        SOC_IF_ERROR_RETURN(_phy_84740_quad_to_single_mode(unit, port));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_84740_firmware_to_rom(int unit, int port, int offset, uint8 *array,
                           int datalen)
{
    uint16 rd_data;
    uint16 wr_data;
    int j;
    int i = 0;
    int count;
    uint8 spi_values[WR_BLOCK_SIZE];
    uint16 data16;
    soc_timeout_t to;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /*
     * Enable SPI for Port0
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xC843, 0x0000));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xC840, 0x0000));
    /*
     * Remove M8051 resets for port 0.
     * and wait for Micro to complete Init.
     */
    /* clear bits 3:0 */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA10, 0, 0xf));
    sal_usleep(10000);

    /* set bit(request) in 0xc848 for the required channel, remove done bit */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xC848, 0, 0x2000));

    /* enable SPI port enable */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xC848, 0xc004, 0xc004));

    /*
     * Read LASI Status registers To clear initial Failure status.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9004, &rd_data));
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9005, &rd_data));

    /*
     * Enable the LASI For Message out. 
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9000, 0x0004));
    /*
     * 0x9002, bit 2 Is Rx Alarm enabled For LASI. 
     */
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9002, 0x0004));
    /*
     * Read Any Residual Message out register.
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &rd_data));
    /*
     * Clear LASI Message Out Status. 
     */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &rd_data));

    /* set SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(phy_84740_rom_write_enable_set(unit, port, 1));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "init0: u=%d p=%d\n"),
                         unit, port));

    for (j = 0; j < datalen; j += WR_BLOCK_SIZE) {
        /*
         * Setup SPI Controller. 
         */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Address. */
        wr_data = SPI_CTRL_2_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Word-1. */
        wr_data = 0x8200;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -2 Register Word-2. */
        wr_data = 0x0100;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Fill-up SPI Transmit Fifo.
         * Write SPI Control Register Write Command.
         */
        count = 4 + (WR_BLOCK_SIZE / 2);
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write Tx Fifo Register Address. */
        wr_data = SPI_TXFIFO;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-1. */
        wr_data = ((1 * 0x0100) | MSGTYPE_HWR);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-2. */
        wr_data = ((MSGTYPE_HWR * 0x0100) | WREN_OPCODE);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-3. */
        wr_data = ((WR_OPCODE * 0x0100) | (0x3 + WR_BLOCK_SIZE));
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Tx Fifo Control Word-4. */
        wr_data = (((j & 0x00FF) * 0x0100) | ((j & 0xFF00) / 0x0100));
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "loop: u=%d p=%d,inxj: %d,inxi:%d\n"),
                             unit, port,j,i));

        if (datalen < (j + WR_BLOCK_SIZE)) {    /* last block */
            sal_memset(spi_values, 0, WR_BLOCK_SIZE);
            sal_memcpy(spi_values, &array[j], datalen - j);

            for (i = 0; i < WR_BLOCK_SIZE; i += 2) {
                /* Write SPI Tx Fifo Data Word-4. */
                wr_data = ((spi_values[i + 1] * 0x0100) | spi_values[i]);
                SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
            }
        } else {
            for (i = 0; i < WR_BLOCK_SIZE; i += 2) {
                /* Write SPI Tx Fifo Data Word-4. */
                wr_data = ((array[j + i + 1] * 0x0100) | array[j + i]);
                SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
            }
        }

        /* 
         * Set-up SPI Controller To Transmit.
         * Write SPI Control Register Write Command.
         */
        count = 2;
        wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -1 Register Word-1. */
        wr_data = 0x0501;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Write SPI Control -1 Register Word-2. */
        wr_data = 0x0003;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* Wait For 64 bytes To be written.   */
        SOC_IF_ERROR_RETURN(phy_84740_rom_wait(unit, port));

    }                           /* SPI_WRITE */

    /* clear SPI-ROM write enable */
    SOC_IF_ERROR_RETURN(phy_84740_rom_write_enable_set(unit, port, 0));

    /* Disable SPI EEPROM. */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Address. */
    wr_data = SPI_CTRL_2_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Word-1. */
    wr_data = 0x8200;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -2 Register Word-2. */
    wr_data = 0x0100;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Fill-up SPI Transmit Fifo With SPI EEPROM Messages. */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write Tx Fifo Register Address. */
    wr_data = SPI_TXFIFO;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Tx Fifo Control Word-1. */
    wr_data = ((0x1 * 0x0100) | MSGTYPE_HWR);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Tx Fifo Control Word-2. */
    wr_data = WRDI_OPCODE;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control Register Write Command. */
    count = 2;
    wr_data = ((WR_CPU_CTRL_FIFO * 0x0100) | count);
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Address. */
    wr_data = SPI_CTRL_1_L;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Word-1. */
    wr_data = 0x0101;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Write SPI Control -1 Register Word-2. */
    wr_data = 0x0003;
    SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

    /* Wait For WRDI Command To be written */
    rd_data = 0;
    soc_timeout_init(&to, WR_TIMEOUT, 0);
    do {
        /* Write SPI Control Register Read Command. */
        count = 1;
        wr_data = ((RD_CPU_CTRL_REGS * 0x0100) | count);
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));
        wr_data = SPI_CTRL_1_L;
        SOC_IF_ERROR_RETURN(write_message(unit, pc, wr_data, &rd_data));

        /* extra reads to clear the status */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &data16));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &data16));

        if (rd_data & 0x0100) {
            break;
        }
    } while (!soc_timeout_check(&to));

    if (!(rd_data & 0x0100)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "phy_84740_rom_program: u = %d p = %d "
                              "WRDI command timeout\n"), unit, port));
        return SOC_E_TIMEOUT;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_rom_program: u=%d p=%d done\n"), unit, port));

    return SOC_E_NONE;
}

static int
write_message(int unit, phy_ctrl_t *pc, uint16 wrdata, uint16 *rddata)
{

    uint16 tmp_data = 0;
    soc_timeout_t to;

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA12, wrdata));

    soc_timeout_init(&to, WR_TIMEOUT, 0);
    do {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9005, &tmp_data));
        if (tmp_data & 0x4)
            break;
    } while (!soc_timeout_check(&to));
    if (!(tmp_data & 0x4)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "write_message failed: wrdata %04x\n"), wrdata));
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xCA13, &tmp_data));
    *rddata = tmp_data;
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x9003, &tmp_data));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_precondition_before_probe
 * Purpose:
 *      When the device is in 40G, ie. after a power reset, and the 10G
 *      port being probed does not have the 40G address in PRTAD, the
 *      device will not respond until the port that has the 40G address
 *      is probed. To avoid not programming ports correctly, make
 *      sure the device is in quad (4x10G) PMD mode when the SDK port is 10G.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */

STATIC int
phy_84740_precondition_before_probe(int unit, phy_ctrl_t *pc)
{
    int ids;
    int rv = SOC_E_NOT_FOUND;
    uint16 saved_phy_id;
    uint16 id0, id1;
    uint32 devid;

    /* Not required when the port is single PMD */
    if (PHY84740_SINGLE_PORT_MODE(pc)) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PMD must not be called when in single PMD\n")));
        return rv;
    }

    saved_phy_id = pc->phy_id;

    /* Look for first address that responds */
    for (ids = 0; ids < 4; ids++) {

        pc->phy_id = (saved_phy_id & ~0x3) + ids;
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Try device with address %x\n"), pc->phy_id));

        SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                            (pc->unit, pc->port, pc, &devid));

        
        /* 
         * Make sure the device has the correct dev IDs 
         * Note that this is being called either because the soc probe found
         * the ids or there was an id override. In the later case, the device
         * might not have the 84740 ids so this PMD setup will not be supported
         */
        id0 = 0;
        READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 
                                      2 /*PHY84740_DEV1_PMD_IDENTIFIER_REGISTER_0 */,
                                      &id0);
        if (id0 == 0xffff) {
            continue;
        }
        id1 = 0;
        READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 
                                      3 /*PHY84740_DEV1_PMD_IDENTIFIER_REGISTER_1 */,
                                      &id1);
        if (id1 == 0xffff) {
            continue;
        }

        switch(devid) {
            case PHY84740_ID_84740:
            case PHY84740_ID_84752:
            case PHY84740_ID_84753:
            case PHY84740_ID_84754:
            case PHY84740_ID_84064:
                break;
            default:
               if (PHY84740_IS_QUADRA(devid)) {
                    break;
               } else {  /* not found */
                    LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "port %d: BCM84xxx type PHY device detected, please use "
                                     "phy_84<xxx> config variable to select the specific type\n"),
                          pc->port));
                    continue;
               }
        }

        /* switch to quad-mode */
        SOC_IF_ERROR_RETURN(_phy_84740_single_to_quad_mode(unit, pc->port, pc));

        /* Found the address that responds and the device now is in 4x10G */
        rv = SOC_E_NONE;
        break;
    }

    pc->phy_id = saved_phy_id;

    return rv;
}


/*
 * Function:
 *      phy_84740_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */
STATIC int phy_84740_probe(int unit, phy_ctrl_t *pc)
{
    uint32 devid;

    SOC_IF_ERROR_RETURN(_phy_84740_config_devid
                        (pc->unit, pc->port, pc, &devid));

    if (devid == PHY84740_ID_84740) {
        pc->dev_name = dev_name_84740;
    } else if (devid == PHY84740_ID_84752) {
        pc->dev_name = dev_name_84752;
    } else if (devid == PHY84740_ID_84753) {
        pc->dev_name = dev_name_84753;
    } else if (devid == PHY84740_ID_84754) {
        pc->dev_name = dev_name_84754;
    } else if (devid == PHY84740_ID_84064) {
        pc->dev_name = dev_name_84064;
    } else if (PHY84740_IS_QUADRA(devid)) {
        /*pc->dev_name = already set;*/
    } else {  /* not found */
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "port %d: BCM84xxx type PHY device detected, please use "
                             "phy_84<xxx> config variable to select the specific type\n"),
                  pc->port));
        return SOC_E_NOT_FOUND;
    }
    pc->size = sizeof(PHY84740_DEV_DESC_t);
    return SOC_E_NONE;
}

STATIC int
_phy_84740_bsc_rw(int unit, soc_port_t port, int dev_addr, int opr,
                  int addr, int count, void *data_array, uint32 ram_start)
{
    phy_ctrl_t *pc;
    soc_timeout_t to;
    uint16 data16;
    int i;
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    sal_usecs_t start;
    sal_usecs_t end;
#endif                          /* defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT */
    int access_type;
    int data_type;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84740_bsc_read: u=%d p=%d addr=%04x\n"),
                         unit, port, addr));

    if (!data_array) {
        return SOC_E_PARAM;
    }

    if (count > PHY84740_BSC_XFER_MAX) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    data_type = PHY84740_I2C_DATA_TYPE(opr);
    access_type = PHY84740_I2C_ACCESS_TYPE(opr);

    if (access_type == PHY84740_I2CDEV_WRITE) {
        for (i = 0; i < count; i++) {
            if (data_type == PHY84740_I2C_8BIT) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, ram_start + i,
                                                    ((uint8 *) data_array)[i]));
            } else {            /* 16 bit */
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, ram_start + i,
                                                    ((uint16 *)
                                                     data_array)[i]));
            }
        }
    }

    data16 = ram_start;
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x8004, data16));
    SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x8003, addr));
    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x8002, count));

    data16 = 1;
    data16 |= (dev_addr << 9);
    if (access_type == PHY84740_I2CDEV_WRITE) {
        data16 |= PHY84740_WR_FREQ_400KHZ;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x8005, data16));

    if (access_type == PHY84740_I2CDEV_WRITE) {
        data16 = 0x8000 | PHY84740_BSC_WRITE_OP;
    } else {
        data16 = 0x8000 | PHY84740_BSC_READ_OP;
    }

    if (data_type == PHY84740_I2C_16BIT) {
        data16 |= (1 << 12);
    }

    /* for single port mode, there should be only one I2C interface active
     * from lane0. The 0x800x register block is bcst type registers. If writing
     * to 0x8000 directly, it will enable all four I2C masters. Use indirect access
     * to enable only the lane 0.
     */

    if (PHY84740_SINGLE_PORT_MODE(pc)) {

        /* point to lane 0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, PHY84740_PMAD_AER_ADDR_REG, 0));

        /* indirect access: 0x8000=data16 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xcd0c, 0x8000));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xcd0d, data16));

        /* issue the indirect access command */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xcd13, 0x3));

        soc_timeout_init(&to, 1000000, 0);
        while (!soc_timeout_check(&to)) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xcd13, &data16));
            if (!data16) {
                break;
            }
        }
        if (data16) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "port %d: Fail register 0xcd13 status 0x%x\n"),
                                 port,data16));
            return SOC_E_TIMEOUT; 
        }
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x8000, data16));
    }
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    start = sal_time_usecs();
#endif                          /* defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT */
    soc_timeout_init(&to, 1000000, 0);
    while (!soc_timeout_check(&to)) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0x8000, &data16));
        if (((data16 & PHY84740_2W_STAT) == PHY84740_2W_STAT_COMPLETE)) {
            break;
        }
    }
#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    end = sal_time_usecs();
#endif                          /* defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT */
    /* need some delays */
    sal_usleep(10000);

#if defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT)
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "BSC command status %d time=%d\n"),
                         (data16 & PHY84740_2W_STAT), SAL_USECS_SUB(end, start)));
#endif /* defined(BROADCOM_DEBUG) || defined(DEBUG_PRINT */

    if (access_type == PHY84740_I2CDEV_WRITE) {
        return SOC_E_NONE;
    }

    if ((data16 & PHY84740_2W_STAT) == PHY84740_2W_STAT_COMPLETE) {
        for (i = 0; i < count; i++) {
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, (ram_start + i), &data16));
            if (data_type == PHY84740_I2C_16BIT) {
                ((uint16 *)data_array)[i] = data16;
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "%04x "), data16));
            } else {
                ((uint8 *)data_array)[i] = (uint8)data16;
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "%02x "), data16));
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Read a slave device such as NVRAM/EEPROM connected to the 84740's I2C
 * interface. This function will be mainly used for diagnostic or workaround
 * purpose.
 * Note:
 * The size of read_array buffer must be greater than or equal to the
 * parameter nbytes.
 * usage example:
 *   Retrieve the first 100 byte data of the non-volatile storage device with
 *   I2C bus device address 0x50(default SPF eeprom I2C address) on unit 0,
 *   port 2.
 *   uint8 data8[100];
 *   phy_84740_i2cdev_read(0,2,0x50,0,100,data8);
 */
int phy_84740_i2cdev_read(int unit, soc_port_t port, int dev_addr,      /* 7 bit I2C bus device address */
                          int offset,   /* starting data address to read */
                          int nbytes,   /* number of bytes to read */
                          uint8 *read_array)
{                               /* buffer to hold retrieved data */
    return _phy_84740_bsc_rw(unit, port, dev_addr, PHY84740_I2CDEV_READ,
                             offset, nbytes, (void *)read_array,
                             PHY84740_READ_START_ADDR);

}

/*
 * Write to a slave device such as NVRAM/EEPROM connected to the 84740's I2C
 * interface. This function will be mainly used for diagnostic or workaround
 * purpose.
 * Note:
 * The size of write_array buffer should be equal to the parameter nbytes.
 * The EEPROM may limit the maximun write size to 16 bytes
 * usage example:
 *   Write to first 100 byte space of the non-volatile storage device with
 *   I2C bus device address 0x50(default SPF eeprom I2C address) on unit 0,
 *   port 2, with written data specified in array data8.
 *   uint8 data8[100];
 *   *** initialize the data8 array with written data ***
 *
 *   phy_84740_i2cdev_write(0,2,0x50,0,100,data8);
 */

int phy_84740_i2cdev_write(int unit, soc_port_t port, int dev_addr,     /* I2C bus device address */
                           int offset,  /* starting data address to write to */
                           int nbytes,  /* number of bytes to write */
                           uint8 *write_array)
{                               /* buffer to hold written data */
    int j;
    int rv = SOC_E_NONE;

    for (j = 0; j < (nbytes / PHY84740_BSC_WR_MAX); j++) {
        rv = _phy_84740_bsc_rw(unit, port, dev_addr, PHY84740_I2CDEV_WRITE,
                               offset + j * PHY84740_BSC_WR_MAX,
                               PHY84740_BSC_WR_MAX,
                               (void *)(write_array + j * PHY84740_BSC_WR_MAX),
                               PHY84740_WRITE_START_ADDR);
        if (rv != SOC_E_NONE) {
            return rv;
        }
        sal_usleep(20000);
    }
    if (nbytes % PHY84740_BSC_WR_MAX) {
        rv = _phy_84740_bsc_rw(unit, port, dev_addr, PHY84740_I2CDEV_WRITE,
                               offset + j * PHY84740_BSC_WR_MAX,
                               nbytes % PHY84740_BSC_WR_MAX,
                               (void *)(write_array + j * PHY84740_BSC_WR_MAX),
                               PHY84740_WRITE_START_ADDR);
    }
    return rv;
}

/*
 *   Callout interface to phy_84740_i2cdev_read(); 
 *   TBD: Extend to handle max_size > PHY84740_BSC_XFER_MAX. 
 */

STATIC int
phy_84740_multi_get(int unit, soc_port_t port, uint32 flags,
                    uint32 dev_addr, uint32 offset, int max_size, uint8 *data,
                    int *actual_size)
{
    int rv;

    rv = phy_84740_i2cdev_read(unit, port, (int)dev_addr, (int)offset, max_size,
                               data);

    if (SOC_FAILURE(rv)) {
        *actual_size = 0;
        return SOC_E_FAIL;
    }

    *actual_size = max_size;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_84740_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      port         - port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_84740_reg_read(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16 data16;
    uint16 regdata;
    phy_ctrl_t *pc;             /* PHY software state */
    int rv = SOC_E_NONE;
    int rd_cnt;

    pc = EXT_PHY_SW_STATE(unit, port);

    rd_cnt = 1;

    if (flags & SOC_PHY_I2C_DATA8) {

        SOC_IF_ERROR_RETURN
            (phy_84740_i2cdev_read(unit, port,
                                   SOC_PHY_I2C_DEVAD(phy_reg_addr),
                                   SOC_PHY_I2C_REGAD(phy_reg_addr),
                                   rd_cnt, (uint8 *) &data16));
        *phy_data = *((uint8 *) &data16);

    } else if (flags & SOC_PHY_I2C_DATA16) {
        /* This operation is generally targeted to access 16-bit device,
         * such as PHY IC inside the SFP.  However there is no 16-bit
         * scratch register space on the device.  Use 1.800e
         * for this operation.
         */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_I2C_TEMP_RAM,
                                           &regdata));

        rv = _phy_84740_bsc_rw(unit, port,
                               SOC_PHY_I2C_DEVAD(phy_reg_addr),
                               PHY84740_I2C_OP_TYPE(PHY84740_I2CDEV_READ,
                                                    PHY84740_I2C_16BIT),
                               SOC_PHY_I2C_REGAD(phy_reg_addr), 1,
                               (void *)&data16, PHY84740_I2C_TEMP_RAM);

        *phy_data = data16;

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_I2C_TEMP_RAM,
                                            regdata));
    } else {
        SOC_IF_ERROR_RETURN(READ_PHY_REG(unit, pc, phy_reg_addr, &data16));
        *phy_data = data16;
    }

    return rv;
}

/*
 * Function:
 *      phy_84740_reg_write
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
STATIC int
phy_84740_reg_write(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data)
{
    uint8 data8[4];
    uint16 data16[2];
    uint16 regdata[2];
    phy_ctrl_t *pc;             /* PHY software state */
    int rv = SOC_E_NONE;
    int wr_cnt;

    pc = EXT_PHY_SW_STATE(unit, port);
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
    if ( SOC_WARM_BOOT(pc->unit) || SOC_IS_RELOADING(pc->unit) ) {
        return SOC_E_NONE;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    wr_cnt = 1;

    if (flags & SOC_PHY_I2C_DATA8) {
        data8[0] = (uint8) phy_data;
        SOC_IF_ERROR_RETURN
            (phy_84740_i2cdev_write(unit, port,
                                    SOC_PHY_I2C_DEVAD(phy_reg_addr),
                                    SOC_PHY_I2C_REGAD(phy_reg_addr),
                                    wr_cnt, data8));
    } else if (flags & SOC_PHY_I2C_DATA16) {
        /* This operation is generally targeted to access 16-bit device,
         * such as PHY IC inside the SFP.  However there is no 16-bit
         * scratch register space on the device.  Use 1.800e
         * for this operation.
         */
        /* save the temp ram register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_I2C_TEMP_RAM,
                                           &regdata[0]));
        data16[0] = phy_data;
        rv = _phy_84740_bsc_rw(unit, port,
                               SOC_PHY_I2C_DEVAD(phy_reg_addr),
                               PHY84740_I2C_OP_TYPE(PHY84740_I2CDEV_WRITE,
                                                    PHY84740_I2C_16BIT),
                               SOC_PHY_I2C_REGAD(phy_reg_addr), wr_cnt,
                               (void *)data16, PHY84740_I2C_TEMP_RAM);

        /* restore the ram register value */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, PHY84740_I2C_TEMP_RAM,
                                            regdata[0]));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, phy_reg_addr, (uint16) phy_data));
    }

    return rv;
}

int
phy_84740_timesync_config_set(int unit, soc_port_t port,
                              soc_port_phy_timesync_config_t *conf)
{
    uint16 rx_control_reg = 0, tx_control_reg = 0, rx_tx_control_reg = 0,
        capture_en_reg = 0, rx_ts_cap = 0, tx_ts_cap = 0,
        nse_sc_8 = 0, nse_nco_3 = 0, value, mpls_control_reg = 0, temp = 0,
        gmode = 0, framesync_mode = 0, syncout_mode = 0, mask = 0,
        pulse_1_length, pulse_2_length;
    uint32 interval;
    int i;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FLAGS) {

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_ENABLE) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc600, 0x0003));
            nse_sc_8 |= (1U << 12);
        }

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE) {
            capture_en_reg |= 3U;
        }

        /* TXRX SOP TS CAPTURE ENABLE reg */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc607, capture_en_reg, 3U));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE) {
            nse_sc_8 |= (1U << 13);
        }

        /* NSE SC 8 register */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63a, nse_sc_8,
                                             (1U << 12) | (1U << 13)));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE) {
            rx_tx_control_reg |= (1U << 3);
        }

        /* RX TX CONTROL register */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc61c, rx_tx_control_reg, (1U << 3)));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE) {
            rx_control_reg |= (1U << 3);
            tx_control_reg |= (1U << 3);
        }

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_L2_ENABLE) {
            rx_control_reg |= (1U << 2);
            tx_control_reg |= (1U << 2);
        }

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_IP4_ENABLE) {
            rx_control_reg |= (1U << 1);
            tx_control_reg |= (1U << 1);
        }

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_IP6_ENABLE) {
            rx_control_reg |= (1U << 0);
            tx_control_reg |= (1U << 0);
        }

        /* TX CONTROL register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc618, tx_control_reg));

        /* RX CONTROL register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc61a, rx_control_reg));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT) {
            nse_nco_3 &= ~(1U << 14);
        } else {
            nse_nco_3 |= (1U << 14);
        }

        /* NSE NCO 3 register */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc630, nse_nco_3,
                                             (1U << 14)));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_64BIT_TIMESTAMP_ENABLE) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG
                 (unit, pc, 0xc609, (uint16) 0x2));
            SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                                (unit, pc, 0xc608, (uint16) 0x2));
        }
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_SYNC) {
            tx_ts_cap |= (0x1U << 0);
        }
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_DELAY_REQ) {
            tx_ts_cap |= (0x1U << 1);
        }
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_REQ) {
            tx_ts_cap |= (0x1U << 2);
        }
        if (conf->
            flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_RESP) {
            tx_ts_cap |= (0x1U << 3);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc602, tx_ts_cap));

        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_SYNC) {
            rx_ts_cap |= (0x1U << 0);
        }
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_DELAY_REQ) {
            rx_ts_cap |= (0x1U << 1);
        }
        if (conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_REQ) {
            rx_ts_cap |= (0x1U << 2);
        }
        if (conf->
            flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_RESP) {
            rx_ts_cap |= (0x1U << 3);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc604, rx_ts_cap));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID) {
        /* VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc61d, conf->itpid));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID) {
        /* OUTER VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc61e, conf->otpid));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2) {
        /* INNER VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc61f, conf->otpid2));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc636, conf->ts_divider & 0xfff));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc605, conf->rx_link_delay & 0xffff));
        SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc606,
                             (conf->rx_link_delay >> 16) & 0xffff));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE) {

        /* TIME CODE 5 register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc610,
              (uint16) (conf->original_timecode.nanoseconds & 0xffff)));

        /* TIME CODE 4 register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc60f,
              (uint16) ((conf->original_timecode.nanoseconds >> 16) & 0xffff)));

        /* TIME CODE 3 register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc60e,
              (uint16) (COMPILER_64_LO(conf->original_timecode.seconds) &
                        0xffff)));

        /* TIME CODE 2 register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc60d,
              (uint16) ((COMPILER_64_LO(conf->original_timecode.seconds) >> 16)
                        & 0xffff)));

        /* TIME CODE 1 register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc60c,
              (uint16) (COMPILER_64_HI(conf->original_timecode.seconds) &
                        0xffff)));
    }

    mask = 0;

    /* NSE SC 8 register */

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE) {
        _phy_84740_encode_gmode(conf->gmode, &gmode);
        mask |= (0x3 << 14);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE) {
        _phy_84740_encode_framesync_mode(conf->framesync.mode, &framesync_mode);
        mask |= (0xf << 2) | (0x1 << 11) | (0x1 << 6);
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc63c, conf->framesync.length_threshold));
        SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc63b, conf->framesync.event_offset));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE) {
        _phy_84740_encode_syncout_mode(conf->syncout.mode, &syncout_mode);
        mask |= (0x3 << 0);

        /* Divide by 6.4 without floating point */
        interval = (conf->syncout.interval * 10) / 64;
        pulse_1_length = (conf->syncout.pulse_1_length * 10) / 64;
        pulse_2_length = (conf->syncout.pulse_2_length * 10) / 64;

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc635, interval & 0xffff));
        SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc634,
                             ((pulse_2_length & 0x3) << 14) | ((interval >> 16)
                                                               & 0x3fff)));
        SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc633,
                             ((pulse_1_length & 0x1ff) << 7) |
                             ((pulse_2_length >> 2) & 0x7f)));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc639,
                                            (uint16) (COMPILER_64_LO
                                                      (conf->syncout.
                                                       syncout_ts) & 0xfff0)));
        SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc638,
                             (uint16) ((COMPILER_64_LO(conf->syncout.syncout_ts)
                                        >> 16) & 0xffff)));
        SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc637,
                             (uint16) (COMPILER_64_HI(conf->syncout.syncout_ts)
                                       & 0xffff)));
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                         0xc63a,
                                         (gmode << 14) |
                                         ((framesync_mode & 0x2) ? (1U << 11) : (1U << 6)) |   /* if bits 1 or 2 are set, turn off syncout */
                                         (framesync_mode << 2) |
                                         (syncout_mode << 0),
                                         mask));

    mask = 0;

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET) {
        /* RX TS OFFSET register */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc60b, (uint16) (conf->rx_timestamp_offset & 0xffff)));
        mask |= 0xf000;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET) {
        mask |= 0x0fff;
    }

    /* TXRX TS OFFSET register */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG
         (unit, pc, 0xc60a,
          (uint16) (((conf->rx_timestamp_offset & 0xf0000) >> 4) | (conf->
                                                                    tx_timestamp_offset
                                                                    & 0xfff)),
          mask));
    value = 0;
    mask = 0;
    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE) {
        _phy_84740_encode_egress_message_mode(conf->tx_sync_mode, 0, &value);
        mask |= 0x3 << 0;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE) {
        _phy_84740_encode_egress_message_mode(conf->tx_delay_request_mode, 2,
                                              &value);
        mask |= 0x3 << 2;
    }

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE) {
        _phy_84740_encode_egress_message_mode(conf->tx_pdelay_request_mode, 4,
                                              &value);
        mask |= 0x3 << 4;
    }

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE) {
        _phy_84740_encode_egress_message_mode(conf->tx_pdelay_response_mode, 6,
                                              &value);
        mask |= 0x3 << 6;
    }

    /* TX EVENT MESSAGE MODE SEL register */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc601, value, mask));

    value = 0;
    mask = 0;

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE) {
        _phy_84740_encode_ingress_message_mode(conf->rx_sync_mode, 0, &value);
        mask |= 0x3 << 0;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE) {
        _phy_84740_encode_ingress_message_mode(conf->rx_delay_request_mode, 2,
                                               &value);
        mask |= 0x3 << 2;
    }

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE) {
        _phy_84740_encode_ingress_message_mode(conf->rx_pdelay_request_mode, 4,
                                               &value);
        mask |= 0x3 << 4;
    }

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE) {
        _phy_84740_encode_ingress_message_mode(conf->rx_pdelay_response_mode, 6,
                                               &value);
        mask |= 0x3 << 6;
    }

    /* RX EVENT MESSAGE MODE SEL register */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc603, value, mask));

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {

        /* Initial ref phase [15:0] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc623,
              (uint16) (COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) &
                        0xffff)));

        /* Initial ref phase [31:16]  */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc622,
              (uint16) ((COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) >> 16) &
                        0xffff)));

        /*  Initial ref phase [47:32] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc621,
              (uint16) (COMPILER_64_HI(conf->phy_1588_dpll_ref_phase) &
                        0xffff)));
    }

    if (conf->
        validity_mask &
        SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA) {
        SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc625,
                             conf->phy_1588_dpll_ref_phase_delta & 0xffff));
        SOC_IF_ERROR_RETURN(WRITE_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc624,
                             (conf->
                              phy_1588_dpll_ref_phase_delta >> 16) & 0xffff));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
        /* DPLL K1 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc626, conf->phy_1588_dpll_k1 & 0xff));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
        /* DPLL K2 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc627, conf->phy_1588_dpll_k2 & 0xff));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
        /* DPLL K3 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc628, conf->phy_1588_dpll_k3 & 0xff));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL) {

        /* MPLS controls */
        if (conf->mpls_control.flags & SOC_PORT_PHY_TIMESYNC_MPLS_ENABLE) {
            mpls_control_reg |= (1U << 7) | (1U << 3);
        }

        if (conf->mpls_control.
            flags & SOC_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE) {
            mpls_control_reg |= (1U << 6) | (1U << 2);
        }

        if (conf->mpls_control.
            flags & SOC_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE) {
            mpls_control_reg |= (1U << 4);
        }

        if (conf->mpls_control.
            flags & SOC_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE) {
            mpls_control_reg |= (1U << 5) | (1U << 1);
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc651, mpls_control_reg,
                                             (1U << 7) | (1U << 3) | (1U << 6) |
                                             (1U << 2) | (1U << 4) | (1U << 5) |
                                             (1U << 1)));

        /* special label [19:16] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc653,
              (uint16) ((conf->mpls_control.special_label >> 16) & 0xf)));

        /* special label [15:0] */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc652,
              (uint16) (conf->mpls_control.special_label & 0xffff)));

        for (i = 0; i < 10; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                0xc654 + i,
                                                (uint16) (conf->mpls_control.labels[i].value & 0xffff)));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                0xc661 + i,
                                                (uint16) (conf->mpls_control.labels[i].mask & 0xffff)));
        }

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            0xc65e, 
                                            (uint16) (((conf->mpls_control.labels[3].value >> 4) & 0xf000) |
                                                      ((conf->mpls_control.labels[2].value >> 8) & 0x0f00) |
                                                      ((conf->mpls_control.labels[1].value >> 12) & 0x00f0) |
                                                      ((conf->mpls_control.labels[0].value >> 16) & 0x000f))));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            0xc65f,
                                            (uint16) (((conf->mpls_control.labels[7].value >> 4) & 0xf000) |
                                                      ((conf->mpls_control.labels[6].value >> 8) & 0x0f00) |
                                                      ((conf->mpls_control.labels[5].value >> 12) & 0x00f0) |
                                                      ((conf->mpls_control.labels[4].value >> 16) & 0x000f))));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            0xc660,
                                            (uint16) (((conf->mpls_control.labels[9].value >> 12) & 0x00f0) |
                                                      ((conf->mpls_control.labels[8].value >> 16) & 0x000f))));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            0xc66b,
                                            (uint16) (((conf->mpls_control.labels[3].mask >> 4) & 0xf000) |
                                                      ((conf->mpls_control.labels[2].mask >> 8) & 0x0f00) |
                                                      ((conf->mpls_control.labels[1].mask >> 12) & 0x00f0) |
                                                      ((conf->mpls_control.labels[0].mask >> 16) & 0x000f))));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            0xc66c,
                                            (uint16) (((conf->mpls_control.labels[7].mask >> 4) & 0xf000) |
                                                      ((conf->mpls_control.labels[6].mask >> 8) & 0x0f00) |
                                                      ((conf->mpls_control.labels[5].mask >> 12) & 0x00f0) |
                                                      ((conf->mpls_control.labels[4].mask >> 16) & 0x000f))));

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                            0xc66d,
                                            (uint16) (((conf->mpls_control.labels[9].mask >> 12) & 0x00f0) |
                                                      ((conf->mpls_control.labels[8].mask >> 16) & 0x000f))));

        /* label flags */
        temp = 0;
        for (i = 0; i < 8; i++) {
            temp |=
                conf->mpls_control.labels[i].flags & SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT ? (0x2 << (i << 1)) : 0;
            temp |=
                (conf->mpls_control.labels[i].flags & SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN) ? (0x1 << (i << 1)) : 0;
        }

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc66e, temp));

        temp = 0;
        for (i = 0; i < 2; i++) {
            temp |=
                (conf->mpls_control.labels[i + 8].flags & SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT) ? (0x2 << (i << 1)) : 0;
            temp |=
                (conf->mpls_control.labels[i + 8].flags & SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN) ? (0x1 << (i << 1)) : 0;
        }

        SOC_IF_ERROR_RETURN
            (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc66f, temp));
    }
    return SOC_E_NONE;
}

int
phy_84740_timesync_config_get(int unit, soc_port_t port,
                              soc_port_phy_timesync_config_t *conf)
{
    uint16 rx_tx_control_reg = 0, tx_control_reg = 0,
        en_control_reg = 0, capture_en_reg = 0, rx_ts_cap = 0, tx_ts_cap = 0,
        nse_sc_8 = 0, nse_nco_3 =
        0, temp1, temp2, temp3, value, mpls_control_reg;
    soc_port_phy_timesync_global_mode_t gmode = SOC_PORT_PHY_TIMESYNC_MODE_FREE;
    soc_port_phy_timesync_framesync_mode_t framesync_mode =
        SOC_PORT_PHY_TIMESYNC_FRAMESYNC_NONE;
    soc_port_phy_timesync_syncout_mode_t syncout_mode =
        SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE;
    int i;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    conf->flags = 0;

    /* NSE SC 8 register */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63a, &nse_sc_8));

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FLAGS) {
        /* SLICE ENABLE CONTROL register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc600, &en_control_reg));

        if (en_control_reg & 1U) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_ENABLE;
        }

        /* TXRX SOP TS CAPTURE ENABLE reg */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc607, &capture_en_reg));

        if (capture_en_reg & 1U) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;
        }

        /* NSE SC 8 register */
        /* SOC_IF_ERROR_RETURN
           (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63a, &nse_sc_8));
         */

        if (nse_sc_8 & (1U << 13)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE;
        }

        /* RX TX CONTROL register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc61c, &rx_tx_control_reg));

        if (rx_tx_control_reg & (1U << 3)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE;
        }

        /* TX CONTROL register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc618, &tx_control_reg));

        if (tx_control_reg & (1U << 3)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE;
        }

        if (tx_control_reg & (1U << 2)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_L2_ENABLE;
        }

        if (tx_control_reg & (1U << 1)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_IP4_ENABLE;
        }

        if (tx_control_reg & (1U << 0)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_IP6_ENABLE;
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc630, &nse_nco_3));

        if (!(nse_nco_3 & (1U << 14))) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc609, &temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc608, &temp2));
        if (temp2 == 0x2 && temp1 == 0x2) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_64BIT_TIMESTAMP_ENABLE;
        }
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc602, &tx_ts_cap));
        if (tx_ts_cap & (0x1U << 0)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_SYNC;
        }
        if (tx_ts_cap & (0x1U << 1)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_DELAY_REQ;
        }
        if (tx_ts_cap & (0x1U << 2)) {
            conf->flags |=
                SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_REQ;
        }
        if (tx_ts_cap & (0x1U << 3)) {
            conf->flags |=
                SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_RESP;
        }

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc604, &rx_ts_cap));
        if (rx_ts_cap & (0x1U << 0)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_SYNC;
        }
        if (rx_ts_cap & (0x1U << 1)) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_DELAY_REQ;
        }
        if (rx_ts_cap & (0x1U << 2)) {
            conf->flags |=
                SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_REQ;
        }
        if (rx_ts_cap & (0x1U << 3)) {
            conf->flags |=
                SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_RESP;
        }
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE) {
        _phy_84740_decode_gmode((nse_sc_8 >> 14), &gmode);
        conf->gmode = gmode;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE) {
        _phy_84740_decode_framesync_mode((nse_sc_8 >> 2), &framesync_mode);
        conf->framesync.mode = framesync_mode;
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63c, &temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63b, &temp2));
        conf->framesync.length_threshold = temp1;
        conf->framesync.event_offset = temp2;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE) {
        _phy_84740_decode_syncout_mode((nse_sc_8 >> 0), &syncout_mode);
        conf->syncout.mode = syncout_mode;

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc635, &temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc634, &temp2));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc633, &temp3));
        /* Multiply by 6.4 without floating point */
        conf->syncout.pulse_1_length = (((temp3 >> 7) & 0x1ff) * 64) / 10;
        conf->syncout.pulse_2_length =
            ((((temp3 & 0x7f) << 2) | ((temp2 >> 14) & 0x3)) * 64) / 10;
        conf->syncout.interval = ((((temp2 & 0x3fff) << 16) | temp1) * 64) / 10;

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc639, &temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc638, &temp2));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc637, &temp3));

        COMPILER_64_SET(conf->syncout.syncout_ts, ((uint32) temp3),
                        (((uint32) temp2 << 16) | ((uint32) temp1)));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID) {
        /* VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc61d, &conf->itpid));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID) {
        /* OUTER VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc61e, &conf->otpid));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2) {
        /* INNER VLAN TAG register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc61f, &conf->otpid2));
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc636, &conf->ts_divider));
        conf->ts_divider &= 0xfff;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc605, &temp1));
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc606, &temp2));

        conf->rx_link_delay = ((uint32) temp2 << 16) | temp1;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE) {
        /* TIME CODE 5 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc610, &temp1));

        /* TIME CODE 4 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc60f, &temp2));

        conf->original_timecode.nanoseconds = ((uint32) temp2 << 16) | temp1;

        /* TIME CODE 3 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc60e, &temp1));

        /* TIME CODE 2 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc60d, &temp2));

        /* TIME CODE 1 register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc60c, &temp3));

        /* conf->original_timecode.seconds = ((uint64)temp3 << 32) | ((uint32)temp2 << 16) | temp1; */

        COMPILER_64_SET(conf->original_timecode.seconds, ((uint32) temp3),
                        (((uint32) temp2 << 16) | ((uint32) temp1)));
    }

    /* TXRX TS OFFSET register */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc60a, &temp1));

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET) {
        /* RX TS OFFSET register */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc60b, &temp2));
        conf->rx_timestamp_offset = (((uint32) (temp1 & 0xf000)) << 4) | temp2;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET) {
        conf->tx_timestamp_offset = temp1 & 0xfff;
    }

    /* TX EVENT MESSAGE MODE SEL register */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc601, &value));

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE) {
        _phy_84740_decode_egress_message_mode(value, 0, &conf->tx_sync_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE) {
        _phy_84740_decode_egress_message_mode(value, 2,
                                              &conf->tx_delay_request_mode);
    }

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE) {
        _phy_84740_decode_egress_message_mode(value, 4,
                                              &conf->tx_pdelay_request_mode);
    }

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE) {
        _phy_84740_decode_egress_message_mode(value, 6,
                                              &conf->tx_pdelay_response_mode);
    }

    /* RX EVENT MESSAGE MODE SEL register */
    SOC_IF_ERROR_RETURN
        (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc603, &value));

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE) {
        _phy_84740_decode_ingress_message_mode(value, 0, &conf->rx_sync_mode);
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE) {
        _phy_84740_decode_ingress_message_mode(value, 2,
                                               &conf->rx_delay_request_mode);
    }

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE) {
        _phy_84740_decode_ingress_message_mode(value, 4,
                                               &conf->rx_pdelay_request_mode);
    }

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE) {
        _phy_84740_decode_ingress_message_mode(value, 6,
                                               &conf->rx_pdelay_response_mode);
    }

    if (conf->
        validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {
        /* Initial ref phase [15:0] */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc623, &temp1));

        /* Initial ref phase [31:16] */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc622, &temp2));

        /* Initial ref phase [47:32] */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc621, &temp3));

        /* conf->phy_1588_dpll_phase_initial = ((uint64)temp3 << 32) | ((uint32)temp2 << 16) | temp1; */

        COMPILER_64_SET(conf->phy_1588_dpll_ref_phase, ((uint32) temp3),
                        (((uint32) temp2 << 16) | ((uint32) temp1)));
    }
    if (conf->
        validity_mask &
        SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA) {
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc625, &temp1));
        SOC_IF_ERROR_RETURN(READ_PHY84740_MMF_PMA_PMD_REG
                            (unit, pc, 0xc624, &temp2));

        conf->phy_1588_dpll_ref_phase_delta = ((uint32) temp2 << 16) | temp1;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
        /* DPLL K1 */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc626, &conf->phy_1588_dpll_k1));
        conf->phy_1588_dpll_k1 &= 0xff;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
        /* DPLL K2 */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc627, &conf->phy_1588_dpll_k2));
        conf->phy_1588_dpll_k2 &= 0xff;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
        /* DPLL K3 */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc628, &conf->phy_1588_dpll_k3));
        conf->phy_1588_dpll_k3 &= 0xff;
    }

    if (conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL) {
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG
             (unit, pc, 0xc651, &mpls_control_reg));

        conf->mpls_control.flags = 0;

        if (mpls_control_reg & (1U << 7)) {
            conf->mpls_control.flags |= SOC_PORT_PHY_TIMESYNC_MPLS_ENABLE;
        }

        if (mpls_control_reg & (1U << 6)) {
            conf->mpls_control.flags |=
                SOC_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE;
        }

        if (mpls_control_reg & (1U << 4)) {
            conf->mpls_control.flags |=
                SOC_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE;
        }

        if (mpls_control_reg & (1U << 5)) {
            conf->mpls_control.flags |=
                SOC_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE;
        }

        /* special label [19:16] */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc653, &temp2));

        /* special label [15:0] */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc652, &temp1));

        conf->mpls_control.special_label =
            ((uint32) (temp2 & 0x0f) << 16) | temp1;

        for (i = 0; i < 10; i++) {

            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc654 + i, &temp1));
            conf->mpls_control.labels[i].value = temp1;

            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc661 + i, &temp1));
            conf->mpls_control.labels[i].mask = temp1;
        }

        /* now get [19:16] of labels */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc65e, &temp1));

        conf->mpls_control.labels[0].value |= ((uint32) (temp1 & 0x000f) << 16);
        conf->mpls_control.labels[1].value |= ((uint32) (temp1 & 0x00f0) << 12);
        conf->mpls_control.labels[2].value |= ((uint32) (temp1 & 0x0f00) << 8);
        conf->mpls_control.labels[3].value |= ((uint32) (temp1 & 0xf000) << 4);

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc65f, &temp1));

        conf->mpls_control.labels[4].value |= ((uint32) (temp1 & 0x000f) << 16);
        conf->mpls_control.labels[5].value |= ((uint32) (temp1 & 0x00f0) << 12);
        conf->mpls_control.labels[6].value |= ((uint32) (temp1 & 0x0f00) << 8);
        conf->mpls_control.labels[7].value |= ((uint32) (temp1 & 0xf000) << 4);

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc660, &temp1));

        conf->mpls_control.labels[8].value |= ((uint32) (temp1 & 0x000f) << 16);
        conf->mpls_control.labels[9].value |= ((uint32) (temp1 & 0x00f0) << 12);

        /* now get [19:16] of masks */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc66b, &temp1));

        conf->mpls_control.labels[0].mask |= ((uint32) (temp1 & 0x000f) << 16);
        conf->mpls_control.labels[1].mask |= ((uint32) (temp1 & 0x00f0) << 12);
        conf->mpls_control.labels[2].mask |= ((uint32) (temp1 & 0x0f00) << 8);
        conf->mpls_control.labels[3].mask |= ((uint32) (temp1 & 0xf000) << 4);

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc66c, &temp1));
        conf->mpls_control.labels[4].mask |= ((uint32) (temp1 & 0x000f) << 16);
        conf->mpls_control.labels[5].mask |= ((uint32) (temp1 & 0x00f0) << 12);
        conf->mpls_control.labels[6].mask |= ((uint32) (temp1 & 0x0f00) << 8);
        conf->mpls_control.labels[7].mask |= ((uint32) (temp1 & 0xf000) << 4);

        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc66d, &temp1));

        conf->mpls_control.labels[8].mask |= ((uint32) (temp1 & 0x000f) << 16);
        conf->mpls_control.labels[9].mask |= ((uint32) (temp1 & 0x00f0) << 12);

        /* label flags */
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc66e, &temp1));
        for (i = 0; i < 8; i++) {
            conf->mpls_control.labels[i].flags |=
                temp1 & (0x2 << (i << 1)) ? SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT: 0;
            conf->mpls_control.labels[i].flags |=
                temp1 & (0x1 << (i << 1)) ? SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN: 0;
        }
        SOC_IF_ERROR_RETURN
            (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc66f, &temp2));
        for (i = 0; i < 2; i++) {
            conf->mpls_control.labels[i + 8].flags |=
                temp2 & (0x2 << (i << 1)) ? SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT: 0;
            conf->mpls_control.labels[i + 8].flags |=
                temp2 & (0x1 << (i << 1)) ? SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN: 0;
        }

    }
    return SOC_E_NONE;
}

int
phy_84740_timesync_control_set(int unit, soc_port_t port,
                               soc_port_control_phy_timesync_t type,
                               uint64 value)
{
    uint16 temp1, temp2;
    uint32 value0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch (type) {

        case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:
        case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:
            return SOC_E_FAIL;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                0xc62f,
                                                (uint16) (COMPILER_64_LO(value) & 0xffff)));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                0xc62e,
                                                (uint16) ((COMPILER_64_LO(value) >> 16) & 0xffff)));
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC:

            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63a, &temp1));
            temp2 = ((temp1 |
                     (0x3 << 14) |
                     (0x1 << 12)) &
                     ~(0xf << 2)) |
                     (0x1 << 5);
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63a, temp2));
            sal_usleep(1);
            temp2 &= ~((0x1 << 5) | (0x1 << 12));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63a, temp2));
            sal_usleep(1);
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63a, temp1));
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
            /* COMPILER_64_SHR(value, 4); *//* shifting 4 bits extra, not required */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                0xc632,
                                                (uint16) (COMPILER_64_LO(value) & 0xffff)));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                0xc631,
                                                (uint16) ((COMPILER_64_LO(value) >> 16) &
                                                0xffff)));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                 0xc630,
                                                 (uint16) COMPILER_64_HI(value),
                                                 0x0fff));
            break;
        case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:
            temp1 = 0;
            temp2 = 0;

            value0 = COMPILER_64_LO(value);

            if (value0 & SOC_PORT_PHY_TIMESYNC_TN_LOAD) {
                temp1 |= 1U << 11;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD) {
                temp2 |= 1U << 11;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD) {
                temp1 |= 1U << 10;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD) {
                temp2 |= 1U << 10;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD) {
                temp1 |= 1U << 9;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD) {
                temp2 |= 1U << 9;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD) {
                temp1 |= 1U << 8;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD) {
                temp2 |= 1U << 8;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD) {
                temp1 |= 1U << 7;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD) {
                temp2 |= 1U << 7;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD) {
                temp1 |= 1U << 6;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD) {
                temp2 |= 1U << 6;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD) {
                temp1 |= 1U << 5;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD) {
                temp2 |= 1U << 5;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD) {
                temp1 |= 1U << 4;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD) {
                temp2 |= 1U << 4;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD) {
                temp1 |= 1U << 3;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD) {
                temp2 |= 1U << 3;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD) {
                temp1 |= 1U << 2;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD) {
                temp2 |= 1U << 2;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD) {
                temp1 |= 1U << 1;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD) {
                temp2 |= 1U << 1;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD) {
                temp1 |= 1U << 0;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD) {
                temp2 |= 1U << 0;
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc615, temp1));

            /* always load not supported at the moment */
            /*
               SOC_IF_ERROR_RETURN
               (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc615, temp2));
             */

            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT:
            temp1 = 0;

            value0 = COMPILER_64_LO(value);

            if (value0 & SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT) {
                temp1 |= 1U << 1;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT) {
                temp1 |= 1U << 0;
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc617, temp1));
            break;
        case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK:
            temp1 = 0;

            value0 = COMPILER_64_LO(value);

            if (value0 & SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK) {
                temp1 |= 1U << 1;
            }
            if (value0 & SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK) {
                temp1 |= 1U << 0;
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc616, temp1));
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
            value0 = COMPILER_64_LO(value);

            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                 0xc60a,
                                                 (uint16) (value0 & 0x0fff),
                                                 0x0fff));
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
            value0 = COMPILER_64_LO(value);

            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                0xc60b,
                                                (uint16) (value0 & 0xffff)));
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY84740_MMF_PMA_PMD_REG(unit, pc,
                                                 0xc60a,
                                                 (uint16) ((value0 >> 4) & 0xf000), 0xf000));
            break;
        default:
            return SOC_E_NONE;
            break;
    }

    return SOC_E_NONE;
}

int
phy_84740_timesync_control_get(int unit, soc_port_t port,
                               soc_port_control_phy_timesync_t type,
                               uint64 *value)
{
    uint16 value0 = 0;
    uint16 value1 = 0;
    uint16 value2 = 0;
    uint16 value3 = 0;
    uint32 value4 = 0;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    switch (type) {
        case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:

            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63d, 0x4));

            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc647, &value0));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc646, &value1));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc645, &value2));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63d, 0x8));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63d, 0x0));

            /*    *value = (((uint64)value3) << 48) | (((uint64)value2) << 32) | (((uint64)value1) << 16) | ((uint64)value0); */
            COMPILER_64_SET((*value),
                            (((uint32) value3 << 16) | ((uint32) value2)),
                            (((uint32) value1 << 16) | ((uint32) value0)));
            break;
        case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:

            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63d, 0x1));

            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc640, &value0));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63f, &value1));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63e, &value2));

            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63d, 0x2));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc63d, 0x0));

            /*   *value = (((uint64)value3) << 48) | (((uint64)value2) << 32) | (((uint64)value1) << 16) | ((uint64)value0); */
            COMPILER_64_SET((*value),
                            (((uint32) value3 << 16) | ((uint32) value2)),
                            (((uint32) value1 << 16) | ((uint32) value0)));
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc62f, &value0));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc62e, &value1));
            COMPILER_64_SET((*value), 0,
                            (((uint32) value1 << 16) | ((uint32) value0)));
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc632, &value0));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc631, &value1));
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc630, &value2));
            COMPILER_64_SET((*value), ((uint32) (value2 & 0x0fff)),
                            (((uint32) value1 << 16) | ((uint32) value0)));
            COMPILER_64_SHL((*value), 4);
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:

            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc615, &value1));
            /* always load not supported at the moment */
            /*
               SOC_IF_ERROR_RETURN
               (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc615, &value2));
             */
            value2 = 0;

            value4 = 0;

            if (value1 & (1U << 11)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_TN_LOAD;
            }
            if (value2 & (1U << 11)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD;
            }
            if (value1 & (1U << 10)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD;
            }
            if (value2 & (1U << 10)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD;
            }
            if (value1 & (1U << 9)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD;
            }
            if (value2 & (1U << 9)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD;
            }
            if (value1 & (1U << 8)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD;
            }
            if (value1 & (1U << 8)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD;
            }
            if (value1 & (1U << 7)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD;
            }
            if (value1 & (1U << 7)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD;
            }
            if (value1 & (1U << 6)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD;
            }
            if (value2 & (1U << 6)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD;
            }
            if (value1 & (1U << 5)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD;
            }
            if (value2 & (1U << 5)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD;
            }
            if (value1 & (1U << 4)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD;
            }
            if (value2 & (1U << 4)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD;
            }
            if (value1 & (1U << 3)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD;
            }
            if (value2 & (1U << 3)) {
                value4 |=
                    SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD;
            }
            if (value1 & (1U << 2)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD;
            }
            if (value2 & (1U << 2)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD;
            }
            if (value1 & (1U << 1)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD;
            }
            if (value2 & (1U << 1)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD;
            }
            if (value1 & (1U << 0)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD;
            }
            if (value2 & (1U << 0)) {
                value4 |= SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD;
            }
            COMPILER_64_SET((*value), 0, value4);

            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT:
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc617, &value1));

            value0 = 0;

            if (value1 & (1U << 1)) {
                value0 |= SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT;
            }
            if (value1 & (1U << 0)) {
                value0 |= SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT;
            }
            COMPILER_64_SET((*value), 0, (uint32) value0);

            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK:
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc616, &value1));
            value0 = 0;

            if (value1 & (1U << 1)) {
                value0 |= SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK;
            }
            if (value1 & (1U << 0)) {
                value0 |= SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK;
            }
            COMPILER_64_SET((*value), 0, (uint32) value0);

            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc60a, &value0));

            COMPILER_64_SET((*value), 0, (uint32) (value0 & 0x0fff));
            break;

        case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc60b, &value0));

            SOC_IF_ERROR_RETURN
                (READ_PHY84740_MMF_PMA_PMD_REG(unit, pc, 0xc60a, &value1));

            COMPILER_64_SET((*value), 0,
                            (uint32) (((((uint32) value1) << 4) & 0xf0000) |
                                      value0));
            break;

        default:
            return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

void
_phy_84740_encode_egress_message_mode(soc_port_phy_timesync_event_message_egress_mode_t mode,
                                      int offset, uint16 *value)
{

    switch (mode) {
        case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE:
            *value |= (0x0 << offset);
            break;
        case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD:
            *value |=
                (0x1 << offset);
            break;
        case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN:
            *value |=
                (0x2 << offset);
            break;
        case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP:
            *value |= (0x3 << offset);
            break;
    }

}

void
_phy_84740_encode_ingress_message_mode(soc_port_phy_timesync_event_message_ingress_mode_t mode,
                                       int offset, uint16 *value)
{

    switch (mode) {
        case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE:
            *value |= (0x0 << offset);
            break;
        case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD:
            *value |=
                (0x1 << offset);
            break;
        case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP:
            *value |= (0x2 << offset);
            break;
        case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME:
            *value |= (0x3 << offset);
            break;
    }

}

void _phy_84740_decode_egress_message_mode(uint16 value, int offset,
                                           soc_port_phy_timesync_event_message_egress_mode_t
                                           *mode)
{

    switch ((value >> offset) & 0x3) {
        case 0x0:
            *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE;
            break;
        case 0x1:
            *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD;
            break;
        case 0x2:
            *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN;
            break;
        case 0x3:
            *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP;
            break;
    }

}

void _phy_84740_decode_ingress_message_mode(uint16 value, int offset,
                                            soc_port_phy_timesync_event_message_ingress_mode_t
                                            *mode)
{

    switch ((value >> offset) & 0x3) {
        case 0x0:
            *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE;
            break;
        case 0x1:
            *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD;
            break;
        case 0x2:
            *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP;
            break;
        case 0x3:
            *mode = SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME;
            break;
    }

}

void _phy_84740_encode_gmode(soc_port_phy_timesync_global_mode_t mode,
                             uint16 *value)
{

    switch (mode) {
        case SOC_PORT_PHY_TIMESYNC_MODE_FREE:
            *value = 0x1;
            break;
        case SOC_PORT_PHY_TIMESYNC_MODE_SYNCIN:
            *value = 0x2;
            break;
        case SOC_PORT_PHY_TIMESYNC_MODE_CPU:
            *value = 0x3;
            break;
        default:
            break;
    }

}

void _phy_84740_decode_gmode(uint16 value,
                             soc_port_phy_timesync_global_mode_t *mode)
{

    switch (value & 0x3) {
        case 0x1:
            *mode = SOC_PORT_PHY_TIMESYNC_MODE_FREE;
            break;
        case 0x2:
            *mode = SOC_PORT_PHY_TIMESYNC_MODE_SYNCIN;
            break;
        case 0x3:
            *mode = SOC_PORT_PHY_TIMESYNC_MODE_CPU;
            break;
        default:
            break;
    }

}

void _phy_84740_encode_framesync_mode(soc_port_phy_timesync_framesync_mode_t
                                      mode, uint16 *value)
{

    switch (mode) {
        case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0:
            *value = 1U << 0;
            break;
        case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1:
            *value = 1U << 1;
            break;
        case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT:
            *value = 1U << 2;
            break;
        case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU:
            *value = 1U << 3;
            break;
        default:
            break;
    }

}

void _phy_84740_decode_framesync_mode(uint16 value,
                                      soc_port_phy_timesync_framesync_mode_t *mode)
{

    switch (value & 0xf) {
        case 1U << 0:
            *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0;
            break;
        case 1U << 1:
            *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1;
            break;
        case 1U << 2:
            *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT;
            break;
        case 1U << 3:
            *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU;
            break;
        default:
            break;
    }

}

void _phy_84740_encode_syncout_mode(soc_port_phy_timesync_syncout_mode_t mode,
                                    uint16 *value)
{

    switch (mode) {
        case SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE:
            *value = 0x0;
            break;
        case SOC_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME:
            *value = 0x1;
            break;
        case SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN:
            *value = 0x2;
            break;
        case SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC:
            *value = 0x3;
            break;
    }

}

void _phy_84740_decode_syncout_mode(uint16 value,
                                    soc_port_phy_timesync_syncout_mode_t *mode)
{

    switch (value & 0x3) {
        case 0x0:
            *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE;
            break;
        case 0x1:
            *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME;
            break;
        case 0x2:
            *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN;
            break;
        case 0x3:
            *mode = SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC;
            break;
    }

}

/*
 * Variable:
 *    phy_84740_drv
 * Purpose:
 *    Phy Driver for 40G PHY.
 */

phy_driver_t phy_84740drv_xe = {
    "84740 10-Gigabit PHY Driver",
    phy_84740_init,             /* Init */
    phy_null_reset,             /* Reset (dummy) */
    phy_84740_link_get,         /* Link get   */
    phy_84740_enable_set,       /* Enable set */
    phy_null_enable_get,        /* Enable get */
    phy_null_set,               /* Duplex set */
    phy_null_one_get,           /* Duplex get */
    phy_84740_speed_set,        /* Speed set  */
    phy_84740_speed_get,        /* Speed get  */
    phy_null_set,               /* Master set */
    phy_null_zero_get,          /* Master get */
    phy_84740_an_set,           /* ANA set */
    phy_84740_an_get,           /* ANA get */
    NULL,                       /* Local Advert set */
    NULL,                       /* Local Advert get */
    phy_null_mode_get,          /* Remote Advert get */
    phy_84740_lb_set,           /* PHY loopback set */
    phy_84740_lb_get,           /* PHY loopback set */
    phy_84740_interface_set,    /* IO Interface set */
    phy_84740_interface_get,    /* IO Interface get */
    NULL,                       /* PHY abilities mask */
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_null_medium_get,
    NULL,                       /* phy_cable_diag  */
    NULL,                       /* phy_link_change */
    phy_84740_control_set,      /* phy_control_set */
    phy_84740_control_get,      /* phy_control_get */
    phy_84740_reg_read,         /* phy_reg_read */
    phy_84740_reg_write,        /* phy_reg_write */
    NULL,                       /* phy_reg_modify */
    NULL,                       /* phy_notify */
    phy_84740_probe,            /* pd_probe  */
    phy_84740_ability_advert_set,       /* pd_ability_advert_set */
    phy_84740_ability_advert_get,       /* pd_ability_advert_get */
    phy_84740_ability_remote_get,       /* pd_ability_remote_get */
    phy_84740_ability_local_get,        /* pd_ability_local_get  */
    phy_84740_firmware_set,     /* pd_firmware_set */
    phy_84740_timesync_config_set,      /* pd_timesync_config_set */
    phy_84740_timesync_config_get,      /* pd_timesync_config_get */
    phy_84740_timesync_control_set,     /* pd_timesync_control_set */
    phy_84740_timesync_control_get,     /* pd_timesync_control_get */
    phy_84740_diag_ctrl,        /* pd_diag_ctrl */
    NULL,                       /* pd_lane_control_set */
    NULL,                       /* pd_lane_control_get */
    NULL,                       /* pd_lane_reg_read */
    NULL,                       /* pd_lane_reg_write */
    NULL,                       /* pd_oam_config_set */
    NULL,                       /* pd_oam_config_get */
    NULL,                       /* pd_oam_control_set */
    NULL,                       /* pd_oam_control_get */
    NULL,                       /* pd_timesync_enhanced_capture_get */
    phy_84740_multi_get,         /* pd_multi_get */
    phy_84740_precondition_before_probe /* pd_precondition_before_probe */
};

#else                           /* INCLUDE_PHY_84740 */
typedef int _phy_84740_not_empty; /* Make ISO compilers happy. */
#endif                          /* INCLUDE_PHY_84740 */

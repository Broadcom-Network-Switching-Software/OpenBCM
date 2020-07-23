/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef   PHY_UNIMAC_H_
#define   PHY_UNIMAC_H_

#include "phy_mac_ctrl.h"
#define BIT_U(_n)     (1U << (_n))
extern int phy_unimac_mac_init(phy_mac_ctrl_t *mmc, int dev_port);

extern int phy_unimac_mac_auto_cfg(phy_mac_ctrl_t *mmc,
                                 int dev_port, int fval);

extern int phy_unimac_mac_reset(phy_mac_ctrl_t *mmc,
                              int dev_port, int fval);

extern int phy_unimac_mac_enable_set(phy_mac_ctrl_t *mmc,
                                   int dev_port, int en);

extern int phy_unimac_mac_enable_get(phy_mac_ctrl_t *mmc,
                                   int dev_port, int *en) ;

extern int phy_unimac_mac_speed_set(phy_mac_ctrl_t *mmc,
                                  int dev_port, int speed);

extern int phy_unimac_mac_speed_get(phy_mac_ctrl_t *mmc,
                                  int dev_port, int *speed);

extern int phy_unimac_mac_duplex_set(phy_mac_ctrl_t *mmc,
                                   int dev_port, int duplex);

extern int phy_unimac_mac_duplex_get(phy_mac_ctrl_t *mmc,
                                   int dev_port, int *fd);

extern int phy_unimac_mac_pause_set(phy_mac_ctrl_t *mmc,
                                      int dev_port, int pause);

extern int phy_unimac_mac_update_ipg(phy_mac_ctrl_t *mmc,
                               int dev_port, int ipg);

extern int phy_unimac_mac_max_frame_set(phy_mac_ctrl_t *mmc,
                                      int dev_port, uint32 max_frame);

extern int phy_unimac_mac_max_frame_get(phy_mac_ctrl_t *mmc,
                                      int dev_port, uint32 *flen);

extern int phy_unimac_mac_pause_frame_fwd_set(phy_mac_ctrl_t *mmc,
                                        int dev_port, int pause_fwd);
extern int phy_unimac_mac_rx_enable_set(phy_mac_ctrl_t *mmc, int dev_port, int enable);
extern int phy_unimac_mac_tx_enable_set(phy_mac_ctrl_t *mmc, int dev_port, int enable);
extern int phy_unimac_mac_tx_datapath_flush(phy_mac_ctrl_t *mmc, int dev_port, int enable);


/*
 * Command Register
 */

#define UNIMAC_REG_CMD_CFG_TXEN_SHIFT             0
#define UNIMAC_REG_CMD_CFG_RXEN_SHIFT             1
#define UNIMAC_REG_CMD_CFG_SPEED_SHIFT            2
#define UNIMAC_REG_CMD_CFG_PROMIS_EN_SHIFT        4
#define UNIMAC_REG_CMD_CFG_CRC_FWD_SHIFT          6
#define UNIMAC_REG_CMD_CFG_PAUSE_FWD_SHIFT        7
#define UNIMAC_REG_CMD_CFG_RX_PAUSE_IGNORE_SHIFT  8
#define UNIMAC_REG_CMD_CFG_DUPLEX_SHIFT           10
#define UNIMAC_REG_CMD_CFG_SW_RESET_SHIFT         13
#define UNIMAC_REG_CMD_CFG_SW_OVERRIDE_TX_SHIFT   17
#define UNIMAC_REG_CMD_CFG_SW_OVERRIDE_RX_SHIFT   18
#define UNIMAC_REG_CMD_CFG_ENA_EXA_CONFIG_SHIFT   22
#define UNIMAC_REG_CMD_CTL_FRM_ENA_SHIFT          23
#define UNIMAC_REG_CMD_CTL_FRM_NO_LGTH_CHECK      24
#define UNIMAC_REG_CMD_CFG_TX_PAUSE_IGNORE_SHIFT  28
#define UNIMAC_REG_CMD_CFG_RUNT_FILTER_SHIFT      31

#define UNIMAC_REG_CMD_CFG_SPEED_SHIFT_L        UNIMAC_REG_CMD_CFG_SPEED_SHIFT
#define UNIMAC_REG_CMD_CFG_SPEED_SHIFT_H       (UNIMAC_REG_CMD_CFG_SPEED_SHIFT + 1)

#define UNIMAC_REG_CMD_CFG_TXEN_MASK            BIT_U(UNIMAC_REG_CMD_CFG_TXEN_SHIFT)
#define UNIMAC_REG_CMD_CFG_RXEN_MASK            BIT_U(UNIMAC_REG_CMD_CFG_RXEN_SHIFT)
#define UNIMAC_REG_CMD_CFG_SPEED_MASK          (BIT_U(UNIMAC_REG_CMD_CFG_SPEED_SHIFT_L) \
                                               |BIT_U(UNIMAC_REG_CMD_CFG_SPEED_SHIFT_H))
#define UNIMAC_REG_CMD_CFG_PROMIS_EN_MASK       BIT_U(UNIMAC_REG_CMD_CFG_PROMIS_EN_SHIFT)
#define UNIMAC_REG_CMD_CFG_CRC_FWD_MASK         BIT_U(UNIMAC_REG_CMD_CFG_CRC_FWD_SHIFT)
#define UNIMAC_REG_CMD_CFG_PAUSE_FWD_MASK       BIT_U(UNIMAC_REG_CMD_CFG_PAUSE_FWD_SHIFT)
#define UNIMAC_REG_CMD_CFG_RX_PAUSE_IGNORE_MASK BIT_U(UNIMAC_REG_CMD_CFG_RX_PAUSE_IGNORE_SHIFT)
#define UNIMAC_REG_CMD_CFG_DUPLEX_MASK          BIT_U(UNIMAC_REG_CMD_CFG_DUPLEX_SHIFT)
#define UNIMAC_REG_CMD_CFG_SW_RESET_MASK        BIT_U(UNIMAC_REG_CMD_CFG_SW_RESET_SHIFT)
#define UNIMAC_REG_CMD_CFG_SW_OVERRIDE_TX_MASK  BIT_U(UNIMAC_REG_CMD_CFG_SW_OVERRIDE_TX_SHIFT)
#define UNIMAC_REG_CMD_CFG_SW_OVERRIDE_RX_MASK  BIT_U(UNIMAC_REG_CMD_CFG_SW_OVERRIDE_RX_SHIFT)
#define UNIMAC_REG_CMD_CFG_ENA_EXA_CONFIG_MASK  BIT_U(UNIMAC_REG_CMD_CFG_ENA_EXA_CONFIG_SHIFT)
#define UNIMAC_REG_CMD_CFG_CTL_FRM_ENA_MASK     BIT_U(UNIMAC_REG_CMD_CTL_FRM_ENA_SHIFT)
#define UNIMAC_REG_CMD_CFG_CTL_FRM_NO_LGTH_CHECK_MASK     BIT_U(UNIMAC_REG_CMD_CTL_FRM_NO_LGTH_CHECK)
#define UNIMAC_REG_CMD_CFG_TX_PAUSE_IGNORE_MASK BIT_U(UNIMAC_REG_CMD_CFG_TX_PAUSE_IGNORE_SHIFT)
#define UNIMAC_REG_CMD_CFG_RUNT_FILTER_DIS_MASK BIT_U(UNIMAC_REG_CMD_CFG_RUNT_FILTER_SHIFT)
#define UNIMAC_REG_GPORT_RSV_FRM_LEN_CHK_MASK   (BIT_U(5) | BIT_U(6))
#define UNIMAC_REG_EEE_CTRL_RX_FIFO_CHECK         (1<<4)
#define UNIMAC_REG_CMD_CFG_OVERFLOW_EN            (1<<12)

/* Unimac MAC_MODE register */
#define UNIMAC_REG_MAC_MODE_DUPLEX_SHIFT        2
#define UNIMAC_REG_MAC_MODE_SPEED_SHIFT         0
#define UNIMAC_REG_MAC_MODE_SPEED_SHIFT_L       UNIMAC_REG_MAC_MODE_SPEED_SHIFT
#define UNIMAC_REG_MAC_MODE_SPEED_SHIFT_H       (UNIMAC_REG_MAC_MODE_SPEED_SHIFT + 1)

#define UNIMAC_REG_MAC_MODE_DUPLEX_MASK        BIT_U(UNIMAC_REG_MAC_MODE_DUPLEX_SHIFT)
#define UNIMAC_REG_MAC_MODE_SPEED_MASK          (BIT_U(UNIMAC_REG_MAC_MODE_SPEED_SHIFT_L) \
                                               |BIT_U(UNIMAC_REG_MAC_MODE_SPEED_SHIFT_H))

/* UNIMAC TX Flush Control */
#define UNIMAC_REG_TX_FLUSH_CTRL_MASK          1

#define BMACSEC_PHY54580_LINE_MAC_PORT(p)    (p)
#define BMACSEC_PHY54580_SWITCH_MAC_PORT(p)  ((p) + 8)
/*
 * Half-Duplex Back-Pressure Control register
 */
#define UNIMAC_REG_BKP_HD_FC_ENA_SHIFT     0
#define UNIMAC_REG_BKP_HD_FC_BKOFF_SHIFT   1
#define UNIMAC_REG_BKP_HD_FC_ENA_MASK      BIT_U(UNIMAC_REG_BKP_HD_FC_ENA_SHIFT)
#define UNIMAC_REG_BKP_HD_FC_BKOFF_MASK    BIT_U(UNIMAC_REG_BKP_HD_FC_BKOFF_SHIFT)


/*
 * CRC Forward
 */
#define UNIMAC_REG_CMD_CFG_CRC_FWD_DISABLE 0
#define UNIMAC_REG_CMD_CFG_CRC_FWD_ENABLE  1

/*
 * Frame Length
 */
#define UNIMAC_REG_DUPLEX_GATEWAY_MAX_FRAME_SIZE  0x0003fe4
#define UNIMAC_REG_DEFAULT_MAX_FRAME_SIZE        0x0003fff
#define UNIMAC_REG_FRM_LENr_FMR_LEN_MASK         0x0003fff

/* DEFAULT IPG */
#define UNIMAC_REG_DEFAULT_IPG        12


/* MACSEC Control register */
#define UNIMAC_REG_TX_CRC_ENABLE_MASK        (1 << 1)
#define UNIMAC_REG_TX_CRC_ENABLE_SHIFT       1

#define UNIMAC_REG_TX_CRC_ENABLE             1


#define UNIMAC_COMMAND_CONFIGr(_mmc, p) \
((_mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2)  ?  \
(PHY_UNIMAC_V2_GET_SIDE(p) ?  0x00007c08 : 0x00007808 ) : \
(0x00100202 + (((p) & 0xf) << 12))

#define UNIMAC_MAC_MODEr(_mmc, p) \
((_mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2)  ?  \
(PHY_UNIMAC_V2_GET_SIDE(p) ?  0x00007c44 : 0x00007844 ) : \
(0x00100211 + (((p) & 0xf) << 12))


#define UNIMAC_FRM_LENGTHr(_mmc, p) \
((_mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2)  ?  \
(PHY_UNIMAC_V2_GET_SIDE(p) ?  0x00007c14 : 0x00007814 ) : \
(0x00100205 + (((p) & 0xf) << 12))

#define UNIMAC_TX_IPG_LENGTHr(_mmc, p) \
((_mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2)  ?  \
(PHY_UNIMAC_V2_GET_SIDE(p) ?  0x00007c5c : 0x0000785c ) : \
(0x00100217 + (((p) & 0xf) << 12))

#define UNIMAC_MACSEC_CNTRLr(_mmc, p)  \
((_mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2)  ?  \
(PHY_UNIMAC_V2_GET_SIDE(p) ?  0x00007f14 : 0x00007b14 ) : \
(0x001002c5 + (((p) & 0xf) << 12))
#define UNIMAC_UMAC_EEE_CTRLr(_mmc, p)   \
((_mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2)  ?  \
(PHY_UNIMAC_V2_GET_SIDE(p) ?  0x00007c64 : 0x00007864 ) : \
(0x00100219 + (((p) & 0xf) << 12))
#define UNIMAC_MACSEC_IPG_HD_BKP_CNTLr(_mmc,_p) \
((_mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2)  ?  \
(PHY_UNIMAC_V2_GET_SIDE(_p) ?  0x00007c04 : 0x00007804 ) : \
(0x00100201 + (((_p) & 0xf) << 12))
#define UNIMAC_MACSEC_GPORT_RSV_MASKr(_mmc,_p) \
((_mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2)  ?  \
(PHY_UNIMAC_V2_GET_SIDE(_p) ?  0x0000000c : 0x00000008 ) : \
(0x02100036 + (((_p) & 0xf) << 12))

#define UNIMAC_TX_FLUSH_CTRLr(_mmc,_p) \
((_mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2)  ?  \
(PHY_UNIMAC_V2_GET_SIDE(_p) ?  0x00007f34 : 0x00007b34 ) : \
(0x001002CD + (((_p) & 0xf) << 12))


#define BMACSEC_NUM_WORDS 1

#define BMACSEC_MAC_READ_REG(mmc, _dev_port, _io_addr, _data)         \
               (mmc)->devio_f(((mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2) ? (mmc)->unit : (mmc)->dev_addr, PHY_UNIMAC_V2_GET_PORT(_dev_port), BLMI_IO_REG_RD, \
                               (_io_addr), BMACSEC_NUM_WORDS, 1, (_data))

#define BMACSEC_MAC_WRITE_REG(mmc, _dev_port, _io_addr, _data)        \
               (mmc)->devio_f(((mmc)->flag &  PHY_MAC_CTRL_FLAG_UNIMAC_V2) ? (mmc)->unit : (mmc)->dev_addr, PHY_UNIMAC_V2_GET_PORT(_dev_port), BLMI_IO_REG_WR, \
                               (_io_addr), BMACSEC_NUM_WORDS, 1, &(_data))


/* Command Config Register */
#define UNIMAC_READ_CMD_CFG_REG(mmc, _dev_port, _data)         \
            BMACSEC_MAC_READ_REG((mmc), (_dev_port),           \
                                 UNIMAC_COMMAND_CONFIGr((mmc), _dev_port),  \
                                 (_data))

#define UNIMAC_WRITE_CMD_CFG_REG(mmc, _dev_port, _data)        \
            BMACSEC_MAC_WRITE_REG((mmc), (_dev_port),          \
                                  UNIMAC_COMMAND_CONFIGr((mmc), _dev_port), \
                                  (_data))
/* UNIMAC Status MAC_MODE Register */
#define UNIMAC_READ_MAC_MODE_REG(mmc, _dev_port, _data)         \
            BMACSEC_MAC_READ_REG((mmc), (_dev_port),           \
                                 UNIMAC_MAC_MODEr((mmc), _dev_port),  \
                                 (_data))

/* Frame Length Register */
#define UNIMAC_READ_FRM_LEN_REG(mmc, _dev_port, _data)         \
            BMACSEC_MAC_READ_REG((mmc), (_dev_port),           \
                                 UNIMAC_FRM_LENGTHr((mmc), _dev_port),      \
                                 (_data))

#define UNIMAC_WRITE_FRM_LEN_REG(mmc, _dev_port, _data)        \
            BMACSEC_MAC_WRITE_REG((mmc), (_dev_port),          \
                                  UNIMAC_FRM_LENGTHr((mmc), _dev_port),     \
                                  (_data))
/* Enable frame length check error by setting mask bit for RSV[21] */
#define UNIMAC_READ_GPORT_RSV_MASK_REG(mmc, _dev_port, _data)        \
            BMACSEC_MAC_READ_REG((mmc), (_dev_port),          \
                                  UNIMAC_MACSEC_GPORT_RSV_MASKr((mmc), _dev_port),     \
                                  (_data))
#define UNIMAC_WRITE_GPORT_RSV_MASK_REG(mmc, _dev_port, _data)        \
            BMACSEC_MAC_WRITE_REG((mmc), (_dev_port),          \
                                  UNIMAC_MACSEC_GPORT_RSV_MASKr((mmc), _dev_port),     \
                                  (_data))

/*  TX IPG Register */
#define UNIMAC_READ_TX_IPG_REG(mmc, _dev_port, _data)          \
            BMACSEC_MAC_READ_REG((mmc), (_dev_port),           \
                                 UNIMAC_TX_IPG_LENGTHr((mmc), _dev_port),   \
                                 (_data))

#define UNIMAC_WRITE_TX_IPG_REG(mmc, _dev_port, _data)         \
            BMACSEC_MAC_WRITE_REG((mmc), (_dev_port),          \
                                  UNIMAC_TX_IPG_LENGTHr((mmc), _dev_port),  \
                                  (_data))

/* MACSEC Control register */
#define UNIMAC_READ_MACSEC_CNTL_REG(mmc, _dev_port, _data)     \
            BMACSEC_MAC_READ_REG((mmc), (_dev_port),           \
                                 UNIMAC_MACSEC_CNTRLr((mmc), _dev_port),    \
                                 (_data))

#define UNIMAC_WRITE_MACSEC_CNTL_REG(mmc, _dev_port, _data)    \
            BMACSEC_MAC_WRITE_REG((mmc), (_dev_port),          \
                                  UNIMAC_MACSEC_CNTRLr((mmc), _dev_port),   \
                                  (_data))
    /* Half-Duplex Back-Pressure Control register */
#define UNIMAC_READ_IPG_HD_BKP_CNTL_REG(mmc, _dev_port, _data)     \
                BMACSEC_MAC_READ_REG((mmc), (_dev_port),           \
                                     UNIMAC_MACSEC_IPG_HD_BKP_CNTLr((mmc), _dev_port),   \
                                     (_data))

#define UNIMAC_WRITE_IPG_HD_BKP_CNTL_REG(mmc, _dev_port, _data)    \
                BMACSEC_MAC_WRITE_REG((mmc), (_dev_port),          \
                                      UNIMAC_MACSEC_IPG_HD_BKP_CNTLr((mmc), _dev_port),  \
                                      (_data))
/* UMAC_EEE_CTRL read and write function */
#define UNIMAC_READ_UMAC_EEE_CTRL_REG(mmc, _dev_port, _data)    \
                BMACSEC_MAC_READ_REG((mmc), (_dev_port),           \
                                      UNIMAC_UMAC_EEE_CTRLr((mmc), _dev_port),   \
                                      (_data))
#define UNIMAC_WRITE_UMAC_EEE_CTRL_REG(mmc, _dev_port, _data)    \
                BMACSEC_MAC_WRITE_REG((mmc), (_dev_port),          \
                                      UNIMAC_UMAC_EEE_CTRLr((mmc), _dev_port),   \
                                      (_data))
/* UNIMAC TX FLUSH_CONTROL read and write function */
#define UNIMAC_READ_TX_FLUSH_CTRL_REG(mmc, _dev_port, _data)    \
                    BMACSEC_MAC_READ_REG((mmc), (_dev_port),           \
                                          UNIMAC_TX_FLUSH_CTRLr((mmc), _dev_port),   \
                                          (_data))
#define UNIMAC_WRITE_TX_FLUSH_CTRL_REG(mmc, _dev_port, _data)    \
                    BMACSEC_MAC_WRITE_REG((mmc), (_dev_port),          \
                                          UNIMAC_TX_FLUSH_CTRLr((mmc), _dev_port),   \
                                          (_data))

#endif   /* PHY_UNIMAC_H_ */


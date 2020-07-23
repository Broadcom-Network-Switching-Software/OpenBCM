/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef   _XMAC_MAC_H_
#define   _XMAC_MAC_H_ 

#include <bbase_types.h>
#include "phy_mac_ctrl.h"

extern int phy_xmac_mac_init(phy_mac_ctrl_t *mmc, int dev_port);

extern int phy_xmac_mac_fcmap_init(phy_mac_ctrl_t *mmc, int dev_port);

extern int phy_xmac_mac_reset(phy_mac_ctrl_t *mmc, int dev_port, int fval);

extern int phy_xmac_mac_enable_set(phy_mac_ctrl_t *mmc, 
                                   int dev_port, int en);

extern int phy_xmac_mac_enable_get(phy_mac_ctrl_t *mmc, 
                                   int dev_port, int *en) ;

extern int phy_xmac_mac_update_ipg(phy_mac_ctrl_t *mmc,  
                               int dev_port, int ipg);

extern int phy_xmac_mac_max_frame_set(phy_mac_ctrl_t *mmc, 
                                      int dev_port, buint32_t max_frame);

extern int phy_xmac_mac_max_frame_get(phy_mac_ctrl_t *mmc, 
                                      int dev_port, buint32_t *flen);

extern int phy_xmac_mac_pause_frame_fwd(phy_mac_ctrl_t *mmc, 
                                          int dev_port, int pause_fwd);

extern int phy_xmac_mac_pause_set(phy_mac_ctrl_t *mmc, 
                                    int dev_port, int pause);

extern int phy_xmac_mac_speed_set(phy_mac_ctrl_t *mmc, 
                                    int dev_port, int speed);

extern int phy_xmac_mac_speed_get(phy_mac_ctrl_t *mmc, 
                                    int dev_port, int *speed);

extern int phy_xmac_mac_duplex_set(phy_mac_ctrl_t *mmc, 
                                     int dev_port, int duplex);

extern int phy_xmac_mac_duplex_get(phy_mac_ctrl_t *mmc, 
                                     int dev_port, int *duplex);

extern int phy_xmac_mac_auto_cfg_set(phy_mac_ctrl_t *mmc, 
                                       int dev_port, int enable);

extern int phy_xmac_mac_readtx(phy_mac_ctrl_t *mmc, int dev_port);

#define XMAC_64_HI_SET(u64, val)\
    COMPILER_64_SET(u64, (val) , COMPILER_64_LO(u64));

#define XMAC_64_LO_SET(u64, val)\
    COMPILER_64_SET(u64, COMPILER_64_HI(u64), (val));

/*
 * XMAC Control Register
 */
#define XMAC_MAC_CTRL_TXEN_MASK             (1 << 0)
#define XMAC_MAC_CTRL_RXEN_MASK             (1 << 1)
#define XMAC_MAC_CTRL_LB_LL_MASK            (1 << 2)
#define XMAC_MAC_CTRL_LB_CL_MASK            (1 << 3)
#define XMAC_MAC_CTRL_LB_LR_MASK            (1 << 4)
#define XMAC_MAC_CTRL_LB_CR_MASK            (1 << 5)
#define XMAC_MAC_CTRL_RESET_MASK            (1 << 6)

#define XMAC_MAC_CTRL_TXEN_SHIFT             0
#define XMAC_MAC_CTRL_RXEN_SHIFT             1
#define XMAC_MAC_CTRL_RESET_SHIFT            6


/*
 * XMAC TX Control Register
 */
#define XMAC_TX_CTRL_CRCMODE_MASK           (3 << 0)
#define XMAC_TX_CTRL_IPG_MASK               (0x7f << 12)


#define XMAC_TX_CTRL_CRCMODE_SHIFT          0
#define XMAC_TX_CTRL_IPG_SHIFT              12

/* DEFAULT IPG */
#define XMAC_MAC_DEFAULT_IPG        12


/*
 * XMAC RX Control Register
 */
#define XMAC_RX_CTRL_PASS_MAC_CNTL_MASK     (1 << 0)
#define XMAC_RX_CTRL_CRCSTRIP_MASK          (1 << 2)


#define XMAC_RX_CTRL_PASS_MAC_CNTL_SHIFT    0
#define XMAC_RX_CTRL_CRCSTRIP_SHIFT         2

/*
 * XMAC RX LSS Control Register
 */
#define XMAC_RX_LSS_CTRL_DISABLE_MASK           (3 << 0)

/*
 * XMAC RX MACSEC Control Register
 */
#define XMAC_MACSEC_CTRL_TX_CRC_CORRUPT_MASK     (1 << 1)

#define XMAC_MACSEC_CTRL_TX_CRC_CORRUPT_SHIFT     1

/*
 * XMAC PAUSE Control Register
 */
#define XMAC_TX_EN_PAUSE_MASK             (1 << 17)
#define XMAC_RX_EN_PAUSE_MASK             (1 << 18)

#define XMAC_TX_EN_PAUSE_SHIFT            17
#define XMAC_RX_EN_PAUSE_SHIFT            18


/* 
 * CRC Forward
 */
#define XMAC_MAC_CTRL_CRC_FWD_DISABLE 0
#define XMAC_MAC_CTRL_CRC_FWD_ENABLE  1

/* MAX Frame Size */
#define XMAC_MAC_DEFAULT_MAX_FRAME_SIZE       0x00003fe8

/* 
 * TX Frame Length 
 */
#define XMAC_TXMAXSZ_MAX_FRAME_SIZE        \
                           XMAC_MAC_DEFAULT_MAX_FRAME_SIZE
#define XMAC_TXMAXSZ_MAX_FRAME_SIZE_MASK   0x0003fff

/* 
 * RX Frame Length 
 */
#define XMAC_RXMAXSZ_MAX_FRAME_SIZE        \
                           XMAC_MAC_DEFAULT_MAX_FRAME_SIZE
#define XMAC_RXMAXSZ_MAX_FRAME_SIZE_MASK   0x0003fff



#define XMAC_IS_V2(m) \
( (m)->flag & PHY_MAC_CTRL_FLAG_XMAC_V2)

/* 
 * Register Access Macros
 */

#define XMAC_REG_MAC_CTRLr(p,m) \
		(0x01100000 + (((p) & 0x7) << 12))

#define XMAC_REG_TX_CTRLr(p,m) \
		(0x01100004 + (((p) & 0x7) << 12))

#define XMAC_REG_RX_CTRLr(p,m)  \
        (0x01100006 + (((p) & 0x7) << 12))

#define XMAC_REG_RX_MAXSZr(p,m)  \
        (0x01100008 + (((p) & 0x7) << 12))

#define XMAC_REG_RX_LSS_CTRLr(p,m)  \
        (0x0110000a + (((p) & 0x7) << 12))

#define XMAC_REG_PAUSE_CTRLr(p,m)  \
        (0x0110000d + (((p) & 0x7) << 12))

#define XMAC_REG_MAC_MACSEC_CTRLr(p,m)  \
((XMAC_IS_V2(m) ? 0x01100025 : 0x01100024) + (((p) & 0x7) << 12))


#define BMACSEC_XMAC_REG_SIZE 2
#ifdef COMPILER_HAS_LONGLONG_SHIFT
#define XMAC_MMI_READ_REG(_mmc, _dev_port, _io_addr, _data)             \
(                                                                       \
    rv = (_mmc)->devio_f((_mmc)->dev_addr, (_dev_port), BLMI_IO_REG_RD, \
                               (_io_addr), BMACSEC_XMAC_REG_SIZE, 1,    \
                               (buint32_t *)tmp_val),                   \
    COMPILER_64_SET(*(_data), tmp_val[1], tmp_val[0]),                  \
    rv                                                                  \
)
#else
#define XMAC_MMI_READ_REG(_mmc, _dev_port, _io_addr, _data)             \
(                                                                       \
    rv = (_mmc)->devio_f((_mmc)->dev_addr, (_dev_port), BLMI_IO_REG_RD, \
                               (_io_addr), BMACSEC_XMAC_REG_SIZE, 1,    \
                               (buint32_t *)tmp_val),                   \
    COMPILER_64_LO(*(_data)) = tmp_val[0],                              \
    COMPILER_64_HI(*(_data)) = tmp_val[1],                              \
    rv                                                                  \
)
#endif

#define XMAC_MMI_WRITE_REG(_mmc, _dev_port, _io_addr, _data)            \
(                                                                       \
    tmp_val[0] = COMPILER_64_LO((_data)),                               \
    tmp_val[1] = COMPILER_64_HI((_data)),                               \
    rv = (_mmc)->devio_f((_mmc)->dev_addr, (_dev_port), BLMI_IO_REG_WR, \
                               (_io_addr), BMACSEC_XMAC_REG_SIZE, 1,    \
                               (buint32_t *)tmp_val),                   \
    rv                                                                  \
)


/* MAC Control Register */
#define XMAC_READ_MAC_CTRL_REG(_mmc, _dev_port, _data)             \
            XMAC_MMI_READ_REG((_mmc), (_dev_port),                 \
                                 XMAC_REG_MAC_CTRLr(_dev_port,_mmc),    \
                                 (_data))

#define XMAC_WRITE_MAC_CTRL_REG(_mmc, _dev_port, _data)            \
            XMAC_MMI_WRITE_REG((_mmc), (_dev_port),                \
                                  XMAC_REG_MAC_CTRLr(_dev_port,_mmc),   \
                                  (_data))

/*  TX Control Register */
#define XMAC_READ_TXCTRL_REG(_mmc, _dev_port, _data)               \
            XMAC_MMI_READ_REG((_mmc), (_dev_port),                 \
                                 XMAC_REG_TX_CTRLr(_dev_port,_mmc),     \
                                 (_data))

#define XMAC_WRITE_TXCTRL_REG(_mmc, _dev_port, _data)                 \
            XMAC_MMI_WRITE_REG((_mmc), (_dev_port),                   \
                                 XMAC_REG_TX_CTRLr(_dev_port,_mmc),        \
                                  (_data))

/* MAC MACSEC Control register */
#define XMAC_READ_MAC_MACSEC_CTRL_REG(_mmc, _dev_port, _data)          \
            XMAC_MMI_READ_REG((_mmc), (_dev_port),                     \
                                 XMAC_REG_MAC_MACSEC_CTRLr(_dev_port,_mmc), \
                                 (_data))

#define XMAC_WRITE_MAC_MACSEC_CTRL_REG(_mmc, _dev_port, _data)         \
            XMAC_MMI_WRITE_REG((_mmc), (_dev_port),                    \
                                 XMAC_REG_MAC_MACSEC_CTRLr(_dev_port,_mmc), \
                                  (_data))

/* RX Control register */
#define XMAC_READ_RXCTRL_REG(_mmc, _dev_port, _data)                  \
            XMAC_MMI_READ_REG((_mmc), (_dev_port),                    \
                                 XMAC_REG_RX_CTRLr(_dev_port,_mmc),        \
                                 (_data))

#define XMAC_WRITE_RXCTRL_REG(_mmc, _dev_port, _data)                 \
            XMAC_MMI_WRITE_REG((_mmc), (_dev_port),                   \
                                  XMAC_REG_RX_CTRLr(_dev_port,_mmc),       \
                                  (_data))

/* RX Max Size register */
#define XMAC_READ_RXMAXSZ_REG(_mmc, _dev_port, _data)                 \
            XMAC_MMI_READ_REG((_mmc), (_dev_port),                    \
                                 XMAC_REG_RX_MAXSZr(_dev_port,_mmc),       \
                                 (_data))
#define XMAC_WRITE_RXMAXSZ_REG(_mmc, _dev_port, _data)                \
            XMAC_MMI_WRITE_REG((_mmc), (_dev_port),                   \
                                  XMAC_REG_RX_MAXSZr(_dev_port,_mmc),      \
                                  (_data))

/* PAUSE Control register */
#define XMAC_READ_PAUSE_CTRL_REG(_mmc, _dev_port, _data)              \
            XMAC_MMI_READ_REG((_mmc), (_dev_port),                    \
                                 XMAC_REG_PAUSE_CTRLr(_dev_port,_mmc),     \
                                 (_data))
#define XMAC_WRITE_PAUSE_CTRL_REG(_mmc, _dev_port, _data)             \
            XMAC_MMI_WRITE_REG((_mmc), (_dev_port),                   \
                                  XMAC_REG_PAUSE_CTRLr(_dev_port,_mmc),    \
                                  (_data))


/* LSS Control register */
#define XMAC_READ_RX_LSS_CTRL_REG(_mmc, _dev_port, _data)             \
            XMAC_MMI_READ_REG((_mmc), (_dev_port),                    \
                                 XMAC_REG_RX_LSS_CTRLr(_dev_port,_mmc),    \
                                 (_data))
#define XMAC_WRITE_RX_LSS_CTRL_REG(_mmc, _dev_port, _data)            \
            XMAC_MMI_WRITE_REG((_mmc), (_dev_port),                   \
                                 XMAC_REG_RX_LSS_CTRLr(_dev_port,_mmc),    \
                                  (_data))

#endif   /* _XMAC_MAC_H_ */


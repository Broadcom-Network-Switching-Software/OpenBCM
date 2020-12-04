/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PHY_MAC_CTRL_H
#define PHY_MAC_CTRL_H

#include <sal/types.h>
#include <sal/core/spl.h>
#include <sal/core/libc.h>
#ifdef INCLUDE_PLP_IMACSEC
#include "bcm_imacsec.h"
#endif

#if  !defined(INCLUDE_MACSEC)  &&  !defined(INCLUDE_FCMAP)

typedef uint32   blmi_dev_addr_t;

/*
 * MACSEC I/O operation type identifier.
 */
typedef enum {
    BLMI_IO_REG_RD,   /* Register Read    */
    BLMI_IO_REG_WR,   /* Register Write   */
    BLMI_IO_TBL_RD,   /* Table Read       */
    BLMI_IO_TBL_WR    /* Table Write      */
} blmi_io_op_t;

typedef int (*blmi_dev_io_f)(  uint32 unit,      /* Switch unit  */
                               int port,         /* Switch port */
                               blmi_io_op_t op,  /* I/O operation   */
                               uint32 io_addr,   /* I/O address     */
                               int word_sz,      /* Word size       */
                               int num_entry,    /* Num entry       */
                               uint32 *data);    /* Data buffer     */

typedef enum {
    BLMI_E_NONE        =  0,
    BLMI_E_INTERNAL    = -1,
    BLMI_E_MEMORY      = -2,
    BLMI_E_PARAM       = -3,
    BLMI_E_EMPTY       = -4,
    BLMI_E_FULL        = -5,
    BLMI_E_NOT_FOUND   = -6,
    BLMI_E_EXISTS      = -7,
    BLMI_E_TIMEOUT     = -8,
    BLMI_E_FAIL        = -9,
    BLMI_E_DISABLED    = -10,
    BLMI_E_BADID       = -11,
    BLMI_E_RESOURCE    = -12,
    BLMI_E_CONFIG      = -13,
    BLMI_E_UNAVAIL     = -14,
    BLMI_E_INIT        = -15,
    BLMI_E_PORT        = -16,
    BLMI_E_UNKNOWN     = -17
} blmi_error_t;


#else
#include <blmi_io.h>
#endif

#if  !defined(INCLUDE_MACSEC)

typedef enum {
    BMACSEC_E_NONE      =  0,
    BMACSEC_E_INTERNAL      = -1,
    BMACSEC_E_MEMORY        = -2,
    BMACSEC_E_PARAM     = -3,
    BMACSEC_E_EMPTY             = -4,
    BMACSEC_E_FULL      = -5,
    BMACSEC_E_NOT_FOUND     = -6,
    BMACSEC_E_EXISTS        = -7,
    BMACSEC_E_TIMEOUT       = -8,
    BMACSEC_E_FAIL      = -9,
    BMACSEC_E_DISABLED      = -10,
    BMACSEC_E_BADID     = -11,
    BMACSEC_E_RESOURCE      = -12,
    BMACSEC_E_CONFIG        = -13,
    BMACSEC_E_UNAVAIL       = -14,
    BMACSEC_E_INIT      = -15,
    BMACSEC_E_PORT      = -16,
    BMACSEC_E_UNKNOWN           = -17
} bmacsec_error_t;
#else
#include <bmacsec_err.h>
#endif
#ifndef BMACSEC_E_UNAVAIL
#define BMACSEC_E_UNAVAIL  BLMI_E_UNAVAIL
#endif

#ifndef BMACSEC_E_PARAM
#define BMACSEC_E_PARAM    BLMI_E_PARAM
#endif

typedef enum {
    PHY_MAC_SD_LINE    = 0,
    PHY_MAC_SD_SWITCH  = 1
} phy_mac_side_e;

#define PHY_UNIMAC_V2_SIDE_MASK   (1 << PHY_UNIMAC_V2_SIDE_SHIFT)
#define PHY_UNIMAC_V2_SIDE_SHIFT  (15)

#define PHY_UNIMAC_V2_LINE_PORT(p)  (p)

#define PHY_UNIMAC_V2_SWITCH_PORT(p)\
((p) |  (1 << PHY_UNIMAC_V2_SIDE_SHIFT))

#define PHY_UNIMAC_V2_GET_SIDE(p)\
(((p) & (PHY_UNIMAC_V2_SIDE_MASK)) >> PHY_UNIMAC_V2_SIDE_SHIFT)

#define PHY_UNIMAC_V2_GET_PORT(p)\
((p) & ~(PHY_UNIMAC_V2_SIDE_MASK))
#ifdef INCLUDE_PLP_IMACSEC
#define IMACSEC_DES_PTR_GET(_pc)\
((bcm_plp_base_t_sec_phy_access_t *)(((char *)((_pc) + 1) + (_pc)->size) - sizeof(bcm_plp_base_t_sec_phy_access_t)))
#endif
struct phy_mac_ctrl_s;
struct phy_mac_duplex_gateway_s;
/*
 * MAC Driver Function prototypes
 */
typedef int (*mac_init)(struct phy_mac_ctrl_s*, int);
typedef int (*mac_reset)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_auto_cfg_set)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_enable_set)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_enable_get)(struct phy_mac_ctrl_s*, int, int *);
typedef int (*mac_speed_set)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_speed_get)(struct phy_mac_ctrl_s*, int, int *);
typedef int (*mac_duplex_set)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_duplex_get)(struct phy_mac_ctrl_s*, int, int *);
typedef int (*mac_ipg_set)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_max_frame_set)(struct phy_mac_ctrl_s*, int, uint32);
typedef int (*mac_max_frame_get)(struct phy_mac_ctrl_s*, int, uint32*);
typedef int (*mac_pause_set)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_pause_get)(struct phy_mac_ctrl_s*, int, int*);
typedef int (*mac_pause_frame_fwd_set)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_pause_frame_fwd_get)(struct phy_mac_ctrl_s*, int, int*);
typedef int (*mac_duplex_gateway_set)(struct phy_mac_ctrl_s*,
                                      struct phy_mac_duplex_gateway_s*);
typedef int (*mac_frame_len_check_enable)(struct phy_mac_ctrl_s*, int, int);

typedef int (*mac_rx_enable_set)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_tx_enable_set)(struct phy_mac_ctrl_s*, int, int);
typedef int (*mac_tx_datapath_flush)(struct phy_mac_ctrl_s*, int, int);

typedef struct phy_mac_drv_s {
    mac_init                   f_mac_init;
    mac_reset                  f_mac_reset;
    mac_auto_cfg_set           f_mac_auto_config_set;
    mac_enable_set             f_mac_enable_set;
    mac_enable_get             f_mac_enable_get;
    mac_speed_set              f_mac_speed_set;
    mac_speed_get              f_mac_speed_get;
    mac_duplex_set             f_mac_duplex_set;
    mac_duplex_get             f_mac_duplex_get;
    mac_ipg_set                f_mac_ipg_set;
    mac_max_frame_set          f_mac_max_frame_set;
    mac_max_frame_get          f_mac_max_frame_get;
    mac_pause_set              f_mac_pause_set;
    mac_pause_get              f_mac_pause_get;
    mac_pause_frame_fwd_set    f_mac_pause_frame_fwd_set;
    mac_pause_frame_fwd_get    f_mac_pause_frame_fwd_get;
    mac_duplex_gateway_set     f_mac_duplex_gateway_set;
    mac_frame_len_check_enable f_mac_frame_len_check_enable;
    mac_rx_enable_set          f_mac_rx_enable_set;
    mac_tx_enable_set          f_mac_tx_enable_set;
    mac_tx_datapath_flush      f_mac_tx_datapath_flush;
} phy_mac_drv_t;

typedef struct phy_mac_ctrl_s {
    /* Device address for the MMI */
    blmi_dev_addr_t  dev_addr;

    /* MAC driver */
    phy_mac_drv_t   mac_drv;

    /* MAC IO handler for MMI */
    blmi_dev_io_f    devio_f;

    /* Config flag */
    uint32 flag;
#define PHY_MAC_CTRL_FLAG_PHY_FIX_LATENCY_EN  (1<<0)
#define PHY_MAC_CTRL_FLAG_XMAC_V2                (1<<1)
#define PHY_MAC_CTRL_FLAG_UNIMAC_V2              (1<<2)

    /* Switch Unit */
    uint32 unit;

} phy_mac_ctrl_t;
typedef enum phy_mac_duplex_e {
    PHY_MAC_HALF_DUPLEX = 0,
    PHY_MAC_FULL_DUPLEX
}phy_mac_duplex_t;
typedef struct phy_mac_duplex_gateway_s {
    int  lport, sport;
    int  speed, duplex;
    int  ipg  , max_frame;
} phy_mac_duplex_gateway_t;
/*
 * Switch MAC speed and duplex policies
 */
typedef enum phy_mac_policy_s {
    PHY_MAC_SWITCH_FIXED = 0,
    PHY_MAC_SWITCH_FOLLOWS_LINE,
    PHY_MAC_SWITCH_DUPLEX_GATEWAY
}phy_mac_policy_t;
#define BMACSEC_MAC_REG_FIELD_SET(_d, _v, _mask, _shift)    \
            do { (_d) &= ~(_mask);  (_d) |= ((_v) << (_shift)); } while(0)

#define BMACSEC_MAC_DRV_FN(mmc,_mf)   (mmc)->mac_drv._mf

#define _MAC_DRV_CALL(mmc, _mf, _ma)                             \
        (mmc == NULL ? BMACSEC_E_PARAM :                         \
         (BMACSEC_MAC_DRV_FN(mmc,_mf) == NULL ?                  \
         BMACSEC_E_UNAVAIL : BMACSEC_MAC_DRV_FN(mmc,_mf) _ma))

#define BMACSEC_MAC_INIT(mmc, _p) \
        _MAC_DRV_CALL((mmc), f_mac_init, ((mmc), (_p)))
#define BMACSEC_MAC_RESET(mmc, _p , _r) \
        _MAC_DRV_CALL((mmc), f_mac_reset, ((mmc), (_p), (_r)))
#define BMACSEC_MAC_AUTO_CONFIG_SET(mmc, _p, _v) \
        _MAC_DRV_CALL((mmc), f_mac_auto_config_set, ((mmc), (_p), (_v)))

#define BMACSEC_MAC_ENABLE_SET(mmc, _p, _e) \
        _MAC_DRV_CALL((mmc), f_mac_enable_set, ((mmc), (_p), (_e)))
#define BMACSEC_MAC_ENABLE_GET(mmc, _p, _e) \
        _MAC_DRV_CALL((mmc), f_mac_enable_get, ((mmc), (_p), (_e)))

#define BMACSEC_MAC_SPEED_SET(mmc, _p, _s) \
        _MAC_DRV_CALL((mmc), f_mac_speed_set, ((mmc), (_p), (_s)))

#define BMACSEC_MAC_SPEED_GET(mmc, _p,_s) \
        _MAC_DRV_CALL((mmc), f_mac_speed_get, ((mmc), (_p), (_s)))

#define BMACSEC_MAC_DUPLEX_SET(mmc, _p, _d) \
        _MAC_DRV_CALL((mmc), f_mac_duplex_set, ((mmc), (_p), (_d)))

#define BMACSEC_MAC_DUPLEX_GET(mmc, _p,_d) \
        _MAC_DRV_CALL((mmc), f_mac_duplex_get, ((mmc), (_p), (_d)))

#define BMACSEC_MAC_IPG_SET(mmc, _p, _ipg) \
        _MAC_DRV_CALL((mmc), f_mac_ipg_set, ((mmc), (_p), (_ipg)))

#define BMACSEC_MAC_IPG_GET(mmc, _p) \
        _MAC_DRV_CALL((mmc), f_mac_ipg_get, ((mmc), (_p)))

#define BMACSEC_MAC_MAX_FRAME_SET(mmc, _p, _max) \
        _MAC_DRV_CALL((mmc), f_mac_max_frame_set, ((mmc), (_p), (_max)))

#define BMACSEC_MAC_MAX_FRAME_GET(mmc, _p,_max) \
        _MAC_DRV_CALL((mmc), f_mac_max_frame_get, ((mmc), (_p),(_max)))

#define BMACSEC_MAC_PAUSE_SET(mmc, _p, _v) \
        _MAC_DRV_CALL((mmc), f_mac_pause_set, ((mmc), (_p), (_v)))

#define BMACSEC_MAC_PAUSE_GET(mmc, _p,_v) \
        _MAC_DRV_CALL((mmc), f_mac_pause_get, ((mmc), (_p),(_v)))

#define BMACSEC_MAC_PAUSE_FRAME_FWD_SET(mmc, _p, _v) \
        _MAC_DRV_CALL((mmc), f_mac_pause_frame_fwd_set, ((mmc), (_p), (_v)))
#define BMACSEC_MAC_PAUSE_FRAME_FWD_GET(mmc, _p, _v) \
        _MAC_DRV_CALL((mmc), f_mac_pause_frame_fwd_get, ((mmc), (_p), (_v)))
#define BMACSEC_MAC_DUPLEX_GATEWAY(mmc, _v) \
        _MAC_DRV_CALL((mmc), f_mac_duplex_gateway_set, ((mmc), (_v)))
#define BMACSEC_MAC_FRAME_LEN_CHECK_ENABLE(mmc, _p, _e) \
        _MAC_DRV_CALL((mmc), f_mac_frame_len_check_enable, ((mmc), (_p), (_e)))
#define BMACSEC_MAC_RX_ENABLE_SET(mmc, _p, _e) \
        _MAC_DRV_CALL((mmc), f_mac_rx_enable_set, ((mmc), (_p), (_e)))
#define BMACSEC_MAC_TX_ENABLE_SET(mmc, _p, _e) \
        _MAC_DRV_CALL((mmc), f_mac_tx_enable_set, ((mmc), (_p), (_e)))
#define BMACSEC_MAC_TX_DATAPATH_FLUSH(mmc, _p, _e)   \
        _MAC_DRV_CALL((mmc), f_mac_tx_datapath_flush, ((mmc), (_p), (_e)))

/*
 * MAC driver types.
 */
typedef enum {
    PHY_MAC_CORE_UNKNOWN    = 0,
    PHY_MAC_CORE_UNIMAC,
    PHY_MAC_CORE_BIGMAC,
    PHY_MAC_CORE_XMAC
} phy_mactype_t;

/*
 * Allocate mac control block and attach the MAC driver.
 */
phy_mac_ctrl_t*
phy_mac_driver_attach(blmi_dev_addr_t dev_addr,
                         phy_mactype_t mac_type,
                         blmi_dev_io_f mmi_cbf);

int phy_mac_driver_detach(phy_mac_ctrl_t *mmc);

void
phy_mac_driver_config(phy_mac_ctrl_t *mmc,
                         phy_mactype_t  mac_type,
                         uint32 flag);

void
phy_mac_driver_unit_set(phy_mac_ctrl_t *mmc,
                         phy_mactype_t  mac_type,
                         uint32 unit);
#ifdef INCLUDE_PLP_IMACSEC
int
phy_unimac_switch_side_set_params(int unit, soc_port_t port,
                          phy_mac_policy_t switch_side_policy,
                          int speed, int duplex, int pause_enable);
#endif

#endif /* PHY_MAC_CTRL_H */


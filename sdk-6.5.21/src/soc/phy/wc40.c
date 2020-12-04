/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        wc40.c
 * Purpose:     Broadcom 10M/100M/1G/2.5G/10G/12G/13G/16G/20G/25G/30G/40G SerDes 
 *              (Warpcore 40nm with x1 and x4 lane support)
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_XGXS_WC40)
#include "phyconfig.h"     /* Must include before other phy related includes */
#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>
#include <soc/phyctrl.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif

#include "phyreg.h"
#include "phyfege.h"
#include "phynull.h"
#include "serdesid.h"
#include "wc40.h"
#include "wc40_extra.h"

/* Do not change the value, used as index */
#define WC_UTIL_VEYE           0  /* vertical eye */
#define WC_UTIL_HEYE_R         1  /* horizontal right eye */
#define WC_UTIL_HEYE_L         2  /* horizontal left eye */

#define WC40_SDK32387_REVS(_pc) (WC40_REVID_A0(_pc) || WC40_REVID_A1(_pc) || WC40_REVID_B(_pc))
#define WC40_PHY400_REVS(_pc) (WC40_REVID_B(_pc))

#define WC40_CL73_SOFT_KR2(_pc)  ((DEV_CFG_PTR(_pc)->cl73an == WC40_CL73_KR2) || \
                                  (DEV_CFG_PTR(_pc)->cl73an == WC40_CL73_KR2_WAR) || \
                                  (DEV_CFG_PTR(_pc)->cl73an == WC40_CL73_KR2_CUSTOM) || \
                                  (DEV_CFG_PTR(_pc)->cl73an == WC40_CL73_KR2_CUSTOM_WAR))
 
#define DSC1B0_UC_CTRLr 0x820e
#define WC40_UC_INFO_B1_REGB_SERDES_PLL_RATE_MASK (0x1f << WC40_UC_INFO_B1_REGB_SERDES_PLL_RATE_SHIFT)
#define WC40_UC_INFO_B1_REGB_SERDES_PLL_RATE_SHIFT (7)

/*
   SOFTWARE RX LOS feature introduces link flaps untill the link is stablized.
   Some customer applications do not like it. In order to minimize the link flaps
   a WAR is proposed to disable link fault reporitng briefly during SOFTWARE 
   RX LOS state machine transitions. 
 */
#define SW_RX_LOS_LINK_FLAP_WAR


/* CL73 AN TX FIFO RESET feature in wc firmware has a requirement that 
   this feature be disabled when reg 0x8345[15:14] is to be used with cl73 
   an and this feature enabled. Restore the state of the feature/routine
   afterwards.
   -DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status-
*/
#define WC40_CL73_AN_TX_FIFO_RESET_STOP(pc) {\
          if(DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status == ACTIVE) {\
              SOC_IF_ERROR_RETURN \
                  (MODIFY_WC40_MICROBLK_MDIO_UC_MAILBOXr(unit, pc, 0x0, \
                                                          0, 1<<pc->lane_num)); \
              DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status = REACTIVATE; \
          } \
        }   

#define WC40_CL73_AN_TX_FIFO_RESET_START(pc) {\
          if(DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status == REACTIVATE) {\
              SOC_IF_ERROR_RETURN \
                  (MODIFY_WC40_MICROBLK_MDIO_UC_MAILBOXr(unit, pc, 0x0, \
                                         1<<pc->lane_num, 1<<pc->lane_num)); \
              DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status = ACTIVE; \
          } \
        }   

/* static variables */
STATIC char wc_device_name[] = "WarpCore";
int (*_phy_wc40_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *,int) = {NULL};
 
static int ln_access[4] = {LANE0_ACCESS, LANE1_ACCESS,LANE2_ACCESS,LANE3_ACCESS};

/* uController's firmware */
extern uint8 wc40_ucode_revA0[];
extern int   wc40_ucode_revA0_len;
extern uint8 wc40_ucode_revB0[];
extern int   wc40_ucode_revB0_len;

STATIC WC40_UCODE_DESC wc40_ucodes[] = {
    {&wc40_ucode_revA0[0], &wc40_ucode_revA0_len, WC40_SERDES_ID0_REVID_A0},
    {&wc40_ucode_revA0[0], &wc40_ucode_revA0_len, WC40_SERDES_ID0_REVID_A1},
    {&wc40_ucode_revA0[0], &wc40_ucode_revA0_len, WC40_SERDES_ID0_REVID_A2},
    {&wc40_ucode_revB0[0], &wc40_ucode_revB0_len, WC40_SERDES_ID0_REVID_B0},
    {&wc40_ucode_revB0[0], &wc40_ucode_revB0_len, WC40_SERDES_ID0_REVID_B1}
};
#define WC40_UCODE_NUM_ENTRIES  (sizeof(wc40_ucodes)/sizeof(wc40_ucodes[0]))
/* function forward declaration */
STATIC int _phy_wc40_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int _phy_wc40_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int _phy_wc40_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_wc40_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_wc40_notify_interface(int unit, soc_port_t port, uint32 intf);
STATIC int _phy_wc40_notify_mac_loopback(int unit, soc_port_t port, uint32 enable);
STATIC int _phy_wc40_combo_speed_get(int unit, soc_port_t port, int *speed,
                                     int *intf, int *scr);
STATIC int _phy_wc40_ind_speed_get(int unit, soc_port_t port, int *speed,
                                   int *intf, int *scr);
STATIC int phy_wc40_an_set(int unit, soc_port_t port, int an);
STATIC int phy_wc40_lb_set(int unit, soc_port_t port, int enable);
STATIC int phy_wc40_ability_advert_set(int unit, soc_port_t port,
                                      soc_port_ability_t *ability);
STATIC int phy_wc40_ability_local_get(int unit, soc_port_t port,
                                soc_port_ability_t *ability);
STATIC int phy_wc40_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_wc40_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_wc40_an_get(int unit, soc_port_t port, int *an, int *an_done);
STATIC int _wc40_xgmii_scw_config (int unit, phy_ctrl_t *pc);
STATIC int _wc40_rxaui_config(int unit, phy_ctrl_t  *pc,int rxaui);
STATIC int _wc40_soft_reset(int unit, phy_ctrl_t  *pc);
STATIC int phy_wc40_reg_aer_read(int unit, phy_ctrl_t *pc, uint32 flags,
                  uint32 phy_reg_addr, uint16 *phy_data);
STATIC int phy_wc40_reg_aer_write(int unit, phy_ctrl_t *pc,  uint32 flags,
                   uint32 phy_reg_addr, uint16 phy_data);
STATIC int phy_wc40_reg_aer_modify(int unit, phy_ctrl_t *pc,  uint32 flags,
                    uint32 phy_reg_addr, uint16 phy_data,uint16 phy_data_mask);
STATIC int _wc40_chip_init_done(int unit,int chip_num,int phy_mode);
STATIC int phy_wc40_firmware_load(int unit, int port, int offset, 
                                 uint8 *array,int datalen);
STATIC int _phy_wc40_ind_speed_ctrl_get(int unit, soc_port_t port, int speed,
                                        uint16 *speed_val, int *tx_inx);
STATIC int _phy_wc40_vco_set(int unit, soc_port_t port, int speed,int speed_val);
STATIC int _phy_wc40_control_vco_disturbed_set(int unit, soc_port_t port);
STATIC int _wc40_soft_an_cl73kr2(int unit, soc_port_t port, int an);
STATIC int _wc40_soft_an_cl73kr2_custom(int unit, soc_port_t port, int an);
STATIC int _wc40_soft_an_cl73kr2_check(int unit, soc_port_t port);
STATIC int _wc40_soft_an_cl73kr2_custom_check(int unit, soc_port_t port);
STATIC int phy_wc40_diag_ctrl(int, soc_port_t, uint32, int, int, void *); 
STATIC int wc40_eye_margin(int unit, soc_port_t port, int type);
STATIC int wc40_uc_status_dump(int unit, soc_port_t port);
STATIC int _wc40_soft_an_cl73kr2_init(int unit, soc_port_t port);
STATIC int _phy_wc40_tx_disable(int, soc_port_t, int, int, int);
STATIC int _phy_wc40_tx_control_set(int, soc_port_t, WC40_TX_DRIVE_t *);
STATIC int _phy_wc40_tx_control_get(int, soc_port_t, WC40_TX_DRIVE_t *, int);
STATIC int _phy_wc40_firmware_mode_set(int, soc_port_t, uint32);
STATIC int _phy_wc40_firmware_mode_get(int, soc_port_t, uint32 *);
STATIC int _phy_wc40_firmware_dfe_mode_set(int, soc_port_t, uint32);
STATIC int _phy_wc40_firmware_dfe_mode_get(int, soc_port_t, uint32 *);
STATIC int _phy_wc40_speed_set(int unit, soc_port_t port, int speed);
STATIC int _phy_wc40_ind_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_wc40_enable_set(int unit, soc_port_t port, int enable);
STATIC int phy_wc40_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium);

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Warm Boot Version Map:
 *  BCM_WB_VERSION_1_0
 *      scache_ver
 *      sw_rx_los.sys_link
 *      sw_rx_los.state
 *      sw_rx_los.link_status
 */
STATIC int wc40_wb_sc_handler(int unit, soc_port_t port, 
                                WC40_WB_HANDLER_TYPE_t type, WC40_WB_DATA_ID_t id) ;
STATIC int wc40_wb_id_command(int unit, soc_port_t port, int offset, 
                                WC40_WB_HANDLER_TYPE_t type, WC40_WB_DATA_ID_t id, int *size) ;
STATIC int wc40_wb_sc_size_count(int unit, soc_port_t port, int *size) ;
STATIC int wc40_wb_sc_init(int unit, soc_port_t port) ;
STATIC int wc40_wb_sc_realloc_init(int unit, soc_port_t port) ;
STATIC int wc40_wb_sc_sync(int unit, soc_port_t port) ;
STATIC int wc40_wb_sc_reinit(int unit, soc_port_t port) ;
#endif

STATIC int
_phy_wc40_ucode_get(int unit, soc_port_t port, uint8 **ppdata, int *len, 
                    int *mem_alloced)
{
    int ix;
    phy_ctrl_t     *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *mem_alloced = FALSE;
    *ppdata = NULL;

    /* check which firmware to load */
    for (ix = 0; ix < WC40_UCODE_NUM_ENTRIES; ix++) {
        if (wc40_ucodes[ix].chip_rev == WC40_REVID(pc)) {
            break;
        }
    }
    if (ix >= WC40_UCODE_NUM_ENTRIES) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "no firmware matches the chip rev number!!! use default\n")));
        ix = WC40_UCODE_NUM_ENTRIES - 1;
    }  
    for (; ix >= 0; ix--) {
        if ((wc40_ucodes[ix].pdata != NULL) && 
            (*(wc40_ucodes[ix].plen) != 0)) {
            break;
        }
    }
    if (ix < 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "no valid firmware found!!!\n")));
        return SOC_E_NOT_FOUND;
    }
    
    *ppdata = wc40_ucodes[ix].pdata;
    *len = *(wc40_ucodes[ix].plen);
    return SOC_E_NONE;
}


STATIC int
_phy_wc40_config_init(int unit, soc_port_t port)
{
    WC40_DEV_CFG_t *pCfg;
    WC40_DEV_INFO_t  *pInfo;
    phy_ctrl_t      *pc;
    WC40_DEV_DESC_t *pDesc;
    uint16 serdes_id0;
    WC40_TX_DRIVE_t *p_tx;
    int i;
    int line_intf = 0;

    pc = INT_PHY_SW_STATE(unit, port);

    pDesc = (WC40_DEV_DESC_t *)(pc + 1);

    /* clear the memory */
    WC40_MEM_SET(pDesc, 0, sizeof(*pDesc));

    pCfg = &pDesc->cfg; 
    pInfo = &pDesc->info;

    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESID_SERDESID0r(unit, pc, 0x00, &serdes_id0));
    pInfo->serdes_id0 = serdes_id0;

    /* init the default configuration values */

    for (i = 0; i < NUM_LANES; i++) {
        pCfg->preemph[i] = WC40_NO_CFG_VALUE;
        pCfg->idriver[i] = WC40_NO_CFG_VALUE;
        pCfg->pdriver[i] = WC40_NO_CFG_VALUE;
        pCfg->post2driver[i] = WC40_NO_CFG_VALUE;
    }

   /* default */
    p_tx = &pCfg->tx_drive[TXDRV_DFT_INX];
    p_tx->u.tap.post  = 0x00;
    p_tx->u.tap.main  = 0x00;
    p_tx->u.tap.pre   = 0x00;
    p_tx->u.tap.force = 0x00;
    p_tx->post2     = 0x00;
    p_tx->idrive    = 0x09;
    p_tx->ipredrive = 0x09;

    pCfg->hg_mode    = FALSE;
    pCfg->sgmii_mstr = FALSE;
    pCfg->pdetect10g = TRUE;
    pCfg->cx42hg     = FALSE;
    pCfg->rxlane_map = WC40_RX_LANE_MAP_DEFAULT;
    pCfg->txlane_map = WC40_RX_LANE_MAP_DEFAULT;
    pCfg->medium = SOC_PORT_MEDIUM_COPPER;
    pCfg->cx4_10g    = TRUE;
    pCfg->lane0_rst  = TRUE;  
    pCfg->rxaui      = FALSE;
    pCfg->load_mthd = 2;             
    pCfg->uc_cksum  = 0;
    pCfg->refclk    = 156;

    /* this property is for debug and diagnostic purpose. byte0: 
     * 0: not loading WC firmware
     * 1: load from MDIO. default method.
     * 2: load from parallel bus if applicable. Provide fast downloading time
     * 
     * byte1:
     * 0: inform uC not to perform checksum calculation(default). Save ~70ms for WC init time
     * 1: inform uC to perform checksum calculation. 
     */
    pCfg->load_mthd = soc_property_port_get(unit, port, spn_LOAD_FIRMWARE, pCfg->load_mthd);
    pCfg->load_mthd &= 0xff;
    pCfg->uc_cksum = soc_property_port_get(unit, port, spn_LOAD_FIRMWARE, pCfg->uc_cksum);
    pCfg->uc_cksum = (pCfg->uc_cksum >> 8) & 0xff;

    /* before performing device configuration, must not call any driver function. 
     * Only allowed operation is to read common register shared by all four lanes
     */
    SOC_IF_ERROR_RETURN(phy_wc40_config_init(pc));

    /* CL73 auto recover feature:
       Monitor the CL73 fsm for particular FSM states
       and take action if it sees the same state for a 
       a fixed number of times (implying stuck fsm) to 
       recover fsm.
       TO disable this feature use PHY ctrl:
       PORT_PHY_CONTROL_CL73_FSM_AUTO_RECOVER.

       Recommended by DE to enable it by default.

    */
    pCfg->cl73_fsm_auto_recover.enable = 1;

    /* Select firmware based implementation */
    pCfg->force_cl72_fw = 1; /* Select by default */

    /* Forced cl72 config - enabled(default)/disabled */
    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) {
        pCfg->force_cl72 = 0;
    } else {
        pCfg->force_cl72 = 1;
    }
    pCfg->force_cl72 = soc_property_port_get(unit, port,
                                        spn_PORT_INIT_CL72, pCfg->force_cl72);

    line_intf = soc_property_port_get(unit, port, spn_SERDES_IF_TYPE, line_intf) ;
    if (line_intf > 0) {
         pCfg->line_intf = 1 << line_intf;
    }

    {
        uint16 data16;

        SOC_IF_ERROR_RETURN
            (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit,pc,LANE0_ACCESS,&data16));
        if (((data16 >> (pc->lane_num * 4)) & 0xf) == WC40_UC_CTRL_SFP_DAC) {
            pCfg->medium = SOC_PORT_MEDIUM_COPPER;
        } else {
            pCfg->medium = SOC_PORT_MEDIUM_FIBER;
        }
    }

    
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        int current_ver = BCM_WB_DEFAULT_VERSION;
        int rv = SOC_E_NONE;
        int intf;
        int scr;
        int speed;

        intf = 0;
        /* recovery software states from device */
        /* recover interface value from hardware */
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            SOC_IF_ERROR_RETURN
                (_phy_wc40_combo_speed_get(unit, port, &speed,&intf,&scr));
        } else {
            SOC_IF_ERROR_RETURN
                (_phy_wc40_ind_speed_get(unit, port, &speed,&intf,&scr));
        }
        pCfg->line_intf =  1 << intf;
        pCfg->scrambler_en = scr;
        
        rv = wc40_wb_sc_reinit(unit, port);
        if(rv == SOC_E_NONE){
            if(current_ver > pCfg->scache_ver) { /* upgrade expandsion and update */ 
                SOC_IF_ERROR_RETURN(wc40_wb_sc_realloc_init(unit, port)) ;
                SOC_IF_ERROR_RETURN(wc40_wb_sc_sync(unit, port)) ;
            }
        }else if(rv == SOC_E_NOT_FOUND){
            SOC_IF_ERROR_RETURN(wc40_wb_sc_init(unit, port)) ;
            SOC_IF_ERROR_RETURN(wc40_wb_sc_sync(unit, port)) ;
        }else{
            return rv;
        }
    } else {
            SOC_IF_ERROR_RETURN(wc40_wb_sc_init(unit, port)) ;
            SOC_IF_ERROR_RETURN(wc40_wb_sc_sync(unit, port)) ;
    }
#endif
    
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_regbit_set_wait_check(phy_ctrl_t *pc,
                  int reg,   /* register to check */
                  int bit_num,   /* bit number to check */
                  int bitset, /* check bit set or bit clear */
                  int timeout,  /* max wait time to check */
                  int lane_ctrl) /*  point to the specified lane  */
{
    uint16           data16 = 0;
    soc_timeout_t    to;
    int              rv;
    int  cond;

    soc_timeout_init(&to, timeout, 0);
    do {
        rv = WC40_REG_READ(pc->unit, pc, lane_ctrl, reg,&data16);
        cond = (bitset && (data16 & bit_num)) || 
               (!bitset && !(data16 & bit_num));
        if (cond || SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    return (cond? SOC_E_NONE: SOC_E_TIMEOUT);
}

STATIC int
_phy_wc40_pll_lock_wait(int unit, soc_port_t port)
{
    phy_ctrl_t      *pc;
    int              rv;

    pc = INT_PHY_SW_STATE(unit, port);
    rv = _phy_wc40_regbit_set_wait_check(pc,WC40_XGXSBLK0_XGXSSTATUSr,
                     XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK,1,WC40_PLL_WAIT,0);
 
    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WC40 : TXPLL did not lock: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_TIMEOUT);
    }
    return (SOC_E_NONE);
}

/*
 * Select the analog Rx div/16 and div/33 clocks for digital lanes 
 * this function must called after RX lane swap is done 
 */
STATIC int
_phy_wc40_rx_div16_clk_select(int unit, phy_ctrl_t *pc)
{
    uint16 rxlane_map;
    uint16 data16;
    uint16 mask16;
    uint16 div16_mode;
    uint16 div33_mode;

    if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
    
        /* check the pcs lane remapping register */
        SOC_IF_ERROR_RETURN     
            (READ_WC40_XGXSBLK8_RXLNSWAP1r(unit, pc, LANE0_ACCESS, &rxlane_map));

        SOC_IF_ERROR_RETURN     
            (MODIFY_WC40_XGXSBLK2_TESTMODEMUXr(unit, pc, LANE0_ACCESS,
                   (rxlane_map<<8),0xFF00));

        mask16 = 0x80;
        data16 = 0;
        /* enable override in single lane port */
        if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            if (!IS_DUAL_LANE_PORT(pc)) {
                data16 = mask16;
            }
        }
        SOC_IF_ERROR_RETURN     
            (MODIFY_WC40_XGXSBLK2_TESTMODEMUXr(unit, pc, LANE0_ACCESS, data16,mask16));

    } else { /* B0 and above */
        /* 
         * Table 20 Rev B0 Lane Swap Programming Settings
         * Mode       0x8108.7  0x82EC[8:5] 0x82EC[4:3]
         * Single     0x0   0x1         0x1
         * Dual       0x0   0x2         0x2
         * Quad       0x0   0x3         0x3
         */
        /* not to force the override */
        SOC_IF_ERROR_RETURN     
            (MODIFY_WC40_XGXSBLK2_TESTMODEMUXr(unit, pc, LANE0_ACCESS, 0,0x80));

        /* configure for single port mode */
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            div16_mode = 1;
            div33_mode = 1;

        } else { /* quad-port or dual-port modes */

            /* dual-port mode */
            if (IS_DUAL_LANE_PORT(pc)) {
                div16_mode = 2;
                div33_mode = 2;
            } else {
            /* quad-port mode */
                div16_mode = 3;
                div33_mode = 3;
            }
        }
        SOC_IF_ERROR_RETURN     
            (MODIFY_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(unit, pc, LANE0_ACCESS, 
              CL72_MISC4_CTRL_DIV16_FIELD(div16_mode) |
              CL72_MISC4_CTRL_DIV33_FIELD(div33_mode),
              CL72_MISC4_CTRL_DIV16_MASK |
              CL72_MISC4_CTRL_DIV33_MASK));
    } 
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_rxlane_map_set(int unit, phy_ctrl_t *pc, uint16 req_map)
{
    uint16 i;
    uint16 lane, hw_map, chk_map;

    /* Update RX lane map */

    if (req_map != WC40_RX_LANE_MAP_DEFAULT) {
        hw_map  = 0;
        chk_map = 0;
        for (i = 0; i < 4; i++) {
            lane     = (req_map >> (i * 4)) & 0xf;
            hw_map  |= lane << (i * 2);
            chk_map |= 1 << lane;
        }
        if (chk_map == 0xf) {
#ifdef WC40_PCS_LANE_SWAP
            SOC_IF_ERROR_RETURN     /* Enable RX Lane swap */
               (MODIFY_WC40_XGXSBLK8_RXLNSWAP1r(unit, pc, LANE0_ACCESS,
                   hw_map,
                   XGXSBLK2_RXLNSWAP_RX_LNSWAP_EN_MASK |
                   XGXSBLK8_RXLNSWAP1_RX0_LNSWAP_SEL_MASK |
                   XGXSBLK8_RXLNSWAP1_RX1_LNSWAP_SEL_MASK |
                   XGXSBLK8_RXLNSWAP1_RX2_LNSWAP_SEL_MASK |
                   XGXSBLK8_RXLNSWAP1_RX3_LNSWAP_SEL_MASK));
#else
            SOC_IF_ERROR_RETURN     /* Enable RX Lane swap */
               (MODIFY_WC40_XGXSBLK2_RXLNSWAPr(unit, pc,
                   XGXSBLK2_RXLNSWAP_RX_LNSWAP_EN_MASK |
                   XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE_EN_MASK | hw_map,
                   XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE_EN_MASK | 
                   XGXSBLK2_RXLNSWAP_RX_LNSWAP_EN_MASK |
                   XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE0_MASK |
                   XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE1_MASK |
                   XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE2_MASK |
                   XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE3_MASK));
#endif
        } else {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "unit %d port %s: Invalid RX lane map 0x%04x.\n"),
                       unit, SOC_PORT_NAME(unit, pc->port), req_map));
        }
    }
    return SOC_E_NONE;
}
STATIC int
_phy_wc40_txlane_map_set(int unit, phy_ctrl_t *pc, uint16 req_map)
{
    uint16 i;
    uint16 lane, hw_map, chk_map;

    /* Update TX lane map */

    if (req_map != WC40_TX_LANE_MAP_DEFAULT) {
        hw_map  = 0;
        chk_map = 0;
        for (i = 0; i < 4; i++) {
            lane     = (req_map >> (i * 4)) & 0xf;
            hw_map  |= lane << (i * 2);
            chk_map |= 1 << lane;
        }
        if (chk_map == 0xf) {
#ifdef WC40_PCS_LANE_SWAP
            SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
               (MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(unit, pc, LANE0_ACCESS,
                    hw_map,
                    XGXSBLK8_TXLNSWAP1_TX3_LNSWAP_SEL_MASK |
                    XGXSBLK8_TXLNSWAP1_TX2_LNSWAP_SEL_MASK |
                    XGXSBLK8_TXLNSWAP1_TX1_LNSWAP_SEL_MASK |
                    XGXSBLK8_TXLNSWAP1_TX0_LNSWAP_SEL_MASK));
#else
            SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
               (MODIFY_WC40_XGXSBLK2_TXLNSWAPr(unit, pc, 0x00, 
                    XGXSBLK2_TXLNSWAP_TX_LNSWAP_EN_MASK | hw_map,
                    XGXSBLK2_TXLNSWAP_TX_LNSWAP_EN_MASK |
                    XGXSBLK2_TXLNSWAP_TX_LNSWAP_FORCE0_MASK |
                    XGXSBLK2_TXLNSWAP_TX_LNSWAP_FORCE1_MASK |
                    XGXSBLK2_TXLNSWAP_TX_LNSWAP_FORCE2_MASK |
                    XGXSBLK2_TXLNSWAP_TX_LNSWAP_FORCE3_MASK));
#endif
        } else {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "unit %d port %s: Invalid TX lane map 0x%04x.\n"),
                       unit, SOC_PORT_NAME(unit, pc->port), req_map));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_ind_lane_polarity_set (int unit, phy_ctrl_t *pc, int enable)
{
    uint16 data16;
    uint16 mask16;
    int cur_lane = FALSE;
    int nxt_lane = FALSE;

    if (DEV_CFG_PTR(pc)->txpol) {
        /* Flip TX polarity */
        data16 = enable? TX0_ANATXACONTROL0_TXPOL_FLIP_MASK: 0;
        mask16 = TX0_ANATXACONTROL0_TXPOL_FLIP_MASK;

        if (IS_DUAL_LANE_PORT(pc)) {
            if (DEV_CFG_PTR(pc)->txpol == 1) {
                cur_lane = TRUE;
                nxt_lane = TRUE;
            } else {
                if ((DEV_CFG_PTR(pc)->txpol & 0x000F) == 0x000F) {
                    cur_lane = TRUE;
                } 
                if ((DEV_CFG_PTR(pc)->txpol & 0x00F0) == 0x00F0) {
                    nxt_lane = TRUE;
                }
            }
        } else {
            if((DEV_CFG_PTR(pc)->txpol == 1) 
                    || ((DEV_CFG_PTR(pc)->txpol & 0x000F) == 0x000F)) {
                cur_lane = TRUE;
            }
        }
        
        if (cur_lane) {    
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,0x0,
                    WC40_TX0_ANATXACONTROL0r + (0x10 * pc->lane_num), 
                     data16, mask16));
        }
        if (nxt_lane) {    
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,0x0,
                    WC40_TX0_ANATXACONTROL0r + (0x10 * (pc->lane_num + 1)), 
                     data16, mask16));
        }
    }

    cur_lane = FALSE;
    nxt_lane = FALSE;
    if (DEV_CFG_PTR(pc)->rxpol) {
        /* Flip Rx polarity */
        mask16 = RX0_ANARXCONTROLPCI_RX_POLARITY_FORCE_SM_MASK |
                 RX0_ANARXCONTROLPCI_RX_POLARITY_R_MASK;
        data16 = enable? mask16: 0;

        if (IS_DUAL_LANE_PORT(pc)) {
            if (DEV_CFG_PTR(pc)->rxpol == 1) {
                cur_lane = TRUE;
                nxt_lane = TRUE;
            } else {
                if ((DEV_CFG_PTR(pc)->rxpol & 0x000F) == 0x000F) {
                    cur_lane = TRUE;
                }
                if ((DEV_CFG_PTR(pc)->rxpol & 0x00F0) == 0x00F0) {
                    nxt_lane = TRUE;
                }
            }
        } else {
            if((DEV_CFG_PTR(pc)->rxpol == 1) 
                    || ((DEV_CFG_PTR(pc)->rxpol & 0x000F) == 0x000F)) {
                cur_lane = TRUE;
            }
        }

        if (cur_lane) {
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,0x0,
                    WC40_RX0_ANARXCONTROLPCIr + (0x10 * pc->lane_num),
                     data16, mask16));
        }
        if (nxt_lane) {
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,0x0,
                    WC40_RX0_ANARXCONTROLPCIr + (0x10 * (pc->lane_num + 1)),
                     data16, mask16));
        }
         
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wc40_firmware_mode_set(int unit, soc_port_t port, uint32 value) {
    uint16 data16 = 0;
    uint16 mask16 = 0;
    int lane_start;
    int lane_end;
    int i, rv;
    phy_ctrl_t     *pc;

    /* Param 'value' check */
    switch(value) {
        case SOC_PHY_FIRMWARE_DEFAULT:
            value = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_DEFAULT;
            break;
        case SOC_PHY_FIRMWARE_SFP_OPT_SR4:
            value = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_SFP_OPT_LR;
            break;
        case SOC_PHY_FIRMWARE_SFP_DAC:
            value = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_SFP_DAC;
            break;
        case SOC_PHY_FIRMWARE_XLAUI:
            value = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_XLAUI;
            break;
        case SOC_PHY_FIRMWARE_FORCE_OSDFE:
            value = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_LONG_CH_6G;
            break;
        case SOC_PHY_FIRMWARE_FORCE_BRDFE:
            value = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_BR_DFE;
            break;
        case SOC_PHY_FIRMWARE_SW_CL72:
            value = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_SW_CL72;
            break;
        case SOC_PHY_FIRMWARE_CL72_WITHOUT_AN:
            value = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_CL72_WO_AN;
            break;
        default:
            return SOC_E_PARAM;
    }    

    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_start = pc->lane_num;
        lane_end = lane_start + 1;
    } else {  /* quad-port mode */
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    }

    for(i = lane_start; i <= lane_end; ++i) {
        /* Restart the firmware */
        data16 = 3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
        data16 |= 1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT;
        mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
            DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc,
                                         ln_access[i],
                                         data16,mask16));

        /* wait for uC ready for command:  bit7=1 */
        rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
                1, WC40_PLL_WAIT,ln_access[i]);
        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_wc40_firmware_mode_set: uController not ready: u=%d p=%d\n lane=%d"), unit, port, i));
            return (SOC_E_TIMEOUT);
        }

        /* Stop the firmware */
        data16 = 0 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
        data16 |= 1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT;    
        mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
            DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, 
                                         ln_access[i], 
                                         data16,mask16));

        sal_usleep(1000);
        
        /* wait for uC ready for command:  bit7=1 */
        rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
                1, WC40_PLL_WAIT,ln_access[i]);
        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_wc40_firmware_mode_set: uController not ready (post stop): u=%d p=%d\n lane=%d"), unit, port, i));
            return (SOC_E_TIMEOUT);
        }
       
        /* Program firmware mode */
        mask16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK << 
            (UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_BITS * i);
        data16 = value << (UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_BITS * i);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 
                                            LANE0_ACCESS, data16, mask16));

        /* Resume the firmware */
        data16 = 2 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
        data16 |= 1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT;    
        mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
            DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, 
                                         ln_access[i], 
                                         data16,mask16));
        
        sal_usleep(1000);
        
        /* wait for uC ready for command:  bit7=1 */
        rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
                1, WC40_PLL_WAIT,ln_access[i]);
        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_wc40_firmware_mode_set: uController not ready (post resume): u=%d p=%d\n lane=%d"), unit, port, i));
            return (SOC_E_TIMEOUT);
        }
        
        /* Restart the firmware */
        data16 = 3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
        data16 |= 1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT;    
        mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
            DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, 
                                         ln_access[i], 
                                         data16,mask16));
        
        
        sal_usleep(1000);

        /* wait for uC ready for command:  bit7=1 */
        rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
                1, WC40_PLL_WAIT,ln_access[i]);
        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_wc40_firmware_mode_set: uController not ready (post restart): u=%d p=%d lane=%d\n"), unit, port, i));
        }
    }
    if (DEV_CFG_PTR(pc)->fw_dfe == SOC_PHY_DFE_FREEZE) {
       rv = _phy_wc40_firmware_dfe_mode_set(unit, port, DEV_CFG_PTR(pc)->fw_dfe); 
       if (rv != SOC_E_NONE) {
           LOG_WARN(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                        "%s: DFE Set fail: u=%d p=%d\n"),FUNCTION_NAME(), unit, port));
       }
    }

    return SOC_E_NONE;
}

STATIC int 
_phy_wc40_firmware_mode_get(int unit, soc_port_t port, uint32 *value) {
    uint16 data16 = 0, temp16 = 0;
    int lane_start;
    int lane_end;
    int i;
    phy_ctrl_t     *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_start = pc->lane_num;
        lane_end = lane_start + 1;
    } else {  /* quad-port mode */
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    }
    
    /* Read firmware mode */
    SOC_IF_ERROR_RETURN
        (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 
                                             LANE0_ACCESS, &data16));

    /* Check all the lanes have matching firmware mode */
    *value = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK 
        & (data16 >> (UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_BITS * lane_start));
    for(i = lane_start + 1; i <= lane_end; ++i) {
        temp16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK
            & (data16 >> (UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_BITS * i));
    
        if(*value != temp16) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "WC: _phy_wc40_firmware_mode_get: "
                                 "value mismatch u=%d p=%d\n"), unit, port));
            return SOC_E_FAIL;
        }

    }

    switch(*value) {
        case UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_DEFAULT:
            *value = SOC_PHY_FIRMWARE_DEFAULT;
            break;
        case UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_SFP_OPT_LR:
            *value = SOC_PHY_FIRMWARE_SFP_OPT_SR4;
            break;
        case UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_SFP_DAC: 
            *value = SOC_PHY_FIRMWARE_SFP_DAC;
            break;
        case UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_XLAUI:
            *value = SOC_PHY_FIRMWARE_XLAUI;
            break;
        case UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_LONG_CH_6G:
            *value = SOC_PHY_FIRMWARE_FORCE_OSDFE;
            break;
        case UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_BR_DFE:
            *value = SOC_PHY_FIRMWARE_FORCE_BRDFE;
            break;
        case UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_SW_CL72:
            *value = SOC_PHY_FIRMWARE_SW_CL72;
            break;
        case UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_CL72_WO_AN:
            *value = SOC_PHY_FIRMWARE_CL72_WITHOUT_AN;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wc40_firmware_dfe_mode_set(int unit, soc_port_t port, uint32 value) {
    uint16 data16 = 0;
    uint16 mask16 = 0;
    int lane_start;
    int lane_end;
    int i, rv;
    phy_ctrl_t     *pc;

    /* Param 'value' check */
    switch(value) {
        case SOC_PHY_DFE_AUTO:
            data16 = 0 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
            break;
        case SOC_PHY_DFE_FREEZE:
            data16 = 0x7f << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
            break;
        default:
            return SOC_E_PARAM;
    }    
    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_start = pc->lane_num;
        lane_end = lane_start + 1;
    } else {  /* quad-port mode */
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    }

    for(i = lane_start; i <= lane_end; ++i) {
        /* wait for uC ready for command:  bit7=1 */
        rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
                1, WC40_PLL_WAIT,ln_access[i]);
        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "%s: uController not ready: u=%d p=%d\n lane=%d"),
                     FUNCTION_NAME(), unit, port, i));
            return (SOC_E_TIMEOUT);
        }
        /*Command*/
        data16 |= 4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT;
        mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
                 DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc,
                                         ln_access[i],
                                         data16,mask16));
        
        sal_usleep(1000);
        /* wait for uC ready for command:  bit7=1 */
        rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
                1, WC40_PLL_WAIT,ln_access[i]);
        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "%s: uController not ready2: u=%d p=%d\n lane=%d"),
                     FUNCTION_NAME(), unit, port, i));
        }
    }
    DEV_CFG_PTR(pc)->fw_dfe = value;
    return SOC_E_NONE;
}

STATIC int 
_phy_wc40_firmware_dfe_mode_get(int unit, soc_port_t port, uint32 *value) {
    uint16 data16 = 0;
    int rv, lane;
    phy_ctrl_t     *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane = 0;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane = pc->lane_num;
    } else {  /* quad-port mode */
        lane = pc->lane_num;
    }

    rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
            DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
            1, WC40_PLL_WAIT,ln_access[lane]);
    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                            "%s: uController not ready: u=%d p=%d\n lane=%d"),
                             FUNCTION_NAME(), unit, port, lane));
        return (SOC_E_TIMEOUT);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], 0x8004));
    rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
            DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
            1, WC40_PLL_WAIT,ln_access[lane]);
    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "%s: uController not ready2: u=%d p=%d\n lane=%d"),
                 FUNCTION_NAME(), unit, port, lane));
        return (SOC_E_TIMEOUT);
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], &data16));
    
    switch(data16 >> DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT) {
        case 0x7f:
            *value = SOC_PHY_DFE_FREEZE;
            break;
        default:
            *value = SOC_PHY_DFE_AUTO;
            break;
    }
    return SOC_E_NONE;
}


STATIC int
_phy_wc40_combo_polarity_set (int unit, phy_ctrl_t *pc, int enable)
{
    uint16 data16;
    uint16 mask16;
    if (DEV_CFG_PTR(pc)->txpol) {
        /* Flip TX polarity */
        data16 = enable? TX0_ANATXACONTROL0_TXPOL_FLIP_MASK:0;
        mask16 = TX0_ANATXACONTROL0_TXPOL_FLIP_MASK;
        if (DEV_CFG_PTR(pc)->txpol == 1) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TXB_ANATXACONTROL0r(unit, pc, 0x00, data16, mask16));
        }
        if ((DEV_CFG_PTR(pc)->txpol & 0x000F) == 0x000F) {
            SOC_IF_ERROR_RETURN
              (MODIFY_WC40_TX0_ANATXACONTROL0r(unit, pc, 0x00, data16, mask16));
        }
        if ((DEV_CFG_PTR(pc)->txpol & 0x00F0) == 0x00F0) {
            SOC_IF_ERROR_RETURN
              (MODIFY_WC40_TX1_ANATXACONTROL0r(unit, pc, 0x00, data16, mask16));
        }
        if ((DEV_CFG_PTR(pc)->txpol & 0x0F00) == 0x0F00) {
            SOC_IF_ERROR_RETURN
              (MODIFY_WC40_TX2_ANATXACONTROL0r(unit, pc, 0x00, data16, mask16));
        }
        if ((DEV_CFG_PTR(pc)->txpol & 0xF000) == 0xF000) {
            SOC_IF_ERROR_RETURN
              (MODIFY_WC40_TX3_ANATXACONTROL0r(unit, pc, 0x00, data16, mask16));
        }
    }

    if (DEV_CFG_PTR(pc)->rxpol) {
        /* Flip Rx polarity */
        mask16 = RX0_ANARXCONTROLPCI_RX_POLARITY_FORCE_SM_MASK |
                 RX0_ANARXCONTROLPCI_RX_POLARITY_R_MASK;
        data16 = enable? mask16:0;
        if (DEV_CFG_PTR(pc)->rxpol == 1) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RXB_ANARXCONTROLPCIr(unit,pc,0x00,data16,mask16));
        } 
        if ((DEV_CFG_PTR(pc)->rxpol & 0x000F) == 0x000F) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX0_ANARXCONTROLPCIr(unit,pc,0x00,data16,mask16));
        } 
        if ((DEV_CFG_PTR(pc)->rxpol & 0x00F0) == 0x00F0) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX1_ANARXCONTROLPCIr(unit,pc,0x00,data16,mask16));
        } 
        if ((DEV_CFG_PTR(pc)->rxpol & 0x0F00) == 0x0F00) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX2_ANARXCONTROLPCIr(unit,pc,0x00,data16,mask16));
        } 
        if ((DEV_CFG_PTR(pc)->rxpol & 0xF000) == 0xF000) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX3_ANARXCONTROLPCIr(unit,pc,0x00,data16,mask16));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_cl72_enable(int unit, soc_port_t port, int en)
{
    phy_ctrl_t     *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        DUAL_LANE_BCST_ENABLE(pc);
        /* disable KR PMD start-up protocol */
        SOC_IF_ERROR_RETURN
          (MODIFY_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(unit,pc,0x00,
             en? PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK: 0,
             PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK));

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(unit, pc, 0x00,
               en? 0:CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK,
               CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK));
        DUAL_LANE_BCST_DISABLE(pc);

    } else {  /* combo mode */
        /* bcst to PMD */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,pc,0x00,
                     (AERBLK_AER_MMD_DEVICETYPE_PMA_PMD <<
                     AERBLK_AER_MMD_DEVICETYPE_SHIFT) |
                     WC_AER_BCST_OFS_STRAP));

        SOC_IF_ERROR_RETURN
          (WC40_REG_MODIFY(unit, pc, 0x00,  0x0096,
             en? PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK: 0,
             PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK));

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(unit, pc, 0x00,
               en? 0:CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK,
               CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK));

        /* restore AER */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,pc,0x00,0));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wc40_firmware_force_cl72_config
 * Purpose:     
 *      Configure the device to perform CL72 tunning in forced mode 
 *      (w/o autoneg) through firmware 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - Forced_CL72_WO_AN enable  
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
_phy_wc40_firmware_force_cl72_config(int unit, soc_port_t port, int enable)
{
    uint16 data16;
    uint16 mask16;
    int lane_start;
    int lane_end;
    int pll_reset = 0x00;
    int i, rv;
    phy_ctrl_t     *pc;
    
    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_start = pc->lane_num;
        lane_end = lane_start + 1;
    } else {  /* quad-port mode */
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    }
    

    if(enable) {
        /* 1. Program PLL into reset by programming 0x8000.13 == 1'b0. */
        SOC_IF_ERROR_RETURN
            (READ_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, &data16));
        mask16 = XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
        if(data16 & mask16) {
            /* Reset only if it is not already in reset */
            pll_reset = 1;
            data16 = 0x00;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00,
                                              data16, mask16));
        }
    }
    for (i = lane_start; i <= lane_end; i++) {
        /* Restart_cl72_training */
        mask16 =
        PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK;
        data16 = enable? mask16 : 0;
        if (!(SOC_IS_KATANA2(unit)) && !(lane_start == lane_end)) {/* Skiped for HG[11] */
            SOC_IF_ERROR_RETURN
              (MODIFY_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(unit, 
                  pc, ln_access[i], data16, mask16));
        }
        /* 3. Enable cl72 (0x82E3[15:14] == 0x3)
         *    0x82e3[15:14]=0x3 force enable cl72
         *                 =0x2 force disable cl72
         *                 =0x0 controlled by autoneg
         */
        mask16 = CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK |
                 CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK;
        data16 = enable? mask16: 
                       CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(unit, pc, 
                ln_access[i], data16, mask16));
        
        /* 4. Disable CL72 timers */
        mask16 = CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCE_MASK |
             CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCEVAL_MASK;
        data16 = enable? mask16: 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_MISC2_CONTROLr(unit, pc, 
                ln_access[i], data16, mask16));

       /* 5. 0x82e2[15] = 0 */ 
        mask16 = CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK;
        data16 = 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(unit, pc, 
                ln_access[i], data16, mask16));

       /* 6. Program firmware mode (per lane) to 0x7. */
       mask16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK << 
                           (UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_BITS * i);
       data16 = enable? WC40_UC_CTRL_CL72_WO_AN : WC40_UC_CTRL_DEFAULT;
       SOC_IF_ERROR_RETURN
           (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 
                                             LANE0_ACCESS, data16, mask16));

    }

    /* 7. Program Core PLL speed by setting 0x81FB[11:7] == 0x16 (22). 
     *    val = 0x16, PLL rate forced to 11.00GHz 
     *    val = 00, default (PLL rate computed from forced speed reg)
     */
    mask16 =  WC40_UC_INFO_B1_REGB_SERDES_PLL_RATE_MASK; 
    data16 = enable? 0x16 << WC40_UC_INFO_B1_REGB_SERDES_PLL_RATE_SHIFT: 0;
    SOC_IF_ERROR_RETURN    
        (MODIFY_WC40_UC_INFO_B1_REGBr(unit, pc, 0x00, data16, mask16)); 
            
    
    if(enable) {   
        for(i = lane_start; i <= lane_end; ++i) {

            /* wait for uC ready for command:  bit7=1 */
            rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
                                 1, WC40_PLL_WAIT,ln_access[i]);
            if (rv == SOC_E_TIMEOUT) {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "WC: Force CL72 uController not ready: u=%d p=%d\n"), 
                          unit, port));
                return (SOC_E_TIMEOUT);
            }

            /* 8. Write below values in one mdio write command. 
             *    This write should be done before configuring any 
             *    speed. 
             *  a. 0x820E[15:8] = 0x3 restart link 
             *  b. 0x820E[7:6] = 0x0  ready_cmd =0 , error =0 
             *  c. 0x820E[3:0] = 0x1  command restart 
            */
            data16 = 3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
            data16 |= 1;    
            mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
                     DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, 
                                             ln_access[i], 
                                             data16,mask16));
        }
    }
    
    sal_udelay(4000);
    
    /* 9. Release PLL reset (0x8000.13 == 1'b1). */
    if(pll_reset && enable) {
        /* if this routine did pll reset, release pll reset */
        mask16 = XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00,
                                          data16, mask16));
        (void) _phy_wc40_pll_lock_wait(unit, port);
        _phy_wc40_control_vco_disturbed_set(unit, port);
    }
 
    FORCE_CL72_STATE(pc) = WC40_FORCED_CL72_STATE_WAIT_4_LINK;
    FORCE_CL72_TICK_CNT(pc) = 0;

    return SOC_E_NONE;
}

/* 
 * configure the device to perform CL72 tunning in forced mode thru software 
 */

STATIC int
_phy_wc40_force_cl72_config(int unit, soc_port_t port, int enable)
{
    uint16 data16;
    uint16 mask16;
    uint16 ver = 0;
    int lane_start;
    int lane_end;
    int i;
    phy_ctrl_t     *pc;
    WC40_TX_DRIVE_t tx_drv[NUM_LANES];

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_WC40_UC_INFO_B1_VERSIONr(unit, pc, 0x00, &ver));
    
    /* Decide whether to use forced cl72 
       software implementation or firmware implementation (hw)
       For firmware version >= 0xd10a, use firmware implementation.
     */
    if( ((ver >= 0xd10a) || ((ver & 0xD000) == 0)) 
            && (DEV_CFG_PTR(pc)->force_cl72_fw))
    {
        _phy_wc40_firmware_force_cl72_config(unit, port, enable);
        return SOC_E_NONE;
    }

    DEV_CFG_PTR(pc)->force_cl72_fw  = FALSE;

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_start = pc->lane_num;
        lane_end = lane_start + 1;
    } else {  /* quad-port mode */
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    }

    for (i = lane_start; i <= lane_end; i++) {
        if (enable) { 
            /* enable sigdet status */ 
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,LANE0_ACCESS,
                   0x80b1 + (0x10 * i), 
                   RX0_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                   RX0_ANARXCONTROL_STATUS_SEL_MASK));
        }

        /* Disable CL72 timers */
        mask16 = CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCE_MASK |
             CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCEVAL_MASK;
        data16 = enable? mask16: 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_MISC2_CONTROLr(unit, pc, 
                ln_access[i], data16, mask16));

        /* 0x82e3[15:14]=0x3 force enable cl72
         *              =0x2 force disable cl72
         *              =0x0 controlled by autoneg
         */
        mask16 = CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK |
                 CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK;
        data16 = enable? mask16: 
                       CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(unit, pc, 
                ln_access[i], data16, mask16));
    
        if (!enable) {
            (void)_phy_wc40_tx_disable(unit,port,lane_start,lane_end,FALSE);

            /* clear rx pi reset */
            mask16 = DSC1B0_PI_CTRL0_PI_CW_RST_MASK;
            data16 = 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_PI_CTRL0r(unit, pc, ln_access[i], data16,
                      mask16));

            /* clear rx_asic reset */
            mask16 = DIGITAL5_MISC6_RESET_RX_ASIC_MASK;
            data16 = 0;
            SOC_IF_ERROR_RETURN
             (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, ln_access[i], data16,
                      mask16));
        }   
    }
    if (enable) {
        /* set uC firmware mode to default */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00, 0x0));

        /* configure the TX driver parameters, use AN entry  */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_tx_control_get(unit, port,&tx_drv[0],TXDRV_AN_INX));
        SOC_IF_ERROR_RETURN
            (_phy_wc40_tx_control_set(unit, port,&tx_drv[0]));

        FORCE_CL72_STATE(pc) = WC40_CL72_STATE_LINK_CHECK;
        FORCE_CL72_TICK_CNT(pc) = 0;
    }
    return SOC_E_NONE; 
}

STATIC int
_phy_wc40_tx_disable(int unit, soc_port_t port, int lane_start, int lane_end, int disable)
{
    uint16 data16;
    uint16 mask16;
    int i;
    phy_ctrl_t     *pc;
    pc = INT_PHY_SW_STATE(unit, port);

    for (i = lane_start; i <= lane_end; i++) {
        /* tx enable */
        mask16 = TXB_TX_DRIVER_ELECIDLE_MASK;
        data16 = disable? mask16: 0;
        SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,LANE0_ACCESS,
                   0x8067 + (0x10 * i), data16, mask16));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_force_cl72_init(int unit, soc_port_t port)
{
    uint16 data16;
    uint16 mask16;
    uint16 ver;
    int lane_start;
    int lane_end;
    int i;
    phy_ctrl_t     *pc;
    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_WC40_UC_INFO_B1_VERSIONr(unit, pc, 0x00, &ver));

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_start = pc->lane_num;
        lane_end = lane_start + 1;
    } else {  /* quad-port mode */
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    }

    for (i = lane_start; i <= lane_end; i++) {
         /* toggle reset_cl72 */
         mask16 = CL72_USERB1_CL72_DEBUG_4_REGISTER_MICRO_REG_RESET_MASK;
         data16 = 0;
         SOC_IF_ERROR_RETURN
             (MODIFY_WC40_CL72_USERB1_CL72_DEBUG_4_REGISTERr(unit, pc, 
                    ln_access[i], data16,mask16));

         data16 = mask16;
         SOC_IF_ERROR_RETURN
             (MODIFY_WC40_CL72_USERB1_CL72_DEBUG_4_REGISTERr(unit, pc, 
                    ln_access[i], data16,mask16));

         /* rx_asic_reset: put receiver in reset state */
         mask16 = DIGITAL5_MISC6_RESET_RX_ASIC_MASK;
         data16 = mask16;
         SOC_IF_ERROR_RETURN
             (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, ln_access[i], data16, 
                      mask16));

         /* firmware version d109 and above doesn't need pi reset*/
         if ((ver >= 0xd100) && (ver <= 0xd108)) {
             /* toggle rx pi reset */
             mask16 = DSC1B0_PI_CTRL0_PI_CW_RST_MASK;
             data16 = mask16;
             SOC_IF_ERROR_RETURN
                 (MODIFY_WC40_DSC1B0_PI_CTRL0r(unit, pc, ln_access[i], data16, 
                      mask16));

             data16 = 0;
             SOC_IF_ERROR_RETURN
                 (MODIFY_WC40_DSC1B0_PI_CTRL0r(unit, pc, ln_access[i], data16, 
                      mask16));
         }

         /* rxseqstart */
         mask16 = DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK;
         data16 = mask16;
         SOC_IF_ERROR_RETURN
             (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, pc, ln_access[i], 
                      data16, mask16));

         data16 = 0;
         SOC_IF_ERROR_RETURN
             (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, pc, ln_access[i],
                      data16, mask16));

         /* restart_cl72_training */
         mask16 = PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_RESTART_TRAINING_MASK;
         mask16 |=
         PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK;
         data16 = mask16;
         SOC_IF_ERROR_RETURN
          (MODIFY_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(unit, 
              pc, ln_access[i], data16, mask16));

        /* bring rx_asic out of reset */
        mask16 = DIGITAL5_MISC6_RESET_RX_ASIC_MASK;
        data16 = 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, ln_access[i], 
                    data16, mask16));

        /* remove transmit_disable */
        (void)_phy_wc40_tx_disable(unit,port,lane_start,lane_end,FALSE);
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_wc40_firmware_force_cl72_state_machine(int unit, soc_port_t port,int link)
{
    int lane;
    int lane_start;
    int lane_end;
    int rv = SOC_E_NONE;
    uint16 data16, mask16, pmd_restarted = 0x1;
    phy_ctrl_t     *pc;
    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3; 
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_start = pc->lane_num;
        lane_end = lane_start + 1;
    } else {  /* quad-port mode */
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    }


    switch (FORCE_CL72_STATE(pc)) {
        case WC40_FORCED_CL72_STATE_RESET:
             
            for(lane = lane_start; lane <= lane_end; ++lane) {

                /* wait for uC ready for command:  bit7=1 */
                rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,
                                 1, WC40_PLL_WAIT,ln_access[lane]);
                if (rv == SOC_E_TIMEOUT) {
                    LOG_WARN(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                         "WC: Force CL72 uController not ready: u=%d p=%d\n"), 
                              unit, port));
                    return (SOC_E_TIMEOUT);
                }

                /* Write below values in one mdio write command. 
                 * This write should be done before configuring any 
                 * speed. 
                 *  a. 0x820E[15:8] = 0x3 restart link 
                 *  b. 0x820E[7:6] = 0x0  ready_cmd =0 , error =0 
                 *  c. 0x820E[3:0] = 0x1  command restart 
                 */
                data16 = 3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
                data16 |= 1;    
                mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
                         DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, 
                                                 ln_access[lane], 
                                                 data16,mask16));
            }

            FORCE_CL72_STATE(pc) = WC40_FORCED_CL72_STATE_WAIT_4_LINK;
            FORCE_CL72_TICK_CNT(pc) = 0;
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "WC: force cl72: u=%d p=%d state STATE_RESET\n"), 
                      unit, port));
            break;

        case WC40_FORCED_CL72_STATE_WAIT_4_LINK:
            if(link) {
                /* Set pmd_restarted=0 for uController.
                   Reg 0x81F7..0x81FA[0]= 0
                 */
                for(lane = lane_start; lane <= lane_end; ++lane) {
                    SOC_IF_ERROR_RETURN
                        (WC40_REG_MODIFY(unit, pc, LANE0_ACCESS, 
                                 0x81f7 + (0x1 * lane), 0, 1));
                }
                FORCE_CL72_STATE(pc) = WC40_FORCED_CL72_STATE_LINK_GOOD;
            } else { /* link down */
                FORCE_CL72_TICK_CNT(pc) += 1;
                if (FORCE_CL72_TICK_CNT(pc) > 
                                  WC40_FORCED_CL72_LINK_WAIT_CNT_LIMIT) {
                    /* Timer expired, do not wait anymore - restart*/
                    FORCE_CL72_STATE(pc) = WC40_FORCED_CL72_STATE_RESET;
                    FORCE_CL72_TICK_CNT(pc) = 0;
                }
            } 
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "WC: force cl72: u=%d p=%d state WAIT 4 LINK: "
                                 "link: 0x%x\n"), unit, port, link));
            break;

        case WC40_FORCED_CL72_STATE_LINK_GOOD:
            if (link == FALSE) {
                pmd_restarted = 0x1; 
                for(lane = lane_start; lane <= lane_end; ++lane) {
                    SOC_IF_ERROR_RETURN
                        (WC40_REG_READ(unit, pc, LANE0_ACCESS, 
                                 0x81f7 + (0x1 * lane), &data16));
                    pmd_restarted &= (data16 & 0x1);
                    if(!pmd_restarted) {
                        LOG_INFO(BSL_LS_SOC_PHY,
                                 (BSL_META_U(unit,
                                             "WC: force cl72: u=%d p=%d state LINK GOOD "
                                             "pmd_restarted=0: " "link: 0x%x\n"), unit, port, link));
                        break;
                    }

                }
                FORCE_CL72_STATE(pc) = pmd_restarted ? 
                                     WC40_FORCED_CL72_STATE_WAIT_4_LINK : 
                                     WC40_FORCED_CL72_STATE_RESET;
                FORCE_CL72_TICK_CNT(pc) = 0;
            
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "WC: force cl72: u=%d p=%d state LINK GOOD: "
                                     "link: 0x%x\n"), unit, port, link));
            }
            break;
        default:
            break;
    } 
    return rv;
}

STATIC int
_phy_wc40_force_cl72_state_machine(int unit, soc_port_t port,int link)
{
    uint16 data16;
    int i;
    int lane_start;
    int lane_end;
    int sig_loss;
    phy_ctrl_t     *pc;
    pc = INT_PHY_SW_STATE(unit, port);
    
    /* Decide whether to use forced cl72 
       software implementation or firmware implementation (hw)
       For firmware version >= 0xd10a, use firmware implementation.
    */
    if(DEV_CFG_PTR(pc)->force_cl72_fw)
    {
        _phy_wc40_firmware_force_cl72_state_machine(unit, port, link);
        return SOC_E_NONE;
    }

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3; 
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_start = pc->lane_num;
        lane_end = lane_start + 1;
    } else {  /* quad-port mode */
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    }
    
    switch (FORCE_CL72_STATE(pc)) {
        case WC40_CL72_STATE_TX_DISABLED:
            FORCE_CL72_STATE(pc) = WC40_CL72_STATE_INIT_CL72;
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "force cl72: u=%d p=%d state TX_DISABLED\n"), unit, port));
            break;

        case WC40_CL72_STATE_INIT_CL72:
            (void)_phy_wc40_force_cl72_init(unit,port);
            FORCE_CL72_TICK_CNT(pc) = 0;
            FORCE_CL72_STATE(pc) = WC40_CL72_STATE_LINK_CHECK;
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "force cl72: u=%d p=%d state INIT_CL72\n"), unit, port));
            break;

        case WC40_CL72_STATE_LINK_CHECK:
            sig_loss = 0;
            for (i = lane_start; i <= lane_end; i++) {
                data16 = 0;
                SOC_IF_ERROR_RETURN
                        (WC40_REG_READ(unit,pc,LANE0_ACCESS,
                            0x80b0 + (0x10 * i), &data16));
                sig_loss |= data16 & 
                    RX0_ANARXSTATUS_SIGDET_STATUS_CS4_SIGDET_LL_MASK;
            }
            if (sig_loss) {
                for (i = lane_start; i <= lane_end; i++) {
                    /* tx disable */
                    (void)_phy_wc40_tx_disable(unit,port,lane_start,
                            lane_end,TRUE);
                }
                FORCE_CL72_STATE(pc) = WC40_CL72_STATE_TX_DISABLED;
            } else if (link == FALSE) {
                FORCE_CL72_TICK_CNT(pc) += 1;
                if (FORCE_CL72_TICK_CNT(pc) > WC40_CL72_LINK_WAIT_CNT_LIMIT) {
                    for (i = lane_start; i <= lane_end; i++) {
                        /* tx disable */
                        (void)_phy_wc40_tx_disable(unit,port,lane_start,
                                lane_end,TRUE);
                    }
                    FORCE_CL72_STATE(pc) = WC40_CL72_STATE_TX_DISABLED;
                }
            } else {  /* link up */
                FORCE_CL72_STATE(pc) = WC40_CL72_STATE_LINK_GOOD;
            }
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "force cl72: u=%d p=%d state LINK_CHECK sig_loss: 0x%x "
                                    "link: 0x%x\n"), unit, port,sig_loss,link));
            break;

        case WC40_CL72_STATE_LINK_GOOD:
            if (link == FALSE) {
                FORCE_CL72_STATE(pc) = WC40_CL72_STATE_TX_DISABLED;
                (void)_phy_wc40_tx_disable(unit,port,lane_start,
                            lane_end,TRUE);
            }
            LOG_VERBOSE(BSL_LS_SOC_PHY,
                        (BSL_META_U(unit,
                                    "force cl72: u=%d p=%d state LINK_GOOD: link 0x%x\n"), 
                         unit, port,link));
            break;
        default:
            break;
    } 
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_tx_control_get(int unit, soc_port_t port, WC40_TX_DRIVE_t *tx_drv, int inx)
{
    phy_ctrl_t     *pc;
    int rv = SOC_E_NONE;
    int size;
    int i;

    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        size = 4;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        size = 2;
    } else {
        size = 1;
    }

    for (i = 0; i < size; i++) {
        if (DEV_CFG_PTR(pc)->idriver[i] == WC40_NO_CFG_VALUE) {
            /* take from the default value table */
            (tx_drv + i)->idrive    = DEV_CFG_PTR(pc)->tx_drive[inx].idrive;
        } else {
            /* taking from user */
            (tx_drv + i)->idrive = DEV_CFG_PTR(pc)->idriver[i];
        }
        if (DEV_CFG_PTR(pc)->pdriver[i] == WC40_NO_CFG_VALUE) {
            /* take from the default value table */
            (tx_drv + i)->ipredrive = DEV_CFG_PTR(pc)->tx_drive[inx].ipredrive;
        } else {
            /* taking from user */
            (tx_drv + i)->ipredrive = DEV_CFG_PTR(pc)->pdriver[i];
        }

        /* in autoneg mode no forced preemphasis */
        if ((DEV_CFG_PTR(pc)->preemph[i] == WC40_NO_CFG_VALUE) ||
            ((inx == TXDRV_AN_INX)) ) {
            /* take from the default value table */
            (tx_drv + i)->u.preemph = TXDRV_PREEMPH(DEV_CFG_PTR(pc)->tx_drive[inx].u.tap);
        } else {
            /* taking from user */
            (tx_drv + i)->u.preemph = DEV_CFG_PTR(pc)->preemph[i];
        }
        if (DEV_CFG_PTR(pc)->post2driver[i] == WC40_NO_CFG_VALUE) {
            /* take from the default value table */
            (tx_drv + i)->post2 = DEV_CFG_PTR(pc)->tx_drive[inx].post2;
        } else {
            /* taking from user */
            (tx_drv + i)->post2 = DEV_CFG_PTR(pc)->post2driver[i];
        }
    }

    return rv;
}

STATIC int
_phy_wc40_tx_control_set(int unit, soc_port_t port, WC40_TX_DRIVE_t *tx_drv)
{
    phy_ctrl_t     *pc;
    uint32          preemph;
    uint32          idriver;
    uint32          pdriver;
    uint32          post2;
    uint16          data16;
    int lane_num;
    int i;
    int size;
 
    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        size = 4;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        size = 2;
    } else {
        size = 1;
    }

    for (i = 0; i < size; i++) {
        idriver = (tx_drv + i)->idrive;
        pdriver = (tx_drv + i)->ipredrive;
        post2   = (tx_drv + i)->post2;
        preemph = (tx_drv + i)->u.preemph; 
        lane_num = pc->lane_num + i;
        /* WC40 SerDes
         *  idriver[11:8], pdriver[7:4],post2[14:12] 
         */
        idriver = (idriver << TXB_TX_DRIVER_IDRIVER_SHIFT) & 
              TXB_TX_DRIVER_IDRIVER_MASK;
        pdriver = (pdriver << TXB_TX_DRIVER_IPREDRIVER_SHIFT) &
              TXB_TX_DRIVER_IPREDRIVER_MASK;
        post2   = (post2 << TXB_TX_DRIVER_POST2_COEFF_SHIFT) &
              TXB_TX_DRIVER_POST2_COEFF_MASK;
        data16  = (uint16)(idriver | pdriver | post2);

        if (lane_num == 0) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX0_TX_DRIVERr(unit, pc, 0x00, data16,
                              TXB_TX_DRIVER_IDRIVER_MASK |
                              TXB_TX_DRIVER_IPREDRIVER_MASK |
                              TXB_TX_DRIVER_POST2_COEFF_MASK));
            if (IS_DUAL_LANE_PORT(pc)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TX1_TX_DRIVERr(unit, pc, 0x00, data16,
                              TXB_TX_DRIVER_IDRIVER_MASK |
                              TXB_TX_DRIVER_IPREDRIVER_MASK |
                              TXB_TX_DRIVER_POST2_COEFF_MASK));
            }
        } else if (lane_num == 1) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX1_TX_DRIVERr(unit, pc, 0x00, data16,
                              TXB_TX_DRIVER_IDRIVER_MASK |
                              TXB_TX_DRIVER_IPREDRIVER_MASK |
                              TXB_TX_DRIVER_POST2_COEFF_MASK));
        } else if (lane_num == 2) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX2_TX_DRIVERr(unit, pc, 0x00, data16,
                              TXB_TX_DRIVER_IDRIVER_MASK |
                              TXB_TX_DRIVER_IPREDRIVER_MASK |
                              TXB_TX_DRIVER_POST2_COEFF_MASK));
            if (IS_DUAL_LANE_PORT(pc)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TX3_TX_DRIVERr(unit, pc, 0x00, data16,
                              TXB_TX_DRIVER_IDRIVER_MASK |
                              TXB_TX_DRIVER_IPREDRIVER_MASK |
                              TXB_TX_DRIVER_POST2_COEFF_MASK));
            }
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX3_TX_DRIVERr(unit, pc, 0x00, data16,
                              TXB_TX_DRIVER_IDRIVER_MASK |
                              TXB_TX_DRIVER_IPREDRIVER_MASK |
                              TXB_TX_DRIVER_POST2_COEFF_MASK));
        }

        /* Warpcore preemphasis fields have been taken over by IEEE CL72.
         * It is automatically handled by H/W by default. But application can override 
         * these values by writing to 0x82e2. The value preemph should has the same
         * format as the register 0x82e2.
         */ 

        SOC_IF_ERROR_RETURN
            (WRITE_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(unit, pc, ln_access[lane_num],
                  preemph));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_tx_fir_default_set(int unit, soc_port_t port,int lane)
{
    uint16              data16, mask16;
    phy_ctrl_t         *pc;
    pc                       = INT_PHY_SW_STATE(unit, port);

    if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
        SOC_IF_ERROR_RETURN
             (MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(unit, pc, ln_access[lane],
                63 << CL72_USERB0_CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_SHIFT,
               CL72_USERB0_CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_MASK));

        mask16 = CL72_USERB0_CL72_KR_DEFAULT_CONTROL_TX_FIR_TAP_POST_KR_INIT_VAL_MASK |
                 CL72_USERB0_CL72_KR_DEFAULT_CONTROL_TX_FIR_TAP_MAIN_KR_INIT_VAL_MASK |
                 CL72_USERB0_CL72_KR_DEFAULT_CONTROL_TX_FIR_TAP_PRE_KR_INIT_VAL_MASK;
        data16 = (22 << CL72_USERB0_CL72_KR_DEFAULT_CONTROL_TX_FIR_TAP_POST_KR_INIT_VAL_SHIFT) |
                 (37 << CL72_USERB0_CL72_KR_DEFAULT_CONTROL_TX_FIR_TAP_MAIN_KR_INIT_VAL_SHIFT) |
                 (4 << CL72_USERB0_CL72_KR_DEFAULT_CONTROL_TX_FIR_TAP_PRE_KR_INIT_VAL_SHIFT);
    
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_KR_DEFAULT_CONTROLr(unit, pc, ln_access[lane], 
                            data16,mask16));
    
        mask16 = CL72_USERB0_CL72_BR_DEFAULT_CONTROL_TX_FIR_TAP_POST_BR_INIT_VAL_MASK;
        data16 = 22 << CL72_USERB0_CL72_BR_DEFAULT_CONTROL_TX_FIR_TAP_POST_BR_INIT_VAL_SHIFT;
        mask16 += CL72_USERB0_CL72_BR_DEFAULT_CONTROL_TX_FIR_TAP_MAIN_BR_INIT_VAL_MASK;
        data16 += 37 << CL72_USERB0_CL72_BR_DEFAULT_CONTROL_TX_FIR_TAP_MAIN_BR_INIT_VAL_SHIFT;
        mask16 += CL72_USERB0_CL72_BR_DEFAULT_CONTROL_TX_FIR_TAP_PRE_BR_INIT_VAL_MASK;
        data16 += 4 << CL72_USERB0_CL72_BR_DEFAULT_CONTROL_TX_FIR_TAP_PRE_BR_INIT_VAL_SHIFT;
    
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_BR_DEFAULT_CONTROLr(unit, pc, ln_access[lane],
                    data16,mask16));
    
        mask16 = CL72_USERB0_CL72_OS_DEFAULT_CONTROL_TX_FIR_TAP_POST_OS_INIT_VAL_MASK;
        data16 = 0;
        mask16 += CL72_USERB0_CL72_OS_DEFAULT_CONTROL_TX_FIR_TAP_MAIN_OS_INIT_VAL_MASK;
        data16 += 63 << CL72_USERB0_CL72_OS_DEFAULT_CONTROL_TX_FIR_TAP_MAIN_OS_INIT_VAL_SHIFT;
        mask16 += CL72_USERB0_CL72_OS_DEFAULT_CONTROL_TX_FIR_TAP_PRE_OS_INIT_VAL_MASK;
        data16 += 0;
    
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_OS_DEFAULT_CONTROLr(unit, pc, ln_access[lane], 
                data16,mask16));
    
        mask16 = CL72_USERB0_CL72_2P5_DEFAULT_CONTROL_TX_FIR_TAP_POST_2P5_INIT_VAL_MASK;
        data16 = 0;
        mask16 += CL72_USERB0_CL72_2P5_DEFAULT_CONTROL_TX_FIR_TAP_MAIN_2P5_INIT_VAL_MASK;
        data16 += 63 << CL72_USERB0_CL72_2P5_DEFAULT_CONTROL_TX_FIR_TAP_MAIN_2P5_INIT_VAL_SHIFT;
        mask16 += CL72_USERB0_CL72_2P5_DEFAULT_CONTROL_TX_FIR_TAP_PRE_2P5_INIT_VAL_MASK;
        data16 += 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB0_CL72_2P5_DEFAULT_CONTROLr(unit, pc, ln_access[lane], 
                   data16,mask16));
    
        mask16 = CL72_USERB1_CL72_DEBUG_1_REGISTER_TAP_MAX_VAL_MASK;
        data16 = 63 << CL72_USERB1_CL72_DEBUG_1_REGISTER_TAP_MAX_VAL_SHIFT;
        mask16 += CL72_USERB1_CL72_DEBUG_1_REGISTER_TAP_MIN_VAL_MASK;
        data16 += 27 << CL72_USERB1_CL72_DEBUG_1_REGISTER_TAP_MIN_VAL_SHIFT;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB1_CL72_DEBUG_1_REGISTERr(unit, pc, ln_access[lane],
                      data16,mask16));
    
        mask16 = CL72_USERB1_CL72_DEBUG_4_REGISTER_TAP_V2_VAL_MASK;
        data16 = 9 << CL72_USERB1_CL72_DEBUG_4_REGISTER_TAP_V2_VAL_SHIFT;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB1_CL72_DEBUG_4_REGISTERr(unit, pc, ln_access[lane], 
                   data16,mask16));

        if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            /* Write below values in one mdio write command.
             * This write should be done before configuring any speed.
             *  a. 0x820E[15:8] = 0x3 restart link
             *  b. 0x820E[7:6] = 0x0  ready_cmd =0 , error =0
             *  c. 0x820E[3:0] = 0x1  command restart
             */
            data16 = 3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
            data16 |= 1;
            mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
                     DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], data16,mask16));
        }
    } else if (WC40_REVID_B(pc)) {
        mask16 = CL72_USERB1_CL72_DEBUG_4_REGISTER_TAP_V2_VAL_MASK;
        data16 = 9 << CL72_USERB1_CL72_DEBUG_4_REGISTER_TAP_V2_VAL_SHIFT;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_USERB1_CL72_DEBUG_4_REGISTERr(unit, pc, ln_access[lane], 
                   data16,mask16));

        if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            /* Write below values in one mdio write command. 
             * This write should be done before configuring any speed. 
             *  a. 0x820E[15:8] = 0x3 restart link 
             *  b. 0x820E[7:6] = 0x0  ready_cmd =0 , error =0 
             *  c. 0x820E[3:0] = 0x1  command restart 
             */
            data16 = 3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
            data16 |= 1;    
            mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
                     DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], data16,mask16));
        }
    }
    return SOC_E_NONE; 
}

STATIC int
_phy_wc40_independent_lane_init(int unit, soc_port_t port)
{
    int                 fiber;
    uint16              data16, mask16;
    int                 rv;
    phy_ctrl_t         *pc;
    soc_port_ability_t  ability;
    WC40_TX_DRIVE_t tx_drv[NUM_LANES];

    /* Only need to track fiber mode state.
     * Copper mode state is tracked by external PHY.
     */
    pc                       = INT_PHY_SW_STATE(unit, port);
    fiber                    = DEV_CFG_PTR(pc)->fiber_pref;
    pc->fiber.enable         = fiber;
    pc->fiber.preferred      = fiber;
    pc->fiber.autoneg_enable = 1;
    pc->fiber.force_speed    = 1000;
    pc->fiber.force_duplex   = TRUE;
    pc->fiber.master         = SOC_PORT_MS_NONE;
    pc->fiber.mdix           = SOC_PORT_MDIX_NORMAL;


    /* cannot do the RESET on lane0 for B0 and above. The div16/div33 clock configuration 
     * for all lanes uses the per-lane register on lane0. 
     */ 
    if ((pc->lane_num != 0) || WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
        /* perform IEEE control register reset on each lane  */
     
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, MII_CTRL_RESET));

        rv = _phy_wc40_regbit_set_wait_check(pc,WC40_COMBO_IEEE0_MIICNTLr,
                     MII_CTRL_RESET,0,10000,0);

        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Combo SerDes reset failed: u=%d p=%d\n"),
                      unit, port));
        }
    }

    WC40_UDELAY(25000);
    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        data16 = 3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
        data16 |= 1;
        mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
                     DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
        SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[pc->lane_num], data16,mask16));
    }

    /*disbale the tx lane disable for hexli4 */
    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) { /* ToBeDeleted */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0,
                                          0, DIGITAL4_MISC3_LANEDISABLE_MASK));
    }

    /* restore tx fir setting after reset  */
    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_fir_default_set(unit,port,pc->lane_num));
    if (IS_DUAL_LANE_PORT(pc)) {
        SOC_IF_ERROR_RETURN
            (_phy_wc40_tx_fir_default_set(unit,port,pc->lane_num + 1));
    }

    /* configure autoneg default settings */
    SOC_IF_ERROR_RETURN  /* OS CDR default */
        (WRITE_WC40_CL72_USERB0_CL72_OS_DEFAULT_CONTROLr(unit, pc, 0x00,0x03f0));

    SOC_IF_ERROR_RETURN  /* BR CDR default */
        (WRITE_WC40_CL72_USERB0_CL72_2P5_DEFAULT_CONTROLr(unit, pc, 0x00,0x03f0));

    SOC_IF_ERROR_RETURN  /* BR CDR default */
        (MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(unit, pc, 0x00,
                  0x3f << CL72_USERB0_CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_SHIFT,
                  CL72_USERB0_CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_MASK));

    /* Warpcore B0 workaround in autoneg mode Jira# PHY-388 */
    if (WC40_REVID_B(pc) && (!IS_DUAL_LANE_PORT(pc))) {
        if (DEV_CFG_PTR(pc)->cl73an) {
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,LANE0_ACCESS,
                    WC40_RX0_ANARXCONTROLPCIr + (0x10 * pc->lane_num), 
                      RXB_ANARXCONTROLPCI_SYNC_STATUS_FORCE_R_SM_MASK,
                      RXB_ANARXCONTROLPCI_SYNC_STATUS_FORCE_R_SM_MASK |
                      RXB_ANARXCONTROLPCI_SYNC_STATUS_FORCE_R_MASK));
        }
    }

    /* Workaround Jira# SDK-32387, init. default IEEE 1G speed */
    /* the right register to program is the PMD register 0x800_0000. not the IEEE
        0x0000_0000 */
    if (WC40_SDK32387_REVS(pc) && (!IS_DUAL_LANE_PORT(pc))) {
        data16 = PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION1_MASK;
        mask16 = PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION0_MASK |
                 PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION1_MASK |
                 PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION2_MASK;
        SOC_IF_ERROR_RETURN
             (MODIFY_WC40_PMD_IEEE0BLK_PMD_IEEECONTROL1r(unit, pc, 0x00,
             data16, mask16));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00, 
                      XGXSBLK2_UNICOREMODE10G_RESERVED0_MASK,
                      XGXSBLK2_UNICOREMODE10G_RESERVED0_MASK));
    }

    /* 0x8357[10:9] = 2'b0
     * Set new advertisement page count identifier(5 user pages) that differentiates it from 
     * HyperCore or other legacy devices(3 user pages)
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL6_UD_FIELDr(unit, pc, 0x00, 
                DIGITAL6_UD_FIELD_UD_ADV_ID_WC, DIGITAL6_UD_FIELD_UD_ADV_ID_MASK));

    
    /* Stop the cl73an tx fifo reset routine in f/w if it is active */
    WC40_CL73_AN_TX_FIFO_RESET_STOP(pc);
    
    /* hold tx/rx in reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

    /* RX_LOS configuration */
    mask16 = SERDESDIGITAL_CONTROL1000X1_SIGNAL_DETECT_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_INVERT_SIGNAL_DETECT_MASK;
    data16 = 0;
    if (DEV_CFG_PTR(pc)->rx_los) {
        SOC_IF_ERROR_RETURN
            (WC40_REG_MODIFY(unit, pc, LANE0_ACCESS, 0x80b4 + (0x10*pc->lane_num),
                   0x60, 0xfc));
        data16 |= SERDESDIGITAL_CONTROL1000X1_SIGNAL_DETECT_EN_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_EEE_USERB0_EEE_COMBO_CONTROL0r(unit, pc, 0x00,
               0,
               EEE_USERB0_EEE_COMBO_CONTROL0_EEE_BYPASS_SIGDET_FILT_MASK));
    }
    data16 |= DEV_CFG_PTR(pc)->rx_los_invert? 
             SERDESDIGITAL_CONTROL1000X1_INVERT_SIGNAL_DETECT_MASK : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, pc, 0x00,data16,mask16));

    if (IS_DUAL_LANE_PORT(pc)) {
        /* sync clock divider for dxgxs two lanes, B0 and above */
        if (!(WC40_REVID_A0(pc) || WC40_REVID_A1(pc))) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL49_USERB0_CONTROLr(unit, pc, 0x00, 0xc0, 0xc0));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL49_USERB0_CONTROLr(unit, pc, ln_access[pc->lane_num+1], 
                             0xc0, 0xc0));
        }
   
        /* workaround for KR2 speed status, select the PCS type as 40GBASE-R PCS */
        if (WC40_REVID_B(pc)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_PCS_IEEE0BLK_PCS_IEEECONTROL2r(unit, pc, 0x00, 
                         0x4, PCS_IEEE0BLK_PCS_IEEECONTROL2_PCS_TYPESEL_MASK));
        }
    }

    /* cannot be applied in dual-lane port */
    if (!IS_DUAL_LANE_PORT(pc)) {
        /* keep CL48 sync acquisition state machine in reset to avoid CL73 problem*/
        if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit, pc, LANE0_ACCESS, 0x80ba+(0x10*pc->lane_num),
                       RX0_ANARXCONTROLPCI_LINK_EN_FORCE_SM_MASK,
                       RX0_ANARXCONTROLPCI_LINK_EN_FORCE_SM_MASK |
                       RX0_ANARXCONTROLPCI_LINK_EN_R_MASK));
        }
    }

    /* XXX not to control the speeds thru pma/pmd register */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                           0,
                           SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));

    /* check if TX/RX polarities should be flipped */
    SOC_IF_ERROR_RETURN
        (_phy_wc40_ind_lane_polarity_set(unit,pc, TRUE));

    /* config RX clock compensation used in single-lane and dual-lane ports */
    if (IS_DUAL_LANE_PORT(pc)) { 
        if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00,
                         IND_CLOCK_COMP_DEFAULT_REV_A));
        } else { /* B0 and other versions */ 
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00, 
                 0x8000 | IND_CLOCK_COMP_DEFAULT));
        }
    } else { 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_RX66_CONTROLr(unit, pc, 0x00,
        RX66_CONTROL_CC_EN_MASK |RX66_CONTROL_CC_DATA_SEL_MASK, 
                RX66_CONTROL_CC_EN_MASK | RX66_CONTROL_CC_DATA_SEL_MASK));
    }

    /*  support back to back packets in higig+/higig2 mode without IPG */
    if (!(WC40_REVID_A0(pc) || WC40_REVID_A1(pc))) {
        if (IS_DUAL_LANE_PORT(pc)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_XGMIIRCONTROLr(unit, pc, 0x00,
            DEV_CFG_PTR(pc)->hg_mode? XGXSBLK0_XGMIIRCONTROL_CKCMP_NOIPG_EN_MASK: 0, 
                XGXSBLK0_XGMIIRCONTROL_CKCMP_NOIPG_EN_MASK));
        } else { /* quad-port mode */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX66_CONTROLr(unit, pc, 0x00,
            DEV_CFG_PTR(pc)->hg_mode? RX66_CONTROL_CC_NOIPG_EN_MASK: 0, 
                RX66_CONTROL_CC_NOIPG_EN_MASK));
        }
    }

    /* Set Local Advertising Configuration */
    SOC_IF_ERROR_RETURN
        (phy_wc40_ability_local_get(unit, port, &ability));
 
    pc->fiber.advert_ability = ability;
    SOC_IF_ERROR_RETURN
        (phy_wc40_ability_advert_set(unit, port, &ability));

    data16 = SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_MASK |
             SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_MASK;
    mask16 = data16 | SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;

    if (DEV_CFG_PTR(pc)->pdetect1000x) {
        /* Enable 1000X parallel detect */
        data16 |= SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
    }
    
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0x00, data16, mask16));

    /* Configure default preemphasis, predriver, idriver values. Use
     * KR entry as default
     */
    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_control_get(unit, port,&tx_drv[0],TXDRV_DFT_INX));
    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_control_set(unit, port,&tx_drv[0]));

    /* Configuring operating mode */
    data16 = SERDESDIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK;
    mask16 = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK |
             SERDESDIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK;

    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    if (DEV_CFG_PTR(pc)->auto_medium) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK;
    }

    /*
     * Put the Serdes in Fiber or SGMII mode
     */
    if (DEV_CFG_PTR(pc)->fiber_pref) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    } else {
        data16 &= ~SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }

    if (DEV_CFG_PTR(pc)->sgmii_mstr) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK;
        mask16 |= SERDESDIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, pc, 0x00, data16, mask16));

    /* Set FIFO Elasticity to 13.5k and
     * Disable CRS generation on TX in half duplex mode
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X3r(unit, pc, 0x00,
             SERDESDIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK |
             (1 << 2),
             SERDESDIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK |
             SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_MASK));

    /* Set the dswin to 7 as deskew fifo depth is reduced */
    if (WC40_LINK_WAR_REVS(pc)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK2_ASWAP66CONTROL2r(unit, pc, 0x00, 0x7, 0x1F));
    }

    if (IS_DUAL_LANE_PORT(pc)) {
        DUAL_LANE_BCST_ENABLE(pc);
        /* config the 64B/66B for 20g dxgxs, won't affect other speeds and AN  */
        SOC_IF_ERROR_RETURN
            (_wc40_xgmii_scw_config (unit,pc));

        DUAL_LANE_BCST_DISABLE(pc);
    }

    if (IS_DUAL_LANE_PORT(pc)) {
        if (WC40_CL73_SOFT_KR2(pc)) {
            SOC_IF_ERROR_RETURN
                (_wc40_soft_an_cl73kr2_init(unit, port));
        }
    }

    /* autoneg is not supported in dual-port and custom1 mode */
    if (CUSTOM1_MODE(pc) || IS_DUAL_LANE_PORT(pc)) {
        uint16 sp_ctrl0;
        uint16 sp_ctrl1;
        uint16 speed_val = 0;
        int    tx_inx;

        DUAL_LANE_BCST_ENABLE(pc);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, 0, MII_CTRL_AE));

        
        if (CUSTOM1_MODE(pc)) {
            sp_ctrl0 = 0xf;
            sp_ctrl1 = DIGITAL4_MISC3_FORCE_SPEED_B5_MASK;
        } else { /* general dual-port mode, default 20G if supported, otherwise 10G*/
            SOC_IF_ERROR_RETURN
                (_phy_wc40_ind_speed_ctrl_get(unit,port,pc->speed_max,&speed_val,&tx_inx));
            sp_ctrl1 = (speed_val & 0x20)? DIGITAL4_MISC3_FORCE_SPEED_B5_MASK: 0;
            sp_ctrl0 = speed_val & 0x1f;
        }
        
        /* force to the speed */
        SOC_IF_ERROR_RETURN
             (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, sp_ctrl0, 
                                  SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
         SOC_IF_ERROR_RETURN
             (MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0x00, 
                     sp_ctrl1,
                     DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));
        DUAL_LANE_BCST_DISABLE(pc);

        if (SOC_IS_KATANA2(unit)) {
            _phy_wc40_speed_set(unit, port, pc->speed_max);
        }

    } else { /* all other modes, enable autoneg by default */
        /* clear forced speed control. Some two-lane ports are strapped
         * in forced dxgxs speed.
         */
        uint16 sp_ctrl0 = 0;
        uint16 sp_ctrl1 = 0;
        uint16 speed_val = 0;
        int    tx_inx;
        
        DUAL_LANE_BCST_ENABLE(pc);

        if (WC40_SDK32387_REVS(pc) && (!IS_DUAL_LANE_PORT(pc))) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                               SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK,
                               SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
        }

        /* If CL37 and CL73 is disabled, apply forced speed */
        if(!DEV_CFG_PTR(pc)->cl37an && !DEV_CFG_PTR(pc)->cl73an) {
        SOC_IF_ERROR_RETURN
                (_phy_wc40_ind_speed_ctrl_get(unit,port,pc->speed_max,&speed_val,&tx_inx));
            sp_ctrl1 = (speed_val & 0x20)? DIGITAL4_MISC3_FORCE_SPEED_B5_MASK: 0;
            sp_ctrl0 = speed_val & 0x1f;

        }

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, sp_ctrl0,
                               SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0x00, sp_ctrl1,
                    DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));

        if (WC40_SDK32387_REVS(pc) && (!IS_DUAL_LANE_PORT(pc))) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                       0,
                       SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
        }
        DUAL_LANE_BCST_DISABLE(pc);

        if (SOC_IS_KATANA2(unit)) {
            _phy_wc40_speed_set(unit, port, pc->speed_max);
        }

        mask16 = MII_CTRL_AE | MII_CTRL_RAN;
        
        /* Enable CL37 Autoneg*/
        data16 = 0;
        if (DEV_CFG_PTR(pc)->cl37an) {
            data16 = MII_CTRL_AE | MII_CTRL_RAN;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, 
                                                  data16, mask16));
        
        /* os8_txfifo_patch_en[3:0], which is a per-lane control 
           bit to enable the war_tx_os8_fifo_reset routine via s/w; 

           MDIO Address: 0xFFC8.3:0 = mdio_uC_mailbox[3:0] 
                                        = txfifo_patch_en[3:0]
           where txfifo_p/RECiu/atch_en[N] controls lane(N).
           Default: 0xFFC8.3:0 = txfifo_patch_en[3:0] = 4'h0
          
           S/W need to enable firmware WAR before enabling CL73

        */
        if((DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status == ENABLE) 
                || (DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status == REACTIVATE)){
            mask16 = 1 << pc->lane_num;
            data16 = mask16;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_MICROBLK_MDIO_UC_MAILBOXr(unit, pc, 0x0,
                                                    data16, mask16));
            DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status = ACTIVE;
        }

        data16 = 0; 
        /* Enable CL73 Autoneg*/
        if (DEV_CFG_PTR(pc)->cl73an) {
            data16 = MII_CTRL_AE | MII_CTRL_RAN;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 
                                                      0x00, data16, mask16)); 

    }

    if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
        /* put inband mdio rx in reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK3_LOCALCONTROL0r(unit,pc,0x00, (1 << 3), (1 << 3)));
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00,
              0,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

    WC40_CL73_AN_TX_FIFO_RESET_START(pc);

    /* XXX 0x833d bit 15 set, enable the auto select of KR,KRx4,KXx4 KX link
     * reporting selection of PCS device
     */
    SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL4_MISC4r(unit, pc, 0x00, 
                    DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK,
                    DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK));

    /* enable the LPI pass-thru by default */
    mask16 = (1 << 14) | (1 << 15);
    data16 = mask16;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL4_MISC5r(unit,pc,0x00, data16, mask16));
    mask16 = XGXSBLK7_EEECONTROL_LPI_EN_TX_MASK |
             XGXSBLK7_EEECONTROL_LPI_EN_RX_MASK |
             XGXSBLK7_EEECONTROL_LPI_COL_DET_EN_MASK;
    data16 = mask16;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK7_EEECONTROLr(unit,pc,0x00,data16,mask16));

    /* Clear the CL37 FEC advertisement */
    mask16 = DIGITAL3_UP3_FEC_MASK;
    data16 = 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL3_UP3r(unit, pc, 0x00, data16, mask16));

    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) {
     SOC_IF_ERROR_RETURN(phy_wc40_an_set(unit, port, 0)); 
        _phy_wc40_ind_speed_set(unit, port, pc->speed_max);
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_independent_lane_init: u=%d p=%d %s\n"),
              unit, port, (fiber) ? "Fiber" : "Copper"));
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_combo_core_init(int unit, soc_port_t port)
{
    phy_ctrl_t        *pc;
    soc_port_ability_t ability;
    uint16             data16, mask16;
    uint8 *pdata;
    int   ucode_len;
    int   alloc_flag;
    int   lane;
    WC40_TX_DRIVE_t tx_drv[NUM_LANES];

    pc = INT_PHY_SW_STATE(unit, port);

    /* must be done after reset, otherwise will be cleared  */
    data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_ComboCoreMode <<
                        XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT);
    SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00,data16,
                                       (XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK |
                                        XGXSBLK0_XGXSCONTROL_MODE_10G_MASK)));

    /*disbale the tx lane disable for hexli4 */
    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) { /* ToBeDeleted */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, LANE_BCST,
                                          0, DIGITAL4_MISC3_LANEDISABLE_MASK));
    }

    /* configure default fir settings for all lanes */
    for (lane = 0; lane < 4; lane++) {
        SOC_IF_ERROR_RETURN
            (_phy_wc40_tx_fir_default_set(unit,port,lane));
    }

    /* configure autoneg default settings */
    SOC_IF_ERROR_RETURN  /* OS CDR default */
        (WRITE_WC40_CL72_USERB0_CL72_OS_DEFAULT_CONTROLr(unit, pc, LANE_BCST,0x03f0));

    if (WC40_REVID_B(pc)) {
        SOC_IF_ERROR_RETURN  /* BR CDR default */
            (WRITE_WC40_CL72_USERB0_CL72_BR_DEFAULT_CONTROLr(unit, pc, LANE_BCST,0x035a));
    }

    /* 0x8357[10:9] = 2'b0
     * Set new advertisement page count identifier(5 user pages) that differentiates it from 
     * HyperCore or other legacy devices(3 user pages)
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL6_UD_FIELDr(unit, pc, 0x00, 
                DIGITAL6_UD_FIELD_UD_ADV_ID_WC, DIGITAL6_UD_FIELD_UD_ADV_ID_MASK));

    /* XXX not to control the speeds thru pma/pmd register */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                           0,
                           SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));

    /* configure txck/rx  */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc,0x00, 
                  WC40_TXCK_MASK | WC40_RXCK_OVERIDE_MASK,
                  WC40_TXCK_MASK | WC40_RXCK_OVERIDE_MASK)); 

    /* RX_LOS configuration */
    mask16 = SERDESDIGITAL_CONTROL1000X1_SIGNAL_DETECT_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_INVERT_SIGNAL_DETECT_MASK;
    data16 = 0;
    if (DEV_CFG_PTR(pc)->rx_los) {
        /* lane0 RX_LOS enable */
        SOC_IF_ERROR_RETURN
            (WC40_REG_MODIFY(unit, pc, LANE0_ACCESS, 0x80b4, 0x60, 0xfc));
        data16 |= SERDESDIGITAL_CONTROL1000X1_SIGNAL_DETECT_EN_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_EEE_USERB0_EEE_COMBO_CONTROL0r(unit, pc, 0x00,
               0,
               EEE_USERB0_EEE_COMBO_CONTROL0_EEE_BYPASS_SIGDET_FILT_MASK));
    }
    data16 |= DEV_CFG_PTR(pc)->rx_los_invert?
             SERDESDIGITAL_CONTROL1000X1_INVERT_SIGNAL_DETECT_MASK : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, pc, 0x00,data16,mask16));

    /* disable CL73 BAM */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(unit,pc,0x00,0,
               CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK));
                                                                                
    /* disable CL73 AN device*/
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit,pc,0x00,0));
    
    /* disable CL37 AN device*/
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, 0));

    /* Clear the CL37 FEC advertisement */
    mask16 = DIGITAL3_UP3_FEC_MASK;
    data16 = 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL3_UP3r(unit, pc, 0x00, data16, mask16));

    SOC_IF_ERROR_RETURN
        (phy_wc40_ability_local_get(unit, port, &ability));
    
    SOC_IF_ERROR_RETURN
        (phy_wc40_ability_advert_set(unit, port, &ability));

    data16 = 0;
    mask16 = XGXSBLK5_PARDET10GCONTROL_PD_12G_TXDON_DISABLE_MASK |
             XGXSBLK5_PARDET10GCONTROL_PD_12G_DISABLE_MASK |
             XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK;
    if (DEV_CFG_PTR(pc)->pdetect10g) { 
        data16 |=XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK;
        if (pc->speed_max <= 10000) {
            /* Disable parallel detect for 12Gbps. */
            data16 |= XGXSBLK5_PARDET10GCONTROL_PD_12G_TXDON_DISABLE_MASK |
                      XGXSBLK5_PARDET10GCONTROL_PD_12G_DISABLE_MASK;
        }
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(unit, pc, 0x00, data16, mask16));

    /* Configure default preemphasis, predriver, idriver values. Use
     * KR4 entry as default
     */
    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_control_get(unit, port,&tx_drv[0],TXDRV_DFT_INX));
    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_control_set(unit, port,&tx_drv[0]));

    /*
     * Transform CX4 pin out on the board to HG pinout
     */
    if (DEV_CFG_PTR(pc)->cx42hg) {
        /* If CX4 to HG conversion is enabled, do not allow individual lane
         * swapping.
         */
        SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
            (WRITE_WC40_XGXSBLK8_TXLNSWAP1r(unit, pc, 0x00, 0x80e4));

        SOC_IF_ERROR_RETURN     /* Flip TX Lane polarity */
            (WRITE_WC40_TXB_ANATXACONTROL0r(unit, pc, 0x00, 0x1020));
    } else {

        /* check if any tx/rx lanes are remapped */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_rxlane_map_set(unit,pc,
                 (uint16)(DEV_CFG_PTR(pc)->rxlane_map)));
   
        SOC_IF_ERROR_RETURN
            (_phy_wc40_txlane_map_set(unit,pc,
                 (uint16)(DEV_CFG_PTR(pc)->txlane_map)));
        
        /* Tx/Rx Polarity control */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_combo_polarity_set(unit,pc,TRUE));

        /* select div16 clock */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_rx_div16_clk_select(unit,pc));
    }
    /* Reset the value for this register to the recommended 
       values before f/w is started.
       This register is managed by firmware in cl73 mode

       Clear 0x80a1[3:2] for lane 0, lane 1, lane 2, lane 3 
       Cannot broadcast it using 0x80a1 since lanes could 
       have a different settings such as tx polarity.
       Broadcasting will write over the same setting for 
       all the lanes. Reset the bits individually per lane.
    */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX0_ANATXACONTROL0r(unit, pc, 0x0,
            0x00,
            TX0_ANATXACONTROL0_EDEN_FORCE_R_MASK 
            | TX0_ANATXACONTROL0_EDEN_R_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX1_ANATXACONTROL0r(unit, pc, 0x0,
            0x00,
            TX1_ANATXACONTROL0_EDEN_FORCE_R_MASK 
            | TX1_ANATXACONTROL0_EDEN_R_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX2_ANATXACONTROL0r(unit, pc, 0x0,
            0x00,
            TX2_ANATXACONTROL0_EDEN_FORCE_R_MASK 
            | TX2_ANATXACONTROL0_EDEN_R_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX3_ANATXACONTROL0r(unit, pc, 0x0,
            0x00,
            TX3_ANATXACONTROL0_EDEN_FORCE_R_MASK 
            | TX3_ANATXACONTROL0_EDEN_R_MASK));

    if (DEV_CFG_PTR(pc)->refclk == 161) {
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_SERDESDIGITAL_MISC1r(unit,pc,0x00,0xB900));
    }

    /* broadcast to all lanes */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,pc,0x00, WC_AER_BCST_OFS_STRAP));

    /* set BRCM 31G control */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, 0x00, 
                       DIGITAL5_MISC6_USE_BRCM6466_31500_CYA_MASK, 
                       DIGITAL5_MISC6_USE_BRCM6466_31500_CYA_MASK));

    /* config the 64B/66B for 25G, won't affect other speeds and AN  */
    SOC_IF_ERROR_RETURN
        (_wc40_xgmii_scw_config (unit,pc));

    /* Set FIFO Elasticity to 13.5k and
     * Disable CRS generation on TX in half duplex mode
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X3r(unit, pc, 0x00,
             SERDESDIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK |
             (1 << 2),
             SERDESDIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK |
             SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_MASK));

    /* clock compensation configuration in combo mode */
    if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00, 
                          COMBO_CLOCK_COMP_DEFAULT));
    } else {  /* B0 and ... */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00, 
                0x8000 | COMBO_CLOCK_COMP_DEFAULT));
    }

    /* Reset the value for these registers to the recommended 
       values before f/w is started.
       These registers are managed by firmware in cl73 mode
    */
    /* Set 0x8370[1] */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_UCTRL1r(unit, pc, 0x0, 
            CL73_USERB0_CL73_UCTRL1_CL73_USTAT1_MUXSEL_MASK,
            CL73_USERB0_CL73_UCTRL1_CL73_USTAT1_MUXSEL_MASK));

    /* Clear 0x8378[1], 0x8378[3] & 0x8378[12] */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_UCTRL2r(unit, pc, 0x0, 
            0x0000,
            CL73_USERB0_CL73_UCTRL2_CONFIG_MATCH_DIS_MASK 
            | CL73_USERB0_CL73_UCTRL2_SIGDET_TIMER_DIS_MASK
            | 0x1000));
    
    /* Disable firmware war for OS8 CL73 tx fifo reset */            
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_MICROBLK_MDIO_UC_MAILBOXr(unit, pc, 0x0,
                                               0, 0xf));
    /* reset AER */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,pc,0x00, 0));

    /* Set the dswin to 7 as deskew fifo depth is reduced */
    if (WC40_LINK_WAR_REVS(pc)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK2_ASWAP66CONTROL2r(unit, pc, 0x00, 0x7, 0x1F));
    }

    /*
     * Configure signal auto-detection between SGMII and fiber.
     */
    data16 = SERDESDIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK;
    mask16 = SERDESDIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK |
             SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;

    if (DEV_CFG_PTR(pc)->auto_medium) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK;
    }
   
    if (DEV_CFG_PTR(pc)->fiber_pref) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, pc, 0x00, data16, mask16));

    /* set filter_force_link and disable_false_link */

    mask16 = SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_MASK |
             SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_MASK;
    data16 = mask16;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0x00, data16, mask16));

    if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
        /* put inband mdio rx in reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK3_LOCALCONTROL0r(unit,pc,0x00, (1 << 3), (1 << 3)));
    }

    /* uCode should be done in the last in init sequence. Firmware may do additional
     * configuration.
     */
    if (DEV_CFG_PTR(pc)->load_mthd) {
        SOC_IF_ERROR_RETURN
            (_phy_wc40_ucode_get(unit,port,&pdata,&ucode_len,&alloc_flag));
        SOC_IF_ERROR_RETURN
            (phy_wc40_firmware_load(unit,port,0,pdata, ucode_len));
        if (alloc_flag) {
            sal_free(pdata);
        }
    } else {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WC40 combo mode : uC RAM download skipped: u=%d p=%d\n"),
                  unit, port));
    }

    mask16 = XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
    data16 = mask16; 
    /* Workaround: tx fifo recenter problem in aggregated mode */ 
    if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
        mask16 |= XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK;
    }
    SOC_IF_ERROR_RETURN
         (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, data16, mask16));

    (void) _phy_wc40_pll_lock_wait(unit, port);

    /* disable CL73 AN device in case it is not used*/
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit,pc,0x00,0));
    
    /* put device in autoneg mode */
    /* clear forced bit */
    SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, 0, 
                         SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0x00, 0, 
                         DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));

    if (WC40_CL73_SOFT_KR2(pc)) {
        SOC_IF_ERROR_RETURN
            (_wc40_soft_an_cl73kr2_init(unit, port));
    } else {
        if (DEV_CFG_PTR(pc)->cl37an) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00,
                                          MII_CTRL_AE | MII_CTRL_RAN,
                                          MII_CTRL_AE | MII_CTRL_RAN));
        }
        if (DEV_CFG_PTR(pc)->cl73an) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 0x00,
                                          MII_CTRL_AE | MII_CTRL_RAN,
                                          MII_CTRL_AE | MII_CTRL_RAN));
        }
    }

    /* XXX 0x833d bit 15 set, enable the auto select of KR,KRx4,KXx4 KX link
     * reporting selection of PCS device
     */
    SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL4_MISC4r(unit, pc, 0x00, 
                    DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK,
                    DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK));

    /* XXX set default speed mode */
    /* SOC_IF_ERROR_RETURN(phy_wc40_an_set(unit, port, TRUE)); */
    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) {
     SOC_IF_ERROR_RETURN(phy_wc40_an_set(unit, port, 0)); 
        _phy_wc40_speed_set(unit, port, pc->speed_max);
    }


    return SOC_E_NONE;
}

STATIC int
_phy_wc40_interlaken_mode_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;
    uint16 pdetect;
    uint16 ctrl0;
    uint16             xgxs_ctrl;
    uint16             misc2;
    uint16 data16;
    uint16 mask16;
    uint8 *pdata;
    int   ucode_len;
    int   alloc_flag;

    pc = INT_PHY_SW_STATE(unit, port);

    if (!_wc40_chip_init_done(unit,pc->chip_num,pc->phy_mode)) {
        /* configure and initialize the resource shared by all 4 lanes*/

        SOC_IF_ERROR_RETURN(_wc40_soft_reset(unit,pc)); /* reset once */

        /* select multi mmd and multi-port address mode */
        data16 = XGXSBLK0_MMDSELECT_DEVCL22_EN_MASK |
                 XGXSBLK0_MMDSELECT_DEVDEVAD_EN_MASK |
                 XGXSBLK0_MMDSELECT_DEVPMD_EN_MASK  |
                 XGXSBLK0_MMDSELECT_DEVAN_EN_MASK |
                 XGXSBLK0_MMDSELECT_MULTIMMDS_EN_MASK;
        mask16 = data16 |
                 XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK;

        if (!(pc->flags & PHYCTRL_MDIO_ADDR_SHARE)) {
            data16 |= XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_MMDSELECTr(unit, pc, LANE0_ACCESS, data16,mask16));

        xgxs_ctrl = 5;
        xgxs_ctrl <<= XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, LANE0_ACCESS,
                       xgxs_ctrl,
                       XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK |
                       XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

        /* Turn off 8B10B */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_XGXSBLK1_LANECTRL2r(unit, pc, LANE0_ACCESS, 0));

        /* Turn off 64B66B endec and turn off cl36 pcs */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_XGXSBLK1_LANECTRL0r(unit, pc, LANE0_ACCESS, 0));

        if (DEV_CFG_PTR(pc)->custom == PHYCTRL_LANE_MODE_CUSTOM_3p125MHZ) {
            /* Double wide data at 1/2 pll rate */ 
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_XGXSBLK1_LANECTRL1r(unit, pc, LANE0_ACCESS, 0xAAAA));
        } else {
            /* default 6.25G Double wide data at pll rate */
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_XGXSBLK1_LANECTRL1r(unit, pc, LANE0_ACCESS, 0xFFFF));
        }

        /* disable 10G parallel detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(unit, pc, LANE0_ACCESS,
                      0,
                      XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));

        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, LANE0_ACCESS,&pdetect));
    
        SOC_IF_ERROR_RETURN
            (READ_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, LANE0_ACCESS, &ctrl0));
        pdetect &= ~SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
        ctrl0 &= ~MII_CTRL_AE;

        /* XXX not to control the speeds thru pma/pmd register */
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_MISC2r(unit, pc, LANE0_ACCESS, &misc2));
        misc2 &= ~SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK;

        /* disable cl37,parallel detect 1G on all lanes*/
        /* broadcast to all lanes */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,pc,LANE0_ACCESS, WC_AER_BCST_OFS_STRAP));

        /* XXX not to control the speeds thru pma/pmd register */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_SERDESDIGITAL_MISC2r(unit, pc, LANE0_ACCESS, misc2));

        /* disable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
              (WRITE_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, LANE0_ACCESS, pdetect));

        /* disable CL37 */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, LANE0_ACCESS, ctrl0));

        /* Clear the CL37 FEC advertisement */
        mask16 = DIGITAL3_UP3_FEC_MASK;
        data16 = 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL3_UP3r(unit, pc, LANE0_ACCESS, data16, mask16));

        /* pll multiplier 40 with 156.25MHz refclkc */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, pc, LANE0_ACCESS, 0x721B));

        /* Reset the value for these registers to the recommended 
           values before f/w is started.
           These registers are managed by firmware in cl73 mode.
        */
        /* Set 0x8370[1] */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL73_USERB0_CL73_UCTRL1r(unit, pc, LANE_BCST, 
                CL73_USERB0_CL73_UCTRL1_CL73_USTAT1_MUXSEL_MASK,
                CL73_USERB0_CL73_UCTRL1_CL73_USTAT1_MUXSEL_MASK));

        /* Clear 0x8378[1], 0x8378[3] & 0x8378[12] */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL73_USERB0_CL73_UCTRL2r(unit, pc, LANE_BCST, 
                0x0000,
                CL73_USERB0_CL73_UCTRL2_CONFIG_MATCH_DIS_MASK 
                | CL73_USERB0_CL73_UCTRL2_SIGDET_TIMER_DIS_MASK
                | 0x1000));

        /* Clear 0x80a1[3:2] for lane 0, lane 1, lane 2, lane 3
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TXB_ANATXACONTROL0r(unit, pc, LANE0_ACCESS,
                0x00,
                TX0_ANATXACONTROL0_EDEN_FORCE_R_MASK 
                | TX0_ANATXACONTROL0_EDEN_R_MASK));

        /* Disable firmware war for OS8 CL73 tx fifo reset */            
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_MDIO_UC_MAILBOXr(unit, pc, LANE0_ACCESS,
                                                   0, 0xf));

        /* restore AER */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,pc,LANE0_ACCESS, 0));

        /* check if any tx/rx lanes are remapped */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_rxlane_map_set(unit,pc,
                 (uint16)(DEV_CFG_PTR(pc)->rxlane_map)));

        SOC_IF_ERROR_RETURN
            (_phy_wc40_txlane_map_set(unit,pc,
                 (uint16)(DEV_CFG_PTR(pc)->txlane_map)));

        /* Tx/Rx Polarity control */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_combo_polarity_set(unit,pc,TRUE));

        if (DEV_CFG_PTR(pc)->load_mthd) { 
            SOC_IF_ERROR_RETURN
                (_phy_wc40_ucode_get(unit,port,&pdata,&ucode_len,&alloc_flag));
            SOC_IF_ERROR_RETURN
                (phy_wc40_firmware_load(unit,port,0,pdata, ucode_len));
            if (alloc_flag) {
                sal_free(pdata);
            }
        } else {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "WC40 custom mode : uC RAM download skipped: u=%d p=%d\n"),
                      unit, port));
        }
        /* put inband mdio rx in reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK3_LOCALCONTROL0r(unit,pc,LANE0_ACCESS, 
                       (1 << 3), (1 << 3)));

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, LANE0_ACCESS,
                         XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                         XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

        /* PLL clock lock wait */
        (void) _phy_wc40_pll_lock_wait(unit, port);
        _phy_wc40_control_vco_disturbed_set(unit, port);
    }

    pc->flags |= PHYCTRL_INIT_DONE;

    /* XXX a register 0x81f2  value to indicate this mode to allow ucontroller to tune*/

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_interlaken_mode_init: u=%d p=%d\n"),
              unit, port));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_ind_init
 * Purpose:     
 *      Initialize hc phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
_phy_wc40_ind_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;               
    uint16             xgxs_ctrl;
    uint16             data16;
    uint16             mask16;
    uint16             txck_mode;
    int vco_freq;
    uint8 *pdata;
    int   ucode_len;
    int   alloc_flag;

    pc = INT_PHY_SW_STATE(unit, port);

    /* In this mode, the device's 4 lanes are usaually connected to more than
     * one switch port.
     * It is necessary to configure and initialize the shared resource
     * once before going to each lane. 
     */

    if (!_wc40_chip_init_done(unit,pc->chip_num,pc->phy_mode)) {
        /* configure and initialize the resource shared by all 4 lanes*/

        SOC_IF_ERROR_RETURN(_wc40_soft_reset(unit,pc)); /* reset once */

        /* select multi mmd and multi-port address mode */
        data16 = XGXSBLK0_MMDSELECT_DEVCL22_EN_MASK |
                 XGXSBLK0_MMDSELECT_DEVDEVAD_EN_MASK |
                 XGXSBLK0_MMDSELECT_DEVPMD_EN_MASK  |
                 XGXSBLK0_MMDSELECT_DEVAN_EN_MASK |
                 XGXSBLK0_MMDSELECT_MULTIMMDS_EN_MASK;
        mask16 = data16 |
                 XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK;
        if (!(pc->flags & PHYCTRL_MDIO_ADDR_SHARE)) {
            data16 |= XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_MMDSELECTr(unit, pc, LANE0_ACCESS, data16,mask16));

        /* set mdio override control to send out 312.5MHz clock on txck_out[0].
         * this allow lane1-lane3 to support 10G speed while lane 0 runs at 1G.
         * Apply to a1/b0. No workaround for a0
         */
        if ((!IS_DUAL_LANE_PORT(pc)) && (WC40_REVID_A1(pc) || WC40_REVID_B(pc))) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, pc, LANE0_ACCESS,
                       (1 << 13), (1 << 13)));
        }

        /* warpcore does not support os4(xgxs_ctrl=6). 
         * Should use os8 for independent channel mode
         */
        xgxs_ctrl = DEV_CFG_PTR(pc)->lane_mode;

        xgxs_ctrl <<= XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT;

        /* set device's mode and disable PLL sequencer */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, LANE0_ACCESS, 
                       xgxs_ctrl,
                       XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK |
                       XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

        /* configure the txck/rxck for all lanes */
        if (IS_DUAL_LANE_PORT(pc)) {
            txck_mode = 0x9;
        } else {
            txck_mode = 0;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc,LANE0_ACCESS, 
                  (txck_mode << WC40_TXCK_SHIFTER) | WC40_RXCK_OVERIDE_MASK,
                      WC40_TXCK_MASK | WC40_RXCK_OVERIDE_MASK)); 

        /* broadcast to all lanes */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,pc,LANE0_ACCESS, WC_AER_BCST_OFS_STRAP));

        if (DEV_CFG_PTR(pc)->lane_mode == xgxs_operationModes_IndLane_OS5) {
            vco_freq = WC40_OS5_VCO_FREQ;
        } else  { /* OS8 */
            vco_freq = WC40_OS8_VCO_FERQ;
            if (DEV_CFG_PTR(pc)->refclk == 161) {
                vco_freq = 0xB900;
            }
        }

        /* configure VCO frequency */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_SERDESDIGITAL_MISC1r(unit,pc,LANE0_ACCESS,vco_freq));

        /* disable 10G parallel detect */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_XGXSBLK5_PARDET10GCONTROLr(unit, pc, LANE0_ACCESS, 0));

        /* Reset the value for these registers to the recommended 
           values before f/w is started.
           These registers are managed by firmware in cl73 mode.
        */
        /* Set 0x8370[1] */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL73_USERB0_CL73_UCTRL1r(unit, pc, LANE_BCST, 
                CL73_USERB0_CL73_UCTRL1_CL73_USTAT1_MUXSEL_MASK,
                CL73_USERB0_CL73_UCTRL1_CL73_USTAT1_MUXSEL_MASK));

        /* Clear 0x8378[1], 0x8378[3] & 0x8378[12] */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL73_USERB0_CL73_UCTRL2r(unit, pc, LANE_BCST, 
                0x0000,
                CL73_USERB0_CL73_UCTRL2_CONFIG_MATCH_DIS_MASK 
                | CL73_USERB0_CL73_UCTRL2_SIGDET_TIMER_DIS_MASK
                | 0x1000));

        /* Clear 0x80a1[3:2] for lane 0, lane 1, lane 2, lane 3
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TXB_ANATXACONTROL0r(unit, pc, LANE0_ACCESS,
                0x00,
                TX0_ANATXACONTROL0_EDEN_FORCE_R_MASK 
                | TX0_ANATXACONTROL0_EDEN_R_MASK));

        /* Disable firmware war for OS8 CL73 tx fifo reset */            
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_MDIO_UC_MAILBOXr(unit, pc, LANE0_ACCESS,
                                                   0, 0xf));

        /* reset AER */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,pc,LANE0_ACCESS, 0));

        /* check if any tx/rx lanes are remapped */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_rxlane_map_set(unit,pc,
                 (uint16)(DEV_CFG_PTR(pc)->rxlane_map)));
   
        SOC_IF_ERROR_RETURN
            (_phy_wc40_txlane_map_set(unit,pc,
                 (uint16)(DEV_CFG_PTR(pc)->txlane_map)));

        /* select div16 clock */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_rx_div16_clk_select(unit,pc));

        if (DEV_CFG_PTR(pc)->load_mthd) { 
            SOC_IF_ERROR_RETURN
                (_phy_wc40_ucode_get(unit,port,&pdata,&ucode_len,&alloc_flag));
            SOC_IF_ERROR_RETURN
                (phy_wc40_firmware_load(unit,port,0,pdata, ucode_len));
            if (alloc_flag) {
                sal_free(pdata);
            }
        } else {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "WC40 independent mode : uC RAM download skipped: u=%d p=%d\n"),
                      unit, port));
        }

        mask16 = XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
        data16 = mask16;
        /* Workaround: tx fifo recenter problem in aggregated mode */
        if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
            if (IS_DUAL_LANE_PORT(pc)) {
                mask16 |= XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK;
            }
        }

        /* enable PLL sequencer */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, LANE0_ACCESS, data16, mask16));

        /* PLL clock lock wait */
        (void) _phy_wc40_pll_lock_wait(unit, port);
    }

    SOC_IF_ERROR_RETURN
        (_phy_wc40_independent_lane_init(unit, port));

    pc->flags |= PHYCTRL_INIT_DONE;

    return SOC_E_NONE;
}
    
/*
 * Function:
 *      phy_wc40_init
 * Purpose:     
 *      Initialize hc phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wc40_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;               

    /* init the configuration value */
    _phy_wc40_config_init(unit,port);

    pc = INT_PHY_SW_STATE(unit, port);

    if (!PHY_EXTERNAL_MODE(unit, port)) {
         /* PHY reset callbacks: 
         * It calls the user registered reset handler
         * or the PHY driver's reset handler otherwise, 
         * if the user did not register any of it's own
         * reset routine 
         */
        SOC_IF_ERROR_RETURN
            (soc_phy_reset(unit, port));
    }

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        if (DEV_CFG_PTR(pc)->custom) {
            SOC_IF_ERROR_RETURN
                (_phy_wc40_interlaken_mode_init(unit, port));
        } else {
            SOC_IF_ERROR_RETURN
                (_phy_wc40_ind_init(unit,port));
        }
    } else {
        SOC_IF_ERROR_RETURN(_wc40_soft_reset(unit,pc)); /* soft reset */

        /* Force to comboCore mode.
         * Support SGMII 10/100/1000Mbps, 1000X, 2500X, 10G, and 12G.
         */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_combo_core_init(unit, port));
    }
	 
#ifdef BCM_PORT_DEFAULT_DISABLE
    if (!PHY_EXTERNAL_MODE(unit, port) && !SOC_WARM_BOOT(unit)) {
        phy_wc40_enable_set(unit, port, FALSE);
    }
#endif

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_wc40_init: u=%d p=%d\n"), unit, port));
    return SOC_E_NONE;
}

#define PCS_STATUS_LANE4_SYNC   0x80F
STATIC int
_phy_wc40_war_link_check(int unit, soc_port_t port)
{     
    uint16 sync_stat;
    uint16 sync_good;
    uint16 link;
    phy_ctrl_t         *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    /* need to read the current link state */
    SOC_IF_ERROR_RETURN
        (READ_WC40_XGXSBLK4_XGXSSTATUS1r(unit, pc, 0x00, &link));

    if (link & XGXSBLK4_XGXSSTATUS1_LINK10G_MASK) {
        link = TRUE;
    } else {
        link = FALSE;
    }

    /* if not a valid state, such as in autoneg mode or not BRCM 64B/66B speeds,
     * or if linkup, no need to go thru workaround check
     */
    if ((!WC40_SP_VALID(pc)) || (link==TRUE)) {
        WC40_SP_CNT_CLEAR(pc);
        return SOC_E_NONE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_war_link_check: u=%d p=%d: \n"), unit, port));

    sync_good = PCS_STATUS_LANE4_SYNC;

    /* now check the sync status on appropriate lanes. dxgxs0/dxgxs1 and combo
     * should have the same sync status, i.e. 0x80F. dxgxs1 is read from lane 2.
     */
    SOC_IF_ERROR_RETURN
        (READ_WC40_PCS_IEEE1BLK_PCS_LANESTATUSr(unit, pc, 0x00, &sync_stat));

    /* check if all lane sync'ed up */
    if (sync_stat == sync_good) {
        if (WC40_SP_CNT(pc)) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_wc40_war_link_check workaround applied: u=%d p=%d: \n"), unit, port));

            if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
                /* broadcast to all lanes */
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AERBLK_AERr(unit,pc,0x00, WC_AER_BCST_OFS_STRAP));

            } else { /* dual xgxs mode */
                DUAL_LANE_BCST_ENABLE(pc);
            }
               
            /* if it is synced and no link, do the rx reset */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, 0x00,
                                     DIGITAL5_MISC6_RESET_RX_ASIC_MASK,
                                     DIGITAL5_MISC6_RESET_RX_ASIC_MASK));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, 0x00, 0,
                                         DIGITAL5_MISC6_RESET_RX_ASIC_MASK));

            if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
                /* restore AER */
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AERBLK_AERr(unit,pc,0x00, 0));
            } else { /* dual xgxs mode */
                DUAL_LANE_BCST_DISABLE(pc);
            }
        } else {
            WC40_SP_CNT_INC(pc);
        }
    }

    return SOC_E_NONE;
}

/*
 *      _phy_wc40_cl73_auto_recover
 * Purpose: Recovers CL73 fsm from 
 *          lockup during some of 
 *          the problematic states
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
_phy_wc40_cl73_auto_recover(int unit, soc_port_t port, int *link)
{
   /* WC40_CL73_AUTO_RECOVER feature addresses a link issue 
      found with WarpCore connected to a certain Broadcom PHY, 
      primarily BCM8073. Following autoneg FSM states are 
      known to be symptoms of the issue:
       0x002: Transmit Disable
       0x004: Ability Detect
       0x008: Acknowledge
       0x010: Acknowledge Complete
       0x020: Next Page Wait
    */
    phy_ctrl_t *pc;
    uint16 fsm_state1, fsm_state2, data16, temp_data;
    WC40_CL73_AUTO_RECOVER_t *pdata; 

    pc = INT_PHY_SW_STATE(unit, port);
    
    pdata = &DEV_CFG_PTR(pc)->cl73_fsm_auto_recover;

    if (DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.enable
            && DEV_CFG_PTR(pc)->cl73an 
            && !(COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode))) {

            /* Read RX0_ANARXSTATUS register to check for sig detect first. */
            SOC_IF_ERROR_RETURN
                (WC40_REG_READ(unit, pc, LANE0_ACCESS,
                               0x80b1 + (0x10 * pc->lane_num), &temp_data));
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit, pc, LANE0_ACCESS,
                                 0x80b1 + (0x10 * pc->lane_num),
                                 RXB_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                 RXB_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (WC40_REG_READ(unit, pc, LANE0_ACCESS, 
                               0x80b0 + (0x10 * pc->lane_num), &data16));
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit, pc, LANE0_ACCESS,
                                 0x80b1 + (0x10 * pc->lane_num),
                                 temp_data,
                                 RXB_ANARXCONTROL_STATUS_SEL_MASK));

            if ((*link == 0) && (DEV_CFG_PTR(pc)->cl73an) && 
            (data16 & RX0_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK)) {

                /* Enable cl73 fsm test mux */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL73_USERB0_CL73_UCTRL1r(unit, pc, 0x00, 
                                             CL73_USERB0_CL73_UCTRL1_CL73_USTAT1_MUXSEL_MASK,
                                             CL73_USERB0_CL73_UCTRL1_CL73_USTAT1_MUXSEL_MASK));
                /* Read fsm status and ignore it first time */
                SOC_IF_ERROR_RETURN
                    (READ_WC40_CL73_USERB0_CL73_USTAT1r(unit, pc, 0x00, &fsm_state1));
                /* Read fsm status second time */
                SOC_IF_ERROR_RETURN
                    (READ_WC40_CL73_USERB0_CL73_USTAT1r(unit, pc, 0x00, &fsm_state1));
                fsm_state1 = fsm_state1 & 0x0fff;
                /* Read fsm status third time */
                SOC_IF_ERROR_RETURN
                    (READ_WC40_CL73_USERB0_CL73_USTAT1r(unit, pc, 0x00, &fsm_state2));
                fsm_state2 = fsm_state2 & 0x0fff;

                if (fsm_state1 == fsm_state2) {
                    int apply_recovery = 0;

                    if (pdata->fsm_state_prev == fsm_state1) {
                        /* The state is still the same as previous iteration.
                           Track how many times we have seen this state. */
                        pdata->fsm_state_count++;
                    } else {
                        /* New state seen compared to the previous iteration.
                           Just save the new state data & start over. */
                        pdata->fsm_state_prev  = fsm_state1;
                        pdata->fsm_state_count = 1;
                    }

                    /* FSM state 0x008 is handled differently. */
                    if (0x008 == fsm_state1) {

                        if (pdata->fsm_state_count > CL73_AUTO_RECOVER_STATE08_WAIT_COUNT) {
                            /* We've seen enough of state 0x008.  Clear
                               the counter & apply recovery action. */
                            pdata->fsm_state_prev  = 0;
                            pdata->fsm_state_count = 0;
                            apply_recovery = 1;

                            /* Update statistics for each recovery. */
                            pdata->action_count_state08++;
                        }

                    /* Not state 0x008. */
                    } else {
                        if (pdata->fsm_state_count > CL73_AUTO_RECOVER_LP_STATE_WAIT_COUNT) {
                            /* We've seen enough of this state.  Clear
                               the counter & apply recovery action if
                               it's a state we care about. */
                            pdata->fsm_state_prev  = 0;
                            pdata->fsm_state_count = 0;

                            /* Update statistics for each recovery. */
                            switch (fsm_state1) {
                            case 0x002:
                                    apply_recovery = 1;
                                    pdata->action_count_state02++;
                                    break;
                            case 0x004:
                                    apply_recovery = 1;
                                    pdata->action_count_state04++;
                                    break;
                            case 0x010:
                                    apply_recovery = 1;
                                    pdata->action_count_state10++;
                                    break;
                            case 0x020:
                                    apply_recovery = 1;
                                    pdata->action_count_state20++;
                                    break;
                            }
                        }
                    }

                    if (apply_recovery) {
                        LOG_INFO(BSL_LS_SOC_PHY,
                                 (BSL_META_U(unit,
                                             "WC: Applying recovery for u:0x%x p:0x%x "
                                             "fsm_state:0x%x\n"), unit, port, fsm_state1));

                        WC40_CL73_AN_TX_FIFO_RESET_STOP(pc);

                        /* Toggle RX/TX ASIC reset */
                        SOC_IF_ERROR_RETURN
                            (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, 0x00,
                                                  (DIGITAL5_MISC6_RESET_RX_ASIC_MASK
                                                    | DIGITAL5_MISC6_RESET_TX_ASIC_MASK),
                                                  (DIGITAL5_MISC6_RESET_RX_ASIC_MASK
                                                    | DIGITAL5_MISC6_RESET_TX_ASIC_MASK)));
                        SOC_IF_ERROR_RETURN
                            (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, 0x00,
                                                  0,
                                                  (DIGITAL5_MISC6_RESET_RX_ASIC_MASK
                                                    | DIGITAL5_MISC6_RESET_TX_ASIC_MASK)));

                        WC40_CL73_AN_TX_FIFO_RESET_START(pc);

                        LOG_INFO(BSL_LS_SOC_PHY,
                                 (BSL_META_U(unit,
                                             "\n WC: CL73 fsm recovery action counters u:0x%x p:0x%x \n"
                                             " CL73 FSM state 0x%x - %d times\n"
                                             " CL73 FSM state 0x%x - %d times\n"
                                             " CL73 FSM state 0x%x - %d times\n"
                                             " CL73 FSM state 0x%x - %d times\n" 
                                             " CL73 FSM state 0x%x - %d times\n"),
                                  unit, port, 0x2, pdata->action_count_state02,
                                  0x4, pdata->action_count_state04, 
                                  0x8, pdata->action_count_state08, 
                                  0x10, pdata->action_count_state10,
                                  0x20, pdata->action_count_state20));

                    }
                } else {
                    /* (fsm_state1 != fsm_state2). Reset FSM data. */
                    pdata->fsm_state_prev  = 0;
                    pdata->fsm_state_count = 0;
                }

                /* Disable cl73 fsm test mux */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL73_USERB0_CL73_UCTRL1r(unit, pc, 0x00, 
                                            0,
                                            CL73_USERB0_CL73_UCTRL1_CL73_USTAT1_MUXSEL_MASK));
            }
    }

    return SOC_E_NONE;
}

/*
 *      _phy_wc40_softare_rx_los
 * Purpose:
 *      Psuedo rx los state machine
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
_phy_wc40_softare_rx_los(int unit, soc_port_t port, int *link)
{
    uint16      data16 = 0, rx_seq_done = 0;
    phy_ctrl_t *pc;
    int pcs_link = 0;
    uint8 sys_link = 0;
    SOFTWARE_RX_LOS_STATES_t rx_los_state;
    int lane_start = 0, lane_end = 0, index = 0;
    int rv = SOC_E_NONE;

    pc = INT_PHY_SW_STATE(unit, port);

    if(DEV_CFG_PTR(pc)->sw_rx_los.enable) {
        pcs_link = *link; 
        sys_link = DEV_CFG_PTR(pc)->sw_rx_los.sys_link;
        rx_los_state = DEV_CFG_PTR(pc)->sw_rx_los.state;
        DEV_CFG_PTR(pc)->sw_rx_los.link_status = 0;
       
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            lane_start = 0;
            lane_end   = 3;
        } else if (IS_DUAL_LANE_PORT(pc)) {
            lane_start = pc->lane_num;
            lane_end = lane_start + 1;
        } else {  /* quad-port mode */
            lane_start = pc->lane_num;
            lane_end   = lane_start;
        }

        /* Read RX SEQUENCE DONE status */
        for (index = lane_start; index <= lane_end; index++) {
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit, pc, LANE0_ACCESS, 
                                 0x80b1 + (0x10 * index), 
                                 RXB_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                 RXB_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (WC40_REG_READ(unit, pc, LANE0_ACCESS, 
                                 0x80b0 + (0x10 * index), &data16));

            if(data16 & RX1_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
                rx_seq_done = 1;
            } else {
                rx_seq_done = 0;
                break;
            }
        }

        /* 
          Current State: Don't Care
          Next State: IDLE
          If link was previously TRUE and is still true, 
          set the state to IDLE
        */
        if((sys_link == TRUE) && (pcs_link == TRUE)) {
            rx_los_state = IDLE;
        }

        /* 
          Current State: IDLE
          Next State: RESET
          If the state was IDLE(link up) but now link is down, 
          enter the RESET state 
        */
        if((rx_los_state == IDLE) && (pcs_link == FALSE)) {
            rx_los_state = RESET;
            sys_link = FALSE;
        } 

        /* 
          Current State: RESET
          Next State: INITIAL_LINK or START_TIMER
          If state is RESET, it's next state could be INITIAL LINK
          or START_TIMER depending upon the pcs_link state
        */
        if(rx_los_state == RESET) {
            if ((pcs_link == TRUE) && (rx_seq_done)){
                rx_los_state = INITIAL_LINK;
            } else if (rx_seq_done){
            /*Do Nothing to allow rx_los_state to go into RESET atleast once */
            }
        }

        /* Take action depending upon the current state */
        switch (rx_los_state) {
            case IDLE:
                /* Do nothing */
                /* Link ON condition */
                break;

            case RESET:
#ifdef SW_RX_LOS_LINK_FLAP_WAR
                if(DEV_CFG_PTR(pc)->sw_rx_los.fault_report_dis) {
                    SOC_IF_ERROR_RETURN
                        (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                         SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
                    DEV_CFG_PTR(pc)->sw_rx_los.fault_report_dis = 0;
                }
                /* Reset CL82 machine (Intermittent FSM errors) 0x8420.0=1->0 */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL82_USERB0_TX_CONTROL_1r(unit,pc, 0x00, 
                     CL82_USERB0_TX_CONTROL_1_CL82_EN_OVERRIDE_MASK,
                     CL82_USERB0_TX_CONTROL_1_CL82_EN_OVERRIDE_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL82_USERB0_TX_CONTROL_1r(unit,pc, 0x00, 
                     0, CL82_USERB0_TX_CONTROL_1_CL82_EN_OVERRIDE_MASK));  
#endif
                sys_link = FALSE;
                if(rx_seq_done){
                  rx_los_state = START_TIMER;
                }  
                break;
	
            case INITIAL_LINK:
#ifdef SW_RX_LOS_LINK_FLAP_WAR
                /* Disable local fault reporting to avoid link flaps untill
                  link stablizes
                 */
                SOC_IF_ERROR_RETURN
                    (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                     SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, FALSE));
                DEV_CFG_PTR(pc)->sw_rx_los.fault_report_dis = 1;
#endif

                /* rxSeqStart */
                for (index = lane_start; index <= lane_end; index++) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, pc, 
                            ln_access[index], 
                            DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK, 
                            DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
                }
#ifdef SW_RX_LOS_LINK_FLAP_WAR
                
                /* Reset CL82 machine (Intermittent FSM errors) 0x8420.0=1->0 */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL82_USERB0_TX_CONTROL_1r(unit,pc, 0x00, 
                     CL82_USERB0_TX_CONTROL_1_CL82_EN_OVERRIDE_MASK,
                     CL82_USERB0_TX_CONTROL_1_CL82_EN_OVERRIDE_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL82_USERB0_TX_CONTROL_1r(unit,pc, 0x00, 
                     0, CL82_USERB0_TX_CONTROL_1_CL82_EN_OVERRIDE_MASK)); 
#endif
                /* Skip the link transition for this rx reset */
                DEV_CFG_PTR(pc)->sw_rx_los.link_status = 1;
                rx_los_state = LINK;
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                             unit, port, rx_los_state));
                break;

            case LINK:
                if( pcs_link == TRUE) {
                    sys_link = TRUE;
                    rx_los_state = IDLE;
                } else {
                    rx_los_state = RESET;
                }
#ifdef SW_RX_LOS_LINK_FLAP_WAR
                /* Enable local fault reporting disabled earlier.
                 */
                SOC_IF_ERROR_RETURN
                    (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                     SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, TRUE));
                DEV_CFG_PTR(pc)->sw_rx_los.fault_report_dis = 0;
#endif
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                             unit, port, rx_los_state));
                break;

            case START_TIMER:
                rx_los_state = RX_RESTART;  
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                             unit, port, rx_los_state));
                break;

            case RX_RESTART:
#ifdef SW_RX_LOS_LINK_FLAP_WAR
                /* Disable local fault reporting to avoid link flaps untill
                  link stablizes
                 */
                SOC_IF_ERROR_RETURN
                    (MAC_CONTROL_SET(DEV_CFG_PTR(pc)->macd, unit, port, 
                                     SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, FALSE));
                DEV_CFG_PTR(pc)->sw_rx_los.fault_report_dis = 1;
#endif
                /* rxSeqStart */ 
                for (index = lane_start; index <= lane_end; index++) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, pc, 
                            ln_access[index], 
                            DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK, 
                            DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
                }
                if( pcs_link == FALSE) {
                    rx_los_state = RESET;
                } else {
                    rx_los_state = LINK;
                }
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "SOFTWARE RX LOS : u=%d p=%d state:0x%x \n"), 
                             unit, port, rx_los_state));
                /* Skip the link transition for this rx reset */
                DEV_CFG_PTR(pc)->sw_rx_los.link_status = 1;
                break;

            default:
                break;
        }
        DEV_CFG_PTR(pc)->sw_rx_los.sys_link = sys_link;;
        DEV_CFG_PTR(pc)->sw_rx_los.state = rx_los_state;
        *link = sys_link;

#ifdef BCM_WARM_BOOT_SUPPORT
        rv |= wc40_wb_sc_handler(unit, port, WC40_WB_SC_UPDATE, WC40_WB_RXLOS) ;
#endif    
    }

    return rv;

}




/*
 *      phy_wc40_link_get
 * Purpose:
 *      Get layer2 connection status.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wc40_link_get(int unit, soc_port_t port, int *link)
{
    uint16      mask16;
    uint16      mii_stat, data16 = 0;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    if (CUSTOM_MODE(pc)) {
        /* interlaken mode */
        /* configure the status register for sigdet */
        SOC_IF_ERROR_RETURN
            (WC40_REG_MODIFY(unit,pc,0x0,0x80b1 + (0x10 * pc->lane_num),
                   RX0_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                   RX0_ANARXCONTROL_STATUS_SEL_MASK));

        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, 0x00, 0x80b0 + (0x10 * pc->lane_num),
                           &data16));
        mask16 = RX0_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK |
                 RX0_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_MASK;
        if ((data16 & mask16) == mask16) { 
            *link = TRUE;
        } else {
            *link = FALSE;
        }
        return SOC_E_NONE;
    }

    if (PHY_DISABLED_MODE(unit, port)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    *link = FALSE;

    /* IF SW RX LOS is enabled and it's link_status is set, it means the 
       lane is reset during the previous link get - read the link status 
       again to skip the OFF link transition that was forced during the
       SW RX LOS. */
    if((DEV_CFG_PTR(pc)->sw_rx_los.enable) 
         && (DEV_CFG_PTR(pc)->sw_rx_los.link_status)) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_PCS_IEEE0BLK_PCS_IEEESTATUS1r(unit, pc, 0x00, &mii_stat));
    }

    /* Check XAUI link first if XAUI mode */
    SOC_IF_ERROR_RETURN
        (READ_WC40_PCS_IEEE0BLK_PCS_IEEESTATUS1r(unit, pc, 0x00, &mii_stat));
    if (mii_stat & MII_STAT_LA) {
        *link = TRUE;
    }

    if (FORCE_CL72_IS_ENABLED(pc)) {
        (void)_phy_wc40_force_cl72_state_machine(unit,port,*link);
    }

    /* Finally Check combo link */
    if (*link == FALSE) {
        /* IF SW RX LOS is enabled and it's link_status is set, it means 
           the lane is reset during the previous link get - read the link 
           status again to skip the OFF link transition that was forced 
           during the SW RX LOS. */
        if((DEV_CFG_PTR(pc)->sw_rx_los.enable) 
             && (DEV_CFG_PTR(pc)->sw_rx_los.link_status)) {
            SOC_IF_ERROR_RETURN
                (READ_WC40_COMBO_IEEE0_MIISTATr(unit, pc, 0x00, &mii_stat));
        }
        SOC_IF_ERROR_RETURN
            (READ_WC40_COMBO_IEEE0_MIISTATr(unit, pc, 0x00, &mii_stat));

        if (mii_stat & MII_STAT_LA) {
            *link = TRUE;
        }
    }

    /*To address the Synchronization issue related to a registered clock gate module
      responsible for providing the 312.5MHz write clock to the TX over-sample FIFO.
      Resetting the FIFO to fix the issue.
     */ 
    if (DEV_CFG_PTR(pc)->cl73an 
            && !(COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode))
            && !DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, 0x00,
                 (*link == TRUE)? 0: 
                 DIGITAL5_MISC6_TX_OS8_AFRST_EN_MASK,
                 DIGITAL5_MISC6_TX_OS8_AFRST_EN_MASK));
    }

    if (WC40_LINK_WAR_REVS(pc)) {
        (void)_phy_wc40_war_link_check(unit,port);
    }

    /* restore mld local fault configuration one second after autoneg starts
     * It needs to be disabled during autoneg
     */
    if (!WC40_CL73_SOFT_KR2(pc)) {
        if (WC40_AN_VALID(pc)) {
            if (WC40_AN_CHECK_TIMEOUT(pc)) {
                SOC_IF_ERROR_RETURN
                    (WC40_REG_MODIFY(unit, pc, 0x00, 0x842A,0,(1 << 5)));
                WC40_AN_VALID_RESET(pc);
            }
        }
    }

    if ((DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2)
           || (DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_WAR)) {
        SOC_IF_ERROR_RETURN
            (_wc40_soft_an_cl73kr2_check(unit,port));
    } else if ((DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_CUSTOM)
             || (DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_CUSTOM_WAR)) {
        SOC_IF_ERROR_RETURN
            (_wc40_soft_an_cl73kr2_custom_check(unit,port));
    }

    /* Monitor the CL73 fsm for particular FSM states
       and take action if it sees the same state for a 
       a fixed number of times (implying stuck fsm) to 
       recover fsm.
       TO enable/disable this feature use PHY ctrl:
       PORT_PHY_CONTROL_CL73_FSM_AUTO_RECOVER
     */
    SOC_IF_ERROR_RETURN
        (_phy_wc40_cl73_auto_recover(unit, port, link));


    /* The SOFTWARE RX LOS attaempts to handle the situation 
       through s/w where no reliable LOS detect is available 
       to WC. This feature can be enabled using phy control.
     */
    SOC_IF_ERROR_RETURN
        (_phy_wc40_softare_rx_los(unit, port, link));


    /* XXX probably need to qualify  with speed status */
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_wc40_ind_enable_set
 * Purpose:
 *      Enable/Disable phy
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - on/off state to set
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
_phy_wc40_ind_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    uint16 mask16;
    uint16 data16;
    uint16 ln_ctrl;
    pc = INT_PHY_SW_STATE(unit, port);
    /* Firmware manages the CL73 timer only if CL73 is enabled 
       and mode10g = 0x4. Otherwise, s/w manages it */ 
    if(!(IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode) 
          && DEV_CFG_PTR(pc)->cl73an 
          && DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status)) {
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_UCTRL2r(unit,pc,0x00, 
             enable? 0 : CL73_USERB0_CL73_UCTRL2_SIGDET_TIMER_DIS_MASK, 
             CL73_USERB0_CL73_UCTRL2_SIGDET_TIMER_DIS_MASK));
    }

    /* disable the TX */
    mask16 = 1 << (XGXSBLK0_MISCCONTROL1_PMD_LANE0_TX_DISABLE_SHIFT + pc->lane_num); 
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK0_MISCCONTROL1r(unit,pc,LANE0_ACCESS,
                enable? 0: mask16, mask16 ));

    /* now power down the lane if not used */
    SOC_IF_ERROR_RETURN
        (READ_WC40_XGXSBLK1_LANECTRL3r(unit,pc,0x00,&ln_ctrl));

    mask16 = (1 << pc->lane_num);    /* rx lane */
    if (IS_DUAL_LANE_PORT(pc)) {
        mask16 |= (mask16 << 1);
    }

    mask16 |= (mask16 << 4); /* both tx and rx lane */

    if (!enable) {
        mask16 |= 0x800;
        /* MAC uses rx0_ck/tx0_ck clock from lane0 for all lanes.
         * Only powerdown lane0 when all other three lane's are powered down
         */ 
        if ((pc->speed_max >= 10000) && (pc->lane_num == 0)) {
            if (((ln_ctrl | mask16) & XGXSBLK1_LANECTRL3_PWRDN_RX_MASK) != 
                XGXSBLK1_LANECTRL3_PWRDN_RX_MASK) {
                mask16 &= ~0x11;  /* remove lane0 RX/TX control */
            }
        }
        data16 = mask16;
    } else {
        data16 = 0;
        mask16 |= 0x11;  /* always add lane0 RX when any lane is enabled */
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK1_LANECTRL3r(unit,pc,0x00,data16,mask16));

    if (pc->lane_num == 0) {
        /* Digital Rx/Tx reset */
        SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00,
                enable? 0 : DIGITAL5_MISC6_RESET_RX_ASIC_MASK |
                DIGITAL5_MISC6_RESET_TX_ASIC_MASK,
                DIGITAL5_MISC6_RESET_RX_ASIC_MASK |
                DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
        /* Override analog Rx signal detect */
        SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX0_ANARXCONTROLr(unit,pc,0x00,
                 enable? 0 : RX0_ANARXCONTROL_OVERRIDE_SIGDET_EN_MASK,
                 RX0_ANARXCONTROL_OVERRIDE_SIGDET_EN_MASK | 
                 RX0_ANARXCONTROL_OVERRIDE_SIGDET_VAL_MASK));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_wc40_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    uint16 mask16;
    uint16 data16;

    pc = INT_PHY_SW_STATE(unit, port);
    
    /* Firmware manages the CL73 timer only if CL73 is enabled 
       and mode10g = 0x4. Otherwise, s/w manages it */ 
    if(!(IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode) 
          && DEV_CFG_PTR(pc)->cl73an 
          && DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status)) {
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_UCTRL2r(unit,pc,0x00, 
             enable? 0 : CL73_USERB0_CL73_UCTRL2_SIGDET_TIMER_DIS_MASK, 
             CL73_USERB0_CL73_UCTRL2_SIGDET_TIMER_DIS_MASK));
    }

    /* For WC-A0/A1, when Rx power down is asserted, cl82 pcs is not reset. 
     * Clock switching to refclock from recovered clock while PCS is still
     * running might have steered PCS into unrecoverable state. Disable/enable
     * cl82 PCS accordingly when powering down the device.
     */

    mask16 = XGXSBLK1_LANECTRL3_PWRDN_RX_MASK | XGXSBLK1_LANECTRL3_PWRDN_TX_MASK |
             XGXSBLK1_LANECTRL3_PWRDWN_FORCE_MASK;
    if (enable) {
        data16 = 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK1_LANECTRL3r(unit,pc,0x00,data16,mask16));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_MISCCONTROL1r(unit, pc, 0x00, 0,
                         XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK));
        if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL82_USERB0_TX_CONTROL_1r(unit, pc, 0x00,
                         0,
                         CL82_USERB0_TX_CONTROL_1_CL82_EN_VAL_MASK |
                         CL82_USERB0_TX_CONTROL_1_CL82_EN_OVERRIDE_MASK));
        } else {
            int i = 0;
            /* restart the uc */
            data16 = 3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
            data16 |= 1;    
            mask16 = DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK |
                     DSC1B0_UC_CTRL_GP_UC_REQ_MASK;
            for (i= 0; i < 4; i++) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, 
                                                 ln_access[i], 
                                                 data16,mask16));
            }
        }
    } else {
        data16 = mask16;
        if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL82_USERB0_TX_CONTROL_1r(unit, pc, 0x00,
                         CL82_USERB0_TX_CONTROL_1_CL82_EN_OVERRIDE_MASK,
                         CL82_USERB0_TX_CONTROL_1_CL82_EN_VAL_MASK |
                         CL82_USERB0_TX_CONTROL_1_CL82_EN_OVERRIDE_MASK));
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK1_LANECTRL3r(unit,pc,0x00,data16,mask16));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_MISCCONTROL1r(unit, pc, 0x00,
                         XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK,
                         XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_enable_set
 * Purpose:
 *      Enable/Disable phy 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wc40_enable_set(int unit, soc_port_t port, int enable)
{
    int rv;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }
    if (CUSTOM_MODE(pc)) {
        if (enable) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TXB_ANATXACONTROL0r(unit,pc,0x00,
                    TXB_ANATXACONTROL0_TX1G_FIFO_RST_MASK, 
                    TXB_ANATXACONTROL0_TX1G_FIFO_RST_MASK));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TXB_ANATXACONTROL0r(unit,pc,0x00,
                    0, 
                    TXB_ANATXACONTROL0_TX1G_FIFO_RST_MASK));
        }
        return SOC_E_NONE;
    }

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        rv = _phy_wc40_enable_set(unit,port,enable);
    } else {
        rv = _phy_wc40_ind_enable_set(unit, port, enable);
    }
    return rv;
}

/* XXX VCO 10.3125: 10G,20G_dxgxs,1G,2.5,10M,100M, div66  autoneg
 *          6.25:  10M,100M, 1G, 2.5G, 10G_dxgxs   div40  autoneg
 *          6.5625: 10.5G_dxgxs and 12G_dxgxs      div42  force mode only in ind.
 *          8.125:  15.77G_dxgxs                   div52  force mode only in ind.
 * VCO is only controlled by lane0. All VCO settings, i.e. reg. 0x8308.[8:15]
 * in all lanes must have the same value as for lane0. Switching VCO will bring down
 * the link on all four lanes. 
 * Generally the port configuration thru local_ability_get function should ensure the 
 * supported speeds in a given port should not require switching the VCO in the independent
 * channel mode. However the driver speed/autoneg function will not enforce this 
 * restriction.  
 */
STATIC int
_phy_wc40_vco_set(int unit, soc_port_t port, int speed,int speed_val)
{
    phy_ctrl_t  *pc;
    uint16 data16;
    uint16 mask16;
    uint16 vco_freq;
    uint16 misc1_ctrl;
    uint16 misc3_ctrl;
    int vco_625 = 0;

    pc = INT_PHY_SW_STATE(unit, port);

    /* only need to check in dxgxs mode */

    if ((SOC_IS_HELIX4(unit) || SOC_IS_TD_TT(unit) || SOC_IS_KATANA2(unit)) && 
        (!IS_DUAL_LANE_PORT(pc))) {
        if (DEV_CFG_PTR(pc)->hg_mode) {
            if (speed == 11000) {
                vco_freq = SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << 
                           SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT;
                vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_BITS <<
                            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_SHIFT;
                vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div80 <<
                            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
            } else {
                vco_freq = SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << 
                           SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT;
            }
        } else {
            vco_freq = SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << 
                       SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT;
        }
    } else {
        if (!IS_DUAL_LANE_PORT(pc)) {
            return SOC_E_NONE;
        }
        vco_freq = SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << 
                   SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT;
        vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_BITS <<
                    SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_SHIFT;

        if (DEV_CFG_PTR(pc)->custom1) {
            vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 <<
                        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
            vco_625 = 1;

        /* use 10G dxgxs for ethernet mode, 10.5G in HG mode to make it compatiable
         * with HC.
         */
        } else if (speed == 10000) {
            if (DEV_CFG_PTR(pc)->hg_mode) { 
                vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div42 <<
                        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
            } else {
                vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 <<
                        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                vco_625 = 1;
            }
        } else if (speed == 12000) {
            vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div42 <<
                        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
        } else if (speed == 15000) {
            vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div52 <<
                        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
        } else if (speed == 20000) {
            vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 <<
                        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
        } else if (speed == 21000) {
            vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div80 <<
                        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
        } else if (speed == 0) {
          /* autoneg, vco needs to be div66 */
          vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 <<
                      SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
        } else { /* default */
          vco_freq |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 <<
                      SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
        }
    }

    /* configure recovery clock */
    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) {
        /* SINGLE_LANE_IN_DUAL_PORT_MODE indicates 1/2.5G @6.25G VCO in Katana2 */
        if ((SOC_IS_HELIX4(unit) && (speed == 2500) && soc_property_port_get(unit, port, spn_SERDES_1000X_AT_6250_VCO, 0)) ||
            (SOC_IS_KATANA2(unit) && IS_SINGLE_LANE_IN_DUAL_PORT_MODE(pc))) {
            vco_625 = 1;
        }

        /* Set rx_wclk20_sel(0x8141.12) and txckout33 (0x8141.14)
         * core mode  serdes VCO  0x8141.12  0x8141.14  recovery clock frequency
         *   OS5       6.25G       1'b1       1'b0          156.25Mhz
         *   OS8       10.3125     1'b0       1'b0          156.25Mhz
         */
        if (vco_625 == 1 ) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, pc, LANE0_ACCESS,
                                                     (1 << 12), (1 << 12)));
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, pc, LANE0_ACCESS,
                                                    0, (1 << 12)));
        }

    }

    /* read current vco freq */
    if (SOC_IS_HELIX4(unit) || SOC_IS_TD_TT(unit) || SOC_IS_KATANA2(unit)) {
        /* restore AER */ 
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,pc, LANE0_ACCESS, 0));
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_MISC1r(unit, pc, LANE0_ACCESS, &data16));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, &data16));
    }

    mask16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK;

    /* return if speed is in same vco frequency */
    if ((data16  & mask16) == vco_freq) {
        return SOC_E_NONE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_vco_set: u=%d p=%d: vco freq switched: 0x%x\n"), unit, port,
              vco_freq));

    /* switch to desired VCO freq */

    /* read current speed value */
    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, &misc1_ctrl));
    misc1_ctrl &= ~(SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
                    SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                    SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
                    SERDESDIGITAL_MISC1_FORCE_SPEED_MASK);                   
    misc1_ctrl |= vco_freq;
    misc1_ctrl |= (speed_val & SERDESDIGITAL_MISC1_FORCE_SPEED_MASK);

    SOC_IF_ERROR_RETURN
        (READ_WC40_DIGITAL4_MISC3r(unit, pc, 0x00, &misc3_ctrl));
    if (speed_val & 0x20) {
        misc3_ctrl |= DIGITAL4_MISC3_FORCE_SPEED_B5_MASK;
    } else {
        misc3_ctrl &= ~DIGITAL4_MISC3_FORCE_SPEED_B5_MASK;
    }

    /* stop PLL sequencer. Bring down link on all ports */
    /* restore AER */
    if (SOC_IS_HELIX4(unit) || SOC_IS_TD_TT(unit) || SOC_IS_KATANA2(unit)) { 
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_AERBLK_AERr(unit,pc, LANE0_ACCESS, 0));
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, LANE0_ACCESS,
                                       0,
                                       XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

    /* broadcast to all lanes */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,pc, LANE0_ACCESS, WC_AER_BCST_OFS_STRAP));

    SOC_IF_ERROR_RETURN
        (WRITE_WC40_SERDESDIGITAL_MISC1r(unit, pc, LANE0_ACCESS, misc1_ctrl));
   
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_DIGITAL4_MISC3r(unit, pc, LANE0_ACCESS, misc3_ctrl));
   
    /* restore AER */ 
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(unit,pc, LANE0_ACCESS, 0));

    /* start PLL sequencer */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, LANE0_ACCESS,
                                       XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                                       XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    (void) _phy_wc40_pll_lock_wait(unit, port);
    _phy_wc40_control_vco_disturbed_set(unit, port);

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_enable_get
 * Purpose:
 *      Get Enable/Disable status
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of where to store on/off state
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wc40_enable_get(int unit, soc_port_t port, int *enable)
{
    *enable = !PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE);

    return SOC_E_NONE;
}

STATIC int
_phy_wc40_ind_speed_ctrl_get(int unit, soc_port_t port, int speed,
                             uint16 *speed_val,int *tx_inx)
{
    phy_ctrl_t  *pc;
    int          hg10g_port = FALSE;

    pc = INT_PHY_SW_STATE(unit, port);

    if (DEV_CFG_PTR(pc)->hg_mode) { 
        hg10g_port = TRUE;
    }
   
    *speed_val = 0;
    switch (speed) {
    case 3000:
    case 2500:
        *speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_2500BRCM_X1;
        *tx_inx = TXDRV_6GOS2_INX; 
        break;

    /* need to revisit here */
    /* XXX VCO 10.3125: 10G,20G_dxgxs,1G,2.5,10M,100M, div66  autoneg
     *          6.25:  10M,100M, 1G, 2.5G, 10G_dxgxs   div40  autoneg
     *          6.5625: 10.5G_dxgxs and 12G_dxgxs      div42  force mode only in ind. 
     *          8.125:  15.77G_dxgxs                   div52  force mode only in ind.
     */
    case 10000:  /* 10G_XFI, 10G_SFI, 10G dxgxs, 10G dxgxs hig */
        /* speed_val = 0x25;  10G XFI */
        /* speed_val = 0x29;  10G SFI */

        if (IS_DUAL_LANE_PORT(pc)) {
            if (DEV_CFG_PTR(pc)->custom1) {
               *speed_val = 0x30;  /* speed_x2_10000 */
            } else if (hg10g_port == TRUE) {
                /* speed_val = 0x21;   10.5HiG dual-XGXS SCR only */
                /* speed_val = 0x2D 10GHig DXGXS SCR */
                *speed_val = 0x21;
            } else {
                /* speed_val = 0x20;  10G ethernet dual-XGXS */
                /* speed_val = 0x2E 10G ethernet DXGXS SCR */
                *speed_val = DEV_CFG_PTR(pc)->scrambler_en? 0x2E: 0x20;
                *tx_inx = TXDRV_6GOS1_INX; 
            }
        } else {
           if ((DEV_CFG_PTR(pc)->line_intf & WC40_IF_SFI) ||
               (DEV_CFG_PTR(pc)->line_intf & WC40_IF_SR) ||
               (DEV_CFG_PTR(pc)->line_intf & WC40_IF_CR)) {
               *speed_val = SERDESDIGITAL_FORCE_SPEED_dr_10G_SFI;
           } else if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_XFI) {
               *speed_val = SERDESDIGITAL_FORCE_SPEED_dr_10G_XFI;
               *tx_inx = TXDRV_XFI_INX; 
           } else {
               /* XXX default to XFI */
               *speed_val = SERDESDIGITAL_FORCE_SPEED_dr_10G_XFI;
               *tx_inx = TXDRV_XFI_INX; 
           }
        }
        break;
    case 11000:  /* 11G higig solo  */
        /* speed_val = 0x25;  10G XFI */
        /* speed_val = 0x29;  10G SFI */

       if ((DEV_CFG_PTR(pc)->line_intf & WC40_IF_SFI) ||
           (DEV_CFG_PTR(pc)->line_intf & WC40_IF_SR) ||
           (DEV_CFG_PTR(pc)->line_intf & WC40_IF_CR)) {
           *speed_val = SERDESDIGITAL_FORCE_SPEED_dr_10G_SFI;
       } else if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_XFI) {
           *speed_val = SERDESDIGITAL_FORCE_SPEED_dr_10G_XFI;
           *tx_inx = TXDRV_XFI_INX; 
       } else {
           /* XXX default to XFI */
           *speed_val = SERDESDIGITAL_FORCE_SPEED_dr_10G_XFI;
           *tx_inx = TXDRV_XFI_INX; 
       }
        break;
    case 12000:  /* dxgxs */
        /* 12.773G       0x23
         * 12.773G CX4   0x24
         * 12G X2        0x2F
         */
        if (DEV_CFG_PTR(pc)->custom1) {
            *speed_val =  0x2F; /* custom1 12.7HiG dual-XGXS*/
        } else if (hg10g_port) {
            *speed_val = 0x23;
        } else {
            *speed_val = 0x24;
        }
        break;
    /* XXX PLL mode sets to SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div52*/ 
    case 15000: /* speed_15750_hi_dxgxs */
        *speed_val = 0x36; 
        break;
    case 20000: /* 20G dxgxs, 20G dxgxs hig */
        *speed_val = (hg10g_port == TRUE) ?
                     0x27:  /* 20GHiG dual-XGXS */
                     0x28; /* 20G ethernet dual-XGXS */
        if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_KR) {
            *speed_val = 0x3F;  /* 20G KR2 mld2 */

            /* part of workaround for KR2 speed status,
             * disable PCS type auto selection
             */
            if (WC40_REVID_B(pc)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL4_MISC4r(unit, pc, 0x00,
                         0,
                         DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK));
            }
        }
        break;
    case 21000:  /* only for HG, 20G speed mode + incresed VCO freq */
        if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_KR) {
            *speed_val = 0x3F;  /* 20G KR2 mld */

            /* part of workaround for KR2 speed status, 
             * disable PCS type auto selection 
             */
            if (WC40_REVID_B(pc)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL4_MISC4r(unit, pc, 0x00, 
                         0,  
                         DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK));
            }
        } else {
            *speed_val = 0x27;
        }
        break;            
    default:
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_ind_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    uint16       speed_val, mask16, data16;
    uint16       speed_mii;
    uint16       sgmii_status = 0;
#ifdef BCM_HELIX4_SUPPORT
    int config_speed = 0;
#endif
    int tx_inx;
    WC40_TX_DRIVE_t tx_drv[NUM_LANES];
    int force_cl72;
    int cur_mode = 0, new_mode = 0;

    pc = INT_PHY_SW_STATE(unit, port);

    /* set default entry first */
    tx_inx = TXDRV_DFT_INX;

    /* part of workaround for KR2 speed status, 
     * enable PCS type auto selection by default for all speeds
     */
    if (IS_DUAL_LANE_PORT(pc)) {
        if (WC40_REVID_B(pc)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL4_MISC4r(unit, pc, 0x00, 
                         DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK, 
                         DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK));
        }
    }
#ifdef BCM_HELIX4_SUPPORT
    if (SOC_IS_HELIX4(unit) &&
        soc_property_port_get(unit, port, spn_PORT_MAX_SPEED, 0)) {
        config_speed =  soc_property_port_get(unit, port, 
                                              spn_PORT_MAX_SPEED, speed);
        if (speed > config_speed) {
            speed = config_speed;
        }
    }
#endif /* BCM_HELIX4_SUPPORT */
    speed_val = 0;
    speed_mii = 0;
    switch (speed) {
    case 0:
        /* Do not change speed */
        return SOC_E_NONE;
    case 10:
        tx_inx = TXDRV_6GOS2_INX;
        speed_mii = MII_CTRL_SS_10;
        break;
    case 100:
        tx_inx = TXDRV_6GOS2_INX;
        speed_mii = MII_CTRL_SS_100;
        break;
    case 1000:
        tx_inx = TXDRV_6GOS2_INX;
        speed_mii = MII_CTRL_SS_1000;
        break;
    default:   /* check brcm speeds */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_ind_speed_ctrl_get(unit,port,speed,&speed_val,&tx_inx));
        break;
    }

    /* configure uC for copper medium in SFI mode */
    mask16 = WC40_UC_CTRL_SFP_DAC << (pc->lane_num * 4);
    data16 = 0;
    if (speed_val == SERDESDIGITAL_FORCE_SPEED_dr_10G_SFI) {
        if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_SR) {
            data16 = 0;
            tx_inx = TXDRV_SFI_INX;
        } else if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_CR) {
            data16 = mask16;
            tx_inx = TXDRV_SFIDAC_INX; 
        } else if (DEV_CFG_PTR(pc)->medium == SOC_PORT_MEDIUM_COPPER) {
            data16 =mask16;
            tx_inx = TXDRV_SFIDAC_INX; 
        }
    }
    
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit,pc,LANE0_ACCESS,data16,mask16));

    /* configure the TX driver parameters per speed mode */        
    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_control_get(unit, port,&tx_drv[0],tx_inx));
    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_control_set(unit, port,&tx_drv[0]));

    /* check if need to switch to a different VCO */
    if (speed) {
        SOC_IF_ERROR_RETURN
            (_phy_wc40_vco_set(unit,port,speed,speed_val));
    }

    DUAL_LANE_BCST_ENABLE(pc);

    WC40_CL73_AN_TX_FIFO_RESET_STOP(pc);

    /* hold tx/rx in reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK, 
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
    /* When the VCO is operating at 6.25G, XMAC is receiving a clock from the WarpCore that is a divided version of the 6.25G clock.
       This divided version is not of a high enough frequency in order for the XMAC to process the data at 2.5G on the GMII interface.
       So 4x2.5G on Katana2 and Helix4 using a 6.25G VCO are not supported, only 2x2.5G using a 6.25G VCO is supported.
       When works on 2x2.5G mode, property "serdes_1000x_at_6250_vco" should be configured to allow vco change.
    */
    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) {
        SOC_IF_ERROR_RETURN(READ_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, LANE0_ACCESS, &data16));
        cur_mode = (data16 & XGXSBLK0_XGXSCONTROL_MODE_10G_MASK) >> XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT;
        /* SINGLE_LANE_IN_DUAL_PORT_MODE indicates 1/2.5G @6.25G VCO in Katana2 */
        if ((SOC_IS_HELIX4(unit) && (speed == 2500) && soc_property_port_get(unit, port, spn_SERDES_1000X_AT_6250_VCO, 0)) ||
            (SOC_IS_KATANA2(unit) && IS_SINGLE_LANE_IN_DUAL_PORT_MODE(pc))) {
            /* OS5 mode to reduce transmit jitter */
            new_mode = XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS5;
        } else {
            new_mode = XGXSBLK0_XGXSCONTROL_MODE_10G_Indlane_OS8;
        }

        if (cur_mode != new_mode) {
            DUAL_LANE_BCST_DISABLE(pc);
            data16 = 0;
            SOC_IF_ERROR_RETURN(READ_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, LANE0_ACCESS, &data16));
            /* Switch to the new mode (os5 or os8) */
            data16 &= ~(XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK);
            data16 &= ~(XGXSBLK0_XGXSCONTROL_MODE_10G_MASK);
            data16 |= new_mode << XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT;
            SOC_IF_ERROR_RETURN(WRITE_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, data16));
            DUAL_LANE_BCST_ENABLE(pc);
        }
    }

    /* disable 100FX and 100FX auto-detect */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_FX100_CONTROL1r(unit,pc,0x00,
                              0,
                              FX100_CONTROL1_AUTODET_EN_MASK |
                              FX100_CONTROL1_ENABLE_MASK));

    /* disable 100FX idle detect */
    SOC_IF_ERROR_RETURN
            (MODIFY_WC40_FX100_CONTROL3r(unit,pc,0x00,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK));

    /* Workaround Jira# SDK-32387 */
    /* control the speeds thru pma/pmd register */
    if (WC40_SDK32387_REVS(pc) && (!IS_DUAL_LANE_PORT(pc))) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                           SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK,
                           SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
    }

    data16 = speed_val;
    mask16 = SERDESDIGITAL_MISC1_FORCE_SPEED_MASK;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, data16, mask16));

    data16 = (speed_val & 0x20)?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0;
    if (speed_val == SERDESDIGITAL_FORCE_SPEED_dr_10G_XFI ||
        speed_val == SERDESDIGITAL_FORCE_SPEED_dr_10G_SFI) {
        data16 |= DIGITAL4_MISC3_IND_40BITIF_MASK;
    } 
    mask16 = DIGITAL4_MISC3_FORCE_SPEED_B5_MASK | 
             DIGITAL4_MISC3_IND_40BITIF_MASK;   
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0x00, data16, mask16));

    force_cl72 = FALSE;
    if (speed == 11000) {
        force_cl72 = DEV_CFG_PTR(pc)->force_cl72 ? TRUE : FALSE;
    }

    if ((force_cl72 != FORCE_CL72_IS_ENABLED(pc)) || (force_cl72 == TRUE)) {
        SOC_IF_ERROR_RETURN
            (_phy_wc40_force_cl72_config(unit, port, force_cl72));
        FORCE_CL72_ENABLE(pc) = force_cl72;
    }

    /* Workaround Jira# SDK-32387 */
    /* control the speeds thru pma/pmd register */
    if (WC40_SDK32387_REVS(pc) && (!IS_DUAL_LANE_PORT(pc))) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                           0,
                           SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
    }

    if (speed <= 1000) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, pc, 0x00, &sgmii_status));

        sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;
        if (!sgmii_status && (speed == 100)) {

            /* fiber mode 100fx, enable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_FX100_CONTROL1r(unit,pc,0x00,
                                  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK,
                                  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK));

            /* enable 100fx extended packet size */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_FX100_CONTROL2r(unit,pc,0x00,
                                FX100_CONTROL2_EXTEND_PKT_SIZE_MASK,
                                FX100_CONTROL2_EXTEND_PKT_SIZE_MASK));
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, speed_mii,
                                              MII_CTRL_SS_MASK));
        }
    }

    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) {
        if (cur_mode != new_mode) {
            DUAL_LANE_BCST_DISABLE(pc);
            data16 = 0;
            SOC_IF_ERROR_RETURN(READ_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, LANE0_ACCESS, &data16));
            data16 |= XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
            SOC_IF_ERROR_RETURN(WRITE_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, data16));
            DUAL_LANE_BCST_ENABLE(pc);

            (void) _phy_wc40_pll_lock_wait(unit, port);
            _phy_wc40_control_vco_disturbed_set(unit, port);
        }
    }
    /* release the tx/tx reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00,
              0,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

    WC40_CL73_AN_TX_FIFO_RESET_START(pc);

    DUAL_LANE_BCST_DISABLE(pc);
    return SOC_E_NONE;
}


STATIC int
_phy_wc40_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    uint16       speed_val, mask;
    uint16       speed_mii;
    uint16       sgmii_status = 0;
    uint16       data16;
    int          hg10g_port = FALSE;
    int          tx_inx;
    int          force_cl72;
    WC40_TX_DRIVE_t tx_drv[NUM_LANES];

    pc = INT_PHY_SW_STATE(unit, port);

    /* in case it was enabled in previous speed mode */
    force_cl72 = FALSE;

    /* set default entry first */
    tx_inx = TXDRV_DFT_INX;

    pc = INT_PHY_SW_STATE(unit, port);
    if ((DEV_CFG_PTR(pc)->hg_mode) && (!DEV_CFG_PTR(pc)->cx4_10g)) { 
        hg10g_port = TRUE;
    }
  
    /* set to the default */ 
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00, 0x0));

    /* don't reset to default if clock comp is enabled by user */
    if (!(DEV_CTRL_PTR(pc)->clk_comp)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00, 
                  COMBO_HG_XGXS_nLQnCC,
                  XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_MASK));
    }

    if (!(WC40_REVID_A0(pc) || WC40_REVID_A1(pc))) {
        /* clear 42G control, PLL autotune */
        if (DEV_CFG_PTR(pc)->refclk == 161) {
            data16 = 0x900 |   /*div64 */
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK;
        } else {
            data16 = 0;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00,
                   data16, 
                   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                   SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK));
    }

    speed_val = 0;
    speed_mii = 0;
    mask      =  SERDESDIGITAL_MISC1_FORCE_SPEED_MASK;
    switch (speed) {
    case 0:
        /* Do not change speed */
        return SOC_E_NONE;
    case 10:
        speed_mii = MII_CTRL_SS_10;
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 100:
        speed_mii = MII_CTRL_SS_100;
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 1000:
        speed_mii = MII_CTRL_SS_1000;
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 2500:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_2500BRCM_X1; 
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 5000:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_5000BRCM_X4;
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 6000:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_6000BRCM_X4;
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 10000:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10GBASE_CX4; /* 10G CX4 */
        tx_inx = TXDRV_6GOS2_CX4_INX;
        if ((hg10g_port == TRUE) && DEV_CFG_PTR(pc)->rxaui) {
            /* speed_val = 0x1f;   10HiG dual-XGXS */
            /* speed_val = 0x2D 10GHig DXGXS SCR */
            speed_val = DEV_CFG_PTR(pc)->scrambler_en? 0x2D: 0x1F;
            tx_inx = TXDRV_6GOS1_INX;
        } else if (DEV_CFG_PTR(pc)->rxaui) {
            /* speed_val = 0x20;  10G ethernet dual-XGXS */
            /* speed_val = 0x2E 10G ethernet DXGXS SCR */
            speed_val = DEV_CFG_PTR(pc)->scrambler_en? 0x2E: 0x20;
            tx_inx = TXDRV_6GOS1_INX;
        } else if (hg10g_port == TRUE) {
            speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10GHiGig_X4; /* 10G HiG */
            tx_inx = TXDRV_6GOS2_INX;
        }
        break;
    case 12000:
        speed_val = DEV_CFG_PTR(pc)->rxaui? 0x23: /* 12.7HiG dual-XGXS*/
                     SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12GHiGig_X4;   /* 12 HiG */
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 12500:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p5GHiGig_X4;
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 13000:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_13GHiGig_X4;
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 15000:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_15GHiGig_X4;
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 16000:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_16GHiGig_X4;
        tx_inx = TXDRV_6GOS2_INX;
        break;
    case 20000:
        /* speed_val = 0x2C 20G_SCR */
        speed_val = DEV_CFG_PTR(pc)->scrambler_en? 
                    0x2C:
                    SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20GHiGig_X4;
        if (!(DEV_CFG_PTR(pc)->hg_mode)) {
            /* don't change  if clock comp is enabled by user */
            if (!(DEV_CTRL_PTR(pc)->clk_comp)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00,
                      COMBO_HG_XGXS_nCC, 
                      XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_MASK));
            }
        }
        tx_inx = TXDRV_6GOS1_INX;
        break;
    case 21000:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_21GHiGig_X4;
        break;
    case 25000:
        speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_25p45GHiGig_X4;
        break;
    case 30000:
        speed_val = 0x2a;
        tx_inx = TXDRV_XLAUI_INX;
        break;
    case 40000:
        if (DEV_CFG_PTR(pc)->hg_mode) {
            tx_inx = TXDRV_XLAUI_INX;
            if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_KR4) {
                speed_val = 0x31;  /* 40GKR */
            } else {
                speed_val = 0x26; /* brcm 40G */
            }
        } else {
            if ((DEV_CFG_PTR(pc)->line_intf & WC40_IF_XLAUI) ||
                (DEV_CFG_PTR(pc)->line_intf & WC40_IF_KR4) ||
                (DEV_CFG_PTR(pc)->line_intf & WC40_IF_SR) || 
                (DEV_CFG_PTR(pc)->line_intf & WC40_IF_KR)) { 
                speed_val = 0x31;  /* 40GKR */
                tx_inx = TXDRV_XLAUI_INX;
            } else if ((DEV_CFG_PTR(pc)->line_intf & WC40_IF_CR4) ||
                       (DEV_CFG_PTR(pc)->line_intf & WC40_IF_CR) ) {
                speed_val = 0x32;  /* 40GCR4 */
                tx_inx = TXDRV_XLAUI_INX;
            } else {
                speed_val = 0x31;  /* default 40GKR */
                tx_inx = TXDRV_XLAUI_INX;
            }

            if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
                SOC_IF_ERROR_RETURN
                    (WC40_REG_MODIFY(unit, pc, 0x00, 0x00008420, 0x1, 0x3));
            }
        }
        if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_XLAUI) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00, WC40_UC_CTRL_XLAUI));
            tx_inx = TXDRV_XLAUI_INX;
        } else if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_SR) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00, WC40_UC_CTRL_SR4));
            tx_inx = TXDRV_SR4_INX;
        }
        break;

    /* only work for BRCM 40G */
    case 42000:
        if (!(WC40_REVID_A0(pc) || WC40_REVID_A1(pc))) {
            speed_val = 0x26; /* use same brcm 40G speed setting */

            tx_inx = TXDRV_XLAUI_INX;
            /* force the PLL multiplier x70 */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, 
                       SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                       (0xc << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT),
                       SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                       SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK));
            if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_XLAUI) {
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00, 
                                                          WC40_UC_CTRL_XLAUI));
            } else if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_SR) {
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00, 
                                                           WC40_UC_CTRL_SR4));
            }
            force_cl72 = DEV_CFG_PTR(pc)->force_cl72? TRUE : FALSE;
        }
        break;
    default:
        return SOC_E_PARAM;
    }
    
    /* configure the TX driver parameters per speed mode */        
    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_control_get(unit, port,&tx_drv[0],tx_inx));
    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_control_set(unit, port,&tx_drv[0]));

    /* Puts PLL in reset state and forces all datapath into reset state */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, 0,
                                  XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

    /* Select a different OS mode if possible to reduce jitter */
    if (speed <= 2500) {
        /* At 2.5G or less, use OS5 mode */
        data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS5 <<
                        XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT);
    } else {
        /* For all other speeds, just use combo mode */
        data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_ComboCoreMode <<
                        XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT);
    }
    SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, data16,
                                        XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

    if ((force_cl72 != FORCE_CL72_IS_ENABLED(pc)) || (force_cl72 == TRUE)) {
        SOC_IF_ERROR_RETURN
            (_phy_wc40_force_cl72_config(unit,port,force_cl72));
        FORCE_CL72_ENABLE(pc) = force_cl72;
    }

    /* 2wire XAUI configuration */
    SOC_IF_ERROR_RETURN
        (_wc40_rxaui_config(unit,pc,DEV_CFG_PTR(pc)->rxaui));

    /* disable 100FX and 100FX auto-detect */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_FX100_CONTROL1r(unit,pc,0x00,
                              0,
                              FX100_CONTROL1_AUTODET_EN_MASK |
                              FX100_CONTROL1_ENABLE_MASK));

    /* disable 100FX idle detect */
    SOC_IF_ERROR_RETURN
            (MODIFY_WC40_FX100_CONTROL3r(unit,pc,0x00,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK));

    if ((speed == 42000) && (DEV_CFG_PTR(pc)->line_intf & WC40_IF_KR4)){
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00,
                SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_KR4,
                SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, speed_val, mask));
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0x00, 
                (speed_val & 0x20)?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0,
                DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));

    if (speed <= 1000) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, pc, 0x00, &sgmii_status));

        sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;
        if (!sgmii_status && (speed == 100)) {

            /* fiber mode 100fx, enable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_FX100_CONTROL1r(unit,pc,0x00,
                                  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK,
                                  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK));

            /* enable 100fx extended packet size */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_FX100_CONTROL2r(unit,pc,0x00,
                                FX100_CONTROL2_EXTEND_PKT_SIZE_MASK,
                                FX100_CONTROL2_EXTEND_PKT_SIZE_MASK));
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, speed_mii,
                                              MII_CTRL_SS_MASK));
        }
    }

    /* Bring PLL out of reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    (void) _phy_wc40_pll_lock_wait(unit, port);
    _phy_wc40_control_vco_disturbed_set(unit, port);

    if ((WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) && (speed == 40000) &&
        (!DEV_CFG_PTR(pc)->hg_mode)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_PCS_IEEE2BLK_PCS_TPCONTROLr(unit, pc, 0x00, (1U << 4),
                        ((1U << 4)|(1U << 1))));
                sal_usleep(100);
                SOC_IF_ERROR_RETURN
                    (WC40_REG_MODIFY(unit, pc, 0x00, 0x00008420, 0x0, 0x3));
                SOC_IF_ERROR_RETURN
                    (WC40_REG_MODIFY(unit, pc, 0x00, 0x00008020, (1U << 2), (1U << 2)));
                SOC_IF_ERROR_RETURN
                    (WC40_REG_MODIFY(unit, pc, 0x00, 0x00008020, 0, (1U << 2)));
                sal_usleep(100);
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_PCS_IEEE2BLK_PCS_TPCONTROLr(unit, pc, 0x00, 0x0, (1U << 4)));
    }

       return SOC_E_NONE;
}

STATIC int
_phy_wc40_interlaken_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc;
    uint16 data16;
    uint16 mask16;

    pc = INT_PHY_SW_STATE(unit, port);

    mask16 = 0x0303 << (pc->lane_num * 2);

    switch (speed) {
        case 6250:
        case 6000:
            data16 = mask16;
            break;
        case 3125:
        case 3000: 
            data16 = 0x0202 << (pc->lane_num * 2);
            break;
        default:
            return SOC_E_PARAM;
    }
    /* hold tx/rx in reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK1_LANECTRL1r(unit, pc, 0x00, data16,mask16));

    /* release tx/rx reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00,
              0,
              DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wc40_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);

    if (WC40_LINK_WAR_REVS(pc)) {
        WC40_SP_VALID_RESET(pc);
    }
    
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        rv = _phy_wc40_speed_set(unit,port,speed);
        if (WC40_LINK_WAR_REVS(pc)) {
            if (speed >=25000) {  /* BRCM 64B/66B mode */
                WC40_SP_VALID_SET(pc);
            }
        }
    } else {
        if (DEV_CFG_PTR(pc)->custom) {
            rv = _phy_wc40_interlaken_speed_set(unit, port, speed);
        } else {
            rv = _phy_wc40_ind_speed_set(unit,port,speed);
            if (WC40_LINK_WAR_REVS(pc)) {
                if (speed >=12000) {  /* BRCM 64B/66B mode */
                    WC40_SP_VALID_SET(pc);
                }
            }
        }
    }
    return rv;
}

STATIC int
_phy_wc40_tx_fifo_reset(int unit, soc_port_t port,uint32 speed)
{
    uint16 data16;
    phy_ctrl_t  *pc;
    pc = INT_PHY_SW_STATE(unit, port);

    if (speed == 100) {
        /* check if it is in 100fx mode */
        SOC_IF_ERROR_RETURN
            (READ_WC40_FX100_CONTROL1r(unit,pc,0x00,&data16));

        if (data16 & FX100_CONTROL1_ENABLE_MASK) {
            /* reset TX FIFO  */
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,LANE0_ACCESS,0x8061 + (0x10 * pc->lane_num),
                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK));

            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,LANE0_ACCESS,0x8061 + (0x10 * pc->lane_num),
                              0,
                              TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wc40_notify_mac_loopback
 * Purpose:
 *      turn on rx clock compensation in mac loopback mode for
 *      applicable XGXS devices
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - TRUE mac loopback mode, FALSE not in mac loopback mode
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_wc40_notify_mac_loopback(int unit, soc_port_t port, uint32 enable)
{
    phy_ctrl_t  *pc;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_notify_mac_loopback: u=%d p=%d enable=0x%x\n"),
              unit, port, enable));

    pc = INT_PHY_SW_STATE(unit, port);

    /* XMAC loopback mode needs to enable clock comp for single-port mode */
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        if ((DEV_CTRL_PTR(pc)->clk_comp && (!enable)) ||
            ((!DEV_CTRL_PTR(pc)->clk_comp) && enable)) {
            /* clock compensation configuration in combo mode */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00,
                     enable? COMBO_HG_XGXS_nLQ | COMBO_XE_XGXS:
                            COMBO_CLOCK_COMP_DEFAULT,
                     COMBO_CLOCK_COMP_MASK));
            DEV_CTRL_PTR(pc)->clk_comp = enable;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_speed_mode_decode(int speed_mode, int *speed, int *intf, int *scr)
{
    *scr  = FALSE;
    *intf = SOC_PORT_IF_XGMII;  /* default to XGMII */
    switch (speed_mode) {
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10M:
            *speed = 10;
            *intf = SOC_PORT_IF_MII;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_100M:
            *speed = 100;
            *intf = SOC_PORT_IF_MII;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_1G:
            *speed = 1000;
            *intf = SOC_PORT_IF_GMII;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_2p5G:
            *speed = 2500;
            *intf = SOC_PORT_IF_GMII;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_5G_X4:
            *speed = 5000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_6G_X4:
            *speed = 6000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_HiG          :
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_CX4          :
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12G_HiG          :
            *speed = 12000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12p5G_X4         :
            *speed = 12500;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_13G_X4           :
            *speed = 13000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_15G_X4           :
            *speed = 15000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_16G_X4           :
            *speed = 16000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_1G_KX            :
            *speed = 1000;
            *intf = SOC_PORT_IF_GMII;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_KX4          :
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_KR           :
            *speed = 10000;
            *intf = SOC_PORT_IF_KR;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_5G               :
            *speed = 5000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_6p4G             :
            *speed = 6000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_X4           :
            *speed = 20000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_KR2           :
            *speed = 20000;
            *intf = SOC_PORT_IF_KR;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_CR2           :
            *speed = 20000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_21G_X4           :
            *speed = 21000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_25G_X4           :
            *speed = 25000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_HiG_DXGXS    :
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_DXGXS        :
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10p5G_HiG_DXGXS  :
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10p5G_DXGXS      :
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12p773G_HiG_DXGXS:
            *speed = 12000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12p773G_DXGXS    :
            *speed = 12000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_XFI          :
            *speed = 10000;
            *intf = SOC_PORT_IF_XFI;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_40G              :
            *speed = 40000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_HiG_DXGXS    :
            *speed = 20000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_DXGXS        :
            *speed = 20000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_SFI          :
            *speed = 10000;
            *intf = SOC_PORT_IF_SFI;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_31p5G            :
            *speed = 30000;  /* MAC has only 30G speed*/
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_32p7G            :
            *speed = 32000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_SCR          :
            *scr = TRUE;
            *speed = 20000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_HiG_DXGXS_SCR:
            *scr = TRUE;
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_DXGXS_SCR    :
            *scr = TRUE;
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12G_R2           :
            *speed = 12000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_X2           :
            *speed = 10000;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_40G_KR4          :
            *speed = 40000;
            *intf = SOC_PORT_IF_KR4;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_40G_CR4          :
            *speed = 40000;
            *intf = SOC_PORT_IF_CR4;
            break;
        case XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_100G_CR10        :
            *speed = 100000;
            *intf = SOC_PORT_IF_XLAUI;
            break;
        case 0x2c:
            *speed = 15000;
            break;
        default:
            break;
    }

    return SOC_E_NONE;
}

STATIC int
_phy_wc40_ind_speed_get(int unit, soc_port_t port, int *speed, int *intf, int *scr)
{
    phy_ctrl_t *pc;
    uint16 speed_mode, temp16;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);

    *speed = 0;
    if (pc->lane_num < 2) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_GP2_REG_GP2_2r(unit,pc,0x00,&speed_mode));
        if (pc->lane_num == 0) {
            speed_mode &= GP2_REG_GP2_2_ACTUAL_SPEED_LN0_MASK;
        } else {
            speed_mode &= GP2_REG_GP2_2_ACTUAL_SPEED_LN1_MASK;
            speed_mode >>= GP2_REG_GP2_2_ACTUAL_SPEED_LN1_SHIFT;
        } 
    } else {
        SOC_IF_ERROR_RETURN
            (READ_WC40_GP2_REG_GP2_3r(unit,pc,0x00,&speed_mode));
        if (pc->lane_num == 2) {
            speed_mode &= GP2_REG_GP2_3_ACTUAL_SPEED_LN2_MASK;
        } else {
            speed_mode &= GP2_REG_GP2_3_ACTUAL_SPEED_LN3_MASK;
            speed_mode >>= GP2_REG_GP2_3_ACTUAL_SPEED_LN3_SHIFT;
        } 
    }
    rv = _phy_wc40_speed_mode_decode(speed_mode, speed,intf,scr);
     
    if ((speed_mode == XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_SFI) ||
        (speed_mode == XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_XFI)) {
        /* PLL mode */
        SOC_IF_ERROR_RETURN
            (READ_WC40_GP2_REG_GP2_0r(unit, pc, 0x00, &temp16));

        /* Check PLL mode */
        if( 0xC000 == (temp16 & 0xF000)) {
            *speed = 11000;
        }
    }
    
    if((speed_mode == XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_HiG_DXGXS) || 
       (speed_mode == XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_KR2)) {
        /* PLL mode */
        SOC_IF_ERROR_RETURN
            (READ_WC40_GP2_REG_GP2_0r(unit, pc, 0x00, &temp16));

        /* Check PLL mode */
        if( 0xC000 == (temp16 & 0xF000)) {
            *speed = 21000;
        }
    }
    return rv;
}

STATIC int
_phy_wc40_combo_speed_get(int unit, soc_port_t port, int *speed, int *intf, int *scr)
{
    phy_ctrl_t *pc;
    uint16 speed_mode;
    uint16 data16;
    int rv;

    pc = INT_PHY_SW_STATE(unit, port);
    
    /* XXX should use 0x81d2 same as the ind_speed_get */
    *speed = 0;
    SOC_IF_ERROR_RETURN
        (READ_WC40_XGXSBLK5_XGXSSTATUS4r(unit,pc,0x00,&speed_mode));
    speed_mode &= XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_MASK;
    rv = _phy_wc40_speed_mode_decode(speed_mode, speed,intf,scr);
    if (*speed == 40000) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00,&data16));
        if ((data16 & (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                      SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK)) ==
                   (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                   (0xc << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT)) ) {
            *speed = 42000;
        } 
    }
    return rv;
}

STATIC int
_phy_wc40_interlaken_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;
    uint16 data16;
    uint16 speed_mode;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_WC40_XGXSBLK1_LANECTRL1r(unit, pc, 0x00, &data16));
    speed_mode = (data16 >> (pc->lane_num * 2)) & 
              XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK;

    switch (speed_mode) {
        case XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div1:
            *speed = 6250;
            break;
        case XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div2:
            *speed = 3125;
            break;
        default:
            *speed = 0;
            break;
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_interlaken_speed_get: u=%d p=%d LANECTRL1r %04x speed= %d\n"),
              unit, port,data16, *speed));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - current link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_wc40_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;
    int rv;
    int intf;
    int scr;

    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        rv = _phy_wc40_combo_speed_get(unit, port, speed,&intf,&scr);
    } else {
        if (DEV_CFG_PTR(pc)->custom) {
            rv = _phy_wc40_interlaken_speed_get(unit, port, speed);
        } else {
            rv = _phy_wc40_ind_speed_get(unit, port, speed,&intf,&scr);
        }
    }
    return rv;
}

/*
 * software autoneg
 */
STATIC int
_wc40_soft_an_cl73kr2_init(int unit, soc_port_t port)
{
    phy_ctrl_t       *pc;
    uint16 mask16;
    int i;
    uint16 data16;

    pc = INT_PHY_SW_STATE(unit, port);

    /* disable 1000X parallel detect */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0x00,
          0,
          SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));

    /* disable 10G parallel detect */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(unit, pc, 0x00,
              0,
              XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));

    /* diable CL37 BAM */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(unit,pc,0x00,0));

    /* disable CL37 */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, 0,
                                         MII_CTRL_AE));

    /* diable the CL73 BAM */
    /* need to enable these two bits even BAM is disabled to interop
     * with other BRCM devices properly.
     */
    mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK |
                 CL73_USERB0_CL73_BAMCTRL1_CL73_BAMNP_AFTER_BP_EN_MASK |
                 CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(unit, pc, 0x00,
                   0, mask16));

    /* Set cl73_suppress_mr_page_rx_dis: 0x8378[15:0]=0x100 */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_UCTRL2r(unit, pc, 0x00, 0x100, 0x100));

    DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_INIT;

    /* configure CL73 adv0 page: add next page */
    /* BAM73 disabled and NP is set, in this mode, the autoneg will not continue until
     * a null page is sent
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit,pc,0x0, 0x8001,0x8001));

    /* clear KR2 ETA bit */
    SOC_IF_ERROR_RETURN
        (WC40_REG_MODIFY(unit,pc,0x0,0x8450, 0, 1));

    SOC_IF_ERROR_RETURN
        (READ_WC40_CL49_USERB0_CONTROLr(unit, pc, 0x00, &data16));
    if ((data16 & (CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_VALUE_MASK |
              CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_CTRL_MASK)) !=
         (CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_VALUE_MASK |
          CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_CTRL_MASK)) {

          /* restart the sequence */
          SOC_IF_ERROR_RETURN
              (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00,
                              0,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

          for (i = 0; i < NUM_LANES; i++) {
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_CL49_USERB0_CONTROLr(unit, pc,
                    ln_access[i],
                    0,
                    CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_VALUE_MASK |
                    CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_CTRL_MASK));
          }
          SOC_IF_ERROR_RETURN
              (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
          /* wait for PLL lock */
          _phy_wc40_pll_lock_wait(unit, port);
          _phy_wc40_control_vco_disturbed_set(unit, port);

    }
    return SOC_E_NONE;
}

STATIC int
_wc40_soft_an_cl73kr2(int unit, soc_port_t port, int an)
{
    phy_ctrl_t       *pc;
    uint16 mask16;
    int lane = 0, lane_end = 0, lane_start = 0;

    pc = INT_PHY_SW_STATE(unit, port);
    
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3;
    } else {
        lane_start = pc->lane_num;
        lane_end   = lane_start + 1;
    }

    /* Set cl73_suppress_mr_page_rx_dis: 0x8378[15:0]=0x100 */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_UCTRL2r(unit, pc, 0x00, an? 0x100: 0,0x100));

    if (an) {
        DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_INIT;

        /* disable BAM mode, set 0x8372[15:13]=0.  
         * BAM73 disabled and NP is set, in this mode, the autoneg will be controlled by
         * software and completed until a null page is sent
         */
        mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK |
                 CL73_USERB0_CL73_BAMCTRL1_CL73_BAMNP_AFTER_BP_EN_MASK |
                 CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(unit, pc, 0x00,
                       0, mask16));

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit,pc,0x0, 0x8001,0x8001));

        /* clear KR2 ETA bit */
        SOC_IF_ERROR_RETURN
            (WC40_REG_MODIFY(unit,pc,0x0,0x8450, 0, 1));
    }

    /* restart CL73 AN */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit,pc,0x0, an? MII_CTRL_AE | MII_CTRL_RAN: 0));
    /* Clean up: To cover the special case in SOFT KR2 in which tx is squelched */
    if(SOFT_KR2_TIMER_VALID(pc)) {
        SOFT_KR2_TIMER_DIS(pc);
        for (lane = lane_start; lane <= lane_end; lane++) {
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,0,
                                WC40_PER_LANE_TX_DRIVERr(lane), 
                                 0x0000, TX0_TX_DRIVER_ELECIDLE_MASK));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_wc40_soft_an_cl73kr2_custom(int unit, soc_port_t port, int an)
{

    return SOC_E_NONE;
}


STATIC int
_wc40_soft_an_cl73kr2_custom_check(int unit, soc_port_t port)
{
    return SOC_E_NONE;
}

STATIC int
_wc40_soft_an_cl73kr2_check(int unit, soc_port_t port)
{
    uint16 data16;
    uint16 lp_nxp1;
    uint16 ld_adv;
    uint16 lp_adv = 0;
    uint16 lp_nxp0, lp_nxp0_msg;
    uint16 lp_nxp2, ld_nxp0 = 0, ld_nxp1 = 0, ld_nxp2 = 0;
    phy_ctrl_t       *pc;
    int msg_processed = FALSE;
    int lane = 0, lane_end = 0, lane_start = 0;

    pc = INT_PHY_SW_STATE(unit, port);
    
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        lane_start = 0;
        lane_end   = 3;
    } else {
        lane_start = pc->lane_num;
        lane_end   = lane_start + 1;
    }
    
    /* Waiting for cl73_mr_page_rx, wait for 0x1[6]=1, from linkscan thread */
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE0BLK_AN_IEEESTATUS1r(unit,pc,0x0, &data16));

    if (data16 & (1 << 6)) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_AN_IEEE1BLK_AN_LP_XNP_ABILITY0r(unit, pc, 0x00,&lp_nxp0));
        SOC_IF_ERROR_RETURN
            (READ_WC40_AN_IEEE1BLK_AN_LP_XNP_ABILITY1r(unit, pc, 0x00, &lp_nxp1));
        SOC_IF_ERROR_RETURN
            (READ_WC40_AN_IEEE1BLK_AN_LP_XNP_ABILITY2r(unit, pc, 0x00, &lp_nxp2));
        SOC_IF_ERROR_RETURN
            (READ_WC40_CL73_USERB0_CL73_USTAT1r(unit, pc, 0x00, &data16));

        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "KR2 autoneg wait page_rx bit set: u=%d p=%d kr2_state:0x%x,nxp0:0x%x,nxp1:0x%x,nxp2:0x%x,fsm:0x%x\n"), 
                     unit, port,DEV_CTRL_PTR(pc)->soft_kr2.state,lp_nxp0,lp_nxp1,lp_nxp2,data16));
        lp_nxp0_msg = lp_nxp0 & AN_IEEE1BLK_AN_LP_XNP_ABILITY0_MSG_MASK;

        /* received base page, start soft autoneg */
        if ((lp_nxp0_msg == 0) && (lp_nxp1 == 0) && (lp_nxp2 == 0)
            && !(DEV_CTRL_PTR(pc)->soft_kr2.state == WC40_SOFT_KR2_STATE_WAIT_4_BP)) {
           DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_INIT; 

            /* clear KR2 ETA bit */
            SOC_IF_ERROR_RETURN
                (WC40_REG_MODIFY(unit,pc,0x0,0x8450, 0, 1));
        }

        if((DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_WAR)
          && (SOFT_KR2_TIMER_VALID(pc))) {
            DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_TIMER;
        }     

        if (DEV_CTRL_PTR(pc)->soft_kr2.state == WC40_SOFT_KR2_STATE_INIT) {
            if ((lp_nxp0_msg == 0) && (lp_nxp1 == 0) && (lp_nxp2 == 0)) {
                /* all three Extend pages should be 0 when base page is received */
                /* store the link partner ability */
                SOC_IF_ERROR_RETURN
                    (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY1r(unit, pc, 0x00, &data16));
                DEV_CTRL_PTR(pc)->soft_kr2.lp_adv = data16;

                SOC_IF_ERROR_RETURN
                    (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY0r(unit, pc, 0x00, &data16));

                /* WAR to address interop issue with devices such as 8073 and similar.
                   8073 get's into lock up state if we send it a base page with NP=1
                   (already sent). It is found that in order to break that lock up state 
                   WC must squelch the TX for a >= fixed amount of time.
                */ 
                if(!(data16 & AN_IEEE1BLK_AN_LP_BASEPAGEABILITY0_NP_MASK)
                   && (DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_WAR)
                   && (!SOFT_KR2_TIMER_VALID(pc))) {
                    /* Base page, next page NP=0 */ 
                    SOC_IF_ERROR_RETURN
                        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit,pc,0x0, 0x0001,0x8001));

                    /* restart CL73 AN */
                    SOC_IF_ERROR_RETURN
                        (WRITE_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit,pc,0x0, MII_CTRL_AE | MII_CTRL_RAN));

                    /* Start Tx Squelch */
                    for (lane = lane_start; lane <= lane_end; lane++) {
                        SOC_IF_ERROR_RETURN
                            (WC40_REG_MODIFY(unit,pc,0,
                                            WC40_PER_LANE_TX_DRIVERr(lane), 
                                             TX0_TX_DRIVER_ELECIDLE_MASK, TX0_TX_DRIVER_ELECIDLE_MASK));
                    }

                    SOFT_KR2_TIMER_EN(pc);
                    SOFT_KR2_RECORD_TIME(pc);
                    DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_TIMER;
                    msg_processed = TRUE;
                    return SOC_E_NONE;
                }
                
                /* Base page, next page NP=1 */ 
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit,pc,0x0, 0x8001,0x8001));

                ld_nxp1 = WC40_BRCM_OUI0;
                /* BRCM method send BRCM next page */
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT1r(unit, pc, 0x00, ld_nxp1));
                ld_nxp2 = WC40_BRCM_OUI1;
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT2r(unit, pc, 0x00, ld_nxp2));

                /* indicate MP-5 page and next page set */
                ld_nxp0 = 0x5 | AN_IEEE1BLK_AN_XNP_TRANSMIT0_NP_MASK |
                       AN_IEEE1BLK_AN_XNP_TRANSMIT0_MP_MASK;
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT0r(unit, pc, 0x00, ld_nxp0));
                DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_BASE_PAGE; 
                msg_processed = TRUE;
            }
        } else if (DEV_CTRL_PTR(pc)->soft_kr2.state == WC40_SOFT_KR2_STATE_BASE_PAGE) { 
      
           /* If (7.0019.13 = 1 and 7.0019.10:0 = h5) [MP5 is received], 
              remote partner appears to be sending out next pages symmetrically 
            */ 
            if ((lp_nxp0 & AN_IEEE1BLK_AN_LP_XNP_ABILITY0_NP_MASK)
                 &&(lp_nxp0_msg == WC40_AUTONEG_MSG_MP5) 
                 && (lp_nxp1 == WC40_BRCM_OUI0) 
                 && (lp_nxp2 == WC40_BRCM_OUI1)) {
            
                /* match BRCM MP-5, now send BAM3 page indicating KR2 */
                ld_nxp1 = DEV_CTRL_PTR(pc)->soft_kr2.ld_kr2? WC40_BRCM_ETA_KR2: 0;
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT1r(unit, pc, 0x00, ld_nxp1));

                ld_nxp2 = 0x00;
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT2r(unit, pc, 0x00, ld_nxp2));
                /* Send UP1 with KR2 ability set */
                ld_nxp0 = WC40_BRCM_CODE_BAM3 | AN_IEEE1BLK_AN_XNP_TRANSMIT0_NP_MASK;
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT0r(unit, pc, 0x00, ld_nxp0));
                
                DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_MP5; 
                msg_processed = TRUE;
            }
        } else if (DEV_CTRL_PTR(pc)->soft_kr2.state == WC40_SOFT_KR2_STATE_MP5) {
            if ((lp_nxp0_msg == WC40_BRCM_CODE_BAM3) && (lp_nxp2 == 0)) {
                /* KR2 match */
                if (lp_nxp0_msg == WC40_BRCM_CODE_BAM3 && lp_nxp1 == WC40_BRCM_ETA_KR2) {
                    lp_adv = CL73_AN_ADV_TECH_20G_KR2;
                }
                DEV_CTRL_PTR(pc)->soft_kr2.lp_adv |= lp_adv; 
                DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_UP1;
            }
        } else if(DEV_CTRL_PTR(pc)->soft_kr2.state == WC40_SOFT_KR2_STATE_TIMER) {
            /* Received a page:
               Disable the timer if it is timed out. Remove the tx squelch.
            */
            if(SOFT_KR2_TIMER_VALID(pc)
               && (DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_WAR)) {
               if(SOFT_KR2_CHECK_TIMEOUT(pc)) {
                   SOFT_KR2_TIMER_DIS(pc);
                    for (lane = lane_start; lane <= lane_end; lane++) {
                        SOC_IF_ERROR_RETURN
                            (WC40_REG_MODIFY(unit,pc,0,
                                            WC40_PER_LANE_TX_DRIVERr(lane), 
                                             0x0000, TX0_TX_DRIVER_ELECIDLE_MASK));
                    }
                   DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_WAIT_4_BP;
               }
               msg_processed = TRUE;
               return SOC_E_NONE;
            }
        } else if(DEV_CTRL_PTR(pc)->soft_kr2.state == WC40_SOFT_KR2_STATE_WAIT_4_BP) {

          if((lp_nxp0_msg == 0) && (lp_nxp1 == 0) && (lp_nxp2 == 0)) {
              DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_INIT;
          }
          msg_processed = TRUE;
          return SOC_E_NONE;

        } else if(DEV_CTRL_PTR(pc)->soft_kr2.state == WC40_SOFT_KR2_STATE_WAIT_4_NP) {
            /* Added state WC40_SOFT_KR2_STATE_WAIT_4_NP to avoid sending extra NP*/ 
            if(data16 & AN_IEEE1BLK_AN_LP_BASEPAGEABILITY0_NP_MASK) {
                /* If next page is still set, send NULL page */
                DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_DONE;
                msg_processed = TRUE;
            } else if (lp_nxp0_msg == 0x1) {
                /* Recevied NULL page */
                DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_INIT;
                msg_processed = TRUE;
                return SOC_E_NONE;
            }
        }

        if(DEV_CTRL_PTR(pc)->soft_kr2.state == WC40_SOFT_KR2_STATE_UP1) {

            /* 
               1. If remote partner is sending next pages symmetrically, 
               then if (7.0019.10:0 = h1), the remote partner is sending 
               null pages and is likely another BRCM device.
               
               OR

               2. If remote partner is sending next pages pre-MP5 and MP5 was received, 
               then if (7.0019.15 = 0) [UP1 was received]
             */ 

            if( (lp_nxp0_msg == WC40_AUTONEG_MSG_NULL) ||
                ((lp_nxp0 & AN_IEEE1BLK_AN_LP_XNP_ABILITY0_NP_MASK) == 0)) {

                SOC_IF_ERROR_RETURN
                    (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(unit, pc, 0x00, &ld_adv));
                ld_adv |= DEV_CTRL_PTR(pc)->soft_kr2.ld_kr2? CL73_AN_ADV_TECH_20G_KR2: 0;
                lp_adv = DEV_CTRL_PTR(pc)->soft_kr2.lp_adv;
                
                /* use PCS type auto-selection for all speeds other than KR2 */
                if (WC40_REVID_B(pc)) {
                        SOC_IF_ERROR_RETURN
                            (MODIFY_WC40_DIGITAL4_MISC4r(unit, pc, 0x00, 
                                     DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK, 
                                     DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK));
                }
                /* resolve to KR4 */      
                if ((ld_adv & lp_adv & CL73_AN_ADV_PLUS_KR2_MASK) & 
                    CL73_AN_ADV_TECH_40G_KR4) { 
                    /* do nothing */ 
                    LOG_VERBOSE(BSL_LS_SOC_PHY,
                                (BSL_META_U(unit,
                                            "resolved to KR4 : u=%d p=%d\n"), unit, port));
                } else if ((ld_adv & lp_adv & CL73_AN_ADV_PLUS_KR2_MASK) & 
                    CL73_AN_ADV_TECH_20G_KR2) {  

                    /* workaround for KR2 speed status, use fixed PCS type */
                    if (WC40_REVID_B(pc)) {
                        /* disable PCS type auto selection */
                        SOC_IF_ERROR_RETURN
                            (MODIFY_WC40_DIGITAL4_MISC4r(unit, pc, 0x00, 
                                     0, 
                                     DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK));

                        /* 40GBASE-R PCS type */ 
                        SOC_IF_ERROR_RETURN
                            (MODIFY_WC40_PCS_IEEE0BLK_PCS_IEEECONTROL2r(unit, pc, 0x00, 
                                 0x4, PCS_IEEE0BLK_PCS_IEEECONTROL2_PCS_TYPESEL_MASK));
                    }

                    /* set KR2 ETA bit */
                    SOC_IF_ERROR_RETURN
                        (WC40_REG_MODIFY(unit,pc,0x0,0x8450, 1, 1));
                    LOG_VERBOSE(BSL_LS_SOC_PHY,
                                (BSL_META_U(unit,
                                            "KR2 autoneg resolves to KR2: u=%d p=%d\n"), unit, port));

                } else {  /* all other speeds */
                    LOG_VERBOSE(BSL_LS_SOC_PHY,
                                (BSL_META_U(unit,
                                            "resolved to non KR2/KR4 speeds: u=%d p=%d\n"), unit, port));
                }
        
                DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_DONE; 
                msg_processed = TRUE;
            }
        }
        /* handle unknow next page */
        if (msg_processed == FALSE) {
            if (lp_nxp0 & AN_IEEE1BLK_AN_LP_XNP_ABILITY0_NP_MASK) {
                /* Send a NULL next page */
                ld_nxp1 =0x00;
                SOC_IF_ERROR_RETURN
                   (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT1r(unit, pc, 0x00, ld_nxp1));
                ld_nxp2 =0x00;
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT2r(unit, pc, 0x00, ld_nxp2));
                ld_nxp0 =0xA001;
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT0r(unit, pc, 0x00, ld_nxp0));
            } else { 
               /* received autoneg termination null page, responds accordingly */
               DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_DONE;
            }
        }

        if (DEV_CTRL_PTR(pc)->soft_kr2.state == WC40_SOFT_KR2_STATE_DONE) {
            /* completes the autoneg by send out a null page with NP=0 */
            ld_nxp1 =0x00;
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT1r(unit, pc, 0x00, ld_nxp1));
            ld_nxp2 =0x00;
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT2r(unit, pc, 0x00, ld_nxp2));
            ld_nxp0 =0x2001;
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_AN_IEEE1BLK_AN_XNP_TRANSMIT0r(unit, pc, 0x00, ld_nxp0));
            DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_WAIT_4_NP;
        }
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "KR2 autoneg tx page: u=%d p=%d kr2_state:0x%x,tx_nxp0:0x%x,tx_nxp1:0x%x,tx_nxp2:0x%x\n"), 
                     unit, port,DEV_CTRL_PTR(pc)->soft_kr2.state,ld_nxp0,ld_nxp1,ld_nxp2));
    } else {
        /* No page received */
        if(SOFT_KR2_TIMER_VALID(pc)
           && (DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_WAR)) {
            if(SOFT_KR2_CHECK_TIMEOUT(pc)) {
                SOFT_KR2_TIMER_DIS(pc);
                for (lane = lane_start; lane <= lane_end; lane++) {
                    SOC_IF_ERROR_RETURN
                        (WC40_REG_MODIFY(unit,pc,0,
                                        WC40_PER_LANE_TX_DRIVERr(lane), 
                                         0x0000, TX0_TX_DRIVER_ELECIDLE_MASK));
                }
                DEV_CTRL_PTR(pc)->soft_kr2.state = WC40_SOFT_KR2_STATE_WAIT_4_BP;
                LOG_VERBOSE(BSL_LS_SOC_PHY,
                            (BSL_META_U(unit,
                                        "KR2 autoneg (WAR) no page rcvd: u=%d p=%d kr2_state:0x%x\n"), 
                             unit, port,DEV_CTRL_PTR(pc)->soft_kr2.state));
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_wc40_an_set
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
int
phy_wc40_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16             an_enable;
    uint16             auto_det;
    uint16             mask16;
    uint16             data16;
    WC40_TX_DRIVE_t tx_drv[NUM_LANES];

    pc = INT_PHY_SW_STATE(unit, port);

    if (AUTONEG_MODE_UNAVAIL(pc)) {
        return SOC_E_NONE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_wc40_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));

    an_enable = 0;
    auto_det  = 0;

    if (an) {
        /* clear force cl72 configuration */
        if (FORCE_CL72_IS_ENABLED(pc) == TRUE) {
            SOC_IF_ERROR_RETURN
                (_phy_wc40_force_cl72_config(unit,port,FALSE));
            FORCE_CL72_ENABLE(pc) = FALSE;
        }
    } 
   /* Skip, not required if forced cl72 (f/w) is enabled and is active. 
    */ 
    if(!((DEV_CFG_PTR(pc)->force_cl72_fw) 
                && (FORCE_CL72_IS_ENABLED(pc) == TRUE))) {
        /* enable cl72 device in autoneg mode and disable it in forced mode */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_cl72_enable(unit,port,an));
    }
    
    if (an) {
        /* disable 100FX and 100FX auto-detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_FX100_CONTROL1r(unit,pc,0x00,
                                  0,
                                  FX100_CONTROL1_AUTODET_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK));

        /* disable the cl82 local fault. Needed to allow device link up at 40G KR4
         * when XMAC interface is still in XGMII mode. Once WC indicates 40G KR4 linkup,
         * the port update function will set the XMAC in XLGMII mode.
         */
        SOC_IF_ERROR_RETURN
            (WC40_REG_MODIFY(unit, pc, 0x00, 0x842A, (1 << 5),(1 << 5)));
        WC40_AN_VALID_SET(pc);
        WC40_AN_RECORD_TIME(pc);

        /* set to the default */ 
        SOC_IF_ERROR_RETURN
            (_phy_wc40_firmware_mode_set(unit, port, UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_DEFAULT));

        if (WC40_LINK_WAR_REVS(pc)) {
            WC40_SP_VALID_RESET(pc);
        }

        if (IS_DUAL_LANE_PORT(pc)) {
            if (WC40_REVID_B(pc)) {
            /* part of workaround for KR2 speed status, enable PCS type auto selection */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL4_MISC4r(unit, pc, 0x00, 
                             DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK, 
                             DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK));

                /* for KR2,KR, no clock divider sync for dxgxs */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL49_USERB0_CONTROLr(unit, pc,0x00,
                      0,
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_VALUE_MASK |
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_CTRL_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL49_USERB0_CONTROLr(unit, pc,
                      ln_access[pc->lane_num+1],
                      0,
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_VALUE_MASK |
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_CTRL_MASK));

                /* cl49 single lane rx clock comp, needed for KR */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_RX66_CONTROLr(unit, pc, 0x00,
                    RX66_CONTROL_CC_EN_MASK |RX66_CONTROL_CC_DATA_SEL_MASK,
                    RX66_CONTROL_CC_EN_MASK | RX66_CONTROL_CC_DATA_SEL_MASK));

            }
            /* tx_out uses div33 clock. mixed speeds(10G/1G) on each lane */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, pc, LANE0_ACCESS,
                       (1 << 13), (1 << 13)));
        }

        /* configure the TX driver parameters for autoneg mode, use AN entry  */        
        SOC_IF_ERROR_RETURN
            (_phy_wc40_tx_control_get(unit, port,&tx_drv[0],TXDRV_AN_INX));
        SOC_IF_ERROR_RETURN
            (_phy_wc40_tx_control_set(unit, port,&tx_drv[0]));

        an_enable = MII_CTRL_AE | MII_CTRL_RAN;

        /*
         * Should read one during init and cache it in Phy flags
         */
        if (DEV_CFG_PTR(pc)->auto_medium) {
            auto_det = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK; 
        }

        /* check if need to switch to a different VCO */
        SOC_IF_ERROR_RETURN
            (_phy_wc40_vco_set(unit,port,0,0));

        /* stop rxseq and txfifo, need to properly configure for 1G speeds
         * before doing AN. 
         */
        DUAL_LANE_BCST_ENABLE(pc);

        WC40_CL73_AN_TX_FIFO_RESET_STOP(pc);

        /* hold tx/rx in reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00,
                  DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK,
                  DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));

        /* If auto-neg is enabled, make sure not forcing any speed */

        /* Workaround Jira# SDK-32387 */
        /* control the speeds thru pma/pmd register */
        if (WC40_SDK32387_REVS(pc)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                           SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK,
                           SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
        }

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, 0, 
                                   SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0x00, DIGITAL4_MISC3_IND_40BITIF_MASK, 
                           DIGITAL4_MISC3_IND_40BITIF_MASK | DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));

        /* Workaround Jira# SDK-32387 */
        /* control the speeds thru pma/pmd register */
        if (WC40_SDK32387_REVS(pc)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                               0,
                               SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
        }

        /* release the tx/tx reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00,
                  0,
                  DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK));


        WC40_CL73_AN_TX_FIFO_RESET_START(pc);

        DUAL_LANE_BCST_DISABLE(pc);

        /* Enable/Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, pc, 0x00, auto_det,
                                 SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 

            /* clear 2wire XAUI configuration */
            SOC_IF_ERROR_RETURN
                (_wc40_rxaui_config(unit,pc,FALSE));

            /* set default clock comp clear the configuration for 20X XE combo mode */
            /* don't change  if clock comp is enabled by user */
            if (!(DEV_CTRL_PTR(pc)->clk_comp)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00,
                          COMBO_HG_XGXS_nLQnCC, 
                          XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_MASK));
            }

            if (!(WC40_REVID_A0(pc) || WC40_REVID_A1(pc))) {
                /* clear 42G control, PLL auto tune */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00,
                           0, 
                           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK));
            }

            /* only in combo mode, reset the sequence. In independent mode,
             * resetting sequence affects all lanes
             */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, 0,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

            if (!WC40_CL73_SOFT_KR2(pc)) {
                /* Enable 1000X parallel detect */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0x00, 
                      SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK, 
                      SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));

                /* enable 10G parallel detect */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(unit, pc, 0x00,
                          XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK, 
                          XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));
            }
        }
        if (!WC40_CL73_SOFT_KR2(pc)) {
    
            if(DEV_CFG_PTR(pc)->cl37an == WC40_CL37_AND_CL37BAM) {
                /* set BAM/TETON enable */
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(unit,pc,0x00,
                       DIGITAL6_MP5_NEXTPAGECTRL_BAM_MODE_MASK |
                       DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_MASK |
                       DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_UP3_EN_MASK));
            } else {
                /* Disable Clause 37 BAM. */
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(unit,pc,0x00, 0));
            }

            if(DEV_CFG_PTR(pc)->cl37an) {
                /* enable autoneg */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, MII_CTRL_AE,
                                              MII_CTRL_AE));
            }

            if (DEV_CFG_PTR(pc)->cl73an) {
                /*To address the Synchronization issue related to a registered clock gate 
                  module responsible for providing the 312.5MHz write clock to the TX 
                  over-sample FIFO. Resetting the FIFO to fix the issue.
                */  
                if (!COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)
                       && !DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status) {
                    SOC_IF_ERROR_RETURN
                        (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, 0x00,
                                              DIGITAL5_MISC6_TX_OS8_AFRST_EN_MASK,
                                             DIGITAL5_MISC6_TX_OS8_AFRST_EN_MASK));
                }

                /* CL73 BAM setup, revisit BAMNP_AFTER_BP bit, any conflict
                 * with EEE page?
                 */
                mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK |
                     CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK |
                     CL73_USERB0_CL73_BAMCTRL1_CL73_BAMNP_AFTER_BP_EN_MASK;
                /* not to enable CL73 BAM in independent channel mode:
                 * has problem with 8073 phy
                 */
                if (DEV_CFG_PTR(pc)->cl73an == WC40_CL73_WO_CL73BAM) {
                    /* Disable Clause 37 BAM. */
                    SOC_IF_ERROR_RETURN
                        (WRITE_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(unit,pc,0x00, 0));

                    /* If Individual lane mode and tx_fifo_reset is enable,
                       skip for firmware to manage it.
                     */
                    if(!(IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)
                         && DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status)) {
                    SOC_IF_ERROR_RETURN
                            (MODIFY_WC40_CL73_USERB0_CL73_UCTRL2r(unit, pc, 
                                0x00,
                                          CL73_USERB0_CL73_UCTRL2_CONFIG_MATCH_DIS_MASK
                                            | CL73_USERB0_CL73_UCTRL2_SIGDET_TIMER_DIS_MASK,
                                          CL73_USERB0_CL73_UCTRL2_CONFIG_MATCH_DIS_MASK
                                            | CL73_USERB0_CL73_UCTRL2_SIGDET_TIMER_DIS_MASK));
                    }

                    /* need to enable these two bits even BAM is disabled 
                       to interop with other BRCM devices properly. 
                     */
                    data16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK |
                             CL73_USERB0_CL73_BAMCTRL1_CL73_BAMNP_AFTER_BP_EN_MASK;
                } else {
                    data16 = mask16;
                }

                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(unit, pc, 0x00, 
                           data16, mask16));

                /* os8_txfifo_patch_en[3:0], which is a per-lane control 
                   bit to enable the war_tx_os8_fifo_reset routine via s/w; 

                   MDIO Address: 0xFFC8.3:0 = mdio_uC_mailbox[3:0] 
                                                = txfifo_patch_en[3:0]
                   where txfifo_p/RECiu/atch_en[N] controls lane(N).
                   Default: 0xFFC8.3:0 = txfifo_patch_en[3:0] = 4'h0
                  
                   S/W need to enable firmware WAR before enabling CL73

                */
                if((DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status == ENABLE) &&
                    IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)){
                    mask16 = 1 << pc->lane_num;
                    data16 = mask16;
                    SOC_IF_ERROR_RETURN
                        (MODIFY_WC40_MICROBLK_MDIO_UC_MAILBOXr(unit, pc, 0x0,
                                                            data16, mask16));
                    DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status = ACTIVE;
                }
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 0x00, 
                                          MII_CTRL_AE,
                                          MII_CTRL_AE));
            }
        }
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
            /* restart the sequence */
            SOC_IF_ERROR_RETURN
                 (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, 
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
            /* wait for PLL lock */
            _phy_wc40_pll_lock_wait(unit, port);
            _phy_wc40_control_vco_disturbed_set(unit, port);
        }
    } else {
        if (IS_DUAL_LANE_PORT(pc)) {
            if (WC40_REVID_B(pc)) {
               SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL49_USERB0_CONTROLr(unit, pc,0x00,
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_VALUE_MASK |
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_CTRL_MASK,
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_VALUE_MASK |
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_CTRL_MASK));
               SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_CL49_USERB0_CONTROLr(unit, pc,
                      ln_access[pc->lane_num+1],
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_VALUE_MASK |
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_CTRL_MASK,
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_VALUE_MASK |
                      CL49_USERB0_CONTROL_CL82_CKGATE_EN_OVR_CTRL_MASK));
               SOC_IF_ERROR_RETURN
                   (MODIFY_WC40_RX66_CONTROLr(unit, pc, 0x00,
                    0,
                    RX66_CONTROL_CC_EN_MASK | RX66_CONTROL_CC_DATA_SEL_MASK));
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit, pc, LANE0_ACCESS,
                       0, (1 << 13)));
        }

        /* Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, pc, 0x00, auto_det,
                                 SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, 0,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

            /* disable 1000X parallel detect */
            SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0x00,
                  0,
                  SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
                                                                                
            /* disable 10G parallel detect */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(unit, pc, 0x00,
                      0,
                      XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));
        }

        SOC_IF_ERROR_RETURN
            (WRITE_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(unit,pc,0x00,0));

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, 0,
                                         MII_CTRL_AE));
        if (DEV_CFG_PTR(pc)->cl73an) {
           if (!COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)
                  && !DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL5_MISC6r(unit, pc, 0x00,
                                          0,
                                         DIGITAL5_MISC6_TX_OS8_AFRST_EN_MASK));
            } 
            /* 
               S/W need to disable TX FIFO RESET firmware WAR 
               before disabling CL73.
            */
            if((DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status == ACTIVE) &&
                IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)){
                mask16 = 1 << pc->lane_num;
                data16 = 0;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_MICROBLK_MDIO_UC_MAILBOXr(unit, pc, 0x0,
                                                           data16, mask16));
                DEV_CFG_PTR(pc)->cl73an_tx_fifo_reset_status = ENABLE;
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 0x00, 
                                          0,
                                          MII_CTRL_AE));
        }

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            SOC_IF_ERROR_RETURN
                 (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, 
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
            /* wait for PLL lock */
            _phy_wc40_pll_lock_wait(unit, port);
            _phy_wc40_control_vco_disturbed_set(unit, port);
        }
    }

    if (WC40_CL73_SOFT_KR2(pc)) {
        if ((DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2)
           || (DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_WAR)) {
            SOC_IF_ERROR_RETURN
                (_wc40_soft_an_cl73kr2(unit,port,an));
        } else if ((DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_CUSTOM)
                || (DEV_CFG_PTR(pc)->cl73an == WC40_CL73_KR2_CUSTOM_WAR)) {
            SOC_IF_ERROR_RETURN
                (_wc40_soft_an_cl73kr2_custom(unit,port,an));
        }
    } else {
        /* restart the autoneg if enabled, or disable the autoneg */
        if(DEV_CFG_PTR(pc)->cl37an) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, an_enable,
                                          MII_CTRL_AE | MII_CTRL_RAN));
        }

        if (DEV_CFG_PTR(pc)->cl73an) { 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 0x00, an_enable,
                                          MII_CTRL_AE | MII_CTRL_RAN));
        }
    }

    pc->fiber.autoneg_enable = an;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_wc40_an_get
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

STATIC int
phy_wc40_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      mii_ctrl;
    uint16      mii_stat;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    if (AUTONEG_MODE_UNAVAIL(pc)) {
        *an = FALSE;
        *an_done = FALSE;
        return SOC_E_NONE;
    }

    if (WC40_CL73_SOFT_KR2(pc)) {
        *an = FALSE;
        *an_done = FALSE;
    } else {        
        SOC_IF_ERROR_RETURN
            (READ_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, &mii_ctrl));
        SOC_IF_ERROR_RETURN
            (READ_WC40_GP2_REG_GP2_4r(unit, pc, LANE0_ACCESS, &mii_stat)); 
        
        *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;
        *an_done = (mii_stat & (1 << (pc->lane_num + GP2_REG_GP2_4_AUTONEG_COMPLETE_SHIFT))) ? TRUE: FALSE;  
    }

    if (!((*an == TRUE) && (*an_done == TRUE))) {
        if (DEV_CFG_PTR(pc)->cl73an) { 
            /* check clause 73 */
            SOC_IF_ERROR_RETURN
                (READ_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 0x00, &mii_ctrl));

            /* make sure it is autoneged link. don't use IEEE0BLK_AN_IEEESTATUS1r, 
             * read by soft kr2 
             */
            SOC_IF_ERROR_RETURN
                (READ_WC40_GP2_REG_GP2_1r(unit, pc, LANE0_ACCESS, &mii_stat));

            *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;
            *an_done = (GP2_REG_GP2_1_LINK_AN_MASK & mii_stat & (1 << pc->lane_num)) ?
                       TRUE : FALSE;
        }
    }
    return SOC_E_NONE; 
}

STATIC int
_phy_wc40_c73_adv_local_set(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    uint16            pause;
#ifdef WC_EEE_SUPPORT
    uint16            mask16;
    uint16            data16;
#endif
    phy_ctrl_t       *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    if (WC40_CL73_SOFT_KR2(pc)) {
        if (ability->speed_full_duplex & SOC_PA_SPEED_20GB) {
            DEV_CTRL_PTR(pc)->soft_kr2.ld_kr2 = 1;
        } else {
            DEV_CTRL_PTR(pc)->soft_kr2.ld_kr2 = 0;
        }
    }

    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 
              CL73_AN_ADV_TECH_1G_KX : 0;

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ? 
               CL73_AN_ADV_TECH_10G_KR: 0;
    }

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ? 
              CL73_AN_ADV_TECH_10G_KX4: 0;
    }    

    an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_40GB) ?
          CL73_AN_ADV_TECH_40G_KR4: 0;
    if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_CR4) {
        an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_40GB) ?
              CL73_AN_ADV_TECH_40G_CR4: 0;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(unit, pc, 0x00, an_adv,
                                     CL73_AN_ADV_TECH_SPEEDS_MASK));

#ifdef WC_EEE_SUPPORT
    /* EEE advertisement 0x3c: 
     * 06 RW EEE_10G_KR 1 = EEE is supported for 10GBASE-KR.
     *                  0 = EEE is not supported for 10GBASE-KR. 
     * 05 RW EEE_10G_KX4 1 = EEE is supported for 10GBASE-KX4.
     *                   0 = EEE is not supported for 10GBASE-KX4. 
     * 04 RW EEE_1G_KX 1 = EEE is supported for 1000BASE-KX.
     *                 0 = EEE is not supported for 1000BASE-KX.
     * XXX assume supporting all speeds. use PA_EEE 
     */
    mask16 = AN_IEEE3BLK_EEE_ADV_EEE_10G_KR_MASK |
             AN_IEEE3BLK_EEE_ADV_EEE_10G_KX4_MASK |
             AN_IEEE3BLK_EEE_ADV_EEE_1G_KX_MASK;
    data16 = (ability->eee & SOC_PA_EEE_10GB_KX) ? 
              AN_IEEE3BLK_EEE_ADV_EEE_1G_KX_MASK : 0;
    data16 |= (ability->eee & SOC_PA_EEE_10GB_KX4) ? 
              AN_IEEE3BLK_EEE_ADV_EEE_10G_KX4_MASK:0;
    data16 |= (ability->eee & SOC_PA_EEE_10GB_KR) ? 
              AN_IEEE3BLK_EEE_ADV_EEE_10G_KR_MASK: 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE3BLK_EEE_ADVr(unit, pc, 0x00, data16, mask16));
#endif

    /* CL73 UD_code_field */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL3r(unit,pc,0x00,1,
                CL73_USERB0_CL73_BAMCTRL3_UD_CODE_FIELD_MASK));

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        pause = CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE;
        break;
    default:
        pause = 0;
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit, pc, 0x00, pause,
                                     (CL73_AN_ADV_PAUSE |
                                      CL73_AN_ADV_ASYM_PAUSE)));
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_c73_adv_local_set: u=%d p=%d pause=%08x speeds=%04x,adv=0x%x\n"),
              unit, port, pause, an_adv,ability->speed_full_duplex));
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_c73_adv_local_get(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   speeds,pause;
    phy_ctrl_t       *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(unit, pc, 0x00, &an_adv));

    speeds = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_10G_KR) ? SOC_PA_SPEED_10GB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_40G_KR4) ? SOC_PA_SPEED_40GB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_40G_CR4) ? SOC_PA_SPEED_40GB : 0;

    if (WC40_CL73_SOFT_KR2(pc)) {
        speeds |= DEV_CTRL_PTR(pc)->soft_kr2.ld_kr2? SOC_PA_SPEED_20GB:0;
    }

    ability->speed_full_duplex |= speeds;
#ifdef WC_EEE_SUPPORT
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE3BLK_EEE_ADVr(unit, pc, 0x00, &an_adv));
    ability->eee = (an_adv & AN_IEEE3BLK_EEE_ADV_EEE_10G_KR_MASK)?
                   SOC_PA_EEE_10GB_KR:0;
    ability->eee |= (an_adv & AN_IEEE3BLK_EEE_ADV_EEE_10G_KX4_MASK)?
                    SOC_PA_EEE_10GB_KX4:0;
    ability->eee |= (an_adv & AN_IEEE3BLK_EEE_ADV_EEE_1G_KX_MASK)?
                    SOC_PA_EEE_10GB_KX:0;
#endif

    /* advert register 0x12, bit 15 FEC requested,bit 14 FEC ability */
    /* SOC_IF_ERROR_RETURN
     *    (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, pc, 0x00, &an_adv));
     */
    /* (an_adv & AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK) */

    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit, pc, 0x00, &an_adv));

    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_RX;
            break;
        default:
            pause = 0;
            break;
    }
    ability->pause = pause;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_c73_adv_local_get: u=%d p=%d pause=%08x speeds=%04x\n"),
              unit, port, pause, speeds));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_ability_advert_set
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
phy_wc40_ability_advert_set(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
    uint16           an_adv = 0;
    uint16           an_sp_20g;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;
    uint16 mask16;
    uint16 data16;
    uint16 up3;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    /* If the phy medium is copper, only set MII speed and advertised duplex. */
    if (!DEV_CFG_PTR(pc)->fiber_pref) {
        an_adv = 0;
        if (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) {
            an_adv = MII_CTRL_SS_1000;
        } else if (ability->speed_full_duplex & SOC_PA_SPEED_100MB) {
            an_adv = MII_CTRL_SS_100;
        } else if (ability->speed_full_duplex & SOC_PA_SPEED_10MB) {
            an_adv = MII_CTRL_SS_10;
        }

        an_adv |= ability->speed_full_duplex ? (MII_CTRL_FD | MII_CTRL_AE) : 0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x0, an_adv,
                MII_CTRL_AE | MII_CTRL_FD | MII_CTRL_SS_LSB | MII_CTRL_SS_MSB));

        return SOC_E_NONE;
    }

    /*
     * Set advertised duplex (only FD supported) if CL37 is enabled
     */
    if (DEV_CFG_PTR(pc)->cl37an) { 
        an_adv = ability->speed_full_duplex? MII_ANA_C37_FD : 0;

        /*
         * Set advertised pause bits in link code word.
         */
        switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
        case SOC_PA_PAUSE_TX:
            an_adv |= MII_ANA_C37_ASYM_PAUSE;
            break;
        case SOC_PA_PAUSE_RX:
            an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE;
            break;
        case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
            an_adv |= MII_ANA_C37_PAUSE;
            break;
        }
    }
    /* Update less than 1G capability */ 
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_COMBO_IEEE0_AUTONEGADVr(unit, pc, 0x00, an_adv));

    mode = ability->speed_full_duplex;
    an_adv = 0;
    an_sp_20g = 0;
    an_adv |= (mode & SOC_PA_SPEED_2500MB) ? 
               DIGITAL3_UP1_DATARATE_2P5GX1_MASK : 0;

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_DIGITAL3_UP1r(unit, pc, 0x00, an_adv));

        SOC_IF_ERROR_RETURN
            (WRITE_WC40_DIGITAL3_UP3r(unit, pc, 0x00, 
                          DIGITAL3_UP3_CL72_MASK | DIGITAL3_UP3_LAST_MASK)); 

        if (DEV_CFG_PTR(pc)->cl73an) { 
            SOC_IF_ERROR_RETURN
                (_phy_wc40_c73_adv_local_set(unit, port, ability));
        }

        /*next need to check FEC ability */
        mask16 = AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK;
        data16 = 0;
        if (ability->fec == SOC_PA_FEC_CL74) {
            data16 = mask16;
            up3 = DIGITAL3_UP3_FEC_MASK;
        } else {
            data16 = 1 << AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_SHIFT;
            up3 = 0;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, pc, 0x00, data16, mask16));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL3_UP3r(unit, pc, 0x00, up3, DIGITAL3_UP3_FEC_MASK));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_wc40_ability_advert_set: u=%d p=%d pause=%08x OVER1G_UP1 %04x\n"),
                  unit, port, ability->pause, an_adv));
        return SOC_E_NONE;
    }

    /* high speeds, 40G,30G and 20G, 20G/scrambler */ 
    an_adv |= (mode & SOC_PA_SPEED_5000MB) ? 
                DIGITAL3_UP1_DATARATE_5GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_6000MB) ? 
                DIGITAL3_UP1_DATARATE_6GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_12GB) ? 
                DIGITAL3_UP1_DATARATE_12GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_12P5GB) ? 
                DIGITAL3_UP1_DATARATE_12P5GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_13GB) ? 
                    DIGITAL3_UP1_DATARATE_13GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_15GB) ? 
                DIGITAL3_UP1_DATARATE_15GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_16GB) ? 
                DIGITAL3_UP1_DATARATE_16GX4_MASK : 0;
    an_adv |= (mode & SOC_PA_SPEED_20GB) ? 
              DIGITAL3_UP1_DATARATE_20GX4_MASK : 0;
    /* advertise 20G with scrambler */
    if (mode & SOC_PA_SPEED_20GB) {
        data16 = DIGITAL6_UP4_DATARATE_20G_MASK;
    } else {
        data16 = 0;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL6_UP4r(unit, pc, 0x00, 
                 data16,
                 DIGITAL6_UP4_DATARATE_20G_MASK));
    
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL6_UP4r(unit, pc, 0x00, 
                 DIGITAL6_UP4_LAST_MASK,
                 DIGITAL6_UP4_LAST_MASK));

    an_sp_20g |= (mode & SOC_PA_SPEED_21GB) ? 
                  DIGITAL3_UP3_DATARATE_21GX4_MASK : 0;
    an_sp_20g |= (mode & SOC_PA_SPEED_25GB) ? 
                  DIGITAL3_UP3_DATARATE_25P45GX4_MASK : 0;
    an_sp_20g |= (mode & SOC_PA_SPEED_30GB) ? 
                  DIGITAL3_UP3_DATARATE_31P5G_MASK : 0;

    if (DEV_CFG_PTR(pc)->hg_mode) {
        an_sp_20g |= (mode & SOC_PA_SPEED_40GB) ? 
                  DIGITAL3_UP3_DATARATE_40G_MASK : 0;
    }

    if (mode & SOC_PA_SPEED_10GB) {
        if (DEV_CFG_PTR(pc)->hg_mode) {
            /* For Draco and Hercules, use pre-CX4 signalling */
            an_adv |= DIGITAL3_UP1_DATARATE_10GX4_MASK; 
            if (DEV_CFG_PTR(pc)->cx4_10g) {
                /* Also include 10G CX4 signalling by default */
                an_adv |= DIGITAL3_UP1_DATARATE_10GCX4_MASK; 
            }
        } else {
            an_adv |= DIGITAL3_UP1_DATARATE_10GCX4_MASK; 
        }
    }
    
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_DIGITAL3_UP1r(unit, pc, 0x00, an_adv));

    /* CL72 and Higig2, check Higig2 cap on this port before adv 
     * UP3 last page bit DIGITAL3_UP3_LAST_MASK needs to be set to work with HC/HL.
     */
    mask16 = OVER1G_UP3_20GPLUS_MASK;
    data16 = an_sp_20g;
    mask16 |= DIGITAL3_UP3_CL72_MASK | DIGITAL3_UP3_HIGIG2_MASK;

    /* 20G with CL72 advertised in WC B0 doesn't work. But cl72 should not
     * be advertised below 21G anyway
     */ 
    if (!(DEV_CFG_PTR(pc)->hg_mode &&
        (!(mode & (SOC_PA_SPEED_40GB | SOC_PA_SPEED_21GB |
                 SOC_PA_SPEED_25GB | SOC_PA_SPEED_30GB)))) ) {
        data16 |= DIGITAL3_UP3_CL72_MASK;
    }
#if 0 
    data16 |= DIGITAL3_UP3_FEC_MASK;
#endif

    /* XXX revisit: no autoneg in dxgxs mode */
    
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL3_UP3r(unit, pc, 0x00, data16, mask16));

    /* preemphasis attributes */
    /*
     * mask16 = DIGITAL3_UP2_PREEMPHASIS_MASK | DIGITAL3_UP2_IDRIVER_MASK |
     *          DIGITAL3_UP2_IPREDRIVER_MASK | DIGITAL3_UP2_VALID_MASK;
     * data16 = (DEV_CFG_PTR(pc)->preemph << DIGITAL3_UP2_PREEMPHASIS_SHIFT) |
     *          (DEV_CFG_PTR(pc)->idriver << DIGITAL3_UP2_IDRIVER_SHIFT) |
     *          (DEV_CFG_PTR(pc)->pdriver << DIGITAL3_UP2_IPREDRIVER_SHIFT);
     * data16 |= DIGITAL3_UP2_VALID_MASK;
     * SOC_IF_ERROR_RETURN
     *     (MODIFY_WC40_DIGITAL3_UP2r(unit, pc, 0x00, data16,mask16));
     */

    if (DEV_CFG_PTR(pc)->cl73an) { 
        SOC_IF_ERROR_RETURN
            (_phy_wc40_c73_adv_local_set(unit, port, ability));
    }

    /*next need to check FEC ability */
    mask16 = AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK;
    data16 = 0;
    if (ability->fec == SOC_PA_FEC_CL74) {
        data16 = mask16;
        up3 = DIGITAL3_UP3_FEC_MASK;
    } else {
        data16 = 1 << AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_SHIFT;
        up3 = 0;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, pc, 0x00, data16, mask16));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL3_UP3r(unit, pc, 0x00, up3, DIGITAL3_UP3_FEC_MASK));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_wc40_ability_advert_set: u=%d p=%d pause=%08x OVER1G_UP1 %04x\n"),
              unit, port, ability->pause, an_adv));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_ability_advert_get
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
phy_wc40_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
    uint16           an_adv;
    uint16           up4;
    uint16           up3;
    uint16           up2;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;
    soc_port_mode_t  fec;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    WC40_MEM_SET(ability, 0, sizeof(*ability));

    if (!DEV_CFG_PTR(pc)->fiber_pref) {
        SOC_IF_ERROR_RETURN
          (READ_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, &an_adv));
        mode = 0;
        mode = (an_adv & MII_CTRL_SS_MSB) ? SOC_PA_SPEED_1000MB :
               (an_adv & MII_CTRL_SS_LSB) ? SOC_PA_SPEED_100MB :
                SOC_PA_SPEED_10MB;
        ability->speed_full_duplex = mode;

        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_DIGITAL3_UP1r(unit, pc, 0x00, &an_adv));

    SOC_IF_ERROR_RETURN
        (READ_WC40_DIGITAL3_UP2r(unit, pc, 0x00, &up2));

    SOC_IF_ERROR_RETURN
        (READ_WC40_DIGITAL3_UP3r(unit, pc, 0x00, &up3));

    SOC_IF_ERROR_RETURN
        (READ_WC40_DIGITAL6_UP4r(unit, pc, 0x00, &up4));

    /* preemphasis settings */
    /* preemphasis (up2 & DIGITAL3_UP2_PREEMPHASIS_MASK) >>
     *              DIGITAL3_UP2_PREEMPHASIS_SHIFT
     * idriver     (up2 & DIGITAL3_UP2_IDRIVER_MASK) >>
     *              DIGITAL3_UP2_IDRIVER_SHIFT
     * ipredriver  (up2 & DIGITAL3_UP2_IPREDRIVER_MASK) >>
     *              DIGITAL3_UP2_IPREDRIVER_SHIFT
     */
    /* CL72,FEC and Higig2 
     * CL72 cap - (up3 & DIGITAL3_UP3_CL72_MASK)
     * FEC cap  - (up3 & DIGITAL3_UP3_FEC_MASK)
     * Higig2   - (up3 & DIGITAL3_UP3_HIGIG2_MASK)
     */
    ability->encap = (up3 & DIGITAL3_UP3_HIGIG2_MASK)?
                      SOC_PA_ENCAP_HIGIG2: 0;
    
    mode = 0;
    mode |= (up3 & DIGITAL3_UP3_DATARATE_40G_MASK) ?
              SOC_PA_SPEED_40GB : 0;
    mode |= (up3 & DIGITAL3_UP3_DATARATE_31P5G_MASK) ?
              SOC_PA_SPEED_30GB : 0;
    mode |= (up3 & DIGITAL3_UP3_DATARATE_25P45GX4_MASK) ?
              SOC_PA_SPEED_25GB : 0;
    mode |= (up3 & DIGITAL3_UP3_DATARATE_21GX4_MASK) ?
              SOC_PA_SPEED_21GB : 0;

    mode |= (up4 & DIGITAL6_UP4_DATARATE_20G_MASK) ? SOC_PA_SPEED_20GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_20GX4_MASK) ? SOC_PA_SPEED_20GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_16GX4_MASK) ? SOC_PA_SPEED_16GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_15GX4_MASK) ? SOC_PA_SPEED_15GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_13GX4_MASK) ? SOC_PA_SPEED_13GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_12P5GX4_MASK) ? 
            SOC_PA_SPEED_12P5GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_12GX4_MASK) ? SOC_PA_SPEED_12GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_10GCX4_MASK) ? SOC_PA_SPEED_10GB: 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_10GX4_MASK) ? SOC_PA_SPEED_10GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_6GX4_MASK)? SOC_PA_SPEED_6000MB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_5GX4_MASK)? SOC_PA_SPEED_5000MB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_2P5GX1_MASK) ? 
                    SOC_PA_SPEED_2500MB : 0;

    SOC_IF_ERROR_RETURN
        (READ_WC40_COMBO_IEEE0_AUTONEGADVr(unit, pc, 0x00, &an_adv));
      
    mode |= (an_adv & MII_ANA_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
    ability->speed_full_duplex = mode;

    mode = 0;
    switch (an_adv & (MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE)) {
        case MII_ANA_C37_PAUSE:
            mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANA_C37_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_TX;
            break;
        case MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    /* check for clause73 */
    if (DEV_CFG_PTR(pc)->cl73an) {
        SOC_IF_ERROR_RETURN
            (_phy_wc40_c73_adv_local_get(unit, port, ability));
    }

    /* Check FEC base on AN and CL37BAM*/
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, pc, 0x00, &an_adv));
    if ((an_adv & AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK) ==
                    AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK) {
        fec = SOC_PA_FEC_CL74;
    } else if (an_adv & (1<<AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_SHIFT)) {
        fec = SOC_PA_FEC_NONE;
    } else {
        fec = 0;
    }

    fec |= (up3 & DIGITAL3_UP3_FEC_MASK) ? SOC_PA_FEC_CL74 : SOC_PA_FEC_NONE;

    if (fec & SOC_PA_FEC_CL74) {
        ability->fec = SOC_PA_FEC_CL74;
    } else {
        ability->fec = SOC_PA_FEC_NONE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_wc40_ability_advert_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return SOC_E_NONE;
}

STATIC int
_phy_wc40_c73_adv_remote_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   mode;
    phy_ctrl_t       *pc;

    pc = INT_PHY_SW_STATE(unit, port);
     SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY1r(unit, pc, 0x00, &an_adv));

    mode =  (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_10G_KR) ? SOC_PA_SPEED_10GB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_40G_KR4) ? SOC_PA_SPEED_40GB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_40G_CR4) ? SOC_PA_SPEED_40GB : 0;

    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2r(unit, pc, 0x00, &an_adv));
    mode |= (an_adv & CL73_AN_ADV_TECH_20G_KR2) ? SOC_PA_SPEED_20GB : 0;

    ability->speed_full_duplex |= mode;

    /* advert register 0x12, bit 15 FEC requested,bit 14 FEC ability */
    /* SOC_IF_ERROR_RETURN
     *   (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2r(unit, pc, 0x00, &an_adv));
     */
    /* (an_adv & AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK) */


    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY0r(unit, pc, 0x00, &an_adv));

    mode = 0;
    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_ability_remote_get
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
phy_wc40_ability_remote_get(int unit, soc_port_t port,
                        soc_port_ability_t *ability)
{
    uint16           an_adv;
    uint16           up4 = 0;
    uint16           up3 = 0;
    uint16           up2 = 0;
    uint16           data16;
    uint16           mii_ctrl;
    int              an_enable;
    int              link_1000x = FALSE;
    int              link_combo = FALSE;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;
    soc_port_mode_t  fec;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, &data16));

    /* check clause 73 */
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 0x00, &mii_ctrl));

    an_enable = ((data16 & MII_CTRL_AE) || (mii_ctrl & MII_CTRL_AE)) ? TRUE : FALSE;

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_XGXSBLK4_XGXSSTATUS1r(unit, pc, 0x00, &data16));
        if (data16 & (XGXSBLK4_XGXSSTATUS1_LINKSTAT_MASK |
                               XGXSBLK4_XGXSSTATUS1_LINK10G_MASK)) {
            link_combo = TRUE;
        }
    } else {
        SOC_IF_ERROR_RETURN
            (READ_WC40_GP2_REG_GP2_1r(unit, pc, 0x00, &data16));
        if (data16 & (1 << pc->lane_num)) {
            link_1000x = TRUE;
        }
    } 

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "u=%d p=%d an_enable=%04x link_1000x=%04x link_combo=%04x\n"),
              unit, port, an_enable, link_1000x,link_combo));

    WC40_MEM_SET(ability, 0, sizeof(*ability));
    mode = 0;

    if (an_enable && (link_1000x || link_combo)) {
        /* Decode remote advertisement only when link is up and autoneg is 
         * completed.
         */
 
        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL3_LP_UP1r(unit, pc, 0x00, &an_adv));

        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL3_LP_UP2r(unit, pc, 0x00, &up2));

        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL3_LP_UP3r(unit, pc, 0x00, &up3));

        SOC_IF_ERROR_RETURN
            (READ_WC40_DIGITAL5_LP_UP4r(unit, pc, 0x00, &up4));

        /* preemphasis settings */
        /* preemphasis (up2 & DIGITAL3_UP2_PREEMPHASIS_MASK) >>
         *              DIGITAL3_UP2_PREEMPHASIS_SHIFT
         * idriver     (up2 & DIGITAL3_UP2_IDRIVER_MASK) >>
         *              DIGITAL3_UP2_IDRIVER_SHIFT
         * ipredriver  (up2 & DIGITAL3_UP2_IPREDRIVER_MASK) >>
         *              DIGITAL3_UP2_IPREDRIVER_SHIFT
         */
        /* CL72,FEC and Higig2 
         * CL72 cap - (up3 & DIGITAL3_UP3_CL72_MASK)
         * FEC cap  - (up3 & DIGITAL3_UP3_FEC_MASK)
         * Higig2   - (up3 & DIGITAL3_UP3_HIGIG2_MASK)
         */
        ability->encap = (up3 & DIGITAL3_UP3_HIGIG2_MASK)?
                      SOC_PA_ENCAP_HIGIG2: 0;

        mode |= (up3 & DIGITAL3_LP_UP3_DATARATE_40G_MASK)?
                SOC_PA_SPEED_40GB: 0;
        mode |= (up3 & DIGITAL3_LP_UP3_DATARATE_31P5G_MASK)?
                SOC_PA_SPEED_30GB: 0;
        mode |= (up3 & DIGITAL3_LP_UP3_DATARATE_25P45GX4_MASK)?
                SOC_PA_SPEED_25GB: 0;
        mode |= (up3 & DIGITAL3_LP_UP3_DATARATE_21GX4_MASK)?
                SOC_PA_SPEED_21GB: 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_20GX4_MASK) ? 
                SOC_PA_SPEED_20GB : 0;
        mode |= (up4 & DIGITAL5_LP_UP4_DATARATE_20G_MASK) ? 
                SOC_PA_SPEED_20GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_16GX4_MASK) ? 
                SOC_PA_SPEED_16GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_15GX4_MASK) ? 
                SOC_PA_SPEED_15GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_13GX4_MASK) ? 
                SOC_PA_SPEED_13GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_12P5GX4_MASK) ? 
                SOC_PA_SPEED_12P5GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_12GX4_MASK) ? 
                SOC_PA_SPEED_12GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_10GCX4_MASK) ? 
                SOC_PA_SPEED_10GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_10GX4_MASK) ? 
                SOC_PA_SPEED_10GB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_6GX4_MASK) ?
                 SOC_PA_SPEED_6000MB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_5GX4_MASK) ?
                 SOC_PA_SPEED_5000MB : 0;
        mode |= (an_adv & DIGITAL3_LP_UP1_DATARATE_2P5GX1_MASK) ?
                 SOC_PA_SPEED_2500MB : 0;

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d over1G an_adv=%04x\n"),
                  unit, port, an_adv));

        SOC_IF_ERROR_RETURN
            (READ_WC40_COMBO_IEEE0_AUTONEGLPABILr(unit, pc, 0x00, &an_adv));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d combo an_adv=%04x\n"),
                  unit, port, an_adv));

        mode |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
        ability->speed_full_duplex = mode;
          
        switch (an_adv & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
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
        if (DEV_CFG_PTR(pc)->cl73an) {
            /* make sure it is autoneged link, don't use IEEE0BLK_AN_IEEESTATUS1r,
             * read by soft kr2
             */
            SOC_IF_ERROR_RETURN
                (READ_WC40_GP2_REG_GP2_1r(unit, pc, LANE0_ACCESS, &data16));
            if (GP2_REG_GP2_1_LINK_AN_MASK & data16 & (1 << pc->lane_num)) {
                /* Only read when link partner has/enable CL73 ability. */
                SOC_IF_ERROR_RETURN
                    (READ_WC40_GP2_REG_GP2_4r(unit, pc, LANE0_ACCESS, &data16)); 
                if (GP2_REG_GP2_4_CL73_MR_LP_AUTONEG_ABLE_MASK & data16 & 
                    (1 << (pc->lane_num + GP2_REG_GP2_4_CL73_MR_LP_AUTONEG_ABLE_SHIFT))) {
                    SOC_IF_ERROR_RETURN
                        (_phy_wc40_c73_adv_remote_get(unit, port, ability));
                }
            }
        }
    } else {
        /* Simply return local abilities */
        phy_wc40_ability_advert_get(unit, port, ability);
    }

    /* Check FEC base on AN and CL37BAM*/
    SOC_IF_ERROR_RETURN
        (READ_WC40_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2r(unit, pc, 0x00, &an_adv));
    if ((an_adv & AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2_FEC_REQUESTED_MASK) ==
                    AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2_FEC_REQUESTED_MASK) {
        fec = SOC_PA_FEC_CL74;
    } else if (an_adv & (1<<AN_IEEE1BLK_AN_LP_BASEPAGEABILITY2_FEC_REQUESTED_SHIFT)) {
        fec = SOC_PA_FEC_NONE;
    } else {
        fec = 0;
    }

    fec |= (up3 & DIGITAL3_LP_UP3_FEC_MASK) ? SOC_PA_FEC_CL74 : SOC_PA_FEC_NONE;

    if (fec & SOC_PA_FEC_CL74) {
        ability->fec = SOC_PA_FEC_CL74;
    } else {
        ability->fec = SOC_PA_FEC_NONE;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_wc40_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_lb_set
 * Purpose:
 *      Put hc/FusionCore in PHY loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wc40_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;

    uint16 lane_mask;
    uint16 lane;
    uint16 data16;

    pc = INT_PHY_SW_STATE(unit, port);

    if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc) || WC40_REVID_B(pc)) {
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK1_LANETESTr(unit,pc,0x00,
                     enable? XGXSBLK1_LANETEST_PWRDN_SAFE_DIS_MASK: 0,
                     XGXSBLK1_LANETEST_PWRDN_SAFE_DIS_MASK));
        }
    }

    /* Use register 0x8017 bit 3-0 gloop, set 0x8000 bit4 for all modes */
     
    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        if (IS_DUAL_LANE_PORT(pc)) {
            lane_mask = 1 << pc->lane_num;
            lane_mask |= (lane_mask << 1);
        } else {
            lane_mask = 1 << pc->lane_num;
        }       
    } else {  /* combo mode */
            lane_mask = XGXSBLK1_LANECTRL2_GLOOP1G_MASK;
    }
    if (enable) {
        lane = lane_mask;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit,pc,0x00,
                 XGXSBLK0_XGXSCONTROL_MDIO_CONT_EN_MASK,
                 XGXSBLK0_XGXSCONTROL_MDIO_CONT_EN_MASK));

        if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            /* reverse flipped TX/RX polarities if any*/
            SOC_IF_ERROR_RETURN
                (_phy_wc40_ind_lane_polarity_set(unit,pc,FALSE));
        } else {
            /* reverse flipped Tx/Rx Polarity  */
            SOC_IF_ERROR_RETURN
                (_phy_wc40_combo_polarity_set(unit,pc,FALSE));
        }
    } else { 
        if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            /* reverse flipped TX/RX polarities if any*/
            SOC_IF_ERROR_RETURN
                (_phy_wc40_ind_lane_polarity_set(unit,pc, TRUE));
        } else {
            /* reverse flipped Tx/Rx Polarity  */
            SOC_IF_ERROR_RETURN
                (_phy_wc40_combo_polarity_set(unit,pc,TRUE));
        }
        SOC_IF_ERROR_RETURN
            (READ_WC40_XGXSBLK1_LANECTRL2r(unit,pc,0x00,&data16));
        lane = ~lane_mask;
        lane &= data16;
        lane &= XGXSBLK1_LANECTRL2_GLOOP1G_MASK; 
        if (!lane) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit,pc,0x00,
                     0,
                     XGXSBLK0_XGXSCONTROL_MDIO_CONT_EN_MASK));
        }
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK1_LANECTRL2r(unit, pc, 0x00, lane, lane_mask));
  
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_lb_get
 * Purpose:
 *      Get hc/FusionCore PHY loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_wc40_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc;

    uint16 lane;
    uint16 lane_mask;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_WC40_XGXSBLK1_LANECTRL2r(unit,pc,0x00,&lane));

    lane &= XGXSBLK1_LANECTRL2_GLOOP1G_MASK;
 
    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        if (IS_DUAL_LANE_PORT(pc)) {
            lane_mask = 1 << pc->lane_num;
            lane_mask |= (lane_mask << 1);
        } else {
            lane_mask = 1 << pc->lane_num;
        }
    } else {  /* combo mode */
            lane_mask = XGXSBLK1_LANECTRL2_GLOOP1G_MASK;
    }
    lane &= lane_mask;

    *enable = lane;

    return SOC_E_NONE;
}

STATIC int
phy_wc40_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t      *pc;
    uint32 intf;

    pc = INT_PHY_SW_STATE(unit, port);

    if (pif > 31) {
        return SOC_E_PARAM;
    }

    /* need to check valid interfaces
     * ALL 40G/10G interfaces are mutually exclusive
     */
    intf = DEV_CFG_PTR(pc)->line_intf;

    if (WC40_40G_10G_INTF(pif)) {
        intf &= ~WC40_40G_10G_INTF_ALL;  /* clear all 10G/40G interfaces */
        intf |= 1 << pif;
    } else {
        intf |= 1 << pif;
    }

    DEV_CFG_PTR(pc)->line_intf = intf;
    return SOC_E_NONE;
}

STATIC int
phy_wc40_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t *pc;
    int speed;
    int intf;
    int scr;
    int rv;
    uint16 data16;

    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        rv = _phy_wc40_combo_speed_get(unit, port, &speed,&intf,&scr);
    } else {
        rv = _phy_wc40_ind_speed_get(unit, port, &speed,&intf,&scr);
    }

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        if (speed < 10000) {
            if (DEV_CFG_PTR(pc)->fiber_pref) {
                *pif = SOC_PORT_IF_GMII;
            } else {
                *pif = SOC_PORT_IF_SGMII;
            }
        } else {
            if (intf == SOC_PORT_IF_SFI) {
                if (DEV_CFG_PTR(pc)->line_intf & WC40_IF_CR) {
                    intf = SOC_PORT_IF_CR;
                }
            }
            *pif = intf;
        }
    } else { /* combo mode */
        if (intf == SOC_PORT_IF_KR4) {
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00, &data16));
            if (data16 == WC40_UC_CTRL_XLAUI) {
                intf = SOC_PORT_IF_XLAUI;
            } else if ((data16 == WC40_UC_CTRL_SR4) || 
                       (data16 == WC40_UC_CTRL_OPT_PF_TEMP_COMP)) {
                intf = SOC_PORT_IF_SR;
            }
        }
        *pif = intf;
    }

    return rv;
}

STATIC int
phy_wc40_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    uint8 cl73_bam = FALSE;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    pc = INT_PHY_SW_STATE(unit, port);

    WC40_MEM_SET(ability, 0, sizeof(*ability));

    if(DEV_CFG_PTR(pc)->cl73an == WC40_CL73_AND_CL73BAM) {
        cl73_bam = TRUE;
    }

    if (CUSTOM_MODE(pc)) {
        ability->speed_full_duplex  = SOC_PA_SPEED_3000MB; 
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = SOC_PA_LB_PHY;
        return (SOC_E_NONE);
    }

    if (CUSTOM1_MODE(pc)) {    
        ability->speed_full_duplex  = SOC_PA_SPEED_12GB;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = SOC_PA_LB_PHY;
        ability->interface |= SOC_PA_INTF_XGMII;
        return (SOC_E_NONE);
    }
    ability->fec = SOC_PA_FEC_NONE | SOC_PA_FEC_CL74;

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        if (DEV_CFG_PTR(pc)->fiber_pref)   {
            ability->speed_full_duplex  |= cl73_bam ? SOC_PA_SPEED_2500MB : 0;
            ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
            ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
            /* Rest of the abilities based on max speed */
        } else {
            ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB;
        }
        switch(pc->speed_max) {
            
            /* coverity[unterminated_case:FALSE] */
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
            /* coverity[fallthrough:FALSE] */
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB | SOC_PA_SPEED_21GB;
                /*passthru*/
            case 15000:
                ability->speed_full_duplex |= SOC_PA_SPEED_15GB;
                /*passthru*/
            case 13000:
                ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
                /*passthru*/
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
                /*passthru*/
            case 11000:
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
                /*passthru*/
            case 10000:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
                if (soc_feature(unit, soc_feature_higig_misc_speed_support)) {
                    ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
                }
                if (IS_XE_PORT(unit, port) ) {
                    ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
                    ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
                    ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
                }
                /*passthru*/
            default:
                break;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
        if (pc->speed_max >= 10000) {
            ability->interface |= SOC_PA_INTF_XGMII;
        } 
        phy_wc40_medium_get(unit, port, &ability->medium);
        ability->loopback  = SOC_PA_LB_PHY;
        if (IS_DUAL_LANE_PORT(pc)) {
            ability->flags     = 0;
        } else {
            ability->flags     = SOC_PA_AUTONEG;
        }
    } else {
        /* XXX when XLPORT is in single port mode, it does not support 1G and 2.5G.
         */
        if ((pc->speed_max <= 10000) || IS_XE_PORT(unit, port)) {
            ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
            ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
            if (DEV_CFG_PTR(pc)->fiber_pref)   {
                ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
                    ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
                    ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
            } else {
                ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
                ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB;
            }
        } else {
            ability->speed_full_duplex  = 0;
            ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
        }
        switch(pc->speed_max) {
            case 42000:
            case 40000:
                ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
                if ((!(WC40_REVID_A0(pc) || WC40_REVID_A1(pc))) && 
                	  (IS_HG_PORT(unit, port))) {
                    ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
                }
                /* coverity[MISSING_BREAK:FALSE] */
                /* passthru */
            case 32000:
            case 30000:
                ability->speed_full_duplex |= SOC_PA_SPEED_30GB;
                /* coverity[MISSING_BREAK:FALSE] */
                /* passthru */
            case 25000:
                ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
                /* coverity[MISSING_BREAK:FALSE] */
                /* passthru */
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
                /* coverity[MISSING_BREAK:FALSE] */
                /* passthru */
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
                /* coverity[MISSING_BREAK:FALSE] */
                /* passthru */
            case 16000:
                ability->speed_full_duplex |= SOC_PA_SPEED_16GB; 
                /* coverity[MISSING_BREAK:FALSE] */
                /* passthru */
            case 15000:
                ability->speed_full_duplex |= SOC_PA_SPEED_15GB; 
                /* coverity[MISSING_BREAK:FALSE] */
                /* passthru */
            case 13000:
                ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
                /* coverity[MISSING_BREAK:FALSE] */
                /* passthru */
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
                /* coverity[MISSING_BREAK:FALSE] */
                /* passthru */
            default:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
                break; 
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        phy_wc40_medium_get(unit, port, &ability->medium);
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = SOC_PA_AUTONEG;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_wc40_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, port, ability->speed_full_duplex));

    return (SOC_E_NONE);
}


#define PHY_WC40_LANEPRBS_LANE_SHIFT   4

STATIC int
_phy_wc40_control_prbs_polynomial_set(int unit, soc_port_t port, uint32 value)
{
    uint16 data16 = 0;
    uint16 mask16 = 0;
    int i;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
       
    if (DEV_CTRL_PTR(pc)->prbs.type == WC40_PRBS_TYPE_CL49) {
        DEV_CTRL_PTR(pc)->prbs.poly = WC40_PRBS_CFG_POLY31;  /* only support the PRBS31 for now */
        return SOC_E_NONE;
    }
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        /* configure for all four lanes */
        for (i = 0; i < 4; i++) {
            data16 |= value << (PHY_WC40_LANEPRBS_LANE_SHIFT * i);
            mask16 |= XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK << 
                      (PHY_WC40_LANEPRBS_LANE_SHIFT * i);
        }
    } else if (IS_DUAL_LANE_PORT(pc)) {
        for (i = pc->lane_num; i <= pc->lane_num+1; i++) {
            data16 |= value << (PHY_WC40_LANEPRBS_LANE_SHIFT * i);
            mask16 |= XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK << 
                      (PHY_WC40_LANEPRBS_LANE_SHIFT * i);
        }
    } else { /* single lane mode */
        data16 = value << (PHY_WC40_LANEPRBS_LANE_SHIFT * pc->lane_num);
        mask16 |= XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK << 
                      (PHY_WC40_LANEPRBS_LANE_SHIFT * pc->lane_num);
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK1_LANEPRBSr(unit, pc, 0x00, data16, mask16));
    DEV_CTRL_PTR(pc)->prbs.poly = value;

    return SOC_E_NONE;
}

STATIC int
_phy_wc40_control_prbs_polynomial_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    if (DEV_CTRL_PTR(pc)->prbs.type == WC40_PRBS_TYPE_CL49) {
        *value = WC40_PRBS_CFG_POLY31;  /*  prbs31 */
        return SOC_E_NONE;  
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_XGXSBLK1_LANEPRBSr(unit, pc, 0x00, &data));

    /* Extract prbs polynomial setting from register */
    data = ((data >> (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num)) &
            XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK);
    *value = (uint32) data;

    return SOC_E_NONE;
}


#define INV_SHIFTER(ln)  (PHY_WC40_LANEPRBS_LANE_SHIFT * (ln))

STATIC int
_phy_wc40_control_prbs_tx_invert_data_set(int unit, soc_port_t port, uint32 value)
{
    int i;
    uint16 data16 = 0;
    uint16 mask16 = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    if (DEV_CTRL_PTR(pc)->prbs.type == WC40_PRBS_TYPE_CL49) {
        return SOC_E_NONE;  /* not supported */
    }

    value <<= XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        /* configure for all four lanes */
        for (i = 0; i < 4; i++) {
            data16 |= value << INV_SHIFTER(i);
            mask16 |= XGXSBLK1_LANEPRBS_PRBS_INV0_MASK <<
                      INV_SHIFTER(i);
        }
    } else if (IS_DUAL_LANE_PORT(pc)) {
        for (i = pc->lane_num; i <= pc->lane_num+1; i++) {
            data16 |= value << INV_SHIFTER(i);
            mask16 |= XGXSBLK1_LANEPRBS_PRBS_INV0_MASK <<
                      INV_SHIFTER(i);
        }
    } else { /* single lane mode */
        data16 = value << INV_SHIFTER(pc->lane_num);
        mask16 |= XGXSBLK1_LANEPRBS_PRBS_INV0_MASK <<
                      INV_SHIFTER(pc->lane_num);
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK1_LANEPRBSr(unit, pc, 0x00, data16, mask16));
    
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_control_prbs_tx_invert_data_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    int inv_shifter;
        
    if (DEV_CTRL_PTR(pc)->prbs.type == WC40_PRBS_TYPE_CL49) {
        *value = 0;  /* no inversion */
        return SOC_E_NONE;  
    }
    inv_shifter = (PHY_WC40_LANEPRBS_LANE_SHIFT * pc->lane_num) +  
                   XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;

    SOC_IF_ERROR_RETURN
        (READ_WC40_XGXSBLK1_LANEPRBSr(unit, pc, 0x00, &data));
    
    data &= 1 << inv_shifter;
    
    *value = (data) ?  1 : 0;
    
    return SOC_E_NONE;
}

#define PRBS_LANES_MASK  (XGXSBLK1_LANEPRBS_PRBS_EN3_MASK | \
                XGXSBLK1_LANEPRBS_PRBS_EN2_MASK | \
                XGXSBLK1_LANEPRBS_PRBS_EN1_MASK | \
                XGXSBLK1_LANEPRBS_PRBS_EN0_MASK)

/* PRBS test
 * this routine enables the PRBS generator on applicable lanes depending on
 * the port mode. Before calling this function, a forced speed mode should
 * be set and either the external loopback or internal loopback should be
 * configured. Once this function is called, application should wait to
 * let the test run for a while and then calls the 
 * _phy_wc40_control_prbs_rx_status_get() to retrieve PRBS test status.
 * When calling this function to disable the PRBS test, the device or 
 * specific lane will be re-initialized.  
 */

STATIC int
_phy_wc40_control_prbs_enable_set(int unit, soc_port_t port, 
                                          uint32 enable)
{
    uint16      data16;
    uint16      mask16;
    int an;
    int an_done;
    soc_port_if_t intf;
    int prbs_lanes = 0;
    int lane;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* If mode is autoneg KR/KR4, do it thru CL49 PRBS and configure KR forced mode and disable autoneg
     * This way the FIR settings negotiated thru CL72 is preserved
     */

    if (DEV_CTRL_PTR(pc)->prbs.type != WC40_PRBS_TYPE_CL49) {
        SOC_IF_ERROR_RETURN
            (phy_wc40_an_get(unit,port,&an,&an_done));

        if (an && an_done) {
            /* check interface */
            SOC_IF_ERROR_RETURN
                (phy_wc40_interface_get(unit,port,&intf));
            if ((intf == SOC_PORT_IF_KR) || (intf == SOC_PORT_IF_KR4) ) {
                DEV_CTRL_PTR(pc)->prbs.type = WC40_PRBS_TYPE_CL49;
            }
        }
    }

    if (DEV_CTRL_PTR(pc)->prbs.type == WC40_PRBS_TYPE_CL49) {
        SOC_IF_ERROR_RETURN
            (phy_wc40_interface_get(unit,port,&intf));

        if (!enable) {  /* disable */
            if (intf == SOC_PORT_IF_KR4) {
                for (lane = 0; lane < 4; lane++) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_WC40_PCS_IEEE2BLK_PCS_TPCONTROLr(unit, pc, ln_access[lane], 0));
                }
            } else {
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_PCS_IEEE2BLK_PCS_TPCONTROLr(unit, pc, 0x00, 0));
            }
            DEV_CTRL_PTR(pc)->prbs.type = 0;
        } 

        if (intf == SOC_PORT_IF_KR4) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x00, 
                      enable? 0x11: 0, 
                      SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));

            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0x00,
                        enable?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0,
                        DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));

        } else {  /* KR */
            if (WC40_PHY400_REVS(pc)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, pc, 0x00,
                             enable? DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_MASK:0,
                             DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_MASK));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                             enable? SERDESDIGITAL_MISC2_AN_TXDISABLE_LN_MASK:0,
                             SERDESDIGITAL_MISC2_AN_TXDISABLE_LN_MASK));
            }

            /* force KR speed */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_MISC2r(unit, pc, 0x00,
                   enable? SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK:0,
                   SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK));
            mask16 =  PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION1_MASK |
                      PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION0_MASK;
            data16 = mask16;
            mask16 |= PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION2_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_PMD_IEEE0BLK_PMD_IEEECONTROL1r(unit, pc, 0x00, 
                             data16, mask16));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_PMD_IEEE0BLK_PMD_IEEECONTROL2r(unit, pc, 0x00, 
                             0xb, 0xf));
        }
        /* disable cl73 */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 0x00, 
                                          enable? 0: MII_CTRL_AE | MII_CTRL_RAN,
                                              MII_CTRL_AE | MII_CTRL_RAN));
        /* disable cl37 */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, 
                                  enable? 0: MII_CTRL_AE | MII_CTRL_RAN,
                                      MII_CTRL_AE | MII_CTRL_RAN));

        SOC_IF_ERROR_RETURN
            (READ_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 0x00, 
                                          &data16));
        /* not to enable PRBS here. Once PRBS is enabled, the link will go down.
         * Autoneg will be restarted by link partner and Tx settings will be lost.
         * It will be enabled in get function when first time called 
         */ 
        return SOC_E_NONE;
    }

    /* non-CL49 PRBS */ 

    /* A forced speed mode should be set before running PRBS */

    /* CDED & EDEN off: 0x8000 */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, 
               enable? 0: XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
                          XGXSBLK0_XGXSCONTROL_EDEN_MASK,
               XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
               XGXSBLK0_XGXSCONTROL_EDEN_MASK));

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        /* CDED & EDEN off: 0x8017 */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK1_LANECTRL2r(unit, pc, 0x00, 
                   enable? 0: XGXSBLK1_LANECTRL2_CDET_EN1G_MASK |
                              XGXSBLK1_LANECTRL2_EDEN1G_MASK,
                   XGXSBLK1_LANECTRL2_CDET_EN1G_MASK |
                   XGXSBLK1_LANECTRL2_EDEN1G_MASK));
        
        /* Turn off CL36 Tx PCS and 64/66 Endec: 0x8015 */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK1_LANECTRL0r(unit, pc, 0x00, 
                     enable? 0: XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK |
                                XGXSBLK1_LANECTRL0_ED66EN_MASK,
                     XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK |
                     XGXSBLK1_LANECTRL0_ED66EN_MASK));
    } else {
        /* Turn off CL36 Tx PCS and 64/66 Endec for active lane(s): 0x8015 */
        mask16 = 1 << pc->lane_num;
        if (IS_DUAL_LANE_PORT(pc)) {
            mask16 |= 1 << (pc->lane_num + 1);
        }
        mask16 |=  (mask16 << 12);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK1_LANECTRL0r(unit, pc, 0x00, 
             enable? 0: mask16, mask16));

        /* CDED & EDEN off for the active lane(s): 0x8017 */
        mask16 = 1 << pc->lane_num;
        if (IS_DUAL_LANE_PORT(pc)) {
            mask16 |= 1 << (pc->lane_num + 1);
        }
        mask16 =  (mask16 << 8) | (mask16 << 12);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK1_LANECTRL2r(unit, pc, 0x00, 
                   enable? 0 : mask16, mask16));

        /* clear RX clock compensation which seems messed up PRBS function */
        if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc) || WC40_REVID_B(pc)) {
            if (IS_DUAL_LANE_PORT(pc)) { 
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_XGXSBLK2_UNICOREMODE10Gr(unit, pc, 0x00, 
                             enable? 0 : 0x8091));
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_RX66_CONTROLr(unit, pc, 0x00,
                        enable? 0 : RX66_CONTROL_CC_EN_MASK |
                                    RX66_CONTROL_CC_DATA_SEL_MASK,
                        RX66_CONTROL_CC_EN_MASK | RX66_CONTROL_CC_DATA_SEL_MASK));
            }
        }
    }

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        /* Enable PRBS read on all lanes: 0x80f1=0x1c47 */ 
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_RXB_ANARXCONTROLr(unit, pc, 0x00,
                        RXB_ANARXCONTROL_STATUS_SEL_prbsStatus, 
                        RXB_ANARXCONTROL_STATUS_SEL_MASK));
        prbs_lanes = PRBS_LANES_MASK;

    } else if (IS_DUAL_LANE_PORT(pc)) {
        /* enable both lanes */
        if (pc->lane_num == 0) {
            prbs_lanes = XGXSBLK1_LANEPRBS_PRBS_EN1_MASK |
                         XGXSBLK1_LANEPRBS_PRBS_EN0_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX0_ANARXCONTROLr(unit, pc, 0x00,
                        RXB_ANARXCONTROL_STATUS_SEL_prbsStatus, 
                        RXB_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX1_ANARXCONTROLr(unit, pc, 0x00,
                        RXB_ANARXCONTROL_STATUS_SEL_prbsStatus, 
                        RXB_ANARXCONTROL_STATUS_SEL_MASK));

        } else { /* lane2 */
            prbs_lanes = XGXSBLK1_LANEPRBS_PRBS_EN3_MASK |
                         XGXSBLK1_LANEPRBS_PRBS_EN2_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX2_ANARXCONTROLr(unit, pc, 0x00,
                        RXB_ANARXCONTROL_STATUS_SEL_prbsStatus, 
                        RXB_ANARXCONTROL_STATUS_SEL_MASK));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_RX3_ANARXCONTROLr(unit, pc, 0x00,
                        RXB_ANARXCONTROL_STATUS_SEL_prbsStatus, 
                        RXB_ANARXCONTROL_STATUS_SEL_MASK));
        }

    } else {  /* single lane mode */
        prbs_lanes = 1 << ((XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * 
                        pc->lane_num) + XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT);
        SOC_IF_ERROR_RETURN
            (WC40_REG_MODIFY(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), 
                   RX0_ANARXCONTROL_STATUS_SEL_prbsStatus,
                   RX0_ANARXCONTROL_STATUS_SEL_MASK));
    }
    
    /* enable PRBS generator on applicable lanes: 0x8019 */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK1_LANEPRBSr(unit, pc, 0x00, 
                      enable? prbs_lanes : 0,
                      prbs_lanes));
    return SOC_E_NONE;
}


STATIC int
_phy_wc40_control_prbs_enable_get(int unit, soc_port_t port, 
                                        uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    if (DEV_CTRL_PTR(pc)->prbs.type == WC40_PRBS_TYPE_CL49) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_PCS_IEEE2BLK_PCS_TPCONTROLr(unit, pc, 0x00, &data));
        if (data == WC40_PRBS_CL49_POLY31) {
            *value = 1; 
        } else {
            *value = 0; 
        }
        return SOC_E_NONE;
    }

    /*
     * Hypercore PRBS - note that in the Hypercore there is only 1 enable 
     * for both TX/RX 
     */
    SOC_IF_ERROR_RETURN
        (READ_WC40_XGXSBLK1_LANEPRBSr(unit, pc, 0x00, &data));

    data = data & ((1 << (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num))
                   << XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT);
    
    *value = (data) ?  1 : 0;
  
    return SOC_E_NONE;
}

/*
 * Returns value 
 *      ==  0: PRBS receive is in sync
 *      == -1: PRBS receive never got into sync
 *      ==  n: number of errors
 */
STATIC int
_phy_wc40_control_prbs_rx_status_get(int unit, soc_port_t port, 
                                          uint32 *value)
{
    int         lane;
    int         lane_end;
    uint16      data;
    int         prbs_cfg;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    /* Get status for all 4 lanes and check for sync
     * 0x80b0, 0x80c0, 0x80d0, 0x80e0 
     */
    *value = 0;
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        /* combo mode, pc->lane_num = 0 */
        lane_end = 3;  
    } else if (IS_DUAL_LANE_PORT(pc)) {
        lane_end = pc->lane_num + 1;  
    } else {
        lane_end = pc->lane_num;  
    }

    if (DEV_CTRL_PTR(pc)->prbs.type == WC40_PRBS_TYPE_CL49) {
        /* enable PRBS if not already. PRBS31 or PRBS9 */
        for (lane = pc->lane_num; lane <= lane_end; lane++) {
            prbs_cfg = TRUE;
            SOC_IF_ERROR_RETURN
                (READ_WC40_PCS_IEEE2BLK_PCS_TPCONTROLr(unit, pc, ln_access[lane], &data));
            if (DEV_CTRL_PTR(pc)->prbs.poly == WC40_PRBS_CFG_POLY31) {
                if (data == WC40_PRBS_CL49_POLY31) { /* prbs already in PRBS31 */
                    prbs_cfg = FALSE;
                }
            } else {
                DEV_CTRL_PTR(pc)->prbs.poly = WC40_PRBS_CFG_POLY31; /* support PRBS31 only */
            }

            if (prbs_cfg) {
                SOC_IF_ERROR_RETURN
                    (WRITE_WC40_PCS_IEEE2BLK_PCS_TPCONTROLr(unit, pc, ln_access[lane], 
                              WC40_PRBS_CL49_POLY31));
            }
        }

        data = 0;
        for (lane = pc->lane_num; lane <= lane_end; lane++) {
            SOC_IF_ERROR_RETURN
                (READ_WC40_CL49_USERB0_STATUSr(unit,pc,ln_access[lane],&data));

            if (data & CL49_USERB0_STATUS_PRBS_LOCK_MASK) { /* PRBS lock */
                /* check error count */
                SOC_IF_ERROR_RETURN
                    (READ_WC40_PCS_IEEE2BLK_PCS_TPERRCOUNTERr(unit,pc,ln_access[lane],&data));
                *value += data;
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "prbs_rx_status_get: u=%d p=%d (lane %d errors 0x%x)\n"), 
                                     unit, port, lane, data));

            } else { /* PRBS not in sync */
                *value = -1;
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "prbs_rx_status_get: u=%d p=%d (lane %d not in sync)\n"),
                                     unit, port, lane));
                break;
            } 
        }
        return SOC_E_NONE;
    }
 
    for (lane = pc->lane_num; lane <= lane_end; lane++) {
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, 0x00, 0x80b0 + (0x10 * lane), &data));

        if (data & RX0_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_MASK) {
            if (data & RX0_ANARXSTATUS_PRBS_STATUS_PRBS_STKY_MASK) {
                /* locked now but loss before */
                *value = -2;
                continue;
            } else {
                /* locked now and no loss before. Get errors */
                *value += data & RX0_ANARXSTATUS_PRBS_STATUS_PTBS_ERRORS_MASK;
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "prbs_rx_status_get: u=%d p=%d (lane %d errors 0x%x)\n"),
                                     unit, port, lane, (data & RX0_ANARXSTATUS_PRBS_STATUS_PTBS_ERRORS_MASK)));
            }
        } else {
            /* PRBS not in sync */
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "prbs_rx_status_get: u=%d p=%d (lane %d not in sync)\n"),
                                 unit, port, lane));
            *value = -1;
            break;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_control_8b10b_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data16;
    uint16      mask16;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    mask16 = 1 << (XGXSBLK1_LANECTRL2_CDET_EN1G_SHIFT + pc->lane_num);
    mask16 |= 1 << (XGXSBLK1_LANECTRL2_EDEN1G_SHIFT + pc->lane_num);

    data16 = value? mask16: 0;
 
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK1_LANECTRL2r(unit, pc, 0x00, data16,mask16));

    return SOC_E_NONE;
}

STATIC int
_phy_wc40_control_preemphasis_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
    int     lane_ctrl;
    int     lane_num = 0;
    int     i;

    if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {
        lane_ctrl = LANE0_ACCESS;
        lane_num  = 0;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) {
        lane_ctrl = LANE1_ACCESS;
        lane_num  = 1;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) {
        lane_ctrl = LANE2_ACCESS;
        lane_num  = 2;
    
      /* coverity[mixed_enums:FALSE] */
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) {
        lane_ctrl = LANE3_ACCESS;
        lane_num  = 3;
    } else {
        lane_ctrl = TX_DRIVER_DFT_LN_CTRL;
    }

   
    /* write to specific lane */
    if (lane_ctrl != TX_DRIVER_DFT_LN_CTRL) {
        /* qualify the operation: only writes to its own lane. */
        if (!COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) { 
            if (IS_DUAL_LANE_PORT(pc)) {
                if ((lane_num < pc->lane_num) || (lane_num > (pc->lane_num + 1))) {
                    return SOC_E_PARAM;
                } 
            } else {
                if (lane_num != pc->lane_num) {
                    return SOC_E_PARAM;
                }
            }
        }
    
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(unit,pc,
                     lane_ctrl,value));
        DEV_CFG_PTR(pc)->preemph[lane_num - pc->lane_num] = value;
    } else { /* default mode */
        int size;

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            size = 4;
            /* broadcast to all lanes */
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_AERBLK_AERr(unit,pc,0x00, WC_AER_BCST_OFS_STRAP));

        } else { /* dual-lane bcst if in dxgxs mode */
            size = IS_DUAL_LANE_PORT(pc)? 2: 1;
            DUAL_LANE_BCST_ENABLE(pc);
        }

        SOC_IF_ERROR_RETURN
            (WRITE_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(unit,pc,0x00,value));

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            /* restore AER */
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_AERBLK_AERr(unit,pc,0x00, 0));
        } else {
            DUAL_LANE_BCST_DISABLE(pc);
        }
        for (i = 0; i < size; i++) {
            DEV_CFG_PTR(pc)->preemph[i] = value;
        }
    }
    return SOC_E_NONE;
}


STATIC int
_phy_wc40_tx_driver_field_get(soc_phy_control_t type,int *ln_ctrl,uint16 *mask,int *shfter)
{
    int lane_ctrl;
    
    lane_ctrl = TX_DRIVER_DFT_LN_CTRL;
    *mask = 0;
    *shfter = 0;
    /* _LANEn(n=0-3) control type only applies to combo mode or dxgxs in
     * independent channel mode
     */
    switch(type) {
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
    /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
    /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
    /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
    /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        *shfter = TXB_TX_DRIVER_IDRIVER_SHIFT;
        *mask = TXB_TX_DRIVER_IDRIVER_MASK;
        if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) {
            lane_ctrl = 0;
        } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) {
            lane_ctrl = 1;
        } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) {
            lane_ctrl = 2;
        } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) {
            lane_ctrl = 3;
        }
        break;

    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
    /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
    /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
    /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
    /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        *shfter = TXB_TX_DRIVER_IPREDRIVER_SHIFT;
        *mask = TXB_TX_DRIVER_IPREDRIVER_MASK;
        if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0) {
            lane_ctrl = 0;
        } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1) {
            lane_ctrl = 1;
        } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2) {
            lane_ctrl = 2;

          /* coverity[mixed_enums:FALSE] */
        } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3) {
            lane_ctrl = 3;
        }
        break;
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:
        *shfter = TXB_TX_DRIVER_POST2_COEFF_SHIFT;
        *mask = TXB_TX_DRIVER_POST2_COEFF_MASK;
        break;
    default:
         /* should never get here */
        return SOC_E_PARAM;
    }
    *ln_ctrl = lane_ctrl;
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_control_tx_driver_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
    uint16  data;             /* Temporary holder of reg value to be written */
    uint16  mask;             /* Bit mask of reg value to be updated */
    int     lane_ctrl;
    int     lane_num;
    int     shifter;
    int     i;
    int     size;

    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_driver_field_get(type,&lane_ctrl,&mask,&shifter));

    data = value << shifter;

    /* qualify the operation: only writes to its own lane. */
    if (lane_ctrl != TX_DRIVER_DFT_LN_CTRL) {
        if (!COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            if (IS_DUAL_LANE_PORT(pc)) {
                if ((lane_ctrl < pc->lane_num) || (lane_ctrl > (pc->lane_num + 1))) {
                    return SOC_E_PARAM;
                }
            } else {
                if (lane_ctrl != pc->lane_num) {
                    return SOC_E_PARAM;
                }
            }
        }
    }
    if ((lane_ctrl != TX_DRIVER_DFT_LN_CTRL) ||
        ((!COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) && (!IS_DUAL_LANE_PORT(pc))) ) {
        if (lane_ctrl != TX_DRIVER_DFT_LN_CTRL) {
            lane_num = lane_ctrl;
        } else {
            lane_num = pc->lane_num;         
        }
        if (shifter == TXB_TX_DRIVER_IDRIVER_SHIFT) {
            DEV_CFG_PTR(pc)->idriver[lane_num - pc->lane_num] = value;
        } else if (shifter == TXB_TX_DRIVER_IPREDRIVER_SHIFT) {
            DEV_CFG_PTR(pc)->pdriver[lane_num - pc->lane_num] = value;
        } else {
            DEV_CFG_PTR(pc)->post2driver[lane_num - pc->lane_num] = value;
        }

        if (lane_num == 0) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX0_TX_DRIVERr(unit, pc, 0x00, data, mask));
        } else if (lane_num == 1) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX1_TX_DRIVERr(unit, pc, 0x00, data, mask));
        } else if (lane_num == 2) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX2_TX_DRIVERr(unit, pc, 0x00, data, mask));
        } else if (lane_num == 3) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX3_TX_DRIVERr(unit, pc, 0x00, data, mask));
        }
    } else { /* default control in combo mode, or dxgxs port */
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            size = 4;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX0_TX_DRIVERr(unit, pc, 0x00, data, mask));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX1_TX_DRIVERr(unit, pc, 0x00, data, mask));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX2_TX_DRIVERr(unit, pc, 0x00, data, mask));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX3_TX_DRIVERr(unit, pc, 0x00, data, mask));
        } else if (IS_DUAL_LANE_PORT(pc)) {
            size = 2;
            if (pc->lane_num == 0) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TX0_TX_DRIVERr(unit, pc,0x00, data, mask));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TX1_TX_DRIVERr(unit, pc,0x00, data, mask));
            } else {
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TX2_TX_DRIVERr(unit, pc, LANE0_ACCESS, data, mask));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TX3_TX_DRIVERr(unit, pc, LANE0_ACCESS, data, mask));
            }
        } else {
            size = 0;
        }
        for (i = pc->lane_num; i < size; i++) {
            if (shifter == TXB_TX_DRIVER_IDRIVER_SHIFT) {
                DEV_CFG_PTR(pc)->idriver[i] = value;
            } else if (shifter == TXB_TX_DRIVER_IPREDRIVER_SHIFT) {
                DEV_CFG_PTR(pc)->pdriver[i] = value;
            } else {
                DEV_CFG_PTR(pc)->post2driver[i] = value;
            }
        }

    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_control_preemphasis_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    uint16  data16;             /* Temporary holder of reg value to be written */
    int     lane_ctrl;

    if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {
        lane_ctrl = LANE0_ACCESS;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) {
        lane_ctrl = LANE1_ACCESS;
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) {
        lane_ctrl = LANE2_ACCESS;

      /* coverity[mixed_enums:FALSE] */
    } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) {
        lane_ctrl = LANE3_ACCESS;
    } else {
        lane_ctrl = TX_DRIVER_DFT_LN_CTRL;
    }

    /* write to specific lane */
    if (lane_ctrl != TX_DRIVER_DFT_LN_CTRL) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(unit,pc,
                     lane_ctrl,&data16));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(unit,pc,0x00,&data16));
    }
    *value = data16;

    return SOC_E_NONE;
}
STATIC int
_phy_wc40_control_tx_driver_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    uint16  data16;           /* Temporary holder of 16 bit reg value */
    uint16  mask;
    int     shifter;
    int     lane_ctrl;
    int     lane_num;

    SOC_IF_ERROR_RETURN
        (_phy_wc40_tx_driver_field_get(type,&lane_ctrl,&mask,&shifter));

    if (lane_ctrl != TX_DRIVER_DFT_LN_CTRL) {
        lane_num = lane_ctrl;
    } else {
        lane_num = pc->lane_num;
    }
    if (lane_num == 0) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX0_TX_DRIVERr(unit, pc, LANE0_ACCESS, &data16));
    } else if (lane_num == 1) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX1_TX_DRIVERr(unit, pc, LANE0_ACCESS, &data16));
    } else if (lane_num == 2) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX2_TX_DRIVERr(unit, pc, LANE0_ACCESS, &data16));
    } else if (lane_num == 3) {
        SOC_IF_ERROR_RETURN
            (READ_WC40_TX3_TX_DRIVERr(unit, pc, LANE0_ACCESS, &data16));
    } else {
        return SOC_E_PARAM;
    }
    *value = (data16 & mask) >> shifter;

    return SOC_E_NONE;
}

/*
 * Works for 10GX4 8B/10B endec in forced mode. May not work with scrambler 
 * enabled. Doesn't work for 1G/2.5G. Doesn't work for 10G XFI and 40G 
 * using 64/66 endec yet.
 */ 
STATIC int
_phy_wc40_control_bert_set(int unit, phy_ctrl_t *pc, int ctrl,
                                uint32 value)
{
    uint16 data16 = 0;
    uint16 mask16 = 0;
    int rv = SOC_E_NONE;

    switch (ctrl) {
        case SOC_PHY_CONTROL_BERT_PATTERN:   
            if (value == SOC_PHY_CONTROL_BERT_CRPAT) {
                data16 = TXBERT_TXBERTCTRL_CRPAT_EN_MASK;
            } else if (value == SOC_PHY_CONTROL_BERT_CJPAT) {
                data16 = TXBERT_TXBERTCTRL_CJPAT_EN_MASK;
            } else {
                return SOC_E_PARAM;
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TXBERT_TXBERTCTRLr(unit, pc, 0x00,
                     data16,
                     TXBERT_TXBERTCTRL_CJPAT_EN_MASK |
                     TXBERT_TXBERTCTRL_CRPAT_EN_MASK));
            break;
    
        case SOC_PHY_CONTROL_BERT_RUN:    
            if (value) {
                /* bert test setup */
                /* enable |E| monitor  */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_RXB_ANARXCONTROL1Gr(unit,pc,0x00,
                             RXB_ANARXCONTROL1G_EMON_EN_MASK,
                             RXB_ANARXCONTROL1G_EMON_EN_MASK));

                /* with |E| monitor enabled, this selects 8B/10B endec status*/
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_RXB_ANARXCONTROLr(unit, pc, 0x00,
                            RXB_ANARXCONTROL_STATUS_SEL_prbsStatus, 
                            RXB_ANARXCONTROL_STATUS_SEL_MASK));

                /* enable pattern generator and comparator */
                mask16 = XGXSBLK0_XGXSCONTROL_PGEN_EN_MASK |
                         XGXSBLK0_XGXSCONTROL_PCMP_EN_MASK;
                data16 = mask16;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit,pc,0x00,
                          data16, mask16));

                /* enable CJ(R)PAT generator wiht Q_en and clear TX counter */
                mask16 = TXBERT_TXBERTCTRL_Q_EN_MASK |
                         TXBERT_TXBERTCTRL_Q_LINK_EN_MASK |
                         TXBERT_TXBERTCTRL_COUNT_CLR_MASK;
                data16 = mask16;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TXBERT_TXBERTCTRLr(unit, pc, 0x00, data16, mask16)); 

                /* re-enable the TX counter */
                data16 = 0;
                mask16 = TXBERT_TXBERTCTRL_COUNT_CLR_MASK;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TXBERT_TXBERTCTRLr(unit, pc, 0x00, data16, mask16)); 
 
                /* clear RX counters and enable RX CRC checker */ 
                mask16 = RXBERT_RXBERTCTRL_COUNT_CLR_MASK |
                         RXBERT_RXBERTCTRL_ERR_CLR_MASK |
                         RXBERT_RXBERTCTRL_CRCCHK_EN_MASK;
                data16 = mask16;               
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_RXBERT_RXBERTCTRLr(unit, pc, 0x00,
                         data16, mask16));
 
                /* re-enable the RX counters */
                data16 = 0;
                mask16 = RXBERT_RXBERTCTRL_COUNT_CLR_MASK |
                         RXBERT_RXBERTCTRL_ERR_CLR_MASK;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_RXBERT_RXBERTCTRLr(unit, pc, 0x00,
                         data16, mask16));

                /* enable packet transmission  */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TXBERT_TXBERTCTRLr(unit, pc, 0x00,
                         TXBERT_TXBERTCTRL_PKT_EN_MASK,
                         TXBERT_TXBERTCTRL_PKT_EN_MASK)); 
            } else {
                /* clear bert test setup */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_RXB_ANARXCONTROL1Gr(unit,pc,0x00,
                             0,
                             RXB_ANARXCONTROL1G_EMON_EN_MASK));

                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_RXB_ANARXCONTROLr(unit, pc, 0x00,
                            0,
                            RXB_ANARXCONTROL_STATUS_SEL_MASK));

                /* disable packet transmission  */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_TXBERT_TXBERTCTRLr(unit, pc, 0x00,
                         0,
                         TXBERT_TXBERTCTRL_PKT_EN_MASK));

                /* this clears counters */
                /* disable pattern generator and comparator */
                /*SOC_IF_ERROR_RETURN
                 *  (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit,pc,0x00,
                 *        0,
                 *        XGXSBLK0_XGXSCONTROL_PGEN_EN_MASK |
                 *        XGXSBLK0_XGXSCONTROL_PCMP_EN_MASK));
                 */
            }
            break;

        case SOC_PHY_CONTROL_BERT_PACKET_SIZE:
            break;
        case SOC_PHY_CONTROL_BERT_IPG:
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }
    return rv;
}

STATIC int
_phy_wc40_control_bert_get(int unit, phy_ctrl_t *pc, int ctrl,
                                uint32 *value)
{
    uint16 data16;
    int rv = SOC_E_NONE;

    switch (ctrl) {
        case SOC_PHY_CONTROL_BERT_TX_PACKETS:
            SOC_IF_ERROR_RETURN
                (READ_WC40_TXBERT_TXBERTPACKETUr(unit,pc,0x00,&data16));
            *value = data16 << 16;
            SOC_IF_ERROR_RETURN
                (READ_WC40_TXBERT_TXBERTPACKETLr(unit,pc,0x00,&data16));
            *value |= data16;
            break;

        case SOC_PHY_CONTROL_BERT_RX_PACKETS:
            SOC_IF_ERROR_RETURN
                (READ_WC40_RXBERT_RXBERTPACKETUr(unit,pc,0x00,&data16));
            *value = data16 << 16;
            SOC_IF_ERROR_RETURN
                (READ_WC40_RXBERT_RXBERTPACKETLr(unit,pc,0x00,&data16));
            *value |= data16;
            break;

        case SOC_PHY_CONTROL_BERT_RX_ERROR_BITS:
            SOC_IF_ERROR_RETURN
                (READ_WC40_RXBERT_RXBERTBITERRr(unit,pc,0x00,&data16));
            *value = data16;
            break;

        case SOC_PHY_CONTROL_BERT_RX_ERROR_BYTES:
            SOC_IF_ERROR_RETURN
                (READ_WC40_RXBERT_RXBERTBYTEERRr(unit,pc,0x00,&data16));
            *value = data16;
            break;

        case SOC_PHY_CONTROL_BERT_RX_ERROR_PACKETS:
            SOC_IF_ERROR_RETURN
                (READ_WC40_RXBERT_RXBERTPKTERRr(unit,pc,0x00,&data16));
            *value = data16;
            break;
        case SOC_PHY_CONTROL_BERT_PATTERN:
            SOC_IF_ERROR_RETURN
                (READ_WC40_TXBERT_TXBERTCTRLr(unit, pc, 0x00, &data16));
            if (data16 & TXBERT_TXBERTCTRL_CRPAT_EN_MASK) {
                *value = SOC_PHY_CONTROL_BERT_CRPAT;
            } else {
                *value = SOC_PHY_CONTROL_BERT_CJPAT;
            }
            break;
        case SOC_PHY_CONTROL_BERT_PACKET_SIZE:
            break;
        case SOC_PHY_CONTROL_BERT_IPG:
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }
    return rv;
}

#ifdef WC_EEE_SUPPORT     
STATIC int
_phy_wc40_control_eee_set(int unit, phy_ctrl_t *pc, 
                          soc_phy_control_t type,uint32 enable)
{
    uint16 data0;
    uint16 mask0;
    uint16 data1;
    uint16 mask1;

    switch (type) { 
        case SOC_PHY_CONTROL_EEE:
            mask0 = EEE_USERB0_EEE_COMBO_CONTROL0_EEE_EN_FORCE_VAL_MASK |
                     EEE_USERB0_EEE_COMBO_CONTROL0_EEE_EN_FORCE_MASK |
                     EEE_USERB0_EEE_COMBO_CONTROL0_LPI_EN_FORCE_MASK;
            if (enable == 1) {
                data0 = mask0;
                /* enable EEE, disable the LPI pass-thru */
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL4_MISC5r(unit,pc,0x00, 0,
                                (1 << 14) | (1 << 15)));
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_XGXSBLK7_EEECONTROLr(unit,pc,0x00,0,
                                XGXSBLK7_EEECONTROL_LPI_EN_TX_MASK |
                                XGXSBLK7_EEECONTROL_LPI_EN_RX_MASK));
            } else if (enable == 2) {
                /* enable the LPI pass-thru */
                data0 = mask0; 
                mask1 = (1 << 14) | (1 << 15);
                data1 = mask1;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL4_MISC5r(unit,pc,0x00, data1, mask1));
                mask1 = XGXSBLK7_EEECONTROL_LPI_EN_TX_MASK |
                        XGXSBLK7_EEECONTROL_LPI_EN_RX_MASK;
                data1 = mask1;
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_XGXSBLK7_EEECONTROLr(unit,pc,0x00,data1,mask1));
            } else {
                /* disable EEE */
                data0 = EEE_USERB0_EEE_COMBO_CONTROL0_EEE_EN_FORCE_MASK;
            }
            /* forced EEE  */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_EEE_USERB0_EEE_COMBO_CONTROL0r(unit,pc,0x00,
                      data0, mask0));
            break;

        case SOC_PHY_CONTROL_EEE_AUTO:
            /* EEE through autoneg */
            mask0 = EEE_USERB0_EEE_COMBO_CONTROL0_EEE_EN_FORCE_MASK |
                     EEE_USERB0_EEE_COMBO_CONTROL0_LPI_EN_FORCE_MASK;
            if (enable == 1) {
                data0 = 0;
            } else {
                data0 = mask0;
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_EEE_USERB0_EEE_COMBO_CONTROL0r(unit,pc,0x00,data0,
                      mask0));
            break;

    default:
         /* should never get here */
         return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}
#endif

STATIC int 
_phy_wc40_rloop_set(int unit, phy_ctrl_t *pc, uint32 enable)
{
    uint16 mask16;
    uint16 data16;
    int speed;
    int intf;
    int scr;
    int rv;

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        rv = _phy_wc40_combo_speed_get(unit, pc->port, &speed,&intf,&scr);
    } else {
        rv = _phy_wc40_ind_speed_get(unit, pc->port, &speed,&intf,&scr);
    }
    SOC_IF_ERROR_RETURN(rv);

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        if (speed <= 2500) {
            mask16 = SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_MASK;
            data16 = enable? mask16: 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit,pc,0x00, data16,mask16));
           } else if (speed == 10000) { /* XFI/SFI/KR */
            mask16 = TX66_CONTROL_RLOOP_EN_MASK;
            data16 = enable? mask16: 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TX66_CONTROLr(unit,pc,0x00, data16,mask16));

            /* need to enable RX clock compensation which is done already in the init */
        } else if (speed > 10000) { /* dxgxs mode */
            mask16 = XGXSBLK0_XGXSCONTROL_RLOOP_MASK;
            data16 = enable? mask16: 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit,pc,0x00, data16,mask16));
            /* clock compensation should be already enabled in init */
        }
    } else { /* combo mode */
        mask16 = XGXSBLK0_XGXSCONTROL_RLOOP_MASK;
        data16 = enable? mask16:0;

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit,pc,0x00, data16,mask16));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_rloop_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    uint16 mask16;
    uint16 data16;

    if (IND_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        mask16 = 1 << (XGXSBLK1_LANECTRL2_RLOOP1G_SHIFT + pc->lane_num);
        SOC_IF_ERROR_RETURN
            (READ_WC40_XGXSBLK1_LANECTRL2r(unit,pc,0x00, &data16));
        *value = (mask16 & data16)? TRUE: FALSE;

    } else {
        mask16 = XGXSBLK0_XGXSCONTROL_RLOOP_MASK;
        SOC_IF_ERROR_RETURN
            (READ_WC40_XGXSBLK0_XGXSCONTROLr(unit,pc,0x00, &data16));
        *value = (mask16 & data16)? TRUE: FALSE;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_fec_set(int unit, phy_ctrl_t *pc, uint32 fec_ctrl)
{
    int rv = SOC_E_NONE;
    uint16 mask16;
    uint16 data16;
	int lane_start, lane_end,index;

    mask16 = CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_VAL_MASK |
             CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_MASK;
	
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
       lane_start = 0;
       lane_end   = 3;
    } else if (IS_DUAL_LANE_PORT(pc)) {
       lane_start = pc->lane_num;
       lane_end = lane_start + 1;
    } else {  /* quad-port mode */
       lane_start = pc->lane_num;
       lane_end   = lane_start;
    }

   for (index = lane_start; index <= lane_end; index++) {
	
    switch (fec_ctrl) {
        case SOC_PHY_CONTROL_FEC_OFF:
            data16 = CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(unit,pc,ln_access[index],data16,mask16));
            break;
        case SOC_PHY_CONTROL_FEC_ON:
            data16 = mask16;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(unit,pc,ln_access[index],data16,mask16));
            break;
        case SOC_PHY_CONTROL_FEC_AUTO:
            /* turn off forced override */
            data16 = 0; 
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(unit,pc,ln_access[index],data16,mask16));

            /* configure the CL73 FEC advertisement */
            mask16 = AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK;
            data16 = mask16;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(unit, pc, ln_access[index], data16,
                                     mask16));
            /* configure the CL37 FEC advertisement */
            data16 = mask16 = DIGITAL3_UP3_FEC_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL3_UP3r(unit, pc, ln_access[index], data16, mask16));

            break;
        default:
            rv = SOC_E_PARAM;
            break;
    } 
		}
    return rv;
}

STATIC int
_phy_wc40_fec_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    uint16 data16;

    SOC_IF_ERROR_RETURN
        (READ_WC40_CL74_USERB0_UFECCONTROL3r(unit,pc,0x00,&data16));
    if (data16 & CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_MASK) {
        *value = data16 & CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_VAL_MASK?
                 SOC_PHY_CONTROL_FEC_ON:
                 SOC_PHY_CONTROL_FEC_OFF; 
    } else {
        *value = SOC_PHY_CONTROL_FEC_AUTO;
    }
    return SOC_E_NONE;         
}

STATIC int
_phy_wc40_control_linkdown_transmit_set(int unit, soc_port_t port, uint32 value)
{
    uint16      ctrl2_data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    if (value) {
        ctrl2_data = (SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) |
                     (SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS  <<
                      SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |
                     (SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0x00, ctrl2_data));

    } else {
        ctrl2_data = (SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |
                     (SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) |
                     (SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0x00, ctrl2_data));
    }

    return SOC_E_NONE;
}
STATIC int
_phy_wc40_control_linkdown_transmit_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0x00, &data));

    /* Check if FORCE_XMIT_DATA_ON_TXSIDE is set */
    *value = (data & (SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
                      SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT))
             ? 1 : 0;

    return SOC_E_NONE;
}

STATIC int
_phy_wc40_control_vco_freq_get(int unit, soc_port_t port, uint32 *value)
{
    uint16 data, divider = 32;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(READ_WC40_PLL_ANAPLLSTATUSr(unit, pc, 0x00, &data));
    switch(data & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK){
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div32: divider  = 32;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div36: divider  = 36;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div40: divider  = 40;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div42: divider  = 42;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div48: divider  = 48;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div50: divider  = 50;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div52: divider  = 52;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div54: divider  = 54;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div60: divider  = 60;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div64: divider  = 64;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div66: divider  = 66;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div68: divider  = 68;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div80: divider  = 70;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div120: divider  = 80;  break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div200: divider = 92; break;
        case PLL_ANAPLLSTATUS_PLL_MODE_AFE_div240: divider = 100; break;
        
        /* default: return SOC_E_CONFIG; */
    } 
    *value = 156 * divider;
    return(SOC_E_NONE);
}


/*
 * Function:
 *      phy_wc40_control_set
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
phy_wc40_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv, lane_start = 0, lane_end = 0, index = 0;
    phy_ctrl_t *pc;
    int speed, intf, scr;
    uint16 mask16, data16;
    soc_port_if_t interface;
    int an, an_done;
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }

    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        /*passthru*/
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        /*passthru*/
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        /*passthru*/
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        /*passthru*/
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = _phy_wc40_control_preemphasis_set(unit, pc, type, value);
        break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:     
        rv = _phy_wc40_control_tx_driver_set(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_wc40_control_prbs_polynomial_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_wc40_control_prbs_tx_invert_data_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* passthru */
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_wc40_control_prbs_enable_set(unit, port, value);
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_8B10B:
        rv = _phy_wc40_control_8b10b_set(unit, port, value);
        break;
#ifdef WC_EEE_SUPPORT
    case SOC_PHY_CONTROL_EEE:
        /*passthru*/
    case SOC_PHY_CONTROL_EEE_AUTO:
        rv = _phy_wc40_control_eee_set(unit,pc,type,value);
        break;
#endif
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = _phy_wc40_rloop_set(unit,pc,value);
        break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        rv = _phy_wc40_fec_set(unit,pc,value);
        break;
    /* XXX obsolete */
    case SOC_PHY_CONTROL_CUSTOM1:
        DEV_CFG_PTR(pc)->custom1 = value? TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_SCRAMBLER:
        DEV_CFG_PTR(pc)->scrambler_en = value? TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_wc40_control_linkdown_transmit_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        /* enable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0,
              value? SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK:0,
              SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_BERT_PATTERN:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_RUN:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_PACKET_SIZE:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_IPG:
        rv = _phy_wc40_control_bert_set(unit,pc,type,value);
        break;
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:

       /* value = offset
        * ----------------------------------
        * Reference Values:
        * PPM   Offset  Setting
        * -5    0xff99
        * -10   0xff31
        * -15   0xfec9
        * -20   0xfe61
        * -25   0xfdf9
        * -30   0xfd91
        * -35   0xfd29
        * -40   0xfcc1
        * -45   0xfc59
        * -50   0xfbf1
        * -55   0xfb89
        * -60   0xfb21
        * -65   0xfab9
        * -70   0xfa51
        *     
        * 5     0x69
        * 10    0xd1
        * 15    0x139
        * 20    0x1a1
        * 25    0x209
        * 30    0x271
        * 35    0x2d0
        * 40    0x341
        * 45    0x3a9
        * 50    0x411
        * 55    0x479
        * 60    0x4e1
        * 65    0x549
        * 70    0x5b1
        * ----------------------------------
       */

        if(value & 0x1) {
            /* Enable Tx PI */
            SOC_IF_ERROR_RETURN(
                    (MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL1r(unit, pc, 0x00, 
                            1, 
                            XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK)));
            /* TX PI Frequency Override Enable BIT[0]
               Frequency Override value BIT[15:1] 
             */
            SOC_IF_ERROR_RETURN(
                    (WRITE_WC40_XGXSBLK8_TX_PI_CONTROL3r(unit, pc, 0x00, value)));
        } else {
            /* Disable Tx PI */
            SOC_IF_ERROR_RETURN(
                    (MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL1r(unit, pc, 0x00, 
                     0,
                     XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK))); 
            /* TX PI Frequency Override Enable BIT[0]
               Frequency Override value BIT[15:1] 
             */
            SOC_IF_ERROR_RETURN(
                    (WRITE_WC40_XGXSBLK8_TX_PI_CONTROL3r(unit, pc, 0x00, 0)));

        }
        rv = SOC_E_NONE;
        break; 
    case SOC_PHY_CONTROL_RX_RESET:

        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            lane_start = 0;
            lane_end   = 3;
        } else if (IS_DUAL_LANE_PORT(pc)) {
            lane_start = pc->lane_num;
            lane_end = lane_start + 1;
        } else {  /* quad-port mode */
            lane_start = pc->lane_num;
            lane_end   = lane_start;
        }
        for (index = lane_start; index <= lane_end; index++) {
             SOC_IF_ERROR_RETURN
                     (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(unit, pc, ln_access[index], 
                      DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK, 
                      DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
        }
        rv = SOC_E_NONE;
        break;  
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
#if 0 
        rv = SOC_E_FAIL;
        phy_wc40_interface_get(unit, port, &interface);
        /* Only enable the feature for 10G/40G fiber */
        if( (interface == SOC_PORT_IF_SFI) || (interface == SOC_PORT_IF_SR)) { 
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00, &data16));
            if ((data16 == WC40_UC_CTRL_DEFAULT) 
                || (data16 == WC40_UC_CTRL_SR4) 
                || (data16 == WC40_UC_CTRL_OPT_PF_TEMP_COMP)) {

#endif
        /* Only enable the feature for 10G/40G fiber */

#ifdef SW_RX_LOS_LINK_FLAP_WAR
        SOC_IF_ERROR_RETURN
            (soc_mac_probe(unit, port, &DEV_CFG_PTR(pc)->macd));
#endif

        DEV_CFG_PTR(pc)->sw_rx_los.enable = (value == 0? 0: 1);
#ifdef BCM_WARM_BOOT_SUPPORT
        if(!SOC_WARM_BOOT(unit))
#endif
        {
            DEV_CFG_PTR(pc)->sw_rx_los.sys_link = 0; 
            DEV_CFG_PTR(pc)->sw_rx_los.state = RESET;
            DEV_CFG_PTR(pc)->sw_rx_los.link_status = 0;
        }
        /* Manage the _SERVICE_INT_PHY_LINK_GET flag so that 
           if external phy is present link_get for 
           internal phy is still called to process 
           software rx los feature */ 
        if(value) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SERVICE_INT_PHY_LINK_GET);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_SERVICE_INT_PHY_LINK_GET);
        }
        rv = SOC_E_NONE;
        break;     
    case SOC_PHY_CONTROL_RX_PEAK_FILTER_TEMP_COMP:
 
        /* Get speed */
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            rv = _phy_wc40_combo_speed_get(unit, port, &speed,&intf,&scr);
        } else {
            rv = _phy_wc40_ind_speed_get(unit, port, &speed,&intf,&scr);
        }

        /* Get interface */
        phy_wc40_interface_get(unit, port, &interface);

        if( (speed == 40000) && (interface == SOC_PORT_IF_SR) ) {
            /* Puts PLL in reset state and forces all datapath
               into reset state */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00, 0,
                             XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
            if(value) {
                data16 = WC40_UC_CTRL_OPT_PF_TEMP_COMP;
            } else {
                data16 = WC40_UC_CTRL_SR4; /* default for SR4 */
            }
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, 0x00,
                                                      data16));
            /* Brings PLL out of reset state */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x00,
                             XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                             XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));


        } else if ( (speed == 10000) && ((interface == SOC_PORT_IF_SFI) 
                   || (interface == SOC_PORT_IF_SR)) ) {
            /* Do not apply for 10G SFP DAC */
            SOC_IF_ERROR_RETURN
                (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit,pc,LANE0_ACCESS,&data16));
            if (((data16 >> (pc->lane_num * 4)) & 0xf) == WC40_UC_CTRL_SFP_DAC) {
                return SOC_E_NONE;
            }
    
            WC40_CL73_AN_TX_FIFO_RESET_STOP(pc);
            
            /* Hold rx_asic_reset */
            mask16 = DIGITAL5_MISC6_RESET_RX_ASIC_MASK;
            data16 = mask16;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc, ln_access[pc->lane_num], 
                                             data16, mask16));
            mask16 = 0xf << (4*pc->lane_num);
            if( value) {
                /* Set the firmware mode to  OPT_PF_TEMP_COMP */
                data16 = WC40_UC_CTRL_OPT_PF_TEMP_COMP;  
            } else {
                /* Set the firmware mode to default */
                data16 = 0x0000; 
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit, pc, LANE0_ACCESS, 
                                                       data16, mask16));
            /* Release rx_asic_reset */
            mask16 = DIGITAL5_MISC6_RESET_RX_ASIC_MASK;
            data16 = 0;
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc, ln_access[pc->lane_num], 
                                             data16, mask16));

            WC40_CL73_AN_TX_FIFO_RESET_START(pc);
        } 
        /* rv = SOC_E_NONE; */
        break;
    case SOC_PHY_CONTROL_CL73_FSM_AUTO_RECOVER:
        DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.enable = value? 1 : 0;
        DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.fsm_state_prev = 0;
        DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.fsm_state_count = 0;
        DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.action_count_state02 = 0;
        DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.action_count_state04 = 0;
        DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.action_count_state08 = 0;
        DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.action_count_state10 = 0;
        DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.action_count_state20 = 0;

        rv = SOC_E_NONE;     
        break;
    case SOC_PHY_CONTROL_CL72:
        value = value? TRUE : FALSE;
        SOC_IF_ERROR_RETURN(
                phy_wc40_an_get(unit, port, &an, &an_done));
        if(!(an)) {
            /* Autoneg OFF */
            if (value != FORCE_CL72_IS_ENABLED(pc)) {
                SOC_IF_ERROR_RETURN
                    (_phy_wc40_force_cl72_config(unit,port,value));
                FORCE_CL72_ENABLE(pc) = value;
            }
        } else {
            /* Autoneg ON */
            SOC_IF_ERROR_RETURN(
                _phy_wc40_cl72_enable(unit, port, value));
        }
        rv = SOC_E_NONE;     
        break;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
        rv = _phy_wc40_firmware_mode_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_DFE:
        rv = _phy_wc40_firmware_dfe_mode_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_AUTONEG_MODE:
        rv = SOC_E_NONE;     
        SOC_IF_ERROR_RETURN(
                phy_wc40_an_get(unit, port, &an, &an_done));
        if(an) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "phy_wc40_control_set: PHY_CONTROL_AUTONEG_MODE "
                                 "-> disable autoneg u=%d p=%d\n"), unit, port));
            return SOC_E_FAIL;
        }
        switch(value) {
            case SOC_PHY_CONTROL_AUTONEG_MODE_CL37:
                DEV_CFG_PTR(pc)->cl37an = WC40_CL37_WO_CL37BAM;
                break;
            case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM:
                DEV_CFG_PTR(pc)->cl37an = WC40_CL37_AND_CL37BAM;
                break;
            case SOC_PHY_CONTROL_AUTONEG_MODE_CL73:
                DEV_CFG_PTR(pc)->cl73an = WC40_CL73_WO_CL73BAM;
                break;
            case SOC_PHY_CONTROL_AUTONEG_MODE_CL73_BAM:
                DEV_CFG_PTR(pc)->cl73an = WC40_CL73_AND_CL73BAM;
                break;
            case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_CL73:
                DEV_CFG_PTR(pc)->cl37an = WC40_CL37_WO_CL37BAM;
                DEV_CFG_PTR(pc)->cl73an = WC40_CL73_WO_CL73BAM;
                break;    
            case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_CL73_BAM:
                DEV_CFG_PTR(pc)->cl37an = WC40_CL37_WO_CL37BAM;
                DEV_CFG_PTR(pc)->cl73an = WC40_CL73_AND_CL73BAM;
                break;    
            case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73:
                DEV_CFG_PTR(pc)->cl37an = WC40_CL37_AND_CL37BAM;
                DEV_CFG_PTR(pc)->cl73an = WC40_CL73_WO_CL73BAM;
                break;    
            case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73_BAM:
                DEV_CFG_PTR(pc)->cl37an = WC40_CL37_AND_CL37BAM;
                DEV_CFG_PTR(pc)->cl73an = WC40_CL73_AND_CL73BAM;
                break;    
            default:
                rv = SOC_E_PARAM;
                break;    
        }
        break;        
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/* Make use of the TX0_ANATXACONTROL5.TX_PRBS_INV_DECOUPLE as a scratch pad for VCO status */
#define READ_WC40_TX0_ANATXACONTROL5r(_unit, _pc, _flags, _val) \
        WC40_REG_READ((_unit), (_pc), (_flags), 0x00008069, (_val))
#define WRITE_WC40_TX0_ANATXACONTROL5r(_unit, _pc, _flags, _val) \
        WC40_REG_WRITE((_unit), (_pc), (_flags), 0x00008069, (_val))
#define MODIFY_WC40_TX0_ANATXACONTROL5r(_unit, _pc, _flags, _val, _mask) \
        WC40_REG_MODIFY((_unit), (_pc), (_flags), 0x00008069, (_val), (_mask))

#define TX0_ANATXACONTROL5_VCO_DISTURBED_MASK                            0x0010
#define TX0_ANATXACONTROL5_VCO_DISTURBED_ALIGN                           0
#define TX0_ANATXACONTROL5_VCO_DISTURBED_BITS                            1
#define TX0_ANATXACONTROL5_VCO_DISTURBED_SHIFT                           4

STATIC int
_phy_wc40_control_vco_disturbed_get(int unit, soc_port_t port, uint32 *value)
{
    uint16 data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    if(!pc) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(READ_WC40_TX0_ANATXACONTROL5r(unit, pc, 0x00, &data));
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX0_ANATXACONTROL5r(unit, pc, 0x00, 0, (1 << TX0_ANATXACONTROL5_VCO_DISTURBED_SHIFT)));
    
    *value = (data & TX0_ANATXACONTROL5_VCO_DISTURBED_MASK) ? 1 : 0;
    
    return SOC_E_NONE;
}

STATIC int
_phy_wc40_control_vco_disturbed_set(int unit, soc_port_t port)
{
    uint16 data, mask;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    if(!pc) {
        return SOC_E_NONE;
    }
    data = 1 << TX0_ANATXACONTROL5_VCO_DISTURBED_SHIFT;
    mask = data;
    
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX0_ANATXACONTROL5r(unit, pc, 0x00, data, mask));
    
    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_wc40_control_get
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
phy_wc40_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    uint16 data16;
    int intf;
    int scr;
    int speed;
    phy_ctrl_t *pc;
    int cl37, cl73;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }
   
    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        /*passthru*/
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        /*passthru*/
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        /*passthru*/
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        /*passthru*/
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = _phy_wc40_control_preemphasis_get(unit, pc, type, value);
        break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /*passthru*/
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        /*passthru*/
    case SOC_PHY_CONTROL_DRIVER_POST2_CURRENT:    
        rv = _phy_wc40_control_tx_driver_get(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_wc40_control_prbs_polynomial_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_wc40_control_prbs_tx_invert_data_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /*passthru*/
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_wc40_control_prbs_enable_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = _phy_wc40_control_prbs_rx_status_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = _phy_wc40_rloop_get(unit,pc,value);
        break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        rv = _phy_wc40_fec_get(unit,pc,value);
        break;
    case SOC_PHY_CONTROL_CUSTOM1:
        *value = DEV_CFG_PTR(pc)->custom1;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_SCRAMBLER:
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            rv = _phy_wc40_combo_speed_get(unit, port, &speed,&intf,&scr);
        } else {
            rv = _phy_wc40_ind_speed_get(unit, port, &speed,&intf,&scr);
        }
        *value = scr;
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_wc40_control_linkdown_transmit_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        SOC_IF_ERROR_RETURN
            (READ_WC40_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0, &data16));
        *value = data16 & SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK?
                 TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_BERT_TX_PACKETS:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_RX_PACKETS:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_RX_ERROR_BITS:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_RX_ERROR_BYTES:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_RX_ERROR_PACKETS:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_PATTERN:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_PACKET_SIZE:
        /*passthru*/
    case SOC_PHY_CONTROL_BERT_IPG:
        rv = _phy_wc40_control_bert_get(unit,pc,type,value);
        break;
    case SOC_PHY_CONTROL_TX_PPM_ADJUST:
        SOC_IF_ERROR_RETURN(
                    (READ_WC40_XGXSBLK8_TX_PI_CONTROL3r(unit, pc, 0x00, &data16)));
        *value = data16;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER_TEMP_COMP:
        SOC_IF_ERROR_RETURN
            (READ_WC40_UC_INFO_B1_FIRMWARE_MODEr(unit,pc,
                                                 LANE0_ACCESS,&data16));
        if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
            *value = (data16 == WC40_UC_CTRL_OPT_PF_TEMP_COMP);
        } else {
            data16 = (data16 & (0xf << 4*pc->lane_num)) >> (4*pc->lane_num);
            *value = (data16 == WC40_UC_CTRL_OPT_PF_TEMP_COMP_PER_LANE);
        }
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_SOFTWARE_RX_LOS:
        *value = DEV_CFG_PTR(pc)->sw_rx_los.enable;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_CL73_FSM_AUTO_RECOVER:
        *value = DEV_CFG_PTR(pc)->cl73_fsm_auto_recover.enable;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
        rv = _phy_wc40_firmware_mode_get(unit, port, value);
        break;       
    case SOC_PHY_CONTROL_DFE:
        rv = _phy_wc40_firmware_dfe_mode_get(unit, port, value);
        break;       
    case SOC_PHY_CONTROL_VCO_FREQ:
      rv = _phy_wc40_control_vco_freq_get(unit, port, value);
        break;       
    case SOC_PHY_CONTROL_VCO_DISTURBED:
      rv = _phy_wc40_control_vco_disturbed_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_AUTONEG_MODE:
        rv = SOC_E_NONE;   
        cl37 = 0;
        cl73 = 0; 
        switch(DEV_CFG_PTR(pc)->cl37an) {
            case WC40_CL37_WO_CL37BAM:
                cl37 = SOC_PHY_CONTROL_AUTONEG_MODE_CL37;
                break;
            case WC40_CL37_AND_CL37BAM:
                cl37 = SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM;
                break;
            default: 
                break;    
        }
        switch(DEV_CFG_PTR(pc)->cl73an) {
            case WC40_CL73_WO_CL73BAM:
                cl73 = SOC_PHY_CONTROL_AUTONEG_MODE_CL73;
                break;
            case WC40_CL73_AND_CL73BAM:
                cl73 = SOC_PHY_CONTROL_AUTONEG_MODE_CL73_BAM;
                break;
            default: 
                break;    
        }

        if(cl37 && cl73) {
            if((cl37 == SOC_PHY_CONTROL_AUTONEG_MODE_CL37)
               && (cl73 == SOC_PHY_CONTROL_AUTONEG_MODE_CL73)) {
                return *value = SOC_PHY_CONTROL_AUTONEG_MODE_CL37_CL73;
            }     
            if((cl37 == SOC_PHY_CONTROL_AUTONEG_MODE_CL37)
               && (cl73 == SOC_PHY_CONTROL_AUTONEG_MODE_CL73_BAM)) {
                return *value = SOC_PHY_CONTROL_AUTONEG_MODE_CL37_CL73_BAM;
            }     
            if((cl37 == SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM)
               && (cl73 == SOC_PHY_CONTROL_AUTONEG_MODE_CL73)) {
                return *value = SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73;
            }     
            if((cl37 == SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM)
               && (cl73 == SOC_PHY_CONTROL_AUTONEG_MODE_CL73_BAM)) {
                return *value = SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM_CL73_BAM;
            }     
        }
        *value = cl37 ?  cl37 : cl73;
        break;        
    
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}     

typedef struct {
    uint16 start;
    uint16 end;
} WC40_LANE0_REG_BLOCK;

STATIC WC40_LANE0_REG_BLOCK wc40_lane0_reg_block[] = {
    {0x8050,0x80FE},  /* register in this block only valid at lane0 */
    {0x8000,0x8001},  /* valid at lane0 */
    {0x8015,0x8019}  /* valid at lane0 */
};

STATIC int
_wc40_lane0_reg_access(int unit, phy_ctrl_t *pc,uint16 reg_addr)
{
    int ix = 0;
    int num_blk;
    WC40_LANE0_REG_BLOCK * pBlk;

    num_blk = sizeof(wc40_lane0_reg_block)/sizeof(wc40_lane0_reg_block[0]);
    for (ix = 0; ix < num_blk; ix++) {
        pBlk = &wc40_lane0_reg_block[ix];
        if ((reg_addr >= pBlk->start) && (reg_addr <= pBlk->end)) {
            return TRUE;
        }
    }
    return FALSE;
}

STATIC int
phy_wc40_reg_aer_read(int unit, phy_ctrl_t *pc, uint32 flags,
                  uint32 phy_reg_addr, uint16 *phy_data)
{
    uint16 data;     
    uint32 lane;         /* lane to access, override the default lane */

    lane = flags & LANE_ACCESS_MASK;

    /* safety check */
    if ((lane > 4) || (lane == LANE_BCST)) {
        lane = LANE0_ACCESS;
    }

    /* check if register only available on lane0 */
    if (_wc40_lane0_reg_access(unit,pc,(uint16)(phy_reg_addr & 0xffff))) {
        lane = LANE0_ACCESS;
    }

    /* default lane override
     * register access is done in AER mode
     */
    if (lane) { 
        /* set the base address in multi mdio address mode */
        if (!(pc->flags & PHYCTRL_MDIO_ADDR_SHARE)) {
            pc->phy_id -= pc->lane_num;
        }
        phy_reg_addr |= ((lane - 1) << 16);
        
    } else { /* default lane access */ 
        if (pc->flags & PHYCTRL_MDIO_ADDR_SHARE) {
            phy_reg_addr |= (pc->lane_num << 16);
        }
    }

    /* don't set the lane number if it is access to AER ifself */
    if ((phy_reg_addr & 0xffff) == PHY_AER_REG) {
        phy_reg_addr &= 0xffff;
    }

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_read(unit, pc,  phy_reg_addr, &data));

    /* restore the mdio address for current lane in multi mdio address mode */
    if (lane) {
        if (!(pc->flags & PHYCTRL_MDIO_ADDR_SHARE)) {
            pc->phy_id += pc->lane_num;
        }
    }
    *phy_data = data;

    return SOC_E_NONE;
}

STATIC int
phy_wc40_reg_aer_write(int unit, phy_ctrl_t *pc, uint32 flags,
                   uint32 phy_reg_addr, uint16 phy_data)
{
    uint16               data;     
    int dxgxs;
    uint32 lane;

    dxgxs = DEV_CFG_PTR(pc)->dxgxs;
    data  = (uint16) (phy_data & 0x0000FFFF);

    lane = flags & LANE_ACCESS_MASK;

    /* safety check */
    if ((lane > 4) && (lane != LANE_BCST)) {
        lane = LANE0_ACCESS;
    }

    /* check if register only available on lane0 or should be done from lane0 */
    if ((_wc40_lane0_reg_access(unit,pc,(uint16)(phy_reg_addr & 0xffff))) || dxgxs) {
        lane = LANE0_ACCESS;
    }

    /* default lane override
     * register access is done in AER mode
     */
    if (lane) {
        /* set AER to broadcast two lanes, dxgxs=1 1st dxgxs, dxgxs=2 2nd  */
        if (dxgxs) {
            phy_reg_addr |= (WC_AER_BCST_OFS_STRAP + dxgxs) << 16;
        } else {
            if (lane == LANE_BCST) {
                phy_reg_addr |= (WC_AER_BCST_OFS_STRAP << 16);
            } else {
                phy_reg_addr |= ((lane - 1) << 16);
            }
        }
        if (!(pc->flags & PHYCTRL_MDIO_ADDR_SHARE)) {
            pc->phy_id -= pc->lane_num;
        }
    } else {
        if (pc->flags & PHYCTRL_MDIO_ADDR_SHARE) {
            phy_reg_addr |= (pc->lane_num << 16);
        }
    }

    /* don't set the lane number if it is access to AER ifself */
    if ((phy_reg_addr & 0xffff) == PHY_AER_REG) {
        phy_reg_addr &= 0xffff;
    }

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_write(unit, pc,  phy_reg_addr, data));

    /* restore the mdio address for current lane in multi mdio address mode */
    if (lane) {
        if (!(pc->flags & PHYCTRL_MDIO_ADDR_SHARE)) {
            pc->phy_id += pc->lane_num;
        }
    }

    return SOC_E_NONE;
}

STATIC int
phy_wc40_reg_aer_modify(int unit, phy_ctrl_t *pc, uint32 flags,
                    uint32 phy_reg_addr, uint16 phy_data,
                    uint16 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    uint16  tmp;

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_wc40_reg_aer_read(unit, pc,  flags, phy_reg_addr, &tmp));

    data &= mask;
    tmp &= ~(mask);
    tmp |= data;

    SOC_IF_ERROR_RETURN
        (phy_wc40_reg_aer_write(unit, pc,  flags, phy_reg_addr, tmp));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_wc40_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_wc40_reg_aer_read(unit, pc, 0x00, phy_reg_addr, &data));

    *phy_data = data;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_wc40_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_wc40_reg_aer_write(unit, pc, 0x00, phy_reg_addr, data));

    return SOC_E_NONE;
}  

/*
 * Function:
 *      phy_wc40_reg_modify
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_mo_data  - New value for the bits specified in phy_mo_mask
 *      phy_mo_mask  - Bit mask to modify
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_wc40_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_wc40_reg_aer_modify(unit, pc, 0x00, phy_reg_addr, data, mask));

    return SOC_E_NONE;
}

STATIC int
phy_wc40_probe(int unit, phy_ctrl_t *pc)
{
    uint16      serdes_id0;

    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESID_SERDESID0r(unit, pc, 0x00, &serdes_id0));

    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) !=
             SERDES_ID0_MODEL_NUMBER_WARPCORE) {
        return SOC_E_NOT_FOUND;
    }

    /* ask to allocate the driver specific descripor  */
    pc->size = sizeof(WC40_DEV_DESC_t);
    pc->dev_name = wc_device_name;
    return SOC_E_NONE;
}

STATIC int
phy_wc40_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    if (CUSTOM_MODE(pc)) {
        return SOC_E_NONE;
    }

    switch(event) {
    case phyEventSpeed:
    case phyEventStop:
    case phyEventResume:
    case phyEventAutoneg:
    case phyEventInterface:
        if (DEV_CFG_PTR(pc)->phy_passthru) {
            return SOC_E_NONE;
        }
        break;
    default:
        break;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_wc40_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_wc40_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_wc40_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_wc40_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_wc40_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        rv = (phy_wc40_an_set(unit, port, value));
        break;
    case phyEventTxFifoReset:
        rv = (_phy_wc40_tx_fifo_reset(unit, port, value));
        break;
    case phyEventMacLoopback:
        rv = (_phy_wc40_notify_mac_loopback(unit, port, value));
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

STATIC int
phy_wc40_linkup_evt (int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_wc40_linkup_evt: "
                         "u=%d p=%d\n"), unit, port));

    /* restore mld local fault configuration */
    if (WC40_CL73_SOFT_KR2(pc)) {
        if (WC40_AN_VALID(pc)) {
            if (WC40_AN_CHECK_TIMEOUT(pc)) {
                SOC_IF_ERROR_RETURN
                    (WC40_REG_MODIFY(unit, pc, 0x00, 0x842A,0,(1 << 5)));
                WC40_AN_VALID_RESET(pc);
            }
        }
    }

    /* TX fifo recenter workaround in aggregated mode */
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode) || IS_DUAL_LANE_PORT(pc)) {
        if (WC40_REVID_A0(pc) || WC40_REVID_A1(pc)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TXBERT_TXBERTCTRLr(unit,pc,0x00,
                       TXBERT_TXBERTCTRL_FIFO_RST_MASK,
                       TXBERT_TXBERTCTRL_FIFO_RST_MASK));
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_TXBERT_TXBERTCTRLr(unit,pc,0x00,
                       0,
                       TXBERT_TXBERTCTRL_FIFO_RST_MASK));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_wc40_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16       data16;
    phy_ctrl_t  *pc;
    int duplex_value;

    pc = INT_PHY_SW_STATE(unit, port);
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode) ||
        CUSTOMX_MODE(pc)) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, pc, 0x00, &data16));

    if (!(data16 & SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK)) {

        /* 1000X fiber mode, 100fx  */
            
        duplex_value = duplex? FX100_CONTROL1_FULL_DUPLEX_MASK:0;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_FX100_CONTROL1r(unit,pc,0x00,
                   duplex_value,
                   FX100_CONTROL1_FULL_DUPLEX_MASK));
        
        /* 1000X should always be full duplex */
        duplex = TRUE;
    }

    data16 = duplex? MII_CTRL_FD: 0;

    /* program the duplex setting */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, data16,MII_CTRL_FD));

    return SOC_E_NONE;
}

STATIC int
phy_wc40_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode) ||
        CUSTOMX_MODE(pc)) {
        *duplex = TRUE;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, pc, 0x00, &reg0_16));

    /* default to fiber mode duplex */
    *duplex = (reg0_16 & SERDESDIGITAL_STATUS1000X1_DUPLEX_STATUS_MASK)?
              TRUE:FALSE;

    if (reg0_16 & SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK) {

    /* retrieve the duplex setting in SGMII mode */
        SOC_IF_ERROR_RETURN
            (READ_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, &mii_ctrl));

        if (mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_WC40_COMBO_IEEE0_AUTONEGLPABILr(unit,pc,0x00,&reg0_16));

            /* make sure link partner is also in SGMII mode
             * otherwise fall through to use the FD bit in MII_CTRL reg
             */
            if (reg0_16 & MII_ANP_SGMII_MODE) {
                if (reg0_16 & MII_ANP_SGMII_FD) {
                    *duplex = TRUE;
                } else {
                    *duplex = FALSE;
                }
                return SOC_E_NONE;
            }
        }
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    } 

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_firmware_set
 * Purpose:
 *      write the given firmware to the uController's RAM 
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
phy_wc40_firmware_set(int unit, int port, int offset, uint8 *array,int datalen)
{
    return SOC_E_FAIL;
}

STATIC int
phy_wc40_firmware_load(int unit, int port, int offset, uint8 *array,int datalen)
{
    phy_ctrl_t  *pc;
    int rv;
    int i;
    int len;
    uint16 data16;
    uint16 mask16;
    uint16 ver;
    uint16 cksum = 0;
    int no_cksum;

    pc = INT_PHY_SW_STATE(unit, port);
    no_cksum = !(DEV_CFG_PTR(pc)->uc_cksum);

    rv = SOC_E_NONE;

    /* initialize the RAM */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_MICROBLK_COMMANDr(unit, pc, 0x00,
                                      MICROBLK_COMMAND_INIT_CMD_MASK));

   /* wait for init done */
    rv = _phy_wc40_regbit_set_wait_check(pc,WC40_MICROBLK_DOWNLOAD_STATUSr,
                     MICROBLK_DOWNLOAD_STATUS_INIT_DONE_MASK,1,2000000,0);

    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WC40 : uC init fails: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_TIMEOUT);
    }

    /* enable uC timers */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_MICROBLK_COMMAND2r(unit,pc,0x00,MICROBLK_COMMAND2_TMR_EN_MASK,
                    MICROBLK_COMMAND2_TMR_EN_MASK));

   /* starting RAM location */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_MICROBLK_ADDRESSr(unit, pc, 0x00, offset));

    if ((_phy_wc40_firmware_set_helper[unit] != NULL) && (DEV_CFG_PTR(pc)->load_mthd == 2)) {

        /* transfer size, 16bytes quantity*/
        if (datalen%16) {
            len = (((datalen/16)+1) * 16) -1;
        } else {
            len = datalen -1;
        }
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_MICROBLK_RAMWORDr(unit, pc, 0x00, len));

        /* allow external access from Warpcore */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_COMMAND3r(unit, pc, 0x00,
                            MICROBLK_COMMAND3_EXT_MEM_ENABLE_MASK,
                            MICROBLK_COMMAND3_EXT_MEM_ENABLE_MASK));
        WC40_UDELAY(1000);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_COMMAND3r(unit, pc, 0x00,
                            MICROBLK_COMMAND3_EXT_CLK_ENABLE_MASK,
                            MICROBLK_COMMAND3_EXT_CLK_ENABLE_MASK));
        WC40_UDELAY(1000);

        _phy_wc40_firmware_set_helper[unit](unit,port,array,datalen);

        /* restore back no external access from Warpcore */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_COMMAND3r(unit, pc, 0x00, 0,
                            MICROBLK_COMMAND3_EXT_MEM_ENABLE_MASK));
        WC40_UDELAY(1000);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_MICROBLK_COMMAND3r(unit, pc, 0x00, 0,
                            MICROBLK_COMMAND3_EXT_CLK_ENABLE_MASK));
        WC40_UDELAY(1000);

    } else {
        /* transfer size */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_MICROBLK_RAMWORDr(unit, pc, 0x00, datalen - 1));

        /* start write operation */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_MICROBLK_COMMANDr(unit, pc, 0x00,
                 MICROBLK_COMMAND_WRITE_MASK | MICROBLK_COMMAND_RUN_MASK));


        /* write 16-bit word to data register */
        for (i = 0; i < datalen/2; i++) {
            data16 = array[2*i] | (array[2*i+1] << 8);
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_MICROBLK_WRDATAr(unit, pc, 0x00, data16));
        }

        /* check if the firmware size is odd number */
        if (datalen%2) {
            data16 = array[datalen-1];
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_MICROBLK_WRDATAr(unit, pc, 0x00, data16));
        }
        /* complete the writing */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_MICROBLK_COMMANDr(unit, pc, 0x00, MICROBLK_COMMAND_STOP_MASK));
    }

    SOC_IF_ERROR_RETURN
            (READ_WC40_MICROBLK_DOWNLOAD_STATUSr(unit, pc, 0x00, &data16));

    mask16 = MICROBLK_DOWNLOAD_STATUS_ERR0_MASK | MICROBLK_DOWNLOAD_STATUS_ERR1_MASK;

    if (data16 & mask16) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WC40 : uC RAM download fails: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_FAIL);
    }

    /* write a non-zero value to this register to signal uC not perform cksum calculation */
    if (no_cksum) {
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_UC_INFO_B1_CRCr(unit, pc, 0x00, 0x1234));
    }

    /* release uC's reset */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_MICROBLK_COMMANDr(unit, pc, 0x00,
                            MICROBLK_COMMAND_MDIO_UC_RESET_N_MASK));

    /* wait for checksum to be written to this register by uC */

    if (!no_cksum) {
        rv = _phy_wc40_regbit_set_wait_check(pc,0x81fe,0xffff,1,100000,0);
        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "WC40 : uC download: u=%d p=%d timeout: wait for checksum\n"),
                      unit, port));
        } else {
           SOC_IF_ERROR_RETURN
            (READ_WC40_UC_INFO_B1_CRCr(unit, pc, 0x00, &cksum));
        } 
    } 

    SOC_IF_ERROR_RETURN
        (READ_WC40_UC_INFO_B1_VERSIONr(unit, pc, 0x00, &ver));

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "WC40 : uC RAM download success: u=%d p=%d ver=%x"), unit, port,ver));

    if (!no_cksum) {
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                " cksum=0x%x\n"), cksum));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "\n")));
    }
    return rv;
}

/*
 * Function:
 *      phy_wc40_medium_config_set
 * Purpose:
 *      set the configured medium the device is operating on.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - not used
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_wc40_medium_config_set(int unit, soc_port_t port,
                           soc_port_medium_t  medium,
                           soc_phy_config_t  *cfg)
{
    phy_ctrl_t       *pc;
    WC40_DEV_DESC_t  *pDesc;
    WC40_DEV_CFG_t   *pCfg;
    uint16           fiber_mode = 0;

    pc            = INT_PHY_SW_STATE(unit, port);
    pDesc = (WC40_DEV_DESC_t *)(pc + 1);
    pCfg = &pDesc->cfg;

    switch (medium) {
    case SOC_PORT_MEDIUM_COPPER:
        DEV_CFG_PTR(pc)->medium = SOC_PORT_MEDIUM_COPPER;
        fiber_mode = 0;
        break;
    case SOC_PORT_MEDIUM_FIBER:
        DEV_CFG_PTR(pc)->medium = SOC_PORT_MEDIUM_FIBER;
        fiber_mode = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
        break;
    default:
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, pc, 0x00, fiber_mode,
                           SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
    pCfg->fiber_pref = fiber_mode;

    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_wc40_medium_get
 * Purpose:
 *      Indicate the configured medium
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) One of:
 *              SOC_PORT_MEDIUM_COPPER
 *              SOC_PORT_MEDIUM_FIBER
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_wc40_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    phy_ctrl_t       *pc;

    if (medium == NULL) {
        return SOC_E_PARAM;
    }

    pc            = INT_PHY_SW_STATE(unit, port);
    *medium = DEV_CFG_PTR(pc)->medium;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_master_get
 * Purpose:
 *      this function is meant for 1000Base-T PHY. Added here to support
 *      internal PHY regression test
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The master mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_wc40_master_get(int unit, soc_port_t port, int *master)
{
    *master = SOC_PORT_MS_NONE;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_wc40_diag_ctrl
 * Purpose:
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 */

STATIC int
phy_wc40_diag_ctrl(
   int unit, /* unit */
   soc_port_t port, /* port */
   uint32 inst, /* the specific device block the control action directs to */
   int op_type,  /* operation types: read,write or command sequence */
   int op_cmd,   /* command code */
   void *arg)     /* command argument based on op_type/op_cmd */
{
    int rv;

    rv = SOC_E_NONE;

    switch(op_cmd) {
        case PHY_DIAG_CTRL_EYE_MARGIN_VEYE:
            (void)wc40_eye_margin(unit,port,WC_UTIL_VEYE);
        break;

        case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_LEFT:
            (void)wc40_eye_margin(unit,port,WC_UTIL_HEYE_L);
        break;

        case PHY_DIAG_CTRL_EYE_MARGIN_HEYE_RIGHT:
            (void)wc40_eye_margin(unit,port,WC_UTIL_HEYE_R);
        break;

        case PHY_DIAG_CTRL_DSC:
             LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "phy_wc40_diag_ctrl: "
                                  "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                       unit, port, PHY_DIAG_CTRL_DSC));
            (void)wc40_uc_status_dump(unit, port);
        break;

        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                rv = phy_wc40_control_set(unit, port, op_cmd, PTR_TO_INT(arg));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                rv = phy_wc40_control_get(unit, port, op_cmd, (uint32 *)arg);
            }
        break;
    }
    return rv;
}

/*
 * Variable:
 *      phy_wc40_drv
 * Purpose:
 *      Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */
phy_driver_t phy_wc40_hg = {
    /* .drv_name                      = */ "Warpcore PHY Driver",
    /* .pd_init                       = */ phy_wc40_init,
    /* .pd_reset                      = */ phy_null_reset,
    /* .pd_link_get                   = */ phy_wc40_link_get,
    /* .pd_enable_set                 = */ phy_wc40_enable_set,
    /* .pd_enable_get                 = */ phy_wc40_enable_get,
    /* .pd_duplex_set                 = */ phy_wc40_duplex_set,
    /* .pd_duplex_get                 = */ phy_wc40_duplex_get,
    /* .pd_speed_set                  = */ phy_wc40_speed_set,
    /* .pd_speed_get                  = */ phy_wc40_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_wc40_master_get,
    /* .pd_an_set                     = */ phy_wc40_an_set,
    /* .pd_an_get                     = */ phy_wc40_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */
    /* .pd_lb_set                     = */ phy_wc40_lb_set,
    /* .pd_lb_get                     = */ phy_wc40_lb_get,
    /* .pd_interface_set              = */ phy_wc40_interface_set,
    /* .pd_interface_get              = */ phy_wc40_interface_get,
    /* .pd_ability                    = */ NULL, /* Deprecated */
    /* .pd_linkup_evt                 = */ phy_wc40_linkup_evt,
    /* .pd_linkdn_evt                 = */ NULL,
    /* .pd_mdix_set                   = */ phy_null_mdix_set,
    /* .pd_mdix_get                   = */ phy_null_mdix_get,
    /* .pd_mdix_status_get            = */ phy_null_mdix_status_get,
    /* .pd_medium_config_set          = */ phy_wc40_medium_config_set,
    /* .pd_medium_config_get          = */ NULL, 
    /* .pd_medium_get                 = */ phy_wc40_medium_get,
    /* .pd_cable_diag                 = */ NULL,
    /* .pd_link_change                = */ NULL,
    /* .pd_control_set                = */ phy_wc40_control_set,    
    /* .pd_control_get                = */ phy_wc40_control_get,
    /* .pd_reg_read                   = */ phy_wc40_reg_read,
    /* .pd_reg_write                  = */ phy_wc40_reg_write, 
    /* .pd_reg_modify                 = */ phy_wc40_reg_modify,
    /* .pd_notify                     = */ phy_wc40_notify,
    /* .pd_probe                      = */ phy_wc40_probe,
    /* .pd_ability_advert_set         = */ phy_wc40_ability_advert_set,
    /* .pd_ability_advert_get         = */ phy_wc40_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_wc40_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_wc40_ability_local_get,
    /* .pd_firmware_set               = */ phy_wc40_firmware_set,
    /* .pdpd_timesync_config_set      = */ NULL,
    /* .pdpd_timesync_config_get      = */ NULL,
    /* .pdpd_timesync_control_set     = */ NULL,
    /* .pdpd_timesync_control_set     = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_wc40_diag_ctrl,
    /* .pd_lane_control_set           = */ NULL,
    /* .pd_lane_control_get           = */ NULL,
    /* .pd_lane_reg_read              = */ NULL,
    /* .pd_lane_reg_write             = */ NULL,
    /* .pd_oam_config_set             = */ NULL,
    /* .pd_oam_config_get             = */ NULL,
    /* .pd_oam_control_set            = */ NULL,
    /* .pd_oam_control_get            = */ NULL,
    /* .pd_timesync_enhanced_capture_get   = */ NULL,
    /* .pd_multi_get                       = */ NULL,
    /* .pd_precondition_before_probe       = */ NULL,
    /* .pd_linkfault_get                   = */ NULL,
    /* .pd_synce_clk_ctrl_set              = */ NULL,
    /* .pd_synce_clk_ctrl_get              = */ NULL,
    /* .pd_fec_error_inject_set              = */ NULL,
    /* .pd_fec_error_inject_get              = */ NULL
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _phy_wc40_notify_duplex
 * Purpose:
 *      Program duplex if (and only if) serdeswc40 is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, TRUE indicates full duplex, FALSE
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_wc40_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    fiber = DEV_CFG_PTR(pc)->fiber_pref;
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_notify_duplex: "
                         "u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00,
                                          MII_CTRL_FD, MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
        (_phy_wc40_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ? MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc, 0x00, mii_ctrl, MII_CTRL_FD));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_wc40_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if ((!PHY_SGMII_AUTONEG_MODE(unit, port)) && 
        (!DEV_CFG_PTR(pc)->phy_passthru)) {
        SOC_IF_ERROR_RETURN
            (phy_wc40_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wc40_notify_speed
 * Purpose:
 *      Program duplex if (and only if) serdeswc40 is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Speed to program.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_wc40_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int          fiber;
    uint16       sgmii_status;
    phy_ctrl_t  *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    fiber = DEV_CFG_PTR(pc)->fiber_pref;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_wc40_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESDIGITAL_STATUS1000X1r(unit, pc, 0x00, &sgmii_status));

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_wc40_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (phy_wc40_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_wc40_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && PHY_EXTERNAL_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_wc40_an_set(unit, port, FALSE));
    }

    /* Toggle TX FIFO reset if SGMII mode.
       Necessary to address traffic corruption
     */  
    if((speed <= 1000) && 
            (sgmii_status & SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00, 
                             DIGITAL5_MISC6_TX_OS8_FRST_SM_MASK, 
                             DIGITAL5_MISC6_TX_OS8_FRST_SM_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00, 
                             0, 
                             DIGITAL5_MISC6_TX_OS8_FRST_SM_MASK));
        LOG_VERBOSE(BSL_LS_SOC_PHY,
                    (BSL_META_U(unit,
                                "_phy_wc40_notify_speed: APPLY TX FIFO RST u=%d p=%d \n"),
                     unit, port));
    }


    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wc40_stop
 * Purpose:
 *      Put serdeswc40 SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_wc40_stop(int unit, soc_port_t port)
{
    int                 stop, copper;
    uint16 mask16,data16;
    phy_ctrl_t  *pc;
    int speed;

    pc = INT_PHY_SW_STATE(unit, port);

    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        return SOC_E_NONE;
    }

    /* 'Stop' only for speeds < 10G. 
     */  
    SOC_IF_ERROR_RETURN
        (phy_wc40_speed_get(unit,port,&speed));
    if(10000 <= speed) {
        return SOC_E_NONE;
    }

    copper = (pc->stop &
              PHY_STOP_COPPER) != 0;

    stop = ((pc->stop &
             (PHY_STOP_PHY_DIS |
              PHY_STOP_DRAIN)) != 0 ||
            (copper &&
             (pc->stop &
              (PHY_STOP_MAC_DIS |
               PHY_STOP_DUPLEX_CHG |
               PHY_STOP_SPEED_CHG)) != 0));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_wc40_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    /* tx/rx reset */
    mask16 = DIGITAL5_MISC6_RESET_RX_ASIC_MASK | DIGITAL5_MISC6_RESET_TX_ASIC_MASK;
    data16 = stop? mask16: 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL5_MISC6r(unit,pc,0x00, data16, mask16));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_wc40_stop: u=%d p=%d mask=0x%x value=0x%x\n"),
              unit, port, mask16, data16));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_wc40_notify_stop
 * Purpose:
 *      Add a reason to put serdeswc40 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_wc40_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;

    return _phy_wc40_stop(unit, port);
}

/*  
 * Function:
 *      _phy_wc40_notify_resume
 * Purpose:
 *      Remove a reason to put serdeswc40 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_wc40_notify_resume(int unit, soc_port_t port, uint32 flags)
{   
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;
    
    return _phy_wc40_stop(unit, port);
}

/*
 * Function:
 *      phy_wc40_media_setup
 * Purpose:     
 *      Configure 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      fiber_mode - Configure for fiber mode
 *      fiber_pref - Fiber preferrred (if fiber mode)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
_phy_wc40_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;
    
    pc = INT_PHY_SW_STATE(unit, port);

    data16 = 0;
    if (intf != SOC_PORT_IF_SGMII) {
        data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
 
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(unit, pc, 0x00, data16,
                           SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
 
    return SOC_E_NONE;
}

STATIC
int _wc40_xgmii_scw_config (int unit, phy_ctrl_t *pc)
{
    /* Dual Broadcast mode write */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_RX66_SCW0r(unit,pc,0x00,0xE070));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_RX66_SCW1r(unit,pc,0x00,0xC0D0));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_RX66_SCW2r(unit,pc,0x00,0xA0B0));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_RX66_SCW3r(unit,pc,0x00,0x8090));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_RX66_SCW0_MASKr(unit,pc,0x00,0xF0F0));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_RX66_SCW1_MASKr(unit,pc,0x00,0xF0F0));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_RX66_SCW2_MASKr(unit,pc,0x00,0xF0F0));
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_RX66_SCW3_MASKr(unit,pc,0x00,0xF0F0));
    return(SOC_E_NONE);
}

STATIC 
int _wc40_rxaui_config(int unit, phy_ctrl_t  *pc,int rxaui)
{
    uint16 mask16,data16;
    mask16 = XGXSBLK1_LANECTRL3_PWRDWN_FORCE_MASK |
                0xc |    /* lane 2 and 3 RX */
                (0xc << XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT);  /* TX */

    /* if in reduced XAUI mode, disable lane 2 and 3 */
    data16 = rxaui? mask16: 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK1_LANECTRL3r(unit,pc,0x00, data16,mask16));
    return(SOC_E_NONE);
}

STATIC
int _wc40_soft_reset(int unit, phy_ctrl_t  *pc)
{
   uint16 data16;
   uint16 mask16;

   data16 = XGXSBLK0_MMDSELECT_DEVCL22_EN_MASK |
                 XGXSBLK0_MMDSELECT_DEVDEVAD_EN_MASK |
                 XGXSBLK0_MMDSELECT_DEVPMD_EN_MASK  |
                 XGXSBLK0_MMDSELECT_DEVAN_EN_MASK |
                 XGXSBLK0_MMDSELECT_MULTIMMDS_EN_MASK;
   mask16 = data16 |
            XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK;

    if (DEV_CFG_PTR(pc)->lane0_rst) {
        /* reset entire device */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_XGXSBLK2_LANERESETr(unit,pc,LANE0_ACCESS, 
                          XGXSBLK2_LANERESET_RESET_MDIO_MASK,
                          XGXSBLK2_LANERESET_RESET_MDIO_MASK));
        
        /* wait 10us for reset to complete */
        WC40_UDELAY(10);
    }
    /* select multi mmd */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK0_MMDSELECTr(unit, pc, LANE0_ACCESS, data16,mask16));

    return(SOC_E_NONE);
}

STATIC
int _wc40_chip_init_done(int unit,int chip_num,int phy_mode)
{
    int port;
    phy_ctrl_t  *pc;

    PBMP_PORT_ITER(unit, port) {
        pc = INT_PHY_SW_STATE(unit, port);
        if (pc == NULL) {
            continue;
        }
        if (!pc->dev_name || pc->dev_name != wc_device_name) {
            continue;
        }
        if (pc->chip_num != chip_num) {
            continue;
        }

        if ((pc->flags & PHYCTRL_INIT_DONE) && (phy_mode == pc->phy_mode)) {
            return TRUE;
        }
    }
    return FALSE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC 
int wc40_wb_id_command(int unit, soc_port_t port, int offset, 
                         WC40_WB_HANDLER_TYPE_t type, WC40_WB_DATA_ID_t id, int *size) {
    phy_ctrl_t *pc;
    uint8 *wc_scache_ptr;
    int rv, vsize, loc_off;
    uint16 ver;
    
    pc    = INT_PHY_SW_STATE(unit, port);
    wc_scache_ptr = DEV_CFG_PTR(pc)->scache_ptr ;
    ver            = DEV_CFG_PTR(pc)->scache_ver ;
    vsize = 0 ; 
    loc_off = offset ;
    rv = SOC_E_NONE ;
    
    if(type==WC40_WB_SC_COUNT){
        /* no basic check at this moment */
    } else {
        /* check for ptr */
        if(wc_scache_ptr==NULL) {
            return rv ;
        }
    }
    switch(id) {
    case WC40_WB_SC_VER:
        if(ver >= BCM_WB_VERSION_1_0) {
            WC40_WB_ID_CMD(wc_scache_ptr, DEV_CFG_PTR(pc)->scache_ver,type,&loc_off,&vsize);
        }
        break ;
    case WC40_WB_RXLOS:
        if(ver >= BCM_WB_VERSION_1_0) {
            WC40_WB_ID_CMD(wc_scache_ptr, DEV_CFG_PTR(pc)->sw_rx_los.state, type,&loc_off,&vsize);
            WC40_WB_ID_CMD(wc_scache_ptr, DEV_CFG_PTR(pc)->sw_rx_los.sys_link, type,&loc_off,&vsize);
            WC40_WB_ID_CMD(wc_scache_ptr, DEV_CFG_PTR(pc)->sw_rx_los.link_status, type,&loc_off,&vsize);
        }
        break ;
    case WC40_WB_SC_LAST:
        break ;
    default:
        rv = SOC_E_PARAM ;
        break ;
    }
    *size = vsize ;
    return rv ;    
}

STATIC 
int wc40_wb_sc_handler(int unit, soc_port_t port, WC40_WB_HANDLER_TYPE_t type, WC40_WB_DATA_ID_t id) 
{
    int i, rv, size, offset ;
    rv   = SOC_E_NONE ; 
    size   = 0 ;
    offset = 0 ;
    
    for(i=0; i<WC40_WB_SC_LAST;i++) {
        if(i==id) {
            rv |= wc40_wb_id_command(unit,port, offset, type, id, &size) ;
            break ;
        } else {
            rv |= wc40_wb_id_command(unit, port, 0, WC40_WB_SC_COUNT, i, &size) ;
            offset += size ;
        }
    }

    return rv ;
}

STATIC 
int wc40_wb_sc_size_count(int unit, soc_port_t port, int *size) 
{
    int i, rv, total, sz ;
    rv   = SOC_E_NONE ;
    sz    = 0 ;
    total = 0 ;
    for(i=0; i<WC40_WB_SC_LAST;i++) {
        rv |= wc40_wb_id_command(unit, port, 0, WC40_WB_SC_COUNT, i, &sz) ;
        total += sz ;
    }
    *size = total ;

    return rv ;
}

STATIC 
int wc40_wb_sc_sync(int unit, soc_port_t port) 
{
    int i, rv, offset, size ;

    rv     = SOC_E_NONE ;
    size   = 0 ;
    offset = 0 ;
    for(i=0; i<WC40_WB_SC_LAST;i++) {
        rv |= wc40_wb_id_command(unit, port, offset, WC40_WB_SC_UPDATE, i, &size) ;
        offset += size ;
    }
    
    return rv ;
}

/* retrieve all variables from SCache */
STATIC 
int wc40_wb_sc_reinit(int unit, soc_port_t port) 
{
    int i, rv, offset, size ;
    soc_scache_handle_t scache_handle;
    uint8 *wc_scache_ptr;
    uint32 alloc_get;
    int    alloc_size ;
    phy_ctrl_t *pc;
    int    current_ver = BCM_WB_DEFAULT_VERSION;


    pc = INT_PHY_SW_STATE(unit, port);

    rv     = SOC_E_NONE ;
    size   = 0 ;
    offset = 0 ;
    alloc_size = 0 ;
    alloc_get  = 0 ;

    /* init default status */
    if(current_ver >= BCM_WB_VERSION_1_0) {
         DEV_CFG_PTR(pc)->sw_rx_los.sys_link = 0;
         DEV_CFG_PTR(pc)->sw_rx_los.state = RESET;
         DEV_CFG_PTR(pc)->sw_rx_los.link_status = 0;
     }


    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_SERDES_HANDLE, port);
    /* Get the pointer for the  cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &wc_scache_ptr, &alloc_get);
    
    if(rv) {
       DEV_CFG_PTR(pc)->scache_ptr = NULL ;
       DEV_CFG_PTR(pc)->scache_ver = 0 ;

       return rv ;
    } else {
        /* to find out the existing scache is an old version or not */
       if(alloc_get < sizeof(DEV_CFG_PTR(pc)->scache_ver)) {
           DEV_CFG_PTR(pc)->scache_ptr = NULL ;
           DEV_CFG_PTR(pc)->scache_ver = 0 ;
           return SOC_E_INTERNAL;
       } else {
           DEV_CFG_PTR(pc)->scache_ptr = wc_scache_ptr ;
           DEV_CFG_PTR(pc)->scache_size= alloc_size ;
           /* copy from tsc_scache_ptr to pCfg->scache_ver */
           sal_memcpy(&DEV_CFG_PTR(pc)->scache_ver, wc_scache_ptr, sizeof(DEV_CFG_PTR(pc)->scache_ver));
       }
       rv = wc40_wb_sc_size_count(unit, port, &alloc_size) ;
       if((alloc_get < alloc_size)||(alloc_get > (alloc_size+15))) {
            /* Expected size doesn't match retrieved size */
            DEV_CFG_PTR(pc)->scache_ptr = NULL ;
            DEV_CFG_PTR(pc)->scache_ver = 0 ;
            return SOC_E_INTERNAL;
       }
       rv = SOC_E_NONE ;
    }
    
    for(i=0; i<WC40_WB_SC_LAST;i++) {
        rv |= wc40_wb_id_command(unit, port, offset, WC40_WB_SC_RETRIEVE, i, &size) ;
        offset += size ;
    }

    return rv ;
}

/* initialize the space, only be called when !SOC_WARM_BOOT(unit) */
STATIC 
int wc40_wb_sc_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    soc_scache_handle_t scache_handle;
    uint8 *wc_scache_ptr;
    uint32 alloc_get =0;
    int    alloc_size =0;
    int    current_ver = BCM_WB_DEFAULT_VERSION;
    int rv = SOC_E_NONE;
    
    pc = INT_PHY_SW_STATE(unit, port);
        
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_SERDES_HANDLE, port);
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &wc_scache_ptr, &alloc_get);
    
    if (SOC_E_NOT_FOUND == rv) {
        DEV_CFG_PTR(pc)->scache_ver = current_ver ;
        rv = wc40_wb_sc_size_count(unit, port, &alloc_size) ;
        SOC_IF_ERROR_RETURN
            (soc_scache_alloc(unit, scache_handle, alloc_size));
        rv = soc_scache_ptr_get(unit, scache_handle,
                                &wc_scache_ptr, &alloc_get);

        if (SOC_FAILURE(rv)) {
            return rv;
        } else if ((alloc_get < alloc_size)||(alloc_get > (alloc_size+15))) {
            /* Expected size doesn't match retrieved size */
            return SOC_E_INTERNAL;
        } else if (NULL == wc_scache_ptr) {
            return SOC_E_MEMORY;
        }
        DEV_CFG_PTR(pc)->scache_ptr = wc_scache_ptr ;
        DEV_CFG_PTR(pc)->scache_size= alloc_size ;
        
        /* set up version */
        DEV_CFG_PTR(pc)->scache_ver = current_ver ;  
        sal_memcpy(wc_scache_ptr, &DEV_CFG_PTR(pc)->scache_ver, sizeof(DEV_CFG_PTR(pc)->scache_ver));
    } else if(SOC_E_PARAM ==rv) {
        DEV_CFG_PTR(pc)->scache_ver = 0 ;
        DEV_CFG_PTR(pc)->scache_ptr = NULL ;
        DEV_CFG_PTR(pc)->scache_size= 0 ;
    } else {
        /* mem space has been created already */
        DEV_CFG_PTR(pc)->scache_ver = current_ver ;
        rv = wc40_wb_sc_size_count(unit, port, &alloc_size) ;
        if((alloc_get < alloc_size)||(alloc_get > (alloc_size+15))) {
            /* Expected size doesn't match retrieved size */
            DEV_CFG_PTR(pc)->scache_ptr = NULL ;
            return SOC_E_INTERNAL;
        }
        DEV_CFG_PTR(pc)->scache_ptr = wc_scache_ptr ;
        DEV_CFG_PTR(pc)->scache_size= alloc_size ;
        DEV_CFG_PTR(pc)->scache_ver = current_ver ;    
        sal_memcpy(wc_scache_ptr, &DEV_CFG_PTR(pc)->scache_ver, sizeof(DEV_CFG_PTR(pc)->scache_ver));
    }
    
    return rv ;
}

STATIC int wc40_wb_sc_realloc_init(int unit, soc_port_t port) 
{
    phy_ctrl_t *pc;
    soc_scache_handle_t scache_handle;
    uint8 *wc_scache_ptr;
    uint32 alloc_get = 0;
    int    alloc_size1 = 0 ;
    int    alloc_size2 = 0;
    int    current_ver  = BCM_WB_DEFAULT_VERSION;
    int rv =  SOC_E_NONE;

    pc = INT_PHY_SW_STATE(unit, port);
        
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_SERDES_HANDLE, port);

    /* Get the pointer for the  cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &wc_scache_ptr, &alloc_get);
     if ( rv) {
        DEV_CFG_PTR(pc)->scache_ver = 0 ;
        DEV_CFG_PTR(pc)->scache_ptr = NULL ;
        DEV_CFG_PTR(pc)->scache_size= 0 ;
        }else {
        /* mem space has been created already */
        rv = wc40_wb_sc_size_count(unit, port, &alloc_size1) ;
        DEV_CFG_PTR(pc)->scache_ver = current_ver ;
        rv = wc40_wb_sc_size_count(unit, port, &alloc_size2) ;
        SOC_IF_ERROR_RETURN
            (soc_scache_realloc(unit, scache_handle, (alloc_size2-alloc_size1)));

        rv = soc_scache_ptr_get(unit, scache_handle,
                                &wc_scache_ptr, &alloc_get);
        if((alloc_get < alloc_size2)||(alloc_get > (alloc_size2+15))) {
            /* Expected size doesn't match retrieved size */
            DEV_CFG_PTR(pc)->scache_ptr = NULL ;
            return SOC_E_INTERNAL;
        }
        DEV_CFG_PTR(pc)->scache_ptr = wc_scache_ptr ;
        DEV_CFG_PTR(pc)->scache_size= alloc_size2 ;
        DEV_CFG_PTR(pc)->scache_ver = current_ver ;    
        sal_memcpy(wc_scache_ptr, &DEV_CFG_PTR(pc)->scache_ver, sizeof(DEV_CFG_PTR(pc)->scache_ver));
    }
    return rv ;   
}

#endif


/******************************************************************************
*              Begin Diagnostic Code                                          *
 */
#ifdef BROADCOM_DEBUG
int
_phy_wc40_cfg_dump(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    WC40_DEV_CFG_t *pCfg;
    WC40_DEV_INFO_t  *pInfo;
    WC40_DEV_DESC_t *pDesc;
    soc_phy_info_t *pi;
    int i;
    int size;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (WC40_DEV_DESC_t *)(pc + 1);
    pi = &SOC_PHY_INFO(unit, port);

    pCfg = &pDesc->cfg;
    pInfo = &pDesc->info;
    if (COMBO_LANE_MODE(DEV_CFG_PTR(pc)->lane_mode)) {
        size = 4;
    } else if (IS_DUAL_LANE_PORT(pc)) {
        size = 2;
    } else {
        size = 1;
    }
 
    LOG_CLI((BSL_META_U(unit,
                        "pc = 0x%x, pCfg = 0x%x, pInfo = 0x%x\n"), (int)(size_t)pc,
             (int)(size_t)pCfg,(int)(size_t)pInfo));
    for (i = 0; i < size; i++) { 
        LOG_CLI((BSL_META_U(unit,
                            "preemph%d     0x%x\n"),
                 i,pCfg->preemph[i]));
        LOG_CLI((BSL_META_U(unit,
                            "idriver%d     0x%04x\n"),
                 i, pCfg->idriver[i]));
        LOG_CLI((BSL_META_U(unit,
                            "pdriver%d     0x%04x\n"),
                 i, pCfg->pdriver[i]));
        LOG_CLI((BSL_META_U(unit,
                            "post2driver%d 0x%04x\n"),
                 i, pCfg->post2driver[i]));
    }
    LOG_CLI((BSL_META_U(unit,
                        "auto_medium  0x%04x\n"),
             pCfg->auto_medium));
    LOG_CLI((BSL_META_U(unit,
                        "fiber_pref   0x%04x\n"),
             pCfg->fiber_pref));
    LOG_CLI((BSL_META_U(unit,
                        "sgmii_mstr   0x%04x\n"),
             pCfg->sgmii_mstr));
    LOG_CLI((BSL_META_U(unit,
                        "pdetect10g   0x%04x\n"),
             pCfg->pdetect10g));
    LOG_CLI((BSL_META_U(unit,
                        "pdetect1000x 0x%04x\n"),
             pCfg->pdetect1000x));
    LOG_CLI((BSL_META_U(unit,
                        "cx42hg       0x%04x\n"), pCfg->cx42hg));
    LOG_CLI((BSL_META_U(unit,
                        "rxlane_map   0x%04x\n"), pCfg->rxlane_map));
    LOG_CLI((BSL_META_U(unit,
                        "txlane_map   0x%04x\n"), pCfg->txlane_map));
    LOG_CLI((BSL_META_U(unit,
                        "rxpol        0x%04x\n"), pCfg->rxpol));
    LOG_CLI((BSL_META_U(unit,
                        "txpol        0x%04x\n"), pCfg->txpol));
    LOG_CLI((BSL_META_U(unit,
                        "cl73an       0x%04x\n"), pCfg->cl73an));
    LOG_CLI((BSL_META_U(unit,
                        "phy_mode     0x%04x\n"), pc->phy_mode));
    LOG_CLI((BSL_META_U(unit,
                        "cx4_10g      0x%04x\n"), pCfg->cx4_10g));
    LOG_CLI((BSL_META_U(unit,
                        "lane0_rst    0x%04x\n"), pCfg->lane0_rst));
    LOG_CLI((BSL_META_U(unit,
                        "rxaui        0x%04x\n"), pCfg->rxaui));
    LOG_CLI((BSL_META_U(unit,
                        "dxgxs        0x%04x\n"), pCfg->dxgxs));
    LOG_CLI((BSL_META_U(unit,
                        "line_intf    0x%04x\n"), pCfg->line_intf));
    LOG_CLI((BSL_META_U(unit,
                        "hg_mode      0x%04x\n"), pCfg->hg_mode));
    LOG_CLI((BSL_META_U(unit,
                        "chip_num     0x%04x\n"), pc->chip_num));
    LOG_CLI((BSL_META_U(unit,
                        "lane_num     0x%04x\n"), pc->lane_num));
    LOG_CLI((BSL_META_U(unit,
                        "speedMax     0x%04x\n"), pc->speed_max));
    LOG_CLI((BSL_META_U(unit,
                        "pc->flags    0x%04x\n"), pc->flags));
    LOG_CLI((BSL_META_U(unit,
                        "pc->stop     0x%04x\n"), pc->stop));
    LOG_CLI((BSL_META_U(unit,
                        "pi->phy_flags   0x%04x\n"), pi->phy_flags));

    return SOC_E_NONE;
}
#endif

#define WC_UC_DEBUG
#ifdef WC_UC_DEBUG
typedef struct {
    int tx_pre_cursor;
    int tx_main;
    int tx_post_cursor;
    char *vga_bias_reduced;
    int postc_metric;
    int pf_ctrl;
    int vga_sum;
    int dfe1_bin;
    int dfe2_bin;
    int dfe3_bin;
    int dfe4_bin;
    int dfe5_bin;
    int integ_reg;
    int integ_reg_xfer;
    int clk90_offset;
    int slicer_target;
    int offset_pe;
    int offset_ze;
    int offset_me;
    int offset_po;
    int offset_zo;
    int offset_mo;
} UC_DESC;

#define MAX_LANES 4
static UC_DESC uc_desc[MAX_LANES];
static int lanes[MAX_LANES] = {LANE0_ACCESS,LANE1_ACCESS,LANE2_ACCESS,LANE3_ACCESS};

int
wc40_uc_status_dump(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    UC_DESC * pDesc;
    int i;
    int reg;
    uint16 data16;
    uint16 mask16; 
    int regval;
    pc = INT_PHY_SW_STATE(unit, port);
/*
 *Const DSC_1_ADDR = &H8200&
 *Const DSC_2_ADDR = &H8210&
 *Const DSC_3_ADDR = &H8220&
 *Const DSC_4_ADDR = &H8230&
 *Const DSC_5_ADDR = &H8240&
 * 0x822c,2d,2e,
*/
    WC40_MEM_SET((char *)&uc_desc[0], 0, (sizeof(UC_DESC))*MAX_LANES);
    for (i = 0; i < MAX_LANES; i++) {
        pDesc = &uc_desc[i];

        /* tx_pre_cursor */
        reg = 0x8063 + 0x10*i;      
        mask16 = (1 << 14) | (1 << 15);
        data16 = 1 << 14;
        SOC_IF_ERROR_RETURN
            (WC40_REG_MODIFY(unit, pc, LANE0_ACCESS, reg, data16,mask16));

        reg = 0x8060 + 0x10*i;      
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, LANE0_ACCESS, reg, &data16));
        regval = data16 & 0xF;
        pDesc->tx_pre_cursor = regval;

        /* tx_main */
        regval = (data16 >> 4) & 0x3F;
        pDesc->tx_main = regval;

        /* tx_post_cursor */
        regval = (data16 >> 10) & 0x1F;
        pDesc->tx_post_cursor = regval;

        /* restore 0x8063 */
        reg = 0x8063 + 0x10*i;      
        mask16 = (1 << 14) | (1 << 15);
        data16 = 0;
        SOC_IF_ERROR_RETURN
            (WC40_REG_MODIFY(unit, pc, LANE0_ACCESS, reg, data16,mask16));

        /* vga_bias_reduced */ 
        reg = 0x80BD + 0x10*i;      
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, LANE0_ACCESS, reg, &data16));
        regval = data16 & (1 << 4);
        if (regval) {
            pDesc->vga_bias_reduced = "88%";
        } else {
            pDesc->vga_bias_reduced = "100%";
        }
    }

    for (i = 0; i < MAX_LANES; i++) {
        pDesc = &uc_desc[i];
        /* postc_metric(lane) = rd22_postc_metric (phy, lane), DSC_5_ADDR=0x8240 */
        reg = 0x8241;      
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 0x7ff;
        if (regval >=1024) {
            regval -= 2048;
        } 
        pDesc->postc_metric = regval;

        /* pf_ctrl(lane) = rd22_pf_ctrl (phy, lane) DSC_3_ADDR=0x8220 */
        reg = 0x822b;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 0xf;
        pDesc->pf_ctrl = regval;

        /* vga_sum(lane) = rd22_vga_sum (phy, lane) DSC_3_ADDR=0x8220  */
        reg = 0x8225;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 63;
        pDesc->vga_sum = regval;

        /* dfe1_bin(lane) = rd22_dfe_tap_1_bin (phy, lane) DSC_3_ADDR=0x8220  */
        reg = 0x8225;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval /= 64; 
        regval &= 63;
        pDesc->dfe1_bin = regval;

        /* dfe2_bin(lane) = rd22_dfe_tap_2_bin (phy, lane) DSC_3_ADDR=0x8220  */
        reg = 0x8226;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        pDesc->dfe2_bin = regval;
        
        /* dfe3_bin(lane) = rd22_dfe_tap_3_bin (phy, lane) DSC_3_ADDR=0x8220  */
        reg = 0x8226;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval /= 64;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        pDesc->dfe3_bin = regval;
        
        /* dfe4_bin(lane) = rd22_dfe_tap_4_bin (phy, lane) DSC_3_ADDR=0x8220  */
        reg = 0x8227;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 31;
        if (regval >= 16) {
            regval -=32;
        }
        pDesc->dfe4_bin = regval;
  
        /* dfe5_bin(lane) = rd22_dfe_tap_5_bin (phy, lane) DSC_3_ADDR=0x8220  */
        reg = 0x8227;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval /= 32;
        regval &= 31;
        if (regval >= 16) {
            regval -=32;
        }
        pDesc->dfe5_bin = regval;

        /* integ_reg(lane) = rd22_integ_reg (phy, lane) DSC_3_ADDR=0x8220  */
        reg = 0x8220;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 65535;
        if (regval >= 32768) {
            regval -=65536;
        }
        regval /= 84;
        pDesc->integ_reg = regval;

        /* integ_reg_xfer(lane) = rd22_integ_reg_xfer (phy, lane)   */
        reg = 0x8221;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 65535;
        if (regval >= 32768) {
            regval -=65536;
        }
        pDesc->integ_reg_xfer = regval;

        /* clk90_offset(lane) = rd22_clk90_phase_offset (phy, lane)   */
        reg = 0x8223;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval /= 128;
        regval &= 127;
        pDesc->clk90_offset = regval;

        /* slicer_target(lane) = ((25*rd22_rx_thresh_sel (phy, lane)) + 125)   */
        reg = 0x821c;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval /= 32;
        regval &= 3;
        pDesc->slicer_target = regval * 25 + 125;

        /* offset_pe(lane) = rd22_slicer_offset_pe (phy, lane)   */
        reg = 0x822c;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        pDesc->offset_pe = regval;

        /* offset_ze(lane) = rd22_slicer_offset_ze (phy, lane)   */
        reg = 0x822d;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        pDesc->offset_ze = regval;

        /* offset_me(lane) = rd22_slicer_offset_me (phy, lane)   */
        reg = 0x822e;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        pDesc->offset_me = regval;

        /* offset_po(lane) = rd22_slicer_offset_po (phy, lane) */
        reg = 0x822c;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval /= 64;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        pDesc->offset_po = regval;

        /* offset_zo(lane) = rd22_slicer_offset_zo (phy, lane) */
        reg = 0x822d;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval /= 64;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        pDesc->offset_zo = regval;

        /* offset_mo(lane) = rd22_slicer_offset_mo (phy, lane) */
        reg = 0x822e;
        SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, lanes[i], reg, &data16));
        regval = data16;
        regval /= 64;
        regval &= 63;
        if (regval >= 32) {
            regval -=64;
        }
        pDesc->offset_mo = regval;
    }

    /* 0x8058, bits [13:6].  */
    SOC_IF_ERROR_RETURN
            (WC40_REG_READ(unit, pc, LANE0_ACCESS, 0x8058, &data16));
    data16 = (data16 >> 6) & 0xff;

    /* display */
    LOG_CLI((BSL_META_U(unit,
                        "\n\nDSC parameters for port %d\n\n"), port));
    LOG_CLI((BSL_META_U(unit,
                        "PLL Range: %d\n\n"),data16));

    LOG_CLI((BSL_META_U(unit,
                        "LN  PPM PPM_XFR clk90_ofs PF SL_TRGT VGA BIAS DFE1 DFE2 "
                        "DFE3 DFE4 DFE5 PREC MAIN POSTC MTRC PE   ZE   "
                        "ME  PO  ZO  MO\n")));
    for (i = 0; i < MAX_LANES; i++) {
        pDesc = &uc_desc[i];
        LOG_CLI((BSL_META_U(unit,
                            "%02d %04d %07d %09d %04d %04d %04d %4s %04d %04d %04d %04d "
                            "%04d %04d %04d %04d  %04d %04d %04d %2d %3d %3d %2d\n"),
                 i, pDesc->integ_reg, pDesc->integ_reg_xfer,
                 pDesc->clk90_offset,
                 pDesc->pf_ctrl, pDesc->slicer_target,
                 pDesc->vga_sum, pDesc->vga_bias_reduced,
                 pDesc->dfe1_bin, pDesc->dfe2_bin,
                 pDesc->dfe3_bin, pDesc->dfe4_bin,
                 pDesc->dfe5_bin, pDesc->tx_pre_cursor,
                 pDesc->tx_main, pDesc->tx_post_cursor,
                 pDesc->postc_metric, pDesc->offset_pe,
                 pDesc->offset_ze, pDesc->offset_me,
                 pDesc->offset_po, pDesc->offset_zo, pDesc->offset_mo));

    }
    return SOC_E_NONE;

}
#endif

/********   Eye Margin Diagnostic  **********/

#ifndef __KERNEL__
#include <math.h> 

/* #define BER_EXTRAP_SPD_UP */
#define MAX_LOOPS 47
#define MIN_RUNTIME  1  
#define INDEX_UNINITIALIZED  (-1)
#define SPD_UP_MAX_RUNTIME 64
#ifdef BER_EXTRAP_SPD_UP
#define MAX_RUNTIME SPD_UP_MAX_RUNTIME
#else
#define MAX_RUNTIME 256
#endif 
#define HI_CONFIDENCE_ERR_CNT 100    /* bit errors to determine high confidence */
#define LO_CONFIDENCE_MIN_ERR_CNT 10 /* bit errors, exit condition for low confidence */
#define HI_CONFIDENCE_MIN_ERR_CNT 20 /* bit errors exit condition for high confidence */
#define VEYE_UNIT 1.75
#define HEYE_UNIT 3.125
#define DSC1B0_UC_CTRLr 0x820e
#define WC_UTIL_MAX_ROUND_DIGITS (8) 

typedef struct {
    int total_errs[MAX_LOOPS];
    int total_elapsed_time[MAX_LOOPS];
    int mono_flags[MAX_LOOPS];
    int max_loops;
    int offset_max;
    int veye_cnt;
    uint32 rate;      /* frequency in KHZ */
    int first_good_ber_idx;
    int first_small_errcnt_idx;
} WC40_EYE_DIAG_INFOt;

static char *eye_test_name_str[] = {"Vertical Eye","Right Eye","Left Eye"};

STATIC int 
_wc40_veye_margin_data_get( int unit, int port, int lane, WC40_EYE_DIAG_INFOt *pInfo,int type); 
STATIC int  
_wc40_eye_margin_diagram_cal(WC40_EYE_DIAG_INFOt *pInfo,int type); 
STATIC int 
_wc40_eye_margin_ber_cal(WC40_EYE_DIAG_INFOt *pInfo,int type); 


#ifdef BROADCOM_DEBUG 
/* DEBUG function: manually construct a set of test data to test
 * extrapolation algorithem. 
 * type: 0 vertical eye, 1 horizontal right eye, 2 horizontal left eye
 * max_offset: the slicer max offset, for example 23
 * the clock rate is hardcoded to 10312500Khz
 */ 
STATIC WC40_EYE_DIAG_INFOt wc40_eye_test_data;

int wc40_eye_margin_test(int type, int max_offset) 
{
    WC40_EYE_DIAG_INFOt *pInfo;
    int cnt;

/*
 *  WC_UTIL_VEYE           0   vertical eye 
 *  WC_UTIL_HEYE_R         1   horizontal right eye 
 *  WC_UTIL_HEYE_L         2   horizontal left eye 
*/


    WC40_MEM_SET(&wc40_eye_test_data, 0, sizeof(wc40_eye_test_data));
    pInfo = &wc40_eye_test_data;
    pInfo->max_loops = MAX_LOOPS;
    pInfo->offset_max = max_offset; /*24 */
    cnt = 0;

    /* SFP-DAC test data from serdes team set 1 */
    pInfo->total_errs[cnt]         = 9786;
    pInfo->total_elapsed_time[cnt] = 1;
    cnt++;
    pInfo->total_errs[cnt]         = 10020;
    pInfo->total_elapsed_time[cnt] = 1;
    cnt++;
    pInfo->total_errs[cnt]         = 8443;
    pInfo->total_elapsed_time[cnt] = 1;
    cnt++;
    pInfo->total_errs[cnt]         = 8422;
    pInfo->total_elapsed_time[cnt] = 1;
    cnt++;
    pInfo->total_errs[cnt]         = 8312;
    pInfo->total_elapsed_time[cnt] = 1;
    cnt++;
    pInfo->total_errs[cnt]         = 144;
    pInfo->total_elapsed_time[cnt] = 4;
    cnt++;
    pInfo->total_errs[cnt]         = 100;
    pInfo->total_elapsed_time[cnt] = 8;
    cnt++;
    pInfo->total_errs[cnt]         = 107;
    pInfo->total_elapsed_time[cnt] = 32;
    cnt++;
    pInfo->total_errs[cnt]         = 31;
    pInfo->total_elapsed_time[cnt] = 64;
    cnt++;
    pInfo->total_errs[cnt]         = 3;
    pInfo->total_elapsed_time[cnt] = 64;
    cnt++;

    pInfo->veye_cnt   = cnt;
    pInfo->rate   = 10312500;
    pInfo->first_good_ber_idx = INDEX_UNINITIALIZED;
    pInfo->first_small_errcnt_idx = INDEX_UNINITIALIZED;

    SOC_IF_ERROR_RETURN
        (_wc40_eye_margin_ber_cal(pInfo,type)); 
    SOC_IF_ERROR_RETURN
        (_wc40_eye_margin_diagram_cal(pInfo,type));
    return SOC_E_NONE;
}
#endif /* end debug function */

int
wc40_eye_margin(int unit, soc_port_t port, int type)
{
    phy_ctrl_t      *pc;
    int lane_start;
    int lane_end;
    int i;
    WC40_EYE_DIAG_INFOt veye_info[4];

    pc = INT_PHY_SW_STATE(unit, port);

    /* clear the memory */
    WC40_MEM_SET(veye_info, 0, sizeof(veye_info));
    for (i = 0; i < 4; i++) {
         veye_info[i].first_good_ber_idx = INDEX_UNINITIALIZED;
         veye_info[i].first_small_errcnt_idx = INDEX_UNINITIALIZED;
    }

    if (pc->phy_mode == PHYCTRL_ONE_LANE_PORT || pc->phy_mode == PHYCTRL_SINGLE_LANE_IN_DUAL_PORT_MODE) {
        lane_start = pc->lane_num;
        lane_end   = lane_start;
    } else if (pc->phy_mode == PHYCTRL_DUAL_LANE_PORT) {
        lane_start = pc->lane_num;
        lane_end   = lane_start + 1;
    } else {
        lane_start = 0;
        lane_end   = 3;
    }
    LOG_CLI((BSL_META_U(unit,
                        "\nPort %d : Start BER extrapolation for %s\n"),
             port, eye_test_name_str[type]));
    LOG_CLI((BSL_META_U(unit,
                        "Port %d : Test time varies from a few minutes to "
                        "over 20 minutes. " 
                        "Please wait ...\n"), port));

    for (i = lane_start; i <= lane_end; i++) {
        if (lane_start != lane_end) {
            LOG_CLI((BSL_META_U(unit,
                                "\nStart test for lane %d\n"), i));
        }
        SOC_IF_ERROR_RETURN
            (_wc40_veye_margin_data_get(unit,port,i,&veye_info[i],type));
        SOC_IF_ERROR_RETURN
            (_wc40_eye_margin_ber_cal(&veye_info[i],type)); 
        SOC_IF_ERROR_RETURN
            (_wc40_eye_margin_diagram_cal(&veye_info[i],type));
    }

    return SOC_E_NONE;
}
int
_wc40_lane_prbs_rx_status_get(int unit, soc_port_t port, int lane, 
                                          uint32 *value)
{
    uint16      data;
    int prbs_cfg;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    *value = 0;
    if (DEV_CTRL_PTR(pc)->prbs.type == WC40_PRBS_TYPE_CL49) {
        data = 0;
        /* enable PRBS if not already. PRBS31 or PRBS9 */
        prbs_cfg = TRUE;
        SOC_IF_ERROR_RETURN
            (READ_WC40_PCS_IEEE2BLK_PCS_TPCONTROLr(unit, pc, ln_access[lane], &data));
        if (DEV_CTRL_PTR(pc)->prbs.poly == WC40_PRBS_CFG_POLY31) {
            if (data == WC40_PRBS_CL49_POLY31) { /* prbs already in PRBS31 */
                prbs_cfg = FALSE;
            }
        } else {
            DEV_CTRL_PTR(pc)->prbs.poly = WC40_PRBS_CFG_POLY31; /* only support PRBS31 for now*/
        }

        if (prbs_cfg) {
            SOC_IF_ERROR_RETURN
                (WRITE_WC40_PCS_IEEE2BLK_PCS_TPCONTROLr(unit, pc, ln_access[lane],
                        WC40_PRBS_CL49_POLY31));
        }

        SOC_IF_ERROR_RETURN
            (READ_WC40_CL49_USERB0_STATUSr(unit,pc,ln_access[lane],&data));

        if (data & CL49_USERB0_STATUS_PRBS_LOCK_MASK) { /* PRBS lock */
            /* check error count */
            SOC_IF_ERROR_RETURN
                (READ_WC40_PCS_IEEE2BLK_PCS_TPERRCOUNTERr(unit,pc,ln_access[lane],&data));
            *value = data;

        } else { /* PRBS not in sync */
            *value = -1;
        }
        return SOC_E_NONE;
    }

    /* Get status for all 4 lanes and check for sync
     * 0x80b0, 0x80c0, 0x80d0, 0x80e0
     */
    SOC_IF_ERROR_RETURN
        (WC40_REG_READ(unit, pc, LANE0_ACCESS, 0x80b0 + (0x10 * lane), &data));

    if (data == (RX0_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_BITS  <<
                     RX0_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_SHIFT)) {
        /* PRBS is in sync and error free */
    } else if (!(data & RX0_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_MASK)) {
        /* PRBS not in sync */
        *value = -1;
    } else if (data & RX0_ANARXSTATUS_PRBS_STATUS_PRBS_STKY_MASK) {
        /* PRBS momentarily lose sync, error cnt not accurate */
        *value = -2;
    } else {
        /* Get errors */
        *value = data & RX0_ANARXSTATUS_PRBS_STATUS_PTBS_ERRORS_MASK;
    }
    return SOC_E_NONE;
}

STATIC int 
_wc40_avg_vga_dfe( int unit, int port, int lane, int tapsel, int *avg_value) 
{
    uint16 vga_frzval;
    uint16 vga_frcfrz;
    uint16 dfe_frzval;
    uint16 dfe_frcfrz;
    uint16 frzval;
    uint16 frcfrz;
    int val_min = 0;
    int val_max = 0;
    int val_avg;
    int val_curr = 0;
    uint16 avg_cnt;   
    uint16 loop_var;
    uint16 data16;
    int rem;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

   avg_cnt = 40;
   val_avg = 0;

    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC2B0_ACQ_SM_CTRL1r(unit, pc, ln_access[lane], &data16));
    vga_frzval = (data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK) >>
                 DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_SHIFT;
    vga_frcfrz = (data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK) >>
                  DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_SHIFT;
    dfe_frzval = (data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK) >>
                  DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT;
    dfe_frcfrz = (data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK) >>
                  DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_SHIFT;

   if (tapsel == 0) {
       frzval = vga_frzval;
       frcfrz = vga_frcfrz;
       val_min = 47;
       val_max = 0;
   } else {
       frzval = (dfe_frzval & (1 << (tapsel-1)))/(1 << (tapsel-1));
       frcfrz = dfe_frcfrz; 
       if (tapsel == 1) {
           val_min = 63;
           val_max = 0;
       } else if ((tapsel == 2) || (tapsel == 3)) {
           val_min = 31;
           val_max = -31;
       } else if ((tapsel == 4) || (tapsel == 5)) {
           val_min = 15;
           val_max = -15;
       } 
   } 

   if ((frzval == 1) && (frcfrz == 1)) {
       avg_cnt = 1;
   } 

   /* Const DSC_3_ADDR = &H8220 */
   for (loop_var = 0; loop_var < avg_cnt; loop_var++) {
       switch(tapsel) {
           case 0:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(unit, pc,
                       ln_access[lane], &data16));
               val_curr = data16 & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK;
               break;

           case 1:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(unit, pc,
                       ln_access[lane], &data16));
               val_curr = (data16 & DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_MASK) >>
                          DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_SHIFT;
               break;

           case 2:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS1r(unit, pc,
                       ln_access[lane], &data16));
               val_curr = data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK;
               if (val_curr >= 32) {
                   val_curr -= 64;
               }
               break;

           case 3:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS1r(unit, pc,
                       ln_access[lane], &data16));
               val_curr = (data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >>
                          DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT;
               if (val_curr >= 32) {
                   val_curr -= 64;
               }
               break;

           case 4:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS2r(unit, pc,
                       ln_access[lane], &data16));
               val_curr = data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK;
               if (val_curr >= 16) {
                   val_curr -= 32;
               }
               break;

           case 5:
               SOC_IF_ERROR_RETURN
                   (READ_WC40_DSC3B0_DFE_VGA_STATUS2r(unit, pc,
                       ln_access[lane], &data16));
               val_curr = (data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >>
                          DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT;
               if (val_curr >= 16) {
                   val_curr -= 32;
               }
               break;

           default:
               break;
      } 

      LOG_INFO(BSL_LS_SOC_PHY,
               (BSL_META_U(unit,
                           "WC_VEYE : val_curr = %d, val_avg = %d, tap_select = %d\n"), 
                val_curr,val_avg,tapsel));
      val_avg = (val_avg + val_curr);

      if (val_curr < val_min) {
          val_min = val_curr;
      }

      if (val_curr > val_max) {
          val_max = val_curr;
      } 
      /* delay 20ms */
      sal_usleep(20000);
   } /* end for loop */ 

   /* average value */
   rem = val_avg%avg_cnt;
   val_avg = val_avg/avg_cnt;
   
   /* add one if remainder is more than half */ 
   val_avg += (2*rem)/avg_cnt;
   *avg_value = val_avg;

   return SOC_E_NONE;
}

STATIC int 
_wc40_veye_margin_data_get( int unit, int port, int lane, WC40_EYE_DIAG_INFOt *pInfo,int type) 
{
    int runtime_loop;
    int max_runtime_loop;
    int curr_runtime = 0;
    int offset[MAX_LOOPS];
    int offset_max;
    int loop_var;
    int veye_cnt;
    int hi_confidence_cnt;
    uint32 prbs_status;
    phy_ctrl_t *pc;
    int rv = SOC_E_NONE;
    uint16 data16;
    uint16 vga_frzval;
    uint16 vga_frcfrz;
    uint16 dfe_frzval;
    uint16 dfe_frcfrz;
    int pll_div[16] = {32,36,40,42,48,50,52,54,60,64,66,68,80,120,200,240};
    int ref_clk_freq[8] = {25000,100000,125000,156250,187500,161250,50000,106250};
    uint16 clk90_p_offset;
    int dfe_vga[6];
    int tmp;
    int max_total_time;
    int prbs_stky_flag;
    int prbs_stky_cnt;
    int max_stky_cnt;


    pc = INT_PHY_SW_STATE(unit, port);

    /* check what rate the test will run for */
    SOC_IF_ERROR_RETURN
        (READ_WC40_PLL_ANAPLLSTATUSr(unit,pc,ln_access[0],&data16));

    /* get the ref_clk divider first */
    pInfo->rate = pll_div[data16 & PLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK];

    /* get the ref_clk frequency frequency */    
    SOC_IF_ERROR_RETURN
        (READ_WC40_SERDESDIGITAL_MISC1r(unit,pc,ln_access[0],&data16));
    data16 = (data16 >> SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) & 7;
    pInfo->rate *= ref_clk_freq[data16];

    max_total_time = MAX_RUNTIME;

    /* max loop_cnt for BER test with test time doubling every iteration */
    /* max_runtime_loop = log(MAX_RUNTIME/MIN_RUNTIME)/log(2); */  
    max_runtime_loop = 80;  
    pInfo->max_loops = MAX_LOOPS;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "WC_VEYE : max_runtime_loop: %d u=%d p=%d\n"), 
              max_runtime_loop,unit, port));

    /* Initialize BER array */
    for( loop_var = 0; loop_var < pInfo->max_loops; loop_var++ ) {
        pInfo->total_errs[loop_var] = 0;
    }

    /* 0x820d[0]=1 enable diagnostic */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(unit, pc, ln_access[lane], 
               DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
               DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    /* save original DFE/VGA settings */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC2B0_ACQ_SM_CTRL1r(unit, pc, ln_access[lane], &data16));
    vga_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK; 
    vga_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK;
    dfe_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK; 
    dfe_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK;

    for (loop_var = 0; loop_var <= 5; loop_var++) {
        (void)_wc40_avg_vga_dfe (unit, port,lane, loop_var,&dfe_vga[loop_var]);
        if (loop_var == 0) {
            /* freeze VGA */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(unit, pc, ln_access[lane], 
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));

            /* select VGA */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(unit, pc, ln_access[lane], 
                  loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

            /* set the VGA value */
            /* coverity[uninit_use:FALSE] */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(unit, pc, ln_access[lane], 
                   dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

            /* VGA/DFE write enable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(unit, pc, ln_access[lane], 
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

            /* VGA/DFE write disable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(unit, pc, ln_access[lane], 
                  0,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
        }
    }
    /* freeze the DFE */ 
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(unit, pc, ln_access[lane], 
          (0x1f << DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT) |
          DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK,
          DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK |
          DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

    for (loop_var = 1; loop_var <= 5; loop_var++) {
        /* set DFE taps */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(unit, pc, ln_access[lane], 
              loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

        /* set the DFE tap value */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(unit, pc, ln_access[lane], 
               dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

        /* VGA/DFE write enable */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(unit, pc, ln_access[lane], 
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

        /* VGA/DFE write disable */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(unit, pc, ln_access[lane], 
              0,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
    }

    /* display the average value written to VGA/DFE */ 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "WC_EYE avg dfe/vga: u=%d p=%d, VGA:0x%x, dfe1:0x%x, dfe2:0x%x, dfe3:0x%x, "
                         "dfe4:0x%x, dfe5:0x%x\n"), unit, port, dfe_vga[0],dfe_vga[1],
              dfe_vga[2],dfe_vga[3],dfe_vga[4],dfe_vga[5]));

#if 0   
    /* first time, uC seems always indicating not ready */
    /* wait for uC ready for command bit7 */
    rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                     DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT,ln_access[lane]);

    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WC_EYE : uController not ready!!!: u=%d p=%d\n"), unit, port));
        return (SOC_E_TIMEOUT);
    }
#endif

    if (type == WC_UTIL_HEYE_L) {
        /* Write max to get left eye offset range */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], 0x7f02));

    } else if (type == WC_UTIL_HEYE_R) {
        /* Write min to get right eye offset range */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane],0x8002));

    } else {  /* vertical eye */
        /* write max to get vertical offset range */
        SOC_IF_ERROR_RETURN
            (WRITE_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], 0x7f03));
    }

    /* wait for uC ready for command:  bit7=1 */
    rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                     DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT,ln_access[lane]);

    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WC_EYE : uController not ready pass 1!!!: u=%d p=%d\n"), 
                  unit, port));
        return (SOC_E_TIMEOUT);
    }

    /* read out the max value */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], &data16));
    offset_max = (data16 >> 8) & 0xff;

    if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
        offset_max -= 4;
        pInfo->offset_max = offset_max;
    } else {
        pInfo->offset_max = offset_max;
    }   
    
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC2B0_ACQ_SM_CTRL1r(unit, pc, ln_access[lane], &data16));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "WC_EYE : offset_max %d DSC2B0_ctrl 0x%x u=%d p=%d\n"), 
              offset_max,data16,unit, port));

    if (offset_max >= MAX_LOOPS) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "WC_EYE ERROR: offset_max %d greater than MAX %d  u=%d p=%d\n"), 
                   offset_max,MAX_LOOPS,unit, port));
        return SOC_E_NONE;
    }
 
    sal_usleep(10000);  /* 10ms */

    hi_confidence_cnt = 0;
    veye_cnt = 0;

    for (loop_var = 0; loop_var < offset_max; loop_var++) { 
        offset[loop_var] = offset_max-loop_var;
        veye_cnt += 1;
        /*
         * Set a offset
         */
        if (type == WC_UTIL_HEYE_R) {
            data16 =-offset[loop_var];
        } else {
            data16 = offset[loop_var];
        }
        /* write vertical offset */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], 
                      data16 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                      DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));

        /* 0x8223 register read out */
        SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_PI_STATUS0r(unit, pc, ln_access[lane], &clk90_p_offset));
        clk90_p_offset >>= 7;
        clk90_p_offset &= 0x7f;
        tmp = (short)data16;
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Starting BER measurement at offset: %d clk90_p_offset: 0x%x u=%d p=%d\n"),
                  tmp,clk90_p_offset,unit, port));

        if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
            data16 = 2;
        } else {
            data16 = 3;
        }

        /* set offset cmd */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], 
                  data16,
                  DSC1B0_UC_CTRL_GP_UC_REQ_MASK));

        /* wait for uC ready for command:  bit7=1 */
        rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                         DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT,ln_access[lane]);

        if (rv == SOC_E_TIMEOUT) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "WC_EYE : uC waits for offset fail!!!: u=%d p=%d\n"), 
                      unit, port));
            return (SOC_E_TIMEOUT);
        }

        pInfo->total_errs[loop_var] = 0;
        pInfo->total_elapsed_time[loop_var] = 0;

        /* PRBS should be enabled before this function called. clear PRBS error. read a few times */
        SOC_IF_ERROR_RETURN
            ( _wc40_lane_prbs_rx_status_get(unit,port, lane,&prbs_status));
        SOC_IF_ERROR_RETURN
            ( _wc40_lane_prbs_rx_status_get(unit,port, lane,&prbs_status));
        SOC_IF_ERROR_RETURN
            ( _wc40_lane_prbs_rx_status_get(unit,port, lane,&prbs_status));

        for (runtime_loop = 0; runtime_loop <= max_runtime_loop; runtime_loop++) {
            if (runtime_loop == 0) {
                curr_runtime = 1 * MIN_RUNTIME;
            } else if (curr_runtime < (4 * MIN_RUNTIME)) {
                curr_runtime = (1 << (runtime_loop - 1)) * MIN_RUNTIME;
            }
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Starting prbs run for %d seconds : u=%d p=%d\n"), curr_runtime,
                      unit, port));
            /* XXX total_errs[loop_var] +=Round(check_prbs(phy, lane, curr_runtime)/2);*/

            prbs_stky_flag = 1;
            prbs_stky_cnt = 0;
            if (curr_runtime < 4) {
                max_stky_cnt = 4;
            } else {
                max_stky_cnt = 2;
            }

            while (prbs_stky_flag) {
                /* wait for specified amount of time to collect the PRBS error */
                sal_usleep(curr_runtime * 1000000);

                SOC_IF_ERROR_RETURN
                    ( _wc40_lane_prbs_rx_status_get(unit,port, lane,&prbs_status));

                prbs_stky_flag = 0;
                if (prbs_status == -1) { /* not lock */
                      LOG_WARN(BSL_LS_SOC_PHY,
                               (BSL_META_U(unit,
                                           "PRBS not locked, loop_num %d status=%d u=%d p=%d\n"), 
                                loop_var,prbs_status,unit, port));
                    prbs_status = 0x7fff;  /* max error */
                } else if (prbs_status == -2) {
                      LOG_WARN(BSL_LS_SOC_PHY,
                               (BSL_META_U(unit,
                                           "PRBS momentarily lost lock, loop_num %d u=%d p=%d\n"), 
                                loop_var,unit, port));
                    /* legacy PRBS momentarily lose link, retry limited times */
                    prbs_stky_cnt += 1;
                    if (prbs_stky_cnt < max_stky_cnt) { 
                        prbs_stky_flag = 1;
                    } else {
                        prbs_status = 0x7fff; /* default if retry fails */
                    }
                }
            }
 
            pInfo->total_errs[loop_var] +=  prbs_status/2;
            pInfo->total_elapsed_time[loop_var] +=  curr_runtime;

            if ((pInfo->total_errs[loop_var] >= HI_CONFIDENCE_ERR_CNT) ||
                (pInfo->total_elapsed_time[loop_var] >= max_total_time)) {
                 LOG_INFO(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "WC_EYE: done PRBS err count: u=%d p=%d, total_err: %d"
                                      " elapsed_time: %d, loop_num: %d\n"), unit, port,
                           pInfo->total_errs[loop_var],pInfo->total_elapsed_time[loop_var],loop_var));
                break;
            }
        }

        /* Determine high-confidence iterations */
        if (pInfo->total_errs[loop_var] >= HI_CONFIDENCE_ERR_CNT) {
            hi_confidence_cnt = hi_confidence_cnt + 1;
        } 

        if (((hi_confidence_cnt >= 2) && (pInfo->total_errs[loop_var] < 
             HI_CONFIDENCE_MIN_ERR_CNT)) || 
            ((hi_confidence_cnt <  2) && 
             (pInfo->total_errs[loop_var] < LO_CONFIDENCE_MIN_ERR_CNT)) ) {
             break;  /* exit for loop */
        }
    }   /* for loop_var */

    /* Undo setup */
    if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
        data16 = 2;
    } else {
        data16 = 3;
    }
 
    /* set vertical offset back to 0 */
    SOC_IF_ERROR_RETURN
        (WRITE_WC40_DSC1B0_UC_CTRLr(unit, pc, ln_access[lane], data16));

    /* wait for uC ready for command:  bit7=1 */
    rv = _phy_wc40_regbit_set_wait_check(pc,DSC1B0_UC_CTRLr,
                     DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,WC40_PLL_WAIT,ln_access[lane]);

    if (rv == SOC_E_TIMEOUT) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WC_VEYE : uC waits for offset=0 fail!!!: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_TIMEOUT);
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(unit, pc, ln_access[lane],
                      vga_frzval,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(unit, pc, ln_access[lane],
                      dfe_frzval,
                      DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(unit, pc, ln_access[lane],
                      vga_frcfrz,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(unit, pc, ln_access[lane],
                      dfe_frcfrz,
                      DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

    /* disable diagnostics */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(unit, pc, ln_access[lane],
               0,
               DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    /* Clear prbs monitor */
    SOC_IF_ERROR_RETURN
        ( _wc40_lane_prbs_rx_status_get(unit,port, lane,&prbs_status));

    pInfo->veye_cnt = veye_cnt;

    return SOC_E_NONE;
}

#ifdef COMPILER_HAS_DOUBLE
STATIC COMPILER_DOUBLE
_wc40_util_round_real( COMPILER_DOUBLE original_value, int decimal_places ) 
{
    COMPILER_DOUBLE shift_digits[WC_UTIL_MAX_ROUND_DIGITS+1] = { 0.0, 10.0, 100.0, 1000.0, 
                          10000.0, 100000.0, 1000000.0, 10000000.0, 100000000.0 };
    COMPILER_DOUBLE shifted;
    COMPILER_DOUBLE rounded;   
    if (decimal_places > WC_UTIL_MAX_ROUND_DIGITS ) {
        LOG_CLI((BSL_META("ERROR: Maximum digits to the right of decimal for rounding "
                          "exceeded. Max %d, requested %d\n"), 
                 WC_UTIL_MAX_ROUND_DIGITS, decimal_places));
        return 0.0;
    } 
    /* shift to preserve the desired digits to the right of the decimal */   
    shifted = original_value * shift_digits[decimal_places];

    /* convert to integer and back to COMPILER_DOUBLE to truncate undesired precision */
    shifted = (COMPILER_DOUBLE)(floor(shifted+0.5));

    /* shift back to place decimal point correctly */   
    rounded = shifted / shift_digits[decimal_places];
    return rounded;
}
#endif
/*
 * enable PRBS31 on both ports before executing this function
 * Example:
 * phy prbs xe0,xe1 set mode=hc p=3
 */
STATIC int 
_wc40_eye_margin_ber_cal(WC40_EYE_DIAG_INFOt *pInfo, int type) 
{
#ifdef COMPILER_HAS_DOUBLE

    COMPILER_DOUBLE bers[MAX_LOOPS]; /* computed bit error rate */
    COMPILER_DOUBLE margins[MAX_LOOPS]; /* Eye margin @ each measurement*/
    int loop_var;
    COMPILER_DOUBLE eye_unit;
    COMPILER_DOUBLE curr_ber_log;
    COMPILER_DOUBLE prev_ber_log = 0;
    COMPILER_DOUBLE good_ber_level = -7.8;

    /* Initialize BER array */
    for( loop_var = 0; loop_var < MAX_LOOPS; loop_var++ ) {
        bers[loop_var] = 0.0;
    }

    if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
        eye_unit = HEYE_UNIT;
    } else {
        eye_unit = VEYE_UNIT;
    }
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META("\nBER measurement at each offset, num_data_points: %d\n"),
pInfo->veye_cnt));

    for (loop_var = 0; loop_var < pInfo->veye_cnt; loop_var++) { 
        margins[loop_var] = (pInfo->offset_max-loop_var)*eye_unit;
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("BER measurement at offset: %f\n"), margins[loop_var]));
             
        /* Compute BER */
        if (pInfo->total_errs[loop_var] == 0) { 
            bers[loop_var] = 1.0/(COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_var]/pInfo->rate;
            bers[loop_var] /= 1000;

            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META("BER @ %04f %% = 1e%04f (%d errors in %d seconds)\n"),
(COMPILER_DOUBLE)((pInfo->offset_max-loop_var)*eye_unit), 
                      1.0*(log10(bers[loop_var])),
                      pInfo->total_errs[loop_var],
                      pInfo->total_elapsed_time[loop_var]));
        } else { 
            bers[loop_var] = (COMPILER_DOUBLE)(pInfo->total_errs[loop_var])/
                             (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_var]/pInfo->rate;

            /* the rate unit is KHZ, add -3(log10(1/1000)) for actual display  */
            bers[loop_var] /= 1000;
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META("BER @ %2.2f%% = 1e%2.2f (%d errors in %d seconds)\n"),
(pInfo->offset_max-loop_var)*eye_unit,
                      log10(bers[loop_var]),
                      pInfo->total_errs[loop_var],
                      pInfo->total_elapsed_time[loop_var]));
        }
        curr_ber_log = log10(bers[loop_var]);

        /* Detect and record nonmonotonic data points */
        if ((loop_var > 0) && (curr_ber_log > prev_ber_log)) {
            pInfo->mono_flags[loop_var] = 1;
        }
        prev_ber_log = curr_ber_log;
        
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("cur_be_log %2.2f\n"), curr_ber_log));
        /* find the first data point with good BER */
        if ((curr_ber_log <= good_ber_level) && 
            (pInfo->first_good_ber_idx == INDEX_UNINITIALIZED)) { 
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META("cur_be_log %2.2f, loop_var %d\n"), 
curr_ber_log,loop_var));
            pInfo->first_good_ber_idx = loop_var;
        }
        if ((pInfo->total_errs[loop_var] < HI_CONFIDENCE_MIN_ERR_CNT) &&
            (pInfo->first_small_errcnt_idx == INDEX_UNINITIALIZED)) {
             /* find the first data point with small error count */
             pInfo->first_small_errcnt_idx = loop_var;
        }
        
    }   /* for loop_var */

    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}


STATIC int  
_wc40_eye_margin_diagram_cal(WC40_EYE_DIAG_INFOt *pInfo, int type) 
{
#ifdef COMPILER_HAS_DOUBLE
    COMPILER_DOUBLE lbers[MAX_LOOPS]; /*Internal linear scale sqrt(-log(ber))*/
    COMPILER_DOUBLE margins[MAX_LOOPS]; /* Eye margin @ each measurement*/
    COMPILER_DOUBLE bers[MAX_LOOPS]; /* computed bit error rate */
    int delta_n;
    COMPILER_DOUBLE Exy = 0.0;
    COMPILER_DOUBLE Eyy = 0.0;
    COMPILER_DOUBLE Exx = 0.0;
    COMPILER_DOUBLE Ey  = 0.0;
    COMPILER_DOUBLE Ex  = 0.0;
    COMPILER_DOUBLE alpha;
    COMPILER_DOUBLE beta;
    COMPILER_DOUBLE proj_ber;
    COMPILER_DOUBLE proj_margin_12;
    COMPILER_DOUBLE proj_margin_15;
    COMPILER_DOUBLE proj_margin_18;
    COMPILER_DOUBLE sq_err2;
    COMPILER_DOUBLE ierr;
    int beta_max=0;
    int ber_conf_scale[20];
    int start_n;
    int stop_n;
    int low_confidence;
    int loop_index;
    COMPILER_DOUBLE outputs[4];
    COMPILER_DOUBLE eye_unit;
    int n_mono;

    /* Initialize BER confidence scale */
    ber_conf_scale[0] = 3.02;
    ber_conf_scale[1] = 4.7863;
    ber_conf_scale[2] = 3.1623;
    ber_conf_scale[3] = 2.6303;
    ber_conf_scale[4] = 2.2909;
    ber_conf_scale[5] = 2.138;
    ber_conf_scale[6] = 1.9953;
    ber_conf_scale[7] = 1.9055;
    ber_conf_scale[8] = 1.8197;
    ber_conf_scale[9] = 1.7783;
    ber_conf_scale[10] = 1.6982;
    ber_conf_scale[11] = 1.6596;
    ber_conf_scale[12] = 1.6218;
    ber_conf_scale[13] = 1.6218;
    ber_conf_scale[14] = 1.5849;
    ber_conf_scale[15] = 1.5488;
    ber_conf_scale[16] = 1.5488;
    ber_conf_scale[17] = 1.5136;
    ber_conf_scale[18] = 1.5136;
    ber_conf_scale[19] = 1.4791;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META("first_good_ber_idx: %d, first_small_errcnt_idx: %d\n"),
pInfo->first_good_ber_idx,pInfo->first_small_errcnt_idx));

    /* Find the highest data point to use, currently based on at least 1E-8 BER level */
    if (pInfo->first_good_ber_idx == INDEX_UNINITIALIZED) {
        start_n = pInfo->veye_cnt;
    } else {
        start_n = pInfo->first_good_ber_idx;
    }
    stop_n = pInfo->veye_cnt;

    /* Find the lowest data point to use */
    if (pInfo->first_small_errcnt_idx == INDEX_UNINITIALIZED) {
        stop_n = pInfo->veye_cnt;
    } else { 
        stop_n = pInfo->first_small_errcnt_idx;
    }

    /* determine the number of non-monotonic outliers */
    n_mono = 0;
    for (loop_index = start_n; loop_index < stop_n; loop_index++) {
        if (pInfo->mono_flags[loop_index] == 1) {
            n_mono = n_mono + 1;
        }
    } 

    if (type == WC_UTIL_HEYE_L || type == WC_UTIL_HEYE_R) {
        eye_unit = HEYE_UNIT;
    } else {
        eye_unit = VEYE_UNIT;
    }

    for (loop_index = 0; loop_index < pInfo->veye_cnt; loop_index++) {
        if (pInfo->total_errs[loop_index] == 0) {
            bers[loop_index] = 1.0/(COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_index]/pInfo->rate;
        } else {
            bers[loop_index] = (COMPILER_DOUBLE)pInfo->total_errs[loop_index]/
                               (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_index]/pInfo->rate;
        }
        bers[loop_index] /= 1000;
        margins[loop_index] = (pInfo->offset_max-loop_index)*eye_unit;
    }

    if( start_n >= pInfo->veye_cnt ) {
        outputs[0] = -_wc40_util_round_real(log(bers[pInfo->veye_cnt-1])/log(10), 1);
        outputs[1] = -100.0;
        outputs[2] = -100.0;
        outputs[3] = -100.0;
    /*  No need to print out the decimal portion of the BER */
    LOG_CLI((BSL_META("BER *worse* than 1e-%d\n"), (int)outputs[0]));
    LOG_CLI((BSL_META("Negative margin @ 1e-12, 1e-15 & 1e-18\n")));
    } else {
        low_confidence = 0;
    if( (stop_n-start_n - n_mono) < 2  ) {
        /* Code triggered when less than 2 statistically valid extrapolation points */
            for( loop_index = stop_n; loop_index < pInfo->veye_cnt; loop_index++ ) {
            if( pInfo->total_errs[loop_index] < 20 ) {
            bers[loop_index] = ber_conf_scale[pInfo->total_errs[loop_index]] * bers[loop_index];
        } else {
            bers[loop_index] = ber_conf_scale[19] * bers[loop_index];
        }
                pInfo->mono_flags[loop_index] = 0;    /* remove flags; or assess again */
        }
        /* Add artificial point at 100% margin to enable interpolation */
            margins[pInfo->veye_cnt] = 100.0;
            bers[pInfo->veye_cnt] = 0.1;
            low_confidence = 1;
            stop_n = pInfo->veye_cnt + 1;
    }

    /* Below this point the code assumes statistically valid point available */
        delta_n = stop_n - start_n - n_mono;

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("start_n: %d, stop_n: %d, veye: %d, n_mono: %d\n"),
start_n,stop_n,pInfo->veye_cnt,n_mono));

    /* Find all the correlations */
    for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
        lbers[loop_index] = (COMPILER_DOUBLE)sqrt(-log(bers[loop_index]));
    }

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\tstart=%d, stop=%d, low_confidence=%d\n"),
start_n, stop_n, low_confidence));
        for (loop_index=start_n; loop_index < stop_n; loop_index++){
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META("\ttotal_errs[%d]=0x%08x\n"),
loop_index,(int)pInfo->total_errs[loop_index]));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META("\tbers[%d]=%f\n"),
loop_index,bers[loop_index]));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META("\tlbers[%d]=%f\n"),
loop_index,lbers[loop_index]));
    }

    for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
            if (pInfo->mono_flags[loop_index] == 0) {
            Exy = Exy + margins[loop_index] * lbers[loop_index]/(COMPILER_DOUBLE)delta_n;
            Eyy = Eyy + lbers[loop_index]*lbers[loop_index]/(COMPILER_DOUBLE)delta_n;
            Exx = Exx + margins[loop_index]*margins[loop_index]/(COMPILER_DOUBLE)delta_n;
            Ey  = Ey + lbers[loop_index]/(COMPILER_DOUBLE)delta_n;
            Ex  = Ex + margins[loop_index]/(COMPILER_DOUBLE)delta_n;
            }
    }

    /* Compute fit slope and offset */
        alpha = (Exy - Ey * Ex)/(Exx - Ex * Ex);
        beta = Ey - Ex * alpha;
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META("Exy=%f, Eyy=%f, Exx=%f, Ey=%f,Ex=%f alpha=%f, beta=%f\n"),
Exy,Eyy,Exx,Ey,Ex,alpha,beta));

    /* JPA> Due to the limit of floats, I need to test for a maximum Beta of 9.32 */
    if(beta > 9.32){
      beta_max=1;
      LOG_CLI((BSL_META("\n\tWARNING: intermediate float variable is maxed out, "
                        "what this means is:\n")));
      LOG_CLI((BSL_META("\t\t- The *extrapolated* minimum BER will be reported "
                        "as 1E-37.\n")));
      LOG_CLI((BSL_META("\t\t- This may occur if the channel is near ideal "
                        "(e.g. test loopback)\n")));
      LOG_CLI((BSL_META("\t\t- While not discrete, reporting an extrapolated "
                        "BER < 1E-37 is numerically corect, and informative\n\n")));
    }

       
        proj_ber = exp(-beta * beta);
        proj_margin_12 = (sqrt(-log(1e-12)) - beta)/alpha;
        proj_margin_15 = (sqrt(-log(1e-15)) - beta)/alpha;
        proj_margin_18 = (sqrt(-log(1e-18)) - beta)/alpha;

        sq_err2 = 0;
    for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
      /* coverity[uninit_use:FALSE] */
      ierr = (lbers[loop_index] - (alpha*margins[loop_index] + beta));
      sq_err2 = sq_err2 + ierr*ierr/(COMPILER_DOUBLE)delta_n;
        }

        outputs[0] = -_wc40_util_round_real(log(proj_ber)/log(10),1);
    outputs[1] = _wc40_util_round_real(proj_margin_18,1);
        outputs[2] = _wc40_util_round_real(proj_margin_12,1);
        outputs[3] = _wc40_util_round_real(proj_margin_15,1);

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tlog1e-12=%f, sq=%f\n"),(COMPILER_DOUBLE)log(1e-12),(COMPILER_DOUBLE)sqrt(-log(1e-12))));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\talpha=%f\n"),alpha));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tbeta=%f\n"),beta));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tproj_ber=%f\n"),proj_ber));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tproj_margin12=%f\n"),proj_margin_12));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tproj_margin12=%f\n"),proj_margin_15));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tproj_margin18=%f\n"),proj_margin_18));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\toutputs[0]=%f\n"),outputs[0]));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\toutputs[1]=%f\n"),outputs[1]));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\toutputs[2]=%f\n"),outputs[2]));

        /* Extrapolated results, low confidence */
        if( low_confidence == 1 ) {
          if(beta_max){
            LOG_CLI((BSL_META("BER(extrapolated) is *better* than 1e-37\n")));
            LOG_CLI((BSL_META("Margin @ 1e-12    is *better* than %f\n"), outputs[2]));
            LOG_CLI((BSL_META("Margin @ 1e-15    is *better* than %f\n"), outputs[3]));
            LOG_CLI((BSL_META("Margin @ 1e-18    is *better* than %f\n"), outputs[1]));
          }
          else{
            LOG_CLI((BSL_META("BER(extrapolated) is *better* than 1e-%f\n"),
                     outputs[0]));
            LOG_CLI((BSL_META("Margin @ 1e-12    is *better* than %f\n"), outputs[2]));
            LOG_CLI((BSL_META("Margin @ 1e-15    is *better* than %f\n"), outputs[3]));
            LOG_CLI((BSL_META("Margin @ 1e-18    is *better* than %f\n"), outputs[1]));
          }

        /* JPA> Extrapolated results, high confidence */
        } else {           
          if(beta_max){
            LOG_CLI((BSL_META("BER(extrapolated) = 1e-37\n")));
            LOG_CLI((BSL_META("Margin @ 1e-12    is *better* than %f\n"), outputs[2]));
            LOG_CLI((BSL_META("Margin @ 1e-15    is *better* than %f\n"), outputs[3]));
            LOG_CLI((BSL_META("Margin @ 1e-18    is *better* than %f\n"), outputs[1]));
          }
          else{
          LOG_CLI((BSL_META("BER(extrapolated) = 1e-%4.2f\n"), outputs[0]));
          LOG_CLI((BSL_META("Margin @ 1e-12    = %4.2f%%\n"), outputs[2]));
          LOG_CLI((BSL_META("Margin @ 1e-15    = %4.2f%%\n"), outputs[3]));
          LOG_CLI((BSL_META("Margin @ 1e-18    = %4.2f%%\n"), outputs[1]));
          }
        }
    }
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
} 
#else   /* #ifndef __KERNEL__ */

int
wc40_eye_margin(int unit, soc_port_t port, int type)
{
    LOG_CLI((BSL_META_U(unit,
                        "\nThis function is not supported in Linux kernel mode\n")));
    return SOC_E_NONE;
}
#endif /* #ifndef __KERNEL__ */
#else /* INCLUDE_XGXS_WC40 */
typedef int _xgxs_wc40_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_XGXS_WC40 */

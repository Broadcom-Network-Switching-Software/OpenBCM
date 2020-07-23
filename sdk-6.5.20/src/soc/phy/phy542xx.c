/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 $Id$
 */

/*
 * File:        phy542xx.c
 * Purpose:     PHY driver for BCM542xx (Non-MacSec) 
 *
 * Supported BCM542X Family of PHY devices:
 *
 *  Device    Ports        Media            MAC Interface
 *  --------- -----   ------------------   ---------------
 *  54210       1     1 Copper / Fiber        SGMII/RGMII
 *  54220       2     2 Copper / Fiber        SGMII/RGMII
 *  54240       4     4 Copper / Fiber        SGMII
 *  54294       4     4 Copper / Fiber        SGMII
 *  54140/94    4     4 Copper / Fiber        SGMII
 *  54290/80    8     8 Copper only           SGMII
 *  54180/90    8     8 Copper only           SGMII
 *  54292/82    8     8 Copper only          QSGMII*2
 *  54182       8     8 Copper only          QSGMII*2
 *  54192       8     8 Copper only          QSGMII*2
 *  54296       4     4 Copper / Fiber       QSGMII
 *  54295       8     8 Copper / 4 Fiber     QSGMII*2
 *  54185       8     8 Copper / 8 Fiber     QSGMII*2
 *  54195       8     8 Copper / 8 Fiber     QSGMII*2
 *  54285       8     8 Copper / Fiber       QSGMII*2
 *
 * Workarounds:
 *
 * Notes:
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_542XX)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phyident.h"
#include "phyreg.h"
#include "phyfege.h"
#include "phy542xx_int.h"
#include "phy542xx_regdefs.h"
#ifdef INCLUDE_PLP_IMACSEC
#include "bcm_imacsec.h"
#include "bcm_iunimac.h"
#include "phy_mac_ctrl.h"
#endif
#include <soc/oam.h>

#define DISABLE_CLK125          0       /* Disable if unused to reduce */
                                        /*  EMI emissions */

#define LC_CM_DISABLE_WORKAROUND
#define AUTO_MDIX_WHEN_AN_DIS   0

#ifdef INCLUDE_PLP_IMACSEC

#define IMACSEC_MDIO_ADDR_GET(id) (id & 0x3ff)

#define IMACSEC_DES_PTR_GET(_pc)\
((bcm_plp_base_t_sec_phy_access_t *)(((char *)((_pc) + 1) + (_pc)->size) - sizeof(bcm_plp_base_t_sec_phy_access_t)))

extern void _bcm_plp_register_mdio_primitives(
    int unit, unsigned int slice, unsigned int secy_phy_id,
    int                 (*read)(int, uint32, uint32, uint16*), /* HW read */
    int                 (*write)(int, uint32, uint32, uint16));/* HW write */


/*
 * Function:     
 *    imacsec_blmi_io_mmi2
 * Purpose:    
 *    Access to UNIMAC and SECY register space
 * Parameters:
 *    unit            - Switch unit  (dev_addr is overloaded by this)
 *    port            - Switch port  (dev_port is overloaded by this)
 *    op              - access type  (read/write register )
 *    io_addr         - Register address
 *    word_sz         - word size
 *    num_entry       - number of words
 *    data            - data buffer
 * Notes: 
 *    Existing  "phy_mac_ctrl" infrastructure uses dev_addr/dev_port
 *    for i/o access.  For IMACSEC i/o access  we will overload these 
 *    with  (Switch unit)/(Switch port).
 */
static int
imacsec_blmi_io_mmi2( uint32 unit,      /* Switch unit  */
                      int port,         /* Switch port */
                      blmi_io_op_t op,  /* I/O operation   */  
                      uint32 io_addr,   /* I/O address     */
                      int word_sz,      /* Word size       */
                      int num_entry,    /* Num entry       */
                      uint32 *data)     /* Data buffer     */
{
    int rv;

    switch(op) {

        case BLMI_IO_REG_WR:
            rv = imacsec_plp_addr_write(unit, port, io_addr, *data);
            break;
        default:
            rv = imacsec_plp_addr_read(unit, port, io_addr, data);
            break;
    }

    return rv;
}

#define SPEED_MAPPING(_sp, _spr)    do { \
    switch ( _sp ) { \
    case 0:    return BLMI_E_NONE;  \
    case 10:   _spr = 0; break;     \
    case 100:  _spr = 1; break;     \
    case 1000: _spr = 2; break;     \
    default: return (BLMI_E_CONFIG); }  } while(0)\

static int get_port(const bcm_plp_base_t_access_t *pa)
{
    unsigned char lm = pa->lane_map ;

    switch(lm)
    {
        case 0x80:
            return 7;
        case 0x40:
            return 6;
        case 0x20:
            return 5;
        case 0x10:
            return 4;
        case 0x08:
            return 3;
        case 0x04:
            return 2;
        case 0x02:
            return 1;
        default:
            return 0;
    }
}

/*
 * Function:     
 *    _bcm_phy5419x_phy_mac_driver_detach
 * Purpose:    
 *    Delete Mapping of PHY device address and Device port
 * Parameters:
 *    phy_dev_addr    - PHY Device Address
 * Notes: 
 */
int 
_bcm_phy5419x_phy_mac_driver_detach(phy_ctrl_t     *pc)
{
    int rv = SOC_E_NONE;
    bcm_plp_base_t_sec_phy_access_t *imacsec_des;

    imacsec_des = IMACSEC_DES_PTR_GET(pc);

    if (imacsec_des->unimac) {
        phy_mac_driver_detach((phy_mac_ctrl_t *)imacsec_des->unimac);
        imacsec_des->unimac = 0;
    }
    return rv;
}


/*
 * Function:     
 *    _bcm_phy5419x_phy_mac_driver_attach
 * Purpose:    
 *    Add Mapping of PHY device address, Device port
 * Parameters:
 *    phy_dev_addr    - PHY Device Address
 *    dev_port        - Port number
 * Notes: 
 */
int 
_bcm_phy5419x_phy_mac_driver_attach(phy_ctrl_t *pc,
                                 uint32 ms_dev_addr,
                                 int dev_port, blmi_dev_io_f mmi_cbf)
{
    bcm_plp_base_t_sec_phy_access_t *imacsec_des;

    /* Delete the mapping if already present */
    (void)_bcm_phy5419x_phy_mac_driver_detach(pc);

    imacsec_des = IMACSEC_DES_PTR_GET(pc);

    /* Alloc unimac driver and attach */
    imacsec_des->unimac =  
            phy_mac_driver_attach(imacsec_des->macsec_dev_addr,
                                     PHY_MAC_CORE_UNIMAC, mmi_cbf);
    if (!imacsec_des->unimac) {
        _bcm_phy5419x_phy_mac_driver_detach(pc);
        return SOC_E_INTERNAL;
    }


    phy_mac_driver_config((phy_mac_ctrl_t *) imacsec_des->unimac,
                             PHY_MAC_CORE_UNIMAC,
                             PHY_MAC_CTRL_FLAG_UNIMAC_V2);

    phy_mac_driver_unit_set((phy_mac_ctrl_t *) imacsec_des->unimac,
                             PHY_MAC_CORE_UNIMAC,
                             pc->unit);


    return SOC_E_NONE;
}



#endif /* INCLUDE_PLP_IMACSEC */

/*Copper*/
#define ADVERT_ALL_COPPER_FLOW_CTRL   (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)
#define ADVERT_ALL_COPPER_HD_SPEED    (SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB | SOC_PA_SPEED_1000MB)
#define ADVERT_ALL_COPPER_FD_SPEED    (SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB | SOC_PA_SPEED_1000MB)
#define ADVERT_ALL_COPPER_EEE_SPEED   (SOC_PA_EEE_100MB_BASETX | \
                                       SOC_PA_EEE_1GB_BASET    | \
                                       SOC_PA_EEE_10GB_BASET)
/*Fiber*/
#define ADVERT_ALL_FIBER_FLOW_CTRL    (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)
#define ADVERT_ALL_FIBER_FD_SPEED     (SOC_PA_SPEED_100MB | SOC_PA_SPEED_1000MB)


#define SET_FLOW_CTRL_ABILITY(ability, p)  ((ability)->pause |= p)
#define SET_HD_SPEED_ABILITY(ability, s)   ((ability)->speed_half_duplex |= s)
#define SET_FD_SPEED_ABILITY(ability, s)   ((ability)->speed_full_duplex |= s)
#define SET_EEE_SPEED_ABILITY(ability, s)  ((ability)->eee |= s)

#define ADVERT_ALL_COPPER(ability)  do { \
            SET_FLOW_CTRL_ABILITY(ability, ADVERT_ALL_COPPER_FLOW_CTRL); \
            SET_HD_SPEED_ABILITY(ability, ADVERT_ALL_COPPER_HD_SPEED);   \
            SET_FD_SPEED_ABILITY(ability, ADVERT_ALL_COPPER_FD_SPEED);   \
            SET_EEE_SPEED_ABILITY(ability, ADVERT_ALL_COPPER_EEE_SPEED); \
        } while ( 0 )

#define ADVERT_ALL_FIBER(ability)  do { \
            SET_FLOW_CTRL_ABILITY(ability, ADVERT_ALL_FIBER_FLOW_CTRL); \
            SET_FD_SPEED_ABILITY(ability, ADVERT_ALL_FIBER_FD_SPEED);   \
        } while ( 0 )

#define PHY_IS_BCM54210(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54210_OUI,  \
                                  PHY_BCM54210_MODEL))
#define PHY_IS_BCM54220(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54220_OUI,  \
                                  PHY_BCM54220_MODEL))
#define PHY_IS_BCM54280(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54280_OUI,  \
                                  PHY_BCM54280_MODEL)) 
#define PHY_IS_BCM54180(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54180_OUI,  \
                                  PHY_BCM54180_MODEL) \
                                  && (!(((_pc)->phy_rev & 0x8))) )
#define PHY_IS_BCM54182(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54182_OUI,  \
                                  PHY_BCM54182_MODEL))
#define PHY_IS_BCM54185(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54185_OUI,  \
                                  PHY_BCM54185_MODEL)\
                                  && (((_pc)->phy_rev & 0x8)) )
#define PHY_IS_BCM54140(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54140_OUI,  \
                                  PHY_BCM54140_MODEL) \
                                  && (((_pc)->phy_rev & 0x8)) )
#define PHY_IS_BCM54190(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54190_OUI,  \
                                  PHY_BCM54190_MODEL) \
                                  && (!(((_pc)->phy_rev & 0x8))) )
#define PHY_IS_BCM54192(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54192_OUI,  \
                                  PHY_BCM54192_MODEL))
#define PHY_IS_BCM54195(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54195_OUI,  \
                                  PHY_BCM54195_MODEL)\
                                  && (((_pc)->phy_rev & 0x8)) )
#define PHY_IS_BCM54194(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54194_OUI,  \
                                  PHY_BCM54194_MODEL) \
                                  && (((_pc)->phy_rev & 0x8)) )
#define PHY_IS_BCM54282(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54282_OUI,  \
                                  PHY_BCM54282_MODEL) \
                                  && ((_pc)->phy_rev & 0x8))
#define PHY_IS_BCM54240(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54240_OUI,  \
                                  PHY_BCM54240_MODEL))
#define PHY_IS_BCM54285(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54285_OUI,  \
                                  PHY_BCM54285_MODEL) \
                                  && (!((_pc)->phy_rev & 0x8)))
#define PHY_IS_BCM5428x(pc)  (    PHY_IS_BCM54280(pc) \
                               || PHY_IS_BCM54282(pc) \
                               || PHY_IS_BCM54240(pc) \
                               || PHY_IS_BCM54285(pc) )
#define PHY_IS_BCM54210_220(pc)  (PHY_IS_BCM54210(pc) \
                               || PHY_IS_BCM54220(pc) )
#define PHY_IS_BCM5418x_19x(pc)  (PHY_IS_BCM54140(pc) \
                               || PHY_IS_BCM54180(pc) \
                               || PHY_IS_BCM54182(pc) \
                               || PHY_IS_BCM54185(pc) \
                               || PHY_IS_BCM54190(pc) \
                               || PHY_IS_BCM54192(pc) \
                               || PHY_IS_BCM54194(pc) \
                               || PHY_IS_BCM54195(pc) )
#define PHY_IS_BCM5418x_14x(pc)  (PHY_IS_BCM54140(pc) \
                               || PHY_IS_BCM54180(pc) \
                               || PHY_IS_BCM54182(pc) \
                               || PHY_IS_BCM54185(pc) )

#if 0
#define PHY_BCM54282_QSGMII_DEV_ADDR(phy_base_addr) (phy_base_addr + 8)
#define PHY_BCM54282_QSGMII_DEV_ADDR_SHARED(phy_base_addr) (phy_base_addr + 3)
#endif

#define PHY_PORTS_PER_QSGMII        4
#define PHY_QSGMII0_HEAD            0x0
#define PHY_QSGMII1_HEAD            (PHY_QSGMII0_HEAD + PHY_PORTS_PER_QSGMII)

#define PHY_BCM54282_QSGMII_DEV_ADDR(_pc) \
                (PHY_BCM542XX_FLAGS((_pc)) & PHY_BCM542XX_PHYADDR_REVERSE) \
                ? (PHY_BCM542XX_DEV_PHY_ID_BASE(_pc) + 1) \
                : (PHY_BCM542XX_DEV_PHY_ID_BASE(_pc) + 8)

#define PHY_BCM542XX_QSGMII_DEV_ADDR(_pc) \
                (PHY_BCM542XX_FLAGS((_pc)) & PHY_BCM542XX_PHYADDR_REVERSE) \
                ? (PHY_BCM542XX_DEV_PHY_ID_BASE(_pc) + 1) \
                : (PHY_BCM542XX_DEV_PHY_ID_BASE(_pc) + 8)

#define PHY_BCM54282_QSGMII_DEV_ADDR_SHARED(_pc) PHY_BCM542XX_SLICE_ADDR((_pc), 3)

#define PHY_BCM54240_28X_REV_BO(model, rev)  ( \
                                          ((rev & 0x7) == 0x1) && \
                                           ((model == PHY_BCM54280_MODEL) || \
                                            (model == PHY_BCM54282_MODEL) || \
                                            (model == PHY_BCM54285_MODEL) || \
                                            (model == PHY_BCM54240_MODEL)) )
#define PHY_BCM54240_28X_REV_C1(model, rev)  ( \
                                          ((rev & 0x7) == 0x3) && \
                                           ((model == PHY_BCM54280_MODEL) || \
                                            (model == PHY_BCM54282_MODEL) || \
                                            (model == PHY_BCM54285_MODEL) || \
                                            (model == PHY_BCM54240_MODEL)) )
#define PHY_BCM54240_28X_REV_C0_OR_OLDER(model, rev)  ( \
                                          ((rev & 0x7) < 0x3) && \
                                           ((model == PHY_BCM54280_MODEL) || \
                                            (model == PHY_BCM54282_MODEL) || \
                                            (model == PHY_BCM54285_MODEL) || \
                                            (model == PHY_BCM54240_MODEL)) )

#define PHY_IS_BCM54290(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54290_OUI,  \
                                  PHY_BCM54290_MODEL) \
                                  && (!((_pc)->phy_rev & 0x8)) )
#define PHY_IS_BCM54292(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54292_OUI,  \
                                  PHY_BCM54292_MODEL))
#define PHY_IS_BCM54294(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54294_OUI,  \
                                  PHY_BCM54294_MODEL)\
                                  && ((_pc)->phy_rev & 0x8))
#define PHY_IS_BCM54295(_pc) (PHY_MODEL_CHECK((_pc), \
                                  PHY_BCM54295_OUI,  \
                                  PHY_BCM54295_MODEL) \
                                  && (((_pc)->phy_rev & 0x8)))
#define PHY_IS_BCM54296  PHY_IS_BCM54295
#define PHY_IS_BCM5429X(pc)  (    PHY_IS_BCM54290(pc) \
                               || PHY_IS_BCM54294(pc) \
                               || PHY_IS_BCM54292(pc) \
                               || PHY_IS_BCM54295(pc) )

#define SOC_IF_ERROR_BREAK(op, _rv)       if ( ((_rv) = (op)) < 0 ) {  break;  }
#define PHY_RETAIN_SUPER_ISOLATE          0x80
/* macros for ext_phy_autodetect_en and ext_phy_fiber_iface */
#define EXT_PHY_AUTODETECT_DIS     0x0
#define EXT_PHY_AUTODETECT_EN      0x1
#define EXT_PHY_FIBER_IFACE_1000X  0x0
#define EXT_PHY_FIBER_IFACE_100FX  0x1
#define EXT_PHY_FIBER_IFACE_SGMII  0x2

int phy_bcm542xx_reset(int unit, soc_port_t port, void *user_arg);
STATIC int _phy_bcm542xx_no_reset_setup(int unit, soc_port_t port);
STATIC int phy_bcm542xx_duplex_get(int unit, soc_port_t port, int *duplex);
STATIC int phy_bcm542xx_duplex_set(int unit, soc_port_t port, int  duplex);
STATIC int phy_bcm542xx_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_bcm542xx_speed_set(int unit, soc_port_t port, int  speed);
STATIC int phy_bcm542xx_master_get(int unit, soc_port_t port, int *master);
STATIC int phy_bcm542xx_master_set(int unit, soc_port_t port, int  master);
STATIC int phy_bcm542xx_autoneg_get(int unit, soc_port_t port, int *autoneg, int *autoneg_done);
STATIC int phy_bcm542xx_autoneg_set(int unit, soc_port_t port, int  autoneg);
STATIC int phy_bcm542xx_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode);
STATIC int phy_bcm542xx_mdix_set(int unit, soc_port_t port, soc_port_mdix_t  mode);
STATIC int phy_bcm542xx_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int phy_bcm542xx_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int phy_bcm542xx_init_setup(int unit, soc_port_t port, int reset, int automedium,
                                   int fiber_preferred, int fiber_detect,
                                   int fiber_enable, int copper_enable,
                                   int ext_phy_autodetect_en, int ext_phy_fiber_iface);
STATIC int phy_bcm542xx_control_set(int unit, soc_port_t port,
                                    soc_phy_control_t type, uint32 value);
int phy_bcm542xx_oam_config_get(int unit, soc_port_t port, soc_port_config_phy_oam_t *conf);
extern int phy_ecd_cable_diag_init_40nm(int unit, soc_port_t port);
extern int phy_ecd_cable_diag_40nm(int unit, soc_port_t port,
                                   soc_port_cable_diag_t *status, uint16 ecd_ctrl);
int _phy_bcm542xx_rgmii_to_copper_mode(int unit, soc_port_t port);
int _phy_bcm542xx_rgmii_to_fiber_mode(int unit, soc_port_t port);

/***********************************************************************
 *
 * HELPER FUNCTIONS
 */

int
phy_bcm542xx_reg_read(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                      uint8 reg_addr, uint16 *data)
{
    int rv;
    uint16 val;

    rv = SOC_E_NONE;
    if ( flags & PHY_BCM542XX_REG_QSGMII ) {
        /* Do not apply shadow register logic for QSGMII register access */
        rv = PHY_BCM542XX_READ_PHY_REG(unit, pc, reg_addr, data);

    } else if ( flags & (PHY_BCM542XX_REG_1000X |
                         PHY_BCM542XX_REG_PRI_SERDES) ) {  /* fiber registers */
        if ( reg_addr <= 0x0f ) {
            if ( PHY_IS_BCM54210_220(pc) && (flags & PHY_BCM542XX_REG_1000X) ) {
                /* 54220/210 fiber is controlled by Secondary SerDes */
                rv = phy_bcm542xx_direct_reg_read(unit, pc, (uint16)
                                reg_addr | PHY_BCM542XX_2ND_SERDES_BASE_OFFSET,
                                data);
            } else {  /* not 54220_210 or PHY_BCM542XX_REG_PRI_SERDES */
                /* Map 1000X page */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
                uint16 blk_sel, val;
                val = 0x7c00;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x1c, val));
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_READ_PHY_REG( unit, pc, 0x1c, &blk_sel));
                val = blk_sel | 0x8001;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x1c, val));
#else
                SOC_IF_ERROR_RETURN(
                    phy_bcm542xx_direct_reg_modify(unit, pc,
                                    PHY_BCM542XX_MODE_CONTROL_REG_OFFSET,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_EN,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_EN) );
#endif
                /* Read 1000X IEEE register */
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_READ_PHY_REG(unit, pc, reg_addr, data));

               /* Restore IEEE mapping */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
                val = (blk_sel & 0xfffe) | 0x8000;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x1c, val));
#else
                SOC_IF_ERROR_RETURN(
                    phy_bcm542xx_direct_reg_modify(unit, pc,
                                    PHY_BCM542XX_MODE_CONTROL_REG_OFFSET,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_DIS,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_EN) );
#endif
            }
        } else {  /*  (reg_addr > 0xf)  */
            rv = SOC_E_PARAM;
        }

    } else {
        switch ( reg_addr ) {
        /* Map shadow registers */
        case 0x15:
            SOC_IF_ERROR_RETURN
                (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x17, reg_bank));
            break;
        case 0x18:
            if ( reg_bank <= 0x0007 ) {
                val = (reg_bank << 12) | 0x7;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, reg_addr, val));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1C:
            if ( reg_bank <= 0x001F ) {
                val = (reg_bank << 10);
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, reg_addr, val));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1D:
            if ( reg_bank <= 0x0001 ) {
                val = reg_bank << 15; 
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, reg_addr, val));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        }
        if ( SOC_SUCCESS(rv) ) {
            rv = PHY_BCM542XX_READ_PHY_REG(unit, pc, reg_addr, data);
        }
    } 

    if ( SOC_FAILURE(rv) ) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_bcm542xx_reg_read: failed:"
                             "phy_id=0x%2x reg_bank=0x%04x reg_addr=0x%02x "
                             "rv=%d\n"), pc->phy_id, reg_bank, reg_addr, rv)); 
    }
    return rv;
}


int
phy_bcm542xx_reg_write(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                       uint8 reg_addr, uint16 data)
{
    int     rv;

    if ( flags & PHY_BCM542XX_REG_QSGMII ) {
        /* Do not apply shadow register logic for QSGMII register access */
        SOC_IF_ERROR_RETURN
            (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, reg_addr, data));
        return SOC_E_NONE;
    }

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }
    rv = SOC_E_NONE;

    if ( flags & (PHY_BCM542XX_REG_1000X |
                  PHY_BCM542XX_REG_PRI_SERDES) ) {  /* fiber registers */
        if ( reg_addr <= 0x0f ) {
            if ( PHY_IS_BCM54210_220(pc) && (flags & PHY_BCM542XX_REG_1000X) ) {
                /* 54220/210 fiber is controlled by Secondary SerDes */
                rv = phy_bcm542xx_direct_reg_write(unit, pc,(uint16)
                                reg_addr | PHY_BCM542XX_2ND_SERDES_BASE_OFFSET,
                                data);
            } else {  /* not 54220_210 or PHY_BCM542XX_REG_PRI_SERDES */
                /* Map 1000X page */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
                uint16 blk_sel, val;
                val = 0x7c00;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x1c, val));
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_READ_PHY_REG( unit, pc, 0x1c, &blk_sel));
                val = blk_sel | 0x8001;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x1c, val));
#else
                SOC_IF_ERROR_RETURN(
                    phy_bcm542xx_direct_reg_modify(unit, pc,
                                    PHY_BCM542XX_MODE_CONTROL_REG_OFFSET,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_EN,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_EN) );
#endif
                /* Write 1000X IEEE register */
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, reg_addr, data));

               /* Restore IEEE mapping */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
                val = (blk_sel & 0xfffe) | 0x8000;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x1c, val));
#else
                SOC_IF_ERROR_RETURN(
                    phy_bcm542xx_direct_reg_modify(unit, pc,
                                    PHY_BCM542XX_MODE_CONTROL_REG_OFFSET,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_DIS,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_EN) );
#endif
            }
        } else {  /*  (reg_addr > 0xf)  */
            rv = SOC_E_PARAM;
        }

    } else {
        switch ( reg_addr ) {
        /* Map shadow registers */
        case 0x15:
            SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x17, reg_bank));
            break;
        case 0x18:
            if ( reg_bank <= 0x0007 ) {
                if (reg_bank == 0x0007) {
                    data |= 0x8000;
                }
                data = (data & ~(0x0007)) | reg_bank;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1C:
            if ( reg_bank <= 0x001F ) {
                data = 0x8000 | (reg_bank << 10) | (data & 0x03FF);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1D:
            if ( reg_bank == 0x0000 ) {
                data = data & 0x07FFF;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        }
        if ( SOC_SUCCESS(rv) ) {
            rv = PHY_BCM542XX_WRITE_PHY_REG(unit, pc, reg_addr, data);
        }
    } 

    if ( SOC_FAILURE(rv) ) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_bcm542xx_reg_write: failed:"
                             "phy_id=0x%2x reg_bank=0x%04x reg_addr=0x%02x "
                             "rv=%d\n"), pc->phy_id, reg_bank, reg_addr, rv)); 
    }
    return rv;
}

int
phy_bcm542xx_reg_modify(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                        uint8 reg_addr, uint16 data, uint16 mask)
{
    int     rv;
    uint16 val;

    if ( flags & PHY_BCM542XX_REG_QSGMII ) {
        /* Do not apply shadow register logic for QSGMII register access */
        SOC_IF_ERROR_RETURN
            (PHY_BCM542XX_MODIFY_PHY_REG(unit, pc, reg_addr, data, mask));
        return SOC_E_NONE;
    }

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }
    rv = SOC_E_NONE;

    if ( flags & (PHY_BCM542XX_REG_1000X |
                  PHY_BCM542XX_REG_PRI_SERDES) ) {  /* fiber registers */
        if ( reg_addr <= 0x0f ) {
            if ( PHY_IS_BCM54210_220(pc) && (flags & PHY_BCM542XX_REG_1000X) ) {
                /* 54220/210 fiber is controlled by Secondary SerDes */
                rv = phy_bcm542xx_direct_reg_modify(unit, pc,(uint16)
                                reg_addr | PHY_BCM542XX_2ND_SERDES_BASE_OFFSET,
                                data, mask);
            } else {  /* not 54220_210 or PHY_BCM542XX_REG_PRI_SERDES */
                /* Map 1000X page */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
                uint16 blk_sel;
                val = 0x7c00;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x1c, val));
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_READ_PHY_REG( unit, pc, 0x1c, &blk_sel));
                val = blk_sel | 0x8001;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x1c, val));
#else
                SOC_IF_ERROR_RETURN(
                    phy_bcm542xx_direct_reg_modify(unit, pc,
                                    PHY_BCM542XX_MODE_CONTROL_REG_OFFSET,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_EN,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_EN) );
#endif
                /* Modify 1000X IEEE register */
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_MODIFY_PHY_REG(unit, pc, reg_addr, data, mask));

               /* Restore IEEE mapping */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
                val = (blk_sel & 0xfffe) | 0x8000;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x1c, val));
#else
                SOC_IF_ERROR_RETURN(
                    phy_bcm542xx_direct_reg_modify(unit, pc,
                                    PHY_BCM542XX_MODE_CONTROL_REG_OFFSET,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_DIS,
                                    PHY_BCM542XX_MODE_CONTROL_REG_1000X_EN) );
#endif
            }
        } else {  /*  (reg_addr > 0xf)  */
            rv = SOC_E_PARAM;
        }

    } else {
        switch ( reg_addr ) {
        /* Map shadow registers */
        case 0x15:
            SOC_IF_ERROR_RETURN
                (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x17, reg_bank));
            break;
        case 0x18:
            if ( reg_bank <= 0x0007 ) {
                val = (reg_bank << 12) | 0x7;
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, reg_addr, val));

                if (reg_bank == 0x0007) {
                    data |= 0x8000;
                    mask |= 0x8000;
                }
                mask &= ~(0x0007);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1C:
            if ( reg_bank <= 0x001F ) {
                val = (reg_bank << 10);
                SOC_IF_ERROR_RETURN
                    (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, reg_addr, val));
                data |= 0x8000;
                mask |= 0x8000;
                mask &= ~(0x1F << 10);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1D:
            if ( reg_bank == 0x0000 ) {
                mask &= 0x07FFF;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        }
        if ( SOC_SUCCESS(rv) ) {
            rv = PHY_BCM542XX_MODIFY_PHY_REG(unit, pc, reg_addr, data, mask);
        }
    }

    if ( SOC_FAILURE(rv) ) {
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_bcm542xx_reg_modify: failed:"
                             "phy_id=0x%2x reg_bank=0x%04x reg_addr=0x%02x "
                             "rv=%d\n"), pc->phy_id, reg_bank, reg_addr, rv)); 
    }
    return rv;
}

int 
phy_bcm542xx_reg_read_modify_write(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                                   uint16 reg_data, uint16 reg_mask)
{
    uint16  tmp, otmp;

    reg_data = reg_data & reg_mask;

    SOC_IF_ERROR_RETURN
        (PHY_BCM542XX_READ_PHY_REG(unit, pc, reg_addr, &tmp));
    otmp = tmp;
    tmp &= ~(reg_mask);
    tmp |= reg_data;

    if (otmp != tmp) {
        SOC_IF_ERROR_RETURN
            (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, reg_addr, tmp));
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *     phy_bcm542xx_rdb_reg_read
 * Purpose:  RDB register read
 *           (works ONLY under RDB register addressing mode !!)
 * Parameters:
 *     reg_addr - RDB register address
 *     data     - the 16-bit value got from the specific RDB register
 * Returns:
 */
int phy_bcm542xx_rdb_reg_read(int unit, phy_ctrl_t *pc,
                              uint16 reg_addr, uint16 *data)
{
    int rv;

    /* MDIO write the RDB reg. address to reg.0x1E = <reg_addr> */
    rv = PHY_BCM542XX_WRITE_PHY_REG(unit, pc,
                        PHY_BCM542XX_RDB_ADDR_REG_OFFSET,
                        PHY_BCM542XX_RDB_ADDR_REG_ADDR & reg_addr);
    if ( SOC_E_NONE == rv ) {
        /* MDIO read from reg.0x1F to get the RDB register's value as <data> */
        rv = PHY_BCM542XX_READ_PHY_REG(unit, pc,
                        PHY_BCM542XX_RDB_DATA_REG_OFFSET, data);
    }

#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
    if ( SOC_E_NONE != rv ) {          /* failed to access reg. 0x1e or 0x1f */
        PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc);   /* quit RDB assess mode */
        _SHR_ERROR_TRACE(rv);
    }
#endif
    return (rv);
}

/*
 * Function:
 *     phy_bcm542xx_rdb_reg_write
 * Purpose:  RDB register write
 *           (works ONLY under RDB register addressing mode !!)
 * Parameters:
 *     reg_addr - RDB register address
 *     data     - 16-bit value writting to the specific RDB register
 * Returns:
 */
int phy_bcm542xx_rdb_reg_write(int unit, phy_ctrl_t *pc,
                               uint16 reg_addr, uint16 data)
{
    int rv;

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }

    /* MDIO write the RDB reg. address to reg.0x1E = <reg_addr> */
    rv = PHY_BCM542XX_WRITE_PHY_REG(unit, pc,
                        PHY_BCM542XX_RDB_ADDR_REG_OFFSET,
                        PHY_BCM542XX_RDB_ADDR_REG_ADDR & reg_addr);
    if ( SOC_E_NONE == rv ) {
        /* MDIO write to reg.0x1F to set the RDB resister's value as <data> */
        rv = PHY_BCM542XX_WRITE_PHY_REG(unit, pc,
                        PHY_BCM542XX_RDB_DATA_REG_OFFSET, data);
    }

#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
    if ( SOC_E_NONE != rv ) {          /* failed to access reg. 0x1e or 0x1f */
        PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc);   /* quit RDB assess mode */
        _SHR_ERROR_TRACE(rv);
    }
#endif
    return (rv);
}

/*
 * Function:
 *     phy_bcm542xx_rdb_reg_modify
 * Purpose:  RDB register modification
 *           (works ONLY under RDB register addressing mode !!)
 * Parameters:
 *     reg_addr - RDB register address
 *     data     - 16-bit value writting to the specific RDB register
 *     mask     - bit mask
 * Returns:
 */
int phy_bcm542xx_rdb_reg_modify(int unit, phy_ctrl_t *pc,
                                uint16 reg_addr, uint16 data, uint16 mask)
{
    int rv;

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }

    /* MDIO write the RDB reg. address to reg.0x1E = <reg_addr> */
    rv = PHY_BCM542XX_WRITE_PHY_REG(unit, pc,
                        PHY_BCM542XX_RDB_ADDR_REG_OFFSET,
                        PHY_BCM542XX_RDB_ADDR_REG_ADDR & reg_addr);
    if ( SOC_E_NONE == rv ) {
        /* MDIO read from reg.0x1F to modify the RDB register's value */
        rv = PHY_BCM542XX_MODIFY_PHY_REG(unit, pc,
                        PHY_BCM542XX_RDB_DATA_REG_OFFSET, data, mask);
    }

#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
    if ( SOC_E_NONE != rv ) {          /* failed to access reg. 0x1e or 0x1f */
        PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc);   /* quit RDB assess mode */
        _SHR_ERROR_TRACE(rv);
    }
#endif
    return (rv);
}


#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING

/*
 * Function:
 *     phy_bcm542xx_direct_reg_read
 * Purpose:
 *     switching to RDB access mode and read an RDB register,
 *     then back to the legacy access mode (quit the RDB mode).
 * Parameters:
 * Returns:
 */
int phy_bcm542xx_direct_reg_read(int unit, phy_ctrl_t *pc,
                                 uint16 reg_addr, uint16 *data)
{
    int  rv;

    PHY_BCM542XX_RDB_ACCESS_MODE(unit, pc);   /* set RDB register access mode */
    rv = phy_bcm542xx_rdb_reg_read(unit, pc, reg_addr, data);
    PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc); /* legacy register access mode */
    return (rv);
}

/*
 * Function:
 *    phy_bcm542xx_direct_reg_write
 * Purpose:
 *     switching to RDB access mode and write an RDB register,
 *     then back to the legacy access mode (quit the RDB mode).
 * Parameters:
 * Returns:
 */
int phy_bcm542xx_direct_reg_write(int unit, phy_ctrl_t *pc,
                                  uint16 reg_addr, uint16 data)
{
    int  rv;

    PHY_BCM542XX_RDB_ACCESS_MODE(unit, pc);   /* set RDB register access mode */
    rv = phy_bcm542xx_rdb_reg_write(unit, pc, reg_addr, data);
    PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc); /* legacy register access mode */
    return (rv);
}

/*
 * Function:
 *    phy_bcm542xx_direct_reg_modify
 * Purpose:
 *     switching to RDB access mode and modify an RDB register,
 *     then back to the legacy access mode (quit the RDB mode).
 * Parameters:
 * Returns:
 */
int phy_bcm542xx_direct_reg_modify(int unit, phy_ctrl_t *pc,
                                   uint16 reg_addr, uint16 data, uint16 mask)
{
    int  rv;

    PHY_BCM542XX_RDB_ACCESS_MODE(unit, pc);   /* set RDB register access mode */
    rv = phy_bcm542xx_rdb_reg_modify(unit, pc, reg_addr, data, mask);
    PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc); /* legacy register access mode */
    return (rv);
}

#endif /* PHY_BCM542XX_DEFAULT_TO_LEGACY_ACCESS */

/*
 * QSGMII register READ
 */
int 
phy_bcm542xx_qsgmii_reg_read(int unit, phy_ctrl_t *pc,
                             int dev_port, uint16 block,
                             uint8 reg, uint16 *data)
{
    uint16  val;
    
    /* Lanes from 0 to 7 */
    if ((dev_port < 0) || (dev_port > 7) ) {
        return SOC_E_FAIL;
    }

    /* Set BAR to AER */
    if ( PHY_IS_BCM5418x_19x(pc) ) {
        val = 0xFFDE;
    } else {
        val = 0xFFD0;
    }
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_reg_write(unit, pc, PHY_BCM542XX_REG_QSGMII,
                                0, 0x1F, val));

    /* Set AER reg to access SGMII lane */
    val = dev_port;
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_reg_write(unit, pc, PHY_BCM542XX_REG_QSGMII,
                                0, 0x1E, val));

    /* Set BAR to Register Block */
    val = (block & 0xfff0);
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_reg_write(unit, pc, PHY_BCM542XX_REG_QSGMII,
                                0, 0x1F, val));
    /* Read the register */
    if ( block >= 0x8000 ) {
        reg |= 0x10;
    }

    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_reg_read(unit, pc, PHY_BCM542XX_REG_QSGMII,
                               0, reg, data));

    return SOC_E_NONE;

}

/*
 * QSGMII register WRITE
 */
int 
phy_bcm542xx_qsgmii_reg_write(int unit, phy_ctrl_t *pc,
        int dev_port, uint16 block, uint8 reg, 
        uint16 data)
{
    uint16  val;

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }

    /* Lanes from 0 to 7 */
    if ((dev_port < 0) || (dev_port > 7) ) {
        return SOC_E_FAIL;
    }

    /* Set BAR to AER */
    if ( PHY_IS_BCM5418x_19x(pc) ) {
        val = 0xFFDE;
    } else {
        val = 0xFFD0;
    }

    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_reg_write(unit, pc,
                                PHY_BCM542XX_REG_QSGMII, 0, 0x1F, val));

    /* set aer reg to access sgmii lane */
    val = dev_port;
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_reg_write(unit, pc,
                                PHY_BCM542XX_REG_QSGMII, 0, 0x1E, val));

    /* set bar to register block */
    val = (block & 0xfff0);
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_reg_write(unit, pc,
                                PHY_BCM542XX_REG_QSGMII, 0, 0x1F, val));

    /* Write the register */
    if (block >= 0x8000) {
        reg |= 0x10;
    }

    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_reg_write(unit, pc,
                                PHY_BCM542XX_REG_QSGMII, 0, reg, data));

    return SOC_E_NONE;
}

/*
 * QSGMII register MODIFY
 */
int 
phy_bcm542xx_qsgmii_reg_modify(int unit, phy_ctrl_t *pc,
                               int dev_port, uint16 block,
                               uint8 reg_addr, uint16 reg_data,
                               uint16 reg_mask)
{
    uint16  tmp, otmp;

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }

    reg_data = reg_data & reg_mask;

    phy_bcm542xx_qsgmii_reg_read(unit, pc, dev_port, block,
                                 reg_addr, &tmp);
    otmp = tmp;
    tmp &= ~(reg_mask);
    tmp |= reg_data;

    if (otmp != tmp) {
        phy_bcm542xx_qsgmii_reg_write(unit, pc, dev_port, block,
                                      reg_addr, tmp);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_bcm542xx_cl45_reg_read
 * Purpose:
 *      Read Clause 45 Register using Clause 22 register access
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      pc        - Phy control
 *      flags     - Flags
 *      dev_addr  - Clause 45 Device
 *      reg_addr  - Register Address to read
 *      val       - Value Read
 * Returns:
 *      SOC_E_XXX
 */

int
phy_bcm542xx_cl45_reg_read(int unit, phy_ctrl_t *pc, uint8 dev_addr,
                           uint16 reg_addr, uint16 *val)
{
    uint16  temp16 = (dev_addr & 0x001f);

    /* Write Device Address to register 0x0D (Set Function field to Address) */
    SOC_IF_ERROR_RETURN
        (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x0D, temp16));

    /* Select the register by writing to register address to register 0x0E */
    SOC_IF_ERROR_RETURN
        (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x0E, reg_addr));

    temp16 = 0x4000 | (dev_addr & 0x001f);
    /* Write Device Address to register 0x0D (Set Function field to Data) */
    SOC_IF_ERROR_RETURN
        (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x0D, temp16)); 

    /* Read register 0x0E to get the value */
    SOC_IF_ERROR_RETURN
        (PHY_BCM542XX_READ_PHY_REG( unit, pc, 0x0E, val));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_bcm542xx_cl45_reg_write
 * Purpose:
 *      Write Clause 45 Register content using Clause 22 register access
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      pc        - Phy control
 *      flags     - Flags
 *      dev_addr  - Clause 45 Device
 *      reg_addr  - Register Address to read
 *      val       - Value to be written
 * Returns:
 *      SOC_E_XXX
 */

int
phy_bcm542xx_cl45_reg_write(int unit, phy_ctrl_t *pc, uint8 dev_addr,
                            uint16 reg_addr, uint16 val)
{
    uint16  temp16 = (dev_addr & 0x001f);

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }

    /* Write Device Address to register 0x0D (Set Function field to Address)*/
    SOC_IF_ERROR_RETURN
        (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x0D, temp16));

    /* Select the register by writing to register address to register 0x0E */
    SOC_IF_ERROR_RETURN
        (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x0E, reg_addr));

    temp16 = 0x4000 | (dev_addr & 0x001f);
    /* Write Device Address to register 0x0D (Set Function field to Data)*/
    SOC_IF_ERROR_RETURN
        (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x0D, temp16)); 

    /* Write register 0x0E to write the value */
    SOC_IF_ERROR_RETURN
        (PHY_BCM542XX_WRITE_PHY_REG(unit, pc, 0x0E, val));

    return SOC_E_NONE;
}

/*
 * Function:
 *     _phy_bcm542xx_cl45_reg_modify
 * Purpose:
 *      Modify Clause 45 Register contents using Clause 22 register access
 * Parameters:
 *      unit      - StrataSwitch unit #.
 *      pc        - Phy control
 *      flags     - Flags
 *      reg_bank  - Register bank
 *      dev_addr  - Clause 45 Device
 *      reg_addr  - Register Address to read
 *      val       - Value 
 *      mask      - Mask for modifying the contents of the register
 * Returns:
 *      SOC_E_XXX
 */

int
phy_bcm542xx_cl45_reg_modify(int unit, phy_ctrl_t *pc, uint8 dev_addr, 
                             uint16 reg_addr, uint16 val, uint16 mask)
{
    uint16  value = 0;

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }

    phy_bcm542xx_cl45_reg_read(unit, pc, dev_addr, reg_addr, &value);

    value = (val & mask) | (value & ~mask);

    phy_bcm542xx_cl45_reg_write(unit, pc, dev_addr, reg_addr, value);
    
    return SOC_E_NONE;
}

/*
 * Function:
 *    _phy_bcm542xx_hidden_rev_num_get
 * Purpose:
 *    Get hidden revision number from CORE_ExpFA - Hidden Identifier Reg.
 *    For distinguishing  54296 vs. 54295  ( 0x8 vs. 0x0 )
 * Parameters:
 *
 * Notes: 
 */
STATIC uint16
_phy_bcm542xx_hidden_rev_num_get(int unit, phy_ctrl_t *pc)
{
    uint16  hidden_rev;
    /* CORE_ExpFA - Hidden Identifier Reg., RDB.0x0FA[3:0] HIDDEN_REV_NUM */
    phy_bcm542xx_direct_reg_read(unit, pc,
                        PHY_BCM542XX_HIDDEN_IDENTIFIER_REG_OFFSET, &hidden_rev);
    return  (hidden_rev & PHY_BCM542XX_HIDDEN_REV_NUM_MASK);
}

/*
 * Function:
 *    _phy_bcm542xx_get_model_rev
 * Purpose:
 *    Get OUI, Model and Revision of the PHY
 * Parameters:
 *    phy_dev_addr - PHY Device Address
 *    oui          - (OUT) Organization Unique Identifier
 *    model        - (OUT)Device Model number`
 *    rev          - (OUT)Device Revision number
 * Notes: 
 */
STATIC int
_phy_bcm542xx_model_rev_get(int unit, phy_ctrl_t *pc,
                            int *oui, int *model, int *rev)
{
    uint16  id0, id1;

    SOC_IF_ERROR_RETURN
        (PHY_READ_BCM542XX_MII_PHY_ID0r(unit, pc, &id0));
    SOC_IF_ERROR_RETURN
        (PHY_READ_BCM542XX_MII_PHY_ID1r(unit, pc, &id1));

    *oui   = PHY_OUI(id0, id1);
    *model = PHY_MODEL(id0, id1);
    *rev   = PHY_REV(id0, id1);

    return SOC_E_NONE;
}


/*
 * Function:
 *    _phy_bcm542xx_auto_negotiate_ew (Autoneg-ed mode with E@W on).
 * Purpose:
 *    Determine autoneg-ed mode between
 *    two ends of a link
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *    speed - (OUT) greatest common speed.
 *    duplex - (OUT) greatest common duplex.
 *    link - (OUT) Boolean, true indicates link established.
 * Returns:
 *    SOC_E_XXX
 * Notes: 
 *    No synchronization performed at this level.
 */

static int
_phy_bcm542xx_auto_negotiate_ew(int unit, phy_ctrl_t *pc, int *speed, int *duplex)
{
    int        t_speed, t_duplex;
    uint16  mii_assr;

    SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_MII_AUX_STATUSr(unit, pc,&mii_assr));

    switch ((mii_assr >> 8) & 0x7) {
    case 0x7:
        t_speed = 1000;
        t_duplex = TRUE;
        break;
    case 0x6:
        t_speed = 1000;
        t_duplex = FALSE;
        break;
    case 0x5:
        t_speed = 100;
        t_duplex = TRUE;
        break;
    case 0x3:
        t_speed = 100;
        t_duplex = FALSE;
        break;
    case 0x2:
        t_speed = 10;
        t_duplex = TRUE;
        break;
    case 0x1:
        t_speed = 10;
        t_duplex = FALSE;
        break;
    default:
        t_speed = 0; /* 0x4 is 100BASE-T4 which is not supported */
        t_duplex = FALSE;
        break;
    }

    if (speed)  *speed  = t_speed;
    if (duplex)    *duplex = t_duplex;

    return(SOC_E_NONE);
}

/*
 * Function:
 *    _phy_bcm542xx_auto_negotiate_gcd (greatest common denominator).
 * Purpose:
 *    Determine the current greatest common denominator between 
 *    two ends of a link
 * Parameters:
 *    speed - (OUT) greatest common speed.
 *    duplex - (OUT) greatest common duplex.
 *    link - (OUT) Boolean, true indicates link established.
 * Notes: 
 */

STATIC int
_phy_bcm542xx_auto_negotiate_gcd(int unit,soc_port_t port, phy_ctrl_t *pc, int *speed, int *duplex)
{
    int       t_speed, t_duplex;
    uint16 mii_ana, mii_anp, mii_stat;
    uint16 mii_gb_stat, mii_esr, mii_gb_ctrl;
#ifdef INCLUDE_PLP_IMACSEC
    int speed_m;
    unsigned int  port_m;
    bcm_plp_base_t_sec_phy_access_t *imacsec_des;
    uint16 imacsec_cfg;
    soc_port_t port_s;
    PHY_BCM542XX_DEV_DESC_t *phy_des=NULL;
    unsigned int speed_mask=0x3;
#else
    COMPILER_REFERENCE(port);
#endif
#ifdef INCLUDE_PLP_IMACSEC
    imacsec_des = IMACSEC_DES_PTR_GET(pc);
    phy_des = (PHY_BCM542XX_DEV_DESC_t *)(pc+1);
#endif

    mii_gb_stat = 0;            /* Start off 0 */
    mii_gb_ctrl = 0;            /* Start off 0 */

    SOC_IF_ERROR_RETURN(
        PHY_READ_BCM542XX_MII_ANAr(unit, pc,&mii_ana));
    SOC_IF_ERROR_RETURN(
        PHY_READ_BCM542XX_MII_ANPr(unit, pc,&mii_anp));
    SOC_IF_ERROR_RETURN(
        PHY_READ_BCM542XX_MII_STATr(unit, pc,&mii_stat));

    if (mii_stat & PHY_BCM542XX_MII_STAT_ES) {    /* Supports extended status */
        /*
         * If the PHY supports extended status, check if it is 1000MB
         * capable.  If it is, check the 1000Base status register to see
         * if 1000MB negotiated.
         */
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_ESRr(unit, pc,&mii_esr));

        if (mii_esr & (PHY_BCM542XX_MII_ESR_1000_X_FD | PHY_BCM542XX_MII_ESR_1000_X_HD | 
                       PHY_BCM542XX_MII_ESR_1000_T_FD | PHY_BCM542XX_MII_ESR_1000_T_HD) ) {
            SOC_IF_ERROR_RETURN(
                PHY_READ_BCM542XX_MII_GB_STATr(unit, pc,
                                                   &mii_gb_stat));
            SOC_IF_ERROR_RETURN(
                PHY_READ_BCM542XX_MII_GB_CTRLr(unit, pc,
                                                   &mii_gb_ctrl));
        }
    }

    /*
     * At this point, if we did not see Gig status, one of mii_gb_stat or 
     * mii_gb_ctrl will be 0. This will cause the first 2 cases below to 
     * fail and fall into the default 10/100 cases.
     */
    mii_ana &= mii_anp;

    if ((mii_gb_ctrl & PHY_BCM542XX_MII_GB_CTRL_ADV_1000FD) &&
        (mii_gb_stat & PHY_BCM542XX_MII_GB_STAT_LP_1000FD) ) {
        t_speed  = 1000;
        t_duplex = 1;
    } else if ((mii_gb_ctrl & PHY_BCM542XX_MII_GB_CTRL_ADV_1000HD) &&
               (mii_gb_stat & PHY_BCM542XX_MII_GB_STAT_LP_1000HD) ) {
        t_speed  = 1000;
        t_duplex = 0;
    } else if (mii_ana & PHY_BCM542XX_MII_ANA_FD_100) {        /* [a] */
        t_speed = 100;
        t_duplex = 1;
    } else if (mii_ana & PHY_BCM542XX_MII_ANA_T4) {            /* [b] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & PHY_BCM542XX_MII_ANA_HD_100) {        /* [c] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & PHY_BCM542XX_MII_ANA_FD_10) {         /* [d] */
        t_speed = 10;
        t_duplex = 1 ;
    } else if (mii_ana & PHY_BCM542XX_MII_ANA_HD_10) {         /* [e] */
        t_speed = 10;
        t_duplex = 0;
    } else {
        return(SOC_E_FAIL);
    }

    if (speed) {
        *speed  = t_speed;
    }
    if (duplex) {
        if (t_duplex) {
            *duplex = TRUE;
        } else {
            *duplex = FALSE;
        }
    }
#ifdef INCLUDE_PLP_IMACSEC
    if(phy_des->port_pre_speed != t_speed){   /* If speed changes from 10M/100M to 1G speed or vice-versa, MAC needs to be reconfigured again to restart autoneg */
        if(pc->macsec_enable){
            port_m = get_port(&imacsec_des->phy_info);
            SPEED_MAPPING(t_speed,speed_m);
            /* Use base phy id to write top register */
            pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
            /* SW set speed MACSEC_CONFIG2 reg */
            SOC_IF_ERROR_RETURN
                (phy_bcm542xx_cl45_reg_read(unit, pc,  0x7, 0x9870, &imacsec_cfg));
            imacsec_cfg |= 1<<port_m;
            SOC_IF_ERROR_RETURN
                (phy_bcm542xx_cl45_reg_write(unit, pc,  0x7, 0x9870, imacsec_cfg));
            /* SW set speed MACSEC_CONFIG3 reg */
            SOC_IF_ERROR_RETURN
                (phy_bcm542xx_cl45_reg_read(unit, pc,  0x7, 0x9871, &imacsec_cfg));
            imacsec_cfg &= ~(speed_mask << (port_m*2));
            imacsec_cfg |= (speed_m << (port_m*2));
            SOC_IF_ERROR_RETURN
               (phy_bcm542xx_cl45_reg_write(unit, pc,  0x7, 0x9871, imacsec_cfg));
            /* Restore the phy mdio address */
            pc->phy_id =  PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

            port_s= PHY_UNIMAC_V2_SWITCH_PORT(port);
            SOC_IF_ERROR_RETURN(
               BMACSEC_MAC_AUTO_CONFIG_SET((phy_mac_ctrl_t *)imacsec_des->unimac, port_s, 0));

            SOC_IF_ERROR_RETURN(
            BMACSEC_MAC_AUTO_CONFIG_SET((phy_mac_ctrl_t *)imacsec_des->unimac, port_s, 1));
        }
    }
    phy_des->port_pre_speed = t_speed;
#endif

    return SOC_E_NONE;
}


STATIC int
_phy_bcm542xx_serdes_update(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    /* check if ( Copper_disabled ) or ( fiber enabled and preferred ) */
    if ( (!(pc->copper.enable)) ||
          ((pc->fiber.enable) && (pc->fiber.preferred)) ) {
        /* turn on the passive external SerDes 100FX if fiber 100FX mode */
        uint16 serdes_mode = (pc->fiber.force_speed== 100)
                           ? PHY_BCM542XX_EXT_SERDES_FX : 0;
        /* External Serdes Control Reg., DIGX_SHD_1C_14, RDB_0x234 */
        SOC_IF_ERROR_RETURN(
            PHY_MODIFY_BCM542XX_EXT_SERDES_CTRLr(unit, pc, serdes_mode,
                                        PHY_BCM542XX_EXT_SERDES_FX_MASK) );
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_bcm542xx_medium_status
 * Purpose:
 *      Indicate the current active medium
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
phy_bcm542xx_medium_status(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    if ( PHY_COPPER_MODE(unit, port) ) {
        *medium = SOC_PORT_MEDIUM_COPPER;
    } else if ( PHY_FIBER_MODE(unit, port) ) {
        *medium = SOC_PORT_MEDIUM_FIBER;
    } else {
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_bcm542xx_medium_config_update
 * Purpose:
 *      Update the PHY with config parameters
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      cfg - Config structure.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_bcm542xx_medium_config_update(int unit, soc_port_t port,
                                   soc_phy_config_t *cfg)
{
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_speed_set(unit, port, cfg->force_speed));
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_duplex_set(unit, port, cfg->force_duplex));
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_master_set(unit, port, cfg->master));
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_ability_advert_set(unit, port, &cfg->advert_ability));
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_autoneg_set(unit, port, cfg->autoneg_enable));
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_mdix_set(unit, port, cfg->mdix));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_bcm542xx_medium_check
 * Purpose:
 *      Determine if chip should operate in copper or fiber mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      medium - (OUT) SOC_PORT_MEDIUM_XXX
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_bcm542xx_medium_check(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    phy_ctrl_t    *pc = EXT_PHY_SW_STATE(unit, port);
    uint16 mode_ctrl;

    *medium = SOC_PORT_MEDIUM_COPPER;

    if ( pc->automedium) {
        SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_MODE_CTRLr(unit, pc,&mode_ctrl));

        if ( pc->fiber.preferred) {
           if ((mode_ctrl & 0x30) == 0x20) {
                /* Only Copper is linked up */
                *medium = SOC_PORT_MEDIUM_COPPER;
            } else {
                *medium = SOC_PORT_MEDIUM_FIBER;
            }
        } else {
            if ((mode_ctrl & 0x30) == 0x10) {
                /* Only Fiber is linked up */
                *medium = SOC_PORT_MEDIUM_FIBER;
            } else {
                *medium = SOC_PORT_MEDIUM_COPPER;
            }
        }
    } else {
       *medium = pc->fiber.preferred ? SOC_PORT_MEDIUM_FIBER
                                     : SOC_PORT_MEDIUM_COPPER; 
    }
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit, "_phy_bcm542xx_medium_check: "
                            "u=%d p=%d fiber_pref=%d fiber=%d\n"),
                 unit, port, pc->fiber.preferred,
                 (*medium == SOC_PORT_MEDIUM_FIBER) ? 1 : 0));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_bcm542xx_medium_change
 * Purpose:
 *      Change medium when media change detected or forced
 * Parameters:
 *      unit         - StrataSwitch unit #.
 *      port         - StrataSwitch port #.
 *      force_update - Force update
 *      force_medium - Force update medium
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_bcm542xx_medium_change(int unit, soc_port_t port,
                            soc_port_medium_t force_medium)
{
    phy_ctrl_t        *pc;
    soc_port_medium_t  medium;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_bcm542xx_medium_check(unit, port, &medium));

    if ( ((medium == SOC_PORT_MEDIUM_COPPER) && (!PHY_COPPER_MODE(unit, port)))
         || (force_medium == SOC_PORT_MEDIUM_COPPER) ) {
        /* Was fiber  or  being forced copper */ 
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);

        SOC_IF_ERROR_RETURN
            (_phy_bcm542xx_no_reset_setup(unit, port));
        SOC_IF_ERROR_RETURN
            (_phy_bcm542xx_serdes_update(unit, port));

        /* Do Not power up the interface if medium is disabled */
        if ( pc->copper.enable) {
            SOC_IF_ERROR_RETURN
                (_phy_bcm542xx_medium_config_update(unit, port, &pc->copper));
        }
        LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                  "%s() u=%d p=%d [F->C] force_medium=%d\n"), __func__,
                  unit, port, force_medium));

        /* Configure BCM542XX to RGMII-to-copper mode */
        if ( IS_GMII_PORT(unit, port) && PHY_IS_BCM54210_220(pc) ) {
            _phy_bcm542xx_rgmii_to_copper_mode(unit, port);
        }

    } else
    if ( ((medium == SOC_PORT_MEDIUM_FIBER) && (PHY_COPPER_MODE(unit, port)))
         || (force_medium == SOC_PORT_MEDIUM_FIBER) ) {
        /* Was copper  or  being forced fiber */ 
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_MEDIUM_CHANGE);

        SOC_IF_ERROR_RETURN
            (_phy_bcm542xx_no_reset_setup(unit, port));
        SOC_IF_ERROR_RETURN
            (_phy_bcm542xx_serdes_update(unit, port));

        if ( pc->fiber.enable ) {
            SOC_IF_ERROR_RETURN
                (_phy_bcm542xx_medium_config_update(unit, port, &pc->fiber));
        }
        LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
              "%s() u=%d p=%d [C->F] force_medium=%d\n"), __func__,
              unit, port, force_medium));
       
        /* Configure BCM542XX to RGMII-to-Fiber mode */
        if ( IS_GMII_PORT(unit, port) && PHY_IS_BCM54210_220(pc) ) {
            _phy_bcm542xx_rgmii_to_fiber_mode(unit, port);
        } 
    }

    return SOC_E_NONE;
}

/***********************************************************************
 *
 * PHY542xx DRIVER ROUTINES
 */

/*
 * Function:
 *      phy_bcm542xx_medium_config_set
 * Purpose:
 *      Set the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - Operating parameters
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
phy_bcm542xx_medium_config_set(int unit, soc_port_t port, 
                               soc_port_medium_t  medium,
                               soc_phy_config_t  *cfg)
{
    phy_ctrl_t        *pc;
    soc_phy_config_t  *active_medium;  /* Currently active medium */
    soc_phy_config_t  *change_medium;  /* Requested medium */
    soc_phy_config_t  *other_medium;   /* The other medium */
    soc_port_ability_t advert_ability;
    int                medium_update;

    if ( NULL == cfg ) {
        return SOC_E_PARAM;
    }
    pc            = EXT_PHY_SW_STATE(unit, port);
    medium_update = FALSE;
    /* clear the struct advert_ability */
    sal_memset(&advert_ability, 0, sizeof(soc_port_ability_t));

    switch ( medium ) {
    case  SOC_PORT_MEDIUM_COPPER:
        if (!pc->automedium && !PHY_COPPER_MODE(unit, port) ) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->copper;
        other_medium   = &pc->fiber;
        ADVERT_ALL_COPPER(&advert_ability);

        break;
    case  SOC_PORT_MEDIUM_FIBER:
        if (!pc->automedium && !PHY_FIBER_MODE(unit, port) ) {
            return SOC_E_UNAVAIL;
        }
        change_medium  = &pc->fiber;
        other_medium   = &pc->copper;
        ADVERT_ALL_FIBER(&advert_ability);
        break;
    default:
        return SOC_E_PARAM;
    }

    /* Changes take effect immediately if the                   *\
    \* target medium is active or the preferred medium changes. */
    if ( change_medium->enable != cfg->enable ) {
        medium_update = TRUE;
    }
    if ( change_medium->preferred != cfg->preferred ) {
        /* Make sure that only one medium is preferred */
        other_medium->preferred = !cfg->preferred;
        medium_update = TRUE;
    }

    /* copy given cfg to change_medium and update advert_ability */
    sal_memcpy(change_medium, cfg, sizeof(*change_medium));
    change_medium->advert_ability.pause &= advert_ability.pause;
    change_medium->advert_ability.speed_half_duplex &= advert_ability.speed_half_duplex;
    change_medium->advert_ability.speed_full_duplex &= advert_ability.speed_full_duplex;
    change_medium->advert_ability.interface &= advert_ability.interface;
    change_medium->advert_ability.medium &= advert_ability.medium;
    change_medium->advert_ability.loopback &= advert_ability.loopback;
    change_medium->advert_ability.flags &= advert_ability.flags;

    SOC_IF_ERROR_RETURN( _phy_bcm542xx_serdes_update(unit, port) );

    if ( medium_update ) {
        /* The new config may cause medium change. Check and update medium */
        SOC_IF_ERROR_RETURN
            (_phy_bcm542xx_medium_change(unit, port, medium));
    } else {
        active_medium = (PHY_COPPER_MODE(unit, port)) ? &pc->copper
                                                      : &pc->fiber;
        if ( active_medium == change_medium ) {
            /* If the medium to update is active, update the configuration */
            SOC_IF_ERROR_RETURN
                (_phy_bcm542xx_medium_config_update(unit, port, change_medium));
        }
        else {
            /* Make sure that connected medium has been updated if port is disabled */
            if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE) ) {
                /* Check for medium change and update HW/SW accordingly. */
                SOC_IF_ERROR_RETURN
                    (_phy_bcm542xx_medium_change(unit, port, SOC_PORT_MEDIUM_NONE));
            }
        } 
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_bcm542xx_medium_config_get
 * Purpose:
 *      Get the operating parameters that are automatically selected
 *      when medium switches type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 *      medium - SOC_PORT_MEDIUM_COPPER/FIBER
 *      cfg - (OUT) Operating parameters
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_bcm542xx_medium_config_get(int unit, soc_port_t port, 
                               soc_port_medium_t medium,
                               soc_phy_config_t *cfg)
{
    int            rv;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = SOC_E_NONE;

    switch ( medium ) {
    case SOC_PORT_MEDIUM_COPPER:
        if ( pc->automedium || PHY_COPPER_MODE(unit, port) ) {
            sal_memcpy(cfg, &pc->copper, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PORT_MEDIUM_FIBER:
        if ( pc->automedium || PHY_FIBER_MODE(unit, port) ) {
            sal_memcpy(cfg, &pc->fiber, sizeof (*cfg));
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    default:
        rv = SOC_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *      _phy_bcm542xx_no_reset_setup
 * Purpose:
 *      Setup the operating parameters of the PHY without resetting PHY
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - Port number
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_phy_bcm542xx_no_reset_setup(int unit, soc_port_t port)
{
    phy_ctrl_t    *pc;
    int rv = SOC_E_NONE;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "%s: u=%d p=%d medium=%s\n"),
              __func__, unit, port,
              PHY_COPPER_MODE(unit, port) ? "COPPER" : "FIBER"));

    pc = EXT_PHY_SW_STATE(unit, port);
    rv = phy_bcm542xx_init_setup(unit, port, 0, 
                                     pc->automedium,
                                     pc->fiber.preferred,
                                     pc->fiber_detect,
                                     pc->fiber.enable,
                                     pc->copper.enable,
                                     pc->ext_phy_autodetect_en,
                                     pc->ext_phy_fiber_iface); 
    if ( rv != SOC_E_NONE ) {
        return SOC_E_FAIL;
    }
    return rv;
}

/*
 * Function:
 *    phy_bcm542xx_set_led_selectors
 * Purpose:
 *    Set LED modes and Control.
 * Parameters:
 *    phy_dev_addr    - PHY's device address
 *    led_mode0       - LED Mode 0
 *    led_mode1       - LED Mode 1
 *    led_mode2       - LED Mode 2
 *    led_mode3       - LED Mode 3
 *    led_select      - LED select 
 *    led_select_ctrl        - LED Control
 * Returns:
 */
int
phy_bcm542xx_set_led_selectors(int unit, phy_ctrl_t *pc)
{
    uint16 data;
    uint16 led_link_speed_mode;

    /* Configure LED selectors */
    data = (((pc->ledmode[1] & 0xf) << 4) | (pc->ledmode[0] & 0xf));
    SOC_IF_ERROR_RETURN
        (PHY_WRITE_BCM542XX_LED_SELECTOR_1r(unit, pc, data));

    data = ((pc->ledmode[3] & 0xf) << 4) | (pc->ledmode[2] & 0xf);
    SOC_IF_ERROR_RETURN
        (PHY_WRITE_BCM542XX_LED_SELECTOR_2r(unit, pc, data));

    data = (pc->ledctrl & 0x3ff);
    SOC_IF_ERROR_RETURN
        (PHY_WRITE_BCM542XX_LED_CTRLr(unit, pc, data));

    SOC_IF_ERROR_RETURN
        (PHY_WRITE_BCM542XX_EXP_LED_SELECTORr(unit, pc, pc->ledselect));

    led_link_speed_mode = soc_property_port_get(unit, pc->port, 
                              spn_PHY_LED_LINK_SPEED_MODE, 0x0);
    switch(led_link_speed_mode) {
        case 1:
            SOC_IF_ERROR_RETURN
                (PHY_MODIFY_BCM542XX_SPARE_CTRLr(unit, pc,
                     PHY_BCM542XX_SPARE_CTRL_REG_LINK_LED, 
                     PHY_BCM542XX_SPARE_CTRL_REG_LINK_LED
                     | PHY_BCM542XX_SPARE_CTRL_REG_LINK_SPEED_LED));
            break;
        case 2:
            SOC_IF_ERROR_RETURN
                (PHY_MODIFY_BCM542XX_SPARE_CTRLr(unit, pc,
                     PHY_BCM542XX_SPARE_CTRL_REG_LINK_SPEED_LED, 
                     PHY_BCM542XX_SPARE_CTRL_REG_LINK_SPEED_LED
                     | PHY_BCM542XX_SPARE_CTRL_REG_LINK_LED));
            break;
        default:
            SOC_IF_ERROR_RETURN
                (PHY_MODIFY_BCM542XX_SPARE_CTRLr(unit, pc,
                     0,
                     PHY_BCM542XX_SPARE_CTRL_REG_LINK_LED  
                     | PHY_BCM542XX_SPARE_CTRL_REG_LINK_SPEED_LED));
            break;
    }   

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm54280_init    ( for 54280 / 290 )
 * Purpose:
 *    54280 Device specific Initialization 
 * Parameters:
 *    unit, port          - 
 * Notes: 
 */
STATIC int
_phy_bcm54280_init(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
#ifdef INCLUDE_PLP_IMACSEC
    int rv;
    int uport;
    unsigned int ms_dev_addr =
           soc_property_port_get(unit, port, spn_MACSEC_DEV_ADDR, -1);

    unsigned int ms_slice    =
           soc_property_port_get(unit, port, spn_MACSEC_PORT_INDEX, 0);

    int imacsec_mdio_addr;
    bcm_plp_base_t_sec_phy_access_t *imacsec_des;
#endif

    pc = EXT_PHY_SW_STATE(unit, port);
    pc->driver_data = (void *)(pc+1);
#ifdef INCLUDE_PLP_IMACSEC
    /* Base PHY ID is used to access MACSEC core */
    imacsec_mdio_addr = soc_property_port_get(unit, port,
                                            spn_MACSEC_DEV_ADDR, -1);
    pc->macsec_enable = soc_property_port_get(unit, port, spn_MACSEC_ENABLE, 0);
    imacsec_des = IMACSEC_DES_PTR_GET(pc);
    imacsec_des->macsec_dev_addr = IMACSEC_MDIO_ADDR_GET(imacsec_mdio_addr);
    imacsec_des->phy_info.lane_map = 
               soc_property_port_get(unit, port, spn_MACSEC_PORT_INDEX, 0);
    imacsec_des->phy_info.lane_map =  1 << imacsec_des->phy_info.lane_map ;
#endif
    /*
     * Configure LED mode and Jumbo frame, set max Jumbo packet length to 18KB
     */

    /*
     * PHY Extended Control Reg., RDB.0x000, Legacy Reg.0x10
     * bit[0] GMII_FIFO_JUMBO_LSB
     * bit[5] LED_TRAFFIC_EN, Enable LED traffic mode
     */
    SOC_IF_ERROR_RETURN
        (PHY_MODIFY_BCM542XX_MII_ECRr(unit, pc,
                            PHY_BCM542XX_MII_ECR_FIFO_ELAST_0
                          | PHY_BCM542XX_MII_ECR_EN_LEDT,
                            PHY_BCM542XX_MII_ECR_FIFO_ELAST_0
                          | PHY_BCM542XX_MII_ECR_EN_LEDT));

    /*
     * Pattern Generator Status Reg., RDB.0x006, Legacy EXP_Reg.0x46
     * bit[14] GMII_FIFO_JUMBO_MSB
     * bit[15] GMII_FIFO_JUMBO_MSB
     */
    SOC_IF_ERROR_RETURN
        (PHY_MODIFY_BCM542XX_EXP_PATT_GEN_STATr(unit, pc,
                            PHY_BCM542XX_PATT_GEN_STAT_FIFO_ELAST_1
                          | PHY_BCM542XX_PATT_GEN_STAT_GMII_FIFO_JUMBO_MSB,
                            PHY_BCM542XX_PATT_GEN_STAT_FIFO_ELAST_1
                          | PHY_BCM542XX_PATT_GEN_STAT_GMII_FIFO_JUMBO_MSB));

    /* Disable USGMII mode. Clear RDB.0x0810 Bit[6]  */ 
    if (PHY_IS_BCM54182(pc) || PHY_IS_BCM54185(pc) ||
        PHY_IS_BCM54192(pc) || PHY_IS_BCM54195(pc) ) {   
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                        PHY_BCM542XX_DISABLE_USGMII,
                        0, PHY_BCM542XX_DISABLE_USGMII_6U_BIT_SEL) );
    }

    /*
     * Misc 1000-X Control 2 Reg., RDB.0x236, Legacy Reg.0x1C shadow 0x16
     * bit[0] PRIMARY_SERDES_JUMBO_MSB
     */
    SOC_IF_ERROR_RETURN
        (PHY_MODIFY_BCM542XX_1ST_SERDES_CTRLr(unit, pc,
                            PHY_BCM542XX_SERDES_CTRL_JUMBO_MSB,
                            PHY_BCM542XX_SERDES_CTRL_JUMBO_MSB));

    /*
     * Auto Detect SGMII MC Reg., RDB.0x238
     * bit[2] GMII_FIFO_JUMBO_LSB
     */
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_AUTO_DETECT_SGMII_MC_REG_OFFSET,
                            PHY_BCM542XX_AUTO_DETECT_GMII_FIFO_JUMBO_LSB,
                            PHY_BCM542XX_AUTO_DETECT_GMII_FIFO_JUMBO_LSB));
 
    /*
     * Auxiliary 1000-X Contro Reg., RDB.0x23B, Legacy Reg.0x1C shadow 0x1B
     * bit[1] PRIMARY_SERDES_JUMBO_LSB
     */
    SOC_IF_ERROR_RETURN
        (PHY_MODIFY_BCM542XX_AUX_1000X_CTRLr(unit, pc,
                            PHY_BCM542XX_SERDES_CTRL_JUMBO_LSB,
                            PHY_BCM542XX_SERDES_CTRL_JUMBO_LSB));

    /*
     * Secondary Serdes MISC 1000-X Control 2 Reg. RDB.0xB16
     * bit[0] SECONDARY_SERDES_JUMBO_MSB
     */
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_2ND_SERDES_MISC_1000x_CTL_2_REG_OFFSET,
                            PHY_BCM542XX_2ND_SERDES_MISC_1000x_CTL_2_2ND_SERDES_MSB,
                            PHY_BCM542XX_2ND_SERDES_MISC_1000x_CTL_2_2ND_SERDES_MSB));

    /*
     * Secondary Serdes Auxiliary 1000-X Control Reg. RDB.0xB1B
     * bit[1] SECONDARY_SERDES_JUMBO_LSB
     */
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_2ND_SERDES_AUX_1000x_CTL_REG_OFFSET,
                            PHY_BCM542XX_2ND_SERDES_AUX_1000x_CTL_2ND_SERDES_LSB,
                            PHY_BCM542XX_2ND_SERDES_AUX_1000x_CTL_2ND_SERDES_LSB));

    /*
     * Copper Auxiliary Control Reg., RDB.0x028, Legacy Reg.0x18 shadow 0
     * bit[14] EXT_PKT_LEN(100BT_JUMBO), 100BASE-Tx jumbo frames support
     */
    SOC_IF_ERROR_RETURN
        (PHY_MODIFY_BCM542XX_MII_AUX_CTRLr(unit, pc,
                            PHY_BCM542XX_MII_AUX_CTRL_REG_EXT_PKT_LEN,
                            PHY_BCM542XX_MII_AUX_CTRL_REG_EXT_PKT_LEN));

    /*
     * Secondary Serdes 1000-X Error Counter Setting Reg. RDB.0xB10
     * bit[0] EXT_PKT_LEN
     */
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_2ND_SERDES_1000x_ERR_CNT_SET_REG_OFFSET,
                            PHY_BCM542XX_2ND_SERDES_1000x_ERR_CNT_SET_EXT_PKT_LEN,
                            PHY_BCM542XX_2ND_SERDES_1000x_ERR_CNT_SET_EXT_PKT_LEN));


#if defined (INCLUDE_PLP_IMACSEC)  /*  */
    if(pc->macsec_enable){
		/*  MACSEC  is ENABLED 
        *  MACSEC  Specific Initialization
        *
        * Some reasonable defaults
        */
         /* default MACSEC MAC policy: MACSEC_SWITCH_FOLLOWS_LINE == 1 */
        pc->macsec_mac_policy = soc_property_port_get(unit, port,
                                              spn_MACSEC_SWITCH_SIDE_POLICY,
                                              1);

        pc->macsec_switch_fixed_speed  = 0;
        /* If switch mac policy is PHY_MAC_SWITCH_DUPLEX_GATEWAY set the 
         * duplex mode on line side as half duplex and system side as full duplex */
            
        pc->macsec_switch_fixed_duplex = 1;
        pc->macsec_switch_fixed_pause  = 1;
        pc->macsec_pause_rx_fwd = 0;
        pc->macsec_pause_tx_fwd = 0;
        pc->macsec_line_ipg     = 0xc;
        pc->macsec_switch_ipg   = 0xc;


        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

        /* DATAPATH_SELECT  :
         * [15:8] 0 means port NOT bypassed  , [7:0] 0 means 1588 not encrypted
         */
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_direct_reg_modify(unit, pc, 0x84a, 0x0000,
                                            1 << (ms_slice + 8) | 0xff ));

        /* MACSEC_PWRDN  :
         * [15]    1 means MACSEC MDIO enabled
         * [14:10] 0  reset value for QUAD1 PHY addr
         * [9:8] 0 means quad1,quad0 power up
         * [7:0] 0 means port power up
         */
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_direct_reg_modify(unit, pc, 0x84b, 0x8000,
                                           0xff00 | 1 << (ms_slice ) ));
       /* MACSEC_OVERRIDE enable
        * [14] 1 means 1: overwrite with bit 13
        * bit 13, 0: macsec not disabled
       */

        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_direct_reg_modify(unit, pc, 0x84d, 0x40ff,
                                           0xffff ));

        /* Switch side speed  config2 reg */
        /* bit 0 to 7 value = 0 means use line side , 1 means use config3 reg */
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_cl45_reg_write(unit, pc,  0x7, 0x9870, 0xFF));

        /* Switch side speed  config3 reg */
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_cl45_reg_write(unit, pc,  0x7, 0x9871, 0xAAAA));


        /* Restore the phy mdio address */
        pc->phy_id =  PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);



        /* register mdio read/write */
        _bcm_plp_register_mdio_primitives(
                unit, ms_slice, ms_dev_addr, pc->read , pc->write);


        /* Attach MAC driver */
        rv = _bcm_phy5419x_phy_mac_driver_attach( pc, 
                                                  ms_dev_addr,
                                                  pc->port,
                                                  imacsec_blmi_io_mmi2);
        if(rv != SOC_E_NONE) {
             LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                     "phy_mac_driver_attach "
                     "returned error for u=%d p=%d\n"), unit, port));
                    return SOC_E_FAIL;
        }


        /* Initialize the Line side MAC */
        uport = PHY_UNIMAC_V2_LINE_PORT(pc->port);
        SOC_IF_ERROR_RETURN(
            BMACSEC_MAC_INIT((phy_mac_ctrl_t *)imacsec_des->unimac, uport));

        /* Initialize the Switch side MAC */
        uport = PHY_UNIMAC_V2_SWITCH_PORT(pc->port);
        SOC_IF_ERROR_RETURN(
            BMACSEC_MAC_INIT((phy_mac_ctrl_t *)imacsec_des->unimac, uport));
    }
#endif  /* INCLUDE_PLP_IMACSEC  */

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm54282_init    ( for 54282 / 292 )
 * Purpose:
 *    Initalize the PHY device. 
 * Parameters:
 *    unit, port          - 
 * Notes: 
 */
STATIC int
_phy_bcm54282_init(int unit, soc_port_t port) 
{
    phy_ctrl_t  *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    /* 
    * Configure extended control register for led mode and 
    * jumbo frame support
    */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc); /* RV */

    /* Enable QSGMII MDIO sharing feature 
       - Enable access to QSGMII reg space using port3's MDIO address
       - Use PHYA[4:0]+3 (Port3's MDIO) instead of PHYA[4:0)+8 as MDIO address.
       - It saves one MDIO address for customer.
       - Access to this top level register via port 0 only
     */
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_TOP_MISC_TOP_CFG_REG_OFFSET,
                            PHY_BCM542XX_TOP_MISC_CFG_REG_QSGMII_SEL
                          | PHY_BCM542XX_TOP_MISC_CFG_REG_QSGMII_PHYA,
                            PHY_BCM542XX_TOP_MISC_CFG_REG_QSGMII_SEL
                          | PHY_BCM542XX_TOP_MISC_CFG_REG_QSGMII_PHYA));
    
    /* replace it with qsgmii phy id */
    pc->phy_id = PHY_BCM54282_QSGMII_DEV_ADDR_SHARED(pc);    
    
    /* QSGMII FIFO Elasticity */
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_qsgmii_reg_write(unit, pc,
                            (int) PHY_BCM542XX_DEV_PHY_SLICE(pc) /*dev_port*/,
                            0x8300, 0x12, 0x0006));
    
    /* Restore access to Copper/Fiber register space.
     *  TOP lvl register, access through port0 only
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_TOP_MISC_TOP_CFG_REG_OFFSET,
                            0,
                            PHY_BCM542XX_TOP_MISC_CFG_REG_QSGMII_SEL
                          | PHY_BCM542XX_TOP_MISC_CFG_REG_QSGMII_PHYA));
    /* Restore the phy mdio address */
    pc->phy_id =  PHY_BCM542XX_DEV_PHY_ID_ORIG(pc); 

    /* system side QSGMII AutoNeg mode */
    if ( soc_property_port_get(unit, port, spn_PHY_SGMII_AUTONEG, TRUE) ) {
        PHY_BCM542XX_SYS_SIDE_AUTONEG_SET(pc);
    } else {
        PHY_BCM542XX_SYS_SIDE_AUTONEG_CLR(pc);
    }

    /* Configure LED mode and turn on jumbo frame support */
    _phy_bcm54280_init(unit, port);

    /* In case of QSGMII devices, there is a LPI pass through mode
       which has to be enabled for native EEE to work. In case of VNG, 
       it is already set by default. To enable it: 
    SOC_IF_ERROR_RETURN
       (phy_bcm542xx_qsgmii_reg_write(PHY_BCM54282_QSGMII_DEV_ADDR(_pc), 
                                      (int)PHY_BCM542XX_DEV_PHY_SLICE(pc),
                                      0x833e, 0x0e, 0xc000));
    */
    return SOC_E_NONE;
}

int
_phy_bcm542xx_rgmii_to_copper_mode(int unit, soc_port_t port)
{
    phy_ctrl_t    *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    /* Configure BCM542XX to RGMII-to-copper mode */
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MII_MISC_CTRLr(unit, pc, 0x71E7));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_CLK_ALIGN_CTRLr(unit, pc, 0x0E00));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_TOP_LVL_CONFGr(unit, pc, 0x00B0));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MODE_CTRLr(unit, pc, 0x7C01));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc, 0x1940));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MODE_CTRLr(unit, pc, 0x7C00));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MII_GB_CTRLr(unit, pc, 0x0200));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_1000X_MII_ANAr(unit, pc, 0x01E1));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc, 0x1340));

    return SOC_E_NONE;
}

int
_phy_bcm542xx_rgmii_to_fiber_mode(int unit, soc_port_t port)
{
    phy_ctrl_t    *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    /* Configure BCM542XX to RGMII-to-Fiber mode */
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MII_MISC_CTRLr(unit, pc, 0x71E7));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_CLK_ALIGN_CTRLr(unit, pc, 0x0E00));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_TOP_LVL_CONFGr(unit, pc, 0x00B0));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MODE_CTRLr(unit, pc, 0x7C02));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc, 0x1940));
     SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MODE_CTRLr(unit, pc, 0x7C03));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_AUTO_DETECT_MEDIUMr(unit, pc, 0x78E2));
    SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc, 0x1140));

    return SOC_E_NONE;
}


/*
 * Function:
 *    phy_bcm54200_init    ( for 54210/220/240/285/294/295/296/140 )  dual media
 * Purpose:
 *    Initalize the dual media (combo) PHY device.
 * Parameters:
 *    automedium            - Automedium select in combo ports
 *    fiber_preferred       - Fiber preferrence over copper
 *    fiber_detect          - Fiber Signal Detect
 *    ext_phy_autodetect_en - Enable/Disable SGMII Slave Autodetect function
 *    ext_phy_fiber_iface   - Fiber interface when Autodetect is disabled.
 * Notes: 
 * Fiber Detect  Detect Fiber signal 
 *  Automedium     Fiber Preferred       Active Medium
 * ------------  ------------------- -----------------------------------------
 *     0                 1            Fiber  mode is forced
 *     0                 0            Copper mode is forced
 *     1                 1            Auto medium is enabled,  Fiber preferred
 *     1                 0            Auto medium is enabled, Copper preferred
 */
STATIC int
_phy_bcm54200_init(int unit, soc_port_t port, int automedium,
                   int fiber_preferred, int fiber_detect,
                   int fiber_enable   , int copper_enable,
                   int ext_phy_autodetect_en, int ext_phy_fiber_iface)
{
    uint16         data, mask;
    phy_ctrl_t    *pc;
    /*
     * Don't power up the PHY if PHY is administratively disabled.
     */
    uint16 pwr_down_bit = PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE) ? PHY_BCM542XX_MII_CTRL_PWR_DOWN : 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Configure Auto-detect Medium (0x1c shadow 11110, RDB.0x23e) */
    mask = PHY_BCM542XX_MII_AUTO_DET_MASK;    /* Auto-detect bit mask */
    data = 0x0;
    if ( PHY_IS_BCM54210_220(pc) ) {
        /* Enable dual serdes auto-detect medium */  /* 0xfae7 */
        data |= (PHY_BCM542XX_MII_AUTO_DET_MED_2ND_SERDES |
                 PHY_BCM542XX_MII_FIBER_IN_USE_LED        |
                 PHY_BCM542XX_MII_FIBER_LED               |
                 PHY_BCM542XX_MII_FIBER_SD_SYNC);
    }
    if ( automedium ) {
        /* Enable auto-detect medium */
        data |= PHY_BCM542XX_MII_AUTO_DET_MED_EN;
    }

    /* When no link partner exists, fiber should be default medium */
    /* When both link partners exipc, fiber should be prefered medium
     * for the following reasons.
     * 1. Most fiber module generates signal detect when the fiber cable is
     *    plugged in regardless of any activity even if the link partner is
     *    powered down.
     * 2. Fiber mode consume much lesser power than copper mode.
     */
    if ( fiber_preferred ) {
                 /* Fiber selected when both media are active */
        data |= (PHY_BCM542XX_MII_AUTO_DET_MED_PRI |
                 PHY_BCM542XX_MII_AUTO_DET_MED_DEFAULT);
                 /* Fiber selected when no medium is active */
    }

    /* Qulalify fiber link with SYNC from PCS. */
    data |= PHY_BCM542XX_MII_FIBER_SD_SYNC;
    
    /*
     * Enable internal inversion of LED2/SD input pin.  Used only if
     * the fiber module provides RX_LOS instead of Signal Detect.
     */
    if ( fiber_detect < 0 ) {
        /* Fiber signal detect is active low from pin */
        data |= PHY_BCM542XX_MII_INV_FIBER_SD;
    }
    
    /* Auto-Detect Medium Register (DIGX_SHD1C_1E, RDB.0x23e) */
    /* coverity[copy_paste_error : FALSE] */
    SOC_IF_ERROR_RETURN
        (PHY_MODIFY_BCM542XX_AUTO_DETECT_MEDIUMr(unit, pc, data, mask));

    if ( PHY_IS_BCM54210_220(pc) ) {
        /* Power up primary SerDes, Fiber MII_CONTROL Reg. bit[11]*/
        SOC_IF_ERROR_RETURN
                (PHY_MODIFY_BCM542XX_PRI_SERDES_MII_CTRLr(unit, pc, 0,
                                      PHY_BCM542XX_MII_CTRL_PWR_DOWN));
        data = PHY_BCM542XX_MODE_SEL_SGMII_2_COPPER;
    } else {
        data = PHY_BCM542XX_MODE_SEL_COPPER_2_SGMII;
    }
    /* MODE_CONTROL register,  DIGX_SHD_1C_1F, RDB_0x21 */
    SOC_IF_ERROR_RETURN
        (PHY_MODIFY_BCM542XX_MODE_CTRLr(unit, pc, data,
                                      PHY_BCM542XX_MODE_CNTL_MODE_SEL_MASK));

    /*----------- COPPER INTERFACE ---------- */
    if ( (! copper_enable) || (!automedium && fiber_preferred) ) {
        SOC_IF_ERROR_RETURN
            (PHY_MODIFY_BCM542XX_MII_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_CTRL_PWR_DOWN,
                                      PHY_BCM542XX_MII_CTRL_PWR_DOWN));
    } else {
        if (!pwr_down_bit) {
        SOC_IF_ERROR_RETURN
            (PHY_MODIFY_BCM542XX_MII_CTRLr(unit, pc,
                                      0,
                                      PHY_BCM542XX_MII_CTRL_PWR_DOWN));
        }
        SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MII_GB_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_GB_CTRL_ADV_1000FD
                                    | PHY_BCM542XX_MII_GB_CTRL_PT));
        SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MII_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_CTRL_FD
                                    | PHY_BCM542XX_MII_CTRL_SS_10
                                    | PHY_BCM542XX_MII_CTRL_SS_100
                                    | PHY_BCM542XX_MII_CTRL_AN_EN
                                    | pwr_down_bit
                                    | PHY_BCM542XX_MII_CTRL_RST_AN));

        /* Enable/disable auto-detection between SGMII-slave and 1000BASE-X */
        if ( PHY_IS_BCM54210_220(pc) ) {
            /* External Serdes Control Reg., DIGX_SHD_1C_14, RDB_0x234 */
            SOC_IF_ERROR_RETURN
                    (PHY_MODIFY_BCM542XX_EXT_SERDES_CTRLr(unit, pc, 0x0,
                                      PHY_BCM542XX_EXT_SERDES_FX_MASK));
            /* SGMII Slave Control Register, DIGX_SHD_1C_15, RDB_0x235 */
            SOC_IF_ERROR_RETURN
                    (PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc, 0,
                                      PHY_BCM542XX_SGMII_SLAVE_AUTO));
        } else {  /* PHY is 54240/28x/29x */
            /* SGMII Slave Control Register, DIGX_SHD_1C_15, RDB_0x235 */
            SOC_IF_ERROR_RETURN
                    (PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc,
                                      PHY_BCM542XX_SGMII_SLAVE_AUTO,
                                      PHY_BCM542XX_SGMII_SLAVE_AUTO));
            SOC_IF_ERROR_RETURN        /* Power down SerDes */
                    (PHY_MODIFY_BCM542XX_1000X_MII_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_CTRL_PWR_DOWN,
                                      PHY_BCM542XX_MII_CTRL_PWR_DOWN));
        }

        /* Configure BCM542XX to RGMII-to-copper mode */
        if ( IS_GMII_PORT(unit, port) && PHY_IS_BCM54210_220(pc) ) {
            _phy_bcm542xx_rgmii_to_copper_mode(unit, port);
        }
    }

    /*----------- FIBER INTERFACE ---------- */
    if ( fiber_enable && (automedium || fiber_preferred) ) {
        /* remove power down of SerDes */
        if (!pwr_down_bit) {
        SOC_IF_ERROR_RETURN
            (PHY_MODIFY_BCM542XX_1000X_MII_CTRLr(unit, pc, 0,
                                      PHY_BCM542XX_MII_CTRL_PWR_DOWN));
        }
        /* set the advertisement of serdes */
        data = PHY_BCM542XX_1000X_MII_ANA_FD    |
               PHY_BCM542XX_1000X_MII_ANA_HD    |
               PHY_BCM542XX_1000X_MII_ANA_PAUSE |
               PHY_BCM542XX_1000X_MII_ANA_ASYM_PAUSE;
        /*Advertising Next Page capability in 1000X AN mode*/
        if (fiber_preferred && pc->fiber.autoneg_enable &&
            (ext_phy_autodetect_en == EXT_PHY_AUTODETECT_DIS) &&
            (ext_phy_fiber_iface == EXT_PHY_FIBER_IFACE_1000X)) {
            data |= PHY_BCM542XX_1000X_MII_ANA_NP;
        }

        mask = data;
        SOC_IF_ERROR_RETURN
            (PHY_MODIFY_BCM542XX_1000X_MII_ANAr(unit, pc, data, mask));

        /* Enable auto-detection between SGMII-slave and 1000BASE-X */
        if ( PHY_IS_BCM54210_220(pc) ) {
            data = PHY_BCM542XX_EXT_SERDES_LED | PHY_BCM542XX_EXT_SEL_SYNC_ST |
                   PHY_BCM542XX_EXT_SELECT_SD  | PHY_BCM542XX_EXT_SERDES_SEL;
            if ( fiber_preferred ) {
                data |= 0;  /* don't support PHY_BCM542XX_EXT_SERDES_AUTO_FX */
            }
            /* External Serdes Control Reg., DIGX_SHD_1C_14, RDB_0x234 */
            SOC_IF_ERROR_RETURN                              /* 0xd1cf */
                    (PHY_WRITE_BCM542XX_EXT_SERDES_CTRLr(unit, pc, data));

            /* SGMII Slave Control Register, DIGX_SHD_1C_15, RDB_0x235 */
            SOC_IF_ERROR_RETURN
                (PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc, 0,
                                      PHY_BCM542XX_SGMII_SLAVE_AUTO));
            /* Miscellanous Control Reg., CORE_SHD18_111, RDB_0x02f */
            SOC_IF_ERROR_RETURN
                (PHY_WRITE_BCM542XX_MII_MISC_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_MISC_CTRL_CLEARALL));
            /* Second SERDES 100-FX CONTROL Register, RDB_0xb17 */
            SOC_IF_ERROR_RETURN
                (phy_bcm542xx_direct_reg_write(unit, pc,
                            PHY_BCM542XX_2ND_SERDES_MISC_1000x_REG_OFFSET,
                            PHY_BCM542XX_2ND_SERDES_MISC_1000x_INIT));

        } else {  /* PHY is 54240/28x/29x */
            if (fiber_preferred && (ext_phy_autodetect_en == EXT_PHY_AUTODETECT_EN)) {
                /* SGMII Slave Control Register, DIGX_SHD_1C_15, RDB_0x235 */
                SOC_IF_ERROR_RETURN
                        (PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc,
                                          PHY_BCM542XX_SGMII_SLAVE_AUTO,
                                          PHY_BCM542XX_SGMII_SLAVE_AUTO));
            } else if(fiber_preferred && (ext_phy_autodetect_en == EXT_PHY_AUTODETECT_DIS)) {
                /* SGMII Slave Control Register, DIGX_SHD_1C_15, RDB_0x235 */
                SOC_IF_ERROR_RETURN
                        (PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc, 0,
                                          PHY_BCM542XX_SGMII_SLAVE_AUTO));
            } else {
                /* SGMII Slave Control Register, DIGX_SHD_1C_15, RDB_0x235 */
                SOC_IF_ERROR_RETURN
                        (PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc,
                                          PHY_BCM542XX_SGMII_SLAVE_AUTO,
                                          PHY_BCM542XX_SGMII_SLAVE_AUTO));
            }
            /* MODE_CONTROL register, DIGX_SHD_1C_1F, RDB_0x21 */
            SOC_IF_ERROR_RETURN
                (PHY_MODIFY_BCM542XX_MODE_CTRLr(unit, pc,
                                      PHY_BCM542XX_MODE_SEL_FIBER_2_SGMII,
                                      PHY_BCM542XX_MODE_CNTL_MODE_SEL_MASK));
        }
        if (fiber_preferred && (ext_phy_autodetect_en == EXT_PHY_AUTODETECT_DIS)
            && (ext_phy_fiber_iface >= EXT_PHY_FIBER_IFACE_1000X)) {

            if (ext_phy_fiber_iface == EXT_PHY_FIBER_IFACE_1000X) { /*1000x Fiber more*/
                /* Enable 1000X mode. MODE_CONTROL register,  DIGX_SHD_1C_1F, RDB_0x21 */
                SOC_IF_ERROR_RETURN
                    (PHY_MODIFY_BCM542XX_MODE_CTRLr(unit, pc, PHY_BCM542XX_MODE_CNTL_1000X_EN,
                                      PHY_BCM542XX_MODE_CNTL_1000X_EN));

                if (pc->fiber.autoneg_enable) {
                    /*
                     * Misc 1000-X Control 2 Reg., RDB.0x236, Legacy Reg.0x1C shadow 0x16
                     * bit[0] SERDES_AUTONEG_PARALLEL_DETECT_EN
                     */
                    SOC_IF_ERROR_RETURN
                        (PHY_MODIFY_BCM542XX_1ST_SERDES_CTRLr(unit, pc,
                            PHY_BCM542XX_SERDES_CTRL_AN_PD_EN,
                            PHY_BCM542XX_SERDES_CTRL_AN_PD_EN));

                    /* Default SerDes config & restart autonegotiation */
                    SOC_IF_ERROR_RETURN
                        (PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_CTRL_FD
                                    | PHY_BCM542XX_MII_CTRL_SS_1000
                                    | PHY_BCM542XX_MII_CTRL_AN_EN
                                    | pwr_down_bit
                                    | PHY_BCM542XX_MII_CTRL_RST_AN));
                 } else {
                    SOC_IF_ERROR_RETURN
                        (PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_CTRL_FD
                                    | PHY_BCM542XX_MII_CTRL_SS_1000));
                 }
            } else if (ext_phy_fiber_iface == EXT_PHY_FIBER_IFACE_100FX) {
                if (! PHY_IS_BCM54210_220(pc) ) {
                    PHY_FLAGS_SET(unit, port, PHY_FLAGS_100FX);
                    /* Enable 100BASE-FX mode*/
                    /* SERDES 100BASE-FX Control Reg., DIGX_SHD_1C_13, RDB_0x233 */
                    SOC_IF_ERROR_RETURN(
                        phy_bcm542xx_direct_reg_modify(unit, pc,
                                 PHY_BCM542XX_100FX_CTRL_REG_OFFSET,
                                 PHY_BCM542XX_100FX_CTRL_REG_FX_SERDES_EN,
                                 PHY_BCM542XX_100FX_CTRL_REG_FX_SERDES_EN) );
                }
            } else if (ext_phy_fiber_iface == EXT_PHY_FIBER_IFACE_SGMII) {
                if (! PHY_IS_BCM54210_220(pc) ) {
                    /* Enable SGMII Slave mode. SGMII Slave Control Register, DIGX_SHD_1C_15, RDB_0x235 */
                    SOC_IF_ERROR_RETURN
                        (PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc, PHY_BCM542XX_SGMII_SLAVE_EN,
                                          PHY_BCM542XX_SGMII_SLAVE_EN));
                }
            } else {
                /*
                 * Incorrect fiber interface setting provided, applying
                 * Default SerDes config & restart autonegotiation
                 */
                 SOC_IF_ERROR_RETURN
                    (PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_CTRL_FD
                                    | PHY_BCM542XX_MII_CTRL_SS_1000
                                    | PHY_BCM542XX_MII_CTRL_AN_EN
                                    | pwr_down_bit
                                    | PHY_BCM542XX_MII_CTRL_RST_AN));
            }
        } else {
            /* Default SerDes config & restart autonegotiation */
            SOC_IF_ERROR_RETURN
                (PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_CTRL_FD
                                    | PHY_BCM542XX_MII_CTRL_SS_1000
                                    | PHY_BCM542XX_MII_CTRL_AN_EN
                                    | pwr_down_bit
                                    | PHY_BCM542XX_MII_CTRL_RST_AN));
        }
        /* Configure BCM542XX to RGMII-to-Fiber mode */
        if ( IS_GMII_PORT(unit, port) && PHY_IS_BCM54210_220(pc) ) {
            _phy_bcm542xx_rgmii_to_fiber_mode(unit, port);
        }

        
    }

    /* Configure LED mode and turn on jumbo frame support */
    _phy_bcm54280_init(unit, port);

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_dev_init
 * Purpose:
 *    Initialize the PHY without reset.
 * Parameters:
 *    phy_dev_addr          - PHY Device Address
 *    automedium            - Automedium select in combo ports
 *    fiber_preferred       - Fiber preferrence over copper
 *    fiber_detect          - Fiber Signal Detect
 *    ext_phy_autodetect_en - Enable/Disable SGMII Slave Autodetect function
 *    ext_phy_fiber_iface   - Fiber interface when Autodetect is disabled.
 * Notes: 
 */
STATIC int
phy_bcm542xx_dev_init(int unit, soc_port_t port, int automedium,
                      int fiber_preferred, int fiber_detect,
                      int fiber_enable   , int copper_enable,
                      int ext_phy_autodetect_en, int ext_phy_fiber_iface)
{
    int oui = 0, model = 0, rev = 0;
    int rv = SOC_E_NONE;
    phy_ctrl_t    *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
 
    rv = _phy_bcm542xx_model_rev_get(unit, pc,&oui, &model, &rev);
    if (rv != SOC_E_NONE) {
        return SOC_E_FAIL;
    }

    switch ( model ) {

    case (PHY_BCM54192_MODEL | PHY_BCM54195_MODEL):
    case (PHY_BCM54182_MODEL | PHY_BCM54185_MODEL):
        if ( rev & 0x8 ) {  /* PHY_BCM54185_MODEL */
            rv = _phy_bcm54200_init(unit, port, automedium,
                                    fiber_preferred,
                                    fiber_detect,
                                    fiber_enable,
                                    copper_enable,
                                    ext_phy_autodetect_en,
                                    ext_phy_fiber_iface);
        } else {
            rv = _phy_bcm54282_init(unit, port);
        }
        break;
   
    case PHY_BCM54280_MODEL:
        rv = _phy_bcm54280_init(unit, port);
        break;

    case (PHY_BCM54190_MODEL | PHY_BCM54194_MODEL):    
    case (PHY_BCM54180_MODEL | PHY_BCM54140_MODEL):    
        if ( rev & 0x8 ) {  /* PHY_BCM54140_MODEL */
            rv = _phy_bcm54200_init(unit, port, automedium,
                                    fiber_preferred,
                                    fiber_detect,
                                    fiber_enable,
                                    copper_enable,
                                    ext_phy_autodetect_en,
                                    ext_phy_fiber_iface); 
        } else {
            rv = _phy_bcm54280_init(unit, port);
        }
        break;

    case (PHY_BCM54282_MODEL | PHY_BCM54285_MODEL):
    case PHY_BCM5428X_MODEL:
        if ( rev & 0x8 ) {  /* PHY_BCM54282_MODEL */
            rv = _phy_bcm54282_init(unit, port);

        } else {         /* PHY_BCM54285_MODEL */
            rv = _phy_bcm54200_init(unit, port, automedium,
                                    fiber_preferred,
                                    fiber_detect,
                                    fiber_enable,
                                    copper_enable,
                                    ext_phy_autodetect_en,
                                    ext_phy_fiber_iface); 
        }
        break;
    case PHY_BCM54210_MODEL:
    case PHY_BCM54220_MODEL:
    case PHY_BCM54240_MODEL:
        rv = _phy_bcm54200_init(unit, port, automedium,
                                    fiber_preferred,
                                    fiber_detect,
                                    fiber_enable,
                                    copper_enable,
                                    ext_phy_autodetect_en,
                                    ext_phy_fiber_iface); 

        break;
    case (PHY_BCM54292_MODEL | PHY_BCM54295_MODEL | PHY_BCM54296_MODEL):
        if ( rev & 0x8 ) {    /* PHY_BCM54295/296_MODEL */
            if ( 0x8 & _phy_bcm542xx_hidden_rev_num_get(unit, pc) ) {
                PHY_BCM542XX_REAR_HALF_SET(pc);
            }
            rv = _phy_bcm54200_init(unit, port, automedium,
                                    fiber_preferred,
                                    fiber_detect,
                                    fiber_enable,
                                    copper_enable,
                                    ext_phy_autodetect_en,
                                    ext_phy_fiber_iface); 
        } else {         /* PHY_BCM54292_MODEL */
            rv = _phy_bcm54282_init(unit, port);
        }
        break;
    case (PHY_BCM54290_MODEL | PHY_BCM54294_MODEL):
        if ( rev & 0x8 ) {  /* PHY_BCM54294_MODEL */
            PHY_BCM542XX_REAR_HALF_SET(pc);
            rv = _phy_bcm54200_init(unit, port, automedium,
                                    fiber_preferred,
                                    fiber_detect,
                                    fiber_enable,
                                    copper_enable,
                                    ext_phy_autodetect_en,
                                    ext_phy_fiber_iface); 
        } else {         /* PHY_BCM54290_MODEL */
            rv = _phy_bcm54280_init(unit, port);
        }
        break;

    default :
        return SOC_E_FAIL;
    }

    return rv;
}

#if 0    /* 54240/28x do not go to production with Rev.B0 */
/*
 * Function:
 *    phy_bcm542xx_power_seq_war
 * Purpose:   A failure to link can sometimes 
 *            occur after initial power up.  
 *            This is the workaround. 
 *    
 * Parameters:
 *    phy_id        - PHY Device Address
 * Notes: 
 */
STATIC int
phy_bcm542xx_power_seq_war(int unit, soc_port_t port)
{
    phy_ctrl_t    *pc;
    int index=0, num_ports;
    int oui=0, model=0, rev=0;
    
    pc = EXT_PHY_SW_STATE(unit, port);

    _phy_bcm542xx_model_rev_get(unit, pc,&oui, &model, &rev);
 
    /* Recommended: 
       To be done in this fixed sequence for a power sequencing
       bug. All the ports in one-go as it is not advisable to enter and exit 
       test mode for each port.
       Use pc_base->phy_id as the filter to escape this in context of other ports
       since it will be already be done during pc_base's turn. 
        - Do it after power recycle.
    */
    if ( pc->phy_id == PHY_BCM542XX_DEV_PHY_ID_BASE(pc) ) {
        /* POWER SEQUENCE FIX */
        if ( (model == PHY_BCM54240_MODEL) || (model == PHY_BCM54294_MODEL) ) {
            num_ports = 4;
        } else if ( model == PHY_BCM54210_MODEL ) {
            num_ports = 1;
        } else if ( model == PHY_BCM54220_MODEL ) {
            num_ports = 2;
        } else {
            num_ports = 8;   
        }

        for ( index = 0; index < num_ports; ++index ) {
            pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc) + index;
            
            /* enable dsp clock */
            PHY_MODIFY_BCM542XX_MII_AUX_CTRLr(unit, pc,
                                      PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK, 
                                      PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK);
            phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_LED_2_SEL_REG_OFFSET,
                                      0xB8EE);
            phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_LED_1_SEL_REG_OFFSET,
                                      0xB4EE);
            phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_DSP_TAP17_C0_REG_OFFSET,
                                      0x0009);
            phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_DSP_TAP17_C1_REG_OFFSET,
                                      0x0001);
            phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_DSP_TAP17_C1_REG_OFFSET,
                                      0x0003);
            /* Disable dsp clock */
            PHY_MODIFY_BCM542XX_MII_AUX_CTRLr(unit, pc,
                                      0, 
                                      PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK);

        }   
        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
         * To access TOP level registers use phy_id of port0. Override this ports 
         */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

#if 0 
        phy_bcm542xx_direct_reg_write(unit, pc,
                                       PHY_BCM542XX_TOP_MISC_BOOT_REG_1_OFFSET,
                                       0x8800);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                       PHY_BCM542XX_TOP_MISC_BOOT_REG_1_OFFSET,
                                       0x0000);
        
        phy_bcm542xx_direct_reg_write(unit, pc,
                                       PHY_BCM542XX_TOP_MISC_BOOT_REG_0_OFFSET,
                                       0x0DA3);

        phy_bcm542xx_direct_reg_write(unit, pc,
                                       PHY_BCM542XX_TOP_MISC_BOOT_REG_0_OFFSET,
                                       0x0D23);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                       PHY_BCM542XX_TOP_MISC_BOOT_REG_1_OFFSET,
                                       0x8000);
#else
        WRITE_PHY_REG(unit, pc,0x17, 0xD19);
        WRITE_PHY_REG(unit, pc,0x15, 0x8800);
        WRITE_PHY_REG(unit, pc,0x17, 0xD19);
        WRITE_PHY_REG(unit, pc,0x15, 0x0000);
        WRITE_PHY_REG(unit, pc,0x17, 0xD18);
        WRITE_PHY_REG(unit, pc,0x15, 0x0DA3);
        WRITE_PHY_REG(unit, pc,0x17, 0xD18);
        WRITE_PHY_REG(unit, pc,0x15, 0x0D23);
        WRITE_PHY_REG(unit, pc,0x17, 0xD19);
        WRITE_PHY_REG(unit, pc,0x15, 0x8000);
#endif

        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
         * To access TOP level registers use phy_id of port0. Now restore back 
         */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
    }
    return SOC_E_NONE;
} 
#endif /* 0 */   /* 54240/28x do not go to production with Rev.B0 */

/*
 * Function:
 *    phy_bcm542xx_reset_setup 
 * Purpose:
 *    Reset the PHY.
 * Parameters:
 *    phy_id - PHY Device Address of the PHY to reset
 */
int 
phy_bcm542xx_reset_setup(int unit, soc_port_t port, 
                         int automedium,
                         int fiber_preferred, 
                         int fiber_detect,
                         int fiber_enable,
                         int copper_enable,
                         int ext_phy_autodetect_en,
                         int ext_phy_fiber_iface)
{
    soc_port_t     primary_port;
    int            index;
    phy_ctrl_t    *pc;
    int            oui = 0, model = 0, rev = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Set primary port & offset */
    if ( soc_phy_primary_and_offset_get(unit, port, &primary_port, &index)
                                    == SOC_E_NONE) {
        if ( index & 0x80 ) {
            PHY_BCM542XX_FLAGS(pc) |=  PHY_BCM542XX_PHYADDR_REVERSE;
        } else {
            PHY_BCM542XX_FLAGS(pc) &= ~PHY_BCM542XX_PHYADDR_REVERSE;
        }

        /* Do not change the order */
        index &= ~0x80; /* clear reverse bit (PHYA_REV) */
        PHY_BCM542XX_DEV_PHY_ID_ORIG(pc) = pc->phy_id;
        PHY_BCM542XX_DEV_PHY_SLICE(pc) = index;
        PHY_BCM542XX_DEV_SET_BASE_ADDR(pc); /* phy address of port 0 */

        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_control_set(unit, port,
                                      SOC_PHY_CONTROL_PORT_PRIMARY,
                                      primary_port));
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_control_set(unit, port, 
                                      SOC_PHY_CONTROL_PORT_OFFSET, 
                                      index ));
        /* workaround for 50140/54140/5018x/5418x/5019x/5419x : restarting AFE PLL *\
        \*      after the power rails are stable, to fix the issue of AFE PLL lock */
        if (  PHY_IS_BCM5418x_19x(pc) &&
              ((PHY_QSGMII0_HEAD == index) || (PHY_QSGMII1_HEAD == index)) ) {
            int  qii = 0;
            /* reset PLL of port 0-3/4-7 in one-shot before initializing port 0/4 */
            for ( qii = 0; qii < PHY_PORTS_PER_QSGMII; qii++ ) {
                SOC_IF_ERROR_RETURN(  /* RDB_reg.0x015[1]   enable Auto-Power-Down DLL */
                        PHY_MODIFY_BCM542XX_SPARE_CTRL_3r(unit, pc,
                                                0,
                                                BCM542XX_AUTO_PWR_DOWN_DLL_DIS) );
                SOC_IF_ERROR_RETURN(  /* Reg.0x00[11]  Power down copper interface */
                        PHY_MODIFY_BCM542XX_MII_CTRLr(unit, pc,
                                                PHY_BCM542XX_MII_CTRL_PWR_DOWN,
                                                PHY_BCM542XX_MII_CTRL_PWR_DOWN) );
                SOC_IF_ERROR_RETURN(  /* Reg.0x00[11]  Power up   copper interface */
                        PHY_MODIFY_BCM542XX_MII_CTRLr(unit, pc,
                                                0,
                                                PHY_BCM542XX_MII_CTRL_PWR_DOWN) );
                SOC_IF_ERROR_RETURN(  /* RDB_reg.0x015[1]  disable Auto-Power-Down DLL */
                        PHY_MODIFY_BCM542XX_SPARE_CTRL_3r(unit, pc,
                                                BCM542XX_AUTO_PWR_DOWN_DLL_DIS,
                                                BCM542XX_AUTO_PWR_DOWN_DLL_DIS) );
                (pc->phy_id)++;
            }
            /* resume the PHY ID */
            pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
        }

    } else {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_bcm542xx_reset_setup: Config property "
                             "'phy_port_primary_and_offset' "
                             "not set for u=%d, p=%d\n"), unit, port));
        return SOC_E_FAIL;
    }

    /* PHY reset */
    SOC_IF_ERROR_RETURN( phy_bcm542xx_reset(unit, port, (void*) NULL) );
    if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_INIT_DONE) ) {
        /* Reset the Top level register block  (RDB Reg.0x800-0xAFF) *\
        \* during the 1st initialization only.                       */
        phy_bcm542xx_direct_reg_modify(unit, pc,
                              PHY_BCM542XX_TOP_MISC_GLOBAL_RESET_OFFSET,
                              PHY_BCM542XX_TOP_MISC_GLOBAL_RESET_TOP_MII_SOFT |
                              PHY_BCM542XX_TOP_MISC_GLOBAL_RESET_TIMESYNC,
                              PHY_BCM542XX_TOP_MISC_GLOBAL_RESET_TOP_MII_SOFT |
                              PHY_BCM542XX_TOP_MISC_GLOBAL_RESET_TIMESYNC);
    }

    /* get the model & revision ID of PHY chip */
    SOC_IF_ERROR_RETURN(
            _phy_bcm542xx_model_rev_get(unit, pc,&oui, &model, &rev));
    if ( PHY_IS_BCM5418x_14x(pc) ) {
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x01E7, 0xA008));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x0028, 0x0C30));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x00D8, 0x0020));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x012D, 0x0352));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x012E, 0xA04D));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x0164, 0x0500));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x0165, 0x7859));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x0125, 0x091B));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x01E8, 0x00C3));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x01E9, 0x00CC));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x01EA, 0x0300));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x01E2, 0x02E3));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x01E3, 0x7FC1));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x01E5, 0xA7AB));
        SOC_IF_ERROR_RETURN(
                phy_bcm542xx_rdb_reg_write(unit, pc, 0x0028, 0x0430));
    }

#if 0    /* 54240/28x do not go to production with Rev.B0 */
    /* WAR only for B0 rev */
    if ( PHY_BCM54240_28X_REV_BO(model, rev) ) {
        phy_bcm542xx_power_seq_war(unit, port);
    }
#endif

    /* WAR for C0 and revisions before C0 */
    if ( PHY_BCM54240_28X_REV_C0_OR_OLDER(model, rev) ) {
        /* EEE: Suggested defailt settings for these registers */
        /* ENable dsp clock */
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AUX_CTRL_REG_OFFSET,
                                      0x0C00);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_DSP_TAP33_C2_REG_OFFSET,
                                      0x8787);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_DSP_TAP34_C2_REG_OFFSET,
                                      0x017D);
        /* DISable dsp clock */
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AUX_CTRL_REG_OFFSET,
                                      0x0400);

        /* Recommended INIT default settings for registers */
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AFE_RXCONFIG_0_REG_OFFSET,
                                      0xD771);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AFE_RXCONFIG_2_REG_OFFSET,
                                      0x0072);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AFE_HPF_TRIM_REG_OFFSET,
                                      0x0006);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_HALT_AGC_CTRL_REG_OFFSET,
                                      0x0020);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AFE_AFE_PLLCTRL_4_REG_OFFSET,
                                      0x0500);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AFE_VDACCTRL_1_REG_OFFSET,
                                      0xC100);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_ANLOG_PWR_CTRL_OVRIDE_REG_OFFSET,
                                      0x1000);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_TDR_OVRIDE_VAL_REG_OFFSET,
                                      0x7C00);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_TDR_OVRIDE_VAL_REG_OFFSET,
                                      0x0000);
    }
    
    /* Recommended DSP_TAP10 settings */
    /* 54240/5428x C1 rev */
    if ( PHY_BCM54240_28X_REV_C1(model, rev) ) {
        /* 54240/5428x C1 Long Cable BER suppor, JIRA PHY-1629. *\
        \*                Errata 54240-ES101-R, 54280-ES101-R   */
        /* ENable dsp clock */
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AUX_CTRL_REG_OFFSET,
                                      0x0C00);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AFE_VDACCTRL_2_REG_OFFSET,
                                      0x0007);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_DSP_TAP33_C2_REG_OFFSET,
                                      0x87F6);
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_DSP_TAP10_REG_OFFSET,
                                      0x011B);
        /* DISable dsp clock */
        phy_bcm542xx_direct_reg_write(unit, pc,
                                      PHY_BCM542XX_AUX_CTRL_REG_OFFSET,
                                      0x0400);
    } else if (PHY_IS_BCM5418x_14x(pc)) {
           /* DISable dsp clock */
           phy_bcm542xx_direct_reg_write(unit, pc,
                                         PHY_BCM542XX_AUX_CTRL_REG_OFFSET,
                                         0x0430);
           /* Enhanced Amplitude and AB Symmetry */
           phy_bcm542xx_direct_reg_write(unit, pc,
                                         PHY_BCM542XX_DSP_TAP10_REG_OFFSET,
                                         0x091B);
    } else {
    
           phy_bcm542xx_direct_reg_write(unit, pc,
                                         PHY_BCM542XX_AUX_CTRL_REG_OFFSET,
                                         0x0C00);
           phy_bcm542xx_direct_reg_write(unit, pc,
                                         PHY_BCM542XX_DSP_TAP10_REG_OFFSET,
                                         0x011B);
    }
    /* config property PHY_OPERATIONAL_MODE == 0x80 or 0x8000 or 0x8080    *\
    \*        means to keep Super Isolate setting (depend on HW strap pin) */
    if ( 0 == (PHY_RETAIN_SUPER_ISOLATE &
               soc_property_port_get(unit, port, spn_PHY_OPERATIONAL_MODE, 0)) ) {
        /* Remove  Super Isolate */
        phy_bcm542xx_direct_reg_modify(unit, pc,
                                  PHY_BCM542XX_POWER_MII_CTRL_REG_OFFSET, 0x0,
                                  PHY_BCM542XX_POWER_MII_CTRL_SUPER_ISOLATE);
        LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                        "Remove Super Isolate: u=%d p=%d\n"), unit, port));
    }

    /* Enable current mode LED */
    phy_bcm542xx_direct_reg_write(unit, pc,
                                  PHY_BCM542XX_LED_GPIO_CTRL_STATUS_REG_OFFSET,
                                  0xBC00);

    SOC_IF_ERROR_RETURN(
            phy_bcm542xx_dev_init(unit, port, automedium, 
                                  fiber_preferred, fiber_detect,
                                  fiber_enable, copper_enable,
                                  ext_phy_autodetect_en,
                                  ext_phy_fiber_iface));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_init_setup
 * Purpose:
 *    Initialize the PHY with PHY reset.
 * Parameters:
 *    phy_dev_addr          - PHY Device Address
 *    automedium            - Automedium select in combo ports
 *    fiber_preferred       - Fiber preferrence over copper
 *    fiber_detect          - Fiber Signal Detect
 *    ext_phy_autodetect_en - Enable/Disable SGMII Slave Autodetect function
 *    ext_phy_fiber_iface   - Fiber interface when Autodetect is disabled.
 * Notes: 
 */
STATIC int 
phy_bcm542xx_init_setup( int unit,
                         soc_port_t port,
                         int reset, 
                         int automedium,
                         int fiber_preferred, 
                         int fiber_detect,
                         int fiber_enable,
                         int copper_enable,
                         int ext_phy_autodetect_en,
                         int ext_phy_fiber_iface)
{
    phy_ctrl_t    *pc;
    int            dev_port;

    pc = EXT_PHY_SW_STATE(unit, port);
   
    if ( reset ) {
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_reset_setup(unit, port, automedium, 
                                     fiber_preferred, fiber_detect,
                                     fiber_enable,
                                     copper_enable,
                                     ext_phy_autodetect_en,
                                     ext_phy_fiber_iface));
    } else {
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_dev_init(   unit, port, automedium, 
                                     fiber_preferred, fiber_detect,
                                     fiber_enable,
                                     copper_enable,
                                     ext_phy_autodetect_en,
                                     ext_phy_fiber_iface));
        /* called by _phy_bcm542xx_no_reset_setup() or _phy_bcm542xx_medium_change() *\
        \* no need to do the remaining RESET process below                           */
        return SOC_E_NONE;
    }

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }

    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    /* Reset EEE to default state i.e disable */
    
    /* Native */
    /* Disable LPI feature */
    SOC_IF_ERROR_RETURN(
            PHY_MODIFY_BCM542XX_EEE_803Dr(unit, pc, 0x0000, 0xC000) );
    /* Reset counters and other settings */   
    SOC_IF_ERROR_RETURN(
            PHY_MODIFY_BCM542XX_EEE_STAT_CTRLr(unit, pc, 0x0000, 0x3fff) );

    /* AutogrEEEn */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
    /* Disable AutogrEEEn and reset other settings */
    SOC_IF_ERROR_RETURN(
            phy_bcm542xx_direct_reg_write(unit, pc,
                            PHY_BCM542XX_TOP_MISC_MII_BUFF_CNTL_PHYN(dev_port), 
                            0x8000) );  

    /* Long cable EEE support */
    if ( PHY_IS_BCM5429X(pc) ) {
        #ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
        PHY_BCM542XX_RDB_ACCESS_MODE(unit, pc);   /* set RDB register access mode */
        #endif
        /* ENable dsp clock */
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                    PHY_BCM542XX_AUX_CTRL_REG_OFFSET, 0x0C00));
        
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                    PHY_BCM542XX_AFE_RXCONFIG_2_REG_OFFSET, 0x1872));
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                    0x1C1, 0xA5F5));
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                    0x150, 0xC70B));

        /* DISable dsp clock */
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                    PHY_BCM542XX_AUX_CTRL_REG_OFFSET, 0x0400));

        #ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
        PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc); /* legacy register access mode */
        #endif
    }

    /* restore original phy_id */ 
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

#if AUTO_MDIX_WHEN_AN_DIS
    /* Enable Auto-MDIX When autoneg disabled */
    SOC_IF_ERROR_RETURN(
        PHY_MODIFY_BCM542XX_MII_MISC_CTRLr(unit, pc,
                            PHY_BCM542XX_MII_FORCE_AUTO_MDIX_MODE, 
                            PHY_BCM542XX_MII_FORCE_AUTO_MDIX_MODE) );
#endif 

#ifdef LC_CM_DISABLE_WORKAROUND
    SOC_IF_ERROR_RETURN(      /* Disable LED current mode */
        phy_bcm542xx_direct_reg_write(unit, pc, 0x1F, 0xBC0F) );
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_bcm542xx_init
 * Purpose:
 *      Init function for 542xx PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */

STATIC int
phy_bcm542xx_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    int        fiber_capable = 0, rv;
    int        fiber_preferred;
    PHY_BCM542XX_DEV_DESC_t *pDesc;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "phy_bcm542xxx_init: u=%d p=%d\n"),
                         unit, port));

    pc = EXT_PHY_SW_STATE(unit, port);
    
    /* Only SGMII interface to switch MAC is supported */ 
    if (IS_GMII_PORT(unit, port) && !(PHY_IS_BCM54210_220(pc)) ) {
        return SOC_E_CONFIG; 
    }

    if ( IS_GMII_PORT(unit, port) ) {
        pc->interface = SOC_PORT_IF_GMII;
    } else {
        /* interface type has been determined by phy_bcm542xx_probe() */
    }
    
    if ( ! (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)) ) {
        /* if not during warmboot or reloading */
        PHY_BCM542XX_DEFAULT_REG_ACCESS_MODE(unit, pc);
    }

    /*
     * get the PHY description structure, the software table
     * recording the PHY address info and OAM config.
     */
    pDesc = (PHY_BCM542XX_DEV_DESC_t *)(pc+1);
    sal_memset(pDesc, 0, sizeof(PHY_BCM542XX_DEV_DESC_t));  /* clear the memory */

    /* all 542xx support Energy-Efficient Ethernet */
    PHY_FLAGS_SET(unit, port, PHY_FLAGS_EEE_CAPABLE);

    /*
     * Default settings for the PHY control table (phy_ctrl_t *pc)
     */

    /* fiber_capable? */
    if ( PHY_IS_BCM54140(pc) || PHY_IS_BCM54185(pc) ||
         PHY_IS_BCM54194(pc) || PHY_IS_BCM54195(pc) ||
         PHY_IS_BCM54210(pc) || PHY_IS_BCM54220(pc) ||
         PHY_IS_BCM54240(pc) || PHY_IS_BCM54285(pc) ||
         PHY_IS_BCM54294(pc) || PHY_IS_BCM54295(pc) || PHY_IS_BCM54296(pc) ) {
        fiber_capable = 1;
    }

    if ( fiber_capable == 0 ) {    /* not fiber capable */
        fiber_preferred  = 0;
        pc->automedium   = 0;
        pc->fiber_detect = 0;
        pc->ext_phy_autodetect_en = -1;
        pc->ext_phy_fiber_iface = -1;
    } else {    /* Change it to copper prefer for default. */
        fiber_preferred  = soc_property_port_get(unit, port, spn_PHY_FIBER_PREF, 0);
        pc->automedium   = soc_property_port_get(unit, port, spn_PHY_AUTOMEDIUM, 0);
        pc->fiber_detect = soc_property_port_get(unit, port, spn_PHY_FIBER_DETECT, -4);
        pc->ext_phy_autodetect_en = soc_property_port_get(unit, port, spn_EXT_PHY_AUTODETECT_EN, -1);
        pc->ext_phy_fiber_iface = soc_property_port_get(unit, port, spn_EXT_PHY_SERDES_FIBER_IFACE, -1);
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_542xx_init: u=%d p=%d type=bcm542xx%s "
                         "automedium=%d fiber_pref=%d detect=%d "
                         "ext_phy_autodetect_en=%d ext_phy_fiber_iface=%d\n"),
              unit, port, fiber_capable ? "S" : "",
              pc->automedium, fiber_preferred, pc->fiber_detect,
              pc->ext_phy_autodetect_en, pc->ext_phy_fiber_iface));

    pc->copper.enable = TRUE;
    pc->copper.preferred = ! fiber_preferred;
    pc->copper.autoneg_enable = TRUE;
    ADVERT_ALL_COPPER(&pc->copper.advert_ability);
    pc->copper.force_speed = 1000;
    pc->copper.force_duplex = TRUE;
    pc->copper.master = SOC_PORT_MS_AUTO;
    pc->copper.mdix = SOC_PORT_MDIX_AUTO;

    pc->fiber.enable = fiber_capable;
    pc->fiber.preferred = fiber_preferred;
    /* Auto-neg is unsupported in 100FX mode */
    if (fiber_preferred && (pc->ext_phy_autodetect_en == EXT_PHY_AUTODETECT_DIS) &&
        (pc->ext_phy_fiber_iface == EXT_PHY_FIBER_IFACE_100FX)) {
        pc->fiber.autoneg_enable = 0;
    } else {
        pc->fiber.autoneg_enable = fiber_capable;
    }

    ADVERT_ALL_FIBER(&pc->fiber.advert_ability);
    pc->fiber.force_speed = 1000;
    pc->fiber.force_duplex = TRUE;
    pc->fiber.master = SOC_PORT_MS_NONE;
    pc->fiber.mdix = SOC_PORT_MDIX_NORMAL;

    /* Initially configure for the preferred medium. */
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_PASSTHRU);
    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
    if ( pc->fiber.preferred ) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
    }
    
    if (fiber_preferred && (pc->ext_phy_autodetect_en == EXT_PHY_AUTODETECT_DIS)) {
        if (pc->ext_phy_fiber_iface == EXT_PHY_FIBER_IFACE_100FX) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_100FX);
            pc->fiber.force_speed = 100;
        }     
    }
    
    /* Get Requested LED selectors (defaults are hardware defaults) */
    pc->ledmode[0] = soc_property_port_get(unit, port, spn_PHY_LED1_MODE, 0);
    pc->ledmode[1] = soc_property_port_get(unit, port, spn_PHY_LED2_MODE, 1);
    pc->ledmode[2] = soc_property_port_get(unit, port, spn_PHY_LED3_MODE, 3);
    pc->ledmode[3] = soc_property_port_get(unit, port, spn_PHY_LED4_MODE, 6);
    pc->ledctrl    = soc_property_port_get(unit, port, spn_PHY_LED_CTRL , 0x8);
    pc->ledselect  = soc_property_port_get(unit, port, spn_PHY_LED_SELECT, 0);

   /* Init PHYS and MACs to defaults */
    rv = phy_bcm542xx_init_setup(unit, port, 1,
                                 pc->automedium,
                                 fiber_preferred,
                                 pc->fiber_detect,  
                                 pc->fiber.enable,
                                 pc->copper.enable, 
                                 pc->ext_phy_autodetect_en,
                                 pc->ext_phy_fiber_iface);
    if ( rv != SOC_E_NONE ) {
        return SOC_E_FAIL;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* if WarmBoot, retrieve PHY states from PHY registers */
    if ( SOC_WARM_BOOT(unit) ) {
        soc_port_medium_t  medium;
        int                andone;

        /* retrieve the medium mode (copper or fiber) */
        SOC_IF_ERROR_RETURN( _phy_bcm542xx_medium_check(unit, port, &medium) );

        if ( medium == SOC_PORT_MEDIUM_COPPER ) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_COPPER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
            /* retrieve copper staus from MII registers */
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_ability_advert_get(unit, port, &(pc->copper.advert_ability)) );
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_autoneg_get(unit, port, &(pc->copper.autoneg_enable), &andone) );
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_duplex_get(unit, port, &(pc->copper.force_duplex)) );
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_master_get(unit, port, &(pc->copper.master)) );
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_speed_get(unit, port, &(pc->copper.force_speed)) );
            if ( pc->copper.force_speed == 0 ) {
                pc->copper.force_speed = 1000;    /* default speed */
            }
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_mdix_get(unit, port, &(pc->copper.mdix)) );
        } else {      /* SOC_PORT_MEDIUM_FIBER */
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_COPPER);
            /* retrieve fiber status from MII registers */
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_ability_advert_get(unit, port, &(pc->fiber.advert_ability)) );
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_autoneg_get(unit, port, &(pc->fiber.autoneg_enable), &andone) );
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_duplex_get(unit, port, &(pc->fiber.force_duplex)) );
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_master_get(unit, port, &(pc->fiber.master)) );
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_speed_get(unit, port, &(pc->fiber.force_speed)) );
            if ( pc->fiber.force_speed == 0 ) {
                pc->fiber.force_speed = 1000;    /* default speed */
            } else if ( pc->fiber.force_speed == 100 ) {
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_100FX);
            }
        }

        if ( fiber_capable == 1 ) {
            uint16  val16;
            /* determine default medium, copper or fiber */
            SOC_IF_ERROR_RETURN(
                PHY_READ_BCM542XX_AUTO_DETECT_MEDIUMr(unit, pc, &val16) );
            pc->fiber_detect     = ( val16 & BIT(8) ) ? -4 : 0;
            pc->fiber.preferred  = ( val16 & BIT(1) ) ? TRUE : FALSE;
            pc->copper.preferred = ! pc->fiber.preferred;
        }
    } /* if (WARM_BOOT) */

    /* retreve the OAM setting from chip registers */
    phy_bcm542xx_oam_config_get(unit, port, &(pDesc->oam_config));
#endif /* BCM_WARM_BOOT_SUPPORT */

    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
        /* if WarmBoot, skip the following steps */
    }

    /* Set LED Modes and Control */
    rv = phy_bcm542xx_set_led_selectors(unit, pc);
    if (rv != SOC_E_NONE) {
        return SOC_E_FAIL;
    }

    /* Access LED related register  */
    if ( soc_property_port_get(unit, port, "phy_led3_output_disable", 0) ) {
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_direct_reg_modify(unit, pc, 0x814, 0x0001, 0x0001) );
    }

    SOC_IF_ERROR_RETURN
        (_phy_bcm542xx_medium_config_update(unit, port,
                                        PHY_COPPER_MODE(unit, port) ?
                                        &pc->copper :
                                        &pc->fiber));
    return SOC_E_NONE;
}


/*
 * Function:
 *  phy_bcm542xx_reset
 * Purpose: 
 *  Reset PHY and wait for it to come out of reset.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 * Returns: 
 *  SOC_E_XXX
 */

int
phy_bcm542xx_reset(int unit, soc_port_t port, void *user_arg)
{
    int rv;
    phy_ctrl_t*  pc = EXT_PHY_SW_STATE(unit, port);

    rv = phy_fe_ge_reset(unit, port, NULL);
    PHY_BCM542XX_DEFAULT_REG_ACCESS_MODE(unit, pc);
    return ( rv );
}

/*
 * Function:
 *      phy_bcm542xx_enable_set
 * Purpose:
 *      Enable or disable the physical interface.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_bcm542xx_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t  *pc    = EXT_PHY_SW_STATE(unit, port);
    uint16       power = (enable) ? 0 : PHY_BCM542XX_MII_CTRL_PWR_DOWN;
    /* 
     * Updated _phy_bcm54200_init to not power up the fiber and
     * copper PHY if port PHY is disabled. Here we need to power them up.
     * There is a timing issue that the fiber medium otherwise may not
     * be detected when both mediums are present at the boot time.
     */
    if ( PHY_COPPER_MODE(unit, port) || pc->automedium ) {
        SOC_IF_ERROR_RETURN
            (PHY_MODIFY_BCM542XX_MII_CTRLr(unit, pc, power,
                                           PHY_BCM542XX_MII_CTRL_PWR_DOWN));
    }
    if ( PHY_FIBER_MODE(unit, port)  || pc->automedium ) {
        SOC_IF_ERROR_RETURN
            (PHY_MODIFY_BCM542XX_1000X_MII_CTRLr(unit, pc, power,
                                           PHY_BCM542XX_MII_CTRL_PWR_DOWN));
    }

    if ( pc->automedium || PHY_FIBER_MODE(unit, port) ) {
        phy_ctrl_t* int_pc = INT_PHY_SW_STATE(unit, port);

        if ( NULL != int_pc ) {
            SOC_IF_ERROR_RETURN
                (PHY_ENABLE_SET(int_pc->pd, unit, port, enable));
        }
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit, "phy_bcm542xx_enable_set: "
                                   "Power %s fiber medium, u=%d p=%d\n"),
                  (enable) ? "up" : "down", unit, port));
    }

    /* Update software state */
    SOC_IF_ERROR_RETURN(phy_fe_ge_enable_set(unit, port, enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_bcm542xx_enable_get
 * Purpose:
 *      Enable or disable the physical interface for a 542xx device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - (OUT) Boolean, true = enable PHY, false = disable.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_bcm542xx_enable_get(int unit, soc_port_t port, int *enable)
{
    return phy_fe_ge_enable_get(unit, port, enable);
}

/*
 * Function:
 *      phy_bcm542xx_link_get
 * Purpose:
 *      Determine the current link up/down status for a 542xx device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      link - (OUT) Boolean, true indicates link established.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If using automedium, also switches the mode.
 */
STATIC int
phy_bcm542xx_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t    *pc;
    uint16         powerdown;
    uint16  mii_stat, temp16;
    pc    = EXT_PHY_SW_STATE(unit, port);
    *link = FALSE;      /* Default return */

    if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE) ) {
        return SOC_E_NONE;
    }

    if (!pc->fiber.enable && !pc->copper.enable) {
        return SOC_E_NONE;
    }
    
    if ( PHY_IS_BCM54240(pc) || PHY_IS_BCM54285(pc) ) {
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_direct_reg_read(unit, pc,
                          PHY_BCM542XX_OPER_MODE_STATUS_REG_OFFSET, &temp16));
        if ( (!(temp16 & BIT(5))) && ((temp16 & 0x1f) == 0xb) ) {
            /* no sync and in SGMII slave */
            /* Disable auto-detection between SGMII-slave and 1000BASE-X
               and set mode to 1000BASE-X */
            SOC_IF_ERROR_RETURN
                   (PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc, 0, 0x3));
            /* Now enable auto-detection between SGMII-slave and 1000BASE-X */
            SOC_IF_ERROR_RETURN
                    (PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc, 1, 1));
        }
    }

    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_MEDIUM_CHANGE);
    if ( pc->automedium) {
        /* Check for medium change and update HW/SW accordingly. */
        SOC_IF_ERROR_RETURN
            (_phy_bcm542xx_medium_change(unit, port, SOC_PORT_MEDIUM_NONE));
    }

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(phy_fe_ge_link_get(unit, port, link));
    } else if ( PHY_FIBER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_1000X_MII_STATr(unit, pc,&mii_stat));
        *link = (mii_stat & PHY_BCM542XX_MII_STAT_LA) ? TRUE : FALSE;
    } else {
        return  SOC_E_INTERNAL;
    }

    /* If preferred medium is up, disable the other medium to prevent false link. */
    if ( pc->automedium ) {
        if ( pc->copper.preferred ) {
            if ( PHY_IS_BCM54210_220(pc) ) {
                /* don't power down fiber at 54220/210, it impacts traffic Tx/Rx */
                powerdown = 0;
            } else if ( pc->fiber.enable ) {
                /* Power down Serdes if Copper mode is up */
                powerdown = (*link && PHY_COPPER_MODE(unit, port))
                             ? PHY_BCM542XX_MII_CTRL_PWR_DOWN : 0;
            } else {
                powerdown = PHY_BCM542XX_MII_CTRL_PWR_DOWN;

            }
            SOC_IF_ERROR_RETURN
                (PHY_MODIFY_BCM542XX_1000X_MII_CTRLr(unit, pc, powerdown, 
                                               PHY_BCM542XX_MII_CTRL_PWR_DOWN));
        } else {  /* pc->fiber.preferred */
            if ( pc->copper.enable ) {
                powerdown = (*link && PHY_FIBER_MODE(unit, port))
                             ? PHY_BCM542XX_MII_CTRL_PWR_DOWN : 0;
            } else {
                powerdown = PHY_BCM542XX_MII_CTRL_PWR_DOWN;
            }
            SOC_IF_ERROR_RETURN
                (PHY_MODIFY_BCM542XX_MII_CTRLr(unit, pc, powerdown,
                                               PHY_BCM542XX_MII_CTRL_PWR_DOWN));
        }
    } /* if ( automedium ) */

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit, "phy_bcm542xx_link_get: "
                                  "u=%d p=%d mode=%s%s link=%d\n"),
                 unit, port,
                 PHY_COPPER_MODE(unit, port) ? "C" : "F",
                 PHY_FIBER_100FX_MODE(unit, port)? "(100FX)" : "",
                 *link));

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_bcm542xx_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - Half duplex requested, and not supported.
 * Notes:
 *      The duplex is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 *      Autonegotiation is not manipulated.
 */

STATIC int
phy_bcm542xx_duplex_set(int unit, soc_port_t port, int duplex)
{
    phy_ctrl_t  *pc;
    uint16       data, is_duplex;

    pc = EXT_PHY_SW_STATE(unit, port);
    is_duplex = (duplex == SOC_PORT_DUPLEX_FULL) ? TRUE : FALSE;
    data = (is_duplex) ? PHY_BCM542XX_MII_CTRL_FD : 0;

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_MODIFY_BCM542XX_MII_CTRLr(unit, pc, data,
                                    PHY_BCM542XX_MII_CTRL_FD) );
        pc->copper.force_duplex = duplex;

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_MODIFY_BCM542XX_1000X_MII_CTRLr(unit, pc, data,
                                    PHY_BCM542XX_MII_CTRL_FD) );

        if ( PHY_FIBER_100FX_MODE(unit, port) ) {
            data = (is_duplex) ? PHY_BCM542XX_100FX_CTRL_REG_FD_FX_SERDES : 0;
            SOC_IF_ERROR_RETURN(
                phy_bcm542xx_direct_reg_modify(unit, pc,
                                    PHY_BCM542XX_100FX_CTRL_REG_OFFSET, data,
                                    PHY_BCM542XX_100FX_CTRL_REG_FD_FX_SERDES) );
            if ( PHY_IS_BCM54210_220(pc) ) {
                data = (is_duplex) ? PHY_BCM542XX_EXT_SERDES_FX_FD : 0;
                /* External Serdes Control Reg., DIGX_SHD_1C_14, RDB_0x234 */
                SOC_IF_ERROR_RETURN(
                    PHY_MODIFY_BCM542XX_EXT_SERDES_CTRLr(unit, pc, data,
                                    PHY_BCM542XX_EXT_SERDES_FX_FD) );
            }
            pc->fiber.force_duplex = duplex;
        } else {    /* 1000BASE-X */
            if ( ! is_duplex ) {    /* 1000X must be full-duplex */
                return SOC_E_UNAVAIL;
            }
        }

    } else {
        return SOC_E_INTERNAL;
    }
    LOG_INFO(BSL_LS_SOC_PHY,
          (BSL_META_U(unit, "phy_bcm542xx_duplex_set: u=%d p=%d d=%d\n"),
          unit, port, duplex));
    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx_duplex_get
 * Purpose:
 *      Get the current operating duplex mode. If autoneg is enabled,
 *      then operating mode is returned, otherwise forced mode is returned.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The duplex is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. Autonegotiation is
 *      not manipulated.
 */

STATIC int
phy_bcm542xx_duplex_get(int unit, soc_port_t port, int *duplex)
{
    int          rv = SOC_E_NONE;
    phy_ctrl_t  *pc;
    uint16       opt_mode;
    uint16       mii_ctrl, mii_stat;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_FIBER_MODE(unit, port) ) {
        *duplex = TRUE;
    }

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_CTRLr(unit, pc,&mii_ctrl));
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_STATr(unit, pc,&mii_stat));

        if ( mii_ctrl & PHY_BCM542XX_MII_CTRL_AN_EN ) {        /* Auto-negotiation enabled */
            if ( ! (mii_stat & PHY_BCM542XX_MII_STAT_AN_DONE) ) { /* Auto-neg NOT complete */
                *duplex = FALSE;
            } else {
                uint16  misc_ctrl;
                SOC_IF_ERROR_RETURN(
                    PHY_READ_BCM542XX_MII_MISC_CTRLr(unit, pc,&misc_ctrl));
                /* First check for Ethernet@Wirespeed */
                if ( misc_ctrl & BIT(4) ) {   /* Ethernet@Wirespeed enabled */
                    rv = _phy_bcm542xx_auto_negotiate_ew(unit, pc, NULL, duplex);
                } else {
                    rv = _phy_bcm542xx_auto_negotiate_gcd(unit, port, pc, NULL, duplex);
                }
            }
        } else {                           /* Auto-negotiation disabled */
            *duplex = (mii_ctrl & PHY_BCM542XX_MII_CTRL_FD) ? TRUE : FALSE;
        }

    } else if ( PHY_FIBER_MODE(unit, port) ) {    /* Fiber mode */
        if ( PHY_FIBER_100FX_MODE(unit, port) ) {
            SOC_IF_ERROR_RETURN
                (PHY_READ_BCM542XX_EXP_OPT_MODE_STATr(unit, pc,&opt_mode));
            if ( opt_mode & PHY_BCM542XX_OPER_MODE_STATUS_SERDES_DUPLEX ) {
                *duplex = TRUE;
            } else {
                *duplex = FALSE;
            }
        } else { /* 1000X */
            *duplex = TRUE;
        }

    } else {
        return SOC_E_INTERNAL;
    }
    return rv;
}

/*
 * Function:
 *      phy_bcm542xx_speed_set
 * Purpose:
 *      Set the current operating speed (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Requested speed, only 1000 supported.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The speed is set only for the ACTIVE medium.
 */

STATIC int
phy_bcm542xx_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    uint16       mii_ctrl = 0;
    uint16  serdes_100fx_ctrl = 0x0;
    uint16  serdes_100fx_ctrl_mask = PHY_BCM542XX_100FX_CTRL_REG_RX_QUALIFY_DIS;
#ifdef INCLUDE_PLP_IMACSEC
    int speed_m;
    unsigned int  port_m;
    bcm_plp_base_t_sec_phy_access_t *imacsec_des;
    uint16 imacsec_cfg;
    soc_port_t port_s;
    unsigned int speed_mask=0x3;
#endif
    pc = EXT_PHY_SW_STATE(unit, port);
#ifdef INCLUDE_PLP_IMACSEC
    imacsec_des = IMACSEC_DES_PTR_GET(pc);
#endif

#ifdef INCLUDE_PLP_IMACSEC
    if(pc->macsec_enable){
        port_m = get_port(&imacsec_des->phy_info);
        SPEED_MAPPING(speed,speed_m);
        /* Use base phy id to write top register */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
        /* SW set speed MACSEC_CONFIG2 reg */
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_cl45_reg_read(unit, pc,  0x7, 0x9870, &imacsec_cfg));
        imacsec_cfg |= 1<<port_m;
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_cl45_reg_write(unit, pc,  0x7, 0x9870, imacsec_cfg));
        /* SW set speed MACSEC_CONFIG3 reg */
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_cl45_reg_read(unit, pc,  0x7, 0x9871, &imacsec_cfg));
        imacsec_cfg &= ~(speed_mask << (port_m*2));
        imacsec_cfg |= (speed_m << (port_m*2));
        SOC_IF_ERROR_RETURN
          (phy_bcm542xx_cl45_reg_write(unit, pc,  0x7, 0x9871, imacsec_cfg));
        /* Restore the phy mdio address */
        pc->phy_id =  PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

        port_s= PHY_UNIMAC_V2_SWITCH_PORT(port);
        SOC_IF_ERROR_RETURN(
           BMACSEC_MAC_AUTO_CONFIG_SET((phy_mac_ctrl_t *)imacsec_des->unimac, port_s, 0));

        SOC_IF_ERROR_RETURN(
        BMACSEC_MAC_AUTO_CONFIG_SET((phy_mac_ctrl_t *)imacsec_des->unimac, port_s, 1));
     }
#endif
    /* get current MII control Register */
    if ( PHY_COPPER_MODE(unit, port) ) {  /* copper mode */
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_CTRLr(unit, pc,&mii_ctrl) );
    } else {                              /* fiber mode */
        if ( speed == 10 ) {
            return SOC_E_UNAVAIL;   /* nehter 1000Base-X nor 100Base-FX */
        }
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_1000X_MII_CTRLr(unit, pc,&mii_ctrl) );
    }
    /* mask off the Speed Selection bit[6,13] */
    mii_ctrl &= ~(PHY_BCM542XX_MII_CTRL_SS_LSB | PHY_BCM542XX_MII_CTRL_SS_MSB);

    switch ( speed ) {
    case  10:
        mii_ctrl |= PHY_BCM542XX_MII_CTRL_SS_10;
        break;
    case  100:
        mii_ctrl |= PHY_BCM542XX_MII_CTRL_SS_100;
        serdes_100fx_ctrl      = (PHY_IS_BCM54210_220(pc)) ?  0  :
                                  PHY_BCM542XX_100FX_CTRL_REG_FX_SERDES_EN;
        serdes_100fx_ctrl_mask |= PHY_BCM542XX_100FX_CTRL_REG_FX_SERDES_EN |
                                  PHY_BCM542XX_100FX_CTRL_REG_FD_FX_SERDES;
        if ( pc->fiber.force_duplex)  {
            serdes_100fx_ctrl  |= PHY_BCM542XX_100FX_CTRL_REG_FD_FX_SERDES;
        }
        break;
    case  1000:
        mii_ctrl |= PHY_BCM542XX_MII_CTRL_SS_1000;
        serdes_100fx_ctrl_mask |= PHY_BCM542XX_100FX_CTRL_REG_FX_SERDES_EN;
        break;
    default:
        return SOC_E_CONFIG;
    }

    if ( PHY_COPPER_MODE(unit, port) ) {  /* COPPER mode */
        SOC_IF_ERROR_RETURN(
            PHY_WRITE_BCM542XX_MII_CTRLr(unit, pc, mii_ctrl) );
        pc->copper.force_speed = speed;

    } else {                              /* FIBER  mode */
        if ( speed == 100 ) {
            /* Disable autoneg for 100Base-FX speed */ 
            pc->fiber.autoneg_enable = FALSE;
            if ( PHY_IS_BCM54140(pc) || PHY_IS_BCM54194(pc) ||
                 PHY_IS_BCM54185(pc) || PHY_IS_BCM54195(pc)  ) {
                /* turn on RX_QUALIFY_DIS for 28nm SerDes AFE, RDB.0x233[5] */
                serdes_100fx_ctrl |= PHY_BCM542XX_100FX_CTRL_REG_RX_QUALIFY_DIS;
            }
        }
        /* Select 1000x/100fx and duplexity in case of 100fx */ 
        /* SERDES 100BASE-FX Control Reg., DIGX_SHD_1C_13, RDB_0x233 */
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_direct_reg_modify(unit, pc,
                                 PHY_BCM542XX_100FX_CTRL_REG_OFFSET,
                                 serdes_100fx_ctrl, serdes_100fx_ctrl_mask) );

        if ( pc->fiber.autoneg_enable ) {
            mii_ctrl |= MII_CTRL_AE | MII_CTRL_RAN | MII_CTRL_FD;
        } else {
            mii_ctrl |= MII_CTRL_FD;
        }

        if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE) ) {
            mii_ctrl |= PHY_BCM542XX_MII_CTRL_PWR_DOWN; 
        }
        SOC_IF_ERROR_RETURN(
            PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc, mii_ctrl) );
      
        if ( speed == 100 ) {
            if ( PHY_IS_BCM54210_220(pc) ) {
                /* External Serdes Control Reg., DIGX_SHD_1C_14, RDB_0x234 */
                SOC_IF_ERROR_RETURN(
                    PHY_MODIFY_BCM542XX_EXT_SERDES_CTRLr(unit, pc,
                                              PHY_BCM542XX_EXT_SERDES_FX,
                                              PHY_BCM542XX_EXT_SERDES_FX_MASK) );
            }
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_100FX);
        }
        /* Enforce duplexity to full for 1000X */
        if ( speed == 1000 ) {
            if ( PHY_IS_BCM54210_220(pc) ) {
                /* 54220_210_B0 don't support PHY_BCM542XX_EXT_SERDES_AUTO_FX, */
                /* External Serdes Control Reg., DIGX_SHD_1C_14, RDB_0x234 */
                SOC_IF_ERROR_RETURN(
                    PHY_MODIFY_BCM542XX_EXT_SERDES_CTRLr(unit, pc, 0,
                                              PHY_BCM542XX_EXT_SERDES_FX_MASK) );
            }
            pc->fiber.force_duplex = TRUE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_100FX);
        }

        pc->fiber.force_speed = speed;
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_bcm542xx_speed_set: u=%d p=%d s=%d fiber=%d\n"),
              unit, port, speed, PHY_FIBER_MODE(unit, port)));
    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx_speed_get
 * Purpose:
 *      Get the current operating speed for a bcm542xx device.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Boolean, true indicates full duplex, false
 *              indicates half.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The speed is retrieved for the ACTIVE medium.
 */

STATIC int
phy_bcm542xx_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t  *pc;
    uint16       mii_ctrl, mii_aux_stat, temp16;
#ifdef INCLUDE_PLP_IMACSEC  
    int speed_m;
    bcm_plp_base_t_sec_phy_access_t *imacsec_des;
#endif
    pc = EXT_PHY_SW_STATE(unit, port);
#ifdef INCLUDE_PLP_IMACSEC
    imacsec_des = IMACSEC_DES_PTR_GET(pc);
#endif
    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_CTRLr(unit, pc,&mii_ctrl));

        if ( mii_ctrl & PHY_BCM542XX_MII_CTRL_AN_EN ) {  /* Auto-negotiation enabled */
        /* Check the autoneg status if it is done or not */
            SOC_IF_ERROR_RETURN(
                PHY_READ_BCM542XX_MII_AUX_STATUSr(unit, pc,&mii_aux_stat));

            if ( ! (mii_aux_stat & PHY_BCM542XX_MII_ASSR_ANC) ) {
                /* Auto-neg NOT complete return force speed*/
                /* Pick up the values we force in CTRL register */
                switch(PHY_BCM542XX_MII_CTRL_SS(mii_ctrl) ) {
                case PHY_BCM542XX_MII_CTRL_SS_10:
                    *speed = 10;
                    break;
                case PHY_BCM542XX_MII_CTRL_SS_100:
                    *speed = 100;
                    break;
                case PHY_BCM542XX_MII_CTRL_SS_1000:
                    *speed = 1000;
                    break;
                default:   /* Pass error back */
                    return SOC_E_UNAVAIL;
                }
            } else {
                switch ( mii_aux_stat & PHY_BCM542XX_MII_ASSR_AUTONEG_HCD_MASK ) {
                case PHY_BCM542XX_MII_ASSR_HCD_FD_1000:
                case PHY_BCM542XX_MII_ASSR_HCD_HD_1000:
                    *speed = 1000;
                break;
                case PHY_BCM542XX_MII_ASSR_HCD_FD_100:
                case PHY_BCM542XX_MII_ASSR_HCD_HD_100:
                case PHY_BCM542XX_MII_ASSR_HCD_T4_100:
                    *speed = 100;
                    break;
                case PHY_BCM542XX_MII_ASSR_HCD_FD_10:
                case PHY_BCM542XX_MII_ASSR_HCD_HD_10:
                    *speed = 10;
                    break;
                default:
                    *speed = 0;
                }
            }
        } else {    /* Auto-negotiation disabled */
            /* Pick up the values we force in CTRL register */
            switch(PHY_BCM542XX_MII_CTRL_SS(mii_ctrl) ) {
            case PHY_BCM542XX_MII_CTRL_SS_10:
                *speed = 10;
                break;
            case PHY_BCM542XX_MII_CTRL_SS_100:
                *speed = 100;
                break;
            case PHY_BCM542XX_MII_CTRL_SS_1000:
                *speed = 1000;
                break;
            default:   /* Pass error back */
                return SOC_E_UNAVAIL;
            }
        }

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        if (pc->ext_phy_autodetect_en == EXT_PHY_AUTODETECT_DIS && 
            pc->ext_phy_fiber_iface == EXT_PHY_FIBER_IFACE_SGMII) {
            SOC_IF_ERROR_RETURN(
                PHY_READ_BCM542XX_SGMII_SLAVEr(unit, pc, &temp16));
            switch ( temp16 & PHY_BCM542XX_SGMII_SPEED )  {
            case PHY_BCM542XX_SGMII_SPEED_10:
                *speed = 10;
                break;
            case PHY_BCM542XX_SGMII_SPEED_100:
                *speed = 100;
                break;
            case PHY_BCM542XX_SGMII_SPEED_1000:
                *speed = 1000;
                break;
            default:
                return SOC_E_UNAVAIL;
            }
        } else {
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_OPER_MODE_STATUS_REG_OFFSET, &temp16);
            switch ( temp16 & PHY_BCM542XX_OPER_MODE_STATUS_REG_SERDES_SPEED )  {
            case PHY_BCM542XX_OPER_MODE_STATUS_SERDES_SPEED_10:
                *speed = 10;
                break;
            case PHY_BCM542XX_OPER_MODE_STATUS_SERDES_SPEED_100:
                *speed = 100;
                break;
            case PHY_BCM542XX_OPER_MODE_STATUS_SERDES_SPEED_1000:
                *speed = 1000;
                break;
            default:
                return SOC_E_UNAVAIL;
            }
        }
    } else {
        return SOC_E_INTERNAL;
    }
#ifdef INCLUDE_PLP_IMACSEC
    if(pc->macsec_enable){  
        SOC_IF_ERROR_RETURN(
          BMACSEC_MAC_SPEED_GET((phy_mac_ctrl_t *)imacsec_des->unimac, port, &speed_m));
        if(*speed !=0 && *speed == speed_m){
            LOG_INFO(BSL_LS_SOC_PHY,
               (BSL_META_U(unit, "phy_bcm542xx_speed_get: u=%d p=%d sp=%d\n"),
                 unit, port, speed_m));
   
           return ( SOC_E_NONE );
        }
    }
#endif
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "phy_bcm542xx_speed_get: u=%d p=%d sp=%d\n"),
              unit, port, *speed));
   
    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx_master_set
 * Purpose:
 *      Set the current master mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - SOC_PORT_MS_*
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The master mode is set only for the ACTIVE medium.
 */
STATIC int
phy_bcm542xx_master_set(int unit, soc_port_t port, int master)
{
    phy_ctrl_t  *pc;
    uint16       mii_gb_ctrl;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_GB_CTRLr(unit, pc,&mii_gb_ctrl));
        switch (master) {
        case SOC_PORT_MS_SLAVE:
            mii_gb_ctrl |= PHY_BCM542XX_MII_GB_CTRL_MS_MAN;
            mii_gb_ctrl &= ~PHY_BCM542XX_MII_GB_CTRL_MS;
            break;
        case SOC_PORT_MS_MASTER:
            mii_gb_ctrl |= PHY_BCM542XX_MII_GB_CTRL_MS_MAN;
            mii_gb_ctrl |= PHY_BCM542XX_MII_GB_CTRL_MS;
            break;
        case SOC_PORT_MS_AUTO:
            mii_gb_ctrl &= ~PHY_BCM542XX_MII_GB_CTRL_MS_MAN;
            break;
        case SOC_PORT_MS_NONE:
            break;
        default:
            return SOC_E_CONFIG;
        }

        SOC_IF_ERROR_RETURN(
            PHY_WRITE_BCM542XX_MII_GB_CTRLr(unit, pc, mii_gb_ctrl) );
        pc->copper.master = master;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "phy_bcm542xx_master_set: "
                               "u=%d p=%d master=%d fiber=%d\n"),
              unit, port, master, PHY_FIBER_MODE(unit, port)));
    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx_master_get
 * Purpose:
 *      Get the current master mode for a bcm542xx device.
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
phy_bcm542xx_master_get(int unit, soc_port_t port, int *master)
{
    phy_ctrl_t *pc;
    uint16      mii_gb_ctrl;
    
    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_GB_CTRLr(unit, pc,&mii_gb_ctrl) );

        if ( ! (mii_gb_ctrl & PHY_BCM542XX_MII_GB_CTRL_MS_MAN) ) {
            *master = SOC_PORT_MS_AUTO;
        } else if ( mii_gb_ctrl & PHY_BCM542XX_MII_GB_CTRL_MS ) {
            *master = SOC_PORT_MS_MASTER;
        } else {
            *master = SOC_PORT_MS_SLAVE;
        }

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        *master = SOC_PORT_MS_NONE;

    } else {
        return SOC_E_INTERNAL;
    }

    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx_autoneg_set
 * Purpose:
 *      Enable or disable auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - Boolean, if true, auto-negotiation is enabled
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is set only for the ACTIVE medium.
 */

STATIC int
phy_bcm542xx_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    phy_ctrl_t  *pc;
    uint16       mii_ctrl;
    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_CTRLr(unit, pc,&mii_ctrl));

        if (autoneg) {
            mii_ctrl |= PHY_BCM542XX_MII_CTRL_AN_EN | PHY_BCM542XX_MII_CTRL_RST_AN;
        } else {
            mii_ctrl &= ~(PHY_BCM542XX_MII_CTRL_AN_EN | PHY_BCM542XX_MII_CTRL_RST_AN);
        }
        SOC_IF_ERROR_RETURN(
                PHY_WRITE_BCM542XX_MII_CTRLr(unit, pc, mii_ctrl));
        
        pc->copper.autoneg_enable = autoneg ? TRUE : FALSE;

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        if (PHY_FIBER_100FX_MODE(unit, port)) {
            /* 100FX mode doesn't support autoneg*/
            autoneg = 0;
        }
        if ( autoneg ) {
           /* When enabling autoneg, set the default speed to 1000Mbps first.
             * PHY will not enable autoneg if the PHY is in 100FX mode.
             */
            SOC_IF_ERROR_RETURN
               (phy_bcm542xx_speed_set(unit, port, 1000));
        }

        /* Disable/Enable auto-detection between SGMII-slave and 1000BASE-X */
        /* don't change SGMII_SLAVE_AUTO_DET at 54220/210, it impacts traffic Tx/Rx */
        if ( ! PHY_IS_BCM54210_220(pc) ) {
            if (pc->fiber.preferred && pc->ext_phy_autodetect_en == EXT_PHY_AUTODETECT_DIS) {
                /* SGMII Slave Register, DIGX_SHD1C_15 bit[1] SGMII_SLAVE_AUTO_DET*/
                SOC_IF_ERROR_RETURN(
                    PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc, 0,
                    PHY_BCM542XX_SGMII_SLAVE_AUTO) );
            } else {
                /* SGMII Slave Register, DIGX_SHD1C_15 bit[0] SGMII_SLAVE_AUTO_DET*/
                SOC_IF_ERROR_RETURN(
                    PHY_MODIFY_BCM542XX_SGMII_SLAVEr(unit, pc,(autoneg ? 1 : 0), 1) );
            }
        }

        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_ability_advert_set(unit, port, &pc->fiber.advert_ability) );

        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_1000X_MII_CTRLr(unit, pc,&mii_ctrl) );

        if ( autoneg ) {
            mii_ctrl |=   PHY_BCM542XX_MII_CTRL_AN_EN | PHY_BCM542XX_MII_CTRL_RST_AN;
        } else {
            mii_ctrl &= ~(PHY_BCM542XX_MII_CTRL_AN_EN | PHY_BCM542XX_MII_CTRL_RST_AN);
        }
        SOC_IF_ERROR_RETURN(
                PHY_WRITE_BCM542XX_1000X_MII_CTRLr(unit, pc, mii_ctrl) );

        pc->fiber.autoneg_enable = autoneg ? TRUE : FALSE;

    } else {
        return SOC_E_INTERNAL;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_bcm542xx_autoneg_set: u=%d p=%d autoneg=%d\n"),
              unit, port, autoneg));
    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx_autoneg_get
 * Purpose:
 *      Get the current auto-negotiation status (enabled/busy).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      autoneg - (OUT) if true, auto-negotiation is enabled.
 *      autoneg_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if autoneg == FALSE.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The autoneg mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_bcm542xx_autoneg_get(int unit, soc_port_t port,
                         int *autoneg, int *autoneg_done)
{
    phy_ctrl_t   *pc;
    uint16 mii_ctrl, mii_stat;

    pc = EXT_PHY_SW_STATE(unit, port);

    *autoneg      = FALSE;
    *autoneg_done = FALSE;

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_CTRLr(unit, pc,&mii_ctrl) );
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_STATr(unit, pc,&mii_stat) );

        *autoneg      = (mii_ctrl & PHY_BCM542XX_MII_CTRL_AN_EN)   ? 1 : 0;
        *autoneg_done = (mii_stat & PHY_BCM542XX_MII_STAT_AN_DONE) ? 1 : 0;

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        /* autoneg is not enabled in 100FX mode */
        if ( PHY_FIBER_100FX_MODE(unit, port) ) {
            *autoneg      = FALSE;
            *autoneg_done = TRUE;
        } else {
            SOC_IF_ERROR_RETURN(
                PHY_READ_BCM542XX_1000X_MII_CTRLr(unit, pc,&mii_ctrl) );
            SOC_IF_ERROR_RETURN(
                PHY_READ_BCM542XX_1000X_MII_STATr(unit, pc,&mii_stat) );

            *autoneg      = (mii_ctrl & PHY_BCM542XX_MII_CTRL_AN_EN)   ? 1 : 0;
            *autoneg_done = (mii_stat & PHY_BCM542XX_MII_STAT_AN_DONE) ? 1 : 0;
        }

    } else {
        return SOC_E_INTERNAL;
    }

    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx_ability_advert_set
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
phy_bcm542xx_ability_advert_set(int unit, soc_port_t port,
                                soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16       mii_adv, mii_gb_ctrl;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        mii_adv = PHY_BCM542XX_MII_ANA_ASF_802_3;
        SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_MII_GB_CTRLr(unit, pc,&mii_gb_ctrl));

        mii_gb_ctrl &= ~(PHY_BCM542XX_MII_GB_CTRL_ADV_1000HD |
                         PHY_BCM542XX_MII_GB_CTRL_ADV_1000FD);
        mii_gb_ctrl |= PHY_BCM542XX_MII_GB_CTRL_PT; /* repeater / switch port */

        if ( (ability->speed_half_duplex) & SOC_PA_SPEED_10MB ) {
            mii_adv |= PHY_BCM542XX_MII_ANA_HD_10;
        }
        if ( (ability->speed_full_duplex) & SOC_PA_SPEED_10MB ) {
            mii_adv |= PHY_BCM542XX_MII_ANA_FD_10;
        }
        if ( (ability->speed_half_duplex) & SOC_PA_SPEED_100MB ) {
            mii_adv |= PHY_BCM542XX_MII_ANA_HD_100;
        }
        if ( (ability->speed_full_duplex) & SOC_PA_SPEED_100MB ) {
            mii_adv |= PHY_BCM542XX_MII_ANA_FD_100;
        }
        if ( (ability->speed_half_duplex) & SOC_PA_SPEED_1000MB ) {
            mii_gb_ctrl |= PHY_BCM542XX_MII_GB_CTRL_ADV_1000HD;
        }
        if ( (ability->speed_full_duplex) & SOC_PA_SPEED_1000MB ) {
            mii_gb_ctrl |= PHY_BCM542XX_MII_GB_CTRL_ADV_1000FD;
        }

        if ( ((ability->pause) & SOC_PA_PAUSE) == SOC_PA_PAUSE ) {
            /* Advertise symmetric pause */
            mii_adv |= PHY_BCM542XX_MII_ANA_PAUSE;
        } else {
            if ( (ability->pause) & SOC_PA_PAUSE_TX ) {
                mii_adv |= PHY_BCM542XX_MII_ANA_ASYM_PAUSE;
            } else { 
                if ( (ability->pause) & SOC_PA_PAUSE_RX ) {
                    mii_adv |= PHY_BCM542XX_MII_ANA_ASYM_PAUSE;
                    mii_adv |= PHY_BCM542XX_MII_ANA_PAUSE;
                }
            }
        }

        SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MII_ANAr(unit, pc, mii_adv));
        SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_MII_GB_CTRLr(unit, pc, mii_gb_ctrl));

        /* EEE advertisement settings */
        if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            uint16 eee_ability = 0;
            if (ability->eee & SOC_PA_EEE_10GB_BASET) {
                eee_ability |= PHY_BCM542XX_EEE_ADV_10G;
            }
            if (ability->eee & SOC_PA_EEE_1GB_BASET) {
                eee_ability |= PHY_BCM542XX_EEE_ADV_1000;
            }
            if (ability->eee & SOC_PA_EEE_100MB_BASETX) {
                eee_ability |= PHY_BCM542XX_EEE_ADV_100;
            }
            SOC_IF_ERROR_RETURN
                (PHY_MODIFY_BCM542XX_EEE_ADVr(unit, pc, eee_ability,
                                              PHY_BCM542XX_EEE_ADV_MASK));
        }

        /* update the PHY property software table (phy_ctrl_t) */
        sal_memcpy( &(pc->copper.advert_ability), ability,
                    sizeof(soc_port_ability_t) );

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        mii_adv = PHY_BCM542XX_MII_ANA_C37_FD;  /* Always advertise 1000X full duplex */
        switch ( ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX) ) {
        case SOC_PA_PAUSE_TX:
            mii_adv |= PHY_BCM542XX_MII_ANA_C37_ASYM_PAUSE;
            break;
        case SOC_PA_PAUSE_RX:
            mii_adv |= (PHY_BCM542XX_MII_ANA_C37_ASYM_PAUSE |
                        PHY_BCM542XX_MII_ANA_C37_PAUSE);
            break;
        case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
            mii_adv |= PHY_BCM542XX_MII_ANA_C37_PAUSE;
            break;
        }
        /* Write to SerDes Auto-Neg Advertisement Reg */
        SOC_IF_ERROR_RETURN
            (PHY_WRITE_BCM542XX_1000X_MII_ANAr(unit, pc, mii_adv));

        /* update the PHY property software table (phy_ctrl_t) */
        sal_memcpy( &(pc->fiber.advert_ability), ability,
                    sizeof(soc_port_ability_t) );
    } else {
        return SOC_E_INTERNAL;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "phy_bcm542xx_adv_local_set: u=%d p=%d "
                               "ability_hd_speed=0x%x, ability_fd_speed=0x%x, "
                               "ability_pause=0x%x\n"),
              unit, port, ability->speed_half_duplex, ability->speed_full_duplex,
              ability->pause));
    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx_ability_advert_get
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
phy_bcm542xx_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16       mii_ana, mii_gb_ctrl, mii_adv;

    if ( NULL == ability ) {
        return SOC_E_PARAM;
    }
    sal_memset(ability, 0,  sizeof(soc_port_ability_t)); /* zero initialize */
    
    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_MII_ANAr(unit, pc,&mii_ana));
        SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_MII_GB_CTRLr(unit, pc,&mii_gb_ctrl));

        if ( mii_ana & PHY_BCM542XX_MII_ANA_HD_10 ) {
            ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
        }
        if ( mii_ana & PHY_BCM542XX_MII_ANA_FD_10 ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        }
        if ( mii_ana & PHY_BCM542XX_MII_ANA_HD_100 ) {
            ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
        }
        if ( mii_ana & PHY_BCM542XX_MII_ANA_FD_100 ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        }

        if ( mii_ana & PHY_BCM542XX_MII_ANA_ASYM_PAUSE ) {
            if ( mii_ana & PHY_BCM542XX_MII_ANA_PAUSE ) {
                ability->pause |= SOC_PA_PAUSE_RX;
            } else {
                ability->pause |= SOC_PA_PAUSE_TX;
            }
        } else if (mii_ana & PHY_BCM542XX_MII_ANA_PAUSE ) {
            ability->pause |= SOC_PA_PAUSE;
        }

        /* GE Specific values */
        if ( mii_gb_ctrl & PHY_BCM542XX_MII_GB_CTRL_ADV_1000FD ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        }
        if ( mii_gb_ctrl & PHY_BCM542XX_MII_GB_CTRL_ADV_1000HD ) {
            ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
        }
        /* EEE advertisement settings */
        if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE)) {
            uint16 eee_ability;
            SOC_IF_ERROR_RETURN
                (PHY_READ_BCM542XX_EEE_ADVr(unit, pc, &eee_ability));
            if (eee_ability & PHY_BCM542XX_EEE_ADV_10G) {
                ability->eee |= SOC_PA_EEE_10GB_BASET;
            }
            if (eee_ability & PHY_BCM542XX_EEE_ADV_1000) {
                ability->eee |= SOC_PA_EEE_1GB_BASET;
            }
            if (eee_ability & PHY_BCM542XX_EEE_ADV_100) {
                ability->eee |= SOC_PA_EEE_100MB_BASETX;
            }
        }

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_1000X_MII_ANAr(unit, pc,&mii_adv) );

        if ( mii_adv & PHY_BCM542XX_MII_ANA_C37_FD ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        }
        if ( mii_adv & PHY_BCM542XX_MII_ANA_C37_HD ) {
            ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
        }

        switch ( mii_adv & (PHY_BCM542XX_MII_ANA_C37_PAUSE |
                            PHY_BCM542XX_MII_ANA_C37_ASYM_PAUSE) ) {
        case PHY_BCM542XX_MII_ANA_C37_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case PHY_BCM542XX_MII_ANA_C37_ASYM_PAUSE:
            ability->pause = SOC_PA_PAUSE_TX;
            break;
        case (PHY_BCM542XX_MII_ANA_C37_PAUSE | PHY_BCM542XX_MII_ANA_C37_ASYM_PAUSE):
            ability->pause = SOC_PA_PAUSE_RX;
            break;
        }

    } else {
        return SOC_E_INTERNAL;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "phy_bcm542xx_adv_local_get: u=%d p=%d "
                               "ability_hd_speed=0x%x, ability_fd_speed=0x%x, "
                               "ability_pause=0x%x\n"),
              unit, port, ability->speed_half_duplex,
              ability->speed_full_duplex, ability->pause));
    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx0_ability_remote_get
 * Purpose:
 *      Get partners current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The remote advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level. If Autonegotiation is
 *      disabled or in progress, this routine will return an error.
 */

STATIC int
phy_bcm542xx_ability_remote_get(int unit, soc_port_t port,
                                soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;
    uint16       mii_stat, mii_anp, mii_gb_stat;
    uint16       eee_resolution;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(ability, 0, sizeof(soc_port_ability_t)); /*zero init*/
    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_MII_STATr(unit, pc,&mii_stat));

         if ( ! (mii_stat & PHY_BCM542XX_MII_STAT_AN_DONE) ) {
            return SOC_E_NONE;
        }

        SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_MII_ANPr(unit, pc,&mii_anp));
        SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_MII_GB_STATr(unit, pc,&mii_gb_stat));
        
        if ( mii_anp & PHY_BCM542XX_MII_ANA_HD_10 ) {
            ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
        }
        if ( mii_anp & PHY_BCM542XX_MII_ANA_FD_10 ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        }
        if ( mii_anp & PHY_BCM542XX_MII_ANA_HD_100 ) {
            ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
        }
        if ( mii_anp & PHY_BCM542XX_MII_ANA_FD_100 ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        }
        if ( mii_gb_stat & PHY_BCM542XX_MII_GB_STAT_LP_1000HD ) {
            ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
        }
        if ( mii_gb_stat & PHY_BCM542XX_MII_GB_STAT_LP_1000FD ) {
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        }

        if ( mii_anp & PHY_BCM542XX_MII_ANA_ASYM_PAUSE ) {
            if ( mii_anp & PHY_BCM542XX_MII_ANA_PAUSE ) {
                ability->pause |= SOC_PA_PAUSE_RX;
            } else {
                ability->pause |= SOC_PA_PAUSE_TX;
            }
        } else {
            if ( mii_anp & PHY_BCM542XX_MII_ANA_PAUSE ) {
                ability->pause |= SOC_PA_PAUSE;
            }
        }

        /* EEE settings */
        if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
            SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_EEE_RESOLUTION_STATr(unit, pc,&eee_resolution));
            if ( eee_resolution & 0x04 ) {
                ability->eee |= SOC_PA_EEE_1GB_BASET;
            }
            if ( eee_resolution & 0x02 ) {
                ability->eee |= SOC_PA_EEE_100MB_BASETX;
            }
        }

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN
            (PHY_READ_BCM542XX_1000X_MII_ANPr(unit, pc,&mii_anp));
        if ( mii_anp & PHY_BCM542XX_MII_ANP_SGMII_MODE ) { /* Link partner is SGMII master */
            if ( mii_anp & PHY_BCM542XX_MII_ANP_SGMII_FD ) { /* Full Duplex */
                switch(mii_anp & PHY_BCM542XX_MII_ANP_SGMII_SS_MASK) {
                case PHY_BCM542XX_MII_ANP_SGMII_SS_10:
                    ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
                    break;
                case PHY_BCM542XX_MII_ANP_SGMII_SS_100:
                    ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
                    break;
                case PHY_BCM542XX_MII_ANP_SGMII_SS_1000:
                    ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
                    break;
                }
            } else { /* Half duplex */
                switch(mii_anp & PHY_BCM542XX_MII_ANP_SGMII_SS_MASK) {
                case PHY_BCM542XX_MII_ANP_SGMII_SS_10:
                    ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
                    break;
                case PHY_BCM542XX_MII_ANP_SGMII_SS_100:
                    ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
                    break;
                case PHY_BCM542XX_MII_ANP_SGMII_SS_1000:
                    ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
                    break;
                }
            }

        } else {
            if ( mii_anp & PHY_BCM542XX_MII_ANP_C37_FD ) {
                ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
            }
            if ( mii_anp & PHY_BCM542XX_MII_ANP_C37_HD ) {
                ability->speed_half_duplex |= SOC_PA_SPEED_1000MB;
            }

            switch ( mii_anp & (PHY_BCM542XX_MII_ANP_C37_PAUSE |
                                PHY_BCM542XX_MII_ANP_C37_ASYM_PAUSE) ) {
            case PHY_BCM542XX_MII_ANP_C37_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case PHY_BCM542XX_MII_ANP_C37_ASYM_PAUSE:
                ability->pause = SOC_PA_PAUSE_TX;
                break;
            case (PHY_BCM542XX_MII_ANP_C37_PAUSE | PHY_BCM542XX_MII_ANP_C37_ASYM_PAUSE):
                ability->pause = SOC_PA_PAUSE_RX;
                break;
            }
        }

    } else {
        return SOC_E_INTERNAL;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "phy_bcm542xx_adv_remote_get: u=%d p=%d "
                               "ability_hd_speed=0x%x ability_fd_speed=0x%x "
                               "ability_pause=0x%x\n"),
              unit, port, ability->speed_half_duplex,
              ability->speed_full_duplex, ability->pause));
    return ( SOC_E_NONE );
}


/*
 * Function:
 *      phy_bcm542xx_ability_local_get
 * Purpose:
 *      Get the abilities of the local gigabit PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The ability is retrieved only for the ACTIVE medium.
 */

STATIC int
phy_bcm542xx_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t  *pc;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }
    pc = EXT_PHY_SW_STATE(unit, port);
    sal_memset(ability, 0,  sizeof(soc_port_ability_t)); /* zero initialize */

    if ( PHY_COPPER_MODE(unit, port) ) {
        ability->speed_half_duplex  = SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB |
                                      SOC_PA_SPEED_100MB | 
                                      SOC_PA_SPEED_10MB;
                                      
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_SGMII;
        ability->medium    = SOC_PA_MEDIUM_COPPER; 
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = SOC_PA_AUTONEG;
        
        /* EEE settings */
        if ( PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
                ability->eee |= SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX;
        }

        if ( pc->automedium ) {
            ability->flags     |= SOC_PA_COMBO;
        }
    } else if ( PHY_FIBER_MODE(unit, port) ) {
        ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB |SOC_PA_SPEED_100MB;
                                      
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_SGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER; 
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = SOC_PA_AUTONEG;

        if ( pc->automedium ) {
            ability->flags     |= SOC_PA_COMBO;
        }

    } else {
        return SOC_E_INTERNAL;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "phy_bcm542xx_ability_local_get: u=%d p=%d "
                               "ability_hd_speed=0x%x, ability_fd_speed=0x%x, "
                               "ability_pause=0x%x\n"),
              unit, port, ability->speed_half_duplex,
              ability->speed_full_duplex, ability->pause));
    return ( SOC_E_NONE );
}

/*
 * Function:
 *      phy_bcm542xx_lb_set
 * Purpose:
 *      Set the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_bcm542xx_lb_set(int unit, soc_port_t port, int enable)
{
    int          rv = SOC_E_NONE;
    phy_ctrl_t  *pc;
    int l_enable = 0;
    soc_timeout_t  to;
    uint16       mii_ctrl, temp16;
    pc = EXT_PHY_SW_STATE(unit, port);
    PHY_BCM542XX_DEFAULT_REG_ACCESS_MODE(unit, pc);

    if ( PHY_COPPER_MODE(unit, port) ) {
#if AUTO_MDIX_WHEN_AN_DIS
        {
            /* Disable Auto-MDIX When autoneg disabled */
            /* Enable Auto-MDIX When autoneg disabled */
            temp16 = (enable) ? 0x0000 : PHY_BCM542XX_MII_FORCE_AUTO_MDIX_MODE;
            PHY_MODIFY_BCM542XX_MII_MISC_CTRLr(unit, pc, temp16, 
                                        PHY_BCM542XX_MII_FORCE_AUTO_MDIX_MODE);
        }
#endif
        /* Set loopback setting for 1G/100M/10M */
        SOC_IF_ERROR_RETURN( PHY_READ_BCM542XX_MII_CTRLr(unit, pc,&mii_ctrl) );
        mii_ctrl &= ~PHY_BCM542XX_MII_CTRL_LPBK_EN;
        mii_ctrl |= (enable) ? PHY_BCM542XX_MII_CTRL_LPBK_EN : 0;
        SOC_IF_ERROR_RETURN(
                PHY_WRITE_BCM542XX_MII_CTRLr(unit, pc, mii_ctrl) );
        /* Force link is required for 100/10M speeds to work in loopback */
        phy_bcm542xx_direct_reg_modify(unit, pc,
                PHY_BCM542XX_TEST1_REG_OFFSET, 
                (enable) ? PHY_BCM542XX_TEST1_REG_FORCE_LINK : 0,
                PHY_BCM542XX_TEST1_REG_FORCE_LINK);
        /* Check the loopback status if it is enabled or not */
        soc_timeout_init(&to, 2000000, 0);
        do {
            SOC_IF_ERROR_RETURN(
                PHY_READ_BCM542XX_MII_CTRLr(unit, pc,&mii_ctrl) );
            l_enable = (mii_ctrl & PHY_BCM542XX_MII_CTRL_LPBK_EN) ? 1 : 0;
        } while((enable !=l_enable) && !soc_timeout_check(&to));
        if(enable !=l_enable){
            LOG_ERROR(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                      "phy_bcm542xx_lb_set: u=%d p=%d TIMEOUT\n"),
                      unit, port));

            rv = SOC_E_TIMEOUT;
        }

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        temp16 = (enable) ? PHY_BCM542XX_1000BASE_X_CTRL_REG_LE : 0;
        rv = PHY_MODIFY_BCM542XX_1000X_MII_CTRLr(unit, pc, temp16,
                                        PHY_BCM542XX_1000BASE_X_CTRL_REG_LE);
        /* Check the loopback status if it is enabled or not */
        soc_timeout_init(&to, 2000000, 0);
        do{
            SOC_IF_ERROR_RETURN(
                PHY_READ_BCM542XX_1000X_MII_CTRLr(unit, pc,&mii_ctrl) );
            l_enable = (mii_ctrl & PHY_BCM542XX_1000BASE_X_CTRL_REG_LE) ? 1 : 0;
        } while((enable != l_enable) && !soc_timeout_check(&to));
        if(enable !=l_enable){
            LOG_ERROR(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                      "phy_bcm542xx_lb_set: u=%d p=%d TIMEOUT\n"),
                      unit, port));

            rv = SOC_E_TIMEOUT;
        }

        if (enable && SOC_SUCCESS(rv) ) {
            uint16         stat;
            int            link;

            /* Wait up to 5000 msec for link up */
            soc_timeout_init(&to, 5000000, 0);
            link = 0;
            while ( ! soc_timeout_check(&to) ) {
                rv = PHY_READ_BCM542XX_1000X_MII_STATr(unit, pc,&stat);
                link = stat & PHY_BCM542XX_1000BASE_X_STAT_REG_LA;
                if ( link || SOC_FAILURE(rv) ) {
                    break;
                }
            }
            if ( ! link ) {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "phy_bcm542xx_lb_set: u=%d p=%d TIMEOUT\n"),
                          unit, port));

                rv = SOC_E_TIMEOUT;
            }
        }

    } else {
        return SOC_E_INTERNAL;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "phy_bcm542xx_lb_set: u=%d p=%d en=%d rv=%d\n"), 
              unit, port, enable, rv));
    return rv; 
}

/*
 * Function:
 *      phy_bcm542xx_lb_get
 * Purpose:
 *      Get the local PHY loopback mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      loopback - (OUT) Boolean: true = enable loopback, false = disable.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The loopback mode is retrieved for the ACTIVE medium.
 */
STATIC int
phy_bcm542xx_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16 mii_ctrl;
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_MII_CTRLr(unit, pc,&mii_ctrl) );
        *enable = (mii_ctrl & PHY_BCM542XX_MII_CTRL_LPBK_EN) ? 1 : 0;
    } else if ( PHY_FIBER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(
            PHY_READ_BCM542XX_1000X_MII_CTRLr(unit, pc,&mii_ctrl) );
        *enable = (mii_ctrl & PHY_BCM542XX_1000BASE_X_CTRL_REG_LE) ? 1 : 0;
    } else {
        return SOC_E_INTERNAL;
    }

    return ( SOC_E_NONE ); 
}

/*
 * Function:
 *      phy_bcm542xx_interface_set
 * Purpose:
 *      Set the current operating mode of the PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_UNAVAIL - unsupported interface
 */

STATIC int
phy_bcm542xx_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    switch (pif) {
    case SOC_PORT_IF_QSGMII:
    case SOC_PORT_IF_SGMII:
    case SOC_PORT_IF_GMII:
    case SOC_PORT_IF_RGMII:    
        return SOC_E_NONE;
    default:
        return SOC_E_UNAVAIL;
    }
}

/*
 * Function:
 *      phy_bcm542xx_interface_get
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
phy_bcm542xx_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if ( IS_GMII_PORT(unit, port) ) {
        *pif = SOC_PORT_IF_GMII;
    } else {
        *pif = SOC_PORT_IF_SGMII;
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_bcm542xx_mdix_set
 * Description:
 *      Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_bcm542xx_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    int rv = SOC_E_NONE;
    phy_ctrl_t *pc;
    int speed = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    PHY_BCM542XX_DEFAULT_REG_ACCESS_MODE(unit, pc);

    if ( PHY_COPPER_MODE(unit, port) ) {
        switch( mode ) {
        case SOC_PORT_MDIX_AUTO:
            /* Clear bit 14 for automatic MDI crossover */
            SOC_IF_ERROR_RETURN(
                PHY_MODIFY_BCM542XX_MII_ECRr(unit, pc, 0,
                                        PHY_BCM542XX_MII_ECR_DAMC) );
            /* Clear bit 9 to disable forced auto MDI xover */
            SOC_IF_ERROR_RETURN(
                PHY_MODIFY_BCM542XX_MII_MISC_CTRLr(unit, pc, 0, 
                                        PHY_BCM542XX_MII_FORCE_AUTO_MDIX_MODE) );
            break;
        case SOC_PORT_MDIX_FORCE_AUTO:
            /* Clear bit 14 for automatic MDI crossover */
            SOC_IF_ERROR_RETURN(
                PHY_MODIFY_BCM542XX_MII_ECRr(unit, pc, 0,
                                        PHY_BCM542XX_MII_ECR_DAMC) );

            /* Set bit 9 to enable forced auto MDI xover */
            SOC_IF_ERROR_RETURN(
                PHY_MODIFY_BCM542XX_MII_MISC_CTRLr(unit, pc,
                                        PHY_BCM542XX_MII_FORCE_AUTO_MDIX_MODE,
                                        PHY_BCM542XX_MII_FORCE_AUTO_MDIX_MODE) );
            break;
        case SOC_PORT_MDIX_NORMAL:
            rv = phy_bcm542xx_speed_get(unit, port, &speed);
            if (SOC_FAILURE(rv) ) {
                return SOC_E_FAIL;
            }
            if (speed == 0 || speed == 10 || speed == 100) {
                /* Set bit 14 for automatic MDI crossover */
                SOC_IF_ERROR_RETURN(
                    PHY_MODIFY_BCM542XX_MII_ECRr(unit, pc,
                                                 PHY_BCM542XX_MII_ECR_DAMC, 
                                                 PHY_BCM542XX_MII_ECR_DAMC) );
                SOC_IF_ERROR_RETURN(
                    PHY_WRITE_BCM542XX_TEST1r(unit, pc, 0));
            } else {
                return SOC_E_UNAVAIL;
            }
            break;
        case SOC_PORT_MDIX_XOVER:
            rv = phy_bcm542xx_speed_get(unit, port, &speed);
            if (SOC_FAILURE(rv) ) {
                return SOC_E_FAIL;
            }
            if (speed == 0 || speed == 10 || speed == 100) {
                /* Set bit 14 for automatic MDI crossover */
                SOC_IF_ERROR_RETURN(
                    PHY_MODIFY_BCM542XX_MII_ECRr(unit, pc,
                                                 PHY_BCM542XX_MII_ECR_DAMC, 
                                                 PHY_BCM542XX_MII_ECR_DAMC) );
                SOC_IF_ERROR_RETURN(
                    PHY_WRITE_BCM542XX_TEST1r(unit, pc, 0x0080));
            } else {
                return SOC_E_UNAVAIL;
            }
            break;
        default :
            return SOC_E_CONFIG;
        }
        pc->copper.mdix = mode;

    } else if ( PHY_FIBER_MODE(unit, port) ) {
        if ( mode != SOC_PORT_MDIX_NORMAL ) {
            return SOC_E_UNAVAIL;
        }

    } else {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}        

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *      phy_542xx_mdix_wb_update
 * Description:
 *      Read the hw, derive the mode and update the mdix mode in sw structure
 *      during wb.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      mode - (OUT) :One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_542xx_mdix_wb_update(int unit, soc_port_t port)
{
    phy_ctrl_t    *pc;
    int            speed;
    int            mode = 0;
    uint16         val = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Check bit 14 for automatic MDI crossover */
    SOC_IF_ERROR_RETURN (PHY_READ_BCM542XX_MII_ECRr(unit, pc, &val));
    if(val & 0x4000) {
        SOC_IF_ERROR_RETURN(phy_bcm542xx_speed_get(unit, port, &speed));
        if (speed == 0 || speed == 10 || speed == 100) {
            SOC_IF_ERROR_RETURN (PHY_READ_BCM542XX_TEST1r(unit, pc, &val));
            if (0 == val) {
                mode = SOC_PORT_MDIX_NORMAL;
            } else if (0x80 == val) {
                mode = SOC_PORT_MDIX_XOVER;
            } else {
                return SOC_E_UNAVAIL;
            }
        } else {
            return SOC_E_UNAVAIL;
        }
    } else {
        /* Check bit 9 forced auto MDI xover */
        SOC_IF_ERROR_RETURN (PHY_READ_BCM542XX_MII_MISC_CTRLr(unit, pc,&val));
        if (val & 0x200) {
            /* bit 9 Set, forced auto MDI xover */
            mode = SOC_PORT_MDIX_FORCE_AUTO;
        } else {
            /* bit 9 Clear, forced auto MDI xover */
            mode = SOC_PORT_MDIX_AUTO;
        }
    }
    pc->copper.mdix = mode;

    return SOC_E_NONE;
}
#endif /* defined(BCM_WARM_BOOT_SUPPORT) */


/*
 * Function:
 *      phy_bcm542xx_mdix_get
 * Description:
 *      Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - (Out) One of:
 *              SOC_PORT_MDIX_AUTO
 *                      Enable auto-MDIX when autonegotiation is enabled
 *              SOC_PORT_MDIX_FORCE_AUTO
 *                      Enable auto-MDIX always
 *              SOC_PORT_MDIX_NORMAL
 *                      Disable auto-MDIX
 *              SOC_PORT_MDIX_XOVER
 *                      Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_bcm542xx_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    phy_ctrl_t *pc;
    int speed = 0;

    if ( mode == NULL ) {
        return SOC_E_PARAM;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        return phy_542xx_mdix_wb_update(unit, port);
    }
#endif /* defined(BCM_WARM_BOOT_SUPPORT) */

    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN(phy_bcm542xx_speed_get(unit, port, &speed));
        if (speed == 1000) {
           *mode = SOC_PORT_MDIX_AUTO;
        } else {
            *mode = pc->copper.mdix;
        }
    } else {
        *mode = SOC_PORT_MDIX_NORMAL;
    }

    return SOC_E_NONE;
}    

/*
 * Function:
 *      phy_bcm542xx_mdix_status_get
 * Description:
 *      Get the current MDIX status on a port/PHY
 * Parameters:
 *      unit    - Device number
 *      port    - Port number
 *      status  - (OUT) One of:
 *              SOC_PORT_MDIX_STATUS_NORMAL
 *                      Straight connection
 *              SOC_PORT_MDIX_STATUS_XOVER
 *                      Crossover has been performed
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
phy_bcm542xx_mdix_status_get(int unit, soc_port_t port, 
                             soc_port_mdix_status_t *status)
{
    phy_ctrl_t    *pc;
    uint16 mii_esrr;

    if ( status == NULL ) {
        return SOC_E_PARAM;
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    if ( PHY_COPPER_MODE(unit, port) ) {
        SOC_IF_ERROR_RETURN
            (phy_bcm542xx_direct_reg_read(unit, pc,
                        PHY_BCM542XX_PHY_EXT_STATUS_REG_OFFSET,
                        &mii_esrr));
        if ( mii_esrr & 0x2000 ) {
            *status = SOC_PORT_MDIX_STATUS_XOVER;
        } else {
            *status = SOC_PORT_MDIX_STATUS_NORMAL;
        }

    } else if ( PHY_FIBER_MODE(unit, port) ) {
            *status = SOC_PORT_MDIX_STATUS_NORMAL;

    } else {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}    

/*
 * Function:
 *     phy_bcm542xx_encode_gmode 
 * Purpose:
 *     Global synchronization mode selection
 *     P1588 NSE DPLL NCO Register 6, RDB.0xA7F[15:14] GMODE
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_encode_gmode(soc_port_phy_timesync_global_mode_t mode,
                               uint16 *value)
{
    switch ( mode ) {
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

/*
 * Function:
 *     phy_bcm542xx_decode_gmode
 * Purpose:
 *     Global synchronization mode selection
 *     P1588 NSE DPLL NCO Register 6, RDB.0xA7F[15:14] GMODE
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_decode_gmode(uint16 value,
                               soc_port_phy_timesync_global_mode_t *mode)
{
    switch ( value & 0x3 ) {
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

/*
 * Function:
 *     phy_bcm542xx_encode_framesync_mode 
 * Purpose:
 *     FrameSync mode selection
 *     P1588 NSE DPLL NCO Register 6, RDB.0xA7F[5:2] FRAMESYNC_MODE
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_encode_framesync_mode(soc_port_phy_timesync_framesync_mode_t mode,
                                        uint16 *value)
{
    switch ( mode & 0xf ) {
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0:
        *value = BIT(0);
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1:
        *value = BIT(1);
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT:
        *value = BIT(2);
        break;
    case SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU:
        *value = BIT(3);
        break;
    default:
        *value = 0;
        break;
    }
}

/*
 * Function:
 *     phy_bcm542xx_decode_framesync_mode
 * Purpose:
 *     FrameSync mode selection
 *     P1588 NSE DPLL NCO Register 6, RDB.0xA7F[5:2] FRAMESYNC_MODE
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_decode_framesync_mode(uint16 value,
                                        soc_port_phy_timesync_framesync_mode_t *mode)
{
    switch ( value & 0xf ) {
    case BIT(0):
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0;
        break;
    case BIT(1):
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1;
        break;
    case BIT(2):
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT;
        break;
    case BIT(3):
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU;
        break;
    default:
        *mode = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_NONE;
        break;
    }
}


/*
 * Function:
 *     phy_bcm542xx_encode_sync_out_mode
 * Purpose:
 *     Sync_Out mode selection
 *     P1588 NSE DPLL NCO Register 6, RDB.0xA7F[1:0] SYNC_OUT_MODE
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_encode_sync_out_mode(soc_port_phy_timesync_syncout_mode_t mode,
                                       uint16 *value)
{
    switch ( mode ) {
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
    default:
        break;
    }
}


/*
 * Function:
 *     phy_bcm542xx_decode_sync_out_mode
 * Purpose:
 *     Sync_Out mode selection
 *     P1588 NSE DPLL NCO Register 6, RDB.0xA7F[1:0] SYNC_OUT_MODE
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_decode_sync_out_mode(uint16 value,
                                       soc_port_phy_timesync_syncout_mode_t *mode)
{
    switch ( value & 0x3 ) {
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
 * Function:
 *     phy_bcm542xx_encode_egress_message_mode
 * Purpose:
 *     P1588 Port_n TX Event Message Mode1/2 Selection Reg., RDB.0xa11+(n)
 *     for Message 0-3  ( Sync, Delay_Req, Pdelay_Req, Pdelay_Resp )
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_encode_egress_message_mode(
                soc_port_phy_timesync_event_message_egress_mode_t mode,
                int offset, uint16 *value, uint16 *mask)
{
    switch ( mode ) {
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE:
        *value |= (0x0 << offset);
        break; 
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD:
        *value |= (0x1 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN:
        *value |= (0x2 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP:
        *value |= (0x3 << offset);
        break; 
    default:
        break;
    }
    *mask |= 0x3 << offset;
}

/*
 * Function:
 *    phy_bcm542xx_decode_egress_message_mode
 * Purpose:
 *     P1588 Port_n TX Event Message Mode1/2 Selection Reg., RDB.0xa11+(n)
 *     for Message 0-3  ( Sync, Delay_Req, Pdelay_Req, Pdelay_Resp )
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_decode_egress_message_mode(uint16 value, int offset,
                soc_port_phy_timesync_event_message_egress_mode_t *mode)
{
    switch ( (value >> offset) & 0x3 ) {
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

/*
 * Function:
 *     phy_bcm542xx_encode_ingress_message_mode
 * Purpose:
 *     P1588 Port_n RX Event Message Mode1/2 Selection Reg., RDB.0xa19+(n)
 *     for Message 0-3  ( Sync, Delay_Req, Pdelay_Req, Pdelay_Resp )
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_encode_ingress_message_mode(
                soc_port_phy_timesync_event_message_ingress_mode_t mode,
                int offset, uint16 *value, uint16 *mask)
{
    switch ( mode ) {
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE:
        *value |= (0x0 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD:
        *value |= (0x1 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP:
        *value |= (0x2 << offset);
        break;
    case SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME:
        *value |= (0x3 << offset);
        break;
    default:
        break;
    }
    *mask |= 0x3 << offset;
}

/*
 * Function:
 *    phy_bcm542xx_decode_ingress_message_mode 
 * Purpose:
 *     P1588 Port_n RX Event Message Mode1/2 Selection Reg., RDB.0xa19+(n)
 *     for Message 0-3  ( Sync, Delay_Req, Pdelay_Req, Pdelay_Resp )
 * Parameters:
 *
 * Returns:
 */
void phy_bcm542xx_decode_ingress_message_mode(uint16 value, int offset,
                soc_port_phy_timesync_event_message_ingress_mode_t *mode)
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

/*
 * Function:
 *    phy_bcm542xx_timesync_config_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy_bcm542xx_timesync_config_set(int unit, soc_port_t port,
                                 soc_port_phy_timesync_config_t *conf)
{
    uint16  dev_port = 0;
    uint16  rx_control_reg = 0, tx_control_reg = 0;
    uint16  en_control_reg = 0, tx_capture_en_reg = 0, rx_capture_en_reg = 0;
    uint16  nse_nco_6 = 0, nse_nco_2c = 0, gmode =0, framesync_mode=0;
    uint16  syncout_mode=0, tx_ts_event_cap = 0, rx_ts_event_cap = 0;
    uint16  mpls_control_reg_tx = 0, mpls_control_reg_rx = 0, i;
    uint16  data16 = 0, mask16 = 0;
    phy_ctrl_t  *pc;

    if ( NULL == conf ) {
        return SOC_E_PARAM;
    }
   
    pc = EXT_PHY_SW_STATE(unit, port);

    /* Dev port numbers from 0-7 */
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    if ( conf->flags & SOC_PORT_PHY_TIMESYNC_ENABLE ) {
        en_control_reg |= ( BIT(dev_port + 8) | BIT(dev_port) );
        data16 = PHY_BCM542XX_EXPF5_TIME_SYNC_ENABLE;
    } else {
        data16 = 0;
    }
    /* CORE_EXPF5 - Time Sync control, RDB.0x0f5,  bit[0] TIMESYNC_EN */
    SOC_IF_ERROR_RETURN
        (PHY_MODIFY_BCM542XX_TIME_SYNC_CTRLr(unit, pc, data16,
                        PHY_BCM542XX_EXPF5_TIME_SYNC_ENABLE));

    /* Enable TX SOP & RX SOP capability in 10bt. */
    /* CORE_EXP45 - Pattern Generator Control, RDB.0x005, bit[12] TX_CRC_CHECKER_EN */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                        PHY_BCM542XX_PATT_GEN_CTRL_REG_OFFSET,
                        PHY_BCM542XX_PATT_GEN_CTRL_REG_TX_CRC_CHECK_EN,
                        PHY_BCM542XX_PATT_GEN_CTRL_REG_TX_CRC_CHECK_EN);
    /* CORE_EXPF5 - Time Sync control, RDB.0x0f5,  bit[8:7] */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                        PHY_BCM542XX_EXPF5_TIME_SYNC_REG_OFFSET,
                        PHY_BCM542XX_EXPF5_TIME_SYNC_TX_SOP_10BT_EN
                      | PHY_BCM542XX_EXPF5_TIME_SYNC_RX_SOP_10BT_EN,
                        PHY_BCM542XX_EXPF5_TIME_SYNC_TX_SOP_10BT_EN
                      | PHY_BCM542XX_EXPF5_TIME_SYNC_RX_SOP_10BT_EN);

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

    if ( conf->flags & SOC_PORT_PHY_TIMESYNC_ENABLE ) {
        data16 = 0;
    } else {
        data16 = PHY_BCM542XX_TOP_INTERRUPT_MASK;
    }
    /* TOP_MISCREG_INTERRUPT_MASK, RDB.0x82d */
    SOC_IF_ERROR_RETURN
        (phy_bcm542xx_direct_reg_modify(unit, pc,
                        PHY_BCM542XX_TOP_INTERRUPT_MASK_OFFSET, data16, 
                        PHY_BCM542XX_TOP_INTERRUPT_MASK));

    /* Enable/disable 1588 tx/rx per port*/
    phy_bcm542xx_direct_reg_modify(unit, pc,
                        PHY_BCM542XX_TOP_1588_SLICE_EN_CTRL_REG_OFFSET,  
                        en_control_reg, en_control_reg);
    /* TOP_1588_SLICE_ENABLE, RDB.0xa10 */
    phy_bcm542xx_direct_reg_read(unit, pc,
                        PHY_BCM542XX_TOP_1588_SLICE_EN_CTRL_REG_OFFSET,  
                        &en_control_reg);

    /* Enable TS capture + Select TS capture for event message type */
    if ( conf->flags & SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE ) {
        tx_capture_en_reg |= BIT(dev_port);
        rx_capture_en_reg |= BIT(dev_port);

        /* TOP_1588_NSE_NCO_6, RDB.0xa7f[12],  Initialize NSE Block */
        phy_bcm542xx_direct_reg_modify(unit, pc,
                        PHY_BCM542XX_TOP_1588_NSE_NCO_6_REG_OFFSET,
                        PHY_BCM542XX_TOP_1588_NSE_NCO_6_FRMSYN_EN,
                        PHY_BCM542XX_TOP_1588_NSE_NCO_6_FRMSYN_EN);                    

        if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE ) {
            tx_ts_event_cap |= BIT(8);
        }
        if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE ) { 
            rx_ts_event_cap |= BIT(8);
        }
        if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE ) {
            tx_ts_event_cap |= BIT(9);
        }
        if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE ) { 
            rx_ts_event_cap |= BIT(9);
        }
        if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE ) {
            tx_ts_event_cap |= BIT(10);
        }
        if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE ) { 
            rx_ts_event_cap |= BIT(10);
        }
        if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE ) {
            tx_ts_event_cap |= BIT(11);
        }
        if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE ) { 
            rx_ts_event_cap |= BIT(11);
        }
    }

    /* Select TS capture for event message type */
    /* TOP_1588_TX_PORT_n_TS_OFFSET_MSB, RDB.0xa35...,  bit[11:8] */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                     PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_MSB(dev_port),
                     tx_ts_event_cap,
                     tx_ts_event_cap);
    /* TOP_1588_RX_PORT_n_TS_OFFSET_MSB, RDB.0xa45...,  bit[11:8] */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                     PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_MSB(dev_port),
                     rx_ts_event_cap,
                     rx_ts_event_cap);

    /* Timestamp capture enable for Rx & TX */
    /* P1588 TX SOP Timestamp Capture Enable, RDB.0xa21 */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                     PHY_BCM542XX_TOP_1588_TX_TS_CAP_REG_OFFSET, 
                     tx_capture_en_reg,
                     BIT(dev_port));
    /* P1588 RX SOP Timestamp Capture Enable, RDB.0xa22 */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                     PHY_BCM542XX_TOP_1588_RX_TS_CAP_REG_OFFSET, 
                     rx_capture_en_reg,
                     BIT(dev_port));

    /* Enable timestamp capture on the next frame sync event */
    if ( conf->flags & SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE ) {
        nse_nco_6 |= PHY_BCM542XX_TOP_1588_NSE_NCO_6_TS_CAP;
    }
    /* P1588 NSE DPLL NCO Reg.6, RDB.0xa7f,  bit[13] */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                      PHY_BCM542XX_TOP_1588_NSE_NCO_6_REG_OFFSET, 
                      nse_nco_6, 
                      PHY_BCM542XX_TOP_1588_NSE_NCO_6_TS_CAP); 
                
    /* Enable the CRC check in PTP detection receiving side */
    data16 = (conf->flags & SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE)
             ? PHY_BCM542XX_TOP_1588_RX_TX_CTL_REG_RX_CRC_EN : 0;
    /* P1588 Receive/Transmit Control, RDB.0xa62,  bit[3] */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                     PHY_BCM542XX_TOP_1588_RX_TX_CTL_REG_OFFSET, data16, 
                     PHY_BCM542XX_TOP_1588_RX_TX_CTL_REG_RX_CRC_EN);

    /* Enable different packet detections & checks in Rx & Tx direction */
    if ( conf->flags & SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE ) {
        rx_control_reg |= PHY_BCM542XX_TOP_1588_RX_CTL_REG_RX_AS_EN;
        tx_control_reg |= PHY_BCM542XX_TOP_1588_TX_CTL_REG_TX_AS_EN;
    }
    if ( conf->flags & SOC_PORT_PHY_TIMESYNC_L2_ENABLE ) {
        rx_control_reg |= PHY_BCM542XX_TOP_1588_RX_CTL_REG_RX_L2_EN;
        tx_control_reg |= PHY_BCM542XX_TOP_1588_TX_CTL_REG_TX_L2_EN;
    }
    if ( conf->flags & SOC_PORT_PHY_TIMESYNC_IP4_ENABLE ) {
        rx_control_reg |= PHY_BCM542XX_TOP_1588_RX_CTL_REG_RX_IPV4_UDP_EN;
        tx_control_reg |= PHY_BCM542XX_TOP_1588_TX_CTL_REG_TX_IPV4_UDP_EN;
    }
    if ( conf->flags & SOC_PORT_PHY_TIMESYNC_IP6_ENABLE ) {
        rx_control_reg |= PHY_BCM542XX_TOP_1588_TX_CTL_REG_TX_IPV6_UDP_EN;
        tx_control_reg |= PHY_BCM542XX_TOP_1588_RX_CTL_REG_RX_IPV6_UDP_EN;
    }
    phy_bcm542xx_direct_reg_write(unit, pc,
                     PHY_BCM542XX_TOP_1588_TX_CTL_REG_OFFSET, 
                     tx_control_reg);
    phy_bcm542xx_direct_reg_write(unit, pc,
                     PHY_BCM542XX_TOP_1588_RX_CTL_REG_OFFSET, 
                     rx_control_reg);

    /* Set input for NCO adder */
    if ( conf->flags & SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT) {
        nse_nco_2c &= ~PHY_BCM542XX_TOP_1588_NSE_NCO_2_2_FREQ_MDIO_SEL;
    } else {
        nse_nco_2c |=  PHY_BCM542XX_TOP_1588_NSE_NCO_2_2_FREQ_MDIO_SEL;
    }

     phy_bcm542xx_direct_reg_modify(unit, pc,
                      PHY_BCM542XX_TOP_1588_NSE_NCO_2_2_REG_OFFSET, 
                      nse_nco_2c, 
                      PHY_BCM542XX_TOP_1588_NSE_NCO_2_2_FREQ_MDIO_SEL); 
    
    
    /* VLAN Tags */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID ) {
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_VLAN_ITPID_REG_OFFSET, 
                         conf->itpid); 
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID ) {

        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_VLAN_OTPID_REG_OFFSET, 
                         conf->otpid); 
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2 ) {
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_OTHER_OTPID_REG_OFFSET,
                         conf->otpid2); 
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER ) {
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_4_REG_OFFSET,
                         conf->ts_divider & 0xfff); 
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY ) {
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_PORTN_LINK_DELAY_LSB(dev_port),
                         conf->rx_link_delay & 0xffff); 
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_PORTN_LINK_DELAY_MSB(dev_port),
                         (conf->rx_link_delay>>16) & 0xffff);
    }

    /* TIMECODE */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE ) {
        /* P1588 Original Time Code Register 0-4 (80 bits), RDB.0xa54-a58 */
        phy_bcm542xx_direct_reg_write(unit, pc,
                     PHY_BCM542XX_TOP_1588_TIME_CODE_0_REG_OFFSET, 
                     (uint16)(conf->original_timecode.nanoseconds & 0xffff));
        phy_bcm542xx_direct_reg_write(unit, pc,
                     PHY_BCM542XX_TOP_1588_TIME_CODE_1_REG_OFFSET, 
                     (uint16)((conf->original_timecode.nanoseconds >> 16) & 0xffff));
        phy_bcm542xx_direct_reg_write(unit, pc,
                     PHY_BCM542XX_TOP_1588_TIME_CODE_2_REG_OFFSET, 
                     (uint16)(COMPILER_64_LO(conf->original_timecode.seconds) & 0xffff));
        phy_bcm542xx_direct_reg_write(unit, pc,
                     PHY_BCM542XX_TOP_1588_TIME_CODE_3_REG_OFFSET, 
                     (uint16)((COMPILER_64_LO(conf->original_timecode.seconds) >> 16) & 0xffff));
        phy_bcm542xx_direct_reg_write(unit, pc,
                     PHY_BCM542XX_TOP_1588_TIME_CODE_4_REG_OFFSET, 
                     (uint16)(COMPILER_64_HI(conf->original_timecode.seconds) & 0xffff));
    }

    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE ) {
        /* P1588 NSE DPLL NCO Reg.6, bit[15:14] Global sync mode selection */
        phy_bcm542xx_encode_gmode(conf->gmode, &gmode);
    }

    /* FrameSync */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE ) {
        phy_bcm542xx_encode_framesync_mode(conf->framesync.mode, &framesync_mode);

        /* P1588 NSE DPLL NCO Register 7, RDB.0xA80-A81 */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_1588_NSE_DPLL_NCO_7_0_REG_OFFSET,
                          conf->framesync.length_threshold);
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_1588_NSE_DPLL_NCO_7_1_REG_OFFSET,
                          conf->framesync.event_offset);
    }

    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE ) {
        phy_bcm542xx_encode_sync_out_mode(conf->syncout.mode, &syncout_mode);

        /* Interval & pulse width of syncout pulse */
        /* P1588 NSE DPLL NCO Register 3, RDB.0xA78-A7A */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_3_0_REG_OFFSET,
                         conf->syncout.interval & 0xffff); 
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_3_1_REG_OFFSET,
                         ( ((conf->syncout.pulse_1_length & 0x3) << 14) 
                         | ((conf->syncout.interval >> 16) & 0x3fff   )) );
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_3_2_REG_OFFSET ,
                         ( ((conf->syncout.pulse_2_length & 0x1ff) << 7) 
                         | ((conf->syncout.pulse_1_length >> 2) & 0x7f )) );

        /* Local timer for SYNC_OUT */
        /* P1588 NSE DPLL NCO Register 5, RDB.0xA7C-A7E, SYNOUT_TS_REG */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_5_0_REG_OFFSET,
                         (uint16)(COMPILER_64_LO(conf->syncout.syncout_ts) & 0xffff));
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_5_1_REG_OFFSET,
                         (uint16)((COMPILER_64_LO(conf->syncout.syncout_ts) >> 16) & 0xffff));
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_5_2_REG_OFFSET,
                         (uint16)(COMPILER_64_HI(conf->syncout.syncout_ts) & 0xffff));
    }

    /* GMODE */    /* P1588 NSE DPLL NCO Register 6, RDB.0xA7F */
    /* GMODE[15:14] FRAMESUNC_EN[12] FRAMESYNC_MODE[5:2] SYNC_OUT_MODE[1:0] */
    phy_bcm542xx_direct_reg_modify( unit, pc,
          PHY_BCM542XX_TOP_1588_NSE_NCO_6_REG_OFFSET,
          (gmode          << PHY_BCM542XX_TOP_1588_NSE_NCO_6_GMODE_SHIFT)       |
          (framesync_mode << PHY_BCM542XX_TOP_1588_NSE_NCO_6_FRMSYN_MODE_SHIFT) |
          (syncout_mode   << PHY_BCM542XX_TOP_1588_NSE_NCO_6_SYNCOUT_MODE_SHIFT)|
          (framesync_mode ?  PHY_BCM542XX_TOP_1588_NSE_NCO_6_FRMSYN_EN : 0),
          ( PHY_BCM542XX_TOP_1588_NSE_NCO_6_GMODE
          | PHY_BCM542XX_TOP_1588_NSE_NCO_6_FRMSYN_EN
          | PHY_BCM542XX_TOP_1588_NSE_NCO_6_FRMSYN_MODE
          | PHY_BCM542XX_TOP_1588_NSE_NCO_6_SYNCOUT_MODE) );

    /* TX TS OFFSET */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET ) {
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_LSB(dev_port), 
                         (uint16)(conf->tx_timestamp_offset & 0xffff));
        phy_bcm542xx_direct_reg_modify(unit, pc,
                         PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_MSB(dev_port), 
                         (uint16)((conf->tx_timestamp_offset >> 16) & 0xf ),
                         PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_TX_MSB);
    }

    /* RX TS OFFSET */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET ) {
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_LSB(dev_port), 
                         (uint16)(conf->rx_timestamp_offset & 0xffff));
        phy_bcm542xx_direct_reg_modify(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_MSB(dev_port), 
                         (uint16)((conf->rx_timestamp_offset >> 16) & 0xf),
                         PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_RX_MSB);
    }

    /* Tx Event Message Mode  for  Message 0-3 */
    /*                           ( Sync, Delay_Req, Pdelay_Req, Pdelay_Resp ) */
    data16 = mask16 = 0;
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE ) {
        phy_bcm542xx_encode_egress_message_mode(conf->tx_sync_mode,
                         0, &data16, &mask16);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE ) {
        phy_bcm542xx_encode_egress_message_mode(conf->tx_delay_request_mode,
                         2, &data16, &mask16);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE ) {
        phy_bcm542xx_encode_egress_message_mode(conf->tx_pdelay_request_mode,
                         4, &data16, &mask16);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE ) {
        phy_bcm542xx_encode_egress_message_mode(conf->tx_pdelay_response_mode,
                         6, &data16, &mask16);
    }
    /* P1588 Port_n TX Event Message Mode1/2 Selection Reg., RDB.0xa11+(n) */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                         PHY_BCM542XX_TOP_1588_TX_MODE_PORT_N_OFFSET(dev_port),
                         data16, mask16);

    /* Rx Event Message Mode  for  Message 0-3 */
    /*                           ( Sync, Delay_Req, Pdelay_Req, Pdelay_Resp ) */
    data16 = mask16 = 0;
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE ) {
        phy_bcm542xx_encode_ingress_message_mode(conf->rx_sync_mode,
                         0, &data16, &mask16);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE ) {
        phy_bcm542xx_encode_ingress_message_mode(conf->rx_delay_request_mode,
                         2, &data16, &mask16);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE ) {
        phy_bcm542xx_encode_ingress_message_mode(conf->rx_pdelay_request_mode,
                         4, &data16, &mask16);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE ) {
        phy_bcm542xx_encode_ingress_message_mode(conf->rx_pdelay_response_mode,
                         6, &data16, &mask16);
    }
    /* P1588 Port_n RX Event Message Mode1/2 Selection Reg., RDB.0xa19+(n) */
    phy_bcm542xx_direct_reg_modify(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_MODE_PORT_N_OFFSET(dev_port),
                         data16, mask16);

    /* DPLL initial reference phase */
    /* P1588 NSE DPLL Register 2 ,  RDB.0xA67-0xA69  (48 bits) */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {
        /* REF_PHASE[15:0] */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_2_0_REG_OFFSET,
                         (uint16)(COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) & 0xffff));   
        /* REF_PHASE[31:16] */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_2_1_REG_OFFSET,
                         (uint16)((COMPILER_64_LO(conf->phy_1588_dpll_ref_phase) >> 16) & 0xffff));   
        /* REF_PHASE[47:32] */    
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_2_2_REG_OFFSET,
                         (uint16)(COMPILER_64_HI(conf->phy_1588_dpll_ref_phase) & 0xffff));
    }

    /* P1588 NSE DPLL Register 3 ,  RDB.0xA6A-0xA6B  (32 bits) */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA) {
        /* Ref phase delta [15:0] */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_3_LSB_REG_OFFSET,
                         (uint16)(conf->phy_1588_dpll_ref_phase_delta & 0xffff));
        /* Ref phase delta [31:16]  */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_3_MSB_REG_OFFSET,
                         (uint16)((conf->phy_1588_dpll_ref_phase_delta >> 16)& 0xffff));
    }

    /* P1588 NSE DPLL Register 4/5/6 (K1/K2/K3) ,  RDB.0xA6C/0xA6D/0xA6E */
    /* DPLL K1 */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_4_REG_OFFSET,
                         conf->phy_1588_dpll_k1 & 0xff);
    }

    /* DPLL K2 */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_5_REG_OFFSET,
                         conf->phy_1588_dpll_k2 & 0xff);
    }

    /* DPLL K3 */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_6_REG_OFFSET,
                         conf->phy_1588_dpll_k3 & 0xff);    
    }

    /* P1588 NSE DPLL Register 7 ,  RDB.0xA6F-0xA72  (64 bits) */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER) {
        /* Initial loop filter[15:0] */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_7_0_REG_OFFSET,
                         (uint16)(COMPILER_64_LO(conf->phy_1588_dpll_loop_filter) & 0xffff));
        /* Initial loop filter[31:16]  */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_7_1_REG_OFFSET,
                         (uint16)((COMPILER_64_LO(conf->phy_1588_dpll_loop_filter) >> 16) & 0xffff));

        /*  Initial loop filter[47:32] */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_7_2_REG_OFFSET,
                         (uint16)(COMPILER_64_HI(conf->phy_1588_dpll_loop_filter) & 0xffff));
 

        /* Initial loop filter[63:48]  */
        phy_bcm542xx_direct_reg_write(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_7_3_REG_OFFSET,
                         (uint16)((COMPILER_64_HI(conf->phy_1588_dpll_loop_filter) >> 16) & 0xffff));
    }

    /* P1588 MPLS */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL ) {
        /* MPLS controls */
        if ( conf->mpls_control.flags & SOC_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE ) {
            mpls_control_reg_tx |= PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_ENTROPY_EN;
            mpls_control_reg_rx |= PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_ENTROPY_EN;
        }

        if (!(conf->mpls_control.flags & SOC_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE) ) {
            mpls_control_reg_tx |= PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_CW_DIS;
            mpls_control_reg_rx |= PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_CW_DIS;
        }

        if ( conf->mpls_control.flags & SOC_PORT_PHY_TIMESYNC_MPLS_ENABLE ) {
            mpls_control_reg_tx |= PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_EN;
            mpls_control_reg_rx |= PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_EN;
        }

        if ( conf->mpls_control.flags & SOC_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE ) {
            mpls_control_reg_tx |= PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_SP_LBL_EN; 
            mpls_control_reg_rx |= PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_SP_LBL_EN;
        }

        phy_bcm542xx_direct_reg_modify(unit, pc,
                         PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_OFFSET, 
                         mpls_control_reg_tx,
                         PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_ENTROPY_EN
                         | PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_CW_DIS 
                         | PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_EN
                         | PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_SP_LBL_EN);


        phy_bcm542xx_direct_reg_modify(unit, pc,
                         PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_OFFSET, 
                         mpls_control_reg_rx,
                         PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_ENTROPY_EN
                         | PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_CW_DIS 
                         | PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_EN
                         | PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_SP_LBL_EN);

        for ( i = 0 ; i < conf->mpls_control.size; i++ ) {
            phy_bcm542xx_direct_reg_write( unit, pc,
                    PHY_BCM542XX_TOP_1588_LABEL_N_LSB_MASK_REG_OFFSET(i), 
                    (conf->mpls_control.labels[i].mask & 0xffff) );
            phy_bcm542xx_direct_reg_modify( unit, pc,
                    PHY_BCM542XX_TOP_1588_LABEL_N_MSB_MASK_REG_OFFSET(i), 
                    ((conf->mpls_control.labels[i].mask >> 16) & 0xf), 0xf );
            phy_bcm542xx_direct_reg_write( unit, pc,
                    PHY_BCM542XX_TOP_1588_LABEL_N_LSB_VAL_REG_OFFSET(i), 
                    (conf->mpls_control.labels[i].value & 0xffff) );
            phy_bcm542xx_direct_reg_modify( unit, pc,
                    PHY_BCM542XX_TOP_1588_LABEL_N_MSB_VAL_REG_OFFSET(i), 
                    ((conf->mpls_control.labels[i].value >> 16) & 0xf), 0xf );
            /*Enable comparison for MPLS label value N */
            phy_bcm542xx_direct_reg_modify( unit, pc,
                    PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_OFFSET,
                    (BIT(6) << i), (BIT(6) << i) );
            phy_bcm542xx_direct_reg_modify( unit, pc,
                    PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_OFFSET,
                    (BIT(6) << i), (BIT(6) << i) );
        }
    }

    /* P1588 direct Timer adjustment */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TIMER_ADJUSTMENT ) {
        int val32 = conf->timer_adjust.delta;

        /* Timer adjustment delta should be a 14-bit two's complement value */
        if ( val32 > PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_VALUE_MAX ) {
            /* over  the 14-bit range, set the max. */
            data16 = PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_VALUE_MAX;
        } else
        if ( val32 < PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_VALUE_MIN ) {
            /* under the 14-bit range, set the min. */
            data16 = PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_VALUE_MIN;
        } else {
            /* value in range, extract the 14-bit two's complement value */
            data16 = ((uint16 ) val32) &
                     PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_VALUE_MASK;
        }

        /* determine which counter to be adjusted, 80- or 48-bit or both */
        if ( conf->timer_adjust.mode &
                                SOC_PORT_PHY_TIMESYNC_TIMER_MODE_48BIT ) {
            data16 |= PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_48BIT;
        }
        if ( conf->timer_adjust.mode &
                                SOC_PORT_PHY_TIMESYNC_TIMER_MODE_80BIT ) {
            data16 |= PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_80BIT;
        }

        /* P1588 Timestamp Delta Register, RDB.0xAf7 */
        phy_bcm542xx_direct_reg_write(unit, pc,
                PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_REG_OFFSET, data16);
    }

    /* IEEE-1588 Gen-2 Inband operation */
    if ( conf->validity_mask &
                     SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL ) {
        /*
         *  Inband control parameters setting
         */
        data16 = (conf->inband_control.resv0_id
                            << PHY_BCM542XX_TOP_1588_INBAND_CNTL_RESV0_ID_SHIFT)
                 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_RESV0_ID;
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_SYNC_EN;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_DELAY_REQ_EN;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_PDELAY_REQ_EN;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_PDELAY_RESP_EN;
        }

        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_RESV0_ID_CHECK;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_RESV0_ID_UPDATE;
        }
        if ( (conf->flags & SOC_PORT_PHY_TIMESYNC_FOLLOW_UP_ASSIST_ENABLE) ||
             (conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST) ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_TS_UPD_FOLLOW_UP |
                      PHY_BCM542XX_TOP_1588_INBAND_CNTL_TS_CAP_MODE;
        }
        if ( (conf->flags & SOC_PORT_PHY_TIMESYNC_DELAY_RESP_ASSIST_ENABLE) ||
             (conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST) ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_TS_UPD_DELAY_RSP |
                      PHY_BCM542XX_TOP_1588_INBAND_CNTL_TS_CAP_MODE;
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_TIMER_MODE_SELECT ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_TS_SEL;
        }
        if ( conf->inband_control.timer_mode &
                        SOC_PORT_PHY_TIMESYNC_TIMER_MODE_80BIT ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_TS_SEL;
        }

        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR ) {
            data16 |= PHY_BCM542XX_TOP_1588_INBAND_CNTL_IP_ADDR_EN; 
        }
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR ) {
            data16 |= (PHY_BCM542XX_TOP_1588_INBAND_CNTL_IP_ADDR_EN |
                       PHY_BCM542XX_TOP_1588_INBAND_CNTL_IP_MAC_SEL);
        }

        /* P1588 Inband Control Port_n Register, RDB.0xAEE~Af5 */
        phy_bcm542xx_direct_reg_write(unit, pc,
                PHY_BCM542XX_TOP_1588_INBAND_CNTL_N_REG_OFFSET(dev_port),
                data16);

        /*
         *  MPLS setting for the label_1 mask
         */
        data16 = 0;
        if ( conf->inband_control.flags & 
                        SOC_PORT_PHY_TIMESYNC_INBAND_CAP_SRC_PORT_CLK_ID) {
            data16 |= PHY_BCM542XX_TOP_1588_CAP_SRC_PORT_CLK_ID;
        } 
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM) {
            data16 |= PHY_BCM542XX_TOP_1588_MATCH_SRC_PORT_NUM;
        } 
        if ( conf->inband_control.flags &
                        SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID) {
            data16 |= PHY_BCM542XX_TOP_1588_MATCH_VLAN_ID;
        }
        mask16 = PHY_BCM542XX_TOP_1588_CAP_SRC_PORT_CLK_ID
               | PHY_BCM542XX_TOP_1588_MATCH_SRC_PORT_NUM
               | PHY_BCM542XX_TOP_1588_MATCH_VLAN_ID;

        /* P1588 MPLS label1 mask msb bit, RDB.0xA98, bit[15:13] */
        phy_bcm542xx_direct_reg_modify( unit, pc,
                PHY_BCM542XX_TOP_1588_LABEL_N_MSB_MASK_REG_OFFSET(dev_port),
                data16, mask16 );

        /*
         *  Timestamp capture enable for Rx & TX
         */
        /* P1588 TX SOP Timestamp Capture Enable, RDB.0xa21 */
        phy_bcm542xx_direct_reg_modify( unit, pc,
                        PHY_BCM542XX_TOP_1588_TX_TS_CAP_REG_OFFSET, 
                        BIT(dev_port), BIT(dev_port) ); 
        /* P1588 RX SOP Timestamp Capture Enable, RDB.0xa22 */
        phy_bcm542xx_direct_reg_modify( unit, pc,
                        PHY_BCM542XX_TOP_1588_RX_TS_CAP_REG_OFFSET, 
                        BIT(dev_port), BIT(dev_port) ); 

        /* P1588 RX CF + Insertion Reg., RDB.0xAC0 */
        phy_bcm542xx_direct_reg_modify( unit, pc,
                        PHY_BCM542XX_TOP_1588_RX_CF_REG_OFFSET,
                        PHY_BCM542XX_TOP_1588_RX_CF_UPD(dev_port)
                      | PHY_BCM542XX_TOP_1588_RX_CS_UPD(dev_port),
                        PHY_BCM542XX_TOP_1588_RX_CF_UPD(dev_port)
                      | PHY_BCM542XX_TOP_1588_RX_CS_UPD(dev_port) );
        /* P1588 TX CS Update Reg., RDB.0xAC1 */
        phy_bcm542xx_direct_reg_modify( unit, pc,
                        PHY_BCM542XX_TOP_1588_TX_CS_UPD_REG_OFFSET,
                        PHY_BCM542XX_TOP_1588_TX_CS_UPD_MODE2(dev_port)
                      | PHY_BCM542XX_TOP_1588_TX_CS_UPD_MODE3(dev_port),
                        PHY_BCM542XX_TOP_1588_TX_CS_UPD_MODE2(dev_port)
                      | PHY_BCM542XX_TOP_1588_TX_CS_UPD_MODE3(dev_port) );
        /* P1588 P1588 TIMECODE Select Reg., RDB.0xAC3 */
        phy_bcm542xx_direct_reg_modify( unit, pc,
                        PHY_BCM542XX_TOP_1588_64b_TIMECODE_SEL_REG_OFFSET,
                        PHY_BCM542XX_TOP_1588_64b_TIMECODE_SEL_RX_N(dev_port)
                      | PHY_BCM542XX_TOP_1588_64b_TIMECODE_SEL_TX_N(dev_port),
                        PHY_BCM542XX_TOP_1588_64b_TIMECODE_SEL_RX_N(dev_port)
                      | PHY_BCM542XX_TOP_1588_64b_TIMECODE_SEL_TX_N(dev_port) );
        /* P1588 P1588 DPLL Debug Select Reg., RDB.0xA5B */
        phy_bcm542xx_direct_reg_modify( unit, pc,
                        PHY_BCM542XX_TOP_1588_DPLL_DB_SEL_REG_OFFSET,
                        PHY_BCM542XX_TOP_1588_DPLL_DB_SEL_RSVD,
                        PHY_BCM542XX_TOP_1588_DPLL_DB_SEL_RSVD );
    }/* TIMESYNC_VALID_PHY_1588_INBAND_CONTROL */

    /* Tx/Rx gphycore SOP or internal 1588 SOP: Select internal 1588 SOP (1) */
    /* P1588 SOP Selection Register, RDB.0xAF8, TX[7:0] RX[15:8] */
    phy_bcm542xx_direct_reg_modify( unit, pc,
                        PHY_BCM542XX_TOP_1588_SOP_SEL_REG_OFFSET,
                        PHY_BCM542XX_TOP_1588_SOP_SEL_TX_PORT_N(dev_port)
                      | PHY_BCM542XX_TOP_1588_SOP_SEL_RX_PORT_N(dev_port),
                        PHY_BCM542XX_TOP_1588_SOP_SEL_TX_PORT_N(dev_port)
                      | PHY_BCM542XX_TOP_1588_SOP_SEL_RX_PORT_N(dev_port) );

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
     * To access TOP level registers use phy_id of port0. Now restore back 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
    
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_timesync_config_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy_bcm542xx_timesync_config_get(int unit, soc_port_t port,
                                 soc_port_phy_timesync_config_t *conf)
{
    uint16 dev_port, tx_control_reg = 0, en_control_reg = 0; 
    uint16 tx_capture_en_reg = 0, rx_capture_en_reg=0, rx_tx_control_reg;
    uint16 nse_nco_6 = 0, nse_nco_2_2 = 0;
    uint16 i, temp1, temp2, temp3, temp4, value, mpls_control_reg_rx; 
    uint16 data16;

    soc_port_phy_timesync_global_mode_t  gmode
                                      = SOC_PORT_PHY_TIMESYNC_MODE_FREE;
    soc_port_phy_timesync_framesync_mode_t  framesync_mode
                                      = SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1;
    soc_port_phy_timesync_syncout_mode_t  syncout_mode
                                      = SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE;
    phy_ctrl_t *pc;

    if ( NULL == conf ) {
        return SOC_E_PARAM;
    }
    
    pc = EXT_PHY_SW_STATE(unit, port);

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
    
    /* Dev port numbers from 0-7 */
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    conf->flags = 0;

    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FLAGS) {
        /* SLICE ENABLE CONTROL register */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_SLICE_EN_CTRL_REG_OFFSET, 
                         &en_control_reg); 

        /* Tx & Rx (+8) */
        if ( (en_control_reg & BIT(dev_port)) &&
             (en_control_reg & BIT(dev_port + 8)) ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_ENABLE;
        }

        /* TxRx SOP TS CAPTURE ENABLE reg */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TX_TS_CAP_REG_OFFSET, 
                         &tx_capture_en_reg);

        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_TS_CAP_REG_OFFSET, 
                         &rx_capture_en_reg); 

        if ( (tx_capture_en_reg & BIT(dev_port)) &&
             (rx_capture_en_reg & BIT(dev_port)) ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;
        }

        /* P1588 NSE DPLL NCO Register 6, RDB.0xA7F */
        /*                GMODE[15:14] FRAMESYNC_MODE[5:2] SYNC_OUT_MODE[1:0] */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_6_REG_OFFSET,
                         &nse_nco_6);

        if ( nse_nco_6 & PHY_BCM542XX_TOP_1588_NSE_NCO_6_TS_CAP) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE;
        }

        /* RX TX CONTROL register */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_TX_CTL_REG_OFFSET,
                         &rx_tx_control_reg);

        if ( rx_tx_control_reg & PHY_BCM542XX_TOP_1588_RX_TX_CTL_REG_RX_CRC_EN ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE;
        }

        /* TX CONTROL register */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TX_CTL_REG_OFFSET,
                         &tx_control_reg);
                                          
        if ( tx_control_reg & PHY_BCM542XX_TOP_1588_TX_CTL_REG_TX_AS_EN ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE;
        }

        if ( tx_control_reg & PHY_BCM542XX_TOP_1588_TX_CTL_REG_TX_L2_EN ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_L2_ENABLE;
        }

        if ( tx_control_reg & PHY_BCM542XX_TOP_1588_TX_CTL_REG_TX_IPV4_UDP_EN ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_IP4_ENABLE;
        }

        if ( tx_control_reg & PHY_BCM542XX_TOP_1588_TX_CTL_REG_TX_IPV6_UDP_EN ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_IP6_ENABLE;
        }
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_2_2_REG_OFFSET,
                         &nse_nco_2_2);

        if ( !(nse_nco_2_2 & PHY_BCM542XX_TOP_1588_NSE_NCO_2_2_FREQ_MDIO_SEL) ) {
            conf->flags |= SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;
        }
    }

    /* P1588 NSE DPLL NCO Register 6, RDB.0xA7F */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_GMODE ) {
        /* P1588 NSE DPLL NCO Reg.6, bit[15:14] Global sync mode selection */
        phy_bcm542xx_decode_gmode((nse_nco_6 >> 14), &gmode);
        conf->gmode = gmode;
    }

    /* framesync */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE ) {
        /* P1588 NSE DPLL NCO Reg.6, FRAMESYNC_MODE[5:2] */
        phy_bcm542xx_decode_framesync_mode((nse_nco_6 >> 2), &framesync_mode);
        conf->framesync.mode = framesync_mode;

        /* P1588 NSE DPLL NCO Register 7, RDB.0xa80-a81 */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_1588_NSE_DPLL_NCO_7_0_REG_OFFSET,
                         &value);
        conf->framesync.length_threshold = value;
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_1588_NSE_DPLL_NCO_7_1_REG_OFFSET,
                         &value);
        conf->framesync.event_offset = value;
    }

    /* Syncout */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE ) {
        /* P1588 NSE DPLL NCO Reg.6, SYNC_OUT_MODE[1:0] */
        phy_bcm542xx_decode_sync_out_mode((nse_nco_6 ), &syncout_mode); 
        conf->syncout.mode = syncout_mode;

        /* TOP_1588_NSE_NCO_3 - P1588 NSE DPLL NCO Register 3, RDB.0xa78-a7a */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_3_0_REG_OFFSET,
                         &temp1);
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_3_1_REG_OFFSET,
                         &temp2);
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_3_2_REG_OFFSET,
                         &temp3);
        /* PULSE_TRAIN_LENGTH */
        conf->syncout.pulse_1_length = ((temp3 & 0x7f) << 2)
                                     | ((temp2 >> 14) & 0x3);
        /* FRMSYNC_PULSE_LENGTH */
        conf->syncout.pulse_2_length = (temp3 >> 7) & 0x1ff;
        /* INTERVAL_LENGTH */
        conf->syncout.interval =  ((temp2 & 0x3fff) << 16) | temp1;

        /* TOP_1588_NSE_NCO_5_0 - P1588 NSE DPLL NCO Register, RDB.0xa7c-a7e */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_5_0_REG_OFFSET,
                         &temp1);
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_5_1_REG_OFFSET,
                         &temp2);
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_5_2_REG_OFFSET,
                         &temp3);
        /* SYNOUT_TS_REG (48 bits, but only [47:4] are useful)
           time interval to trigger SYNC pulse                  */
        COMPILER_64_SET( conf->syncout.syncout_ts, ((uint32)temp3),
                         (((uint32)temp2 << 16) | ((uint32)temp1)) );
    }

    /* VLAN Tags */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ITPID ) {

        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_VLAN_ITPID_REG_OFFSET, 
                         &conf->itpid); 
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID ) {

        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_VLAN_OTPID_REG_OFFSET, 
                         &conf->otpid); 
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_OTPID2 ) {
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_OTHER_OTPID_REG_OFFSET,
                         &conf->otpid2); 
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER ) {
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_NCO_4_REG_OFFSET,
                         &conf->ts_divider); 
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY ) {
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_PORTN_LINK_DELAY_LSB(dev_port),
                         &temp1); 
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_PORTN_LINK_DELAY_MSB(dev_port),
                         &temp2);
        conf->rx_link_delay = ((uint32)temp2 << 16) | temp1; 
    }

    /* TIMECODE */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE ) {
        /* P1588 Original Time Code Register 0-4 (80 bits), RDB.0xa54-a58 */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TIME_CODE_0_REG_OFFSET, 
                         &temp1);
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TIME_CODE_1_REG_OFFSET, 
                         &temp2);
        conf->original_timecode.nanoseconds = ((uint32)temp2 << 16) | temp1;

        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TIME_CODE_2_REG_OFFSET, 
                         &temp1);
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TIME_CODE_3_REG_OFFSET, 
                         &temp2);
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TIME_CODE_4_REG_OFFSET, 
                         &temp3);
        COMPILER_64_SET( conf->original_timecode.seconds, ((uint32)temp3),
                         (((uint32)temp2 << 16) | ((uint32)temp1)) );
    }

    /* TX TS OFFSET */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET ) {
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_LSB(dev_port), 
                         &temp1);
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_MSB(dev_port), 
                         &temp2);
        conf->tx_timestamp_offset = ((temp2 & 0xf) << 16) | temp1;
    }   

    /* RX TS OFFSET */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET ) {
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_LSB(dev_port), 
                         &temp1);
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_MSB(dev_port), 
                         &temp2);
        conf->rx_timestamp_offset = ((temp2 & 0xf) << 16) | temp1;
    }

    /* P1588 Port_n TX Event Message Mode1/2 Selection Reg., RDB.0xa11+(n) */
    phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_TX_MODE_PORT_N_OFFSET(dev_port),
                         &value);
    /* Tx Event Message Mode  for  Message 0-3 */
    /*                           ( Sync, Delay_Req, Pdelay_Req, Pdelay_Resp ) */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE ) {
        phy_bcm542xx_decode_egress_message_mode(value, 0, &conf->tx_sync_mode);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE ) {
        phy_bcm542xx_decode_egress_message_mode(value, 2, &conf->tx_delay_request_mode);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE ) {
        phy_bcm542xx_decode_egress_message_mode(value, 4, &conf->tx_pdelay_request_mode);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE ) {
        phy_bcm542xx_decode_egress_message_mode(value, 6, &conf->tx_pdelay_response_mode);
    }

    /* P1588 Port_n RX Event Message Mode1/2 Selection Reg., RDB.0xa19+(n) */
    phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_RX_MODE_PORT_N_OFFSET(dev_port),
                         &value);
    /* Rx Event Message Mode  for  Message 0-3 */
    /*                           ( Sync, Delay_Req, Pdelay_Req, Pdelay_Resp ) */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE ) {
        phy_bcm542xx_decode_ingress_message_mode(value, 0, &conf->rx_sync_mode);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE ) {
        phy_bcm542xx_decode_ingress_message_mode(value, 2, &conf->rx_delay_request_mode);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE ) {
        phy_bcm542xx_decode_ingress_message_mode(value, 4, &conf->rx_pdelay_request_mode);
    }
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE ) {
        phy_bcm542xx_decode_ingress_message_mode(value, 6, &conf->rx_pdelay_response_mode);
    }

    /* DPLL initial reference phase */
    /* P1588 NSE DPLL Register 2 ,  RDB.0xA67-0xA69  (48 bits) */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE) {
        /*[15:0]*/
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_2_0_REG_OFFSET,
                         &temp1);   
        /*[31:16]*/
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_2_1_REG_OFFSET,
                         &temp2);   
        /*[47:32]*/    
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_2_2_REG_OFFSET,
                         &temp3);
        COMPILER_64_SET( conf->phy_1588_dpll_ref_phase, ((uint32)temp3),
                         (((uint32)temp2 << 16) | ((uint32)temp1)) );
    }

    /* P1588 NSE DPLL Register 3 ,  RDB.0xA6A-0xA6B  (32 bits) */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA ) {
            /* Ref phase delta [15:0] */
            phy_bcm542xx_direct_reg_read(unit, pc,
                             PHY_BCM542XX_TOP_1588_NSE_DPLL_3_LSB_REG_OFFSET,
                             &temp1);
            /* Ref phase delta [31:16]  */
            phy_bcm542xx_direct_reg_read(unit, pc,
                             PHY_BCM542XX_TOP_1588_NSE_DPLL_3_MSB_REG_OFFSET,
                             &temp2);
            conf->phy_1588_dpll_ref_phase_delta = (temp2 << 16) | temp1;
    }

    /* P1588 NSE DPLL Register 4/5/6 (K1/K2/K3) ,  RDB.0xA6C/0xA6D/0xA6E */
    /* DPLL K1 */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1) {
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_4_REG_OFFSET,
                         &temp1);
        conf->phy_1588_dpll_k1 = temp1 & 0xff;
    }

    /* DPLL K2 */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2) {
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_5_REG_OFFSET,
                         &temp1);
        conf->phy_1588_dpll_k2 = temp1 & 0xff;
    }

    /* DPLL K3 */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3) {
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_6_REG_OFFSET,
                         &temp1);
        conf->phy_1588_dpll_k3 = temp1 & 0xff;
    }

    /* P1588 NSE DPLL Register 7 ,  RDB.0xA6F-0xA72  (64 bits) */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER ) {
        /* Initial loop filter[15:0] */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_7_0_REG_OFFSET,
                         &temp1);
        /* Initial loop filter[31:16]  */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_7_1_REG_OFFSET,
                         &temp2);
        /*  Initial loop filter[47:32] */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_7_2_REG_OFFSET,
                         &temp3);
        /* Initial loop filter[63:48]  */
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_NSE_DPLL_7_3_REG_OFFSET,
                         &temp4);
        COMPILER_64_SET( conf->phy_1588_dpll_loop_filter,
                         (((uint32)temp4 << 16) | ((uint32)temp3)),
                         (((uint32)temp2 << 16) | ((uint32)temp1)) );
    }

    /* P1588 MPLS */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL) {
        /* MPLS controls */
#if 0
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_OFFSET, 
                         &mpls_control_reg_tx);
#endif
        phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_OFFSET, 
                         &mpls_control_reg_rx);

        if ( (mpls_control_reg_rx & PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_ENTROPY_EN)
            /*&& (mpls_control_reg_tx & PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_ENTROPY_EN)*/) {
            conf->mpls_control.flags |= SOC_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE;
        }
        if ( !(mpls_control_reg_rx & PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_CW_DIS))
            /*&& (mpls_control_reg_tx & PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_CW_DIS)*/ {
            conf->mpls_control.flags |= SOC_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE;
        }
        if ( (mpls_control_reg_rx & PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_EN)
            /*&& (mpls_control_reg_tx & PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_EN)*/) {
            conf->mpls_control.flags |= SOC_PORT_PHY_TIMESYNC_MPLS_ENABLE;
        }
        if ( (mpls_control_reg_rx & PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_SP_LBL_EN)
            /*&& (mpls_control_reg_tx & PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_SP_LBL_EN)*/) {
            conf->mpls_control.flags |= SOC_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE;
        }

        for ( i = 0 ; i < conf->mpls_control.size; i++ ) {
            phy_bcm542xx_direct_reg_read( unit, pc,
                    PHY_BCM542XX_TOP_1588_LABEL_N_LSB_MASK_REG_OFFSET(i), 
                    &temp1 );
            conf->mpls_control.labels[i].mask = temp1;
            phy_bcm542xx_direct_reg_read( unit, pc,
                    PHY_BCM542XX_TOP_1588_LABEL_N_MSB_MASK_REG_OFFSET(i), 
                    &temp1 );
            conf->mpls_control.labels[i].mask |= (temp1 & 0xf) << 16;

            phy_bcm542xx_direct_reg_read( unit, pc,
                    PHY_BCM542XX_TOP_1588_LABEL_N_LSB_VAL_REG_OFFSET(i), 
                    &temp1 );
            conf->mpls_control.labels[i].value = temp1;
            phy_bcm542xx_direct_reg_read( unit, pc,
                    PHY_BCM542XX_TOP_1588_LABEL_N_MSB_VAL_REG_OFFSET(i), 
                    &temp1 );
            conf->mpls_control.labels[i].value |= (temp1 & 0xf) << 16;
        }
    }

    /* P1588 direct Timer adjustment */
    if ( conf->validity_mask & SOC_PORT_PHY_TIMESYNC_VALID_TIMER_ADJUSTMENT ) {
        uint16  val16;
        /* P1588 Timestamp Delta Register, RDB.0xAf7 */
        phy_bcm542xx_direct_reg_read(unit, pc,
                PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_REG_OFFSET, &data16);

        /* Timestamp Delta Reg. bit[13:0] = Timer adjustment delta value */
        val16 = data16 & PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_VALUE_MASK;
        /* Timer adjustment delta is a 14-bit two's complement value */
        if ( val16 & PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_VALUE_SIGN ) {
            /* sign extension if negative */
            val16 |= PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_VALUE_SIGN_EXT;
        }
        conf->timer_adjust.delta = (int) val16;

        if ( data16 & PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_48BIT ) {
            conf->timer_adjust.mode |= SOC_PORT_PHY_TIMESYNC_TIMER_MODE_48BIT;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_TIMESTAMP_DELTA_80BIT ) {
            conf->timer_adjust.mode |= SOC_PORT_PHY_TIMESYNC_TIMER_MODE_80BIT;
        }
    }

    /* IEEE-1588 Gen-2 Inband operations */
    if ( (conf->validity_mask &
                SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL) ) {
        /* P1588 Inband Control Port_n Register, RDB.0xAEE~Af5 */
        phy_bcm542xx_direct_reg_read(unit, pc,
                PHY_BCM542XX_TOP_1588_INBAND_CNTL_N_REG_OFFSET(dev_port),
                &data16);
        conf->inband_control.resv0_id =         /* Rev0_ID[7:4] */
                (data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_RESV0_ID)
                 >> PHY_BCM542XX_TOP_1588_INBAND_CNTL_RESV0_ID_SHIFT;

        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_SYNC_EN ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_DELAY_REQ_EN ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_PDELAY_REQ_EN ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_PDELAY_RESP_EN ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE;
        }

        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_RESV0_ID_CHECK ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_RESV0_ID_UPDATE ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_TS_UPD_FOLLOW_UP ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST;
            conf->flags |=
                SOC_PORT_PHY_TIMESYNC_FOLLOW_UP_ASSIST_ENABLE;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_TS_UPD_DELAY_RSP ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST;
            conf->flags |=
                SOC_PORT_PHY_TIMESYNC_DELAY_RESP_ASSIST_ENABLE;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_TS_SEL ) {
            conf->inband_control.timer_mode |=
                      SOC_PORT_PHY_TIMESYNC_TIMER_MODE_80BIT;
        } else {
            conf->inband_control.timer_mode |=
                      SOC_PORT_PHY_TIMESYNC_TIMER_MODE_48BIT;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_IP_ADDR_EN ) {
            if ( data16 & PHY_BCM542XX_TOP_1588_INBAND_CNTL_IP_MAC_SEL ) {
                conf->inband_control.flags |=
                      SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR;
            } else {
                conf->inband_control.flags |=
                      SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR;
            }
        }

        /* P1588 MPLS label1 mask msb bit, RDB.0xA98, bit[15:13] */
        phy_bcm542xx_direct_reg_read(unit, pc,
                PHY_BCM542XX_TOP_1588_LABEL_N_MSB_MASK_REG_OFFSET(dev_port),
                &data16);
        if ( data16 & PHY_BCM542XX_TOP_1588_CAP_SRC_PORT_CLK_ID ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_CAP_SRC_PORT_CLK_ID;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_MATCH_SRC_PORT_NUM ) {
            conf->inband_control.flags |=
                PHY_BCM542XX_TOP_1588_MATCH_SRC_PORT_NUM;
        }
        if ( data16 & PHY_BCM542XX_TOP_1588_MATCH_VLAN_ID ) {
            conf->inband_control.flags |=
                SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID;
        }
    }/* SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL */

    /* TOP LEVEL REGISTER ACCESS -> RESTORE                                 *\
    \* To access TOP level registers use phy_id of port0. Now restore back  */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_timesync_control_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy_bcm542xx_timesync_control_set(int unit, soc_port_t port,
                                  soc_port_control_phy_timesync_t type,
                                  uint64 value)
{
    int          rv = SOC_E_NONE;
    uint16       dev_port, temp1, temp2;
    uint32       value32;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
    
    /* Dev port numbers from 0-7 */
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
    PHY_BCM542XX_RDB_ACCESS_MODE(unit, pc);   /* set RDB register access mode */
#endif

    switch ( type ) {
    case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:
        /* TOP_1588_HEARTBEAT, Heartbeat (48 bits), RDB.0xa86-0xa88 */
    case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:
        /* TOP_1588_TIME_STAMP, Tx Timestamp (48 bits), RDB.0xa89-0xa8b */
        return SOC_E_FAIL;    /* read-only registers */

    case SOC_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC:
        /* P1588 NSE DPLL NCO Register 6, RDB.0xA7F[5:2] FRAMESYNC_MODE */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                        PHY_BCM542XX_TOP_1588_NSE_NCO_6_REG_OFFSET,
                        (COMPILER_64_LO(value) & 0x0f) << 2, (0x0f << 2)), rv );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
       /* TOP_1588_NSE_NCO_1, NSE_REG_NCO_FREQCNTR (32 bits), RDB.0xa73-0xa74 */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,    /* LSB 16 bits */
                        PHY_BCM542XX_TOP_1588_NSE_NCO_1_0_REG_OFFSET,
                        (uint16) (COMPILER_64_LO(value) & 0xffff)), rv );
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,    /* MSB 16 bits */
                        PHY_BCM542XX_TOP_1588_NSE_NCO_1_1_REG_OFFSET,
                        (uint16) ((COMPILER_64_LO(value) >> 16) & 0xffff)), rv);
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
        /* TOP_1588_NSE_NCO_2, Local Time (44 bits), RDB.0xa75-0xa77 */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                        PHY_BCM542XX_TOP_1588_NSE_NCO_2_0_REG_OFFSET,
                        (uint16) (COMPILER_64_LO(value) & 0xffff)), rv );
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                        PHY_BCM542XX_TOP_1588_NSE_NCO_2_1_REG_OFFSET,
                        (uint16) ((COMPILER_64_LO(value) >> 16) & 0xffff)), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                        PHY_BCM542XX_TOP_1588_NSE_NCO_2_2_REG_OFFSET,
                        (uint16) (COMPILER_64_HI(value) & 0xfff), 0xfff), rv );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:
        temp1 = temp2 = 0;
        value32 = COMPILER_64_LO(value);

        if ( value32 & SOC_PORT_PHY_TIMESYNC_TN_LOAD ) {
           temp1 |= BIT(11);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD ) {
           temp2 |= BIT(11);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD ) {
           temp1 |= BIT(10);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD ) {
           temp2 |= BIT(10);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD ) {
           temp1 |= BIT(9);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD ) {
           temp2 |= BIT(9);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD ) {
           temp1 |= BIT(8);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD ) {
           temp2 |= BIT(8);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD ) {
           temp1 |= BIT(7);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD ) {
           temp2 |= BIT(7);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD ) {
           temp1 |= BIT(6);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD ) {
           temp2 |= BIT(6);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD ) {
           temp1 |= BIT(5);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD ) {
           temp2 |= BIT(5);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD ) {
           temp1 |= BIT(4);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD ) {
           temp2 |= BIT(4);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD ) {
           temp1 |= BIT(3);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD ) {
           temp2 |= BIT(3);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD ) {
           temp1 |= BIT(2);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD ) {
           temp2 |= BIT(2);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD ) {
           temp1 |= BIT(1);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD ) {
           temp2 |= BIT(1);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD ) {
           temp1 |= BIT(0);
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD ) {
           temp2 |= BIT(0);
        }

        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_SHD_LD_REG_OFFSET , temp1), rv );
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_SHD_CTL_REG_OFFSET, temp2), rv );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK:
        /* disables the interrupt */
        temp1 = 0;
        value32 = COMPILER_64_LO(value);
        if ( value32 & SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK) {
           temp1 |= BIT((dev_port + 1));
        }
        if ( value32 & SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK) {
           temp1 |= BIT(0);
        }
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                    PHY_BCM542XX_TOP_1588_INT_MASK_REG_OFFSET,
                    temp1, BIT((dev_port + 1)) | BIT(0)), rv );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
        value32 = COMPILER_64_LO(value);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_LSB(dev_port),
                    (uint16) (value32 & 0xffff)), rv );
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                    PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_MSB(dev_port),
                    (uint16) ((value32 >> 16) & 0xf ),
                    PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_TX_MSB), rv );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
        value32 = COMPILER_64_LO(value);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_LSB(dev_port),
                    (uint16) (value32 & 0xffff)), rv );
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                    PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_MSB(dev_port),
                    (uint16) ((value32 >> 16) & 0xf),
                    PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_RX_MSB), rv );
        break;
       
    default:
        rv = SOC_E_UNAVAIL;
        break;
    } 

#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
    PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc); /* legacy register access mode */
#endif
    /* NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE                          */
    /* To access TOP level registers use phy_id of port0. Now restore back */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

    return (rv);
}

/*
 * Function:
 *    phy_bcm542xx_timesync_control_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy_bcm542xx_timesync_control_get(int unit, soc_port_t port,
                                  soc_port_control_phy_timesync_t type,
                                  uint64 *value)
{
    int          rv = SOC_E_NONE;
    uint32       value32 = 0;
    uint16       dev_port, value0, value1, value2;

    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

    /* Dev port numbers from 0-7 */
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
    PHY_BCM542XX_RDB_ACCESS_MODE(unit, pc);   /* set RDB register access mode */
#endif

    switch ( type ) {
    case SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP:
        /* P1588 Control/Debug Register, RDB.0xa8e[11:10] HB_CNTL */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                  PHY_BCM542XX_TOP_1588_CNTR_DBG_REG_OFFSET,  
                  PHY_BCM542XX_TOP_1588_CNTR_DBG_HB_RD_START,
                  PHY_BCM542XX_TOP_1588_CNTR_DBG_HB_CNTL), rv);
        /* TOP_1588_HEARTBEAT, Heartbeat (48 bits), RDB.0xa86-0xa88 */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_HEARTBEAT_0_REG_OFFSET, &value0), rv);
        SOC_IF_ERROR_BREAK(
        phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_HEARTBEAT_1_REG_OFFSET, &value1), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_HEARTBEAT_2_REG_OFFSET, &value2), rv);
        /* P1588 Control/Debug Register, RDB.0xa8e[11:10] HB_CNTL */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                  PHY_BCM542XX_TOP_1588_CNTR_DBG_REG_OFFSET,  
                  PHY_BCM542XX_TOP_1588_CNTR_DBG_HB_RD_END,
                  PHY_BCM542XX_TOP_1588_CNTR_DBG_HB_CNTL), rv);
        COMPILER_64_SET( (*value), ((uint32)value2),
                         (((uint32)value1 << 16) | ((uint32)value0)) );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP:
        /* P1588 Control/Debug Register, RDB.0xa8e[9:7] TS_SLICE_SEL */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                  PHY_BCM542XX_TOP_1588_CNTR_DBG_REG_OFFSET,  
                  (dev_port << CNTR_DBG_TS_SLICE_SEL_SHIFT),
                  PHY_BCM542XX_TOP_1588_CNTR_DBG_TS_SLICE_SEL), rv);
        /* P1588 Timestamp READ START/END Register, RDB.0xa85 */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                  PHY_BCM542XX_TOP_1588_TS_RD_START_END_REG_OFFSET,  
                  PHY_BCM542XX_TOP_1588_TS_RD_START(dev_port)), rv);

        /* TOP_1588_TIME_STAMP, Tx Timestamp (48 bits), RDB.0xa89-0xa8b */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_TIMESTAMP_0_REG_OFFSET, &value0), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_TIMESTAMP_1_REG_OFFSET, &value1), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_TIMESTAMP_2_REG_OFFSET, &value2), rv);
        SOC_IF_ERROR_BREAK(

        /* P1588 Timestamp READ START/END Register, RDB.0xa85 */
        phy_bcm542xx_rdb_reg_write(unit, pc,
                  PHY_BCM542XX_TOP_1588_TS_RD_START_END_REG_OFFSET,  
                  PHY_BCM542XX_TOP_1588_TS_RD_END(dev_port)), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_write(unit, pc,
                  PHY_BCM542XX_TOP_1588_TS_RD_START_END_REG_OFFSET, 0), rv);

        COMPILER_64_SET( (*value), ((uint32)value2),
                         (((uint32)value1 << 16) | ((uint32)value0)) );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC:
        /* P1588 NSE DPLL NCO Register 6, RDB.0xA7F[5:2] FRAMESYNC_MODE */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_NSE_NCO_6_REG_OFFSET, &value0), rv);
        COMPILER_64_SET( (*value), 0, ((uint32) value0 >> 2) & 0x0f );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND:
        /* TOP_1588_NSE_NCO_1, NSE_REG_NCO_FREQCNTR (32 bits), RDB.0xa73-0xa74 */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_NSE_NCO_1_0_REG_OFFSET, &value0), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_NSE_NCO_1_1_REG_OFFSET, &value1), rv);
        COMPILER_64_SET( (*value), ((uint32) 0),
                         (((uint32)value1 << 16) | ((uint32)value0)) );
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME:
        /* TOP_1588_NSE_NCO_2, Local Time (44 bits), RDB.0xa75-0xa77 */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_NSE_NCO_2_0_REG_OFFSET, &value0), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_NSE_NCO_2_1_REG_OFFSET, &value1), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_NSE_NCO_2_2_REG_OFFSET, &value2), rv);
        COMPILER_64_SET( (*value), (value2 & 0x0fff),          /* bit [43:32] */ 
                         ((uint32)value1 << 16) | value0 );    /* bit [31:0]  */
        COMPILER_64_SHL((*value), 4);
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL:
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_SHD_LD_REG_OFFSET , &value1), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                  PHY_BCM542XX_TOP_1588_SHD_CTL_REG_OFFSET, &value2), rv);

        if ( value1 & BIT(11) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TN_LOAD;
        }
        if ( value2 & BIT(11) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD;
        }
        if ( value1 & BIT(10) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD;
        }
        if ( value2 & BIT(10) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD;
        }
        if ( value1 & BIT(9) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD;
        }
        if ( value2 & BIT(9) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD;
        }
        if ( value1 & BIT(8) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD;
        }
        if ( value1 & BIT(8) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD;
        }
        if ( value1 & BIT(7) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD;
        }
        if ( value1 & BIT(7) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD;
        }
        if ( value1 & BIT(6) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD;
        }
        if ( value2 & BIT(6) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD;
        }
        if ( value1 & BIT(5) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD;
        }
        if ( value2 & BIT(5) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD;
        }
        if ( value1 & BIT(4) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD;
        }
        if ( value2 & BIT(4) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD;
        }
        if ( value1 & BIT(3) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD;
        }
        if ( value2 & BIT(3) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD;
        }
        if ( value1 & BIT(2) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD;
        }
        if ( value2 & BIT(2) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD;
        }
        if ( value1 & BIT(1) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD;
        }
        if ( value2 & BIT(1) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD;
        }
        if ( value1 & BIT(0) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD;
        }
        if ( value2 & BIT(0) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD;
        }

        COMPILER_64_SET((*value), (0), (value32));
        break;
 
    case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT:
        /* This doesn't clear the interrupts. 
           To clear read the timestamp.
         */
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_INT_STATUS_REG_OFFSET, &value1), rv);
        if ( value1 & BIT((dev_port + 1)) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT;
        }
        if ( value1 & BIT(0) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT;
        }
        COMPILER_64_SET((*value), (uint32)((value1 >> 1) & 0xff), (value32));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK:
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_INT_MASK_REG_OFFSET, &value1), rv);
        if ( value1 & BIT((dev_port + 1)) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK;
        }
        if ( value1 & BIT(0) ) {
            value32 |= SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK;
        }
        COMPILER_64_SET((*value), (0), (value32));
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET:
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_LSB(dev_port), 
                    &value1), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_TX_PORT_N_TS_OFFSET_MSB(dev_port), 
                    &value2), rv);
        COMPILER_64_SET((*value), 0, (((value2 & 0xf) << 16) | value1)); 
        break;

    case SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET:
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_LSB(dev_port), 
                    &value1), rv);
        SOC_IF_ERROR_BREAK(
            phy_bcm542xx_rdb_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_RX_PORT_N_TS_OFFSET_MSB(dev_port), 
                    &value2), rv);
        COMPILER_64_SET((*value), 0, (((value2 & 0xf) << 16) | value1)); 
        break; 

    default:
        rv = SOC_E_UNAVAIL;
    }

#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
    PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc); /* legacy register access mode */
#endif
    /* NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE                             */
    /* To access TOP level registers use phy_id of port0. Now restore back    */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
    
    return (rv);
}

STATIC int
_phy_phy542xx_inband_timesync_get_msg_type(uint16 value,
                              soc_port_phy_timesync_msg_t *msg_type) {
    value = (value  >> PHY_BCM542XX_TOP_1588_TS_INFO_MSG_TYPE_SHIFT)
        & PHY_BCM542XX_TOP_1588_TS_INFO_MSG_TYPE;
    switch(value) {
        case 0:
            *msg_type = SOC_PORT_PHY_TIMESYNC_MSG_SYNC;
            break;
        case 1:
            *msg_type = SOC_PORT_PHY_TIMESYNC_MSG_DELAY_REQ;
            break;
        case 2:
            *msg_type = SOC_PORT_PHY_TIMESYNC_MSG_PDELAY_REQ;
            break;
        case 3:
            *msg_type = SOC_PORT_PHY_TIMESYNC_MSG_PDELAY_RESP;
            break;
        default:
            return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_phy_phy542xx_inband_timesync_get_protocol(uint16 value,
                              soc_port_phy_timesync_prot_t *protocol) {
    value = (value  >> PHY_BCM542XX_TOP_1588_TS_INFO_DOMAIN_NUM_SHIFT)
        & PHY_BCM542XX_TOP_1588_TS_INFO_DOMAIN_NUM;
    switch(value) {
        case 0:
            *protocol = SOC_PORT_PHY_TIMESYNC_PROT_LAYER2;
            break;
        case 1:
            *protocol = SOC_PORT_PHY_TIMESYNC_PROT_IPV4_UDP;
            break;
        case 2:
            *protocol = SOC_PORT_PHY_TIMESYNC_PROT_IPV6_UDP;
            break;
        default:
            return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}

STATIC int
phy_bcm542xx_timesync_enhanced_capture_get(int unit, soc_port_t port, 
                              soc_port_phy_timesync_enhanced_capture_t *value) {
    uint16 dev_port, value0, value1, value2, value3;
    phy_ctrl_t *pc;
    uint16 data16 = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

    /* Dev port numbers from 0-7 */
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);

    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    /* MSG_TYPE, PROTOCOL, DIRECTION */
    SOC_IF_ERROR_RETURN(
        phy_bcm542xx_direct_reg_modify(unit, pc,
                    PHY_BCM542XX_TOP_1588_CNTR_DBG_REG_OFFSET,  
                    (dev_port << CNTR_DBG_TS_SLICE_SEL_SHIFT),
                    PHY_BCM542XX_TOP_1588_CNTR_DBG_TS_SLICE_SEL) );

    SOC_IF_ERROR_RETURN(
        phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_TS_INFO_1_REG_OFFSET, &value0) );
    SOC_IF_ERROR_RETURN(
        _phy_phy542xx_inband_timesync_get_msg_type(value0, &value->msg_type) );
    SOC_IF_ERROR_RETURN(
        _phy_phy542xx_inband_timesync_get_protocol(value0, &value->protocol) );

    value->direction = ( ((value0 >> PHY_BCM542XX_TOP_1588_TS_INFO_DIR_SHIFT)
                           & PHY_BCM542XX_TOP_1588_TS_INFO_DIR) == 0 )
                       ? SOC_PORT_PHY_TIMESYNC_DIR_EGRESS
                       : SOC_PORT_PHY_TIMESYNC_DIR_INGRESS;
    /* SEQ_ID */
    SOC_IF_ERROR_RETURN(
        phy_bcm542xx_direct_reg_read(unit, pc,
                     PHY_BCM542XX_TOP_1588_TS_INFO_2_REG_OFFSET, &value0) );
    value->seq_id = (value0 >> PHY_BCM542XX_TOP_1588_TS_INFO_SEQ_ID_SHIFT)
                      & PHY_BCM542XX_TOP_1588_TS_INFO_SEQ_ID;

    /* DOMAIN */

    /* TIMESTAMP */

    /* SRC PORT ID selected? */
    SOC_IF_ERROR_RETURN(
        phy_bcm542xx_direct_reg_read(unit, pc,
            PHY_BCM542XX_TOP_1588_LABEL_N_MSB_MASK_REG_OFFSET(dev_port),
            &data16) );

    if ( data16 & PHY_BCM542XX_TOP_1588_CAP_SRC_PORT_CLK_ID ) {
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_TS_INFO_3_REG_OFFSET, &value0) );
        value0 &= PHY_BCM542XX_TOP_1588_TS_INFO_SRC_PORT_CLK_ID_1;
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_TS_INFO_4_REG_OFFSET, &value1) );
        value1 &= PHY_BCM542XX_TOP_1588_TS_INFO_SRC_PORT_CLK_ID_2;
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_TS_INFO_5_REG_OFFSET, &value2) );
        value2 &= PHY_BCM542XX_TOP_1588_TS_INFO_SRC_PORT_CLK_ID_3;
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_TS_INFO_6_REG_OFFSET, &value3) );
        value3 &= PHY_BCM542XX_TOP_1588_TS_INFO_SRC_PORT_CLK_ID_4;
        COMPILER_64_SET( (value->source_port_identity), 
                         (value3 << 16) | value2, 
                         (value1 << 16) | value0 ); 
    } else {
        COMPILER_64_SET((value->source_port_identity), 0, 0); 
    }


    /* SRC PORT */
    SOC_IF_ERROR_RETURN(
        phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_TS_INFO_7_REG_OFFSET, &value0) );
    value->source_port &= PHY_BCM542XX_TOP_1588_TS_INFO_SRC_PORT_NUM;

    /* VLAN ID */
    SOC_IF_ERROR_RETURN(
        phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_TS_INFO_8_REG_OFFSET, &value0) );
    value->vlan_id &= PHY_BCM542XX_TOP_1588_TS_INFO_VLAN_ID;

   /***********************************************************************
    * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
    * To access TOP level registers use phy_id of port0. Now restore back 
    */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
        
    return SOC_E_NONE;
}


/*
 * Function:
 *      phy_54280_cable_diag_dispatch
 * Purpose:
 *      Run 54280 cable diagnostics
 * Parameters:
 *      unit - device number
 *      port - port number
 *      status - (OUT) cable diagnotic status structure
 * Returns: 
 *      SOC_E_XXX
 */
STATIC int
phy_54280_cable_diag_dispatch(int unit, soc_port_t port,
                              soc_port_cable_diag_t *status)
{
    uint16 ipair = 0;
    /* ECD_CTRL : break link */
    uint16 ecd_ctrl = 0x1000;
    phy_ctrl_t *pc = EXT_PHY_SW_STATE(unit, port);

    /* ECD functions use legacy register addressing mode */
    PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc);
    status->fuzz_len = 10;
    status->npairs = 4;
    status->state = SOC_PORT_CABLE_STATE_OK;
    
    SOC_IF_ERROR_RETURN(
        phy_ecd_cable_diag_init_40nm(unit, port));

    SOC_IF_ERROR_RETURN(
        phy_ecd_cable_diag_40nm(unit, port, status, ecd_ctrl));
    for(ipair=0; ipair <=3; ipair++){
        if (status->pair_state[ipair] > status->state) {
            status->state = status->pair_state[ipair];
        }
    }
    PHY_BCM542XX_DEFAULT_REG_ACCESS_MODE(unit, pc);
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_clock_enable_set
 * Purpose:
 *    SyncE primary/secondary recovery clodk
 * Parameters:
 *    bit_shift :  0 for primary, 4 for secondary
 * Returns:
 */
int
phy_bcm542xx_clock_enable_set(int unit, phy_ctrl_t *pc,
                              int bit_shift, uint32 enable)
{
    int     rv   = SOC_E_NONE;
    uint16  data = 0;
    int     dev_port;

    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    /* act on the 1st port of this chip */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

    /* SyncE Recovery Clock Registe, RDB.0x83c                            *\
    |*  bit[7]: disable secondary, bit[6:4] source of secondary clock     *|
    \*  bit[3]: disable primary  , bit[2:0] source of primary   clock     */
    data = (enable)
           ? ((dev_port & PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK_SEL)
               << bit_shift)
           : ((bit_shift == 0) ? (PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK1_DIS)
                               : (PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK2_DIS));
    /* Set bit[14]: primary recovery clock, bit[13]: secondary recovery clock for BCM54292*/
    if(enable && (PHY_IS_BCM54292(pc))) {
            data |= ((bit_shift == 0) ? PHY_BCM542XX_SYNCE_RECOVERY_CLK1_EN
                                      : PHY_BCM542XX_SYNCE_RECOVERY_CLK2_EN);
    }
 
    /* write to SYNCE Recovery Clock Registe, RDB 0x83c */
    if (PHY_IS_BCM54292(pc)) {
        rv = phy_bcm542xx_direct_reg_modify( unit, pc,
              PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_OFFSET, data,
              ((PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK_MASK << bit_shift) |
               ((bit_shift == 0) ? PHY_BCM542XX_SYNCE_RECOVERY_CLK1_EN : PHY_BCM542XX_SYNCE_RECOVERY_CLK2_EN)));
    } else {
        rv = phy_bcm542xx_direct_reg_modify( unit, pc,
              PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_OFFSET, data,
              (PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK_MASK << bit_shift));
    }

    /* resume to the original phy id */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
    return rv;
}

/*
 * Function:
 *    phy_bcm542xx_clock_enable_get
 * Purpose:
 *    SyncE primary/secondary recovery clock
 * Parameters:
 *    bit_shift :  0 for primary, 4 for secondary
 * Returns:
 */
int
phy_bcm542xx_clock_enable_get(int unit, phy_ctrl_t *pc,
                              int bit_shift, uint32 *value)
{
    int     rv   = SOC_E_NONE;
    uint16  data = 0;
    int     dev_port;

    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    /* act on the 1st port of this chip */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

    /* read from SYNCE Recovery Clock Registe, RDB 0x83c */
    rv = phy_bcm542xx_direct_reg_read( unit, pc,
                        PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_OFFSET, &data );

    /* resume to the original phy id */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

    /* SyncE Recovery Clock Registe, RDB.0x83c                            *\
    |*  bit[7]: disable secondary, bit[6:4] source of secondary clock     *|
    \*  bit[3]: disable primary  , bit[2:0] source of primary   clock     */
    data >>= bit_shift;
    *value = ( (data & PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK_DIS) ||
              ((data & PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK_SEL) != dev_port) ) 
             ? FALSE : TRUE;
    return rv;
}

/*
 * Function:
 *    phy_bcm542xx_clock_control_set
 * Purpose:
 * Parameters:
 * Returns:
 */
STATIC int
_phy_bcm542xx_clock_control_set(int unit, phy_ctrl_t *pc,
                                uint16 mask, uint32 value)
{
    int  rv = SOC_E_NONE;

    /* act on the 1st port of this chip */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

    /* TOP_MISCREG_SYNCE - SYNCE Recovery Clock Registe, RDB.0x83c */
    rv = phy_bcm542xx_direct_reg_modify(unit, pc,
                        PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_OFFSET,
                        ((value) ? mask : 0), mask );
    /* resume to the original phy id */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
    return rv;
}

/*
 * Function:
 *    phy_bcm542xx_clock_control_get
 * Purpose:
 * Parameters:
 * Returns:
 */
STATIC int
_phy_bcm542xx_clock_control_get(int unit, phy_ctrl_t *pc,
                                uint16 mask, uint32 *value)
{
    int     rv = SOC_E_NONE;
    uint16  data;

    /* act on the 1st port of this chip */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

    /* TOP_MISCREG_SYNCE - SYNCE Recovery Clock Registe, RDB.0x83c */
    rv = phy_bcm542xx_direct_reg_read( unit, pc,
                         PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_OFFSET, &data );
    *value = (data & mask) ? TRUE : FALSE;

    /* resume to the original phy id */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
    return rv;
}

/*
 * Function:
 *    phy_bcm542xx_power_mode_set
 * Purpose:
 * Parameters:
 * Returns:
 */
int
phy_bcm542xx_power_mode_set(int unit, phy_ctrl_t *pc, int mode)
{
    if ( pc->power_mode == mode) {
        return SOC_E_NONE;
    }

    if (mode == SOC_PHY_CONTROL_POWER_LOW) {
        /* enable dsp clock */
        PHY_MODIFY_BCM542XX_MII_AUX_CTRLr(unit, pc,
                            PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK, 
                            PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK);

        if (1) { /* CHECK if the interface is SGMII? */
            /* Select mode: SGMII->Copper */
            phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_MODE_CONTROL_REG_OFFSET, 
                            PHY_BCM542XX_MODE_CONTROL_REG_MODE_SEL 
                                & MODE_SELECT_SGMII_2_COPPER, 
                            PHY_BCM542XX_MODE_CONTROL_REG_MODE_SEL);

           /* Power down SGMII Interface  */ 
            phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_SGMII_CONTROL_REG_OFFSET, 
                            PHY_BCM542XX_SGMII_CONTROL_REG_POWER_DOWN, 
                            PHY_BCM542XX_SGMII_CONTROL_REG_POWER_DOWN);
        }

        /* disable dsp clock */
        PHY_MODIFY_BCM542XX_MII_AUX_CTRLr(unit, pc, 0x00,
                            PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK);

    } else if (mode == SOC_PHY_CONTROL_POWER_FULL) {
         /* enable dsp clock */
         PHY_MODIFY_BCM542XX_MII_AUX_CTRLr(unit, pc,
                            PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK, 
                            PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK);

         if (1) { /* CHECK if the interface is SGMII? */
            /* Select mode: SGMII->Copper */
            phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_MODE_CONTROL_REG_OFFSET, 
                            PHY_BCM542XX_MODE_CONTROL_REG_MODE_SEL 
                               & MODE_SELECT_SGMII_2_COPPER, 
                            PHY_BCM542XX_MODE_CONTROL_REG_MODE_SEL);

           /* Normal operation mode - disable power down - SGMII Interface  */ 
            phy_bcm542xx_direct_reg_modify(unit, pc,
                            PHY_BCM542XX_SGMII_CONTROL_REG_OFFSET, 
                            0x00, 
                            PHY_BCM542XX_SGMII_CONTROL_REG_POWER_DOWN);
        }

        /* disable dsp clock */
        PHY_MODIFY_BCM542XX_MII_AUX_CTRLr(unit, pc, 0x00,
                            PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK);

        /* disable the auto power mode */
        phy_bcm542xx_direct_reg_modify( unit, pc,
                            PHY_BCM542XX_POWER_AUTO_DOWN_REG_OFFSET, 
                            0x00, 
                            PHY_BCM542XX_POWER_AUTO_DOWN_REG_APD_MODE);
    } else if (mode == SOC_PHY_CONTROL_POWER_AUTO) {
        phy_bcm542xx_direct_reg_modify( unit, pc,
                            PHY_BCM542XX_POWER_AUTO_DOWN_REG_OFFSET, 
                            PHY_BCM542XX_POWER_AUTO_DOWN_REG_APD_MODE, 
                            PHY_BCM542XX_POWER_AUTO_DOWN_REG_APD_MODE);
    } else if (mode == SOC_PHY_CONTROL_POWER_AUTO_DISABLE) {
        phy_bcm542xx_direct_reg_modify( unit, pc,
                            PHY_BCM542XX_POWER_AUTO_DOWN_REG_OFFSET, 
                            0, 
                            PHY_BCM542XX_POWER_AUTO_DOWN_REG_APD_MODE);
    } else {
        return SOC_E_UNAVAIL;
    }

    pc->power_mode = mode;
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_power_auto_wake_time_set
 * Purpose:
 * Parameters:
 * Returns:
 */
int
phy_bcm542xx_power_auto_wake_time_set(int unit, phy_ctrl_t *pc, int value)
{
    value = (value <  PHY_BCM542XX_AUTO_PWRDWN_WAKEUP_MAX)? value : PHY_BCM542XX_AUTO_PWRDWN_WAKEUP_MAX;
    /* at least one unit */
    value = (value < PHY_BCM542XX_AUTO_PWRDWN_WAKEUP_UNIT)? PHY_BCM542XX_AUTO_PWRDWN_WAKEUP_UNIT : value;

    phy_bcm542xx_direct_reg_modify(unit, pc,
             PHY_BCM542XX_POWER_AUTO_DOWN_REG_OFFSET, 
             PHY_BCM542XX_POWER_AUTO_DOWN_REG_WAKE_UP_TIMER_SEL 
               & (value/PHY_BCM542XX_AUTO_PWRDWN_WAKEUP_UNIT), 
             PHY_BCM542XX_POWER_AUTO_DOWN_REG_WAKE_UP_TIMER_SEL);

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_power_auto_wake_time_get
 * Purpose:
 * Parameters:
 * Returns:
 */
int
phy_bcm542xx_power_auto_wake_time_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    uint16 data;
    phy_bcm542xx_direct_reg_read(unit, pc,
            PHY_BCM542XX_POWER_AUTO_DOWN_REG_OFFSET, &data);

    data &= PHY_BCM542XX_POWER_AUTO_DOWN_REG_WAKE_UP_TIMER_SEL;
    *value = data * PHY_BCM542XX_AUTO_PWRDWN_WAKEUP_UNIT;
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_power_auto_sleep_time_set
 * Purpose:
 * Parameters:
 * Returns:
 */
int
phy_bcm542xx_power_auto_sleep_time_set(int unit, phy_ctrl_t *pc, int value)
{
    /* sleep time configuration is either 2.7s or 5.4 s, default is 2.7s */
    value = (value < PHY_BCM542XX_AUTO_PWRDWN_SLEEP_MAX)? 0x00 : PHY_BCM542XX_POWER_AUTO_DOWN_REG_WAKE_UP_TIMER_SEL;
    phy_bcm542xx_direct_reg_modify( unit, pc,
            PHY_BCM542XX_POWER_AUTO_DOWN_REG_OFFSET, 
            value, 
            PHY_BCM542XX_POWER_AUTO_DOWN_REG_SLEEP_TIMER_SEL);

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_power_auto_sleep_time_get
 * Purpose:
 * Parameters:
 * Returns:
 */
int
phy_bcm542xx_power_auto_sleep_time_get(int unit, phy_ctrl_t *pc, uint32 *value)
{
    uint16 data;
    phy_bcm542xx_direct_reg_read(unit, pc,
            PHY_BCM542XX_POWER_AUTO_DOWN_REG_OFFSET, &data);
   
    *value = (data & PHY_BCM542XX_POWER_AUTO_DOWN_REG_SLEEP_TIMER_SEL)
             ? PHY_BCM542XX_AUTO_PWRDWN_SLEEP_MAX
             : PHY_BCM542XX_AUTO_PWRDWN_SLEEP_MIN;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_bcm542xx_eee_enable
 * Purpose:
 *      Enable or disable EEE (Native)
 * Parameters:
 *      unit   - StrataSwitch unit #.
 *      port   - StrataSwitch port #.
 *      mode   - 0:Native EEE  or  1:AutogrEEEn
 *      enable - 1:Enable or 0:disable
 * Returns:
 *      SOC_E_NONE
 */
int
phy_bcm542xx_eee_enable(int unit, soc_port_t port, int mode, int enable)
{
    int                 dev_port;
    phy_ctrl_t         *pc;

    if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
        return ( SOC_E_UNAVAIL );
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);  /* Dev port numbers from 0-7 */

    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7  *\
    |  of the device. For example to enable autogrEEEn on port0,         |
    |  rdb register for port 4 0x808 need to be used.                    |
    \* Refer package configuration document.                            */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    if ( enable ) {
        soc_port_ability_t  abi = { 0 };
        /* Enable EEE 
         * 1. Enable EEE mode
         * 2. Advertise EEE auto-negotiation capabilities
         *    < Normal auto-negotiation properties >
         *    < Enable optional variable/fixed latency modes >  if AutogrEEEn
         * 3. Intiate auto-negotiation
         * 4. Enable AutogrEEEn / Native EEE
         */

        /* Enable EEE mode */
        SOC_IF_ERROR_RETURN(         /* 7.0x803D[15:14] */
                PHY_MODIFY_BCM542XX_EEE_803Dr(unit, pc, 0xc000, 0xc000) );
        /* Advertise EEE Auto Negotiation Capabilities */
        SOC_IF_ERROR_RETURN( phy_bcm542xx_ability_advert_get(unit, port, &abi) );
        abi.eee |= (SOC_PA_EEE_1GB_BASET | SOC_PA_EEE_100MB_BASETX);
        SOC_IF_ERROR_RETURN( phy_bcm542xx_ability_advert_set(unit, port, &abi) );
        /* Initiate Auto Negotiation */
        SOC_IF_ERROR_RETURN( phy_bcm542xx_autoneg_set(unit, port, TRUE) );

        if ( SOC_PHY_CONTROL_EEE_AUTO == mode ) {       /* AutogrEEEn mode */
                /***********************************************************************
                 * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
                 * To access TOP level registers use phy_id of port0. Override this ports 
                 */
                pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
                
                phy_bcm542xx_direct_reg_modify(unit, pc,
                                    PHY_BCM542XX_TOP_MISC_MII_BUFF_CNTL_PHYN(dev_port),
                                    PHY_BCM542XX_TOP_MISC_MII_BUFF_EN,
                                    PHY_BCM542XX_TOP_MISC_MII_BUFF_EN);
                /***********************************************************************
                 * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
                 * To access TOP level registers use phy_id of port0. Now restore back 
                 */
                pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
        }
        /* 
           Enable stat counters 
           Clear BIT14 to get lower 16bits of the counters 
        */
        SOC_IF_ERROR_RETURN(
                PHY_MODIFY_BCM542XX_EEE_STAT_CTRLr(unit, pc,
                            PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_EN,
                            PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_EN
                          | PHY_BCM542XX_EEE_STAT_CTRL_SATURATE_MODE) );

    } else {  /* enable == 0 */
        /* Disable EEE mode */
        SOC_IF_ERROR_RETURN(         /* 7.0x803D[15:14] */
                PHY_MODIFY_BCM542XX_EEE_803Dr(unit, pc, 0x0000, 0xc000) );
        phy_bcm542xx_autoneg_set(unit, port, 1);

        if ( SOC_PHY_CONTROL_EEE_AUTO == mode ) {       /* AutogrEEEn mode */
                /***********************************************************************
                 * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
                 * To access TOP level registers use phy_id of port0. Override this ports 
                 */
                pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
                
                phy_bcm542xx_direct_reg_modify(unit, pc,
                                    PHY_BCM542XX_TOP_MISC_MII_BUFF_CNTL_PHYN(dev_port),
                                    0x00,
                                    PHY_BCM542XX_TOP_MISC_MII_BUFF_EN);
                /***********************************************************************
                 * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
                 * To access TOP level registers use phy_id of port0. Now restore back 
                 */
                pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
        }
        /* Reset stat counters */
        SOC_IF_ERROR_RETURN(
                PHY_MODIFY_BCM542XX_EEE_STAT_CTRLr(unit, pc,
                            PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_RESET,
                            PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_RESET) );
        /* Disable stat counters */
        SOC_IF_ERROR_RETURN(
                PHY_MODIFY_BCM542XX_EEE_STAT_CTRLr(unit, pc, 0,
                            PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_RESET
                          | PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_EN) );
    }

    return SOC_E_NONE;
}

STATIC int
_phy_bcm542xx_get_eee_control_status(int unit, phy_ctrl_t *pc, uint32 *value)
{
    int     rv = SOC_E_NONE;
    uint16  temp16 = 0;

    rv = PHY_READ_BCM542XX_EEE_803Dr(unit, pc,&temp16);    /* 7.803d */
    *value = ((temp16 & 0xC000) == 0xC000) ? 1 : 0;
    return rv;
}

STATIC int
_phy_bcm542xx_get_auto_eee_control_status(int unit, soc_port_t port,
                                          uint32 *value)
{
    int          rv = SOC_E_NONE;
    uint16       temp16 = 0;
    int          dev_port;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    /* Dev port numbers from 0-7 */
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }
    
    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
    /* TOP_MISCREG_MII_BUFFER_CNTL_0 Reg., RDB 0x800+2n */
    rv = phy_bcm542xx_direct_reg_read(unit, pc,    /* bit[0] MII_BUFFER_ENABLE */
                     PHY_BCM542XX_TOP_MISC_MII_BUFF_CNTL_PHYN(dev_port), 
                     &temp16);

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
     * To access TOP level registers use phy_id of port0. Now restore back 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

    *value = (temp16 & PHY_BCM542XX_TOP_MISC_MII_BUFF_EN)? 1: 0;
    return rv;
}

/*
 * Function:
 *    _phy_bcm542xx_eee_counter_get
 * Purpose:
 *    Get the 32-bit value of EEE Statistic counter/timer
 *    CORE_ExpAA, ExpAB, ExpAC, ExpAD
 * Parameters:
 *                
 * Returns:
 */
STATIC int
_phy_bcm542xx_eee_counter_get(int unit, phy_ctrl_t *pc,
                              uint16 reg_addr, uint32 *value32)
{
    uint16  msb1, lsb1, msb2, lsb2;

#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
    PHY_BCM542XX_RDB_ACCESS_MODE(unit, pc);   /* set RDB register access mode */
#endif

    /* CORE_ExpAF - EEE Statistic counters ctrl/status, RDB 0x0af bit[14]     */
    /* STATISTIC_UPPER_16BITS_SEL=1 to get the upper 16-bit of counters first */
    SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_modify(unit, pc,
                                PHY_BCM542XX_EEE_STAT_CTRL_REG_OFFSET,
                                PHY_BCM542XX_EEE_STAT_CTRL_16U_BIT_SEL,
                                PHY_BCM542XX_EEE_STAT_CTRL_16U_BIT_SEL) );
    /* CORE_ExpAA/AB/AC/AD: EEE Statistic timer/counter, TxRx duration & event*/
    SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_read(unit, pc, reg_addr, &msb1) );    /* upper 16 bits */
    SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_read(unit, pc, reg_addr, &lsb1) );    /* lower 16 bits */
    if ( PHY_IS_BCM54210_220(pc) ) {
        *value32 = (((uint32) msb1) << 16) | lsb1;
    } else {
        /* Workaround for old version of 5428x/29x/220/210   */
        /* read the counter [CORE_ExpAA/AB/AC/AD] four times */
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_read(unit, pc, reg_addr, &msb2) );
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_rdb_reg_read(unit, pc, reg_addr, &lsb2) );
        /* form the 32-bit value by concatenating the upper & lower 16 bits   */
        /*                see the 'NOTE' section of RDB Reg.0x0AF spec        */
        /*                in BCM54220SE/5428x Programmer's Reference Guide    */
        *value32 = ( lsb1 & BIT(15) ) ? (((uint32) msb1) << 16) | lsb1
                                      : (((uint32) msb2) << 16) | lsb1;
    }

#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING
    PHY_BCM542XX_LEGACY_ACCESS_MODE(unit, pc); /* legacy register access mode */
#endif
    return (SOC_E_NONE);
}

/*
 * Function:
 *    phy_bcm542xx_eee_control_set
 * Purpose:
 *    Implements different 'setting' controls
 *    for EEE
 * Parameters:
 *    phy_id    - PHY's device address
 *    type      - type of control
 *    value     - helps decide what needs to be
 *                for the control type 'type'
 * Returns:
 */
int
phy_bcm542xx_eee_control_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value)
{
    int          rv = SOC_E_NONE;
    uint16       temp16 = 0;
    int          dev_port;
    phy_ctrl_t  *pc;

    if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
        return SOC_E_UNAVAIL;
    }
    pc = EXT_PHY_SW_STATE(unit, port);
    /* Dev port numbers from 0-7 */
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    switch ( type ) {
    case (SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD):
        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE
         * To access TOP level registers use phy_id of port0. Override this ports
         */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
        /* TOP_MISCREG_MII_BUFFER_CNTL_0 Reg., RDB 0x800+2n */
        /*     bit[10:8] WAIT_IFG_LENGTH */
        rv = phy_bcm542xx_direct_reg_modify(unit, pc,
                         PHY_BCM542XX_TOP_MISC_MII_BUFF_CNTL_PHYN(dev_port),
                         value << 8,
                         PHY_BCM542XX_TOP_MISC_MII_BUFF_WAIT_IFG_LEN );
        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
         * To access TOP level registers use phy_id of port0. Now restore back
         */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
        break;

    case (SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY):
        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE
         * To access TOP level registers use phy_id of port0. Override this ports
         */
        /* Fixed latency can have only 0 or 1 value  */
        if ((value != FALSE) && (value != TRUE)) {
            return SOC_E_PARAM;
        }
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
        temp16 = (value == FALSE) ? PHY_BCM542XX_TOP_MISC_MII_BUFF_VAR_LAT_EN : 0;

        /* TOP_MISCREG_MII_BUFFER_CNTL_0 Reg., RDB 0x800+2n */
        /*     bit[2] VARIABLE_LATENCY_EN */
        rv = phy_bcm542xx_direct_reg_modify(unit, pc,
                         PHY_BCM542XX_TOP_MISC_MII_BUFF_CNTL_PHYN(dev_port),
                         temp16,
                         PHY_BCM542XX_TOP_MISC_MII_BUFF_VAR_LAT_EN );
        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
         * To access TOP level registers use phy_id of port0. Now restore back
         */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

        /* Need to set MII buffer register as well : phy54682 ? */
        break;

    case (SOC_PHY_CONTROL_EEE_STATISTICS_CLEAR):
        if ( 1 == value ) {
            /* Enable stat counters, Soft reset the counters */
            SOC_IF_ERROR_RETURN(
                 PHY_MODIFY_BCM542XX_EEE_STAT_CTRLr(unit, pc,
                         PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_RESET
                       | PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_EN,
                         PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_RESET
                       | PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_EN) );
            /*Clear the counter reset bit*/
            SOC_IF_ERROR_RETURN(
                 PHY_MODIFY_BCM542XX_EEE_STAT_CTRLr(unit, pc, 0x0,
                         PHY_BCM542XX_EEE_STAT_CTRL_COUNTERS_RESET));
        }
        break;

    default:
        break;
    }  /* switch (type) */

    return rv;
}

/*
 * Function:
 *    phy_bcm542xx_eee_control_get
 * Purpose:
 *    Implements different 'get' controls 
 *    for EEE
 * Parameters:
 *    phy_id    - PHY's device address
 *    type      - type of control
 *    value     - gets the value for the 
 *                control 'type
 * Returns:
 *    SOC_E_xxxx            
 */
int
phy_bcm542xx_eee_control_get(int unit, soc_port_t port,
                             soc_phy_control_t type, uint32 *value)
{
    int          rv = SOC_E_NONE;
    uint16       temp16 = 0;
    int          dev_port;
    phy_ctrl_t  *pc;

    if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
        return SOC_E_UNAVAIL;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);  /* Dev port numbers from 0-7 */
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    switch ( type ) {
    case (SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD):
        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE
         * To access TOP level registers use phy_id of port0. Override this ports
         */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
        /* TOP_MISCREG_MII_BUFFER_CNTL_0 Reg., RDB 0x800+2n */
        /*     bit[10:8] WAIT_IFG_LENGTH */
        rv = phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_MISC_MII_BUFF_CNTL_PHYN(dev_port),
                         &temp16);
        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
         * To access TOP level registers use phy_id of port0. Now restore back
         */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

        *value = (temp16 & PHY_BCM542XX_TOP_MISC_MII_BUFF_WAIT_IFG_LEN) >> 8;
        break;

    case (SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY):
        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE
         * To access TOP level registers use phy_id of port0. Override this ports
         */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
        /* TOP_MISCREG_MII_BUFFER_CNTL_0 Reg., RDB 0x800+2n */
        /*     bit[2] VARIABLE_LATENCY_EN */
        rv = phy_bcm542xx_direct_reg_read(unit, pc,
                         PHY_BCM542XX_TOP_MISC_MII_BUFF_CNTL_PHYN(dev_port),
                         &temp16);
        /***********************************************************************
         * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
         * To access TOP level registers use phy_id of port0. Now restore back
         */
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

        *value = ((temp16 & PHY_BCM542XX_TOP_MISC_MII_BUFF_VAR_LAT_EN) == 0);
        break;

    case SOC_PHY_CONTROL_EEE_TRANSMIT_EVENTS:
        /* CORE_ExpAC - EEE Statistic loc lpi req 0_to_1 counter, RDB 0x0ac */
        rv = _phy_bcm542xx_eee_counter_get(unit, pc,
                         PHY_BCM542XX_EEE_STAT_TX_EVENTS_REG_OFFSET  , value);
        break;
    case SOC_PHY_CONTROL_EEE_TRANSMIT_DURATION:
        /* CORE_ExpAA - EEE Statistic timer 12hours lpi, RDB 0x0aa */
        rv = _phy_bcm542xx_eee_counter_get(unit, pc,
                         PHY_BCM542XX_EEE_STAT_TX_DURATION_REG_OFFSET, value);
        break;
    case SOC_PHY_CONTROL_EEE_RECEIVE_EVENTS:
        /* CORE_ExpAD - EEE Statistic rem lpi_req 0_to_1 counter, RDB 0x0ad */
        rv = _phy_bcm542xx_eee_counter_get(unit, pc,
                         PHY_BCM542XX_EEE_STAT_RX_EVENTS_REG_OFFSET  , value);
        break;
    case SOC_PHY_CONTROL_EEE_RECEIVE_DURATION:
        /* CORE_ExpAB - EEE Statistic timer 12hours local, RDB 0x0ab */
        rv = _phy_bcm542xx_eee_counter_get(unit, pc,
                         PHY_BCM542XX_EEE_STAT_RX_DURATION_REG_OFFSET, value);
        break;

    default:
        break;
    }  /* switch (type) */

    return rv;
}

/*
 * Function:
 *      phy_bcm542xx_control_set
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
phy_bcm542xx_control_set(int unit, soc_port_t port,
                         soc_phy_control_t type, uint32 value)
{
    int               rv   = SOC_E_NONE;
    phy_ctrl_t       *pc;
 
    pc = EXT_PHY_SW_STATE(unit, port);

    PHY_CONTROL_TYPE_CHECK(type);

    switch ( type ) {
    /* SyncE recovered clock controls */
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        rv = phy_bcm542xx_clock_enable_set(unit, pc,
                          PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK1_SHIFT,
                          value);
        break;
    case SOC_PHY_CONTROL_CLOCK_SECONDARY_ENABLE:
        rv = phy_bcm542xx_clock_enable_set(unit, pc,
                          PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK2_SHIFT,
                          value);
        break;
    case SOC_PHY_CONTROL_CLOCK_MODE_AUTO:
        rv = _phy_bcm542xx_clock_control_set(unit, pc,
                          PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_AUTO_CLK_DIS,
                          value);
        break;
    case SOC_PHY_CONTROL_CLOCK_AUTO_SECONDARY:
        rv = _phy_bcm542xx_clock_control_set(unit, pc,
                          PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_AUTO_SW_MODE,
                          value);
        break;
    case SOC_PHY_CONTROL_CLOCK_SOURCE:
    case SOC_PHY_CONTROL_CLOCK_FREQUENCY:
        return SOC_E_UNAVAIL;
        break;

    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        if (PHY_IS_BCM54240(pc)) {
            if ( PHY_COPPER_MODE(unit, port) ) {
                SOC_IF_ERROR_RETURN(
                    PHY_MODIFY_BCM542XX_MII_MISC_TESTr(unit, pc, (value) ? PHY_BCM542XX_MII_MISC_TEST_RMT_LB : 0,
                                            PHY_BCM542XX_MII_MISC_TEST_RMT_LB) );
                SOC_IF_ERROR_RETURN
                    (PHY_MODIFY_BCM542XX_MII_CTRLr(unit, pc, 1, PHY_BCM542XX_MII_CTRL_RST_AN));
            } else {/* MEDIUM_FIBER */
                if(value) {
                    SOC_IF_ERROR_RETURN(
                        PHY_WRITE_BCM542XX_AUTO_DETECT_MEDIUMr(unit, pc, 0x7800) );
                    SOC_IF_ERROR_RETURN(
                        PHY_WRITE_BCM542XX_MODE_CTRLr(unit, pc, 0x7F06) );
                    SOC_IF_ERROR_RETURN(                
                        PHY_WRITE_BCM542XX_MII_CTRLr(unit, pc, 0x5140) );
                } else { /* Diable loopback */
                    SOC_IF_ERROR_RETURN(
                        PHY_WRITE_BCM542XX_MII_CTRLr(unit, pc, 0x1140) );
                }
            }
        } else {
            return SOC_E_UNAVAIL;
        }
        break;

    /* power controls */
    case SOC_PHY_CONTROL_POWER:
        rv = phy_bcm542xx_power_mode_set(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
        phy_bcm542xx_power_auto_wake_time_set(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
        phy_bcm542xx_power_auto_sleep_time_set(unit, pc, value);
        break;
    /* port phy address setting */
    case SOC_PHY_CONTROL_PORT_PRIMARY:
        SOC_IF_ERROR_RETURN
                (soc_phyctrl_primary_set(unit, port, (soc_port_t)value));
        break;
    case SOC_PHY_CONTROL_PORT_OFFSET:
        SOC_IF_ERROR_RETURN
                (soc_phyctrl_offset_set(unit, port, (int)value));
        break;
    case SOC_PHY_CONTROL_SUPER_ISOLATE:
        SOC_IF_ERROR_RETURN
                (phy_bcm542xx_direct_reg_modify(unit, pc,
                          PHY_BCM542XX_POWER_MII_CTRL_REG_OFFSET,
                          (value) ? PHY_BCM542XX_POWER_MII_CTRL_SUPER_ISOLATE
                                  : 0,
                          PHY_BCM542XX_POWER_MII_CTRL_SUPER_ISOLATE) );
        break;
    case SOC_PHY_CONTROL_SOFT_RESET:
        SOC_IF_ERROR_RETURN
                (phy_bcm542xx_direct_reg_modify(unit, pc,
                          PHY_BCM542XX_SOFT_RESET_REG_OFFSET,
                          (value) ? PHY_BCM542XX_SOFT_RESET_ISSUE_RESET : 0,
                          PHY_BCM542XX_SOFT_RESET_ISSUE_RESET) );
        PHY_BCM542XX_DEFAULT_REG_ACCESS_MODE(unit, pc);
        break;

    /* EEE settings */
    case SOC_PHY_CONTROL_EEE:
        if(value == 1 )
        {
            /*First Disable Auto EEE*/
            SOC_IF_ERROR_RETURN
                (phy_bcm542xx_eee_enable(unit, port, SOC_PHY_CONTROL_EEE_AUTO, 0));
        }
        /*Enable/Disable Native EEE*/
        SOC_IF_ERROR_RETURN
                (phy_bcm542xx_eee_enable(unit, port, type, value));
        break;
    case SOC_PHY_CONTROL_EEE_AUTO:
        if(value == 1 )
        {
            /*First Disable Native  EEE*/
            SOC_IF_ERROR_RETURN
                (phy_bcm542xx_eee_enable(unit, port, SOC_PHY_CONTROL_EEE, 0));
        }
        /*Enable/Disable Auto EEE*/
        SOC_IF_ERROR_RETURN
                (phy_bcm542xx_eee_enable(unit, port, type, value));
        break;
    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
    case SOC_PHY_CONTROL_EEE_STATISTICS_CLEAR:
        SOC_IF_ERROR_RETURN
                (phy_bcm542xx_eee_control_set(unit, port, type, value));
        break;       
    case SOC_PHY_CONTROL_EEE_AUTO_BUFFER_LIMIT:
        /* Buffer limit modification is not allowed */
    case SOC_PHY_CONTROL_EEE_TRANSMIT_WAKE_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_WAKE_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_SLEEP_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_SLEEP_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_QUIET_TIME:
    case SOC_PHY_CONTROL_EEE_RECEIVE_QUIET_TIME:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_REFRESH_TIME:
        return SOC_E_UNAVAIL;

    default:  /* -- unrecognized PHY control commands -- */
        return SOC_E_UNAVAIL;
    } /* switch ( type ) */

    return rv;
}

/*
 * Function:
 *      phy_bcm542xx_control_get
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
phy_bcm542xx_control_get(int unit, soc_port_t port,
                         soc_phy_control_t type, uint32 *value)
{
    int          rv = SOC_E_NONE;
    phy_ctrl_t  *pc;
    soc_port_t   primary;
    uint16       value16;

    pc = EXT_PHY_SW_STATE(unit, port);
    
    if ( NULL == value ) {
        return SOC_E_PARAM;
    }
    
    PHY_CONTROL_TYPE_CHECK(type);

    switch ( type ) {
    /* power controls */
    case SOC_PHY_CONTROL_POWER:
        *value = pc->power_mode;
        break;
    case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
        phy_bcm542xx_power_auto_sleep_time_get(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
        phy_bcm542xx_power_auto_wake_time_get(unit, pc, value);
        break;
    /* port phy address setting */
    case SOC_PHY_CONTROL_PORT_PRIMARY:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_primary_get(unit, port, &primary));
        *value = (uint32) primary;
        break;
    case SOC_PHY_CONTROL_PORT_OFFSET:
        *value = (uint32) PHY_BCM542XX_DEV_PHY_SLICE(pc);
        break;

    /* SyncE recovered clock controls */
    case SOC_PHY_CONTROL_CLOCK_ENABLE:
        rv = phy_bcm542xx_clock_enable_get(unit, pc,
                          PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK1_SHIFT,
                          value);
        break;
    case SOC_PHY_CONTROL_CLOCK_SECONDARY_ENABLE:
        rv = phy_bcm542xx_clock_enable_get(unit, pc,
                          PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_REC_CLK2_SHIFT,
                          value);
        break;
    case SOC_PHY_CONTROL_CLOCK_MODE_AUTO:
        rv = _phy_bcm542xx_clock_control_get(unit, pc,
                    PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_AUTO_CLK_DIS, value);
        break;
    case SOC_PHY_CONTROL_CLOCK_AUTO_SECONDARY:
        rv = _phy_bcm542xx_clock_control_get(unit, pc,
                    PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_AUTO_SW_MODE, value);
        break;
    case SOC_PHY_CONTROL_CLOCK_SOURCE:
        rv = _phy_bcm542xx_clock_control_get(unit, pc,
                    PHY_BCM542XX_SYNCE_RECOVERY_CLK_REG_AUTO_SW_STS , value);
        *value = (0 == *value) ? SOC_PORT_PHY_CLOCK_SOURCE_PRIMARY
                               : SOC_PORT_PHY_CLOCK_SOURCE_SECONDARY;
        break;
    case SOC_PHY_CONTROL_SUPER_ISOLATE:
        SOC_IF_ERROR_RETURN
                (phy_bcm542xx_direct_reg_read(unit, pc,
                          PHY_BCM542XX_POWER_MII_CTRL_REG_OFFSET, &value16) );

        *value = ((value16) & PHY_BCM542XX_POWER_MII_CTRL_SUPER_ISOLATE)
                 ? TRUE : FALSE;
        break;

    /* EEE settings */
    case SOC_PHY_CONTROL_EEE:
        if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        rv = _phy_bcm542xx_get_eee_control_status(unit, pc, value);
        break;
    case SOC_PHY_CONTROL_EEE_AUTO:
        if ( ! PHY_FLAGS_TST(unit, port, PHY_FLAGS_EEE_CAPABLE) ) {
            rv = SOC_E_UNAVAIL;
            break;
        }
        rv = _phy_bcm542xx_get_auto_eee_control_status(unit, port, value);
        break;
    case SOC_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY:
    case SOC_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_EVENTS:
    case SOC_PHY_CONTROL_EEE_TRANSMIT_DURATION:
    case SOC_PHY_CONTROL_EEE_RECEIVE_EVENTS:
    case SOC_PHY_CONTROL_EEE_RECEIVE_DURATION:
        rv = phy_bcm542xx_eee_control_get(unit, port, type, value);
        break;

    default:  /* -- unrecognized PHY control commands -- */
        return SOC_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      phy_bcm542xx_probe
 * Purpose:
 *      Complement the generic phy probe routine to identify this phy when its
 *      phy id0 and id1 is same as some other phy's.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE, SOC_E_NOT_FOUND and SOC_E_<error>
 */
STATIC int 
phy_bcm542xx_probe(int unit, phy_ctrl_t *pc) {
    int  oui = 0, model = 0, rev = 0, rv = SOC_E_NONE;
    soc_phy_info_t *pi = &SOC_PHY_INFO(unit, pc->port);

    if ( SOC_E_NONE != (rv = _phy_bcm542xx_model_rev_get(unit, pc,
                                                   &oui, &model, &rev)) ) {
        return SOC_E_FAIL;
    }
    pc->interface = SOC_PORT_IF_SGMII;  /* 54210/220/240/280/290/294/190/180/140 */

    /* determine the chip model */
    switch ( model ) {

    case PHY_BCM54210_MODEL:
    case PHY_BCM54220_MODEL:
    case PHY_BCM54280_MODEL:
        /*passthru*/
    case PHY_BCM5428X_MODEL:
        /*passthru*/
    case PHY_BCM54240_MODEL:
        break;

    case (PHY_BCM54282_MODEL | PHY_BCM54285_MODEL):
        pi->phy_name = (rev & 0x8) ? "BCM54282" : "BCM54285";
        pc->interface = SOC_PORT_IF_QSGMII;
        break;

    case (PHY_BCM54290_MODEL | PHY_BCM54294_MODEL):
        pi->phy_name = (rev & 0x8) ? "BCM54294" : "BCM54290";
        break;

    case (PHY_BCM54292_MODEL | PHY_BCM54295_MODEL | PHY_BCM54296_MODEL):
        if ( rev & 0x8 ) {
            pi->phy_name = (0x8 & _phy_bcm542xx_hidden_rev_num_get(unit, pc))
                         ? "BCM54296" : "BCM54295";
        } else {
            pi->phy_name = "BCM54292";
        }
        pc->interface = SOC_PORT_IF_QSGMII;
        break;

    case (PHY_BCM54190_MODEL | PHY_BCM54194_MODEL):
        pi->phy_name = (rev & 0x8) ? "BCM54194" : "BCM54190";
        break;

    case (PHY_BCM54180_MODEL | PHY_BCM54140_MODEL):
        pi->phy_name = (rev & 0x8) ? "BCM54140" : "BCM54180";
        break;

    case (PHY_BCM54182_MODEL | PHY_BCM54185_MODEL):
        if ( rev & 0x8 ) {
            pi->phy_name = (0x8 & _phy_bcm542xx_hidden_rev_num_get(unit, pc))
                         ? "BCM54186" : "BCM54185";
        } else {
            pi->phy_name = "BCM54182";
        }
        pc->interface = SOC_PORT_IF_QSGMII;
        break;

    case (PHY_BCM54192_MODEL | PHY_BCM54195_MODEL):
        if ( rev & 0x8 ) {
            pi->phy_name = (0x8 & _phy_bcm542xx_hidden_rev_num_get(unit, pc))
                         ? "BCM54196" : "BCM54195";
        } else {
            pi->phy_name = "BCM54192";
        }
        pc->interface = SOC_PORT_IF_QSGMII;
        break;

    default :
        return SOC_E_NOT_FOUND;
    }
#ifdef INCLUDE_PLP_IMACSEC
    pc->size = sizeof(PHY_BCM542XX_DEV_DESC_t) + sizeof(bcm_plp_base_t_sec_phy_access_t);
#else
    pc->size = sizeof(PHY_BCM542XX_DEV_DESC_t);
#endif
    return rv;
}

/*
 * Function:
 *      phy_bcm542xx_link_up
 * Purpose:
 *      Link up handler
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
phy_bcm542xx_link_up(int unit, soc_port_t port)
{
    phy_ctrl_t* pc = EXT_PHY_SW_STATE(unit, port);
    int speed = 0, duplex = TRUE;
    /* get the speed and duplex status*/
    SOC_IF_ERROR_RETURN( phy_bcm542xx_speed_get( unit, port, &speed ) );
    SOC_IF_ERROR_RETURN( phy_bcm542xx_duplex_get(unit, port, &duplex) );

    /* Notify interface to internal PHY */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventInterface,
                    IS_GMII_PORT(unit, port) ? SOC_PORT_IF_GMII : SOC_PORT_IF_SGMII));

    /* Situation observed with Saber devices XGXS16g1l+BCM54640E
       Linkscan is not updating speed for internal phy xgxs16g1l. 
       It was observed that when speed is set to 100M the int phy 
       is still set to 1G and traffic does not flow through untill 
       int phy speed is also update to 100M. This applies to other 
       speeds as well. 
       Explicitly send speed and duplex notification to int phy from ext phy.
    */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventDuplex, duplex));
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventSpeed , speed) );

    /* set QSGMII forced mode if not AutoNeg */
    if ( (SOC_PORT_IF_QSGMII == pc->interface ) &&
         (! PHY_BCM542XX_IS_SYS_SIDE_AUTONEG(pc)) ) {
        uint16  reg_val = (1000 == speed) ? PHY_BCM542XX_SGMII_CONTROL_FORCED_1000 :
                          ( 100 == speed) ? PHY_BCM542XX_SGMII_CONTROL_FORCED_100  :
                                            PHY_BCM542XX_SGMII_CONTROL_FORCED_10   ;

        pc->phy_id = PHY_BCM542XX_QSGMII_DEV_ADDR(pc);    /* get QSGMII PHY addr */
        /* QSGMII_CONTROL register (BAR=0x0, RegAD=0x0) */
        SOC_IF_ERROR_RETURN( phy_bcm542xx_qsgmii_reg_write(unit, pc,
                                  (int) PHY_BCM542XX_DEV_PHY_SLICE(pc) /*dev_port*/,
                                        PHY_BCM542XX_SGMII_CONTROL_REG_OFFSET,
                                        PHY_BCM542XX_SGMII_CONTROL_REG_OFFSET,
                                        reg_val) );
        pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);     /* resume PHY address */
    }

#ifdef INCLUDE_PLP_IMACSEC
    /* Check if MACSEC_SWITCH_SIDE_POLICY is DUPLEX_GATEWAY mode 
     * Half duplex mode is supported only in 10 Mbps and 100 Mbps*/
    if  (pc->macsec_enable) {
        int duplex;
        /* Duplex Gateway mode for supporting half-duplex-free switches:   *\
        |* When line-side PHY lilnks up at half-duplex,                    *|
        \* set half-duplex to line-side MAC, full-duplex to switch-side MAC*/

        /* get the duplex status from line side */
        SOC_IF_ERROR_RETURN(
            phy_bcm542xx_duplex_get(unit, port, &duplex) );
        if ( pc->macsec_switch_fixed_speed  == speed  &&
            pc->macsec_switch_fixed_duplex == duplex) {
            return SOC_E_NONE;    /* speed/duplex unchanged */
        }

        pc->macsec_switch_fixed_speed  = speed;
        pc->macsec_switch_fixed_duplex = duplex;   /* Change the duplex mode to half duplex */
        /* Config duplex Gateway parameters accordingly */
        SOC_IF_ERROR_RETURN(
                phy_unimac_switch_side_set_params(unit,port,
                              pc->macsec_mac_policy,
                              speed, duplex, pc->macsec_switch_fixed_pause) );
        /* under DUPLEX_GATEWAY mode, always Full-Duplex on system side */
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventDuplex, TRUE));
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_oam_config_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy_bcm542xx_oam_config_set(int unit, soc_port_t port, soc_port_config_phy_oam_t *conf) {
    uint16  data = 0, mask = 0;
    uint16 dm_tx_cntl_data = 0, dm_rx_cntl_data = 0;
    phy_ctrl_t *pc;
    soc_port_phy_timesync_config_t timesync_conf;
    
    sal_memset(&timesync_conf, 0, sizeof(soc_port_phy_timesync_config_t));

    if (NULL == conf) {
        return SOC_E_PARAM;
    }
   
    pc = EXT_PHY_SW_STATE(unit, port);

    /* 1. en_1588_softrst */ 
    /* 2. en_RGMII_Timing_RGMII */
    
    /* Setup timesync 1588 */ 
    timesync_conf.flags = SOC_PORT_PHY_TIMESYNC_ENABLE;
    phy_bcm542xx_timesync_config_set(unit, port, &timesync_conf);
    
    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
  
    /* TX: Delay measurement mode */
    switch( conf->tx_dm_config.mode ) {
        case SOC_PORT_CONFIG_PHY_OAM_DM_Y1731:
            dm_tx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_TX_CNTL_EN;
            if ( conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) {
                dm_tx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_TX_CNTL_Y1731_TS_SEL;
            }   
            break;
        case SOC_PORT_CONFIG_PHY_OAM_DM_BHH:
            data = 0;
            mask = 0;
            if (!(conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE) ) {
                data |= PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_CW_DIS;
            }    
            if ( conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_ENTROPY_ENABLE ) {
                data |= PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_ENTROPY_EN;
            }   
            data |=  PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_EN;
            mask =  PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_EN
                    | PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_CW_DIS
                    | PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_ENTROPY_EN;
            phy_bcm542xx_direct_reg_modify(unit, pc,
                    PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_OFFSET,
                    data, mask);

            if ( conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) {
                dm_tx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_TX_CNTL_BHH_TS_SEL;
            }   
            break;
        case SOC_PORT_CONFIG_PHY_OAM_DM_IETF:
            data = 0;
            mask = 0;
            if (!(conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE) ) {
                data |= PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_CW_DIS;
            }    
            if ( conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_ENTROPY_ENABLE ) {
                data |= PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_ENTROPY_EN;
            }   
            data |=  PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_EN;
            mask =  PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_EN
                    | PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_CW_DIS
                    | PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_ENTROPY_EN;
            phy_bcm542xx_direct_reg_modify(unit, pc,
                    PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_OFFSET,
                    data, mask);

            dm_tx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_TX_CNTL_IETF_EN;
            if ( conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) {
                /* Same bit as for BHH */
                dm_tx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_TX_CNTL_BHH_TS_SEL;
            }   
            break;
        default:
            if ( conf->tx_dm_config.mode == 0) {
                break;
            }
            /********************************************************
             * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
             * To access TOP level registers use phy_id of port0. 
               Now restore back 
             */
            pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
            return SOC_E_CONFIG;
    }

    /* RX: Delay measurement mode */
    switch(conf->rx_dm_config.mode) {
        case SOC_PORT_CONFIG_PHY_OAM_DM_Y1731:
            dm_rx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_RX_CNTL_EN;
            if ( conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) {
                dm_rx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_RX_CNTL_Y1731_TS_SEL;
            }   
            break;
        case SOC_PORT_CONFIG_PHY_OAM_DM_BHH:
            data = 0;
            mask = 0;
            if (!(conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE) ) {
                data |= PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_CW_DIS;
            }    
            if ( conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_ENTROPY_ENABLE ) {
                data |= PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_ENTROPY_EN;
            }   
            data |=  PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_EN;
            mask =  PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_EN
                    | PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_CW_DIS
                    | PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_ENTROPY_EN;
            phy_bcm542xx_direct_reg_modify(unit, pc,
                    PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_OFFSET,
                    data, mask);
            
            if ( conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) {
                dm_rx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_RX_CNTL_BHH_TS_SEL;
            }   
            break;
        case SOC_PORT_CONFIG_PHY_OAM_DM_IETF:
            data = 0;
            mask = 0;
            if (!(conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE) ) {
                data |= PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_CW_DIS;
            }    
            if ( conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_ENTROPY_ENABLE ) {
                data |= PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_ENTROPY_EN;
            }   
            data |=  PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_EN;
            mask =  PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_EN
                    | PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_CW_DIS
                    | PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_ENTROPY_EN;
            phy_bcm542xx_direct_reg_modify(unit, pc,
                    PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_OFFSET,
                    data, mask);

            dm_rx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_RX_CNTL_IETF_EN;
            if ( conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) {
                /* Same bit as for BHH */
                dm_rx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_RX_CNTL_BHH_TS_SEL;
            }   
            break;
        default:
            if ( conf->rx_dm_config.mode == 0) {
                break;
            }
            /********************************************************
             * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
             * To access TOP level registers use phy_id of port0. 
               Now restore back 
             */
            pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
            return SOC_E_CONFIG;
    }    

    /* Enable the 1588 counter includes the delay measurement packet */   
   dm_tx_cntl_data |= (dm_tx_cntl_data)? PHY_BCM542XX_TOP_1588_DM_TX_CNTL_1588_CNT_EN : dm_tx_cntl_data;
   dm_rx_cntl_data |= (dm_rx_cntl_data)? PHY_BCM542XX_TOP_1588_DM_TX_CNTL_1588_CNT_EN : dm_rx_cntl_data;

   if ( conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE ) {
       dm_tx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_TX_CNTL_MAC_EN;
   }    
   if ( conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE ) {
       dm_tx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_TX_CNTL_CW_EN;
   }   
   if ( conf->tx_dm_config.flags & SOC_PORT_PHY_OAM_DM_ENTROPY_ENABLE ) {
       dm_tx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_TX_CNTL_ENTROPY_EN;
   }   

   if ( conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE ) {
       dm_rx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_RX_CNTL_MAC_EN;
   }    
   if ( conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE ) {
       dm_rx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_RX_CNTL_CW_EN;
   }   
   if ( conf->rx_dm_config.flags & SOC_PORT_PHY_OAM_DM_ENTROPY_ENABLE ) {
       dm_rx_cntl_data |= PHY_BCM542XX_TOP_1588_DM_RX_CNTL_ENTROPY_EN;
   }   

   mask = PHY_BCM542XX_TOP_1588_DM_TX_CNTL_EN 
          | PHY_BCM542XX_TOP_1588_DM_TX_CNTL_MAC_EN 
          | PHY_BCM542XX_TOP_1588_DM_TX_CNTL_CW_EN 
          | PHY_BCM542XX_TOP_1588_DM_TX_CNTL_ENTROPY_EN 
          | PHY_BCM542XX_TOP_1588_DM_TX_CNTL_Y1731_TS_SEL 
          | PHY_BCM542XX_TOP_1588_DM_TX_CNTL_BHH_TS_SEL  
          | PHY_BCM542XX_TOP_1588_DM_TX_CNTL_IETF_EN 
          | PHY_BCM542XX_TOP_1588_DM_TX_CNTL_1588_CNT_EN;

   phy_bcm542xx_direct_reg_modify(unit, pc,
           PHY_BCM542XX_TOP_1588_DM_TX_CNTL_REG_OFFSET,
           dm_tx_cntl_data, mask);
   
   mask = PHY_BCM542XX_TOP_1588_DM_RX_CNTL_EN 
          | PHY_BCM542XX_TOP_1588_DM_RX_CNTL_MAC_EN 
          | PHY_BCM542XX_TOP_1588_DM_RX_CNTL_CW_EN 
          | PHY_BCM542XX_TOP_1588_DM_RX_CNTL_ENTROPY_EN 
          | PHY_BCM542XX_TOP_1588_DM_RX_CNTL_Y1731_TS_SEL 
          | PHY_BCM542XX_TOP_1588_DM_RX_CNTL_BHH_TS_SEL  
          | PHY_BCM542XX_TOP_1588_DM_RX_CNTL_IETF_EN 
          | PHY_BCM542XX_TOP_1588_DM_RX_CNTL_1588_CNT_EN;
   
   phy_bcm542xx_direct_reg_modify(unit, pc,
           PHY_BCM542XX_TOP_1588_DM_RX_CNTL_REG_OFFSET,
           dm_rx_cntl_data, mask);

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
     * To access TOP level registers use phy_id of port0. Now restore back 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);

    /* Save the config */
    sal_memcpy(PHY_BCM542XX_DEV_OAM_CONFIG_PTR(pc), conf, sizeof(soc_port_config_phy_oam_t));

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_oam_config_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy_bcm542xx_oam_config_get(int unit, soc_port_t port, soc_port_config_phy_oam_t *conf) {
    uint16 dm_tx_cntl_data = 0, dm_rx_cntl_data = 0;
    uint16 mpls_tx_cntl_data = 0, mpls_rx_cntl_data = 0;
    phy_ctrl_t *pc;
   
    sal_memset(conf, 0, sizeof(soc_port_config_phy_oam_t));

    pc = EXT_PHY_SW_STATE(unit, port);

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);
   
    /* Read config registers */ 
    phy_bcm542xx_direct_reg_read(unit, pc,
            PHY_BCM542XX_TOP_1588_DM_TX_CNTL_REG_OFFSET,
            &dm_tx_cntl_data);
    phy_bcm542xx_direct_reg_read(unit, pc,
            PHY_BCM542XX_TOP_1588_DM_RX_CNTL_REG_OFFSET,
            &dm_rx_cntl_data);
    phy_bcm542xx_direct_reg_read(unit, pc,
            PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_OFFSET,
            &mpls_tx_cntl_data);
    phy_bcm542xx_direct_reg_read(unit, pc,
            PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_OFFSET,
            &mpls_rx_cntl_data);

    /* Tx Config Mode */
    if (dm_tx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_EN ) {
        conf->tx_dm_config.mode = SOC_PORT_CONFIG_PHY_OAM_DM_Y1731;
        if (dm_tx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_Y1731_TS_SEL) {
            conf->tx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_TS_FORMAT;
        }     
    } else if ((dm_tx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_IETF_EN) 
            && (mpls_tx_cntl_data & PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_EN)){
        conf->tx_dm_config.mode = SOC_PORT_CONFIG_PHY_OAM_DM_IETF;
        if (dm_tx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_BHH_TS_SEL) {
            conf->tx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_TS_FORMAT;
        }     
    } else if (mpls_tx_cntl_data & PHY_BCM542XX_TOP_1588_MPLS_TX_CNTL_REG_MPLS_EN ) {
        conf->tx_dm_config.mode = SOC_PORT_CONFIG_PHY_OAM_DM_BHH;
        if (dm_tx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_BHH_TS_SEL) {
            conf->tx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_TS_FORMAT;
        }     
    }

    /* Rx Config Mode */
    if (dm_rx_cntl_data & PHY_BCM542XX_TOP_1588_DM_RX_CNTL_EN ) {
        conf->rx_dm_config.mode = SOC_PORT_CONFIG_PHY_OAM_DM_Y1731;
        if (dm_rx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_Y1731_TS_SEL) {
            conf->rx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_TS_FORMAT;
        }     
    } else if ((dm_rx_cntl_data & PHY_BCM542XX_TOP_1588_DM_RX_CNTL_IETF_EN) 
            && (mpls_rx_cntl_data & PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_EN)){
        conf->rx_dm_config.mode = SOC_PORT_CONFIG_PHY_OAM_DM_IETF;
        if (dm_rx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_BHH_TS_SEL) {
            conf->rx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_TS_FORMAT;
        }     
    } else if (mpls_rx_cntl_data & PHY_BCM542XX_TOP_1588_MPLS_RX_CNTL_REG_MPLS_EN ) {
        conf->rx_dm_config.mode = SOC_PORT_CONFIG_PHY_OAM_DM_BHH;
        if (dm_rx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_BHH_TS_SEL) {
            conf->rx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_TS_FORMAT;
        }     
    }
    
    /* Tx Config Flags */
    if (dm_tx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_MAC_EN ) {
        conf->tx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE;
    }    
    if (dm_tx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_CW_EN ) {
        conf->tx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE; 
    }     
    if (dm_tx_cntl_data & PHY_BCM542XX_TOP_1588_DM_TX_CNTL_ENTROPY_EN ) {
        conf->tx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_ENTROPY_ENABLE;
    }     

    /* Rx Config Flags */
    if (dm_rx_cntl_data & PHY_BCM542XX_TOP_1588_DM_RX_CNTL_MAC_EN ) {
        conf->rx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE;
    }    
    if (dm_rx_cntl_data & PHY_BCM542XX_TOP_1588_DM_RX_CNTL_CW_EN ) {
        conf->rx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE; 
    }     
    if (dm_rx_cntl_data & PHY_BCM542XX_TOP_1588_DM_RX_CNTL_ENTROPY_EN ) {
        conf->rx_dm_config.flags |= SOC_PORT_PHY_OAM_DM_ENTROPY_ENABLE;
    }     

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
     * To access TOP level registers use phy_id of port0. Now restore back 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
    /* Save the config */
    sal_memcpy(PHY_BCM542XX_DEV_OAM_CONFIG_PTR(pc), conf, sizeof(soc_port_config_phy_oam_t));
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_oam_control_set
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy_bcm542xx_oam_control_set(int unit, soc_port_t port, soc_port_control_phy_oam_t type, uint64 value)
{
    uint16  dev_port = 0;
    phy_ctrl_t *pc = NULL;
    soc_port_config_phy_oam_t *config = NULL;
    soc_port_config_phy_oam_dm_mode_t tx_mode = 0;
    soc_port_config_phy_oam_dm_mode_t rx_mode = 0;
    uint32 tx_flags;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Dev port numbers from 0-7 */
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }
    
   
    /* Config saved during config _set or read during config _get */ 
    config = PHY_BCM542XX_DEV_OAM_CONFIG_PTR(pc); 
    tx_mode = config->tx_dm_config.mode;
    tx_flags = config->tx_dm_config.flags;
    rx_mode = config->rx_dm_config.mode;
    
    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

    switch(type) {
        case SOC_PORT_CONTROL_PHY_OAM_DM_TX_ETHERTYPE:
            switch(tx_mode) {
                case SOC_PORT_CONFIG_PHY_OAM_DM_Y1731:
                    if (tx_flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) { /*NTP */
                        phy_bcm542xx_direct_reg_write(unit, pc,
                                PHY_BCM542XX_TOP_1588_DM_ETHTYPE2_REG_OFFSET,
                                (COMPILER_64_LO(value) & 0xffff));
                    } else { /*PTP */
                        phy_bcm542xx_direct_reg_write(unit, pc,
                                PHY_BCM542XX_TOP_1588_DM_ETHTYPE1_REG_OFFSET,
                                (COMPILER_64_LO(value) & 0xffff));
                    }
                    break;
                case SOC_PORT_CONFIG_PHY_OAM_DM_BHH:
                    if (tx_flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) { /*NTP */
                        phy_bcm542xx_direct_reg_write(unit, pc,
                                PHY_BCM542XX_TOP_1588_DM_ETHTYPE4_REG_OFFSET,
                                (COMPILER_64_LO(value) & 0xffff));
                    } else { /*PTP */
                        phy_bcm542xx_direct_reg_write(unit, pc,
                                PHY_BCM542XX_TOP_1588_DM_ETHTYPE3_REG_OFFSET,
                                (COMPILER_64_LO(value) & 0xffff));
                    }
                    break;
                case SOC_PORT_CONFIG_PHY_OAM_DM_IETF:
                    phy_bcm542xx_direct_reg_write(unit, pc,
                            PHY_BCM542XX_TOP_1588_DM_ETHTYPE9_REG_OFFSET,
                            (COMPILER_64_LO(value) & 0xffff));
                    break;
                default:
                    /********************************************************
                     * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
                     * To access TOP level registers use phy_id of port0. 
                     Now restore back 
                     */
                    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
                    return SOC_E_CONFIG;    
            }

            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_RX_ETHERTYPE:
            switch(rx_mode) {
                case SOC_PORT_CONFIG_PHY_OAM_DM_Y1731:
                    phy_bcm542xx_direct_reg_write(unit, pc,
                            PHY_BCM542XX_TOP_1588_DM_ETHTYPE5_REG_OFFSET,
                            (COMPILER_64_LO(value) & 0xffff));
                    break;
                case SOC_PORT_CONFIG_PHY_OAM_DM_BHH:
                    phy_bcm542xx_direct_reg_write(unit, pc,
                            PHY_BCM542XX_TOP_1588_DM_ETHTYPE8_REG_OFFSET,
                            (COMPILER_64_LO(value) & 0xffff));
                    break;
                case SOC_PORT_CONFIG_PHY_OAM_DM_IETF:
                    phy_bcm542xx_direct_reg_write(unit, pc,
                            PHY_BCM542XX_TOP_1588_DM_ETHTYPE10_REG_OFFSET,
                            (COMPILER_64_LO(value) & 0xffff));
                    break;
                default:
                    /********************************************************
                     * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
                     * To access TOP level registers use phy_id of port0. 
                     Now restore back 
                     */
                    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
                    return SOC_E_CONFIG;    
            }
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_TX_PORT_MAC_ADDRESS_INDEX:
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_CTL_0_REG_OFFSET,
                    (COMPILER_64_LO(value) & 0x0003) << (dev_port*2));
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_RX_PORT_MAC_ADDRESS_INDEX:
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_CTL_1_REG_OFFSET,
                    (COMPILER_64_LO(value) & 0x0003) << (dev_port*2));
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_1:
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L1_0_REG_OFFSET,
                    (COMPILER_64_LO(value) & 0xffff));
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L1_1_REG_OFFSET,
                    (COMPILER_64_LO(value) >> 16) & 0xffff);
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L1_2_REG_OFFSET,
                    (COMPILER_64_HI(value) & 0xffff));
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_2:
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L2_0_REG_OFFSET,
                    (COMPILER_64_LO(value) & 0xffff));
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L2_1_REG_OFFSET,
                    (COMPILER_64_LO(value) >> 16) & 0xffff);
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L2_2_REG_OFFSET,
                    (COMPILER_64_HI(value) & 0xffff));
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_3:
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L3_0_REG_OFFSET,
                    (COMPILER_64_LO(value) & 0xffff));
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L3_1_REG_OFFSET,
                    (COMPILER_64_LO(value) >> 16) & 0xffff);
            phy_bcm542xx_direct_reg_write(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L3_2_REG_OFFSET,
                    (COMPILER_64_HI(value) & 0xffff));
            break;
        default:
            /********************************************************
             * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
             * To access TOP level registers use phy_id of port0. 
               Now restore back 
             */
            pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
            return SOC_E_PARAM;
    }     
    

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
     * To access TOP level registers use phy_id of port0. Now restore back 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
                
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_bcm542xx_oam_control_get
 * Purpose:
 *
 * Parameters:
 *
 * Returns:
 */
int
phy_bcm542xx_oam_control_get(int unit, soc_port_t port, soc_port_control_phy_oam_t type, uint64 *value) {
    uint16  dev_port = 0;
    phy_ctrl_t *pc = NULL;
    soc_port_config_phy_oam_t *config = NULL;
    uint16 value01 = 0, value02 = 0, value03 = 0;
    soc_port_config_phy_oam_dm_mode_t tx_mode = 0, rx_mode = 0;
    uint32 tx_flags;

    pc = EXT_PHY_SW_STATE(unit, port);

    /* Dev port numbers from 0-7 */
    dev_port = PHY_BCM542XX_DEV_PHY_SLICE(pc);
    
    /* For BCM54294/296 package the logical ports p0-p3 are on port4-7 
       of the device. For example to enable autogrEEEn on port0, 
       rdb register for port 4 0x808 need to be used. 
       Refer package configuration document.
     */
    if ( PHY_BCM542XX_IS_REAR_HALF(pc) ) {
        dev_port += 4;
    }

    /* Config saved during config _set */ 
    config = PHY_BCM542XX_DEV_OAM_CONFIG_PTR(pc); 
    tx_mode = config->tx_dm_config.mode;
    tx_flags = config->tx_dm_config.flags;
    rx_mode = config->rx_dm_config.mode;

    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> OVERRIDE  
     * To access TOP level registers use phy_id of port0. Override this ports 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_BASE(pc);

    switch(type) {
        case SOC_PORT_CONTROL_PHY_OAM_DM_TX_ETHERTYPE:
            switch(tx_mode) {
                case SOC_PORT_CONFIG_PHY_OAM_DM_Y1731:
                    if (tx_flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) { /*NTP */
                        phy_bcm542xx_direct_reg_read(unit, pc,
                                PHY_BCM542XX_TOP_1588_DM_ETHTYPE2_REG_OFFSET,
                                &value01);
                    } else { /*PTP */
                        phy_bcm542xx_direct_reg_read(unit, pc,
                                PHY_BCM542XX_TOP_1588_DM_ETHTYPE1_REG_OFFSET,
                                &value01);
                    }
                    break;
                case SOC_PORT_CONFIG_PHY_OAM_DM_BHH:
                    if (tx_flags & SOC_PORT_PHY_OAM_DM_TS_FORMAT) { /*NTP */
                        phy_bcm542xx_direct_reg_read(unit, pc,
                                PHY_BCM542XX_TOP_1588_DM_ETHTYPE4_REG_OFFSET,
                                &value01);
                    } else { /*PTP */
                        phy_bcm542xx_direct_reg_read(unit, pc,
                                PHY_BCM542XX_TOP_1588_DM_ETHTYPE3_REG_OFFSET,
                                &value01);
                    }
                    break;
                case SOC_PORT_CONFIG_PHY_OAM_DM_IETF:
                    phy_bcm542xx_direct_reg_read(unit, pc,
                            PHY_BCM542XX_TOP_1588_DM_ETHTYPE9_REG_OFFSET,
                                &value01);
                    break;
                default:
                    break;    
            }
 
            COMPILER_64_SET((*value), 0,  (uint32)value01);

            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_RX_ETHERTYPE:
            switch(rx_mode) {
                case SOC_PORT_CONFIG_PHY_OAM_DM_Y1731:
                    phy_bcm542xx_direct_reg_read(unit, pc,
                            PHY_BCM542XX_TOP_1588_DM_ETHTYPE5_REG_OFFSET,
                                &value01);
                    break;
                case SOC_PORT_CONFIG_PHY_OAM_DM_BHH:
                    phy_bcm542xx_direct_reg_read(unit, pc,
                            PHY_BCM542XX_TOP_1588_DM_ETHTYPE8_REG_OFFSET,
                                &value01);
                    break;
                case SOC_PORT_CONFIG_PHY_OAM_DM_IETF:
                    phy_bcm542xx_direct_reg_read(unit, pc,
                            PHY_BCM542XX_TOP_1588_DM_ETHTYPE10_REG_OFFSET,
                                &value01);
                    break;
                default:
                    break;    
            }
            COMPILER_64_SET((*value), 0, (uint32)value01);
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_TX_PORT_MAC_ADDRESS_INDEX:
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_CTL_0_REG_OFFSET,
                    &value01);
            COMPILER_64_SET((*value), 0, ((uint32)value01 >> (dev_port*2)) & 0x3);
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_RX_PORT_MAC_ADDRESS_INDEX:
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_CTL_1_REG_OFFSET,
                    &value01);
            COMPILER_64_SET((*value), 0, ((uint32)value01 >> (dev_port*2)) & 0x3);
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_1:
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L1_0_REG_OFFSET,
                    &value01);
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L1_1_REG_OFFSET,
                    &value02);
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L1_2_REG_OFFSET,
                    &value03);
            COMPILER_64_SET((*value), (uint32)value03, 
                             ((uint32)value02 << 16) | ((uint32)value01));
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_2:
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L2_0_REG_OFFSET,
                    &value01);
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L2_1_REG_OFFSET,
                    &value02);
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L2_2_REG_OFFSET,
                    &value03);
            COMPILER_64_SET((*value), (uint32)value03, 
                             ((uint32)value02 << 16) | ((uint32)value01));
            break;
        case SOC_PORT_CONTROL_PHY_OAM_DM_MAC_ADDRESS_3:
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L3_0_REG_OFFSET,
                    &value01);
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L3_1_REG_OFFSET,
                    &value02);
            phy_bcm542xx_direct_reg_read(unit, pc,
                    PHY_BCM542XX_TOP_1588_DM_MAC_L3_2_REG_OFFSET,
                    &value03);
            COMPILER_64_SET((*value), (uint32)value03, 
                             ((uint32)value02 << 16) | ((uint32)value01));
            break;
        default:
            /********************************************************
             * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
             * To access TOP level registers use phy_id of port0. 
               Now restore back 
             */
            pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
            return SOC_E_PARAM;
    }
    /***********************************************************************
     * NOTE: TOP LEVEL REGISTER ACCESS -> RESTORE
     * To access TOP level registers use phy_id of port0. Now restore back 
     */
    pc->phy_id = PHY_BCM542XX_DEV_PHY_ID_ORIG(pc);
    
    return SOC_E_NONE;
}

#ifdef BROADCOM_DEBUG

int
phy_bcm542xx_rdb_reg_set(int unit, soc_port_t port, int rdb, int value){
    phy_ctrl_t *pc = NULL;
    uint16 valuer;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_bcm542xx_direct_reg_write(unit, pc,(uint16)rdb, (uint16)(value & 0xffff));
    phy_bcm542xx_direct_reg_read(unit, pc,(uint16)rdb, &valuer);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit, "u=%d p=%d rdb=0x%04x value read back=0x%04x\n"),
              unit, port, (uint16) rdb, valuer));
    return SOC_E_NONE;
}

int
phy_bcm542xx_rdb_reg_get(int unit, soc_port_t port, int rdb) {
    phy_ctrl_t *pc = NULL;
    uint16 value;

    pc = EXT_PHY_SW_STATE(unit, port);

    phy_bcm542xx_direct_reg_read(unit, pc,(uint16)rdb, &value);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,"u=%d p=%d rdb=0x%04x value=0x%04x\n"),
              unit, port, (uint16)rdb, value));
    return SOC_E_NONE;
}

#endif

/*
 * Variable:    phy_542xxdrv_ge
 * Purpose:     PHY driver for 542XX
 */
phy_driver_t phy_542xxdrv_ge = {
    "542XX Gigabit PHY Driver",
    phy_bcm542xx_init,
    phy_bcm542xx_reset,         /* phy_fe_ge_reset */
    phy_bcm542xx_link_get,
    phy_bcm542xx_enable_set,
    phy_bcm542xx_enable_get,
    phy_bcm542xx_duplex_set,
    phy_bcm542xx_duplex_get,
    phy_bcm542xx_speed_set,
    phy_bcm542xx_speed_get,
    phy_bcm542xx_master_set,
    phy_bcm542xx_master_get,
    phy_bcm542xx_autoneg_set,
    phy_bcm542xx_autoneg_get,
    NULL,                       /* pd_adv_local_set */
    NULL,                       /* pd_adv_local_get */
    NULL,                       /* pd_adv_remote_get */
    phy_bcm542xx_lb_set,
    phy_bcm542xx_lb_get,
    phy_bcm542xx_interface_set,
    phy_bcm542xx_interface_get,
    NULL,                       /* pd_ability */
    phy_bcm542xx_link_up,       /* pd_linkup */
    NULL,                       /* pd_linkdn_evt */
    phy_bcm542xx_mdix_set,
    phy_bcm542xx_mdix_get,
    phy_bcm542xx_mdix_status_get,
    phy_bcm542xx_medium_config_set,
    phy_bcm542xx_medium_config_get,
    phy_bcm542xx_medium_status,
    phy_54280_cable_diag_dispatch,
    NULL,                       /* pd_link_change */
    phy_bcm542xx_control_set,      
    phy_bcm542xx_control_get,      
    phy_ge_reg_read,
    phy_ge_reg_write,
    phy_ge_reg_modify,
    NULL,                       /* pd_notify */
    phy_bcm542xx_probe,         /* pd_probe */
    phy_bcm542xx_ability_advert_set,  
    phy_bcm542xx_ability_advert_get,  
    phy_bcm542xx_ability_remote_get,  
    phy_bcm542xx_ability_local_get,
    NULL,                       /* pd_firmware_set */
    phy_bcm542xx_timesync_config_set,
    phy_bcm542xx_timesync_config_get,
    phy_bcm542xx_timesync_control_set,
    phy_bcm542xx_timesync_control_get,
    NULL, /* pd_diag_ctrl */
    NULL, /* pd_lane_control_set */
    NULL, /* pd_lane_control_get */
    NULL, /* pd_lane_reg_read */
    NULL, /* pd_lane_reg_write */
    phy_bcm542xx_oam_config_set,
    phy_bcm542xx_oam_config_get,
    phy_bcm542xx_oam_control_set,
    phy_bcm542xx_oam_control_get,
    phy_bcm542xx_timesync_enhanced_capture_get
};

#else /* INCLUDE_PHY_542XX */
typedef int _soc_phy_542xx_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_542XX */

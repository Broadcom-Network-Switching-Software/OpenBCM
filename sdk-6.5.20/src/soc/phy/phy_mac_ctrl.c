/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

typedef int phy_mac_not_empty; /* Make ISO compilers happy. */
#if defined(INCLUDE_FCMAP) || defined(INCLUDE_MACSEC) || defined(INCLUDE_PLP_IMACSEC)
#include <sal/types.h>
#include <sal/core/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/libc.h>

#include "phy_mac_ctrl.h"

extern phy_mac_drv_t phy_unimac_drv;
#if 0
extern phy_mac_drv_t phy_bigmac_drv;
#endif
extern phy_mac_drv_t phy_xmac_drv;

/*
 * Allocate mac control block and attach the MAC driver.
 */
phy_mac_ctrl_t* 
phy_mac_driver_attach(blmi_dev_addr_t dev_addr,
                         phy_mactype_t  mac_type,
                         blmi_dev_io_f   mmi_cbf)
{
    phy_mac_ctrl_t *mmc;
    
    mmc = sal_alloc(sizeof(phy_mac_ctrl_t), 
                               "bmac_driver");
    if (mmc == NULL) {
        return NULL;
    }

    if (mac_type == PHY_MAC_CORE_UNIMAC) {
        sal_memcpy(&mmc->mac_drv, &phy_unimac_drv, sizeof(phy_mac_drv_t));
#if 0
    } else if (mac_type == PHY_MAC_CORE_BIGMAC) {
        sal_memcpy(&mmc->mac_drv, &phy_bigmac_drv, sizeof(phy_mac_drv_t));
#endif
#if defined(INCLUDE_FCMAP) || defined(INCLUDE_MACSEC)
    } else if (mac_type == PHY_MAC_CORE_XMAC) {
        sal_memcpy(&mmc->mac_drv, &phy_xmac_drv, sizeof(phy_mac_drv_t));
#endif
    } else {
        sal_free(mmc);
        mmc = NULL;
    }

    if (mmc) {
        mmc->devio_f = mmi_cbf;
        mmc->dev_addr = dev_addr;
        mmc->flag = 0;
        mmc->unit = 0;
    }

    return mmc;
}

int phy_mac_driver_detach(phy_mac_ctrl_t *mmc)
{
    sal_free(mmc);
    return 0;
}


/*
 *  Set config flag for the mac driver init
*/
void 
phy_mac_driver_config(phy_mac_ctrl_t *mmc,
                         phy_mactype_t  mac_type,
                         uint32 flag)
{
    if (mmc == NULL) {
        return;
    }

    mmc->flag = flag;
    return;
}

/*
 *  Set config flag for the mac driver init
*/
void 
phy_mac_driver_unit_set(phy_mac_ctrl_t *mmc,
                         phy_mactype_t  mac_type,
                         uint32 unit)
{
    if (mmc == NULL) {
        return;
    }

    mmc->unit = unit;
    return;
}
#ifdef INCLUDE_PLP_IMACSEC

/*
 * Frame Length 
 */
#define PHY_UNIMAC_REG_DUPLEX_GATEWAY_MAX_FRAME_SIZE  0x0003fe4


/* DEFAULT IPG */
#define PHY_UNIMAC_REG_DEFAULT_IPG        12
#define BMACSEC_E_SUCCESS(rv)		((rv) >= 0)
#define BMACSEC_E_FAILURE(rv)		((rv) < 0)
/*
 * Function:     
 *    phy_unimac_duplex_gateway_set
 * Purpose:    
 *    To set the Duplex Gateway mode for MACSEC PHY's line/switch side MAC.
 * Parameters:
 *    phy_id    - PHY's device address
 * Returns:    
 */
int
phy_unimac_duplex_gateway_set(phy_mac_ctrl_t *mac_ctrl,
                                        int dev_port, int speed, int duplex)
{
    int rv=0;
	phy_mac_duplex_gateway_t dgw;

    dgw.lport     = PHY_UNIMAC_V2_LINE_PORT(dev_port);
    dgw.sport     = PHY_UNIMAC_V2_SWITCH_PORT(dev_port);
    dgw.max_frame = PHY_UNIMAC_REG_DUPLEX_GATEWAY_MAX_FRAME_SIZE;
    dgw.ipg       = PHY_UNIMAC_REG_DEFAULT_IPG;
    dgw.speed     = speed;
    dgw.duplex    = duplex;

    rv =  BMACSEC_MAC_DUPLEX_GATEWAY(mac_ctrl, &dgw);

    return rv;
}

/*
 * Function:     
 *    phy_unimac_switch_side_set_params
 * Purpose:    
 *    To set Switch side speed, duplex and pause parameters
 * Parameters:
 *    phy_id    - PHY's device address
 *    port_mode - Port in Copper or Fiber mode
 *    switch_side_policy - PHY_MAC_SWITCH_FIXED
 *                         ( Switch side is in fixed mode, and the speed
 *                           duplex and pause settings are provided by the
 *                           caller ).
 *                       - PHY_MAC_SWITCH_FOLLOWS_LINE
 *                         ( Switch side follows line side operating
 *                           parameters. Autoconfig mode ).
 *                       - PHY_MAC_SWITCH_DUPLEX_GATEWAY
 *                         ( half-duplex on Line-side MAC, and
 *                           full-duplex on switch-side MAC )
 *    speed     - Speed to set
 *                   10   = 10Mbps
 *                   100  = 100Mbps
 *                   1000 = 1000Mbps
 *    duplex    - Half Duplex = BMACSEC_HALF_DUPLEX
 *              - Full Duplex = BMACSEC_FULL_DUPLEX
 *    pause_enable - 1 = enables pause
 *                 - 0 = disables pause.
 * Returns:    
 */
int
phy_unimac_switch_side_set_params(int unit, soc_port_t port,
                          phy_mac_policy_t switch_side_policy, 
                          int speed, int duplex, int pause_enable)
{
    int    rv = BMACSEC_E_NONE; 
    phy_ctrl_t  *pc;
    soc_port_t port_s;
    bcm_plp_base_t_sec_phy_access_t *phy_info;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    phy_info = IMACSEC_DES_PTR_GET(pc);
    port_s =PHY_UNIMAC_V2_SWITCH_PORT(port);

    switch ( switch_side_policy ) {
    case  PHY_MAC_SWITCH_FIXED :
        /* Disable MAC Auto config */
        rv = BMACSEC_MAC_AUTO_CONFIG_SET(phy_info->unimac, port_s, 0);
        if (!BMACSEC_E_SUCCESS(rv)) {
            return BMACSEC_E_FAIL;
        }
        /* Set MAC Speed */
        rv = BMACSEC_MAC_SPEED_SET(phy_info->unimac, port_s, speed);
        if (!BMACSEC_E_SUCCESS(rv)) {
            return BMACSEC_E_FAIL;
        }
        /* Set MAC Duplex */
        rv = BMACSEC_MAC_DUPLEX_SET(phy_info->unimac, port_s, duplex);
        if (!BMACSEC_E_SUCCESS(rv)) {
            return BMACSEC_E_FAIL;
        }
        /* Set Pause settings */
        rv = BMACSEC_MAC_PAUSE_SET(phy_info->unimac, port_s, pause_enable);
        if (!BMACSEC_E_SUCCESS(rv)) {
            return BMACSEC_E_FAIL;
        }
        break;

    case  PHY_MAC_SWITCH_FOLLOWS_LINE :
        /*  Put Switch side MAC in auto config */
        rv = BMACSEC_MAC_AUTO_CONFIG_SET(phy_info->unimac, port_s, 1);
        if (!BMACSEC_E_SUCCESS(rv)) {
            return BMACSEC_E_FAIL;
        }
        break;

    case  PHY_MAC_SWITCH_DUPLEX_GATEWAY :
        rv = phy_unimac_duplex_gateway_set(phy_info->unimac,
                                                     port, speed, duplex);
        break;

    default :
        return BMACSEC_E_CONFIG;
    }

    return BMACSEC_E_NONE;
}
#endif
#endif /* defined(INCLUDE_FCMAP) || defined (INCLUDE_MACSEC) || defined(INCLUDE_PLP_IMACSEC) */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Xmac Driver (10G MAC)
 */

typedef int phy_xmac_not_empty; /* Make ISO compilers happy. */
#if defined(INCLUDE_FCMAP) || defined(INCLUDE_MACSEC)
#include "phy_xmac.h"
#include <blmi_err.h>
#include "phy_mac_ctrl.h"

/***************************************************************
 * XMAC functions.
 */
/*
 * Function:     
 *    xmac_mac_init
 * Purpose:    
 *    Initialize the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    pt
 * Notes: 
 * The MAC will be put in Auto config mode at initilization
 * MAX Frame Size = 0x3fff
 * IPG = 12
 * TX and RX are enabled
 */
int 
phy_xmac_mac_init(phy_mac_ctrl_t *mmc, int dev_port)
{
    uint64 rxctrl_reg;
    uint64 txctrl_reg;
    uint64 ms_ctrl_reg;
    uint64 rx_lss_ctrl;
    uint64 mac_ctrl;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    /* Put the mac in reset */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_reset(mmc, dev_port, 1));

    /* Take mac out of reset */
    /* Disable Tx/Rx */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_reset(mmc, dev_port, 0));

    /* Set MAX Frame length to 0x3fff */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_max_frame_set(mmc, dev_port, 
                               XMAC_MAC_DEFAULT_MAX_FRAME_SIZE));
    /* Set IPG to 12 */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_update_ipg(mmc, dev_port, XMAC_MAC_DEFAULT_IPG));

    /* Enable Pause */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_pause_set(mmc, dev_port, 1));

    /* CRC Strip for RX */
    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_RXCTRL_REG(mmc, dev_port, &rxctrl_reg));
    XMAC_64_LO_SET(rxctrl_reg, 
            COMPILER_64_LO(rxctrl_reg) | (XMAC_RX_CTRL_CRCSTRIP_MASK));
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_RXCTRL_REG(mmc, dev_port, rxctrl_reg));

    /* TX CRC corrupt enable */
    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_MAC_MACSEC_CTRL_REG(mmc, dev_port, &ms_ctrl_reg));
    XMAC_64_LO_SET(ms_ctrl_reg, 
    COMPILER_64_LO(ms_ctrl_reg) |  XMAC_MACSEC_CTRL_TX_CRC_CORRUPT_MASK);
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_MAC_MACSEC_CTRL_REG(mmc, dev_port, ms_ctrl_reg));

    /* Append CRC on transmit */
    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_TXCTRL_REG(mmc, dev_port, &txctrl_reg));
    XMAC_64_LO_SET(txctrl_reg, 
    COMPILER_64_LO(txctrl_reg) |  XMAC_TX_CTRL_CRCMODE_MASK);
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_TXCTRL_REG(mmc, dev_port, txctrl_reg));

    /* Set PHY LSS (Local fault, remote fault termination disable) */
    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_RX_LSS_CTRL_REG(mmc, dev_port, &rx_lss_ctrl));
    XMAC_64_LO_SET(rx_lss_ctrl, 
    COMPILER_64_LO(rx_lss_ctrl) |  XMAC_RX_LSS_CTRL_DISABLE_MASK);
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_RX_LSS_CTRL_REG( mmc, dev_port, rx_lss_ctrl));

    /* Disable Loopback */
    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_MAC_CTRL_REG(mmc, dev_port, &mac_ctrl));

    XMAC_64_LO_SET(mac_ctrl,
    COMPILER_64_LO(mac_ctrl) & ~(XMAC_MAC_CTRL_LB_LL_MASK |
                                          XMAC_MAC_CTRL_LB_CL_MASK |
                                          XMAC_MAC_CTRL_LB_LR_MASK |
                                          XMAC_MAC_CTRL_LB_CR_MASK ));
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_MAC_CTRL_REG(mmc, dev_port, mac_ctrl));

    /* Enable TX and RX */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_enable_set(mmc, dev_port, 1));

    return BLMI_E_NONE;
}

/*
 * Function:     
 *    xmac_mac_fcmap_init
 * Purpose:    
 *    Initialize the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    pt
 * Notes: 
 * The MAC will be put in Auto config mode at initilization
 * MAX Frame Size = 0x3fff
 * IPG = 12
 * TX and RX are enabled
 */
int 
phy_xmac_mac_fcmap_init(phy_mac_ctrl_t *mmc, int dev_port)
{
    uint64 rxctrl_reg;
    uint64 txctrl_reg;
    uint64 rx_lss_ctrl;
    uint64 mac_ctrl;
    uint64 pause_reg ;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    /* Put the mac in reset */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_reset(mmc, dev_port, 1));

    /* Take mac out of reset */
    /* Disable Tx/Rx */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_reset(mmc, dev_port, 0));

    /* Set MAX Frame length to 0x3fff */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_max_frame_set(mmc, dev_port, 
                               XMAC_MAC_DEFAULT_MAX_FRAME_SIZE));
    /* Set IPG to 12 */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_update_ipg(mmc, dev_port, XMAC_MAC_DEFAULT_IPG));

    /* Enable Pause */
    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_PAUSE_CTRL_REG(mmc, dev_port, &pause_reg));
    if (XMAC_IS_V2(mmc)) {
        COMPILER_64_SET(pause_reg, 0x1f, 0xffe7c000);
    }
    else {
        COMPILER_64_SET(pause_reg, 0, 0x7c000);
    }
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_PAUSE_CTRL_REG(mmc, dev_port, pause_reg));

    COMPILER_64_SET(rxctrl_reg, 0, 0);
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_RXCTRL_REG(mmc, dev_port, rxctrl_reg));

    COMPILER_64_SET(txctrl_reg, 0x10, 0xc811);
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_TXCTRL_REG(mmc, dev_port, txctrl_reg));

    /* Set PHY LSS (Local fault, remote fault termination disable) */
    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_RX_LSS_CTRL_REG(mmc, dev_port, &rx_lss_ctrl));
    XMAC_64_LO_SET(rx_lss_ctrl,
    COMPILER_64_LO(rx_lss_ctrl) |  XMAC_RX_LSS_CTRL_DISABLE_MASK);
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_RX_LSS_CTRL_REG( mmc, dev_port, rx_lss_ctrl));

    /* Disable Loopback */
    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_MAC_CTRL_REG(mmc, dev_port, &mac_ctrl));

    XMAC_64_LO_SET(mac_ctrl,
    COMPILER_64_LO(mac_ctrl) &  ~(XMAC_MAC_CTRL_LB_LL_MASK |
                                          XMAC_MAC_CTRL_LB_CL_MASK |
                                          XMAC_MAC_CTRL_LB_LR_MASK |
                                          XMAC_MAC_CTRL_LB_CR_MASK ));
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_MAC_CTRL_REG(mmc, dev_port, mac_ctrl));

    /* Enable TX and RX */
    BLMI_E_IF_ERROR_RETURN(
        phy_xmac_mac_enable_set(mmc, dev_port, 1));

    return BLMI_E_NONE;
}

int 
phy_xmac_mac_readtx(phy_mac_ctrl_t *mmc, int dev_port)
{
    uint64 txctrl_reg;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    /* Append CRC on transmit */
    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_TXCTRL_REG(mmc, dev_port, &txctrl_reg));
    /*
    BMF_SAL_PRINTF("READING tx ctrl = %x %x\n", 
                        COMPILER_64_HI(txctrl_reg),
                        COMPILER_64_LO(txctrl_reg));
     */
    return BLMI_E_NONE;
}



/*
 * Function:     
 *    xmac_mac_reset
 * Purpose:    
 *    This function puts the XMAC in reset if reset is 1 else takes mac
 *    out of reset 
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    reset    - Reset MAC
 *               1 = Put in Reset
 *               0 = Take out of reset
 * Notes: 
 */
int 
phy_xmac_mac_reset(phy_mac_ctrl_t *mmc, int dev_port, int reset)
{
    uint64 mac_ctrl;
    buint32_t val;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    val = (reset) ? 1 : 0;

    if (dev_port > 3) {
        return BLMI_E_NONE;
    }
    BLMI_E_IF_ERROR_RETURN(
        XMAC_READ_MAC_CTRL_REG(mmc, dev_port, &mac_ctrl));

    if (val) {
        XMAC_64_LO_SET(mac_ctrl,
        COMPILER_64_LO(mac_ctrl) |  (XMAC_MAC_CTRL_RESET_MASK));
    } else { /* out of reset */
        XMAC_64_LO_SET(mac_ctrl,
        COMPILER_64_LO(mac_ctrl) &  ~(XMAC_MAC_CTRL_RESET_MASK |
                                      XMAC_MAC_CTRL_TXEN_MASK  |
                                      XMAC_MAC_CTRL_RXEN_MASK));
    }
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_MAC_CTRL_REG(mmc, dev_port, mac_ctrl));


    return BLMI_E_NONE;
}


/*
 * Function:     
 *    xmac_mac_enable_set
 * Purpose:    
 *    Enable/Disable XMAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    enable   - Enable flag
 *               1 = Enable MAC
 *               0 = Disable MAC
 * Notes: 
 */
int 
phy_xmac_mac_enable_set(phy_mac_ctrl_t *mmc, int dev_port, int enable)
{
    uint64 mac_ctrl;
    buint32_t val;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    val = (enable) ? 1 : 0;

    /* Enable TX and RX */
    BLMI_E_IF_ERROR_RETURN(
        XMAC_READ_MAC_CTRL_REG(mmc, dev_port, &mac_ctrl));

    if (val) {
        /* Enable TX and RX domains */
        XMAC_64_LO_SET(mac_ctrl,
        COMPILER_64_LO(mac_ctrl) |  (XMAC_MAC_CTRL_TXEN_MASK|
                                             XMAC_MAC_CTRL_RXEN_MASK));
    } else {
        /* Don't disable TX since it stops egress and hangs if CPU sends */
        XMAC_64_LO_SET(mac_ctrl,
        COMPILER_64_LO(mac_ctrl) |   (XMAC_MAC_CTRL_TXEN_MASK));
        XMAC_64_LO_SET(mac_ctrl,
        COMPILER_64_LO(mac_ctrl) &  ~(XMAC_MAC_CTRL_RXEN_MASK));
    }
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_MAC_CTRL_REG(mmc, dev_port, mac_ctrl));
    return BLMI_E_NONE;
}

/*
 * Function:     
 *    xmac_mac_enable_get
 * Purpose:    
 *    Get the enable status of XMAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    enable   - (OUT)Enable flag
 *               1 = Enable MAC
 *               0 = Disable MAC
 * Notes: 
 */
int 
phy_xmac_mac_enable_get(phy_mac_ctrl_t *mmc, int dev_port, int *enable) 
{
    uint64   mac_ctrl;
    buint32_t   val;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    BLMI_E_IF_ERROR_RETURN(
        XMAC_READ_MAC_CTRL_REG(mmc, dev_port, &mac_ctrl));
    val = (COMPILER_64_LO(mac_ctrl) & 
               XMAC_MAC_CTRL_RXEN_MASK) >> XMAC_MAC_CTRL_RXEN_SHIFT;

    *(enable) = (val) ? 1 : 0;

    return BLMI_E_NONE;
}


/*
 * Function:     
 *    xmac_mac_update_ipg
 * Purpose:    
 *    Set IPG of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    ipg      - IPG value
 * Notes: 
 */
int 
phy_xmac_mac_update_ipg(phy_mac_ctrl_t *mmc, int dev_port, int ipg)
{
    uint64 ipg_reg;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    if((ipg < 8) || (ipg > 64)) {
        return BLMI_E_PARAM;
    }
    BLMI_E_IF_ERROR_RETURN(
        XMAC_READ_TXCTRL_REG(mmc, dev_port, &ipg_reg));

    XMAC_64_LO_SET(ipg_reg,
    COMPILER_64_LO(ipg_reg) &  ~(XMAC_TX_CTRL_IPG_MASK));
    XMAC_64_LO_SET(ipg_reg,
    COMPILER_64_LO(ipg_reg) |  ((ipg&0x7f)<< XMAC_TX_CTRL_IPG_SHIFT));

    BLMI_E_IF_ERROR_RETURN(
        XMAC_WRITE_TXCTRL_REG(mmc, dev_port, ipg_reg));

    return BLMI_E_NONE;
}


/*
 * Function:     
 *    xmac_mac_max_frame_set
 * Purpose:    
 *    Set Max frame of the MAC
 * Parameters:
 *    mmc  - mac driver control
 *    dev_port  - Port number
 *    max_frame - Max frame value
 * Notes: 
 */
int 
phy_xmac_mac_max_frame_set(phy_mac_ctrl_t *mmc, int dev_port, 
                             buint32_t max_frame)
{
    uint64 max_sz;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    /* 
     * Maximum Frame size shouldn;t be less 48 and cannot be greater than
     * 0x3ffc
     */
    if((max_frame < 48) || (max_frame > XMAC_MAC_DEFAULT_MAX_FRAME_SIZE)) {
        return BLMI_E_PARAM;
    }

    COMPILER_64_SET(max_sz, 0, (max_frame & XMAC_RXMAXSZ_MAX_FRAME_SIZE_MASK));

    BLMI_E_IF_ERROR_RETURN(
        XMAC_WRITE_RXMAXSZ_REG(mmc, dev_port, max_sz));

    return BLMI_E_NONE;
}

/*
 * Function:     
 *    xmac_mac_max_frame_get
 * Purpose:    
 *    Get Max frame of the MAC
 * Parameters:
 *    mmc  - mac driver control
 *    dev_port  - Port number
 *    max_frame - (OUT) Max frame
 * Notes: 
 */
int 
phy_xmac_mac_max_frame_get(phy_mac_ctrl_t *mmc, int dev_port, 
                       buint32_t *max_frame) 
{
    uint64 frame_len;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    BLMI_E_IF_ERROR_RETURN(
        XMAC_READ_RXMAXSZ_REG(mmc, dev_port, &frame_len));
    *max_frame = (COMPILER_64_LO(frame_len) & 
                  XMAC_RXMAXSZ_MAX_FRAME_SIZE_MASK);

    return BLMI_E_NONE;
}


/*
 * Function:     
 *    xmac_mac_pause_frame_fwd
 * Purpose:    
 *    Enable/Disable Pause Frame Forward
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    pause_fwd - 1 - Forward Pause enable
 *                0 - Consume Pause 
 * Notes: 
 */
int 
phy_xmac_mac_pause_frame_fwd(phy_mac_ctrl_t *mmc, int dev_port, 
                         int pause_fwd)
{
    uint64 rxctrl_reg;
    int  pass_mac_cntl;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    if(pause_fwd) {
        pass_mac_cntl = 1; 
    } else {
        pass_mac_cntl = 0; 
    }

    BLMI_E_IF_ERROR_RETURN(
        XMAC_READ_RXCTRL_REG(mmc, dev_port, &rxctrl_reg));

    XMAC_64_HI_SET(rxctrl_reg, 0);
    XMAC_64_LO_SET(rxctrl_reg,
    COMPILER_64_LO(rxctrl_reg) &  ~(XMAC_RX_CTRL_PASS_MAC_CNTL_MASK));

    XMAC_64_LO_SET(rxctrl_reg,
    COMPILER_64_LO(rxctrl_reg) |  
        (pass_mac_cntl << XMAC_RX_CTRL_PASS_MAC_CNTL_SHIFT));

    BLMI_E_IF_ERROR_RETURN(
        XMAC_WRITE_RXCTRL_REG(mmc, dev_port, rxctrl_reg));

    return rv;
}


/*
 * Function:     
 *    xmac_mac_pause_set
 * Purpose:    
 *    Set pause of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    pause    - 1 - Pause enable
 *               0 - Pause disable
 * Notes: 
 */
int 
phy_xmac_mac_pause_set(phy_mac_ctrl_t *mmc, int dev_port, int pause)
{
    uint64 pause_reg ;
    int  tx_pause_en, rx_pause_en;
    int  rv = BLMI_E_NONE;
    buint32_t tmp_val[2];

    if(pause == 1) {
        tx_pause_en = 1; 
        rx_pause_en = 1; 
    } else {
        tx_pause_en = 0; 
        rx_pause_en = 0; 
    }

    BLMI_E_IF_ERROR_RETURN(
            XMAC_READ_PAUSE_CTRL_REG(mmc, dev_port, &pause_reg));
    XMAC_64_HI_SET(pause_reg, 0);
    XMAC_64_LO_SET(pause_reg,
    COMPILER_64_LO(pause_reg) &  ~(XMAC_TX_EN_PAUSE_MASK |
                                           XMAC_RX_EN_PAUSE_MASK));
    XMAC_64_LO_SET(pause_reg,
    COMPILER_64_LO(pause_reg) |  
                            (tx_pause_en << XMAC_TX_EN_PAUSE_SHIFT));
    XMAC_64_LO_SET(pause_reg,
    COMPILER_64_LO(pause_reg) |  
                            (rx_pause_en << XMAC_RX_EN_PAUSE_SHIFT));
    BLMI_E_IF_ERROR_RETURN(
            XMAC_WRITE_PAUSE_CTRL_REG(mmc, dev_port, pause_reg));

    return rv;
}

/*
 * Function:     
 *    xmac_mac_speed_set
 * Purpose:    
 *    Set Speed of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    speed    - 10000 - 10Gbps
 *               1000  - 1000Mbps
 *               100   - 100Mbps
 *               10    - 10Mbps
 * Notes: 
 */
int 
phy_xmac_mac_speed_set(phy_mac_ctrl_t *mmc, int dev_port, int speed) 
{

    switch (speed) {
    case 0:     /* No change */
    case 10000:
        break;
    default:
        return (BLMI_E_CONFIG);
    }

    return BLMI_E_NONE;
}

/*
 * Function:     
 *    xmac_mac_speed_get
 * Purpose:    
 *    Get Speed of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    speed    - 10000 - 10Gbps
 * Notes: 
 */
int 
phy_xmac_mac_speed_get(phy_mac_ctrl_t *mmc, int dev_port, int *speed) 
{
    *speed = 10000;
    return BLMI_E_NONE;
}


/*
 * Function:     
 *    xmac_mac_duplex_set
 * Purpose:    
 *    Set Duplex of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    duplex   - 1 - Full Duplex
 *               0 - Half Duplex
 * Notes: 
 */
int 
phy_xmac_mac_duplex_set(phy_mac_ctrl_t *mmc, int dev_port, int duplex)
{
    /* XMAC always in full duplex mode */
    return BLMI_E_NONE;
}

/*
 * Function:     
 *    xmac_mac_duplex_get
 * Purpose:    
 *    Get Duplex setting of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    duplex   - (OUT) 1 - Full Duplex
 *                     0 - Half Duplex
 * Notes: 
 */
int 
phy_xmac_mac_duplex_get(phy_mac_ctrl_t *mmc, int dev_port, int *duplex)
{
    /* XMAC always in full duplex mode */
    *duplex = 1;
    return BLMI_E_NONE;
}

/*
 * Function:     
 *    xmac_mac_auto_cfg_set
 * Purpose:    
 *    Set Auto mode configuration.
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    enable   - 1 - Enable Auto mode
 *               0 - Disable Auto Mode
 * Notes: 
 */
int 
phy_xmac_mac_auto_cfg_set(phy_mac_ctrl_t *mmc, int dev_port, int enable)
{
    /* No auto mode configuration in XMAC */
    return BLMI_E_NONE;
}

phy_mac_drv_t phy_xmac_drv = {
    phy_xmac_mac_init,
    phy_xmac_mac_reset,
    NULL,
    phy_xmac_mac_enable_set,
    phy_xmac_mac_enable_get,
    phy_xmac_mac_speed_set,
    phy_xmac_mac_speed_get,
    phy_xmac_mac_duplex_set,
    phy_xmac_mac_duplex_get,
    phy_xmac_mac_update_ipg,
    phy_xmac_mac_max_frame_set,
    phy_xmac_mac_max_frame_get,
    phy_xmac_mac_pause_set,
    NULL,
    phy_xmac_mac_pause_frame_fwd,
};

#endif /* defined(INCLUDE_FCMAP) || defined (INCLUDE_MACSEC) */

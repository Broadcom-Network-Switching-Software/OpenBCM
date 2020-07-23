/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
typedef int phy_unimac_not_empty; /* Make ISO compilers happy. */
#if defined(INCLUDE_FCMAP) || defined(INCLUDE_MACSEC)  || defined(INCLUDE_PLP_IMACSEC)
#include <sal/types.h>
#include <sal/core/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/libc.h>

#include "phy_unimac.h"
#include "phy_mac_ctrl.h"

#if !defined(INCLUDE_FCMAP) && !defined(INCLUDE_MACSEC)
extern void sal_usleep(uint32 usec);
#define BMF_SAL_USLEEP sal_usleep
#define BLMI_E_IF_ERROR_RETURN(op) \
        do { int __rv__; if ((__rv__ = (op)) < 0) return(__rv__); } while(0)
#else
#include <buser_sal.h>
#include <blmi_err.h>
#endif
#define SPEED_MAPPING(_sp, _spr)    do { \
    switch ( _sp ) { \
    case 0:    return BLMI_E_NONE;  \
    case 10:   _spr = 0; break;     \
    case 100:  _spr = 1; break;     \
    case 1000: _spr = 2; break;     \
    default: return (BLMI_E_CONFIG); }  } while(0)

/* JIRA BMACSEC-255 specific enhancement */
#define UNIMAC_REG_BKP_IPG_CONFIG_RX_MASK    0x7c
#define UNIMAC_REG_BKP_IPG_CONFIG_RX_SHIFT   2
/***************************************************************
 * MACSEC driver functions.
 */
/*
 * Function:
 *    phy_unimac_mac_init
 * Purpose:
 *    Initialize the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 * Notes:
 * The MAC will be put in Auto config mode at initilization
 * MAX Frame Size = 0x3fff
 * IPG = 12
 * TX and RX are enabled
 */
int
phy_unimac_mac_init(phy_mac_ctrl_t *mmc, int dev_port)
{

    uint32 macsec_ctrl, cmd_cfg;

    /* Set MAC in Auto Config Mode */
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_auto_cfg(mmc, dev_port, 1));

    /* Set MAX Frame length to 0x3fff */
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_max_frame_set(mmc, dev_port,
                               UNIMAC_REG_DEFAULT_MAX_FRAME_SIZE));

    /* Set IPG to 12 */
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_update_ipg(mmc, dev_port,
                            UNIMAC_REG_DEFAULT_IPG));

    /* Enable TX and RX */
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_enable_set(mmc, dev_port, 1));

    /* Enable the TX CRC corrupt in MACSEC Control register */
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_MACSEC_CNTL_REG(mmc, dev_port,
                                              &macsec_ctrl));
    macsec_ctrl &= ~(UNIMAC_REG_TX_CRC_ENABLE_MASK);
    macsec_ctrl |= (UNIMAC_REG_TX_CRC_ENABLE <<
                UNIMAC_REG_TX_CRC_ENABLE_SHIFT);
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_MACSEC_CNTL_REG(mmc, dev_port,macsec_ctrl));

    /* Set terminate CRC in the MAC and Pass MAC control Frames*/
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_cfg));
    cmd_cfg &= ~(UNIMAC_REG_CMD_CFG_CRC_FWD_MASK);
    cmd_cfg &= ~(UNIMAC_REG_CMD_CFG_CTL_FRM_ENA_MASK);
    cmd_cfg |= (UNIMAC_REG_CMD_CFG_CRC_FWD_DISABLE <<
                UNIMAC_REG_CMD_CFG_CRC_FWD_SHIFT);
    cmd_cfg |= (1 << UNIMAC_REG_CMD_CTL_FRM_ENA_SHIFT);
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_cfg));
    /* OVERFLOW check enable  */
#ifdef INCLUDE_PLP_IMACSEC
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_UMAC_EEE_CTRL_REG(mmc, dev_port, &cmd_cfg));
    cmd_cfg |= UNIMAC_REG_EEE_CTRL_RX_FIFO_CHECK;
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_UMAC_EEE_CTRL_REG(mmc, dev_port, cmd_cfg));
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_cfg));
    cmd_cfg |= UNIMAC_REG_CMD_CFG_OVERFLOW_EN;
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_cfg));
#endif

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_auto_cfg
 * Purpose:
 *    This function puts the MAC in auto config if auto_cfg is 1.
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    auto_cfg - Auto configuration flag
 *               1 = Auto configuration
 *               0 = Forced mode
 * Notes:
 */
int
phy_unimac_mac_auto_cfg(phy_mac_ctrl_t *mmc, int dev_port, int auto_cfg)
{
    uint32 cmd_reg, val;

    val = (auto_cfg) ? 1 : 0;

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
    cmd_reg &= ~(UNIMAC_REG_CMD_CFG_ENA_EXA_CONFIG_MASK);
    cmd_reg |= (val << UNIMAC_REG_CMD_CFG_ENA_EXA_CONFIG_SHIFT);
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));

    /* Need to sleep for 400ns (or 256 core clock cycles) */
    BMF_SAL_USLEEP(1);

    return BLMI_E_NONE;
}

/*
 */
/*
 * Function:
 *    phy_unimac_mac_reset
 * Purpose:
 *    This function puts the MAC in reset if reset is 1 else takes mac
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
phy_unimac_mac_reset(phy_mac_ctrl_t *mmc, int dev_port, int reset)
{
    uint32 cmd_reg, val;

    val = (reset) ? 1 : 0;

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
    cmd_reg &= ~(UNIMAC_REG_CMD_CFG_SW_RESET_MASK);
    cmd_reg |= (val << UNIMAC_REG_CMD_CFG_SW_RESET_SHIFT);
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_enable_set
 * Purpose:
 *    Enable/Disable MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    enable   - Enable flag
 *               1 = Enable MAC
 *               0 = Disable MAC
 * Notes:
 */
int
phy_unimac_mac_enable_set(phy_mac_ctrl_t *mmc, int dev_port, int enable)
{
    uint32 cmd_reg, val;

    val = (enable) ? 1 : 0;
    /* Put the mac in reset */
    BLMI_E_IF_ERROR_RETURN(
    phy_unimac_mac_reset(mmc, dev_port, 1));

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
    cmd_reg &= ~(UNIMAC_REG_CMD_CFG_TXEN_MASK);
    cmd_reg |= (val << UNIMAC_REG_CMD_CFG_TXEN_SHIFT);
    cmd_reg &= ~(UNIMAC_REG_CMD_CFG_RXEN_MASK);
    cmd_reg |= (val << UNIMAC_REG_CMD_CFG_RXEN_SHIFT);
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));

    /* Take mac out of reset */
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_reset(mmc, dev_port, 0));

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_enable_get
 * Purpose:
 *    Get the enable status of MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    enable   - (OUT)Enable flag
 *               1 = Enable MAC
 *               0 = Disable MAC
 * Notes:
 */
int
phy_unimac_mac_enable_get(phy_mac_ctrl_t *mmc, int dev_port, int *enable)
{
    uint32   cmd_reg, val ;

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
    val = (cmd_reg & UNIMAC_REG_CMD_CFG_TXEN_MASK) >>
                     UNIMAC_REG_CMD_CFG_TXEN_SHIFT;

    *(enable) = (val) ? 1 : 0;

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_speed_set
 * Purpose:
 *    Set Speed of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    speed    - 1000 - 1000Mbps
 *               100  - 100Mbps
 *               10   - 10Mbps
 * Notes:
 */
int
phy_unimac_mac_speed_set(phy_mac_ctrl_t *mmc, int dev_port, int speed)
{
    uint32 cmd_reg;
    int      speed_select;

    switch (speed) {
    case 0:     /* No change */
        return BLMI_E_NONE;
    case 10:
        speed_select = 0;
    break;
    case 100:
        speed_select = 1;
    break;
    case 1000:
        speed_select = 2;
        break;
    default:
        return (BLMI_E_CONFIG);
    }

    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_reset(mmc, dev_port, 1));

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));

    cmd_reg &= ~(UNIMAC_REG_CMD_CFG_SPEED_MASK);
    cmd_reg |= (speed_select << UNIMAC_REG_CMD_CFG_SPEED_SHIFT);

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));

    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_reset(mmc, dev_port, 0));

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_speed_get
 * Purpose:
 *    Get Speed of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    speed    - (OUT) 1000 - 1000Mbps
 *                     100  - 100Mbps
 *                     10   - 10Mbps
 * Notes:
 */
int
phy_unimac_mac_speed_get(phy_mac_ctrl_t *mmc, int dev_port, int *speed)
{
    int speed_select;
    uint32 cmd_reg;

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_MAC_MODE_REG(mmc, dev_port, &cmd_reg));
    speed_select = (cmd_reg & UNIMAC_REG_MAC_MODE_SPEED_MASK) >>
                   UNIMAC_REG_MAC_MODE_SPEED_SHIFT;

    switch (speed_select) {
    case 0:
        *speed = 10;
    break;
    case 1:
        *speed = 100;
    break;
    case 2:
        *speed= 1000;
        break;
    default:
        return (BLMI_E_INTERNAL);
    }
    return BLMI_E_NONE;
}


/*
 * Function:
 *    phy_unimac_mac_duplex_set
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
phy_unimac_mac_duplex_set(phy_mac_ctrl_t *mmc, int dev_port, int duplex)
{

    uint32 cmd_reg;
    int hd_enable, speed, rv = BLMI_E_NONE;

    hd_enable = (duplex) ? 0 : 1;

    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_reset(mmc, dev_port, 1));

    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_speed_get(mmc, dev_port, &speed));
    if ((speed >= 1000) && (hd_enable == 1)) {
        /* Half duplex with Gigabit speed not supported */
        rv = BLMI_E_PARAM;
        goto error;
    }

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
    cmd_reg &= ~(UNIMAC_REG_CMD_CFG_DUPLEX_MASK);
    cmd_reg |= (hd_enable << UNIMAC_REG_CMD_CFG_DUPLEX_SHIFT);
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));

error:
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_reset(mmc, dev_port, 0));
    return rv;
}


/*
 * Function:
 *    phy_unimac_mac_duplex_get
 * Purpose:
 *    Get Duplex of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    duplex   - (OUT) 0 - Full Duplex
 *                     1 - Half Duplex
 * Notes:
 */
int
phy_unimac_mac_duplex_get(phy_mac_ctrl_t *mmc, int dev_port, int *fd)
{
    uint32 cmd_reg;
    int      hd_enable;

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_MAC_MODE_REG(mmc, dev_port, &cmd_reg));
    hd_enable = (cmd_reg & UNIMAC_REG_MAC_MODE_DUPLEX_MASK) >>
                                    UNIMAC_REG_MAC_MODE_DUPLEX_SHIFT;

    *fd = (hd_enable) ? 1 : 0;
    return BLMI_E_NONE;
}



/*
 * Function:
 *    phy_unimac_mac_pause_set
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
phy_unimac_mac_pause_set(phy_mac_ctrl_t *mmc, int dev_port, int pause)
{
    uint32 cmd_reg;
    int  tx_pause_ignore, rx_pause_ignore;
    int  duplex, rv = BLMI_E_NONE;

    if(pause == 1) {
        tx_pause_ignore = 0;
        rx_pause_ignore = 0;
    } else {
        tx_pause_ignore = 1;
        rx_pause_ignore = 1;
    }

    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_reset(mmc, dev_port, 1));

    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_duplex_get(mmc, dev_port, &duplex));

    /* handling of PAUSE is differnet in Half duplex more */
    if(duplex == 1) { /* Full Duplex */
        BLMI_E_IF_ERROR_RETURN(
            UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
        cmd_reg &= ~(UNIMAC_REG_CMD_CFG_TX_PAUSE_IGNORE_MASK);
        cmd_reg &= ~(UNIMAC_REG_CMD_CFG_RX_PAUSE_IGNORE_MASK);
        cmd_reg |= (tx_pause_ignore <<
                    UNIMAC_REG_CMD_CFG_TX_PAUSE_IGNORE_SHIFT);
        cmd_reg |= (rx_pause_ignore <<
                    UNIMAC_REG_CMD_CFG_RX_PAUSE_IGNORE_SHIFT);
        BLMI_E_IF_ERROR_RETURN(
            UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));
    } else {
        
    }

    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_reset(mmc, dev_port, 0));
    return rv;
}


/*
 * Function:
 *    phy_unimac_mac_update_ipg
 * Purpose:
 *    Set IPG of the MAC
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    ipg      - IPG value
 * Notes:
 */
int
phy_unimac_mac_update_ipg(phy_mac_ctrl_t *mmc, int dev_port, int ipg)
{
    uint32 ipg_reg;

    if((ipg < 8) || (ipg > 64)) {
        return BLMI_E_PARAM;
    }

    ipg_reg = ipg;
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_TX_IPG_REG(mmc, dev_port, ipg_reg));

    return BLMI_E_NONE;
}


/*
 * Function:
 *    phy_unimac_mac_max_frame_set
 * Purpose:
 *    Set Max frame of the MAC
 * Parameters:
 *    mmc  - MAC control structure pointer
 *    dev_port  - Port number
 *    max_frame - Max frame value
 * Notes:
 */
int
phy_unimac_mac_max_frame_set(phy_mac_ctrl_t *mmc, int dev_port,
                           uint32 max_frame)
{
    /*
     * Maximum Frame size shouldn;t be less 48 and cannot be greater than
     * 0x3fff
     */
    if((max_frame < 48) || (max_frame > 0x3fff)) {
        return BLMI_E_PARAM;
    }
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_FRM_LEN_REG(mmc, dev_port, max_frame));

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_frame_len_check_enable
 * Purpose:
 *    Enable the frame length check error mask in RSV vector
 *    to signal error to encryption block
 * Parameters:
 *    mmc  - MAC control structure pointer
 *    dev_port  - Port number
 *    enable - Enable the error mask
 *    1 -  enable
 *    0 -  disable
 * Notes:
 */
int
phy_unimac_mac_frame_len_check_enable(phy_mac_ctrl_t *mmc, int dev_port, int enable)
{
    uint32 cmd_reg,rsv_mask;
    /*    */
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
    if(enable){
        cmd_reg &= ~UNIMAC_REG_CMD_CFG_CTL_FRM_NO_LGTH_CHECK_MASK;
    }else{
        cmd_reg |= UNIMAC_REG_CMD_CFG_CTL_FRM_NO_LGTH_CHECK_MASK;
    }
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_GPORT_RSV_MASK_REG(mmc, dev_port, &rsv_mask));
    if(enable){
        rsv_mask |= UNIMAC_REG_GPORT_RSV_FRM_LEN_CHK_MASK;
    }else{
        rsv_mask &= ~UNIMAC_REG_GPORT_RSV_FRM_LEN_CHK_MASK;
    }
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_GPORT_RSV_MASK_REG(mmc, dev_port, rsv_mask));

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_rx_enable_set
 * Purpose:
 *    Enable/Disable MAC RX
 * Parameters:
 *    mmc  - MAC control structure pointer
 *    dev_port  - Port number
 *    enable - Enable the Rx
 *    1 -  enable
 *    0 -  disable
 * Notes:
 */
int
phy_unimac_mac_rx_enable_set(phy_mac_ctrl_t *mmc, int dev_port, int enable)
{
    uint32 cmd_reg;
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
    if(enable){
        cmd_reg |= UNIMAC_REG_CMD_CFG_RXEN_MASK;
    }else{
        cmd_reg &= ~UNIMAC_REG_CMD_CFG_RXEN_MASK;
    }
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));
    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_tx_enable_set
 * Purpose:
 *    Enable/Disable MAC TX
 * Parameters:
 *    mmc  - MAC control structure pointer
 *    dev_port  - Port number
 *    enable - Enable the Tx
 *    1 -  enable
 *    0 -  disable
 * Notes:
 */

int
phy_unimac_mac_tx_enable_set(phy_mac_ctrl_t *mmc, int dev_port, int enable)
{
    uint32 cmd_reg;
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
    if(enable){ 
        cmd_reg |= UNIMAC_REG_CMD_CFG_TXEN_MASK;
    }else{
        cmd_reg &= ~UNIMAC_REG_CMD_CFG_TXEN_MASK;
    }
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));
    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_tx_datapath_flush
 * Purpose:
 *    TX-FIFO flush control enable
 * Parameters:
 *    mmc  - MAC control structure pointer
 *    dev_port  - Port number
 *
 * Notes: Before calling TX datapath flush user has to disable pause function
 *  once TX_FIFO is empty reenable the pause function
 */
int
phy_unimac_mac_tx_datapath_flush(phy_mac_ctrl_t *mmc, int dev_port, int enable)
{
   uint32 flush_ctrl;
   BLMI_E_IF_ERROR_RETURN(
      UNIMAC_READ_TX_FLUSH_CTRL_REG(mmc, dev_port, &flush_ctrl));
   if(enable){
       flush_ctrl |= UNIMAC_REG_TX_FLUSH_CTRL_MASK;
   }else{
       flush_ctrl &= ~UNIMAC_REG_TX_FLUSH_CTRL_MASK;
   }

   BLMI_E_IF_ERROR_RETURN(
         UNIMAC_WRITE_TX_FLUSH_CTRL_REG(mmc, dev_port, flush_ctrl));


    return BLMI_E_NONE;
}
/*
 * Function:
 *    phy_unimac_mac_max_frame_get
 * Purpose:
 *    Get Max frame of the MAC
 * Parameters:
 *    mmc  - MAC control structure pointer
 *    dev_port  - Port number
 *    max_frame - (OUT) Max frame
 * Notes:
 */
int
phy_unimac_mac_max_frame_get(phy_mac_ctrl_t *mmc, int dev_port,
                           uint32 *max_frame)
{
    uint32 frame_len;

    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_FRM_LEN_REG(mmc, dev_port, &frame_len));
    *max_frame = frame_len & UNIMAC_REG_FRM_LENr_FMR_LEN_MASK;

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_pause_frame_fwd_set
 * Purpose:
 *    Enable/Disable Pause Frame Forward
 * Parameters:
 *    mmc  - MAC control structure pointer
 *    dev_port  - Port number
 *    pause_fwd - Pause forward
 *                1 - Forward pause
 *                0 - Consume PAUSE.
 * Notes:
 */
int
phy_unimac_mac_pause_frame_fwd_set(phy_mac_ctrl_t *mmc, int dev_port,
                             int pause_fwd)
{
    int pause;
    uint32 cmd_reg;

    if (pause_fwd == 1) {
        pause = 1;
    } else {
        if (pause_fwd == 0) {
            pause = 0;
        } else {
            return BLMI_E_CONFIG;
        }
    }
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg));
    cmd_reg &= ~(UNIMAC_REG_CMD_CFG_PAUSE_FWD_MASK);
    cmd_reg |= (pause << UNIMAC_REG_CMD_CFG_PAUSE_FWD_SHIFT);
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, dev_port, cmd_reg));

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_pause_frame_fwd_get
 * Purpose:
 *    Get the status of Pause Frame Forward
 * Parameters:
 *    mmc  - MAC control structure pointer
 *    dev_port  - Port number
 *    pause_fwd - Pause forward
 *                1 - Forward pause
 *                0 - Consume PAUSE.
 * Notes:
 */
int
phy_unimac_mac_pause_frame_fwd_get(phy_mac_ctrl_t *mmc, int dev_port,
                               int *pause_fwd)
{
    uint32 cmd_reg;

    BLMI_E_IF_ERROR_RETURN(
            UNIMAC_READ_CMD_CFG_REG(mmc, dev_port, &cmd_reg) );
    *pause_fwd = (cmd_reg & UNIMAC_REG_CMD_CFG_PAUSE_FWD_MASK)
                         >> UNIMAC_REG_CMD_CFG_PAUSE_FWD_SHIFT;

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_hd_flow_cntl_set
 * Purpose:
 *    Enable/Disable Half-Duplex Flow Control
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    enable   - Enable flag
 *               1 = Enable  Half-Duplex Flow Control
 *               0 = Disable Half-Duplex Flow Control
 * Notes:
 */
int
phy_unimac_mac_hd_flow_cntl_set(phy_mac_ctrl_t *mmc, int dev_port, int enable)
{
    uint32  cmd_reg, val;

    val = (enable) ? 1 : 0;

    BLMI_E_IF_ERROR_RETURN(
            UNIMAC_READ_IPG_HD_BKP_CNTL_REG(mmc, dev_port, &cmd_reg));
    cmd_reg &= ~(UNIMAC_REG_BKP_HD_FC_ENA_MASK) |
               ~(UNIMAC_REG_BKP_HD_FC_BKOFF_MASK);
    cmd_reg |= (val << UNIMAC_REG_BKP_HD_FC_ENA_SHIFT) |
               (val << UNIMAC_REG_BKP_HD_FC_BKOFF_SHIFT);
    BLMI_E_IF_ERROR_RETURN(
            UNIMAC_WRITE_IPG_HD_BKP_CNTL_REG(mmc, dev_port, cmd_reg) );

    return BLMI_E_NONE;
}

/*
 * Function:
 *    phy_unimac_mac_hd_flow_cntl_get
 * Purpose:
 *    Get the enable status of Half-Duplex Flow Control
 * Parameters:
 *    mmc - mac driver control
 *    dev_port - Port number
 *    enable   - (OUT)Enable flag
 *               1 = Enable  Half-Duplex Flow Control
 *               0 = Disable Half-Duplex Flow Control
 * Notes:
 */
int
phy_unimac_mac_hd_flow_cntl_get(phy_mac_ctrl_t *mmc, int dev_port, int *enable)
{
    uint32   cmd_reg;

    BLMI_E_IF_ERROR_RETURN(
            UNIMAC_READ_IPG_HD_BKP_CNTL_REG(mmc, dev_port, &cmd_reg) );
    *(enable) = (cmd_reg & UNIMAC_REG_BKP_HD_FC_ENA_MASK)
                        >> UNIMAC_REG_BKP_HD_FC_ENA_SHIFT;
    return BLMI_E_NONE;
}


/* Duplex Gateway support :   Line-side MAC = half-duplex  *\
\*                          Switch-side MAC = full-duplex  */

/*
 * Function:
 *    _mac_duplex_gateway_set
 * Purpose:
 *    Config line/switch-side MAC for Duplex Gateway mode
 * Notes:
 */
static int
_mac_duplex_gateway_set(phy_mac_ctrl_t *mmc, int port,
                        int speed, int fullduplex,
                        int ipg, int max_frame, int line_side)
{
    uint32  cmd_reg;

    /* Put the MAC in reset */
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_reset(mmc, port, 1) );

    /* set Max Frame Size and Interpacket Gap */
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_max_frame_set(mmc, port, max_frame) );
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_update_ipg(mmc, port, ipg) );
    /* enable back-pressure in half-duplex mode for the line-side MAC */
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_hd_flow_cntl_set(mmc, port,
                                    (line_side && (!fullduplex)) ? 1 : 0) );

    /* read the COMMAND_CONFIG register */
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_READ_CMD_CFG_REG(mmc, port, &cmd_reg));

    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, speed,
                                          UNIMAC_REG_CMD_CFG_SPEED_MASK,
                                          UNIMAC_REG_CMD_CFG_SPEED_SHIFT);
    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, ((fullduplex) ? 0 : 1),
                                          UNIMAC_REG_CMD_CFG_DUPLEX_MASK,
                                          UNIMAC_REG_CMD_CFG_DUPLEX_SHIFT);
    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, ((line_side) ? 1 : 0),
                                          UNIMAC_REG_CMD_CFG_ENA_EXA_CONFIG_MASK,
                                          UNIMAC_REG_CMD_CFG_ENA_EXA_CONFIG_SHIFT);

    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, 1, UNIMAC_REG_CMD_CFG_TXEN_MASK,
                                          UNIMAC_REG_CMD_CFG_TXEN_SHIFT);
    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, 1, UNIMAC_REG_CMD_CFG_RXEN_MASK,
                                          UNIMAC_REG_CMD_CFG_RXEN_SHIFT);
    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, 1, UNIMAC_REG_CMD_CFG_PROMIS_EN_MASK,
                                          UNIMAC_REG_CMD_CFG_PROMIS_EN_SHIFT);
    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, 0, UNIMAC_REG_CMD_CFG_CRC_FWD_MASK,
                                          UNIMAC_REG_CMD_CFG_CRC_FWD_SHIFT);
    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, 0, UNIMAC_REG_CMD_CFG_PAUSE_FWD_MASK,
                                          UNIMAC_REG_CMD_CFG_PAUSE_FWD_SHIFT);
    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, ((line_side) ? 0 : 1),
                                          UNIMAC_REG_CMD_CFG_SW_OVERRIDE_TX_MASK,
                                          UNIMAC_REG_CMD_CFG_SW_OVERRIDE_TX_SHIFT);
    BMACSEC_MAC_REG_FIELD_SET(cmd_reg, ((line_side) ? 0 : 1),
                                          UNIMAC_REG_CMD_CFG_SW_OVERRIDE_RX_MASK,
                                          UNIMAC_REG_CMD_CFG_SW_OVERRIDE_RX_SHIFT);

    /* write to COMMAND_CONFIG register */
    BLMI_E_IF_ERROR_RETURN(
        UNIMAC_WRITE_CMD_CFG_REG(mmc, port, cmd_reg));

    /* JIRA BMACSEC-255 specific enhancement  */
    if(!line_side) {
        uint32  data;
        BLMI_E_IF_ERROR_RETURN(UNIMAC_READ_IPG_HD_BKP_CNTL_REG(mmc, port, &data));
        if(speed == 0 || speed== 1) { /* 0 is 10M mode, 1 is 100M mode */
           data &= ~(UNIMAC_REG_BKP_IPG_CONFIG_RX_MASK);
           data |= 10 << UNIMAC_REG_BKP_IPG_CONFIG_RX_SHIFT;
        } else {  /* Otherwise, 1G or above mode */
           data &= ~(UNIMAC_REG_BKP_IPG_CONFIG_RX_MASK);
           data |= 5 << UNIMAC_REG_BKP_IPG_CONFIG_RX_SHIFT;
        }
        BLMI_E_IF_ERROR_RETURN(UNIMAC_WRITE_IPG_HD_BKP_CNTL_REG(mmc, port, data));
    }

    /* Take MAC out of reset */
    BLMI_E_IF_ERROR_RETURN(
        phy_unimac_mac_reset(mmc, port, 0));

    return BLMI_E_NONE;
}

/*
 * Function:
 *    unimac_mac_duplex_gateway_set
 * Purpose:
 *    Set Duplex Gateway mode
 * Parameters:
 *    mmc - mac driver control
 *    dgw - duplex gateway control structure
 * Notes:
 */
int
phy_unimac_mac_duplex_gateway_set(phy_mac_ctrl_t *mmc,
                              phy_mac_duplex_gateway_t *dgw)
{
    int  speed;
    SPEED_MAPPING(dgw->speed, speed);

    /* config line-side MAC */
    BLMI_E_IF_ERROR_RETURN(
        _mac_duplex_gateway_set(mmc, dgw->lport,
                        speed, dgw->duplex, dgw->ipg, dgw->max_frame, 1) );
    /* config switch-side MAC, always full-duplex */
    BLMI_E_IF_ERROR_RETURN(
        _mac_duplex_gateway_set(mmc, dgw->sport,
                        speed, 1          , dgw->ipg, dgw->max_frame, 0) );

    return BLMI_E_NONE;
}

/***************************************************/
/* MAC Driver Functions */
phy_mac_drv_t phy_unimac_drv = {
    phy_unimac_mac_init,
    phy_unimac_mac_reset,
    phy_unimac_mac_auto_cfg,
    phy_unimac_mac_enable_set,
    phy_unimac_mac_enable_get,
    phy_unimac_mac_speed_set,
    phy_unimac_mac_speed_get,
    phy_unimac_mac_duplex_set,
    phy_unimac_mac_duplex_get,
    phy_unimac_mac_update_ipg,
    phy_unimac_mac_max_frame_set,
    phy_unimac_mac_max_frame_get,
    phy_unimac_mac_pause_set,
    NULL,
    phy_unimac_mac_pause_frame_fwd_set,
    phy_unimac_mac_pause_frame_fwd_get,
    phy_unimac_mac_duplex_gateway_set,
    phy_unimac_mac_frame_len_check_enable,
    phy_unimac_mac_rx_enable_set,
    phy_unimac_mac_tx_enable_set,
    phy_unimac_mac_tx_datapath_flush
};

#endif /* defined(INCLUDE_FCMAP) || defined (INCLUDE_MACSEC) */

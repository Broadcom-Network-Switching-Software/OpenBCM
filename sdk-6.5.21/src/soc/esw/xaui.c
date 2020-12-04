/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XAUI Error monitoring features. 
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/boot.h>

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/portmode.h>
#include <soc/ll.h>
#include <soc/xaui.h>
#include <soc/phyctrl.h>
#include <soc/debug.h>
#include <soc/esw/portctrl.h>

#ifdef BCM_XGS3_SWITCH_SUPPORT

/*
 * Function:
 *      soc_xaui_err_sym_detect_set
 * Description:
 *      Enable/Disable XAUI error symbol monitoring feature.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      enable - TRUE, enable |E| monitoring feature on the port.
 *               FALSE, disable |E| monitoring feature on the port.
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - Functionality not available
 */
int
soc_xaui_err_sym_detect_set(int unit, soc_port_t port, int enable)
{
    uint8         phy_addr;
    uint16        octrl, rx_ctrl, rx_ctrl_1g, status;

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }
    /* Map RxAll Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x00f0));

    /* Read Rx Control 1G Register */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x19, &rx_ctrl_1g));
    octrl = rx_ctrl_1g;
    if (enable) {
        /*
         * Enable cgbad_tst, Emon_en, and cgbad_en
         */
        rx_ctrl_1g |= 0x0680;
    } else {
        /*
         * Disable cgbad_tst and Emon_en
         */
        rx_ctrl_1g &= ~0x0600;
    }
    if (octrl != rx_ctrl_1g) {
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x19,
                                           rx_ctrl_1g));
    }

    /* Read Rx Control Register */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x11, &rx_ctrl));
    octrl = rx_ctrl;
    if (enable) {
        /*
         * Enable SigDetected_en, SigDetRestart_en, and
         * SigDetMonitor_en and map PRBS status register at 0x80F0.
         */
        rx_ctrl |= 0x1c47;
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x11, rx_ctrl));

        /*
         * Clear the |E| count in PRBS status register (Clear-on-read)
         */
        SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x10, &status));

        /*
         * Restore original status register select mapping.
         */
        rx_ctrl = octrl | 0x1c40;
        if (rx_ctrl != octrl) {
            SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x11,
                                rx_ctrl));
        }

    }
    /*
     * SigDetected_en, SigDetRestart_en and SigDetMonitor_en are not disabled
     * when disabling |E| monitoring because they are enabled by default.
     * Just made sure to enable them when enabling |E| monitoring.
     */

    /* Restore the block 0 mapping */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_err_sym_detect_get
 * Description:
 *      Get the status of XAUI error symbol monitoring feature.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      enable - (OUT) TRUE, port is enabled, FALSE port is disabled.
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - Functionality not available
 */
int
soc_xaui_err_sym_detect_get(int unit, soc_port_t port, int *enable)
{
    uint8         phy_addr;
    uint16        rx_ctrl;
    uint16        rx_ctrl_1g;

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Map RxAll block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x00f0));

    /*
     * Read Rx Control register and mask
     * SigDetected_en, SigDetRestart_en, and SigDetMonitor_en bits.
     */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x11, &rx_ctrl));
    rx_ctrl &= 0x1c00;

    /*
     * Read Rx Control 1G register and mask
     * cgbad_tst, Emon_en, and cgbad_en bits.
     */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x19, &rx_ctrl_1g));
    rx_ctrl_1g &= 0x0680;

    if (rx_ctrl == 0x1c00 && rx_ctrl_1g == 0x680) {
        *enable = 1;
    } else {
        *enable = 0;
    }

    /* Restore the block 0 mapping */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_err_sym_count
 * Description:
 *      Get the number of |E| symbol in XAUI lanes since last read.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      count - (OUT) Number of |E| error since last read.
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - Functionality not available
 *      SOC_E_CONFIG  - Error symbol detect feature is not enabled
 */
int
soc_xaui_err_sym_count(int unit, soc_port_t port, int *count)
{
    uint8         phy_addr;
    uint16        rx_ctrl, octrl;
    uint16        rx_ctrl_1g;
    uint16        rx_status;

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }
    /*
     * Map Rx All register block
     */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x00f0));

    /*
     * Read Rx Control 1G and Rx Control register to check whether
     * |E| monitoring is enabled.
     */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x19, &rx_ctrl_1g));
    rx_ctrl_1g &= 0x0680;

    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x11, &rx_ctrl));
    octrl = rx_ctrl;
    rx_ctrl &= 0x1c00;

    if (rx_ctrl != 0x1c00 || rx_ctrl_1g != 0x0680) {
        return SOC_E_CONFIG;
    }

    /*
     * If |E| monitor is enabled, map PRPS status register at 0x00F0
     */
    rx_ctrl = octrl | 0x0047;
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x11, rx_ctrl));

    /*
     * Read PRBS status and parse error count field.
     * If sync_status indicates that symbol is not aligned to proper
     * symbol boundry, error count is invalid.
     */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x10, &rx_status));
    if (rx_status & 0x8000) {
        *count = rx_status & 0x3fff;
    } else {
        *count = 0;
    }

    /*
     * Restore original status register mapping at 0x00F0
     */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x11, octrl));

    /*
     * Restore block 0 mapping.
     */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_txbert_enable
 * Description:
 *      Enable Tx BERT in XAUI core.
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      enable - Enable or disable Tx BERT
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_PARAM   - port does not have XAUI core.
 * Note:
 *      This function clears the Tx counters before enabling
 * Tx BERT.
 */ 
int
soc_xaui_txbert_enable(int unit, soc_port_t port, int enable) 
{
    uint8     phy_addr;
    uint16    phy_reg;
    uint16    blk_address;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block address to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Tx BERT Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0020));

    /* Configure Tx BERT */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x10, &phy_reg));
    if (enable) {
        /* Clear the Tx BERT counters */
        phy_reg |= 1 << 0; 
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x10, phy_reg));

        /* Run BERT */
        phy_reg |= 1 << 15;   /* Enable Tx packet generator */
        phy_reg &= ~(1 << 0);  
    } else {
        /* Disable packet generator */
        phy_reg &= ~(1 << 15);
    }
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x10, phy_reg));

    /* Restore block mapping */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_txbert_pkt_count_get
 * Description:
 *      Read total transmitted packet. 
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      count  - Number of packet transmitted 
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_PARAM   - port does not have XAUI core.
 * Note:
 *      The count is only 32 bit. The counter could overflow.
 */ 
int
soc_xaui_txbert_pkt_count_get(int unit, soc_port_t port, uint32 *count)
{
    uint8  phy_addr;
    uint16 pkt_cnt_hi, pkt_cnt_lo;
    uint16 blk_address;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block address to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Tx BERT Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0020));

    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x19, &pkt_cnt_lo));
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1a, &pkt_cnt_hi));

    *count = (pkt_cnt_hi << 16) | pkt_cnt_lo;

    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_txbert_byte_count_get
 * Description:
 *      Read total transmitted byte. 
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      count  - Number of byte transmitted 
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_PARAM   - port does not have XAUI core.
 * Note:
 *      The count is only 32 bit. The counter could overflow.
 */ 
int
soc_xaui_txbert_byte_count_get(int unit, soc_port_t port, uint32 *count)
{
    uint8  phy_addr;
    uint16 byte_cnt_hi, byte_cnt_lo;
    uint16 blk_address;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block address to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Tx BERT Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0020));

    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x17, &byte_cnt_lo));
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x18, &byte_cnt_hi));

    *count = (byte_cnt_hi << 16) | byte_cnt_lo;

    /* Restore original block address */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_rxbert_enable
 * Description:
 *      Enable Rx BERT in XAUI core.
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      enable - Enable or disable Rx BERT
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_PARAM   - port does not have XAUI core.
 * Note:
 *      This function clears the Rx counters before enabling
 * Rx BERT.
 */ 
int
soc_xaui_rxbert_enable(int unit, soc_port_t port, int enable)
{
    uint8   phy_addr;
    uint16  phy_reg;
    uint16  blk_address;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block address to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Block 0 */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0000));

    if (enable) {
        /* Turn off check end functions in xgmiiRcontrol */
        SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x17, &phy_reg));
        phy_reg &= ~(1 << 10);
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x17, phy_reg));

        /* Enable pgen_en in xgxsControl */
        SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x10, &phy_reg));
        phy_reg |= 1 << 15;  /* Enable pattern generator */
        phy_reg |= 1 << 14;  /* Enable pattern comparator */
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x10, phy_reg));

        /* Map Rx BERT Block */
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0030));

        /* Clear the Rx BERT counters */
        phy_reg  = 1 << 1;  /* Clear Rx BERT error counters */
        phy_reg |= 1 << 0;  /* Clear Rx BERT receive counters */
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x10, phy_reg));

        /* Enable packet monitor */
        phy_reg  = 1 << 15; /* Enable Rx packet monitor */
        phy_reg |= 3 << 4;  /* Get the source of Rx BERT from 
                             * clock compensation 
                             */
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x10, phy_reg));
    } else {
        /* Turn on check end function */
        SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x17, &phy_reg));
        phy_reg |= 1 << 10;
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x17, phy_reg));

        /* Disable pgen_en in xgxsControl */
        SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x10, &phy_reg));
        phy_reg &= ~(1 << 15);  /* Disable pattern generator */
        phy_reg &= ~(1 << 14);  /* Disable pattern comparator */
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x10, phy_reg));

        /* Map Rx BERT Block */
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0030));

        /* Disable packet monitor */
        phy_reg  &= ~(1 << 15); /* Disable Rx packet monitor */
        SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x10, phy_reg));
   }

     /* Restore original block address */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_rxbert_pkt_count_get
 * Description:
 *      Read the number of packet received in BERT.
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      count  - Number of packet received. 
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_PARAM   - port does not have XAUI core.
 */ 
int
soc_xaui_rxbert_pkt_count_get(int unit, soc_port_t port, 
                              uint32 *count, int *prbs_lock)
{
    uint8  phy_addr;
    uint16 pkt_cnt_hi, pkt_cnt_lo;
    uint16 blk_address, rx_status;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block address to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Rx BERT Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0030));

    /* Get Rx Packet count */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x19, &pkt_cnt_lo));
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1a, &pkt_cnt_hi));
    *count = (pkt_cnt_hi << 16) | pkt_cnt_lo;

    /* Get PRBS Lock status */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1e, &rx_status));
    *prbs_lock = (rx_status >> 15);

    /* Restore original block address */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_rxbert_byte_count_get
 * Description:
 *      Read the number of byte received in BERT.
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      count  - Number of byte received. 
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_PARAM   - port does not have XAUI core.
 */ 
int
soc_xaui_rxbert_byte_count_get(int unit, soc_port_t port, 
                               uint32 *count, int *prbs_lock)
{
    uint8  phy_addr;
    uint16 byte_cnt_hi, byte_cnt_lo;
    uint16 blk_address, rx_status;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block address to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Rx BERT Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0030));

    /* Get byte count */ 
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x17, &byte_cnt_lo));
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x18, &byte_cnt_hi));
    *count = (byte_cnt_hi << 16) | byte_cnt_lo;

    /* Get PRBS Lock status */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1e, &rx_status));
    *prbs_lock = (rx_status >> 15);

    /* Restore block address */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_rxbert_bit_err_count_get
 * Description:
 *      Read the number of bit error received in BERT.
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      count  - Number of bit error received. 
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_PARAM   - port does not have XAUI core.
 */ 
int
soc_xaui_rxbert_bit_err_count_get(int unit, soc_port_t port, 
                                  uint32 *count, int *prbs_lock)
{
    uint8  phy_addr;
    uint16 phy_reg;
    uint16 blk_address;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block mapping to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Rx BERT Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0030));

    /* Get bit error count */ 
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1b, &phy_reg));
    *count = phy_reg;

    /* Get PRBS Lock status */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1e, &phy_reg));
    *prbs_lock = (phy_reg >> 15);

    /* Restore original block mapping */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_rxbert_byte_err_count_get
 * Description:
 *      Read the number of byte error received in BERT.
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      count  - Number of byte error received. 
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_PARAM   - port does not have XAUI core.
 */ 
int
soc_xaui_rxbert_byte_err_count_get(int unit, soc_port_t port, 
                                   uint32 *count, int *prbs_lock)
{
    uint8  phy_addr;
    uint16 phy_reg;
    uint16 blk_address;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block mappping to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Rx BERT Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0030));

    /* Read byte error count */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1c, &phy_reg));
    *count = phy_reg;

    /* Get PRBS Lock status */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1e, &phy_reg));
    *prbs_lock = (phy_reg >> 15);

    /* Restore original block mapping */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_xaui_rxbert_pkt_err_count_get
 * Description:
 *      Read the number of packet error received in BERT.
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      count  - Number of packet error received. 
 * Return Value:
 *      SOC_E_NONE
 *      SOC_E_PARAM   - port does not have XAUI core.
 */ 
int
soc_xaui_rxbert_pkt_err_count_get(int unit, soc_port_t port, 
                                  uint32 *count, int *prbs_lock)
{
    uint8  phy_addr;
    uint16 phy_reg;
    uint16 blk_address;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block address to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Rx BERT Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x0030));

    /* Get packet error count */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1d, &phy_reg));
    *count = phy_reg;

    /* Get PRBS Lock status */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1e, &phy_reg));
    *prbs_lock = (phy_reg >> 15);

    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
}

int
soc_xaui_config_set(int unit, soc_port_t port, soc_xaui_config_t *config) 
{
    uint8  phy_addr;
    uint16 phy_reg;
    uint16 blk_address;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block address to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Tx All Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x00a0));

    /* Read Tx Driver Configuration*/
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x17, &phy_reg));

    phy_reg &= ~(0xfff0);
    phy_reg |= (_shr_bit_rev8(config->preemphasis) >> 4) << 12;
    phy_reg |= (_shr_bit_rev8(config->idriver) >> 4) << 8;
    phy_reg |= (_shr_bit_rev8(config->ipredriver) >> 4) << 4;
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x17, phy_reg));

    /* Map Rx All Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x00f0));

    /* Read Rx Eq Boost */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1c, &phy_reg));
    phy_reg &= ~(0x0007);
    phy_reg |= config->equalizer_ctrl;
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1c, phy_reg));
 
    /* Restore block address */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    return SOC_E_NONE;
} 

int
soc_xaui_config_get(int unit, soc_port_t port, soc_xaui_config_t *config)
{
    uint8  phy_addr;
    uint16 phy_reg;
    uint16 blk_address;

    if (!IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_port_to_phyaddr(unit, port,
                            &phy_addr));
    } else {
        phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    }

    /* Save block address to restore at the end */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1f, &blk_address));

    /* Map Tx All Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x00a0));

    /* Read Tx Driver Configuration*/
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x17, &phy_reg));

    config->preemphasis = (phy_reg & 0xf000) >> 12;
    config->idriver     = (phy_reg & 0x0f00) >> 8;
    config->ipredriver  = (phy_reg & 0x00f0) >> 4;

    /* Map Rx All Block */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, 0x00f0));

    /* Read Rx Eq Boost */
    SOC_IF_ERROR_RETURN(soc_miim_read(unit, phy_addr, 0x1c, &phy_reg));
    config->equalizer_ctrl = (phy_reg & 0x0007);  

    /* Restore block address */
    SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x1f, blk_address));

    config->preemphasis = (_shr_bit_rev8(config->preemphasis) >> 4);
    config->idriver     = (_shr_bit_rev8(config->idriver) >> 4);
    config->ipredriver  = (_shr_bit_rev8(config->ipredriver) >> 4);

    return SOC_E_NONE;
}

#endif /* BCM_XGS3_SWITCH */

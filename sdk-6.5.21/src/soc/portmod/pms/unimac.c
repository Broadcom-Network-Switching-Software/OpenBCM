/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/portmod/portmod.h>
#include <soc/portmod/unimac.h>
#include <soc/drv.h>

#if defined(PORTMOD_PM4x10Q_SUPPORT) || defined(PORTMOD_PM_QTC_SUPPORT) || defined(PORTMOD_PM4X10_QTC_SUPPORT)


#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT


#define SOC_UNIMAC_SPEED_10     0x0
#define SOC_UNIMAC_SPEED_100    0x1
#define SOC_UNIMAC_SPEED_1000   0x2
#define SOC_UNIMAC_SPEED_2500   0x3

#define SOC_UNIMAC_MAX_FRAME_SIZE   16360
#define SOC_UNIMAC_WAKE_TIMER       17
#define SOC_UNIMAC_LPI_TIMER        4

/* EEE related defination */
#define SOC_UNIMAC_MAX_EEE_SPEED    2500
#define SOC_UNIMAC_EEE_REF_CNT       250


/*
 * Function:
 *      _unimac_soft_reset_and_config_set
 * Purpose:
 *      In register COMMAND_CONFIGr, some field must be set under software reset.
 *      This function checks software reset status and then set these fields.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      config - value for COMMAND_CONFIGr.
 * Returns:
 *      SOC_E_XXX
 */
STATIC
int _unimac_soft_reset_and_config_set (int unit, soc_port_t port, uint32 config)
{
    uint32  command_config;
    int     rx_enable = 0, reset = 0;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    reset = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, SW_RESETf);
    rx_enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);

    if (!reset) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 1));

        /* If receive is already enabled, ensure that it is cleanly disabled in
         * order to provide EOP indication to downstream logic */
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(unimac_rx_enable_set(unit, port, FALSE));
            /* Update config */
            soc_reg_field_set(unit, COMMAND_CONFIGr, &config, RX_ENAf, 0);

            sal_udelay(20);
        }
        /* Put the MAC in reset */
        SOC_IF_ERROR_RETURN(unimac_soft_reset_set(unit, port, TRUE));
        /* Update config */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &config, SW_RESETf, 1);
    }

    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, config));

    if (!reset) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, FLUSH_CONTROLr, port, FLUSHf, 0));
        /* Bring the MAC out of reset */
        SOC_IF_ERROR_RETURN(unimac_soft_reset_set(unit, port, FALSE));
        if (rx_enable) {
            SOC_IF_ERROR_RETURN(unimac_rx_enable_set(unit, port, TRUE));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_init
 * Purpose:
 *      Initialize UniMAC into a known good state.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The initialization speed/duplex is arbitrary and must be
 *      updated by linkscan before enabling the MAC.
 */

int unimac_init(int unit, soc_port_t port, int init_flags)
{
    uint32 command_config, old_command_config, reg_val;
    int    frame_max, ignore_pause;
    int    is_crc_fwd;

    is_crc_fwd = (init_flags & UNIMAC_INIT_F_RX_STRIP_CRC) ? 0 : 1;

    /* Get MAC configurations from config settings. */
    
    frame_max = SOC_UNIMAC_MAX_FRAME_SIZE;
    SOC_IF_ERROR_RETURN(unimac_rx_max_size_set(unit, port, frame_max));
 
    /* First put the MAC in reset and sleep */
    SOC_IF_ERROR_RETURN(unimac_soft_reset_set(unit, port, TRUE));

    /* Do the initialization */
    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    old_command_config = command_config;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, ETH_SPEEDf, SOC_UNIMAC_SPEED_1000);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PROMIS_ENf, 1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAD_ENf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, CRC_FWDf, is_crc_fwd);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAUSE_FWDf, 0);

    /* Ignore pause if using as stack port */
    ignore_pause = IS_ST_PORT(unit, port) ? 1 : 0;
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAUSE_IGNOREf, ignore_pause);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, IGNORE_TX_PAUSEf, ignore_pause);

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ADDR_INSf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, HD_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, LOOP_ENAf, 0);
    
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, NO_LGTH_CHECKf, 1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, LINE_LOOPBACKf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ERR_DISCf, 0);

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, CNTL_FRM_ENAf, 1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, ENA_EXT_CONFIGf, (init_flags & UNIMAC_INIT_F_AUTO_CFG) ? 1 : 0);

    if (init_flags & UNIMAC_INIT_F_AUTO_CFG) {
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_OVERRIDE_RXf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_OVERRIDE_TXf, 1);
    }

    if (old_command_config != command_config) {
        SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
    }

    SOC_IF_ERROR_RETURN(READ_TAG_0r(unit, port, &reg_val));
    soc_reg_field_set(unit, TAG_0r, &reg_val, CONFIG_OUTER_TPID_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TAG_0r(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(READ_TAG_1r(unit, port, &reg_val));
    soc_reg_field_set(unit, TAG_1r, &reg_val, CONFIG_INNER_TPID_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TAG_1r(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(READ_UMAC_TIMESTAMP_ADJUSTr(unit, port, &reg_val));
    soc_reg_field_set(unit, UMAC_TIMESTAMP_ADJUSTr, &reg_val, AUTO_ADJUSTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_UMAC_TIMESTAMP_ADJUSTr(unit, port, reg_val));

    /* Bring the UniMAC out of reset */
    SOC_IF_ERROR_RETURN(unimac_soft_reset_set(unit, port, FALSE));

    reg_val = 0;
    soc_reg_field_set(unit, PAUSE_CONTROLr, &reg_val, ENABLEf, 1);
    soc_reg_field_set(unit, PAUSE_CONTROLr, &reg_val, VALUEf, 0x1ffff);
    SOC_IF_ERROR_RETURN(WRITE_PAUSE_CONTROLr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(WRITE_PAUSE_QUANTr(unit, port, 0xffff));

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_REFRESH_CTRLr(unit, port, &reg_val));
    soc_reg_field_set(unit, MAC_PFC_REFRESH_CTRLr, &reg_val, PFC_REFRESH_ENf, 1);
    soc_reg_field_set(unit, MAC_PFC_REFRESH_CTRLr, &reg_val, PFC_REFRESH_TIMERf, 0xc000);
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_REFRESH_CTRLr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(WRITE_TX_IPG_LENGTHr(unit, port, 12));

    /* Set egress enable */
   

    /* assigning proper setting for EEE feature :
     * Note : GE speed force assigned for timer setting 
     */
    if (soc_feature(unit, soc_feature_eee)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, UMAC_EEE_REF_COUNTr, port, EEE_REF_COUNTf, SOC_UNIMAC_EEE_REF_CNT));
        if (SOC_REG_FIELD_VALID(unit, GMII_EEE_WAKE_TIMERr, GMII_EEE_WAKE_TIMER_FLDf)) {
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, GMII_EEE_WAKE_TIMERr, port, GMII_EEE_WAKE_TIMER_FLDf, SOC_UNIMAC_WAKE_TIMER));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, GMII_EEE_WAKE_TIMERr, port, GMII_EEE_WAKE_TIMERf, SOC_UNIMAC_WAKE_TIMER));
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, GMII_EEE_DELAY_ENTRY_TIMERr, port, GMII_EEE_LPI_TIMERf, SOC_UNIMAC_LPI_TIMER));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      unimac_deinit
 * Purpose:
 *      Deinitialize UniMAC into a default state.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */

int unimac_deinit(int unit, soc_port_t port)
{
    uint32 command_config, old_command_config, reg_val;


    SOC_IF_ERROR_RETURN(WRITE_TX_IPG_LENGTHr(unit, port, 0));

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_REFRESH_CTRLr(unit, port, &reg_val));
    soc_reg_field_set(unit, MAC_PFC_REFRESH_CTRLr, &reg_val, PFC_REFRESH_ENf, 0);
    soc_reg_field_set(unit, MAC_PFC_REFRESH_CTRLr, &reg_val, PFC_REFRESH_TIMERf, 0x7fff);
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_REFRESH_CTRLr(unit, port, reg_val));


    reg_val = 0;
    soc_reg_field_set(unit, PAUSE_CONTROLr, &reg_val, ENABLEf, 1);
    soc_reg_field_set(unit, PAUSE_CONTROLr, &reg_val, VALUEf, 0xffff);
    SOC_IF_ERROR_RETURN(WRITE_PAUSE_CONTROLr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(unimac_soft_reset_set(unit, port, TRUE));

    SOC_IF_ERROR_RETURN(READ_UMAC_TIMESTAMP_ADJUSTr(unit, port, &reg_val));
    soc_reg_field_set(unit, UMAC_TIMESTAMP_ADJUSTr, &reg_val, AUTO_ADJUSTf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_UMAC_TIMESTAMP_ADJUSTr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(READ_TAG_0r(unit, port, &reg_val));
    soc_reg_field_set(unit, TAG_0r, &reg_val, CONFIG_OUTER_TPID_ENABLEf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_TAG_0r(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(READ_TAG_1r(unit, port, &reg_val));
    soc_reg_field_set(unit, TAG_1r, &reg_val, CONFIG_INNER_TPID_ENABLEf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_TAG_1r(unit, port, reg_val));

    /* Do the deinitialization */
    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    old_command_config = command_config;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, ETH_SPEEDf, 0x2);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PROMIS_ENf, 1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAD_ENf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, CRC_FWDf, 1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAUSE_FWDf, 1);

    /* Ignore pause if using as stack port */
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAUSE_IGNOREf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, IGNORE_TX_PAUSEf, 0);

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ADDR_INSf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, HD_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, LOOP_ENAf, 0);
    
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, NO_LGTH_CHECKf, 1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, LINE_LOOPBACKf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ERR_DISCf, 0);

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, CNTL_FRM_ENAf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, ENA_EXT_CONFIGf, 0);

    
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_OVERRIDE_RXf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_OVERRIDE_TXf, 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, 0);

    if (old_command_config != command_config) {
        SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
    }


    SOC_IF_ERROR_RETURN(WRITE_FRM_LENGTHr(unit, port, 0x5ee));

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_rx_max_size_set
 * Description:
 *      Set the maximum receive frame size for the GE port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      value - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
int unimac_rx_max_size_set(int unit, soc_port_t port, int value)
{
    uint32 command_config, rx_ena;
    int speed = 0;
   
    if (IS_ST_PORT(unit, port)) {
        value += 16; /* Account for 16 bytes of Higig2 header */
    }

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    rx_ena = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);

    /* If Rx is enabled then disable RX */
    if (rx_ena) {
        /* Disable RX */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
        SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));

        /* Wait for maximum frame receiption time(for 16K) based on speed */
        SOC_IF_ERROR_RETURN(unimac_speed_get(unit, port, &speed));
        switch (speed) {
        case 2500:
            sal_usleep(55);
            break;
        case 1000:
            sal_usleep(131);
            break;
        case 100:
            sal_usleep(1310);
            break;
        case 10:
            sal_usleep(13100);
            break;
        default:
            break;
        }
    }

    SOC_IF_ERROR_RETURN(WRITE_FRM_LENGTHr(unit, port, value));

    /* if Rx was enabled before, restore it */
    if (rx_ena) {
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 1);
        SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_rx_max_size_get
 * Description:
 *      Set the maximum receive frame size for the GE port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      value - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      Depending on chip or port type the actual maximum receive frame size
 *      might be slightly higher.
 */
int unimac_rx_max_size_get(int unit, soc_port_t port, int *value)
{
    uint32  frame_length;
 
    SOC_IF_ERROR_RETURN(READ_FRM_LENGTHr(unit, port, &frame_length));

    *value = (int) frame_length;
    if (IS_ST_PORT(unit, port)) {
        *value -= 16;  /* Account for 16 bytes of Higig2 header */
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_loopback_set
 * Purpose:
 *      Set GE MAC into/out-of loopback mode
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      enable - Boolean: true -> loopback mode, false -> normal operation
 * Returns:
 *      SOC_E_XXX
 */
int unimac_loopback_set(int unit, soc_port_t port, int enable)
{
    uint32 		command_config, old_command_config;

    /* The MAC loopback will not work when ENA_EXT_CONFIGf=1 because hardware puts MAC
     * in reset state if link is down.
     * A simple workaround is to put PHY in loopback mode to get the link. This brings
     * MAC out of reset state, and then clears ENA_EXT_CONFIGf.
     */

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    old_command_config = command_config;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, LOOP_ENAf, enable ? 1 : 0);
    if (command_config == old_command_config) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(_unimac_soft_reset_and_config_set(unit, port, command_config));

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_loopback_get
 * Purpose:
 *      Get current GE MAC loopback mode setting.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit
 *      enable - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
int unimac_loopback_get(int unit, soc_port_t port, int *enable)
{
    uint32 		command_config;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    *enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, LOOP_ENAf);

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_soft_reset_set
 * Purpose:
 *      Set UniMAC reset state 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      enable - TRUE to put MAC in reset, FALSE to take MAC out of reset.
 * Returns:
 *      SOC_E_XXX
 */
int unimac_soft_reset_set(int unit, soc_port_t port, int enable)
{
    uint32 command_config;
    int reset_sleep_usec;

    if (SAL_BOOT_QUICKTURN) {
        reset_sleep_usec = 50000;
    } else {
        /* Minimum of 5 clocks period with the slowest clock is required
         * between each reset step.
         *   10Mbps (2.5MHz) = 2000ns
         *  100Mbps  (25MHz) = 200ns
         * 1000Mbps (125MHz) = 40ns
         * 2500Mbps (133MHz) = 37.5ns
         */
        reset_sleep_usec = 20;  /* choose max delay, change to 20 for ECC error issue */
    }
 
    /* SIDE EFFECT: TX and RX are enabled when SW_RESET is set. */
    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, enable);
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));

    sal_udelay(reset_sleep_usec);

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_soft_reset_get
 * Purpose:
 *      Get UniMAC reset state 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if in reset, FALSE if out of reset
 * Returns:
 *      SOC_E_XXX
 */
int unimac_soft_reset_get(int unit, soc_port_t port, int *enable)
{
    uint32 command_config;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    *enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, SW_RESETf);
    
    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      flags - UNIMAC_ENABLE_SET_FLAGS_TX_EN or UNIMAC_ENABLE_SET_FLAGS_RX_EN
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 */
int unimac_enable_set(int unit, soc_port_t port, int flags, int enable)
{
    uint32              command_config, tx_ena, rx_ena;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    tx_ena = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, TX_ENAf);
    rx_ena = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);

    if ((tx_ena && rx_ena && enable) || (!tx_ena && !rx_ena && !enable)) {
        /* Nothing to do */
        return SOC_E_NONE;
    }

    if (enable) {
        /* Bring the MAC out of reset */
        SOC_IF_ERROR_RETURN(unimac_soft_reset_set(unit, port, FALSE));
        soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, SW_RESETf, 0);

        /* if enable, assert RX_ENA and TX_ENA */
        if (flags & UNIMAC_ENABLE_SET_FLAGS_TX_EN) {
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ENAf, 1);
        }
        if (flags & UNIMAC_ENABLE_SET_FLAGS_RX_EN) {
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 1);
        }
        if (!(flags & UNIMAC_ENABLE_SET_FLAGS_RX_EN) && !(flags & UNIMAC_ENABLE_SET_FLAGS_TX_EN)) {
            /* if no flag was set, it means both Rx and Tx (according to enable_set/enable_get convention) */
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ENAf, 1);
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));
    } else {
        /* de-assert RX_ENA and/or TX_ENA */
        if (flags & UNIMAC_ENABLE_SET_FLAGS_TX_EN) {
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ENAf, 0);
        }
        if (flags & UNIMAC_ENABLE_SET_FLAGS_RX_EN) {
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
        }
        if (!(flags & UNIMAC_ENABLE_SET_FLAGS_RX_EN) && !(flags & UNIMAC_ENABLE_SET_FLAGS_TX_EN)) {
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, TX_ENAf, 0);
            soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, RX_ENAf, 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));

        sal_udelay(20);
        SOC_IF_ERROR_RETURN(unimac_soft_reset_set(unit, port, TRUE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_uni_enable_get
 * Purpose:
 *      Get UniMAC enable state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      flags - UNIMAC_ENABLE_SET_FLAGS_TX_EN or UNIMAC_ENABLE_SET_FLAGS_RX_EN
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */
int unimac_enable_get(int unit, soc_port_t port, int flags, int *enable)
{
    uint32 command_config;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    if (flags & UNIMAC_ENABLE_SET_FLAGS_TX_EN) {
        *enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, TX_ENAf);
    }else{
        *enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, RX_ENAf);
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      unimac_tx_average_ipg_set
 * Purpose:
 *      Set the IPG appropriate for current duplex
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      ipg_val  - IPG value to write.
 * Notes:
 *      The current duplex is read from the hardware registers.
 */
int unimac_tx_average_ipg_set(int unit, soc_port_t port, int ipg_val)
{
    int tx_en;

    /* Modifiable under sw_reset or Tx disable */
    SOC_IF_ERROR_RETURN(unimac_tx_enable_get(unit, port, &tx_en));
    if (tx_en) {
        SOC_IF_ERROR_RETURN(unimac_tx_enable_set(unit, port, FALSE));
    }

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 64 bytes (i.e. multiple of 8 bits) */
    ipg_val = ipg_val < 64 ? 64 : (ipg_val > 512 ? 512 : (ipg_val + 7) & (0x7f << 3));

    SOC_IF_ERROR_RETURN(WRITE_TX_IPG_LENGTHr(unit, port, ipg_val/8));
    if (tx_en) {
        SOC_IF_ERROR_RETURN(unimac_tx_enable_set(unit, port, TRUE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_tx_average_ipg_get
 * Purpose:
 *      Set the IPG appropriate for current duplex
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      ipg_val  - (OUT) IPG value to write.
 * Returns:
 *      SOC_E_XXX
 */
int unimac_tx_average_ipg_get(int unit, soc_port_t port, int *ipg_val)
{
    uint32 reg_val;
    
    SOC_IF_ERROR_RETURN(READ_TX_IPG_LENGTHr(unit, port, &reg_val));

    if (SOC_REG_FIELD_VALID(unit, TX_IPG_LENGTHr, TX_IPG_LENGTH_FLDf)) {
        *ipg_val = soc_reg_field_get(unit, TX_IPG_LENGTHr, reg_val, TX_IPG_LENGTH_FLDf) * 8;
    } else {
        *ipg_val = soc_reg_field_get(unit, TX_IPG_LENGTHr, reg_val, TX_IPG_LENGTHf) * 8;
    }

    return SOC_E_NONE;
}



/*
 * Function:
 *      unimac_encap_set
 * Purpose:
 *      Set the port encapsulation mode.
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      encap - encapsulation mode
 * Returns:
 *      SOC_E_XXX
 */
int unimac_encap_set(int unit, soc_port_t port, portmod_encap_t encap)
{
    return (encap == SOC_ENCAP_IEEE) ? SOC_E_NONE : SOC_E_PARAM;
}

/*
 * Function:
 *      unimac_encap_get
 * Purpose:
 *      Get the port encapsulation mode.
 * Parameters:
 *      unit - unit #.
 *      port - port # on unit.
 *      encap - (INT) encapsulation mode
 * Returns:
 *      SOC_E_XXX
 */
int unimac_encap_get(int unit, soc_port_t port, portmod_encap_t *encap)
{
    *encap = SOC_ENCAP_IEEE;

    return SOC_E_NONE;
}


/*
 * Function:
 *      unimac_duplex_set
 * Purpose:
 *      Set UniMAC in the specified duplex mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Programs an IFG time appropriate to speed and duplex.
 */

int unimac_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint32 	command_config, old_command_config;
    int     speed;
    
    SOC_IF_ERROR_RETURN(unimac_speed_get(unit, port, &speed));

    if (speed >= 1000) {
        /* program the bit full-duplex for speed 1000 or 2500Mbps. */
        duplex = 1;
    }

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    old_command_config = command_config;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, HD_ENAf, duplex ? 0 : 1);
    if (command_config == old_command_config) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(_unimac_soft_reset_and_config_set(unit, port, command_config));

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_duplex_get
 * Purpose:
 *      Get UniMAC duplex mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */

int unimac_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint32 command_config;
    int    speed;

    SOC_IF_ERROR_RETURN(unimac_speed_get(unit, port, &speed));

    if ((1000 == speed) || (2500 == speed)) {
        *duplex = TRUE;
    } else {
        SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
        *duplex = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, HD_ENAf) ? FALSE : TRUE;
    }

    return SOC_E_NONE;
}



/*
 * Function:
 *      unimac_speed_set
 * Purpose:
 *      Set UniMAC in the specified speed.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      speed - 10,100,1000, 2500 for speed.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Programs an IFG time appropriate to speed and duplex.
 */
int unimac_speed_set(int unit, soc_port_t port, int speed)
{
    uint32  command_config;
    int  speed_select = 0;

    switch (speed) {
    case 10:
        speed_select = SOC_UNIMAC_SPEED_10;
        break;
    /* support non-standard speed in Broadreach mode */
    case 20:
    case 25:
    case 33:
    case 50:
	/* fall through to case 100 */
    case 100:
        speed_select = SOC_UNIMAC_SPEED_100;
        break;
    case 1000:
        speed_select = SOC_UNIMAC_SPEED_1000;
        break;
    case 2500:
        speed_select = SOC_UNIMAC_SPEED_2500;
        break;
    case 0:
        return (SOC_E_NONE);              /* Support NULL PHY */            
    default:
        return (SOC_E_CONFIG);
    }

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, ETH_SPEEDf, speed_select);

    SOC_IF_ERROR_RETURN(_unimac_soft_reset_and_config_set(unit, port, command_config));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      unimac_speed_get
 * Purpose:
 *      Get GE MAC speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number on unit.
 *      speed - (OUT) speed in Mb (10/100/1000)
 * Returns:
 *      SOC_E_NONE
 */

int unimac_speed_get(int unit, soc_port_t port, int *speed)
{
    uint32	command_config, uni_speed;
    int     rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    uni_speed = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, ETH_SPEEDf);

    switch(uni_speed) {
    case SOC_UNIMAC_SPEED_10:
        *speed = 10;
        break;
    case SOC_UNIMAC_SPEED_100:
        *speed = 100;
        break;
    case SOC_UNIMAC_SPEED_1000:
        *speed = 1000;
        break;
    case SOC_UNIMAC_SPEED_2500:
        *speed = 2500;
        break;
    default:
        rv = SOC_E_INTERNAL;
    }

    return(rv);
}


int unimac_rx_enable_set (int unit, soc_port_t port, int enable)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &reg_val));
    soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_ENAf, enable ? 1: 0);
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, reg_val));

    return(SOC_E_NONE);
}

int unimac_rx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &reg_val));
    *enable = soc_reg_field_get(unit, COMMAND_CONFIGr, reg_val, RX_ENAf);

    return(SOC_E_NONE);
}


/*
 * Function:
 *      unimac_mac_sa_set
 * Purpose:
 *      Set GE MAC source address for transmitted PAUSE frame
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      mac  - MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */

int
unimac_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint32 mac_0, mac_1;

    mac_0 = _shr_uint32_read(&mac[0]);
    mac_1 = _shr_uint16_read(&mac[4]);

    SOC_IF_ERROR_RETURN(WRITE_MAC_0r(unit, port, mac_0));
    SOC_IF_ERROR_RETURN(WRITE_MAC_1r(unit, port, mac_1));
    
    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_mac_sa_get
 * Purpose:
 *      Return current GE MAC source address for transmitted PAUSE frames
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      mac  - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
int
unimac_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint32 mac_0, mac_1;

    SOC_IF_ERROR_RETURN(READ_MAC_0r(unit, port, &mac_0));
    SOC_IF_ERROR_RETURN(READ_MAC_1r(unit, port, &mac_1));

    mac[0] = (uint8)(mac_0 >> 24);
    mac[1] = (uint8)(mac_0 >> 16);
    mac[2] = (uint8)(mac_0 >> 8);
    mac[3] = (uint8)(mac_0 >> 0);
    mac[4] = (uint8)(mac_1 >> 8);
    mac[5] = (uint8)(mac_1 >> 0);

    return SOC_E_NONE;
}

/*
 * Function:
 *      unimac_tx_preamble_length_set
 * Purpose:
 *      Set the transmit preamble excluding SFD.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port # on unit.
 *      length - length of preamble transmit.
 * Returns:
 *      SOC_E_XXX
 */
int unimac_tx_preamble_length_set(int unit, soc_port_t port, int length)
{
    uint32 reg_val;

    if((length > 7) || length < 2){
        LOG_VERBOSE(BSL_LS_SOC_COMMON,(BSL_META_U(unit, "runt size should be between 2 and 7. got %d"), length));
        return (SOC_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_TX_PREAMBLEr(unit, port, &reg_val));
    soc_reg_field_set(unit, TX_PREAMBLEr, &reg_val, TX_PREAMBLEf, length);

    return (WRITE_TX_PREAMBLEr(unit, port, reg_val));
}


/* 
 * unimac_eee_set
 *
 * @brief EEE Control and Timer set
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int unimac_eee_set(int unit, int port, const portmod_eee_t* eee)
{
    int speed;
    uint32 reg_val;
    soc_reg_t reg;
    soc_field_t field;

    SOC_IF_ERROR_RETURN(READ_UMAC_EEE_CTRLr(unit ,port, &reg_val));
    soc_reg_field_set(unit, UMAC_EEE_CTRLr, &reg_val, EEE_ENf, eee->enable);
    SOC_IF_ERROR_RETURN(WRITE_UMAC_EEE_CTRLr(unit ,port, reg_val));

    SOC_IF_ERROR_RETURN(unimac_speed_get(unit, port, &speed));
    if (speed > SOC_UNIMAC_MAX_EEE_SPEED) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,(BSL_META_U(unit, "max speed for EEE is 2500Mbps")));
        return (SOC_E_PARAM);
    }

    reg = (speed >= 1000) ? GMII_EEE_WAKE_TIMERr : MII_EEE_WAKE_TIMERr;
    if (SOC_REG_FIELD_VALID(unit, GMII_EEE_WAKE_TIMERr, GMII_EEE_WAKE_TIMER_FLDf) &&
        SOC_REG_FIELD_VALID(unit, MII_EEE_WAKE_TIMERr, MII_EEE_WAKE_TIMER_FLDf) ) {
        field = (speed >= 1000) ? GMII_EEE_WAKE_TIMER_FLDf : MII_EEE_WAKE_TIMER_FLDf;
    } else {
        field = (speed >= 1000) ? GMII_EEE_WAKE_TIMERf : MII_EEE_WAKE_TIMERf;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &reg_val));
    soc_reg_field_set(unit, reg, &reg_val, field, eee->tx_wake_time);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, reg_val));

    reg = (speed >= 1000) ? GMII_EEE_DELAY_ENTRY_TIMERr : MII_EEE_DELAY_ENTRY_TIMERr;
    field = (speed >= 1000) ? GMII_EEE_LPI_TIMERf : MII_EEE_LPI_TIMERf;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &reg_val));
    soc_reg_field_set(unit, reg, &reg_val, field, eee->tx_idle_time);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, reg_val));

    return SOC_E_NONE;
}


/*
 * unimac_eee_get
 *
 * @brief EEE Control and Timer get
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int unimac_eee_get(int unit, int port, const portmod_eee_t* eee)
{
    int speed;
    uint32 reg_val;
    soc_reg_t reg;
    soc_field_t field;
    portmod_eee_t* pEEE = (portmod_eee_t*)eee;

    SOC_IF_ERROR_RETURN(READ_UMAC_EEE_CTRLr(unit, port, &reg_val));
    pEEE->enable = soc_reg_field_get (unit, UMAC_EEE_CTRLr, reg_val, EEE_ENf);

    SOC_IF_ERROR_RETURN(unimac_speed_get(unit, port, &speed));
    if (speed > SOC_UNIMAC_MAX_EEE_SPEED) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,(BSL_META_U(unit, "max speed for EEE is 2500Mbps")));
        return (SOC_E_PARAM);
    }

    reg = (speed >= 1000) ? GMII_EEE_WAKE_TIMERr : MII_EEE_WAKE_TIMERr;
    if (SOC_REG_FIELD_VALID(unit, GMII_EEE_WAKE_TIMERr, GMII_EEE_WAKE_TIMER_FLDf) &&
        SOC_REG_FIELD_VALID(unit, MII_EEE_WAKE_TIMERr, MII_EEE_WAKE_TIMER_FLDf) ) {
        field = (speed >= 1000) ? GMII_EEE_WAKE_TIMER_FLDf : MII_EEE_WAKE_TIMER_FLDf;
    } else {
        field = (speed >= 1000) ? GMII_EEE_WAKE_TIMERf : MII_EEE_WAKE_TIMERf;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &reg_val));
    pEEE->tx_wake_time = soc_reg_field_get(unit, reg, reg_val, field);

    reg = (speed >= 1000) ? GMII_EEE_DELAY_ENTRY_TIMERr : MII_EEE_DELAY_ENTRY_TIMERr;
    field = (speed >= 1000) ? GMII_EEE_LPI_TIMERf : MII_EEE_LPI_TIMERf;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &reg_val));
    pEEE->tx_idle_time = soc_reg_field_get(unit, reg, reg_val, field);

    return(SOC_E_NONE);
}


/* 
 * unimac_pfc_config_set
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int unimac_pfc_config_set (int unit, int port, 
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint32 reg_val; 
    int rx_en;
    portmod_pfc_control_t pfc_ctrl;

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_TYPEr(unit ,port, &reg_val));
    soc_reg_field_set(unit, MAC_PFC_TYPEr, &reg_val, PFC_ETH_TYPEf, pfc_cfg->type);
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_TYPEr(unit ,port, reg_val));

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_OPCODEr(unit ,port, &reg_val));
    soc_reg_field_set(unit, MAC_PFC_OPCODEr, &reg_val, PFC_OPCODEf, pfc_cfg->opcode);
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_OPCODEr(unit ,port, reg_val));

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_0r(unit, port, &reg_val));
    reg_val &= 0x00ffffff;
    reg_val |= (pfc_cfg->da_oui & 0xff) << 24;
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_DA_0r(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_1r(unit, port, &reg_val));
    soc_reg_field_set(unit, MAC_PFC_DA_1r, &reg_val, PFC_MACDA_1f, pfc_cfg->da_oui >> 8);
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_DA_1r(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_0r(unit, port, &reg_val));
    reg_val &= 0xff000000;
    reg_val |= pfc_cfg->da_nonoui & 0xffffff;
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_DA_0r(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(unimac_rx_enable_get(unit, port, &rx_en));
    if (rx_en) {
        SOC_IF_ERROR_RETURN(unimac_rx_enable_set(unit, port, FALSE));
    }
    SOC_IF_ERROR_RETURN(unimac_pfc_control_get(unit, port, &pfc_ctrl));
    if (pfc_ctrl.rx_enable) {
        pfc_ctrl.rx_enable = FALSE;
        SOC_IF_ERROR_RETURN(unimac_pfc_control_set(unit, port, &pfc_ctrl));
        pfc_ctrl.rx_enable = TRUE;
    }

    /* Modifiable under sw_reset or PFC Rx disable. */
    SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port, &reg_val));
    soc_reg_field_set(unit, MAC_PFC_CTRLr, &reg_val, RX_PASS_PFC_FRMf, pfc_cfg->rxpass ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_CTRLr_REG32(unit, port, reg_val));

    if (pfc_ctrl.rx_enable) {
        SOC_IF_ERROR_RETURN(unimac_pfc_control_set(unit, port, &pfc_ctrl));
    }
    if (rx_en) {
        SOC_IF_ERROR_RETURN(unimac_rx_enable_set(unit, port, TRUE));
    }

    return (SOC_E_NONE);
}



/* 
 * unimac_pfc_config_get
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int unimac_pfc_config_get (int unit, int port, 
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint32 reg_val;
    uint32 fval, fval0, fval1;
    portmod_pfc_config_t* pcfg = (portmod_pfc_config_t*) pfc_cfg;

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_TYPEr(unit, port, &reg_val));
    pcfg->type = soc_reg_field_get(unit, MAC_PFC_TYPEr, reg_val, PFC_ETH_TYPEf);

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_OPCODEr(unit, port, &reg_val));
    pcfg->opcode = soc_reg_field_get(unit, MAC_PFC_OPCODEr, reg_val, PFC_OPCODEf);

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_0r(unit, port, &reg_val));
    fval0 = soc_reg_field_get(unit, MAC_PFC_DA_0r, reg_val, PFC_MACDA_0f);

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_1r(unit, port, &reg_val));
    fval1 = soc_reg_field_get(unit, MAC_PFC_DA_1r, reg_val, PFC_MACDA_1f);
    pcfg->da_oui = (fval0 >> 24) | (fval1 << 8);

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_DA_0r(unit, port, &reg_val));
    fval = soc_reg_field_get(unit, MAC_PFC_DA_0r, reg_val, PFC_MACDA_0f);

    pcfg->da_nonoui = fval & 0xffffff;

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port, &reg_val));
    pcfg->rxpass = soc_reg_field_get(unit, MAC_PFC_CTRLr, reg_val, RX_PASS_PFC_FRMf);

    return (SOC_E_NONE);
}


/* 
 * unimac_diag_fifo_status_get
 *
 * @brief get port timestamps in fifo 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  diag_info       - 
 */
int unimac_diag_fifo_status_get (int unit, int port, 
                                const portmod_fifo_status_t* diag_info)
{
    uint32 reg_val;
    portmod_fifo_status_t* pinfo =(portmod_fifo_status_t*)diag_info;

    SOC_IF_ERROR_RETURN(READ_TS_STATUS_CNTRLr(unit, port, &reg_val));

    if (soc_reg_field_get(unit, TS_STATUS_CNTRLr, reg_val, TX_TS_FIFO_EMPTYf)) {
        return SOC_E_EMPTY;
    }

    SOC_IF_ERROR_RETURN(READ_TX_TS_SEQ_IDr(unit, port, &reg_val));
    if (soc_reg_field_get(unit, TX_TS_SEQ_IDr, reg_val, TSTS_VALIDf) == 0) {
        return SOC_E_EMPTY;
    }

    SOC_IF_ERROR_RETURN(READ_TX_TS_DATAr(unit, port, &reg_val));
    pinfo->timestamps_in_fifo = soc_reg_field_get(unit, TX_TS_DATAr, reg_val, TX_TS_DATAf);

    return (SOC_E_NONE);
}

int unimac_pass_pause_frame_set(int unit, int port, uint32 value)
{
    uint32 command_config;
    int rx_en;

    SOC_IF_ERROR_RETURN(unimac_rx_enable_get(unit, port, &rx_en));
    if (rx_en) {
        SOC_IF_ERROR_RETURN(unimac_rx_enable_set(unit, port, FALSE));
    }

    /* Modifiable under sw_reset or Rx disable */
    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAUSE_FWDf, value ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));

    if (rx_en) {
        SOC_IF_ERROR_RETURN(unimac_rx_enable_set(unit, port, TRUE));
    }

    return(SOC_E_NONE);
}

int unimac_pass_pause_frame_get(int unit, int port, uint32* value)
{
    uint32 command_config;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    *value = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, PAUSE_FWDf);

    return(SOC_E_NONE);

}

int unimac_pass_control_frame_set(int unit, int port, int value)
{
    uint32 command_config;
    int rx_en;

    SOC_IF_ERROR_RETURN(unimac_rx_enable_get(unit, port, &rx_en));
    if (rx_en) {
        SOC_IF_ERROR_RETURN(unimac_rx_enable_set(unit, port, FALSE));
    }

    /*Modifiable under sw_reset or Rx disable.*/
    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, CNTL_FRM_ENAf, value ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, command_config));

    if (rx_en) {
        SOC_IF_ERROR_RETURN(unimac_rx_enable_set(unit, port, TRUE));
    }

    return(SOC_E_NONE);
}

int unimac_pass_control_frame_get(int unit, int port, int* value)
{
    uint32 command_config;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    *value = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, CNTL_FRM_ENAf);

    return(SOC_E_NONE);
}

int
unimac_mac_ctrl_set(int unit, int port, uint32 mac_ctrl)
{
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, mac_ctrl));
    return (0);
}

int
unimac_mac_ctrl_get(int unit, int port, uint32 *mac_ctrl, int *rx_en)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &reg_val));
    *mac_ctrl = reg_val;
    *rx_en    = soc_reg_field_get(unit, COMMAND_CONFIGr, reg_val, RX_ENAf);

    return (SOC_E_NONE);
}

int unimac_drain_cell_get (int unit, int port, portmod_drain_cells_t *drain_cells)
{   
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port, &reg_val));
    drain_cells->rx_pfc_en = soc_reg_field_get(unit, MAC_PFC_CTRLr, reg_val, PFC_RX_ENBLf);

    
    drain_cells->llfc_en = 0;
                                
    SOC_IF_ERROR_RETURN(READ_MAC_MODEr(unit, port , &reg_val));
    drain_cells->rx_pause  = soc_reg_field_get(unit, MAC_MODEr, reg_val, MAC_RX_PAUSEf);
    drain_cells->tx_pause  = soc_reg_field_get(unit, MAC_MODEr, reg_val, MAC_TX_PAUSEf);

    return (0);
} 

int unimac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag, int inner_vlan_tag)
{
    uint32 reg_val = 0;

    SOC_IF_ERROR_RETURN(READ_TAG_1r(unit, port, &reg_val));
    if(inner_vlan_tag == -1) {
        soc_reg_field_set(unit, TAG_1r, &reg_val, CONFIG_INNER_TPID_ENABLEf, 0);
    } else {
        soc_reg_field_set(unit, TAG_1r, &reg_val, FRM_TAG_1f, inner_vlan_tag);
        soc_reg_field_set(unit, TAG_1r, &reg_val, CONFIG_INNER_TPID_ENABLEf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_TAG_1r(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(READ_TAG_0r(unit, port, &reg_val));
    if(outer_vlan_tag == -1) {
        soc_reg_field_set(unit, TAG_0r, &reg_val, CONFIG_OUTER_TPID_ENABLEf, 0);
    } else {
        soc_reg_field_set(unit, TAG_0r, &reg_val, FRM_TAG_0f, outer_vlan_tag);
        soc_reg_field_set(unit, TAG_0r, &reg_val, CONFIG_OUTER_TPID_ENABLEf, 1);
    }

    return (WRITE_TAG_0r(unit, port, reg_val));
}


int unimac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag, int *inner_vlan_tag)
{
    uint32 reg_val;
    uint32 enable = 0;

    SOC_IF_ERROR_RETURN(READ_TAG_1r(unit, port, &reg_val));

    enable = soc_reg_field_get(unit, TAG_1r, reg_val, CONFIG_INNER_TPID_ENABLEf);

    *inner_vlan_tag = enable ? soc_reg_field_get(unit, TAG_1r, reg_val, FRM_TAG_1f) : -1;

    SOC_IF_ERROR_RETURN(READ_TAG_0r(unit, port, &reg_val));

    enable = soc_reg_field_get(unit, TAG_0r, reg_val, CONFIG_OUTER_TPID_ENABLEf);

    *outer_vlan_tag = enable ? soc_reg_field_get(unit, TAG_0r, reg_val, FRM_TAG_0f) : -1;

    return (SOC_E_NONE);
} 


int unimac_pause_control_set(int unit, soc_port_t port, const portmod_pause_control_t *control)
{
    uint32 		command_config, old_command_config;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));
    old_command_config = command_config;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, PAUSE_IGNOREf, control->rx_enable ? 0 : 1);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &command_config, IGNORE_TX_PAUSEf, control->tx_enable ? 0 : 1);

    if (command_config == old_command_config) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(_unimac_soft_reset_and_config_set(unit, port, command_config));

    return SOC_E_NONE;
}


int unimac_pause_control_get(int unit, soc_port_t port, portmod_pause_control_t *control)
{
    uint32 		command_config;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    control->rx_enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, PAUSE_IGNOREf) ?  FALSE : TRUE;
    control->tx_enable = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, IGNORE_TX_PAUSEf) ?  FALSE : TRUE;

    return SOC_E_NONE;
}


int unimac_pfc_control_set(int unit, soc_port_t port, const portmod_pfc_control_t *control)
{
    uint32 reg_val = 0;

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_REFRESH_CTRLr(unit, port, &reg_val));

    if(control->refresh_timer){
        soc_reg_field_set (unit, MAC_PFC_REFRESH_CTRLr, &reg_val, PFC_REFRESH_TIMERf, control->refresh_timer);
        soc_reg_field_set (unit, MAC_PFC_REFRESH_CTRLr, &reg_val, PFC_REFRESH_ENf, 1);
    } else {
        soc_reg_field_set (unit, MAC_PFC_REFRESH_CTRLr, &reg_val, PFC_REFRESH_ENf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_REFRESH_CTRLr(unit, port ,reg_val));

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port ,&reg_val));
    soc_reg_field_set(unit, MAC_PFC_CTRLr, &reg_val, PFC_STATS_ENf, control->stats_en);
    soc_reg_field_set(unit, MAC_PFC_CTRLr, &reg_val, FORCE_PFC_XONf, control->force_xon);
    soc_reg_field_set(unit, MAC_PFC_CTRLr, &reg_val, PFC_TX_ENBLf, control->tx_enable);
    soc_reg_field_set(unit, MAC_PFC_CTRLr, &reg_val, PFC_RX_ENBLf, control->rx_enable);
    SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_CTRLr_REG32(unit, port , reg_val));

    SOC_IF_ERROR_RETURN(READ_PFC_XOFF_TIMERr(unit, port, &reg_val));
    if (SOC_REG_FIELD_VALID(unit, PFC_XOFF_TIMERr, PFC_XOFF_TIMER_FLDf)) {
        soc_reg_field_set(unit, PFC_XOFF_TIMERr, &reg_val, PFC_XOFF_TIMER_FLDf, control->xoff_timer);
    } else {
        soc_reg_field_set(unit, PFC_XOFF_TIMERr, &reg_val, PFC_XOFF_TIMERf, control->xoff_timer);
    }

    return(WRITE_PFC_XOFF_TIMERr(unit, port ,reg_val));
}


int unimac_pfc_control_get(int unit, soc_port_t port, portmod_pfc_control_t *control)
{
    uint32 reg_val = 0;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_REFRESH_CTRLr(unit, port , &reg_val));

    refresh_enable = soc_reg_field_get (unit, MAC_PFC_REFRESH_CTRLr, reg_val, PFC_REFRESH_ENf);
    if (refresh_enable) {
        refresh_timer  = soc_reg_field_get (unit, MAC_PFC_REFRESH_CTRLr, reg_val, PFC_REFRESH_TIMERf);
    }
    control->refresh_timer = refresh_timer;

    SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr_REG32(unit, port, &reg_val));
    control->stats_en   = soc_reg_field_get (unit, MAC_PFC_CTRLr, reg_val, PFC_STATS_ENf);
    control->force_xon  = soc_reg_field_get (unit, MAC_PFC_CTRLr, reg_val, FORCE_PFC_XONf);
    control->rx_enable  = soc_reg_field_get (unit, MAC_PFC_CTRLr, reg_val, PFC_RX_ENBLf);
    control->tx_enable  = soc_reg_field_get (unit, MAC_PFC_CTRLr, reg_val, PFC_TX_ENBLf);

    SOC_IF_ERROR_RETURN(READ_PFC_XOFF_TIMERr(unit, port , &reg_val));
    if (SOC_REG_FIELD_VALID(unit, PFC_XOFF_TIMERr, PFC_XOFF_TIMER_FLDf)) {
        control->xoff_timer = soc_reg_field_get (unit, PFC_XOFF_TIMERr, reg_val, PFC_XOFF_TIMER_FLDf);
    } else {
        control->xoff_timer = soc_reg_field_get (unit, PFC_XOFF_TIMERr, reg_val, PFC_XOFF_TIMERf);
    }

    return (SOC_E_NONE);
}

int unimac_tx_enable_set(int unit, soc_port_t port, int enable)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &reg_val));
    soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, TX_ENAf, enable ? 1: 0);
    SOC_IF_ERROR_RETURN(WRITE_COMMAND_CONFIGr(unit, port, reg_val));

    return(SOC_E_NONE);
}

int unimac_tx_enable_get(int unit, soc_port_t port, int *enable)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &reg_val));
    *enable = soc_reg_field_get(unit, COMMAND_CONFIGr, reg_val, TX_ENAf);

    return(SOC_E_NONE);
}

int unimac_reset_check(int unit, int port, int enable, int *reset)
{
    uint32 ctrl, octrl;

    *reset = 1;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &ctrl));
    octrl = ctrl;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &ctrl, TX_ENAf, enable ? 1: 0);
    soc_reg_field_set(unit, COMMAND_CONFIGr, &ctrl, RX_ENAf, enable ? 1: 0);

    if (ctrl == octrl) {
        if (enable) {
            *reset = 0;
        } else {
            if (soc_reg_field_get(unit, COMMAND_CONFIGr, ctrl, SW_RESETf)) {
                *reset = 0;
            }
        }
    }

    return SOC_E_NONE;
}


#undef _ERR_MSG_MODULE_NAME


#endif /* PORTMOD_PM4X10Q_SUPPORT */







/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 $Id$
 */

#ifndef BCM_IUNIMAC_H
#define BCM_IUNIMAC_H
#include "phy_mac_ctrl.h"

/***************************************************************
 * Unimac driver functions.
 */
/*
 * Function:
 *    imacsec_plp_base_t_unimac_init
 * Purpose:
 *    Initialize the MAC
 * Initialization is done as a part of the PHY initialization sequence
 * that initializes MAC to default settings.
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * Notes:
 * The MAC will be put in Auto config mode at initilization
 * MAX Frame Size = 0x3fff
 * IPG = 12
 * TX and RX are enabled
 */
int
imacsec_plp_base_t_unimac_init(int unit, soc_port_t port,int if_side);

/*
 * Function:
 *    imacsec_plp_base_t_unimac_auto_cfg
 * Purpose:
 *    This function puts the MAC in auto config if auto_cfg is 1.
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param auto_cfg - Auto configuration flag
 *               1 = Auto configuration
 *               0 = Forced mode
 * Notes:
 */
int
imacsec_plp_base_t_unimac_auto_cfg(int unit, soc_port_t port,int if_side, int auto_cfg);

/*
 * Function:
 *    imacsec_plp_base_t_unimac_reset
 * Purpose:
 *    This function puts the MAC in reset if reset is 1 else takes MAC
 *    out of reset
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   reset    - Reset MAC
 *               1 = Put in Reset
 *               0 = Take out of reset
 * Notes:
 */
int
imacsec_plp_base_t_unimac_reset(int unit, soc_port_t port,int if_side, int reset);

/*
 * Function:
 *    imacsec_plp_base_t_unimac_enable_set
 * Purpose:
 *    Enable/Disable MAC
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   enable - Enable flag
 *               1 = Enable MAC
 *               0 = Disable MAC
 * Notes:
 */
int
imacsec_plp_base_t_unimac_enable_set(int unit, soc_port_t port,int if_side, int enable);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_enable_get
 * Purpose:
 *    Get the enable status of MAC
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param enable   - (OUT)Enable flag
 *               1 = Enable MAC
 *               0 = Disable MAC
 * Notes:
 */
int
imacsec_plp_base_t_unimac_enable_get(int unit, soc_port_t port, int if_side, int *enable);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_speed_set
 * Purpose:
 *    Set Speed of the MAC
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   speed    - 1000 - 1000Mbps
 *               100  - 100Mbps
 *               10   - 10Mbps
 * Notes:
 */
int
imacsec_plp_base_t_unimac_speed_set(int unit, soc_port_t port, int if_side, int speed);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_speed_get
 * Purpose:
 *    Get Speed of the MAC
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   speed    - (OUT) 1000 - 1000Mbps
 *                     100  - 100Mbps
 *                     10   - 10Mbps
 * Notes:
 */
int
imacsec_plp_base_t_unimac_speed_get(int unit, soc_port_t port, int if_side, int *speed);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_duplex_set
 * Purpose:
 *    Set Duplex of the MAC
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   duplex   - 1 - Full Duplex
 *                   0 - Half Duplex
 * Notes:
 */
int
imacsec_plp_base_t_unimac_duplex_set(int unit, soc_port_t port, int if_side, int duplex);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_duplex_get
 * Purpose:
 *    Get Duplex of the MAC
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   duplex   - (OUT) 0 - Full Duplex
 *                     1 - Half Duplex
 * Notes:
 */
int
imacsec_plp_base_t_unimac_duplex_get(int unit, soc_port_t port, int if_side, int *duplex);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_pause_set
 * Purpose:
 *    Set pause of the MAC
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   pause   - 1 - Pause enable
 *               0 - Pause disable
 * Notes:
 */
int
imacsec_plp_base_t_unimac_pause_set(int unit, soc_port_t port, int if_side, int pause);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_update_ipg
 * Purpose:
 *    Set IPG of the MAC
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   ipg      - IPG value
 * Notes:
 */
int
imacsec_plp_base_t_unimac_update_ipg(int unit, soc_port_t port, int if_side, int ipg);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_max_frame_set
 * Purpose:
 *    Set Max frame of the MAC
 *
 * Maximum Frame size shouldn;t be less 48 and cannot be greater than
 * 0x3fff
 *
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   max_frame - Max frame value
 * Notes:
 */
int
imacsec_plp_base_t_unimac_max_frame_set(int unit, soc_port_t port, int if_side, uint32 max_frame);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_max_frame_get
 * Purpose:
 *    Get Max frame of the MAC
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   max_frame - (OUT) Max frame
 * Notes:
 */
int
imacsec_plp_base_t_unimac_max_frame_get(int unit, soc_port_t port, int if_side, uint32 *max_frame);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_pause_frame_fwd_set
 * Purpose:
 *    Enable/Disable Pause Frame Forward
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   pause_fwd - Pause forward
 *                1 - Forward pause
 *                0 - Consume PAUSE.
 * Notes:
 */
int
imacsec_plp_base_t_unimac_pause_frame_fwd_set(int unit, soc_port_t port, int if_side, int pause_fwd);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_pause_frame_fwd_get
 * Purpose:
 *    Get the status of Pause Frame Forward
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * @param   pause_fwd - Pause forward
 *                1 - Forward pause
 *                0 - Consume PAUSE.
 * Notes:
 */
int
imacsec_plp_base_t_unimac_pause_frame_fwd_get(int unit, soc_port_t port, int if_side, int *pause_fwd);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_switch_side_set_params
 * Purpose:
 *    To set Switch side speed, duplex and pause parameters
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
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
imacsec_plp_base_t_unimac_switch_side_set_params(int unit, soc_port_t port,
                                    phy_mac_policy_t switch_side_policy,
                                    int speed, int duplex, int pause_enable);
/*
 * Function:
 *    imacsec_plp_base_t_unimac_frame_len_check_enable
 * Purpose:
 *    Enable the frame length check error mask in RSV vector
 *    to signal error to encryption block
 * Parameters:
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 *    enable - Enable the error mask
 *    1 -  enable
 *    0 -  disable
 * Notes:
 */
int
imacsec_plp_base_t_unimac_frame_len_check_enable(int unit, soc_port_t port,
                                                 int if_side, int enable);

/*
 * Function:
 *    imacsec_plp_base_t_unimac_rx_enable_set
 * Purpose:
 *    Enable/Disable MAC RX
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE

 *    enable - Enable the Rx
 *    1 -  enable
 *    0 -  disable
 * Notes:
 */
int
imacsec_plp_base_t_unimac_rx_enable_set(int unit, soc_port_t port, int if_side, int enable);

/*
 * Function:
 *    imacsec_plp_base_t_unimac_tx_enable_set
 * Purpose:
 *    Enable/Disable MAC TX
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE

 *    enable - Enable the Tx
 *    1 -  enable
 *    0 -  disable
 * Notes:
 */
int
imacsec_plp_base_t_unimac_tx_enable_set(int unit, soc_port_t port, int if_side, int enable);

/*
 * Function:
 *    imacsec_plp_base_t_unimac_tx_datapath_flush
 * Purpose:
 *    TX-FIFO flush control enable
 * Parameters:
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param if_side (input) - Select the side
 *  0       LINE_SIDE
 *  1       SYSTEM_SIDE
 * Notes: Before calling TX datapath flush user has to disable pause function
 *  once TX_FIFO is empty reenable the pause function
 */
int
imacsec_plp_base_t_unimac_tx_datapath_flush(int unit, soc_port_t port, int if_side, int enable);

#endif /* BCM_IUNIMAC_H */

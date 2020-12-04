/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XMAC driver
 */

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)

#include <sal/core/libc.h>
#include <sal/core/spl.h>
#include <sal/core/boot.h>

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/portmode.h>
#include <soc/ll.h>
#include <soc/counter.h>
#include <soc/phyctrl.h>
#include <soc/debug.h>


#define JUMBO_MAXSZ              0x3fe8 /* Max legal value (per regsfile) */

/*
 * Function:
 *      mac_il_init
 * Purpose:
 *      Initialize Xmac into a known good state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_il_init(int unit, soc_port_t port)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_enable_set(int unit, soc_port_t port, int enable)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_enable_get
 * Purpose:
 *      Get MAC enable state
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_enable_get(int unit, soc_port_t port, int *enable)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_duplex_set
 * Purpose:
 *      Set  XMAC in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_il_duplex_set(int unit, soc_port_t port, int duplex)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_duplex_get
 * Purpose:
 *      Get XMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_duplex_get(int unit, soc_port_t port, int *duplex)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_pause_set
 * Purpose:
 *      Configure XMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_pause_get
 * Purpose:
 *      Return the pause ability of XMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause
 *      pause_rx - Boolean: receive pause
 *      pause_mac - MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_speed_set
 * Purpose:
 *      Set XMAC in the specified speed.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - 2500, 3000, 10000, 12000 for speed.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_speed_set(int unit, soc_port_t port, int speed)
{
    SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port, phyEventSpeed, speed));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_speed_get
 * Purpose:
 *      Get XMAC speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mb (2500/3000/10000/12000)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_speed_get(int unit, soc_port_t port, int *speed)
{
    *speed = 50000;

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_loopback_set
 * Purpose:
 *      Set a XMAC into/out-of loopback mode
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Note:
 *      On Xmac, when setting loopback, we enable the TX/RX function also.
 *      Note that to test the PHY, we use the remote loopback facility.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_loopback_set(int unit, soc_port_t port, int lb)
{

  return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_loopback_get
 * Purpose:
 *      Get current XMAC loopback mode setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_loopback_get(int unit, soc_port_t port, int *lb)
{

  return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_pause_addr_set
 * Purpose:
 *      Configure PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t m)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_pause_addr_get
 * Purpose:
 *      Retrieve PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 * NOTE: We always write the same thing to TX & RX SA
 *       so, we just return the contects on RXMACSA.
 */
STATIC int
mac_il_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t m)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_interface_set
 * Purpose:
 *      Set a XMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - requested mode not supported.
 * Notes:
 */
STATIC int
mac_il_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_interface_get
 * Purpose:
 *      Retrieve XMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
mac_il_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_MII;

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
mac_il_frame_max_set(int unit, soc_port_t port, int size)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_frame_max_get
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
mac_il_frame_max_get(int unit, soc_port_t port, int *size)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_ifg_set
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg - number of bits to use for average inter-frame gap
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
STATIC int
mac_il_ifg_set(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int ifg)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_ifg_get
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
STATIC int
mac_il_ifg_get(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int *ifg)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_loopback_remote_set
 * Purpose:
 *      Set the XMAC into remote-loopback state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) loopback enable state
 * Returns:
 *      SOC_E_XXX
 */
int
mac_il_loopback_remote_set(int unit, soc_port_t port, int lb)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_encap_set
 * Purpose:
 *      Set the XMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (IN) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_encap_set(int unit, soc_port_t port, int mode)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_encap_get
 * Purpose:
 *      Get the XMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_encap_get(int unit, soc_port_t port, int *mode)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_control_set
 * Purpose:
 *      To configure MAC control properties.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                  int value)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_control_get
 * Purpose:
 *      To get current MAC control setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                  int *value)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_il_ability_local_get
 * Purpose:
 *      Return the abilities of XMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_il_ability_local_get(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    return SOC_E_NONE;
}

/* Exported XMAC driver structure */
mac_driver_t soc_mac_il = {
    "Interlaken Driver",           /* drv_name */
    mac_il_init,                   /* md_init  */
    mac_il_enable_set,             /* md_enable_set */
    mac_il_enable_get,             /* md_enable_get */
    mac_il_duplex_set,             /* md_duplex_set */
    mac_il_duplex_get,             /* md_duplex_get */
    mac_il_speed_set,              /* md_speed_set */
    mac_il_speed_get,              /* md_speed_get */
    mac_il_pause_set,              /* md_pause_set */
    mac_il_pause_get,              /* md_pause_get */
    mac_il_pause_addr_set,         /* md_pause_addr_set */
    mac_il_pause_addr_get,         /* md_pause_addr_get */
    mac_il_loopback_set,           /* md_lb_set */
    mac_il_loopback_get,           /* md_lb_get */
    mac_il_interface_set,          /* md_interface_set */
    mac_il_interface_get,          /* md_interface_get */
    NULL,                         /* md_ability_get - Deprecated */
    mac_il_frame_max_set,          /* md_frame_max_set */
    mac_il_frame_max_get,          /* md_frame_max_get */
    mac_il_ifg_set,                /* md_ifg_set */
    mac_il_ifg_get,                /* md_ifg_get */
    mac_il_encap_set,              /* md_encap_set */
    mac_il_encap_get,              /* md_encap_get */
    mac_il_control_set,            /* md_control_set */
    mac_il_control_get,            /* md_control_get */
    mac_il_ability_local_get,      /* md_ability_local_get */
    NULL                           /* md_timesync_tx_info_get */
 };

#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) */

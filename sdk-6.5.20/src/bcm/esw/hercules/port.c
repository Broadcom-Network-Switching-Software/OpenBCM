/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Hercules port function implementations
 */

#include <soc/defs.h>

#if defined(BCM_HERCULES_SUPPORT) || defined(BCM_HUMV_SUPPORT)|| \
        defined(BCM_CONQUEROR_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)

#include <soc/ptable.h>

#include <bcm/port.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/hercules.h>

int
bcm_hercules_port_cfg_get(int unit, bcm_port_t port, bcm_port_cfg_t *cfg)
{
    sal_memset(cfg, 0, sizeof(*cfg));
    cfg->pc_cml = PVP_CML_SWITCH;
    cfg->pc_stp_state = PVP_STP_FORWARDING;
    cfg->pc_vlan = 1;
    SOC_PBMP_ASSIGN(cfg->pc_pbm, PBMP_PORT_ALL(unit));

    /*
     * Interpret spanning tree state;  From Daniel Tai:
     *
     * TX_EN in MAC_TXCTRL
     * RX_EN in MAC_RXCTRL
     *
     * Ports may be enabled/disabled by programming the BigMAC.
     *
     * DISABLED  = both TX/RX disabled on MAC
     * BLOCKING  = RX enabled, TX disabled, all tables programmed to
     *             drop execept for CPU packets
     * LISTENING = RX enabled, TX enabled, all tables programmed to
     *             drop execept for CPU packets
     * LEARNING  = N/A
     * FORWARDING = both TX/RX enabled, all tables programmed
     *              normally
     */

    return BCM_E_NONE;
}

int
bcm_hercules_port_cfg_set(int unit, bcm_port_t port, bcm_port_cfg_t *cfg)
{
#if defined(HERCULES_DEBUG_CHECKING)
    /*
     * Check the following are 0:
     *    pc_mirror_ing, pc_vlan, pc_pbm, pc_ut_pbm, pc_l3_enable,
     *    pc_new_pri, pc_remap_pri_en, pc_dse_mode, pc_dscp,
     *    pc_frame_type, pc_ether_type,
     *
     * Issues:
     *    pc_cml, pc_cpu, pc_disc, pc_bpdu_disable, pc_trunk, pc_tgid,
     */
#endif
    /*
     * Settable parameters include:
     *     pc_stp_state:  Get rx/tx enable state from mac registers
     */

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_hercules_port_cfg_init
 * Purpose:
 *	Initialize the port tables according to Initial System
 *	Configuration (see init.c)
 * Parameters:
 *	unit - StrataSwitch unit number.
 *	port - Port number
 *	vd - Initial VLAN data information
 */

int
bcm_hercules_port_cfg_init(int unit, bcm_port_t port, bcm_vlan_data_t *vd)
{
    return BCM_E_NONE;
}

/* Placeholder routines for unsupported functionality */

int
bcm_hercules_port_rate_egress_set(int unit, int port,
				  uint32 kbits_sec,
				  uint32 kbits_burst, uint32 mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_port_rate_egress_get(int unit, int port,
				  uint32 *kbits_sec,
				  uint32 *kbits_burst, uint32 *mode)
{
    return BCM_E_UNAVAIL;
}


#endif	/* BCM_HERCULES_SUPPORT */

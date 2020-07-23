/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       phy8806x.c
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include "phy8806x_xmod_api.h"
#define PHY8806X_DEBUG              0

#ifdef PHYMOD_PHY8806X_SUPPORT

/* ! Byte swap unsigned short */
uint16_t swap_uint16( uint16_t val )
{
    return (val << 8) | (val >> 8 );
}
/* ! Byte swap unsigned int32 */
uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

#ifdef BE_HOST
#define HOST2LE32(_x) (swap_uint32((_x)))
#define LE2HOST32(_x) (swap_uint32((_x)))
#define HOST2LE16(_x) (swap_uint16((_x)))
#define LE2HOST16(_x) (swap_uint16((_x)))
#else
#define HOST2LE32(_x) (_x)
#define LE2HOST32(_x) (_x)
#define HOST2LE16(_x) (_x)
#define LE2HOST16(_x) (_x)
#endif

static int
_phy_8806x_xmod_command(const phymod_access_t* access, int mode_opcode,  uint32_t xmodtxbuff[], int xmodtxlen, uint32_t xmodrxbuff[], int xmodrxlen)
{
    xmod_buf_desc bd;

    if (PHYMOD_ACC_EXT_ACC(access)) {
        bd.pa = access;
        bd.mode_opcode = mode_opcode;
        bd.xmodtxbuff = xmodtxbuff;
        bd.xmodtxlen = xmodtxlen;
        bd.xmodrxbuff = xmodrxbuff;
        bd.xmodrxlen = xmodrxlen;

        return (*PHYMOD_ACC_EXT_ACC(access))(&bd);
    }

    return PHYMOD_E_UNAVAIL;
}

int phy8806x_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    return PHYMOD_E_NONE;    
}


int phy8806x_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    return PHYMOD_E_NONE;
}


int phy8806x_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    return PHYMOD_E_NONE;
}


int phy8806x_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    return PHYMOD_E_NONE;
}


int phy8806x_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return PHYMOD_E_NONE;
}


int phy8806x_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{       
    return PHYMOD_E_UNAVAIL;
}


int phy8806x_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    return PHYMOD_E_NONE; 
}


int phy8806x_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv=PHYMOD_E_NONE;
    xmod_firmware_lane_config_t x_fw_config;

    x_fw_config.SystemSide = (phy->port_loc == phymodPortLocSys) ? 1 : 0;

    x_fw_config.LaneConfigFromPCS = HOST2LE32(fw_config.LaneConfigFromPCS);
    x_fw_config.AnEnabled = HOST2LE32(fw_config.AnEnabled);
    x_fw_config.DfeOn = HOST2LE32(fw_config.DfeOn);
    x_fw_config.ForceBrDfe = HOST2LE32(fw_config.ForceBrDfe);
    x_fw_config.LpDfeOn = HOST2LE32(fw_config.LpDfeOn);
    x_fw_config.MediaType = HOST2LE32(fw_config.MediaType);
    x_fw_config.UnreliableLos = HOST2LE32(fw_config.UnreliableLos);
    x_fw_config.ScramblingDisable = HOST2LE32(fw_config.ScramblingDisable);
    x_fw_config.Cl72AutoPolEn = HOST2LE32(fw_config.Cl72AutoPolEn);
    x_fw_config.Cl72RestTO = HOST2LE32(fw_config.Cl72RestTO);
    x_fw_config.ForceExtenedReach = HOST2LE32(fw_config.ForceExtenedReach);
    x_fw_config.ForceNormalReach = HOST2LE32(fw_config.ForceNormalReach);
    x_fw_config.LpPrecoderEnabled = HOST2LE32(fw_config.LpPrecoderEnabled);
    x_fw_config.ForcePAM4Mode = HOST2LE32(fw_config.ForcePAM4Mode);
    x_fw_config.ForceNRZMode = HOST2LE32(fw_config.ForceNRZMode);
    x_fw_config.SystemSide = HOST2LE32(x_fw_config.SystemSide);

    /* write args to xmod buffer */
    buftxptr = (uint8_t *) xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_fw_config, sizeof(xmod_firmware_lane_config_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_FW_LANE_CFG_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_FW_LANE_CFG_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


int phy8806x_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    uint8_t *buftxptr;
    int xmodrxlen, rv;
    int xmodtxlen;
    xmod_firmware_lane_config_t x_fw_config;
    uint32_t sys_side;

    sys_side = (phy->port_loc == phymodPortLocSys) ? 1 : 0;

    sys_side = HOST2LE32(sys_side);

    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &sys_side, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_FW_LANE_CFG_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_FW_LANE_CFG_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_FW_LANE_CFG_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_fw_config, sizeof(xmod_firmware_lane_config_t));

    fw_config->LaneConfigFromPCS = HOST2LE32(x_fw_config.LaneConfigFromPCS);
    fw_config->AnEnabled = HOST2LE32(x_fw_config.AnEnabled);
    fw_config->DfeOn = HOST2LE32(x_fw_config.DfeOn);
    fw_config->ForceBrDfe = HOST2LE32(x_fw_config.ForceBrDfe);
    fw_config->LpDfeOn = HOST2LE32(x_fw_config.LpDfeOn);
    fw_config->MediaType = HOST2LE32(x_fw_config.MediaType);
    fw_config->UnreliableLos = HOST2LE32(x_fw_config.UnreliableLos);
    fw_config->ScramblingDisable = HOST2LE32(x_fw_config.ScramblingDisable);
    fw_config->Cl72AutoPolEn = HOST2LE32(x_fw_config.Cl72AutoPolEn);
    fw_config->Cl72RestTO = HOST2LE32(x_fw_config.Cl72RestTO);
    fw_config->ForceExtenedReach = HOST2LE32(x_fw_config.ForceExtenedReach);
    fw_config->ForceNormalReach = HOST2LE32(x_fw_config.ForceNormalReach);
    fw_config->LpPrecoderEnabled = HOST2LE32(x_fw_config.LpPrecoderEnabled);
    fw_config->ForcePAM4Mode = HOST2LE32(x_fw_config.ForcePAM4Mode);
    fw_config->ForceNRZMode = HOST2LE32(x_fw_config.ForceNRZMode);

    return rv;
}


/* reset pll sequencer
 * flags - unused parameter
 */
int phy8806x_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    return PHYMOD_E_UNAVAIL;
}


int phy8806x_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* pll_multiplier)
{
    return PHYMOD_E_NONE;
}


/* reset rx sequencer 
 * flags - unused parameter
 */
int phy8806x_phy_rx_restart(const phymod_phy_access_t* phy)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;
    xmod_tx_t  xmod_tx;

    xmod_tx.sys_side = (phy->port_loc == phymodPortLocSys) ? 1 : 0; /* int8_t, no swap */

    xmod_tx.pre   = (int8_t)tx->pre;   /* int8_t, no swap */
    xmod_tx.main  = (int8_t)tx->main;  /* int8_t, no swap */
    xmod_tx.post  = (int8_t)tx->post;  /* int8_t, no swap */
    xmod_tx.post2 = (int8_t)tx->post2; /* int8_t, no swap */
    xmod_tx.post3 = (int8_t)tx->post3; /* int8_t, no swap */
    xmod_tx.amp   = (int8_t)tx->amp;   /* int8_t, no swap */

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &xmod_tx, sizeof(xmod_tx_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_TX_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_TX_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


int phy8806x_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;
    xmod_tx_t  xmod_tx;

    PHYMOD_MEMSET(&xmod_tx, 0, sizeof(xmod_tx));
    xmod_tx.sys_side = (phy->port_loc == phymodPortLocSys) ? 1 : 0; /* int8_t, no swap */

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &xmod_tx, sizeof(xmod_tx_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_TX_GET); /* Same as GET_XMOD_ETHERNET_CMD_OUT_LEN. */
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_TX_GET),
                             xmodtxbuff, xmodtxlen, xmodtxbuff, xmodtxlen);

    /* retrieve the argument from the xmod rx buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    READ_XMOD_ARG_BUFF(buftxptr, &xmod_tx, sizeof(xmod_tx_t));

    tx->pre = xmod_tx.pre;     /* int8_t, no swap */
    tx->main = xmod_tx.main;   /* int8_t, no swap */
    tx->post = xmod_tx.post;   /* int8_t, no swap */
    tx->post2 = xmod_tx.post2; /* int8_t, no swap */
    tx->post3 = xmod_tx.post3; /* int8_t, no swap */
    tx->amp = xmod_tx.amp;     /* int8_t, no swap */

    return rv;
}


int phy8806x_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv, i;
    xmod_rx_t  xmod_rx;

    xmod_rx.sys_side = (phy->port_loc == phymodPortLocSys) ? 1 : 0; /* int8_t, no swap */

    xmod_rx.vga.enable = HOST2LE32(rx->vga.enable);
    xmod_rx.vga.value = HOST2LE32(rx->vga.value);

    xmod_rx.num_of_dfe_taps = HOST2LE32(rx->num_of_dfe_taps); /* number of elements in DFE array */

    for (i = 0; i < XMOD_NUM_DFE_TAPS; i++) {
        xmod_rx.dfe[i].enable = HOST2LE32(rx->dfe[i].enable);
        xmod_rx.dfe[i].value = HOST2LE32(rx->dfe[i].value);
    }

    xmod_rx.peaking_filter.enable = HOST2LE32(rx->peaking_filter.enable);
    xmod_rx.peaking_filter.value = HOST2LE32(rx->peaking_filter.value);

    xmod_rx.low_freq_peaking_filter.enable = HOST2LE32(rx->low_freq_peaking_filter.enable);
    xmod_rx.low_freq_peaking_filter.value = HOST2LE32(rx->low_freq_peaking_filter.value);

    xmod_rx.rx_adaptation_on = HOST2LE32(rx->rx_adaptation_on);

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &xmod_rx, sizeof(xmod_rx_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_RX_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_RX_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


int phy8806x_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv, i;
    xmod_rx_t  xmod_rx;

    PHYMOD_MEMSET(&xmod_rx, 0, sizeof(xmod_rx));
    xmod_rx.sys_side = (phy->port_loc == phymodPortLocSys) ? 1 : 0; /* int8_t, no swap */

    /* write args to xmod buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    WRITE_XMOD_ARG_BUFF(bufrxptr, &xmod_rx, sizeof(xmod_rx_t));
    xmodrxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_RX_GET); /* Same as GET_XMOD_ETHERNET_CMD_OUT_LEN. */
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_RX_GET),
                             xmodrxbuff, xmodrxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &xmod_rx, sizeof(xmod_rx_t));

    rx->vga.enable = LE2HOST32(xmod_rx.vga.enable);
    rx->vga.value = LE2HOST32(xmod_rx.vga.value);

    rx->num_of_dfe_taps =  LE2HOST32(xmod_rx.num_of_dfe_taps); /* number of elements in DFE array */

    for (i = 0; i < XMOD_NUM_DFE_TAPS; i++) {
        rx->dfe[i].enable =  LE2HOST32(xmod_rx.dfe[i].enable);
        rx->dfe[i].value =  LE2HOST32(xmod_rx.dfe[i].value);
    }

    rx->peaking_filter.enable =  LE2HOST32(xmod_rx.peaking_filter.enable);
    rx->peaking_filter.value =  LE2HOST32(xmod_rx.peaking_filter.value);

    rx->low_freq_peaking_filter.enable =  LE2HOST32(xmod_rx.low_freq_peaking_filter.enable);
    rx->low_freq_peaking_filter.value =  LE2HOST32(xmod_rx.low_freq_peaking_filter.value);

    rx->rx_adaptation_on = LE2HOST32(xmod_rx.rx_adaptation_on);

    return rv;
}


int phy8806x_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    return PHYMOD_E_NONE; 
}

int phy8806x_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{   
    return PHYMOD_E_NONE;
}


int phy8806x_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{   
    return PHYMOD_E_NONE;
}


int phy8806x_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{       
    return PHYMOD_E_NONE;
}


int phy8806x_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{   
    return PHYMOD_E_NONE;
}


int phy8806x_phy_tx_disable_set(const phymod_phy_access_t* phy, uint32_t tx_disable)
{       
    return PHYMOD_E_NONE;
}


int phy8806x_phy_tx_disable_get(const phymod_phy_access_t* phy, uint32_t* tx_disable)
{
    return PHYMOD_E_NONE;
}

int phy8806x_reset_interface(const phymod_phy_access_t* phy)
{
#ifdef INCLUDE_PHY_8806X
    int rv;

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_RESET_INTF),
                                0, 0, NULL, 0);

    return rv;
#else
    return PHYMOD_E_NONE;
#endif
}

int phy8806x_port_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv=PHYMOD_E_NONE;
    xmod_port_enable_control_t x_port_en_ctrl;

    enable =  HOST2LE32(enable);
    x_port_en_ctrl.enable = enable;

    /* write args to xmod buffer */
    buftxptr = (uint8_t *) xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_port_en_ctrl, sizeof(xmod_port_enable_control_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PORT_ENABLE_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PORT_ENABLE_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


int phy8806x_port_enable_get(const phymod_phy_access_t* phy, uint32_t *enable)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;
    xmod_port_enable_control_t x_port_en_ctrl;

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PORT_ENABLE_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PORT_ENABLE_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_port_en_ctrl, sizeof(x_port_en_ctrl));

    *enable = HOST2LE32(x_port_en_ctrl.enable);

    return rv;
}



int phy8806x_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t fec_en)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;
    xmod_fec_t  fec_st;

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;

    fec_st.enable = fec_en & 0xFF;
    fec_st.sys_side = 0;
    if (fec_en & PORT_SYS_SIDE_CTRL)
        fec_st.sys_side = 1;

    WRITE_XMOD_ARG_BUFF(buftxptr, &fec_st, sizeof(xmod_fec_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_FEC_ENABLE_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_FEC_ENABLE_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


int phy8806x_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* fec_en)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen;
    uint8_t *bufrxptr;
    int xmodrxlen, rv;
    uint32_t sys_side = 0;

    if (*fec_en & PORT_SYS_SIDE_CTRL) {
        sys_side = 1;
    }
    
    *fec_en = 0;
    sys_side = HOST2LE32(sys_side);

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &sys_side, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_FEC_ENABLE_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_FEC_ENABLE_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_FEC_ENABLE_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, fec_en, sizeof(uint32_t));

    *fec_en = LE2HOST32(*fec_en);

    return rv;
}


int phy8806x_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    uint8_t x_flags;
    xmod_phy_inf_config_t x_config;
    int xmodtxlen, rv;
    uint16_t port_flags = 0;

    /* setting up xmod xmod_phy_interface_config_set:                                             */
    /* int xmod_phy_interface_config_set( IN uint8_t flags, IN xmod_phy_inf_config_t* config )    */
    if (phy->port_loc == phymodPortLocSys) {
        port_flags = (XMOD_SYS_FLAG << XMOD_FLAGS_SHIFT);
    } else if (phy->port_loc == phymodPortLocLine) {
        port_flags = (XMOD_LINE_FLAG << XMOD_FLAGS_SHIFT);
    }
    
    /* write args to xmod buffer */
    x_flags = flags;
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_flags, sizeof(uint8_t));
    x_config.interface_type   = config->interface_type;
    x_config.device_aux_modes = config->device_aux_modes;
    x_config.ref_clock        = config->ref_clock;
    x_config.data_rate        = HOST2LE32(config->data_rate);
    x_config.interface_modes  = HOST2LE32(config->interface_modes);
    x_config.pll_divider_req  = HOST2LE16(config->pll_divider_req);
  
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_config, sizeof(xmod_phy_inf_config_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_INTERFACE_CONFIG_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_INTERFACE_CONFIG_SET) | port_flags,
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


/*flags- unused parameter*/
int phy8806x_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr, *bufrxptr;
    uint8_t x_flags;
    xmod_phy_inf_config_t x_config;
    int xmodtxlen, xmodrxlen, rv;
    uint16_t port_flags = 0;
    
    /* setting up xmod xmod_phy_interface_config_get: */
    /* int xmod_phy_interface_config_get( IN uint8_t flags, OUT xmod_phy_inf_config_t* config  )    */
    if (phy->port_loc == phymodPortLocSys) {
        port_flags = (XMOD_SYS_FLAG << XMOD_FLAGS_SHIFT); /* sys interface */
    } else if (phy->port_loc == phymodPortLocLine) {
        port_flags = (XMOD_LINE_FLAG << XMOD_FLAGS_SHIFT); /* line interface */
    }

    /* write args to xmod buffer */
    x_flags = flags;
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_flags, sizeof(uint8_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_INTERFACE_CONFIG_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_INTERFACE_CONFIG_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_INTERFACE_CONFIG_GET) | port_flags,
                           xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_config, sizeof(xmod_phy_inf_config_t));
    config->interface_type   = x_config.interface_type;
    config->device_aux_modes = x_config.device_aux_modes;
    config->ref_clock        = x_config.ref_clock;
    config->data_rate        = LE2HOST32(x_config.data_rate);
    config->interface_modes  = LE2HOST32(x_config.interface_modes);
    config->pll_divider_req  = LE2HOST16(x_config.pll_divider_req);

    return rv;
}


int phy8806x_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;
    xmod_cl72_t  cl72_st;    

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;

    cl72_st.enable = cl72_en & 0xFF;
    cl72_st.sys_side = 0;
    if (cl72_en & PORT_SYS_SIDE_CTRL)
        cl72_st.sys_side = 1;
    WRITE_XMOD_ARG_BUFF(buftxptr, &cl72_st, sizeof(xmod_cl72_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_CL72_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_CL72_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


int phy8806x_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen;
    uint8_t *bufrxptr;
    int xmodrxlen, rv;
    uint32_t sys_side = 0;

    if (*cl72_en & PORT_SYS_SIDE_CTRL) {
        sys_side = 1;
    }

    *cl72_en = 0;
    sys_side = HOST2LE32(sys_side);

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &sys_side, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_CL72_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_CL72_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_CL72_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, cl72_en, sizeof(uint32_t));

    *cl72_en = LE2HOST32(*cl72_en);

    return rv;
}


int phy8806x_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen;
    uint8_t *bufrxptr;
    int xmodrxlen, rv;
    uint32_t sys_side = 0;
    xmod_cl72_status_t cl72_st;

    sys_side = (phy->port_loc == phymodPortLocSys) ? 1 : 0;

    sys_side = HOST2LE32(sys_side);

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &sys_side, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_CL72_STATUS_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_CL72_STATUS_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_CL72_STATUS_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &cl72_st, sizeof(xmod_cl72_status_t));

    status->enabled = LE2HOST32(cl72_st.enabled);
    status->locked = LE2HOST32(cl72_st.locked);

    return rv;

}


int phy8806x_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    xmod_autoneg_ability_t x_an_ability;
    int xmodtxlen, rv;

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    x_an_ability.sgmii_speed      = an_ability->sgmii_speed;

    x_an_ability.an_cap       = HOST2LE32(an_ability->an_cap);
    x_an_ability.cl73bam_cap  = HOST2LE32(an_ability->cl73bam_cap);
    x_an_ability.cl37bam_cap  = HOST2LE32(an_ability->cl37bam_cap);
    x_an_ability.an_fec       = HOST2LE32(an_ability->an_fec);
    x_an_ability.an_cl72      = HOST2LE32(an_ability->an_cl72);
    x_an_ability.an_hg2       = HOST2LE32(an_ability->an_hg2);
    x_an_ability.capabilities = HOST2LE32(an_ability->capabilities);

    WRITE_XMOD_ARG_BUFF(buftxptr, &x_an_ability, sizeof(xmod_autoneg_ability_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_AUTONEG_ABILITY_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_AUTONEG_ABILITY_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


int phy8806x_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    xmod_autoneg_ability_t x_an_ability;
    int xmodtxlen, xmodrxlen, rv;

    /* write args to xmod buffer */
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_AUTONEG_ABILITY_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_AUTONEG_ABILITY_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_AUTONEG_ABILITY_GET),
                           xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_an_ability, sizeof(xmod_autoneg_ability_t));

    an_ability->sgmii_speed      = x_an_ability.sgmii_speed;
    an_ability->an_cap       = LE2HOST32(x_an_ability.an_cap);
    an_ability->cl73bam_cap  = LE2HOST32(x_an_ability.cl73bam_cap);
    an_ability->cl37bam_cap  = LE2HOST32(x_an_ability.cl37bam_cap);
    an_ability->an_fec       = LE2HOST32(x_an_ability.an_fec);
    an_ability->an_cl72      = LE2HOST32(x_an_ability.an_cl72);
    an_ability->an_hg2       = LE2HOST32(x_an_ability.an_hg2);
    an_ability->capabilities = LE2HOST32(x_an_ability.capabilities);

    return rv;
}


int phy8806x_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    xmod_autoneg_ability_t x_an_ability;
    int xmodtxlen, xmodrxlen, rv;

    /* write args to xmod buffer */
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_AUTONEG_REMOTE_ABILITY_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_AUTONEG_REMOTE_ABILITY_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_AUTONEG_REMOTE_ABILITY_GET),
                           xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_an_ability, sizeof(xmod_autoneg_ability_t));

    an_ability->sgmii_speed      = x_an_ability.sgmii_speed;
    an_ability->an_cap       = LE2HOST32(x_an_ability.an_cap);
    an_ability->cl73bam_cap  = LE2HOST32(x_an_ability.cl73bam_cap);
    an_ability->cl37bam_cap  = LE2HOST32(x_an_ability.cl37bam_cap);
    an_ability->an_fec       = LE2HOST32(x_an_ability.an_fec);
    an_ability->an_cl72      = LE2HOST32(x_an_ability.an_cl72);
    an_ability->an_hg2       = LE2HOST32(x_an_ability.an_hg2);
    an_ability->capabilities = LE2HOST32(x_an_ability.capabilities);

    return rv;
}


int phy8806x_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv=PHYMOD_E_NONE;
    xmod_autoneg_control_t x_an_ctrl;

    x_an_ctrl.an_mode = an->an_mode;
    x_an_ctrl.an_done = 0;
    x_an_ctrl.num_lane_adv = HOST2LE32(an->num_lane_adv);
    x_an_ctrl.flags = HOST2LE32(an->flags);
    x_an_ctrl.enable = HOST2LE32(an->enable);

    if (!(an->enable)) {
        /* write args to xmod buffer */
        buftxptr = (uint8_t *) xmodtxbuff;
        WRITE_XMOD_ARG_BUFF(buftxptr, &x_an_ctrl, sizeof(x_an_ctrl));
        xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_AUTONEG_SET);
        xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

        /* call xmod */
        rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_AUTONEG_SET),
                                    xmodtxbuff, xmodtxlen, NULL, 0);
    }

    return rv;
}


int phy8806x_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;
    xmod_autoneg_control_t x_an_ctrl;
    
    /* write args to xmod buffer */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_AUTONEG_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_AUTONEG_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_an_ctrl, sizeof(xmod_autoneg_control_t));

    an->an_mode = x_an_ctrl.an_mode;  
    an->num_lane_adv = LE2HOST32(x_an_ctrl.num_lane_adv);
    an->flags = LE2HOST32(x_an_ctrl.flags);  
    an->enable = LE2HOST32(x_an_ctrl.enable);

    *an_done = x_an_ctrl.an_done;

    return rv;
}


int phy8806x_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;
    xmod_autoneg_status_t x_an_status;

    /* write args to xmod buffer */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_AUTONEG_STATUS_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_AUTONEG_STATUS_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_an_status, sizeof(xmod_autoneg_status_t));

    status->enabled = LE2HOST32(x_an_status.enabled);
    status->locked = LE2HOST32(x_an_status.locked);
    status->data_rate = LE2HOST32(x_an_status.data_rate);
    status->interface = LE2HOST32(x_an_status.interface);

    return rv;
}


int phy8806x_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen;
    xmod_core_init_config_t x_core_init;
    int idx, rv;

    /* setting up xmod xmod_core_init:                                  */
    /* int xmod_core_init( IN xmod_core_init_config_t* init_config )    */

    /* Initialize the xmod_core_init_config_t structure */    
    x_core_init.num_of_lanes = HOST2LE32(init_config->lane_map.num_of_lanes);
    x_core_init.ref_clock = HOST2LE32(xmodRefClk156Mhz);
    x_core_init.tx_polarity = HOST2LE32(init_config->polarity_map.tx_polarity);
    x_core_init.rx_polarity = HOST2LE32(init_config->polarity_map.rx_polarity);

    for (idx=0; idx<XMOD_MAX_LANES_PER_CORE; idx++) {
        x_core_init.lane_map_rx[idx] = HOST2LE32(init_config->lane_map.lane_map_rx[idx]);
        x_core_init.lane_map_tx[idx] = HOST2LE32(init_config->lane_map.lane_map_tx[idx]);
    }
    x_core_init.core_mode = init_config->core_mode;
    x_core_init.trcvr_host_managed = init_config->trcvr_host_managed;
    
    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_core_init, sizeof(xmod_core_init_config_t));
    xmodtxlen = GET_XMOD_CORE_CMD_IN_LEN(XMOD_CORE_INIT);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);
    
    /* call xmod */
    rv = _phy_8806x_xmod_command(&core->access, XMOD_CORE_INIT, xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


int phy8806x_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    uint8_t x_loopback;
    uint8_t x_enable;
    int xmodtxlen, rv;

    /* setting up xmod xmod_phy_loopback_set:                                                   */
    /* int xmod_phy_loopback_set( IN xmod_loopback_mode_t loopback_type, IN uint8_t enable )    */

    /* Initialize the xmod args */
    x_loopback = loopback;
    x_enable = enable;
    
    /* write args to xmod buffer */
    buftxptr = (uint8_t *) xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_loopback, sizeof(x_loopback));
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_enable, sizeof(x_enable));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_LOOPBACK_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_LOOPBACK_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


int phy8806x_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr, *bufrxptr;
    int xmodtxlen, xmodrxlen, rv;
    xmod_loopback_mode_t x_loopback;
    uint8_t x_enable;

    /* setting up xmod xmod_phy_loopback_get:                                                    */
    /* int xmod_phy_loopback_get( IN xmod_loopback_mode_t loopback_type, OUT uint8_t enable )    */

    /* Initialize the xmod args */
    x_loopback = loopback;
    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_loopback, sizeof(x_loopback));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_LOOPBACK_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_LOOPBACK_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_LOOPBACK_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_enable, sizeof(uint8_t));
    *enable = x_enable;

    return rv;
}


int phy8806x_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;
    int8_t lock_st = -1;

    /* write args to xmod buffer */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_RX_PMD_LOCKED_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_RX_PMD_LOCKED_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);


    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &lock_st, sizeof(lock_st));

    if (lock_st != 1)
        *rx_seq_done = 0;
    else
        *rx_seq_done = 1;

    return rv;
}


int phy8806x_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;
    int8_t link_st = -1;

    /* write args to xmod buffer */
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_LINK_STATUS_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_LINK_STATUS_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);


    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &link_st, sizeof(link_st));

    if (link_st != 1)
        *link_status = 0;
    else
        *link_status = 1;

    return rv;
}


int phy8806x_phy_pcs_userspeed_set(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{
    return PHYMOD_E_UNAVAIL;
}


int phy8806x_phy_pcs_userspeed_get(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{
    return PHYMOD_E_UNAVAIL;
}


int phy8806x_phy_status_dump(const phymod_phy_access_t* phy)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t *val)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    return PHYMOD_E_NONE;  
}


int phy8806x_phy_private_read(const phymod_phy_access_t* phy, uint32_t addr, uint32_t *val)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr, *bufrxptr;
    int xmodtxlen, xmodrxlen, rv;
    uint32_t x_addr;

    x_addr = addr;
    x_addr  = HOST2LE32(x_addr);

    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_addr, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_CORE_CMD_IN_LEN(XMOD_DEV_REG_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    bufrxptr = (uint8_t *)xmodrxbuff;
    xmodrxlen = GET_XMOD_CORE_CMD_OUT_LEN(XMOD_DEV_REG_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_CORE(XMOD_DEV_REG_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the data from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, val, sizeof(uint32_t));
    *val = LE2HOST32(*val);

    return rv; 
}


int phy8806x_phy_private_write(const phymod_phy_access_t* phy, uint32_t addr, uint32_t val)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen,  rv;
    uint32_t x_addr, x_data;
    
    x_addr = addr;
    x_data = val;
    x_addr	= HOST2LE32(x_addr);
    x_data	= HOST2LE32(x_data);

    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_addr, sizeof(uint32_t));
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_data, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_CORE_CMD_IN_LEN(XMOD_DEV_REG_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_CORE(XMOD_DEV_REG_SET),
    							xmodtxbuff, xmodtxlen, NULL, 0);

    return rv; 
}


int phy8806x_phy_i2c_read(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, uint8_t* data)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr, *bufrxptr;
    int xmodtxlen, xmodrxlen, rv;
    xmod_core_i2c_t xmod_core_i2c;
    uint32_t smstat;

    xmod_core_i2c.dev_addr = (uint8_t) (addr & 0xff);
    xmod_core_i2c.offset = (uint8_t) (offset & 0xff);
    xmod_core_i2c.size = (uint8_t) (size & 0xff);
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &xmod_core_i2c, sizeof(xmod_core_i2c_t));
    xmodtxlen = GET_XMOD_CORE_CMD_IN_LEN(XMOD_DEV_I2C_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    bufrxptr = (uint8_t *)xmodrxbuff;
    /* This is the max size of xmod rx buffer */
    xmodrxlen = GET_XMOD_CORE_CMD_OUT_LEN(XMOD_DEV_I2C_GET);
    if (xmod_core_i2c.size > (xmodrxlen - sizeof(uint32_t))) {
        return PHYMOD_E_PARAM;
    }
    /* For optimization, we will use the actual size of
       xmod rx buffer as below. This is run time size as
       opposed to the compile time size coming from 
       xmod_core_cmds_arg_len_tbl. 
     */
    xmodrxlen = sizeof(uint32_t) + xmod_core_i2c.size;
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_CORE(XMOD_DEV_I2C_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the SMBUS Status and the read data from the xmod rx buffer */
    READ_XMOD_ARG_BUFF(bufrxptr, &smstat, sizeof(uint32_t));
    smstat = LE2HOST32(smstat);
    READ_XMOD_ARG_BUFF(bufrxptr, data, xmod_core_i2c.size);
    if (smstat != 0) {
        PHYMOD_DEBUG_ERROR(("I2C / SMBUS Error %d\n", smstat));
        return PHYMOD_E_FAIL;
    }

    return rv;
}


int phy8806x_phy_i2c_write(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, const uint8_t* data)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr, *bufrxptr;
    int xmodtxlen, xmodrxlen, rv;
    xmod_core_i2c_t xmod_core_i2c;
    uint32_t smstat, data32;

    xmod_core_i2c.dev_addr = (uint8_t) (addr & 0xff);
    xmod_core_i2c.offset = (uint8_t) (offset & 0xff);
    xmod_core_i2c.size = (uint8_t) (size & 0xff);
    PHYMOD_MEMCPY((void*)&data32, (void*)data, sizeof(uint32_t));
    data32 = HOST2LE32(data32);

    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &xmod_core_i2c, sizeof(xmod_core_i2c_t));
    WRITE_XMOD_ARG_BUFF(buftxptr, &data32, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_CORE_CMD_IN_LEN(XMOD_DEV_I2C_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    bufrxptr = (uint8_t *)xmodrxbuff;
    xmodrxlen = GET_XMOD_CORE_CMD_OUT_LEN(XMOD_DEV_I2C_SET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_CORE(XMOD_DEV_I2C_SET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the SMBUS Status from the xmod rx buffer */
	READ_XMOD_ARG_BUFF(bufrxptr, &smstat, sizeof(uint32_t));
	smstat = LE2HOST32(smstat);

    if (smstat != 0) {
        PHYMOD_DEBUG_ERROR(("I2C / SMBUS Error %d\n", smstat));
        return PHYMOD_E_FAIL;
    }

    return rv;
}


int phy8806x_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return PHYMOD_E_NONE;
}


int phy8806x_phy_eee_get(const phymod_phy_access_t* phy, uint32_t *enable)
{
    return PHYMOD_E_NONE;
}


int phy8806x_port_init(const phymod_phy_access_t* phy, phymod_port_config_t *port_config)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    xmod_port_config_t x_port_config;
    int xmodtxlen, rv;

    /* setting up xmod xmod_port_init:                                */
    /* int int xmod_port_init( IN xmod_port_config_t port_config )    */

    x_port_config.speed = HOST2LE32(port_config->speed);
    x_port_config.interface = port_config->interface;
    x_port_config.sys_lane_count = port_config->sys_lane_count;
    x_port_config.ln_lane_count = port_config->ln_lane_count;
    x_port_config.an_mode = port_config->an_mode;
    x_port_config.an_cl72 = port_config->an_cl72;
    x_port_config.fs_cl72 = port_config->fs_cl72;
    x_port_config.fs_cl72_sys = port_config->fs_cl72_sys;
    x_port_config.an_fec = port_config->an_fec;
    x_port_config.port_is_higig = port_config->port_is_higig;
    x_port_config.fiber_pref = port_config->fiber_pref;
    x_port_config.fiber_pref_sys = port_config->fiber_pref_sys;
    x_port_config.port_mode = port_config->port_mode;
    x_port_config.is_bootmaster = port_config->is_bootmaster;
    x_port_config.fs_fec = port_config->fs_fec;
    x_port_config.fs_fec_sys = port_config->fs_fec_sys;
    x_port_config.is_flex = port_config->is_flex;
    x_port_config.is_strip_crc = 0;
    x_port_config.quad_mode = port_config->quad_mode;
    
    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_port_config, sizeof(xmod_port_config_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PORT_INIT);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PORT_INIT),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    /* to remove warning */
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(0);

    return rv;
}


int phy8806x_phy_autoneg_try_enable(const phymod_phy_access_t* phy, const phymod_an_try_enable_control_t* an,
                                  phymod_phy_an_status_t* an_status)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    int xmodtxlen, xmodrxlen, rv;
    uint8_t *buftxptr, *bufrxptr;
    xmod_an_status_t x_an_status;
    xmod_an_try_enable_control_t x_an_ctrl;

    /* write args to xmod buffer */
    x_an_ctrl.an_mode = an->an_mode; 
    x_an_ctrl.num_lane_adv = HOST2LE32(an->num_lane_adv);
    x_an_ctrl.flags = HOST2LE32(an->flags);
    x_an_ctrl.enable = HOST2LE32(an->enable);
    x_an_ctrl.speed = HOST2LE32(an->speed);

    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_an_ctrl, sizeof(x_an_ctrl));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_AN_TRY_ENABLE);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);
    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_AN_TRY_ENABLE);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_AN_TRY_ENABLE),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_an_status, sizeof(xmod_an_status_t));
    an_status->result = x_an_status.result;
    an_status->hcd_speed = LE2HOST32(x_an_status.hcd_speed);
 
    return rv;
}

int phy8806x_phy_pfc_use_ip_cos_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_PSM_COS_BMP_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_PSM_COS_BMP_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, enable, sizeof(uint32_t));

    *enable = LE2HOST32(*enable);
    *enable = (*enable) >> XMOD_USE_IP_COS_SHIFT;

    return rv;
}

int phy8806x_phy_pfc_use_ip_cos_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;

    enable |= (XMOD_USE_IP_COS_CMD << XMOD_USE_IP_COS_SHIFT);
    enable =  HOST2LE32(enable);
    WRITE_XMOD_ARG_BUFF(buftxptr, &enable, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_PSM_COS_BMP_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_PSM_COS_BMP_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

int phy8806x_phy_psm_cos_bmp_get(const phymod_phy_access_t* phy, uint32_t* cos_bmp)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_PSM_COS_BMP_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_PSM_COS_BMP_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, cos_bmp, sizeof(uint32_t));

    *cos_bmp = LE2HOST32(*cos_bmp);
    *cos_bmp &= (1 << XMOD_USE_IP_COS_SHIFT) -1 ;

    return rv;
}

int phy8806x_phy_psm_cos_bmp_set(const phymod_phy_access_t* phy, uint32_t cos_bmp)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;

    cos_bmp =  HOST2LE32(cos_bmp);
    WRITE_XMOD_ARG_BUFF(buftxptr, &cos_bmp, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_PSM_COS_BMP_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_PSM_COS_BMP_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;

}

int phy8806x_phy_flow_control_mode_get(const phymod_phy_access_t* phy, uint32_t ingress, uint32_t* mode)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen;
    uint8_t *bufrxptr;
    int xmodrxlen, rv;

    ingress = HOST2LE32(ingress);

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &ingress, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_FLOW_CONTROL_MODE_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_FLOW_CONTROL_MODE_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_FLOW_CONTROL_MODE_GET),
                                xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, mode, sizeof(uint32_t));

    *mode = LE2HOST32(*mode);

    return rv;
}

int phy8806x_phy_flow_control_mode_set(const phymod_phy_access_t* phy, uint32_t mode, uint32_t ingress)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;
    xmod_flow_ctrl_t  flow_ctrl;

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;

    flow_ctrl.mode = mode;
    flow_ctrl.ingress = ingress;

    WRITE_XMOD_ARG_BUFF(buftxptr, &flow_ctrl, sizeof(xmod_flow_ctrl_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_FLOW_CONTROL_MODE_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_FLOW_CONTROL_MODE_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

int phy8806x_phy_forced_speed_line_side_get(const phymod_phy_access_t* phy, uint32_t* speed)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_FORCED_SPEED_LINE_SIDE_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_FORCED_SPEED_LINE_SIDE_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, speed, sizeof(uint32_t));

    *speed = LE2HOST32(*speed);

    return rv;
}

int phy8806x_phy_forced_speed_line_side_set(const phymod_phy_access_t* phy, uint32_t speed)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;

    speed =  HOST2LE32(speed);

    WRITE_XMOD_ARG_BUFF(buftxptr, &speed, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_FORCED_SPEED_LINE_SIDE_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_FORCED_SPEED_LINE_SIDE_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;

}

int phy8806x_phy_autoneg_line_side_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
   uint8_t *bufrxptr;
    int xmodrxlen, rv;

    xmodrxlen = GET_XMOD_ETHERNET_CMD_OUT_LEN(XMOD_PHY_AUTONEG_LINE_SIDE_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_AUTONEG_LINE_SIDE_GET),
                                NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, enable, sizeof(uint32_t));

    *enable = LE2HOST32(*enable);

    return rv;
}

int phy8806x_phy_autoneg_line_side_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;

    enable =  HOST2LE32(enable);
    WRITE_XMOD_ARG_BUFF(buftxptr, &enable, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_PHY_AUTONEG_LINE_SIDE_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);
    
    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_ETH(XMOD_PHY_AUTONEG_LINE_SIDE_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;

}

int phy8806x_debug_ether(const phymod_phy_access_t* phy)
{
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *bufrxptr;
    int xmodrxlen, rv;
    xmod_debug_ether_t x_debug_ether;

    xmodrxlen = GET_XMOD_CORE_CMD_OUT_LEN(XMOD_DEV_DEBUG_ETHER_CMD);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = _phy_8806x_xmod_command(&phy->access, XMOD_CMD_MODE_CORE(XMOD_DEV_DEBUG_ETHER_CMD),
                                NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_debug_ether, sizeof(x_debug_ether));

    PHYMOD_DIAG_OUT(("Line side Link status: %d \n", x_debug_ether.link_state_ln_side));
    PHYMOD_DIAG_OUT(("System side Link status: %d \n", x_debug_ether.link_state_sys_side));

    PHYMOD_DIAG_OUT(("System side Speed: %d \n", LE2HOST32(x_debug_ether.link_speed_sys_side)));
    PHYMOD_DIAG_OUT(("Line side Speed: %d \n", LE2HOST32(x_debug_ether.link_speed_ln_side)));

    PHYMOD_DIAG_OUT(("Line side CL72 status: %d \n", x_debug_ether.link_cl72_ln_side));
    PHYMOD_DIAG_OUT(("System side CL72 status: %d \n", x_debug_ether.link_cl72_sys_side));

    PHYMOD_DIAG_OUT(("Line side FEC status: %d \n", x_debug_ether.link_fec_ln_side));
    PHYMOD_DIAG_OUT(("System side FEC status: %d \n", x_debug_ether.link_fec_sys_side));

    return rv;
}

int phy8806x_phy_op_mode_get(const phymod_phy_access_t* phy, phymod_operation_mode_t* op_mode)
{
    return PHYMOD_E_UNAVAIL;
}

#endif /* PHYMOD_PHY8806X_SUPPORT */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bfcmap88060_a0.c
 * Purpose:    BFCMAP Driver support for Broadcom BCM8806X Phy devices.
 */

#include <bfcmap.h>
#include <bfcmap_int.h>
#include <soc/phyctrl.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <soc/fcmapphy.h>

#include "../../../../../libs/phymod/chip/phy8806x/phy8806x_xmod_api.h"

#include "../../phy8806x.h"

#define BFCMAP88060_MAX_PORTS           1

#define BFCMAP_MAPPER_LENGTH 64

#ifdef BCMSWAP64
#undef BCMSWAP64
#endif
/* Byte swap a 64 bit value */
#define BCMSWAP64(val) \
    ((uint64_t) ( \
        (((uint64_t) (val) & (uint64_t) 0x00000000000000ffULL) << 56) | \
        (((uint64_t) (val) & (uint64_t) 0x000000000000ff00ULL) << 40) | \
        (((uint64_t) (val) & (uint64_t) 0x0000000000ff0000ULL) << 24) | \
        (((uint64_t) (val) & (uint64_t) 0x00000000ff000000ULL) <<  8) | \
        (((uint64_t) (val) & (uint64_t) 0x000000ff00000000ULL) >>  8) | \
        (((uint64_t) (val) & (uint64_t) 0x0000ff0000000000ULL) >> 24) | \
        (((uint64_t) (val) & (uint64_t) 0x00ff000000000000ULL) >> 40) | \
        (((uint64_t) (val) & (uint64_t) 0xff00000000000000ULL) >> 56) ))

STATIC int bfcmap88060_port_config_selective_set(bfcmap_port_ctrl_t *mpc, 
                                                        bfcmap_port_config_t *pCfg);

STATIC int bfcmap88060_port_config_selective_get(bfcmap_port_ctrl_t *mpc, 
                                                        bfcmap_port_config_t *pCfg);

STATIC int bfcmap88060_stat_clear(bfcmap_port_ctrl_t *mpc);

extern int 
bfcmap_event(bfcmap_port_ctrl_t *mpc, bfcmap_event_t event, 
             int chanId, int assocId);

extern int
phy_8806x_xmod_command(int unit, int port, int mode_opcode,  uint32 arg[], int arg_size, uint32 result[], int result_size);

extern int compute_quad_mode (int unit, int port);
/*
 * Function:
 *      bfcmap88060_device_init
 * Purpose:
 *      Initializes a bfcmap device.
 * Parameters:
 *      mdc         - bfcmap device control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap88060_device_init(bfcmap_dev_ctrl_t *mdc)
{
    int     rv = BFCMAP_E_NONE, xmodtxlen, unit, port;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_cmd_t x_bfcmap_port_cmd;

    unit = SOC_FCMAP_PORTID2UNIT(mdc->p);
    port = SOC_FCMAP_PORTID2PORT(mdc->p);
    x_bfcmap_port_cmd = XMOD_BFCMAP_DEV_INIT;
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_cmd, sizeof(xmod_bfcmap_port_cmd_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CMD_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CMD_SET),
                            xmodtxbuff, xmodtxlen, NULL, 0);
    if (rv != BFCMAP_E_NONE) {
        BFCMAP_SAL_DBG_PRINTF("Device Init on port 0x%x failed \n", port);
        return rv;
    }

    if (mdc->mibs == NULL) {
        if ((mdc->mibs = bfcmap_int_dword_malloc(sizeof(bfcmap_mibs_t),
                                     "BFCMAP_mibs")) == NULL) {
            BFCMAP_SAL_DBG_PRINTF("MIBS allocation on port 0x%x failed \n", port);
            return BFCMAP_E_MEMORY;
        }
    }
    
    if (mdc->prev_mibs == NULL) {
        if ((mdc->prev_mibs = bfcmap_int_dword_malloc(sizeof(bfcmap_mibs_t),
                                     "BFCMAP_prev_mibs")) == NULL) {
            BFCMAP_SAL_DBG_PRINTF("Prev MIBS allocation on port 0x%x failed \n", port);
            return BFCMAP_E_MEMORY;
        }
    }

    return BFCMAP_E_NONE;
}

/*
 * Function:
 *      bfcmap88060_device_uninit
 * Purpose:
 *      Initializes a bfcmap device.
 * Parameters:
 *      mdc         - bfcmap device control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap88060_device_uninit(bfcmap_dev_ctrl_t *mdc)
{

#ifdef UNINIT_XMOD
    int     rv = BFCMAP_E_NONE, xmodtxlen, unit, port;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_cmd_t x_bfcmap_port_cmd;

    unit = SOC_FCMAP_PORTID2UNIT(mdc->p);
    port = SOC_FCMAP_PORTID2PORT(mdc->p);
    x_bfcmap_port_cmd = XMOD_BFCMAP_DEV_UNINIT;
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_cmd, sizeof(xmod_bfcmap_port_cmd_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CMD_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CMD_SET),
                            xmodtxbuff, xmodtxlen, NULL, 0);
    if (rv != BFCMAP_E_NONE) {
        BFCMAP_SAL_DBG_PRINTF("Device Un-init on port 0x%x failed \n", port);
    }
#endif

    if (mdc->mibs) {
        bfcmap_int_dword_free(mdc->mibs);
        mdc->mibs = NULL;
    }
    
    if (mdc->prev_mibs) {
        bfcmap_int_dword_free(mdc->prev_mibs);
        mdc->prev_mibs = NULL;
    }

    return BFCMAP_E_NONE;
}

/*
 * Function:
 *      bfcmap88060_port_config_selective_set
 * Purpose:
 *      Initilaizes the bfcmap port corresponding to the configuration
 *      specified.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      pCfg         - Pointer to port configuration
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC  int 
bfcmap88060_port_config_selective_set(bfcmap_port_ctrl_t *mpc, 
                                      bfcmap_port_config_t *pCfg)
{
    uint32  mask1, mask2;
    int     rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_config_t x_bfcmap_port_cfg;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);
    
    if (!pCfg) {
        BFCMAP_SAL_DBG_PRINTF("Port configuration is NULL\n"); 
        rv = BFCMAP_E_PARAM;
        goto exit;
    }

    mask1 = pCfg->action_mask;
    mask2 = pCfg->action_mask2;

    /* BFCMAP_LOCK_PORT(mpc); */

    /* These config parameters are not supported by MT2 */
    if ((mask1 & BFCMAP_ATTR_PORT_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_MAP_TABLE_INPUT_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_MAP_TABLE_INPUT_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_FC_CRC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VLANTAG_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VID_MAPSRC_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_VLAN_PRI_MAP_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_INTERRUPT_ENABLE_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VLAN_PRI_MAP_MODE_MASK) || 
        (mask2 & BFCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK)) {
        BFCMAP_SAL_DBG_PRINTF("Port configuration parameter is Not Supported\n");
        rv = BFCMAP_E_PARAM;
        goto exit;
    }
    
    BFCMAP_SAL_MEMSET(&x_bfcmap_port_cfg, 0, sizeof(xmod_bfcmap_port_config_t));

    /* TX BB Credits */
    if (mask1 & BFCMAP_ATTR_TX_BB_CREDITS_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_TX_BB_CREDITS_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.tx_buffer_to_buffer_credits = _shr_swap32(pCfg->tx_buffer_to_buffer_credits);
#else
        x_bfcmap_port_cfg.tx_buffer_to_buffer_credits = pCfg->tx_buffer_to_buffer_credits;
#endif
    }

    /* RX BB Credits */
    if (mask1 & BFCMAP_ATTR_RX_BB_CREDITS_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_RX_BB_CREDITS_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.rx_buffer_to_buffer_credits = _shr_swap32(pCfg->rx_buffer_to_buffer_credits);
#else
        x_bfcmap_port_cfg.rx_buffer_to_buffer_credits = pCfg->rx_buffer_to_buffer_credits;
#endif
    }

    /* Max Frame Length & RX BB Credits */
    if (mask1 & BFCMAP_ATTR_MAX_FRAME_LENGTH_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_MAX_FRAME_LENGTH_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.max_frame_length            = _shr_swap32(pCfg->max_frame_length);
#else
        x_bfcmap_port_cfg.max_frame_length            = pCfg->max_frame_length;
#endif
    }
    
    /* BB Credit Recovery Parameter */
    if (mask1 & BFCMAP_ATTR_BB_SC_N_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_BB_SC_N_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.bb_sc_n = _shr_swap32(pCfg->bb_sc_n);
#else
        x_bfcmap_port_cfg.bb_sc_n = pCfg->bb_sc_n;
#endif
    }

    /* Receive, transmit Timeout */
    if (mask1 & BFCMAP_ATTR_R_T_TOV_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_R_T_TOV_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.r_t_tov = _shr_swap32(pCfg->r_t_tov);
#else
        x_bfcmap_port_cfg.r_t_tov = pCfg->r_t_tov;
#endif
    }

    /* Interrupt Enable */
    if (mask1 & BFCMAP_ATTR_INTERRUPT_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_INTERRUPT_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.interrupt_enable = _shr_swap32(pCfg->interrupt_enable);
#else
        x_bfcmap_port_cfg.interrupt_enable = pCfg->interrupt_enable;
#endif
    }

    /* Fillword on 8G active state */
    if (mask1 & BFCMAP_ATTR_FW_ON_ACTIVE_8G_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_FW_ON_ACTIVE_8G_MASK;
        x_bfcmap_port_cfg.fw_on_active_8g = pCfg->fw_on_active_8g;
    }

    /* SRC MAC Construct */
    if (mask1 & BFCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK;
        x_bfcmap_port_cfg.src_mac_construct = pCfg->src_mac_construct;
        BFCMAP_SAL_MEMCPY(x_bfcmap_port_cfg.src_mac_addr, pCfg->src_mac_addr, sizeof(xmod_bmac_addr_t));
#ifdef BE_HOST
        x_bfcmap_port_cfg.src_fcmap_prefix = _shr_swap32(pCfg->src_fcmap_prefix);
#else
        x_bfcmap_port_cfg.src_fcmap_prefix = pCfg->src_fcmap_prefix;
#endif
    }

    /* DST MAC Construct */
    if (mask1 & BFCMAP_ATTR_DST_MAC_CONSTRUCT_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_DST_MAC_CONSTRUCT_MASK;
        x_bfcmap_port_cfg.dst_mac_construct = pCfg->dst_mac_construct;
        BFCMAP_SAL_MEMCPY(x_bfcmap_port_cfg.dst_mac_addr, pCfg->dst_mac_addr, sizeof(xmod_bmac_addr_t));
#ifdef BE_HOST
        x_bfcmap_port_cfg.dst_fcmap_prefix = _shr_swap32(pCfg->dst_fcmap_prefix);
#else
        x_bfcmap_port_cfg.dst_fcmap_prefix = pCfg->dst_fcmap_prefix;
#endif
    }

    /* VLAN Tag */
    if (mask1 & BFCMAP_ATTR_VLAN_TAG_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_VLAN_TAG_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.vlan_tag = _shr_swap32(pCfg->vlan_tag);
#else
        x_bfcmap_port_cfg.vlan_tag = pCfg->vlan_tag;
#endif
    }

    /* VFT Tag */
    if (mask1 & BFCMAP_ATTR_VFT_TAG_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_VFT_TAG_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.vft_tag = _shr_swap32(pCfg->vft_tag);
#else
        x_bfcmap_port_cfg.vft_tag = pCfg->vft_tag;
#endif
    }

    /* Ingress Mapper Bypass */
    if (mask1 & BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.ingress_mapper_bypass = _shr_swap32(pCfg->ingress_mapper_bypass);
#else
        x_bfcmap_port_cfg.ingress_mapper_bypass = pCfg->ingress_mapper_bypass;
#endif
    }

    /* Egress Mapper Bypass */
    if (mask1 & BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_mapper_bypass = _shr_swap32(pCfg->egress_mapper_bypass);
#else
        x_bfcmap_port_cfg.egress_mapper_bypass = pCfg->egress_mapper_bypass;
#endif
    }

    /* Egress FC CRC Mode */
    if (mask1 & BFCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK;
        x_bfcmap_port_cfg.egress_fc_crc_mode = pCfg->egress_fc_crc_mode;
    }

    /* Ingress VFT HDR Processing Mode */
    if (mask1 & BFCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK;
        x_bfcmap_port_cfg.ingress_vfthdr_proc_mode = pCfg->ingress_vfthdr_proc_mode;
    }

    /* Egress VFT HDR Processing Mode */
    if (mask1 & BFCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK;
        x_bfcmap_port_cfg.egress_vfthdr_proc_mode = pCfg->egress_vfthdr_proc_mode;
    }

    /* Ingress VLANTAG Processing Mode */
    if (mask1 & BFCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK;
        x_bfcmap_port_cfg.ingress_vlantag_proc_mode = pCfg->ingress_vlantag_proc_mode;
    }

    /* Ingress VFID Map Source */
    if (mask1 & BFCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK;
        x_bfcmap_port_cfg.ingress_vfid_mapsrc = pCfg->ingress_vfid_mapsrc;
    }

    /* Egress VFID Map Source */
    if (mask1 & BFCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK;
        x_bfcmap_port_cfg.egress_vfid_mapsrc = pCfg->egress_vfid_mapsrc;
    }

    /* Ingress VID Map Source */
    if (mask1 & BFCMAP_ATTR_INGRESS_VID_MAPSRC_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_INGRESS_VID_MAPSRC_MASK;
        x_bfcmap_port_cfg.ingress_vid_mapsrc = pCfg->ingress_vid_mapsrc;
    }

    /* Ingress VFT Hop Count Check Mode */
    if (mask1 & BFCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK) {
        x_bfcmap_port_cfg.action_mask |= XMOD_BFCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK;
        x_bfcmap_port_cfg.ingress_hopCnt_check_mode = pCfg->ingress_hopCnt_check_mode;
    }

    /* Egress Hop Count Decrement Enable */
    if (mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_hopCnt_dec_enable = _shr_swap32(pCfg->egress_hopCnt_dec_enable);
#else
        x_bfcmap_port_cfg.egress_hopCnt_dec_enable = pCfg->egress_hopCnt_dec_enable;
#endif
    }

    /* Use TTS or PCS during 16G AutoNeg */
    if (mask2 & BFCMAP_ATTR2_USE_TTS_PCS_16G_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_USE_TTS_PCS_16G_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.use_tts_pcs_16G = _shr_swap32(pCfg->use_tts_pcs_16G);
#else
        x_bfcmap_port_cfg.use_tts_pcs_16G = pCfg->use_tts_pcs_16G;
#endif
    }

    /* Use TTS or PCS during 32G AutoNeg */
    if (mask2 & BFCMAP_ATTR2_USE_TTS_PCS_32G_MASK) {
        /* 32G supports TTS ONLY */
        if (pCfg->use_tts_pcs_32G == 1) {
            BFCMAP_SAL_DBG_PRINTF("PCS NOT supported at 32G, only TTS\n");
            rv = BFCMAP_E_PARAM;
            goto exit;
        }
    }

    /* Enable/Disable link training for 16G  */
    if (mask2 & BFCMAP_ATTR2_TRAINING_ENABLE_16G_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_TRAINING_ENABLE_16G_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.training_enable_16G = _shr_swap32(pCfg->training_enable_16G);
#else
        x_bfcmap_port_cfg.training_enable_16G = pCfg->training_enable_16G;
#endif
    }

    /* Enable/Disable link training for 32G  */
    if (mask2 & BFCMAP_ATTR2_TRAINING_ENABLE_32G_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_TRAINING_ENABLE_32G_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.training_enable_32G = _shr_swap32(pCfg->training_enable_32G);
#else
        x_bfcmap_port_cfg.training_enable_32G = pCfg->training_enable_32G;
#endif
    }

    /* Enable/Disable FEC for 16G  */
    if (mask2 & BFCMAP_ATTR2_FEC_ENABLE_16G_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_FEC_ENABLE_16G_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.fec_enable_16G = _shr_swap32(pCfg->fec_enable_16G);
#else
        x_bfcmap_port_cfg.fec_enable_16G = pCfg->fec_enable_16G;
#endif
    }

    /* Enable/Disable FEC for 32G  */
    if (mask2 & BFCMAP_ATTR2_FEC_ENABLE_32G_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_FEC_ENABLE_32G_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.fec_enable_32G = _shr_swap32(pCfg->fec_enable_32G);
#else
        x_bfcmap_port_cfg.fec_enable_32G = pCfg->fec_enable_32G;
#endif
    }

    /* Enable FCS corruption on if FCoE pkt has EOFni or EOFa on ingress  */
    if (mask2 & BFCMAP_ATTR2_INGRESS_FCS_CRRPT_EOF_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_INGRESS_FCS_CRRPT_EOF_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.ingress_fcs_crrpt_eof_enable = _shr_swap32(pCfg->ingress_fcs_crrpt_eof_enable);
#else
        x_bfcmap_port_cfg.ingress_fcs_crrpt_eof_enable = pCfg->ingress_fcs_crrpt_eof_enable;
#endif
    }

    /* Enable VLAN header insert on ingress  */
    if (mask2 & BFCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.ingress_vlantag_presence_enable = _shr_swap32(pCfg->ingress_vlantag_presence_enable);
#else
        x_bfcmap_port_cfg.ingress_vlantag_presence_enable = pCfg->ingress_vlantag_presence_enable;
#endif
    }

    /* Egress VFT Hop Count Check Mode */
    if (mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_CHECK_MODE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_EGRESS_HOPCNT_CHECK_MODE_MASK;
        x_bfcmap_port_cfg.egress_hopcnt_check_mode = pCfg->egress_hopcnt_check_mode;
    }

    /* Enable passing control frame on egress when mapper is in bypass mode  */
    if (mask2 & BFCMAP_ATTR2_EGRESS_PASS_CTRL_FRAME_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_EGRESS_PASS_CTRL_FRAME_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_pass_ctrl_frame_enable = _shr_swap32(pCfg->egress_pass_ctrl_frame_enable);
#else
        x_bfcmap_port_cfg.egress_pass_ctrl_frame_enable = pCfg->egress_pass_ctrl_frame_enable;
#endif
    }

    /* Enable passing PFC frame on egress when mapper is in bypass mode  */
    if (mask2 & BFCMAP_ATTR2_EGRESS_PASS_PFC_FRAME_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_EGRESS_PASS_PFC_FRAME_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_pass_pfc_frame_enable = _shr_swap32(pCfg->egress_pass_pfc_frame_enable);
#else
        x_bfcmap_port_cfg.egress_pass_pfc_frame_enable = pCfg->egress_pass_pfc_frame_enable;
#endif
    }

    /* Enable passing pause frame on egress when mapper is in bypass mode  */
    if (mask2 & BFCMAP_ATTR2_EGRESS_PASS_PAUSE_FRAME_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_EGRESS_PASS_PAUSE_FRAME_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_pass_pause_frame_enable = _shr_swap32(pCfg->egress_pass_pause_frame_enable);
#else
        x_bfcmap_port_cfg.egress_pass_pause_frame_enable = pCfg->egress_pass_pause_frame_enable;
#endif
    }

    /* Disable FCOE header version field check on egress  */
    if (mask2 & BFCMAP_ATTR2_EGRESS_FCOE_VER_CHK_DISABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_EGRESS_FCOE_VER_CHK_DISABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_fcoe_version_chk_disable = _shr_swap32(pCfg->egress_fcoe_version_chk_disable);
#else
        x_bfcmap_port_cfg.egress_fcoe_version_chk_disable = pCfg->egress_fcoe_version_chk_disable;
#endif
    }

    /* Default CoS value to use on egress  */
    if (mask2 & BFCMAP_ATTR2_EGRESS_DEFAULT_COS_VALUE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_EGRESS_DEFAULT_COS_VALUE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_default_cos_value = _shr_swap32(pCfg->egress_default_cos_value);
#else
        x_bfcmap_port_cfg.egress_default_cos_value = pCfg->egress_default_cos_value;
#endif
    }

    /* Use IP CoS map values on egress */
    if (mask2 & BFCMAP_ATTR2_EGRESS_USE_IP_COS_MAP_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= BFCMAP_ATTR2_EGRESS_USE_IP_COS_MAP_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_use_ip_cos_map = _shr_swap32(pCfg->egress_use_ip_cos_map);
#else
        x_bfcmap_port_cfg.egress_use_ip_cos_map = pCfg->egress_use_ip_cos_map;
#endif
    }

    /* Ingress Hop Count Decrement Enable */
    if (mask2 & BFCMAP_ATTR2_INGRESS_HOPCNT_DEC_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_INGRESS_HOPCNT_DEC_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.ingress_hopcnt_dec_enable = _shr_swap32(pCfg->ingress_hopcnt_dec_enable);
#else
        x_bfcmap_port_cfg.ingress_hopcnt_dec_enable = pCfg->ingress_hopcnt_dec_enable;
#endif
    }

    /* Scrambling Enable/Disable */
    if (mask2 & BFCMAP_ATTR2_SCRAMBLING_ENABLE_MASK) {
        /* Scrambling can be enabled/disabled for 8G FC ONLY */
        if ((pCfg->scrambling_enable_mask & BFCMAP_PORT_SCRAMBLING_SPEED_2GBPS) ||
            (pCfg->scrambling_enable_mask & BFCMAP_PORT_SCRAMBLING_SPEED_4GBPS) ||
            (pCfg->scrambling_enable_mask & BFCMAP_PORT_SCRAMBLING_SPEED_16GBPS) ||
            (pCfg->scrambling_enable_mask & BFCMAP_PORT_SCRAMBLING_SPEED_32GBPS)) {
            
            BFCMAP_SAL_DBG_PRINTF("FC Scrambling is Not Supported for 2G, always disabled for 4G, always enabled for 16G and 32G speeds \n");
        }
        
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_SCRAMBLING_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.scrambling_enable_mask  = _shr_swap32(pCfg->scrambling_enable_mask);
        x_bfcmap_port_cfg.scrambling_enable_value = _shr_swap32(pCfg->scrambling_enable_value);
#else
        x_bfcmap_port_cfg.scrambling_enable_mask  = pCfg->scrambling_enable_mask;
        x_bfcmap_port_cfg.scrambling_enable_value = pCfg->scrambling_enable_value;
#endif
    }

    /* Egress Pause Enable */
    if (mask2 & BFCMAP_ATTR2_EGRESS_PAUSE_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_EGRESS_PAUSE_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_pause_enable = _shr_swap32(pCfg->egress_pause_enable);
#else
        x_bfcmap_port_cfg.egress_pause_enable = pCfg->egress_pause_enable;
#endif
    }

    /* Egress PFC Enable */
    if (mask2 & BFCMAP_ATTR2_EGRESS_PFC_ENABLE_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_EGRESS_PFC_ENABLE_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.egress_pfc_enable = _shr_swap32(pCfg->egress_pfc_enable);
#else
        x_bfcmap_port_cfg.egress_pfc_enable = pCfg->egress_pfc_enable;
#endif
    }

    /* Egress PFC Enable */
    if (mask2 & BFCMAP_ATTR2_STAT_INTERVAL_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_STAT_INTERVAL_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.stat_interval = _shr_swap32(pCfg->stat_interval);
#else
        x_bfcmap_port_cfg.stat_interval = pCfg->stat_interval;
#endif
    }

    /* Tranceiver Attributes  specification by config when 
       Transceiver I2c not connected to FW Micro Controller,
       or Copper Transceiver has no MSA PROM map. */

    if (mask2 & BFCMAP_ATTR2_TRCM_ATTRIBS_MASK) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_TRCM_ATTRIBS_MASK;
#ifdef BE_HOST
        x_bfcmap_port_cfg.trcm_attribs = _shr_swap32(pCfg->trcm_attribs);
#else
        x_bfcmap_port_cfg.trcm_attribs = pCfg->trcm_attribs;
#endif
    }

    /* This configuration determines how COS of an ingress FC frame
       is translated to vlan priority. */

    if (mask2 & BFCMAP_ATTR2_COS_TO_PRI_MASK ) {
        x_bfcmap_port_cfg.action_mask2 |= XMOD_BFCMAP_ATTR2_COS_TO_PRI_MASK ;
#ifdef BE_HOST
        x_bfcmap_port_cfg.cos_to_pri.cos = _shr_swap16(pCfg->cos_to_pri.cos);
        x_bfcmap_port_cfg.cos_to_pri.pri = _shr_swap16(pCfg->cos_to_pri.pri);
#else
        x_bfcmap_port_cfg.cos_to_pri.cos = pCfg->cos_to_pri.cos;
        x_bfcmap_port_cfg.cos_to_pri.pri = pCfg->cos_to_pri.pri;
#endif
    }

    /* Swap the action masks */
#ifdef BE_HOST
        x_bfcmap_port_cfg.action_mask = _shr_swap32(x_bfcmap_port_cfg.action_mask);
        x_bfcmap_port_cfg.action_mask2 = _shr_swap32(x_bfcmap_port_cfg.action_mask2);
#endif

    /* Save the current config. */
    /* BFCMAP_SAL_MEMCPY(&mpc->cfg, pCfg, sizeof(bfcmap_port_config_t)); */

    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_cfg, sizeof(xmod_bfcmap_port_config_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CONFIG_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CONFIG_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);
exit:
    /* BFCMAP_UNLOCK_PORT(mpc); */
    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_config_selective_get
 * Purpose:
 *      Returns the current bfcmap port configuration.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      pCfg         - Pointer to port configuration
 * Returns:
 *      BFCMAP_E_NONE   - Success
 *      BFCMAP_E_PARAM  - if pCfg is NULL
 */
STATIC int 
bfcmap88060_port_config_selective_get(bfcmap_port_ctrl_t *mpc, 
                                      bfcmap_port_config_t *pCfg)
{
    uint32  mask1, mask2, scramb_en_mask, cos;
    uint32 xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32 xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *buftxptr, *bufrxptr;
    int xmodtxlen, xmodrxlen, rv = BFCMAP_E_NONE;
    xmod_bfcmap_port_config_t x_bfcmap_port_cfg;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;
    phy_ctrl_t *pc;
    
    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc && !(pc->fcmap_enable)) {
        rv = BFCMAP_E_PARAM;               
        goto exit;
    }

    if (!pCfg) {
        BFCMAP_SAL_DBG_PRINTF("Port configuration is NULL\n"); 
        rv = BFCMAP_E_PARAM;
        goto exit;
    }

    mask1 = pCfg->action_mask;
    mask2 = pCfg->action_mask2;
    scramb_en_mask = pCfg->scrambling_enable_mask;
    cos = pCfg->cos_to_pri.cos;
    BFCMAP_SAL_MEMSET(pCfg, 0, sizeof(bfcmap_port_config_t));
    pCfg->action_mask = mask1;
    pCfg->action_mask2 = mask2;
    pCfg->scrambling_enable_mask = scramb_en_mask;
    pCfg->cos_to_pri.cos = cos;
 
    /* These config parameters are not supported by MT2 */

    if ((mask1 & BFCMAP_ATTR_PORT_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_MAP_TABLE_INPUT_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_MAP_TABLE_INPUT_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_FC_CRC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VLANTAG_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VID_MAPSRC_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_VLAN_PRI_MAP_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_INTERRUPT_ENABLE_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VLAN_PRI_MAP_MODE_MASK) || 
        (mask2 & BFCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK)) {

        BFCMAP_SAL_DBG_PRINTF("Port configuration parameter is Not Supported\n");
        rv = BFCMAP_E_PARAM;
        goto exit;
    }

    /* BFCMAP_LOCK_PORT(mpc); */
    
    BFCMAP_SAL_MEMSET(&x_bfcmap_port_cfg, 0, sizeof(xmod_bfcmap_port_config_t));

    /* write the mask values to xmod tx buffer */
    buftxptr = (uint8 *)xmodtxbuff;

#ifdef BE_HOST
    mask1 = _shr_swap32(mask1);
    mask2 = _shr_swap32(mask2);
    scramb_en_mask = _shr_swap32(scramb_en_mask);
    cos = _shr_swap32(cos);
#endif

    WRITE_XMOD_ARG_BUFF(buftxptr, &mask1, sizeof(uint32));
    WRITE_XMOD_ARG_BUFF(buftxptr, &mask2, sizeof(uint32)); 
    WRITE_XMOD_ARG_BUFF(buftxptr, &scramb_en_mask, sizeof(uint32));
    WRITE_XMOD_ARG_BUFF(buftxptr, &cos, sizeof(uint32));

    /* call xmod */
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CONFIG_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);
    xmodrxlen = GET_XMOD_FC_CMD_OUT_LEN(XMOD_BFCMAP_PORT_CONFIG_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CONFIG_GET),
                           xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8 *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_bfcmap_port_cfg, sizeof(xmod_bfcmap_port_config_t));

    /* TX BB Credits */
    if (pCfg->action_mask & BFCMAP_ATTR_TX_BB_CREDITS_MASK) {
#ifdef BE_HOST
        pCfg->tx_buffer_to_buffer_credits = _shr_swap32(x_bfcmap_port_cfg.tx_buffer_to_buffer_credits);
#else
        pCfg->tx_buffer_to_buffer_credits = x_bfcmap_port_cfg.tx_buffer_to_buffer_credits;
#endif
    }

    /* RX BB Credits */
    if (pCfg->action_mask & BFCMAP_ATTR_RX_BB_CREDITS_MASK) {
#ifdef BE_HOST
        pCfg->rx_buffer_to_buffer_credits = _shr_swap32(x_bfcmap_port_cfg.rx_buffer_to_buffer_credits);
#else
        pCfg->rx_buffer_to_buffer_credits = x_bfcmap_port_cfg.rx_buffer_to_buffer_credits;
#endif
    }

    /* Max Frame Length */
    if (pCfg->action_mask & BFCMAP_ATTR_MAX_FRAME_LENGTH_MASK) {
#ifdef BE_HOST
        pCfg->max_frame_length = _shr_swap32(x_bfcmap_port_cfg.max_frame_length);
#else
        pCfg->max_frame_length = x_bfcmap_port_cfg.max_frame_length;
#endif
    }

    /* BB Credit Recovery Parameter */
    if (pCfg->action_mask & BFCMAP_ATTR_BB_SC_N_MASK) {
#ifdef BE_HOST
        pCfg->bb_sc_n = _shr_swap32(x_bfcmap_port_cfg.bb_sc_n);
#else
        pCfg->bb_sc_n = x_bfcmap_port_cfg.bb_sc_n;
#endif
    }

    /* Port State */
    if (pCfg->action_mask & BFCMAP_ATTR_PORT_STATE_MASK) {

        switch (x_bfcmap_port_cfg.port_state) {
            case XMOD_BFCMAP_PORT_STATE_ACTIVE:
                pCfg->port_state = BFCMAP_PORT_STATE_ACTIVE;
                break;
            
            case XMOD_BFCMAP_PORT_STATE_LINKDOWN:
                pCfg->port_state = BFCMAP_PORT_STATE_LINKDOWN;
                break;
        }
    }
    
    /* Port Speed */
    if (pCfg->action_mask & BFCMAP_ATTR_SPEED_MASK) {

        switch (x_bfcmap_port_cfg.speed) {
            case XMOD_BFCMAP_PORT_SPEED_4GBPS:
                pCfg->speed = BFCMAP_PORT_SPEED_4GBPS;
                break;
            
            case XMOD_BFCMAP_PORT_SPEED_8GBPS:
                pCfg->speed = BFCMAP_PORT_SPEED_8GBPS;
                break;

            case XMOD_BFCMAP_PORT_SPEED_16GBPS:
                pCfg->speed = BFCMAP_PORT_SPEED_16GBPS;
                break;
 
            case XMOD_BFCMAP_PORT_SPEED_32GBPS:
                pCfg->speed = BFCMAP_PORT_SPEED_32GBPS;
                break;
        }
    }

    /* Receive, transmit Timeout */
    if (pCfg->action_mask & BFCMAP_ATTR_R_T_TOV_MASK) {
#ifdef BE_HOST
        pCfg->r_t_tov = _shr_swap32(x_bfcmap_port_cfg.r_t_tov);
#else
        pCfg->r_t_tov = x_bfcmap_port_cfg.r_t_tov;
#endif
    }

    /* Interrupt Enable */
    if (pCfg->action_mask & BFCMAP_ATTR_INTERRUPT_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->interrupt_enable = _shr_swap32(x_bfcmap_port_cfg.interrupt_enable);
#else
        pCfg->interrupt_enable = x_bfcmap_port_cfg.interrupt_enable;
#endif
    }

    /* Fillword on 8G active state */
    if (pCfg->action_mask & BFCMAP_ATTR_FW_ON_ACTIVE_8G_MASK) {
        
        switch (x_bfcmap_port_cfg.fw_on_active_8g) {
            case XMOD_BFCMAP_8G_FW_ON_ACTIVE_ARBFF:
                pCfg->fw_on_active_8g = BFCMAP_8G_FW_ON_ACTIVE_ARBFF;
                break;
            
            case XMOD_BFCMAP_8G_FW_ON_ACTIVE_IDLE:
                pCfg->fw_on_active_8g = BFCMAP_8G_FW_ON_ACTIVE_IDLE;
                break;
        }
    }

    /* SRC MAC Construct */
    if (pCfg->action_mask & BFCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK) {
#ifdef BE_HOST
        pCfg->src_fcmap_prefix = _shr_swap32(x_bfcmap_port_cfg.src_fcmap_prefix);
#else
        pCfg->src_fcmap_prefix = x_bfcmap_port_cfg.src_fcmap_prefix;
#endif

        BFCMAP_SAL_MEMCPY(pCfg->src_mac_addr, x_bfcmap_port_cfg.src_mac_addr, sizeof(_shr_bmac_addr_t));

        switch (x_bfcmap_port_cfg.src_mac_construct) {
            case XMOD_BFCMAP_ENCAP_FCOE_FPMA:
                pCfg->src_mac_construct = BFCMAP_ENCAP_FCOE_FPMA;
                break;
            
            case XMOD_BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL:
                pCfg->src_mac_construct = BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL;
                break;

            case XMOD_BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER:
                pCfg->src_mac_construct = BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER;
                break;
        }
    }

    /* DST MAC Construct */
    if (pCfg->action_mask & BFCMAP_ATTR_DST_MAC_CONSTRUCT_MASK) {
#ifdef BE_HOST
        pCfg->dst_fcmap_prefix = _shr_swap32(x_bfcmap_port_cfg.dst_fcmap_prefix);
#else
        pCfg->dst_fcmap_prefix = x_bfcmap_port_cfg.dst_fcmap_prefix;
#endif

        BFCMAP_SAL_MEMCPY(pCfg->dst_mac_addr, x_bfcmap_port_cfg.dst_mac_addr, sizeof(_shr_bmac_addr_t));

        switch (x_bfcmap_port_cfg.dst_mac_construct) {
            case XMOD_BFCMAP_ENCAP_FCOE_FPMA:
                pCfg->dst_mac_construct = BFCMAP_ENCAP_FCOE_FPMA;
                break;
            
            case XMOD_BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL:
                pCfg->dst_mac_construct = BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL;
                break;

            case XMOD_BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER:
                pCfg->dst_mac_construct = BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER;
                break;
        }
    }

    /* VLAN Tag */
    if (pCfg->action_mask & BFCMAP_ATTR_VLAN_TAG_MASK) {
#ifdef BE_HOST
        pCfg->vlan_tag = _shr_swap32(x_bfcmap_port_cfg.vlan_tag);
#else
        pCfg->vlan_tag = x_bfcmap_port_cfg.vlan_tag;
#endif
    }

    /* VFT Tag */
    if (pCfg->action_mask & BFCMAP_ATTR_VFT_TAG_MASK) {
#ifdef BE_HOST
        pCfg->vft_tag = _shr_swap32(x_bfcmap_port_cfg.vft_tag);
#else
        pCfg->vft_tag = x_bfcmap_port_cfg.vft_tag;
#endif
    }

    /* Mapper Length */
    if (pCfg->action_mask & BFCMAP_ATTR_MAPPER_LEN_MASK) {
        pCfg->mapper_len = BFCMAP_MAPPER_LENGTH; 
    }

    /* Ingress Mapper Bypass */
    if (pCfg->action_mask & BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK) {
#ifdef BE_HOST
        pCfg->ingress_mapper_bypass = _shr_swap32(x_bfcmap_port_cfg.ingress_mapper_bypass);
#else
        pCfg->ingress_mapper_bypass = x_bfcmap_port_cfg.ingress_mapper_bypass;
#endif
    }

    /* Egress Mapper Bypass */
    if (pCfg->action_mask & BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK) {
#ifdef BE_HOST
        pCfg->egress_mapper_bypass = _shr_swap32(x_bfcmap_port_cfg.egress_mapper_bypass);
#else
        pCfg->egress_mapper_bypass = x_bfcmap_port_cfg.egress_mapper_bypass;
#endif
    }

    /* Egress FC CRC Processing Mode */
    if (pCfg->action_mask & BFCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK) {
        
        switch (x_bfcmap_port_cfg.egress_fc_crc_mode) {
            case XMOD_BFCMAP_FC_CRC_MODE_NORMAL:
                pCfg->egress_fc_crc_mode = BFCMAP_FC_CRC_MODE_NORMAL;
                break;
            
            case XMOD_BFCMAP_FC_CRC_MODE_NO_CRC_CHECK:
                pCfg->egress_fc_crc_mode = BFCMAP_FC_CRC_MODE_NO_CRC_CHECK;
                break;
        }
    }

    /* Ingress VFT Header Processing Mode */
    if (pCfg->action_mask & BFCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK) {
        
        switch (x_bfcmap_port_cfg.ingress_vfthdr_proc_mode) {
            case XMOD_BFCMAP_VFTHDR_PRESERVE:
                pCfg->ingress_vfthdr_proc_mode = BFCMAP_VFTHDR_PRESERVE;
                break;
            
            case XMOD_BFCMAP_VFTHDR_INSERT:
                pCfg->ingress_vfthdr_proc_mode = BFCMAP_VFTHDR_INSERT;
                break;

            case XMOD_BFCMAP_VFTHDR_DELETE:
                 pCfg->ingress_vfthdr_proc_mode = BFCMAP_VFTHDR_DELETE;
                break;
        }
    }

    /* Egress VFT Header Processing Mode */
    if (pCfg->action_mask & BFCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK) {
        
        switch (x_bfcmap_port_cfg.egress_vfthdr_proc_mode) {
            case XMOD_BFCMAP_VFTHDR_PRESERVE:
                pCfg->egress_vfthdr_proc_mode = BFCMAP_VFTHDR_PRESERVE;
                break;
            
            case XMOD_BFCMAP_VFTHDR_INSERT:
                pCfg->egress_vfthdr_proc_mode = BFCMAP_VFTHDR_INSERT;
                break;

            case XMOD_BFCMAP_VFTHDR_DELETE:
                 pCfg->egress_vfthdr_proc_mode = BFCMAP_VFTHDR_DELETE;
                break;
        }
    }

    /* Ingress VLANTAG Processing Mode */
    if (pCfg->action_mask & BFCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK) {
        switch (x_bfcmap_port_cfg.ingress_vlantag_proc_mode) {
            case XMOD_BFCMAP_VLANTAG_PRESERVE:
                pCfg->ingress_vlantag_proc_mode = BFCMAP_VLANTAG_PRESERVE;
                break;
            
            case XMOD_BFCMAP_VLANTAG_INSERT:
                pCfg->ingress_vlantag_proc_mode = BFCMAP_VLANTAG_INSERT;
                break;
        }
    }

    /* Ingress VFID Mapper Source */
    if (pCfg->action_mask & BFCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK) {
        
        switch (x_bfcmap_port_cfg.ingress_vfid_mapsrc) {
            case XMOD_BFCMAP_VFID_MACSRC_PASSTHRU:
                pCfg->ingress_vfid_mapsrc = XMOD_BFCMAP_VFID_MACSRC_PASSTHRU;
                break;
            
            case XMOD_BFCMAP_VFID_MACSRC_PORT_DEFAULT:
                pCfg->ingress_vfid_mapsrc = XMOD_BFCMAP_VFID_MACSRC_PORT_DEFAULT;
                break;

            case XMOD_BFCMAP_VFID_MACSRC_DISABLE:
                 pCfg->ingress_vfid_mapsrc = XMOD_BFCMAP_VFID_MACSRC_DISABLE;
                break;
        }
    }

    /* Egress VFID Mapper Source */
    if (pCfg->action_mask & BFCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK) {
        
        switch (x_bfcmap_port_cfg.egress_vfid_mapsrc) {
            case XMOD_BFCMAP_VFID_MACSRC_PASSTHRU:
                pCfg->egress_vfid_mapsrc = XMOD_BFCMAP_VFID_MACSRC_PASSTHRU;
                break;
            
            case XMOD_BFCMAP_VFID_MACSRC_PORT_DEFAULT:
                pCfg->egress_vfid_mapsrc = XMOD_BFCMAP_VFID_MACSRC_PORT_DEFAULT;
                break;

            case XMOD_BFCMAP_VFID_MACSRC_VID:
                pCfg->egress_vfid_mapsrc = XMOD_BFCMAP_VFID_MACSRC_VID;
                break;

            case XMOD_BFCMAP_VFID_MACSRC_MAPPER:
                pCfg->egress_vfid_mapsrc = XMOD_BFCMAP_VFID_MACSRC_MAPPER;
                break;

            case XMOD_BFCMAP_VFID_MACSRC_DISABLE:
                 pCfg->egress_vfid_mapsrc = XMOD_BFCMAP_VFID_MACSRC_DISABLE;
                break;
        }
    }

    /* Ingress VID Mapper Source */
    if (pCfg->action_mask & BFCMAP_ATTR_INGRESS_VID_MAPSRC_MASK) {
        
        switch (x_bfcmap_port_cfg.ingress_vid_mapsrc) {
            case XMOD_BFCMAP_VID_MACSRC_VFID:
                pCfg->ingress_vid_mapsrc = BFCMAP_VID_MACSRC_VFID;
                break;
            
            case XMOD_BFCMAP_VID_MACSRC_PORT_DEFAULT:
                pCfg->ingress_vid_mapsrc = BFCMAP_VID_MACSRC_PORT_DEFAULT;
                break;

            case XMOD_BFCMAP_VID_MACSRC_MAPPER:
                 pCfg->ingress_vid_mapsrc = BFCMAP_VID_MACSRC_MAPPER;
                break;
        }
    }

    /* Ingress VFT Hop Count Check Mode */
    if (pCfg->action_mask & BFCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK) {
        
        switch (x_bfcmap_port_cfg.ingress_hopCnt_check_mode) {
            case XMOD_BFCMAP_HOPCNT_CHECK_MODE_NO_CHK:
                pCfg->ingress_hopCnt_check_mode = XMOD_BFCMAP_HOPCNT_CHECK_MODE_NO_CHK;
                break;
            
            case XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_DROP:
                pCfg->ingress_hopCnt_check_mode = XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_DROP;
                break;

            case XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_EOFNI:
                 pCfg->ingress_hopCnt_check_mode = XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_EOFNI;
                break;
        }
    }

    /* Enable VFT Hop Count Decrement on egress */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->egress_hopCnt_dec_enable = _shr_swap32(x_bfcmap_port_cfg.egress_hopCnt_dec_enable);
#else
        pCfg->egress_hopCnt_dec_enable = x_bfcmap_port_cfg.egress_hopCnt_dec_enable;
#endif
    }

    /* Use TTS or PCS during 16G AutoNeg */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_USE_TTS_PCS_16G_MASK) {
#ifdef BE_HOST
        pCfg->use_tts_pcs_16G = _shr_swap32(x_bfcmap_port_cfg.use_tts_pcs_16G);
#else
        pCfg->use_tts_pcs_16G = x_bfcmap_port_cfg.use_tts_pcs_16G;
#endif
    }

    /* Use TTS or PCS during 32G AutoNeg */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_USE_TTS_PCS_32G_MASK) {
        /* 32G supports TTS ONLY */
        pCfg->use_tts_pcs_32G = 0;
    }

    /* Enable/Disable link training for 16G */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_TRAINING_ENABLE_16G_MASK) {
#ifdef BE_HOST
        pCfg->training_enable_16G = _shr_swap32(x_bfcmap_port_cfg.training_enable_16G);
#else
        pCfg->training_enable_16G = x_bfcmap_port_cfg.training_enable_16G;
#endif
    }

    /* Enable/Disable link training for 32G */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_TRAINING_ENABLE_32G_MASK) {
#ifdef BE_HOST
        pCfg->training_enable_32G = _shr_swap32(x_bfcmap_port_cfg.training_enable_32G);
#else
        pCfg->training_enable_32G = x_bfcmap_port_cfg.training_enable_32G;
#endif
    }

    /* Enable/Disable FEC for 16G */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_FEC_ENABLE_16G_MASK) {
#ifdef BE_HOST
        pCfg->fec_enable_16G = _shr_swap32(x_bfcmap_port_cfg.fec_enable_16G);
#else
        pCfg->fec_enable_16G = x_bfcmap_port_cfg.fec_enable_16G;
#endif
    }

    /* Enable/Disable link FEC for 32G */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_FEC_ENABLE_32G_MASK) {
#ifdef BE_HOST
        pCfg->fec_enable_32G = _shr_swap32(x_bfcmap_port_cfg.fec_enable_32G);
#else
        pCfg->fec_enable_32G = x_bfcmap_port_cfg.fec_enable_32G;
#endif
    }

    /* Enable FCS corruption on if FCoE pkt has EOFni or EOFa on ingress */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_INGRESS_FCS_CRRPT_EOF_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->ingress_fcs_crrpt_eof_enable = _shr_swap32(x_bfcmap_port_cfg.ingress_fcs_crrpt_eof_enable);
#else
        pCfg->ingress_fcs_crrpt_eof_enable = x_bfcmap_port_cfg.ingress_fcs_crrpt_eof_enable;
#endif
    }

    /* Enable VLAN header insert on ingress */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->ingress_vlantag_presence_enable = _shr_swap32(x_bfcmap_port_cfg.ingress_vlantag_presence_enable);
#else
        pCfg->ingress_vlantag_presence_enable = x_bfcmap_port_cfg.ingress_vlantag_presence_enable;
#endif
    }

    /* Egress VFT Hop Count Check Mode */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_CHECK_MODE_MASK) {
        
        switch (x_bfcmap_port_cfg.egress_hopcnt_check_mode) {
            case XMOD_BFCMAP_HOPCNT_CHECK_MODE_NO_CHK:
                pCfg->egress_hopcnt_check_mode = XMOD_BFCMAP_HOPCNT_CHECK_MODE_NO_CHK;
                break;
            
            case XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_DROP:
                pCfg->egress_hopcnt_check_mode = XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_DROP;
                break;

            case XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_EOFNI:
                 pCfg->egress_hopcnt_check_mode = XMOD_BFCMAP_HOPCNT_CHECK_MODE_FAIL_EOFNI;
                break;
        }
    }

    /* Enable passing control frame on egress when mapper is in bypass mode  */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_PASS_CTRL_FRAME_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->egress_pass_ctrl_frame_enable = _shr_swap32(x_bfcmap_port_cfg.egress_pass_ctrl_frame_enable);
#else
        pCfg->egress_pass_ctrl_frame_enable = x_bfcmap_port_cfg.egress_pass_ctrl_frame_enable;
#endif
    }

    /* Enable passing pfc frame on egress when mapper is in bypass mode  */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_PASS_PFC_FRAME_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->egress_pass_pfc_frame_enable = _shr_swap32(x_bfcmap_port_cfg.egress_pass_pfc_frame_enable);
#else
        pCfg->egress_pass_pfc_frame_enable = x_bfcmap_port_cfg.egress_pass_pfc_frame_enable;
#endif
    }

    /* Enable passing pause frame on egress when mapper is in bypass mode  */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_PASS_PAUSE_FRAME_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->egress_pass_pause_frame_enable = _shr_swap32(x_bfcmap_port_cfg.egress_pass_pause_frame_enable);
#else
        pCfg->egress_pass_pause_frame_enable = x_bfcmap_port_cfg.egress_pass_pause_frame_enable;
#endif
    }

    /* Disable FCOE header version field check on egress  */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_FCOE_VER_CHK_DISABLE_MASK) {
#ifdef BE_HOST
        pCfg->egress_fcoe_version_chk_disable = _shr_swap32(x_bfcmap_port_cfg.egress_fcoe_version_chk_disable);
#else
        pCfg->egress_fcoe_version_chk_disable = x_bfcmap_port_cfg.egress_fcoe_version_chk_disable;
#endif
    }

    /* Default CoS value to use on egress  */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_DEFAULT_COS_VALUE_MASK) {
#ifdef BE_HOST
        pCfg->egress_default_cos_value = _shr_swap32(x_bfcmap_port_cfg.egress_default_cos_value);
#else
        pCfg->egress_default_cos_value = x_bfcmap_port_cfg.egress_default_cos_value;
#endif
    }

    /* Use IP CoS map values on egress  */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_USE_IP_COS_MAP_MASK) {
#ifdef BE_HOST
        pCfg->egress_use_ip_cos_map = _shr_swap32(x_bfcmap_port_cfg.egress_use_ip_cos_map);
#else
        pCfg->egress_use_ip_cos_map = x_bfcmap_port_cfg.egress_use_ip_cos_map;
#endif
    }

    /* Enable VFT Hop Count Decrement on ingress */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_INGRESS_HOPCNT_DEC_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->ingress_hopcnt_dec_enable = _shr_swap32(x_bfcmap_port_cfg.ingress_hopcnt_dec_enable);
#else
        pCfg->ingress_hopcnt_dec_enable = x_bfcmap_port_cfg.ingress_hopcnt_dec_enable;
#endif
    }

    /* Scrambling Enable/Disable */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_SCRAMBLING_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->scrambling_enable_value = _shr_swap32(x_bfcmap_port_cfg.scrambling_enable_value);
#else
        pCfg->scrambling_enable_value = x_bfcmap_port_cfg.scrambling_enable_value;
#endif

        /* 2G FC speed is not support */
        if (pCfg->scrambling_enable_mask & BFCMAP_PORT_SCRAMBLING_SPEED_2GBPS) {
            BFCMAP_SAL_DBG_PRINTF("2G FC speed is not supported \n");
        }

        /* Scrambling is always disabled for 4G */
        if (pCfg->scrambling_enable_mask & BFCMAP_PORT_SCRAMBLING_SPEED_4GBPS) {
            pCfg->scrambling_enable_value &= ~BFCMAP_PORT_SCRAMBLING_SPEED_4GBPS;
        }

        /* Scrambling is always enabled for 16G */
        if (pCfg->scrambling_enable_mask & BFCMAP_PORT_SCRAMBLING_SPEED_16GBPS) {
            pCfg->scrambling_enable_value |= BFCMAP_PORT_SCRAMBLING_SPEED_16GBPS;
        }

        /* Scrambling is always enabled for 32G */
        if (pCfg->scrambling_enable_mask & BFCMAP_PORT_SCRAMBLING_SPEED_32GBPS) {
            pCfg->scrambling_enable_value |= BFCMAP_PORT_SCRAMBLING_SPEED_32GBPS;
        }

        /* Scrambling state for 8G */
        if (pCfg->scrambling_enable_mask & BFCMAP_PORT_SCRAMBLING_SPEED_8GBPS) {
            if (pCfg->scrambling_enable_value & BFCMAP_PORT_SCRAMBLING_SPEED_8GBPS) {
                pCfg->scrambling_enable_value |= BFCMAP_PORT_SCRAMBLING_SPEED_8GBPS;
            } else {
                pCfg->scrambling_enable_value &= ~BFCMAP_PORT_SCRAMBLING_SPEED_8GBPS;
            }
        }
    }

    /* Egress Pause Enable */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_PAUSE_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->egress_pause_enable = _shr_swap32(x_bfcmap_port_cfg.egress_pause_enable);
#else
        pCfg->egress_pause_enable = x_bfcmap_port_cfg.egress_pause_enable;
#endif
    }

    /* Egress PFC Enable */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_EGRESS_PFC_ENABLE_MASK) {
#ifdef BE_HOST
        pCfg->egress_pfc_enable = _shr_swap32(x_bfcmap_port_cfg.egress_pfc_enable);
#else
        pCfg->egress_pfc_enable = x_bfcmap_port_cfg.egress_pfc_enable;
#endif
    }

    /* Stat Interval */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_STAT_INTERVAL_MASK) {
#ifdef BE_HOST
        pCfg->stat_interval = _shr_swap32(x_bfcmap_port_cfg.stat_interval);
#else
        pCfg->stat_interval = x_bfcmap_port_cfg.stat_interval;
#endif
    }
    /* Tranceiver Attributes  specification by config when 
       Transceiver I2c not connected to FW Micro Controller */
    if (pCfg->action_mask2 & BFCMAP_ATTR2_TRCM_ATTRIBS_MASK) {
#ifdef BE_HOST
        pCfg->trcm_attribs = _shr_swap32(x_bfcmap_port_cfg.trcm_attribs);
#else
        pCfg->trcm_attribs = x_bfcmap_port_cfg.trcm_attribs;
#endif
    }

    /* This configuration determines how COS of an ingress FC frame
       is translated to vlan priority. */

    if (pCfg->action_mask2 & BFCMAP_ATTR2_COS_TO_PRI_MASK) {
#ifdef BE_HOST
        pCfg->cos_to_pri.cos = _shr_swap16(x_bfcmap_port_cfg.cos_to_pri.cos);
        pCfg->cos_to_pri.pri = _shr_swap16(x_bfcmap_port_cfg.cos_to_pri.pri);
#else
        pCfg->cos_to_pri.cos = x_bfcmap_port_cfg.cos_to_pri.cos;
        pCfg->cos_to_pri.pri = x_bfcmap_port_cfg.cos_to_pri.pri;
#endif
    }

exit:
    /* BFCMAP_UNLOCK_PORT(mpc); */
    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_init
 * Purpose:
 *      Initializes a bfcmap port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      pCfg        - Pointer to port configuration
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap88060_port_init(bfcmap_port_ctrl_t *mpc, bfcmap_port_config_t *pCfg)
{
    int     rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;
    xmod_port_config_t *fcmap_port_cfg, x_port_cfg;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    fcmap_port_cfg = (xmod_port_config_t*)mpc->fcmap_port_cfg;

    /* FC is always in RETIMER mode */
    fcmap_port_cfg->port_mode = ETH_RETIMER;

    /* STRIP CRC is always 1 */
    fcmap_port_cfg->is_strip_crc = 1;

    fcmap_port_cfg->quad_mode = compute_quad_mode(unit, port);
#ifdef BE_HOST
    x_port_cfg.speed = _shr_swap32(fcmap_port_cfg->speed);
#else
    x_port_cfg.speed = fcmap_port_cfg->speed;
#endif
    x_port_cfg.interface = fcmap_port_cfg->interface;
    x_port_cfg.sys_lane_count = fcmap_port_cfg->sys_lane_count;
    x_port_cfg.ln_lane_count = fcmap_port_cfg->ln_lane_count;
    x_port_cfg.an_mode = fcmap_port_cfg->an_mode;
    x_port_cfg.an_cl72 = fcmap_port_cfg->an_cl72;
    x_port_cfg.fs_cl72 = fcmap_port_cfg->fs_cl72;
    x_port_cfg.fs_cl72_sys = fcmap_port_cfg->fs_cl72_sys;
    x_port_cfg.an_fec = fcmap_port_cfg->an_fec;
    x_port_cfg.port_is_higig = fcmap_port_cfg->port_is_higig;
    x_port_cfg.fiber_pref = fcmap_port_cfg->fiber_pref;
    x_port_cfg.fiber_pref_sys = fcmap_port_cfg->fiber_pref_sys;
    x_port_cfg.port_mode = fcmap_port_cfg->port_mode;
    x_port_cfg.is_bootmaster = fcmap_port_cfg->is_bootmaster;
    x_port_cfg.is_strip_crc = fcmap_port_cfg->is_strip_crc;
    x_port_cfg.is_flex = fcmap_port_cfg->is_flex;
    x_port_cfg.quad_mode = fcmap_port_cfg->quad_mode;

    BFCMAP_SAL_DBG_PRINTF("%s:%d u %d p %d quad_mode %d sys_lane_count %d is_flex %d \n",
                        __FUNCTION__, __LINE__, unit, port, x_port_cfg.quad_mode,
                       fcmap_port_cfg->sys_lane_count, fcmap_port_cfg->is_flex);

    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_port_cfg, sizeof(xmod_port_config_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_INIT);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_INIT),
                                xmodtxbuff, xmodtxlen, NULL, 0);
    /* clear stats */
    rv = bfcmap88060_stat_clear(mpc);

    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_uninit
 * Purpose:
 *      Initializes a bfcmap port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      pCfg        - Pointer to port configuration
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap88060_port_uninit(bfcmap_port_ctrl_t *mpc)
{
#ifdef UNINIT_XMOD
    int     rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_cmd_t x_bfcmap_port_cmd;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    x_bfcmap_port_cmd = XMOD_BFCMAP_PORT_UNINIT;
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_cmd, sizeof(xmod_bfcmap_port_cmd_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CMD_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CMD_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);
    
    return rv;
#else
    return BFCMAP_E_NONE;
#endif
}

/*
 * Function:
 *      bfcmap88060_port_reset
 * Purpose:
 *      Issues a link reset on the port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap88060_port_reset(bfcmap_port_ctrl_t *mpc)
{
    int     rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_cmd_t x_bfcmap_port_cmd;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);


    x_bfcmap_port_cmd = XMOD_BFCMAP_PORT_RESET;
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_cmd, sizeof(xmod_bfcmap_port_cmd_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CMD_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CMD_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_shutdown
 * Purpose:
 *      Disables the port
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap88060_port_shutdown(bfcmap_port_ctrl_t *mpc)
{
    int     rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_cmd_t x_bfcmap_port_cmd;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    x_bfcmap_port_cmd = XMOD_BFCMAP_PORT_SHUTDOWN;
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_cmd, sizeof(xmod_bfcmap_port_cmd_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CMD_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CMD_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_link_enable
 * Purpose:
 *      Enable the port
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap88060_port_link_enable(bfcmap_port_ctrl_t *mpc)
{
    int     rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_cmd_t x_bfcmap_port_cmd;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    x_bfcmap_port_cmd = XMOD_BFCMAP_PORT_LINK_ENABLE;
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_cmd, sizeof(xmod_bfcmap_port_cmd_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CMD_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CMD_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}


/*
 * Function:
 *      bfcmap88060_port_link_bounce
 * Purpose:
 *      Bounce the link
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap88060_port_link_bounce(bfcmap_port_ctrl_t *mpc)
{
    int     rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_cmd_t x_bfcmap_port_cmd;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    x_bfcmap_port_cmd = XMOD_BFCMAP_PORT_BOUNCE;
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_cmd, sizeof(xmod_bfcmap_port_cmd_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CMD_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CMD_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

/*
 * Function:
 *      bfcmap88060_linkfault_trigger_rc_get
 * Purpose:
 *      Returns the current link fault trigger and reason code from the FC port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      tr        - Pointer to the to be returned trigger type
 *      rc       - Pointer to the to be returned reason code
 * Returns:
 *      BFCMAP_E_NONE   - Success
 *      BFCMAP_E_PARAM - parameter error
 */
STATIC int 
bfcmap88060_linkfault_trigger_rc_get(bfcmap_port_ctrl_t *mpc, 
                          bfcmap_lf_tr_t *tr, bfcmap_lf_rc_t *rc)
{
    uint32 xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *bufrxptr;
    int xmodrxlen, rv = BFCMAP_E_NONE;
    xmod_bfcmap_lf_tr_t x_bfcmap_lf_trig;
    xmod_bfcmap_lf_rc_t x_bfcmap_lf_rc;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);
    
    if (tr == (bfcmap_lf_tr_t *)NULL) {
        BFCMAP_SAL_DBG_PRINTF("link fault trigger is NULL\n");
        return BFCMAP_E_PARAM;
    }

    if (rc == (bfcmap_lf_rc_t *)NULL) {
        BFCMAP_SAL_DBG_PRINTF("link fault reason code is NULL\n");
        return BFCMAP_E_PARAM;
    }

    BFCMAP_SAL_MEMSET(&x_bfcmap_lf_trig, 0, sizeof(xmod_bfcmap_lf_tr_t));
    BFCMAP_SAL_MEMSET(&x_bfcmap_lf_rc, 0, sizeof(xmod_bfcmap_lf_rc_t));
    
    /* call xmod */
    xmodrxlen = GET_XMOD_FC_CMD_OUT_LEN(XMOD_BFCMAP_PORT_LINK_FAULT_TRIGGER_RC_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_LINK_FAULT_TRIGGER_RC_GET),
                           NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8 *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_bfcmap_lf_trig, sizeof(xmod_bfcmap_lf_tr_t));
    READ_XMOD_ARG_BUFF(bufrxptr, &x_bfcmap_lf_rc, sizeof(xmod_bfcmap_lf_rc_t));

    switch(x_bfcmap_lf_trig) {
        case XMOD_BFCMAP_LF_TR_NONE:
            *tr = BFCMAP_LF_TR_NONE;
            break;
        case XMOD_BFCMAP_LF_TR_PORT_INIT:
            *tr = BFCMAP_LF_TR_PORT_INIT;
            break;
        case XMOD_BFCMAP_LF_TR_OPEN_LINK:
            *tr = BFCMAP_LF_TR_OPEN_LINK;
            break;
        case XMOD_BFCMAP_LF_TR_LINK_FAILURE:
            *tr = BFCMAP_LF_TR_LINK_FAILURE;
            break;
        case XMOD_BFCMAP_LF_TR_OLS_RCVD:
            *tr = BFCMAP_LF_TR_OLS_RCVD;
            break;
        case XMOD_BFCMAP_LF_TR_NOS_RCVD:
            *tr = BFCMAP_LF_TR_NOS_RCVD;
            break;
        case XMOD_BFCMAP_LF_TR_SYNC_LOSS:
            *tr = BFCMAP_LF_TR_SYNC_LOSS;
            break;
        case XMOD_BFCMAP_LF_TR_BOUCELINK_FROM_ADMIN:
            *tr = BFCMAP_LF_TR_BOUCELINK_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_LF_TR_CHGSPEED_FROM_ADMIN:
            *tr = BFCMAP_LF_TR_CHGSPEED_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_LF_TR_DISABLE_FROM_ADMIN:
            *tr = BFCMAP_LF_TR_DISABLE_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_LF_TR_RESET_FROM_ADMIN:
            *tr = BFCMAP_LF_TR_RESET_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_LF_TR_LR_RCVD:
            *tr = BFCMAP_LF_TR_LR_RCVD;
            break;
        case XMOD_BFCMAP_LF_TR_LRR_RCVD:
            *tr = BFCMAP_LF_TR_LRR_RCVD;
            break;
        case XMOD_BFCMAP_LF_TR_ED_TOV:
            *tr = BFCMAP_LF_TR_ED_TOV;
            break;
        default:
            rv = BFCMAP_E_PARAM;
            break;
    }

    switch(x_bfcmap_lf_rc) {
        case XMOD_BFCMAP_LF_RC_NONE:
            *rc = BFCMAP_LF_RC_NONE;
            break;
        case XMOD_BFCMAP_LF_RC_PORT_INIT:
            *rc = BFCMAP_LF_RC_PORT_INIT;
            break;
        case XMOD_BFCMAP_LF_RC_OPEN_LINK:
            *rc = BFCMAP_LF_RC_OPEN_LINK;
            break;
        case XMOD_BFCMAP_LF_RC_LINK_FAILURE:
            *rc = BFCMAP_LF_RC_LINK_FAILURE;
            break;
        case XMOD_BFCMAP_LF_RC_OLS_RCVD:
            *rc = BFCMAP_LF_RC_OLS_RCVD;
            break;
        case XMOD_BFCMAP_LF_RC_NOS_RCVD:
            *rc = BFCMAP_LF_RC_NOS_RCVD;
            break;
        case XMOD_BFCMAP_LF_RC_SYNC_LOSS:
            *rc = BFCMAP_LF_RC_SYNC_LOSS;
            break;
        case XMOD_BFCMAP_LF_RC_BOUCELINK_FROM_ADMIN:
            *rc = BFCMAP_LF_RC_BOUCELINK_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_LF_RC_CHGSPEED_FROM_ADMIN:
            *rc = BFCMAP_LF_RC_CHGSPEED_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_LF_RC_DISABLE_FROM_ADMIN:
            *rc = BFCMAP_LF_RC_DISABLE_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_LF_RC_RESET_FAILURE:
            *rc = BFCMAP_LF_RC_RESET_FAILURE;
            break;
        default:
            rv = BFCMAP_E_PARAM;
            break;
    }

    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_diag_get
 * Purpose:
 *      Returns the current diagnostic code from the FC port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      diag         - Pointer to the to be returned diagnostic code 
 * Returns:
 *      BFCMAP_E_NONE   - Success
 *      BFCMAP_E_PARAM - parameter error
 */
STATIC int 
bfcmap88060_port_diag_get(bfcmap_port_ctrl_t *mpc, 
                          bfcmap_diag_code_t *diag)
{
    uint32 xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *bufrxptr;
    int xmodrxlen, rv = BFCMAP_E_NONE;
    xmod_bfcmap_diag_code_t x_bfcmap_diag_code;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);
    
    if (diag == (bfcmap_diag_code_t *)NULL) {
        BFCMAP_SAL_DBG_PRINTF("diag is NULL\n");
        return BFCMAP_E_PARAM;
    }

    BFCMAP_SAL_MEMSET(&x_bfcmap_diag_code, 0, sizeof(xmod_bfcmap_diag_code_t));

    /* call xmod */
    xmodrxlen = GET_XMOD_FC_CMD_OUT_LEN(XMOD_BFCMAP_PORT_DIAG_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_DIAG_GET),
                           NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8 *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_bfcmap_diag_code, sizeof(xmod_bfcmap_diag_code_t));

    switch(x_bfcmap_diag_code) {
        case XMOD_BFCMAP_DIAG_OK:
            *diag = BFCMAP_DIAG_OK;
            break;
        case XMOD_BFCMAP_DIAG_PORT_INIT:
            *diag = BFCMAP_DIAG_PORT_INIT;
            break;
        case XMOD_BFCMAP_DIAG_OPEN_LINK:
            *diag = BFCMAP_DIAG_OPEN_LINK;
            break;
        case XMOD_BFCMAP_DIAG_LINK_FAILURE:
            *diag = BFCMAP_DIAG_LINK_FAILURE;
            break;
        case XMOD_BFCMAP_DIAG_OLS_RCVD:
            *diag = BFCMAP_DIAG_OLS_RCVD;
            break;
        case XMOD_BFCMAP_DIAG_NOS_RCVD:
            *diag = BFCMAP_DIAG_NOS_RCVD;
            break;
        case XMOD_BFCMAP_DIAG_SYNC_LOSS:
            *diag = BFCMAP_DIAG_SYNC_LOSS;
            break;
        case XMOD_BFCMAP_DIAG_BOUCELINK_FROM_ADMIN:
            *diag = BFCMAP_DIAG_BOUCELINK_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_DIAG_CHGSPEED_FROM_ADMIN:
            *diag = BFCMAP_DIAG_CHGSPEED_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_DIAG_DISABLE_FROM_ADMIN:
            *diag = BFCMAP_DIAG_DISABLE_FROM_ADMIN;
            break;
        case XMOD_BFCMAP_DIAG_AN_NO_SIGNAL:
            *diag = BFCMAP_DIAG_AN_NO_SIGNAL;
            break;
        case XMOD_BFCMAP_DIAG_AN_TIMEOUT:
            *diag = BFCMAP_DIAG_AN_TIMEOUT;
            break;
        case XMOD_BFCMAP_DIAG_PROTO_TIMEOUT:
            *diag = BFCMAP_DIAG_PROTO_TIMEOUT;
            break;
        default:
            rv = BFCMAP_E_PARAM;
            break;
    }

    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_ability_advert_set
 * Purpose:
 *      Sets discrete port speeds for AN or single forced port speed
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      ability_mask - Pointer to Bitmap representing AN or forced speed values to be set 
 * Returns:
 *      BFCMAP_E_NONE   - Success
 *      BFCMAP_E_PARAM  - parameter error
 */
STATIC int 
bfcmap88060_port_ability_advert_set(bfcmap_port_ctrl_t *mpc, 
                                    bfcmap_port_ability_t *ability_mask)
{
    uint32 speed_bmap = 0;
    int count = 0, rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_ability_t x_bfcmap_port_ability;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    if (ability_mask == (bfcmap_port_ability_t *)NULL) {
        BFCMAP_SAL_DBG_PRINTF("ability_mask is NULL\n");
        return BFCMAP_E_PARAM;
    }

    if (ability_mask->port_ability_speed & BFCMAP_PORT_ABILITY_SPEED_2GBPS) {
        BFCMAP_SAL_DBG_PRINTF("2GBPS speed is not supported on MT2\n");
        return BFCMAP_E_PARAM;
    }

    /* If AN bit is not set, it indicates forced speed is requested
     * Only one forced speed value can be requested.
     */
    if (!(ability_mask->port_ability_speed & BFCMAP_PORT_ABILITY_SPEED_AN)) {
        speed_bmap = ability_mask->port_ability_speed;

        /* Check if multiple bits are set in forced speed request */
        while (speed_bmap) {
            speed_bmap &= (speed_bmap - 1);
            count++;
            if (count >= 2) {
                BFCMAP_SAL_DBG_PRINTF("Multiple forced speeds cannot be set\n");
                return BFCMAP_E_PARAM;
            }
        }
    }

    BFCMAP_SAL_MEMSET(&x_bfcmap_port_ability, 0, sizeof(xmod_bfcmap_port_ability_t));
#ifdef BE_HOST
        x_bfcmap_port_ability.port_ability_speed = _shr_swap32(ability_mask->port_ability_speed);
#else
        x_bfcmap_port_ability.port_ability_speed = ability_mask->port_ability_speed;
#endif

    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_ability, sizeof(xmod_bfcmap_port_ability_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_ABIL_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_ABIL_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_ability_advert_get
 * Purpose:
 *      Retrieves supported port speeds
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      ability_mask - Pointer to Bitmap representing supported AN or forced speed values 
 * Returns:
 *      BFCMAP_E_NONE   - Success
 *      BFCMAP_E_PARAM  - parameter error
 */
STATIC int 
bfcmap88060_port_ability_advert_get(bfcmap_port_ctrl_t *mpc, 
                                    bfcmap_port_ability_t *ability_mask)
{
    uint32 xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *bufrxptr;
    int xmodrxlen, rv = BFCMAP_E_NONE;
    xmod_bfcmap_port_ability_t x_bfcmap_port_ability;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);
    
    if (ability_mask == (bfcmap_port_ability_t *)NULL) {
        BFCMAP_SAL_DBG_PRINTF("ability_mask is NULL\n");
        return BFCMAP_E_PARAM;
    }

    BFCMAP_SAL_MEMSET(&x_bfcmap_port_ability, 0, sizeof(xmod_bfcmap_port_ability_t));

    /* call xmod */
    xmodrxlen = GET_XMOD_FC_CMD_OUT_LEN(XMOD_BFCMAP_PORT_ABIL_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_ABIL_GET),
                           NULL, 0, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8 *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_bfcmap_port_ability, sizeof(xmod_bfcmap_port_ability_t));

#ifdef BE_HOST
        ability_mask->port_ability_speed = _shr_swap32(x_bfcmap_port_ability.port_ability_speed);
#else
        ability_mask->port_ability_speed = x_bfcmap_port_ability.port_ability_speed;
#endif

    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_speed_set
 * Purpose:
 *      Sets speed on a bfcmap port
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      speed        - Speed to set
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */

STATIC int 
bfcmap88060_port_speed_set(bfcmap_port_ctrl_t *mpc, bfcmap_port_speed_t speed)
{
    bfcmap_port_ability_t ability_mask;
    bfcmap_port_ability_t ability_mask_get;
    int rv = BFCMAP_E_NONE;

    BFCMAP_SAL_MEMSET(&ability_mask, 0, sizeof(bfcmap_port_ability_t));
    BFCMAP_SAL_MEMSET(&ability_mask_get, 0, sizeof(bfcmap_port_ability_t));

    rv = bfcmap88060_port_ability_advert_get(mpc, &ability_mask_get);

    switch (speed) {
        case BFCMAP_PORT_SPEED_4GBPS:
            ability_mask.port_ability_speed |= BFCMAP_PORT_ABILITY_SPEED_4GBPS;
            break;
        case BFCMAP_PORT_SPEED_8GBPS:
            ability_mask.port_ability_speed |= BFCMAP_PORT_ABILITY_SPEED_8GBPS;
            break;
        case BFCMAP_PORT_SPEED_16GBPS:
            ability_mask.port_ability_speed |= BFCMAP_PORT_ABILITY_SPEED_16GBPS;
            break;
        case BFCMAP_PORT_SPEED_32GBPS:
            ability_mask.port_ability_speed |= BFCMAP_PORT_ABILITY_SPEED_32GBPS;
            break;
        case BFCMAP_PORT_SPEED_AN_4GBPS:
            ability_mask.port_ability_speed |= (BFCMAP_PORT_ABILITY_SPEED_AN |
                                                BFCMAP_PORT_ABILITY_SPEED_4GBPS);
            break;
        case BFCMAP_PORT_SPEED_AN_8GBPS:
            ability_mask.port_ability_speed |= (BFCMAP_PORT_ABILITY_SPEED_AN    |
                                                BFCMAP_PORT_ABILITY_SPEED_4GBPS |
                                                BFCMAP_PORT_ABILITY_SPEED_8GBPS);
            break;
        case BFCMAP_PORT_SPEED_AN_16GBPS:
            ability_mask.port_ability_speed |= (BFCMAP_PORT_ABILITY_SPEED_AN    |
                                                BFCMAP_PORT_ABILITY_SPEED_4GBPS |
                                                BFCMAP_PORT_ABILITY_SPEED_8GBPS |
                                                BFCMAP_PORT_ABILITY_SPEED_16GBPS);
            break;
        case BFCMAP_PORT_SPEED_AN_32GBPS:
        case BFCMAP_PORT_SPEED_AN:
            ability_mask.port_ability_speed |= (BFCMAP_PORT_ABILITY_SPEED_AN    |
                                                BFCMAP_PORT_ABILITY_SPEED_4GBPS |
                                                BFCMAP_PORT_ABILITY_SPEED_8GBPS |
                                                BFCMAP_PORT_ABILITY_SPEED_16GBPS|
                                                BFCMAP_PORT_ABILITY_SPEED_32GBPS);
            break;
        default:
            return BFCMAP_E_PARAM;
    }

    /* if AN with certain max speed is requested */
    if (ability_mask.port_ability_speed & BFCMAP_PORT_ABILITY_SPEED_AN) {
        /* Remove unsupported speeds from requested AN with max speed */
        ability_mask.port_ability_speed &= ability_mask_get.port_ability_speed;
        /* Add back AN bit, which will be removed above */
        ability_mask.port_ability_speed |= BFCMAP_PORT_ABILITY_SPEED_AN;
    }

    rv = bfcmap88060_port_ability_advert_set(mpc, &ability_mask);

    return rv;
}

/*
 * Function:
 *      bfcmap88060_port_private_data_set
 * Purpose:
 *      Retrieves supported port speeds
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      ability_mask - Pointer to Bitmap representing supported AN or forced speed values 
 * Returns:
 *      BFCMAP_E_NONE   - Success
 *      BFCMAP_E_PARAM  - parameter error
 */
STATIC int 
bfcmap88060_port_private_data_set(bfcmap_port_ctrl_t *mpc, buint8_t *data, int len)
{
    bfcmap_inband_fr_payload_t *payload;
    bfcmap_inb_pl_t            *pl_ptr;
    uint8_t                    tlv_count = 0;
    int                        fc_event  = 0;
    bfcmap_dev_ctrl_t          *mdc;

     if ((data == NULL) || (len == 0)) {
        return BFCMAP_E_PARAM;
    }

    /* BFCMAP_SAL_PRINTF("Received %d bytes starting with 0x%x 0x%x\n", len, data[0], data[1]); */

    mdc = mpc->parent;
    payload = (bfcmap_inband_fr_payload_t*)data;

#ifndef BE_HOST
    payload->fc_inb_signature.sign = _shr_swap16(payload->fc_inb_signature.sign);
#endif

    if (payload->fc_inb_signature.sign != FC_INBAND_FRAME_SIGNATURE) {
        BFCMAP_SAL_DBG_PRINTF("Invalid Inband Frame Signature\n");
        return BFCMAP_E_PARAM;
    }

    if (payload->fc_inb_signature.num_tlv > FC_INBAND_NUM_TLV_MAX) {
        BFCMAP_SAL_DBG_PRINTF("Invalid Number of TLV s\n");
        return BFCMAP_E_PARAM;
    }

    tlv_count = payload->fc_inb_signature.num_tlv;
    pl_ptr = &payload->pl1;

    while (tlv_count) {
#ifndef BE_HOST
        pl_ptr->tlv.pld_type = _shr_swap16(pl_ptr->tlv.pld_type);
        pl_ptr->tlv.pld_len = _shr_swap16(pl_ptr->tlv.pld_len);
#endif

        switch(pl_ptr->tlv.pld_type) {
            case FC_INB_PLD_TYPE_FC_MIBS:
                if(pl_ptr->tlv.pld_len != sizeof(bfcmap_mibs_t)) {
                    BFCMAP_SAL_DBG_PRINTF("Invalid FC MIBs Length\n");
                    return BFCMAP_E_PARAM;
                }

                bfcmap88060_fc_mibs_update(mdc->mibs, mdc->prev_mibs, &pl_ptr->word[0]);
                break;
            case FC_INB_PLD_TYPE_FC_EVENTS:
                /* Assuming single FC event information is passed in the payload
                 * This could be changed to a bitmap of pending events
                 */
                fc_event = bfcmap88060_fc_event_update(&pl_ptr->word[0]); 

                if (fc_event >= BFCMAP_EVENT__COUNT) {
                    BFCMAP_SAL_DBG_PRINTF("Invalid FC Event\n");
                    return BFCMAP_E_PARAM;
                }

                /* BFCMAP_SAL_DBG_PRINTF("Event 0x%x port 0x%x\n", fc_event, BFCMAP_GET_PORT_ID(mpc)); */

                /* indicate that event has occurred */
                bfcmap_event(mpc, fc_event, -1, -1);
                break;
            default:
                BFCMAP_SAL_DBG_PRINTF("Invalid Payload Type 0x%x\n", pl_ptr->tlv.pld_type);
                return BFCMAP_E_PARAM;
        }
        tlv_count--;
        /* Adjust the current payload pointer to next payload */
        pl_ptr = (bfcmap_inb_pl_t*)((uint8_t*)pl_ptr + ((sizeof(bfcmap_inb_tlv_t)+ pl_ptr->tlv.pld_len)));
    }

    return BFCMAP_E_NONE;
}

/*
 * Function:
 *      bfcmap88060_vlan_map_add
 * Purpose:
 *      Sets the vlan to vsan mapping
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      vlan         - vlan to vsan mapping structure to be added
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */

STATIC int 
bfcmap88060_vlan_map_add(bfcmap_port_ctrl_t *mpc, bfcmap_vlan_vsan_map_t *vlan)
{
    int rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_vlan_vsan_map_t x_vlan;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    if (vlan == (bfcmap_vlan_vsan_map_t *)NULL) {
        BFCMAP_SAL_DBG_PRINTF("vlan is NULL\n");
        return BFCMAP_E_PARAM;
    }

    BFCMAP_SAL_MEMSET(&x_vlan, 0, sizeof(xmod_bfcmap_vlan_vsan_map_t));

    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

#ifdef BE_HOST
    x_vlan.vlan_vid  = _shr_swap16(vlan->vlan_vid);
    x_vlan.vsan_vfid = _shr_swap16(vlan->vsan_vfid);
#else
    x_vlan.vlan_vid  = vlan->vlan_vid;
    x_vlan.vsan_vfid = vlan->vsan_vfid;
#endif

    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_vlan, sizeof(xmod_bfcmap_vlan_vsan_map_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_VLAN_MAP_ADD);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_VLAN_MAP_ADD),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

/*
 * Function:
 *      bfcmap88060_vlan_map_delete
 * Purpose:
 *      Deletes the vlan to vsan mapping
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      vlan         - vlan to vsan mapping structure to be delete
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */

STATIC int 
bfcmap88060_vlan_map_delete(bfcmap_port_ctrl_t *mpc, bfcmap_vlan_vsan_map_t *vlan)
{
    int rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_vlan_vsan_map_t x_vlan;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    if (vlan == (bfcmap_vlan_vsan_map_t *)NULL) {
        BFCMAP_SAL_DBG_PRINTF("vlan is NULL\n");
        return BFCMAP_E_PARAM;
    }

    BFCMAP_SAL_MEMSET(&x_vlan, 0, sizeof(xmod_bfcmap_vlan_vsan_map_t));

#ifdef BE_HOST
    x_vlan.vlan_vid  = _shr_swap16(vlan->vlan_vid);
    x_vlan.vsan_vfid = _shr_swap16(vlan->vsan_vfid);
#else
    x_vlan.vlan_vid  = vlan->vlan_vid;
    x_vlan.vsan_vfid = vlan->vsan_vfid;
#endif

    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_vlan, sizeof(xmod_bfcmap_vlan_vsan_map_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_VLAN_MAP_DELETE);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_VLAN_MAP_DELETE),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

/*
 * Function:
 *      bfcmap88060_vlan_map_get
 * Purpose:
 *      Retrieves the vlan to vsan mapping
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      vlan         - vlan to vsan mapping structure to get in
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */

STATIC int 
bfcmap88060_vlan_map_get(bfcmap_port_ctrl_t *mpc, bfcmap_vlan_vsan_map_t *vlan)
{
    int rv = BFCMAP_E_NONE, xmodrxlen, xmodtxlen;
    uint32  xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *bufrxptr, *buftxptr;
    xmod_bfcmap_vlan_vsan_map_t x_rx_vlan, x_tx_vlan;
    bfcmap_dev_ctrl_t *mdc = mpc->parent;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    if (vlan == (bfcmap_vlan_vsan_map_t *)NULL) {
        BFCMAP_SAL_DBG_PRINTF("vlan is NULL\n");
        return BFCMAP_E_PARAM;
    }

    BFCMAP_SAL_MEMSET(&x_rx_vlan, 0, sizeof(xmod_bfcmap_vlan_vsan_map_t));
    BFCMAP_SAL_MEMSET(&x_tx_vlan, 0, sizeof(xmod_bfcmap_vlan_vsan_map_t));

#ifdef BE_HOST
    x_tx_vlan.vlan_vid  = _shr_swap16(vlan->vlan_vid);
    x_tx_vlan.vsan_vfid = _shr_swap16(vlan->vsan_vfid);
#else
    x_tx_vlan.vlan_vid  = vlan->vlan_vid;
    x_tx_vlan.vsan_vfid = vlan->vsan_vfid;
#endif
    
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_tx_vlan, sizeof(xmod_bfcmap_vlan_vsan_map_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_VLAN_MAP_GET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    xmodrxlen = GET_XMOD_FC_CMD_OUT_LEN(XMOD_BFCMAP_VLAN_MAP_GET);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_VLAN_MAP_GET),
                        xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8 *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_rx_vlan, sizeof(xmod_bfcmap_vlan_vsan_map_t));

#ifdef BE_HOST
        vlan->vlan_vid  = _shr_swap16(x_rx_vlan.vlan_vid);
        vlan->vsan_vfid = _shr_swap16(x_rx_vlan.vsan_vfid);
#else
        vlan->vlan_vid  = x_rx_vlan.vlan_vid;
        vlan->vsan_vfid = x_rx_vlan.vsan_vfid;
#endif

    return rv;
}

/*
 * Function:
 *      bfcmap88060_stat_get
 * Purpose:
 *      Return the statistics for the specified statistic type
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      stat         - specified stat type
 *      val          - Pointer to uint64 for returning statistics
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */

STATIC int 
bfcmap88060_stat_get(bfcmap_port_ctrl_t *mpc, bfcmap_stat_t stat, buint64_t *val)
{
    bfcmap_dev_ctrl_t *mdc;
    
    if (val == NULL) {
        BFCMAP_SAL_DBG_PRINTF("Stat return pointer is NULL\n");
        return BFCMAP_E_PARAM;
    }

    if (stat >= bfcmap_stat__count) {
        BFCMAP_SAL_DBG_PRINTF("Invalid Stat counter requested\n");
        return BFCMAP_E_PARAM;
    }

    mdc = mpc->parent;
    
    switch(stat) {
        case fc_rxgoodframes:
            *val = (mdc->mibs->fc_c2_rxgoodframes + mdc->mibs->fc_c3_rxgoodframes + 
                    mdc->mibs->fc_cf_rxgoodframes);
            break;
        case fc_rxbbcredit0:
            *val = mdc->mibs->fc_rxbbcredit0;
            break;
        case fc_rxinvalidcrc:
            *val = (mdc->mibs->fc_c2_rxinvalidcrc + mdc->mibs->fc_c3_rxinvalidcrc + 
                    mdc->mibs->fc_cf_rxinvalidcrc);
            break;
        case fc_rxframetoolong:
            *val = (mdc->mibs->fc_c2_rxframetoolong + mdc->mibs->fc_c3_rxframetoolong + 
                    mdc->mibs->fc_cf_rxframetoolong);
            break;
        case fc_rxdelimitererr:
            *val = mdc->mibs->fc_rxdelimitererr;
            break;
        case fc_rxruntframes:
            *val = (mdc->mibs->fc_rx_c2_runtframes + mdc->mibs->fc_rx_cf_runtframes + 
                    mdc->mibs->fc_rx_c3_runtframes);
            break;
        case fc_rxlinkfail:
            *val = mdc->mibs->fc_rxlinkfail;
            break;
        case fc_rxlipcount:
            *val = mdc->mibs->fc_rxlpicount;
            break;
        case fc_rxlosssig:
            *val = mdc->mibs->fc_rxlosssig;
            break;
        case fc_rxprimseqerr:
            *val = mdc->mibs->fc_rxprimseqerr;
            break;
        case fc_rxinvalidset:
            *val = mdc->mibs->fc_rxinvalidset;
            break;
        case fc_rxencodedisparity:
            *val = mdc->mibs->fc_rxencodedisparity;
            break;
        case fc_rxbyt:
            *val = (mdc->mibs->fc_c2_rxbyt + mdc->mibs->fc_c3_rxbyt + 
                    mdc->mibs->fc_cf_rxbyt);
            break;
        case fc_txbbcredit0:
            *val = mdc->mibs->fc_txbbcredit0;
            break;
        case fc_txbyt:
            *val = (mdc->mibs->fc_c2_txbyt + mdc->mibs->fc_c3_txbyt + 
                    mdc->mibs->fc_cf_txbyt);
            break;
        case fc_class2_rxgoodframes:
            *val = mdc->mibs->fc_c2_rxgoodframes; 
            break;
        case fc_class2_rxinvalidcrc:
            *val = mdc->mibs->fc_c2_rxinvalidcrc; 
            break;
        case fc_class2_rxframetoolong:
            *val = mdc->mibs->fc_c2_rxframetoolong; 
            break;
        case fc_class3_rxgoodframes:
            *val = mdc->mibs->fc_c3_rxgoodframes; 
            break;
        case fc_class3_rxinvalidcrc:
            *val = mdc->mibs->fc_c3_rxinvalidcrc; 
            break;
        case fc_class3_rxframetoolong:
            *val = mdc->mibs->fc_c3_rxframetoolong; 
            break;
        case fc_classf_rxgoodframes:
            *val = mdc->mibs->fc_cf_rxgoodframes; 
            break;
        case fc_classf_rxinvalidcrc:
            *val = mdc->mibs->fc_cf_rxinvalidcrc; 
            break;
        case fc_classf_rxframetoolong:
            *val = mdc->mibs->fc_cf_rxframetoolong; 
            break;
        case fc_rxbbc0drop:
            *val = mdc->mibs->fc_rxbbc0drop;
            break;
        case fc_rxsyncfail:
            *val = mdc->mibs->fc_rxsyncfail;
            break;
        case fc_rxbadxword:
            *val = mdc->mibs->fc_rxbadxword;
            break;
        case fc_class2_rxruntframes:
            *val = mdc->mibs->fc_rx_c2_runtframes;
            break;
        case fc_class3_rxruntframes:
            *val = mdc->mibs->fc_rx_c3_runtframes;
            break;
        case fc_classf_rxruntframes:
            *val = mdc->mibs->fc_rx_cf_runtframes;
            break;
        case fc_class2_rxbyt:
            *val = mdc->mibs->fc_c2_rxbyt;
            break;
        case fc_class3_rxbyt:
            *val = mdc->mibs->fc_c3_rxbyt;
            break;
        case fc_classf_rxbyt:
            *val = mdc->mibs->fc_cf_rxbyt;
            break;
        case fc_class2_txframes:
            *val = mdc->mibs->fc_tx_c2_frames;
            break;
        case fc_class3_txframes:
            *val = mdc->mibs->fc_tx_c3_frames;
            break;
        case fc_classf_txframes:
            *val = mdc->mibs->fc_tx_cf_frames;
            break;
        case fc_txframes:
            *val = (mdc->mibs->fc_tx_c2_frames + mdc->mibs->fc_tx_c3_frames + 
                    mdc->mibs->fc_tx_cf_frames);
            break;
        case fc_class2_txoversized_frames:
            *val = mdc->mibs->fc_tx_c2_oversized_frames;
            break;
        case fc_class3_txoversized_frames:
            *val = mdc->mibs->fc_tx_c3_oversized_frames;
            break;
        case fc_classf_txoversized_frames:
            *val = mdc->mibs->fc_tx_cf_oversized_frames;
            break;
        case fc_txoversized_frames:
            *val = (mdc->mibs->fc_tx_c2_oversized_frames + mdc->mibs->fc_tx_c3_oversized_frames + 
                    mdc->mibs->fc_tx_cf_oversized_frames);
            break;
        case fc_class2_txbyt:
            *val = mdc->mibs->fc_c2_txbyt;
            break;
        case fc_class3_txbyt:
            *val = mdc->mibs->fc_c3_txbyt;
            break;
        case fc_classf_txbyt:
            *val = mdc->mibs->fc_cf_txbyt;
            break;
        default:
            BFCMAP_SAL_DBG_PRINTF("Unsupported Stat Counter Requested\n");
            return BFCMAP_E_PARAM;
    }

    return BFCMAP_E_NONE;
}

/*
 * Function:
 *      bfcmap88060_stat_clear
 * Purpose:
 *      Clear all the statistics for the specified port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */

STATIC int 
bfcmap88060_stat_clear(bfcmap_port_ctrl_t *mpc)
{
    int     rv = BFCMAP_E_NONE, xmodtxlen;
    uint32  xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8   *buftxptr;
    xmod_bfcmap_port_cmd_t x_bfcmap_port_cmd;
    bfcmap_dev_ctrl_t *mdc;
    bfcmap_port_t p;
    int unit, port;

    mdc = mpc->parent;
    p = mdc->p;
    unit = SOC_FCMAP_PORTID2UNIT(p);
    port = SOC_FCMAP_PORTID2PORT(p);

    /* Clear SDK Stats */
    if (mdc->mibs != NULL) {
        BFCMAP_SAL_MEMSET(mdc->mibs, 0, sizeof(bfcmap_mibs_t));
    }
  
    if (mdc->prev_mibs != NULL) {
        BFCMAP_SAL_MEMSET(mdc->prev_mibs, 0, sizeof(bfcmap_mibs_t));
    }
    
    /* Clear FW Stats */
    x_bfcmap_port_cmd = XMOD_BFCMAP_PORT_STAT_CLEAR;
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_bfcmap_port_cmd, sizeof(xmod_bfcmap_port_cmd_t));
    xmodtxlen = GET_XMOD_FC_CMD_IN_LEN(XMOD_BFCMAP_PORT_CMD_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_FC(XMOD_BFCMAP_PORT_CMD_SET),
                                xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

int bfcmap88060_xmod_debug_cmd(int unit, int port, int cmd)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32_t xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr, *bufrxptr;
    int xmodtxlen, rv, xmodrxlen, rsp;
    uint32_t x_cmd, x_rsp;

#ifdef BE_HOST
        x_cmd  = _shr_swap32(cmd); 
#else
        x_cmd = cmd;
#endif

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_cmd, sizeof(x_cmd));
    xmodtxlen = sizeof(x_cmd);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);
    xmodrxlen = sizeof(x_rsp);
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(unit, port, XMOD_CMD_MODE_CORE(XMOD_DEV_DEBUG_CMD), xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8_t *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_rsp, sizeof(x_rsp));

#ifdef BE_HOST
        rsp  = _shr_swap32(x_rsp); 
#else
        rsp  = x_rsp;
#endif

    /* Display Sys Link status */
    /* Overloading this cmd to convey Microsecond clock wrap around */
    if (cmd == 0x9) {
        BFCMAP_SAL_PRINTF("Port %d Sys Link: %d\n", port, rsp & 1);
        BFCMAP_SAL_PRINTF("Port %d Microsecond clock wrap around %s\n",
                       port, rsp & 2? "Yes": "No");
    }
    
    /* Display if link is admin down */
    if (cmd == 0xa) {
        BFCMAP_SAL_PRINTF("Port %d Admin Down: %d\n", port, rsp);
    }

    /* Display if PMD rx tx clk is valid */
    if (cmd == 0xb) {
        BFCMAP_SAL_PRINTF("Port %d Clk Valid: %d\n", port, rsp);
    }

    if (cmd == 0x4) /* Speed get */ {
        BFCMAP_SAL_PRINTF("Port %d Speed: %d G\n", port, (rsp&0xFFF >> 1));
    }

    return (rv ==SOC_E_NONE) ? BFCMAP_E_NONE : BFCMAP_E_FAIL;
}

/* Function to check if 40-bit FC counters have wrapped around */
void bfcmap88060_fc_stat_update(buint64_t *dst_stat_ptr, buint64_t *cur_stat_ptr, buint64_t *prev_stat_ptr, int stat_index)

{
#ifdef BE_HOST
    *cur_stat_ptr = BCMSWAP64(*cur_stat_ptr);
#endif

    /*  Skip counter wrap-around check for RX and TX bytes since they are already 64-bit */
    if ((stat_index != stat_fc_c2_rxbyt) && (stat_index != stat_fc_c3_rxbyt) && 
        (stat_index != stat_fc_cf_rxbyt) && (stat_index != stat_fc_c2_txbyt) &&
        (stat_index != stat_fc_c3_txbyt) && (stat_index != stat_fc_cf_txbyt)) {

        /* Take lower 40 bits of final value and add it to current */
        *dst_stat_ptr = (*dst_stat_ptr & (~FC_COUNTER_MASK)) | *cur_stat_ptr;
        
        /* if current < prev then counter has wrapped */
        if (*cur_stat_ptr < *prev_stat_ptr) {
            *dst_stat_ptr += (1ULL << FC_COUNTER_WIDTH);
        }
    } else {
        /* These are 64 bit values so update final directly */
        *dst_stat_ptr = *cur_stat_ptr;
    }

    /* current becomes previous */
    *prev_stat_ptr = *cur_stat_ptr;
    
    return;
}

void bfcmap88060_fc_mibs_update(bfcmap_mibs_t *mibs, bfcmap_mibs_t *prev_mibs, buint64_t *cur_fc_mibs)
{
    uint32_t *src32, *dst32;
    int idx;
    buint64_t *dst_stat_ptr;
    buint64_t *prev_stat_ptr, *cur_stat_ptr;

    dst32 = (uint32_t *)cur_fc_mibs;
    src32 = (uint32_t *)cur_fc_mibs;
    
    for (idx = 0; idx < sizeof(bfcmap_mibs_t)/4; idx++) {
        *dst32 = _shr_swap32(*src32);
        dst32++;
        src32++;
    }

    dst_stat_ptr = (buint64_t*)mibs;
    prev_stat_ptr = (buint64_t*)prev_mibs;
    cur_stat_ptr = cur_fc_mibs;
    
    for (idx = 0; idx < stat_fc_max_stat; idx++) {
        bfcmap88060_fc_stat_update(dst_stat_ptr, cur_stat_ptr, prev_stat_ptr, idx);
        dst_stat_ptr++;
        cur_stat_ptr++;
        prev_stat_ptr++;
    }

    return;
}

int bfcmap88060_fc_event_update(buint64_t *cur_fc_evt)
{
    uint32_t *src32, *dst32;
    int idx = 0, fc_evt = 0;

    dst32 = (uint32_t *)cur_fc_evt;
    src32 = (uint32_t *)cur_fc_evt;
    
    for (idx = 0; idx < 2; idx++) {
        *dst32 = _shr_swap32(*src32);
        dst32++;
        src32++;
    }

#ifdef BE_HOST
    fc_evt = BCMSWAP64(*cur_fc_evt);
#else
    fc_evt = *cur_fc_evt;
#endif
    return (fc_evt);
}

/* BFCMAP TEST */
int bfcmap_pcfg_set_get(int port)
{
    bfcmap_port_ctrl_t mpc;
    bfcmap_port_config_t pcfg;
    bfcmap_port_ability_t ability_mask;
    bfcmap_diag_code_t diag_code;
    bfcmap_lf_tr_t lf_tr;
    bfcmap_lf_rc_t lf_rc;
    bfcmap_port_speed_t spd;
    bfcmap_vlan_vsan_map_t vln;

    int rc = BFCMAP_E_NONE;

    BFCMAP_SAL_MEMSET(&mpc, 0, sizeof(mpc));
    BFCMAP_SAL_MEMSET(&pcfg, 0, sizeof(pcfg));
    BFCMAP_SAL_MEMSET(&ability_mask, 0, sizeof(ability_mask));
    BFCMAP_SAL_MEMSET(&lf_tr, 0, sizeof(lf_tr));
    BFCMAP_SAL_MEMSET(&lf_rc, 0, sizeof(lf_rc));
    BFCMAP_SAL_MEMSET(&diag_code, 0, sizeof(diag_code));
    BFCMAP_SAL_MEMSET(&spd, 0, sizeof(spd));
    BFCMAP_SAL_MEMSET(&vln, 0, sizeof(vln));
    
    mpc.mdev = 0;
    mpc.mport = port;
    mpc.parent->p = port;
    
    pcfg.action_mask |= BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK;
    pcfg.ingress_mapper_bypass = 0x1;
    pcfg.action_mask |= BFCMAP_ATTR_VLAN_TAG_MASK;
    pcfg.vlan_tag = 0x9;
    rc = bfcmap88060_port_config_selective_set(&mpc, &pcfg);

    BFCMAP_SAL_DBG_PRINTF("set rc %d\n", rc);

    BFCMAP_SAL_MEMSET(&pcfg, 0, sizeof(pcfg));
    pcfg.action_mask |= BFCMAP_ATTR_VLAN_TAG_MASK;
    pcfg.action_mask |= BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK;

    rc = bfcmap88060_port_config_selective_get(&mpc, &pcfg);

    BFCMAP_SAL_DBG_PRINTF("get rc %d, vlan tag 0x%x, in_map_bypass 0x%x \n", 
         rc, pcfg.vlan_tag, pcfg.ingress_mapper_bypass );

    rc = bfcmap88060_port_reset(&mpc);
    BFCMAP_SAL_DBG_PRINTF("reset rc %d\n", rc);

    rc = bfcmap88060_port_shutdown(&mpc);
    BFCMAP_SAL_DBG_PRINTF("shut rc %d\n", rc);

    rc = bfcmap88060_port_link_enable(&mpc);
    BFCMAP_SAL_DBG_PRINTF("enable rc %d\n", rc);

    rc = bfcmap88060_port_link_bounce(&mpc);
    BFCMAP_SAL_DBG_PRINTF("bounce rc %d\n", rc);

    ability_mask.port_ability_speed |= BFCMAP_PORT_ABILITY_SPEED_AN;
    ability_mask.port_ability_speed |= BFCMAP_PORT_ABILITY_SPEED_16GBPS;
    rc = bfcmap88060_port_ability_advert_set(&mpc, &ability_mask);
    BFCMAP_SAL_DBG_PRINTF("set abil 0x%x, rc %d\n", ability_mask.port_ability_speed, rc);

    BFCMAP_SAL_MEMSET(&ability_mask, 0, sizeof(ability_mask));
    rc = bfcmap88060_port_ability_advert_get(&mpc, &ability_mask);
    BFCMAP_SAL_DBG_PRINTF("get abil 0x%x, rc %d\n", ability_mask.port_ability_speed, rc);

    rc = bfcmap88060_port_diag_get(&mpc, &diag_code);
    BFCMAP_SAL_DBG_PRINTF("get diag 0x%x, rc %d\n", diag_code, rc);

    rc = bfcmap88060_linkfault_trigger_rc_get(&mpc, &lf_tr, &lf_rc);
    BFCMAP_SAL_DBG_PRINTF("get tr 0x%x, lf_rc0x%x, rc %d\n", lf_tr, lf_rc, rc);

    spd = BFCMAP_PORT_SPEED_AN_32GBPS;
    rc = bfcmap88060_port_speed_set(&mpc, spd);
    BFCMAP_SAL_DBG_PRINTF("spd set rc %d\n", rc);

    spd = BFCMAP_PORT_SPEED_8GBPS;
    rc = bfcmap88060_port_speed_set(&mpc, spd);
    BFCMAP_SAL_DBG_PRINTF("spd set 8g forced rc %d\n", rc);

    BFCMAP_SAL_MEMSET(&vln, 0, sizeof(vln));
    vln.vlan_vid = 0x200;
    vln.vsan_vfid = 0x2;
    rc = bfcmap88060_vlan_map_add(&mpc, &vln);
    BFCMAP_SAL_DBG_PRINTF("vln add rc %d\n", rc);
    
    BFCMAP_SAL_MEMSET(&vln, 0, sizeof(vln));
    vln.vlan_vid = 0x300;
    vln.vsan_vfid = 0x3;
    rc = bfcmap88060_vlan_map_add(&mpc, &vln);
    BFCMAP_SAL_DBG_PRINTF("vln add rc %d\n", rc);
    
    BFCMAP_SAL_MEMSET(&vln, 0, sizeof(vln));
    vln.vlan_vid = 0x400;
    vln.vsan_vfid = 0x4;
    rc = bfcmap88060_vlan_map_add(&mpc, &vln);
    BFCMAP_SAL_DBG_PRINTF("vln add rc %d\n", rc);

    BFCMAP_SAL_MEMSET(&vln, 0, sizeof(vln));
    vln.vlan_vid = 0x300;
    vln.vsan_vfid = 0x3;
    rc = bfcmap88060_vlan_map_get(&mpc, &vln);
    BFCMAP_SAL_DBG_PRINTF("vln get vid 0x%x, vsan 0x%x, rc %d\n", vln.vlan_vid, vln.vsan_vfid, rc);

    BFCMAP_SAL_MEMSET(&vln, 0, sizeof(vln));
    vln.vlan_vid = 0x200;
    vln.vsan_vfid = 0x2;
    rc = bfcmap88060_vlan_map_delete(&mpc, &vln);
    BFCMAP_SAL_DBG_PRINTF("vln del rc %d\n", rc);

    BFCMAP_SAL_MEMSET(&vln, 0, sizeof(vln));
    vln.vlan_vid = 0x200;
    vln.vsan_vfid = 0x2;
    rc = bfcmap88060_vlan_map_get(&mpc, &vln);
    BFCMAP_SAL_DBG_PRINTF("vln get vid 0x%x, vsan 0x%x, rc %d\n", vln.vlan_vid, vln.vsan_vfid, rc);

    rc = bfcmap88060_stat_clear(&mpc);
    BFCMAP_SAL_DBG_PRINTF("stat clr rc %d\n", rc);
    
    return BFCMAP_E_NONE;
}

/************************************************************
 * BFCMAP88060 API table
 */

bfcmap_drv_t bfcmap88060_a0_drv = {
    bfcmap88060_device_init,
    bfcmap88060_device_uninit,
    bfcmap88060_port_config_selective_set,
    bfcmap88060_port_config_selective_get,
    bfcmap88060_port_init,
    bfcmap88060_port_uninit,
    bfcmap88060_port_reset,
    bfcmap88060_port_shutdown,
    bfcmap88060_port_link_enable,
    bfcmap88060_port_speed_set,
    bfcmap88060_port_link_bounce,
    NULL,
    bfcmap88060_port_diag_get,
    bfcmap88060_linkfault_trigger_rc_get,
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
    NULL,
#endif /* BCM_WARM_BOOT_SUPPORT */
    NULL, 
    NULL,
    bfcmap88060_port_ability_advert_set,
    bfcmap88060_port_ability_advert_get,
    bfcmap88060_port_private_data_set,
    bfcmap88060_vlan_map_add,
    bfcmap88060_vlan_map_get,
    bfcmap88060_vlan_map_delete,
    bfcmap88060_stat_get,
    bfcmap88060_stat_clear
};

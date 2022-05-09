/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    eth_oam_msg.h
 */

#ifndef SHR_ETH_OAM_MSG_H_
#define SHR_ETH_OAM_MSG_H_

#include <soc/shared/eth_oam.h>


/*
 * ETH-OAM init message
 */
typedef struct shr_eth_oam_msg_ctrl_init_s {
    /* Flags */
    uint32 flags;

    /* Max number of OAM lmeps. */
    uint16 max_lmeps;

    /* Max number of RMEPs. */
    uint16 max_rmeps;

    /* Max number of CCM sessions. */
    uint16 max_ccm_sessions;

    /* Max number of SLM sessions. */
    uint16 max_slm_sessions;

    /* PM mode. */
    uint8 pm_mode;
} shr_eth_oam_msg_ctrl_init_t;

/*
 * Flags for lmep set message.
 */
#define SHR_ETH_OAM_MSG_CTRL_LMEP_SET_F_UPDATE (1 << 0)
#define SHR_ETH_OAM_MSG_CTRL_LMEP_SET_F_TRUNK  (1 << 1)

/*
 * Update flags for lmep set message.
 */
#define SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_STICKY_FAULTS_CLR (1 << 0)
#define SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_TX            (1 << 1)
#define SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_ALARM_MASK        (1 << 2)
#define SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_SW_RDI            (1 << 3)
#define SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT           (1 << 4)

/*
 * ETH-OAM lmep set message
 */
typedef struct shr_eth_oam_msg_ctrl_lmep_set_s {
    /* Flags */
    uint32 flags;

    /* Flags indicating what has changeed on an update. */
    uint32 update_flags;

    /* LMEP Id. */
    uint16 id;

    /* Port/Trunk ID */
    uint16 port;

    /* S-Vlan */
    uint16 svlan;

    /* C-Vlan */
    uint16 cvlan;

    /* Local MEP Id. */
    uint16 mep_id;

    /* Offset in the packet to the start of CCM PDU. */
    uint16 ccm_offset;

    /* CCM packet length. */
    uint16 ccm_pkt_length;

    /* MD level of the lmep. */
    uint8 mdl;

    /* MEP Type */
    uint8 type;

    /* Sticky faults that need to be cleared on a get. */
    uint8 sticky_faults_clear;

    /* MEG-ID */
    uint8 meg_id[SHR_ETH_OAM_MEG_ID_LENGTH];

    /* CCM packet. */
    uint8 ccm_pkt[SHR_ETH_OAM_CCM_TX_PKT_MAX_LENGTH];

    /* CCM Tx period. */
    uint8 ccm_period;

    /* Fault mask to raise events and set outgoing RDI. */
    uint8 alarm_mask;

    /* Send RDI on outgoing Tx CCM message. */
    uint8 sw_rdi;
} shr_eth_oam_msg_ctrl_lmep_set_t;

/*
 * ETH-OAM lmep get message
 */
typedef struct shr_eth_oam_msg_ctrl_lmep_get_s {
    /* Flags */
    uint32 flags;

    /* Current active faults. */
    uint8 faults;

    /* Sticky faults. */
    uint8 sticky_faults;
} shr_eth_oam_msg_ctrl_lmep_get_t;


/*
 * Flags for lmep delete message.
 */
#define SHR_ETH_OAM_MSG_CTRL_LMEP_DEL_F_TRUNK  (1 << 0)

/*
 * ETH-OAM lmep delete message
 */
typedef struct shr_eth_oam_msg_ctrl_lmep_del_s {
    /* Flags */
    uint32 flags;

    /* LMEP Id. */
    uint16 id;

    /* Port/Trunk ID */
    uint16 port;

    /* S-Vlan */
    uint16 svlan;

    /* C-Vlan */
    uint16 cvlan;

    /* MD level of the lmep. */
    uint8 mdl;

    /* MEP Type */
    uint8 type;
} shr_eth_oam_msg_ctrl_lmep_del_t;

/*
 * Flags for rmep set message.
 */
#define SHR_ETH_OAM_MSG_CTRL_RMEP_SET_F_UPDATE (1 << 0)

/*
 * Update flags for RMEP set message.
 */
#define SHR_ETH_OAM_MSG_CTRL_RMEP_SET_UPDATE_F_CCM_PERIOD        (1 << 0)
#define SHR_ETH_OAM_MSG_CTRL_RMEP_SET_UPDATE_F_STICKY_FAULTS_CLR (1 << 1)

/*
 * ETH-OAM RMEP set message
 */
typedef struct shr_eth_oam_msg_ctrl_rmep_set_s {
    /* Flags */
    uint32 flags;

    /* Flags indicating what has changed on an update. */
    uint32 update_flags;

    /* RMEP ID. */
    uint16 id;

    /* LMEP ID. */
    uint16 lmep_id;

    /* Remote MEP ID. */
    uint16 mep_id;

    /* CCM Rx period. */
    uint8 ccm_period;

    /* Sticky faults that need to be cleared on a get. */
    uint8 sticky_faults_clear;
} shr_eth_oam_msg_ctrl_rmep_set_t;

/*
 * ETH-OAM RMEP get message
 */
typedef struct shr_eth_oam_msg_ctrl_rmep_get_s {
    /* Flags */
    uint32 flags;

    /* Current active faults. */
    uint8 faults;

    /* Sticky faults. */
    uint8 sticky_faults;

    /* Src MAC of the Rx CCM. */
    uint8 src_mac[SHR_ETH_OAM_MAC_ADDR_LENGTH];

    /* Port status. */
    uint8 port_status;

    /* Interface status. */
    uint8 intf_status;
} shr_eth_oam_msg_ctrl_rmep_get_t;

/*
 * Flags for SLM set message.
 */
#define SHR_ETH_OAM_MSG_CTRL_SLM_SET_F_UPDATE          (1 << 0)
#define SHR_ETH_OAM_MSG_CTRL_SLM_SET_F_SINGLE_ENDED    (1 << 1)

/*
 * Update flags for SLM set message.
 */
#define SHR_ETH_OAM_MSG_CTRL_SLM_SET_UPDATE_F_PERIOD                (1 << 0)
#define SHR_ETH_OAM_MSG_CTRL_SLM_SET_UPDATE_F_PKT                   (1 << 1)
#define SHR_ETH_OAM_MSG_CTRL_SLM_SET_UPDATE_F_ONE_SHOT              (1 << 2)
#define SHR_ETH_OAM_MSG_CTRL_SLM_SET_UPDATE_F_MEASUREMENT_INTERVAL  (1 << 3)

/*
 * ETH-OAM SLM set mesage.
 */
typedef struct shr_eth_oam_msg_ctrl_slm_set_s {
    /* Flags */
    uint32 flags;

    /* Flags indicating what has changeed on an update. */
    uint32 update_flags;

    /* Test ID. */
    uint32 test_id;

    /* Measurement interval in milliseconds. */
    uint32 measurement_interval_msecs;

    /* ID of the SLM resource. */
    uint16 id;

    /* RMEP which is receiving the SLM packet. */
    uint16 rmep_id;

    /* Length of the SLM packet. */
    uint16 pkt_length;

    /* Offset of SLM header within the packet. */
    uint16 slm_offset;

    /* SLM packet. */
    uint8 pkt[SHR_ETH_OAM_SLM_TX_PKT_MAX_LENGTH];

    /* SLM Tx period. */
    uint8 period;

    /* Send a one-shot SLM message. */
    uint8 one_shot;
} shr_eth_oam_msg_ctrl_slm_set_t;

/*
 * ETH-OAM SLM get mesage.
 */
typedef struct shr_eth_oam_msg_ctrl_slm_get_s {
    /* Flags */
    uint32 flags;

    /* Tx near end. */
    uint32 tx_near_end;

    /* Rx far end. */
    uint32 rx_far_end;

    /* Tx far end. */
    uint32 tx_far_end;

    /* Rx near end. */
    uint32 rx_near_end;
} shr_eth_oam_msg_ctrl_slm_get_t;

/*
 * ETH-OAM Trunk ports set message.
 */
typedef struct shr_eth_oam_msg_ctrl_trunk_set_s {
    /* Flags */
    uint32 flags;

    /* Trunk ID. */
    uint16 tid;

    /* Number of ports. */
    uint16 num_ports;

    /* Ports belonging to the trunk. */
    uint16 ports[SHR_ETH_OAM_MAX_PORTS];

    /* 1 - Port add to trunk, 0 - delete. */
    uint8 add;
} shr_eth_oam_msg_ctrl_trunk_set_t;

/*
 * ETH-OAM Trunk ports get message.
 */
typedef struct shr_eth_oam_msg_ctrl_trunk_get_s {
    /* Flags */
    uint32 flags;

    /* Number of ports. */
    uint16 num_ports;

    /* Ports belonging to the trunk. */
    uint16 ports[SHR_ETH_OAM_MAX_PORTS];
} shr_eth_oam_msg_ctrl_trunk_get_t;

/*
 * ETH-OAM Event masks set message.
 */
typedef struct shr_eth_oam_msg_ctrl_event_masks_set_s {
    /* Flags */
    uint32 flags;

    /* LMEP mask. */
    uint32 lmep_mask;

    /* RMEP mask. */
    uint32 rmep_mask;
} shr_eth_oam_msg_ctrl_event_masks_set_t;

/*
 * Flags for PM slm stats get message.
 */
#define SHR_ETH_OAM_MSG_CTRL_PM_SLM_STATS_GET_F_ROLLOVER (1 << 0)

/*
 * ETH-OAM PM stats get message
 */
typedef struct shr_eth_oam_msg_ctrl_pm_pstats_slm_get_s {
    /* Flags */
    uint32 flags;

    /* Max FLR - Loss. */
    uint32 max_flr_loss;

    /* Max FLR - Tx. */
    uint32 max_flr_tx;

    /* Min FLR - Loss. */
    uint32 min_flr_loss;

    /* Min FLR - Tx. */
    uint32 min_flr_tx;

    /* Max NLR - Loss. */
    uint32 max_nlr_loss;

    /* Max NLR - Tx. */
    uint32 max_nlr_tx;

    /* Min NLR - Loss. */
    uint32 min_nlr_loss;

    /* Min NLR - Tx. */
    uint32 min_nlr_tx;

    /* Total far loss */
    uint32 total_far_loss;

    /* Total near loss */
    uint32 total_near_loss;

    /* Total number of Tx SLM packets. */
    uint32 total_tx;

    /* Total number of Rx SLR/1SL packets. */
    uint32 total_rx;

    /* Number of measurement interval elapsed. */
    uint32 num_intervals;
} shr_eth_oam_msg_ctrl_pm_pstats_slm_get_t;

/*
 * ETH-OAM PM raw stats host addr set.
 */
typedef struct shr_eth_oam_msg_ctrl_pm_rstats_addr_set_s {
    /* Flags */
    uint32 flags;

    /* Address */
    uint32 addr;

    /* Enable DMA of raw stats. */
    uint8 enable;
} shr_eth_oam_msg_ctrl_pm_rstats_addr_set_t;

/*
 * ETH-OAM messages
 *
 * Union of all ETH-OAM messages, used to size the DMA buffer.
 */
typedef union shr_eth_oam_msg_ctrl_s {
    /* Init message */
    shr_eth_oam_msg_ctrl_init_t init;

    /* LMEP set */
    shr_eth_oam_msg_ctrl_lmep_set_t lmep_set;

    /* LMEP get */
    shr_eth_oam_msg_ctrl_lmep_get_t lmep_get;

    /* LMEP delete. */
    shr_eth_oam_msg_ctrl_lmep_del_t lmep_del;

    /* RMEP set */
    shr_eth_oam_msg_ctrl_rmep_set_t rmep_set;

    /* RMEP get */
    shr_eth_oam_msg_ctrl_rmep_get_t rmep_get;

    /* SLM set. */
    shr_eth_oam_msg_ctrl_slm_set_t slm_set;

    /* SLM get. */
    shr_eth_oam_msg_ctrl_slm_set_t slm_get;

    /* Trunk ports set. */
    shr_eth_oam_msg_ctrl_trunk_set_t trunk_set;

    /* Trunk ports get. */
    shr_eth_oam_msg_ctrl_trunk_get_t trunk_get;

    /* Event masks set. */
    shr_eth_oam_msg_ctrl_event_masks_set_t event_masks_set;

    /* PM processed stats get. */
    shr_eth_oam_msg_ctrl_pm_pstats_slm_get_t pstats_slm_get;

    /* PM raw stats dma address set. */
    shr_eth_oam_msg_ctrl_pm_rstats_addr_set_t rstats_addr_set;
} shr_eth_oam_msg_ctrl_t;

#endif /* SHR_ETH_OAM_MSG_H_ */

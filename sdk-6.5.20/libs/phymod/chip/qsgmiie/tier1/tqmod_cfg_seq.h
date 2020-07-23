/*----------------------------------------------------------------------
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tqmod_cfg_seq.h
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/

#ifndef TQMOD_CFG_SEQ_H 
#define TQMOD_CFG_SEQ_H
#include "../../tsce/tier1/temod_enum_defines.h"
#include "../../tsce/tier1/temod.h"
typedef enum {
    TQMOD_TX_LANE_RESET = 0,
    TQMOD_TX_LANE_TRAFFIC_ENABLE = 1,
    TQMOD_TX_LANE_TRAFFIC_DISABLE = 2,
    TQMOD_TX_LANE_RESET_TRAFFIC_ENABLE = 3,
    TQMOD_TX_LANE_RESET_TRAFFIC_DISABLE = 4,
    TQMOD_TX_LANE_TYPE_COUNT
}txq_lane_disable_type_t;

typedef struct tqmod_an_control_s {
  temod_an_mode_type_t an_type; 
  uint16_t enable;
  an_property_enable  an_property_type;
} tqmod_an_control_t;


int tqmod_set_spd_intf(const phymod_access_t *pa, int speed);                    /* SET_SPD_INTF */
int tqmod_init_pmd_qsgmii(const phymod_access_t *pa, int pmd_active, int uc_active); /* INIT_PMD */
int tqmod_init_pcs_ilkn(const phymod_access_t *pa);                                  /* INIT_PCS */

typedef struct tqmod_an_adv_ability_s{
  temod_an_pause_t an_pause; 
  temod_cl37_sgmii_speed_t cl37_sgmii_speed;
}tqmod_an_adv_ability_t;


typedef struct tqmod_an_ability_s {
  tqmod_an_adv_ability_t cl37_adv; /*includes cl37 and cl37-bam related (fec, cl72) */
} tqmod_an_ability_t;


/*link status*/
int tqmod_get_pcs_link_status(const phymod_access_t* pa, uint32_t *link);

/* get_speed*/
int tqmod_speed_id_get(const phymod_access_t *pa, int *speed_id);

/* lane_enable*/
int tqmod_lane_enable_set(const phymod_access_t *pa, int enable);

/* set osmode*/
int tqmod_pmd_osmode_set(const phymod_access_t *pa, int os_mode);

/* set autoneg*/
int tqmod_autoneg_set(const phymod_access_t *pa, int *an_en);

/* pmd_per_lane reset*/
int tqmod_pmd_x4_reset(const phymod_access_t *pa);

int tqmod_rx_lane_control_set(const phymod_access_t *pa, int enable);
int tqmod_rx_lane_control_get(const phymod_access_t *pa, int *enable);

int tqmod_tx_lane_control(const phymod_access_t *pa, int enable);

int tqmod_tx_squelch_set(PHYMOD_ST *pc, int enable);
int tqmod_tx_squelch_get(PHYMOD_ST *pc, int *val);

int tqmod_rx_squelch_set(PHYMOD_ST *pc, int enable);
int tqmod_rx_squelch_get(PHYMOD_ST *pc, int *val);

int tqmod_tx_rx_polarity_get (const phymod_access_t *pa, uint32_t* txp, uint32_t* rxp);

int tqmod_tx_rx_polarity_set (const phymod_access_t *pa, uint32_t txp, uint32_t rxp);

int tqmod_tx_lane_control_set(const phymod_access_t *pa, txq_lane_disable_type_t tx_dis_type);
int tqmod_tx_lane_control_get(PHYMOD_ST* pa, int *reset, int *enable);


int tqmod_txfir_tx_disable_set(const phymod_access_t *pa);

int tqmod_eee_control_set(PHYMOD_ST* pc, int enable);

int tqmod_pcs_lane_swap (PHYMOD_ST* pc, int pcs_swap);

int tqmod_pcs_lane_swap_get (PHYMOD_ST* pc, uint32_t *pcs_swap);

#endif /* TQMOD_CFG_SEQ_H */ 

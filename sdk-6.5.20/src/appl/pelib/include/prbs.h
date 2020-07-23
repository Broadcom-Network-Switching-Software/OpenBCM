/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       prbs.h
 */

#ifndef _PRBS_H_
#define _PRBS_H_

#include "types.h"
#include "error.h"
#include "bcm_utils.h"


/*!
 * @enum bcm_pe_prbs_poly_e
 * @brief PRBS polynomial 
 */ 
typedef enum bcm_pe_prbs_poly_e {
    PrbsPoly7 = 0,
    PrbsPoly9,
    PrbsPoly11,
    PrbsPoly15,
    PrbsPoly23,
    PrbsPoly31,
    PrbsPoly58,
    PrbsPolyCount
} bcm_pe_prbs_poly_t;

/** PRBS Checker Mode Enum */
typedef enum srds_prbs_checker_mode_e {
  PRBS_SELF_SYNC_HYSTERESIS       = 0,
  PRBS_INITIAL_SEED_HYSTERESIS    = 1,
  PRBS_INITIAL_SEED_NO_HYSTERESIS = 2
} srds_prbs_checker_mode_t;

/*!
 * @enum bcm_pe_prbs_action_e
 * @brief PRBS action 
 */ 
typedef enum bcm_pe_prbs_action_e {
    PrbsActionSet = 0,
    PrbsActionGet,
    PrbsActionClear,
    PrbsActionCount
} bcm_pe_prbs_action_t;

/*!
 * @enum bcm_pe_prbs_dir_e
 * @brief PRBS direction 
 */ 
typedef enum bcm_pe_prbs_dir_e {
    PrbsDirTxRx = 0,
    PrbsDirRx,
    PrbsDirTx,
    PrbsDirCount
} bcm_pe_prbs_dir_t;

/*! 
 * @brief RX/TX selector. no flags means both 
 */ 
#define BCM_PE_PRBS_DIRECTION_RX 0x1 /**< Config RX */
#define BCM_PE_PRBS_DIRECTION_TX 0x2 /**< Config TX */

#define BCM_PE_PRBS_DIRECTION_RX_SET(flags) (flags |= BCM_PE_PRBS_DIRECTION_RX)
#define BCM_PE_PRBS_DIRECTION_TX_SET(flags) (flags |= BCM_PE_PRBS_DIRECTION_TX)

#define BCM_PE_PRBS_DIRECTION_RX_CLR(flags) (flags &= ~BCM_PE_PRBS_DIRECTION_RX)
#define BCM_PE_PRBS_DIRECTION_TX_CLR(flags) (flags &= ~BCM_PE_PRBS_DIRECTION_TX)

#define BCM_PE_PRBS_DIRECTION_RX_GET(flags) (flags & BCM_PE_PRBS_DIRECTION_RX ? 1 : 0)
#define BCM_PE_PRBS_DIRECTION_TX_GET(flags) (flags & BCM_PE_PRBS_DIRECTION_TX ? 1 : 0)

/*!
 * @struct bcm_pe_prbs_s
 * @brief PRBS control 
 */ 
typedef struct bcm_pe_prbs_s {
    bcm_pe_prbs_poly_t  poly;
    uint32              invert;
} bcm_pe_prbs_t;

/*!
 * @struct bcm_pe_prbs_status_s
 * @brief PRBS status 
 */ 
typedef struct bcm_pe_prbs_status_s {
    uint32 prbs_lock;       /**< Whether PRBS is currently locked */
    uint32 prbs_lock_loss;  /**< Whether PRBS was unlocked since last call */
    uint32 error_count;     /**< PRBS errors count */
} bcm_pe_prbs_status_t;



/*! 
 * bcm_pe_prbs_config_set
 *
 * @brief set/get PRBS configuration 
 *
 * @param [in]  pa              - phy access information
 * @param [in]  flags           - see BCM_PE_PRBS_DIRECTION_
 * @param [in]  prbs            - PRBS configuration to set
 */
int bcm_pe_prbs_config_set(phy_ctrl_t *pa, uint32 flags , bcm_pe_prbs_t *prbs);

/*! 
 * bcm_pe_prbs_config_get
 *
 * @brief set/get PRBS configuration 
 *
 * @param [in]  pa              - phy access information
 * @param [in]  flags           - see BCM_PE_PRBS_DIRECTION_
 * @param [out]  prbs           - PRBS configuration
 */
int bcm_pe_prbs_config_get(phy_ctrl_t *pa, uint32 flags , bcm_pe_prbs_t *prbs);

/*! 
 * bcm_pe_prbs_enable_set
 *
 * @brief Set/get PRBS enable state 
 *
 * @param [in]  pa              - phy access information
 * @param [in]  flags           - see BCM_PE_PRBS_DIRECTION_
 * @param [in]  enable          - Enable \ disable PRBS
 */
int bcm_pe_prbs_enable_set(phy_ctrl_t *pa, uint32 flags , uint32 enable);

/*! 
 * bcm_pe_prbs_enable_get
 *
 * @brief Set/get PRBS enable state 
 *
 * @param [in]  pa              - phy access information
 * @param [in]  flags           - see BCM_PE_PRBS_DIRECTION_
 * @param [out]  enable         - PRBS state
 */
int bcm_pe_prbs_enable_get(phy_ctrl_t *pa, uint32 flags , uint32 *enable);

/*! 
 * bcm_pe_prbs_status_get
 *
 * @brief Get PRBS Status 
 *
 * @param [in]  pa              - phy access information
 * @param [in]  flags           - see BCM_PE_PRBS_DIRECTION_
 * @param [out]  prbs_status    - PRBS status
 */
int bcm_pe_prbs_status_get(phy_ctrl_t *pa, uint32 flags, bcm_pe_prbs_status_t *prbs_status);

int bcm_do_prbs(phy_ctrl_t *pc, int sys_port, int action, int dir, int poly, int invert);


#endif /* _PRBS_H_ */


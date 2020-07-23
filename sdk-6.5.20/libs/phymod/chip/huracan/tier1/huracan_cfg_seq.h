/* This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : huracan_cfg_seq.h
 * Description: c functions implementing Tier1
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/
#ifndef HURACAN_CFG_SEQ_H
#define HURACAN_CFG_SEQ_H
#include <phymod/phymod_acc.h>
#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>

/*
 *  Macros
 */

#define HURACAN_GET_SW_ID(HW_ID)    (HW_ID | (0x8 <<16))
#define GET_SUPPORTED_LANES(__devid,__L) do { \
    switch(__devid) { \
        case HURACAN_82109_SW_ID: \
        case HURACAN_82181_SW_ID: \
                                  __L=EIGHT; \
        break; \
        default: \
                 __L=FOUR; \
    }\
}while(0)                                 

#define MAX_NUM_PACKAGES        3
#define HURACAN_82181_HW_ID     0x2181
#define HURACAN_82109_HW_ID     0x2109
#define HURACAN_82109_SW_ID     HURACAN_GET_SW_ID(HURACAN_82109_HW_ID)
#define HURACAN_82181_SW_ID     HURACAN_GET_SW_ID(HURACAN_82181_HW_ID)
#define LANE_NUM_MASK           0x3   /* four lanes per quad*/
#define CORE_ID_POS_SHIFT       12
#define CORE_ID_MASK            0x3   /* four quads*/
#define LANE_NUM_POS_SHIFT      8
#define LANE_REG_ADDR_MASK      0xf    

enum {
    ZERO,
    ONE,
    TWO,
    FOUR=4,
    EIGHT=8
};
#define HURACAN_GET_IF_SIDE(_FLAGS, _SIDE) \
{                                                      \
    if(((_FLAGS >> PHYMOD_INTERFACE_SIDE_SHIFT) & 0x1) == 0x1) {   \
        _SIDE = SIDE_B;                                 \
    } else {                                            \
        _SIDE = SIDE_A;                                 \
    }                                                   \
}
#define HURACAN_IF_TX_SIDE(chip_id, pkg_side)                     \
(  ((HURACAN_IS_SIMPLEX(chip_id)) && (pkg_side == SIDE_A)) ||     \
   (HURACAN_IS_DUPLEX(chip_id))                                   \
)


#define HURACAN_IF_RX_SIDE(chip_id, pkg_side)                     \
(  (HURACAN_IS_SIMPLEX(chip_id) && (pkg_side == SIDE_B)) ||       \
   (HURACAN_IS_DUPLEX(chip_id))                                   \
) 
#define HURACAN_IS_SIMPLEX(devid)\
((devid) == HURACAN_82109_SW_ID)

#define HURACAN_IS_DUPLEX(devid)\
((devid) == HURACAN_82181_SW_ID)


typedef enum {
    LINE=0, 
    SYS=1,
    MAX_SIDES
} CHIP_SYSEN_E;

enum {
    TX,
    RX
};

/* Huracan Transmiter/Receiver Types. */
typedef enum {
  DUPLEX_XCVR,  /* Duplex transceiver */
  SIMPLEX_TX,   /* Simplex transmitter */
  SIMPLEX_RX    /* Simplex Receiver */
} HURACAN_XCVR_TYPE_E;

/* Huracan Physical Lane Descriptor
 *  Port configuration set by package.
 */
typedef struct tx_rx_lane_info_t {
    uint16_t die_num;   /* on which die*/
    uint16_t lane_num;  /* physical lane number*/
} tx_rx_lane_info;

typedef struct lane_info_t {
    tx_rx_lane_info tx_rx_l_info[MAX_SIDES]; /*0->tx,1->rx*/
} lane_info;

typedef struct _HURACAN_PKG_LANE_CFG_S {
    uint32_t chip_id;              /* Chip id lane belongs to */    
    uint16_t pkg_lane_num;         /* Package lane number. */ 
    HURACAN_XCVR_TYPE_E xcvr_type;
    lane_info intf_l_info[MAX_SIDES];      /* 0->line,1->system*/
} HURACAN_PKG_LANE_CFG_t;
typedef struct chip_id_quad_info_t {
    uint32_t chip_id;
    uint32_t num_of_quads;
    uint8_t quads_in_this_chip[8];
} chip_id_quad_info;
/** Huracan Package Sides Type.
 * Package side is defined as:
 * For Duplex Chips:
 *   Side A = Line Side
 *   Side B = System Side
 * For Simplex Chips:
 *   Side A = TX Side
 *   Side B = RX Side
*/
typedef enum {
    SIDE_A   = 0,
    SIDE_B   = 1,
    ALL_SIDES = 10
} HURACAN_PKG_SIDE_E;
extern const chip_id_quad_info chipid_quad_info[MAX_NUM_PACKAGES-1]; 
extern const HURACAN_PKG_LANE_CFG_t* const glb_pkg_array[MAX_NUM_PACKAGES]; 

/*
 *  Functions
 */


/**   Get Chip ID 
 *    This function retrieves Chip ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @param rev_id             Pointer to Revision ID retrieved from the chip
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int huracan_rev_id(const phymod_access_t *pa, uint32_t *rev_id);

/**   Get Chip ID 
 *    This function retrieves Chip ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return chip_id           Chip id retrieved from the chip
 */
uint32_t _huracan_get_chip_id(const phymod_access_t *pa);

/**   Get  link status of PHY 
 *    This function is used to retrieve the link status of PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @param link_status        link status of the PHY 
 *                              1 - Up 
 *                              0 - Down 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int huracan_link_status(const phymod_access_t *pa, uint32_t *link_status);

int huracan_reg_read(const phymod_access_t *pa, uint32_t addr, uint32_t  *val);

int huracan_reg_write(const phymod_access_t *pa, uint32_t addr, uint32_t  val);



int huracan_tx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_tx_lane_control_t tx_control);

int huracan_tx_squelch_get(const phymod_access_t *pa,
                               int *tx_squelch);

int huracan_rx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_rx_lane_control_t rx_control);


int huracan_rx_squelch_get(const phymod_access_t *pa, int *rx_squelch);

int _huracan_phy_status_dump(const phymod_access_t *pa);


int huracan_hard_reset(const phymod_access_t* pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction);

int _huracan_phy_power_set(const phymod_access_t* pa, const phymod_phy_power_t* power);

int _huracan_phy_power_get(const phymod_access_t* pa, phymod_phy_power_t* power);

int huracan_gpio_config_set(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t gpio_mode);

int huracan_gpio_config_get(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t* gpio_mode);

int huracan_gpio_pin_value_set(const phymod_access_t *pa, int pin_no, int value);

int huracan_gpio_pin_value_get(const phymod_access_t *pa, int pin_no, int* value);


#endif /* HURACAN_CFG_SEQ_H */

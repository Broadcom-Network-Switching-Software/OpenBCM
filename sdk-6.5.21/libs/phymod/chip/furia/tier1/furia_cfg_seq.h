/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.  
 */
#ifndef __FURIA_CFG_SEQ_H__
#define __FURIA_CFG_SEQ_H__

/* 
 * Includes
 */
#include "furia_pkg_cfg.h"
#include "furia_types.h"

#include "furia_micro_seq.h"
#include "furia_regs_structs.h"
#include "common/srds_api_enum.h"
#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
/*
 *  Functions for Furia                            
 */

/*
 *  Defines
 */

/* 
 *  Types
 */

/*
 *  Macros
 */
#define GET_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define FURIA_IS_SIMPLEX(devid)\
((devid) == FURIA_ID_82208 ||\
 (devid) == FURIA_ID_82209 ||\
 (devid) == FURIA_ID_82212 ||\
 (devid) == FURIA_ID_82216   \
)

#define FURIA_IS_DUPLEX(devid)\
((devid) == FURIA_ID_82071 ||\
 (devid) == FURIA_ID_82070 ||\
 (devid) == FURIA_ID_82073 ||\
 (devid) == FURIA_ID_82072 ||\
 (devid) == FURIA_ID_82380 ||\
 (devid) == FURIA_ID_82381 ||\
 (devid) == FURIA_ID_82383 ||\
 (devid) == FURIA_ID_82385 ||\
 (devid) == FURIA_ID_82314 ||\
 (devid) == FURIA_ID_82315   \
)


#define FURIA_IS_DUAL_DIE_DUPLEX(devid)\
((devid) == FURIA_ID_82073 ||\
 (devid) == FURIA_ID_82072 ||\
 (devid) == FURIA_ID_82381 ||\
 (devid) == FURIA_ID_82383 ||\
 (devid) == FURIA_ID_82385   \
)
#define FURIA_GET_IF_SIDE(_FLAGS, _SIDE) \
{                                                      \
    if(((_FLAGS >> PHYMOD_INTERFACE_SIDE_SHIFT) & 0x1) == 0x1) {   \
        _SIDE = SIDE_B;                                 \
    } else {                                            \
        _SIDE = SIDE_A;                                 \
    }                                                   \
}
 
#define FURIA_IF_BCAST_SET(chip_id, lane_map)            \
( ((FURIA_IS_DUPLEX(chip_id)) && (lane_map == 0xF))   \
)   


#define FURIA_IF_TX_SIDE(chip_id, pkg_side)                     \
(  ((FURIA_IS_SIMPLEX(chip_id)) && (pkg_side == SIDE_A)) ||     \
   (FURIA_IS_DUPLEX(chip_id))                                   \
)


#define FURIA_IF_RX_SIDE(chip_id, pkg_side)                     \
(  (FURIA_IS_SIMPLEX(chip_id) && (pkg_side == SIDE_B)) ||       \
   (FURIA_IS_DUPLEX(chip_id))                                   \
)                               

#define FURIA_MAX_INTRS_SUPPORT 8

#define FURIA_PHYTYPE_BP                       0
#define FURIA_PHYTYPE_FP                       1
#define FURIA_PHYTYPE_OTN                      2
#define FURIA_PHYTYPE_FC                       3
#define FURIA_PHYTYPE_PROP                     4

#define FURIA_LINKTYPE_1                       1
#define FURIA_LINKTYPE_2                       2
#define FURIA_LINKTYPE_3                       3
#define FURIA_LINKTYPE_4                       4
#define FURIA_LINKTYPE_5                       5
#define FURIA_LINKTYPE_6                       6
#define FURIA_LINKTYPE_7                       7
#define FURIA_LINKTYPE_8                       8
#define FURIA_LINKTYPE_9                       9
#define FURIA_LINKTYPE_C                       12
#define FURIA_LINKTYPE_D                       13
#define FURIA_LINKTYPE_E                       14
#define FURIA_LINKTYPE_F                       15
#define FURIA_IF_LR_LR4                        0
#define FURIA_IF_SR_SR4                        1
#define FURIA_IF_ER_ER4                        2
#define FURIA_IF_KR_KR4                        3
#define FURIA_IF_CR4                           3
#define FURIA_IF_CR2                           2
#define FURIA_IF_CX                            1
#define FURIA_IF_CR                            2
#define FURIA_IF_SFI                           3
#define FURIA_IF_CAUI                          0
#define FURIA_IF_CC                            1
#define FURIA_IF_CM                            2
#define FURIA_IF_CAUI4                         3
#define FURIA_IF_XFI                           1
#define FURIA_IF_XLAUI                         2
#define FURIA_IF_1E                           0
#define FURIA_IF_2E                            1
#define FURIA_IF_3E1                          0
#define FURIA_IF_3E2                           1
#define FURIA_CLK_106                          0
#define FURIA_CLK_156                 1
#define FURIA_CLK_157                 2
#define FURIA_CLK_158                 3
#define FURIA_CLK_161                 4
#define FURIA_CLK_168                 5
#define FURIA_CLK_174                 6
#define FURIA_CLK_125                 7
#define FURIA_CLK_159                 8
#define FURIA_IF_HIGIG                1

#define FURIA_REG_SET(reg,lane,val)    \
    if (lane == 0) { reg##0=val; }     \
    else if (lane == 1) { reg##1=val;} \
    else if (lane == 2) { reg##2=val;} \
    else if (lane == 3) { reg##3=val;} \
    else if (lane == 4) { reg##4=val;} \
    else if (lane == 5) { reg##5=val;} \
    else if (lane == 6) { reg##6=val;} \
    else if (lane == 7) { reg##7=val;} 

#define FURIA_REG_GET(reg,lane,val)      \
    if (lane == 0) { val = reg##0; }     \
    else if (lane == 1) { val = reg##1;} \
    else if (lane == 2) { val = reg##2;} \
    else if (lane == 3) { val = reg##3;} \
    else if (lane == 4) { val = reg##4;} \
    else if (lane == 5) { val = reg##5;} \
    else if (lane == 6) { val = reg##6;} \
    else if (lane == 7) { val = reg##7;} 

#define FURIA_GET_NUM_LANES(cid, nl)       \
    if (FURIA_IS_SIMPLEX(cid)) {           \
        if (cid == FURIA_ID_82212) {       \
            nl=12;}                        \
        else {                             \
            nl=8; }                        \
    } else {                               \
        nl = 4;}

#define FURIA_GET_LANE_MASK(cid, lm)       \
    if (FURIA_IS_SIMPLEX(cid)) {           \
        if (cid == FURIA_ID_82212) {       \
            lm=0xFFF;}                     \
        else {                             \
            lm=0xFF; }                     \
    } else {                               \
        lm= 0xF;}
/*
 *  Global Variables
 */

/*
 *  Functions
 */

/*
 *  Functions for Manipulating Chip/Port Cfg Descriptors
 */

/*
 * Functions
 */

/*
 *  Functions for Manipulating Chip/Port Cfg Descriptors
 */

/**   Get Revision ID 
 *    This function retrieves Revision ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return rev_id            Revision ID retrieved from the chip
 */
uint16_t _furia_get_rev_id(const phymod_access_t *pa);

/**   Get Chip ID 
 *    This function retrieves Chip ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return chip_id           Chip id retrieved from the chip
 */
uint32_t _furia_get_chip_id(const phymod_access_t *pa);

/**   Reset Chip 
 *    This function is used to reset entire chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_chip_reset(const phymod_access_t *pa);	
	
/**   Reset Register 
 *    This function is used to perform register reset 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_register_rst(const phymod_access_t *pa);


/**   Configure PRBS generator 
 *    This function is used to configure PRBS generator with user provided
 *    polynomial and invert data information 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param prbs_mode          User specified polynomial
 *                              0 - PRBS7
 *                              1 - PRBS9
 *                              2 - PRBS11    
 *                              3 - PRBS15
 *                              4 - PRBS23
 *                              5 - PRBS31
 *                              6 - PRBS58
 * 
 *    @param prbs_inv           User specified invert data config
 *                              0 - do not invert PRBS data  
 *                              1 - invert PRBS data  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_config_set(const phymod_access_t *pa,
                        uint32_t flags,
                        enum srds_prbs_polynomial_enum prbs_mode,
                        uint32_t prbs_inv);


/**   Enable PRBS generator and PRBS checker 
 *    This function is used to enable or disable PRBS generator and PRBS checker
 *    as requested by the user  
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param enable             Enable or disable as specified by the user
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_enable_set(const phymod_access_t *pa, uint32_t flags, uint32_t enable);

/**   Get Enable status of PRBS generator and PRBS checker 
 *    This function is used to retrieve the enable status of PRBS generator and
 *    PRBS checker
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param *enable            Enable or disable read from chip 
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_enable_get(const phymod_access_t *pa, uint32_t flags, uint32_t *enable);

/**   Get PRBS checker configuration 
 *    This function is used to retrieve PRBS checker configuration from 
 *    the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags to prbs config 
 *    @param prbs_mode          Configured Polynomial retrieved from chip 
 *    @param prbs_inv           Configured invert data retrieved from chip
 *                              0 - do not invert PRBS data  
 *                              1 - invert PRBS data  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_config_get(const phymod_access_t *pa,
                         uint32_t flags, 
                         enum srds_prbs_polynomial_enum *prbs_mode,
                         uint32_t *prbs_inv);
 

/**   Get PRBS lock and error status 
 *    This function is used to retrieve PRBS lock, loss of lock and error counts
 *    from the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param lock_status        PRBS lock status denotes PRBS generator and 
 *                              checker are locked to same polynomial data
 *    @param lock_loss          Loss of lock denotes PRBS generator and checker
 *                              are not in sync   
 *    @param error_count        PRBS error count retrieved from chip
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_status_get(const phymod_access_t *pa,
                                uint32_t *lock_status,
                                uint32_t *lock_loss,
                                uint32_t *error_count);

/**   Get lane descriptor 
 *    This function is used to retrieve lane descriptor from package lane 
 *
 *    @param chip_id         chip id number 
 *    @param pa             Pointer to phymod access structure 
 *    @param pkg_lane       Package lane number 
 *
 *    @return pkg_ln_des    Lane descriptor structure contains the info
 *                              about package lane and die lane mapping 
 */
const struct _FURIA_PKG_LANE_CFG_S* _furia_pkg_ln_des(uint32_t chip_id, const phymod_access_t *pa, int pkg_lane);

/**   Get lane descriptor 
 *    This function is used to retrieve lane descriptor from package lane 
 *
 *    @param chip_id         chip id number 
 *    @param pa             Pointer to phymod access structure 
 *    @param pkg_lane       Package lane number
 *    @param tx_rx          Tx or Rx
 *
 *    @return pkg_ln_des    Lane descriptor structure contains the info
 *                              about package lane and die lane mapping 
 */
const struct _FURIA_PKG_LANE_CFG_S* _furia_pkg_ln_des_lane_swap(uint32_t chip_id, const phymod_access_t *pa, int pkg_lane, int tx_rx);

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
int furia_chk_phy_link_mode_status(const phymod_access_t *pa,
                                   uint32_t *link_status); 

/**   Set config mode 
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param config             Pointer to phy interface config structure
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_set_config_mode(const phymod_access_t *pa,
                          const phymod_phy_inf_config_t* config);


/**   Get config mode 
 *    This function is used to retrieve the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param intf               Interface type 
 *    @param speed              Speed val retrieved from PHY 
 *    @param ref_clk            Reference clock frequency to set 
 *                              the PHY into specified interface 
 *                              and speed
 *    @param interface_modes    mode which is currently unused
 *    @param config             used to return the OTN type i.e OTN4.4, OTN3.4 etc
 *    @param saved_lane_map           Lane map provided by user
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_get_config_mode(const phymod_access_t *pa,
                       phymod_interface_t *intf,
                       uint32_t *speed,
                       phymod_ref_clk_t *ref_clk,
                       uint32_t *interface_modes, phymod_phy_inf_config_t* config, uint16_t saved_lane_map);

/**   Get phymod interface type from pma type 
 *    This function is used to retrieve PHYMOD interface type from pma_type
 *
 *    @param pma_type           pma type read from mode register 
 *    @param intf               PHYMOD interface type 
 *    @param speed              Speed val 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_get_phymod_interface_type(uint16_t pma_type,
                                     phymod_interface_t *intf,
                                     uint32_t *speed);

/**   Get pma type from interface and speed 
 *    This function is used to get the pma type from interface and speed. 
 *    This pma type will be programmed to mode register for different PHY modes 
 *
 *    @param intf               PHYMOD interface type 
 *    @param speed              Speed val 
 * 
 *    @return pma_typeselection pma type calculated with interface and speed 
 */
uint16_t _furia_get_pma_type_selection(phymod_interface_t intf, uint32_t speed);


int furia_tx_rx_polarity_set (const phymod_access_t *pa,
                              uint32_t tx_polarity,
                              uint32_t rx_polarity);


int furia_tx_rx_polarity_get (const phymod_access_t *pa,
                              uint32_t *tx_polarity,
                              uint32_t *rx_polarity);
int furia_tx_squelch(const phymod_access_t *pa,
                     uint32_t enable); 
int furia_tx_disable(const phymod_access_t *pa,
                      uint32_t enable); 
int furia_pmd_lock_get(const phymod_access_t *pa,
                       uint32_t *rx_seq_done); 
int furia_enable_set(const phymod_access_t *pa,
                     uint32_t enable); 
int furia_rev_id(const phymod_access_t *pa,
                 uint32_t *rev_id); 
int furia_rx_power_down(const phymod_access_t *pa,
                        uint32_t enable); 
int furia_tx_power_down(const phymod_access_t *pa,
                         uint32_t enable); 
int furia_tx_rx_power_get(const phymod_access_t *pa,
                          uint32_t *power_tx,
                          uint32_t *power_rx);
int furia_link_status(const phymod_access_t *pa,
                      uint32_t *link_status);
int furia_tx_rx_power_set(const phymod_access_t *pa,
                          uint8_t tx_rx,
                          uint32_t enable);
int furia_loopback_set(const phymod_access_t *pa,
                       phymod_loopback_mode_t loopback,
                       uint32_t enable);
int furia_loopback_get(const phymod_access_t *pa,
                       phymod_loopback_mode_t loopback,
                       uint32_t* enable);

int furia_reset_set(const phymod_access_t *pa,
                    phymod_reset_mode_t reset_mode,
                    phymod_reset_direction_t direction);

int furia_tx_set(const phymod_access_t *pa,
                 const phymod_tx_t* tx);

int furia_tx_get(const phymod_access_t *pa,
                 phymod_tx_t* tx);
int furia_rx_set(const phymod_access_t *pa,
                 const phymod_rx_t* rx);
int furia_rx_get(const phymod_access_t *pa,
                 phymod_rx_t* rx);

int furia_tx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_tx_lane_control_t tx_control);

int furia_tx_lane_control_get(const phymod_access_t *pa,
                              phymod_phy_tx_lane_control_t* tx_control);

int furia_rx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_rx_lane_control_t rx_control);  

int furia_rx_lane_control_get(const phymod_access_t *pa,
                              phymod_phy_rx_lane_control_t* rx_control);
int furia_lane_cross_switch_map_set(const phymod_access_t* phy,
                                    const uint32_t* tx_source_array);
int furia_lane_cross_switch_map_get(const phymod_access_t *pa,
                                    uint32_t* tx_source_array);
int furia_force_tx_training_set(const phymod_access_t *pa,
                                uint32_t enable);
int furia_force_tx_training_get(const phymod_access_t *pa,
                                uint32_t *enable);
int furia_force_tx_training_status_get(const phymod_access_t *pa, phymod_cl72_status_t* status);

int _furia_phy_reset_set(const phymod_access_t *pa,
                         const phymod_phy_reset_t* reset);
int _furia_phy_reset_get(const phymod_access_t *pa,
                         phymod_phy_reset_t* reset);
int furia_ber_proj (const phymod_access_t *pa, const phymod_phy_eyescan_options_t* eyescan_options);
int furia_rx_adaptation_resume(const phymod_access_t *pa);
int furia_slice_reg_set(const phymod_access_t *pa);
int furia_slice_reg_unset(const phymod_access_t *pa);
#define FURIA_BIT(x) (1<<x)
#define SLICE_REG_SET_BIT       31
#define FURIA_DEVICE_ID_MASK    0x3F
#define FURIA_PMD_DEVICE_ID     1
#define FURIA_AN_DEVICE_ID      7

typedef enum {
    FURIA_CL73_NO_TECH = 0,
    FURIA_CL73_1000BASE_KX = 0x1,
    FURIA_CL73_10GBASE_KX4 = 0x2,
    FURIA_CL73_10GBASE_KR = 0x4,
    FURIA_CL73_40GBASE_KR4 = 0x8,
    FURIA_CL73_40GBASE_CR4 = 0x10,
    FURIA_CL73_100GBASE_CR10 = 0x20,
    FURIA_CL73_100GBASE_KP4 = 0x40,
    FURIA_CL73_100GBASE_KR4 = 0x80,
    FURIA_CL73_100GBASE_CR4 = 0x100
}furia_cl73_speed_t;

typedef enum {
    FURIA_NO_PAUSE = 0,  
    FURIA_ASYM_PAUSE,
    FURIA_SYMM_PAUSE,
    FURIA_ASYM_SYMM_PAUSE,  
    FURIA_AN_PAUSE_COUNT
}furia_an_pause_t;

typedef struct FURIA_an_adv_ability_s{
  furia_cl73_speed_t an_base_speed; 
  furia_an_pause_t an_pause; 
  uint16_t an_fec; 
  uint16_t an_hg2;
  uint16_t an_cl72; 
}furia_an_adv_ability_t;

typedef struct FURIA_an_ability_s {
  furia_an_adv_ability_t cl73_adv; /*includes cl73 related */
  uint32_t an_master_lane;
} furia_an_ability_t;

int _furia_autoneg_set(const phymod_access_t* pa, 
                       const phymod_autoneg_control_t* an);
int _furia_autoneg_get(const phymod_access_t* pa, 
                       phymod_autoneg_control_t* an, 
                       uint32_t *an_done);
int _furia_autoneg_ability_get (const phymod_access_t* pa, 
                                furia_an_ability_t *an_ablity);
int _furia_autoneg_ability_set (const phymod_access_t* pa, 
                                furia_an_ability_t *an_ablity);
int _furia_config_pll_div(const phymod_access_t *pa, uint16_t pll_mode);

int furia_display_eye_scan(const phymod_access_t *pa);
int furia_pll_sequencer_restart(const phymod_access_t *pa, phymod_sequencer_operation_t operation);
int furia_fec_enable_set(const phymod_access_t *pa, uint32_t enable);
int furia_fec_enable_get(const phymod_access_t *pa, uint32_t* enable);
int _furia_phy_status_dump(const phymod_access_t *pa);
int _furia_core_diagnostics_get(const phymod_access_t *pa, phymod_core_diagnostics_t* diag);
int _furia_phy_diagnostics_get(const phymod_access_t *pa, phymod_phy_diagnostics_t* diag);
int furia_pmd_info_dump(const phymod_access_t *pa, char* type);
int furia_pcs_info_dump(const phymod_access_t *pa, char* type); 
int furia_ext_intr_status_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* intr_status);
int furia_ext_intr_enable_set(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t enable); 
int furia_ext_intr_enable_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* enable); 
int furia_ext_intr_status_clear(const phymod_phy_access_t *phy, uint32_t intr_type);
int furia_fc_pcs_chkr_enable_set(const phymod_access_t *pa, uint32_t fcpcs_chkr_mode, uint32_t enable);
int furia_fc_pcs_chkr_enable_get(const phymod_access_t *pa, uint32_t fcpcs_chkr_mode, uint32_t* enable);
int furia_fc_pcs_chkr_status_get(const phymod_access_t *pa, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count);
int _furia_core_rptr_rtmr_mode_set(const phymod_access_t* pa,
uint32_t rptr_rtmr_mode);
int _furia_core_sys_interface_set(const phymod_core_access_t* core, uint32_t sys_side_intf);
int _furia_autoneg_remote_ability_get (const phymod_access_t* pa, phymod_autoneg_ability_t* an_ability_get);

int _furia_core_avddtxdrv_config_set(const phymod_core_access_t* core, uint32_t avdd_txdrv);
int furia_gpio_config_set(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t gpio_mode);
int furia_gpio_config_get(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t* gpio_mode);
int furia_gpio_pin_value_set(const phymod_access_t *pa, int pin_no, int value);
int furia_gpio_pin_value_get(const phymod_access_t *pa, int pin_no, int* value);
int furia_module_write(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr, uint32_t no_of_bytes, const uint8_t *write_data);
int furia_module_read(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr, uint32_t no_of_bytes, uint8_t *read_data);
int _furia_cfg_fw_ull_dp(const phymod_core_access_t* core, uint32_t ull_dp_enable);
int _furia_cfg_an_master_lane(const phymod_core_access_t* core, uint32_t
an_master_lane); 
int _furia_pcs_monitor_enable_set(const phymod_core_access_t* core, int enable);
int _furia_core_cfg_hcd_link_status_criteria(const phymod_access_t *pa, uint32_t hcd_link_criteria);
int _furia_core_cfg_pfifo_4_max_ieee_input_skew(const phymod_access_t* pa, uint32_t tx_ptr_sync, uint32_t rx_ptr_sync);
int _furia_core_cfg_polarity_set(const phymod_access_t *pa, uint32_t rx_polarity, uint32_t tx_polarity); 
int _furia_core_cfg_tx_set(const phymod_access_t *pa,const phymod_tx_t* tx);
void _furia_get_phy_id_idx(uint32_t phy_id, uint32_t *idx, uint32_t *exist_phy);
int furia_firmware_core_config_set(const phymod_access_t* pa, phymod_firmware_core_config_t fw_config);
int furia_firmware_core_config_get(const phymod_access_t* pa, phymod_firmware_core_config_t* fw_config);
int furia_firmware_lane_config_set(const phymod_access_t* pa, phymod_firmware_lane_config_t fw_config);
int furia_firmware_lane_config_get(const phymod_access_t* pa, phymod_firmware_lane_config_t* fw_config);
int _furia_get_pll_for_serdes(phymod_ref_clk_t ref_clk, uint32_t speed, FALCON_PLL_MODE_E pll_mode, uint16_t *serdes_pll_mode);
int _furia_phy_short_channel_mode_set(const phymod_access_t *pa, uint32_t enable);
int _furia_phy_short_channel_mode_get(const phymod_access_t *pa, uint32_t *enable, uint32_t *status);
int furia_single_pmd_enable(const phymod_access_t *pa);
#endif

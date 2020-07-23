/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

/* 
 * Includes
 */
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
/* Implementation of the interface defined here */
#include "falcon_furia_src/falcon_furia_internal.h"
#include "falcon_furia_src/falcon_furia_functions.h"
#include "falcon_furia_src/falcon_furia_enum.h"
#include "falcon_furia_src/public/falcon_furia_fields_public.h"
#include "furia_cfg_seq.h"
#include "furia_reg_access.h"
#include "furia_address_defines.h"
/*
 *  Defines
 */
#define PHYMOD_INTERFACE_SIDE_SHIFT        31

/* 
 *  Types
 */

/*
 *  Macros
 */
/*
 *  Global Variables
 */

/*
 *  Functions
 */
int _furia_get_ref_clock_freq_in_mhz(FURIA_REF_CLK_E ref_clk_freq);
STATIC uint16_t _furia_get_link_type(phymod_interface_t intf, uint32_t speed, phymod_otn_type_t otn_type);
STATIC uint16_t _furia_get_phy_type(phymod_interface_t intf, uint32_t speed);
STATIC int _furia_config_clk_scaler_val(const phymod_access_t *pa, FURIA_REF_CLK_E ref_clk_freq);
STATIC int _furia_phymod_to_furia_type_ref_clk_freq(phymod_ref_clk_t ref_clk, FURIA_REF_CLK_E *ref_clk_freq);
STATIC int _furia_get_pll_mode(phymod_ref_clk_t ref_clk, uint32_t speed, uint32_t higig, FALCON_PLL_MODE_E *pll_mode);
STATIC int _furia_get_pll_mode_otn(phymod_ref_clk_t ref_clk, uint32_t otn_type, FALCON_PLL_MODE_E *pll_mode);
STATIC int furia_get_pkg_idx(uint32_t chip_id, int *pkg_idx);
STATIC int _furia_config_clk_scaler_without_m0_reset(const phymod_access_t *pa, FURIA_REF_CLK_E ref_clk_freq);
STATIC int _furia_get_pkg_lane(uint32_t phy_id, uint32_t chip_id, uint32_t die_lane, uint32_t lane_index, uint32_t *pkg_lane);
int _furia_fw_enable(const phymod_access_t* pa);
STATIC int _furia_cfg_an_master_lane_get(const phymod_access_t *pa, uint32_t *an_master_lane) ;
STATIC int _furia_set_intf_type(const phymod_access_t *pa, uint16_t phy_type, uint16_t link_type, 
                                uint32_t phymod_interface_type, int lane_index, int otn_type, uint16_t ref_clock, uint32_t higig);
STATIC int _furia_get_intf_type(const phymod_access_t *pa, uint16_t phy_type, uint16_t link_type, 
        int lane_index, uint16_t *saved_intf, uint16_t *saved_intf1e2e, uint16_t *ref_clk, uint32_t *higig) ;
STATIC int _furia_set_module_command(const phymod_access_t *pa, uint16_t xfer_addr,
                               uint32_t slv_addr, unsigned char xfer_cnt, FURIA_I2CM_CMD_E cmd); 

/*
 * Functions for Manipulating Chip/Port Cfg Descriptors
 */

/*
 * Functions for Programming the Chip
 */

/**   Get Revision ID 
 *    This function retrieves Revision ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @param rev_id            Revision ID retrieved from the chip
 */
int furia_rev_id(const phymod_access_t *pa, uint32_t *rev_id) 
{
    FUR_MISC_CTRL_CHIP_REVISION_t chip_revision;
    PHYMOD_MEMSET(&chip_revision, 0 , sizeof(FUR_MISC_CTRL_CHIP_REVISION_t));

    /* Read the chip revision from register */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_chip_revision_Adr,\
                                &chip_revision.data)); 
    *rev_id = chip_revision.fields.chip_rev;
    return PHYMOD_E_NONE;
}

/**   Get Chip ID 
 *    This function retrieves Chip ID from PHY chip
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return chip_id           Chip id retrieved from the chip
 */
uint32_t _furia_get_chip_id(const phymod_access_t *pa) 
{
    uint32_t chip_id = 0;
    FUR_MISC_CTRL_CHIP_REVISION_t chip_revision;
    FUR_MISC_CTRL_CHIP_ID_t chip_id_t;
    PHYMOD_MEMSET(&chip_revision, 0 , sizeof(FUR_MISC_CTRL_CHIP_REVISION_t));
    PHYMOD_MEMSET(&chip_id_t, 0 , sizeof(FUR_MISC_CTRL_CHIP_ID_t));
    
    /* Read upper bits of chip id */ 
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_chip_revision_Adr,\
                                &chip_revision.data)); 
    chip_id = chip_revision.fields.chip_id_19_16;
    /* Read lower 16 bits of chip id */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_chip_id_Adr,\
                                &chip_id_t.data)); 
    chip_id = ( (chip_id << 16) | 
                 (chip_id_t.fields.chip_id_15_0));
    return(chip_id);
}


/**   Reset Chip 
 *    This function is used to reset entire chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_chip_reset (const phymod_access_t *pa) 
{
    FUR_GEN_CNTRLS_GEN_CONTROL1_t gen_cntrl1;
    PHYMOD_MEMSET(&gen_cntrl1, 0 , sizeof(FUR_GEN_CNTRLS_GEN_CONTROL1_t));
   
    /* Read General control1 Reg */ 
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
         FUR_GEN_CNTRLS_gen_control1_Adr,\
         &gen_cntrl1.data));

    /* update bit field for hard reset */
    gen_cntrl1.fields.resetb = 0;

    /* Write to General control1 Reg */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
         FUR_GEN_CNTRLS_gen_control1_Adr,\
         gen_cntrl1.data));
    return PHYMOD_E_NONE;
}    
    
/**   Reset Register 
 *    This function is used to perform register reset 
 *
 *    @param pa                 Pointer to phymod access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_register_rst (const phymod_access_t *pa) 
{
    FUR_GEN_CNTRLS_GEN_CONTROL1_t gen_cntrl1;
    PHYMOD_MEMSET(&gen_cntrl1, 0 , sizeof(FUR_GEN_CNTRLS_GEN_CONTROL1_t));

    /* Read General control1 Reg */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control1_Adr,\
                                &gen_cntrl1.data));

    /* Update bit field for register reset */ 
    gen_cntrl1.fields.reg_rstb = 0;

    /* Write to General control1 Reg */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control1_Adr,\
                                 gen_cntrl1.data));
    return PHYMOD_E_NONE;
}

/**   Configure PRBS generator 
 *    This function is used to configure PRBS generator with user provided
 *    polynomial and invert data information 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
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
                        uint32_t prbs_inv)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
     
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            if (flags == 0 || flags == PHYMOD_PRBS_DIRECTION_TX) {
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                /* Get sys_en, wr_lane, rd_lane using lane descriptor */
                if (pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA; 
                } else {
                    sys_en = tx_pkg_ln_des->sideB;
                }
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : tx_pkg_ln_des->slice_wr_val; 
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_config_tx_prbs(pa, prbs_mode, (uint8_t)prbs_inv)); 
                }
            }
            if (flags == 0 || flags == PHYMOD_PRBS_DIRECTION_RX) {
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                /* Get sys_en, wr_lane, rd_lane using lane descriptor */
                if (pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA; 
                } else {
                    sys_en = rx_pkg_ln_des->sideB;
                } 
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : rx_pkg_ln_des->slice_wr_val; 
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_config_rx_prbs(pa, prbs_mode, PRBS_INITIAL_SEED_HYSTERESIS, (uint8_t)prbs_inv)); 
                }
            } 

            if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                    break;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;  
}

/**   Get PRBS generator configuration 
 *    This function is used to retrieve PRBS generator configuration from 
 *    the chip 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
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
                        uint32_t *prbs_inv)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t chip_id = 0;
    uint32_t acc_flags = 0;
    uint8_t prbs_invert = 0;
    enum srds_prbs_checker_mode_enum prbs_checker_mode = 0; 
    *prbs_inv = 0;
    *prbs_mode = 0; 

    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            if (flags == 0 || flags == PHYMOD_PRBS_DIRECTION_TX) {
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                /* Get sys_en, wr_lane, rd_lane using lane descriptor */
                if (pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA; 
                } else {
                    sys_en = tx_pkg_ln_des->sideB;
                } 
                wr_lane = tx_pkg_ln_des->slice_wr_val;
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_get_tx_prbs_config(pa, prbs_mode, &prbs_invert));
                    *prbs_inv = prbs_invert;
                }
            } 
            if (flags == 0 || flags == PHYMOD_PRBS_DIRECTION_RX) {
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                /* Get sys_en, wr_lane, rd_lane using lane descriptor */
                if (pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA; 
                } else {
                    sys_en = rx_pkg_ln_des->sideB;
                } 
                wr_lane = rx_pkg_ln_des->slice_wr_val;
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_get_rx_prbs_config(pa, prbs_mode, &prbs_checker_mode, &prbs_invert));
                    *prbs_inv = prbs_invert;
                }
            } 
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;  
}


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
                                uint32_t *error_count)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0; 
    uint32_t acc_flags = 0;
    uint8_t chk_lock = 0;  
    uint8_t loss_of_lock = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t per_lane_err_count = 0;
    *lock_status = 1;
    *lock_loss = 1;
    *error_count = 0;
     

    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
         
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                PHYMOD_IF_ERR_RETURN(falcon_furia_prbs_chk_lock_state(pa, &chk_lock));
                *lock_status &= chk_lock;
                PHYMOD_IF_ERR_RETURN(falcon_furia_prbs_err_count_state(pa, &per_lane_err_count, &loss_of_lock));
                *error_count |= per_lane_err_count;
                *lock_loss &= loss_of_lock;
           } 
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

/**   Enable PRBS generator and PRBS checker 
 *    This function is used to enable or disable PRBS generator and PRBS checker
 *    as requested by the user  
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param enable              Enable or disable as specified by the user
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_enable_set(const phymod_access_t *pa, uint32_t flags, uint32_t enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint8_t ena_dis = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    TLB_RX_PRBS_CHK_CONFIG_t prbs_chk_config;
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    uint32_t acc_flags = 0; 
    PHYMOD_MEMSET(&prbs_chk_config, 0 , sizeof(TLB_RX_PRBS_CHK_CONFIG_t));
    
    ena_dis = enable ? 1 : 0;


 
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (flags == 0 || flags == PHYMOD_PRBS_DIRECTION_TX) {
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                /* Get sys_en, wr_lane, rd_lane using lane descriptor */
                if (pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA; 
                } else {
                    sys_en = tx_pkg_ln_des->sideB;
                } 
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : tx_pkg_ln_des->slice_wr_val;
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                PHYMOD_IF_ERR_RETURN(falcon_furia_tx_prbs_en(pa, ena_dis));
            } 
            if (flags == 0 || flags == PHYMOD_PRBS_DIRECTION_RX) {
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                /* Get sys_en, wr_lane, rd_lane using lane descriptor */
                if (pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA; 
                } else {
                    sys_en = rx_pkg_ln_des->sideB;
                } 
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : rx_pkg_ln_des->slice_wr_val;
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

                if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                    if(enable) {
                        /* Read PRBS Checker Control Register */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_prbs_chk_config_Adr,\
                                                    &prbs_chk_config.data));

                        /* Update the field PRBS checker clock enable. */
                        prbs_chk_config.fields.prbs_chk_clk_en_frc_on = 1;

                        /* Write to PRBS Checker Control Register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_prbs_chk_config_Adr,\
                                                     prbs_chk_config.data));
                    }
                    PHYMOD_IF_ERR_RETURN(falcon_furia_rx_prbs_en(pa, ena_dis));
                    if(!enable) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_prbs_chk_config_Adr,\
                                                    &prbs_chk_config.data));

                        /* Update the field PRBS checker clock enable. */
                        prbs_chk_config.fields.prbs_chk_clk_en_frc_on = 0;

                        /* Write to PRBS Checker Control Register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_prbs_chk_config_Adr,\
                                                     prbs_chk_config.data));
                    }
                }
            }

            if(FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                break;
            } 
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

/**   Get Enable status of PRBS generator and PRBS checker 
 *    This function is used to retrieve the enable status of PRBS generator and
 *    PRBS checker
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param flags              Flags for prbs
 *    @param enable              Enable or disable read from chip 
 *                              1 - Enable
 *                              0 - Disable 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_prbs_enable_get(const phymod_access_t *pa, uint32_t flags, uint32_t *enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint8_t gen_en = 0;
    uint8_t chk_en = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    gen_en = 0;
    chk_en = 0; 
    *enable = 0; 
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (flags == 0 || flags == PHYMOD_PRBS_DIRECTION_TX) {
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                /* Get sys_en, wr_lane, rd_lane using lane descriptor */
                if (pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA; 
                } else {
                    sys_en = tx_pkg_ln_des->sideB;
                } 
                wr_lane = tx_pkg_ln_des->slice_wr_val;
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_get_tx_prbs_en(pa, &gen_en));
                    *enable = gen_en;
                }
            }
            if (flags == 0 || flags == PHYMOD_PRBS_DIRECTION_RX) {
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                /* Get sys_en, wr_lane, rd_lane using lane descriptor */
                if (pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA; 
                } else {
                    sys_en = rx_pkg_ln_des->sideB;
                } 
                wr_lane = rx_pkg_ln_des->slice_wr_val;
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_get_rx_prbs_en(pa, &chk_en));
                    *enable = chk_en;
                }
            }
            break; 
        }
    }
    /* Set the slice register to default */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    if(flags == 0) {
        if(gen_en && chk_en) {
            *enable = 1;
        } else {
            *enable = 0;
        }
    }
   
    return PHYMOD_E_NONE;
}

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
int furia_link_status(const phymod_access_t *pa,
                                   uint32_t *link_status) 
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint8_t rx_pmd_lock = 0;
    uint32_t fcpcs_lock = 1;
    uint16_t fcpcs_chkr_mode = 0;
    int pkg_side = 0;
    uint32_t acc_flags = 0; 
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t tmp_main_ctrl_mode;
    SYS_RX_PMA_DP_MAIN_CTRL_t sys_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_LIVE_STATUS_FC_MODE_t          line_rx_pma_dp_live_status_fc_mode;
    LINE_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t         line_rx_pma_dp_live_status_pcs_mode;
    SYS_RX_PMA_DP_LIVE_STATUS_FC_MODE_t          sys_rx_pma_dp_live_status_fc_mode;
    SYS_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t         sys_rx_pma_dp_live_status_pcs_mode;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    PHYMOD_MEMSET(&line_rx_dp_main_ctrl, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&tmp_main_ctrl_mode, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t));
    PHYMOD_MEMSET(&sys_rx_dp_main_ctrl, 0 , sizeof(SYS_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_live_status_fc_mode, 0 , sizeof(LINE_RX_PMA_DP_LIVE_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_live_status_pcs_mode, 0 , sizeof(LINE_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_live_status_fc_mode, 0 , sizeof(SYS_RX_PMA_DP_LIVE_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_live_status_pcs_mode, 0 , sizeof(SYS_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t));
    
    *link_status = 1;
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_pmd_lock_status(pa, &rx_pmd_lock));
                if(sys_en == LINE) {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                &line_rx_dp_main_ctrl.data));
                    tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                } else {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                &sys_rx_dp_main_ctrl.data)); 
                    tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                }
                    
                if(!tmp_main_ctrl_mode.fields.link_mon_en) {
                    *link_status &= rx_pmd_lock;
                } else {
                    if(sys_en == LINE) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_live_status_Adr,\
                                                    &line_rx_pma_dp_live_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_live_status_Adr,\
                                                    &line_rx_pma_dp_live_status_fc_mode.data));
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_live_status_Adr,\
                                                    &sys_rx_pma_dp_live_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_live_status_Adr,\
                                                    &sys_rx_pma_dp_live_status_fc_mode.data));
                    }

                    if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x0)                    {
                        fcpcs_chkr_mode = PCS49_1x10G;
                    }
                    if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x1) {
                        if(tmp_main_ctrl_mode.fields.mode_50g) {
                            fcpcs_chkr_mode = PCS82_2x25G;
                        } else if(tmp_main_ctrl_mode.fields.mode_100g) {
                            fcpcs_chkr_mode = PCS82_4x25G;
                        } else {
                            fcpcs_chkr_mode = PCS82_4x10G;
                        }
                    } 
                    if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x2) {
                        fcpcs_chkr_mode = FC4;
                    }
                    if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x3) {
                        fcpcs_chkr_mode = FC8;
                    }

                    switch (fcpcs_chkr_mode) {
                        case PCS49_1x10G :
                        case PCS82_4x10G :
                        case FC16 :
                        case FC32 :
                        case PCS82_2x25G :
                        case PCS82_4x25G :
                            if(sys_en == LINE) {
                                fcpcs_lock &= line_rx_pma_dp_live_status_pcs_mode.fields.pcs_status;
                            } else {
                                fcpcs_lock &= sys_rx_pma_dp_live_status_pcs_mode.fields.pcs_status;
                            }
                        break;
                        case FC4 :
                        case FC8 :
                            if(sys_en == LINE) {
                                fcpcs_lock = line_rx_pma_dp_live_status_fc_mode.fields.wsyn_link_fail;
                            } else {
                                fcpcs_lock = sys_rx_pma_dp_live_status_fc_mode.fields.wsyn_link_fail;
                            }
                        break;
                    }
                    *link_status &= fcpcs_lock;
                }
            } 
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}
/*    furia_single_pmd_enable
 *    For 100G and 106G the part needs to be in single_pmd mode
 *    i.e. all 4-lanes are part of the same port.
 *    So we should set rg_single_pmd_frc to 0 as this force is only needed for 50G.
 *    rg_single_pmd_frc_val is needed only for 100G SR4 as hardware doesn't set this field.
 *    Its a don't care for KR4, LR4, and CR4 interfaces as hardware programs it correctly.
 *
 *    @param pa                 Pointer to phymod access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_single_pmd_enable(const phymod_access_t *pa)
{
    FUR_MISC_CTRL_MODE_DEC_FRC_t dec_frc;
    FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t dec_frc_val;

    PHYMOD_MEMSET(&dec_frc, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_t));
    PHYMOD_MEMSET(&dec_frc_val, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t));

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,
            FUR_MISC_CTRL_mode_dec_frc_val_Adr,
            &dec_frc_val.data));
    dec_frc_val.fields.rg_single_pmd_frc_val = 1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,
            FUR_MISC_CTRL_mode_dec_frc_val_Adr,
            dec_frc_val.data));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,
            FUR_MISC_CTRL_mode_dec_frc_Adr,
            &dec_frc.data));
    dec_frc.fields.rg_single_pmd_frc = 0;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,
            FUR_MISC_CTRL_mode_dec_frc_Adr,
            dec_frc.data));
    return PHYMOD_E_NONE;
}

/**   Set config mode 
 *    This function is used to set the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param config             Pointer to phy interface config sturcuture 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_set_config_mode(const phymod_access_t *pa,
                       const phymod_phy_inf_config_t* config)
{
    IEEE_AN_BLK0_AN_CONTROL_REGISTER_t an_ctrl_reg;
    FUR_MISC_CTRL_UDMS_PHY_t udms_phy;
    FUR_MISC_CTRL_UDMS_LINK_t udms_link;
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable_reg;
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl;
    SYS_RX_PMA_DP_MAIN_CTRL_t sys_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t tmp_main_ctrl_mode;
    FUR_GEN_CNTRLS_gpreg13_t gen_ctrl_gpreg13;
    FUR_MISC_CTRL_MODE_DEC_FRC_t dec_frc;
    FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t dec_frc_val;
    phymod_phy_inf_config_t config_copy;
    uint16_t phy_type, prev_phy_type = 0;
    uint16_t link_type;
    int lane_map = 0;
    uint16_t lane_map_saved = 0;
    int lane_index = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    uint32_t acc_flags = 0;
    int pkg_side = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    FURIA_REF_CLK_E ref_clk_freq = REF_CLK_INVALID;
    FALCON_PLL_MODE_E pll_mode = PLL_MODE_INVALID;
    phymod_interface_t actual_intf = 0;
    uint32_t actual_speed = 0;
    phymod_ref_clk_t actual_ref_clk;
    uint32_t actual_interface_modes = 0;
    uint8_t fw_enable = 0;
    uint8_t retry_count = 40;
    phymod_access_t l_pa;
    uint32_t curr_intf[MAX_NUM_LANES] = {0,};
    uint32_t curr_speed[MAX_NUM_LANES] = {0,};
    int wr_lane = 0;
    int rd_lane = 0;
    int sys_en = 0;
    uint32_t higig_mode = 0;
    uint16_t curr_pll_mode[2] = {0,};
    uint16_t side_index = 0;
    uint16_t serdes_pll_mode = 99; /*99 as invalid mode*/
    uint16_t otn_type = 8;
    PLL_CAL_COM_CTL_7_t pll_ctrl_7;

    PHYMOD_MEMSET(&an_ctrl_reg, 0 , sizeof(IEEE_AN_BLK0_AN_CONTROL_REGISTER_t));
    PHYMOD_MEMSET(&udms_phy, 0 , sizeof(FUR_MISC_CTRL_UDMS_PHY_t));
    PHYMOD_MEMSET(&udms_link, 0 , sizeof(FUR_MISC_CTRL_UDMS_LINK_t));
    PHYMOD_MEMSET(&fw_enable_reg, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));
    PHYMOD_MEMSET(&line_rx_dp_main_ctrl, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&sys_rx_dp_main_ctrl, 0 , sizeof(SYS_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&tmp_main_ctrl_mode, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t));
    PHYMOD_MEMSET(&dec_frc_val, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t));
    PHYMOD_MEMSET(&dec_frc, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_t));
    PHYMOD_MEMSET(&config_copy, 0 , sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&pll_ctrl_7, 0 , sizeof(PLL_CAL_COM_CTL_7_t));
    /* Get a copy of phymod access */
    PHYMOD_MEMCPY(&l_pa, pa, sizeof(phymod_access_t));
    PHYMOD_MEMCPY(&config_copy, config, sizeof(phymod_phy_inf_config_t));

    if (config_copy.device_aux_modes != NULL) {
        otn_type = ((FURIA_DEVICE_OTN_MODE_T*) config_copy.device_aux_modes)->otn_type;
    }

    /* Get CHIP ID */
    chip_id = _furia_get_chip_id(pa);
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    lane_map_saved = lane_map;
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    higig_mode = PHYMOD_INTF_MODES_HIGIG_GET(config);
    if (pkg_side == SIDE_B) {
        if (FURIA_IS_SIMPLEX(chip_id)) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("Wrong Package side specified, Package side must be 0 for simplex packages(Tx side)")));
        }
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_gpreg13_Adr,\
                                    &gen_ctrl_gpreg13.data));
        if ((config_copy.interface_type == phymodInterfaceKX) ||
            (config_copy.interface_type == phymodInterfaceKR) ||
            (config_copy.interface_type == phymodInterfaceKR4) ||
            (config_copy.interface_type == phymodInterfaceSR)) {  
            gen_ctrl_gpreg13.fields.phy_type_sys = 0; /* Backplane modes */
        }
        if ((config_copy.interface_type == phymodInterfaceCAUI4) ||
            (config_copy.interface_type == phymodInterfaceCAUI) ||
            (config_copy.interface_type == phymodInterfaceXLAUI) ||
            (config_copy.interface_type == phymodInterfaceXFI) || 
            (config_copy.interface_type == phymodInterfaceCAUI4_C2C)) {
            gen_ctrl_gpreg13.fields.phy_type_sys = 1; /* PCB trace for chip to chip module */
        }
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_GEN_CNTRLS_gpreg13_Adr,\
                                     gen_ctrl_gpreg13.data));
        PHYMOD_IF_ERR_RETURN(_furia_fw_enable(pa));
        return PHYMOD_E_NONE;
    }
    /*Disable the FEC in HG mode since FEC is supported only in IEEE modes
      Disable initially for all the IEEE mode and enable FEC for 100G SR4/KR4/CR4 */
    if (FURIA_IS_DUPLEX(chip_id)) {
        PHYMOD_IF_ERR_RETURN
            (furia_fec_enable_set(pa, 0));
    }
    /* Wait for firmware to ready before config to be changed */
    PHYMOD_IF_ERR_RETURN
         (READ_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                 &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;


    while((fw_enable != 0) && (retry_count)) {
         PHYMOD_IF_ERR_RETURN
             (READ_FURIA_PMA_PMD_REG(pa,\
                                     FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                     &fw_enable_reg.data));
         fw_enable = fw_enable_reg.fields.fw_enable_val;
         PHYMOD_USLEEP(200);
         retry_count--;
     }

     if(!retry_count) {
         PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("1:config failed, micro controller is busy..")));
     }

    /* Read current mode configurations from registers */ 
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        l_pa.lane_mask = 0x1 << lane_index;
        PHYMOD_IF_ERR_RETURN
            (furia_get_config_mode(&l_pa, &actual_intf, &actual_speed, &actual_ref_clk, &actual_interface_modes, &config_copy ,lane_map_saved));
        curr_intf[lane_index] = actual_intf;
        curr_speed[lane_index] = actual_speed;
        l_pa.lane_mask = 0x0;
    }

    
    if (FURIA_IS_DUPLEX(chip_id)) {
        if ((curr_speed[0] == SPEED_10G || curr_speed[0] == SPEED_20G) ||
            (curr_speed[1] == SPEED_10G || curr_speed[1] == SPEED_20G) ||
            (curr_speed[2] == SPEED_10G || curr_speed[2] == SPEED_20G) ||
            (curr_speed[3] == SPEED_10G || curr_speed[3] == SPEED_20G)) {
            if((config->data_rate == SPEED_25G || config->data_rate == SPEED_27G ||
                config->data_rate == SPEED_50G || config->data_rate == SPEED_53G) && (lane_map != BROADCAST)) {
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Unsupported mode")));
            }
        }

        if ((curr_speed[0] == SPEED_25G || curr_speed[0] == SPEED_50G) ||
            (curr_speed[1] == SPEED_25G || curr_speed[1] == SPEED_50G) ||
            (curr_speed[2] == SPEED_25G || curr_speed[2] == SPEED_50G) ||
            (curr_speed[3] == SPEED_25G || curr_speed[3] == SPEED_50G)) {
            if((config->data_rate == SPEED_10G || config->data_rate == SPEED_20G || 
                config->data_rate == SPEED_11G || config->data_rate == SPEED_21G) && (lane_map != BROADCAST)) {
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Unsupported mode")));
            }
        }

        if(((config->data_rate == SPEED_100G || config->data_rate == SPEED_106G ||
             config->data_rate == SPEED_40G || config->data_rate == SPEED_42G) && 
             (config->interface_type == phymodInterfaceSR4 ||
              config->interface_type == phymodInterfaceKR4 ||
              config->interface_type == phymodInterfaceCR4 ||
              config->interface_type == phymodInterfaceLR4)) && (lane_map != BROADCAST)) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("2:Wrong Lanemap param")));
        }
    
        if((config->data_rate == SPEED_50G || config->data_rate == SPEED_53G ||
            config->data_rate == SPEED_20G || config->data_rate == SPEED_21G ||
            config->data_rate == SPEED_40G || config->data_rate == SPEED_42G) &&
            (config->interface_type == phymodInterfaceKR2) && 
            (lane_map != MULTICAST01 && lane_map != MULTICAST23 && lane_map != BROADCAST)) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("3:Wrong Lanemap param")));
        }
    } 
   
    if (FURIA_IS_SIMPLEX(chip_id)) {
        if(config_copy.data_rate == SPEED_40G ||
           config_copy.data_rate == SPEED_42G ||
           config_copy.data_rate == SPEED_11G) {
            config_copy.data_rate = SPEED_10G;
            switch (config_copy.interface_type) {
                case phymodInterfaceKR4:
                case phymodInterfaceKR:
                    config_copy.interface_type = phymodInterfaceKR;
                break;
                case phymodInterfaceLR4:
                case phymodInterfaceLR:
                    config_copy.interface_type = phymodInterfaceLR;
                break;
                case phymodInterfaceSR4:
                case phymodInterfaceSR:
                    config_copy.interface_type = phymodInterfaceSR;
                break;
                case phymodInterfaceER4:
                case phymodInterfaceER:
                    config_copy.interface_type = phymodInterfaceER;
                break;
                case phymodInterfaceCX4:
                case phymodInterfaceCX:
                    config_copy.interface_type = phymodInterfaceCX;
                break;
                case phymodInterfaceCR4:
                case phymodInterfaceCR:
                    config_copy.interface_type = phymodInterfaceCR;
                break;
                case phymodInterfaceXLAUI:
                case phymodInterfaceXFI:
                    config_copy.interface_type = phymodInterfaceXFI;
                break;
                case phymodInterfaceSFI:
                    config_copy.interface_type = phymodInterfaceSFI;
                break;
                default:
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("3:Unsupported speed and interface")));
                break;
            }
        } 
        if(config_copy.data_rate == SPEED_50G || config_copy.data_rate == SPEED_53G ||
           config_copy.data_rate == SPEED_100G || config_copy.data_rate == SPEED_106G || 
           config_copy.data_rate == SPEED_27G) { 
           config_copy.data_rate = SPEED_25G;
            switch (config_copy.interface_type) {
                case phymodInterfaceKR4:
                case phymodInterfaceKR2:
                case phymodInterfaceKR:
                    config_copy.interface_type = phymodInterfaceKR;
                break;
                case phymodInterfaceLR4:
                case phymodInterfaceLR:
                    config_copy.interface_type = phymodInterfaceLR;
                break;
                case phymodInterfaceCR4:
                case phymodInterfaceCR2:
                case phymodInterfaceCR:
                    config_copy.interface_type = phymodInterfaceCR;
                break;
                case phymodInterfaceSR4:
                case phymodInterfaceSR:
                    config_copy.interface_type = phymodInterfaceSR;
                break;
                case phymodInterfaceER4:
                case phymodInterfaceER:
                    config_copy.interface_type = phymodInterfaceER;
                break;
                case phymodInterfaceCAUI4:
                case phymodInterfaceCAUI:
                case phymodInterfaceCAUI4_C2C:
                case phymodInterfaceCAUI4_C2M:
                case phymodInterfaceVSR:
                    config_copy.interface_type = phymodInterfaceVSR;
                break;
                default:
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("3:Unsupported speed and interface")));
                break;
            }
        }
        if (config_copy.data_rate == SPEED_21G) {
            config_copy.data_rate = SPEED_20G;
            switch (config_copy.interface_type) {
                case phymodInterfaceKR2:
                case phymodInterfaceKR:
                    config_copy.interface_type = phymodInterfaceKR;
                break;
                case phymodInterfaceLR:
                    config_copy.interface_type = phymodInterfaceLR;
                break;
                case phymodInterfaceSR:
                    config_copy.interface_type = phymodInterfaceSR;
                break;
                case phymodInterfaceER:
                    config_copy.interface_type = phymodInterfaceER;
                break;
                default:
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("3:Unsupported speed and interface")));
                break;
            }
        }
    } 
    /*perform phymod to furia enum conversion */
    PHYMOD_IF_ERR_RETURN (
       _furia_phymod_to_furia_type_ref_clk_freq(config_copy.ref_clock, &ref_clk_freq));
    /*Set the ref_clk freq before programming phy type and link type */ 
    PHYMOD_IF_ERR_RETURN (
       _furia_config_clk_scaler_without_m0_reset(pa, ref_clk_freq));
    phy_type = _furia_get_phy_type(config_copy.interface_type, config_copy.data_rate);
 
    phy_type = (config_copy.interface_modes & PHYMOD_INTF_MODES_FIBER) ? 0x3 /*fiber*/ : phy_type;
    link_type = _furia_get_link_type(config_copy.interface_type, config_copy.data_rate, otn_type);
    if( config_copy.interface_modes & PHYMOD_INTF_MODES_FIBER){
        /*phy_type is 0x3 for FC interfaces*/
        if( config_copy.data_rate == SPEED_8P5G){
            link_type = ( config_copy.interface_type ==  phymodInterfaceSFI)? 0xd : 0x2;
            phy_type = ( config_copy.interface_type ==  phymodInterfaceSFI)? 0x1 : phy_type ;
        }else if( config_copy.data_rate == SPEED_14P025G){
            link_type = ( config_copy.interface_type ==  phymodInterfaceSFI)? 0xd : 0x3;
            phy_type = ( config_copy.interface_type ==  phymodInterfaceSFI)? 0x1 : phy_type ;
        }else if( config_copy.data_rate == SPEED_28P05G){ /*FC32*/
            link_type = ( config_copy.interface_type ==  phymodInterfaceCAUI)? 0xf : 0x4;
            phy_type = ( config_copy.interface_type ==  phymodInterfaceCAUI)? 0x1 : phy_type ;
        }else if( config_copy.data_rate == SPEED_10P518G){ /*FC10 missing from datasheet. same as FC16*/
            link_type = ( config_copy.interface_type ==  phymodInterfaceSFI)? 0xd : 0x3;
            phy_type = ( config_copy.interface_type ==  phymodInterfaceSFI)? 0x1 : phy_type ;
        }else{
            phy_type = 0x1;
        }
    }

    if ((config_copy.interface_type == phymodInterfaceCR || config_copy.interface_type == phymodInterfaceCR2) && 
        (config_copy.data_rate == SPEED_25G || config_copy.data_rate == SPEED_27G ||
         config_copy.data_rate == SPEED_50G || config_copy.data_rate == SPEED_53G)) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_MISC_CTRL_mode_dec_frc_val_Adr,\
                                    &dec_frc_val.data));
        /* Disable FEC */
        dec_frc_val.fields.rg_cl91_enabled_frc_val = 0;
        /* Disable single PMD */
        dec_frc_val.fields.rg_single_pmd_frc_val= 0;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MISC_CTRL_mode_dec_frc_val_Adr,\
                                     dec_frc_val.data));
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_MISC_CTRL_mode_dec_frc_Adr,\
                                &dec_frc.data));
        dec_frc.fields.rg_cl91_enabled_frc = 1;
        dec_frc.fields.rg_single_pmd_frc = 1;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MISC_CTRL_mode_dec_frc_Adr,\
                                     dec_frc.data));
    }

    if (((config_copy.interface_type == phymodInterfaceSR4) ||
        (config_copy.interface_type == phymodInterfaceCR4) ||
        (config_copy.interface_type == phymodInterfaceLR4) ||
        (config_copy.interface_type == phymodInterfaceKR4)) &&
        ((config_copy.data_rate == SPEED_100G) ||
        (config_copy.data_rate == SPEED_106G)) &&
        (FURIA_IS_DUPLEX(chip_id))) {
        PHYMOD_IF_ERR_RETURN
            (furia_single_pmd_enable(pa));
    }

    if (((config_copy.interface_type == phymodInterfaceSR4) ||
        (config_copy.interface_type == phymodInterfaceCR4) ||
        (config_copy.interface_type == phymodInterfaceKR4)) &&
        (config_copy.data_rate == SPEED_100G) &&
        (FURIA_IS_DUPLEX(chip_id))&&(!higig_mode)) {
        PHYMOD_IF_ERR_RETURN
            (furia_fec_enable_set(pa, 1));
    }
    /* Disable autonegotiation on all the PHY lanes */
    FURIA_GET_LANE_MASK(chip_id, lane_map);
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            /* Force AN disable since mode is configured using UDMS*/ 
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        IEEE_AN_BLK0_an_control_register_Adr,\
                                        &an_ctrl_reg.data));
           /* Update auto_negotiationenable bit field */
           an_ctrl_reg.fields.auto_negotiationenable = 0;

           /* Write to AN control register */
           PHYMOD_IF_ERR_RETURN
               (WRITE_FURIA_PMA_PMD_REG(pa,\
                                        IEEE_AN_BLK0_an_control_register_Adr,\
                                        an_ctrl_reg.data));
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        IEEE_AN_BLK0_an_control_register_Adr,\
                                        &an_ctrl_reg.data));
            if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                break;
            }
        }
    }
   
    /* Program udms_en=1 */
    /* Read UDMS PHY Type Register */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_udms_phy_Adr,\
                                &udms_phy.data));
 
    /* Update the field udms enable */
    udms_phy.fields.udms_en = 1;
    /* Update udms_phy_type */
    prev_phy_type = udms_phy.fields.udms_phy_type;
    udms_phy.fields.udms_phy_type = phy_type;

    /* Write to UDMS PHY Type Register */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_udms_phy_Adr,\
                                 udms_phy.data));

    /* Read the link type Register*/
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_udms_link_Adr,\
                                &udms_link.data));
    if(
       (curr_intf[0] == phymodInterfaceKR4 &&
        curr_intf[1] == phymodInterfaceKR4 &&
        curr_intf[2] == phymodInterfaceKR4 &&
        curr_intf[3] == phymodInterfaceKR4) ||
       (curr_intf[0] == phymodInterfaceCR4 &&
        curr_intf[1] == phymodInterfaceCR4 &&
        curr_intf[2] == phymodInterfaceCR4 &&
        curr_intf[3] == phymodInterfaceCR4) ||
       (curr_intf[0] == phymodInterfaceLR4 &&
        curr_intf[1] == phymodInterfaceLR4 &&
        curr_intf[2] == phymodInterfaceLR4 &&
        curr_intf[3] == phymodInterfaceLR4) ||
       (curr_intf[0] == phymodInterfaceSR4 &&
        curr_intf[1] == phymodInterfaceSR4 &&
        curr_intf[2] == phymodInterfaceSR4 &&
        curr_intf[3] == phymodInterfaceSR4)) {
        FURIA_GET_LANE_MASK(chip_id, lane_map);
    } else if ((prev_phy_type != phy_type) && (prev_phy_type == FURIA_PHYTYPE_FP 
            || prev_phy_type == FURIA_PHYTYPE_PROP || prev_phy_type == FURIA_PHYTYPE_BP) && 
            (phy_type == FURIA_PHYTYPE_FP || phy_type == FURIA_PHYTYPE_PROP || phy_type == FURIA_PHYTYPE_BP) &&
            (config_copy.data_rate == SPEED_10G || config_copy.data_rate == SPEED_11G || 
             config_copy.data_rate == SPEED_25G || config_copy.data_rate == SPEED_27G ||
             config_copy.data_rate == SPEED_50G || config_copy.data_rate == SPEED_53G)) 
    {
        /* USED TO MOVE FROM CR TO SR  and KR2 to CR2 PER DIE
         * Condition will not success for cr to cx / sr to er etc */
        FURIA_GET_LANE_MASK(chip_id, lane_map);
    } else if (((/* link_type == FURIA_LINKTYPE_7 ||*/
                 link_type == FURIA_LINKTYPE_5) ||
                 (link_type == FURIA_LINKTYPE_8)) &&
               (config_copy.data_rate == SPEED_25G ||
                config_copy.data_rate == SPEED_27G ||
                config_copy.data_rate == SPEED_50G ||
                config_copy.data_rate == SPEED_53G )) {
        FURIA_GET_LANE_MASK(chip_id, lane_map);
    } else {
        lane_map = lane_map_saved;
    }

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Update the field udms enable */
            if (pkg_ln_des->die_lane_num == 0) {
                udms_link.fields.udms_ln0_lnk_type = link_type;
            }
            if (pkg_ln_des->die_lane_num == 1) {
                udms_link.fields.udms_ln1_lnk_type = link_type;
            }
            if (pkg_ln_des->die_lane_num == 2) {
                udms_link.fields.udms_ln2_lnk_type = link_type;
            }
            if (pkg_ln_des->die_lane_num == 3) {
                udms_link.fields.udms_ln3_lnk_type = link_type;
            }
            PHYMOD_IF_ERR_RETURN
                (_furia_set_intf_type(pa, phy_type, link_type, config_copy.interface_type, lane_index, otn_type,config_copy.ref_clock,higig_mode));
        }
    }


    /* Program the Link Type Register with link type*/
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_udms_link_Adr,\
                                 udms_link.data));
    for(side_index = 0; side_index < 2; side_index ++) {
        /* Set PLL div */
        wr_lane = 1 << 0;
        rd_lane = 0;
        if(side_index == 0) {
            sys_en = SIDE_A;
        } else {
            sys_en = SIDE_B;
        } 
        /* Program the slice register */
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

        /* Program pll_mode */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    PLL_CAL_COM_CTL_7_Adr,\
                                    &pll_ctrl_7.data));
        curr_pll_mode[side_index]= pll_ctrl_7.fields.pll_mode;
    }
    pll_mode = curr_pll_mode[0] & curr_pll_mode[1];

    /* Set the PLL value based on ref clock and speed */ 
    if( config_copy.interface_type == phymodInterfaceOTN ) {
        PHYMOD_IF_ERR_RETURN(
                _furia_get_pll_mode_otn(config_copy.ref_clock, otn_type, &pll_mode));
     }else{
         PHYMOD_IF_ERR_RETURN(
               _furia_get_pll_mode(config_copy.ref_clock, config_copy.data_rate, higig_mode, &pll_mode));
     }
 
    if ((curr_pll_mode[0] != pll_mode) || (curr_pll_mode[1] != pll_mode)) {
    PHYMOD_IF_ERR_RETURN(
    _furia_get_pll_for_serdes(config_copy.ref_clock,config_copy.data_rate,pll_mode,&serdes_pll_mode));  
    PHYMOD_IF_ERR_RETURN(
        _furia_config_pll_div(pa, serdes_pll_mode));
    } 
    /*Wait for fw_enable to go low  before setting fw_enable_val*/
    retry_count = 40;
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                &fw_enable_reg.data));
    fw_enable_reg.fields.fw_enable_val = 1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                 fw_enable_reg.data));
    /*Wait for fw_enable to go low */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;

    while((fw_enable != 0) && (retry_count)) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                    &fw_enable_reg.data));
        fw_enable = fw_enable_reg.fields.fw_enable_val;
        PHYMOD_USLEEP(200);
        retry_count--; 
    }

    if(!retry_count) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("config failed, micro controller is busy..")));
    }
    {
        uint32_t phy_type_1, link_type_1;
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_MISC_CTRL_phy_type_status_Adr,\
                                    &phy_type_1));
        /* Read the link type Register*/
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_MISC_CTRL_link_type_status_Adr,\
                                    &link_type_1));
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                l_pa.lane_mask = 0x1 << lane_index;
                PHYMOD_IF_ERR_RETURN
                    (furia_get_config_mode(&l_pa, &actual_intf, &actual_speed, &actual_ref_clk, &actual_interface_modes, &config_copy,lane_map_saved));
                if((config_copy.interface_type != actual_intf) ||
                   (config_copy.ref_clock != actual_ref_clk)) {
                    PHYMOD_IF_ERR_RETURN
                        (_furia_set_intf_type(pa, phy_type_1, link_type_1, curr_intf[lane_index], lane_index, otn_type,config_copy.ref_clock,higig_mode));
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Set config failed")));
                }
            }
        }
    }
    return PHYMOD_E_NONE;
}

/**   Get config mode 
 *    This function is used to retrieve the operating mode of the PHY
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param intf               Interface type 
 *    @param speed              Speed val retrieved from PHY 
 *    @param ref_clk            Reference clock 
 *    @param interface_modes    Supported interface modes
 *    @param config             config structure
 *    @param saved_lane_map     lane_map passed as per the mode of config
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_get_config_mode(const phymod_access_t *pa,
                       phymod_interface_t *intf,
                       uint32_t *speed,
                       phymod_ref_clk_t *ref_clk,
                       uint32_t *interface_modes, phymod_phy_inf_config_t *config , uint16_t saved_lane_map)
{
    IEEE_AN_BLK0_AN_CONTROL_REGISTER_t an_ctrl_reg;
    FUR_MISC_CTRL_PHY_TYPE_STATUS_t udms_phy;
    FUR_MISC_CTRL_LINK_TYPE_STATUS_t udms_link;
    FUR_GEN_CNTRLS_GEN_CONTROL3_t gen_ctrl_3;
    FUR_GEN_CNTRLS_gpreg13_t gen_ctrl_gpreg13;
    furia_an_ability_t an_ability; 
    uint32_t logic_to_phy_ln0_map = 0; 
    uint16_t master_lane_num = 0;
    int lane_map = 0;
    uint16_t phy_type = 0;
    uint16_t link_type = 0;
    uint16_t saved_intf = 0;
    uint16_t saved_intf1e2e = 0, saved_refclk = 0;
    uint16_t ref_clk_scaler_val = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    int lane_index = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    uint32_t acc_flags = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int rd_lane = 0;
    int sys_en = 0;
    uint16_t curr_pll_mode[2] = {0,};
    uint16_t side_index = 0;
    PLL_CAL_COM_CTL_7_t pll_ctrl_7;
    uint16_t nototn_mode_pll_140 = 0 ;
    uint16_t mode_FC8 = 0 ;
    uint16_t mode_FC10 = 0 ;
    uint16_t mode_FC32 = 0 ;
    uint16_t mode_11p25g = 0 ;
    uint16_t mode_oc192 = 0 ;
    uint16_t otn_type = 100 ;
    uint16_t nonotn_mode2 = 0 ;
    uint32_t higig_mode = 0;
    uint32_t is_OTU2 = 0;

    *speed = 0;
    *intf = 0;
    *ref_clk = 0;
    *interface_modes = 0; 
    PHYMOD_MEMSET(&an_ctrl_reg, 0 , sizeof(IEEE_AN_BLK0_AN_CONTROL_REGISTER_t));
    PHYMOD_MEMSET(&udms_phy, 0 , sizeof(FUR_MISC_CTRL_PHY_TYPE_STATUS_t));
    PHYMOD_MEMSET(&udms_link, 0 , sizeof(FUR_MISC_CTRL_LINK_TYPE_STATUS_t));
    PHYMOD_MEMSET(&gen_ctrl_3, 0 , sizeof(FUR_GEN_CNTRLS_GEN_CONTROL3_t));
    PHYMOD_MEMSET(&an_ability, 0 , sizeof(furia_an_ability_t));
    PHYMOD_MEMSET(&pll_ctrl_7, 0 , sizeof(PLL_CAL_COM_CTL_7_t));
    
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                IEEE_AN_BLK0_an_control_register_Adr,\
                                &an_ctrl_reg.data));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    /* Get the link type and phy type from udms reg if AN is not on */ 
    if (!an_ctrl_reg.fields.auto_negotiationenable) {
        /* Read UDMS PHY Type Register */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_MISC_CTRL_phy_type_status_Adr,\
                                    &udms_phy.data));
        /* Read the link type Register*/
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_MISC_CTRL_link_type_status_Adr,\
                                    &udms_link.data));
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
                PHYMOD_NULL_CHECK(pkg_ln_des);
                /* Update the field udms enable */
                if (pkg_ln_des->die_lane_num == 0) {
                    phy_type = udms_phy.fields.ln0_phy_status;
                    link_type = udms_link.fields.ln0_lnk_status;
                }
                if (pkg_ln_des->die_lane_num == 1) {
                    phy_type = udms_phy.fields.ln1_phy_status;
                    link_type = udms_link.fields.ln1_lnk_status;
                }
                if (pkg_ln_des->die_lane_num == 2) {
                    phy_type = udms_phy.fields.ln2_phy_status;
                    link_type = udms_link.fields.ln2_lnk_status;
                }
                if (pkg_ln_des->die_lane_num == 3) {
                    phy_type = udms_phy.fields.ln3_phy_status;
                    link_type = udms_link.fields.ln3_lnk_status;
                }
            PHYMOD_IF_ERR_RETURN(
                _furia_get_intf_type(pa, phy_type, link_type, lane_index, &saved_intf, &saved_intf1e2e,&saved_refclk,&higig_mode));
            }
        }
    } else {
        PHYMOD_IF_ERR_RETURN
            (_furia_autoneg_ability_get(pa, &an_ability));
       
        if((an_ability.cl73_adv.an_base_speed == FURIA_CL73_100GBASE_KR4) ||
           (an_ability.cl73_adv.an_base_speed == FURIA_CL73_100GBASE_CR4)||
           (an_ability.cl73_adv.an_base_speed == FURIA_CL73_40GBASE_KR4) ||
           (an_ability.cl73_adv.an_base_speed == FURIA_CL73_40GBASE_CR4)) {    
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa, 0x8a08,  &logic_to_phy_ln0_map));
            master_lane_num = logic_to_phy_ln0_map & 0x3;
            switch(master_lane_num) {
                case 0:
                    link_type = udms_link.fields.ln0_lnk_status;
                break;
                case 1:
                    link_type = udms_link.fields.ln1_lnk_status;
                break;
                case 2:
                    link_type = udms_link.fields.ln2_lnk_status;
                break;
                case 3:
                    link_type = udms_link.fields.ln3_lnk_status;
                break;
            }
        } else {
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    switch(pkg_ln_des->die_lane_num) {
                        case 0:
                            link_type = udms_link.fields.ln0_lnk_status;
                        break;
                        case 1:
                            link_type = udms_link.fields.ln1_lnk_status;
                        break;
                        case 2:
                            link_type = udms_link.fields.ln2_lnk_status;
                        break;
                        case 3:
                            link_type = udms_link.fields.ln3_lnk_status;
                        break;
                    }
                PHYMOD_IF_ERR_RETURN(
                    _furia_get_intf_type(pa, phy_type, link_type, lane_index, &saved_intf, &saved_intf1e2e,&saved_refclk,&higig_mode));

                }
            }
        }
    }
    
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control3_Adr,\
                                &gen_ctrl_3.data));

    if(gen_ctrl_3.fields.clock_scaler_frc) {
        ref_clk_scaler_val = gen_ctrl_3.fields.clock_scaler_frcval;
    } else {
        ref_clk_scaler_val = gen_ctrl_3.fields.clock_scaler_code;
    }


    for(side_index = 0; side_index < 2; side_index ++) {
        /* Set PLL div */
        wr_lane = 1 << 0;
        rd_lane = 0;
        if(side_index == 0) {
            sys_en = SIDE_A;
        } else {
            sys_en = SIDE_B;
        }
        /* Program the slice register */
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

        /* Program pll_mode */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    PLL_CAL_COM_CTL_7_Adr,\
                                    &pll_ctrl_7.data));
        curr_pll_mode[side_index]= pll_ctrl_7.fields.pll_mode;
    }


    switch(ref_clk_scaler_val) {
        case 0x0:
            switch (curr_pll_mode[0] & curr_pll_mode[1]) {
                case PLL_MODE_132:
                    if(FURIA_IS_SIMPLEX(chip_id)){
                        *ref_clk = phymodRefClk168Mhz;
                        break;
                    }
                case PLL_MODE_165:
                case PLL_MODE_128:
                    if(FURIA_IS_SIMPLEX(chip_id)){
                        *ref_clk = phymodRefClk174Mhz;
                    }else{
                        *ref_clk = phymodRefClk168Mhz;
                        nonotn_mode2 = 1;
                    }
                    is_OTU2 = 1 ;
                    break;
                case PLL_MODE_160:
                    *ref_clk = phymodRefClk174Mhz;
                    break;
            }
            break;
        case 0x1:
            *ref_clk = phymodRefClk161Mhz;
        break;
        case 0x2:
            *ref_clk = phymodRefClk156Mhz;
            switch (curr_pll_mode[0] & curr_pll_mode[1]) {
                case PLL_MODE_140:
                    nototn_mode_pll_140 = 1 ;
                    break;
                case PLL_MODE_128:
                    mode_oc192 = 1 ;
                    break;
                case PLL_MODE_132:
                    mode_FC10 = 1 ;
                    break;
            }
        break;
        case 0x3:
        switch (curr_pll_mode[0] & curr_pll_mode[1]) {
            case PLL_MODE_180:
                mode_11p25g = 1 ;
                break;
            case PLL_MODE_184:
                mode_11p25g = 0 ;
                break;
        }
        *ref_clk = phymodRefClk125Mhz;
        break;
        case 0x4:
            *ref_clk = phymodRefClk106Mhz;
            if((curr_pll_mode[0] & curr_pll_mode[1]) == PLL_MODE_160 ) {
                mode_FC8 = 1 ;
            }else if((curr_pll_mode[0] & curr_pll_mode[1]) == PLL_MODE_264){
                mode_FC32 = 1 ;
            }
        break;
        default:
        break;
    }
    switch(phy_type) {
        case FURIA_PHYTYPE_BP:
            switch(link_type) {
                case FURIA_LINKTYPE_1:
                    *speed = SPEED_1G;
                    *intf = phymodInterfaceKX;
                break;
                case FURIA_LINKTYPE_2:
                    if (*ref_clk == phymodRefClk125Mhz) {
                        *speed = ( mode_11p25g == 1)? SPEED_11P25G : SPEED_11P5G;
                    } else {
                        *speed = SPEED_10G;
                    }
                    *intf = phymodInterfaceKR;
                break;
                case FURIA_LINKTYPE_3:
                    *speed = SPEED_40G;
                    *intf = phymodInterfaceKR4;
                break;
                case FURIA_LINKTYPE_4:
                    *speed = SPEED_100G;
                    *intf = phymodInterfaceKR4;
                break;
                case FURIA_LINKTYPE_5:
                     *speed = SPEED_20G;
                     *intf = phymodInterfaceKR2;
                break;
                case FURIA_LINKTYPE_6:
                    if (*ref_clk == phymodRefClk125Mhz) {
                        *speed = SPEED_23G;
                    } else {
                        *speed = SPEED_20G;
                    }
                    *intf = phymodInterfaceKR;
                break;
                case FURIA_LINKTYPE_7:
                    *speed = SPEED_40G;
                    *intf = phymodInterfaceKR2;
                break;
                case FURIA_LINKTYPE_8:
                    *speed = SPEED_25G;
                    *intf = phymodInterfaceKR;
                break;
                case FURIA_LINKTYPE_9:
                    *speed = SPEED_50G;
                    *intf = phymodInterfaceKR2;
                break;
                default:
                break;

           }
        break;
        case FURIA_PHYTYPE_FP:
            switch(link_type) {
                case FURIA_LINKTYPE_1:
                    switch (saved_intf) {
                        case 0:
                            *intf = phymodInterfaceLR;
                        break;
                        case 1:
                            *intf = phymodInterfaceSR;
                        break;
                        case 2:
                            *intf = phymodInterfaceER;
                        break;
                    }
                    if (*ref_clk == phymodRefClk125Mhz) {
                        *speed = ( mode_11p25g == 1)? SPEED_11P25G : SPEED_11P5G;
                    }else if (*ref_clk == phymodRefClk106Mhz) {
                        *speed = ( mode_FC8 == 1 )? SPEED_8P5G : SPEED_14P025G ;
                        *intf = phymodInterfaceSFI;
                  } else {
                        *speed = SPEED_10G;
                    }
                break;
                case FURIA_LINKTYPE_2:
                    *speed = SPEED_40G;
                    *intf = phymodInterfaceCR4;
                break;
                case FURIA_LINKTYPE_3:
                    switch (saved_intf) {
                        case 0:
                            *intf = phymodInterfaceLR4;
                        break;
                        case 1:
                            *intf = phymodInterfaceSR4;
                        break;
                        case 2:
                            *intf = phymodInterfaceER4;
                        break;
                    }
                    *speed = SPEED_40G;
                break;
                case FURIA_LINKTYPE_4:
                    *speed = SPEED_100G;
                    switch (saved_intf) {
                        case 0:
                            *intf = phymodInterfaceLR4;
                        break;
                        case 1:
                            *intf = phymodInterfaceSR4;
                        break;
                        case 2:
                            *intf = phymodInterfaceER4;
                        break;
                    }
                break;
                case FURIA_LINKTYPE_5:
                    if (saved_intf != FURIA_IF_CR4) {
                        *speed = SPEED_25G;
                        *intf = phymodInterfaceCR;
                    } else {
                        *speed = SPEED_100G;
                        *intf = phymodInterfaceCR4;
                    }
                    if (saved_intf == FURIA_IF_CR2) {
                        *speed = SPEED_50G;
                        *intf = phymodInterfaceCR2;
                    }
                break;
                case FURIA_LINKTYPE_6:
                    if (*ref_clk == phymodRefClk125Mhz) {
                        *speed = SPEED_23G;
                    } else { 
                        *speed = SPEED_20G;
                    }
                    switch (saved_intf) {
                        case 0:
                            *intf = phymodInterfaceLR;
                        break;
                        case 1:
                            *intf = phymodInterfaceSR;
                        break;
                        case 2:
                            *intf = phymodInterfaceER;
                        break;
                    }
                break;
                case FURIA_LINKTYPE_7:/*50G SR2/LR2/ER2*/
                *speed = (saved_lane_map == 0xc || saved_lane_map==0x3) ? SPEED_50G : SPEED_25G;
                     switch (saved_intf) {
                        case 0:
                            *intf = (saved_lane_map == 0xc || saved_lane_map==0x3) ? phymodInterfaceLR2: phymodInterfaceLR;
                        break;
                        case 1:
                            *intf = (saved_lane_map == 0xc || saved_lane_map==0x3) ?  phymodInterfaceSR2 : phymodInterfaceSR;
                        break;
                        case 2:
                            *intf = (saved_lane_map == 0xc || saved_lane_map==0x3) ? phymodInterfaceER2 : phymodInterfaceER;
                        break;
                    }
                break;
          case FURIA_LINKTYPE_C:
                        *speed = (saved_lane_map == 0xc || saved_lane_map==0x3) ? SPEED_50G :SPEED_25G;
                        *intf =  (saved_lane_map == 0xc || saved_lane_map==0x3) ? phymodInterfaceCR2:phymodInterfaceCR;
                break;
        case FURIA_LINKTYPE_D: /*mixed mode*/
              if (*ref_clk == phymodRefClk125Mhz) {
                        *speed = ( mode_11p25g == 1)? SPEED_11P25G : SPEED_11P5G;
                    }else if (saved_refclk == FURIA_CLK_159) {
                       /*speed = SPEED_10P518G;*//*FC10*/
           *speed =(nototn_mode_pll_140==1)?((saved_intf1e2e==0)? SPEED_44P57G : SPEED_44P583G) : SPEED_10P518G ;
           otn_type=(nototn_mode_pll_140==1)?((saved_intf1e2e==0)? phymodOTNOTU3 : phymodOTNOTU3e2) : otn_type ;
                    }else if (*ref_clk == phymodRefClk161Mhz) {
                        *speed = ( is_OTU2 == 1 )? SPEED_10P7G : SPEED_10G ;
           otn_type = ( is_OTU2 == 1 )? (phymodOTNOTU2) : otn_type ;
                    }else if (*ref_clk == phymodRefClk168Mhz || *ref_clk == phymodRefClk174Mhz ) {
                        *speed = (nonotn_mode2 == 1)? SPEED_10P7G : SPEED_11P18G;
           otn_type = ( nonotn_mode2 ==1 )? phymodOTNOTU2 : phymodOTNOTU4p10 ;
                    }else if (*ref_clk == phymodRefClk106Mhz) { /*FC32*/
                        *speed = (( mode_FC8 == 1 )? SPEED_8P5G : SPEED_14P025G) ;
           *interface_modes   =   PHYMOD_INTF_MODES_FIBER  ;
                    } else {
           switch(saved_intf1e2e){/*158 Mhz*/
           case 0:
               *speed = SPEED_11G;
               otn_type =  (phymodOTNOTU1e);/*or 1e */

               break;
           case 1:
               *speed = SPEED_11G;
               otn_type =  (phymodOTNOTU2e);

               break;
           default:
               *speed = SPEED_10G;
               break;
           }
                        *speed = ( mode_oc192 == 1)?  SPEED_9P95G : *speed ;
           *ref_clk = ( mode_oc192 == 1)?  phymodRefClk156Mhz : *ref_clk ;
           *interface_modes   =  ( mode_oc192 == 1)? PHYMOD_INTF_MODES_FIBER : *interface_modes ;
                    }
                    if (saved_intf == FURIA_IF_CX) {
                         *intf = phymodInterfaceCX;
                    } else if (saved_intf == FURIA_IF_CR) {
                        *intf = phymodInterfaceCR;
                    } else if (saved_intf == FURIA_IF_SFI) {
                        *intf = phymodInterfaceSFI;
                    }
                     break;
        case FURIA_LINKTYPE_E: /*mixed mode. ok !*/
                     if (saved_intf == FURIA_IF_XFI) {
                         if (*ref_clk == phymodRefClk125Mhz) {
                             *speed = ( mode_11p25g == 1)? SPEED_11P25G : SPEED_11P5G;
                         } else {
                             *speed = SPEED_10G;
                         }
                         *intf = phymodInterfaceXFI;
                     } else if (saved_intf == FURIA_IF_XLAUI) {
                         if( nototn_mode_pll_140 && !higig_mode){
                             *speed =(saved_refclk == FURIA_CLK_159) ? ((saved_intf1e2e==0)? SPEED_44P57G : SPEED_44P583G) : SPEED_43G;
                             otn_type =  (saved_intf1e2e==0) ? phymodOTNOTU3 : phymodOTNOTU3e2 ;
                         }else{
                             if (*ref_clk == phymodRefClk168Mhz || *ref_clk == phymodRefClk161Mhz) {
                                 *speed = SPEED_43G;
                                 otn_type =  (phymodOTNOTU3);
                             }else{
                                 *speed = SPEED_40G;
                             }
                         }
                         *intf = phymodInterfaceXLAUI;
                     }
                     break;

        case FURIA_LINKTYPE_F:/*mixed mode. ok !*/
                     if (*ref_clk == phymodRefClk174Mhz || *ref_clk == phymodRefClk168Mhz) {
                         *speed = SPEED_111G;
                         otn_type =  (phymodOTNOTU4);
                     }else if (*ref_clk == phymodRefClk106Mhz) { /*FC32*/
                         *speed = ( SPEED_28P05G );
                         /*  *interface_modes   =   PHYMOD_INTF_MODES_FIBER  ;*/
                     } else if( lane_map != 0xf){
                         *speed = SPEED_25G;
                     }else{
                         *speed = SPEED_100G;
                     }

                     if (saved_intf == FURIA_IF_CC) { 
                         *intf = phymodInterfaceCAUI4_C2C;
                     } else if (saved_intf == FURIA_IF_CM) {
                         *intf = phymodInterfaceCAUI4_C2M;
                     } else if(saved_intf == FURIA_IF_CAUI4) {
                         *intf = phymodInterfaceCAUI4;
                     } else if(saved_intf == FURIA_IF_CAUI) {
                         *intf = phymodInterfaceCAUI;
                     }
                default:
                break;
            }
            break;
        case FURIA_PHYTYPE_FC:
            *intf = phymodInterfaceSR;
            *interface_modes   =  PHYMOD_INTF_MODES_FIBER;
            switch (link_type) {
                case FURIA_LINKTYPE_2:
                    *speed = SPEED_8P5G;
                    break;
                case FURIA_LINKTYPE_3:
                    *speed = (saved_refclk == FURIA_CLK_159) ? SPEED_10P518G : SPEED_14P025G;
                    break;
                case FURIA_LINKTYPE_4:
                        *speed = SPEED_28P05G;
                        break;
                        default:
                        break;
                    }
            break;
        case FURIA_PHYTYPE_OTN:
             *intf = phymodInterfaceOTN;
            switch (link_type) {
                case FURIA_LINKTYPE_1:
                    *speed = SPEED_11G;
                    switch (saved_intf1e2e) {
                         case 0:
                             otn_type =  (phymodOTNOTU1e);/*or 1e */
                             *ref_clk = phymodRefClk157Mhz;
                             break;
                         case 1:
                             otn_type =  (phymodOTNOTU2e);/*or 2e */
                             *ref_clk = phymodRefClk158Mhz;
                             break;
                         default:
                             otn_type =  (phymodOTNOTU2e);/*or 2e */
                             *ref_clk = phymodRefClk158Mhz;
                         break;
                     }

                    break;
                case FURIA_LINKTYPE_2:
                    switch (saved_intf1e2e) {
                         case 0:
                             otn_type =  (phymodOTNOTU3);
                             *speed = SPEED_44P57G;
                             break;
                         case 1:
                             otn_type =  (phymodOTNOTU3e2);
                             *speed = SPEED_44P583G;
                             break;
                         default:
                             break;
                    }
                    *speed = ( saved_refclk == FURIA_CLK_159 ) ? *speed : SPEED_43G;
                    otn_type = ( saved_refclk == FURIA_CLK_159 ) ? otn_type : (phymodOTNOTU3);/*OTN3.4 */
                    break;
                case FURIA_LINKTYPE_3:
                    *speed = SPEED_111G;/*OTU 4.4*/
                    otn_type =  (phymodOTNOTU4);/*OTN4.4 */
                    break;
                case FURIA_LINKTYPE_4:
                    *speed = SPEED_100G;
                    break;
                default:
                break;
            }
        break;

        case FURIA_PHYTYPE_PROP:
            switch(link_type) {
               case FURIA_LINKTYPE_5:
                    *speed = SPEED_6G;
                    *intf = phymodInterfaceSR;
                    break;
                case FURIA_LINKTYPE_3:
                    *speed = SPEED_25G;
                    *intf = phymodInterfaceVSR;
                    break;
                default:
                break;
            }
        default:
        break;
    } 

    switch(saved_refclk){
        case FURIA_CLK_125:
            *ref_clk = phymodRefClk125Mhz;
            break;
        case FURIA_CLK_106:
            *ref_clk = phymodRefClk106Mhz;
            break;
        case FURIA_CLK_157:
            *ref_clk = phymodRefClk157Mhz;
            break;
        case FURIA_CLK_158:
            *ref_clk = phymodRefClk158Mhz;
            break;
        case FURIA_CLK_161:
            *ref_clk = phymodRefClk161Mhz;
            break;
        case FURIA_CLK_168:
            *ref_clk = phymodRefClk168Mhz;
            break;
        case FURIA_CLK_174:
            *ref_clk = phymodRefClk174Mhz;
            break;
        case FURIA_CLK_159:
            *ref_clk = phymodRefClk159Mhz;
            break;
        default:
            if(!( mode_FC10 || mode_FC32)){
                *ref_clk = phymodRefClk156Mhz;
            }
            break;
    }


    if (pkg_side == SIDE_B) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_gpreg13_Adr,\
                                    &gen_ctrl_gpreg13.data));
        switch (gen_ctrl_gpreg13.fields.phy_type_sys) {
            case 0:
                switch (*speed) {
                    case SPEED_10G:
                    case SPEED_11G:
                    case SPEED_11P25G:
                    case SPEED_20G:
                    case SPEED_23G:
                    case SPEED_25G:
                    case SPEED_50G:
                    case SPEED_12P5G:
                    if (*ref_clk == phymodRefClk168Mhz) {
                        *intf = phymodInterfaceXFI;
                    }else {
                         *intf = phymodInterfaceKR;
                    break;
                    case SPEED_40G:
                    case SPEED_43G:
                    case SPEED_100G:
                    case SPEED_111G:
                        *intf = phymodInterfaceKR4;
                        if (*ref_clk == phymodRefClk174Mhz) {
                            *intf = phymodInterfaceOTN;
                        }else if (*ref_clk == phymodRefClk168Mhz) {
                            *intf = phymodInterfaceCAUI4_C2C;
                        }
                        break;
                    }
                    break;
            case 1:
                switch (*speed) {
                    case SPEED_10G:
                    case SPEED_11G:
                    case SPEED_11P25G:
                    case SPEED_10P7G:
                    case SPEED_11P18G:
                    case SPEED_8P5G:
                    case SPEED_14P025G:
                    case SPEED_9P95G:
                    case SPEED_10P518G:/*FC10*/
                    case SPEED_12P5G:
                        *intf = phymodInterfaceXFI;
                    break;
                    case SPEED_23G:
                    case SPEED_25G:
                        *intf = phymodInterfaceCAUI;
                    break;
                    case SPEED_40G:
                    case SPEED_42G:
                    case SPEED_43G:
                    case SPEED_44P57G:
                    case SPEED_44P583G:
                        *intf = phymodInterfaceXLAUI;
                    break;
                    case SPEED_28P05G:
                    case SPEED_100G:
                    case SPEED_106G:
                    case SPEED_111G:
                        *intf = phymodInterfaceCAUI4_C2C;
                    break;
                }
            break;
        }
    }
    }


   switch (curr_pll_mode[0] & curr_pll_mode[1]) {
        case PLL_MODE_175:
        case PLL_MODE_140:
            if(!( *speed == SPEED_43G || *speed == SPEED_44P57G || *speed == SPEED_44P583G ||( *ref_clk == phymodRefClk157Mhz) || (  *ref_clk == phymodRefClk158Mhz)) ){
                *interface_modes   =  1;
       }else if( higig_mode){
                *interface_modes   =  1;
       }
        break;
        default:
        break;
    }

   if (config->device_aux_modes != NULL) {
         ((FURIA_DEVICE_OTN_MODE_T*) config->device_aux_modes)->otn_type = otn_type ;
     }

    return PHYMOD_E_NONE;
}


/**   Config PLL divider 
 *    This function is used to configure PLL divider 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param pll_mode           PLL mode  
 *    @param ref_clk_freq       Ref clk freq type 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_config_pll_div(const phymod_access_t *pa, uint16_t pll_mode)
{
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    int side_index = 0;
   for(side_index = 0; side_index < 2; side_index ++) {
        /* Set PLL div */
        wr_lane = 1 << 0;
        rd_lane = 0;
        if(side_index == 0) {
            sys_en = SIDE_A;
        } else {
            sys_en = SIDE_B;
        } 
        /* Program the slice register */
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

    falcon_furia_core_pwrdn(pa,PWRDN);
        /* Program pll_mode */
    falcon_furia_configure_pll(pa,pll_mode);
    falcon_furia_core_pwrdn(pa,PWR_ON);
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE; 
}

/**   Set Tx Rx polarity 
 *    This function is used to set Tx Rx polarity of a single lane
 *    or multiple lanes 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param tx_polarity        Tx polarity 
 *    @param rx_polarity        Rx polarity 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_tx_rx_polarity_set (const phymod_access_t *pa, uint32_t tx_polarity, uint32_t rx_polarity)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    TLB_RX_TLB_RX_MISC_CONFIG_t rx_misc_config;
    TLB_TX_TLB_TX_MISC_CONFIG_t tx_misc_config;
    PHYMOD_MEMSET(&rx_misc_config, 0 , sizeof(TLB_RX_TLB_RX_MISC_CONFIG_t));
    PHYMOD_MEMSET(&tx_misc_config, 0 , sizeof(TLB_TX_TLB_TX_MISC_CONFIG_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : tx_pkg_ln_des->slice_wr_val;
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA;
                } else { 
                    sys_en = tx_pkg_ln_des->sideB;
                }

                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

                /* Read Tx misc config register */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            TLB_TX_tlb_tx_misc_config_Adr,\
                                            &tx_misc_config.data));

                /* Update bit fields for polarity inversion */
                tx_misc_config.fields.tx_pmd_dp_invert = tx_polarity;
                
                /* Write to Tx misc config register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             TLB_TX_tlb_tx_misc_config_Adr,\
                                             tx_misc_config.data));
            } 
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : rx_pkg_ln_des->slice_wr_val;
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA;
                } else { 
                    sys_en = rx_pkg_ln_des->sideB;
                }

                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                /* Read Rx misc config register */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            TLB_RX_tlb_rx_misc_config_Adr,\
                                            &rx_misc_config.data));

                /* Update bit fields for polarity inversion */
                rx_misc_config.fields.rx_pmd_dp_invert = rx_polarity;
             
                /* Write to Rx misc config register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             TLB_RX_tlb_rx_misc_config_Adr,\
                                             rx_misc_config.data));
            } 
            if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                break;
            }
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));

    return PHYMOD_E_NONE;
}

/**   Get Tx Rx polarity 
 *    This function is used to get Tx Rx polarity of a specific lane
 *    specified by user 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param tx_polarity        Tx polarity 
 *    @param rx_polarity        Rx polarity 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_tx_rx_polarity_get (const phymod_access_t *pa, uint32_t *tx_polarity, uint32_t *rx_polarity)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    TLB_RX_TLB_RX_MISC_CONFIG_t rx_misc_config;
    TLB_TX_TLB_TX_MISC_CONFIG_t tx_misc_config;
    PHYMOD_MEMSET(&rx_misc_config, 0 , sizeof(TLB_RX_TLB_RX_MISC_CONFIG_t));
    PHYMOD_MEMSET(&tx_misc_config, 0 , sizeof(TLB_TX_TLB_TX_MISC_CONFIG_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    *tx_polarity = 0;
    *rx_polarity = 0;
   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                wr_lane = tx_pkg_ln_des->slice_wr_val;
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA;
                } else { 
                    sys_en = tx_pkg_ln_des->sideB;
                }

                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                /* Read Tx misc config register */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            TLB_TX_tlb_tx_misc_config_Adr,\
                                            &tx_misc_config.data));

                /* Update bit fields for polarity inversion */
                *tx_polarity = tx_misc_config.fields.tx_pmd_dp_invert ;
            } 
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                wr_lane = rx_pkg_ln_des->slice_wr_val;
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA;
                } else { 
                    sys_en = rx_pkg_ln_des->sideB;
                }

                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                /* Read Rx misc config register */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            TLB_RX_tlb_rx_misc_config_Adr,\
                                            &rx_misc_config.data));

                /* Update bit fields for polarity inversion */
                *rx_polarity = rx_misc_config.fields.rx_pmd_dp_invert;
            } 
            break;
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));

    return PHYMOD_E_NONE;
}

/**  PMD lock get 
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param rx_seq_done        RX sew done status
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_pmd_lock_get(const phymod_access_t *pa, uint32_t *rx_seq_done) 
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint8_t rx_pmd_lock = 1;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    *rx_seq_done = 1;
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_pmd_lock_status(pa, &rx_pmd_lock));
                *rx_seq_done &= rx_pmd_lock;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


/**  Tx Rx power get 
 *    
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param power_tx           TX Power 
 *    @param power_rx           RX Power 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_tx_rx_power_get(const phymod_access_t *pa, uint32_t *power_tx, uint32_t *power_rx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t rst_pwdn_ctrl;
    uint32_t acc_flags = 0; 
    *power_tx = 0;
    *power_rx = 0;
    PHYMOD_MEMSET(&rst_pwdn_ctrl, 0 , sizeof(CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                wr_lane = rx_pkg_ln_des->slice_wr_val;
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA;
                } else { 
                    sys_en = rx_pkg_ln_des->sideB;
                }

                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                            &rst_pwdn_ctrl.data));
                *power_rx = rst_pwdn_ctrl.fields.ln_rx_s_pwrdn ? 0 : 1;
            } 
            if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) { 
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                wr_lane = tx_pkg_ln_des->slice_wr_val;
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA;
                } else { 
                    sys_en = tx_pkg_ln_des->sideB;
                }

                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                            &rst_pwdn_ctrl.data));
                *power_tx = rst_pwdn_ctrl.fields.ln_tx_s_pwrdn ? 0 : 1 ;
            } 
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


/**  Rx Power Set 
 *    
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param tx_rx              TX_RX Value
 *    @param enable             Set 1 to enable 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_tx_rx_power_set(const phymod_access_t *pa, uint8_t tx_rx, uint32_t enable) 
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t rst_pwdn_ctrl;
    uint32_t acc_flags = 0; 
    PHYMOD_MEMSET(&rst_pwdn_ctrl, 0 , sizeof(CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    enable = enable ? 0 : 1;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : tx_pkg_ln_des->slice_wr_val;
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA;
                } else { 
                    sys_en = tx_pkg_ln_des->sideB;
                }

                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                /* power down Tx*/
                if(!tx_rx) { 
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                                &rst_pwdn_ctrl.data));
                    rst_pwdn_ctrl.fields.ln_tx_s_pwrdn = enable;

                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                                 rst_pwdn_ctrl.data));
                }
            } 
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : rx_pkg_ln_des->slice_wr_val;
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA;
                } else { 
                    sys_en = rx_pkg_ln_des->sideB;
                }

                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                if(tx_rx) {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                                &rst_pwdn_ctrl.data));
                    rst_pwdn_ctrl.fields.ln_rx_s_pwrdn = enable;

                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                                 rst_pwdn_ctrl.data));
                }
            } 
            if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                break;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}
int furia_loopback_set(const phymod_access_t *pa,
                       phymod_loopback_mode_t loopback,
                       uint32_t enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int other_pkg_side = 0;
    int other_side_wr_lane = 0;
    int other_side_sys_en = 0;
    int rd_lane = 0;
    int other_side_rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* other_side_pkg_ln_des = NULL;
    uint32_t acc_flags = 0;
    int retry_count = 40;
    DSC_C_RX_PI_CONTROL_t pi_ctrl; 
    DSC_C_CDR_CONTROL_2_t cd_ctrl_2;
    TX_PI_RPTR_TX_PI_CONTROL_0_t pi_ctrl_0;
    TX_PI_RPTR_TX_PI_CONTROL_5_t pi_ctrl_5;
    TLB_RX_DIG_LPBK_CONFIG_t dig_lpbk_cfg;
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable;
    phymod_access_t l_pa;
    PHYMOD_MEMSET(&pi_ctrl, 0 , sizeof(DSC_C_RX_PI_CONTROL_t));
    PHYMOD_MEMSET(&cd_ctrl_2, 0 , sizeof(DSC_C_CDR_CONTROL_2_t));
    PHYMOD_MEMSET(&pi_ctrl_0, 0 , sizeof(TX_PI_RPTR_TX_PI_CONTROL_0_t));
    PHYMOD_MEMSET(&pi_ctrl_5, 0 , sizeof(TX_PI_RPTR_TX_PI_CONTROL_5_t));
    PHYMOD_MEMSET(&dig_lpbk_cfg, 0 , sizeof(TLB_RX_DIG_LPBK_CONFIG_t));
    PHYMOD_MEMSET(&fw_enable, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));
    PHYMOD_MEMCPY(&l_pa, pa, sizeof(phymod_access_t));
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            other_pkg_side = (pkg_side == SIDE_A) ? SIDE_B : SIDE_A;
            l_pa.flags &= ~(1 << 31);
            if (other_pkg_side == SIDE_B) {
                l_pa.flags |= (1 << 31);
            } 
            other_side_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, &l_pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(other_side_pkg_ln_des);
            if(lane_map == 0xF) {
                other_side_wr_lane = BROADCAST;
            } else {
                other_side_wr_lane = other_side_pkg_ln_des->slice_wr_val;
            }
            other_side_rd_lane = other_side_pkg_ln_des->slice_rd_val;
            if(other_pkg_side == SIDE_A) {
                other_side_sys_en = other_side_pkg_ln_des->sideA;
            } else {
                other_side_sys_en = other_side_pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            switch (loopback) {
                case phymodLoopbackRemotePMD:
                    if(enable) {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_rmt_lpbk_from_nl(pa));
                    } else {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_nl_from_rmt_lpbk(pa, RMT_LPBK_SIDE));
                        PHYMOD_IF_ERR_RETURN(wr_tx_pi_loop_timing_src_sel(1));
                    }
                break;
                case phymodLoopbackRemotePCS:
                    return PHYMOD_E_UNAVAIL;
                break;
                case phymodLoopbackGlobal:
                case phymodLoopbackGlobalPMD:
                    if(enable) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));

                        while(fw_enable.fields.fw_enable_val != 0 && retry_count){
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                        &fw_enable.data));
                            PHYMOD_USLEEP(200);
                            retry_count--;
                        } 
                        if(!retry_count) {
                            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Digital loopback configuration failed...")));
                        }  
                        
                        /* Enable digital loopback */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_dig_lpbk_config_Adr,\
                                                    &dig_lpbk_cfg.data));
                        dig_lpbk_cfg.fields.dig_lpbk_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_dig_lpbk_config_Adr,\
                                                     dig_lpbk_cfg.data));
                        /* Set firmware enable */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));
                        fw_enable.fields.fw_enable_val = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                     fw_enable.data));
                        retry_count = 40; 
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));
                        while(fw_enable.fields.fw_enable_val != 0 && retry_count){
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                        &fw_enable.data));
                            PHYMOD_USLEEP(200);
                            retry_count--;
                        } 
                        if(!retry_count) {
                            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Digital loopback configuration failed...")));
                        }  
                        /* Enable digital loopback */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_dig_lpbk_config_Adr,\
                                                    &dig_lpbk_cfg.data));
                        dig_lpbk_cfg.fields.dig_lpbk_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_dig_lpbk_config_Adr,\
                                                     dig_lpbk_cfg.data));
                    } else {
                        retry_count = 40; 
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));
                        while(fw_enable.fields.fw_enable_val != 0 && retry_count){
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                        &fw_enable.data));

                            PHYMOD_USLEEP(200);
                            retry_count--;
                        } 
                        if(!retry_count) {
                            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Digital loopback configuration failed...")));
                        }  
                        /* Program Other Falcon (one which is not doing digital loopback) */
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, other_side_sys_en, other_side_wr_lane, other_side_rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_C_cdr_control_2_Adr,\
                                                    &cd_ctrl_2.data));
                        cd_ctrl_2.fields.tx_pi_loop_timing_src_sel = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     DSC_C_cdr_control_2_Adr,\
                                                     cd_ctrl_2.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                    &pi_ctrl_5.data));
                        pi_ctrl_5.fields.tx_pi_repeater_mode_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                     pi_ctrl_5.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 0;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));
                        /* Program Falcon (one which is doing digital loopback) */
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                    &pi_ctrl_5.data));
                        pi_ctrl_5.fields.tx_pi_repeater_mode_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                     pi_ctrl_5.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 0;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_dig_lpbk_config_Adr,\
                                                    &dig_lpbk_cfg.data));
                        dig_lpbk_cfg.fields.dig_lpbk_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_dig_lpbk_config_Adr,\
                                                     dig_lpbk_cfg.data));
                        /* Set firmware enable */
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                    &fw_enable.data));
                        fw_enable.fields.fw_enable_val = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                                     fw_enable.data));
                    }
                PHYMOD_IF_ERR_RETURN(wr_falcon_furia_sdk_tx_disable(enable));
        break;
                default :
                break;
            }

        }
        if(lane_map == 0xF) {
            break;
        }
    }


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(lane_map == 0xF) {
                wr_lane = BROADCAST;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
            }
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            other_pkg_side = (pkg_side == SIDE_A) ? SIDE_B : SIDE_A;
            l_pa.flags &= ~(1 << 31);
            if (other_pkg_side == SIDE_B) {
                l_pa.flags |= (1 << 31);
            } 
            other_side_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, &l_pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(other_side_pkg_ln_des);
            if(lane_map == 0xF) {
                other_side_wr_lane = BROADCAST;
            } else {
                other_side_wr_lane = other_side_pkg_ln_des->slice_wr_val;
            }
            other_side_rd_lane = other_side_pkg_ln_des->slice_rd_val;
            if(other_pkg_side == SIDE_A) {
                other_side_sys_en = other_side_pkg_ln_des->sideA;
            } else {
                other_side_sys_en = other_side_pkg_ln_des->sideB;
            }

            switch (loopback) {
                case phymodLoopbackRemotePMD:
                    if(enable) {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_rmt_lpbk_from_nl(pa));
                    } else {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_nl_from_rmt_lpbk(pa, RMT_LPBK_SIDE));
                        PHYMOD_IF_ERR_RETURN(wr_tx_pi_loop_timing_src_sel(1));
                    }
                break;
                case phymodLoopbackRemotePCS:
                    return PHYMOD_E_UNAVAIL;
                break;
                case phymodLoopbackGlobal:
                case phymodLoopbackGlobalPMD:
                    if(enable) {
                        /* Program Other Falcon (one which is not doing digital loopback) */
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, other_side_sys_en, other_side_wr_lane, other_side_rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_C_rx_pi_control_Adr,\
                                                    &pi_ctrl.data));
                        pi_ctrl.fields.rx_pi_manual_mode = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     DSC_C_rx_pi_control_Adr,\
                                                     pi_ctrl.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_C_cdr_control_2_Adr,\
                                                    &cd_ctrl_2.data));
                        cd_ctrl_2.fields.tx_pi_loop_timing_src_sel = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     DSC_C_cdr_control_2_Adr,\
                                                     cd_ctrl_2.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 0;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));

                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                    &pi_ctrl_5.data));
                        pi_ctrl_5.fields.tx_pi_repeater_mode_en = 0;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                     pi_ctrl_5.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 1;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));

                        /* Program Falcon (one which is doing digital loopback) */
                        /* Program the slice register */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                    &pi_ctrl_5.data));
                        pi_ctrl_5.fields.tx_pi_repeater_mode_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_5_Adr,\
                                                     pi_ctrl_5.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                    &pi_ctrl_0.data));
                        pi_ctrl_0.fields.tx_pi_en = 1;
                        pi_ctrl_0.fields.tx_pi_jitter_filter_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TX_PI_RPTR_tx_pi_control_0_Adr,\
                                                     pi_ctrl_0.data));

                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TLB_RX_dig_lpbk_config_Adr,\
                                                    &dig_lpbk_cfg.data));
                        dig_lpbk_cfg.fields.dig_lpbk_en = 1;

                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     TLB_RX_dig_lpbk_config_Adr,\
                                                     dig_lpbk_cfg.data));
                        PHYMOD_USLEEP(50);
                    }
                break;
                default :
                break;
            }

        }
        if(lane_map == 0xF) {
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_loopback_get(const phymod_access_t *pa,
                       phymod_loopback_mode_t loopback,
                       uint32_t* enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint8_t ena_dis = 0;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
  
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                switch (loopback) {
                    case phymodLoopbackRemotePMD :
                        PHYMOD_IF_ERR_RETURN(falcon_furia_rmt_lpbk_get(pa, &ena_dis));
                        *enable = ena_dis;
                    break;
                    case phymodLoopbackRemotePCS :
                        return PHYMOD_E_UNAVAIL;
                    break;
                    case phymodLoopbackGlobal:
                    case phymodLoopbackGlobalPMD:
                        PHYMOD_IF_ERR_RETURN(falcon_furia_dig_lpbk_get(pa, &ena_dis));
                        *enable = ena_dis;
                    break;
                    default :
                    break;
                }
            break;
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_reset_set(const phymod_access_t *pa,
                    phymod_reset_mode_t reset_mode,
                    phymod_reset_direction_t direction)
{
    if((reset_mode == phymodResetModeSoft) &&
       (direction == phymodResetDirectionIn)) { 
        PHYMOD_IF_ERR_RETURN
            (_furia_chip_reset(pa));
    }
    return PHYMOD_E_NONE;
}

int furia_tx_set(const phymod_access_t *pa,
                 const phymod_tx_t* tx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t acc_flags = 0;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(pa, TX_AFE_PRE, (int8_t)tx->pre));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(pa, TX_AFE_MAIN, (int8_t)tx->main));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(pa, TX_AFE_POST1, (int8_t)tx->post));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(pa, TX_AFE_POST2, (int8_t)tx->post2));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(pa, TX_AFE_POST3, (int8_t)tx->post3));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_write_tx_afe(pa, TX_AFE_AMP, (int8_t)tx->amp));
        }
        if(FURIA_IF_BCAST_SET(chip_id, lane_map)) {
            break;
        }
    }

    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_tx_get(const phymod_access_t *pa,
                 phymod_tx_t* tx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    int8_t value = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
  
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_read_tx_afe(pa, TX_AFE_PRE, &value));
                tx->pre = value;
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_read_tx_afe(pa, TX_AFE_MAIN, &value));
                tx->main = value;
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_read_tx_afe(pa, TX_AFE_POST1, &value));
                tx->post = value;
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_read_tx_afe(pa, TX_AFE_POST2, &value));
                tx->post2 = value;
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_read_tx_afe(pa, TX_AFE_POST3, &value));
                tx->post3 = value;
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_read_tx_afe(pa, TX_AFE_AMP, &value));
                tx->amp = value;
            } 
            break;
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}
int furia_rx_set(const phymod_access_t *pa,
                 const phymod_rx_t* rx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t acc_flags = 0; 
    uint32_t i = 0;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            /*params check*/
            for (i = 0 ; i < PHYMOD_NUM_DFE_TAPS; i++){
                if(rx->dfe[i].enable && (rx->num_of_dfe_taps > PHYMOD_NUM_DFE_TAPS)) {
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set")));
                }
            }
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                /* first stop the rx adaption */
                PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(pa, 1));
                /*vga set*/
                if (rx->vga.enable) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_write_rx_afe(pa, RX_AFE_VGA, rx->vga.value));
                } 
                /*dfe set*/
                for (i = 0 ; i < PHYMOD_NUM_DFE_TAPS; i++){
                    if(rx->dfe[i].enable){
                        PHYMOD_IF_ERR_RETURN(falcon_furia_write_rx_afe(pa, RX_AFE_DFE1+i, rx->dfe[i].value));
                    } 
                }
                /*peaking filter set*/
                if(rx->peaking_filter.enable){
                    PHYMOD_IF_ERR_RETURN(falcon_furia_write_rx_afe(pa, RX_AFE_PF, rx->peaking_filter.value));
                } 
                if(rx->low_freq_peaking_filter.enable){
                    PHYMOD_IF_ERR_RETURN(falcon_furia_write_rx_afe(pa, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
                } 
            } 
            if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                break;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


int furia_rx_get(const phymod_access_t *pa,
                 phymod_rx_t* rx)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t i = 0;
    int8_t dfe;
    int8_t vga;
    int8_t pf;
    int8_t low_freq_pf;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
  
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                rx->num_of_dfe_taps = PHYMOD_NUM_DFE_TAPS; 
                PHYMOD_IF_ERR_RETURN(falcon_furia_read_rx_afe(pa, RX_AFE_VGA, &vga));
                rx->vga.value = vga;

                /*dfe get*/
                for (i = 0 ; i < PHYMOD_NUM_DFE_TAPS; i++){
                    PHYMOD_IF_ERR_RETURN(falcon_furia_read_rx_afe(pa, (RX_AFE_DFE1+i), &dfe));
                    rx->dfe[i].value = dfe;
                }

                /*peaking filter get*/
                PHYMOD_IF_ERR_RETURN(falcon_furia_read_rx_afe(pa, RX_AFE_PF, &pf));
                rx->peaking_filter.value = pf;
                PHYMOD_IF_ERR_RETURN(falcon_furia_read_rx_afe(pa, RX_AFE_PF2, &low_freq_pf));
                rx->low_freq_peaking_filter.value = low_freq_pf;
            }
            break;
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_slice_reg_set(const phymod_access_t *pa)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            /* Program the slice register, by checking the device id */
            if ((pa->devad & FURIA_DEVICE_ID_MASK) == FURIA_PMD_DEVICE_ID) { 
                PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            } else if ((pa->devad & FURIA_DEVICE_ID_MASK) == FURIA_AN_DEVICE_ID) {
                PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            } else {
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Slice Reg Set"
                                " Request for Unknown Device Type...")));
                return PHYMOD_E_FAIL;
            }
            if (BROADCAST == wr_lane)
                return PHYMOD_E_NONE;
        }
    }

    return PHYMOD_E_NONE;
}

int furia_slice_reg_unset(const phymod_access_t *pa)
{
    if ((pa->devad & FURIA_DEVICE_ID_MASK) == FURIA_PMD_DEVICE_ID) {
        PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    } else if ((pa->devad & FURIA_DEVICE_ID_MASK) == FURIA_AN_DEVICE_ID) {
        PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));
    }
    return PHYMOD_E_NONE;

}

int furia_tx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_tx_lane_control_t tx_control)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    SYS_TX_PMA_DP_SOFT_RST_t sys_tx_dp_soft_rst;
    LINE_TX_PMA_DP_SOFT_RST_t line_tx_dp_soft_rst; 
    PHYMOD_MEMSET(&sys_tx_dp_soft_rst, 0 , sizeof(SYS_TX_PMA_DP_SOFT_RST_t));
    PHYMOD_MEMSET(&line_tx_dp_soft_rst, 0 , sizeof(LINE_TX_PMA_DP_SOFT_RST_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    switch(tx_control) {
        case phymodTxReset:
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
                    rd_lane = pkg_ln_des->slice_rd_val;
                    if(pkg_side == SIDE_A) {
                        sys_en = pkg_ln_des->sideA;
                    } else {
                        sys_en = pkg_ln_des->sideB;
                    }
                    /* Program the slice register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                    if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                        /* Asserting datapath reset */
                        if (sys_en == LINE) {
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        LINE_TX_PMA_DP_soft_rst_Adr,\
                                                        &line_tx_dp_soft_rst.data));
                            line_tx_dp_soft_rst.fields.rstb_frc = 1;
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                         LINE_TX_PMA_DP_soft_rst_Adr,\
                                                         line_tx_dp_soft_rst.data));
                            /* Releasing datapath reset */
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        LINE_TX_PMA_DP_soft_rst_Adr,\
                                                        &line_tx_dp_soft_rst.data));
                            line_tx_dp_soft_rst.fields.rstb = 1;
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                         LINE_TX_PMA_DP_soft_rst_Adr,\
                                                         line_tx_dp_soft_rst.data));
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        LINE_TX_PMA_DP_soft_rst_Adr,\
                                                        &line_tx_dp_soft_rst.data));
                            line_tx_dp_soft_rst.fields.rstb_frc = 0;
                                PHYMOD_IF_ERR_RETURN
                                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                             LINE_TX_PMA_DP_soft_rst_Adr,\
                                                             line_tx_dp_soft_rst.data));
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_TX_PMA_DP_soft_rst_Adr,\
                                                    &sys_tx_dp_soft_rst.data));
                        sys_tx_dp_soft_rst.fields.rstb_frc = 1;
                        PHYMOD_IF_ERR_RETURN
                           (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_TX_PMA_DP_soft_rst_Adr,\
                                                    sys_tx_dp_soft_rst.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_TX_PMA_DP_soft_rst_Adr,\
                                                    &sys_tx_dp_soft_rst.data));
                        sys_tx_dp_soft_rst.fields.rstb = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SYS_TX_PMA_DP_soft_rst_Adr,\
                                                     sys_tx_dp_soft_rst.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_TX_PMA_DP_soft_rst_Adr,\
                                                    &sys_tx_dp_soft_rst.data));
                        sys_tx_dp_soft_rst.fields.rstb_frc = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SYS_TX_PMA_DP_soft_rst_Adr,\
                                                     sys_tx_dp_soft_rst.data));
                    }
                }
                if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                    break;
                }
            }
        }
    break;
    case phymodTxTrafficDisable:
    case phymodTxTrafficEnable:
        return PHYMOD_E_UNAVAIL;
    break;
    case phymodTxSquelchOn:
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
                PHYMOD_NULL_CHECK(pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
                rd_lane = pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = pkg_ln_des->sideA;
                } else {
                    sys_en = pkg_ln_des->sideB;
                }
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_tx_disable(pa, 1));
                } 
                if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                    break;;
                }
            }
        }
        break;

        case phymodTxSquelchOff:
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
                PHYMOD_NULL_CHECK(pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
                rd_lane = pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = pkg_ln_des->sideA;
                } else {
                    sys_en = pkg_ln_des->sideB;
                }
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                    PHYMOD_IF_ERR_RETURN(falcon_furia_tx_disable(pa, 0)); 
                } 
                if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                    break;;
                }
            }
        }
        break;
        default:
        break; 
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_tx_lane_control_get(const phymod_access_t *pa,
                              phymod_phy_tx_lane_control_t* tx_control)
{
    int lane_map = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    int pkg_side = 0;
    int sys_en = 0;
    int wr_lane = 0;
    int rd_lane = 0;
    uint8_t tx_dis_rd_val = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    switch(*tx_control) {
        case phymodTxReset:
        case phymodTxTrafficDisable:
        case phymodTxTrafficEnable:
            return PHYMOD_E_UNAVAIL;
        case phymodTxSquelchOn:
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    rd_lane = pkg_ln_des->slice_rd_val;
                    wr_lane = pkg_ln_des->slice_wr_val;
                    if(pkg_side == SIDE_A) {
                        sys_en = pkg_ln_des->sideA;
                    } else {
                        sys_en = pkg_ln_des->sideB;
                    }
                    /* Program the slice register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                    if ((FURIA_IS_SIMPLEX(chip_id) && (pkg_side == SIDE_A)) ||
                        (FURIA_IS_DUPLEX(chip_id))) {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_tx_disable_get(pa, &tx_dis_rd_val));
                        *tx_control = tx_dis_rd_val ? phymodTxSquelchOn : phymodTxSquelchOff;
                    } 
                    break;
                }
            }
        break;
        case phymodTxSquelchOff:
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa,
lane_index, 0); 
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    if(pkg_side == SIDE_A) {
                        sys_en = pkg_ln_des->sideA;
                    } else {
                        sys_en = pkg_ln_des->sideB;
                    }
                    /* Program the slice register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                    if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                        PHYMOD_IF_ERR_RETURN(falcon_furia_tx_disable_get(pa, &tx_dis_rd_val));
                        *tx_control = (tx_dis_rd_val == 0) ? 1 : 0;
                    }
                    break;
                }
            }
        break;
        default:
        break; 
    }
    return PHYMOD_E_NONE;
}

int furia_rx_lane_control_set(const phymod_access_t *pa,
                              phymod_phy_rx_lane_control_t rx_control)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl;
    SIGDET_SIGDET_CTRL_1_t sigdet_ctrl; 
    SYS_RX_PMA_DP_main_ctrl_t sys_rx_dp_main_ctrl;
    PHYMOD_MEMSET(&line_rx_dp_main_ctrl, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&sys_rx_dp_main_ctrl, 0 , sizeof(SYS_RX_PMA_DP_main_ctrl_t));
    PHYMOD_MEMSET(&sigdet_ctrl, 0 , sizeof(SIGDET_SIGDET_CTRL_1_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    switch(rx_control) {
        case phymodRxReset:
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    /* Configure Tx side(SIDE_A) first*/
                    pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
                    rd_lane = pkg_ln_des->slice_rd_val;
                    if(pkg_side == SIDE_A) {
                        sys_en = pkg_ln_des->sideA;
                    } else {
                        sys_en = pkg_ln_des->sideB;
                    }
                    /* Program the slice register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                    if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                        if(sys_en == LINE) {
                            /* Program the slice register */
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                            /* Asserting datapath reset */
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                        &line_rx_dp_main_ctrl.data));
                            line_rx_dp_main_ctrl.fields.rstb_frc = 1;
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                         LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                         line_rx_dp_main_ctrl.data));
                            /* Releasing datapath reset */
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                        &line_rx_dp_main_ctrl.data));
                            line_rx_dp_main_ctrl.fields.rstb = 1;
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                         LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                         line_rx_dp_main_ctrl.data));
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                        &line_rx_dp_main_ctrl.data));
                            line_rx_dp_main_ctrl.fields.rstb_frc = 0;
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                         LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                         line_rx_dp_main_ctrl.data));
                        } else { 
                            /* Program the slice register */
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                        &sys_rx_dp_main_ctrl.data));
                            sys_rx_dp_main_ctrl.fields.rstb_frc = 1;
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                         SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                         sys_rx_dp_main_ctrl.data));
                            /* Releasing datapath reset */
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                        &sys_rx_dp_main_ctrl.data));
                            sys_rx_dp_main_ctrl.fields.rstb = 1;
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                         SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                         sys_rx_dp_main_ctrl.data));
                            PHYMOD_IF_ERR_RETURN
                                (READ_FURIA_PMA_PMD_REG(pa,\
                                                        SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                        &sys_rx_dp_main_ctrl.data));
                            sys_rx_dp_main_ctrl.fields.rstb_frc = 0;
                            PHYMOD_IF_ERR_RETURN
                                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                         SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                         sys_rx_dp_main_ctrl.data));
                        }
                    }
                    if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                        break;
                    } 
                }
            }
        break;
        case phymodRxSquelchOn:
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    /* Configure Tx side(SIDE_A) first*/
                    pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
                    rd_lane = pkg_ln_des->slice_rd_val;
                    if(pkg_side == SIDE_A) {
                        sys_en = pkg_ln_des->sideA;
                    } else {
                        sys_en = pkg_ln_des->sideB;
                    }
                    /* Program the slice register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                    if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SIGDET_SIGDET_CTRL_1_Adr,\
                                                    &sigdet_ctrl.data));
                        sigdet_ctrl.fields.signal_detect_frc = 1;
                        sigdet_ctrl.fields.signal_detect_frc_val = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SIGDET_SIGDET_CTRL_1_Adr,\
                                                     sigdet_ctrl.data));
                    }    
                    if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                        break;
                    }
                }
            }
        break;
        case phymodRxSquelchOff:
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    /* Configure Tx side(SIDE_A) first*/
                    pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
                    rd_lane = pkg_ln_des->slice_rd_val;
                    if(pkg_side == SIDE_A) {
                        sys_en = pkg_ln_des->sideA;
                    } else {
                        sys_en = pkg_ln_des->sideB;
                    }
                    /* Program the slice register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                    if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SIGDET_SIGDET_CTRL_1_Adr,\
                                                    &sigdet_ctrl.data));
                        sigdet_ctrl.fields.signal_detect_frc = 0;
                        sigdet_ctrl.fields.signal_detect_frc_val = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                     SIGDET_SIGDET_CTRL_1_Adr,\
                                                     sigdet_ctrl.data));
                    }
                    if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                        break;
                    }
                }
            }
        break;
        default:
        break;
    }

    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_rx_lane_control_get(const phymod_access_t *pa,
                              phymod_phy_rx_lane_control_t* rx_control)
{
    int lane_map = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    SIGDET_SIGDET_CTRL_1_t sigdet_ctrl; 
    PHYMOD_MEMSET(&sigdet_ctrl, 0 , sizeof(SIGDET_SIGDET_CTRL_1_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    switch(*rx_control) {
        case phymodRxReset: 
            return PHYMOD_E_UNAVAIL;
        case phymodRxSquelchOn:
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    wr_lane = pkg_ln_des->slice_wr_val;
                    rd_lane = pkg_ln_des->slice_rd_val;
                    if(pkg_side == SIDE_A) {
                        sys_en = pkg_ln_des->sideA;
                    } else {
                        sys_en = pkg_ln_des->sideB;
                    }
                    /* Program the slice register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                    if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SIGDET_SIGDET_CTRL_1_Adr,\
                                                    &sigdet_ctrl.data));
                       *rx_control = sigdet_ctrl.fields.signal_detect_frc ? phymodRxSquelchOn : phymodRxSquelchOff;
                    } 
                }
            }
        break;
        case phymodRxSquelchOff:
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                    PHYMOD_NULL_CHECK(pkg_ln_des);
                    wr_lane = pkg_ln_des->slice_wr_val;
                    rd_lane = pkg_ln_des->slice_rd_val;
                    if(pkg_side == SIDE_A) {
                        sys_en = pkg_ln_des->sideA;
                    } else {
                        sys_en = pkg_ln_des->sideB;
                    }
                    /* Program the slice register */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                    if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SIGDET_SIGDET_CTRL_1_Adr,\
                                                    &sigdet_ctrl.data));
                        *rx_control = sigdet_ctrl.fields.signal_detect_frc_val ? 1 : 0; 
                    } 
                }
            }
        break;
        default:
        break;
    }
    return PHYMOD_E_NONE;
}

int furia_82212_lane_cross_switch_map_set(const phymod_access_t *pa,
                            const uint32_t *tx_source_array)
{
    uint32_t chip_id = 0;
    uint32_t tx_die_lane = 0;
    uint32_t rx_die_lane = 0;
    uint32_t num_lanes = 0;
    uint32_t acc_flags = 0; 
    int pkg_side = 0;
    int lane_index = 0;
    int sys_en = 0;
    uint32_t phy_id_idx = 0;
    uint32_t exist_phy = 0;
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    SYS_RX_PMA_DP_DP_CTRL_t sys_rx_dp_ctrl;
    LINE_RX_PMA_DP_DP_CTRL_t line_rx_dp_ctrl;
    PHYMOD_MEMSET(&sys_rx_dp_ctrl, 0 , sizeof(SYS_RX_PMA_DP_DP_CTRL_t));
    PHYMOD_MEMSET(&line_rx_dp_ctrl, 0 , sizeof(LINE_RX_PMA_DP_DP_CTRL_t));
    
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (chip_id == FURIA_ID_82212) {
            if (((pa->lane_mask >> lane_index) & 1) != 0x1) {
                continue;
            }
        }
        /* Set Swap configuration with newly supplied dst_lane*/
        tx_pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
        PHYMOD_NULL_CHECK(tx_pkg_ln_des);
        tx_die_lane = tx_pkg_ln_des->die_lane_num;
        rx_pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, tx_source_array[lane_index]);
        PHYMOD_NULL_CHECK(rx_pkg_ln_des);
        rx_die_lane = rx_pkg_ln_des->die_lane_num;
        if(pkg_side == SIDE_A) {
            sys_en = tx_pkg_ln_des->sideA;
        } else { 
            sys_en = tx_pkg_ln_des->sideB;
        }
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, sys_en, 1<< tx_die_lane, tx_die_lane));
        if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
            _furia_get_phy_id_idx(pa->addr, &phy_id_idx, &exist_phy); 
            if (phy_id_idx >= MAX_NUM_PHYS) {
                return PHYMOD_E_LIMIT;
            }
            if (sys_en == LINE) {
                /* Select system Rx as specified in Line Tx array */
                /* SYS_RX_PMA_DP_dp_ctrl */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_RX_PMA_DP_dp_ctrl_Adr,\
                                            &sys_rx_dp_ctrl.data));
                sys_rx_dp_ctrl.fields.out_sel = rx_die_lane;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_RX_PMA_DP_dp_ctrl_Adr,\
                                             sys_rx_dp_ctrl.data));
                if (exist_phy) {
                    if (lane_index < 2) {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * lane_index)); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * lane_index)); 
                    } else if ((lane_index >= 2) && (lane_index <= 5)) {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * (lane_index-2))); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * (lane_index-2))); 
                    } else if ((lane_index > 5) && (lane_index <= 9)) {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * (lane_index-4))); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * (lane_index-4))); 
                    } else {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * (lane_index-6))); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * (lane_index-6))); 
                    }

                }
            } else {
                /* Select Line Rx as specified in System Tx array */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_RX_PMA_DP_dp_ctrl_Adr,\
                                            &line_rx_dp_ctrl.data));
                line_rx_dp_ctrl.fields.out_sel = rx_die_lane;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_RX_PMA_DP_dp_ctrl_Adr,\
                                             line_rx_dp_ctrl.data));
                if (exist_phy) {
                    if (lane_index < 2) {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * lane_index)); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * lane_index)); 
                    } else if (lane_index >= 2 && lane_index <= 5) {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * (lane_index-2))); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * (lane_index-2))); 
                    } else if (lane_index > 5 && lane_index <= 9) {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * (lane_index-4))); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * (lane_index-4))); 
                    } else {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * (lane_index-6))); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * (lane_index-6))); 
                    }
                }
            }
        }
    }
    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_furia_fw_enable(pa));
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_82212_lane_cross_switch_map_get(const phymod_access_t *pa, uint32_t* tx_source_array)
{
    uint32_t chip_id = 0;
    uint32_t tx_die_lane = 0;
    uint32_t rx_die_lane = 0;
    uint32_t rx_pkg_lane = 0;
    uint32_t num_lanes = 0;
    uint32_t acc_flags = 0; 
    int pkg_side = 0;
    int lane_index = 0, tmp_lane_index = 0;
    int sys_en = 0;
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    SYS_RX_PMA_DP_DP_CTRL_t sys_rx_dp_ctrl;
    LINE_RX_PMA_DP_DP_CTRL_t line_rx_dp_ctrl;
    PHYMOD_MEMSET(&sys_rx_dp_ctrl, 0 , sizeof(SYS_RX_PMA_DP_DP_CTRL_t));
    PHYMOD_MEMSET(&line_rx_dp_ctrl, 0 , sizeof(LINE_RX_PMA_DP_DP_CTRL_t));
    
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (chip_id == FURIA_ID_82212) {
            if (((pa->lane_mask >> lane_index) & 1) != 0x1) {
                continue;
            }
        }
        /* Set Swap configuration with newly supplied dst_lane*/
        tx_pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
        PHYMOD_NULL_CHECK(tx_pkg_ln_des);
        tx_die_lane = tx_pkg_ln_des->die_lane_num;
        if(pkg_side == SIDE_A) {
            sys_en = tx_pkg_ln_des->sideA;
        } else { 
            sys_en = tx_pkg_ln_des->sideB;
        }
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, sys_en, 1<< tx_die_lane, tx_die_lane));
        if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
            if (lane_index < 2) {
                tmp_lane_index = lane_index;
            } else if (lane_index >= 2 && lane_index <= 5) {
                tmp_lane_index = lane_index - 2;
            } else if (lane_index > 5 && lane_index <= 9) {
                tmp_lane_index = lane_index - 4;
            } else {
                tmp_lane_index = lane_index - 6;
            }

            if (sys_en == LINE) {
                /* Select system Rx as specified in Line Tx array */
                /* SYS_RX_PMA_DP_dp_ctrl */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_RX_PMA_DP_dp_ctrl_Adr,\
                                            &sys_rx_dp_ctrl.data));
                rx_die_lane = sys_rx_dp_ctrl.fields.out_sel;
                PHYMOD_IF_ERR_RETURN
                    (_furia_get_pkg_lane(pa->addr, chip_id, rx_die_lane, tmp_lane_index, &rx_pkg_lane));
                tx_source_array[lane_index] = rx_pkg_lane;
            } else {
                /* Select Line Rx as specified in System Tx array */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_RX_PMA_DP_dp_ctrl_Adr,\
                                            &line_rx_dp_ctrl.data));
                rx_die_lane = line_rx_dp_ctrl.fields.out_sel;
                PHYMOD_IF_ERR_RETURN
                    (_furia_get_pkg_lane(pa->addr, chip_id, rx_die_lane, tmp_lane_index, &rx_pkg_lane));
                tx_source_array[lane_index] = rx_pkg_lane;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_lane_cross_switch_map_set(const phymod_access_t *pa,
                            const uint32_t *tx_source_array)
{
    uint32_t chip_id = 0;
    uint32_t tx_die_lane = 0;
    uint32_t rx_die_lane = 0;
    uint32_t num_lanes = 0;
    uint32_t acc_flags = 0; 
    int pkg_side = 0;
    int lane_index = 0;
    int sys_en = 0;
    uint32_t phy_id_idx = 0;
    uint32_t exist_phy = 0;
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    SYS_RX_PMA_DP_DP_CTRL_t sys_rx_dp_ctrl;
    LINE_RX_PMA_DP_DP_CTRL_t line_rx_dp_ctrl;
    PHYMOD_MEMSET(&sys_rx_dp_ctrl, 0 , sizeof(SYS_RX_PMA_DP_DP_CTRL_t));
    PHYMOD_MEMSET(&line_rx_dp_ctrl, 0 , sizeof(LINE_RX_PMA_DP_DP_CTRL_t));
    
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (chip_id == FURIA_ID_82212) {
            if (((pa->lane_mask >> lane_index) & 1) != 0x1) {
                continue;
            } else {
                PHYMOD_IF_ERR_RETURN(
                    furia_82212_lane_cross_switch_map_set(pa, tx_source_array));
                return PHYMOD_E_NONE;
            }
          
        }
        /* Set Swap configuration with newly supplied dst_lane*/
        tx_pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
        PHYMOD_NULL_CHECK(tx_pkg_ln_des);
        tx_die_lane = tx_pkg_ln_des->die_lane_num;
        rx_pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, tx_source_array[lane_index]);
        PHYMOD_NULL_CHECK(rx_pkg_ln_des);
        rx_die_lane = rx_pkg_ln_des->die_lane_num;
        if(pkg_side == SIDE_A) {
            sys_en = tx_pkg_ln_des->sideA;
        } else { 
            sys_en = tx_pkg_ln_des->sideB;
        }
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, sys_en, 1<< tx_die_lane, tx_die_lane));
        if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
            _furia_get_phy_id_idx(pa->addr, &phy_id_idx, &exist_phy); 
            if (sys_en == LINE) {
                /* Select system Rx as specified in Line Tx array */
                /* SYS_RX_PMA_DP_dp_ctrl */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_RX_PMA_DP_dp_ctrl_Adr,\
                                            &sys_rx_dp_ctrl.data));
                sys_rx_dp_ctrl.fields.out_sel = rx_die_lane;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_RX_PMA_DP_dp_ctrl_Adr,\
                                             sys_rx_dp_ctrl.data));
                if (exist_phy) {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * lane_index)); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * lane_index)); 
                }
            } else {
                /* Select Line Rx as specified in System Tx array */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_RX_PMA_DP_dp_ctrl_Adr,\
                                            &line_rx_dp_ctrl.data));
                line_rx_dp_ctrl.fields.out_sel = rx_die_lane;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_RX_PMA_DP_dp_ctrl_Adr,\
                                             line_rx_dp_ctrl.data));
                if (exist_phy) {
                    if(FURIA_IS_DUPLEX(chip_id)){
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * (lane_index + 4))); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * (lane_index + 4))); 
                    } else {
                        glb_phy_list[phy_id_idx].tx_lanes &= ~(0xF << (4 * lane_index)); 
                        glb_phy_list[phy_id_idx].tx_lanes |= ((tx_source_array[lane_index] & 0xF) << (4 * lane_index)); 
                    }
                }
            }
        }
    }
    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_furia_fw_enable(pa));
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_lane_cross_switch_map_get(const phymod_access_t *pa, uint32_t* tx_source_array)
{
    uint32_t chip_id = 0;
    uint32_t tx_die_lane = 0;
    uint32_t rx_die_lane = 0;
    uint32_t rx_pkg_lane = 0;
    uint32_t num_lanes = 0;
    uint32_t acc_flags = 0; 
    int pkg_side = 0;
    int lane_index = 0;
    int sys_en = 0;
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    SYS_RX_PMA_DP_DP_CTRL_t sys_rx_dp_ctrl;
    LINE_RX_PMA_DP_DP_CTRL_t line_rx_dp_ctrl;
    PHYMOD_MEMSET(&sys_rx_dp_ctrl, 0 , sizeof(SYS_RX_PMA_DP_DP_CTRL_t));
    PHYMOD_MEMSET(&line_rx_dp_ctrl, 0 , sizeof(LINE_RX_PMA_DP_DP_CTRL_t));
    
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (chip_id == FURIA_ID_82212) {
            if (((pa->lane_mask >> lane_index) & 1) != 0x1) {
                continue;
            } else {
                PHYMOD_IF_ERR_RETURN(
                    furia_82212_lane_cross_switch_map_get(pa, tx_source_array));
                return PHYMOD_E_NONE;
            }
        }
        /* Set Swap configuration with newly supplied dst_lane*/
        tx_pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
        PHYMOD_NULL_CHECK(tx_pkg_ln_des);
        tx_die_lane = tx_pkg_ln_des->die_lane_num;
        if(pkg_side == SIDE_A) {
            sys_en = tx_pkg_ln_des->sideA;
        } else { 
            sys_en = tx_pkg_ln_des->sideB;
        }
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_SLICE_REG(pa, sys_en, 1<< tx_die_lane, tx_die_lane));
        if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
            if (sys_en == LINE) {
                /* Select system Rx as specified in Line Tx array */
                /* SYS_RX_PMA_DP_dp_ctrl */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_RX_PMA_DP_dp_ctrl_Adr,\
                                            &sys_rx_dp_ctrl.data));
                rx_die_lane = sys_rx_dp_ctrl.fields.out_sel;
                PHYMOD_IF_ERR_RETURN
                    (_furia_get_pkg_lane(pa->addr, chip_id, rx_die_lane, lane_index, &rx_pkg_lane));
                tx_source_array[lane_index] = rx_pkg_lane;
            } else {
                /* Select Line Rx as specified in System Tx array */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_RX_PMA_DP_dp_ctrl_Adr,\
                                            &line_rx_dp_ctrl.data));
                rx_die_lane = line_rx_dp_ctrl.fields.out_sel;
                PHYMOD_IF_ERR_RETURN
                    (_furia_get_pkg_lane(pa->addr, chip_id, rx_die_lane, lane_index, &rx_pkg_lane));
                tx_source_array[lane_index] = rx_pkg_lane;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


int furia_force_tx_training_set(const phymod_access_t *pa, uint32_t enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0;
    uint16_t die_lane_num = 0;
    FUR_GEN_CNTRLS_gpreg12_t gen_ctrls_gpreg12;
    FUR_GEN_CNTRLS_gpreg11_t gen_ctrls_gpreg11;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    PHYMOD_MEMSET(&gen_ctrls_gpreg12, 0 , sizeof(FUR_GEN_CNTRLS_gpreg12_t));
    PHYMOD_MEMSET(&gen_ctrls_gpreg11, 0 , sizeof(FUR_GEN_CNTRLS_gpreg11_t));
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Force Tx training is not applicable for simplex packages")));
    }
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            die_lane_num = pkg_ln_des->die_lane_num;
            if(pkg_side == SIDE_A) {
                /* Read GPReg12 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg12_Adr,\
                                            &gen_ctrls_gpreg12.data));
                if(enable) {
                    gen_ctrls_gpreg12.fields.en_frc_tx_trng_line |= (1 << die_lane_num);
                } else {
                    gen_ctrls_gpreg12.fields.en_frc_tx_trng_line &= ~(1 << die_lane_num);
                }

                /* Write to GPReg12 */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             FUR_GEN_CNTRLS_gpreg12_Adr,\
                                             gen_ctrls_gpreg12.data));
            } else {
                /* Read GPReg11 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg11_Adr,\
                                            &gen_ctrls_gpreg11.data));
                if(enable) { 
                    gen_ctrls_gpreg11.fields.en_frc_tx_trng_sys |= (1 << die_lane_num);
                } else {
                    gen_ctrls_gpreg11.fields.en_frc_tx_trng_sys &= ~(1 << die_lane_num);
                }
                /* Write to GPReg11 */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             FUR_GEN_CNTRLS_gpreg11_Adr,\
                                             gen_ctrls_gpreg11.data));
            }
        }
    }
    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_furia_fw_enable(pa));
    return PHYMOD_E_NONE;
}
int furia_force_tx_training_get(const phymod_access_t *pa, uint32_t *enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0;
    uint16_t die_lane_num = 0;
    FUR_GEN_CNTRLS_gpreg12_t gen_ctrls_gpreg12;
    FUR_GEN_CNTRLS_gpreg11_t gen_ctrls_gpreg11;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    PHYMOD_MEMSET(&gen_ctrls_gpreg12, 0 , sizeof(FUR_GEN_CNTRLS_gpreg12_t));
    PHYMOD_MEMSET(&gen_ctrls_gpreg11, 0 , sizeof(FUR_GEN_CNTRLS_gpreg11_t));
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    if(FURIA_IS_SIMPLEX(chip_id)) {
     PHYMOD_DEBUG_ERROR(("Force Tx training is not applicable for simplex packages\n"));
     return PHYMOD_E_UNAVAIL;
    }
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            die_lane_num = pkg_ln_des->die_lane_num;
            if(pkg_side == SIDE_A) {
                /* Read GPReg12 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg12_Adr,\
                                            &gen_ctrls_gpreg12.data));
                *enable = (gen_ctrls_gpreg12.fields.en_frc_tx_trng_line >> die_lane_num) & 1;
            } else {
                /* Read GPReg11 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg11_Adr,\
                                            &gen_ctrls_gpreg11.data));
                *enable = (gen_ctrls_gpreg11.fields.en_frc_tx_trng_sys >> die_lane_num) & 1;
            }
        }
    }
    return PHYMOD_E_NONE;
}

int furia_force_tx_training_status_get(const phymod_access_t *pa, phymod_cl72_status_t* status)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0;
    CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_STATUS_REGISTER_151_t cl93_72_status; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    PHYMOD_MEMSET(&cl93_72_status, 0 , sizeof(CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_STATUS_REGISTER_151_t));
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Force Tx training is not applicable for simplex packages")));
    }
    /* Read Tx Training enable status */
    PHYMOD_IF_ERR_RETURN
        (furia_force_tx_training_get(pa, &status->enabled));
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
        
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;


            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            CL93n72_IEEE_TX_cl93n72it_BASE_R_PMD_status_register_151_Adr,\
                                            &cl93_72_status.data));
                /* Extract Tx training status*/
                status->locked = (!cl93_72_status.fields.cl93n72_ieee_training_failure) && (cl93_72_status.fields.cl93n72_ieee_receiver_status);
                break;
            }
        }
    /* Set the slice register to default */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

/**  Get Package index 
 *    
 *    
 *
 *    @param chip_id            Chip id of furia device
 *    @param pkg_idx            Furia package
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
STATIC int furia_get_pkg_idx(uint32_t chip_id, int *pkg_idx)
{
    int index = 0;
    for (index = 0; index < MAX_NUM_PACKAGES; index ++) {
        if(glb_package_array[index] != NULL) {
           if(glb_package_array[index]->chip_id == chip_id) {
               *pkg_idx = index;
               break;
           } 
        } 
    }
    if(index >= MAX_NUM_PACKAGES) {
        return PHYMOD_E_LIMIT; 
    }
    return PHYMOD_E_NONE; 
}


void _furia_get_phy_id_idx(uint32_t phy_id, uint32_t *idx, uint32_t *exist_phy)
{
    unsigned int found = 0, i;
    /* coverity[write_constant_to_param] */
    *idx = 0xFF;
    for (i=0; i < MAX_NUM_PHYS; i++) {
       if (glb_phy_list[i].valid  && (glb_phy_list[i].phy_id == phy_id)) {
           found = 1;
           *idx = i;
           *exist_phy = 1;
           break;
       }
    }
    if (!found) { /* New PHYID*/
        for (i = 0; i < MAX_NUM_PHYS; i++) {
            if (!glb_phy_list[i].valid) {
                glb_phy_list[i].phy_id = phy_id;
                glb_phy_list[i].valid = 1;
                 *exist_phy = 0;
                *idx = i;
                break;
            }
        }
    }
}

/**   Get lane descriptor 
 *    This function is used to retrieve lane descriptor from package lane 
 *    
 *    @param chip_id            Chip id of furia device
 *    @param pa                 Pointer to phymod access structure 
 *    @param pkg_lane           Package lane number
 *    @param tx_rx              Tx or Rx
 *
 *    @return pkg_ln_des        Lane descriptor structure contains the info
 *                              about package lane and die lane mapping 
 */
const FURIA_PKG_LANE_CFG_t* _furia_pkg_ln_des_lane_swap(uint32_t chip_id, const phymod_access_t *pa, int pkg_lane, int tx_rx)
{
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    int pkg_idx;
    uint32_t acc_flags = 0;
    uint32_t pkg_side = 0;
    int index = 0;
    uint32_t exist_phy = 0;
    uint32_t phy_id_idx = 0; 
    
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    _furia_get_phy_id_idx(pa->addr, &phy_id_idx, &exist_phy); 
    if (FURIA_IS_DUPLEX(chip_id)) {
        if (pkg_side == SIDE_A) {
            if (tx_rx) { /* Rx side */
                if (exist_phy) {
                    pkg_lane = (glb_phy_list[phy_id_idx].tx_lanes >> (4 * (pkg_lane + 4))) & 0xF;  
                } 
            } else { /* Tx side */
                for (index = 0; index < 4; index++) {
                    if (exist_phy) {
                        if ((((glb_phy_list[phy_id_idx].tx_lanes) >> (4 * index)) & 0xF) == pkg_lane) {
                            pkg_lane = index;
                            break;
                        }
                    }  
                }
            }
        } 
            
        if ((pa->addr & 1) == 1) {
            if (FURIA_IS_DUAL_DIE_DUPLEX(chip_id)) { 
                pkg_lane = pkg_lane + 4;
            }
        }
    } else { /* Simplex package */
        if (pkg_side == SIDE_A) {
            for (index = 0; index < 7; index++) {
                if (exist_phy) {
                    if ((((glb_phy_list[phy_id_idx].tx_lanes) >> (4 * index)) & 0xF) == pkg_lane) {
                        if (chip_id == FURIA_ID_82212) {
                            const int array_82212[2] = { 0x987610, 0xba5432};
                            pkg_lane = (array_82212[phy_id_idx & 1] >> (index *4)) & 0xF;
                        } else {
                            pkg_lane = index;
                        }
                        break;
                    }
                }  
            }
        }
    }  
    /* Get the package index */
    if (!furia_get_pkg_idx(chip_id, &pkg_idx)) {
        pkg_ln_des = glb_package_array[pkg_idx] + pkg_lane;
    }
    return pkg_ln_des; 
}  
 
const FURIA_PKG_LANE_CFG_t* _furia_pkg_ln_des(uint32_t chip_id, const phymod_access_t *pa, int pkg_lane)
{
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    int pkg_idx;
    
    if ((pa->addr & 1) == 1) {
        if(FURIA_IS_DUPLEX(chip_id)) {
            if (FURIA_IS_DUAL_DIE_DUPLEX(chip_id)) { 
                pkg_lane = pkg_lane + 4;
            }
        }
    }  
    /* Get the package index */
    if (!furia_get_pkg_idx(chip_id, &pkg_idx)) {
        pkg_ln_des = glb_package_array[pkg_idx] + pkg_lane;
    }
    return pkg_ln_des; 
}  
STATIC int _furia_get_pkg_lane(uint32_t phy_id, uint32_t chip_id, uint32_t die_lane, uint32_t lane_index, uint32_t *pkg_lane)
{
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    int pkg_idx = 0;
    uint16_t i = 0;
    uint16_t start_index = 0;
    uint16_t max_lanes = 0; 
    uint32_t phy_id_idx = 0;
    uint32_t exist_phy = 0;
    if(!furia_get_pkg_idx(chip_id, &pkg_idx)) {
        if(FURIA_IS_SIMPLEX(chip_id)) {
            start_index = 0;
            max_lanes = 8;
            if (chip_id == FURIA_ID_82212) {
                max_lanes = 12;
            }  
        } else { 
            if ((phy_id & 1) == 1) {
                if (FURIA_IS_DUAL_DIE_DUPLEX(chip_id)) { 
                    start_index = 4;
                    max_lanes = 4;
                } else {
                    start_index = 0;
                    max_lanes = 4;
                }
            } else {
                start_index = 0;
                max_lanes = 4;
            } 
        } 
    }
    for(i = start_index; i < (start_index + max_lanes); i++) {
        pkg_ln_des = glb_package_array[pkg_idx] + i;
        if(pkg_ln_des->die_lane_num == die_lane) {
            if (FURIA_IS_SIMPLEX(chip_id)) {
                if ((chip_id == FURIA_ID_82212) && (((phy_id & 1) != pkg_ln_des->die_addr))) {
                   continue;
                }

                _furia_get_phy_id_idx(phy_id, &phy_id_idx, &exist_phy); 
                if (exist_phy) {
                    if (pkg_ln_des->pkg_lane_num == (((glb_phy_list[phy_id_idx].tx_lanes) >> (4 *lane_index)) & 0xF)) {
                        *pkg_lane = pkg_ln_des->pkg_lane_num;
                        break;
                    } else {
                        continue;
                    }
                }
            } else {
                *pkg_lane = pkg_ln_des->pkg_lane_num;
                if (FURIA_IS_DUAL_DIE_DUPLEX(chip_id)) {
                    if ((phy_id & 1) == 1) {
                        *pkg_lane -= 4;
                    }
                }
                break;
            }
        }
    }
    if(i >= (start_index + max_lanes)) {
        return PHYMOD_E_FAIL;
    }
   
    return PHYMOD_E_NONE; 
}

/**   Get ref clock frequency
 *    This function returns ref_clk_freq from ref clock freq type 
 *
 *    @param ref_clk_freq  Ref clk type 
 * 
 *    @return freq         Ref clk freq  in Mhz based on ref clk freq type
 */
int _furia_get_ref_clock_freq_in_mhz(FURIA_REF_CLK_E ref_clk_freq) 
{
    int freq = 0; 
    if (ref_clk_freq == REF_CLK_106p25MHz) {
        freq = 10625;
    } else if (ref_clk_freq == REF_CLK_174p703125MHz) { /* 174.703125 Mhz */
        freq = 17470;
    } else if (ref_clk_freq == REF_CLK_156p25Mhz) { /* 156.25 Mhz */
        freq = 15625;
    } else if (ref_clk_freq == REF_CLK_125Mhz) { /* 125 Mhz */
        freq = 12500;
    } else if (ref_clk_freq == REF_CLK_156p637Mhz) { /* 156.637 Mhz */
        freq = 15663;
    } else if (ref_clk_freq == REF_CLK_161p1328125Mhz) { /* 161.1328125 Mhz */
        freq = 16113;
    } else if (ref_clk_freq == REF_CLK_168p04Mhz) { /* 168.04 Mhz */
        freq = 16804;
    } else if (ref_clk_freq == REF_CLK_172p64Mhz) { /* 172.64 Mhz */
        freq = 17264;
    } else {
        PHYMOD_RETURN_WITH_ERR
            (PHYMOD_E_PARAM,
            (_PHYMOD_MSG(" Ref clk is not supported by furia")));
    }
    return(freq);
}

/**   Config clk scaler  
 *    This function is used to program clk scaler 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param ref_clk_freq       Furia ref clk type 
 *  
 *    @return PHYMOD_E_NONE     successful function execution 
 */
STATIC int _furia_config_clk_scaler_val(const phymod_access_t *pa, FURIA_REF_CLK_E ref_clk_freq) 
{
    uint16_t clock_scaler_code = 0;
    FUR_GEN_CNTRLS_GEN_CONTROL3_t gen_ctrl_3;
    PHYMOD_MEMSET(&gen_ctrl_3, 0 , sizeof(FUR_GEN_CNTRLS_GEN_CONTROL3_t));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control3_Adr,\
                                &gen_ctrl_3.data));
    gen_ctrl_3.fields.clock_sclr_lock = 0x1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control3_Adr,\
                                 gen_ctrl_3.data));
    switch (ref_clk_freq) {
        case REF_CLK_106p25MHz:
            clock_scaler_code = 0x4;
        break;
        case REF_CLK_168p04Mhz:
        case REF_CLK_174p703125MHz:
            clock_scaler_code = 0x0; 
        break;
        case REF_CLK_156p25Mhz:
        case REF_CLK_156p637Mhz:
            clock_scaler_code = 0x2;
        break;
        case REF_CLK_125Mhz:
            clock_scaler_code = 0x3;
        break;
        case REF_CLK_161p1328125Mhz:
            clock_scaler_code = 0x1;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                    (PHYMOD_E_PARAM,
                    (_PHYMOD_MSG(" This Ref Clock is not supported by Furia")));

        break;   

    }
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control3_Adr,\
                                &gen_ctrl_3.data));

    gen_ctrl_3.fields.clock_scaler_code = clock_scaler_code;

    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control3_Adr,\
                                 gen_ctrl_3.data));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control3_Adr,\
                                &gen_ctrl_3.data));
    gen_ctrl_3.fields.clock_sclr_lock = 0x0;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control3_Adr,\
                                 gen_ctrl_3.data));
    return PHYMOD_E_NONE;
}

/**   Get PHY type 
 *    This function is used to retrieve the PHY type
 *    based on the interface value. 
 *
 *    @param intf               Interface type
 *    @param speed              Data rate
 * 
 *    @return phy_type          PHY type calculated based on interface  
 */
STATIC uint16_t _furia_get_phy_type(phymod_interface_t intf, uint32_t speed)
{
    uint16_t phy_type = 0;
    switch (intf) {
        case phymodInterfaceLR2:
        case phymodInterfaceSR2:
        case phymodInterfaceER2:
            if (speed == SPEED_50G || speed == SPEED_53G) {/* 50G SR2/LR2/ER2 */
                phy_type = 0x1;
            }
        break;
        case phymodInterfaceSR:
        case phymodInterfaceCR4:
        case phymodInterfaceCR10:
        case phymodInterfaceLR4:
        case phymodInterfaceLR:
        case phymodInterfaceSR4:
        case phymodInterfaceER:
        case phymodInterfaceER4:
            if (speed == SPEED_6G) {
                phy_type = 0x4; 
            } else {
                phy_type = 0x1;
            }

        break;
        case phymodInterfaceKX:
        case phymodInterfaceKX4:
        case phymodInterfaceKR:
        case phymodInterfaceKR2:
        case phymodInterfaceKR4:
            phy_type = 0x0;
        break;
        case phymodInterfaceCX:
        case phymodInterfaceCR:
        case phymodInterfaceCR2:
        case phymodInterfaceSFI:
        case phymodInterfaceXFI:
        case phymodInterfaceCAUI4_C2C:
        case phymodInterfaceCAUI4_C2M:
        case phymodInterfaceXLAUI:
        case phymodInterfaceCAUI4:
        case phymodInterfaceCAUI:
                phy_type = 0x1;/*mixed mode 10G*/
                break; 
        case phymodInterfaceCX4:
        case phymodInterfaceVSR:
                phy_type = 0x4; 
                break;
        default:
        break;
    }

    return phy_type;
}

/**   Get link type 
 *    This function is used to retrieve the operating mode of the PHY
 *
 *    @param intf               Phymod interface type
 *    @param speed              User specified speed 
 *    @param otn_type           OTN type 
 * 
 *    @return link_type         Link type calculated based on intf and speed 
 */
STATIC uint16_t _furia_get_link_type(phymod_interface_t intf, uint32_t speed, phymod_otn_type_t otn_type)
{
    uint16_t link_type = 0; 
    switch (intf) {
        case phymodInterfaceKX:
                link_type = 0x1;
        break;
        case phymodInterfaceCAUI4_C2C:
        case phymodInterfaceCAUI4_C2M:
        case phymodInterfaceCAUI4:
        case phymodInterfaceCAUI:
                link_type = 0xf; 
        break;
        case phymodInterfaceKR:
            if (speed == SPEED_10G || speed == SPEED_11G || speed == SPEED_11P5G) {
                link_type = 0x2;
            } else if (speed == SPEED_20G || speed == SPEED_21G || speed == SPEED_23G) {
                link_type = 0x6;
            } else {
                link_type = 0x8;
            }    
        break;
        case phymodInterfaceSR:
        case phymodInterfaceLR:
        case phymodInterfaceER:
            if (speed == SPEED_10G || speed == SPEED_11G || speed == SPEED_11P5G) {
                link_type = 0x1;
            } else if (speed == SPEED_20G || speed == SPEED_21G || speed == SPEED_23G) {
                link_type = 0x6;
            } else if (speed == SPEED_6G) {
                link_type = 0x5;
            } else {
                link_type = 0x7;
            }
        break;
        case phymodInterfaceSR2:
        case phymodInterfaceLR2:
        case phymodInterfaceER2:/* 50G SR2/LR2/ER2 */
            if (speed == SPEED_50G || speed == SPEED_53G) {
                link_type = 0x7;
            }
        break;
        case phymodInterfaceCR4:
            if (speed == SPEED_100G || speed == SPEED_106G) {
                link_type = 0x5;
            } else {
                link_type = 0x2;
            }
        break;
        case phymodInterfaceKR4:
        case phymodInterfaceLR4:
        case phymodInterfaceSR4:
        case phymodInterfaceER4:
            if (speed == SPEED_100G || speed == SPEED_106G) {
                link_type = 0x4;
            } else {
                link_type = 0x3;
            }
        break;
        case phymodInterfaceKR2:
            if (speed == SPEED_20G || speed == SPEED_21G) {
                link_type = 0x5;
            } else if (speed == SPEED_40G || speed == SPEED_42G) {
                link_type = 0x7;
            } else {
                link_type = 0x9;
            }
        break;
        case phymodInterfaceCX:
        case phymodInterfaceCR:
        case phymodInterfaceCR2:
        case phymodInterfaceSFI:
        case phymodInterfaceCX4:
            if (speed == SPEED_10G || speed == SPEED_11G || speed == SPEED_11P5G ||
                speed == SPEED_40G || speed == SPEED_42G) {
                link_type = 0xd ; /*mixed mode. old 0x4*/
           }else if (speed == SPEED_10G  || speed == SPEED_11P18G || speed == SPEED_11P5G || speed == SPEED_11P25G ||
                 speed == SPEED_40G || speed == SPEED_42G || speed == SPEED_8P5G   ) {
                 link_type = 0x4;
           } else if ( speed == SPEED_25G || speed == SPEED_27G || speed == SPEED_50G || speed == SPEED_53G) {
                link_type = 0xc; /*mixed mode. old = 0x5*/
            }
        break;
        case phymodInterfaceVSR:
            link_type = 0x3;
        break;
        case phymodInterfaceXFI:
        case phymodInterfaceXLAUI:
            link_type = 0xe; /*mixed mode . old = 0x2*/
        break;
        case phymodInterfaceOTN:
        if(( otn_type == phymodOTNOTU1e )||( otn_type == phymodOTNOTU2e )){
                 link_type = 0x1;
        }else  if( otn_type == phymodOTNOTU3 || otn_type == phymodOTNOTU3e2 ){
                 link_type = 0x2;
        }else  if( otn_type == phymodOTNOTU4 ){
                 link_type = 0x3;
        }
        break;

        default:
        break;
    }
    return link_type;
}

/**   Get furia ref clk type 
 *    This function is used to convert the phymod ref clk type to 
 *    furia ref clk type 
 *
 *    @param ref_clk            Phymod ref clk type 
 *    @param ref_clk_freq       Furia ref clk type  
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
STATIC int _furia_phymod_to_furia_type_ref_clk_freq(phymod_ref_clk_t ref_clk, FURIA_REF_CLK_E *ref_clk_freq)
{
    switch (ref_clk) {
        case phymodRefClk156Mhz:
           *ref_clk_freq = REF_CLK_156p25Mhz;
        break;
        case phymodRefClk158Mhz:
        case phymodRefClk157Mhz:
        case phymodRefClk159Mhz:
           *ref_clk_freq = REF_CLK_156p637Mhz;
        break;
        case phymodRefClk125Mhz:
            *ref_clk_freq = REF_CLK_125Mhz;
        break;
        case phymodRefClk106Mhz:
            *ref_clk_freq = REF_CLK_106p25MHz;
        break;
        case phymodRefClk161Mhz:
            *ref_clk_freq = REF_CLK_161p1328125Mhz;
        break;
        case phymodRefClk168Mhz:
            *ref_clk_freq = REF_CLK_168p04Mhz;
        break;
        case phymodRefClk174Mhz:
            *ref_clk_freq = REF_CLK_174p703125MHz;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                                   (_PHYMOD_MSG(" This Ref Clock is not supported by Furia")));
        break;
    }
    return PHYMOD_E_NONE;
}

/**   Get the PLL mode for OTN
 *    This function calculates pll mode based on the refclock & speed
 *    @param ref_clk            PHYMOD ref clk frequency
 *    @param speed              speed specified by user
 *    @param pll_mode           pll mode calculated based on ref clk and speed
 *    @param higig_mode         Higig mode
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
STATIC int _furia_get_pll_mode_otn(phymod_ref_clk_t ref_clk, uint32_t otn_type, FALCON_PLL_MODE_E *pll_mode) {
#ifdef DEB
printf(" [ %s at %d ] \n",__FUNCTION__,__LINE__);
#endif
    switch((int)ref_clk){
        case phymodRefClk174Mhz:
       switch(otn_type){
       case phymodOTNOTU4:/*OTU4.4*/
           *pll_mode = PLL_MODE_160;
           break;
       case phymodOTNOTU3:/*OTU3.4*/
           *pll_mode = PLL_MODE_128;
           break;
       default:/*OTU4.10. i.e no phymod support for 4.10 so checking it in default state*/
           *pll_mode = PLL_MODE_128;
           break;
       }
   break;
        case phymodRefClk161Mhz:
       switch(otn_type){
       case phymodOTNOTU3:/*OTU3.4*/
           *pll_mode = PLL_MODE_132;
           break;
       }
   break;
        case phymodRefClk157Mhz:
        case phymodRefClk158Mhz:
   *pll_mode = PLL_MODE_140;
   break;
        case phymodRefClk159Mhz:
   *pll_mode = PLL_MODE_140;
   break;
        case phymodRefClk156Mhz:
       switch(otn_type){
       case phymodOTNOTU3:/*OTU3.4*/
           *pll_mode = PLL_MODE_140;
           break;
       }
           break;
        case phymodRefClk168Mhz:
       switch(otn_type){
       case phymodOTNOTU3:/*OTU3.4*/
           *pll_mode = PLL_MODE_128;
           break;
       case phymodOTNOTU4:/*OTU4.4*/
           *pll_mode = PLL_MODE_165;
           break;
       case phymodOTNOTU2e:/*OTU2e*/
       case phymodOTNOTU1e:/*OTU1e*/
           *pll_mode = PLL_MODE_132;
           break;
       }
   break;

   }
    return PHYMOD_E_NONE;
}


/**   Get the PLL mode 
 *    This function calculates pll mode based on the refclock & speed 
 *    @param ref_clk            PHYMOD ref clk frequency
 *    @param speed              speed specified by user
 *    @param pll_mode           pll mode calculated based on ref clk and speed
 *    @param higig_mode         Higig mode 
 * 
 *    @return PHYMOD_E_NONE     successful function execution 
 */
STATIC int _furia_get_pll_mode(phymod_ref_clk_t ref_clk, uint32_t speed, uint32_t higig_mode, FALCON_PLL_MODE_E *pll_mode) {
    switch (ref_clk) {
        case phymodRefClk157Mhz:
        case phymodRefClk158Mhz:
                *pll_mode = PLL_MODE_140;
           break;
        case phymodRefClk156Mhz:
            switch(speed) {
                case SPEED_100G:
                case SPEED_50G:
                case SPEED_25G:
                       *pll_mode = PLL_MODE_165;
                break;
                case SPEED_106G:
                case SPEED_53G:
                case SPEED_27G:
                    if (higig_mode) {
                       *pll_mode = PLL_MODE_175;
                    } 
                break;
                case SPEED_40G:
                case SPEED_20G:
                case SPEED_10G:
                     *pll_mode = PLL_MODE_132;
                     break;
                case SPEED_42G:
                case SPEED_21G:
                case SPEED_11G:
                    if (higig_mode) {
                        *pll_mode = PLL_MODE_140;
                    } 
                break;
                case SPEED_1G:
                    if(*pll_mode != PLL_MODE_165)
                       *pll_mode = PLL_MODE_132;
                break;
                case SPEED_14G:
                    *pll_mode = PLL_MODE_264;
                break;
                case SPEED_8P5G:
                    *pll_mode = PLL_MODE_160;
                break;
                case SPEED_4P25G:
                    *pll_mode = PLL_MODE_160;
                break;
                case SPEED_9P95G:
                    *pll_mode = PLL_MODE_128;
                break;

                default:
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                                   (_PHYMOD_MSG(" This speed is not supported by Furia")));
                break;
            } 
        break;
        case phymodRefClk125Mhz:
            switch(speed) {
                case SPEED_23G:
                    *pll_mode = PLL_MODE_184;
                break;
                case SPEED_15G:
                    *pll_mode = PLL_MODE_120;
                break;
                case SPEED_12P5G:
                    *pll_mode = PLL_MODE_200;
                break;
                case SPEED_11P5G:
                    *pll_mode = PLL_MODE_184;
                break;
                case SPEED_11P25G:
                    *pll_mode = PLL_MODE_180;
                break;

                case SPEED_7P5G:
                    *pll_mode = PLL_MODE_120;
                break;
                default:
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                                   (_PHYMOD_MSG(" This speed is not supported by Furia")));
                break;
            } 
        break;
        case phymodRefClk106Mhz:
            switch(speed) {
                case SPEED_28G:
                    *pll_mode = PLL_MODE_264;
                break;
                case SPEED_14G:
                    *pll_mode = PLL_MODE_264;
                break;
                case SPEED_8P5G:
                    *pll_mode = PLL_MODE_160;
                break;
                case SPEED_4P25G:
                    *pll_mode = PLL_MODE_160;
                break;
                default:
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                                   (_PHYMOD_MSG(" This speed is not supported by Furia")));
                break;
            } 
        break;
        case phymodRefClk161Mhz:
            switch(speed) {
                case SPEED_100G:
                case SPEED_50G:
                case SPEED_25G:
                case SPEED_12P5G:
                    *pll_mode = PLL_MODE_160;
                break;
                case SPEED_43G:
                *pll_mode = PLL_MODE_132;
                break;

                case SPEED_40G:
                case SPEED_20G:
                case SPEED_10G:
                    *pll_mode = PLL_MODE_128;
                break;
                case SPEED_1G:
                 if(*pll_mode !=PLL_MODE_160)
                     *pll_mode = PLL_MODE_128;
                break;
                default:
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                                   (_PHYMOD_MSG(" This speed is not supported by Furia")));
                break;
            } 
        break;
        case phymodRefClk168Mhz:
            switch(speed) {
                case SPEED_43G:
                    *pll_mode = PLL_MODE_128;
                break;
                case SPEED_40G:
                    *pll_mode = PLL_MODE_128;
                    break;
                case SPEED_100G:
                case SPEED_111G:
                    *pll_mode = PLL_MODE_165;
                    break;
                case SPEED_11P18G:
                    *pll_mode = PLL_MODE_132;
                    break;
                case SPEED_10P7G:/*for OTN2*/
                    *pll_mode = PLL_MODE_128;
                    break;
            }
            break;
        case phymodRefClk174Mhz:
            switch(speed) {
                case SPEED_100G:
                case SPEED_50G:
                case SPEED_25G:
                    *pll_mode = PLL_MODE_160;
                break;
                default:
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                                   (_PHYMOD_MSG(" This speed is not supported by Furia")));
                break;
            }
        break;
        case phymodRefClk159Mhz:
                if(speed == SPEED_10P518G){
                    *pll_mode = PLL_MODE_132;
                }else if(speed == SPEED_44P57G || speed == SPEED_44P583G ){
                    *pll_mode = PLL_MODE_140;
                }else{
                    return PHYMOD_E_PARAM;
                }
               break;
        default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                    (_PHYMOD_MSG(" This ref clock is not supported by Furia")));
        break;
    }
    return PHYMOD_E_NONE;
}

int _furia_get_pll_for_serdes(phymod_ref_clk_t ref_clk, uint32_t speed, FALCON_PLL_MODE_E pll_mode, uint16_t *serdes_pll_mode) {
    switch (ref_clk) {
        case phymodRefClk174Mhz:
        switch(speed) {
                case SPEED_11P18G:
                if(pll_mode == PLL_MODE_128){
            *serdes_pll_mode = FALCON_FURIA_pll_div_128x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;
        case SPEED_111G:
                if(pll_mode == PLL_MODE_160){
            *serdes_pll_mode = FALCON_FURIA_pll_div_160x_refc174;   
        }else{
            return PHYMOD_E_PARAM;
        }
        
        } 
        break;
    case phymodRefClk157Mhz:
        case phymodRefClk158Mhz:
        switch(speed) {
                case SPEED_11G:
        if(pll_mode == PLL_MODE_140){
            *serdes_pll_mode = FALCON_FURIA_pll_div_140x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;

        } 
        break;

        case phymodRefClk156Mhz:
            switch(speed) {
                case SPEED_43G:
        if(pll_mode == PLL_MODE_140){
            *serdes_pll_mode = FALCON_FURIA_pll_div_140x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;
                case SPEED_9P95G:
        if(pll_mode == PLL_MODE_128){
            *serdes_pll_mode = FALCON_FURIA_pll_div_128x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;
        case SPEED_40G:
        case SPEED_42G:
        case SPEED_20G:
        case SPEED_21G:
        case SPEED_11G:
        case SPEED_10G:
        case SPEED_1G:
        if(pll_mode == PLL_MODE_140){
            *serdes_pll_mode = FALCON_FURIA_pll_div_140x;   
        }else if(pll_mode == PLL_MODE_132){
            *serdes_pll_mode = FALCON_FURIA_pll_div_132x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;
        case SPEED_14G:
        case SPEED_8P5G:
        case SPEED_4P25G:
        if(pll_mode == PLL_MODE_264){
            *serdes_pll_mode = FALCON_FURIA_pll_div_264x;   
        }else if(pll_mode == PLL_MODE_160){
            *serdes_pll_mode = FALCON_FURIA_pll_div_160x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;

        case SPEED_100G:
        case SPEED_106G:
        case SPEED_50G:
        case SPEED_53G:
        case SPEED_25G:
        case SPEED_27G:
        case SPEED_12P5G:
        if(pll_mode == PLL_MODE_160){
            *serdes_pll_mode = FALCON_FURIA_pll_div_160x;
        }else if(pll_mode == PLL_MODE_175){
            *serdes_pll_mode = FALCON_FURIA_pll_div_175x;   
        }else if(pll_mode == PLL_MODE_165){
            *serdes_pll_mode = FALCON_FURIA_pll_div_165x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;
        } 
        break;
    case phymodRefClk125Mhz:
    switch(speed) {
                case SPEED_23G:
                case SPEED_11P5G:
                case SPEED_11P25G:
                case SPEED_7P5G:
                case SPEED_12P5G:
                case SPEED_15G:
                if(pll_mode == PLL_MODE_184){
            *serdes_pll_mode = FALCON_FURIA_pll_div_184x;   
                }else if(pll_mode == PLL_MODE_200){
            *serdes_pll_mode = FALCON_FURIA_pll_div_200x;   
                }else if(pll_mode == PLL_MODE_120){
            *serdes_pll_mode = FALCON_FURIA_pll_div_120x_refc125;   
                }else if(pll_mode == PLL_MODE_180){
            *serdes_pll_mode = FALCON_FURIA_pll_div_180x_refc125;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;
        }
    break;
        case phymodRefClk161Mhz:
             switch(speed) {
                case SPEED_100G:
                case SPEED_50G:
                case SPEED_25G:
                 if(pll_mode == PLL_MODE_160){
            *serdes_pll_mode = FALCON_FURIA_pll_div_160x;   
        }else{
            return PHYMOD_E_PARAM;
        }  
                break;
                case SPEED_43G:
                case SPEED_40G:
                case SPEED_20G:
                case SPEED_10G:
                case SPEED_1G:
                if(pll_mode == PLL_MODE_132){
            *serdes_pll_mode = FALCON_FURIA_pll_div_132x;   
                }else if(pll_mode == PLL_MODE_128){
            *serdes_pll_mode = FALCON_FURIA_pll_div_128x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;
                default:
                break;
        }
        break;
        case phymodRefClk168Mhz:
    switch(speed) {
                case SPEED_111G:
                case SPEED_100G:
                if(pll_mode == PLL_MODE_165){
            *serdes_pll_mode = FALCON_FURIA_pll_div_165x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;
                case SPEED_11P18G:
                if(pll_mode == PLL_MODE_132){
            *serdes_pll_mode = FALCON_FURIA_pll_div_132x;   
        }else{
            return PHYMOD_E_PARAM;
        } 
        break;
        case SPEED_43G:
        case SPEED_40G:
        case SPEED_10P7G:
                if(pll_mode == PLL_MODE_128){
            *serdes_pll_mode = FALCON_FURIA_pll_div_128x;   
        }else{
            return PHYMOD_E_PARAM;
        } 
        break;
            }
        break;
        case phymodRefClk106Mhz:
        switch(speed) {
                case SPEED_8P5G:
                case SPEED_4P25G:
        if(pll_mode == PLL_MODE_160){
            *serdes_pll_mode = FALCON_FURIA_pll_div_160x_vco2;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;
                case SPEED_14P025G:
                case SPEED_28P05G:
        if(pll_mode == PLL_MODE_264){
            *serdes_pll_mode = FALCON_FURIA_pll_div_264x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;

        }
           break;
    case phymodRefClk159Mhz:
        switch(speed) {
                case SPEED_10P518G:
                case SPEED_44P57G:
                case SPEED_44P583G:
        if(pll_mode == PLL_MODE_132){
            *serdes_pll_mode = FALCON_FURIA_pll_div_132x;   
        }else if(pll_mode == PLL_MODE_140){
            *serdes_pll_mode = FALCON_FURIA_pll_div_140x;   
        }else{
            return PHYMOD_E_PARAM;
        }
        break;

        }
           break;

        default:
        break;
    }
#ifdef DEB    
printf(" [ %s at %d : clk = %d speed = %d serdespll=%d ] \n",__FUNCTION__,__LINE__,ref_clk,speed,*serdes_pll_mode);
#endif
    return PHYMOD_E_NONE;
}


STATIC int _furia_config_clk_scaler_without_m0_reset(const phymod_access_t *pa, FURIA_REF_CLK_E ref_clk_freq)
{
    FUR_MICRO_BOOT_BOOT_POR_t micro_boot_por;
    FUR_GEN_CNTRLS_BOOT_t gen_ctrl_boot;
    int retry_count = 40;
    PHYMOD_MEMSET(&micro_boot_por, 0 , sizeof(FUR_MICRO_BOOT_BOOT_POR_t));
    PHYMOD_MEMSET(&gen_ctrl_boot, 0 , sizeof(FUR_GEN_CNTRLS_BOOT_t));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MICRO_BOOT_boot_por_Adr,\
                                &micro_boot_por.data));
    if((micro_boot_por.fields.serboot == 1) && (micro_boot_por.fields.spi_port_used == 1)) {
        while(((micro_boot_por.fields.mst_dwld_done == 0) || (micro_boot_por.fields.slv_dwld_done == 0)) && (retry_count)) {
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MICRO_BOOT_boot_por_Adr,\
                                        &micro_boot_por.data));
            PHYMOD_USLEEP(200);
            retry_count--; 
        }
        if(!retry_count) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("config failed, micro controller is busy..")));
        } 
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_boot_Adr,\
                                    &gen_ctrl_boot.data));
        retry_count = 40;
        while((gen_ctrl_boot.fields.serboot_busy == 1) && (retry_count)) {
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_GEN_CNTRLS_boot_Adr,\
                                        &gen_ctrl_boot.data));
            PHYMOD_USLEEP(200);
            retry_count--;
        }
        if(!retry_count) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("config failed, micro controller is busy with firmware download..")));
        }
    } 
   _furia_config_clk_scaler_val(pa, ref_clk_freq);
    return PHYMOD_E_NONE;
}

int _furia_fw_enable(const phymod_access_t* pa)
{
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable_reg;
    uint8_t fw_enable = 0;
    uint8_t retry_count = 40;
    PHYMOD_MEMSET(&fw_enable_reg, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));
    /* Wait for firmware to ready before config to be changed */
    PHYMOD_IF_ERR_RETURN
         (READ_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                 &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;

    while((fw_enable != 0) && (retry_count)) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                    &fw_enable_reg.data));
        fw_enable = fw_enable_reg.fields.fw_enable_val;
        PHYMOD_USLEEP(200);
        retry_count--;
    }

    if(!retry_count) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("1:config failed, micro controller is busy..")));
    }
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                &fw_enable_reg.data));
    fw_enable_reg.fields.fw_enable_val = 1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                 fw_enable_reg.data));
    retry_count = 40;
    /* Wait for firmware to ready before config to be changed */
    PHYMOD_IF_ERR_RETURN
         (READ_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                 &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;

    while((fw_enable != 0) && (retry_count)) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                    &fw_enable_reg.data));
        fw_enable = fw_enable_reg.fields.fw_enable_val;
        PHYMOD_USLEEP(200);
        retry_count--;
    }

    if(!retry_count) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("1:config failed, micro controller is busy..")));
    }
    return PHYMOD_E_NONE;
}

int _furia_autoneg_set(const phymod_access_t* pa, const phymod_autoneg_control_t* an)
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0; 
    int an_master_lane_flag = 0;
    uint32_t an_master_lane = 0, pkg_lane = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    IEEE_AN_BLK0_AN_CONTROL_REGISTER_t an_control;
    FUR_MISC_CTRL_UDMS_PHY_t udms_phy;
    PHYMOD_MEMSET(&an_control, 0 , sizeof(IEEE_AN_BLK0_AN_CONTROL_REGISTER_t));
    PHYMOD_MEMSET(&udms_phy, 0 , sizeof(FUR_MISC_CTRL_UDMS_PHY_t));
    
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Autoneg feature is not applicable for simplex packages")));
    }

    /* Program udms_en=0 */
    /* Read UDMS PHY Type Register */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_udms_phy_Adr,\
                                &udms_phy.data));
 
    /* Update the field udms disable*/
    udms_phy.fields.udms_en = (an->enable) ? 0 : 1;
    /* Write to UDMS PHY Type Register */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_udms_phy_Adr,\
                                 udms_phy.data));

    num_lanes = 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if ((lane_map >> lane_index) & 1) {
            an_master_lane ++;
        }
    }
    if (an_master_lane > 1) {
        an_master_lane = 0;
        PHYMOD_IF_ERR_RETURN(
            _furia_cfg_an_master_lane_get(pa, &an_master_lane));

        /* Doing this because for 10G we need lane index not lane MAP*/
        PHYMOD_IF_ERR_RETURN(
           _furia_get_pkg_lane(pa->addr, chip_id, an_master_lane, 0, &pkg_lane));
        lane_map = (1 << pkg_lane);
        an_master_lane_flag = 1;
    }

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (an_master_lane_flag) {
                pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            } else {
                pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            }
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            sys_en = pkg_ln_des->sideA;
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            if (an->an_mode == phymod_AN_MODE_CL73) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_AN_REG(pa,\
                                            IEEE_AN_BLK0_an_control_register_Adr,\
                                            &an_control.data));
                an_control.fields.auto_negotiationenable = an->enable;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_AN_REG(pa,\
                                             IEEE_AN_BLK0_an_control_register_Adr,\
                                             an_control.data));
            } else {
                return PHYMOD_E_PARAM;
            }
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));

    PHYMOD_IF_ERR_RETURN(
            _furia_fw_enable(pa));

    return PHYMOD_E_NONE;
}

int _furia_autoneg_get(const phymod_access_t* pa, phymod_autoneg_control_t* an, uint32_t *an_done)
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    uint32_t an_master_lane =0, pkg_lane = 0;
    int lane_index = 0;
    int an_master_lane_flag = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    IEEE_AN_BLK0_AN_CONTROL_REGISTER_t an_control;
    IEEE_AN_BLK0_AN_STATUS_REGISTER_t an_sts;
    PHYMOD_MEMSET(&an_control, 0 , sizeof(IEEE_AN_BLK0_AN_CONTROL_REGISTER_t));
    PHYMOD_MEMSET(&an_sts, 0 , sizeof(IEEE_AN_BLK0_AN_STATUS_REGISTER_t));
    
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Autoneg feature is not applicable for simplex packages")));
    }

    num_lanes = 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if ((lane_map >> lane_index) & 1) {
            an_master_lane ++;
        }
    }
    if (an_master_lane > 1) {
        an_master_lane = 0;
        PHYMOD_IF_ERR_RETURN(
            _furia_cfg_an_master_lane_get(pa, &an_master_lane));

        /* Doing this because for 10G we need lane index not lane MAP*/
        PHYMOD_IF_ERR_RETURN(
           _furia_get_pkg_lane(pa->addr, chip_id, an_master_lane, 0, &pkg_lane));
        lane_map = (1 << pkg_lane);
        an_master_lane_flag = 1;
    }

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (an_master_lane_flag) {
                pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            } else {
                pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            } 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            sys_en = pkg_ln_des->sideA;
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_AN_REG(pa, IEEE_AN_BLK0_an_control_register_Adr,  
                                     &an_control.data));
            an->enable = an_control.fields.auto_negotiationenable;
            an->an_mode = phymod_AN_MODE_CL73;
            PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_AN_REG(pa, IEEE_AN_BLK0_an_status_register_Adr,  
                                     &an_sts.data));
            *an_done = an_sts.fields.auto_negotiationcomplete;
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));

    return PHYMOD_E_NONE;
}
int _furia_autoneg_ability_get (const phymod_access_t* pa, furia_an_ability_t *an_ability)
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0; 
    uint32_t an_master_lane = 0, pkg_lane = 0;
    int an_master_lane_flag = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    IEEE_AN_BLK1_an_advertisement_1_register_t adv1;
    IEEE_AN_BLK1_an_advertisement_2_register_t adv2;
    IEEE_AN_BLK1_an_advertisement_3_register_t adv3;
    PHYMOD_MEMSET(&adv1, 0 , sizeof(IEEE_AN_BLK1_an_advertisement_1_register_t));
    PHYMOD_MEMSET(&adv2, 0 , sizeof(IEEE_AN_BLK1_an_advertisement_2_register_t));
    PHYMOD_MEMSET(&adv3, 0 , sizeof(IEEE_AN_BLK1_an_advertisement_3_register_t));
    
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Autoneg feature is not applicable for simplex packages")));
    }

    num_lanes = 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if ((lane_map >> lane_index) & 1) {
            an_master_lane ++;
        }
    }
    if (an_master_lane > 1) {
        an_master_lane = 0;
        PHYMOD_IF_ERR_RETURN(
            _furia_cfg_an_master_lane_get(pa, &an_master_lane));

        /* Doing this because for 10G we need lane index not lane MAP*/
        PHYMOD_IF_ERR_RETURN(
           _furia_get_pkg_lane(pa->addr, chip_id, an_master_lane, 0, &pkg_lane));
        an_ability->an_master_lane = pkg_lane;
        lane_map = (1 << pkg_lane);
        an_master_lane_flag = 1;
    }
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (an_master_lane_flag) {
                pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            } else {
                pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            }
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            sys_en = pkg_ln_des->sideA;
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN 
                (READ_FURIA_AN_REG(pa, IEEE_AN_BLK1_an_advertisement_3_register_Adr,  
                                     &adv3.data));
            an_ability->cl73_adv.an_fec = adv3.fields.fec_requested;
            PHYMOD_IF_ERR_RETURN 
                (READ_FURIA_AN_REG(pa, IEEE_AN_BLK1_an_advertisement_2_register_Adr,  
                                     &adv2.data));
            an_ability->cl73_adv.an_base_speed = adv2.fields.techability;
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_AN_REG(pa, IEEE_AN_BLK1_an_advertisement_1_register_Adr,  
                                     &adv1.data));
            an_ability->cl73_adv.an_pause = adv1.fields.pause;
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int _furia_autoneg_remote_ability_get (const phymod_access_t* pa, phymod_autoneg_ability_t* an_ability_get)
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0; 
    uint32_t an_master_lane = 0, pkg_lane =0; 
    int an_master_lane_flag = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    
    IEEE_AN_BLK1_an_lp_base_page_ability_2_reg_t adv2;
    IEEE_AN_BLK1_an_lp_base_page_ability_1_reg_t adv1;

    PHYMOD_MEMSET(&adv1, 0 , sizeof(IEEE_AN_BLK1_an_advertisement_1_register_t));
    PHYMOD_MEMSET(&adv2, 0 , sizeof(IEEE_AN_BLK1_an_advertisement_2_register_t));
    
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if(FURIA_IS_SIMPLEX(chip_id)) {
        return PHYMOD_E_NONE;
    }

    num_lanes = 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if ((lane_map >> lane_index) & 1) {
            an_master_lane ++;
        }
    }
    if (an_master_lane > 1) {
        an_master_lane = 0;
        PHYMOD_IF_ERR_RETURN(
            _furia_cfg_an_master_lane_get(pa, &an_master_lane));
        /* Doing this because for 10G we need lane index not lane MAP*/
        PHYMOD_IF_ERR_RETURN(
           _furia_get_pkg_lane(pa->addr, chip_id, an_master_lane, 0, &pkg_lane));
        lane_map = (1 << pkg_lane);
        an_master_lane_flag = 1;
    }

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (an_master_lane_flag) {
                pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            } else {
                pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            }
            PHYMOD_NULL_CHECK(pkg_ln_des);
            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            sys_en = pkg_ln_des->sideA;
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            PHYMOD_IF_ERR_RETURN(
            READ_FURIA_AN_REG(pa, IEEE_AN_BLK1_an_lp_base_page_ability_2_reg_Adr, &adv2.data));
            an_ability_get->an_cap = (adv2.data & 0xFFE0) >> 5;

            PHYMOD_IF_ERR_RETURN(
            READ_FURIA_AN_REG(pa, IEEE_AN_BLK1_an_lp_base_page_ability_1_reg_Adr, &adv1.data));
            if (adv1.data & 0x400) {
                PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get);
            } else if (adv1.data & 0x800) {
                PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get);
            }
            break;
        }
    }

    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));

    return PHYMOD_E_NONE;
}

int _furia_autoneg_ability_set (const phymod_access_t* pa, furia_an_ability_t *an_ability)
{
    int lane_map = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0; 
    uint32_t chip_id = 0, an_master_lane = 0, pkg_lane = 0;
    int num_lanes = 0;
    int lane_index = 0;
    int an_master_lane_flag = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    IEEE_AN_BLK1_an_advertisement_1_register_t adv1;
    IEEE_AN_BLK1_an_advertisement_2_register_t adv2;
    IEEE_AN_BLK1_an_advertisement_3_register_t adv3;
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable_reg;
    uint8_t fw_enable = 0;
    uint8_t retry_count = 40;
    FUR_GEN_CNTRLS_gpreg13_t gen_ctrl_gpreg13;
    PHYMOD_MEMSET(&adv1, 0, sizeof(IEEE_AN_BLK1_an_advertisement_1_register_t));
    PHYMOD_MEMSET(&adv2, 0, sizeof(IEEE_AN_BLK1_an_advertisement_2_register_t));
    PHYMOD_MEMSET(&adv3, 0, sizeof(IEEE_AN_BLK1_an_advertisement_3_register_t));
   
    /* Get the lane map from phymod access */ 
    lane_map = PHYMOD_ACC_LANE_MASK(pa);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Autoneg feature is not applicable for simplex packages")));
    }
    num_lanes = 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if ((lane_map >> lane_index) & 1) {
            an_master_lane ++;
        }
    }
    if (an_master_lane > 1) {
        an_master_lane = 0;
        PHYMOD_IF_ERR_RETURN(
            _furia_cfg_an_master_lane_get(pa, &an_master_lane));

        /* Doing this because for 10G we need lane index not lane MAP*/
        PHYMOD_IF_ERR_RETURN(
           _furia_get_pkg_lane(pa->addr, chip_id, an_master_lane, 0, &pkg_lane));
        lane_map = (1 << pkg_lane);
        an_master_lane_flag = 1;
    }
    
    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (an_master_lane_flag) {
                pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            } else {
                pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            }
            PHYMOD_NULL_CHECK(pkg_ln_des);

            /* Get sys_en, wr_lane, rd_lane using lane descriptor */
            sys_en = pkg_ln_des->sideA;
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_AN_REG(pa,\
                                        IEEE_AN_BLK1_an_advertisement_3_register_Adr,\
                                        &adv3.data));
            adv3.fields.fec_requested = an_ability->cl73_adv.an_fec; 
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_REG(pa,\
                                         IEEE_AN_BLK1_an_advertisement_3_register_Adr,\
                                         adv3.data));
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_AN_REG(pa,\
                                        IEEE_AN_BLK1_an_advertisement_2_register_Adr,\
                                        &adv2.data));
            adv2.fields.techability = an_ability->cl73_adv.an_base_speed;
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_REG(pa,\
                                         IEEE_AN_BLK1_an_advertisement_2_register_Adr,\
                                         adv2.data));
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_AN_REG(pa,\
                                        IEEE_AN_BLK1_an_advertisement_1_register_Adr,\
                                        &adv1.data));
            adv1.fields.pause = an_ability->cl73_adv.an_pause;
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_AN_REG(pa,\
                                         IEEE_AN_BLK1_an_advertisement_1_register_Adr,\
                                         adv1.data));
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_AN_SLICE_REG(pa, 0, 1, 0));

    /* GPREG13 should only be udpated when firmware_enable.fw_enable_val = 0 */
    PHYMOD_IF_ERR_RETURN
       (READ_FURIA_PMA_PMD_REG(pa,\
                               FUR_GEN_CNTRLS_firmware_enable_Adr,\
                               &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;
    while((fw_enable != 0) && (retry_count)) {
           PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                        &fw_enable_reg.data));
            fw_enable = fw_enable_reg.fields.fw_enable_val;
            PHYMOD_USLEEP(200);
            retry_count--;
    }

    if(!retry_count && fw_enable) {
         PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("1:config failed, micro controller is busy..")));
    }else {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
             FUR_GEN_CNTRLS_gpreg13_Adr,\
              &gen_ctrl_gpreg13.data));
        an_ability->cl73_adv.an_cl72 ? (gen_ctrl_gpreg13.data &= (~(0x1 << 11))) : (gen_ctrl_gpreg13.data |= (0x1 << 11));
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
             FUR_GEN_CNTRLS_gpreg13_Adr,\
              gen_ctrl_gpreg13.data));
    }
    PHYMOD_IF_ERR_RETURN(
            _furia_fw_enable(pa));
    return PHYMOD_E_NONE;
}

int furia_display_eye_scan(const phymod_access_t *pa)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
     
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            PHYMOD_DIAG_OUT((" eyescan for lane = %d\n",     lane_index));
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_display_lane_state_hdr(pa));
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_display_lane_state(pa)); 
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_display_eye_scan(pa));
            } 
        }
    }
    return PHYMOD_E_NONE;
}

int furia_pll_sequencer_restart(const phymod_access_t *pa, phymod_sequencer_operation_t operation)
{
    int pkg_side = 0;
    uint32_t acc_flags = 0; 
    uint32_t chip_id = 0;
    LINE_FALCON_IF_SOFT_MODE0_t line_falcon_soft_mode0;
    LINE_FALCON_IF_C_AND_R_CNTL_t line_falcon_c_and_r_ctrl;
    SYS_FALCON_IF_SOFT_MODE0_t    sys_falcon_soft_mode0;
    SYS_FALCON_IF_C_AND_R_CNTL_t  sys_falcon_c_and_r_ctrl;
    PHYMOD_MEMSET(&line_falcon_soft_mode0, 0 , sizeof(LINE_FALCON_IF_SOFT_MODE0_t));
    PHYMOD_MEMSET(&line_falcon_c_and_r_ctrl, 0 , sizeof(LINE_FALCON_IF_C_AND_R_CNTL_t));
    PHYMOD_MEMSET(&sys_falcon_soft_mode0, 0 , sizeof(SYS_FALCON_IF_SOFT_MODE0_t));
    PHYMOD_MEMSET(&sys_falcon_c_and_r_ctrl, 0 , sizeof(SYS_FALCON_IF_C_AND_R_CNTL_t));
   
    /* Get the lane map from phymod access */
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    switch(operation) {
        case phymodSeqOpStop:
        case phymodSeqOpStart:
            return PHYMOD_E_UNAVAIL; 
        case phymodSeqOpRestart:
            if((FURIA_IS_DUPLEX(chip_id) && pkg_side == LINE) ||
               (FURIA_IS_SIMPLEX(chip_id))) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_soft_mode0_Adr,\
                                            &line_falcon_soft_mode0.data));
                line_falcon_soft_mode0.fields.software_mode_pmd_core_dp_h_rstb = 1;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_soft_mode0_Adr,\
                                             line_falcon_soft_mode0.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                            &line_falcon_c_and_r_ctrl.data));
                line_falcon_c_and_r_ctrl.fields.pmd_core_dp_h_rstb = 0;
                line_falcon_c_and_r_ctrl.fields.pmd_core_dp_h_rstb = 1;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                             line_falcon_c_and_r_ctrl.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_soft_mode0_Adr,\
                                            &line_falcon_soft_mode0.data));
                line_falcon_soft_mode0.fields.software_mode_pmd_core_dp_h_rstb = 0;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_soft_mode0_Adr,\
                                             line_falcon_soft_mode0.data));
            } 
            if((FURIA_IS_DUPLEX(chip_id) && pkg_side == SYS) ||
                (FURIA_IS_SIMPLEX(chip_id))) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_soft_mode0_Adr,\
                                            &sys_falcon_soft_mode0.data));
                sys_falcon_soft_mode0.fields.software_mode_pmd_core_dp_h_rstb = 1;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_soft_mode0_Adr,\
                                             sys_falcon_soft_mode0.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                            &sys_falcon_c_and_r_ctrl.data));
                sys_falcon_c_and_r_ctrl.fields.pmd_core_dp_h_rstb = 0;
                sys_falcon_c_and_r_ctrl.fields.pmd_core_dp_h_rstb = 1;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                             sys_falcon_c_and_r_ctrl.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_soft_mode0_Adr,\
                                            &sys_falcon_soft_mode0.data));
                sys_falcon_soft_mode0.fields.software_mode_pmd_core_dp_h_rstb = 0;
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_soft_mode0_Adr,\
                                             sys_falcon_soft_mode0.data));
            }
        break;
        default:
        break;
    }
    return PHYMOD_E_NONE;
}
int furia_fec_enable_set(const phymod_access_t *pa, uint32_t enable)
{
    FUR_MISC_CTRL_MODE_DEC_FRC_t dec_frc;
    FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t dec_frc_val;
    uint32_t chip_id = 0;
    PHYMOD_MEMSET(&dec_frc, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_t));
    PHYMOD_MEMSET(&dec_frc_val, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t));

    chip_id = _furia_get_chip_id(pa);
    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("FEC is not applicable for simplex packages")));
    }

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_mode_dec_frc_val_Adr,\
                                &dec_frc_val.data));
    dec_frc_val.fields.rg_cl91_enabled_frc_val = enable ? 1 : 0;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_mode_dec_frc_val_Adr,\
                                 dec_frc_val.data));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_mode_dec_frc_Adr,\
                                &dec_frc.data));
    dec_frc.fields.rg_cl91_enabled_frc = 1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_mode_dec_frc_Adr,\
                                 dec_frc.data));
    return PHYMOD_E_NONE;
}
int furia_fec_enable_get(const phymod_access_t *pa, uint32_t* enable)
{
    FUR_MISC_CTRL_MODE_DEC_FRC_t dec_frc;
    FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t dec_frc_val;
    uint32_t chip_id = 0;

    PHYMOD_MEMSET(&dec_frc, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_t));
    PHYMOD_MEMSET(&dec_frc_val, 0 , sizeof(FUR_MISC_CTRL_MODE_DEC_FRC_VAL_t));

    chip_id = _furia_get_chip_id(pa);
    if(FURIA_IS_SIMPLEX(chip_id)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("FEC is not applicable for simplex packages")));
    }

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_mode_dec_frc_val_Adr,\
                                    &dec_frc_val.data));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_mode_dec_frc_Adr,\
                                &dec_frc.data));
    if(dec_frc_val.fields.rg_cl91_enabled_frc_val &&
       dec_frc.fields.rg_cl91_enabled_frc) {
        *enable = 1;
    } else {
        *enable = 0;
    } 
    return PHYMOD_E_NONE;
}
int _furia_phy_status_dump(const phymod_access_t *pa)
{
    int pkg_side = 0;
    int sys_en = 0;
    int wr_lane = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    int lane_map = 0;
    uint32_t acc_flags = 0;
    uint8_t trace_mem[768];
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_DIAG_OUT((" ******* PHY status dump for PHY ID:%d ********\n", pa->addr));
    PHYMOD_DIAG_OUT((" ***************************************\n"));
    /* Program the slice register */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, pkg_side, 1, 0));
    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_DIAG_OUT((" ******* PHY status dump for side:%d ********\n", pkg_side));
    PHYMOD_DIAG_OUT((" ***************************************\n"));
    PHYMOD_IF_ERR_RETURN(falcon_furia_display_core_config(pa));
    PHYMOD_IF_ERR_RETURN(falcon_furia_display_core_state(pa));
    
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_display_lane_state_hdr(pa));
            PHYMOD_DIAG_OUT(("%2d ", lane_index));
            PHYMOD_IF_ERR_RETURN(falcon_furia_display_lane_state(pa));
            PHYMOD_IF_ERR_RETURN(falcon_furia_display_lane_config(pa));
       }
    }
    PHYMOD_IF_ERR_RETURN(falcon_furia_read_event_log(pa, trace_mem, 2));
    return PHYMOD_E_NONE;
}
int _furia_phy_diagnostics_get(const phymod_access_t *pa, phymod_phy_diagnostics_t* diag)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    falcon_furia_lane_state_st state;

    PHYMOD_MEMSET(&state, 0 , sizeof(falcon_furia_lane_state_st));
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
     
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            PHYMOD_IF_ERR_RETURN
                (_falcon_furia_read_lane_state(pa, &state));
            diag->signal_detect = state.sig_det;
            diag->osr_mode = state.osr_mode.tx_rx;
            diag->rx_lock = state.rx_lock;
            diag->tx_ppm = state.tx_ppm;
            diag->clk90_offset = state.clk90;
            diag->clkp1_offset = state.clkp1;
            diag->p1_lvl = state.p1_lvl;
            diag->dfe1_dcd = state.dfe1_dcd;
            diag->dfe2_dcd = state.dfe2_dcd;
            diag->slicer_offset.offset_pe = state.pe;           
            diag->slicer_offset.offset_ze = state.ze;           
            diag->slicer_offset.offset_me = state.me;           
            diag->slicer_offset.offset_po = state.po;           
            diag->slicer_offset.offset_zo = state.zo;           
            diag->slicer_offset.offset_mo = state.mo; 
            diag->eyescan.heye_left = state.heye_left;
            diag->eyescan.heye_right = state.heye_right;
            diag->eyescan.veye_upper = state.veye_upper;
            diag->eyescan.veye_lower = state.veye_lower;
            diag->link_time = state.link_time;
            diag->pf_main = state.pf_main;
            diag->pf_hiz = state.pf_hiz;
            diag->pf_bst = state.pf_bst;
            diag->pf2_ctrl = state.pf2_ctrl;
            diag->vga = state.vga;
            diag->dc_offset = state.dc_offset;
            diag->p1_lvl_ctrl = state.p1_lvl_ctrl;
            diag->dfe1 = state.dfe1;
            diag->dfe2 = state.dfe2;
            diag->dfe3 = state.dfe3;
            diag->dfe4 = state.dfe4;
            diag->dfe5 = state.dfe5;
            diag->dfe6 = state.dfe6;
            diag->txfir_pre = state.txfir_pre;
            diag->txfir_main = state.txfir_main;
            diag->txfir_post1 = state.txfir_post1;
            diag->txfir_post2 = state.txfir_post2;
            diag->txfir_post3 = state.txfir_post3;
            diag->br_pd_en = state.br_pd_en;
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


/** Get bit postion for give interrupt type.
 *
 * @param intr_type Select the interrupt type.
 * @param bit_pos   Select bits position of interrupt
 * @param int_grp   Select interrupt group 
 *
 * Return val:  [7:0]  = interrupt bit postion in interrupt control register.
 *              [31:8] = Interrupt group.
 *                       0 - m0_type
 *                       1 - sys falcon
 *                       2 - line falcon
 *                       3 - rx
 *                       4 - tx
 */

int  _furia_get_intr_reg (const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t *bit_pos, uint32_t *int_grp) {

    uint16_t lane_mask = 0;
    const phymod_access_t *pa = &phy->access;
    uint32_t intr = -1;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    int pkg_side = 0;
    uint32_t acc_flags = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0 ,lane_index = 0;
    uint32_t an_master_lane = 0, pkg_lane = 0;

    /* Get the lane map from phymod access */
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
    
    for( lane_index = 0; lane_index < num_lanes ; lane_index++){
        if (((lane_mask >> lane_index) & 1) == 0x1) {
        break;
    } 
    }
    switch (intr_type) {
        case PHYMOD_INTR_AUTONEG_EVENT:
                if (lane_mask == 0xF) {     
                PHYMOD_IF_ERR_RETURN(
            _furia_cfg_an_master_lane_get(pa,&an_master_lane));
                PHYMOD_IF_ERR_RETURN(
                        _furia_get_pkg_lane(pa->addr, chip_id, an_master_lane, 0, &pkg_lane));
                intr = INT_CL73_AN_COMPLETE_0 + pkg_lane ;
                }else{
             pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);

            if(pkg_ln_des->die_lane_num == 0){      
                intr = INT_CL73_AN_COMPLETE_0;
            } else if(pkg_ln_des->die_lane_num == 1){
                intr = INT_CL73_AN_COMPLETE_1;
            } else if(pkg_ln_des->die_lane_num == 2){
                intr = INT_CL73_AN_COMPLETE_2;
            } else if(pkg_ln_des->die_lane_num == 3){
                intr = INT_CL73_AN_COMPLETE_3;
            }
        }
        break;
        case PHYMOD_INTR_PLL_EVENT:
             intr = ( pkg_side == SIDE_A )? INT_LINE_PLL_LOCK_LOST : INT_SYS_PLL_LOCK_LOST ;
        break;
        default:
            return PHYMOD_E_NONE;
    }

    switch (intr)  {
       case INT_M0_SLV_SYSRESET_REQ:          
            *bit_pos =  0;
            *int_grp =  0; 
        break;
        case INT_M0_MST_SYSRESET_REQ:
            *bit_pos =  1;
            *int_grp =  0;
        break;
        case INT_M0_SLV_LOCKUP:
            *bit_pos =  2;
            *int_grp =  0;
        break;
        case INT_M0_MST_LOCKUP:
            *bit_pos =  3;
            *int_grp =  0;
        break;
        case INT_M0_SLV_MISC_INTR:
            *bit_pos =  4;
            *int_grp =  0;
        break;
        case INT_M0_MST_MISC_INTR:
            *bit_pos =  5;
            *int_grp =  0;
        break;
        case INT_M0_SLV_MSGOUT_INTR:
            *bit_pos =  6;
            *int_grp =  0;
        break;
        case INT_M0_MST_MSGOUT_INTR:
            *bit_pos =  7;
            *int_grp =  0;
        break;
        case INT_M0_SLV_DED:
            *bit_pos =  8;
            *int_grp =  0;
        break;
        case INT_M0_SLV_SEC:
            *bit_pos =  9;
            *int_grp =  0;
        break;
        case INT_M0_MST_DED:
            *bit_pos = 10;
            *int_grp =  0;
        break;
        case INT_M0_MST_SEC:
            *bit_pos = 11;
            *int_grp =  0;
        break;
        case INT_SYS_1GCDR_LOCK_LOST:
            *bit_pos =  0;
            *int_grp =  1;
        break;                                                                       
        case INT_SYS_1GCDR_LOCK_FOUND:
            *bit_pos =  1;
            *int_grp =  1;
        break;                                                                       
        case INT_SYS_PMD_LOCK_LOST:
            *bit_pos =  2;
            *int_grp =  1; 
        break;
        case INT_SYS_PMD_LOCK_FOUND:
            *bit_pos =  3;
            *int_grp =  1;
        break;
        case INT_SYS_RX_SIGDET_LOST:
            *bit_pos =  4;
            *int_grp =  1; 
        break;                                                                       
        case INT_SYS_RX_SIGDET_FOUND:
            *bit_pos =  5;
            *int_grp =  1;
        break;                                                                       
        case INT_SYS_PLL_LOCK_LOST:
            *bit_pos =  6;
            *int_grp =  1;
        break;
        case INT_SYS_PLL_LOCK_FOUND: 
            *bit_pos =  7;
            *int_grp =  1;
        break;
        case INT_SYS_PMI_ARB_WDOG_EXP:
            *bit_pos =  8;
            *int_grp =  1;
        break;                                                                       
        case INT_SYS_PMI_M0_WDOG_EXP:
            *bit_pos =  9;
            *int_grp =  1;
        break;
        case INT_LINE_1GCDR_LOCK_LOST:
            *bit_pos =  0;
            *int_grp =  2;
        break;                                                                       
        case INT_LINE_1GCDR_LOCK_FOUND:
            *bit_pos =  1;
            *int_grp =  2; 
        break;                                                                       
        case INT_LINE_PMD_LOCK_LOST:
            *bit_pos =  2;
            *int_grp =  2;
        break;
        case INT_LINE_PMD_LOCK_FOUND: 
            *bit_pos =  3;   
            *int_grp =  2;
        break;
        case INT_LINE_RX_SIGDET_LOST:
            *bit_pos =  4;
            *int_grp =  2;
        break;                                                                       
        case INT_LINE_RX_SIGDET_FOUND:
            *bit_pos =  5;
            *int_grp =  2;
        break;                                                                       
        case INT_LINE_PLL_LOCK_LOST:   
            *bit_pos =  6;
            *int_grp =  2;
        break;
        case INT_LINE_PLL_LOCK_FOUND: 
            *bit_pos =  7;
            *int_grp =  2;
        break;
        case INT_LINE_PMI_ARB_WDOG_EXP:
            *bit_pos =  8;
            *int_grp =  2; 
        break;                                                                       
        case INT_LINE_PMI_M0_WDOG_EXP:  
            *bit_pos =  9;
            *int_grp =  2;
        break;
        case INT_CL91_RX_ALIGN_LOST:
            *bit_pos =  0;
            *int_grp =  3;
        break;                                                                       
        case INT_CL91_RX_ALIGN_FOUND:
            *bit_pos =  1;
            *int_grp =  3; 
        break;                                                                       
        case INT_CL73_AN_RESTARTED:
            *bit_pos =  2;
            *int_grp =  3;
        break;
        case INT_CL73_AN_COMPLETE:
             *bit_pos =  3;
             *int_grp =  3;
        break;                                                                       
        case INT_LINE_RX_PCSFC_MON_LOCK_LOST:
            *bit_pos =  4;
            *int_grp =  3;
        break;
        case INT_LINE_RX_PCSFC_MON_LOCK_FOUND:
            *bit_pos =  5;
            *int_grp =  3;
        break;
        case INT_SYS_TX_FIFOERR:
            *bit_pos =  6;
            *int_grp =  3;
        break;                                                                       
        case INT_LINE_RX_FIFOERR: 
            *bit_pos =  7;
            *int_grp =  3;
        break;
        case INT_CL73_AN_RESTARTED_3:
            *bit_pos =  8;
            *int_grp =  3;
        break;                                                                       
        case INT_CL73_AN_RESTARTED_2:
            *bit_pos =  9;
            *int_grp =  3;
        break;
        case INT_CL73_AN_RESTARTED_1:
            *bit_pos = 10;
            *int_grp =  3;
        break;                                                                       
        case INT_CL73_AN_RESTARTED_0:
            *bit_pos = 11;
            *int_grp =  3;
        break;
        case INT_CL73_AN_COMPLETE_3:
            *bit_pos = 12;
            *int_grp =  3;
        break;                                                                       
        case INT_CL73_AN_COMPLETE_2:
            *bit_pos = 13;
            *int_grp =  3;
        break;
        case INT_CL73_AN_COMPLETE_1:
            *bit_pos = 14;
            *int_grp =  3;
        break;                                                                       
        case INT_CL73_AN_COMPLETE_0:
            *bit_pos = 15;
            *int_grp =  3;
        break;
        case INT_CL91_TX_ALIGN_LOST: 
            *bit_pos = 16;
            *int_grp =  4;
        break;                                                                       
        case INT_CL91_TX_ALIGN_FOUND: 
            *bit_pos =  1;
            *int_grp =  4; 
        break;                                                                       
        case INT_ONEG_INBAND_MSG_LOST:
            *bit_pos =  2;
            *int_grp =  4; 
        break;
        case INT_ONEG_INBAND_MSG_FOUND:
            *bit_pos =  3;
            *int_grp =  4;
        break;
        case INT_SYS_RX_PCSFC_MON_LOCK_LOST: 
            *bit_pos =  4;
            *int_grp =  4;
        break;                                                                       
        case INT_SYS_RX_PCSFC_MON_LOCK_FOUND:
            *bit_pos =  5;
            *int_grp =  4;
        break;                                                                       
        case INT_LINE_TX_FIFOERR: 
            *bit_pos =  6;
            *int_grp =  4;
        break;
        case INT_SYS_RX_FIFOERR:
            *bit_pos =  7;
            *int_grp =  4;
        break;
    }
    return PHYMOD_E_NONE; 
}

int furia_ext_intr_status_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* intr_status)
{
    uint32_t intr_grp = 0;
    uint32_t bit_pos = 0;
    uint32_t rd_data = 0;
    FUR_MISC_CTRL_M0_EIPR_t m0_eipr;
    FUR_MISC_CTRL_SF_EIPR_t sf_eipr;
    FUR_MISC_CTRL_LF_EIPR_t lf_eipr;
    FUR_MISC_CTRL_RX_EIPR_t rx_eipr;
    FUR_MISC_CTRL_TX_EIPR_t tx_eipr;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eipr, 0 , sizeof(FUR_MISC_CTRL_M0_EIPR_t));
    PHYMOD_MEMSET(&sf_eipr, 0 , sizeof(FUR_MISC_CTRL_SF_EIPR_t));
    PHYMOD_MEMSET(&lf_eipr, 0 , sizeof(FUR_MISC_CTRL_LF_EIPR_t));
    PHYMOD_MEMSET(&rx_eipr, 0 , sizeof(FUR_MISC_CTRL_RX_EIPR_t));
    PHYMOD_MEMSET(&tx_eipr, 0 , sizeof(FUR_MISC_CTRL_TX_EIPR_t));

    _furia_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp);

    switch(intr_grp) {
        case 0:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_m0_eipr_Adr,\
                                        &m0_eipr.data));
            rd_data = m0_eipr.data;
        break;
        case 1:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_sf_eipr_Adr,\
                                        &sf_eipr.data));
            rd_data = sf_eipr.data;
        break;
        case 2:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_lf_eipr_Adr,\
                                        &lf_eipr.data));
            rd_data = lf_eipr.data;
        break;
        case 3:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_rx_eipr_Adr,\
                                        &rx_eipr.data));
            rd_data = rx_eipr.data;
        break;
        case 4:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_tx_eipr_Adr,\
                                        &tx_eipr.data));
            rd_data = tx_eipr.data;
        break;
        default:
        break;
    }

    *intr_status = (rd_data & (0x1 << bit_pos)) ? 1 : 0;
    return PHYMOD_E_NONE;
}

int furia_ext_intr_enable_set(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t enable) 
{
    FUR_MISC_CTRL_M0_EIMR_t m0_eimr;
    FUR_MISC_CTRL_SF_EIMR_t sf_eimr;
    FUR_MISC_CTRL_LF_EIMR_t lf_eimr;
    FUR_MISC_CTRL_RX_EIMR_t rx_eimr;
    FUR_MISC_CTRL_TX_EIMR_t tx_eimr;
    const phymod_access_t *pa = &phy->access;

    uint32_t intr_grp = 0;
    uint32_t bit_pos = 0;
    uint32_t intr_mask = 0;
    PHYMOD_MEMSET(&m0_eimr, 0 , sizeof(FUR_MISC_CTRL_M0_EIMR_t));
    PHYMOD_MEMSET(&sf_eimr, 0 , sizeof(FUR_MISC_CTRL_SF_EIMR_t));
    PHYMOD_MEMSET(&lf_eimr, 0 , sizeof(FUR_MISC_CTRL_LF_EIMR_t));
    PHYMOD_MEMSET(&rx_eimr, 0 , sizeof(FUR_MISC_CTRL_RX_EIMR_t));
    PHYMOD_MEMSET(&tx_eimr, 0 , sizeof(FUR_MISC_CTRL_TX_EIMR_t));

    _furia_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp);

    intr_mask = enable ? (0x1 << bit_pos) : (~(0x1 << bit_pos));
    switch(intr_grp) {
        case 0:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_m0_eimr_Adr,\
                                        &m0_eimr.data));
            m0_eimr.data = enable ? (intr_mask | m0_eimr.data) : (intr_mask & m0_eimr.data);
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MISC_CTRL_m0_eimr_Adr,\
                                         m0_eimr.data));
        break;
        case 1:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_sf_eimr_Adr,\
                                        &sf_eimr.data));
            sf_eimr.data = enable ? (intr_mask | sf_eimr.data) : (intr_mask & sf_eimr.data);
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MISC_CTRL_sf_eimr_Adr,\
                                         sf_eimr.data));
        break;
        case 2:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_lf_eimr_Adr,\
                                        &lf_eimr.data));
            lf_eimr.data = enable ? (intr_mask | lf_eimr.data) : (intr_mask & lf_eimr.data);
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MISC_CTRL_lf_eimr_Adr,\
                                         lf_eimr.data));
        break;
        case 3:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_rx_eimr_Adr,\
                                        &rx_eimr.data));
            rx_eimr.data = enable ? (intr_mask | rx_eimr.data) : (intr_mask & rx_eimr.data);
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MISC_CTRL_rx_eimr_Adr,\
                                         rx_eimr.data));
        break;
        case 4:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_tx_eimr_Adr,\
                                        &tx_eimr.data));
            tx_eimr.data = enable ? (intr_mask | tx_eimr.data) : (intr_mask & tx_eimr.data);
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MISC_CTRL_tx_eimr_Adr,\
                                         tx_eimr.data));
        break;
        default:
        break;
    }
    return PHYMOD_E_NONE;
}

int furia_ext_intr_enable_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* enable) 
{
    FUR_MISC_CTRL_M0_EIMR_t m0_eimr;
    FUR_MISC_CTRL_SF_EIMR_t sf_eimr;
    FUR_MISC_CTRL_LF_EIMR_t lf_eimr;
    FUR_MISC_CTRL_RX_EIMR_t rx_eimr;
    FUR_MISC_CTRL_TX_EIMR_t tx_eimr;
    uint32_t intr_grp = 0;
    uint32_t bit_pos = 0;
    uint32_t intr_mask = 0;
    uint16_t rd_data = 0;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eimr, 0 , sizeof(FUR_MISC_CTRL_M0_EIMR_t));
    PHYMOD_MEMSET(&sf_eimr, 0 , sizeof(FUR_MISC_CTRL_SF_EIMR_t));
    PHYMOD_MEMSET(&lf_eimr, 0 , sizeof(FUR_MISC_CTRL_LF_EIMR_t));
    PHYMOD_MEMSET(&rx_eimr, 0 , sizeof(FUR_MISC_CTRL_RX_EIMR_t));
    PHYMOD_MEMSET(&tx_eimr, 0 , sizeof(FUR_MISC_CTRL_TX_EIMR_t));

    _furia_get_intr_reg(phy,intr_type, &bit_pos, &intr_grp);
    intr_mask = 0x1 << bit_pos;

    switch(intr_grp) {
        case 0:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_m0_eimr_Adr,\
                                        &m0_eimr.data));
            rd_data = m0_eimr.data;
        break;
        case 1:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_sf_eimr_Adr,\
                                        &sf_eimr.data));
            rd_data = sf_eimr.data;
        break;
        case 2:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_lf_eimr_Adr,\
                                        &lf_eimr.data));
            rd_data = lf_eimr.data;
        break;
        case 3:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_rx_eimr_Adr,\
                                        &rx_eimr.data));
            rd_data = rx_eimr.data;
        break;
        case 4:
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MISC_CTRL_tx_eimr_Adr,\
                                        &tx_eimr.data));
            rd_data = tx_eimr.data;
        break;
        default:
        break;
    }
    *enable = (rd_data & intr_mask) ? 1 : 0;
    return PHYMOD_E_NONE;
}

int furia_ext_intr_status_clear(const phymod_phy_access_t *phy, uint32_t intr_type)
{
    uint32_t intr_grp = 0;
    uint32_t bit_pos = 0;
    uint32_t intr_mask = 0;
    const phymod_access_t *pa = &phy->access;

    _furia_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp);
    intr_mask = 0x1 << bit_pos;     

    switch(intr_grp) {
        case 0:
         PHYMOD_IF_ERR_RETURN
             (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MISC_CTRL_m0_eisr_Adr,\
                                     intr_mask));
        break;
        case 1:
         PHYMOD_IF_ERR_RETURN
             (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MISC_CTRL_sf_eisr_Adr,\
                                     intr_mask));
        break;
        case 2:
         PHYMOD_IF_ERR_RETURN
             (WRITE_FURIA_PMA_PMD_REG(pa,\
                                      FUR_MISC_CTRL_lf_eisr_Adr,\
                                      intr_mask));
        break;
        case 3:
         PHYMOD_IF_ERR_RETURN
             (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MISC_CTRL_rx_eisr_Adr,\
                                     intr_mask));
        break;
        case 4:
         PHYMOD_IF_ERR_RETURN
             (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MISC_CTRL_tx_eisr_Adr,\
                                     intr_mask));
        break;
        default:
        break;
    }
    return PHYMOD_E_NONE;    
}
int furia_fc_pcs_chkr_enable_set(const phymod_access_t *pa, uint32_t fcpcs_chkr_mode, uint32_t enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl;
    SYS_RX_PMA_DP_MAIN_CTRL_t sys_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t tmp_main_ctrl_mode;
    PHYMOD_MEMSET(&line_rx_dp_main_ctrl, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&sys_rx_dp_main_ctrl, 0 , sizeof(SYS_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&tmp_main_ctrl_mode, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
     
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                if(sys_en == LINE) {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                &line_rx_dp_main_ctrl.data));
                    tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                } else {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                &sys_rx_dp_main_ctrl.data)); 
                    tmp_main_ctrl_mode.data = sys_rx_dp_main_ctrl.data; 
                }
            }
            tmp_main_ctrl_mode.fields.link_mon_en = 0;
            tmp_main_ctrl_mode.fields.link_mon_mode_frc = 0;
            tmp_main_ctrl_mode.fields.link_mon_mode_frcval = 0;
            tmp_main_ctrl_mode.fields.ber_dis = 0;
            tmp_main_ctrl_mode.fields.mode_50g = 0;
            tmp_main_ctrl_mode.fields.mode_100g = 0;
            tmp_main_ctrl_mode.fields.logic_lane = 0;
            tmp_main_ctrl_mode.fields.pcs_mon_frc = 0;

            if(enable) {
                switch (fcpcs_chkr_mode) {
                    case PCS49_1x10G :
                        tmp_main_ctrl_mode.fields.link_mon_en = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frc = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frcval = 0x0; /* CL49 */
                    break;
                    case PCS82_4x10G :
                    case FC16 :
                    case FC32 :
                        tmp_main_ctrl_mode.fields.link_mon_en = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frc = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frcval = 0x1; /* CL82 with 1-PCS lane per 1-phy-lane */
                    break;
                    case PCS82_2x25G :
                        tmp_main_ctrl_mode.fields.link_mon_en = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frc = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frcval = 0x1; /* CL82 */
                        tmp_main_ctrl_mode.fields.mode_50g = 1; /* 2-PCS lanes per phy-lane */
                        tmp_main_ctrl_mode.fields.pcs_mon_frc = 1;
                    break;
                    case PCS82_4x25G :
                        tmp_main_ctrl_mode.fields.link_mon_en = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frc = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frcval = 0x1; /* CL82 */
                        tmp_main_ctrl_mode.fields.mode_100g = 1; /* 5-PCS lanes per phy-lane */
                        tmp_main_ctrl_mode.fields.pcs_mon_frc = 1;
                    break;
                    case FC4 :
                        tmp_main_ctrl_mode.fields.link_mon_en = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frc = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frcval = 0x2; /* FC4 */
                    break;
                    case FC8 :
                        tmp_main_ctrl_mode.fields.link_mon_en = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frc = 1;
                        tmp_main_ctrl_mode.fields.link_mon_mode_frcval = 0x3; /* FC8 */
                    break;
                    default:
                    break;
                }
            } else {
                tmp_main_ctrl_mode.fields.link_mon_en = 0;
            }
            if ((FURIA_IS_SIMPLEX(chip_id) && (pkg_side == SIDE_B)) ||
                (FURIA_IS_DUPLEX(chip_id))) {
                if(sys_en == LINE) {
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                 tmp_main_ctrl_mode.data));
                } else {
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                 tmp_main_ctrl_mode.data)); 
                }
            }

            if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                break;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;  
}
int furia_fc_pcs_chkr_enable_get(const phymod_access_t *pa, uint32_t fcpcs_chkr_mode, uint32_t* enable)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl;
    SYS_RX_PMA_DP_MAIN_CTRL_t sys_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t tmp_main_ctrl_mode;
    PHYMOD_MEMSET(&line_rx_dp_main_ctrl, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&sys_rx_dp_main_ctrl, 0 , sizeof(SYS_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&tmp_main_ctrl_mode, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t));


    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
     
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if (FURIA_IS_DUPLEX(chip_id)) {
                if(pkg_side == SIDE_A || pkg_side == SIDE_B) {
                    if(sys_en == LINE) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                    &line_rx_dp_main_ctrl.data));
                        tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                    &sys_rx_dp_main_ctrl.data)); 
                        tmp_main_ctrl_mode.data = sys_rx_dp_main_ctrl.data; 
                    }
                }
            } else {
                if(pkg_side == SIDE_B) {
                    if(sys_en == LINE) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                    &line_rx_dp_main_ctrl.data)); 
                        tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                    &sys_rx_dp_main_ctrl.data)); 
                        tmp_main_ctrl_mode.data = sys_rx_dp_main_ctrl.data; 
                    }
                }
            }

            switch (fcpcs_chkr_mode) {
                case PCS49_1x10G :
                    if(tmp_main_ctrl_mode.fields.link_mon_en &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frc &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0) {
                        *enable = 1;
                    } else {
                        *enable = 0;
                    }
                break;
                case PCS82_4x10G :
                case FC16 :
                case FC32 :
                    if(tmp_main_ctrl_mode.fields.link_mon_en &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frc &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frcval) {
                        *enable = 1;
                    } else {
                        *enable = 0;
                    }
                break;
                case PCS82_2x25G :
                    if(tmp_main_ctrl_mode.fields.link_mon_en &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frc &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frcval &&
                       tmp_main_ctrl_mode.fields.mode_50g &&
                       tmp_main_ctrl_mode.fields.pcs_mon_frc) {
                         *enable = 1;
                    } else {
                        *enable = 0;
                    }
                break;
                case PCS82_4x25G :
                    if(tmp_main_ctrl_mode.fields.link_mon_en &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frc &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frcval &&
                       tmp_main_ctrl_mode.fields.mode_100g &&
                       tmp_main_ctrl_mode.fields.pcs_mon_frc) {
                        *enable = 1;
                    } else {
                        *enable = 0;
                    }
                break;
                case FC4 :
                    if(tmp_main_ctrl_mode.fields.link_mon_en &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frc &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x2) {
                        *enable = 1;
                    } else {
                        *enable = 0;
                    }
                break;
                case FC8 :
                    if(tmp_main_ctrl_mode.fields.link_mon_en &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frc &&
                       tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x3) {
                        *enable = 1;
                    } else {
                        *enable = 0;
                    } 
                break;
                default:
                break;
            }
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}
int furia_fc_pcs_chkr_status_get(const phymod_access_t *pa, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0;
    uint16_t fcpcs_chkr_mode = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t tmp_main_ctrl_mode;
    SYS_RX_PMA_DP_MAIN_CTRL_t sys_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_CL82_BER_COUNT_MSB_t           line_rx_pma_dp_cl82_ber_cnt_msb;
    SYS_RX_PMA_DP_CL82_BER_COUNT_MSB_t            sys_rx_pma_dp_cl82_ber_cnt_msb;
    LINE_RX_PMA_DP_LATCHED_STATUS_FC_MODE_t       line_rx_pma_dp_latched_status_fc_mode;
    LINE_RX_PMA_DP_LATCHED_STATUS_PCS_MODE_t      line_rx_pma_dp_latched_status_pcs_mode;
    LINE_RX_PMA_DP_CL49_LATCHED_STATUS_FC_MODE_t  line_rx_pma_dp_cl49_latched_status_fc_mode;
    LINE_RX_PMA_DP_CL49_LATCHED_STATUS_PCS_MODE_t line_rx_pma_dp_cl49_latched_status_pcs_mode;
    LINE_RX_PMA_DP_LIVE_STATUS_FC_MODE_t          line_rx_pma_dp_live_status_fc_mode;
    LINE_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t         line_rx_pma_dp_live_status_pcs_mode;
    SYS_RX_PMA_DP_LATCHED_STATUS_FC_MODE_t        sys_rx_pma_dp_latched_status_fc_mode;
    SYS_RX_PMA_DP_LATCHED_STATUS_PCS_MODE_t       sys_rx_pma_dp_latched_status_pcs_mode;
    SYS_RX_PMA_DP_CL49_LATCHED_STATUS_FC_MODE_t  sys_rx_pma_dp_cl49_latched_status_fc_mode;
    SYS_RX_PMA_DP_CL49_LATCHED_STATUS_PCS_MODE_t sys_rx_pma_dp_cl49_latched_status_pcs_mode;
    SYS_RX_PMA_DP_LIVE_STATUS_FC_MODE_t          sys_rx_pma_dp_live_status_fc_mode;
    SYS_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t         sys_rx_pma_dp_live_status_pcs_mode;

    *error_count = 0;
    *lock_status =0xffffffff;
    *lock_lost_lh =0xffffffff;
    PHYMOD_MEMSET(&line_rx_dp_main_ctrl, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&tmp_main_ctrl_mode, 0 , sizeof(LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t));
    PHYMOD_MEMSET(&sys_rx_dp_main_ctrl, 0 , sizeof(SYS_RX_PMA_DP_MAIN_CTRL_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_cl82_ber_cnt_msb, 0 , sizeof(LINE_RX_PMA_DP_CL82_BER_COUNT_MSB_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_cl82_ber_cnt_msb, 0 , sizeof(SYS_RX_PMA_DP_CL82_BER_COUNT_MSB_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_latched_status_fc_mode, 0 , sizeof(LINE_RX_PMA_DP_LATCHED_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_latched_status_pcs_mode, 0 , sizeof(LINE_RX_PMA_DP_LATCHED_STATUS_PCS_MODE_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_cl49_latched_status_fc_mode, 0 , sizeof(LINE_RX_PMA_DP_CL49_LATCHED_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_cl49_latched_status_pcs_mode, 0 , sizeof(LINE_RX_PMA_DP_CL49_LATCHED_STATUS_PCS_MODE_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_live_status_fc_mode, 0 , sizeof(LINE_RX_PMA_DP_LIVE_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&line_rx_pma_dp_live_status_pcs_mode, 0 , sizeof(LINE_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_latched_status_fc_mode, 0 , sizeof(SYS_RX_PMA_DP_LATCHED_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_latched_status_pcs_mode, 0 , sizeof(SYS_RX_PMA_DP_LATCHED_STATUS_PCS_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_cl49_latched_status_fc_mode, 0 , sizeof(SYS_RX_PMA_DP_CL49_LATCHED_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_cl49_latched_status_pcs_mode, 0 , sizeof(SYS_RX_PMA_DP_CL49_LATCHED_STATUS_PCS_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_live_status_fc_mode, 0 , sizeof(SYS_RX_PMA_DP_LIVE_STATUS_FC_MODE_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_live_status_pcs_mode, 0 , sizeof(SYS_RX_PMA_DP_LIVE_STATUS_PCS_MODE_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
     
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if (FURIA_IS_DUPLEX(chip_id)) {
                if(pkg_side == SIDE_A || pkg_side == SIDE_B) {
                    if(sys_en == LINE) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                    &line_rx_dp_main_ctrl.data));
                        tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_latched_status_Adr,\
                                                    &line_rx_pma_dp_latched_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_cl49_latched_status_Adr,\
                                                    &line_rx_pma_dp_cl49_latched_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_cl49_latched_status_Adr,\
                                                    &line_rx_pma_dp_cl49_latched_status_fc_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_cl82_ber_count_MSB_Adr,\
                                                    &line_rx_pma_dp_cl82_ber_cnt_msb.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_live_status_Adr,\
                                                    &line_rx_pma_dp_live_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_live_status_Adr,\
                                                    &line_rx_pma_dp_live_status_fc_mode.data));
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                    &sys_rx_dp_main_ctrl.data)); 
                        tmp_main_ctrl_mode.data = sys_rx_dp_main_ctrl.data; 
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_latched_status_Adr,\
                                                    &sys_rx_pma_dp_latched_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_cl49_latched_status_Adr,\
                                                    &sys_rx_pma_dp_cl49_latched_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_cl49_latched_status_Adr,\
                                                    &sys_rx_pma_dp_cl49_latched_status_fc_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_cl82_ber_count_MSB_Adr,\
                                                    &sys_rx_pma_dp_cl82_ber_cnt_msb.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_live_status_Adr,\
                                                    &sys_rx_pma_dp_live_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_live_status_Adr,\
                                                    &sys_rx_pma_dp_live_status_fc_mode.data));
                    }
                }
            } else {
                if(pkg_side == SIDE_B) {
                    if(sys_en == LINE) {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                    &line_rx_dp_main_ctrl.data)); 
                        tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_latched_status_Adr,\
                                                    &line_rx_pma_dp_latched_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_cl49_latched_status_Adr,\
                                                    &line_rx_pma_dp_cl49_latched_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_cl49_latched_status_Adr,\
                                                    &line_rx_pma_dp_cl49_latched_status_fc_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_cl82_ber_count_MSB_Adr,\
                                                    &line_rx_pma_dp_cl82_ber_cnt_msb.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_live_status_Adr,\
                                                    &line_rx_pma_dp_live_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    LINE_RX_PMA_DP_live_status_Adr,\
                                                    &line_rx_pma_dp_live_status_fc_mode.data));
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                    &sys_rx_dp_main_ctrl.data)); 
                        tmp_main_ctrl_mode.data = sys_rx_dp_main_ctrl.data; 
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_latched_status_Adr,\
                                                    &sys_rx_pma_dp_latched_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_cl49_latched_status_Adr,\
                                                    &sys_rx_pma_dp_cl49_latched_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_cl49_latched_status_Adr,\
                                                    &sys_rx_pma_dp_cl49_latched_status_fc_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_cl82_ber_count_MSB_Adr,\
                                                    &sys_rx_pma_dp_cl82_ber_cnt_msb.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_live_status_Adr,\
                                                    &sys_rx_pma_dp_live_status_pcs_mode.data));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    SYS_RX_PMA_DP_live_status_Adr,\
                                                    &sys_rx_pma_dp_live_status_fc_mode.data));
                    }
                }
            }

            if(!tmp_main_ctrl_mode.fields.link_mon_en) {
                PHYMOD_DEBUG_VERBOSE(("We can't read the FC/PCS checker status since PCS monitor is not enabled\n"));
                return PHYMOD_E_CONFIG;
            } else {
                if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x0)                    {
                    fcpcs_chkr_mode = PCS49_1x10G;
                }
                if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x1) {
                    if(tmp_main_ctrl_mode.fields.mode_50g) {
                        fcpcs_chkr_mode = PCS82_2x25G;
                    } else if(tmp_main_ctrl_mode.fields.mode_100g) {
                        fcpcs_chkr_mode = PCS82_4x25G;
                    } else {
                        fcpcs_chkr_mode = PCS82_4x10G;
                    }
                } 
                if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x2) {
                    fcpcs_chkr_mode = FC4;
                }
                if(tmp_main_ctrl_mode.fields.link_mon_mode_frcval == 0x3) {
                    fcpcs_chkr_mode = FC8;
                }
            } 

            switch (fcpcs_chkr_mode) {
                case PCS49_1x10G :
                case PCS82_4x10G :
                case FC16 :
                case FC32 :
                case PCS82_2x25G :
                case PCS82_4x25G :
                    if (FURIA_IS_DUPLEX(chip_id)) {
                        if(pkg_side == SIDE_A || pkg_side == SIDE_B) {
                            if(sys_en == LINE) {
                                *lock_status &= line_rx_pma_dp_live_status_pcs_mode.fields.pcs_status;
                                *lock_lost_lh &= (!line_rx_pma_dp_latched_status_pcs_mode.fields.pcs_status_ll);
                                *error_count |= (line_rx_pma_dp_cl82_ber_cnt_msb.data << 6) + line_rx_pma_dp_cl49_latched_status_pcs_mode.fields.ber_count;
                            } else {
                                *lock_status &= sys_rx_pma_dp_live_status_pcs_mode.fields.pcs_status;
                                *lock_lost_lh &= (!sys_rx_pma_dp_latched_status_pcs_mode.fields.pcs_status_ll);
                                *error_count |= (sys_rx_pma_dp_cl82_ber_cnt_msb.data << 6) + sys_rx_pma_dp_cl49_latched_status_pcs_mode.fields.ber_count;
                            }
                        }
                    } else {
                        if(pkg_side == SIDE_B) {
                            if(sys_en == LINE) {
                                *lock_status &= line_rx_pma_dp_live_status_pcs_mode.fields.pcs_status;
                                *lock_lost_lh &= (!line_rx_pma_dp_latched_status_pcs_mode.fields.pcs_status_ll);
                                *error_count |= (line_rx_pma_dp_cl82_ber_cnt_msb.data << 6) + line_rx_pma_dp_cl49_latched_status_pcs_mode.fields.ber_count;
                            } else {
                                *lock_status &= sys_rx_pma_dp_live_status_pcs_mode.fields.pcs_status;
                                *lock_lost_lh &= (!sys_rx_pma_dp_latched_status_pcs_mode.fields.pcs_status_ll);
                                *error_count |= (sys_rx_pma_dp_cl82_ber_cnt_msb.data << 6) + sys_rx_pma_dp_cl49_latched_status_pcs_mode.fields.ber_count;
                            }
                        }
                    }
                break;
                case FC4 :
                case FC8 :
                    if (FURIA_IS_DUPLEX(chip_id)) {
                        if(pkg_side == SIDE_A || pkg_side == SIDE_B) {
                            if(sys_en == LINE) {
                                *lock_status &= line_rx_pma_dp_live_status_fc_mode.fields.wsyn_link_fail;
                                *lock_lost_lh &= line_rx_pma_dp_latched_status_fc_mode.fields.wsyn_link_fail_lh;
                                *error_count |= (line_rx_pma_dp_cl82_ber_cnt_msb.data << 6) + line_rx_pma_dp_cl49_latched_status_pcs_mode.fields.ber_count;
                            } else {
                                *lock_status &= sys_rx_pma_dp_live_status_fc_mode.fields.wsyn_link_fail;
                                *lock_lost_lh &= sys_rx_pma_dp_latched_status_fc_mode.fields.wsyn_link_fail_lh;
                                *error_count |= (sys_rx_pma_dp_cl82_ber_cnt_msb.data << 6) + line_rx_pma_dp_cl49_latched_status_pcs_mode.fields.ber_count;
                            }
                        }
                    } else {
                        if(pkg_side == SIDE_B) {
                            if(sys_en == LINE) {
                                *lock_status &= line_rx_pma_dp_live_status_fc_mode.fields.wsyn_link_fail;
                                *lock_lost_lh &= line_rx_pma_dp_latched_status_fc_mode.fields.wsyn_link_fail_lh;
                                *error_count |= (line_rx_pma_dp_cl82_ber_cnt_msb.data << 6) + line_rx_pma_dp_cl49_latched_status_pcs_mode.fields.ber_count;
                            } else {
                                *lock_status &= sys_rx_pma_dp_live_status_fc_mode.fields.wsyn_link_fail;
                                *lock_lost_lh &= sys_rx_pma_dp_latched_status_fc_mode.fields.wsyn_link_fail_lh;
                                *error_count |= (sys_rx_pma_dp_cl82_ber_cnt_msb.data << 6) + line_rx_pma_dp_cl49_latched_status_pcs_mode.fields.ber_count;
                            }
                        }
                    }
                break;
                default:
                break;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int _furia_core_rptr_rtmr_mode_set(const phymod_access_t* pa, uint32_t rptr_rtmr_mode)
{
    int lane_map = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint16_t die_lane_num = 0;
    uint32_t enable = 0;
    uint8_t fw_enable = 0;
    uint8_t retry_count = 40;
    FUR_GEN_CNTRLS_gpreg12_t gen_ctrls_gpreg12;
    FUR_GEN_CNTRLS_gpreg11_t gen_ctrls_gpreg11;
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable_reg;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    PHYMOD_MEMSET(&gen_ctrls_gpreg12, 0 , sizeof(FUR_GEN_CNTRLS_gpreg12_t));
    PHYMOD_MEMSET(&gen_ctrls_gpreg11, 0 , sizeof(FUR_GEN_CNTRLS_gpreg11_t));
    PHYMOD_MEMSET(&fw_enable_reg, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));

    
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    enable = (rptr_rtmr_mode == 1) ? 1 : 0;

    /*Wait for fw_enable to go low  before setting fw_enable_val*/
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                &fw_enable_reg.data));
    fw_enable = fw_enable_reg.fields.fw_enable_val;

    while((fw_enable != 0) && (retry_count)) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_firmware_enable_Adr,\
                                    &fw_enable_reg.data));
        fw_enable = fw_enable_reg.fields.fw_enable_val;
        PHYMOD_USLEEP(200);
        retry_count--;
    }

    FURIA_GET_LANE_MASK(chip_id, lane_map);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            die_lane_num = pkg_ln_des->die_lane_num;
            if(FURIA_IS_SIMPLEX(chip_id)) {
                if(pkg_ln_des->sideA == LINE) {
                    /* Read GPReg12 */
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                &gen_ctrls_gpreg12.data));
                    if(enable) {
                        gen_ctrls_gpreg12.fields.en_tx_rptr_mode_sys2line |= (1 << die_lane_num);
                    } else {
                        gen_ctrls_gpreg12.fields.en_tx_rptr_mode_sys2line &= ~(1 << die_lane_num);
                    }
                    /* Write GPReg12 */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                gen_ctrls_gpreg12.data));
                } else {
                    /* Read GPReg11 */
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                &gen_ctrls_gpreg11.data));
                    if(enable) {
                        gen_ctrls_gpreg11.fields.en_tx_rptr_mode_line2sys |= (1 << die_lane_num);
                    } else {
                        gen_ctrls_gpreg11.fields.en_tx_rptr_mode_line2sys &= ~(1 << die_lane_num);
                    }
                    /* Write GPReg11 */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                gen_ctrls_gpreg11.data));
                }
            } else {
                /* Read GPReg12 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg12_Adr,\
                                            &gen_ctrls_gpreg12.data));
                /* Read GPReg11 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg11_Adr,\
                                            &gen_ctrls_gpreg11.data));
                if(enable) {
                    gen_ctrls_gpreg12.fields.en_tx_rptr_mode_sys2line |= (1 << die_lane_num);
                    gen_ctrls_gpreg11.fields.en_tx_rptr_mode_line2sys |= (1 << die_lane_num);
                } else {
                    gen_ctrls_gpreg12.fields.en_tx_rptr_mode_sys2line &= ~(1 << die_lane_num);
                    gen_ctrls_gpreg11.fields.en_tx_rptr_mode_line2sys &= ~(1 << die_lane_num);
                }
                /* Write GPReg12 */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             FUR_GEN_CNTRLS_gpreg12_Adr,\
                                             gen_ctrls_gpreg12.data));
                /* Write GPReg11 */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg11_Adr,\
                                            gen_ctrls_gpreg11.data));
            }
        }
    }
    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_furia_fw_enable(pa));
    return PHYMOD_E_NONE;
}

int _furia_core_avddtxdrv_config_set(const phymod_core_access_t* core, uint32_t avdd_txdrv)
{
    uint32_t avdd_txdrv_val = 0;
    uint32_t side_index = 0;
    LINE_FALCON_IF_c_and_r_cntl_t line_c_and_r_cntl;
    SYS_FALCON_IF_c_and_r_cntl_t sys_c_and_r_cntl;
    const phymod_access_t *pa = &core->access;
    
    if (avdd_txdrv == phymodTxInputVoltageDefault) {
       avdd_txdrv_val = 0;
    } else if (avdd_txdrv == phymodTxInputVoltage1p00) {
       avdd_txdrv_val = 1;
    } else {
       avdd_txdrv_val = 0;
    }
    
    PHYMOD_MEMSET(&line_c_and_r_cntl, 0 , sizeof(LINE_FALCON_IF_c_and_r_cntl_t));
    PHYMOD_MEMSET(&sys_c_and_r_cntl, 0 , sizeof(SYS_FALCON_IF_c_and_r_cntl_t));
    for(side_index = 0; side_index < 2; side_index ++) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                    &sys_c_and_r_cntl.data));
        sys_c_and_r_cntl.fields.tx_drv_hv_disable = avdd_txdrv_val;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                     sys_c_and_r_cntl.data));
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                    &line_c_and_r_cntl.data));
        line_c_and_r_cntl.fields.tx_drv_hv_disable = avdd_txdrv_val;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                     line_c_and_r_cntl.data));
    }
    return PHYMOD_E_NONE;
}

int _furia_phy_reset_set(const phymod_access_t *pa, const phymod_phy_reset_t* reset)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    TX_FED_txfir_misc_control1_t txfir_misc_ctrl1;
    DSC_D_DSC_CONTROL_9_t dsc_d_dsc_ctrl9;  

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : tx_pkg_ln_des->slice_wr_val;
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA;
                } else { 
                    sys_en = tx_pkg_ln_des->sideB;
                }
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                /* TX AFE Lane Reset */
                switch (reset->tx) {
                    /* In Reset */
                    case phymodResetDirectionIn:
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_FED_txfir_misc_control1_Adr,\
                                                    &txfir_misc_ctrl1.data));
            txfir_misc_ctrl1.fields.sdk_tx_disable = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    TX_FED_txfir_misc_control1_Adr,\
                                                    txfir_misc_ctrl1.data));
                    break;
                    /* Out Reset */
                    case phymodResetDirectionOut:
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_FED_txfir_misc_control1_Adr,\
                                                    &txfir_misc_ctrl1.data));
            txfir_misc_ctrl1.fields.sdk_tx_disable = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    TX_FED_txfir_misc_control1_Adr,\
                                                    txfir_misc_ctrl1.data));
                    break;
                    /* Toggle Reset */
                    case phymodResetDirectionInOut:
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_FED_txfir_misc_control1_Adr,\
                                                    &txfir_misc_ctrl1.data));
            txfir_misc_ctrl1.fields.sdk_tx_disable = 1;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    TX_FED_txfir_misc_control1_Adr,\
                                                    txfir_misc_ctrl1.data));
                        PHYMOD_USLEEP(10);
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_FED_txfir_misc_control1_Adr,\
                                                    &txfir_misc_ctrl1.data));
            txfir_misc_ctrl1.fields.sdk_tx_disable = 0;
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    TX_FED_txfir_misc_control1_Adr,\
                                                    txfir_misc_ctrl1.data));
                    break;
                    default:
                    break;
                }
            }
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : rx_pkg_ln_des->slice_wr_val;
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA;
                } else { 
                    sys_en = rx_pkg_ln_des->sideB;
                }
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                /* RX AFE Lane Reset */
                switch (reset->rx) {
                    /* In Reset */
                    case phymodResetDirectionIn:
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_D_dsc_sm_ctrl_9_Adr,\
                                                    &dsc_d_dsc_ctrl9.data));
            dsc_d_dsc_ctrl9.fields.rx_restart_pmd_hold = 1; /* now check for pmd rx lock */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_D_dsc_sm_ctrl_9_Adr,\
                                                    dsc_d_dsc_ctrl9.data));
                    break;
                    /* Out Reset */
                    case phymodResetDirectionOut:
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_D_dsc_sm_ctrl_9_Adr,\
                                                    &dsc_d_dsc_ctrl9.data));
            dsc_d_dsc_ctrl9.fields.rx_restart_pmd_hold = 0; /* now check for pmd rx lock */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_D_dsc_sm_ctrl_9_Adr,\
                                                    dsc_d_dsc_ctrl9.data));
                    break;
                    /* Toggle Reset */
                    case phymodResetDirectionInOut:
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_D_dsc_sm_ctrl_9_Adr,\
                                                    &dsc_d_dsc_ctrl9.data));
            dsc_d_dsc_ctrl9.fields.rx_restart_pmd_hold = 1; /* now check for pmd rx lock */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_D_dsc_sm_ctrl_9_Adr,\
                                                    dsc_d_dsc_ctrl9.data));
                        PHYMOD_USLEEP(10);
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_D_dsc_sm_ctrl_9_Adr,\
                                                    &dsc_d_dsc_ctrl9.data));
            dsc_d_dsc_ctrl9.fields.rx_restart_pmd_hold = 0; /* now check for pmd rx lock */
                        PHYMOD_IF_ERR_RETURN
                            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_D_dsc_sm_ctrl_9_Adr,\
                                                    dsc_d_dsc_ctrl9.data));
                   break;
                    default:
                    break;
                }
            }
            if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                break;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}
int _furia_phy_reset_get(const phymod_access_t *pa, phymod_phy_reset_t* reset)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* tx_pkg_ln_des = NULL;
    const FURIA_PKG_LANE_CFG_t* rx_pkg_ln_des = NULL;
    TX_FED_txfir_misc_control1_t txfir_misc_ctrl1;
    DSC_D_DSC_CONTROL_9_t dsc_d_dsc_ctrl9;  

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            
            if (FURIA_IF_TX_SIDE(chip_id, pkg_side)) {
                tx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 0); 
                PHYMOD_NULL_CHECK(tx_pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : tx_pkg_ln_des->slice_wr_val;
                rd_lane = tx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = tx_pkg_ln_des->sideA;
                } else { 
                    sys_en = tx_pkg_ln_des->sideB;
                }
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    TX_FED_txfir_misc_control1_Adr,\
                                                    &txfir_misc_ctrl1.data));
                if (txfir_misc_ctrl1.fields.sdk_tx_disable == 0) {
                    /* Out of Reset */
                    reset->tx = phymodResetDirectionOut;
                } else {
                    /* In Reset */
                    reset->tx = phymodResetDirectionIn;
                }
            }
            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                rx_pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
                PHYMOD_NULL_CHECK(rx_pkg_ln_des);
                wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : rx_pkg_ln_des->slice_wr_val;
                rd_lane = rx_pkg_ln_des->slice_rd_val;
                if(pkg_side == SIDE_A) {
                    sys_en = rx_pkg_ln_des->sideA;
                } else { 
                    sys_en = rx_pkg_ln_des->sideB;
                }
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                        PHYMOD_IF_ERR_RETURN
                            (READ_FURIA_PMA_PMD_REG(pa,\
                                                    DSC_D_dsc_sm_ctrl_9_Adr,\
                                                    &dsc_d_dsc_ctrl9.data));
                if(dsc_d_dsc_ctrl9.fields.rx_restart_pmd_hold == 0) {
                    /* Out of Reset */
                    reset->rx = phymodResetDirectionOut;
                } else {
                    /* In Reset */
                    reset->rx = phymodResetDirectionIn;
                }
            }
            break;  
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int furia_gpio_config_set(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t gpio_mode)
{
    FUR_PAD_CTRL_GPIO_1_CONTROL_t pad_ctrl_gpio_ctrl;

    if (pin_no > 4) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Furia has only 5 GPIOs (0 - 4)")));
    }
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                (pin_no*2) + FUR_PAD_CTRL_gpio_0_control_Adr,\
                                &pad_ctrl_gpio_ctrl.data));
    switch (gpio_mode) {
      case phymodGpioModeDisabled:
          return PHYMOD_E_NONE;
      case phymodGpioModeOutput:
          pad_ctrl_gpio_ctrl.fields.oeb = 0;
      break;     
      case phymodGpioModeInput:
          pad_ctrl_gpio_ctrl.fields.oeb = 1;
      break;    
      default:
          return PHYMOD_E_PARAM;
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                (pin_no*2) + FUR_PAD_CTRL_gpio_0_control_Adr,\
                                 pad_ctrl_gpio_ctrl.data));
    return PHYMOD_E_NONE;
}

int furia_gpio_config_get(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t* gpio_mode)
{
    FUR_PAD_CTRL_GPIO_1_CONTROL_t pad_ctrl_gpio_ctrl;
  
    *gpio_mode = 0;
    if (pin_no > 4) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Furia has only 5 GPIOs (0 - 4)")));
    }

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                (pin_no*2) + FUR_PAD_CTRL_gpio_0_control_Adr,\
                                &pad_ctrl_gpio_ctrl.data));
    
    switch(pad_ctrl_gpio_ctrl.fields.oeb) {
        case 0:
            *gpio_mode = phymodGpioModeOutput; 
        break;
        case 1:
            *gpio_mode = phymodGpioModeInput; 
        break;
        default:
        break;
    }

    return PHYMOD_E_NONE;
}

int furia_gpio_pin_value_set(const phymod_access_t *pa, int pin_no, int value)
{
    FUR_PAD_CTRL_GPIO_1_CONTROL_t pad_ctrl_gpio_ctrl;

    if (pin_no > 4) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Furia has only 5 GPIOs (0 - 4)")));
    }
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                (pin_no*2) + FUR_PAD_CTRL_gpio_0_control_Adr,\
                                &pad_ctrl_gpio_ctrl.data));

    pad_ctrl_gpio_ctrl.fields.ibof = 1;
    pad_ctrl_gpio_ctrl.fields.out_frcval = value ? 1 : 0;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                (pin_no*2) + FUR_PAD_CTRL_gpio_0_control_Adr,\
                                 pad_ctrl_gpio_ctrl.data));
    return PHYMOD_E_NONE;
}
int furia_gpio_pin_value_get(const phymod_access_t *pa, int pin_no, int* value)
{
    FUR_PAD_CTRL_GPIO_1_STATUS_t pad_sts_gpio;
    *value = 0;
   
    if (pin_no > 4) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Furia has only 5 GPIOs (0 - 4)")));
    }

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                ((pin_no*2)+1) + FUR_PAD_CTRL_gpio_0_control_Adr,\
                                &pad_sts_gpio.data));
    *value = pad_sts_gpio.fields.din;
    return PHYMOD_E_NONE;
}

int furia_module_write(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr,
                       uint32_t no_of_bytes, const uint8_t *write_data)
{
    uint32_t index = 0;
    uint32_t lower_page_bytes = 0;
    uint32_t upper_page_bytes = 0;
    uint32_t upper_page_flag = 0;
    uint32_t lower_page_flag = 0;
    uint32_t lower_page_start_addr = 0;
    uint32_t upper_page_start_addr = 0;
    FUR_MISC_CTRL_DEBUG_CTRL_t  debug_ctrl;
    if(start_addr > 255) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("Invalid start address")));
    }
    /* SET qsfp_mode=1 */
    
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_DEBUG_CTRL_Adr,\
                                &debug_ctrl.data));
    debug_ctrl.fields.qsfp_mode = 1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_DEBUG_CTRL_Adr,\
                                 debug_ctrl.data));
    if(no_of_bytes == 0) {
        /* Perform module controller reset and FLUSH */
        PHYMOD_IF_ERR_RETURN
            (_furia_set_module_command(pa, 0, 0, 0, FURIA_FLUSH));
        return PHYMOD_E_NONE;
    }
    /* if requested number of bytes are not within the boundary (0- 255) 
     * need to calculate what maximum number of bytes can be taken into
     * account for reading or writing to module 
     */
    if ((no_of_bytes+start_addr) >= 256) {
        no_of_bytes = 255-start_addr+ 1;
    }

    /* To determine page to be written is lower page or upper page or 
     * both lower and upper page
     */
    if ((start_addr+no_of_bytes-1) > 127) {
        /* lower page */
        if (start_addr <= 127) {
            lower_page_bytes = 127-start_addr+1;
            lower_page_flag = 1;
            lower_page_start_addr = start_addr;
        } 
        /* upper page */
        if ((start_addr+no_of_bytes) > 127) {
            upper_page_flag = 1;
            upper_page_bytes = no_of_bytes-lower_page_bytes;
            if(start_addr > 128) {
                upper_page_start_addr = start_addr;
            } else {
                upper_page_start_addr = 128;
            }
        }
    } else { /* only lower page */
        lower_page_bytes = no_of_bytes;
        lower_page_flag = 1;
        lower_page_start_addr = start_addr;
    }

    
    /* Wtite data to NVRAM */
    for (index = 0; index < no_of_bytes; index++) {
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MODULE_CNTRL_RAM_NVR0_Adr+start_addr+index,\
                                     write_data[index]));
    
    }
   
    if(lower_page_flag) { 
        PHYMOD_IF_ERR_RETURN
            (_furia_set_module_command(pa, 0, 0, 0, FURIA_FLUSH));
        for (index = 0; index < (lower_page_bytes/4); index ++) {
            PHYMOD_IF_ERR_RETURN
                (_furia_set_module_command(pa, FUR_MODULE_CNTRL_RAM_NVR0_Adr+lower_page_start_addr+(4*index), lower_page_start_addr+(4*index), 3, FURIA_I2C_WRITE));
        }
        if ((lower_page_bytes%4) > 0) {
            PHYMOD_IF_ERR_RETURN
                (_furia_set_module_command(pa, FUR_MODULE_CNTRL_RAM_NVR0_Adr+lower_page_start_addr+(4*index), lower_page_start_addr+(4*index), ((lower_page_bytes%4)-1), FURIA_I2C_WRITE));
        }
        lower_page_flag = 0;
    }   
   
    if(upper_page_flag) {
        PHYMOD_IF_ERR_RETURN
            (_furia_set_module_command(pa, 0, 0, 0, FURIA_FLUSH));
        for (index = 0; index < (upper_page_bytes/4); index++) {
            PHYMOD_IF_ERR_RETURN
                (_furia_set_module_command(pa, (FUR_MODULE_CNTRL_RAM_NVR0_Adr+upper_page_start_addr+(4*index)), upper_page_start_addr+(4*index), 3, FURIA_I2C_WRITE));
        }
        if ((upper_page_bytes%4) > 0) {
            PHYMOD_IF_ERR_RETURN
                (_furia_set_module_command(pa, (FUR_MODULE_CNTRL_RAM_NVR0_Adr+upper_page_start_addr+(4*index)), upper_page_start_addr+(4*index), ((upper_page_bytes%4)-1), FURIA_I2C_WRITE));
        }
        upper_page_flag = 0;
    }

    return PHYMOD_E_NONE;
}

int furia_module_read(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr,
                      uint32_t no_of_bytes, uint8_t *read_data)
{
    uint32_t index = 0;
    uint32_t rd_data = 0;
    uint32_t lower_page_bytes = 0;
    uint32_t upper_page_bytes = 0;
    uint32_t upper_page_flag = 0;
    uint32_t lower_page_start_addr = 0;
    uint32_t upper_page_start_addr = 0;
    uint32_t lower_page_flag = 0;
    FUR_MISC_CTRL_DEBUG_CTRL_t  debug_ctrl;
    /* SET qsfp_mode=1 */
    
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_DEBUG_CTRL_Adr,\
                                &debug_ctrl.data));
    debug_ctrl.fields.qsfp_mode = 1;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_DEBUG_CTRL_Adr,\
                                 debug_ctrl.data));
    if(start_addr > 255) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("Invalid start address")));
    }
    if(no_of_bytes == 0) {
        /* Perform module controller reset and FLUSH */
        PHYMOD_IF_ERR_RETURN
            (_furia_set_module_command(pa, 0, 0, 0, FURIA_FLUSH));
        return PHYMOD_E_NONE;
    }
    if ((no_of_bytes+start_addr) >= 256) {
        no_of_bytes = 255-start_addr+1;
    }

    /* To determine page to be written is lower page or upper page or 
     * both lower and upper page
     */
    if ((start_addr+no_of_bytes-1) > 127) {
        /* lower page */
        if (start_addr <= 127) {
            lower_page_bytes = 127-start_addr+1;
            lower_page_flag = 1;
            lower_page_start_addr = start_addr;
        } 
        /* upper page */
        if ((start_addr+no_of_bytes) > 127) {
            upper_page_flag = 1;
            upper_page_bytes = no_of_bytes-lower_page_bytes;
            if(start_addr > 128) {
                upper_page_start_addr = start_addr;
            } else {
                upper_page_start_addr = 128;
            }
        }
    } else { /* only lower page */
        lower_page_bytes = no_of_bytes;
        lower_page_flag = 1;
        lower_page_start_addr = start_addr;
    }


    if (lower_page_flag) {
        PHYMOD_IF_ERR_RETURN
            (_furia_set_module_command(pa, 0, 0, 0, FURIA_FLUSH));
        PHYMOD_IF_ERR_RETURN
            (_furia_set_module_command(pa, FUR_MODULE_CNTRL_RAM_NVR0_Adr+lower_page_start_addr, lower_page_start_addr, lower_page_bytes-1, FURIA_RANDOM_ADDRESS_READ));
        lower_page_flag = 0;
    }
 
    
    if (upper_page_flag) {
        PHYMOD_IF_ERR_RETURN
            (_furia_set_module_command(pa, 0, 0, 0, FURIA_FLUSH));
        PHYMOD_IF_ERR_RETURN
            (_furia_set_module_command(pa, FUR_MODULE_CNTRL_RAM_NVR0_Adr+upper_page_start_addr, upper_page_start_addr, upper_page_bytes-1, FURIA_RANDOM_ADDRESS_READ));
        upper_page_flag = 0;
    }
    /* Read data from NVRAM */
    for (index = 0; index < no_of_bytes; index++) {
       PHYMOD_IF_ERR_RETURN
           (READ_FURIA_PMA_PMD_REG(pa,\
                                   FUR_MODULE_CNTRL_RAM_NVR0_Adr+start_addr+index,\
                                   &rd_data));
       read_data[index] = (unsigned char) (rd_data & 0xff);
    
    }
    return PHYMOD_E_NONE;
}

int _furia_set_module_command(const phymod_access_t *pa, uint16_t xfer_addr,
                               uint32_t slv_addr, unsigned char xfer_cnt, FURIA_I2CM_CMD_E cmd) {
    FUR_MODULE_CNTRL_STATUS_t mctrl_status;
    uint16_t retry_count = 0;
    uint32_t wait_timeout_us = 0;
    /* ((2*100))/20 
     * where 100 us is the worst case byte transfer time 
     */
    wait_timeout_us = 20;
    retry_count = (xfer_cnt + 1) * 20;

    if (cmd == FURIA_FLUSH) {
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MODULE_CNTRL_CONTROL_Adr,\
                                     0xC000));
    } else {
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MODULE_CNTRL_XFER_ADDR_Adr,\
                                     xfer_addr));
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_MODULE_CNTRL_XFER_COUNT_Adr,\
                                     xfer_cnt));
        if (cmd == FURIA_CURRENT_ADDRESS_READ) {
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MODULE_CNTRL_CONTROL_Adr,\
                                         0x8001));
        } else if (cmd == FURIA_RANDOM_ADDRESS_READ ) {
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MODULE_CNTRL_ADDRESS_Adr,\
                                         slv_addr));
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MODULE_CNTRL_CONTROL_Adr,\
                                         0x8003));
        } else {
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MODULE_CNTRL_ADDRESS_Adr,\
                                         slv_addr));
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_MODULE_CNTRL_CONTROL_Adr,\
                                         0x8022));
        }
    }
    
    if ((cmd == FURIA_CURRENT_ADDRESS_READ) ||
        (cmd == FURIA_RANDOM_ADDRESS_READ) ||   
        (cmd == FURIA_I2C_WRITE)) { 
        do {
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_MODULE_CNTRL_STATUS_Adr,\
                                        &mctrl_status.data));
            PHYMOD_USLEEP(wait_timeout_us);
        } while((mctrl_status.fields.xaction_done == 0) && --retry_count);
        if(!retry_count) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Module controller: I2C transaction failed..")));
        }
    }
    return PHYMOD_E_NONE;
}

int _furia_cfg_fw_ull_dp(const phymod_core_access_t* core, uint32_t ull_dp_enable) {
    int lane_map = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint16_t die_lane_num = 0;
    uint32_t enable = 0;
    FUR_GEN_CNTRLS_gpreg12_t gen_ctrls_gpreg12;
    FUR_GEN_CNTRLS_gpreg11_t gen_ctrls_gpreg11;
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable_reg;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    const phymod_access_t *pa = &core->access;
    
    PHYMOD_MEMSET(&gen_ctrls_gpreg12, 0 , sizeof(FUR_GEN_CNTRLS_gpreg12_t));
    PHYMOD_MEMSET(&gen_ctrls_gpreg11, 0 , sizeof(FUR_GEN_CNTRLS_gpreg11_t));
    PHYMOD_MEMSET(&fw_enable_reg, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));

  
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);


    enable = (ull_dp_enable == 1) ? 1 : 0;

    FURIA_GET_LANE_MASK(chip_id, lane_map);


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            die_lane_num = pkg_ln_des->die_lane_num;
            if(FURIA_IS_SIMPLEX(chip_id)) {
                if(pkg_ln_des->sideA == LINE) {
                    /* Read GPReg12 */
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                &gen_ctrls_gpreg12.data));
                    if(enable) {
                        gen_ctrls_gpreg12.fields.en_ull_dp_sys2line |= (1 << die_lane_num);
                    } else {
                        gen_ctrls_gpreg12.fields.en_ull_dp_sys2line &= ~(1 << die_lane_num);
                    }
                    /* Write GPReg12 */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg12_Adr,\
                                                gen_ctrls_gpreg12.data));
                } else {
                    /* Read GPReg11 */
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                &gen_ctrls_gpreg11.data));
                    if(enable) {
                        gen_ctrls_gpreg11.fields.en_ull_dp_line2sys |= (1 << die_lane_num);
                    } else {
                        gen_ctrls_gpreg11.fields.en_ull_dp_line2sys &= ~(1 << die_lane_num);
                    }
                    /* Write GPReg11 */
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                FUR_GEN_CNTRLS_gpreg11_Adr,\
                                                gen_ctrls_gpreg11.data));
                }
            } else {
                /* Read GPReg12 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg12_Adr,\
                                            &gen_ctrls_gpreg12.data));
                /* Read GPReg11 */
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg11_Adr,\
                                            &gen_ctrls_gpreg11.data));
                if(enable) {
                    gen_ctrls_gpreg12.fields.en_ull_dp_sys2line |= (1 << die_lane_num);
                    gen_ctrls_gpreg11.fields.en_ull_dp_line2sys |= (1 << die_lane_num);
                } else {
                    gen_ctrls_gpreg12.fields.en_ull_dp_sys2line &= ~(1 << die_lane_num);
                    gen_ctrls_gpreg11.fields.en_ull_dp_line2sys &= ~(1 << die_lane_num);
                }
                /* Write GPReg12 */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             FUR_GEN_CNTRLS_gpreg12_Adr,\
                                             gen_ctrls_gpreg12.data));
                /* Write GPReg11 */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                            FUR_GEN_CNTRLS_gpreg11_Adr,\
                                            gen_ctrls_gpreg11.data));
            }
        }
    }
    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_furia_fw_enable(pa));
    return PHYMOD_E_NONE;
}

int _furia_cfg_an_master_lane(const phymod_core_access_t* core, uint32_t an_master_lane) 
{
    uint16_t die_lane_num = 0;
    uint32_t chip_id = 0;
    FUR_MISC_CTRL_LOGIC_TO_PHY_LN0_MAP_t logic_to_phy_ln0_map;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    const phymod_access_t *pa = &core->access;
    PHYMOD_MEMSET(&logic_to_phy_ln0_map, 0 , sizeof(FUR_MISC_CTRL_LOGIC_TO_PHY_LN0_MAP_t));

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    /* Get lane descriptor from package lane */
    pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, an_master_lane);
    PHYMOD_NULL_CHECK(pkg_ln_des);
    die_lane_num = pkg_ln_des->die_lane_num;
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_logic_to_phy_ln0_map_Adr,\
                                &logic_to_phy_ln0_map.data));
    logic_to_phy_ln0_map.fields.single_pmd_prt1_ln0_on_pl = die_lane_num;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MISC_CTRL_logic_to_phy_ln0_map_Adr,\
                                 logic_to_phy_ln0_map.data));
    return PHYMOD_E_NONE;
}

STATIC int _furia_cfg_an_master_lane_get(const phymod_access_t *pa, uint32_t *an_master_lane) 
{
    FUR_MISC_CTRL_LOGIC_TO_PHY_LN0_MAP_t logic_to_phy_ln0_map;
    PHYMOD_MEMSET(&logic_to_phy_ln0_map, 0 , sizeof(FUR_MISC_CTRL_LOGIC_TO_PHY_LN0_MAP_t));

    /* Get the chip id */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MISC_CTRL_logic_to_phy_ln0_map_Adr,\
                                &logic_to_phy_ln0_map.data));
    *an_master_lane = logic_to_phy_ln0_map.fields.single_pmd_prt1_ln0_on_pl;

    return PHYMOD_E_NONE;
}

int _furia_pcs_monitor_enable_set(const phymod_core_access_t* core, int enable) {
    uint32_t chip_id = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    int num_lanes = 0;
    int lane_index = 0;
    LINE_RX_PMA_DP_MAIN_CTRL_t line_rx_dp_main_ctrl;
    SYS_RX_PMA_DP_MAIN_CTRL_t sys_rx_dp_main_ctrl;
    LINE_RX_PMA_DP_MAIN_CTRL_PCS_MODE_t tmp_main_ctrl_mode;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    const phymod_access_t *pa = &core->access;
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    /* Disable PCS monitor on all the PHY lanes */
    for(pkg_side = 0; pkg_side <= SIDE_B; pkg_side++) {
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else { 
                sys_en = pkg_ln_des->sideB;
            }
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            if ((FURIA_IS_SIMPLEX(chip_id) && pkg_side == SIDE_B) ||
                (FURIA_IS_DUPLEX(chip_id))) {
                if(sys_en == LINE) {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                &line_rx_dp_main_ctrl.data));
                    tmp_main_ctrl_mode.data = line_rx_dp_main_ctrl.data; 
                    tmp_main_ctrl_mode.fields.link_mon_en = enable;
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 LINE_RX_PMA_DP_main_ctrl_Adr,\
                                                 tmp_main_ctrl_mode.data));
                } else {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                &sys_rx_dp_main_ctrl.data)); 
                    tmp_main_ctrl_mode.data = sys_rx_dp_main_ctrl.data; 
                    tmp_main_ctrl_mode.fields.link_mon_en = enable;
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 SYS_RX_PMA_DP_main_ctrl_Adr,\
                                                 tmp_main_ctrl_mode.data)); 
                }
            } 
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

STATIC int _furia_set_intf_type(const phymod_access_t *pa, uint16_t phy_type, uint16_t link_type, 
        uint32_t phymod_interface_type, int lane_index, int otn_type, uint16_t ref_clk, uint32_t higig_mode) 
{
    FUR_GEN_CNTRLS_micro_sync_7_t gen_ctrls_intf_type;
    FUR_GEN_CNTRLS_micro_sync_6_t gen_ctrls6_intf_type;
    uint32_t chip_id = 0, ref_clk_define = 1 /*  default 156*/;
    PHYMOD_MEMSET(&gen_ctrls_intf_type, 0 , sizeof(FUR_GEN_CNTRLS_micro_sync_7_t));
    PHYMOD_MEMSET(&gen_ctrls6_intf_type, 0 , sizeof(FUR_GEN_CNTRLS_micro_sync_6_t));

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa, FUR_GEN_CNTRLS_micro_sync_7_Adr,
                                    &gen_ctrls_intf_type.data));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa, FUR_GEN_CNTRLS_micro_sync_6_Adr,
                                    &gen_ctrls6_intf_type.data));
    
    chip_id =  _furia_get_chip_id(pa);
    if (chip_id == FURIA_ID_82212) {
        const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
        pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
        PHYMOD_NULL_CHECK(pkg_ln_des);
            if (pkg_ln_des->sideB == LINE) {
                lane_index = (pkg_ln_des->die_lane_num + 4);
            } else {
                lane_index = pkg_ln_des->die_lane_num;
            }

    }

   /*save ref clock*/
    switch(ref_clk){
   case phymodRefClk125Mhz:
        ref_clk_define = FURIA_CLK_125;
        break  ;
   case phymodRefClk156Mhz:
        ref_clk_define = FURIA_CLK_156;
        break  ;
   case phymodRefClk157Mhz:
        ref_clk_define = FURIA_CLK_157;
        break  ;
   case phymodRefClk106Mhz:
        ref_clk_define = FURIA_CLK_106;
        break  ;
   case phymodRefClk158Mhz:
        ref_clk_define = FURIA_CLK_158;
        break  ;
   case phymodRefClk161Mhz:
        ref_clk_define = FURIA_CLK_161;
        break  ;
   case phymodRefClk168Mhz:
        ref_clk_define = FURIA_CLK_168;
        break  ;
   case phymodRefClk174Mhz:
        ref_clk_define = FURIA_CLK_174;
        break  ;
   case phymodRefClk159Mhz:
        ref_clk_define = FURIA_CLK_159;
        break  ;
   default:
        ref_clk_define = FURIA_CLK_156;
   break;
    }
    gen_ctrls6_intf_type.fields.ref_clk = ref_clk_define ;
    gen_ctrls6_intf_type.fields.highgig = ( higig_mode ) ? 1 : 0 ;

    switch (phy_type) {
        case FURIA_PHYTYPE_FC:
        case FURIA_PHYTYPE_BP:
            /*For future use*/
            break;
        case FURIA_PHYTYPE_OTN:
            switch (link_type) {
                case FURIA_LINKTYPE_1:
                    if (phymod_interface_type == phymodInterfaceCAUI4_C2C) { 
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CC);
                    } else if (phymod_interface_type == phymodInterfaceCAUI4_C2M) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CM);
                    } else if (phymod_interface_type == phymodInterfaceOTN) {
            switch(otn_type){
            case phymodOTNOTU1e:
            FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_1E);
            break;
            case phymodOTNOTU2e:
            FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_2E);
            break;
            default:
            break;
            }
                    }
                    break;
                case FURIA_LINKTYPE_2:/*for OTU 3e1 & 3e2 . Bits are shared with OTU1E & 2E*/
                    if (phymod_interface_type == phymodInterfaceOTN) {
                switch(otn_type){
                case phymodOTNOTU3:
                    FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_3E1);
                    break;
                case phymodOTNOTU3e2:
                    FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_3E2);
                    break;
                default:
                    break;
                }
            }
            break;
                case FURIA_LINKTYPE_4:
                case FURIA_LINKTYPE_7:
                    if (phymod_interface_type == phymodInterfaceLR ||phymod_interface_type == phymodInterfaceLR2 ||
                             phymod_interface_type == phymodInterfaceLR4) {
                         FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_LR_LR4);
                    } else if (phymod_interface_type == phymodInterfaceSR ||phymod_interface_type == phymodInterfaceSR2 ||
                             phymod_interface_type == phymodInterfaceSR4) {
                         FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_SR_SR4);
                    } else if (phymod_interface_type == phymodInterfaceER ||phymod_interface_type == phymodInterfaceER2 ||
                            phymod_interface_type == phymodInterfaceER4) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_ER_ER4);
                    } else {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_KR_KR4);
                        /*Nothing to do*/
                        /* Futhure use*/
                    }
                    break;
                case FURIA_LINKTYPE_5:
                    if (phymod_interface_type == phymodInterfaceCR4) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CR4);
                    }
                    break;
                default:
                    break;
            }
        break;
        case FURIA_PHYTYPE_FP:
        {
            switch (link_type) {
                case FURIA_LINKTYPE_F:
                    if (phymod_interface_type == phymodInterfaceCAUI4_C2C) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CC);
                    } else if (phymod_interface_type == phymodInterfaceCAUI4_C2M) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CM);
                    }else if (phymod_interface_type == phymodInterfaceCAUI4) { 
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CAUI4);
                    } else if (phymod_interface_type == phymodInterfaceCAUI) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CAUI);
                    }
                    break;
                case FURIA_LINKTYPE_3:
                case FURIA_LINKTYPE_1:
                case FURIA_LINKTYPE_4:
                case FURIA_LINKTYPE_6:
                case FURIA_LINKTYPE_7:
                    if (phymod_interface_type == phymodInterfaceLR ||phymod_interface_type == phymodInterfaceLR2 ||
                            phymod_interface_type == phymodInterfaceLR4) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_LR_LR4);
                    } else if (phymod_interface_type == phymodInterfaceSR ||phymod_interface_type == phymodInterfaceSR2 ||
                            phymod_interface_type == phymodInterfaceSR4) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_SR_SR4);
                    } else if (phymod_interface_type == phymodInterfaceER ||phymod_interface_type == phymodInterfaceER2 ||
                            phymod_interface_type == phymodInterfaceER4) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_ER_ER4);
                    }
                    break;
                case FURIA_LINKTYPE_C:
                    if (phymod_interface_type == phymodInterfaceCR || phymod_interface_type == phymodInterfaceCR2) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CR);
                    }
                    break;
                case FURIA_LINKTYPE_D:
                    if (phymod_interface_type == phymodInterfaceCR) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CR);
                    } else if (phymod_interface_type == phymodInterfaceCX) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CX);
                    } else if (phymod_interface_type == phymodInterfaceSFI) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_SFI);
                    }
#ifdef DEB 
            printf("[ (%s at %d )ptype = %d : ltype = %d : otn_type = %d\n",__FUNCTION__,__LINE__,phy_type,link_type,otn_type);
#endif
            switch(ref_clk){
            case phymodRefClk157Mhz:
            FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_1E);
            break;
            case phymodRefClk158Mhz:
            FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_2E);
            break;
            case phymodRefClk159Mhz:
            if ( phymod_interface_type == phymodInterfaceSFI ) {
                switch(otn_type){
                case phymodOTNOTU3:
                    FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_3E1);
                    break;
                case phymodOTNOTU3e2:
                    FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_3E2);
                    break;
                default:
                    break;
                }
            }
            break;
            default:
            break;
            }

                    break;
        case FURIA_LINKTYPE_E:
                    if (phymod_interface_type == phymodInterfaceXLAUI) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_XLAUI);
                    } else if (phymod_interface_type == phymodInterfaceXFI) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_XFI);
                    }
                    if (( phymod_interface_type == phymodInterfaceXLAUI )&&(ref_clk == phymodRefClk159Mhz)) {
                        switch(otn_type){
                            case phymodOTNOTU3:
                                FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_3E1);
                                break;
                            case phymodOTNOTU3e2:
                                FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_3E2);
                                break;
                            default:
                                break;
                        }
                    }


                    break;
                case FURIA_LINKTYPE_5:
                    if (phymod_interface_type == phymodInterfaceCR4) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CR4);
                    }
                    break;
                default:
                    break;
            }
        }
        break;
        case FURIA_PHYTYPE_PROP:
        {
            switch (link_type) {
                case FURIA_LINKTYPE_1:
                    if (phymod_interface_type == phymodInterfaceCAUI4_C2C) { 
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CC);
                    } else if (phymod_interface_type == phymodInterfaceCAUI4_C2M) {
                        FURIA_REG_SET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_CM);
                    }
                    break;
        case FURIA_LINKTYPE_4:
            switch(ref_clk){
            case phymodRefClk157Mhz:
            FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_1E);
            break;
            case phymodRefClk158Mhz:
            FURIA_REG_SET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, FURIA_IF_2E);
            break;
            default:
            break;
            }
                    break;
                default:
                    break;
            }
        }
        break;
        default:
        break;
    }
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa, FUR_GEN_CNTRLS_micro_sync_7_Adr,
                                gen_ctrls_intf_type.data));
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa, FUR_GEN_CNTRLS_micro_sync_6_Adr,
                                gen_ctrls6_intf_type.data));
   
    return PHYMOD_E_NONE;
}

STATIC int _furia_get_intf_type(const phymod_access_t *pa, uint16_t phy_type, 
        uint16_t link_type, int lane_index, uint16_t *saved_intf, uint16_t *saved_intf1e2e, uint16_t *saved_refclk, uint32_t *higig_mode) 
{
    FUR_GEN_CNTRLS_micro_sync_7_t gen_ctrls_intf_type;
    FUR_GEN_CNTRLS_micro_sync_6_t gen_ctrls6_intf_type;
    uint32_t chip_id = 0;
    *saved_intf1e2e = 3;
    PHYMOD_MEMSET(&gen_ctrls_intf_type, 0 , sizeof(FUR_GEN_CNTRLS_micro_sync_7_t));
    PHYMOD_MEMSET(&gen_ctrls6_intf_type, 0 , sizeof(FUR_GEN_CNTRLS_micro_sync_6_t));

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa, FUR_GEN_CNTRLS_micro_sync_7_Adr,
                                    &gen_ctrls_intf_type.data));
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa, FUR_GEN_CNTRLS_micro_sync_6_Adr,
                                    &gen_ctrls6_intf_type.data));
   
    chip_id =  _furia_get_chip_id(pa);
    if (chip_id == FURIA_ID_82212) {
        const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
        pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
        PHYMOD_NULL_CHECK(pkg_ln_des);
            if (pkg_ln_des->sideB == LINE) {
                lane_index = (pkg_ln_des->die_lane_num + 4);
            } else {
                lane_index = pkg_ln_des->die_lane_num;
            }

    }

    *higig_mode = gen_ctrls6_intf_type.fields.highgig ;
    *saved_refclk = gen_ctrls6_intf_type.fields.ref_clk ;
    switch (phy_type) {
        case FURIA_PHYTYPE_FC:
        case FURIA_PHYTYPE_BP:
            /*For future use*/
            break;
        case FURIA_PHYTYPE_OTN:
            FURIA_REG_GET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, *saved_intf);
            FURIA_REG_GET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, *saved_intf1e2e);
        break;
        case FURIA_PHYTYPE_FP:
        {
            switch (link_type) {
                case FURIA_LINKTYPE_D:
                case FURIA_LINKTYPE_E:
                    if( *saved_refclk != 1){
                        FURIA_REG_GET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, *saved_intf1e2e);
                    }
           /* coverity[fallthrough] */
                case FURIA_LINKTYPE_1:
                case FURIA_LINKTYPE_3:
                case FURIA_LINKTYPE_4:
                case FURIA_LINKTYPE_5:
                case FURIA_LINKTYPE_6:
                case FURIA_LINKTYPE_7:
                case FURIA_LINKTYPE_C:
                case FURIA_LINKTYPE_F:
                    FURIA_REG_GET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index, *saved_intf);
                    break;
                default:
                    break;
            }
        }
        break;
        case FURIA_PHYTYPE_PROP:
        {
            switch (link_type) {
                case FURIA_LINKTYPE_4:
                case FURIA_LINKTYPE_1:
                case FURIA_LINKTYPE_2:
                    FURIA_REG_GET(gen_ctrls_intf_type.fields.intf_type_ln, lane_index,*saved_intf);
                    break;
 
                default:
                    break;
            }
            if( *saved_refclk != 1){
                FURIA_REG_GET(gen_ctrls6_intf_type.fields.intf_type_ln, lane_index, *saved_intf1e2e);
            }

        }
        break;
        default:
            break;;
    }
    return PHYMOD_E_NONE;
}

void _furia_get_reg_val_from_sync_type(uint32_t ptr_sync, uint32_t *reg_val)
{
    switch (ptr_sync) {
        case 0: 
            *reg_val = 0;
            break;
        case 1:
            *reg_val = 1;
            break;
        case 2:
            *reg_val = 3;
            break;
        case 3:
            *reg_val = 2;
            break;
        case 4:
            *reg_val = 6;
            break;
        case 5:
            *reg_val = 7;
            break;
        case 6:
            *reg_val = 0xF;
            break;
        case 7:
            *reg_val = 0xe;
            break;
        case 8:
            *reg_val = 0xa;
            break;
        case 9:
            *reg_val = 0xb;
            break;
        case 10:
            *reg_val = 0x9;
            break;
        case 11:
            *reg_val = 0x8;
            break;
        default :
            *reg_val = 0;
            break;
    }

}
int _furia_core_cfg_pfifo_4_max_ieee_input_skew(const phymod_access_t* pa, uint32_t tx_ptr_sync, uint32_t rx_ptr_sync)
{
    int lane_map = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    int side_index = 0;
    LINE_RX_PMA_DP_DP_CTRL_t line_rx_pma_dp_ctrl;
    LINE_TX_PMA_DP_CTRL_t    line_tx_pma_dp_ctrl;
    SYS_RX_PMA_DP_DP_CTRL_t  sys_rx_pma_dp_ctrl;
    SYS_TX_PMA_DP_CTRL_t     sys_tx_pma_dp_ctrl;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t rx_ptr_sync_reg_val = 0;
    uint32_t tx_ptr_sync_reg_val = 0;
    PHYMOD_MEMSET(&line_rx_pma_dp_ctrl, 0 , sizeof(LINE_RX_PMA_DP_DP_CTRL_t));
    PHYMOD_MEMSET(&line_tx_pma_dp_ctrl, 0 , sizeof(LINE_TX_PMA_DP_CTRL_t));
    PHYMOD_MEMSET(&sys_rx_pma_dp_ctrl, 0 , sizeof(SYS_RX_PMA_DP_DP_CTRL_t));
    PHYMOD_MEMSET(&sys_tx_pma_dp_ctrl, 0 , sizeof(SYS_TX_PMA_DP_CTRL_t));

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes);

    FURIA_GET_LANE_MASK(chip_id, lane_map);
    _furia_get_reg_val_from_sync_type(tx_ptr_sync, &tx_ptr_sync_reg_val); 
    _furia_get_reg_val_from_sync_type(rx_ptr_sync, &rx_ptr_sync_reg_val); 

    for (side_index = 0; side_index < 2; side_index++) { 
        for(lane_index = 0; lane_index < num_lanes; lane_index++) {
            if (((lane_map >> lane_index) & 1) == 0x1) {
                /* Get lane descriptor from package lane */
                pkg_ln_des = _furia_pkg_ln_des(chip_id, pa, lane_index);
                PHYMOD_NULL_CHECK(pkg_ln_des);
                if(side_index == SIDE_A) {
                    sys_en = pkg_ln_des->sideA;
                } else {
                    sys_en = pkg_ln_des->sideB;
                }

                wr_lane = pkg_ln_des->slice_wr_val;
                rd_lane = pkg_ln_des->slice_rd_val;

                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

                if(sys_en == LINE) {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                LINE_RX_PMA_DP_dp_ctrl_Adr,\
                                                &line_rx_pma_dp_ctrl.data));
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                LINE_TX_PMA_DP_ctrl_Adr,\
                                                &line_tx_pma_dp_ctrl.data));
                    line_rx_pma_dp_ctrl.fields.ptr_sync = rx_ptr_sync_reg_val;
                    line_tx_pma_dp_ctrl.fields.ptr_sync = tx_ptr_sync_reg_val;
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 LINE_RX_PMA_DP_dp_ctrl_Adr,\
                                                 line_rx_pma_dp_ctrl.data));
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 LINE_TX_PMA_DP_ctrl_Adr,\
                                                 line_tx_pma_dp_ctrl.data));
                } else {
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                SYS_RX_PMA_DP_dp_ctrl_Adr,\
                                                &sys_rx_pma_dp_ctrl.data));
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                SYS_TX_PMA_DP_ctrl_Adr,\
                                                &sys_tx_pma_dp_ctrl.data));
                    sys_rx_pma_dp_ctrl.fields.ptr_sync = rx_ptr_sync_reg_val;
                    sys_tx_pma_dp_ctrl.fields.ptr_sync = tx_ptr_sync_reg_val;
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 SYS_RX_PMA_DP_dp_ctrl_Adr,\
                                                 sys_rx_pma_dp_ctrl.data));
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 SYS_TX_PMA_DP_ctrl_Adr,\
                                                 sys_tx_pma_dp_ctrl.data));

                }
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


int _furia_core_cfg_hcd_link_status_criteria(const phymod_access_t *pa, uint32_t hcd_link_criteria)
{
    int lane_map = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    int wr_lane = 0;
    int rd_lane = 0;
    uint32_t an_master_lane = 0;
    RX_ANA_REGS_ANARXCONTROL3_t an_rx_ctrl3;
    FUR_MISC_CTRL_FURIA_QSFI_MISC_t qsfi_misc; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    
    PHYMOD_MEMSET(&an_rx_ctrl3, 0 , sizeof(RX_ANA_REGS_ANARXCONTROL3_t));
    PHYMOD_MEMSET(&qsfi_misc, 0 , sizeof(FUR_MISC_CTRL_FURIA_QSFI_MISC_t));

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    if (FURIA_IS_SIMPLEX(chip_id)) {
        return PHYMOD_E_NONE;
    }
    num_lanes = 4;
    lane_map = pa->lane_mask; 

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Get lane descriptor from package lane */
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if (lane_map == 0xF) {
                PHYMOD_IF_ERR_RETURN(
                    _furia_cfg_an_master_lane_get(pa, &an_master_lane));
                wr_lane = 1 << an_master_lane;
                rd_lane = an_master_lane;
            } else {
                wr_lane = pkg_ln_des->slice_wr_val;
                rd_lane = pkg_ln_des->slice_rd_val;
            } 

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, LINE, wr_lane, rd_lane));


            switch(hcd_link_criteria) {
                case FURIA_HCD_LINK_REG_WR:
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                RX_ANA_REGS_anaRxControl3_Adr,\
                                                &an_rx_ctrl3.data));
                    an_rx_ctrl3.fields.link_en_force_sm = 1;
                    an_rx_ctrl3.fields.link_en_r= 1;
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 RX_ANA_REGS_anaRxControl3_Adr,\
                                                 an_rx_ctrl3.data));
                break;
                case FURIA_HCD_LINK_KR_BLOCK_LOCK:
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                RX_ANA_REGS_anaRxControl3_Adr,\
                                                &an_rx_ctrl3.data));
                    an_rx_ctrl3.fields.link_en_force_sm = 0;
                    an_rx_ctrl3.fields.use_kr_block_lock_sm = 1;
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 RX_ANA_REGS_anaRxControl3_Adr,\
                                                 an_rx_ctrl3.data));
                break;
                case FURIA_HCD_LINK_KR4_BLOCK_LOCK:
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                RX_ANA_REGS_anaRxControl3_Adr,\
                                                &an_rx_ctrl3.data));
                    an_rx_ctrl3.fields.link_en_force_sm = 0;
                    an_rx_ctrl3.fields.use_kr_block_lock_sm = 0;
                    an_rx_ctrl3.fields.use_kr4_block_lock_sm = 1;

                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 RX_ANA_REGS_anaRxControl3_Adr,\
                                                 an_rx_ctrl3.data));
                break;
                case FURIA_HCD_LINK_KR4_PMD_LOCK:
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                RX_ANA_REGS_anaRxControl3_Adr,\
                                                &an_rx_ctrl3.data));
                    an_rx_ctrl3.fields.link_en_force_sm = 0;
                    an_rx_ctrl3.fields.use_kr_block_lock_sm = 0;
                    an_rx_ctrl3.fields.use_kr4_block_lock_sm = 0;
                    an_rx_ctrl3.fields.use_kr4_pmd_lock_sm = 1;

                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 RX_ANA_REGS_anaRxControl3_Adr,\
                                                 an_rx_ctrl3.data));
                break;
                case FURIA_HCD_LINK_EXT_PCS:
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                RX_ANA_REGS_anaRxControl3_Adr,\
                                                &an_rx_ctrl3.data));
                    an_rx_ctrl3.fields.link_en_force_sm = 0;
                    an_rx_ctrl3.fields.use_kr_block_lock_sm = 0;
                    an_rx_ctrl3.fields.use_kr4_block_lock_sm = 0;
                    an_rx_ctrl3.fields.use_kr4_pmd_lock_sm = 0;

                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 RX_ANA_REGS_anaRxControl3_Adr,\
                                                 an_rx_ctrl3.data));
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                FUR_MISC_CTRL_furia_qsfi_misc_Adr,\
                                                &qsfi_misc.data));
                    qsfi_misc.fields.ext_pcs_link_en = 1;
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 FUR_MISC_CTRL_furia_qsfi_misc_Adr,\
                                                 qsfi_misc.data));
                break;
                default:
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                RX_ANA_REGS_anaRxControl3_Adr,\
                                                &an_rx_ctrl3.data));
                    an_rx_ctrl3.fields.link_en_force_sm = 0;
                    an_rx_ctrl3.fields.use_kr_block_lock_sm = 0;
                    an_rx_ctrl3.fields.use_kr4_block_lock_sm = 0;
                    an_rx_ctrl3.fields.use_kr4_pmd_lock_sm = 0;

                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 RX_ANA_REGS_anaRxControl3_Adr,\
                                                 an_rx_ctrl3.data));
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(pa,\
                                                FUR_MISC_CTRL_furia_qsfi_misc_Adr,\
                                                &qsfi_misc.data));
                    qsfi_misc.fields.ext_pcs_link_en = 0;
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                                 FUR_MISC_CTRL_furia_qsfi_misc_Adr,\
                                                 qsfi_misc.data));
                break;
            }

        }
        if (lane_map == 0xF) {
            break;
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));

    return PHYMOD_E_NONE;
}

int _furia_core_cfg_tx_set(const phymod_access_t *pa,const phymod_tx_t* tx)
{
    phymod_tx_t tmp_tx;
    PHYMOD_MEMSET(&tmp_tx,0x0,sizeof(phymod_tx_t));
    PHYMOD_IF_ERR_RETURN(
            furia_tx_get(pa,&tmp_tx));
    if (!(((tx->main & 0x7F) == 0x7F) && ((tx->pre & 0x7F) == 0x7F) && ((tx->post & 0x7F) == 0x7F))) {
        tmp_tx.pre = tx->pre;
        tmp_tx.main = tx->main;
        tmp_tx.post = tx->post;

    }
    tmp_tx.amp = tx->amp;
    PHYMOD_IF_ERR_RETURN(
            furia_tx_set(pa,&tmp_tx));
    return PHYMOD_E_NONE;
}
int _furia_core_cfg_polarity_set(const phymod_access_t *pa, uint32_t rx_polarity, uint32_t tx_polarity) 
{
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    phymod_access_t pa_copy;
    uint32_t rx_lane_polarity = 0;
    uint32_t tx_lane_polarity = 0;

    if (rx_polarity == 0xFFFF && tx_polarity == 0xFFFF) {
        return PHYMOD_E_NONE;
    }
   
    PHYMOD_MEMCPY(&pa_copy, pa, sizeof(phymod_access_t)); 

    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);

    FURIA_GET_NUM_LANES(chip_id, num_lanes); 

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((pa->lane_mask >> lane_index) & 1) == 0x1) {
            pa_copy.lane_mask = (1 << lane_index);
            pa_copy.flags &= ~(1 << PHYMOD_INTERFACE_SIDE_SHIFT); 
            PHYMOD_IF_ERR_RETURN
                (furia_tx_rx_polarity_get(&pa_copy, &tx_lane_polarity, &rx_lane_polarity));
            if (rx_polarity != 0xFFFF) {
                rx_lane_polarity = (rx_polarity >> lane_index) & 0x1;
            }
            if (tx_polarity != 0xFFFF) {
                tx_lane_polarity = (tx_polarity >> lane_index) & 0x1;
            }
            PHYMOD_IF_ERR_RETURN
                (furia_tx_rx_polarity_set(&pa_copy, tx_lane_polarity, rx_lane_polarity));
            if (FURIA_IS_SIMPLEX(chip_id)) {
                pa_copy.flags |= (1 << PHYMOD_INTERFACE_SIDE_SHIFT);
                PHYMOD_IF_ERR_RETURN
                    (furia_tx_rx_polarity_set(&pa_copy, tx_lane_polarity, rx_lane_polarity));
                pa_copy.flags &= ~(1 << PHYMOD_INTERFACE_SIDE_SHIFT); 
            }
        }
    }  
    return PHYMOD_E_NONE;
}


int furia_firmware_core_config_set(const phymod_access_t* pa, phymod_firmware_core_config_t fw_config)
{
    return PHYMOD_E_NONE;
}


int furia_firmware_core_config_get(const phymod_access_t* pa, phymod_firmware_core_config_t* fw_config)
{
    return PHYMOD_E_NONE; 
}

int furia_firmware_lane_config_set(const phymod_access_t* pa, phymod_firmware_lane_config_t fw_config)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    FUR_GEN_CNTRLS_gpreg13_t fw_ln_config;
    uint16_t die_lane_number = 0;
    FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t fw_enable_reg;
    struct falcon_furia_uc_lane_config_st furia_ln_config;
    LINE_FALCON_IF_SOFT_MODE0_t line_falcon_soft_mode0;
    LINE_FALCON_IF_C_AND_R_CNTL_t line_falcon_c_and_r_ctrl;
    SYS_FALCON_IF_SOFT_MODE0_t    sys_falcon_soft_mode0;
    SYS_FALCON_IF_C_AND_R_CNTL_t  sys_falcon_c_and_r_ctrl;
    CKRST_CTRL_RPTR_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL_t pwrdn_pin_kill_ctrl;
    PHYMOD_MEMSET(&line_falcon_soft_mode0, 0 , sizeof(LINE_FALCON_IF_SOFT_MODE0_t));
    PHYMOD_MEMSET(&line_falcon_c_and_r_ctrl, 0 , sizeof(LINE_FALCON_IF_C_AND_R_CNTL_t));
    PHYMOD_MEMSET(&sys_falcon_soft_mode0, 0 , sizeof(SYS_FALCON_IF_SOFT_MODE0_t));
    PHYMOD_MEMSET(&sys_falcon_c_and_r_ctrl, 0 , sizeof(SYS_FALCON_IF_C_AND_R_CNTL_t));
    PHYMOD_MEMSET(&fw_ln_config, 0 , sizeof(FUR_GEN_CNTRLS_gpreg13_t));
    PHYMOD_MEMSET(&fw_enable_reg, 0 , sizeof(FUR_GEN_CNTRLS_FIRMWARE_ENABLE_t));
    PHYMOD_MEMSET(&furia_ln_config, 0 , sizeof(struct falcon_furia_uc_lane_config_st));
    PHYMOD_MEMSET(&pwrdn_pin_kill_ctrl, 0 , sizeof(CKRST_CTRL_RPTR_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL_t));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);
     
    /* update structure members */
    furia_ln_config.field.lane_cfg_from_pcs = fw_config.LaneConfigFromPCS;
    furia_ln_config.field.an_enabled = fw_config.AnEnabled;
    furia_ln_config.field.dfe_on     = fw_config.DfeOn;
    furia_ln_config.field.force_brdfe_on = fw_config.ForceBrDfe;
    furia_ln_config.field.dfe_lp_mode = fw_config.LpDfeOn;
    furia_ln_config.field.media_type = fw_config.MediaType;
    furia_ln_config.field.unreliable_los = fw_config.UnreliableLos;
    furia_ln_config.field.scrambling_dis = fw_config.ScramblingDisable;

    if ((furia_ln_config.field.dfe_on == 0xF) &&
        (furia_ln_config.field.dfe_lp_mode == 0xF) &&
        (furia_ln_config.field.force_brdfe_on == 0xF)) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_gpreg13_Adr,\
                                    &fw_ln_config.data));
        fw_ln_config.fields.lane_cfg_override_en = 0;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_GEN_CNTRLS_gpreg13_Adr,\
                                     fw_ln_config.data));
        PHYMOD_IF_ERR_RETURN(
            _furia_fw_enable(pa));
        return PHYMOD_E_NONE;
    }
 
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            die_lane_number = pkg_ln_des->die_lane_num;
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        FUR_GEN_CNTRLS_gpreg13_Adr,\
                                        &fw_ln_config.data));
            fw_ln_config.fields.lane_cfg_override_en = 1;
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_GEN_CNTRLS_gpreg13_Adr,\
                                         fw_ln_config.data));
            PHYMOD_IF_ERR_RETURN(
                _furia_fw_enable(pa));

            /* Keep the lane under reset */
            if (sys_en == SYS) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_soft_mode0_Adr,\
                                            &sys_falcon_soft_mode0.data));
                sys_falcon_soft_mode0.fields.software_mode_pmd_ln_dp_h_rstb &= ~(1 << die_lane_number);
                sys_falcon_soft_mode0.fields.software_mode_pmd_ln_dp_h_rstb |= (1 << die_lane_number);
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_soft_mode0_Adr,\
                                             sys_falcon_soft_mode0.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                            &sys_falcon_c_and_r_ctrl.data));
                sys_falcon_c_and_r_ctrl.fields.pmd_ln_dp_h_rstb &= ~(1 << die_lane_number);
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                             sys_falcon_c_and_r_ctrl.data));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_soft_mode0_Adr,\
                                            &line_falcon_soft_mode0.data));
                line_falcon_soft_mode0.fields.software_mode_pmd_ln_dp_h_rstb &= ~(1 << die_lane_number);
                line_falcon_soft_mode0.fields.software_mode_pmd_ln_dp_h_rstb |= (1 << die_lane_number);
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_soft_mode0_Adr,\
                                             line_falcon_soft_mode0.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                            &line_falcon_c_and_r_ctrl.data));
                line_falcon_c_and_r_ctrl.fields.pmd_ln_dp_h_rstb &= ~(1 << die_lane_number);
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                             line_falcon_c_and_r_ctrl.data));
            }
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        CKRST_CTRL_RPTR_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL_Adr,\
                                        &pwrdn_pin_kill_ctrl.data));
            pwrdn_pin_kill_ctrl.fields.pmd_ln_dp_h_rstb_pkill = 0;
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         CKRST_CTRL_RPTR_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL_Adr,\
                                         pwrdn_pin_kill_ctrl.data));
 
            PHYMOD_IF_ERR_RETURN
                (falcon_furia_set_uc_lane_cfg(pa, furia_ln_config));

            /* Bring the lane out from reset */
            if (sys_en == SYS) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_soft_mode0_Adr,\
                                            &sys_falcon_soft_mode0.data));
                sys_falcon_soft_mode0.fields.software_mode_pmd_ln_dp_h_rstb &= ~(1 << die_lane_number);
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_soft_mode0_Adr,\
                                             sys_falcon_soft_mode0.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                            &sys_falcon_c_and_r_ctrl.data));
                sys_falcon_c_and_r_ctrl.fields.pmd_ln_dp_h_rstb &= ~(1 << die_lane_number);
                sys_falcon_c_and_r_ctrl.fields.pmd_ln_dp_h_rstb |= (1 << die_lane_number);
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             SYS_FALCON_IF_c_and_r_cntl_Adr,\
                                             sys_falcon_c_and_r_ctrl.data));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_soft_mode0_Adr,\
                                            &line_falcon_soft_mode0.data));
                line_falcon_soft_mode0.fields.software_mode_pmd_ln_dp_h_rstb &= ~(1 << die_lane_number);
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_soft_mode0_Adr,\
                                             line_falcon_soft_mode0.data));
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                            LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                            &line_falcon_c_and_r_ctrl.data));
                line_falcon_c_and_r_ctrl.fields.pmd_ln_dp_h_rstb &= ~(1 << die_lane_number);
                line_falcon_c_and_r_ctrl.fields.pmd_ln_dp_h_rstb |= (1 << die_lane_number);
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             LINE_FALCON_IF_c_and_r_cntl_Adr,\
                                             line_falcon_c_and_r_ctrl.data));
            }
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pa,\
                                        CKRST_CTRL_RPTR_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL_Adr,\
                                        &pwrdn_pin_kill_ctrl.data));
            pwrdn_pin_kill_ctrl.fields.pmd_ln_dp_h_rstb_pkill = 1;
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         CKRST_CTRL_RPTR_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL_Adr,\
                                         pwrdn_pin_kill_ctrl.data));
            PHYMOD_IF_ERR_RETURN(
                _furia_fw_enable(pa));
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


int furia_firmware_lane_config_get(const phymod_access_t* pa, phymod_firmware_lane_config_t* fw_config)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    struct falcon_furia_uc_lane_config_st furia_ln_config;
    PHYMOD_MEMSET(&furia_ln_config, 0 , sizeof(struct falcon_furia_uc_lane_config_st));

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);

   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    FURIA_GET_NUM_LANES(chip_id, num_lanes);


    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1);
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }
  
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            PHYMOD_IF_ERR_RETURN
                (falcon_furia_get_uc_lane_cfg(pa, &furia_ln_config));

            fw_config->LaneConfigFromPCS = furia_ln_config.field.lane_cfg_from_pcs;
            fw_config->AnEnabled = furia_ln_config.field.an_enabled;
            fw_config->DfeOn = furia_ln_config.field.dfe_on;
            fw_config->ForceBrDfe = furia_ln_config.field.force_brdfe_on;
            fw_config->LpDfeOn = furia_ln_config.field.dfe_lp_mode;
            fw_config->MediaType = furia_ln_config.field.media_type;
            fw_config->UnreliableLos = furia_ln_config.field.unreliable_los;
            fw_config->ScramblingDisable = furia_ln_config.field.scrambling_dis;
            break;
        }
    }
    /* Set the slice register with default values */
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int _furia_ber_proj_lane_speed_get(phymod_phy_inf_config_t *config, uint32_t *speed)
{
    if (config->data_rate == SPEED_100G || config->data_rate == SPEED_106G ||
        config->data_rate == SPEED_25G || config->data_rate == SPEED_27G) {
        *speed = 25781250;
        if ( config->interface_type == phymodInterfaceOTN ) {
            *speed = 27952500;
        } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            *speed = 27343750;
        }
    } else if ((config->data_rate == SPEED_40G || config->data_rate == SPEED_42G)) {
        *speed = 20625000;
        if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            *speed = 21875000;
        }
    } else if ((config->data_rate == SPEED_20G || config->data_rate == SPEED_21G)) {
        *speed = 20625000;
        if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            *speed = 21875000;
        }
    } else {
        *speed = 10312500;
        if ( config->interface_type == phymodInterfaceOTN ) {
            *speed = 10937500;
        } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            *speed = 11180000;
        }
    }

    return PHYMOD_E_NONE;
}

int furia_ber_proj(const phymod_access_t *pa, const phymod_phy_eyescan_options_t* eyescan_options)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0; 
    phymod_access_t l_pa;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
#ifdef SERDES_API_FLOATING_POINT
    phymod_interface_t intf = 0;
    phymod_ref_clk_t ref_clk;
    uint32_t interface_modes = 0;
    uint32_t data_rate = 0;
    phymod_phy_inf_config_t config;
    uint32_t speed;
    uint16_t lane_map_saved = 0;
    USR_DOUBLE speedD;
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
#endif
    PHYMOD_MEMCPY(&l_pa, pa, sizeof(phymod_access_t));
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
#ifdef SERDES_API_FLOATING_POINT
    lane_map_saved = lane_map ; 
#endif
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
   
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    PHYMOD_IF_ERR_RETURN
        (falcon_furia_display_core_state_hdr(pa));
    PHYMOD_IF_ERR_RETURN
        (falcon_furia_display_core_state_line(pa));

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            PHYMOD_DIAG_OUT((" Eye margin projection for lane = %d\n",     lane_index));
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            if (FURIA_IF_RX_SIDE(chip_id, pkg_side)) {
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_display_lane_state_hdr(pa));
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_display_lane_state(pa)); 
#ifdef SERDES_API_FLOATING_POINT
                l_pa.lane_mask = 0x1 << lane_index;
                PHYMOD_IF_ERR_RETURN
                    (furia_get_config_mode(&l_pa, &intf, &data_rate, &ref_clk, &interface_modes,&config, lane_map_saved ));
                config.interface_type = intf;
                config.data_rate = data_rate;
                config.interface_modes = interface_modes;
                config.ref_clock = ref_clk;
                l_pa.lane_mask = 0x0;
                PHYMOD_IF_ERR_RETURN
                    (_furia_ber_proj_lane_speed_get(&config, &speed));
                speedD = speed * 1000.0;
                PHYMOD_IF_ERR_RETURN
                    (falcon_furia_eye_margin_proj(pa, speedD, eyescan_options->ber_proj_scan_mode,
                                                  eyescan_options->ber_proj_timer_cnt, 
                                                  eyescan_options->ber_proj_err_cnt));
#else
                PHYMOD_DIAG_OUT(("BER PROJ needs SERDES_API_FLOATING_POINT define to operate\n"));
                return PHYMOD_E_RESOURCE;
#endif
            } 
        }
    }
    return PHYMOD_E_NONE;
}
int furia_rx_adaptation_resume(const phymod_access_t *pa)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    uint32_t acc_flags = 0; 

    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;

    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            wr_lane = FURIA_IF_BCAST_SET(chip_id, lane_map) ? BROADCAST : pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));

            /* Resume the rx adaption */
            PHYMOD_IF_ERR_RETURN(falcon_furia_stop_rx_adaptation(pa, 0));
            if (FURIA_IF_BCAST_SET(chip_id, lane_map)) {
                break;
            }
        }
    }
    /* Set the slice register with default values */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}


int _furia_phy_short_channel_mode_set(const phymod_access_t *pa, uint32_t enable)
{
    int lane_map = 0;
    int pkg_side = 0;
   int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t acc_flags = 0;
    FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_TYPE_T mst_dram_add;
    FUR_M0ACCESS_DATA_MASTER_DRAM_RDATA_TYPE_T   mst_dram_rdata1;
    FUR_M0ACCESS_DATA_MASTER_DRAM_WDATA_TYPE_T  mst_dram_wdata1;
    FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_TYPE_T slv_dram_add;
    FUR_M0ACCESS_DATA_SLAVE_DRAM_RDATA_TYPE_T   slv_dram_rdata1;
    FUR_M0ACCESS_DATA_SLAVE_DRAM_WDATA_TYPE_T    slv_dram_wdata1;
    
    CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t rst_pwdn_ctrl;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    PHYMOD_MEMSET(&mst_dram_add,0,sizeof(FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_TYPE_T));
    PHYMOD_MEMSET(&mst_dram_rdata1,0,sizeof(FUR_M0ACCESS_DATA_MASTER_DRAM_RDATA_TYPE_T));
    PHYMOD_MEMSET(&mst_dram_wdata1,0,sizeof(FUR_M0ACCESS_DATA_MASTER_DRAM_WDATA_TYPE_T));
    PHYMOD_MEMSET(&slv_dram_add,0,sizeof(FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_TYPE_T));
    PHYMOD_MEMSET(&slv_dram_rdata1,0,sizeof(FUR_M0ACCESS_DATA_SLAVE_DRAM_RDATA_TYPE_T));
    PHYMOD_MEMSET(&slv_dram_wdata1,0,sizeof(FUR_M0ACCESS_DATA_SLAVE_DRAM_WDATA_TYPE_T));
    PHYMOD_MEMSET(&rst_pwdn_ctrl, 0 , sizeof(CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_t));
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;
            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
            #ifdef DEB
            printf(" %s,sys_en=%d wr_lan=%d rd_lan=%d pkg_ln_des->die_lane_num=%d\n",__FUNCTION__,sys_en,wr_lane,rd_lane,pkg_ln_des->die_lane_num);
            #endif

            /* put the lane under dp reset */
            PHYMOD_IF_ERR_RETURN
                  (READ_FURIA_PMA_PMD_REG(pa,\
                                          CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                          &rst_pwdn_ctrl.data));
            rst_pwdn_ctrl.fields.ln_dp_s_rstb = 0;

            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                         rst_pwdn_ctrl.data));
                        
             if(sys_en){
                 PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                         FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_ADR,\
                                          &(mst_dram_add.data )));
                 mst_dram_add.fields.mst_dram_mem_add = 0x420 + (pkg_ln_des->die_lane_num * 0x130);
                 PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                         FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_ADR,\
                                             mst_dram_add.data ));
                     
                 PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                          FUR_M0ACCESS_DATA_MASTER_DRAM_RDATA_ADR ,\
                                          &(mst_dram_rdata1.data) ));
                 enable ? (mst_dram_rdata1.data |= (0x1 << 11)): (mst_dram_rdata1.data &= (~(0x1 << 11)));
                 mst_dram_add.fields.mst_dram_mem_add = 0x420 + (pkg_ln_des->die_lane_num * 0x130);
                 PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                          FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_ADR,\
                                           mst_dram_add.data ));
                 PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                          FUR_M0ACCESS_DATA_MASTER_DRAM_WDATA_ADR,\
                                           mst_dram_rdata1.data ));
            }else{
                 PHYMOD_IF_ERR_RETURN
                 (READ_FURIA_PMA_PMD_REG(pa,\
                                             FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_ADR,\
                                              &(slv_dram_add.data )));
           slv_dram_add.fields.slv_dram_mem_add = 0x420 + (pkg_ln_des->die_lane_num * 0x130);
                 PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                             FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_ADR,\
                                             slv_dram_add.data ));
            
                 PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pa,\
                                           FUR_M0ACCESS_DATA_SLAVE_DRAM_RDATA_ADR ,\
                                          &(slv_dram_rdata1.data) ));
                 enable ? (slv_dram_rdata1.data |= (0x1 << 11)): (slv_dram_rdata1.data &= (~(0x1 << 11)));
                 slv_dram_add.fields.slv_dram_mem_add = 0x420 + (pkg_ln_des->die_lane_num * 0x130);
                 PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                            FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_ADR,\
                                           slv_dram_add.data ));
                
                 PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(pa,\
                                           FUR_M0ACCESS_DATA_SLAVE_DRAM_WDATA_ADR,\
                                           slv_dram_rdata1.data ));

            }
            
            /* Release the lane under dp reset */
            PHYMOD_IF_ERR_RETURN
                  (READ_FURIA_PMA_PMD_REG(pa,\
                                          CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                          &rst_pwdn_ctrl.data));
            rst_pwdn_ctrl.fields.ln_dp_s_rstb = 1;

            PHYMOD_IF_ERR_RETURN
                  (WRITE_FURIA_PMA_PMD_REG(pa,\
                                           CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr,\
                                           rst_pwdn_ctrl.data));
                            
        }
    }
    
     /* Set the slice register to default */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

int _furia_phy_short_channel_mode_get(const phymod_access_t *pa, uint32_t *enable, uint32_t *status)
{
    int lane_map = 0;
    int pkg_side = 0;
    int wr_lane = 0;
    int sys_en = 0;
    int rd_lane = 0;
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int lane_index = 0;
    uint32_t data;
    uint32_t acc_flags = 0; 
    uint16_t lane_map_saved = 0;
    FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_TYPE_T mst_dram_add;
    FUR_M0ACCESS_DATA_MASTER_DRAM_RDATA_TYPE_T mst_dram_rdata1;
    FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_TYPE_T slv_dram_add;
    FUR_M0ACCESS_DATA_SLAVE_DRAM_RDATA_TYPE_T   slv_dram_rdata1;
    const FURIA_PKG_LANE_CFG_t* pkg_ln_des = NULL;
    phymod_interface_t intf;
    phymod_phy_inf_config_t config;
    phymod_ref_clk_t ref_clk;
    uint32_t interface_modes = 0;
    uint32_t data_rate = 0;
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(pa);
    lane_map_saved = lane_map ;
    acc_flags = PHYMOD_ACC_FLAGS(pa);
    FURIA_GET_IF_SIDE(acc_flags, pkg_side);
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&intf, 0, sizeof(phymod_interface_t));
    PHYMOD_MEMSET(&mst_dram_add,0,sizeof(FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_TYPE_T));
    PHYMOD_MEMSET(&mst_dram_rdata1,0,sizeof(FUR_M0ACCESS_DATA_MASTER_DRAM_RDATA_TYPE_T));
    PHYMOD_MEMSET(&slv_dram_add,0,sizeof(FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_TYPE_T));
    PHYMOD_MEMSET(&slv_dram_rdata1,0,sizeof(FUR_M0ACCESS_DATA_SLAVE_DRAM_RDATA_TYPE_T));
    *enable=1;
    *status=0;
    /* Get the chip id */
    chip_id = _furia_get_chip_id(pa);
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    for(lane_index = 0; lane_index < num_lanes; lane_index++) {
        if (((lane_map >> lane_index) & 1) == 0x1) {
            /* Configure Tx side(SIDE_A) first*/
            
            pkg_ln_des = _furia_pkg_ln_des_lane_swap(chip_id, pa, lane_index, 1); 
            PHYMOD_NULL_CHECK(pkg_ln_des);
            if(pkg_side == SIDE_A) {
                sys_en = pkg_ln_des->sideA;
            } else {
                sys_en = pkg_ln_des->sideB;
            }

            wr_lane = pkg_ln_des->slice_wr_val;
            rd_lane = pkg_ln_des->slice_rd_val;

            /* Program the slice register */
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
    #ifdef DEB
            printf(" %s,sys_en=%d wr_lan=%d rd_lan=%d pkg_ln_des->die_lane_num=%d\n",__FUNCTION__,sys_en,wr_lane,rd_lane,pkg_ln_des->die_lane_num);
    #endif
            if(sys_en){
               PHYMOD_IF_ERR_RETURN
                   (READ_FURIA_PMA_PMD_REG(pa,\
                       FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_ADR,\
                       &(mst_dram_add.data )));
               mst_dram_add.fields.mst_dram_mem_add = 0x420 + (pkg_ln_des->die_lane_num * 0x130);
               PHYMOD_IF_ERR_RETURN
                   (WRITE_FURIA_PMA_PMD_REG(pa,\
                          FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_ADR,\
                          mst_dram_add.data ));
               PHYMOD_IF_ERR_RETURN
                   (READ_FURIA_PMA_PMD_REG(pa,\
                       FUR_M0ACCESS_DATA_MASTER_DRAM_RDATA_ADR ,\
                       &(mst_dram_rdata1.data) ));
               *enable &= ((mst_dram_rdata1.data >> 11) & 0x1);
#ifdef DEB
printf("%s short channel enable sys side= %d\n",__FUNCTION__,*enable);
#endif
            }else {
               PHYMOD_IF_ERR_RETURN
                   (READ_FURIA_PMA_PMD_REG(pa,\
                        FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_ADR,\
                        &(slv_dram_add.data )));
               slv_dram_add.fields.slv_dram_mem_add = 0x420 + (pkg_ln_des->die_lane_num * 0x130);
               PHYMOD_IF_ERR_RETURN
                   (WRITE_FURIA_PMA_PMD_REG(pa,\
                        FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_ADR,\
                        slv_dram_add.data ));
               PHYMOD_IF_ERR_RETURN
                   (READ_FURIA_PMA_PMD_REG(pa,\
                        FUR_M0ACCESS_DATA_SLAVE_DRAM_RDATA_ADR ,\
                        &(slv_dram_rdata1.data) ));
                *enable &= ((slv_dram_rdata1.data >> 11) & 0x1);
                #ifdef DEB
                printf("%s short channel enable line side: =%d\n",__FUNCTION__,*enable);
                #endif
           }
           
              PHYMOD_IF_ERR_RETURN
              (furia_get_config_mode(pa, &intf, &data_rate, &ref_clk, &interface_modes,&config, lane_map_saved));
            #ifdef DEB
            printf("%s intf=%d\n",__FUNCTION__,intf);
            #endif
            if(intf == phymodInterfaceCAUI4 || intf == phymodInterfaceCAUI4_C2C ||
                intf == phymodInterfaceCAUI || intf == phymodInterfaceCAUI4_C2M){
                /* Program the slice register */
                PHYMOD_IF_ERR_RETURN
                   (WRITE_FURIA_SLICE_REG(pa, sys_en, wr_lane, rd_lane));
                PHYMOD_IF_ERR_RETURN
                   (READ_FURIA_PMA_PMD_REG(pa,\
                                            FUR_AMS_RX_RX_CNTRL0_Adr,\
                                            &data));
                *status = ((data & (1<<9)) >> 9);
                 
                #ifdef DEB
                printf("%s short channel reg 0xd0c0 data=0x%x\n",__FUNCTION__,data);
                #endif
            }
                             
           
        }
    }
     /* Set the slice register to default */ 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_SLICE_REG(pa, 0, 1, 0));
    return PHYMOD_E_NONE;
}

